#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_LIBCLANG 0
#define IS_SUPPORTED(x) \
  ( \
   (USE_LIBCLANG && strcmp("USE_LIBCLANG",(x))==0) || \
  0)
#endif