#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#include <fstream>

#include "containers.h"
#include "qcstring.h"

class Buf;

/** @file
 *  @brief Portable versions of functions that are platform dependent.
 */

namespace Portable
{
  int            system(const QCString &command,const QCString &args,bool commandHasConsole=true);
  uint32_t       pid();
  QCString       getenv(const QCString &variable);
  void           setenv(const QCString &variable,const QCString &value);
  void           unsetenv(const QCString &variable);
  FILE *         fopen(const QCString &fileName,const QCString &mode);
  int            fclose(FILE *f);
  void           unlink(const QCString &fileName);
  QCString       pathSeparator();
  QCString       pathListSeparator();
  const char *   ghostScriptCommand();
  const char *   commandExtension();
  bool           fileSystemIsCaseSensitive();
  FILE *         popen(const QCString &name,const QCString &type);
  int            pclose(FILE *stream);
  double         getSysElapsedTime();
  bool           isAbsolutePath(const QCString &fileName);
  void           correctPath(const StringVector &list);
  void           setShortDir();
  const char *   strnstr(const char *haystack, const char *needle, size_t haystack_len);
  const char *   devNull();
  bool           checkForExecutable(const QCString &fileName);
  size_t         recodeUtf8StringToW(const QCString &inputStr,uint16_t **buf);
  std::ofstream  openOutputStream(const QCString &name,bool append=false);
  std::ifstream  openInputStream(const QCString &name,bool binary=false,bool openAtEnd=false);
}


extern "C" {
  void *         portable_iconv_open(const char *tocode, const char *fromcode);
  size_t         portable_iconv (void *cd, const char** inbuf, size_t *inbytesleft,
                                 char* * outbuf, size_t *outbytesleft);
  int            portable_iconv_close (void *cd);
}

#endif

