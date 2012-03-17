/****************************************************************************
** 
**
** Implementation of QTextCodec class
**
** Created : 981015
**
** Copyright (C)1998-2000 Trolltech AS.  All rights reserved.
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

#include "qtextcodec.h"
#ifndef QT_NO_TEXTCODEC

#include "qlist.h"
#ifndef QT_NO_CODECS
#include "qutfcodec.h"
//#include "qgbkcodec.h"
//#include "qeucjpcodec.h"
//#include "qjiscodec.h"
//#include "qsjiscodec.h"
//#include "qeuckrcodec.h"
//#include "qbig5codec.h"
//#include "qrtlcodec.h"
//#include "qtsciicodec.h"
#endif

#include "qfile.h"
#include "qstrlist.h"
#include "qstring.h"

#include <stdlib.h>
#include <ctype.h>
#include <locale.h>


static QList<QTextCodec> * all = 0;
static bool destroying_is_ok; // starts out as 0

/*!  Deletes all the created codecs.

  \warning Do not call this function.

  QApplication calls this just before exiting, to delete any
  QTextCodec objects that may be lying around.  Since various other
  classes hold pointers to QTextCodec objects, it is not safe to call
  this function earlier.

  If you are using the utility classes (like QString) but not using
  QApplication, calling this function at the very end of your
  application can be helpful to chasing down memory leaks, as
  QTextCodec objects will not show up.
*/

void QTextCodec::deleteAllCodecs()
{
    if ( !all )
        return;

    destroying_is_ok = TRUE;
    QList<QTextCodec> * ball = all;
    all = 0;
    ball->clear();
    delete ball;
    destroying_is_ok = FALSE;
}


static void setupBuiltinCodecs();


static void realSetup()
{
#if defined(CHECK_STATE)
    if ( destroying_is_ok )
        qWarning( "creating new codec during codec cleanup" );
#endif
    all = new QList<QTextCodec>;
    all->setAutoDelete( TRUE );
    setupBuiltinCodecs();
}


static inline void setup()
{
    if ( !all )
        realSetup();
}


class QTextStatelessEncoder: public QTextEncoder {
    const QTextCodec* codec;
public:
    QTextStatelessEncoder(const QTextCodec*);
    QCString fromUnicode(const QString& uc, int& lenInOut);
};


class QTextStatelessDecoder : public QTextDecoder {
    const QTextCodec* codec;
public:
    QTextStatelessDecoder(const QTextCodec*);
    QString toUnicode(const char* chars, int len);
};

QTextStatelessEncoder::QTextStatelessEncoder(const QTextCodec* c) :
    codec(c)
{
}


QCString QTextStatelessEncoder::fromUnicode(const QString& uc, int& lenInOut)
{
    return codec->fromUnicode(uc,lenInOut);
}


QTextStatelessDecoder::QTextStatelessDecoder(const QTextCodec* c) :
    codec(c)
{
}


QString QTextStatelessDecoder::toUnicode(const char* chars, int len)
{
    return codec->toUnicode(chars,len);
}



// NOT REVISED
/*!
  \class QTextCodec qtextcodec.h
  \brief Provides conversion between text encodings.

  By making objects of subclasses of QTextCodec, support for
  new text encodings can be added to Qt.

  The abstract virtual functions describe the encoder to the
  system and the coder is used as required in the different
  text file formats supported QTextStream and, under X11 for the
  locale-specific character input and output (under Windows NT
  codecs are not needed for GUI I/O since the system works
  with Unicode already, and Windows 95/98 has built-in convertors
  for the 8-bit local encoding).

  More recently created QTextCodec objects take precedence
  over earlier ones.

  To add support for another 8-bit encoding to Qt, make a subclass
  or QTextCodec and implement at least the following methods:
  <dl>
   <dt>\c const char* name() const
    <dd>Return the official name for the encoding.
   <dt>\c int mibEnum() const
    <dd>Return the MIB enum for the encoding if it is listed in the
      <a href=ftp://ftp.isi.edu/in-notes/iana/assignments/character-sets>
      IANA character-sets encoding file</a>.
  </dl>
  If the encoding is multi-byte then it will have "state"; that is,
  the interpretation of some bytes will be dependent on some preceding
  bytes.  For such an encoding, you will need to implement
  <dl>
   <dt> \c QTextDecoder* makeDecoder() const
    <dd>Return a QTextDecoder that remembers incomplete multibyte
        sequence prefixes or other required state.
  </dl>
  If the encoding does \e not require state, you should implement:
  <dl>
   <dt> \c QString toUnicode(const char* chars, int len) const
    <dd>Converts \e len characters from \e chars to Unicode.
  </dl>
  The base QTextCodec class has default implementations of the above
  two functions, <i>but they are mutually recursive</i>, so you must
  re-implement at least one of them, or both for improved efficiency.

  For conversion from Unicode to 8-bit encodings, it is rarely necessary
  to maintain state.  However, two functions similar to the two above
  are used for encoding:
  <dl>
   <dt> \c QTextEncoder* makeEncoder() const
    <dd>Return a QTextDecoder.
   <dt> \c QCString fromUnicode(const QString& uc, int& lenInOut ) const;
    <dd>Converts \e lenInOut characters (of type QChar) from the start
        of the string \a uc, returning a QCString result, and also returning
        the \link QCString::length() length\endlink
        of the result in lenInOut.
  </dl>
  Again, these are mutually recursive so only one needs to be implemented,
  or both if better efficiency is possible.

  Finally, you must implement:
  <dl>
   <dt> \c int heuristicContentMatch(const char* chars, int len) const
    <dd>Gives a value indicating how likely it is that \e len characters
        from \e chars are in the encoding.
  </dl>
  A good model for this function is the
  QWindowsLocalCodec::heuristicContentMatch function found in the Qt sources.

  A QTextCodec subclass might have improved performance if you also
  re-implement:
  <dl>
   <dt> \c bool canEncode( QChar ) const
    <dd>Test if a Unicode character can be encoded.
   <dt> \c bool canEncode( const QString& ) const
    <dd>Test if a string of Unicode characters can be encoded.
   <dt> \c int heuristicNameMatch(const char* hint) const
    <dd>Test if a possibly non-standard name is referring to the codec.
  </dl>
*/


/*!
  Constructs a QTextCodec, making it of highest precedence.
  The QTextCodec should always be constructed on the heap
  (with new), and once constructed it becomes the responsibility
  of Qt to delete it (which is done at QApplication destruction).
*/
QTextCodec::QTextCodec()
{
    setup();
    all->insert(0,this);
}


/*!
  Destructs the QTextCodec.  Note that you should not delete
  codecs yourself - once created they become the responsibility
  of Qt to delete.
*/
QTextCodec::~QTextCodec()
{
    if ( !destroying_is_ok )
        qWarning("QTextCodec::~QTextCodec() called by application");
    if ( all )
        all->remove( this );
}


/*!
  Returns a value indicating how likely this decoder is
  for decoding some format that has the given name.

  A good match returns a positive number around
  the length of the string.  A bad match is negative.

  The default implementation calls simpleHeuristicNameMatch()
  with the name of the codec.
*/
int QTextCodec::heuristicNameMatch(const char* hint) const
{
    return simpleHeuristicNameMatch(name(),hint);
}


// returns a string cotnaining the letters and numbers from input,
// with a space separating run of a character class.  e.g. "iso8859-1"
// becomes "iso 8859 1"
static QString lettersAndNumbers( const char * input )
{
    QString result;
    QChar c;

    while( input && *input ) {
        c = *input;
        if ( c.isLetter() || c.isNumber() )
            result += c.lower();
        if ( input[1] ) {
            // add space at character class transition, except
            // transition from upper-case to lower-case letter
            QChar n( input[1] );
            if ( c.isLetter() && n.isLetter() ) {
                if ( c == c.lower() && n == n.upper() )
                    result += ' ';
            } else if ( c.category() != n.category() ) {
                result += ' ';
            }
        }
        input++;
    }
    return result.simplifyWhiteSpace();
}

/*!
  A simple utility function for heuristicNameMatch() - it
  does some very minor character-skipping
  so that almost-exact matches score high.
*/
int QTextCodec::simpleHeuristicNameMatch(const char* name, const char* hint)
{
    // if they're the same, return a perfect score.
    if ( name && hint && qstrcmp( name, hint ) == 0 )
        return qstrlen( hint );

    // if the letters and numbers are the same, we have an "almost"
    // perfect match.
    QString h( lettersAndNumbers( hint ) );
    QString n( lettersAndNumbers( name ) );
    if ( h == n )
        return qstrlen( hint )-1;

    if ( h.stripWhiteSpace() == n.stripWhiteSpace() )
        return qstrlen( hint )-2;

    // could do some more here, but I don't think it's worth it

    return 0;
}


/*!
  Returns the QTextCodec \a i places from the more recently
  inserted, or NULL if there is no such QTextCodec.  Thus,
  codecForIndex(0) returns the most recently created QTextCodec.
*/
QTextCodec* QTextCodec::codecForIndex(int i)
{
    setup();
    return (uint)i >= all->count() ? 0 : all->at(i);
}


/*!
  Returns the QTextCodec which matches the
  \link QTextCodec::mibEnum() MIBenum\endlink \a mib.
*/
QTextCodec* QTextCodec::codecForMib(int mib)
{
    setup();
    QListIterator<QTextCodec> i(*all);
    QTextCodec* result;
    for ( ; (result=i); ++i ) {
        if ( result->mibEnum()==mib )
            break;
    }
    return result;
}





#ifdef _OS_WIN32_
class QWindowsLocalCodec: public QTextCodec
{
public:
    QWindowsLocalCodec();
    ~QWindowsLocalCodec();

    QString toUnicode(const char* chars, int len) const;
    QCString fromUnicode(const QString& uc, int& lenInOut ) const;

    const char* name() const;
    int mibEnum() const;

    int heuristicContentMatch(const char* chars, int len) const;
};

QWindowsLocalCodec::QWindowsLocalCodec()
{
}

QWindowsLocalCodec::~QWindowsLocalCodec()
{
}


QString QWindowsLocalCodec::toUnicode(const char* chars, int len) const
{
    if ( len == 1 && chars ) {          // Optimization; avoids allocation
        char c[2];
        c[0] = *chars;
        c[1] = 0;
        return qt_winMB2QString( c, 2 );
    }
    if ( len < 0 )
        return qt_winMB2QString( chars );
    QCString s(chars,len+1);
    return qt_winMB2QString(s);
}

QCString QWindowsLocalCodec::fromUnicode(const QString& uc, int& lenInOut ) const
{
    QCString r = qt_winQString2MB( uc, lenInOut );
    lenInOut = r.length();
    return r;
}


const char* QWindowsLocalCodec::name() const
{
    return "System";
}

int QWindowsLocalCodec::mibEnum() const
{
    return 0;
}


int QWindowsLocalCodec::heuristicContentMatch(const char* chars, int len) const
{
    // ### Not a bad default implementation?
    QString t = toUnicode(chars,len);
    int l = t.length();
    QCString mb = fromUnicode(t,l);
    int i=0;
    while ( i < len )
        if ( chars[i] == mb[i] )
            i++;
    return i;
}

#else

/* locale names mostly copied from XFree86 */
static const char * const iso8859_2locales[] = {
    "croatian", "cs", "cs_CS", "cs_CZ","cz", "cz_CZ", "czech", "hr",
    "hr_HR", "hu", "hu_HU", "hungarian", "pl", "pl_PL", "polish", "ro",
    "ro_RO", "rumanian", "serbocroatian", "sh", "sh_SP", "sh_YU", "sk",
    "sk_SK", "sl", "sl_CS", "sl_SI", "slovak", "slovene", "sr_SP", 0 };

static const char * const iso8859_3locales[] = {
    "eo", 0 };

static const char * const iso8859_4locales[] = {
    "ee", "ee_EE", "lt", "lt_LT", "lv", "lv_LV", 0 };

static const char * const iso8859_5locales[] = {
    "bg", "bg_BG", "bulgarian", "mk", "mk_MK",
    "sp", "sp_YU", 0 };

static const char * const iso8859_6locales[] = {
    "ar_AA", "ar_SA", "arabic", 0 };

static const char * const iso8859_7locales[] = {
    "el", "el_GR", "greek", 0 };

static const char * const iso8859_8locales[] = {
    "hebrew", "he", "he_IL", "iw", "iw_IL", 0 };

static const char * const iso8859_9locales[] = {
    "tr", "tr_TR", "turkish", 0 };

static const char * const iso8859_15locales[] = {
    "fr", "fi", "french", "finnish", "et", "et_EE", 0 };

static const char * const koi8_ulocales[] = {
    "uk", "uk_UA", "ru_UA", "ukrainian", 0 };

static const char * const tis_620locales[] = {
    "th", "th_TH", "thai", 0 };


static bool try_locale_list( const char * const locale[], const char * lang )
{
    int i;
    for( i=0; locale[i] && qstrcmp(locale[i], lang); i++ )
    { }
    return locale[i] != 0;
}

// For the probably_koi8_locales we have to look. the standard says
// these are 8859-5, but almsot all Russion users uses KOI8-R and
// incorrectly set $LANG to ru_RU. We'll check tolower() to see what
// tolower() thinks ru_RU means.

// If you read the history, it seems that many Russians blame ISO and
// Peristroika for the confusion.
//
// The real bug is that some programs break if the user specifies
// ru_RU.KOI8-R.

static const char * const probably_koi8_rlocales[] = {
    "ru", "ru_SU", "ru_RU", "russian", 0 };

// this means ANY of these locale aliases. if they're aliases for
// different locales, the code breaks.
static QTextCodec * ru_RU_codec = 0;

static QTextCodec * ru_RU_hack( const char * i ) {
    if ( ! ru_RU_codec ) {
        QCString origlocale = setlocale( LC_CTYPE, i );
        // unicode   koi8r   latin5   name
        // 0x044E    0xC0    0xEE     CYRILLIC SMALL LETTER YU
        // 0x042E    0xE0    0xCE     CYRILLIC CAPITAL LETTER YU
        int latin5 = tolower( 0xCE );
        int koi8r = tolower( 0xE0 );
        if ( koi8r == 0xC0 && latin5 != 0xEE ) {
            ru_RU_codec = QTextCodec::codecForName( "KOI8-R" );
        } else if ( koi8r != 0xC0 && latin5 == 0xEE ) {
            ru_RU_codec = QTextCodec::codecForName( "ISO 8859-5" );
        } else {
            // something else again... let's assume... *throws dice*
            ru_RU_codec = QTextCodec::codecForName( "KOI8-R" );
            qWarning( "QTextCodec: using KOI8-R, probe failed (%02x %02x %s)",
                      koi8r, latin5, i );
        }
        setlocale( LC_CTYPE, origlocale.data() );
    }
    return ru_RU_codec;
}

#endif

static QTextCodec * localeMapper = 0;

void qt_set_locale_codec( QTextCodec *codec )
{
    localeMapper = codec;
}

/*!  Returns a pointer to the codec most suitable for this locale. */

QTextCodec* QTextCodec::codecForLocale()
{
    if ( localeMapper )
        return localeMapper;

    setup();

#ifdef _OS_WIN32_
    localeMapper = new QWindowsLocalCodec;
#else
    // Very poorly defined and followed standards causes lots of code
    // to try to get all the cases...

    char * lang = qstrdup( getenv("LANG") );

    char * p = lang ? strchr( lang, '.' ) : 0;
    if ( !p || *p != '.' ) {
        // Some versions of setlocale return encoding, others not.
        char *ctype = qstrdup( setlocale( LC_CTYPE, 0 ) );
        // Some Linux distributions have broken locales which will return
        // "C" for LC_CTYPE
        if ( qstrcmp( ctype, "C" ) == 0 ) {
            delete [] ctype;
        } else {
            if ( lang )
                delete [] lang;
            lang = ctype;
            p = lang ? strchr( lang, '.' ) : 0;
        }
    }

    if( p && *p == '.' ) {
        // if there is an encoding and we don't know it, we return 0
        // User knows what they are doing.  Codecs will believe them.
        localeMapper = codecForName( lang );
        if ( !localeMapper ) {
            // Use or codec disagree.
            localeMapper = codecForName( p+1 );
        }
    }
    if ( !localeMapper || !(p && *p == '.') ) {
        // if there is none, we default to 8859-1
        // We could perhaps default to 8859-15.
        if ( try_locale_list( iso8859_2locales, lang ) )
            localeMapper = codecForName( "ISO 8859-2" );
        else if ( try_locale_list( iso8859_3locales, lang ) )
            localeMapper = codecForName( "ISO 8859-3" );
        else if ( try_locale_list( iso8859_4locales, lang ) )
            localeMapper = codecForName( "ISO 8859-4" );
        else if ( try_locale_list( iso8859_5locales, lang ) )
            localeMapper = codecForName( "ISO 8859-5" );
        else if ( try_locale_list( iso8859_6locales, lang ) )
            localeMapper = codecForName( "ISO 8859-6-I" );
        else if ( try_locale_list( iso8859_7locales, lang ) )
            localeMapper = codecForName( "ISO 8859-7" );
        else if ( try_locale_list( iso8859_8locales, lang ) )
            localeMapper = codecForName( "ISO 8859-8-I" );
        else if ( try_locale_list( iso8859_9locales, lang ) )
            localeMapper = codecForName( "ISO 8859-9" );
        else if ( try_locale_list( iso8859_15locales, lang ) )
            localeMapper = codecForName( "ISO 8859-15" );
        else if ( try_locale_list( tis_620locales, lang ) )
            localeMapper = codecForName( "ISO 8859-11" );
        else if ( try_locale_list( koi8_ulocales, lang ) )
            localeMapper = codecForName( "KOI8-U" );
         else if ( try_locale_list( probably_koi8_rlocales, lang ) )
            localeMapper = ru_RU_hack( lang );
        else if (!lang || !(localeMapper = codecForName(lang) ))
            localeMapper = codecForName( "ISO 8859-1" );
    }
    delete[] lang;
#endif

    return localeMapper;
}


/*!
  Searches all installed QTextCodec objects, returning the one
  which best matches given name.  Returns NULL if no codec has
  a match closeness above \a accuracy.

  \sa heuristicNameMatch()
*/
QTextCodec* QTextCodec::codecForName(const char* hint, int accuracy)
{
    setup();
    QListIterator<QTextCodec> i(*all);
    QTextCodec* result = 0;
    int best=accuracy;
    for ( QTextCodec* cursor; (cursor=i); ++i ) {
        int s = cursor->heuristicNameMatch(hint);
        if ( s > best ) {
            best = s;
            result = cursor;
        }
    }
    return result;
}


/*!
  Searches all installed QTextCodec objects, returning the one
  which most recognizes the given content.  May return 0.

  Note that this is often a poor choice, since character
  encodings often use most of the available character sequences,
  and so only by linguistic analysis could a true match be made.

  \sa heuristicContentMatch()
*/
QTextCodec* QTextCodec::codecForContent(const char* chars, int len)
{
    setup();
    QListIterator<QTextCodec> i(*all);
    QTextCodec* result = 0;
    int best=0;
    for ( QTextCodec* cursor; (cursor=i); ++i ) {
        int s = cursor->heuristicContentMatch(chars,len);
        if ( s > best ) {
            best = s;
            result = cursor;
        }
    }
    return result;
}


/*!
  \fn const char* QTextCodec::name() const
  Subclasses of QTextCodec must reimplement this function.  It returns
  the name of the encoding supported by the subclass.  When choosing
  a name for an encoding, consider these points:
  <ul>
    <li>On X11, heuristicNameMatch( const char * hint )
        is used to test if a the QTextCodec
        can convert between Unicode and the encoding of a font
        with encoding \e hint, such as "iso8859-1" for Latin-1 fonts,
        "koi8-r" for Russian KOI8 fonts.
        The default algorithm of heuristicNameMatch() uses name().
    <li>Some applications may use this function to present
        encodings to the end user.
  </ul>
*/

/*!
  \fn int QTextCodec::mibEnum() const

  Subclasses of QTextCodec must reimplement this function.  It returns the
  MIBenum (see
  <a href="ftp://ftp.isi.edu/in-notes/iana/assignments/character-sets">
  the IANA character-sets encoding file</a> for more information).
  It is important that each QTextCodec subclass return the correct unique
  value for this function.
*/


/*!
  \fn int QTextCodec::heuristicContentMatch(const char* chars, int len) const

  Subclasses of QTextCodec must reimplement this function.  It examines
  the first \a len bytes of \a chars and returns a value indicating how
  likely it is that the string is a prefix of text encoded in the
  encoding of the subclass.  Any negative return value indicates that the text
  is detectably not in the encoding (eg. it contains undefined characters).
  A return value of 0 indicates that the text should be decoded with this
  codec rather than as ASCII, but there
  is no particular evidence.  The value should range up to \a len.  Thus,
  most decoders will return -1, 0, or -\a len.

  The characters are not null terminated.

  \sa codecForContent().
*/


/*!
  Creates a QTextDecoder which stores enough state to decode chunks
  of char* data to create chunks of Unicode data.  The default implementation
  creates a stateless decoder, which is sufficient for only the simplest
  encodings where each byte corresponds to exactly one Unicode character.

  The caller is responsible for deleting the returned object.
*/
QTextDecoder* QTextCodec::makeDecoder() const
{
    return new QTextStatelessDecoder(this);
}


/*!
  Creates a QTextEncoder which stores enough state to encode chunks
  of Unicode data as char* data.  The default implementation
  creates a stateless encoder, which is sufficient for only the simplest
  encodings where each Unicode character corresponds to exactly one char.

  The caller is responsible for deleting the returned object.
*/
QTextEncoder* QTextCodec::makeEncoder() const
{
    return new QTextStatelessEncoder(this);
}


/*!
  Subclasses of QTextCodec must reimplement this function or
  makeDecoder().  It converts the first \a len characters of \a chars
  to Unicode.

  The default implementation makes a decoder with makeDecoder() and
  converts the input with that.  Note that the default makeDecoder()
  implementation makes a decoder that simply calls
  this function, hence subclasses \e must reimplement one function or
  the other to avoid infinite recursion.
*/
QString QTextCodec::toUnicode(const char* chars, int len) const
{
    QTextDecoder* i = makeDecoder();
    QString result = i->toUnicode(chars,len);
    delete i;
    return result;
}


/*!
  Subclasses of QTextCodec must reimplement either this function or
  makeEncoder().  It converts the first \a lenInOut characters of \a
  uc from Unicode to the encoding of the subclass.  If \a lenInOut
  is negative or too large, the length of \a uc is used instead.

  The value returned is the property of the caller, which is
  responsible for deleting it with "delete []".  The length of the
  resulting Unicode character sequence is returned in \a lenInOut.

  The default implementation makes an encoder with makeEncoder() and
  converts the input with that.  Note that the default makeEncoder()
  implementation makes an encoder that simply calls
  this function, hence subclasses \e must reimplement one function or
  the other to avoid infinite recursion.
*/

QCString QTextCodec::fromUnicode(const QString& uc, int& lenInOut) const
{
    QTextEncoder* i = makeEncoder();
    QCString result = i->fromUnicode(uc, lenInOut);
    delete i;
    return result;
}

/*!
  \overload QCString QTextCodec::fromUnicode(const QString& uc) const
*/
QCString QTextCodec::fromUnicode(const QString& uc) const
{
    int l = uc.length();
    return fromUnicode(uc,l);
}

/*!
  \overload QString QTextCodec::toUnicode(const QByteArray& a, int len) const
*/
QString QTextCodec::toUnicode(const QByteArray& a, int len) const
{
    int l = a.size();
    if( l > 0 && a.data()[l - 1] == '\0' ) l--;
    l = QMIN( l, len );
    return toUnicode( a.data(), l );
}

/*!
  \overload QString QTextCodec::toUnicode(const QByteArray& a) const
*/
QString QTextCodec::toUnicode(const QByteArray& a) const
{
    int l = a.size();
    if( l > 0 && a.data()[l - 1] == '\0' ) l--;
    return toUnicode( a.data(), l );
}

/*!
  \overload QString QTextCodec::toUnicode(const char* chars) const
*/
QString QTextCodec::toUnicode(const char* chars) const
{
    return toUnicode(chars,qstrlen(chars));
}

/*!
  Returns TRUE if the unicode character \a ch can be fully encoded
  with this codec.  The default implementation tests if the result of
  toUnicode(fromUnicode(ch)) is the original \a ch. Subclasses may be
  able to improve the efficiency.
*/
bool QTextCodec::canEncode( QChar ch ) const
{
    return toUnicode(fromUnicode(ch)) == ch;
}

/*!
  Returns TRUE if the unicode string \a s can be fully encoded
  with this codec.  The default implementation tests if the result of
  toUnicode(fromUnicode(s)) is the original \a s. Subclasses may be
  able to improve the efficiency.
*/
bool QTextCodec::canEncode( const QString& s ) const
{
    return toUnicode(fromUnicode(s)) == s;
}



/*!
  \class QTextEncoder qtextcodec.h
  \brief State-based encoder

  A QTextEncoder converts Unicode into another format, remembering
  any state that is required between calls.

  \sa QTextCodec::makeEncoder()
*/

/*!
  Destructs the encoder.
*/
QTextEncoder::~QTextEncoder()
{
}
/*!
  \fn QCString QTextEncoder::fromUnicode(const QString& uc, int& lenInOut)

  Converts \a lenInOut characters (not bytes) from \a uc, producing
  a QCString.  \a lenInOut will also be set to the
  \link QCString::length() length\endlink of the result (in bytes).

  The encoder is free to record state to use when subsequent calls are
  made to this function (for example, it might change modes with escape
  sequences if needed during the encoding of one string, then assume that
  mode applies when a subsequent call begins).
*/

/*!
  \class QTextDecoder qtextcodec.h
  \brief State-based decoder

  A QTextEncoder converts a text format into Unicode, remembering
  any state that is required between calls.

  \sa QTextCodec::makeEncoder()
*/


/*!
  Destructs the decoder.
*/
QTextDecoder::~QTextDecoder()
{
}

/*!
  \fn QString QTextDecoder::toUnicode(const char* chars, int len)

  Converts the first \a len bytes at \a chars to Unicode, returning the
  result.

  If not all characters are used (eg. only part of a multi-byte
  encoding is at the end of the characters), the decoder remembers
  enough state to continue with the next call to this function.
*/

#define CHAINED 0xffff

struct QMultiByteUnicodeTable {
    // If multibyte, ignore unicode and index into multibyte
    //  with the next character.
    QMultiByteUnicodeTable() : unicode(0xfffd), multibyte(0) { }

    ~QMultiByteUnicodeTable()
    {
        if ( multibyte )
            delete [] multibyte;
    }

    ushort unicode;
    QMultiByteUnicodeTable* multibyte;
};

#ifndef QT_NO_CODECS
static int getByte(char* &cursor)
{
    int byte = 0;
    if ( *cursor ) {
        if ( cursor[1] == 'x' )
            byte = strtol(cursor+2,&cursor,16);
        else if ( cursor[1] == 'd' )
            byte = strtol(cursor+2,&cursor,10);
        else
            byte = strtol(cursor+2,&cursor,8);
    }
    return byte&0xff;
}

class QTextCodecFromIOD;

class QTextCodecFromIODDecoder : public QTextDecoder {
    const QTextCodecFromIOD* codec;
    QMultiByteUnicodeTable* mb;
public:
    QTextCodecFromIODDecoder(const QTextCodecFromIOD* c);
    QString toUnicode(const char* chars, int len);
};

class QTextCodecFromIOD : public QTextCodec {
    friend class QTextCodecFromIODDecoder;

    QCString n;

    // If from_unicode_page[row()][cell()] is 0 and from_unicode_page_multibyte,
    //  use from_unicode_page_multibyte[row()][cell()] as string.
    char** from_unicode_page;
    char*** from_unicode_page_multibyte;
    char unkn;

    // Only one of these is used
    ushort* to_unicode;
    QMultiByteUnicodeTable* to_unicode_multibyte;
    int max_bytes_per_char;
    QStrList aliases;

    bool stateless() const { return !to_unicode_multibyte; }

public:
    QTextCodecFromIOD(QIODevice* iod)
    {
        from_unicode_page = 0;
        to_unicode_multibyte = 0;
        to_unicode = 0;
        from_unicode_page_multibyte = 0;
        max_bytes_per_char = 1;

        const int maxlen=100;
        char line[maxlen];
        char esc='\\';
        char comm='%';
        bool incmap = FALSE;
        while (iod->readLine(line,maxlen) > 0) {
            if (0==qstrnicmp(line,"<code_set_name>",15))
                n = line+15;
            else if (0==qstrnicmp(line,"<escape_char> ",14))
                esc = line[14];
            else if (0==qstrnicmp(line,"<comment_char> ",15))
                comm = line[15];
            else if (line[0]==comm && 0==qstrnicmp(line+1," alias ",7)) {
                aliases.append(line+8);
            } else if (0==qstrnicmp(line,"CHARMAP",7)) {
                if (!from_unicode_page) {
                    from_unicode_page = new char*[256];
                    for (int i=0; i<256; i++)
                        from_unicode_page[i]=0;
                }
                if (!to_unicode) {
                    to_unicode = new ushort[256];
                }
                incmap = TRUE;
            } else if (0==qstrnicmp(line,"END CHARMAP",11))
                break;
            else if (incmap) {
                char* cursor = line;
                int byte=0,unicode=-1;
                ushort* mb_unicode=0;
                const int maxmb=8; // more -> we'll need to improve datastructures
                char mb[maxmb+1];
                int nmb=0;

                while (*cursor) {
                    if (cursor[0]=='<' && cursor[1]=='U' &&
                        cursor[2]>='0' && cursor[2]<='9' &&
                        cursor[3]>='0' && cursor[3]<='9') {

                        unicode = strtol(cursor+2,&cursor,16);

                    } else if (*cursor==esc) {

                        byte = getByte(cursor);

                        if ( *cursor == esc ) {
                            if ( !to_unicode_multibyte ) {
                                to_unicode_multibyte =
                                    new QMultiByteUnicodeTable[256];
                                for (int i=0; i<256; i++) {
                                    to_unicode_multibyte[i].unicode =
                                        to_unicode[i];
                                    to_unicode_multibyte[i].multibyte = 0;
                                }
                                delete [] to_unicode;
                                to_unicode = 0;
                            }
                            QMultiByteUnicodeTable* mbut =
                                to_unicode_multibyte+byte;
                            mb[nmb++] = byte;
                            while ( nmb < maxmb && *cursor == esc ) {
                                // Always at least once

                                mbut->unicode = CHAINED;
                                byte = getByte(cursor);
                                mb[nmb++] = byte;
                                if (!mbut->multibyte) {
                                    mbut->multibyte =
                                        new QMultiByteUnicodeTable[256];
                                }
                                mbut = mbut->multibyte+byte;
                                mb_unicode = & mbut->unicode;
                            }

                            if ( nmb > max_bytes_per_char )
                                max_bytes_per_char = nmb;
                        }
                    } else {
                        cursor++;
                    }
                }

                if (unicode >= 0 && unicode <= 0xffff)
                {
                    QChar ch((ushort)unicode);
                    if (!from_unicode_page[ch.row()]) {
                        from_unicode_page[ch.row()] = new char[256];
                        for (int i=0; i<256; i++)
                            from_unicode_page[ch.row()][i]=0;
                    }
                    if ( mb_unicode ) {
                        from_unicode_page[ch.row()][ch.cell()] = 0;
                        if (!from_unicode_page_multibyte) {
                            from_unicode_page_multibyte = new char**[256];
                            for (int i=0; i<256; i++)
                                from_unicode_page_multibyte[i]=0;
                        }
                        if (!from_unicode_page_multibyte[ch.row()]) {
                            from_unicode_page_multibyte[ch.row()] = new char*[256];
                            for (int i=0; i<256; i++)
                                from_unicode_page_multibyte[ch.row()][i] = 0;
                        }
                        mb[nmb++] = 0;
                        from_unicode_page_multibyte[ch.row()][ch.cell()]
                            = qstrdup(mb);
                        *mb_unicode = unicode;
                    } else {
                        from_unicode_page[ch.row()][ch.cell()] = (char)byte;
                        if ( to_unicode )
                            to_unicode[byte] = unicode;
                        else
                            to_unicode_multibyte[byte].unicode = unicode;
                    }
                } else {
                }
            }
        }
        n = n.stripWhiteSpace();

        unkn = '?'; // ##### Might be a bad choice.
    }

    ~QTextCodecFromIOD()
    {
        if ( from_unicode_page ) {
            for (int i=0; i<256; i++)
                if (from_unicode_page[i])
                    delete [] from_unicode_page[i];
        }
        if ( from_unicode_page_multibyte ) {
            for (int i=0; i<256; i++)
                if (from_unicode_page_multibyte[i])
                    for (int j=0; j<256; j++)
                        if (from_unicode_page_multibyte[i][j])
                            delete [] from_unicode_page_multibyte[i][j];
        }
        if ( to_unicode )
            delete [] to_unicode;
        if ( to_unicode_multibyte )
            delete [] to_unicode_multibyte;
    }

    bool ok() const
    {
        return !!from_unicode_page;
    }

    QTextDecoder* makeDecoder() const
    {
        if ( stateless() )
            return QTextCodec::makeDecoder();
        else
            return new QTextCodecFromIODDecoder(this);
    }

    const char* name() const
    {
        return n;
    }

    int mibEnum() const
    {
        return 0; // #### Unknown.
    }

    int heuristicContentMatch(const char*, int) const
    {
        return 0;
    }

    int heuristicNameMatch(const char* hint) const
    {
        int bestr = QTextCodec::heuristicNameMatch(hint);
        QStrListIterator it(aliases);
        char* a;
        while ((a=it.current())) {
            ++it;
            int r = simpleHeuristicNameMatch(a,hint);
            if (r > bestr)
                bestr = r;
        }
        return bestr;
    }

    QString toUnicode(const char* chars, int len) const
    {
        const uchar* uchars = (const uchar*)chars;
        QString result;
        QMultiByteUnicodeTable* multibyte=to_unicode_multibyte;
        if ( multibyte ) {
            while (len--) {
                QMultiByteUnicodeTable& mb = multibyte[*uchars];
                if ( mb.multibyte ) {
                    // Chained multi-byte
                    multibyte = mb.multibyte;
                } else {
                    result += QChar(mb.unicode);
                    multibyte=to_unicode_multibyte;
                }
                uchars++;
            }
        } else {
            while (len--)
                result += QChar(to_unicode[*uchars++]);
        }
        return result;
    }

    QCString fromUnicode(const QString& uc, int& lenInOut) const
    {
        if (lenInOut > (int)uc.length())
            lenInOut = uc.length();
        int rlen = lenInOut*max_bytes_per_char;
        QCString rstr(rlen);
        char* cursor = rstr.data();
        char* s=0;
        int l = lenInOut;
        int lout = 0;
        for (int i=0; i<l; i++) {
            QChar ch = uc[i];
            if ( ch == QChar::null ) {
                // special
                *cursor++ = 0;
            } else if ( from_unicode_page[ch.row()] &&
                from_unicode_page[ch.row()][ch.cell()] )
            {
                *cursor++ = from_unicode_page[ch.row()][ch.cell()];
                lout++;
            } else if ( from_unicode_page_multibyte &&
                      from_unicode_page_multibyte[ch.row()] &&
                      (s=from_unicode_page_multibyte[ch.row()][ch.cell()]) )
            {
                while (*s) {
                    *cursor++ = *s++;
                    lout++;
                }
            } else {
                *cursor++ = unkn;
                lout++;
            }
        }
        *cursor = 0;
        lenInOut = lout;
        return rstr;
    }
};

QTextCodecFromIODDecoder::QTextCodecFromIODDecoder(const QTextCodecFromIOD* c) :
    codec(c)
{
    mb = codec->to_unicode_multibyte;
}

QString QTextCodecFromIODDecoder::toUnicode(const char* chars, int len)
{
    const uchar* uchars = (const uchar*)chars;
    QString result;
    while (len--) {
        QMultiByteUnicodeTable& t = mb[*uchars];
        if ( t.multibyte ) {
            // Chained multi-byte
            mb = t.multibyte;
        } else {
            if ( t.unicode )
                result += QChar(t.unicode);
            mb=codec->to_unicode_multibyte;
        }
        uchars++;
    }
    return result;
}

/*!
  Reads a POSIX2 charmap definition from \a iod.
  The parser recognizes the following lines:
<pre>
   &lt;code_set_name&gt; <i>name</i>
   &lt;escape_char&gt; <i>character</i>
   % alias <i>alias</i>
   CHARMAP
   &lt;<i>token</i>&gt; /x<i>hexbyte</i> &lt;U<i>unicode</i>&gt; ...
   &lt;<i>token</i>&gt; /d<i>decbyte</i> &lt;U<i>unicode</i>&gt; ...
   &lt;<i>token</i>&gt; /<i>octbyte</i> &lt;U<i>unicode</i>&gt; ...
   &lt;<i>token</i>&gt; /<i>any</i>/<i>any</i>... &lt;U<i>unicode</i>&gt; ...
   END CHARMAP
</pre>

  The resulting QTextCodec is returned (and also added to the
  global list of codecs).  The name() of the result is taken
  from the code_set_name.

  Note that a codec constructed in this way uses much more memory
  and is slower than a hand-written QTextCodec subclass, since
  tables in code are in memory shared by all applications simultaneously
  using Qt.

  \sa loadCharmapFile()
*/
QTextCodec* QTextCodec::loadCharmap(QIODevice* iod)
{
    QTextCodecFromIOD* r = new QTextCodecFromIOD(iod);
    if ( !r->ok() ) {
        delete r;
        r = 0;
    }
    return r;
}

/*!
  A convenience function for loadCharmap().
*/
QTextCodec* QTextCodec::loadCharmapFile(QString filename)
{
    QFile f(filename);
    if (f.open(IO_ReadOnly)) {
        QTextCodecFromIOD* r = new QTextCodecFromIOD(&f);
        if ( !r->ok() )
            delete r;
        else
            return r;
    }
    return 0;
}
#endif //QT_NO_CODECS


/*!
  Returns a string representing the current language.
*/

const char* QTextCodec::locale()
{
    static QCString lang;
    if ( lang.isEmpty() ) {
        lang = getenv( "LANG" ); //########Windows??
        if ( lang.isEmpty() )
            lang = "C";
    }
    return lang;
}



#ifndef QT_NO_CODECS

class QSimpleTextCodec: public QTextCodec
{
public:
    QSimpleTextCodec( int );
    ~QSimpleTextCodec();

    QString toUnicode(const char* chars, int len) const;
    QCString fromUnicode(const QString& uc, int& lenInOut ) const;

    const char* name() const;
    int mibEnum() const;

    int heuristicContentMatch(const char* chars, int len) const;

    int heuristicNameMatch(const char* hint) const;

private:
    int forwardIndex;
};


#define LAST_MIB 2259

static struct {
    const char * cs;
    int mib;
    Q_UINT16 values[128];
} unicodevalues[] = {
    // from RFC 1489, ftp://ftp.isi.edu/in-notes/rfc1489.txt
    { "KOI8-R", 2084,
      { 0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
        0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
        0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219/**/, 0x221A, 0x2248,
        0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
        0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
        0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255D, 0x255E,
        0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
        0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0x00A9,
        0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
        0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
        0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
        0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
        0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
        0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
        0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
        0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A } },
    // /**/  - The BULLET OPERATOR is confused.  Some people think
    //          it should be 0x2022 (BULLET).

    // from RFC 2319, ftp://ftp.isi.edu/in-notes/rfc2319.txt
    { "KOI8-U", 2088,
      { 0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
	0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
	0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219, 0x221A, 0x2248,
	0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
	0x2550, 0x2551, 0x2552, 0x0451, 0x0454, 0x2554, 0x0456, 0x0457,
	0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x0491, 0x255D, 0x255E,
	0x255F, 0x2560, 0x2561, 0x0401, 0x0404, 0x2563, 0x0406, 0x0407,
	0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x0490, 0x256C, 0x00A9,
	0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
	0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
	0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
	0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
	0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
	0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
	0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
	0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A } },

    // next bits generated from tables on the Unicode 2.0 CD.  we can
    // use these tables since this is part of the transition to using
    // unicode everywhere in qt.

    // $ for A in 8 9 A B C D E F ; do for B in 0 1 2 3 4 5 6 7 8 9 A B C D E F ; do echo 0x${A}${B} 0xFFFD ; done ; done > /tmp/digits ; for a in 8859-* ; do ( awk '/^0x[89ABCDEF]/{ print $1, $2 }' < $a ; cat /tmp/digits ) | sort | uniq -w4 | cut -c6- | paste '-d ' - - - - - - - - | sed -e 's/ /, /g' -e 's/$/,/' -e '$ s/,$/} },/' -e '1 s/^/{ /' > ~/tmp/$a ; done

    // then I inserted the files manually.
    { "ISO 8859-2", 5,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
              0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
              0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
              0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
              0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
              0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
              0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
              0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
              0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
              0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
              0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
              0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9} },
    { "ISO 8859-3", 6,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0xFFFD, 0x0124, 0x00A7,
              0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0xFFFD, 0x017B,
              0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7,
              0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0xFFFD, 0x017C,
              0x00C0, 0x00C1, 0x00C2, 0xFFFD, 0x00C4, 0x010A, 0x0108, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0xFFFD, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7,
              0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0xFFFD, 0x00E4, 0x010B, 0x0109, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0xFFFD, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,
              0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9} },
    { "ISO 8859-4", 7,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7,
              0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF,
              0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7,
              0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B,
              0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
              0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A,
              0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
              0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF,
              0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
              0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B,
              0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
              0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9} },
    { "ISO 8859-5", 8,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
              0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
              0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
              0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
              0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
              0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
              0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
              0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
              0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
              0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
              0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
              0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F} },
    { "ISO 8859-6-I", 82,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0xFFFD, 0xFFFD, 0xFFFD, 0x00A4, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x060C, 0x00AD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0x061B, 0xFFFD, 0xFFFD, 0xFFFD, 0x061F,
              0xFFFD, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
              0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
              0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
              0x0638, 0x0639, 0x063A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
              0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
              0x0650, 0x0651, 0x0652, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD} },
    { "ISO 8859-7", 10,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x2018, 0x2019, 0x00A3, 0xFFFD, 0xFFFD, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0xFFFD, 0x00AB, 0x00AC, 0x00AD, 0xFFFD, 0x2015,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x00B7,
              0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
              0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
              0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
              0x03A0, 0x03A1, 0xFFFD, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
              0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
              0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
              0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
              0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
              0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0xFFFD} },
    { "ISO 8859-8-I", 85,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0xFFFD, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x203E,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x2017,
              0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
              0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
              0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
              0x05E8, 0x05E9, 0x05EA, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD} },
    { "ISO 8859-9", 12,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
              0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF} },
    { "ISO 8859-10", 13,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7,
              0x013B, 0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A,
              0x00B0, 0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7,
              0x013C, 0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B,
              0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
              0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF,
              0x00D0, 0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168,
              0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
              0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
              0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF,
              0x00F0, 0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169,
              0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138} },
    { "ISO 8859-13", 109,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7,
              0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7,
              0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
              0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
              0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
              0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
              0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
              0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
              0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
              0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
              0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019} },
    { "ISO 8859-14", 110,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7,
              0x1E80, 0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
              0x1E1E, 0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56,
              0x1E81, 0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61,
              0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x1E6A,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x1E6B,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF} },
    { "ISO 8859-15", 111,
        { 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
              0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
              0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
              0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7,
              0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7,
              0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
              0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF} },

    // next bits generated again from tables on the Unicode 3.0 CD.

    // $ for a in CP* ; do ( awk '/^0x[89ABCDEF]/{ print $1, $2 }' < $a ) | sort | sed -e 's/#UNDEF.*$/0xFFFD/' | cut -c6- | paste '-d ' - - - - - - - - | sed -e 's/ /, /g' -e 's/$/,/' -e '$ s/,$/} },/' -e '1 s/^/{ /' > ~/tmp/$a ; done

    { "CP 874", 0, //### what is the mib?
        { 0x20AC, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x2026, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07,
              0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
              0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17,
              0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
              0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27,
              0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
              0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
              0x0E38, 0x0E39, 0x0E3A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x0E3F,
              0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47,
              0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
              0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57,
              0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD} },
    { "CP 1250", 2250,
        { 0x20AC, 0xFFFD, 0x201A, 0xFFFD, 0x201E, 0x2026, 0x2020, 0x2021,
              0xFFFD, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0xFFFD, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
              0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
              0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
              0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
              0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
              0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
              0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
              0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
              0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
              0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
              0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9} },
    { "CP 1251", 2251,
        { 0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
              0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
              0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0xFFFD, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
              0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
              0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
              0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
              0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
              0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
              0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
              0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
              0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
              0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
              0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
              0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
              0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F} },
    { "CP 1252", 2252,
        { 0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xFFFD, 0x017D, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xFFFD, 0x017E, 0x0178,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
              0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF} },
    { "CP 1253", 2253,
        { 0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0xFFFD, 0x2030, 0xFFFD, 0x2039, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0xFFFD, 0x2122, 0xFFFD, 0x203A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0x00A0, 0x0385, 0x0386, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0xFFFD, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x2015,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x00B5, 0x00B6, 0x00B7,
              0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
              0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
              0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
              0x03A0, 0x03A1, 0xFFFD, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
              0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
              0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
              0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
              0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
              0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0xFFFD} },
    { "CP 1254", 2254,
        { 0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xFFFD, 0xFFFD, 0x0178,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
              0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
              0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
              0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF} },
    { "CP 1255", 2255,
        { 0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0x02C6, 0x2030, 0xFFFD, 0x2039, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0x02DC, 0x2122, 0xFFFD, 0x203A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AA, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
              0x05B0, 0x05B1, 0x05B2, 0x05B3, 0x05B4, 0x05B5, 0x05B6, 0x05B7,
              0x05B8, 0x05B9, 0xFFFD, 0x05BB, 0x05BC, 0x05BD, 0x05BE, 0x05BF,
              0x05C0, 0x05C1, 0x05C2, 0x05C3, 0x05F0, 0x05F1, 0x05F2, 0x05F3,
              0x05F4, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
              0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
              0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
              0x05E8, 0x05E9, 0x05EA, 0xFFFD, 0xFFFD, 0x200E, 0x200F, 0xFFFD} },
    { "CP 1256", 2256,
        { 0x20AC, 0x067E, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0x02C6, 0x2030, 0x0679, 0x2039, 0x0152, 0x0686, 0x0698, 0x0688,
              0x06AF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0x06A9, 0x2122, 0x0691, 0x203A, 0x0153, 0x200C, 0x200D, 0x06BA,
              0x00A0, 0x060C, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x06BE, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x061B, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x061F,
              0x06C1, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
              0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
              0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00D7,
              0x0637, 0x0638, 0x0639, 0x063A, 0x0640, 0x0641, 0x0642, 0x0643,
              0x00E0, 0x0644, 0x00E2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0649, 0x064A, 0x00EE, 0x00EF,
              0x064B, 0x064C, 0x064D, 0x064E, 0x00F4, 0x064F, 0x0650, 0x00F7,
              0x0651, 0x00F9, 0x0652, 0x00FB, 0x00FC, 0x200E, 0x200F, 0x06D2} },
    { "CP 1257", 2257,
        { 0x20AC, 0xFFFD, 0x201A, 0xFFFD, 0x201E, 0x2026, 0x2020, 0x2021,
              0xFFFD, 0x2030, 0xFFFD, 0x2039, 0xFFFD, 0x00A8, 0x02C7, 0x00B8,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0xFFFD, 0x2122, 0xFFFD, 0x203A, 0xFFFD, 0x00AF, 0x02DB, 0xFFFD,
              0x00A0, 0xFFFD, 0x00A2, 0x00A3, 0x00A4, 0xFFFD, 0x00A6, 0x00A7,
              0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
              0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
              0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
              0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
              0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
              0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
              0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
              0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
              0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x02D9} },
    { "CP 1258", 2258,
        { 0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
              0x02C6, 0x2030, 0xFFFD, 0x2039, 0x0152, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
              0x02DC, 0x2122, 0xFFFD, 0x203A, 0x0153, 0xFFFD, 0xFFFD, 0x0178,
              0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
              0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
              0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
              0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
              0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
              0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x0300, 0x00CD, 0x00CE, 0x00CF,
              0x0110, 0x00D1, 0x0309, 0x00D3, 0x00D4, 0x01A0, 0x00D6, 0x00D7,
              0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x01AF, 0x0303, 0x00DF,
              0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
              0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0301, 0x00ED, 0x00EE, 0x00EF,
              0x0111, 0x00F1, 0x0323, 0x00F3, 0x00F4, 0x01A1, 0x00F6, 0x00F7,
              0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x01B0, 0x20AB, 0x00FF} },

    // this one is generated from the charmap file located in /usr/share/i18n/charmaps
    // on most Linux distributions. The thai character set tis620 is byte by byte equivalent
    // to iso8859-11, so we name it 8859-11 here, but recognise the name tis620 too.

    // $ for A in 8 9 A B C D E F ; do for B in 0 1 2 3 4 5 6 7 8 9 A B C D E F ; do echo x${A}${B} 0xFFFD ; done ; done > /tmp/digits ; ( cut -c25- < TIS-620 ; cat /tmp/digits ) | awk '/^x[89ABCDEF]/{ print $1, $2 }' | sed -e 's/<U/0x/' -e 's/>//' | sort | uniq -w4 | cut -c5- | paste '-d ' - - - - - - - - | sed -e 's/ /, /g' -e 's/$/,/' -e '$ s/,$/} },/' -e '1 s/^/{ /' > ~/tmp/tis-620
    { "ISO 8859-11", 2259, // Thai character set mib enum taken from tis620 (which is byte by byte equivalent)
        { 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
              0xFFFD, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07,
              0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
              0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17,
              0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
              0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27,
              0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
              0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
              0x0E38, 0x0E39, 0x0E3A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x0E3F,
              0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47,
              0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
              0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57,
              0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD} },

    // change LAST_MIB if you add more, and edit unicodevalues in
    // kernel/qpsprinter.cpp too.
};


static const QSimpleTextCodec * reverseOwner = 0;
static QArray<char> * reverseMap = 0;


QSimpleTextCodec::QSimpleTextCodec( int i )
    : QTextCodec(), forwardIndex( i )
{
}


QSimpleTextCodec::~QSimpleTextCodec()
{
    if ( reverseOwner == this ) {
        delete reverseMap;
        reverseMap = 0;
        reverseOwner = 0;
    }
}

// what happens if strlen(chars)<len?  what happens if !chars?  if len<1?
QString QSimpleTextCodec::toUnicode(const char* chars, int len) const
{
    if(len <= 0)
        return QString::null;

    int clen = qstrlen(chars);
    len = QMIN(len, clen); // Note: NUL ends string

    QString r;
    r.setUnicode(0, len);
    QChar* uc = (QChar*)r.unicode(); // const_cast
    const unsigned char * c = (const unsigned char *)chars;
    for( int i=0; i<len; i++ ) {
        if ( c[i] > 127 )
            uc[i] = unicodevalues[forwardIndex].values[c[i]-128];
        else
            uc[i] = c[i];
    }
    return r;
}


QCString QSimpleTextCodec::fromUnicode(const QString& uc, int& len ) const
{
    if ( reverseOwner != this ) {
        int m = 0;
        int i = 0;
        while( i < 128 ) {
            if ( unicodevalues[forwardIndex].values[i] > m &&
                 unicodevalues[forwardIndex].values[i] < 0xfffd )
                m = unicodevalues[forwardIndex].values[i];
            i++;
        }
        m++;
        if ( !reverseMap )
            reverseMap = new QArray<char>( m );
        if ( m > (int)(reverseMap->size()) )
            reverseMap->resize( m );
        for( i = 0; i < 128 && i < m; i++ )
            (*reverseMap)[i] = (char)i;
        for( ;i < m; i++ )
            (*reverseMap)[i] = '?';
        for( i=128; i<256; i++ ) {
            int u = unicodevalues[forwardIndex].values[i-128];
            if ( u < m )
                (*reverseMap)[u] = (char)(unsigned char)(i);
        }
        reverseOwner = this;
    }
    if ( len <0 || len > (int)uc.length() )
        len = uc.length();
    QCString r( len+1 );
    int i = len;
    int u;
    const QChar* ucp = uc.unicode();
    char* rp = r.data();
    char* rmp = reverseMap->data();
    int rmsize = (int) reverseMap->size();
    while( i-- )
    {
        u = ucp->unicode();
        *rp++ = u < 128 ? u : (( u < rmsize ) ? (*(rmp+u)) : '?' );
        ucp++;
    }
    r[len] = 0;
    return r;
}


const char* QSimpleTextCodec::name() const
{
    return unicodevalues[forwardIndex].cs;
}


int QSimpleTextCodec::mibEnum() const
{
    return unicodevalues[forwardIndex].mib;
}

int QSimpleTextCodec::heuristicNameMatch(const char* hint) const
{
    if ( hint[0]=='k' ) {
        // Help people with messy fonts
        if ( QCString(hint) == "koi8-1" )
            return QTextCodec::heuristicNameMatch("koi8-r")-1;
        if ( QCString(hint) == "koi8-ru" )
            return QTextCodec::heuristicNameMatch("koi8-r")-1;
    } else if ( hint[0] == 't' && QCString(name()) == "ISO 8859-11" ) {
	// 8859-11 and tis620 are byte by bute equivalent
	int i = simpleHeuristicNameMatch("tis620-0", hint);
	if( !i )
	    i = simpleHeuristicNameMatch("tis-620", hint);
	if( i ) return i;
    }
    return QTextCodec::heuristicNameMatch(hint);
}

int QSimpleTextCodec::heuristicContentMatch(const char* chars, int len) const
{
    if ( len<1 || !chars )
        return -1;
    int i = 0;
    const uchar * c = (const unsigned char *)chars;
    int r = 0;
    while( i<len && c && *c ) {
        if ( *c >= 128 ) {
            if ( unicodevalues[forwardIndex].values[(*c)-128] == 0xfffd )
                return -1;
        }
        if ( (*c >= ' ' && *c < 127) ||
             *c == '\n' || *c == '\t' || *c == '\r' )
            r++;
        i++;
        c++;
    }
    if ( mibEnum()==4 )
        r+=1;
    return r;
}


#endif // QT_NO_CODECS

class QLatin1Codec: public QTextCodec
{
public:
    QLatin1Codec();
    ~QLatin1Codec();

    QString toUnicode(const char* chars, int len) const;
    QCString fromUnicode(const QString& uc, int& lenInOut ) const;

    const char* name() const;
    int mibEnum() const;

    int heuristicContentMatch(const char* chars, int len) const;

    int heuristicNameMatch(const char* hint) const;

private:
    int forwardIndex;
};


QLatin1Codec::QLatin1Codec()
    : QTextCodec()
{
}


QLatin1Codec::~QLatin1Codec()
{
}

// what happens if strlen(chars)<len?  what happens if !chars?  if len<1?
QString QLatin1Codec::toUnicode(const char* chars, int len) const
{
    if(len <= 0)
        return QString::null;

    return QString::fromLatin1(chars, len);
}


QCString QLatin1Codec::fromUnicode(const QString& uc, int& len ) const
{
    if ( len <0 || len > (int)uc.length() )
        len = uc.length();
    QCString r( len+1 );
    int i = 0;
    const QChar *ch = uc.unicode();
    while ( i < len ) {
	r[i] = ch->row() ? '?' : ch->cell();
	i++;
	ch++;
    }
    r[len] = 0;
    return r;
}


const char* QLatin1Codec::name() const
{
    return "ISO 8859-1";
}


int QLatin1Codec::mibEnum() const
{
    return 4;
}

int QLatin1Codec::heuristicNameMatch(const char* hint) const
{
    return QTextCodec::heuristicNameMatch(hint);
}

int QLatin1Codec::heuristicContentMatch(const char* chars, int len) const
{
    if ( len<1 || !chars )
        return -1;
    int i = 0;
    const uchar * c = (const unsigned char *)chars;
    int r = 0;
    while( i<len && c && *c ) {
        if ( *c >= 0x80 && *c < 0xa0 )
            return -1;
        if ( (*c >= ' ' && *c < 127) ||
             *c == '\n' || *c == '\t' || *c == '\r' )
            r++;
        i++;
        c++;
    }
    return r;
}


static void setupBuiltinCodecs()
{
    (void)new QLatin1Codec;

#ifndef QT_NO_CODECS
    int i = 0;
    do {
        (void)new QSimpleTextCodec( i );
    } while( unicodevalues[i++].mib != LAST_MIB );

    //(void)new QEucJpCodec;
    //(void)new QSjisCodec;
    //(void)new QJisCodec;
    //(void)new QEucKrCodec;
    //(void)new QGbkCodec;
    //(void)new QBig5Codec;
    (void)new QUtf8Codec;
    (void)new QUtf16Codec;
    //(void)new QHebrewCodec;
    //(void)new QArabicCodec;
    //(void)new QTsciiCodec;
#endif // QT_NO_CODECS
}

#endif // QT_NO_TEXTCODEC
