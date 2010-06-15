/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 * Based on qfile_unix.cpp 
 *
 * Copyright (C) 1992-2000 Trolltech AS.
 */

#include "qglobal.h"

#include "qfile.h"
#include "qfiledefs_p.h"

#if defined(_OS_MAC_) || defined(_OS_MSDOS_) || defined(_OS_WIN32_) || defined(_OS_OS2_)
# define HAS_TEXT_FILEMODE			// has translate/text filemode
#endif
#if defined(O_NONBLOCK)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NONBLOCK
#elif defined(O_NDELAY)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NDELAY
#endif

static void reslashify( QString& n )
{
  for ( int i=0; i<(int)n.length(); i++ ) 
  {
     if ( n[i] == '/' )
          n[i] = '\\';
  }
}

bool qt_file_access( const QString& fn, int t )
{
    if ( fn.isEmpty() )
	return FALSE;
#if defined(__CYGWIN32_)
    return ACCESS( QFile::encodeName(fn), t ) == 0;
#else
    QString str = fn;
    reslashify(str);
    return ( _waccess( (wchar_t*) str.ucs2(), t ) == 0 );
#endif
}

/*!
  Removes the file \a fileName.
  Returns TRUE if successful, otherwise FALSE.
*/

bool QFile::remove( const QString &fileName )
{
    if ( fileName.isEmpty() ) {
#if defined(CHECK_NULL)
	qWarning( "QFile::remove: Empty or null file name" );
#endif
	return FALSE;
    }
#if defined(__CYGWIN32_)
    // unlink more common in UNIX
    return ::remove( QFile::encodeName(fileName) ) == 0;	
#else
    QString str = fileName;
    reslashify(str);
    return ( _wunlink( (wchar_t*) str.ucs2() ) == 0 );
#endif
}

#if defined(O_NONBLOCK)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NONBLOCK
#elif defined(O_NDELAY)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NDELAY
#endif

/*!
  Opens the file specified by the file name currently set, using the mode \e m.
  Returns TRUE if successful, otherwise FALSE.

  The mode parameter \e m must be a combination of the following flags:
  <ul>
  <li>\c IO_Raw specified raw (non-buffered) file access.
  <li>\c IO_ReadOnly opens the file in read-only mode.
  <li>\c IO_WriteOnly opens the file in write-only mode (and truncates).
  <li>\c IO_ReadWrite opens the file in read/write mode, equivalent to
  \c (IO_ReadOnly|IO_WriteOnly).
  <li>\c IO_Append opens the file in append mode. This mode is very useful
  when you want to write something to a log file. The file index is set to
  the end of the file. Note that the result is undefined if you position the
  file index manually using at() in append mode.
  <li>\c IO_Truncate truncates the file.
  <li>\c IO_Translate enables carriage returns and linefeed translation
  for text files under MS-DOS, Windows and OS/2.
  </ul>

  The raw access mode is best when I/O is block-operated using 4kB block size
  or greater. Buffered access works better when reading small portions of
  data at a time.

  <strong>Important:</strong> When working with buffered files, data may
  not be written to the file at once. Call \link flush() flush\endlink
  to make sure the data is really written.

  \warning We have experienced problems with some C libraries when a buffered
  file is opened for both reading and writing. If a read operation takes place
  immediately after a write operation, the read buffer contains garbage data.
  Worse, the same garbage is written to the file. Calling flush() before
  readBlock() solved this problem.

  If the file does not exist and \c IO_WriteOnly or \c IO_ReadWrite is
  specified, it is created.

  Example:
  \code
    QFile f1( "/tmp/data.bin" );
    QFile f2( "readme.txt" );
    f1.open( IO_Raw | IO_ReadWrite | IO_Append );
    f2.open( IO_ReadOnly | IO_Translate );
  \endcode

  \sa name(), close(), isOpen(), flush()
*/

bool QFile::open( int m )
{
    if ( isOpen() ) {				// file already open
#if defined(CHECK_STATE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    if ( fn.isNull() ) {			// no file name defined
#if defined(CHECK_NULL)
	qWarning( "QFile::open: No file name specified" );
#endif
	return FALSE;
    }
    init();					// reset params
    setMode( m );
    if ( !(isReadable() || isWritable()) ) {
#if defined(CHECK_RANGE)
	qWarning( "QFile::open: File access not specified" );
#endif
	return FALSE;
    }
    bool ok = TRUE;
    STATBUF st;
    if ( isRaw() ) {				// raw file I/O
	int oflags = OPEN_RDONLY;
	if ( isReadable() && isWritable() )
	    oflags = OPEN_RDWR;
	else if ( isWritable() )
	    oflags = OPEN_WRONLY;
	if ( flags() & IO_Append ) {		// append to end of file?
	    if ( flags() & IO_Truncate )
		oflags |= (OPEN_CREAT | OPEN_TRUNC);
	    else
		oflags |= (OPEN_APPEND | OPEN_CREAT);
	    setFlags( flags() | IO_WriteOnly ); // append implies write
	} else if ( isWritable() ) {		// create/trunc if writable
	    if ( flags() & IO_Truncate )
		oflags |= (OPEN_CREAT | OPEN_TRUNC);
	    else
		oflags |= OPEN_CREAT;
	}
#if defined(HAS_TEXT_FILEMODE)
	if ( isTranslated() )
	    oflags |= OPEN_TEXT;
	else
	    oflags |= OPEN_BINARY;
#endif
#if defined(HAS_ASYNC_FILEMODE)
	if ( isAsynchronous() )
	    oflags |= OPEN_ASYNC;
#endif


#if defined(__CYGWIN32_)
	fd = OPEN( QFile::encodeName(fn), oflags, 0666 );
#else
        QString str = fn;
        reslashify(str);
        fd = _wopen( (wchar_t*) str.ucs2(), oflags, 0666 );
#endif

	if ( fd != -1 ) {			// open successful
	    FSTAT( fd, &st ); // get the stat for later usage
	} else {
	    ok = FALSE;
	}
    } else {					// buffered file I/O
	QCString perm;
	char perm2[4];
	bool try_create = FALSE;
	if ( flags() & IO_Append ) {		// append to end of file?
	    setFlags( flags() | IO_WriteOnly ); // append implies write
	    perm = isReadable() ? "a+" : "a";
	} else {
	    if ( isReadWrite() ) {
		if ( flags() & IO_Truncate ) {
		    perm = "w+";
		} else {
		    perm = "r+";
		    try_create = TRUE;		// try to create if not exists
		}
	    } else if ( isReadable() ) {
		perm = "r";
	    } else if ( isWritable() ) {
		perm = "w";
	    }
	}
	qstrcpy( perm2, perm );
	if ( isTranslated() )
	    strcat( perm2, "t" );
	else
	    strcat( perm2, "b" );
	while (1) { // At most twice

#if defined(__CYGWIN32_)
	    fh = fopen( QFile::encodeName(fn), perm2 );
#else
            QString str = fn;
            QString prm( perm2 );
            reslashify(str);
            fh = _wfopen( (wchar_t*) str.ucs2(), (wchar_t*) prm.ucs2() );
#endif

	    if ( !fh && try_create ) {
		perm2[0] = 'w';			// try "w+" instead of "r+"
		try_create = FALSE;
	    } else {
		break;
	    }
	}
	if ( fh ) {
	    FSTAT( FILENO(fh), &st ); // get the stat for later usage
	} else {
	    ok = FALSE;
	}
    }
    if ( ok ) {
	setState( IO_Open );
	// on successful open the file stat was got; now test what type
	// of file we have
	if ( (st.st_mode & STAT_MASK) != STAT_REG ) {
	    // non-seekable
	    setType( IO_Sequential );
	    length = INT_MAX;
	    ioIndex  = (flags() & IO_Append) == 0 ? 0 : length;
	} else {
	    length = (int)st.st_size;
	    ioIndex  = (flags() & IO_Append) == 0 ? 0 : length;
	    if ( (flags() & !IO_Truncate) && length == 0 && isReadable() ) {
		// try if you can read from it (if you can, it's a sequential
		// device; e.g. a file in the /proc filesystem)
		int c = getch();
		if ( c != -1 ) {
		    ungetch(c);
		    setType( IO_Sequential );
		    length = INT_MAX;
		}
	    }
	}
    } else {
	init();
	if ( errno == EMFILE )			// no more file handles/descrs
	    setStatus( IO_ResourceError );
	else
	    setStatus( IO_OpenError );
    }
    return ok;
}

/*!
  Opens a file in the mode \e m using an existing file handle \e f.
  Returns TRUE if successful, otherwise FALSE.

  Example:
  \code
    #include <stdio.h>

    void printError( const char* msg )
    {
	QFile f;
	f.open( IO_WriteOnly, stderr );
	f.writeBlock( msg, qstrlen(msg) );	// write to stderr
	f.close();
    }
  \endcode

  When a QFile is opened using this function, close() does not actually
  close the file, only flushes it.

  \warning If \e f is \c stdin, \c stdout, \c stderr, you may not
  be able to seek.  See QIODevice::isSequentialAccess() for more
  information.

  \sa close()
*/

bool QFile::open( int m, FILE *f )
{
    if ( isOpen() ) {
#if defined(CHECK_RANGE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    init();
    setMode( m &~IO_Raw );
    setState( IO_Open );
    fh = f;
    ext_f = TRUE;
    STATBUF st;
    FSTAT( FILENO(fh), &st );
    ioIndex = (int)ftell( fh );
    if ( (st.st_mode & STAT_MASK) != STAT_REG ) {
	// non-seekable
	setType( IO_Sequential );
	length = INT_MAX;
    } else {
	length = (int)st.st_size;
	if ( (flags() & !IO_Truncate) && length == 0 && isReadable() ) {
	    // try if you can read from it (if you can, it's a sequential
	    // device; e.g. a file in the /proc filesystem)
	    int c = getch();
	    if ( c != -1 ) {
		ungetch(c);
		setType( IO_Sequential );
		length = INT_MAX;
	    }
	}
    }
    return TRUE;
}

/*!
  Opens a file in the mode \e m using an existing file descriptor \e f.
  Returns TRUE if successful, otherwise FALSE.

  When a QFile is opened using this function, close() does not actually
  close the file.

  \warning If \e f is one of 0 (stdin), 1 (stdout) or 2 (stderr), you may not
  be able to seek. size() is set to \c INT_MAX (in limits.h).

  \sa close()
*/


bool QFile::open( int m, int f )
{
    if ( isOpen() ) {
#if defined(CHECK_RANGE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    init();
    setMode( m |IO_Raw );
    setState( IO_Open );
    fd = f;
    ext_f = TRUE;
    STATBUF st;
    FSTAT( fd, &st );
    ioIndex  = (int)LSEEK(fd, 0, SEEK_CUR);
    if ( (st.st_mode & STAT_MASK) != STAT_REG ) {
	// non-seekable
	setType( IO_Sequential );
	length = INT_MAX;
    } else {
	length = (int)st.st_size;
	if ( length == 0 && isReadable() ) {
	    // try if you can read from it (if you can, it's a sequential
	    // device; e.g. a file in the /proc filesystem)
	    int c = getch();
	    if ( c != -1 ) {
		ungetch(c);
		setType( IO_Sequential );
		length = INT_MAX;
	    }
	}
    }
    return TRUE;
}

/*!
  Returns the file size.
  \sa at()
*/

uint QFile::size() const
{
    STATBUF st;
    if ( isOpen() ) {
	FSTAT( fh ? FILENO(fh) : fd, &st );
        return st.st_size;
    } else {
#if defined(__CYGWIN32_)
	STAT( QFile::encodeName(fn), &st );
#else
        QString str = fn;
        reslashify(str);
#ifdef QT_LARGEFILE_SUPPORT
        if ( _wstati64( (wchar_t*) str.ucs2(), &st ) != -1 ) {
#else
        if ( _wstat( (wchar_t*) str.ucs2(), &st ) != -1 ) {
#endif
#endif
            return st.st_size;
        }
    }
    return 0;
}

/*!
  \fn int QFile::at() const
  Returns the file index.
  \sa size()
*/

/*!
  Sets the file index to \e pos. Returns TRUE if successful, otherwise FALSE.

  Example:
  \code
    QFile f( "data.bin" );
    f.open( IO_ReadOnly );			// index set to 0
    f.at( 100 );				// set index to 100
    f.at( f.at()+50 );				// set index to 150
    f.at( f.size()-80 );			// set index to 80 before EOF
    f.close();
  \endcode

  \warning The result is undefined if the file was \link open() opened\endlink
  using the \c IO_Append specifier.

  \sa size(), open()
*/

bool QFile::at( int pos )
{
    if ( !isOpen() ) {
#if defined(CHECK_STATE)
	qWarning( "QFile::at: File is not open" );
#endif
	return FALSE;
    }
    bool ok;
    if ( isRaw() ) {				// raw file
	pos = (int)LSEEK(fd, pos, SEEK_SET);
	ok = pos != -1;
    } else {					// buffered file
	ok = fseek(fh, pos, SEEK_SET) == 0;
    }
    if ( ok )
	ioIndex = pos;
#if defined(CHECK_RANGE)
    else
	qWarning( "QFile::at: Cannot set file position %d", pos );
#endif
    return ok;
}

/*!
  Reads at most \e len bytes from the file into \e p and returns the
  number of bytes actually read.

  Returns -1 if a serious error occurred.

  \warning We have experienced problems with some C libraries when a buffered
  file is opened for both reading and writing. If a read operation takes place
  immediately after a write operation, the read buffer contains garbage data.
  Worse, the same garbage is written to the file. Calling flush() before
  readBlock() solved this problem.

  \sa writeBlock()
*/

int QFile::readBlock( char *p, uint len )
{
#if defined(CHECK_NULL)
    if ( !p )
	qWarning( "QFile::readBlock: Null pointer error" );
#endif
#if defined(CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	qWarning( "QFile::readBlock: File not open" );
	return -1;
    }
    if ( !isReadable() ) {			// reading not permitted
	qWarning( "QFile::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    int nread;					// number of bytes read
    if ( isRaw() ) {				// raw file
	nread = READ( fd, p, len );
	if ( len && nread <= 0 ) {
	    nread = 0;
	    setStatus(IO_ReadError);
	}
    } else {					// buffered file
	nread = fread( p, 1, len, fh );
	if ( (uint)nread != len ) {
	    if ( ferror( fh ) || nread==0 )
		setStatus(IO_ReadError);
	}
    }
    ioIndex += nread;
    return nread;
}

/*! \overload int writeBlock( const QByteArray& data )
*/

/*! \reimp

  Writes \e len bytes from \e p to the file and returns the number of
  bytes actually written.

  Returns -1 if a serious error occurred.

  \warning When working with buffered files, data may not be written
  to the file at once. Call flush() to make sure the data is really
  written.

  \sa readBlock()
*/

int QFile::writeBlock( const char *p, uint len )
{
#if defined(CHECK_NULL)
    if ( p == 0 && len != 0 )
	qWarning( "QFile::writeBlock: Null pointer error" );
#endif
#if defined(CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	qWarning( "QFile::writeBlock: File not open" );
	return -1;
    }
    if ( !isWritable() ) {			// writing not permitted
	qWarning( "QFile::writeBlock: Write operation not permitted" );
	return -1;
    }
#endif
    int nwritten;				// number of bytes written
    if ( isRaw() )				// raw file
	nwritten = WRITE( fd, p, len );
    else					// buffered file
	nwritten = fwrite( p, 1, len, fh );
    if ( nwritten != (int)len ) {		// write error
	if ( errno == ENOSPC )			// disk is full
	    setStatus( IO_ResourceError );
	else
	    setStatus( IO_WriteError );
	if ( isRaw() )				// recalc file position
	    ioIndex = (int)LSEEK( fd, 0, SEEK_CUR );
	else
	    ioIndex = fseek( fh, 0, SEEK_CUR );
    } else {
	ioIndex += nwritten;
    }
    if ( ioIndex > length )			// update file length
	length = ioIndex;
    return nwritten;
}

/*!
  Returns the file handle of the file.

  This is a small positive integer, suitable for use with C library
  functions such as fdopen() and fcntl(), as well as with QSocketNotifier.

  If the file is not open or there is an error, handle() returns -1.

  \sa QSocketNotifier
*/

int QFile::handle() const
{
    if ( !isOpen() )
	return -1;
    else if ( fh )
	return FILENO( fh );
    else
	return fd;
}

/*!
  Closes an open file.

  The file is not closed if it was opened with an existing file handle.
  If the existing file handle is a \c FILE*, the file is flushed.
  If the existing file handle is an \c int file descriptor, nothing
  is done to the file.

  Some "write-behind" filesystems may report an unspecified error on
  closing the file. These errors only indicate that something may
  have gone wrong since the previous open(). In such a case status()
  reports IO_UnspecifiedError after close(), otherwise IO_Ok.

  \sa open(), flush()
*/


void QFile::close()
{
    bool ok = FALSE;
    if ( isOpen() ) {				// file is not open
	if ( fh ) {				// buffered file
	    if ( ext_f )
		ok = fflush( fh ) != -1;	// flush instead of closing
	    else
		ok = fclose( fh ) != -1;
	} else {				// raw file
	    if ( ext_f )
		ok = TRUE;			// cannot close
	    else
		ok = CLOSE( fd ) != -1;
	}
	init();					// restore internal state
    }
    if (!ok)
	setStatus (IO_UnspecifiedError);

    return;
}

int64 QFile::pos() const
{
  if (isOpen())
  {
    // TODO: support 64 bit size
    return ftell( fh );
  }
  return -1;
}

int64 QFile::toEnd() 
{
  if (isOpen())
  {
     // TODO: support 64 bit size
     if (fseek( fh, 0, SEEK_END )!=-1)
     {
       return ftell( fh );
     }
  }
  return -1;
}

bool QFile::seek( int64 pos )
{
  if (isOpen())
  {
    // TODO: support 64 bit size
    return fseek( fh, pos, SEEK_SET )!=-1;
  }
  return FALSE;
}



