#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdio.h>
#include <sys/types.h>




#if defined(_WIN32)
typedef __int64 portable_off_t;
#else
typedef off_t portable_off_t;
#endif

/** @file
 *  @brief Portable versions of functions that are platform dependent.
 */

namespace Portable
{
  int            system(const char *command,const char *args,bool commandHasConsole=true);
  unsigned int   pid(void);
  const char *   getenv(const char *variable);
  void           setenv(const char *variable,const char *value);
  void           unsetenv(const char *variable);
  portable_off_t fseek(FILE *f,portable_off_t offset, int whence);
  portable_off_t ftell(FILE *f);
  FILE *         fopen(const char *fileName,const char *mode);
  void           unlink(const char *fileName);
  char           pathSeparator(void);
  char           pathListSeparator(void);
  const char *   ghostScriptCommand(void);
  const char *   commandExtension(void);
  bool           fileSystemIsCaseSensitive();
  FILE *         popen(const char *name,const char *type);
  int            pclose(FILE *stream);
  void           sysTimerStart(void);
  void           sysTimerStop(void);
  double         getSysElapsedTime(void);
  void           sleep(int ms);
  bool           isAbsolutePath(const char *fileName);
  void           correct_path(void);
  void           setShortDir(void);
  char *         strnstr(const char *haystack, const char *needle, size_t haystack_len);
}


extern "C" {
  void *         portable_iconv_open(const char* tocode, const char* fromcode);
  size_t         portable_iconv (void *cd, char** inbuf, size_t *inbytesleft, 
                                 char* * outbuf, size_t *outbytesleft);
  int            portable_iconv_close (void *cd);
}

#endif

