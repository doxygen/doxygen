#ifndef SETTINGS_H
#define SETTINGS_H

#define USE_SQLITE3  0
#define USE_LIBCLANG 0

#define IS_SUPPORTED(x) \
  ((USE_SQLITE3  && strcmp("USE_SQLITE3",(x))==0)  || \
   (USE_LIBCLANG && strcmp("USE_LIBCLANG",(x))==0) || \
  0)

#endif
