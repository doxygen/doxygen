#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

static int s_debugLevel = 0;

void debug(int level,const char *msg,...)
{
  if (level<=s_debugLevel)
  {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args); 
  }
}

void setDebugLevel(int level)
{
  s_debugLevel = level;
}

