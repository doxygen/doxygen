#include <stdlib.h>
#include <ctype.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#undef UNICODE
#define _WIN32_DCOM
#include <windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
extern char **environ;
#endif

#include <qglobal.h>
#include <qdatetime.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

#include "portable.h"
#ifndef NODEBUG
#include "debug.h"
#endif
//#include "doxygen.h"

static double  g_sysElapsedTime;
static QTime   g_time;

int portable_system(const char *command,const char *args,bool commandHasConsole)
{

  if (command==0) return 1;

  QCString fullCmd=command;
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
    return system(fullCmd);
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

    QString commandw = QString::fromUtf8( command );
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

uint portable_pid()
{
  uint pid;
#if !defined(_WIN32) || defined(__CYGWIN__)
  pid = (uint)getpid();
#else
  pid = (uint)GetCurrentProcessId();
#endif
  return pid;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
#else
  static char **last_environ;
#endif

void portable_setenv(const char *name,const char *value)
{
    if (value==0) value="";
#if defined(_WIN32) && !defined(__CYGWIN__)
    SetEnvironmentVariable(name,value);
#else
    register char **ep = 0;
    register size_t size;
    const size_t namelen=qstrlen(name);
    const size_t vallen=qstrlen(value) + 1;

    size = 0;
    if (environ!=0)
    {
      for (ep = environ; *ep; ++ep)
      {
        if (!qstrncmp (*ep, name, (uint)namelen) &&
            (*ep)[namelen] == '=')
          break;
        else
          ++size;
      }
    }

    if (environ==0 || *ep==0) /* add new string */
    {
      char **new_environ;
      if (environ == last_environ && environ!=0)
      {
        // We allocated this space; we can extend it. 
        new_environ = (char **) realloc (last_environ, (size + 2) * sizeof (char *));
      }
      else
      {
        new_environ = (char **) malloc ((size + 2) * sizeof (char *));
      }

      if (new_environ==0) // no more memory 
      {
        return;
      }

      new_environ[size] = (char *)malloc (namelen + 1 + vallen);
      if (new_environ[size]==0)
      {
        free (new_environ);
        return;
      }

      if (environ != last_environ)
      {
        memcpy ((char *) new_environ, environ, size * sizeof (char *));
      }

      memcpy(new_environ[size], name, namelen);
      new_environ[size][namelen] = '=';
      memcpy(&new_environ[size][namelen + 1], value, vallen);
      new_environ[size + 1] = 0;
      last_environ = environ = new_environ;
    }
    else /* replace existing string */
    {
      size_t len = qstrlen (*ep);
      if (len + 1 < namelen + 1 + vallen)
      {
        /* The existing string is too short; malloc a new one.  */
        char *newString = (char *)malloc(namelen + 1 + vallen);
        if (newString==0)
        {
          return;
        }
        *ep = newString;
      }
      memcpy(*ep, name, namelen);
      (*ep)[namelen] = '=';
      memcpy(&(*ep)[namelen + 1], value, vallen);
    }

#endif
}

void portable_unsetenv(const char *variable)
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

    len = qstrlen(variable);

    ep = environ;
    while (*ep != NULL)
    {
      if (!qstrncmp(*ep, variable, (uint)len) && (*ep)[len]=='=')
      {
        /* Found it.  Remove this pointer by moving later ones back.  */
        char **dp = ep;
        do dp[0] = dp[1]; while (*dp++);
        /* Continue the loop in case NAME appears again.  */
      }
      else
      {
        ++ep;
      }
    }
#endif
}

const char *portable_getenv(const char *variable)
{
  return getenv(variable);
}

portable_off_t portable_fseek(FILE *f,portable_off_t offset, int whence)
{
#if defined(__MINGW32__)
  return fseeko64(f,offset,whence);
#elif defined(_WIN32) && !defined(__CYGWIN__)
  return _fseeki64(f,offset,whence);
#else
  return fseeko(f,offset,whence);
#endif
}

portable_off_t portable_ftell(FILE *f)
{
#if defined(__MINGW32__)
  return ftello64(f);  
#elif defined(_WIN32) && !defined(__CYGWIN__)
  return _ftelli64(f);
#else
  return ftello(f);
#endif
}

FILE *portable_fopen(const char *fileName,const char *mode)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  QString fn(fileName);
  QString m(mode);
  wchar_t path[1000];
  GetFullPathNameW((const wchar_t*)fn.ucs2(), 1000UL, path, NULL);
  QString prefix = "\\\\?\\";
  fn = prefix + QString::fromUcs2((const unsigned short *)path);
  return _wfopen((wchar_t*)fn.ucs2(),(wchar_t*)m.ucs2());
#else
  return fopen(fileName,mode);
#endif
}

char  portable_pathSeparator()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return '\\';
#else
  return '/';
#endif
}

char  portable_pathListSeparator()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return ';';
#else
  return ':';
#endif
}

const char *portable_ghostScriptCommand()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    return "gswin32c.exe";
#else
    return "gs";
#endif
}

const char *portable_commandExtension()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    return ".exe";
#else
    return "";
#endif
}

bool portable_fileSystemIsCaseSensitive()
{
#if defined(_WIN32) || defined(macintosh) || defined(__MACOSX__) || defined(__APPLE__)
  return FALSE;
#else
  return TRUE;
#endif
}

FILE * portable_popen(const char *name,const char *type)
{
  return popen(name,type);
}

int portable_pclose(FILE *stream)
{
  return pclose(stream);
}

void portable_sysTimerStart()
{
  g_time.start();
}

void portable_sysTimerStop()
{
  g_sysElapsedTime+=((double)g_time.elapsed())/1000.0;
}

double portable_getSysElapsedTime()
{
  return g_sysElapsedTime;
}

void portable_sleep(int ms)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  Sleep(ms);
#else
  usleep(1000*ms);
#endif
}

bool portable_isAbsolutePath(const char *fileName)
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
 * This routine was inspired by the cause for bug 766059 was that in the Windows path there were forward slahes.
 */
void portable_correct_path(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  const char *p = portable_getenv("PATH");
  char *q = (char *)malloc(strlen(p) + 1);
  strcpy(q, p);
  bool found = false;
  for (int i = 0 ; i < strlen(q); i++)
  {
    if (q[i] == '/')
    {
      q[i] = '\\';
      found = true;
    }
  }
  if (found) portable_setenv("PATH",q);
  free(q);
#endif
}
