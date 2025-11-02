/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
 */

#ifndef TEXTSTREAM_H
#define TEXTSTREAM_H

#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <type_traits>

#include "qcstring.h"
#include "construct.h"

/** @brief Text streaming class that buffers data.
 *
 *  Simpler version of std::ostringstream that has much better
 *  performance.
 */
class TextStream final
{
    static const int INITIAL_CAPACITY = 4096;
  public:
    /** Creates an empty stream object.
     */
    explicit TextStream(size_t capacity = INITIAL_CAPACITY)
    {
      m_buffer.reserve(capacity);
    }
    /** Create a text stream object for writing to a std::ostream.
     *  @note data is buffered until flush() is called or the object is destroyed.
     */
    explicit TextStream(std::ostream *s) : m_s(s)
    {
      m_buffer.reserve(INITIAL_CAPACITY);
    }
    /** Create a text stream, initializing the buffer with string \a s
     */
    explicit TextStream(const std::string &s) : m_buffer(s)
    {
      m_buffer.reserve(s.length()+INITIAL_CAPACITY);
    }

    /** Writes any data that is buffered to the attached std::ostream */
   ~TextStream() { flush(); }

    ONLY_DEFAULT_MOVABLE(TextStream)

    /** Sets or changes the std::ostream to write to.
     *  @note Any data already buffered will be flushed.
     */
    void setStream(std::ostream *s)
    {
      flush();
      m_s = s;
      m_f = nullptr;
    }

    void setFile(FILE *f)
    {
      flush();
      m_s = nullptr;
      m_f = f;
    }

    /** Adds a character to the stream */
    TextStream &operator<<( char c)
    {
      m_buffer+=c;
      return static_cast<TextStream&>(*this);
    }
    /** Adds an unsigned character to the stream */
    TextStream &operator<<( unsigned char c)
    {
      m_buffer+=c;
      return static_cast<TextStream&>(*this);
    }

    /** Adds an unsigned character string to the stream */
    TextStream &operator<<( unsigned char *s)
    {
      if (s)
      {
        unsigned char *p = s;
        while(*p)
        {
          m_buffer+=*p;
          p++;
        }
      }
      return static_cast<TextStream&>(*this);
    }

    /** Adds a C-style string to the stream */
    TextStream &operator<<( const char *s)
    {
      if (s) m_buffer+=s;
      return static_cast<TextStream&>(*this);
    }

    /** Adds a QCString to the stream */
    TextStream &operator<<( const QCString &s )
    {
      m_buffer+=s.str();
      return static_cast<TextStream&>(*this);
    }

    /** Adds a std::string to the stream */
    TextStream &operator<<( const std::string &s )
    {
      m_buffer+=s;
      return static_cast<TextStream&>(*this);
    }

    /** Adds a signed short integer to the stream */
    TextStream &operator<<( signed short i)
    {
      output_int32(i,i<0);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a unsigned short integer to the stream */
    TextStream &operator<<( unsigned short i)
    {
      output_int32(i,false);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a signed integer to the stream */
    TextStream &operator<<( signed int i)
    {
      output_int32(i,i<0);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a unsigned integer to the stream */
    TextStream &operator<<( unsigned int i)
    {
      output_int32(i,false);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a size_t integer to the stream.
     *  We use SFINAE to avoid a compiler error in case size_t already matches the 'unsigned int' overload.
     */
    template<typename T,
             typename std::enable_if<std::is_same<T,size_t>::value,T>::type* = nullptr
            >
    TextStream &operator<<( T i)
    {
      output_int32(static_cast<uint32_t>(i),false);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a float to the stream */
    TextStream &operator<<( float f)
    {
      output_double(static_cast<double>(f));
      return static_cast<TextStream&>(*this);
    }

    /** Adds a double to the stream */
    TextStream &operator<<( double d)
    {
      output_double(d);
      return static_cast<TextStream&>(*this);
    }

    /** Adds a array of character to the stream
     *  @param buf the character buffer
     *  @param len the number of characters in the buffer to write
     */
    void write(const char *buf,size_t len)
    {
      m_buffer.append(buf,len);
    }

    /** Flushes the buffer. If a std::ostream is attached, the buffer's
     *  contents will be written to the stream.
     */
    void flush()
    {
      if (m_s)
      {
        m_s->write(m_buffer.c_str(),m_buffer.length());
      }
      else if (m_f)
      {
        fwrite(m_buffer.c_str(),1,m_buffer.length(),m_f);
      }
      m_buffer.clear();
    }

    /** Clears any buffered data */
    void clear()
    {
      m_buffer.clear();
    }

    /** Return the contents of the buffer as a std::string object */
    std::string str() const
    {
      return m_buffer;
    }

    /** Sets the buffer's contents to string \a s.
     *  Any data already in the buffer will be flushed.
     */
    void str(const std::string &s)
    {
      flush();
      m_buffer=s;
    }

    /** Sets the buffer's contents to string \a s
     *  Any data already in the buffer will be flushed.
     */
    void str(const char *s)
    {
      flush();
      if (s) m_buffer=s;
    }

    /** Returns true iff the buffer is empty */
    bool empty() const
    {
      return m_buffer.empty();
    }

  private:
    /** Writes a string representation of an integer to the buffer
     *  @param n the absolute value of the integer
     *  @param neg indicates if the integer is negative
     */
    void output_int32( uint32_t n, bool neg )
    {
      char buf[20];
      char *p = &buf[19];
      *p = '\0';
      if ( neg )
      {
	n = static_cast<uint32_t>(-static_cast<int32_t>(n));
      }
      do { *--p = (static_cast<char>(n%10)) + '0'; n /= 10; } while ( n );
      if ( neg ) *--p = '-';
      m_buffer+=p;
    }
    void output_double( double d)
    {
      char buf[64];
      snprintf(buf,64,"%f",d);
      m_buffer+=buf;
    }
    std::string m_buffer;
    std::ostream *m_s = nullptr;
    FILE *m_f = nullptr;
};

#endif
