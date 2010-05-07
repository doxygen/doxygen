#include "ftextstream.h"
#include <qfile.h>

//----------------------------------------------------------------------------

class QGStringBuffer : public QIODevice 
{
  public:
    QGStringBuffer( QGString* str );
    ~QGStringBuffer();
    bool  open( int m );
    void  close();
    void  flush();
    uint  size() const;
    int   at()   const;
    bool  at( int pos );
    int   readBlock( char *, uint) { return -1; }
    int   writeBlock( const char *p, uint len );
    int   getch() { return -1; }
    int   putch( int ch );
    int   ungetch( int ) { return -1; }

  protected:
    QGString* m_str;

  private:        // Disabled copy constructor and operator=
    QGStringBuffer( const QGStringBuffer & );
    QGStringBuffer &operator=( const QGStringBuffer & );
};

QGStringBuffer::QGStringBuffer( QGString* str ) : m_str(str)
{
  //printf("QGStringBuffer::QGStringBuffer(%p)\n",str);
}

QGStringBuffer::~QGStringBuffer()
{
}

bool  QGStringBuffer::open( int m )
{
    if ( !m_str ) 
    {
#if defined(CHECK_STATE)
	qWarning( "QGStringBuffer::open: No string" );
#endif
	return FALSE;
    }
    if ( isOpen() ) 
    {                           // buffer already open
#if defined(CHECK_STATE)
	qWarning( "QGStringBuffer::open: Buffer already open" );
#endif
	return FALSE;
    }
    setMode( m );
    if ( m & IO_Truncate ) 
    {                    // truncate buffer
	m_str->truncate( 0 );
    }
    if ( m & IO_Append ) 
    {                      // append to end of buffer
	ioIndex = m_str->length();
    } 
    else 
    {
	ioIndex = 0;
    }
    setState( IO_Open );
    setStatus( 0 );
    return TRUE;
}

void  QGStringBuffer::close()
{
  if ( isOpen() ) 
  {
    setFlags( IO_Direct );
    ioIndex = 0;
  }
}

void  QGStringBuffer::flush()
{
}

uint  QGStringBuffer::size() const
{
  return m_str ? m_str->length() : 0;
}

int   QGStringBuffer::at()   const
{
  return ioIndex;
}

bool  QGStringBuffer::at( int pos )
{
#if defined(CHECK_STATE)
  if ( !isOpen() ) 
  {
    qWarning( "QGStringBuffer::at: Buffer is not open" );
    return FALSE;
  }
#endif
  if ( (uint)pos >= m_str->length() ) 
  {
#if defined(CHECK_RANGE)
    qWarning( "QGStringBuffer::at: Index %d out of range", pos );
#endif
    return FALSE;
  }

  ioIndex = pos;
  return TRUE;
}

int QGStringBuffer::writeBlock( const char *p, uint len )
{
  //printf("QGStringBuffer::writeBlock(%p,%d) m_str=%p ioIndex=%d\n",p,len,
  //    m_str,ioIndex);
  m_str->enlarge(ioIndex+len+1);
  memcpy(m_str->data()+ioIndex,p,len);
  ioIndex+=len;
  m_str->data()[ioIndex]='\0';
  m_str->setLen(ioIndex);
  return len;
}

int QGStringBuffer::putch( int ch )
{
  //printf("QGStringBuffer::putch(%d) m_str=%p ioIndex=%d\n",
  //    ch,m_str,ioIndex);
  m_str->enlarge(ioIndex+2);
  m_str->data()[ioIndex] = (char)ch;
  ioIndex++;
  m_str->data()[ioIndex] = '\0';
  m_str->setLen(ioIndex);
  return ch;
}


//----------------------------------------------------------------------------

FTextStream::FTextStream()
{
  m_dev = 0;
  m_owndev = FALSE;
}

FTextStream::FTextStream( QIODevice *dev )
{
  m_dev = dev;
  m_owndev = FALSE;
}

FTextStream::FTextStream( QGString *s )
{
  m_dev = new QGStringBuffer(s);
  ((QGStringBuffer*)m_dev)->open( IO_WriteOnly );
  m_owndev = TRUE;
}

FTextStream::FTextStream( FILE *fh )
{
  m_dev = new QFile;
  ((QFile *)m_dev)->open( IO_WriteOnly, fh);
}

FTextStream::~FTextStream()
{
  if (m_owndev) delete m_dev;
  m_dev = 0;
}

QIODevice *FTextStream::device() const
{
  return m_dev;
}

void FTextStream::setDevice( QIODevice *dev )
{
  if (m_owndev) 
  {
    delete m_dev;
    m_owndev = FALSE;
  }
  m_dev = dev;
}

void FTextStream::unsetDevice()
{
  setDevice(0);
}

FTextStream &FTextStream::output_int( ulong n, bool neg )
{
  char buf[20];
  char *p = &buf[19];
  *p = '\0';
  if ( neg )
  {
    n = (ulong)(-(long)n);
  }
  do 
  {
    *--p = ((int)(n%10)) + '0';
    n /= 10;
  } while ( n );
  if ( neg ) *--p = '-';
  return operator<<(p);
}

FTextStream &FTextStream::operator<<( signed short i )
{
    return output_int( i, i < 0 );
}

FTextStream &FTextStream::operator<<( unsigned short i )
{
    return output_int( i, FALSE );
}

FTextStream &FTextStream::operator<<( signed int i )
{
    return output_int( i, i < 0 );
}

FTextStream &FTextStream::operator<<( unsigned int i )
{
    return output_int( i, FALSE );
}

FTextStream &FTextStream::operator<<( signed long i )
{
    return output_int( i, i < 0 );
}

FTextStream &FTextStream::operator<<( unsigned long i )
{
    return output_int( i, FALSE );
}

FTextStream &FTextStream::operator<<( float f )
{
    return *this << (double)f;
}

FTextStream &FTextStream::operator<<( double d )
{
    char buf[64];
    sprintf(buf,"%f",d);
    return *this << buf;
}





