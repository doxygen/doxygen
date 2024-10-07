#include "portable.h"
#include "qcstring.h"

#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>

#if defined(_WIN32) && !defined(__CYGWIN__)
#undef UNICODE
#define _WIN32_DCOM
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
extern char **environ;
#endif

#include <assert.h>
#include <ctype.h>
#include <map>
#include <string>

#include "fileinfo.h"
#include "message.h"

#include "util.h"
#include "dir.h"
#ifndef NODEBUG
#include "debug.h"
#endif

#if !defined(_WIN32) || defined(__CYGWIN__)
static bool environmentLoaded = false;
static std::map<std::string,std::string> proc_env = std::map<std::string,std::string>();
#endif


//---------------------------------------------------------------------------------------------------------

/*! Helper class to keep time interval per thread */
class SysTimeKeeper
{
  public:
    static SysTimeKeeper &instance();
    //! start a timer for this thread
    void start()
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_startTimes[std::this_thread::get_id()] = std::chrono::steady_clock::now();
    }
    //! ends a timer for this thread, accumulate time difference since start
    void stop()
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
      auto it = m_startTimes.find(std::this_thread::get_id());
      if (it == m_startTimes.end())
      {
        err("SysTimeKeeper stop() called without matching start()\n");
        return;
      }
      double timeSpent = static_cast<double>(std::chrono::duration_cast<
                         std::chrono::microseconds>(endTime - it->second).count())/1000000.0;
      //printf("timeSpent on thread %zu: %.4f seconds\n",std::hash<std::thread::id>{}(std::this_thread::get_id()),timeSpent);
      m_elapsedTime += timeSpent;
    }

    double elapsedTime() const { return m_elapsedTime; }

  private:
    struct TimeData
    {
      std::chrono::steady_clock::time_point startTime;
    };
    std::map<std::thread::id,std::chrono::steady_clock::time_point> m_startTimes;
    double m_elapsedTime = 0;
    std::mutex m_mutex;
};

SysTimeKeeper &SysTimeKeeper::instance()
{
  static SysTimeKeeper theInstance;
  return theInstance;
}

class AutoTimeKeeper
{
  public:
    AutoTimeKeeper() { SysTimeKeeper::instance().start(); }
   ~AutoTimeKeeper() { SysTimeKeeper::instance().stop();  }
};

double Portable::getSysElapsedTime()
{
  return SysTimeKeeper::instance().elapsedTime();
}

//---------------------------------------------------------------------------------------------------------


int Portable::system(const QCString &command,const QCString &args,bool commandHasConsole)
{
  if (command.isEmpty()) return 1;
  AutoTimeKeeper timeKeeper;

#if defined(_WIN32) && !defined(__CYGWIN__)
  QCString commandCorrectedPath = substitute(command,'/','\\');
  QCString fullCmd=commandCorrectedPath;
#else
  QCString fullCmd=command;
#endif
  fullCmd=fullCmd.stripWhiteSpace();
  if (fullCmd.at(0)!='"' && fullCmd.find(' ')!=-1)
  {
    // add quotes around command as it contains spaces and is not quoted already
    fullCmd="\""+fullCmd+"\"";
  }
  fullCmd += " ";
  fullCmd += args;
#ifndef NODEBUG
  Debug::print(Debug::ExtCmd,0,"Executing external command `%s`\n",qPrint(fullCmd));
#endif

#if !defined(_WIN32) || defined(__CYGWIN__)
  (void)commandHasConsole;
  /*! taken from the system() manpage on my Linux box */
  int pid,status=0;

#ifdef _OS_SOLARIS // for Solaris we use vfork since it is more memory efficient

  // on Solaris fork() duplicates the memory usage
  // so we use vfork instead

  // spawn shell
  if ((pid=vfork())<0)
  {
    status=-1;
  }
  else if (pid==0)
  {
     execl("/bin/sh","sh","-c",fullCmd.data(),(char*)0);
     _exit(127);
  }
  else
  {
    while (waitpid(pid,&status,0 )<0)
    {
      if (errno!=EINTR)
      {
        status=-1;
        break;
      }
    }
  }
  return status;

#else  // Other Unices just use fork

  pid = fork();
  if (pid==-1)
  {
    perror("fork error");
	  return -1;
  }
  if (pid==0)
  {
    const char * const argv[4] = { "sh", "-c", fullCmd.data(), 0 };
    execve("/bin/sh",const_cast<char * const*>(argv),environ);
    exit(127);
  }
  for (;;)
  {
    if (waitpid(pid,&status,0)==-1)
    {
      if (errno!=EINTR) return -1;
    }
    else
    {
      if (WIFEXITED(status))
      {
        return WEXITSTATUS(status);
      }
      else
      {
        return status;
      }
    }
  }
#endif // !_OS_SOLARIS

#else // Win32 specific
  if (commandHasConsole)
  {
    return ::system(fullCmd.data());
  }
  else
  {
    // Because ShellExecuteEx can delegate execution to Shell extensions
    // (data sources, context menu handlers, verb implementations) that
    // are activated using Component Object Model (COM), COM should be
    // initialized before ShellExecuteEx is called. Some Shell extensions
    // require the COM single-threaded apartment (STA) type.
    // For that case COM is initialized as follows
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    uint16_t *commandw = nullptr;
    recodeUtf8StringToW( commandCorrectedPath, &commandw );
    uint16_t *argsw = nullptr;
    recodeUtf8StringToW( args, &argsw );

    // gswin32 is a GUI api which will pop up a window and run
    // asynchronously. To prevent both, we use ShellExecuteEx and
    // WaitForSingleObject (thanks to Robert Golias for the code)

    SHELLEXECUTEINFOW sInfo = {
      sizeof(SHELLEXECUTEINFOW),   /* structure size */
      SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI,  /* tell us the process
                                                       *  handle so we can wait till it's done |
                                                       *  do not display msg box if error
                                                       */
      nullptr,                       /* window handle */
      nullptr,                       /* action to perform: open */
      (LPCWSTR)commandw,          /* file to execute */
      (LPCWSTR)argsw,             /* argument list */
      nullptr,                       /* use current working dir */
      SW_HIDE,                    /* minimize on start-up */
      nullptr,                          /* application instance handle */
      nullptr,                       /* ignored: id list */
      nullptr,                       /* ignored: class name */
      nullptr,                       /* ignored: key class */
      0,                          /* ignored: hot key */
      nullptr,                       /* ignored: icon */
      nullptr                        /* resulting application handle */
    };

    if (!ShellExecuteExW(&sInfo))
    {
      delete[] commandw;
      delete[] argsw;
      return -1;
    }
    else if (sInfo.hProcess)      /* executable was launched, wait for it to finish */
    {
      WaitForSingleObject(sInfo.hProcess,INFINITE);
      /* get process exit code */
      DWORD exitCode;
      bool retval = GetExitCodeProcess(sInfo.hProcess,&exitCode);
      CloseHandle(sInfo.hProcess);
      delete[] commandw;
      delete[] argsw;
      if (!retval) return -1;
      return exitCode;
    }
  }
#endif
  return 1; // we should never get here

}

uint32_t Portable::pid()
{
  uint32_t pid;
#if !defined(_WIN32) || defined(__CYGWIN__)
  pid = static_cast<uint32_t>(getpid());
#else
  pid = static_cast<uint32_t>(GetCurrentProcessId());
#endif
  return pid;
}

#if !defined(_WIN32) || defined(__CYGWIN__)
void loadEnvironment()
{
  if(environ != nullptr)
  {
    unsigned int i = 0;
    char* current = environ[i];

    while(current != nullptr)                            // parse all strings contained by environ til the last element (nullptr)
    {
      std::string env_var(current);                   // load current environment variable string
      size_t pos = env_var.find("=");
      if(pos != std::string::npos)                    // only parse the variable, if it is a valid environment variable...
      {                                               // ...which has to contain an equal sign as delimiter by definition
        std::string name = env_var.substr(0,pos);     // the string til the equal sign contains the name
        std::string value = env_var.substr(pos + 1);  // the string from the equal sign contains the value
        proc_env[name] = std::move(value);            // save the value by the name as its key in the classes map
      }
      i++;
      current = environ[i];
    }
  }

  environmentLoaded = true;
}
#endif

void Portable::setenv(const QCString &name,const QCString &value)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    SetEnvironmentVariable(name.data(),!value.isEmpty() ? value.data() : "");
#else
    if(!environmentLoaded) // if the environment variables are not loaded already...
    {                                 // ...call loadEnvironment to store them in class
      loadEnvironment();
    }

    proc_env[name.str()] = value.str(); // create or replace existing value
    ::setenv(name.data(),value.data(),1);
#endif
}

void Portable::unsetenv(const QCString &variable)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    SetEnvironmentVariable(variable.data(),nullptr);
#else
    /* Some systems don't have unsetenv(), so we do it ourselves */
    if (variable.isEmpty() || variable.find('=')!=-1)
    {
      return; // not properly formatted
    }

    auto it = proc_env.find(variable.str());
    if (it != proc_env.end())
    {
      proc_env.erase(it);
      ::unsetenv(variable.data());
    }
#endif
}

QCString Portable::getenv(const QCString &variable)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    #define ENV_BUFSIZE 32768
    LPTSTR pszVal = (LPTSTR) malloc(ENV_BUFSIZE*sizeof(TCHAR));
    if (GetEnvironmentVariable(variable.data(),pszVal,ENV_BUFSIZE) == 0) return "";
    QCString out;
    out = pszVal;
    free(pszVal);
    return out;
    #undef ENV_BUFSIZE
#else
    if(!environmentLoaded) // if the environment variables are not loaded already...
    {                      // ...call loadEnvironment to store them in class
      loadEnvironment();
    }

    if (proc_env.find(variable.str()) != proc_env.end())
    {
      return QCString(proc_env[variable.str()]);
    }
    else
    {
      return QCString();
    }
#endif
}

FILE *Portable::fopen(const QCString &fileName,const QCString &mode)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  uint16_t *fn = nullptr;
  size_t fn_len = recodeUtf8StringToW(fileName,&fn);
  uint16_t *m  = nullptr;
  size_t m_len = recodeUtf8StringToW(mode,&m);
  FILE *result = nullptr;
  if (fn_len!=(size_t)-1 && m_len!=(size_t)-1)
  {
    result = _wfopen((wchar_t*)fn,(wchar_t*)m);
  }
  delete[] fn;
  delete[] m;
  return result;
#else
  return ::fopen(fileName.data(),mode.data());
#endif
}

int Portable::fclose(FILE *f)
{
  return ::fclose(f);
}

QCString Portable::pathSeparator()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return "\\";
#else
  return "/";
#endif
}

QCString Portable::pathListSeparator()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return ";";
#else
  return ":";
#endif
}

static bool ExistsOnPath(const QCString &fileName)
{
  FileInfo fi1(fileName.str());
  if (fi1.exists()) return true;

  QCString paths = Portable::getenv("PATH");
  char listSep = Portable::pathListSeparator()[0];
  char pathSep = Portable::pathSeparator()[0];
  int strt = 0;
  int idx;
  while ((idx = paths.find(listSep,strt)) != -1)
  {
    QCString locFile(paths.mid(strt,idx-strt));
    locFile += pathSep;
    locFile += fileName;
    FileInfo fi(locFile.str());
    if (fi.exists()) return true;
    strt = idx + 1;
  }
  // to be sure the last path component is checked as well
  QCString locFile(paths.mid(strt));
  if (!locFile.isEmpty())
  {
    locFile += pathSep;
    locFile += fileName;
    FileInfo fi(locFile.str());
    if (fi.exists()) return true;
  }
  return false;
}

bool Portable::checkForExecutable(const QCString &fileName)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  const char *extensions[] = {".bat",".com",".exe"};
  for (int i = 0; i < sizeof(extensions) / sizeof(*extensions); i++)
  {
    if (ExistsOnPath(fileName + extensions[i])) return true;
  }
  return false;
#else
  return ExistsOnPath(fileName);
#endif
}

const char *Portable::ghostScriptCommand()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    static const char *gsexe = nullptr;
    if (!gsexe)
    {
        const char *gsExec[] = {"gswin32c.exe","gswin64c.exe"};
        for (int i = 0; i < sizeof(gsExec) / sizeof(*gsExec); i++)
        {
            if (ExistsOnPath(gsExec[i]))
	    {
                gsexe = gsExec[i];
                return gsexe;
            }
        }
        gsexe = gsExec[0];
        return gsexe;
    }
    return gsexe;
#else
    return "gs";
#endif
}

const char *Portable::commandExtension()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    return ".exe";
#else
    return "";
#endif
}

bool Portable::fileSystemIsCaseSensitive()
{
#if defined(_WIN32) || defined(macintosh) || defined(__MACOSX__) || defined(__APPLE__) || defined(__CYGWIN__)
  return FALSE;
#else
  return TRUE;
#endif
}

FILE * Portable::popen(const QCString &name,const QCString &type)
{
  #if defined(_MSC_VER) || defined(__BORLANDC__)
  return ::_popen(name.data(),type.data());
  #else
  return ::popen(name.data(),type.data());
  #endif
}

int Portable::pclose(FILE *stream)
{
  #if defined(_MSC_VER) || defined(__BORLANDC__)
  return ::_pclose(stream);
  #else
  return ::pclose(stream);
  #endif
}

bool Portable::isAbsolutePath(const QCString &fileName)
{
  const char *fn = fileName.data();
# ifdef _WIN32
  if (fileName.length()>1 && isalpha(fileName[0]) && fileName[1]==':') fn+=2;
# endif
  char const fst = fn[0];
  if (fst == '/') return true;
# ifdef _WIN32
  if (fst == '\\') return true;
# endif
  return false;
}

/**
 * Correct a possible wrong PATH variable
 *
 * This routine was inspired by the cause for bug 766059 was that in the Windows path there were forward slashes.
 */
void Portable::correctPath(const StringVector &extraPaths)
{
  QCString p = Portable::getenv("PATH");
  bool first=true;
  QCString result;
#if defined(_WIN32) && !defined(__CYGWIN__)
  for (const auto &path : extraPaths)
  {
    if (!first) result+=';';
    first=false;
    result += substitute(QCString(path),"/","\\");
  }
  if (!result.isEmpty() && !p.isEmpty()) result+=';';
  result += substitute(p,"/","\\");
#else
  for (const auto &path : extraPaths)
  {
    if (!first) result+=':';
    first=false;
    result += QCString(path);
  }
  if (!result.isEmpty() && !p.isEmpty()) result+=':';
  result += p;
#endif
  if (result!=p) Portable::setenv("PATH",result.data());
  //printf("settingPath(%s) #extraPaths=%zu\n",Portable::getenv("PATH").data(),extraPaths.size());
}

void Portable::unlink(const QCString &fileName)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  _unlink(fileName.data());
#else
  ::unlink(fileName.data());
#endif
}

void Portable::setShortDir()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  long     length = 0;
  TCHAR*   buffer = nullptr;
  // First obtain the size needed by passing nullptr and 0.
  length = GetShortPathName(Dir::currentDirPath().c_str(), nullptr, 0);
  // Dynamically allocate the correct size
  // (terminating null char was included in length)
  buffer = new TCHAR[length];
  // Now simply call again using same (long) path.
  length = GetShortPathName(Dir::currentDirPath().c_str(), buffer, length);
  // Set the correct directory (short name)
  Dir::setCurrent(buffer);
  delete [] buffer;
#endif
}

/* Return the first occurrence of NEEDLE in HAYSTACK.  */
static const char * portable_memmem (const char *haystack, size_t haystack_len,
                        const char *needle, size_t needle_len)
{
  const char *const last_possible = haystack + haystack_len - needle_len;

  if (needle_len == 0)
    // The first occurrence of the empty string should to occur at the beginning of the string.
  {
    return haystack;
  }

  // Sanity check
  if (haystack_len < needle_len)
  {
    return nullptr;
  }

  for (const char *begin = haystack; begin <= last_possible; ++begin)
  {
    if (begin[0] == needle[0] && !memcmp(&begin[1], needle + 1, needle_len - 1))
    {
      return begin;
    }
  }

  return nullptr;
}

const char *Portable::strnstr(const char *haystack, const char *needle, size_t haystack_len)
{
  size_t needle_len = strnlen(needle, haystack_len);
  if (needle_len < haystack_len || !needle[needle_len])
  {
    const char *x = portable_memmem(haystack, haystack_len, needle, needle_len);
    if (x && !memchr(haystack, 0, x - haystack))
    {
      return x;
    }
  }
  return nullptr;
}

const char *Portable::devNull()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return "NUL";
#else
  return "/dev/null";
#endif
}

size_t Portable::recodeUtf8StringToW(const QCString &inputStr,uint16_t **outBuf)
{
  if (inputStr.isEmpty() || outBuf==nullptr) return 0; // empty input or invalid output
  void *handle = portable_iconv_open("UTF-16LE","UTF-8");
  if (handle==reinterpret_cast<void *>(-1)) return 0; // invalid encoding
  size_t len = inputStr.length();
  uint16_t *buf = new uint16_t[len+1];
  *outBuf = buf;
  size_t inRemains  = len;
  size_t outRemains = len*sizeof(uint16_t)+2; // chars + \0
  const char *p = inputStr.data();
  portable_iconv(handle,&p,&inRemains,reinterpret_cast<char **>(&buf),&outRemains);
  *buf=0;
  portable_iconv_close(handle);
  return len;
}

//----------------------------------------------------------------------------------------
// We need to do this part last as including filesystem.hpp earlier
// causes the code above to fail to compile on Windows.

#include "filesystem.hpp"

namespace fs = ghc::filesystem;

std::ofstream Portable::openOutputStream(const QCString &fileName,bool append)
{
  std::ios_base::openmode mode = std::ofstream::out | std::ofstream::binary;
  if (append) mode |= std::ofstream::app;
#if defined(__clang__) && defined(__MINGW32__)
  return std::ofstream(fs::path(fileName.str()).wstring(), mode);
#else
  return std::ofstream(fs::path(fileName.str()), mode);
#endif
}

std::ifstream Portable::openInputStream(const QCString &fileName,bool binary, bool openAtEnd)
{
  std::ios_base::openmode mode = std::ifstream::in | std::ifstream::binary;
  if (binary)     mode |= std::ios::binary;
  if (openAtEnd)  mode |= std::ios::ate;
#if defined(__clang__) && defined(__MINGW32__)
  return std::ifstream(fs::path(fileName.str()).wstring(), mode);
#else
  return std::ifstream(fs::path(fileName.str()), mode);
#endif
}

