/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef ENTRY_H
#define ENTRY_H

#include <qstring.h>
#include <qlist.h>

enum Protection { Public, Protected, Private } ;
enum Specifier { Normal, Virtual, Pure } ;

struct BaseInfo 
{
  BaseInfo(const char *n,Protection p,Specifier v) : name(n),prot(p),virt(v) {}
  QString    name; // the name of the base class
  Protection prot; // inheritance type
  Specifier  virt; // virtualness
};

struct Argument
{
  Argument() {}
  Argument(const Argument &a) 
  { 
    type=a.type.copy(); 
    name=a.name.copy(); 
    defval=a.defval.copy(); 
  }
  Argument &operator=(const Argument &a)
  {
    if (this!=&a)
    {
      type=a.type.copy(); 
      name=a.name.copy(); 
      defval=a.defval.copy(); 
    }
    return *this;
  }
  
  QString type;    // argument type
  QString name;    // argument name (if any)
  QString defval;  // argument default value (if any)
};

class ArgumentList : public QList<Argument> 
{
  public:
    ArgumentList() : QList<Argument>(), 
                     constSpecifier(FALSE),
                     volatileSpecifier(FALSE),
                     pureSpecifier(FALSE) {}
    bool constSpecifier;
    bool volatileSpecifier;
    bool pureSpecifier;
};

typedef QListIterator<Argument> ArgumentListIterator;

class Entry
{
  public:

    enum Sections { 
      CLASS_SEC        = 0x00000001, 
      STRUCT_SEC       = 0x00000002,
      UNION_SEC        = 0x00000004, 
      ENUM_SEC         = 0x00000008,
      EMPTY_SEC        = 0x00000010, 
      PAGEDOC_SEC      = 0x00000020, 
      VARIABLE_SEC     = 0x00000040,
      FUNCTION_SEC     = 0x00000080,
      TYPEDEF_SEC      = 0x00000100,
      CLASSDOC_SEC     = 0x00000200, 
      MEMBERDOC_SEC    = 0x00000400, 
      OVERLOADDOC_SEC  = 0x00000800,
      EXAMPLE_SEC      = 0x00001000, 
      VARIABLEDOC_SEC  = 0x00002000,
      ENUMDOC_SEC      = 0x00004000,
      UNIONDOC_SEC     = 0x00008000,
      STRUCTDOC_SEC    = 0x00010000,
      SOURCE_SEC       = 0x00020000,
      HEADER_SEC       = 0x00040000,
      FILEDOC_SEC      = 0x00080000,
      DEFINEDOC_SEC    = 0x00100000,
      INCLUDE_SEC      = 0x00200000,
      DEFINE_SEC       = 0x00400000,
      GROUPDOC_SEC     = 0x00800000,
      NAMESPACE_SEC    = 0x01000000,
      NAMESPACEDOC_SEC = 0x02000000,
      COMPOUND_MASK    = CLASS_SEC | STRUCT_SEC | UNION_SEC,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC,
      SCOPE_MASK       = COMPOUND_MASK | NAMESPACE_SEC,
      FILE_MASK        = SOURCE_SEC | HEADER_SEC
    };

    Entry();
    Entry(const Entry &);
    ~Entry();
    int getSize();

    void	addSubEntry (Entry* e) ;
    void        reset();

    int        section;       // entry type;
    Protection protection;    // class protection
    bool sig;                 // a Qt signal ?
    bool slot;                // a Qt slot ?
    bool stat;                // static ?
    bool proto;               // prototype ?
    Specifier   virt;         // virtualness of the entry 
    Entry      *parent;       // parent node in the tree
    QString	type;         // member type 
    QString	name;         // member name
    QString     args;         // member argument string
    ArgumentList *argList;    // member arguments as a list
    ArgumentList *tArgList;   // template argument list
    ArgumentList *tNameList;  // template name list
    QString	program;      // the program text
    QString     includeFile;  // include file (2 arg of \class, must be unique)
    QString     includeName;  // include name (3 arg of \class)
    QString     doc;          // documentation block (partly parsed)
    QString     relates;      // related class (doc block)
    QString     brief;        // brief description (doc block)
    QString     inside;       // name of the class in which documents are found
    QString     exception;    // throw specification
    QList<Entry>    *sublist; // entries that are children of this one
    QList<BaseInfo> *extends; // list of base classes
    QList<QString>  *groups;  // list of groups this entry belongs to
    QList<QString>  *anchors; // list of anchors defined in this entry
    QString	fileName;     // file this entry was extracted from
    int		startLine;    // start line of entry in the source
    int         num;
  private:
    Entry &operator=(const Entry &); 
} ;

typedef QList<Entry> EntryList;
typedef QListIterator<Entry> EntryListIterator;

#endif
