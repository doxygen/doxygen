/****************************************************************************
** 
**
** Definition of QXmlSimpleReader and related classes.
**
** Created : 000518
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the XML module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#ifndef QXML_H
#define QXML_H

#include <qmodules.h>

#if !defined(QT_MODULE_XML)
#define QM_EXPORT
#else
#define QM_EXPORT Q_EXPORT
#endif

#ifndef QT_H
#include <qtextstream.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluestack.h>
#include <qmap.h>
#endif // QT_H

#ifndef QT_NO_XML

class QXmlNamespaceSupport;
class QXmlAttributes;
class QXmlContentHandler;
class QXmlDefaultHandler;
class QXmlDTDHandler;
class QXmlEntityResolver;
class QXmlErrorHandler;
class QXmlLexicalHandler;
class QXmlDeclHandler;
class QXmlInputSource;
class QXmlLocator;
class QXmlNamespaceSupport;
class QXmlParseException;

class QXmlReader;
class QXmlSimpleReader;

class QXmlSimpleReaderPrivate;
class QXmlNamespaceSupportPrivate;
class QXmlAttributesPrivate;
class QXmlInputSourcePrivate;
class QXmlParseExceptionPrivate;
class QXmlLocatorPrivate;
class QXmlDefaultHandlerPrivate;


//
// SAX Namespace Support
//

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QM_EXPORT QMap<QString, QString>;
template class QM_EXPORT QValueStack<QMap<QString, QString> >;
template class QM_EXPORT QValueStack<QString>;
// MOC_SKIP_END
#endif

class QM_EXPORT QXmlNamespaceSupport
{
public:
    QXmlNamespaceSupport();
    ~QXmlNamespaceSupport();

    void setPrefix( const QString&, const QString& );

    QString prefix( const QString& ) const;
    QString uri( const QString& ) const;
    void splitName( const QString&, QString&, QString& ) const;
    void processName( const QString&, bool, QString&, QString& ) const;
    QStringList prefixes() const;
    QStringList prefixes( const QString& ) const;

    void pushContext();
    void popContext();
    void reset();
private:
    QValueStack<QMap<QString, QString> > nsStack;
    QMap<QString, QString> ns;

    QXmlNamespaceSupportPrivate *d;
};


//
// SAX Attributes
//

class QM_EXPORT QXmlAttributes
{
public:
    QXmlAttributes() {}
    virtual ~QXmlAttributes() {}

    int index( const QString& qName ) const;
    int index( const QString& uri, const QString& localPart ) const;
    int length() const;
    QString localName( int index ) const;
    QString qName( int index ) const;
    QString uri( int index ) const;
    QString type( int index ) const;
    QString type( const QString& qName ) const;
    QString type( const QString& uri, const QString& localName ) const;
    QString value( int index ) const;
    QString value( const QString& qName ) const;
    QString value( const QString& uri, const QString& localName ) const;

private:
    QStringList qnameList;
    QStringList uriList;
    QStringList localnameList;
    QStringList valueList;

    QXmlAttributesPrivate *d;

    friend class QXmlSimpleReader;
};

//
// SAX Input Source
//

class QM_EXPORT QXmlInputSource
{
public:
    QXmlInputSource();
    QXmlInputSource( QTextStream& stream );
    QXmlInputSource( QFile& file );
    virtual ~QXmlInputSource();

    virtual const QString& data() const;
    virtual void setData( const QString& d );

private:
    void readInput( QByteArray& rawData );

    QString input;

    QXmlInputSourcePrivate *d;
};

//
// SAX Exception Classes
//

class QM_EXPORT QXmlParseException
{
public:
    QXmlParseException( const QString& name="", int c=-1, int l=-1, const QString& p="", const QString& s="" )
	: msg( name ), column( c ), line( l ), pub( p ), sys( s )
    { }

    int columnNumber() const;
    int lineNumber() const;
    QString publicId() const;
    QString systemId() const;
    QString message() const;

private:
    QString msg;
    int column;
    int line;
    QString pub;
    QString sys;

    QXmlParseExceptionPrivate *d;
};


//
// XML Reader
//

class QM_EXPORT QXmlReader
{
public:
    virtual ~QXmlReader() {}
    virtual bool feature( const QString& name, bool *ok = 0 ) const = 0;
    virtual void setFeature( const QString& name, bool value ) = 0;
    virtual bool hasFeature( const QString& name ) const = 0;
    virtual void* property( const QString& name, bool *ok = 0 ) const = 0;
    virtual void setProperty( const QString& name, void* value ) = 0;
    virtual bool hasProperty( const QString& name ) const = 0;
    virtual void setEntityResolver( QXmlEntityResolver* handler ) = 0;
    virtual QXmlEntityResolver* entityResolver() const = 0;
    virtual void setDTDHandler( QXmlDTDHandler* handler ) = 0;
    virtual QXmlDTDHandler* DTDHandler() const = 0;
    virtual void setContentHandler( QXmlContentHandler* handler ) = 0;
    virtual QXmlContentHandler* contentHandler() const = 0;
    virtual void setErrorHandler( QXmlErrorHandler* handler ) = 0;
    virtual QXmlErrorHandler* errorHandler() const = 0;
    virtual void setLexicalHandler( QXmlLexicalHandler* handler ) = 0;
    virtual QXmlLexicalHandler* lexicalHandler() const = 0;
    virtual void setDeclHandler( QXmlDeclHandler* handler ) = 0;
    virtual QXmlDeclHandler* declHandler() const = 0;
    virtual bool parse( const QXmlInputSource& input ) = 0;
};

class QM_EXPORT QXmlSimpleReader : public QXmlReader
{
public:
    QXmlSimpleReader();
    virtual ~QXmlSimpleReader();

    bool feature( const QString& name, bool *ok = 0 ) const;
    void setFeature( const QString& name, bool value );
    bool hasFeature( const QString& name ) const;

    void* property( const QString& name, bool *ok = 0 ) const;
    void setProperty( const QString& name, void* value );
    bool hasProperty( const QString& name ) const;

    void setEntityResolver( QXmlEntityResolver* handler );
    QXmlEntityResolver* entityResolver() const;
    void setDTDHandler( QXmlDTDHandler* handler );
    QXmlDTDHandler* DTDHandler() const;
    void setContentHandler( QXmlContentHandler* handler );
    QXmlContentHandler* contentHandler() const;
    void setErrorHandler( QXmlErrorHandler* handler );
    QXmlErrorHandler* errorHandler() const;
    void setLexicalHandler( QXmlLexicalHandler* handler );
    QXmlLexicalHandler* lexicalHandler() const;
    void setDeclHandler( QXmlDeclHandler* handler );
    QXmlDeclHandler* declHandler() const;

    bool parse( const QXmlInputSource& input );

private:
    // variables
    QXmlContentHandler* contentHnd;
    QXmlErrorHandler*   errorHnd;
    QXmlDTDHandler*     dtdHnd;
    QXmlEntityResolver* entityRes;
    QXmlLexicalHandler* lexicalHnd;
    QXmlDeclHandler*    declHnd;

    QChar c; // the character at reading position
    int lineNr; // number of line
    int columnNr; // position in line
    int pos; // position in string

    int     namePos;
    QChar   nameArray[256]; // only used for names
    QString nameValue; // only used for names
    int     refPos;
    QChar   refArray[256]; // only used for references
    QString refValue; // only used for references
    int     stringPos;
    QChar   stringArray[256]; // used for any other strings that are parsed
    QString stringValue; // used for any other strings that are parsed

    QString xml;
    int xmlLength;
    QString xmlRef; // used for parsing of entity references

    QValueStack<QString> tags;

    QXmlSimpleReaderPrivate* d;

    static const QChar QEOF;

    // inlines
    virtual bool is_S( const QChar& );
    virtual bool is_Letter( const QChar& );
    virtual bool is_NameBeginning( const QChar& );
    virtual bool is_Digit( const QChar& );
    virtual bool is_CombiningChar( const QChar& );
    virtual bool is_Extender( const QChar& );
    virtual bool is_NameChar( const QChar& );

    QString& string();
    void stringClear();
    void stringAddC();
    void stringAddC(const QChar&);
    QString& name();
    void nameClear();
    void nameAddC();
    void nameAddC(const QChar&);
    QString& ref();
    void refClear();
    void refAddC();
    void refAddC(const QChar&);

    // used by parseReference() and parsePEReference()
    enum EntityRecognitionContext { InContent, InAttributeValue, InEntityValue, InDTD };

    // private functions
    void eat_ws();
    void next_eat_ws();

    void next();
    bool atEnd();

    void init( const QXmlInputSource& i );

    bool entityExist( const QString& ) const;

    bool parseProlog();
    bool parseElement();
    bool parseElementEmptyTag( bool &t, QString &uri, QString &lname );
    bool parseElementETagBegin2();
    bool parseElementAttribute( QString &prefix, QString &uri, QString &lname );
    bool parseMisc();
    bool parseContent();

    bool parsePI(bool xmldecl=FALSE);
    bool parseDoctype();
    bool parseComment();

    bool parseName( bool useRef=FALSE );
    bool parseNmtoken();
    bool parseAttribute();
    bool parseReference( bool &charDataRead, EntityRecognitionContext context );
    bool processReference( bool &charDataRead, EntityRecognitionContext context );

    bool parseExternalID( bool allowPublicID = FALSE );
    bool parsePEReference( EntityRecognitionContext context );
    bool parseMarkupdecl();
    bool parseAttlistDecl();
    bool parseAttType();
    bool parseAttValue();
    bool parseElementDecl();
    bool parseNotationDecl();
    bool parseChoiceSeq();
    bool parseEntityDecl();
    bool parseEntityValue();

    bool parseString( const QString& s );

    void reportParseError();

    friend class QXmlSimpleReaderPrivate;
    friend class QXmlLocator;
};

//
// SAX Locator
//

class QM_EXPORT QXmlLocator
{
public:
    QXmlLocator( QXmlSimpleReader* parent )
    { reader = parent; }
    ~QXmlLocator()
    { }

    int columnNumber();
    int lineNumber();
//    QString getPublicId()
//    QString getSystemId()

private:
    QXmlSimpleReader* reader;

    QXmlLocatorPrivate *d;
};

//
// SAX handler classes
//

class QM_EXPORT QXmlContentHandler
{
public:
    virtual ~QXmlContentHandler() {}
    virtual void setDocumentLocator( QXmlLocator* locator ) = 0;
    virtual bool startDocument() = 0;
    virtual bool endDocument() = 0;
    virtual bool startPrefixMapping( const QString& prefix, const QString& uri ) = 0;
    virtual bool endPrefixMapping( const QString& prefix ) = 0;
    virtual bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts ) = 0;
    virtual bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName ) = 0;
    virtual bool characters( const QString& ch ) = 0;
    virtual bool ignorableWhitespace( const QString& ch ) = 0;
    virtual bool processingInstruction( const QString& target, const QString& data ) = 0;
    virtual bool skippedEntity( const QString& name ) = 0;
    virtual QString errorString() = 0;
};

class QM_EXPORT QXmlErrorHandler
{
public:
    virtual ~QXmlErrorHandler() {}
    virtual bool warning( const QXmlParseException& exception ) = 0;
    virtual bool error( const QXmlParseException& exception ) = 0;
    virtual bool fatalError( const QXmlParseException& exception ) = 0;
    virtual QString errorString() = 0;
};

class QM_EXPORT QXmlDTDHandler
{
public:
    virtual ~QXmlDTDHandler() {}
    virtual bool notationDecl( const QString& name, const QString& publicId, const QString& systemId ) = 0;
    virtual bool unparsedEntityDecl( const QString& name, const QString& publicId, const QString& systemId, const QString& notationName ) = 0;
    virtual QString errorString() = 0;
};

class QM_EXPORT QXmlEntityResolver
{
public:
    virtual ~QXmlEntityResolver() {}
    virtual bool resolveEntity( const QString& publicId, const QString& systemId, QXmlInputSource* ret ) = 0;
    virtual QString errorString() = 0;
};

class QM_EXPORT QXmlLexicalHandler
{
public:
    virtual ~QXmlLexicalHandler() {}
    virtual bool startDTD( const QString& name, const QString& publicId, const QString& systemId ) = 0;
    virtual bool endDTD() = 0;
//    virtual bool startEntity( const QString& name ) = 0;
//    virtual bool endEntity( const QString& name ) = 0;
    virtual bool startCDATA() = 0;
    virtual bool endCDATA() = 0;
    virtual bool comment( const QString& ch ) = 0;
    virtual QString errorString() = 0;
};

class QM_EXPORT QXmlDeclHandler
{
public:
    virtual ~QXmlDeclHandler() {}
    virtual bool attributeDecl( const QString& eName, const QString& aName, const QString& type, const QString& valueDefault, const QString& value ) = 0;
    virtual bool internalEntityDecl( const QString& name, const QString& value ) = 0;
    virtual bool externalEntityDecl( const QString& name, const QString& publicId, const QString& systemId ) = 0;
    virtual QString errorString() = 0;
};


class QM_EXPORT QXmlDefaultHandler : public QXmlContentHandler, public QXmlErrorHandler, public QXmlDTDHandler, public QXmlEntityResolver, public QXmlLexicalHandler, public QXmlDeclHandler
{
public:
    QXmlDefaultHandler() { }
    virtual ~QXmlDefaultHandler() { }

    void setDocumentLocator( QXmlLocator* locator );
    bool startDocument();
    bool endDocument();
    bool startPrefixMapping( const QString& prefix, const QString& uri );
    bool endPrefixMapping( const QString& prefix );
    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );
    bool ignorableWhitespace( const QString& ch );
    bool processingInstruction( const QString& target, const QString& data );
    bool skippedEntity( const QString& name );

    bool warning( const QXmlParseException& exception );
    bool error( const QXmlParseException& exception );
    bool fatalError( const QXmlParseException& exception );

    bool notationDecl( const QString& name, const QString& publicId, const QString& systemId );
    bool unparsedEntityDecl( const QString& name, const QString& publicId, const QString& systemId, const QString& notationName );

    bool resolveEntity( const QString& publicId, const QString& systemId, QXmlInputSource* ret );

    bool startDTD( const QString& name, const QString& publicId, const QString& systemId );
    bool endDTD();
//    bool startEntity( const QString& name );
//    bool endEntity( const QString& name );
    bool startCDATA();
    bool endCDATA();
    bool comment( const QString& ch );

    bool attributeDecl( const QString& eName, const QString& aName, const QString& type, const QString& valueDefault, const QString& value );
    bool internalEntityDecl( const QString& name, const QString& value );
    bool externalEntityDecl( const QString& name, const QString& publicId, const QString& systemId );

    QString errorString();

private:
    QXmlDefaultHandlerPrivate *d;
};

#ifdef _WS_QWS_
#ifdef QT_XML_CPP
#define inline
#else
#define QT_NO_XML_INLINE
#endif
#endif

#ifndef QT_NO_XML_INLINE
//
// inlines
//

inline bool QXmlSimpleReader::is_S(const QChar& ch)
{ return ch==' ' || ch=='\t' || ch=='\n' || ch=='\r'; }

inline bool QXmlSimpleReader::is_Letter( const QChar& ch )
{ return ch.isLetter(); }

inline bool QXmlSimpleReader::is_NameBeginning( const QChar& ch )
{ return ch=='_' || ch==':' || ch.isLetter(); }

inline bool QXmlSimpleReader::is_Digit( const QChar& ch )
{ return ch.isDigit(); }

inline bool QXmlSimpleReader::is_CombiningChar( const QChar& )
{ return FALSE; }

inline bool QXmlSimpleReader::is_Extender( const QChar& )
{ return FALSE; }

inline bool QXmlSimpleReader::is_NameChar( const QChar& ch )
{
    return ch=='.' || ch=='-' || ch=='_' || ch==':' ||
	is_Letter(ch) || is_Digit(ch) ||
	is_CombiningChar(ch) || is_Extender(ch);
}

inline void QXmlSimpleReader::next()
{
    if ( !xmlRef.isEmpty() ) {
	c = xmlRef[0];
	xmlRef.remove( 0, 1 );
    } else {
	if ( c=='\n' || c=='\r' ) {
	    lineNr++;
	    columnNr = -1;
	}
	if ( pos >= xmlLength ) {
	    c = QEOF;
	} else {
	    c = xml[pos];
	    columnNr++;
	    pos++;
	}
    }
}

inline bool QXmlSimpleReader::atEnd()
{ return c == QEOF; }

inline void QXmlSimpleReader::eat_ws()
{ while ( !atEnd() && is_S(c) ) next(); }

inline void QXmlSimpleReader::next_eat_ws()
{ next(); eat_ws(); }


// use buffers instead of QString::operator+= when single characters are read
inline QString& QXmlSimpleReader::string()
{
    stringValue += QString( stringArray, stringPos );
    stringPos = 0;
    return stringValue;
}
inline QString& QXmlSimpleReader::name()
{
    nameValue += QString( nameArray, namePos );
    namePos = 0;
    return nameValue;
}
inline QString& QXmlSimpleReader::ref()
{
    refValue += QString( refArray, refPos );
    refPos = 0;
    return refValue;
}

inline void QXmlSimpleReader::stringClear()
{ stringValue = ""; stringPos = 0; }
inline void QXmlSimpleReader::nameClear()
{ nameValue = ""; namePos = 0; }
inline void QXmlSimpleReader::refClear()
{ refValue = ""; refPos = 0; }

inline void QXmlSimpleReader::stringAddC()
{
    if ( stringPos >= 256 ) {
	stringValue += QString( stringArray, stringPos );
	stringPos = 0;
    }
    stringArray[stringPos++] = c;
}
inline void QXmlSimpleReader::nameAddC()
{
    if ( namePos >= 256 ) {
	nameValue += QString( nameArray, namePos );
	namePos = 0;
    }
    nameArray[namePos++] = c;
}
inline void QXmlSimpleReader::refAddC()
{
    if ( refPos >= 256 ) {
	refValue += QString( refArray, refPos );
	refPos = 0;
    }
    refArray[refPos++] = c;
}

inline void QXmlSimpleReader::stringAddC(const QChar& ch)
{
    if ( stringPos >= 256 ) {
	stringValue += QString( stringArray, stringPos );
	stringPos = 0;
    }
    stringArray[stringPos++] = ch;
}
inline void QXmlSimpleReader::nameAddC(const QChar& ch)
{
    if ( namePos >= 256 ) {
	nameValue += QString( nameArray, namePos );
	namePos = 0;
    }
    nameArray[namePos++] = ch;
}
inline void QXmlSimpleReader::refAddC(const QChar& ch)
{
    if ( refPos >= 256 ) {
	refValue += QString( refArray, refPos );
	refPos = 0;
    }
    refArray[refPos++] = ch;
}
#endif

#ifdef _WS_QWS_
#ifdef QT_XML_CPP
#undef inline
#endif
#endif

#endif //QT_NO_XML

#endif
