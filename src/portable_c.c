#include <iconv.h>

// These functions are implemented in a C file, because there are different
// versions of the iconv() prototype, some with a const pointer and some
// without. In C this is just a warning, but in C++ breaks the compilation.
// Looking at the LIBICONV_VERSION is not enough, since for MACOSX the 
// const and non-const version exist with the same version of the file.

void * portable_iconv_open(const char* tocode, const char* fromcode)
{
  return iconv_open(tocode,fromcode);
}

size_t portable_iconv (void *cd, char** inbuf,  size_t *inbytesleft, 
                                 char** outbuf, size_t *outbytesleft)
{
  return iconv((iconv_t)cd,inbuf,inbytesleft,outbuf,outbytesleft);
}

int portable_iconv_close (void *cd)
{
  return iconv_close((iconv_t)cd);
}

