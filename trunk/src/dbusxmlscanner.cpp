/******************************************************************************
 *
 *
 *
 * Copyright (C) 2009 by Tobias Hunger <tobias@aquazul.com>
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

#include "dbusxmlscanner.h"

#include "commentscan.h"
#include "entry.h"

#include <qfile.h>
#include <qxml.h>
#include <qstring.h>

#include "message.h"
#include "util.h"

// -----------------------------------------------------------------------
// Convenience defines:
// -----------------------------------------------------------------------

#define CONDITION(cond, msg) \
    do {\
        if (cond)\
        {\
            if (m_errorString.isEmpty()) { m_errorString = msg; }\
            return false;\
        }\
    }\
    while (0)

#define DOC_ERROR(msg) \
    warn_doc_error(m_fileName.utf8().data(), lineNumber(), msg.utf8().data())

#define COND_DOC_ERROR(cond, msg) \
    do {\
        if (cond)\
        {\
             DOC_ERROR(msg);\
             return true;\
        }\
    }\
    while (0)

#define DBUS(name) isDBusElement(namespaceURI, localName, qName, name)
#define EXTENSION(name) isExtensionElement(namespaceURI, localName, qName, name)

// -----------------------------------------------------------------------
// DBusXMLHandler class
// -----------------------------------------------------------------------

const QString EXTENSION_URI("http://psiamp.org/dtd/doxygen_dbusxml.dtd");

class DBusXMLHandler : public QXmlDefaultHandler
{
public:
    DBusXMLHandler(ParserInterface * parser,
                   QXmlSimpleReader * reader,
                   const char * file_name,
                   Entry * root) :
        m_parser(parser),
        m_locator(reader),
        m_currentEntry(0),
        m_currentInterface(0),
        m_currentMethod(0),
        m_currentArgument(0),
        m_currentProperty(0),
        m_currentEnum(0),
        m_fileName(file_name),
        m_currentComment(0)
    {
        setDocumentLocator(&m_locator);

        m_scopeCount = 0;

        // Set up stack cleanup:
        m_structStack.setAutoDelete(TRUE);
        m_elementStack.setAutoDelete(TRUE);
        m_scopeStack.setAutoDelete(TRUE);

        openScopes(root);
    }

    ~DBusXMLHandler()
    { closeScopes(); }

    QString errorString()
    { return m_errorString; }

    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attributes)
    {
        // add to elements stack:
        m_elementStack.append(new ElementData(qName));

        // First we need a node.
        if (DBUS("node"))
        {
            CONDITION(!m_currentNode.isEmpty(), "Node inside a node.");

            const int idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(idx < 0, QString("Anonymous node found."));

            m_currentNode = attributes.value(idx);
            // A node is actually of little interest, so do nothing here.
            return true;
        }

        // Then we need an interface.
        if (DBUS("interface"))
        {
            // We need a nodeName for interfaces:
            CONDITION(m_currentNode.isEmpty(), "Interface without a node.");
            CONDITION(m_currentInterface, "Interface within another interface.");

            const int idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(idx < 0, QString("Interface without a name found."));

            // A interface is roughly equivalent to a class:
            m_currentInterface = createEntry();

            m_currentInterface->section = Entry::CLASS_SEC;
            m_currentInterface->spec |= Entry::Interface;
            m_currentInterface->type = "Interface";
            m_currentInterface->name = substitute(attributes.value(idx), ".", "::");

            openScopes(m_currentInterface);

            return true;
        }

        if (DBUS("method") || DBUS("signal"))
        {
            // We need a interfaceName for methods and signals:
            CONDITION(!m_currentInterface, "Method or signal found outside a interface.");
            CONDITION(m_currentMethod, "Method or signal found inside another method or signal.");
            CONDITION(m_currentProperty, "Methor or signal found inside a property.");
            CONDITION(!m_structStack.isEmpty(), "Method or signal found inside a struct.");
            CONDITION(m_currentEnum, "Methor or signal found inside a enum.");

            const int idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(idx < 0, QString("Method or signal without a name found."));

            m_currentMethod = createEntry();

            m_currentMethod->section = Entry::FUNCTION_SEC;
            m_currentMethod->name = attributes.value(idx);
            m_currentMethod->mtype = Method;
            m_currentMethod->type = "void";

            if (DBUS("signal"))
            { m_currentMethod->mtype = Signal; }
        }

        if (DBUS("arg"))
        {
            // We need a method for arguments:
            CONDITION(!m_currentMethod, "Argument found outside a method or signal.");
            CONDITION(m_currentArgument, "Argument found inside another argument.");

            const int name_idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(name_idx < 0, QString("Argument without a name found."));
            COND_DOC_ERROR(!hasType(attributes), QString("Argument without a type found."));

            const int direction_idx(indexOf(attributes, "direction"));

            if ((m_currentMethod->mtype == Signal &&
                 direction_idx >= 0 &&
                 attributes.value(direction_idx) != "in") ||
                (m_currentMethod->mtype == Method &&
                 direction_idx >= 0 &&
                 attributes.value(direction_idx) != "in" &&
                 attributes.value(direction_idx) != "out"))
            {
                m_errorString = "Invalid direction found.";
                return false;
            }

            m_currentArgument = new Argument;
            m_currentArgument->type = getType(attributes);
            m_currentArgument->name = attributes.value(name_idx);
            if (direction_idx >= 0)
            { m_currentArgument->attrib = attributes.value(direction_idx); }
            else
            {
                if (m_currentMethod->mtype == Signal)
                { m_currentArgument->attrib = "in"; }
                else
                { m_currentArgument->attrib = "out"; }
            }
        }

        if (DBUS("property"))
        {
            CONDITION(m_currentMethod, "Property found inside a method or signal.");
            CONDITION(!m_currentInterface, "Property found outside an interface.");
            CONDITION(m_currentProperty, "Property found inside another property.");
            CONDITION(!m_structStack.isEmpty(), "Property found inside a struct.");
            CONDITION(m_currentEnum, "Property found inside a enum.");

            const int name_idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(name_idx < 0, QString("Anonymous property found."));
            COND_DOC_ERROR(!hasType(attributes), QString("Property without a type found."));

            const int access_idx(indexOf(attributes, "access"));
            COND_DOC_ERROR(access_idx < 0, QString("Property without a access attribute found."));
            COND_DOC_ERROR(attributes.value(access_idx) != "read" &&
                           attributes.value(access_idx) != "write" &&
                           attributes.value(access_idx) != "readwrite",
                           QString("Property with invalid access attribute \"%1\" found.").
                           arg(attributes.value(access_idx)));

            m_currentProperty = createEntry();

            m_currentProperty->section = Entry::FUNCTION_SEC;

            if (attributes.value(access_idx) == "read" ||
                attributes.value(access_idx) == "readwrite")
            { m_currentProperty->spec |= Entry::Readable; }

            if (attributes.value(access_idx) == "write" ||
                attributes.value(access_idx) == "readwrite")
            { m_currentProperty->spec |= Entry::Writable; }

            m_currentProperty->name = attributes.value(name_idx);
            m_currentProperty->mtype = Property;
            m_currentProperty->type = getType(attributes);
        }

        if (EXTENSION("namespace"))
        {
            CONDITION(m_currentNode.isEmpty(), "Namespace found outside a node.");
            CONDITION(m_currentInterface, "Namespace found inside an interface.");

            const int idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(idx < 0, QString("Anonymous namespace found."));

            m_namespaceStack.append(openNamespace(attributes.value(idx)));
            openScopes(m_namespaceStack.last());
        }

        if (EXTENSION("struct"))
        {
            CONDITION(m_currentMethod, "Struct found inside a method or signal.");
            CONDITION(m_currentProperty, "Struct found inside a property.");
            CONDITION(m_currentEnum, "Struct found inside an enum.");

            const int idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(idx < 0, QString("Anonymous struct found."));

            Entry * current_struct = createEntry();
            current_struct->section = Entry::CLASS_SEC;
            current_struct->spec = Entry::Struct;
            current_struct->name = attributes.value(idx);

            openScopes(current_struct);

            current_struct->type = current_struct->name + " struct";

            m_structStack.append(new StructData(current_struct));
        }

        if (EXTENSION("member"))
        {
            CONDITION(m_structStack.isEmpty(), "Member found outside of struct.");

            const int name_idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(name_idx < 0, QString("Anonymous member found."));
            COND_DOC_ERROR(!hasType(attributes), QString("Member without a type found."));

            createEntry();

            m_currentEntry->section = Entry::VARIABLE_SEC;
            m_currentEntry->name = attributes.value(name_idx);
            m_currentEntry->type = getType(attributes);

            QString type(getDBusType(m_currentEntry->type));
            m_structStack.last()->type.append(type);
        }

        if (EXTENSION("enum") || EXTENSION("flagset"))
        {
            CONDITION(m_currentMethod, "Enum found inside a method or signal.");
            CONDITION(m_currentProperty, "Enum found inside a property.");

            const int name_idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(name_idx < 0, QString("Anonymous enum found."));

            const int type_idx(indexOf(attributes, "type"));
            QString type = "u";
            if (type_idx >= 0)
            { type = attributes.value(type_idx); }
            if (type != "y" && type != "q" && type != "u" && type != "t")
            { DOC_ERROR(QString("Invalid enum type \"%1\" found.").arg(type)); }

            m_currentEnum = createEntry();
            m_currentEnum->section = Entry::ENUM_SEC;
            m_currentEnum->name = attributes.value(name_idx);

            openScopes(m_currentEnum);

            m_currentEnum->type = m_currentEntry->name + " enum";

            addNamedType(type);
        }

        if (EXTENSION("value"))
        {
            CONDITION(!m_currentEnum, "Value found outside an enum.");

            const int name_idx(indexOf(attributes, "name"));
            COND_DOC_ERROR(name_idx < 0, QString("Anonymous value found."));

            const int value_idx(indexOf(attributes, "value"));

            createEntry();

            m_currentEntry->section = Entry::VARIABLE_SEC;
            m_currentEntry->name = attributes.value(name_idx);
            m_currentEntry->type = m_currentEnum->name; // "@"; // enum marker!
            if (value_idx >= 0)
            { m_currentEntry->initializer = attributes.value(value_idx); }
        }

        return true;
    }

    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName)
    {
        // Clean up elements stack:
        // Since we made sure to get the elements in the proper order when
        // adding we do not need to do so again here.
        COND_DOC_ERROR(m_elementStack.last()->element != qName,
                       QString("Malformed XML: Unexpected closing element found.").
                       arg(m_elementStack.last()->element));
        m_elementStack.removeLast();

        // Interface:
        if (DBUS("interface"))
        {
            CONDITION(m_currentInterface, "end of interface found without start.");
            m_currentInterface->endBodyLine = lineNumber();
            closeScopes();
            m_currentInterface = 0;
        }

        if (DBUS("method") || DBUS("signal"))
        {
            CONDITION(m_currentMethod, "end of method found without start.");
            CONDITION(m_currentInterface, "end of method found outside interface.");
            m_currentMethod->endBodyLine = lineNumber();
            m_currentInterface->addSubEntry(m_currentMethod);
            m_currentMethod = 0;
        }

        if (DBUS("property"))
        {
            CONDITION(m_currentMethod, "end of property found without start.");
            CONDITION(m_currentInterface, "end of property found outside interface.");
            m_currentProperty->endBodyLine = lineNumber();
            m_currentInterface->addSubEntry(m_currentProperty);
            m_currentProperty = 0;
        }

        if (DBUS("arg"))
        {
            CONDITION(m_currentMethod, "end of arg found outside method.");
            m_currentMethod->argList->append(m_currentArgument);
            m_currentArgument = 0;
        }

        if (EXTENSION("namespace"))
        {
            Entry * current = m_namespaceStack.last();
            CONDITION(current, "end of namespace without start.");
            m_namespaceStack.removeLast();

            current->endBodyLine = lineNumber();
            closeScopes();
        }

        if (EXTENSION("struct"))
        {
            StructData * data = m_structStack.last();
            CONDITION(data, "end of struct without start.");

            data->entry->endBodyLine = lineNumber();

            QString current_type;
            current_type.append(QString("("));
            current_type.append(data->type);
            current_type.append(QString(")"));

            addNamedType(current_type);

            closeScopes();

            m_structStack.removeLast();
        }

        if (EXTENSION("member"))
        {
           StructData * data = m_structStack.last();
           CONDITION(data, "end of struct without start");
           data->entry->addSubEntry(m_currentEntry);
        }

        if (EXTENSION("enum") || EXTENSION("flagset"))
        {
            CONDITION(m_currentEnum, "end of enum without start");
            m_currentEnum->endBodyLine = lineNumber();
            closeScopes();

            m_currentEnum = 0;
        }

        if (EXTENSION("value"))
        {
            CONDITION(m_currentEntry, "end of value without start");
            m_currentEntry->endBodyLine = lineNumber();

            m_currentEnum->addSubEntry(m_currentEntry);
        }

        return true;
    }

    bool characters(const QString & /*chars*/)
    { return true; }

    bool comment(const QString & comment_)
    {
        if (m_currentComment)
        { handleComment(); }

        m_currentComment = new CommentData(m_fileName, lineNumber(), comment_);

        if (!m_currentComment->shouldIgnore)
        {
            delete m_currentComment;
            m_currentComment = 0;
            return true;
        }

        if (m_currentComment->associateWithPrevious)
        { handleComment(); }

        return true;
    }

    void handleComment()
    {
        if (m_currentComment == 0 || m_currentEntry == 0)
        { return; }

        QCString text(m_currentComment->text);

        m_currentEntry->docFile = m_currentComment->fileName;
        m_currentEntry->docLine = m_currentComment->line;

        int position(0);
        bool needs_entry(false);
        bool brief(false);
        Protection prot(Public);
        int lineNr = lineNumber();

        while (parseCommentBlock(m_parser,
                                 m_currentEntry,
                                 text, m_fileName.utf8().data(), 
                                 lineNr,
                                 brief, m_currentComment->isJavaStyle,
                                 false,
                                 prot,
                                 position,
                                 needs_entry))
        {
            if (needs_entry) { createEntry(); }
        }
        if (needs_entry) { createEntry(); }

        delete m_currentComment;
        m_currentComment = 0;
    }

    QXmlLocator * locator()
    { return &m_locator; }

    int lineNumber()
    { return m_locator.lineNumber(); }

    void setSection()
    {
        Entry * current = createEntry();
        current->reset();

        current->name    = m_fileName.utf8();
        current->section = Entry::SOURCE_SEC;

        // Open/Close the scope to do the bookkeeping:
        openScopes(current);
        closeScopes();
    }

private:
    bool isDBusElement(const QString & namespaceURI,
                       const QString & localName,
                       const QString & qName,
                       const QString & element)
    {
        return (namespaceURI.isEmpty() && localName == element && qName == element) ||
               (namespaceURI.isEmpty() && localName.isEmpty() && qName == element);
    }

    bool isExtensionElement(const QString & namespaceURI,
                            const QString & localName,
                            const QString & qName,
                            const QString & element)
    {
        // isNull happens in startelement if no URI is used.
        if (namespaceURI.isNull())
        { return false; }

        // We are in a endElement: URI is always empty there:-(
        if (namespaceURI.isEmpty())
        { return qName == m_scopeStack.last()->extensionPrefix + element; }

        // startElemennt: We need to save the qName prefix
        // since endElement will forget about the namespaceURi:-(
        if (namespaceURI == EXTENSION_URI)
        {
            int pos = qName.find(':');
            m_scopeStack.last()->extensionPrefix = qName.left(pos + 1);
        }

        return namespaceURI == EXTENSION_URI && localName == element;
    }

    bool hasType(const QXmlAttributes & attributes)
    {
        const int type_idx(indexOf(attributes, "type"));
        const int named_type_idx(indexOf(attributes, "named-type"));

        return named_type_idx >= 0 || type_idx >= 0;
    }

    QString getType(const QXmlAttributes & attributes)
    {
        const int type_idx(indexOf(attributes, "type"));
        const int named_type_idx(indexOf(attributes, "named-type"));

        QString type;

        if (named_type_idx >= 0)
        {
            type = attributes.value(named_type_idx);
            if (!type.startsWith("::"))
            { type = getCurrentScope(attributes.value(named_type_idx)); }
            else
            { type = type.mid(2); }
            if (m_namedTypeMap.contains(type))
            {
                if (type_idx >= 0)
                {
                    const QString dbus_type(attributes.value(type_idx));
                    if (dbus_type != m_namedTypeMap[type])
                    {
                        DOC_ERROR(QString("Type \"%1\" does not match up with "
                                          "previous definition of named type \"%2\" (which was \"%3\".").
                                          arg(dbus_type).
                                          arg(type).
                                          arg(m_namedTypeMap[type]));
                    }
                }
                return type;
            }

            DOC_ERROR(QString("Undefined named type \"%1\" used.").arg(type));
        }

        if (type_idx >= 0)
        {
            type = attributes.value(type_idx);

            QRegExp reg_exp(QCString("(a?[ybnqiuxdtsogv]|a[{]sv[}])"));
            if (reg_exp.match(type.data()))
            { return type; }

            DOC_ERROR(QString("Unnamed complex D-Bus type \"%1\" found.").arg(type));
        }

        return QString();
    }

    QString getDBusType(const QString & type)
    {
        QString scoped_type = type;
        if (!scoped_type.contains("::"))
        { scoped_type = getCurrentScope(type); }

        if (m_namedTypeMap.contains(scoped_type))
        { return m_namedTypeMap[scoped_type]; }
        else
        { return type; }
    }

    void addNamedType(const QString type)
    {
        QString scoped_name(getCurrentScope());

        if (m_namedTypeMap.contains(scoped_name))
        {
            DOC_ERROR(QString("Named type \"%1\" is already defined.").arg(scoped_name));
            return;
        }

        m_namedTypeMap.insert(scoped_name, type);
    }

    QString getCurrentScope(const QString & type = QString())
    {
        QString scoped_name;
        if (!m_scopeStack.isEmpty())
        {
            scoped_name = m_scopeStack.last()->scope->name;
            scoped_name.append("::");
        }
        if (!type.isEmpty())
        { scoped_name.append(type); }
        else
        { scoped_name = scoped_name.left(scoped_name.length() - 2); }

        return scoped_name;
    }

    int indexOf(const QXmlAttributes & attributes, const QString & name,
                 const QString & type = "CDATA", const bool mandatory = true)
    {
        const int idx(attributes.index(name));
        if (idx < 0 || idx > attributes.length()) { return -1; }
        if (attributes.type(idx) != type) { return -1; }
        if (mandatory && attributes.value(idx).isEmpty()) { return -1; }

        return idx;
    }

    Entry * createEntry()
    {
        Entry * entry = new Entry();

        entry->protection = Public ;
        entry->virt       = Normal;
        entry->stat       = false;
        entry->lang       = SrcLangExt_XML;
        entry->spec       = 0;

        entry->fileName  = m_fileName;
        entry->startLine = lineNumber();
        entry->bodyLine = lineNumber();

        entry->callGraph = false;
        entry->callerGraph = false;

        initGroupInfo(entry);

        m_currentEntry = entry;

        handleComment();

        return entry;
    }

    void openScopes(Entry * object)
    {
        int cur_scope_separator_pos = 0;
        int last_scope_separator_pos = 0;
        while (0 <= (cur_scope_separator_pos = object->name.find("::", last_scope_separator_pos)))
        {
            QString scope = object->name.mid(last_scope_separator_pos,
                                             cur_scope_separator_pos - last_scope_separator_pos);
            last_scope_separator_pos = cur_scope_separator_pos + 2;

            Entry * current_namespace = openNamespace(scope);

            if (!m_scopeStack.isEmpty())
            { m_scopeStack.last()->scope->addSubEntry(current_namespace); }

            m_scopeStack.append(new ScopeData(current_namespace, m_scopeCount));
        }

        QString scoped_name(getCurrentScope());
        if (!scoped_name.isEmpty())
        { scoped_name.append("::"); }
        scoped_name.append(object->name.mid(last_scope_separator_pos));

        object->name = scoped_name;

        if (!m_scopeStack.isEmpty())
        { m_scopeStack.last()->scope->addSubEntry(object); }
        m_scopeStack.append(new ScopeData(object, m_scopeCount));

        ++m_scopeCount;
    }

    Entry * openNamespace(const QString & name)
    {
        Entry * current_namespace = createEntry();
        QString scoped_name(getCurrentScope());
        if (!scoped_name.isEmpty())
        { scoped_name.append("::"); }
        scoped_name.append(name);
        current_namespace->name = scoped_name;
        current_namespace->section = Entry::NAMESPACE_SEC;
        current_namespace->type = "namespace" ;

        return current_namespace;
    }

    void closeScopes()
    {
        const int current_scope_count(m_scopeStack.last()->count);

        // Do not close the root scope.
        if (current_scope_count == 0)
        { return; }

        while (current_scope_count == m_scopeStack.last()->count)
        { m_scopeStack.removeLast(); }
    }

    ParserInterface * m_parser;

    QXmlLocator m_locator;
    QString m_currentNode; // Nodes can not be nested, no entry necessary.

    struct ElementData
    {
        ElementData(const QString & e) :
            element(e)
        { }
        ~ElementData() { }

        QString element; //*< The element name
        QString extensionPrefix; //*< The prefix used for our extension.
        QString text; //*< The actual xml code.
    };
    QList<ElementData> m_elementStack;

    Entry * m_currentEntry; // The currently open entry.

    Entry * m_currentInterface; // Interfaces can not be nested.
    Entry * m_currentMethod; // Methods can not be nested.
    Argument * m_currentArgument; // Arguments can not be nested.
    Entry * m_currentProperty; // Properties can not be nested.
    Entry * m_currentEnum; // Enums can not be nested.
    QList<Entry> m_namespaceStack;

    struct StructData
    {
        StructData(Entry * e) : entry(e) { }
        ~StructData() { }

        QString type;
        Entry * entry;
    };
    QList<StructData> m_structStack; // Structs can be nested.

    struct ScopeData
    {
        ScopeData(Entry * s, int c) :
            scope(s),
            count(c)
        { }
        ~ScopeData() { }

        Entry * scope;
        QString extensionPrefix;
        int count;
    };
    QList<ScopeData> m_scopeStack; // Scopes are nested.

    QString m_fileName;

    struct CommentData
    {
        CommentData(const QString & f, const int l, const QString & t) :
            isJavaStyle(false),
            isQtStyle(false),
            line(l),
            fileName(f)
        {
            isJavaStyle = t.startsWith(QChar('*'));
            isQtStyle = t.startsWith(QChar('!'));
            shouldIgnore = (!isJavaStyle && !isQtStyle);
            associateWithPrevious = (t.at(1) == QChar('<'));
            if (associateWithPrevious)
            { text = t.mid(2); }
            else
            { text = t.mid(1); }
        }
        ~CommentData() { }

        QString text;
        bool isJavaStyle;
        bool isQtStyle;
        bool shouldIgnore;
        bool associateWithPrevious;
        int line;
        QString fileName;
    };
    CommentData * m_currentComment;

    int m_scopeCount; //*< unique scope id.

    QString m_errorString;

    QMap<QString, QString> m_namedTypeMap;
};

// -----------------------------------------------------------------------
// DBusXMLScanner
// -----------------------------------------------------------------------

DBusXMLScanner::DBusXMLScanner()
{ }

DBusXMLScanner::~DBusXMLScanner()
{ }

void DBusXMLScanner::parseInput(const char * fileName,
                                const char * /* fileBuf */,
                                Entry * root)
{
    err("Note that the dbusxml parser seems to be broken :-(\nPlease help me to fix it!\n");
    QFile inputFile(fileName);

    QXmlInputSource inputSource(inputFile);
    QXmlSimpleReader reader;

    DBusXMLHandler handler(this, &reader, fileName, root);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    reader.setLexicalHandler(&handler);

    groupEnterFile(fileName, 1);
    handler.setSection();
    reader.parse(inputSource);

    if (handler.errorString())
    { err("DBus XML Parser: Error at line %d: %s\n", 
        handler.locator()->lineNumber(),handler.errorString().utf8().data()); }

    groupLeaveFile(fileName, 1);
}

bool DBusXMLScanner::needsPreprocessing(const QCString & /* extension */)
{ return (false); }

void DBusXMLScanner::parseCode(CodeOutputInterface & /* codeOutIntf */,
                               const char * /* scopeName */,
                               const QCString & /* input */,
                               bool /* isExampleBlock */,
                               const char * /* exampleName */,
                               FileDef * /* fileDef */,
                               int /* startLine */,
                               int /* endLine */,
                               bool /* inlineFragment */,
                               MemberDef * /* memberDef */,
                               bool /*showLineNumbers*/)
{ }

void DBusXMLScanner::resetCodeParserState()
{ }

void DBusXMLScanner::parsePrototype(const char * /* text */)
{ }
