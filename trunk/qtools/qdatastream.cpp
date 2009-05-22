/****************************************************************************
** 
**
** Implementation of QDataStream class
**
** Created : 930831
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

#include "qdatastream.h"

#ifndef QT_NO_DATASTREAM
#include "qbuffer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// REVISED: warwick
/*!
  \class QDataStream qdatastream.h

  \brief The QDataStream class provides serialization of
  binary data to a QIODevice.

  \ingroup io

  A data stream is a binary stream of encoded information which is 100%
  independent of the host computer operation system, CPU or byte order.	 A
  stream that is written by a PC under DOS/Windows can be read by a
  Sun SPARC running Solaris.

  The QDataStream class implements serialization of primitive types, like
  \c char, \c short, \c int, \c char* etc.  Serialization of more complex
  data is accomplished by breaking up the data into primitive units.

  The programmer can select which byte order to use when serializing data.
  The default setting is big endian (MSB first). Changing it to little
  endian breaks the portability (unless the reader also changes to little
  endian).  We recommend keeping this setting unless you have
  special requirements.

  A data stream cooperates closely with a QIODevice. A QIODevice
  represents an input/output medium one can read data from and write data
  to.  The QFile class is an example of an IO device.

  Example (write data to a stream):
  \code
    QFile f( "file.dta" );
    f.open( IO_WriteOnly );			// open file for writing
    QDataStream s( &f );			// serialize using f
    s << "the answer is";			// serialize string
    s << (Q_INT32)42;				// serialize integer
  \endcode

  Example (read data from a stream):
  \code
    QFile f( "file.dta" );
    f.open( IO_ReadOnly );			// open file for reading
    QDataStream s( &f );			// serialize using f
    char   *str;
    Q_INT32 a;
    s >> str >> a;				// "the answer is" and 42
    delete str;					// delete string
  \endcode

  In the last example, if you read into a QString instead of a \c char*
  you do not have to delete it.

  Normally, each item written to the stream is written in a fixed binary
  format.
  For example, a \c char* is written as a 32-bit integer equal to the
  length of the string including the NUL byte, followed by all the
  characters of the string including the NUL byte. Similarly when
  reading a string, 4 bytes are read to create the 32-bit length value,
  then that many characters for the string including the NUL. For a complete
  description of all Qt types supporting data streaming see \link
  datastreamformat.html Format of the QDataStream operators \endlink .

  If you want a "parsing" input stream, see QTextStream. If you just want the
  data to be human-readable to aid in debugging, you can set the data
  stream into printable data mode with setPrintableData(). The data is
  then written slower, in a human readable bloated form that is sufficient
  for debugging.

  If you are producing a new binary data format, such as a file format
  for documents created by your application, you could use a QDataStream
  to write the data in a portable format. Typically, you would write
  a brief header containing a magic string and a version number to give
  yourself room for future expansion. For example:

  \code
    // Open the file.
    QFile f( "file.xxx" );
    f.open( IO_WriteOnly );
    QDataStream s( &f );

    // Write a header with a "magic number" and a version
    s << 0xa0b0c0d0;
    s << 123;

    // Write the data
    s << [lots of interesting data]
  \endcode

  Then read it in with:

  \code
    // Open the file.
    QFile f( "file.xxx" );
    f.open( IO_ReadOnly );
    QDataStream s( &f );

    // Read and check the header
    Q_UINT32 magic;
    s >> magic;
    if ( magic != 0xa0b0c0d0 )
	return XXX_BAD_FILE_FORMAT;

    // Read the version
    Q_INT32 version;
    s >> version;
    if ( version < 100 )
	return XXX_BAD_FILE_TOO_OLD;
    if ( version > 123 )
	return XXX_BAD_FILE_TOO_NEW;
    if ( version <= 110 )
	s.setVersion(1);

    // Read the data
    s >> [lots of interesting data];
    if ( version > 120 )
	s >> [data new in XXX version 1.2];
    s >> [other interesting data];
  \endcode

  \sa QTextStream QVariant
*/


/*****************************************************************************
  QDataStream member functions
 *****************************************************************************/

#if defined(CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				qWarning( "QDataStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif

static int  systemWordSize = 0;
static bool systemBigEndian;

static const int DefaultStreamVersion = 3;
// 3 is default in Qt 2.1
// 2 is the Qt 2.0.x format
// 1 is the Qt 1.x format

/*!
  Constructs a data stream that has no IO device.

  \sa setDevice()
*/

QDataStream::QDataStream()
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = 0;				// no device set
    owndev    = FALSE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
  Constructs a data stream that uses the IO device \a d.

  \sa setDevice(), device()
*/

QDataStream::QDataStream( QIODevice *d )
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = d;				// set device
    owndev    = FALSE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
  Constructs a data stream that operates on a byte array through an
  internal QBuffer device.

  Example:
  \code
    static char bindata[] = { 231, 1, 44, ... };
    QByteArray	a;
    a.setRawData( bindata, sizeof(bindata) );	// a points to bindata
    QDataStream s( a, IO_ReadOnly );		// open on a's data
    s >> [something];				// read raw bindata
    a.resetRawData( bindata, sizeof(bindata) ); // finished
  \endcode

  The QArray::setRawData() function is not for the inexperienced.
*/

QDataStream::QDataStream( QByteArray a, int mode )
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = new QBuffer( a );		// create device
    ((QBuffer *)dev)->open( mode );		// open device
    owndev    = TRUE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
  Destructs the data stream.

  The destructor will not affect the current IO device, unless it
  is an internal IO device processing a QByteArray passed in the constructor.
*/

QDataStream::~QDataStream()
{
    if ( owndev )
	delete dev;
}


/*!
  \fn QIODevice *QDataStream::device() const
  Returns the IO device currently set.
  \sa setDevice(), unsetDevice()
*/

/*!
  void QDataStream::setDevice(QIODevice *d )
  Sets the IO device to \a d.
  \sa device(), unsetDevice()
*/

void QDataStream::setDevice(QIODevice *d )
{
    if ( owndev ) {
	delete dev;
	owndev = FALSE;
    }
    dev = d;
}

/*!
  Unsets the IO device.	 This is the same as calling setDevice( 0 ).
  \sa device(), setDevice()
*/

void QDataStream::unsetDevice()
{
    setDevice( 0 );
}


/*!
  \fn bool QDataStream::atEnd() const
  Returns TRUE if the IO device has reached the end position (end of
  stream or file) or if there is no IO device set.

  Returns FALSE if the current position of the read/write head of the IO
  device is somewhere before the end position.

  \sa QIODevice::atEnd()
*/

/*!\fn bool QDataStream::eof() const

  \obsolete

  Returns TRUE if the IO device has reached the end position (end of
  stream or file) or if there is no IO device set.

  Returns FALSE if the current position of the read/write head of the IO
  device is somewhere before the end position.

  \sa QIODevice::atEnd()
*/

/*!
  \fn int QDataStream::byteOrder() const
  Returns the current byte order setting - either \c BigEndian or
  \c LittleEndian.

  \sa setByteOrder()
*/

/*!
  Sets the serialization byte order to \a bo.

  The \a bo parameter can be \c QDataStream::BigEndian or
  \c QDataStream::LittleEndian.

  The default setting is big endian.  We recommend leaving this setting unless
  you have special requirements.

  \sa byteOrder()
*/

void QDataStream::setByteOrder( int bo )
{
    byteorder = bo;
    if ( systemBigEndian )
	noswap = byteorder == BigEndian;
    else
	noswap = byteorder == LittleEndian;
}


/*!
  \fn bool QDataStream::isPrintableData() const
  Returns TRUE if the printable data flag has been set.
  \sa setPrintableData()
*/

/*!
  \fn void QDataStream::setPrintableData( bool enable )
  Sets or clears the printable data flag.

  If this flag is set, the write functions will generate output that
  consists of printable characters (7 bit ASCII).

  We recommend enabling printable data only for debugging purposes
  (it is slower and creates larger output).
*/


/*!
  \fn int QDataStream::version() const
  Returns the version number of the data serialization format.
  In Qt 2.1, this number is by default 3.
  \sa setVersion()
*/

/*!
  \fn void QDataStream::setVersion( int v )
  Sets the version number of the data serialization format.

  In order to accomodate for new functionality, the datastream
  serialization format of some Qt classes has changed in some versions of
  Qt. If you want to read data that was created by an earlier version of
  Qt, or write data that can be read by a program that was compiled with
  an earlier version of Qt, use this function to modify the serialization
  format of QDataStream.

  For Qt 1.x compatibility, use \a v == 1.

  For Qt 2.0.x compatibility, use \a v == 2 (Not required for reading in
  Qt 2.1).

  \sa version()
*/

/*****************************************************************************
  QDataStream read functions
 *****************************************************************************/


static Q_INT32 read_int_ascii( QDataStream *s )
{
    register int n = 0;
    char buf[40];
    while ( TRUE ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 38 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';
    return atol( buf );
}


/*!
  \fn QDataStream &QDataStream::operator>>( Q_UINT8 &i )
  Reads an unsigned byte from the stream and returns a reference to
  the stream.
*/

/*!
  Reads a signed byte from the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT8 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (Q_INT8)dev->getch();
	if ( i == '\\' ) {			// read octal code
	    char buf[4];
	    dev->readBlock( buf, 3 );
	    i = (buf[2] & 0x07)+((buf[1] & 0x07) << 3)+((buf[0] & 0x07) << 6);
	}
    } else {					// data or text
	i = (Q_INT8)dev->getch();
    }
    return *this;
}


/*!
  \fn QDataStream &QDataStream::operator>>( Q_UINT16 &i )
  Reads an unsigned 16-bit integer from the stream and returns a reference to
  the stream.
*/

/*!
  Reads a signed 16-bit integer from the stream and returns a reference to
  the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT16 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (Q_INT16)read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_INT16) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[2];
	dev->readBlock( b, 2 );
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
  \fn QDataStream &QDataStream::operator>>( Q_UINT32 &i )
  Reads an unsigned 32-bit integer from the stream and returns a reference to
  the stream.
*/

/*!
  Reads a signed 32-bit integer from the stream and returns a reference to
  the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT32 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_INT32) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[4];
	dev->readBlock( b, 4 );
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}

/*!
  \fn QDataStream &QDataStream::operator>>( Q_UINT64 &i )
  Reads an unsigned 64-bit integer from the stream and returns a reference to
  the stream, or uses the Q_UINT32 operator if 64 bit is not available.
*/

/*!
  Reads a signed 64-bit integer from the stream and returns a reference to
  the stream, or uses the Q_UINT32 operator if 64 bit is not available.
*/

QDataStream &QDataStream::operator>>( Q_INT64 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_INT64) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[sizeof(Q_INT64)];
	dev->readBlock( b, sizeof(Q_INT64) );
	if ( sizeof(Q_INT64) == 8 ) {
	    *p++ = b[7];
	    *p++ = b[6];
	    *p++ = b[5];
	    *p++ = b[4];
	}
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}

static double read_double_ascii( QDataStream *s )
{
    register int n = 0;
    char buf[80];
    while ( TRUE ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 78 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';
    return atof( buf );
}


/*!
  Reads a 32-bit floating point number from the stream using the standard
  IEEE754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = (float)read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(float) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&f);
	char b[4];
	dev->readBlock( b, 4 );
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
  Reads a 64-bit floating point number from the stream using the standard
  IEEE754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&f);
	char b[8];
	dev->readBlock( b, 8 );
	*p++ = b[7];
	*p++ = b[6];
	*p++ = b[5];
	*p++ = b[4];
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
  Reads the '\0'-terminated string \a s from the stream and returns
  a reference to the stream.

  Space for the string is allocated using \c new - the caller must
  eventually call delete[] on the value.
*/

QDataStream &QDataStream::operator>>( char *&s )
{
    uint len = 0;
    return readBytes( s, len );
}


/*!
  Reads the buffer \a s from the stream and returns a reference to the
  stream.

  The buffer \a s is allocated using \c new. Destroy it with the \c delete[]
  operator.  If the length is zero or \a s cannot be allocated, \a s is
  set to 0.

  The \a l parameter will be set to the length of the buffer.

  The serialization format is an Q_UINT32 length specifier first, then the
  data (\a l bytes).

  \sa readRawBytes(), writeBytes()
*/

QDataStream &QDataStream::readBytes( char *&s, uint &l )
{
    CHECK_STREAM_PRECOND
    Q_UINT32 len;
    *this >> len;				// first read length spec
    l = (uint)len;
    if ( len == 0 || eof() ) {
	s = 0;
	return *this;
    } else {
	s = new char[len];			// create char array
	CHECK_PTR( s );
	if ( !s )				// no memory
	    return *this;
	return readRawBytes( s, (uint)len );
    }
}


/*!
  Reads \a len bytes from the stream into \a s and returns a reference to
  the stream.

  The buffer \a s must be preallocated.

  \sa readBytes(), QIODevice::readBlock(), writeRawBytes()
*/

QDataStream &QDataStream::readRawBytes( char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	register Q_INT8 *p = (Q_INT8*)s;
	while ( len-- )
	    *this >> *p++;
    } else {					// read data char array
	dev->readBlock( s, len );
    }
    return *this;
}


/*****************************************************************************
  QDataStream write functions
 *****************************************************************************/


/*!
  \fn QDataStream &QDataStream::operator<<( Q_UINT8 i )
  Writes an unsigned byte to the stream and returns a reference to
  the stream.
*/

/*!
  Writes a signed byte to the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT8 i )
{
    CHECK_STREAM_PRECOND
    if ( printable && (i == '\\' || !isprint(i)) ) {
	char buf[6];				// write octal code
	buf[0] = '\\';
	buf[1] = '0' + ((i >> 6) & 0x07);
	buf[2] = '0' + ((i >> 3) & 0x07);
	buf[3] = '0' + (i & 0x07);
	buf[4] = '\0';
	dev->writeBlock( buf, 4 );
    } else {
	dev->putch( i );
    }
    return *this;
}


/*!
  \fn QDataStream &QDataStream::operator<<( Q_UINT16 i )
  Writes an unsigned 16-bit integer to the stream and returns a reference
  to the stream.
*/

/*!
  Writes a signed 16-bit integer to the stream and returns a reference to
  the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT16 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_INT16) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[2];
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 2 );
    }
    return *this;
}


/*!
  \fn QDataStream &QDataStream::operator<<( Q_UINT32 i )
  Writes an unsigned 32-bit integer to the stream and returns a reference to
  the stream.
*/

/*!
  Writes a signed 32-bit integer to the stream and returns a reference to
  the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT32 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_INT32) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[4];
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 4 );
    }
    return *this;
}

/*!
  \fn QDataStream &QDataStream::operator<<( Q_UINT64 i )
  Writes an unsigned 64-bit integer to the stream and returns a reference to
  the stream, or uses the Q_UINT32-operator if 64 bit is not available.
*/

/*!
  Writes a signed 64-bit integer to the stream and returns a reference to
  the stream, or calls the Q_INT32-operator if 64 bit is not available.
*/

QDataStream &QDataStream::operator<<( Q_INT64 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[20];
	sprintf( buf, "%ld\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_INT64) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[sizeof(Q_INT64)];
	if ( sizeof(Q_INT64) == 8 ) {
	    b[7] = *p++;
	    b[6] = *p++;
	    b[5] = *p++;
	    b[4] = *p++;
	}
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, sizeof(Q_INT64) );
    }
    return *this;
}

/*!
  \fn QDataStream &QDataStream::operator<<( uint i )
  Writes an unsigned integer to the stream as a 32-bit unsigned integer
  (Q_UINT32).
  Returns a reference to the stream.
*/

/*!
  \fn QDataStream &QDataStream::operator<<( int i )
  Writes a signed integer to the stream as a 32-bit signed integer (Q_INT32).
  Returns a reference to the stream.
*/


/*!
  Writes a 32-bit floating point number to the stream using the standard
  IEEE754 format.  Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<( float f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[32];
	sprintf( buf, "%g\n", (double)f );
	dev->writeBlock( buf, strlen(buf) );
    } else {
	float g = f;				// fixes float-on-stack problem
	if ( noswap ) {				// no conversion needed
	    dev->writeBlock( (char *)&g, sizeof(float) );
	} else {				// swap bytes
	    register uchar *p = (uchar *)(&g);
	    char b[4];
	    b[3] = *p++;
	    b[2] = *p++;
	    b[1] = *p++;
	    b[0] = *p;
	    dev->writeBlock( b, 4 );
	}
    }
    return *this;
}


/*!
  Writes a 64-bit floating point number to the stream using the standard
  IEEE754 format.  Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[32];
	sprintf( buf, "%g\n", f );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&f);
	char b[8];
	b[7] = *p++;
	b[6] = *p++;
	b[5] = *p++;
	b[4] = *p++;
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 8 );
    }
    return *this;
}


/*!
  Writes the '\0'-terminated string \a s to the stream and returns
  a reference to the stream.

  The string is serialized using writeBytes().
*/

QDataStream &QDataStream::operator<<( const char *s )
{
    if ( !s ) {
	*this << (Q_UINT32)0;
	return *this;
    }
    uint len = qstrlen( s ) + 1;			// also write null terminator
    *this << (Q_UINT32)len;			// write length specifier
    return writeRawBytes( s, len );
}


/*!
  Writes the length specifier \a len and the buffer \a s to the stream and
  returns a reference to the stream.

  The \a len is serialized as an Q_UINT32, followed by \a len bytes from
  \a s.

  \sa writeRawBytes(), readBytes()
*/

QDataStream &QDataStream::writeBytes(const char *s, uint len)
{
    CHECK_STREAM_PRECOND
    *this << (Q_UINT32)len;			// write length specifier
    if ( len )
	writeRawBytes( s, len );
    return *this;
}


/*!
  Writes \a len bytes from \a s to the stream and returns a reference to the
  stream.

  \sa writeBytes(), QIODevice::writeBlock(), readRawBytes()
*/

QDataStream &QDataStream::writeRawBytes( const char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// write printable
	register char *p = (char *)s;
	while ( len-- )
	    *this << *p++;
    } else {					// write data char array
	dev->writeBlock( s, len );
    }
    return *this;
}

#endif // QT_NO_DATASTREAM
