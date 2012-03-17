#ifndef FTEXTSTREAM_H
#define FTEXTSTREAM_H

#include "qtbc.h"
#include "qiodevice.h"
#include "qstring.h"
#include "qgstring.h"
#include <stdio.h>

/** @brief Simplified and optimized version of QTextStream */
class FTextStream
{
  public:
    FTextStream();
    FTextStream( QIODevice * );
    FTextStream( QGString * );
    FTextStream( FILE * );
    virtual ~FTextStream();

    QIODevice	*device() const;
    void	 setDevice( QIODevice * );
    void	 unsetDevice();

    FTextStream &operator<<( char );
    FTextStream &operator<<( const char *);
    FTextStream &operator<<( const QString & );
    FTextStream &operator<<( const QCString & );
    FTextStream &operator<<( signed short );
    FTextStream &operator<<( unsigned short );
    FTextStream &operator<<( signed int );
    FTextStream &operator<<( unsigned int );
    FTextStream &operator<<( signed long );
    FTextStream &operator<<( unsigned long );
    FTextStream &operator<<( float );
    FTextStream &operator<<( double );

  private:
    QIODevice *m_dev;
    bool m_owndev;
    FTextStream &output_int( ulong n, bool neg );

  private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    FTextStream( const FTextStream & );
    FTextStream &operator=( const FTextStream & );
#endif
};

inline FTextStream &FTextStream::operator<<( char c)
{
  m_dev->putch(c);
  return *this;
}

inline FTextStream &FTextStream::operator<<( const char* s)
{
  uint len = qstrlen( s );
  m_dev->writeBlock( s, len );
  return *this;
}

inline FTextStream &FTextStream::operator<<( const QString & s)
{
  return operator<<(s.data());
}

inline FTextStream &FTextStream::operator<<( const QCString &s)
{
  return operator<<(s.data());
}

typedef FTextStream & (*FTSFUNC)(FTextStream &);// manipulator function

inline FTextStream &operator<<( FTextStream &s, FTSFUNC f )
{ return (*f)( s ); }

inline FTextStream &endl( FTextStream & s)
{
  return s << '\n';
}

#endif // FTEXTSTREAM_H
