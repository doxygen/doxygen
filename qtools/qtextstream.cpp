/****************************************************************************
** 
**
** Implementation of QTextStream class
**
** Created : 940922
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

#include "qtextstream.h"

#ifndef QT_NO_TEXTSTREAM
#include "qtextcodec.h"
#include "qregexp.h"
#include "qbuffer.h"
#include "qfile.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(_OS_WIN32_)
#include <windows.h>
#endif

// NOT REVISED
/*!
  \class QTextStream qtextstream.h

  \brief The QTextStream class provides basic functions for reading and
  writing text using a QIODevice.

  \ingroup io

  \define endl
  \define bin
  \define oct
  \define dec
  \define hex
  \define flush
  \define ws

  The text stream class has a functional interface that is very
  similar to that of the standard C++ iostream class.  The difference
  between iostream and QTextStream is that our stream operates on a
  QIODevice, which is easily subclassed, while iostream operates on
  FILE * pointers, which can not be subclassed.

  Qt provides several global functions similar to the ones in iostream:
  <ul>
  <li> \c bin sets the QTextStream to read/write binary numbers
  <li> \c oct sets the QTextStream to read/write octal numbers
  <li> \c dec sets the QTextStream to read/write decimal numbers
  <li> \c hex sets the QTextStream to read/write hexadecimal numbers
  <li> \c endl forces a line break
  <li> \c flush forces the QIODevice to flush any buffered data
  <li> \c ws eats any available white space (on input)
  <li> \c reset resets the QTextStream to its default mode (see reset()).
  </ul>

  \warning By default, QTextStream will automatically detect whether
  integers in the stream are in decimal, octal, hexadecimal or binary
  format when reading from the stream. In particular, a leading '0'
  signifies octal, ie. the sequence "0100" will be interpreted as
  64.

  The QTextStream class reads and writes text and it is not
  appropriate for dealing with binary data (but QDataStream is).

  By default output of Unicode text (ie. QString) is done using the
  local 8-bit encoding.  This can be changed using the setEncoding()
  method.  For input, the QTextStream will auto-detect standard
  Unicode "byte order marked" text files, but otherwise the local
  8-bit encoding is used.

  \sa QDataStream
*/

/*
  \class QTSManip qtextstream.h

  \brief The QTSManip class is an internal helper class for the
  QTextStream.

  It is generally a very bad idea to use this class directly in
  application programs.

  \internal

  This class makes it possible to give the QTextStream function objects
  with arguments, like this:
  \code
    QTextStream cout( stdout, IO_WriteOnly );
    cout << setprecision( 8 );		// QTSManip used here!
    cout << 3.14159265358979323846;
  \endcode

  The setprecision() function returns a QTSManip object.
  The QTSManip object contains a pointer to a member function in
  QTextStream and an integer argument.
  When serializing a QTSManip into a QTextStream, the function
  is executed with the argument.
*/

/*! \fn QTSManip::QTSManip (QTSMFI m, int a)

  Constructs a QTSManip object which will call \a m (a member function
  in QTextStream which accepts a single int) with argument \a a when
  QTSManip::exec() is called.  Used internally in e.g. endl:

  \code
    s << "some text" << endl << "more text";
  \endcode
*/

/*! \fn void QTSManip::exec (QTextStream& s)

  Calls the member function specified in the constructor, for object
  \a s.  Used internally in e.g. endl:

  \code
    s << "some text" << endl << "more text";
  \endcode
*/


/*****************************************************************************
  QTextStream member functions
 *****************************************************************************/

#if defined(CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				qWarning( "QTextStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif


#define I_SHORT		0x0010
#define I_INT		0x0020
#define I_LONG		0x0030
#define I_TYPE_MASK	0x00f0

#define I_BASE_2	QTS::bin
#define I_BASE_8	QTS::oct
#define I_BASE_10	QTS::dec
#define I_BASE_16	QTS::hex
#define I_BASE_MASK	(QTS::bin | QTS::oct | QTS::dec | QTS::hex)

#define I_SIGNED	0x0100
#define I_UNSIGNED	0x0200
#define I_SIGN_MASK	0x0f00


static const QChar QEOF = QChar((ushort)0xffff); //guaranteed not to be a character.

const int QTextStream::basefield   = I_BASE_MASK;
const int QTextStream::adjustfield = ( QTextStream::left |
				       QTextStream::right |
				       QTextStream::internal );
const int QTextStream::floatfield  = ( QTextStream::scientific |
				       QTextStream::fixed );


class QTextStreamPrivate {
public:
#ifndef QT_NO_TEXTCODEC
    QTextStreamPrivate() : decoder( 0 ), sourceType( NotSet ) {}
    ~QTextStreamPrivate() { delete decoder; }
    QTextDecoder *decoder;		//???
#else
    QTextStreamPrivate() : sourceType( NotSet ) {}
    ~QTextStreamPrivate() { }
#endif
    QString ungetcBuf;

    enum SourceType { NotSet, IODevice, String, ByteArray, File };
    SourceType sourceType;
};


// skips whitespace and returns the first non-whitespace character
QChar QTextStream::eat_ws()
{
    QChar c;
    do { c = ts_getc(); } while ( c != QEOF && ts_isspace(c) );
    return c;
}

void QTextStream::init()
{
    // ### ungetcBuf = QEOF;
    dev = 0;					// no device set
    fstrm = owndev = FALSE;
    mapper = 0;
    d = new QTextStreamPrivate;
    doUnicodeHeader = TRUE;	 //default to autodetect
    latin1 = TRUE;		 // ### should use local?
    internalOrder = QChar::networkOrdered(); //default to network order
}

/*!
  Constructs a data stream that has no IO device.
*/

QTextStream::QTextStream()
{
    init();
    setEncoding( Locale ); //###
    reset();
    d->sourceType = QTextStreamPrivate::NotSet;
}

/*!
  Constructs a text stream that uses the IO device \a iod.
*/

QTextStream::QTextStream( QIODevice *iod )
{
    init();
    setEncoding( Locale ); //###
    dev = iod;					// set device
    reset();
    d->sourceType = QTextStreamPrivate::IODevice;
}

// TODO: use special-case handling of this case in QTextStream, and
//	 simplify this class to only deal with QChar or QString data.
class QStringBuffer : public QIODevice {
public:
    QStringBuffer( QString* str );
    ~QStringBuffer();
    bool  open( int m );
    void  close();
    void  flush();
    uint  size() const;
    int   at()   const;
    bool  at( int pos );
    int   readBlock( char *p, uint len );
    int writeBlock( const char *p, uint len );
    int   getch();
    int   putch( int ch );
    int   ungetch( int ch );
protected:
    QString* s;

private:        // Disabled copy constructor and operator=
    QStringBuffer( const QStringBuffer & );
    QStringBuffer &operator=( const QStringBuffer & );
};


QStringBuffer::QStringBuffer( QString* str )
{
    s = str;
}

QStringBuffer::~QStringBuffer()
{
}


bool QStringBuffer::open( int m )
{
    if ( !s ) {
#if defined(CHECK_STATE)
	qWarning( "QStringBuffer::open: No string" );
#endif
	return FALSE;
    }
    if ( isOpen() ) {                           // buffer already open
#if defined(CHECK_STATE)
	qWarning( "QStringBuffer::open: Buffer already open" );
#endif
	return FALSE;
    }
    setMode( m );
    if ( m & IO_Truncate ) {                    // truncate buffer
	s->truncate( 0 );
    }
    if ( m & IO_Append ) {                      // append to end of buffer
	ioIndex = s->length()*sizeof(QChar);
    } else {
	ioIndex = 0;
    }
    setState( IO_Open );
    setStatus( 0 );
    return TRUE;
}

void QStringBuffer::close()
{
    if ( isOpen() ) {
	setFlags( IO_Direct );
	ioIndex = 0;
    }
}

void QStringBuffer::flush()
{
}

uint QStringBuffer::size() const
{
    return s ? s->length()*sizeof(QChar) : 0;
}

int  QStringBuffer::at()   const
{
    return ioIndex;
}

bool QStringBuffer::at( int pos )
{
#if defined(CHECK_STATE)
    if ( !isOpen() ) {
	qWarning( "QStringBuffer::at: Buffer is not open" );
	return FALSE;
    }
#endif
    if ( (uint)pos >= s->length()*2 ) {
#if defined(CHECK_RANGE)
	qWarning( "QStringBuffer::at: Index %d out of range", pos );
#endif
	return FALSE;
    }
    ioIndex = pos;
    return TRUE;
}


int  QStringBuffer::readBlock( char *p, uint len )
{
#if defined(CHECK_STATE)
    CHECK_PTR( p );
    if ( !isOpen() ) {                          // buffer not open
	qWarning( "QStringBuffer::readBlock: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
	qWarning( "QStringBuffer::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    if ( (uint)ioIndex + len > s->length()*sizeof(QChar) ) {
	// overflow
	if ( (uint)ioIndex >= s->length()*sizeof(QChar) ) {
	    setStatus( IO_ReadError );
	    return -1;
	} else {
	    len = s->length()*2 - (uint)ioIndex;
	}
    }
    memcpy( p, ((const char*)(s->unicode()))+ioIndex, len );
    ioIndex += len;
    return len;
}

int QStringBuffer::writeBlock( const char *p, uint len )
{
#if defined(CHECK_NULL)
    if ( p == 0 && len != 0 )
	qWarning( "QStringBuffer::writeBlock: Null pointer error" );
#endif
#if defined(CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
	qWarning( "QStringBuffer::writeBlock: Buffer not open" );
	return -1;
    }
    if ( !isWritable() ) {                      // writing not permitted
	qWarning( "QStringBuffer::writeBlock: Write operation not permitted" );
	return -1;
    }
    if ( ioIndex&1 ) {
	qWarning( "QStringBuffer::writeBlock: non-even index - non Unicode" );
	return -1;
    }
    if ( len&1 ) {
	qWarning( "QStringBuffer::writeBlock: non-even length - non Unicode" );
	return -1;
    }
#endif
    s->replace(ioIndex/2, len/2, (QChar*)p, len/2);
    ioIndex += len;
    return len;
}

int QStringBuffer::getch()
{
#if defined(CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
	qWarning( "QStringBuffer::getch: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
	qWarning( "QStringBuffer::getch: Read operation not permitted" );
	return -1;
    }
#endif
    if ( (uint)ioIndex >= s->length()*2 ) {           // overflow
	setStatus( IO_ReadError );
	return -1;
    }
    return *((char*)s->unicode() + ioIndex++);
}

int QStringBuffer::putch( int ch )
{
    char c = ch;
    if ( writeBlock(&c,1) < 0 )
	return -1;
    else
	return ch;
}

int QStringBuffer::ungetch( int ch )
{
#if defined(CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
	qWarning( "QStringBuffer::ungetch: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
	qWarning( "QStringBuffer::ungetch: Read operation not permitted" );
	return -1;
    }
#endif
    if ( ch != -1 ) { // something to do with eof
	if ( ioIndex )
	    ioIndex--;
	else
	    ch = -1;
    }
    return ch;
}


/*!
  Constructs a text stream that operates on a Unicode QString through an
  internal device.

  If you set an encoding or codec with setEncoding() or setCodec(), this
  setting is ignored for text streams that operate on QString.

  Example:
  \code
    QString str;
    QTextStream ts( &str, IO_WriteOnly );
    ts << "pi = " << 3.14;			// str == "pi = 3.14"
  \endcode

  Writing data to the text stream will modify the contents of the string.
  The string will be expanded when data is written beyond the end of the
  string. Note that the string will not be truncated:
  \code
    QString str = "pi = 3.14";
    QTextStream ts( &str, IO_WriteOnly );
    ts <<  "2+2 = " << 2+2; 		// str == "2+2 = 414"
  \endcode

  Note that since QString is Unicode, you should not use readRawBytes()
  or writeRawBytes() on such a stream.
*/

QTextStream::QTextStream( QString* str, int filemode )
{
    // TODO: optimize for this case as it becomes more common
    //        (see QStringBuffer above)
    init();
    dev = new QStringBuffer( str );
    ((QStringBuffer *)dev)->open( filemode );
    owndev = TRUE;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = QTextStreamPrivate::String;
}

/*! \obsolete

  This constructor is equivalent to the constructor taking a QString*
  parameter.
*/

QTextStream::QTextStream( QString& str, int filemode )
{
    init();
    dev = new QStringBuffer( &str );
    ((QStringBuffer *)dev)->open( filemode );
    owndev = TRUE;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = QTextStreamPrivate::String;
}

/*!
  Constructs a text stream that operates on a byte array through an
  internal QBuffer device.

  Example:
  \code
    QByteArray array;
    QTextStream ts( array, IO_WriteOnly );
    ts << "pi = " << 3.14 << '\0';		// array == "pi = 3.14"
  \endcode

  Writing data to the text stream will modify the contents of the array.
  The array will be expanded when data is written beyond the end of the
  string.

  Same example, using a QBuffer:
  \code
    QByteArray array;
    QBuffer buf( array );
    buf.open( IO_WriteOnly );
    QTextStream ts( &buf );
    ts << "pi = " << 3.14 << '\0';		// array == "pi = 3.14"
    buf.close();
  \endcode
*/

QTextStream::QTextStream( QByteArray a, int mode )
{
    init();
    dev = new QBuffer( a );
    ((QBuffer *)dev)->open( mode );
    owndev = TRUE;
    setEncoding( Locale ); //### Locale???
    reset();
    d->sourceType = QTextStreamPrivate::ByteArray;
}

/*!
  Constructs a text stream that operates on an existing file handle \e fh
  through an internal QFile device.

  Example:
  \code
    QTextStream cout( stdout, IO_WriteOnly );
    QTextStream cin ( stdin,  IO_ReadOnly );
    QTextStream cerr( stderr, IO_WriteOnly );
 \endcode
*/

QTextStream::QTextStream( FILE *fh, int mode )
{
    init();
    setEncoding( Locale ); //###
    dev = new QFile;
    ((QFile *)dev)->open( mode, fh );
    fstrm = owndev = TRUE;
    reset();
    d->sourceType = QTextStreamPrivate::File;
}

/*!
  Destructs the text stream.

  The destructor does not affect the current IO device.
*/

QTextStream::~QTextStream()
{
    if ( owndev )
	delete dev;
    delete d;
}

/*!
  Positions the read pointer at the first non-whitespace character.
*/
void QTextStream::skipWhiteSpace()
{
    ts_ungetc( eat_ws() );
}


/*!
  \fn Encoding QTextStream::encoding() const

  Returns the encoding mode of the stream.

  \sa setEncoding()
*/

/*!
  Tries to read len characters from the stream and stores them in \a buf.
  Returns the number of characters really read.
  Attention: There will no QEOF appended if the read reaches the end of
  the file. EOF is reached when the return value does not equal \a len.
*/
uint QTextStream::ts_getbuf( QChar* buf, uint len )
{
    if( len<1 )
	return 0;

    uint rnum=0;   // the number of QChars really read

    if ( d && d->ungetcBuf.length() ) {
	while( rnum < len && rnum < d->ungetcBuf.length() ) {
	    buf[rnum] = d->ungetcBuf.constref(rnum);
	    rnum++;
	}
	d->ungetcBuf = d->ungetcBuf.mid( rnum );
	if ( rnum >= len )
	    return rnum;
    }

    // we use dev->ungetch() for one of the bytes of the unicode
    // byte-order mark, but a local unget hack for the other byte:
    int ungetHack = EOF;

    if ( doUnicodeHeader ) {
	doUnicodeHeader = FALSE; //only at the top
	int c1 = dev->getch();
	if ( c1 == EOF )
	    return rnum;
	int c2 = dev->getch();
	if ( c1 == 0xfe && c2 == 0xff ) {
	    mapper = 0;
	    latin1 = FALSE;
	    internalOrder = QChar::networkOrdered();   //network order
	} else if ( c1 == 0xff && c2 == 0xfe ) {
	    mapper = 0;
	    latin1 = FALSE;
	    internalOrder = !QChar::networkOrdered();   //reverse network order
	} else {
	    if ( c2 != EOF ) {
	 	dev->ungetch( c2 );
		ungetHack = c1;
	    } else {
	 	dev->ungetch( c1 );
		// note that a small possible bug might hide here
		// here, if only the first byte of a file has made it
		// so far, and that first byte is half of the
		// byte-order mark, then the utfness will not be
		// detected.  whether or not this is a bug depends on
		// taste.  I can't really decide.
	    }
	}
    }

#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	bool shortRead = FALSE;
	if ( !d->decoder )
	    d->decoder = mapper->makeDecoder();
	while( rnum < len ) {
	    QString s;
	    bool readBlock = !( len == 1+rnum );
	    while ( TRUE ) {
		// for efficiency: normally read a whole block
		if ( readBlock ) {
		    // guess buffersize; this may be wrong (too small or too
		    // big). But we can handle this (either iterate reading
		    // or use ungetcBuf).
		    // Note that this might cause problems for codecs where
		    // one byte can result in >1 Unicode Characters if bytes
		    // are written to the stream in the meantime (loss of
		    // synchronicity).
		    uint rlen = len - rnum;
		    char *cbuf = new char[ rlen ];
		    if ( ungetHack != EOF ) {
			rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
			cbuf[0] = (char)ungetHack;
			ungetHack = EOF;
		    } else {
			rlen = dev->readBlock( cbuf, rlen );
		    }
		    s  += d->decoder->toUnicode( cbuf, rlen );
		    delete[] cbuf;
		    // use buffered reading only for the first time, because we
		    // have to get the stream synchronous again (this is easier
		    // with single character reading)
		    readBlock = FALSE;
		}
		// get stream (and codec) in sync
		int c;
		if ( ungetHack == EOF ) {
		    c = dev->getch();
		} else {
		    c = ungetHack;
		    ungetHack = EOF;
		}
		if ( c == EOF ) {
		    shortRead = TRUE;
		    break;
		}
		char b = c;
		uint lengthBefore = s.length();
		s  += d->decoder->toUnicode( &b, 1 );
		if ( s.length() > lengthBefore )
		    break; // it seems we are in sync now
	    }
	    uint i = 0;
	    while( rnum < len && i < s.length() )
		buf[rnum++] = s.constref(i++);
	    if ( s.length() > i )
		// could be = but append is clearer
		d->ungetcBuf.append( s.mid( i ) );
	    if ( shortRead )
		return rnum;
	}
    } else
#endif
    if ( latin1 ) {
	if ( len == 1+rnum ) {
	    // use this method for one character because it is more efficient
	    // (arnt doubts whether it makes a difference, but lets it stand)
	    int c = (ungetHack == EOF) ? dev->getch() : ungetHack;
	    if ( c != EOF )
		buf[rnum++] = (char)c;
	} else {
	    if ( ungetHack != EOF ) {
		buf[rnum++] = (char)ungetHack;
		ungetHack = EOF;
	    }
	    char *cbuf = new char[len - rnum];
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = len - rnum;
		rlen = dev->readBlock( cbuf, rlen );
		uint i = 0;
		while( i < rlen )
		    buf[rnum++] = cbuf[i++];
	    }
	    delete[] cbuf;
	}
    } else { // UCS-2 or UTF-16
	if ( len == 1+rnum ) {
	    int c1 = (ungetHack == EOF) ? dev->getch() : ungetHack;
	    if ( c1 == EOF )
		return rnum;
	    int c2 = dev->getch();
	    if ( c2 == EOF )
		return rnum;
	    if ( isNetworkOrder() )
		buf[rnum++] = QChar( c2, c1 );
	    else
		buf[rnum++] = QChar( c1, c2 );
	} else {
	    char *cbuf = new char[ 2*( len - rnum ) ]; // for paranoids: overflow possible
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = 2 * ( len-rnum );
		if ( ungetHack != EOF ) {
		    rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
		    cbuf[0] = (char)ungetHack;
		    ungetHack = EOF;
		} else {
		    rlen = dev->readBlock( cbuf, rlen );
		}
		// We can't use an odd number of bytes, so put it back. But
		// do it only if we are capable of reading more -- normally
		// there should not be an odd number, but the file might be
		// truncated or not in UTF-16...
		if ( (rlen & 1) == 1 )
		    if ( !dev->atEnd() )
			dev->ungetch( cbuf[--rlen] );
		uint i = 0;
		if ( isNetworkOrder() ) {
		    while( i < rlen ) {
			buf[rnum++] = QChar( cbuf[i+1], cbuf[i] );
			i+=2;
		    }
		} else {
		    while( i < rlen ) {
			buf[rnum++] = QChar( cbuf[i], cbuf[i+1] );
			i+=2;
		    }
		}
	    }
	    delete[] cbuf;
	}
    }
    return rnum;
}


/*!
  Puts one character to the stream.
*/
void QTextStream::ts_putc( QChar c )
{
#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	int len = 1;
	QString s = c;
	QCString block = mapper->fromUnicode( s, len );
	dev->writeBlock( block, len );
    } else
#endif
    if ( latin1 ) {
	if( c.row() )
	    dev->putch( '?' ); //######unknown character???
	else
	    dev->putch( c.cell() );
    } else {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = FALSE;
	    ts_putc( QChar::byteOrderMark );
	}
	if ( internalOrder ) {
	    dev->writeBlock( (char*)&c, sizeof(QChar) );
	} else if ( isNetworkOrder() ) {
	    dev->putch(c.row());
	    dev->putch(c.cell());
	} else {
	    dev->putch(c.cell());
	    dev->putch(c.row());
	}
    }
}

/*!
  Puts one character to the stream.
*/
void QTextStream::ts_putc(int ch)
{
    ts_putc(QChar((ushort)ch));
}

bool QTextStream::ts_isdigit(QChar c)
{
    return c.isDigit();
}

bool QTextStream::ts_isspace( QChar c )
{
    return c.isSpace();
}

void QTextStream::ts_ungetc( QChar c )
{
    if ( c.unicode() == 0xffff )
	return;

    d->ungetcBuf.prepend( c );
}



/*!
  Reads \e len bytes from the stream into \e e s and returns a reference to
  the stream.

  The buffer \e s must be preallocated.

  Note that no encoding is done by this function.

  \warning The behaviour of this function is undefined unless the
  stream's encoding is set to Unicode or Latin1.

  \sa QIODevice::readBlock()
*/

QTextStream &QTextStream::readRawBytes( char *s, uint len )
{
    dev->readBlock( s, len );
    return *this;
}

/*!
  Writes the \e len bytes from \e s to the stream and returns a reference to
  the stream.

  Note that no encoding is done by this function.

  \sa QIODevice::writeBlock()
*/

QTextStream &QTextStream::writeRawBytes( const char* s, uint len )
{
    dev->writeBlock( s, len );
    return *this;
}


QTextStream &QTextStream::writeBlock( const char* p, uint len )
{
    if ( doUnicodeHeader ) {
	doUnicodeHeader = FALSE;
	if ( !mapper && !latin1 )
	    ts_putc( QChar::byteOrderMark );
    }
    //All QCStrings and const char* are defined to be in Latin1
    if ( !mapper && latin1 ) {
	dev->writeBlock( p, len );
    } else if ( !mapper && internalOrder ) {
	QChar *u = new QChar[len];
	for (uint i=0; i<len; i++)
	    u[i] = p[i];
	dev->writeBlock( (char*)u, len*sizeof(QChar) );
	delete [] u;
    } else {
	for (uint i=0; i<len; i++)
	    ts_putc( (uchar)p[i] );
    }
    return *this;
}

QTextStream &QTextStream::writeBlock( const QChar* p, uint len )
{
    if ( !mapper && !latin1 && internalOrder ) {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = FALSE;
	    ts_putc( QChar::byteOrderMark );
	}
	dev->writeBlock( (char*)p, sizeof(QChar)*len );
    } else {
	for (uint i=0; i<len; i++)
	    ts_putc( p[i] );
    }
    return *this;
}



/*!
  Resets the text stream.

  <ul>
  <li> All flags are set to 0.
  <li> The field width is set to 0.
  <li> The fill character is set to ' ' (space).
  <li> The precision is set to 6.
  </ul>

  \sa setf(), width(), fill(), precision()
*/

void QTextStream::reset()
{
    fflags = 0;
    fwidth = 0;
    fillchar = ' ';
    fprec = 6;
}


/*!
  \fn QIODevice *QTextStream::device() const
  Returns the IO device currently set.
  \sa setDevice(), unsetDevice()
*/

/*!
  Sets the IO device to \a iod.
  \sa device(), unsetDevice()
*/

void QTextStream::setDevice( QIODevice *iod )
{
    if ( owndev ) {
	delete dev;
	owndev = FALSE;
    }
    dev = iod;
    d->sourceType = QTextStreamPrivate::IODevice;
}

/*!
  Unsets the IO device.	 Equivalent to setDevice( 0 ).
  \sa device(), setDevice()
*/

void QTextStream::unsetDevice()
{
    setDevice( 0 );
    d->sourceType = QTextStreamPrivate::NotSet;
}

/*!
  \fn bool QTextStream::atEnd() const
  Returns TRUE if the IO device has reached the end position (end of
  stream or file) or if there is no IO device set.

  Returns FALSE if the current position of the read/write head of the IO
  device is somewhere before the end position.

  \sa QIODevice::atEnd()
*/

/*!\fn bool QTextStream::eof() const

  \obsolete

  This function has been renamed to atEnd().

  \sa QIODevice::atEnd()
*/

/*****************************************************************************
  QTextStream read functions
 *****************************************************************************/


/*!
  Reads a \c char from the stream and returns a reference to the stream.
  Note that whitespace is skipped.
*/

QTextStream &QTextStream::operator>>( char &c )
{
    CHECK_STREAM_PRECOND
    c = eat_ws();
    return *this;
}

/*!
  Reads a \c char from the stream and returns a reference to the stream.
  Note that whitespace is \e not skipped.
*/

QTextStream &QTextStream::operator>>( QChar &c )
{
    CHECK_STREAM_PRECOND
    c = ts_getc();
    return *this;
}


ulong QTextStream::input_bin()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while (  dv == 0 || dv == 1 ) {
	val = ( val << 1 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong QTextStream::input_oct()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( dv >= 0 && dv <= 7 ) {
	val = ( val << 3 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( dv == 8 || dv == 9 ) {
	while ( ts_isdigit(ch) )
	    ch = ts_getc();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong QTextStream::input_dec()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( ts_isdigit(ch) ) {
	val = val * 10 + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong QTextStream::input_hex()
{
    ulong val = 0;
    QChar ch = eat_ws();
    char c = ch;
    while ( isxdigit(c) ) {
	val <<= 4;
	if ( ts_isdigit(c) )
	    val += c - '0';
	else
	    val += 10 + tolower(c) - 'a';
	c = ch = ts_getc();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

long QTextStream::input_int()
{
    long val;
    QChar ch;
    char c;
    switch ( flags() & basefield ) {
    case bin:
	val = (long)input_bin();
	break;
    case oct:
	val = (long)input_oct();
	break;
    case dec:
	c = ch = eat_ws();
	if ( ch == QEOF ) {
	    val = 0;
	} else {
	    if ( !(c == '-' || c == '+') )
		ts_ungetc( ch );
	    if ( c == '-' ) {
		ulong v = input_dec();
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)input_dec();
	    }
	}
	break;
    case hex:
	val = (long)input_hex();
	break;
    default:
	val = 0;
	c = ch = eat_ws();
	if ( c == '0' ) {		// bin, oct or hex
	    c = ch = ts_getc();
	    if ( tolower(c) == 'x' )
		val = (long)input_hex();
	    else if ( tolower(c) == 'b' )
		val = (long)input_bin();
	    else {			// octal
		ts_ungetc( ch );
		if ( c >= '0' && c <= '7' ) {
		    val = (long)input_oct();
		} else {
		    val = 0;
		}
	    }
	} else if ( ts_isdigit(ch) ) {
	    ts_ungetc( ch );
	    val = (long)input_dec();
	} else if ( c == '-' || c == '+' ) {
	    ulong v = input_dec();
	    if ( c == '-' ) {
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)v;
	    }
	}
    }
    return val;
}

//
// We use a table-driven FSM to parse floating point numbers
// strtod() cannot be used directly since we're reading from a QIODevice
//

double QTextStream::input_double()
{
    const int Init	 = 0;			// states
    const int Sign	 = 1;
    const int Mantissa	 = 2;
    const int Dot	 = 3;
    const int Abscissa	 = 4;
    const int ExpMark	 = 5;
    const int ExpSign	 = 6;
    const int Exponent	 = 7;
    const int Done	 = 8;

    const int InputSign	 = 1;			// input tokens
    const int InputDigit = 2;
    const int InputDot	 = 3;
    const int InputExp	 = 4;

    static uchar table[8][5] = {
     /* None	 InputSign   InputDigit InputDot InputExp */
	{ 0,	    Sign,     Mantissa,	 Dot,	   0,	   }, // Init
	{ 0,	    0,	      Mantissa,	 Dot,	   0,	   }, // Sign
	{ Done,	    Done,     Mantissa,	 Dot,	   ExpMark,}, // Mantissa
	{ 0,	    0,	      Abscissa,	 0,	   0,	   }, // Dot
	{ Done,	    Done,     Abscissa,	 Done,	   ExpMark,}, // Abscissa
	{ 0,	    ExpSign,  Exponent,	 0,	   0,	   }, // ExpMark
	{ 0,	    0,	      Exponent,	 0,	   0,	   }, // ExpSign
	{ Done,	    Done,     Exponent,	 Done,	   Done	   }  // Exponent
    };

    int state = Init;				// parse state
    int input;					// input token

    char buf[256];
    int i = 0;
    QChar c = eat_ws();

    while ( TRUE ) {

	switch ( c ) {
	    case '+':
	    case '-':
		input = InputSign;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		input = InputDigit;
		break;
	    case '.':
		input = InputDot;
		break;
	    case 'e':
	    case 'E':
		input = InputExp;
		break;
	    default:
		input = 0;
		break;
	}

	state = table[state][input];

	if  ( state == 0 || state == Done || i > 250 ) {
	    if ( i > 250 ) {			// ignore rest of digits
		do { c = ts_getc(); } while ( c != QEOF && ts_isdigit(c) );
	    }
	    if ( c != QEOF )
		ts_ungetc( c );
	    buf[i] = '\0';
	    char *end;
	    return strtod( buf, &end );
	}

	buf[i++] = c;
	c = ts_getc();
    }

#if !defined(_CC_EGG_)
    return 0.0;
#endif
}


/*!
  Reads a signed \c short integer from the stream and returns a reference to
  the stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( signed short &i )
{
    CHECK_STREAM_PRECOND
    i = (signed short)input_int();
    return *this;
}


/*!
  Reads an unsigned \c short integer from the stream and returns a reference to
  the stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( unsigned short &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned short)input_int();
    return *this;
}


/*!
  Reads a signed \c int from the stream and returns a reference to the
  stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( signed int &i )
{
    CHECK_STREAM_PRECOND
    i = (signed int)input_int();
    return *this;
}


/*!
  Reads an unsigned \c int from the stream and returns a reference to the
  stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( unsigned int &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned int)input_int();
    return *this;
}


/*!
  Reads a signed \c long int from the stream and returns a reference to the
  stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( signed long &i )
{
    CHECK_STREAM_PRECOND
    i = (signed long)input_int();
    return *this;
}


/*!
  Reads an unsigned \c long int from the stream and returns a reference to the
  stream. See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( unsigned long &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned long)input_int();
    return *this;
}


/*!
  Reads a \c float from the stream and returns a reference to the stream.
  See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    f = (float)input_double();
    return *this;
}


/*!
  Reads a \c double from the stream and returns a reference to the stream.
  See flags() for an explanation of expected input format.
*/

QTextStream &QTextStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    f = input_double();
    return *this;
}


/*!
  Reads a word from the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator>>( char *s )
{
    CHECK_STREAM_PRECOND
    int maxlen = width( 0 );
    QChar c = eat_ws();
    if ( !maxlen )
	maxlen = -1;
    while ( c != QEOF ) {
	if ( ts_isspace(c) || maxlen-- == 0 ) {
	    ts_ungetc( c );
	    break;
	}
	*s++ = c;
	c = ts_getc();
    }

    *s = '\0';
    return *this;
}

/*!
  Reads a word from the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator>>( QString &str )
{
    CHECK_STREAM_PRECOND
    str=QString::fromLatin1("");
    QChar	c = eat_ws();

    while ( c != QEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}
	str += c;
	c = ts_getc();
    }
    return *this;
}

/*!
  Reads a word from the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator>>( QCString &str )
{
    CHECK_STREAM_PRECOND
    QCString  *dynbuf = 0;
    const int buflen = 256;
    char      buffer[buflen];
    char     *s = buffer;
    int	      i = 0;
    QChar	      c = eat_ws();

    while ( c != QEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}
	if ( i >= buflen-1 ) {
	    if ( !dynbuf )  {			// create dynamic buffer
		dynbuf = new QCString(buflen*2);
		memcpy( dynbuf->data(), s, i );	// copy old data
	    } else if ( i >= (int)dynbuf->size()-1 ) {
		dynbuf->resize( dynbuf->size()*2 );
	    }
	    s = dynbuf->data();
	}
	s[i++] = c;
	c = ts_getc();
    }
    str.resize( i+1 );
    memcpy( str.data(), s, i );
    delete dynbuf;
    return *this;
}


/*!
  Reads a line from the stream and returns a string containing the text.

  The returned string does not contain any trailing newline or carriage
  return. Note that this is different from QIODevice::readLine(), which
  does not strip the newline at the end of the line.

  On EOF you will get a QString that is null. On reading an empty line the
  returned QString is empty but not null.

  \sa QIODevice::readLine()
*/

QString QTextStream::readLine()
{
#if defined(CHECK_STATE)
    if ( !dev ) {
	qWarning( "QTextStream::readLine: No device" );
	return QString::null;
    }
#endif
    QString result( "" );
    const int buf_size = 256;
    QChar c[buf_size];
    int pos = 0;

    c[pos] = ts_getc();
    if ( c[pos] == QEOF )
	return QString::null;

    while ( c[pos] != QEOF && c[pos] != '\n' ) {
	pos++;
	if ( pos >= buf_size ) {
	    result += QString( c, pos );
	    pos = 0;
	}
	c[pos] = ts_getc();
    }
    result += QString( c, pos );

    int len = (int)result.length();
    if ( len && result[len-1] == '\r' )
	result.truncate(len-1); // (if there are two \r, let one stay)

    return result;
}


/*!
  Reads the entire stream and returns a string containing the text.

  \sa QIODevice::readLine()
*/

QString QTextStream::read()
{
#if defined(CHECK_STATE)
    if ( !dev ) {
	qWarning( "QTextStream::read: No device" );
	return QString::null;
    }
#endif
    QString    result;
    const uint bufsize = 512;
    QChar      buf[bufsize];
    uint       i, num, start;
    bool       skipped_cr = FALSE;

    while ( 1 ) {
	num = ts_getbuf(buf,bufsize);
	// do a s/\r\n/\n
	start = 0;
	for ( i=0; i<num; i++ ) {
	    if ( buf[i] == '\r' ) {
		// Only skip single cr's preceding lf's
		if ( skipped_cr ) {
		    result += buf[i];
		    start++;
		} else {
		    result += QString( &buf[start], i-start );
		    start = i+1;
		    skipped_cr = TRUE;
		}
	    } else {
		if ( skipped_cr ) {
		    if ( buf[i] != '\n' ) {
			// Should not have skipped it
			result += '\r';
		    }
		    skipped_cr = FALSE;
		}
	    }
	}
	if ( start < num )
	    result += QString( &buf[start], i-start );
	if ( num != bufsize ) // if ( EOF )
	    break;
    }
    return result;
}



/*****************************************************************************
  QTextStream write functions
 *****************************************************************************/

/*!
  Writes a \c char to the stream and returns a reference to the stream.

  The character \a c is assumed to be Latin1 encoded independent of the Encoding set
  for the QTextStream.
*/
QTextStream &QTextStream::operator<<( QChar c )
{
    CHECK_STREAM_PRECOND
    ts_putc( c );
    return *this;
}

/*!
  Writes a \c char to the stream and returns a reference to the stream.
*/
QTextStream &QTextStream::operator<<( char c )
{
    CHECK_STREAM_PRECOND
    unsigned char uc = (unsigned char) c;
    ts_putc( uc );
    return *this;
}

QTextStream &QTextStream::output_int( int format, ulong n, bool neg )
{
    static char hexdigits_lower[] = "0123456789abcdef";
    static char hexdigits_upper[] = "0123456789ABCDEF";
    CHECK_STREAM_PRECOND
    char buf[76];
    register char *p;
    int	  len;
    char *hexdigits;

    switch ( flags() & I_BASE_MASK ) {

	case I_BASE_2:				// output binary number
	    switch ( format & I_TYPE_MASK ) {
		case I_SHORT: len=16; break;
		case I_INT:   len=sizeof(int)*8; break;
		case I_LONG:  len=32; break;
		default:      len = 0;
	    }
	    p = &buf[74];			// go reverse order
	    *p = '\0';
	    while ( len-- ) {
		*--p = (char)(n&1) + '0';
		n >>= 1;
		if ( !n )
		    break;
	    }
	    if ( flags() & showbase ) {		// show base
		*--p = (flags() & uppercase) ? 'B' : 'b';
		*--p = '0';
	    }
	    break;

	case I_BASE_8:				// output octal number
	    p = &buf[74];
	    *p = '\0';
	    do {
		*--p = (char)(n&7) + '0';
		n >>= 3;
	    } while ( n );
	    if ( flags() & showbase )
		*--p = '0';
	    break;

	case I_BASE_16:				// output hexadecimal number
	    p = &buf[74];
	    *p = '\0';
	    hexdigits = (flags() & uppercase) ?
		hexdigits_upper : hexdigits_lower;
	    do {
		*--p = hexdigits[(int)n&0xf];
		n >>= 4;
	    } while ( n );
	    if ( flags() & showbase ) {
		*--p = (flags() & uppercase) ? 'X' : 'x';
		*--p = '0';
	    }
	    break;

	default:				// decimal base is default
	    p = &buf[74];
	    *p = '\0';
	    if ( neg )
		n = (ulong)(-(long)n);
	    do {
		*--p = ((int)(n%10)) + '0';
		n /= 10;
	    } while ( n );
	    if ( neg )
		*--p = '-';
	    else if ( flags() & showpos )
		*--p = '+';
	    if ( (flags() & internal) && fwidth && !ts_isdigit(*p) ) {
		ts_putc( *p );			// special case for internal
		++p;				//   padding
		fwidth--;
		return *this << (const char*)p;
	    }
    }
    if ( fwidth ) {				// adjustment required
	if ( !(flags() & left) ) {		// but NOT left adjustment
	    len = qstrlen(p);
	    int padlen = fwidth - len;
	    if ( padlen <= 0 ) {		// no padding required
		writeBlock( p, len );
	    } else if ( padlen < (int)(p-buf) ) { // speeds up padding
		memset( p-padlen, (char)fillchar, padlen );
		writeBlock( p-padlen, padlen+len );
	    }
	    else				// standard padding
		*this << (const char*)p;
	}
	else
	    *this << (const char*)p;
	fwidth = 0;				// reset field width
    }
    else
	writeBlock( p, qstrlen(p) );
    return *this;
}


/*!
  Writes a \c short integer to the stream and returns a reference to
  the stream.
*/

QTextStream &QTextStream::operator<<( signed short i )
{
    return output_int( I_SHORT | I_SIGNED, i, i < 0 );
}


/*!
  Writes an \c unsigned \c short integer to the stream and returns a reference
  to the stream.
*/

QTextStream &QTextStream::operator<<( unsigned short i )
{
    return output_int( I_SHORT | I_UNSIGNED, i, FALSE );
}


/*!
  Writes an \c int to the stream and returns a reference to
  the stream.
*/

QTextStream &QTextStream::operator<<( signed int i )
{
    return output_int( I_INT | I_SIGNED, i, i < 0 );
}


/*!
  Writes an \c unsigned \c int to the stream and returns a reference to
  the stream.
*/

QTextStream &QTextStream::operator<<( unsigned int i )
{
    return output_int( I_INT | I_UNSIGNED, i, FALSE );
}


/*!
  Writes a \c long \c int to the stream and returns a reference to
  the stream.
*/

QTextStream &QTextStream::operator<<( signed long i )
{
    return output_int( I_LONG | I_SIGNED, i, i < 0 );
}


/*!
  Writes an \c unsigned \c long \c int to the stream and returns a reference to
  the stream.
*/

QTextStream &QTextStream::operator<<( unsigned long i )
{
    return output_int( I_LONG | I_UNSIGNED, i, FALSE );
}


/*!
  Writes a \c float to the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator<<( float f )
{
    return *this << (double)f;
}


/*!
  Writes a \c double to the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    char buf[64];
    char f_char;
    char format[16];
    if ( (flags()&floatfield) == fixed )
	f_char = 'f';
    else if ( (flags()&floatfield) == scientific )
	f_char = (flags() & uppercase) ? 'E' : 'e';
    else
	f_char = (flags() & uppercase) ? 'G' : 'g';
    register char *fs = format;			// generate format string
    *fs++ = '%';				//   "%.<prec>l<f_char>"
    *fs++ = '.';
    int prec = precision();
    if ( prec > 99 )
	prec = 99;
    if ( prec >= 10 ) {
	*fs++ = prec / 10 + '0';
	*fs++ = prec % 10 + '0';
    } else {
	*fs++ = prec + '0';
    }
    *fs++ = 'l';
    *fs++ = f_char;
    *fs = '\0';
    sprintf( buf, format, f );			// convert to text
    if ( fwidth )				// padding
	*this << (const char*)buf;
    else					// just write it
	writeBlock( buf, qstrlen(buf) );
    return *this;
}


/*!
  Writes a string to the stream and returns a reference to the stream.

  The string \a s is assumed to be Latin1 encoded independent of the Encoding set
  for the QTextStream.
*/

QTextStream &QTextStream::operator<<( const char* s )
{
    CHECK_STREAM_PRECOND
    char padbuf[48];
    uint len = qstrlen( s );			// don't write null terminator
    if ( fwidth ) {				// field width set
	int padlen = fwidth - len;
	fwidth = 0;				// reset width
	if ( padlen > 0 ) {
	    char *ppad;
	    if ( padlen > 46 ) {		// create extra big fill buffer
		ppad = new char[padlen];
		CHECK_PTR( ppad );
	    } else {
		ppad = padbuf;
	    }
	    memset( ppad, (char)fillchar, padlen );	// fill with fillchar
	    if ( !(flags() & left) ) {
		writeBlock( ppad, padlen );
		padlen = 0;
	    }
	    writeBlock( s, len );
	    if ( padlen )
		writeBlock( ppad, padlen );
	    if ( ppad != padbuf )		// delete extra big fill buf
		delete[] ppad;
	    return *this;
	}
    }
    writeBlock( s, len );
    return *this;
}

/*!
  Writes \a s to the stream and returns a reference to the stream.

  The string \a s is assumed to be Latin1 encoded independent of the Encoding set
  for the QTextStream.
*/

QTextStream &QTextStream::operator<<( const QCString & s )
{
    return operator<<(s.data());
}

/*!
  Writes \a s to the stream and returns a reference to the stream.
*/

QTextStream &QTextStream::operator<<( const QString& s )
{
    CHECK_STREAM_PRECOND
    uint len = s.length();
    QString s1 = s;
    if ( fwidth ) {				// field width set
	if ( !(flags() & left) ) {
	    s1 = s.rightJustify(fwidth, (char)fillchar);
	} else {
	    s1 = s.leftJustify(fwidth, (char)fillchar);
	}
	fwidth = 0;				// reset width
    }
    writeBlock( s1.unicode(), len );
    return *this;
}


/*!
  Writes a pointer to the stream and returns a reference to the stream.

  The \e ptr is output as an unsigned long hexadecimal integer.
*/

QTextStream &QTextStream::operator<<( void *ptr )
{
    int f = flags();
    setf( hex, basefield );
    setf( showbase );
    unsetf( uppercase );
    output_int( I_LONG | I_UNSIGNED, (ulong)ptr, FALSE );
    flags( f );
    return *this;
}


/*!
  \fn int QTextStream::flags() const
  Returns the current stream flags. The default value is 0.

  The meaning of the flags are:
  <ul>
    <li> \e skipws - Not currently used - whitespace always skipped
    <li> \e left - Numeric fields are left-aligned
    <li> \e right - Not currently used (by default numerics are right aligned)
    <li> \e internal - Put any padding spaces between +/- and value
    <li> \e bin - Output \e and input only in binary
    <li> \e oct - Output \e and input only in octal
    <li> \e dec - Output \e and input only in decimal
    <li> \e hex - Output \e and input only in hexadecimal
    <li> \e showbase - Annotate numeric outputs with 0b, 0, or 0x if in
		\e bin, \e oct, or \e hex format
    <li> \e showpoint - Not currently used
    <li> \e uppercase - Use 0B and 0X rather than 0b and 0x
    <li> \e showpos - Show + for positive numeric values
    <li> \e scientific - Use scientific notation for floating point values
    <li> \e fixed - Use fixed-point notation for floating point values
  </ul>

  Note that unless \e bin, \e oct, \e dec, or \e hex is set, the input base is
    octal if the value starts with 0, hexadecimal if it starts with 0x, binary
    if the value starts with 0b, and decimal otherwise.

  \sa setf(), unsetf()
*/

/*!
  \fn int QTextStream::flags( int f )
  Sets the stream flags to \e f.
  Returns the previous stream flags.

  \sa setf(), unsetf(), flags()
*/

/*!
  \fn int QTextStream::setf( int bits )
  Sets the stream flag bits \e bits.
  Returns the previous stream flags.

  Equivalent to <code>flags( flags() | bits )</code>.

  \sa setf(), unsetf()
*/

/*!
  \fn int QTextStream::setf( int bits, int mask )
  Sets the stream flag bits \e bits with a bit mask \e mask.
  Returns the previous stream flags.

  Equivalent to <code>flags( (flags() & ~mask) | (bits & mask) )</code>.

  \sa setf(), unsetf()
*/

/*!
  \fn int QTextStream::unsetf( int bits )
  Clears the stream flag bits \e bits.
  Returns the previous stream flags.

  Equivalent to <code>flags( flags() & ~mask )</code>.

  \sa setf()
*/

/*!
  \fn int QTextStream::width() const
  Returns the field width. The default value is 0.
*/

/*!
  \fn int QTextStream::width( int w )
  Sets the field width to \e w. Returns the previous field width.
*/

/*!
  \fn int QTextStream::fill() const
  Returns the fill character. The default value is ' ' (space).
*/

/*!
  \fn int QTextStream::fill( int f )
  Sets the fill character to \e f. Returns the previous fill character.
*/

/*!
  \fn int QTextStream::precision() const
  Returns the precision. The default value is 6.
*/

/*!
  \fn int QTextStream::precision( int p )
  Sets the precision to \e p. Returns the previous precision setting.
*/


 /*****************************************************************************
  QTextStream manipulators
 *****************************************************************************/

QTextStream &bin( QTextStream &s )
{
    s.setf(QTS::bin,QTS::basefield);
    return s;
}

QTextStream &oct( QTextStream &s )
{
    s.setf(QTS::oct,QTS::basefield);
    return s;
}

QTextStream &dec( QTextStream &s )
{
    s.setf(QTS::dec,QTS::basefield);
    return s;
}

QTextStream &hex( QTextStream &s )
{
    s.setf(QTS::hex,QTS::basefield);
    return s;
}

QTextStream &endl( QTextStream &s )
{
    return s << '\n';
}

QTextStream &flush( QTextStream &s )
{
    if ( s.device() )
	s.device()->flush();
    return s;
}

QTextStream &ws( QTextStream &s )
{
    s.skipWhiteSpace();
    return s;
}

QTextStream &reset( QTextStream &s )
{
    s.reset();
    return s;
}


/*!
  \class QTextIStream qtextstream.h
  \brief A convenience class for input streams.

  For simple tasks, code should be simple.  Hence this
  class is a shorthand to avoid passing the \e mode argument
  to the normal QTextStream constructors.

  This makes it easy for example, to write things like this:
\code
    QString data = "123 456";
    int a, b;
    QTextIStream(&data) >> a >> b;
\endcode

  \sa QTextOStream
*/

/*!
  \fn QTextIStream::QTextIStream( QString *s )

  Constructs a stream to read from string \a s.
*/
/*!
  \fn QTextIStream::QTextIStream( QByteArray ba )

  Constructs a stream to read from the array \a ba.
*/
/*!
  \fn QTextIStream::QTextIStream( FILE *f )

  Constructs a stream to read from the file \a f.
*/


/*!
  \class QTextOStream qtextstream.h
  \brief A convenience class for output streams.

  For simple tasks, code should be simple.  Hence this
  class is a shorthand to avoid passing the \e mode argument
  to the normal QTextStream constructors.

  This makes it easy for example, to write things like this:
\code
    QString result;
    QTextOStream(&result) << "pi = " << 3.14;
\endcode
*/

/*!
  \fn QTextOStream::QTextOStream( QString *s )

  Constructs a stream to write to string \a s.
*/
/*!
  \fn QTextOStream::QTextOStream( QByteArray ba )

  Constructs a stream to write to the array \a ba.
*/
/*!
  \fn QTextOStream::QTextOStream( FILE *f )

  Constructs a stream to write to the file \a f.
*/



/*!
  Sets the encoding of this stream to \a e, where \a e is one of:
  <ul>
  <li> \c Locale Using local file format (Latin1 if locale is not
  set), but autodetecting Unicode(utf16) on input.
  <li> \c Unicode Using Unicode(utf16) for input and output. Output
  will be written in the order most efficient for the current platform
  (i.e. the order used internally in QString).
  <li> \c UnicodeUTF8 Using Unicode(utf8) for input and output. If you use it
  for input it will autodetect utf16 and use it instead of utf8.
  <li> \c Latin1  ISO-8859-1. Will not autodetect utf16.
  <li> \c UnicodeNetworkOrder Using network order Unicode(utf16) for
  input and output. Useful when reading Unicode data that does not
  start with the byte order marker.
  <li> \c UnicodeReverse Using reverse network order Unicode(utf16)
  for input and output. Useful when reading Unicode data that does not
  start with the byte order marker, or writing data that should be
  read by buggy Windows applications.
  <li> \c RawUnicode Like Unicode, but does not write the byte order
  marker, nor does it autodetect the byte order. Only useful when
  writing to non-persistent storage used by a single process.
  </ul>

  \c Locale and all Unicode encodings, except \c RawUnicode, will look at
  the first two bytes in a input stream to determine the byte order. The
  initial byte order marker will be stripped off before data is read.

  Note that this function should be called before any data is read
  to/written from the stream.
  \sa setCodec()
*/

void QTextStream::setEncoding( Encoding e )
{
    if ( d->sourceType == QTextStreamPrivate::String )
	return; // QString does not need any encoding
    switch ( e ) {
    case Unicode:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = TRUE;
	break;
    case UnicodeUTF8:
#ifndef QT_NO_CODECS
	mapper = QTextCodec::codecForMib( 106 );
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = TRUE;
#else
	mapper = 0;
	latin1 = TRUE;
	doUnicodeHeader = TRUE;
#endif
	break;
    case UnicodeNetworkOrder:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = QChar::networkOrdered();
	break;
    case UnicodeReverse:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = !QChar::networkOrdered();   //reverse network ordered
	break;
    case RawUnicode:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = FALSE;
	internalOrder = TRUE;
	break;
    case Locale:
	latin1 = TRUE; 				// fallback to Latin 1
#ifndef QT_NO_TEXTCODEC
	mapper = QTextCodec::codecForLocale();
#if defined(_OS_WIN32_)
	if ( GetACP() == 1252 )
	    mapper = 0;				// Optimized latin1 processing
#endif
	if ( mapper && mapper->mibEnum() == 4 )
#endif
	    mapper = 0;				// Optimized latin1 processing
	doUnicodeHeader = TRUE; // If it reads as Unicode, accept it
	break;
    case Latin1:
	mapper = 0;
	doUnicodeHeader = FALSE;
	latin1 = TRUE;
	break;
    }
}


#ifndef QT_NO_TEXTCODEC
/*!  Sets the codec for this stream to \a codec. Will not try to
  autodetect Unicode.

  Note that this function should be called before any data is read
  to/written from the stream.

  \sa setEncoding()
*/

void QTextStream::setCodec( QTextCodec *codec )
{
    if ( d->sourceType == QTextStreamPrivate::String )
	return; // QString does not need any codec
    mapper = codec;
    doUnicodeHeader = FALSE;
}
#endif

#endif // QT_NO_TEXTSTREAM
