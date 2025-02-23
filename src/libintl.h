#ifndef _LIBINTL_H
#define _LIBINTL_H 1

#ifdef __cplusplus
extern "C" {
#endif

# include <stdio.h>
# include <stddef.h>
 # include <locale.h>

#if (defined __STDC__ && __STDC__) || defined __cplusplus || defined _MSC_VER
#  include <stdarg.h>
#else
#  include <varargs.h>
#endif


/* Version number: (major<<16) + (minor<<8) + subminor */
#define LIBINTL_VERSION 0x001605
extern int libintl_version;

extern char    * gettext (const char *__msgid);

extern char    *     textdomain (const char *__domainname);
extern char    * bindtextdomain (const char *__domainname, const char *__dirname);
extern char    * libintl_textdomain (const char *__domainname);
extern wchar_t * libintl_wbindtextdomain (const char *__domainname,
                                         const wchar_t *__wdirname);

extern char   * libintl_bind_textdomain_codeset (const char *__domainname,
                                              const char *__codeset);

#ifdef __cplusplus
}
#endif

#endif /* libintl.h */
