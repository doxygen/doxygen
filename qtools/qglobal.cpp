/****************************************************************************
** 
**
** Global functions
**
** Created : 920604
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

#include "qglobal.h"
#include "qasciidict.h"
#include "qstring.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// NOT REVISED

/*!
  \relates QApplication
  Returns the Qt version number for the library, typically "1.30"
  or "2.1.0".
*/

const char *qVersion()
{
    return QT_VERSION_STR;
}


/*****************************************************************************
  System detection routines
 *****************************************************************************/

static bool si_alreadyDone = FALSE;
static int  si_wordSize;
static bool si_bigEndian;

/*!
  \relates QApplication
  Obtains information about the system.

  The system's word size in bits (typically 32) is returned in \e *wordSize.
  The \e *bigEndian is set to TRUE if this is a big-endian machine,
  or to FALSE if this is a little-endian machine.

  This function calls qFatal() with a message if the computer is truly weird
  (i.e. different endianness for 16 bit and 32 bit integers).
*/

bool qSysInfo( int *wordSize, bool *bigEndian )
{
#if defined(CHECK_NULL)
    ASSERT( wordSize != 0 );
    ASSERT( bigEndian != 0 );
#endif

    if ( si_alreadyDone ) {			// run it only once
	*wordSize  = si_wordSize;
	*bigEndian = si_bigEndian;
	return TRUE;
    }
    si_alreadyDone = TRUE;

    si_wordSize = 0;
    uint n = (uint)(~0);
    while ( n ) {				// detect word size
	si_wordSize++;
	n /= 2;
    }
    *wordSize = si_wordSize;

    if ( *wordSize != 64 &&
	 *wordSize != 32 &&
	 *wordSize != 16 ) {			// word size: 16, 32 or 64
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Unsupported system word size %d", *wordSize );
#endif
	return FALSE;
    }
    if ( sizeof(Q_INT8) != 1 || sizeof(Q_INT16) != 2 || sizeof(Q_INT32) != 4 ||
	 sizeof(float) != 4 || sizeof(double) != 8 ) {
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Unsupported system data type size" );
#endif
	return FALSE;
    }

    bool  be16, be32;				// determine byte ordering
    short ns = 0x1234;
    int	  nl = 0x12345678;

    unsigned char *p = (unsigned char *)(&ns);	// 16-bit integer
    be16 = *p == 0x12;

    p = (unsigned char *)(&nl);			// 32-bit integer
    if ( p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78 )
	be32 = TRUE;
    else
    if ( p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12 )
	be32 = FALSE;
    else
	be32 = !be16;

    if ( be16 != be32 ) {			// strange machine!
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Inconsistent system byte order" );
#endif
	return FALSE;
    }

    *bigEndian = si_bigEndian = be32;
    return TRUE;
}


/*****************************************************************************
  Debug output routines
 *****************************************************************************/

/*!
  \fn void qDebug( const char *msg, ... )

  \relates QApplication
  Prints a debug message, or calls the message handler (if it has been
  installed).

  This function takes a format string and a list of arguments, similar to
  the C printf() function.

  Example:
  \code
    qDebug( "my window handle = %x", myWidget->id() );
  \endcode

  Under X11, the text is printed to stderr.  Under Windows, the text is
  sent to the debugger.

  \warning The internal buffer is limited to 8196 bytes (including the
  0-terminator).

  \sa qWarning(), qFatal(), qInstallMsgHandler(),
  \link debug.html Debugging\endlink
*/

/*!
  \fn void qWarning( const char *msg, ... )

  \relates QApplication
  Prints a warning message, or calls the message handler (if it has been
  installed).

  This function takes a format string and a list of arguments, similar to
  the C printf() function.

  Example:
  \code
    void f( int c )
    {
	if ( c > 200 )
	    qWarning( "f: bad argument, c == %d", c );
    }
  \endcode

  Under X11, the text is printed to stderr.  Under Windows, the text is
  sent to the debugger.

  \warning The internal buffer is limited to 8196 bytes (including the
  0-terminator).

  \sa qDebug(), qFatal(), qInstallMsgHandler(),
  \link debug.html Debugging\endlink
*/

/*!
  \fn void qFatal( const char *msg, ... )

  \relates QApplication
  Prints a fatal error message and exits, or calls the message handler (if it
  has been installed).

  This function takes a format string and a list of arguments, similar to
  the C printf() function.

  Example:
  \code
    int divide( int a, int b )
    {
	if ( b == 0 )				// program error
	    qFatal( "divide: cannot divide by zero" );
	return a/b;
    }
  \endcode

  Under X11, the text is printed to stderr.  Under Windows, the text is
  sent to the debugger.

  \warning The internal buffer is limited to 8196 bytes (including the
  0-terminator).

  \sa qDebug(), qWarning(), qInstallMsgHandler(),
  \link debug.html Debugging\endlink
*/


static msg_handler handler = 0;			// pointer to debug handler


#ifdef _OS_MAC_

static FILE * mac_debug=0;

void qDebug( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+" );
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    } else {
	exit(0);
    }
}

// copied... this looks really bad.
void debug( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+" );
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    }
}

void qWarning( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+" );
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    }
}

// copied... this looks really bad.
void warning( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+" );
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    }
}

void qFatal( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+");
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    }
    exit(0);
}

// copied... this looks really bad.
void fatal( const char *msg, ... )
{
    mac_debug=fopen( "debug.txt", "a+" );
    if(mac_debug) {
	char buf[8196];
	va_list ap;
	va_start( ap, msg );			// use variable arg list
	if ( handler ) {
	    vsprintf( buf, msg, ap );
	    va_end( ap );
	    (*handler)( QtDebugMsg, buf );
	} else {
	    vfprintf( mac_debug, msg, ap );
	    va_end( ap );
	    fprintf( mac_debug, "\n" );		// add newline
	    fflush( mac_debug );
	}
	fclose(mac_debug);
    }
    exit(0);
}

#else

void qDebug( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );		// ### vsnprintf would be great here
	va_end( ap );
	(*handler)( QtDebugMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
    }
}

// copied... this looks really bad.
void debug( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );
	va_end( ap );
	(*handler)( QtDebugMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
    }
}

void qWarning( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );
	va_end( ap );
	(*handler)( QtWarningMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
    }
}


// again, copied
void warning( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );
	va_end( ap );
	(*handler)( QtWarningMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
    }
}

void qFatal( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );
	va_end( ap );
	(*handler)( QtFatalMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
#if defined(_OS_UNIX_) && defined(DEBUG)
	abort();				// trap; generates core dump
#else
	exit( 1 );				// goodbye cruel world
#endif
    }
}

// yet again, copied
void fatal( const char *msg, ... )
{
    char buf[8196];
    va_list ap;
    va_start( ap, msg );			// use variable arg list
    if ( handler ) {
	vsprintf( buf, msg, ap );
	va_end( ap );
	(*handler)( QtFatalMsg, buf );
    } else {
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n" );		// add newline
#if defined(_OS_UNIX_) && defined(DEBUG)
	abort();				// trap; generates core dump
#else
	exit( 1 );				// goodbye cruel world
#endif
    }
}

#endif


/*!
  \fn void ASSERT( bool test )
  \relates QApplication
  Prints a warning message containing the source code file name and line number
  if \e test is FALSE.

  This is really a macro defined in qglobal.h.

  ASSERT is useful for testing required conditions in your program.

  Example:
  \code
    //
    // File: div.cpp
    //

    #include <qglobal.h>

    int divide( int a, int b )
    {
	ASSERT( b != 0 );			// this is line 9
	return a/b;
    }
  \endcode

  If \c b is zero, the ASSERT statement will output the following message
  using the qWarning() function:
  \code
    ASSERT: "b == 0" in div.cpp (9)
  \endcode

  \sa qWarning(), \link debug.html Debugging\endlink
*/


/*!
  \fn void CHECK_PTR( void *p )
  \relates QApplication
  If \e p is null, a fatal messages says that the program ran out of memory
  and exits.  If \e p is not null, nothing happens.

  This is really a macro defined in qglobal.h.

  Example:
  \code
    int *a;
    CHECK_PTR( a = new int[80] );	// never do this!
      // do this instead:
    a = new int[80];
    CHECK_PTR( a );			// this is fine
  \endcode

  \sa qFatal(), \link debug.html Debugging\endlink
*/


//
// The CHECK_PTR macro calls this function to check if an allocation went ok.
//

bool qt_check_pointer( bool c, const char *n, int l )
{
    if ( c )
	qFatal( "In file %s, line %d: Out of memory", n, l );
    return TRUE;
}


static bool firstObsoleteWarning(const char *obj, const char *oldfunc )
{
    static QAsciiDict<int> *obsoleteDict = 0;
    if ( !obsoleteDict ) {			// first time func is called
	obsoleteDict = new QAsciiDict<int>;
#if defined(DEBUG)
	qDebug(
      "You are using obsolete functions in the Qt library. Call the function\n"
      "qSuppressObsoleteWarnings() to suppress obsolete warnings.\n"
	     );
#endif
    }
    QCString s( obj );
    s += "::";
    s += oldfunc;
    if ( obsoleteDict->find(s.data()) == 0 ) {
	obsoleteDict->insert( s.data(), (int*)1 );	// anything different from 0
	return TRUE;
    }
    return FALSE;
}

static bool suppressObsolete = FALSE;

void qSuppressObsoleteWarnings( bool suppress )
{
    suppressObsolete = suppress;
}

void qObsolete(	 const char *obj, const char *oldfunc, const char *newfunc )
{
    if ( suppressObsolete )
	return;
    if ( !firstObsoleteWarning(obj, oldfunc) )
	return;
    if ( obj )
	qDebug( "%s::%s: This function is obsolete, use %s instead.",
	       obj, oldfunc, newfunc );
    else
	qDebug( "%s: This function is obsolete, use %s instead.",
	       oldfunc, newfunc );
}

void qObsolete(	 const char *obj, const char *oldfunc )
{
    if ( suppressObsolete )
	return;
    if ( !firstObsoleteWarning(obj, oldfunc) )
	return;
    if ( obj )
	qDebug( "%s::%s: This function is obsolete.", obj, oldfunc );
    else
	qDebug( "%s: This function is obsolete.", oldfunc );
}

void qObsolete(	 const char *message )
{
    if ( suppressObsolete )
	return;
    if ( !firstObsoleteWarning( "Qt", message) )
	return;
    qDebug( "%s", message );
}


/*!
  \relates QApplication
  Installs a Qt message handler.  Returns a pointer to the message handler
  previously defined.

  The message handler is a function that prints out debug messages,
  warnings and fatal error messages.  The Qt library (debug version)
  contains hundreds of warning messages that are printed when internal
  errors (usually invalid function arguments) occur.  If you implement
  your own message handler, you get total control of these messages.

  The default message handler prints the message to the standard output
  under X11 or to the debugger under Windows.  If it is a fatal message,
  the application aborts immediately.

  Only one message handler can be defined, since this is usually done on
  an application-wide basis to control debug output.

  To restore the message handler, call \c qInstallMsgHandler(0).

  Example:
  \code
    #include <qapplication.h>
    #include <stdio.h>
    #include <stdlib.h>

    void myMessageOutput( QtMsgType type, const char *msg )
    {
	switch ( type ) {
	    case QtDebugMsg:
		fprintf( stderr, "Debug: %s\n", msg );
		break;
	    case QtWarningMsg:
		fprintf( stderr, "Warning: %s\n", msg );
		break;
	    case QtFatalMsg:
		fprintf( stderr, "Fatal: %s\n", msg );
		abort();			// dump core on purpose
	}
    }

    int main( int argc, char **argv )
    {
	qInstallMsgHandler( myMessageOutput );
	QApplication a( argc, argv );
	...
	return a.exec();
    }
  \endcode

  \sa qDebug(), qWarning(), qFatal(), \link debug.html Debugging\endlink
*/

msg_handler qInstallMsgHandler( msg_handler h )
{
    msg_handler old = handler;
    handler = h;
    return old;
}


#ifdef _WS_WIN_
bool qt_winunicode=FALSE;
#endif
