#ifndef MSCGEN_CONFIG_H
#define MSCGEN_CONFIG_H

#if defined(_WIN32) && !defined(__CYGWIN__)
#define strdup _strdup
#define fileno _fileno
#define YY_NO_UNISTD_H 1
#else
#define HAVE_UNISTD_H 1
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
#ifdef _MSC_VER
#  ifndef strcasecmp
#    define strcasecmp _stricmp
#  endif 
#endif

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <unistd.h> header file. */

/* Name of package */
#define PACKAGE "mscgen"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Michael.McTernan.2001@cs.bris.ac.uk"

/* Define to the full name of this package. */
#define PACKAGE_NAME "mscgen"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "mscgen 0.20"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "mscgen"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.20"

/* If set, remove PNG output support thereby removing libgd dependence. */
//#define REMOVE_PNG_OUTPUT /**/

/* Define to 1 if you have the ANSI C header files. */
//#define STDC_HEADERS 1

/* Use FreeType for rendering text in PNGs. */
/* #undef USE_FREETYPE */

/* Version number of package */
#define VERSION "0.20"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1
#endif /* MSCGEN_CONFIG_H */
