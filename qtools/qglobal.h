/****************************************************************************
** 
**
** Global type declarations and definitions
**
** Created : 920529
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QGLOBAL_H
#define QGLOBAL_H


#define QT_VERSION	223
#define QT_VERSION_STR	"2.2.3"


//
// The operating system, must be one of: (_OS_x_)
//
//   MAC	- Macintosh
//   MSDOS	- MS-DOS and Windows
//   OS2	- OS/2
//   OS2EMX	- XFree86 on OS/2 (not PM)
//   WIN32	- Win32 (Windows 95/98 and Windows NT)
//   SUN	- SunOS
//   SOLARIS	- Sun Solaris
//   HPUX	- HP-UX
//   ULTRIX	- DEC Ultrix
//   LINUX	- Linux
//   FREEBSD	- FreeBSD
//   NETBSD	- NetBSD
//   OPENBSD    - OpenBSD
//   IRIX	- SGI Irix
//   OSF	- OSF Unix
//   BSDI	- BSDI Unix
//   SCO	- SCO of some sort
//   AIX	- AIX Unix
//   UNIXWARE	- SCO UnixWare
//   GNU	- GNU Hurd
//   DGUX	- DG Unix
//   UNIX	- Any UNIX bsd/sysv system
//

#if defined(__APPLE__) || defined(macintosh)
#define _OS_MAC_
#  ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#    undef MAC_OS_X_VERSION_MIN_REQUIRED
#  endif
#  define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_3
#  include <AvailabilityMacros.h>
#  if !defined(MAC_OS_X_VERSION_10_3)
#     define MAC_OS_X_VERSION_10_3 MAC_OS_X_VERSION_10_2 + 10
#  endif
#  if !defined(MAC_OS_X_VERSION_10_4)
#       define MAC_OS_X_VERSION_10_4 MAC_OS_X_VERSION_10_3 + 10
#  endif
#  if !defined(MAC_OS_X_VERSION_10_5)
#       define MAC_OS_X_VERSION_10_5 MAC_OS_X_VERSION_10_4 + 10
#  endif
#  if !defined(MAC_OS_X_VERSION_10_6)
#       define MAC_OS_X_VERSION_10_6 MAC_OS_X_VERSION_10_5 + 10
#  endif
#  if !defined(MAC_OS_X_VERSION_10_7)
#       define MAC_OS_X_VERSION_10_7 MAC_OS_X_VERSION_10_6 + 10
#  endif
#  if !defined(MAC_OS_X_VERSION_10_8)
#       define MAC_OS_X_VERSION_10_8 MAC_OS_X_VERSION_10_7 + 10
#  endif
#  if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_8)
#    warning "This version of Mac OS X is unsupported"
#  endif
#elif defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__)
#define _OS_MSDOS_
#elif defined(OS2) || defined(_OS2) || defined(__OS2__)
#if defined(__EMX__)
#define _OS_OS2EMX_
#else
#define _OS_OS2_
#endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define _OS_WIN32_
#elif defined(__MWERKS__) && defined(__INTEL__)
#define _OS_WIN32_
#elif defined(sun) || defined(__sun) || defined(__sun__)
#if defined(__SVR4)
#define _OS_SOLARIS_
#else
#define _OS_SUN_
#endif
#elif defined(hpux) || defined(__hpux) || defined(__hpux__)
#define _OS_HPUX_
#elif defined(ultrix) || defined(__ultrix) || defined(__ultrix__)
#define _OS_ULTRIX_
#elif defined(reliantunix)
#define _OS_RELIANTUNIX_
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#define _OS_LINUX_
#elif defined(__FreeBSD__)
#define _OS_FREEBSD_
#elif defined(__NetBSD__)
#define _OS_NETBSD_
#elif defined(__OpenBSD__)
#define _OS_OPENBSD_
#elif defined(sgi) || defined(__sgi)
#define _OS_IRIX_
#elif defined(__osf__)
#define _OS_OSF_
#elif defined(bsdi) || defined(__bsdi__)
#define _OS_BSDI_
#elif defined(_AIX)
#define _OS_AIX_
#elif defined(__Lynx__)
#define _OS_LYNXOS_
#elif defined(_UNIXWARE)
#define _OS_UNIXWARE_
#elif defined(DGUX)
#define _OS_DGUX_
#elif defined(__QNX__)
#define _OS_QNX_
#elif defined(_SCO_DS) || defined(M_UNIX) || defined(M_XENIX)
#define _OS_SCO_
#elif defined(sco) || defined(_UNIXWARE7)
#define _OS_UNIXWARE7_
#elif !defined(_SCO_DS) && defined(__USLC__) && defined(__SCO_VERSION__)
#define _OS_UNIXWARE7_
#elif defined(__CYGWIN__)
#define _OS_CYGWIN_
#elif defined(__BEOS__)
#define _OS_BEOS_
#elif defined(__MINT__)
#define _OS_MINT_
#else
#error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(_OS_MAC_) || defined(_OS_MSDOS_) || defined(_OS_OS2_) || defined(_OS_WIN32_)
#undef	_OS_UNIX_
#elif !defined(_OS_UNIX_)
#define _OS_UNIX_
// QT_CLEAN_NAMESPACE is not defined by default; it would break too
// much code.
#if !defined(QT_CLEAN_NAMESPACE) && !defined(UNIX)
// ### remove 3.0
#define UNIX
#endif
#endif


//
// The compiler, must be one of: (_CC_x_)
//
//   SYM	- Symantec C++ for both PC and Macintosh
//   MPW	- MPW C++
//   MWERKS	- Metrowerks CodeWarrior
//   MSVC	- Microsoft Visual C/C++
//   BOR	- Borland/Turbo C++
//   WAT	- Watcom C++
//   GNU	- GNU C++
//   COMEAU	- Comeau C++
//   EDG	- Edison Design Group C++
//   OC		- CenterLine C++
//   SUN	- Sun C++
//   DEC	- DEC C++
//   HP		- HPUX C++
//   USLC	- SCO UnixWare7 C++
//   CDS	- Reliant C++
//   KAI	- KAI C++
//


// Should be sorted most-authorative to least-authorative

#if defined(__SC__)
#define _CC_SYM_
#elif defined( __KCC )
#define _CC_KAI_
#define _CC_EDG_
#define Q_HAS_BOOL_TYPE
#elif defined(applec)
#define _CC_MPW_
#elif defined(__MWERKS__)
#define _CC_MWERKS_
#define Q_HAS_BOOL_TYPE
#elif defined(_MSC_VER)
#define _CC_MSVC_
#elif defined(__BORLANDC__) || defined(__TURBOC__)
#define _CC_BOR_
#elif defined(__WATCOMC__)
#define _CC_WAT_
#define Q_HAS_BOOL_TYPE
#elif defined(__GNUC__)
#define _CC_GNU_
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 7
#define Q_FULL_TEMPLATE_INSTANTIATION
#define Q_TEMPLATE_NEEDS_CLASS_DECLARATION
#define Q_TEMPLATE_NEEDS_EXPLICIT_CONVERSION
#define Q_SPURIOUS_NON_VOID_WARNING
#endif
#if __GNUC__ == 2 && __GNUC_MINOR__ >= 95
#define Q_DELETING_VOID_UNDEFINED
#endif
#if (defined(__arm__) || defined(__ARMEL__)) && !defined(QT_MOC_CPP)
#define Q_PACKED __attribute__ ((packed))
#endif
#elif defined(__xlC__)
#define _CC_XLC_
#define Q_FULL_TEMPLATE_INSTANTIATION
#if __xlC__ >= 0x400
#define Q_HAS_BOOL_TYPE
#endif
#if __xlC__ <= 0x0306
#define Q_TEMPLATE_NEEDS_EXPLICIT_CONVERSION
#endif
#elif defined(como40)
#define _CC_EDG_
#define _CC_COMEAU_
#define Q_HAS_BOOL_TYPE
#define Q_C_CALLBACKS
#elif defined(__USLC__)
#define _CC_USLC_
#ifdef __EDG__ // UnixWare7
#define Q_HAS_BOOL_TYPE
#endif
#elif defined(__DECCXX)
#define _CC_DEC_
#if __DECCXX_VER >= 60060005
#define Q_HAS_BOOL_TYPE
#endif
#elif defined(__EDG) || defined(__EDG__)
// one observed on SGI DCC, the other documented
#define _CC_EDG_
#elif defined(OBJECTCENTER) || defined(CENTERLINE_CLPP)
#define _CC_OC_
#if defined(_BOOL)
#define Q_HAS_BOOL_TYPE
#endif
#elif defined(__SUNPRO_CC)
#define _CC_SUN_
#if __SUNPRO_CC >= 0x500
#define Q_HAS_BOOL_TYPE
#define Q_C_CALLBACKS
#endif
#elif defined(__CDS__)
#define _CC_CDS_
#define Q_HAS_BOOL_TYPE
#elif defined(_OS_HPUX_)
// this test is from aCC online help
#if defined(__HP_aCC) || __cplusplus >= 199707L
// this is the aCC
#define _CC_HP_ACC_
#define Q_HAS_BOOL_TYPE
#else
// this is the CC
#define _CC_HP_
#define Q_FULL_TEMPLATE_INSTANTIATION
#define Q_TEMPLATE_NEEDS_EXPLICIT_CONVERSION
#endif // __HP_aCC
#else
#error "Qt has not been tested with this compiler - talk to qt-bugs@trolltech.com"
#endif

// detect Microsoft compiler version
#ifdef _CC_MSVC_
#if _MSC_VER >= 1400
#define _CC_V2005
#elif _MSC_VER >= 1310
#define _CC_V2003
#elif _MSC_VER > 1300
#define _CC_V2002
#else
#define _CC_V1998
#endif 
#endif

#ifndef Q_PACKED
#define Q_PACKED
#endif

// Window system setting

#if defined(_OS_MAC_)
#define _WS_MAC_
#elif defined(_OS_MSDOS_)
#define _WS_WIN16_
#error "Qt requires Win32 and does not work with Windows 3.x"
#elif defined(_WIN32_X11_)
#define _WS_X11_
#elif defined(_OS_WIN32_)
#define _WS_WIN32_
#elif defined(_OS_OS2_)
#error "Qt does not work with OS/2 Presentation Manager or Workplace Shell"
#elif defined(_OS_UNIX_)
#ifdef QWS
#define _WS_QWS_
#else
#define _WS_X11_
#endif
#endif

#if defined(_WS_WIN16_) || defined(_WS_WIN32_)
#define _WS_WIN_
#endif


//
// Some classes do not permit copies to be made of an object.
// These classes contains a private copy constructor and operator=
// to disable copying (the compiler gives an error message).
// Undefine Q_DISABLE_COPY to turn off this checking.
//

#define Q_DISABLE_COPY


//
// Useful type definitions for Qt
//

#if defined(bool)
#define Q_HAS_BOOL_TYPE
#elif __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
#define Q_HAS_BOOL_TYPE
#elif _MSC_VER >= 1100 || __BORLANDC__ >= 0x500
#define Q_HAS_BOOL_TYPE
#elif defined(sgi) && defined(_BOOL)
#define Q_HAS_BOOL_TYPE
#endif

#if (QT_VERSION >= 300)
#error "Use an enum for bool"
#endif

#if !defined(Q_HAS_BOOL_TYPE)
#if defined(_CC_MSVC_)
#define _CC_BOOL_DEF_
#define bool		int
#else
typedef int		bool;
#endif
#endif

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned	uint;
typedef unsigned long	ulong;
typedef char	       *pchar;
typedef uchar	       *puchar;
typedef const char     *pcchar;
#if defined(_OS_WIN32_) && !defined(_CC_GNU_)
typedef __int64            int64;
typedef unsigned __int64   uint64;
#else
typedef long long          int64;
typedef unsigned long long uint64;
#endif


//
// Constant bool values
//

#ifndef TRUE
const bool FALSE = 0;
const bool TRUE = !0;
#endif


#if defined(_CC_MSVC_)
// Workaround for static const members.
#define QT_STATIC_CONST static
#define QT_STATIC_CONST_IMPL
#else
#define QT_STATIC_CONST static const
#define QT_STATIC_CONST_IMPL const
#endif



//
// Utility macros and inline functions
//

#define QMAX(a,b)	((a) > (b) ? (a) : (b))
#define QMIN(a,b)	((a) < (b) ? (a) : (b))
#define QABS(a)		((a) >= 0  ? (a) : -(a))

inline int qRound( double d )
{
    return d > 0.0 ? int(d+0.5) : int(d-0.5);
}


//
// Size-dependent types (architechture-dependent byte order)
//

// QT_CLEAN_NAMESPACE is not defined by default; it would break too
// much code.
#if !defined(QT_CLEAN_NAMESPACE)
typedef signed char	INT8;			// 8 bit signed
typedef unsigned char	UINT8;			// 8 bit unsigned
typedef short		INT16;			// 16 bit signed
typedef unsigned short	UINT16;			// 16 bit unsigned
typedef int		INT32;			// 32 bit signed
typedef unsigned int	UINT32;			// 32 bit unsigned
#endif

typedef signed char	Q_INT8;			// 8 bit signed
typedef unsigned char	Q_UINT8;		// 8 bit unsigned
typedef short		Q_INT16;		// 16 bit signed
typedef unsigned short	Q_UINT16;		// 16 bit unsigned
typedef int		Q_INT32;		// 32 bit signed
typedef unsigned int	Q_UINT32;		// 32 bit unsigned
typedef long		Q_INT64;		// up to 64 bit signed
typedef unsigned long	Q_UINT64;		// up to 64 bit unsigned

//
// Data stream functions is provided by many classes (defined in qdatastream.h)
//

class QDataStream;



#ifdef _WS_WIN_
extern bool qt_winunicode;
#endif

#ifndef QT_H
#include <qfeatures.h>
#endif // QT_H

//
// Create Qt DLL if QT_DLL is defined (Windows only)
//

#if defined(_OS_WIN32_)
#if defined(QT_NODLL)
#undef QT_MAKEDLL
#undef QT_DLL
#endif
#ifdef QT_DLL
#if defined(QT_MAKEDLL)		/* create a Qt DLL library */
#undef QT_DLL
#define Q_EXPORT  __declspec(dllexport)
#define Q_TEMPLATEDLL
#undef  Q_DISABLE_COPY		/* avoid unresolved externals */
#endif
#endif
#if defined(QT_DLL)		/* use a Qt DLL library */
#define Q_EXPORT  __declspec(dllimport)
#define Q_TEMPLATEDLL
#undef  Q_DISABLE_COPY		/* avoid unresolved externals */
#endif
#else // ! _OS_WIN32_
#undef QT_MAKEDLL		/* ignore these for other platforms */
#undef QT_DLL
#endif

#ifndef Q_EXPORT
#define Q_EXPORT
#endif

//
// System information
//

Q_EXPORT const char *qVersion();
Q_EXPORT bool qSysInfo( int *wordSize, bool *bigEndian );


//
// Debugging and error handling
//

#if !defined(NO_CHECK)
#define CHECK_STATE				// check state of objects etc.
#define CHECK_RANGE				// check range of indexes etc.
#define CHECK_NULL				// check null pointers
#define CHECK_MATH				// check math functions
#endif

#if !defined(NO_DEBUG) && !defined(DEBUG)
#define DEBUG					// display debug messages
#endif

//
// Avoid some particularly useless warnings from some stupid compilers.
// To get ALL C++ compiler warnings, define CC_WARNINGS or comment out
// the line "#define Q_NO_WARNINGS"
//

#if !defined(CC_WARNINGS)
#define Q_NO_WARNINGS
#endif
#if defined(Q_NO_WARNINGS)
#if defined(_CC_MSVC_)
#pragma warning(disable: 4244)
#pragma warning(disable: 4275)
#pragma warning(disable: 4514)
#pragma warning(disable: 4800)
#pragma warning(disable: 4097)
#pragma warning(disable: 4706)
#elif defined(_CC_BOR_)
#pragma option -w-inl
#pragma option -w-aus
#pragma warn -inl
#pragma warn -pia
#pragma warn -ccc
#pragma warn -rch
#pragma warn -sig
#elif defined(_CC_MWERKS_)
#pragma warn_possunwant off
#endif
#endif // Q_NO_WARNINGS

//
// Avoid dead code
//

#if defined(_CC_EDG_) || defined(_CC_WAT_)
#define Q_NO_DEAD_CODE
#endif

//
// Use to avoid "unused parameter" warnings
//

#define Q_UNUSED(x) x=x;
#define Q_CONST_UNUSED(x) (void)x;

Q_EXPORT void qDebug( const char *, ... )	// print debug message
#if defined(_CC_GNU_) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void qWarning( const char *, ... )	// print warning message
#if defined(_CC_GNU_) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void qFatal( const char *, ... )	// print fatal message and exit
#if defined(_CC_GNU_)
    __attribute__ ((format (printf, 1, 2)))
#endif
;


// QT_CLEAN_NAMESPACE is not defined by default; it would break too
// much code.
#if !defined(QT_CLEAN_NAMESPACE)
// in that case, also define the old ones...

Q_EXPORT void debug( const char *, ... )	// print debug message
#if defined(_CC_GNU_) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void warning( const char *, ... )	// print warning message
#if defined(_CC_GNU_) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void fatal( const char *, ... )	// print fatal message and exit
#if defined(_CC_GNU_) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

// okay, that was debug()/warning()/fatal()
#endif

#if !defined(ASSERT)
#if defined(CHECK_STATE)
#if defined(QT_FATAL_ASSERT)
#define ASSERT(x)  if ( !(x) )\
	qFatal("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__)
#else
#define ASSERT(x)  if ( !(x) )\
	qWarning("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__)
#endif
#else
#define ASSERT(x)
#endif
#endif

Q_EXPORT bool qt_check_pointer( bool c, const char *, int );

#if defined(CHECK_NULL)
#define CHECK_PTR(p) (qt_check_pointer((p)==0,__FILE__,__LINE__))
#else
#define CHECK_PTR(p)
#endif

enum QtMsgType { QtDebugMsg, QtWarningMsg, QtFatalMsg };

typedef void (*msg_handler)(QtMsgType, const char *);
Q_EXPORT msg_handler qInstallMsgHandler( msg_handler );


Q_EXPORT void qSuppressObsoleteWarnings( bool = TRUE );

#if !defined(QT_REJECT_OBSOLETE)
#define QT_OBSOLETE
Q_EXPORT void qObsolete( const char *obj, const char *oldfunc,
			 const char *newfunc );
Q_EXPORT void qObsolete( const char *obj, const char *oldfunc );
Q_EXPORT void qObsolete( const char *message );
#endif

// DvH: added to avoid warnings on recent gcc versions
#define Q_DELETING_VOID_UNDEFINED

#endif // QGLOBAL_H
