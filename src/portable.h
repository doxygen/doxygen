#ifndef PORTABLE_H
#define PORTABLE_H

#include <sys/types.h>
#include <stdio.h>
#include <qglobal.h>

#if defined(_WIN32)
typedef __int64 portable_off_t;
#else
typedef off_t portable_off_t;
#endif

/** @file
 *  @brief Portable versions of functions that are platform dependent.
 */

int            portable_system(const char *command,const char *args,bool commandHasConsole=TRUE);
uint           portable_pid();
const char *   portable_getenv(const char *variable);
void           portable_setenv(const char *variable,const char *value);
void           portable_unsetenv(const char *variable);
portable_off_t portable_fseek(FILE *f,portable_off_t offset, int whence);
portable_off_t portable_ftell(FILE *f);
FILE *         portable_fopen(const char *fileName,const char *mode);
char           portable_pathSeparator();
char           portable_pathListSeparator();
const char *   portable_ghostScriptCommand();
const char *   portable_commandExtension();
bool           portable_fileSystemIsCaseSensitive();
FILE *         portable_popen(const char *name,const char *type);
int            portable_pclose(FILE *stream);
void           portable_sysTimerStart();
void           portable_sysTimerStop();
double         portable_getSysElapsedTime();
void           portable_sleep(int ms);

extern "C" {
  void *         portable_iconv_open(const char* tocode, const char* fromcode);
  size_t         portable_iconv (void *cd, char** inbuf, size_t *inbytesleft, 
                                 char* * outbuf, size_t *outbytesleft);
  int            portable_iconv_close (void *cd);
}


#endif

