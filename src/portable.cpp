#include "portable.h"

#include <stdlib.h>
#include <stdio.h>

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

#include <ctype.h>
#include <qglobal.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qdir.h>
#include <map>
#include <string>

#include "util.h"
#ifndef NODEBUG
#include "debug.h"
#endif

#if !defined(_WIN32) || defined(__CYGWIN__)
static bool environmentLoaded = false;
static std::map<std::string,std::string> proc_env = std::map<std::string,std::string>();
#endif

static double  g_sysElapsedTime;
static QTime   g_time;


int Portable::system(const char *command,const char *args,bool commandHasConsole)
{

  if (command==0) return 1;

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
    const char * argv[4];
    argv[0] = "sh";
    argv[1] = "-c";
    argv[2] = fullCmd.data();
    argv[3] = 0;
    execve("/bin/sh",(char * const *)argv,environ);
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
    return ::system(fullCmd);
  }
  else
  {
    // Because ShellExecuteEx can delegate execution to Shell extensions 
    // (data sources, context menu handlers, verb implementations) that 
    // are activated using Component Object Model (COM), COM should be 
    // initialized before ShellExecuteEx is called. Some Shell extensions 
    // require the COM single-threaded apartment (STA) type. 
    // For that case COM is initialized as follows
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    QString commandw = QString::fromUtf8( commandCorrectedPath );
    QString argsw = QString::fromUtf8( args );

    // gswin32 is a GUI api which will pop up a window and run
    // asynchronously. To prevent both, we use ShellExecuteEx and
    // WaitForSingleObject (thanks to Robert Golias for the code)

    SHELLEXECUTEINFOW sInfo = {
      sizeof(SHELLEXECUTEINFOW),   /* structure size */
      SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI,  /* tell us the process
                                                       *  handle so we can wait till it's done | 
                                                       *  do not display msg box if error 
                                                       */
      NULL,                       /* window handle */
      NULL,                       /* action to perform: open */
      (LPCWSTR)commandw.ucs2(),   /* file to execute */
      (LPCWSTR)argsw.ucs2(),      /* argument list */ 
      NULL,                       /* use current working dir */
      SW_HIDE,                    /* minimize on start-up */
      0,                          /* application instance handle */
      NULL,                       /* ignored: id list */
      NULL,                       /* ignored: class name */
      NULL,                       /* ignored: key class */
      0,                          /* ignored: hot key */
      NULL,                       /* ignored: icon */
      NULL                        /* resulting application handle */
    };

    if (!ShellExecuteExW(&sInfo))
    {
      return -1;
    }
    else if (sInfo.hProcess)      /* executable was launched, wait for it to finish */
    {
      WaitForSingleObject(sInfo.hProcess,INFINITE); 
      /* get process exit code */
      DWORD exitCode;
      if (!GetExitCodeProcess(sInfo.hProcess,&exitCode))
      {
        exitCode = -1;
      }
      CloseHandle(sInfo.hProcess);
      return exitCode;
    }
  }
#endif
  return 1; // we should never get here

}

unsigned int Portable::pid(void)
{
  unsigned int pid;
#if !defined(_WIN32) || defined(__CYGWIN__)
  pid = (unsigned int)getpid();
#else
  pid = (unsigned int)GetCurrentProcessId();
#endif
  return pid;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
#else
  static char **last_environ;
#endif

#if !defined(_WIN32) || defined(__CYGWIN__)
void loadEnvironment()
{
  if(environ != NULL)
  {
    unsigned int i = 0;
    char* current = environ[i];

    while(current != NULL)                            // parse all strings contained by environ til the last element (NULL)
    {
      std::string env_var(current);                   // load current environment variable string
      size_t pos = env_var.find("=");
      if(pos != std::string::npos)                    // only parse the variable, if it is a valid environment variable...
      {                                               // ...which has to contain an equal sign as delimiter by definition
        std::string name = env_var.substr(0,pos);     // the string til the equal sign contains the name
        std::string value = env_var.substr(pos + 1);  // the string from the equal sign contains the value

        proc_env[name] = value;                       // save the value by the name as its key in the classes map
      }
      i++;
      current = environ[i];
    }
  }

  environmentLoaded = true;
}
#endif

void Portable::setenv(const char *name,const char *value)
{
    if (value==0) value="";
#if defined(_WIN32) && !defined(__CYGWIN__)
    SetEnvironmentVariable(name,value);
#else
    if(!environmentLoaded) // if the environment variables are not loaded already...
    {                                 // ...call loadEnvironment to store them in class
      loadEnvironment();
    }

    proc_env[name] = std::string(value); // create or replace exisiting value
#endif
}

void Portable::unsetenv(const char *variable)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    SetEnvironmentVariable(variable,0);
#else
    /* Some systems don't have unsetenv(), so we do it ourselves */
    size_t len;
    char **ep;

    if (variable == NULL || *variable == '\0' || strchr (variable, '=') != NULL)
    {
      return; // not properly formatted
    }

    if(proc_env.find(variable) != proc_env.end())
    {
      proc_env[variable].erase();
    }
#endif
}

const char *Portable::getenv(const char *variable)
{  
#if defined(_WIN32) && !defined(__CYGWIN__)
    return ::getenv(variable);
#else
    if(!environmentLoaded) // if the environment variables are not loaded already...
    {                                 // ...call loadEnvironment to store them in class
      loadEnvironment();
    }

    if(proc_env.find(variable) != proc_env.end())
    {
      return proc_env[variable].c_str();
    }
    else
    {
      return NULL;
    }
#endif
}

portable_off_t Portable::fseek(FILE *f,portable_off_t offset, int whence)
{
#if defined(__MINGW32__)
  return fseeko64(f,offset,whence);
#elif defined(_WIN32) && !defined(__CYGWIN__)
  return _fseeki64(f,offset,whence);
#else
  return fseeko(f,offset,whence);
#endif
}

portable_off_t Portable::ftell(FILE *f)
{
#if defined(__MINGW32__)
  return ftello64(f);  
#elif defined(_WIN32) && !defined(__CYGWIN__)
  return _ftelli64(f);
#else
  return ftello(f);
#endif
}

FILE *Portable::fopen(const char *fileName,const char *mode)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  QString fn(fileName);
  QString m(mode);
  return _wfopen((wchar_t*)fn.ucs2(),(wchar_t*)m.ucs2());
#else
  return ::fopen(fileName,mode);
#endif
}

char  Portable::pathSeparator(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return '\\';
#else
  return '/';
#endif
}

char  Portable::pathListSeparator(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return ';';
#else
  return ':';
#endif
}

const char *Portable::ghostScriptCommand(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    return "gswin32c.exe";
#else
    return "gs";
#endif
}

const char *Portable::commandExtension(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    return ".exe";
#else
    return "";
#endif
}

bool Portable::fileSystemIsCaseSensitive(void)
{
#if defined(_WIN32) || defined(macintosh) || defined(__MACOSX__) || defined(__APPLE__) || defined(__CYGWIN__)
  return FALSE;
#else
  return TRUE;
#endif
}

FILE * Portable::popen(const char *name,const char *type)
{
  #if defined(_MSC_VER) || defined(__BORLANDC__)
  return ::_popen(name,type);
  #else
  return ::popen(name,type);
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

void Portable::sysTimerStart(void)
{
  g_time.start();
}

void Portable::sysTimerStop(void)
{
  g_sysElapsedTime+=((double)g_time.elapsed())/1000.0;
}

double Portable::getSysElapsedTime(void)
{
  return g_sysElapsedTime;
}

void Portable::sleep(int ms)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  Sleep(ms);
#else
  usleep(1000*ms);
#endif
}

bool Portable::isAbsolutePath(const char *fileName)
{
# ifdef _WIN32
  if (isalpha (fileName [0]) && fileName[1] == ':')
    fileName += 2;
# endif
  char const fst = fileName [0];
  if (fst == '/')  {
    return true;
  }
# ifdef _WIN32
  if (fst == '\\')
    return true;
# endif
  return false;
}

/**
 * Correct a possible wrong PATH variable
 *
 * This routine was inspired by the cause for bug 766059 was that in the Windows path there were forward slashes.
 */
void Portable::correct_path(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  const char *p = Portable::getenv("PATH");
  if (!p) return; // no path nothing to correct
  QCString result = substitute(p,'/','\\');
  if (result!=p) Portable::setenv("PATH",result.data());
#endif
}

void Portable::unlink(const char *fileName)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  _unlink(fileName);
#else
  ::unlink(fileName);
#endif
}

void Portable::setShortDir(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  long     length = 0;
  TCHAR*   buffer = NULL;
  // First obtain the size needed by passing NULL and 0.
  length = GetShortPathName(QDir::currentDirPath().data(), NULL, 0);
  // Dynamically allocate the correct size
  // (terminating null char was included in length)
  buffer = new TCHAR[length];
  // Now simply call again using same (long) path.
  length = GetShortPathName(QDir::currentDirPath().data(), buffer, length);
  // Set the correct directory (short name)
  QDir::setCurrent(buffer);
  delete [] buffer;
#endif
}


char *Portable::strnstr(const char *haystack, const char *needle, size_t haystack_len)
{
  size_t needle_len = strnlen(needle, haystack_len);
  if (needle_len < haystack_len || !needle[needle_len]) 
  {
    char *x = static_cast<char*>(memmem(haystack, haystack_len, needle, needle_len));
    if (x && !memchr(haystack, 0, x - haystack))
    {
      return x;
    }
  }
  return 0;
}
