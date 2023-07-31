// objective: test tooltip in XHTML
// check: 085__tooltip_8cpp.xml
// config: SOURCE_BROWSER=YES
/** \file */
#include <stdio.h>

/** \brief the unit */
FILE *unit = NULL;

/** a general open macro */
#define FOPEN_MACRO(fn ,mod)           \
  if ((unit = fopen(fn, mod)) == NULL) \
  {                                    \
    msg(OPEN_ERR,strerror(errno));     \
  }

/** a general close macro */
#define FCLOSE_MACRO                   \
  if (fclose(unit) != 0)               \
  {                                    \
    msg(CLOSE_ERR,strerror(errno));    \
  }
