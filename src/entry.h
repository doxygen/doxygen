/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef ENTRY_H
#define ENTRY_H

#include "qtbc.h"
#include <qlist.h>

enum Protection { Public, Protected, Private } ;
enum Specifier { Normal, Virtual, Pure } ;

struct BaseInfo 
{
  BaseInfo(const char *n,Protection p,Specifier v) : name(n),prot(p),virt(v) {}
  QCString   name; // the name of the base class
  Protection prot; // inheritance type
  Specifier  virt; // virtualness
};

struct Argument
{
  /*! Construct a new argument. */
  Argument() {}
  /*! Copy an argument (does a deep copy of all strings). */
  Argument(const Argument &a) 
  { 
    attrib=a.attrib.copy();
    type=a.type.copy(); 
    name=a.name.copy(); 
    defval=a.defval.copy(); 
    docs=a.docs.copy();
    array=a.array.copy();
  }
  /* Assignment of an argument (does a deep copy of all strings). */
  Argument &operator=(const Argument &a)
  {
    if (this!=&a)
    {
      attrib=a.attrib.copy();
      type=a.type.copy(); 
      name=a.name.copy(); 
      defval=a.defval.copy(); 
      docs=a.docs.copy();
      array=a.array.copy();
    }
    return *this;
  }
  /*! return TRUE if this argument is documentation and the argument has a
   *  non empty name.
   */
  bool hasDocumentation() const 
  { 
    return !name.isEmpty() && !docs.isEmpty(); 
  }
  
  QCString attrib;   /*!< Argument's attribute (IDL only) */
  QCString type;     /*!< Argument's type */
  QCString name;     /*!< Argument's name (may be empty) */
  QCString array;    /*!< Argument's array specifier (may be empty) */
  QCString defval;   /*!< Argument's default value (may be empty) */
  QCString docs;     /*!< Argument's documentation (may be empty) */
};

class ArgumentList : public QList<Argument> 
{
  public:
   ~ArgumentList() {}
    ArgumentList() : QList<Argument>(), 
                     constSpecifier(FALSE),
                     volatileSpecifier(FALSE),
                     pureSpecifier(FALSE) {}
    bool hasDocumentation() const;
    bool constSpecifier;
    bool volatileSpecifier;
    bool pureSpecifier;
};

typedef QListIterator<Argument> ArgumentListIterator;

/*! Raw entry. parseMain() in scanner.l will generate a tree of these
 *  entries.
 */
class Entry
{
  public:

    enum Sections { 
      CLASS_SEC        = 0x00000001, 
      STRUCT_SEC       = 0x00000002,
      UNION_SEC        = 0x00000004, 
      EXCEPTION_SEC    = 0x00000008,
      NAMESPACE_SEC    = 0x00000010,
      INTERFACE_SEC    = 0x00000020,
      COMPOUND_MASK    = CLASS_SEC | STRUCT_SEC | UNION_SEC | 
                         INTERFACE_SEC | EXCEPTION_SEC,
      SCOPE_MASK       = COMPOUND_MASK | NAMESPACE_SEC,
      
      CLASSDOC_SEC     = 0x00000100, 
      STRUCTDOC_SEC    = 0x00000200,
      UNIONDOC_SEC     = 0x00000400,
      EXCEPTIONDOC_SEC = 0x00000800,
      NAMESPACEDOC_SEC = 0x00001000,
      INTERFACEDOC_SEC = 0x00002000,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC | 
                         INTERFACEDOC_SEC | EXCEPTIONDOC_SEC,

      SOURCE_SEC       = 0x00010000,
      HEADER_SEC       = 0x00020000,
      FILE_MASK        = SOURCE_SEC | HEADER_SEC,

      ENUMDOC_SEC      = 0x00100000,
      ENUM_SEC         = 0x00200000,
      EMPTY_SEC        = 0x00300000, 
      PAGEDOC_SEC      = 0x00400000, 
      VARIABLE_SEC     = 0x00500000,
      FUNCTION_SEC     = 0x00600000,
      TYPEDEF_SEC      = 0x00700000,
      MEMBERDOC_SEC    = 0x00800000, 
      OVERLOADDOC_SEC  = 0x00900000,
      EXAMPLE_SEC      = 0x00a00000, 
      VARIABLEDOC_SEC  = 0x00b00000,
      FILEDOC_SEC      = 0x00c00000,
      DEFINEDOC_SEC    = 0x00d00000,
      INCLUDE_SEC      = 0x00e00000,
      DEFINE_SEC       = 0x00f00000,
      GROUPDOC_SEC     = 0x01000000,
      USINGDIR_SEC     = 0x01100000,
      MAINPAGEDOC_SEC  = 0x01200000,
      MEMBERGRP_SEC    = 0x01300000
    };

    Entry();
    Entry(const Entry &);
    ~Entry();
    int getSize();

    void	addSubEntry (Entry* e) ;
    void        reset();

    int        section;       // entry type (see Sections);
    Protection protection;    // class protection
    bool sig;                 // a Qt signal ?
    bool slot;                // a Qt slot ?
    bool stat;                // static ?
    bool proto;               // prototype ?
    bool inLine;              // inline ?
    bool subGrouping;         // automatically group class members?
    Specifier    virt;        // virtualness of the entry 
    Entry       *parent;      // parent node in the tree
    QCString	 type;        // member type 
    QCString	 name;        // member name
    QCString     args;        // member argument string
    ArgumentList *argList;    // member arguments as a list
    ArgumentList *tArgList;   // template argument list
    ArgumentList *mtArgList;  // member template argument list
    QCString     scopeSpec;   // template specialization of the scope
    QCString     memberSpec;  // template specialization of the member
    QCString	 program;     // the program text
    QCString     initializer; // initial value (for variables)
    int          bodyLine;    // line number of the definition in the source
    int          endBodyLine; // line number where the definition ends
    QCString     includeFile; // include file (2 arg of \class, must be unique)
    QCString     includeName; // include name (3 arg of \class)
    QCString     doc;         // documentation block (partly parsed)
    QCString     relates;     // related class (doc block)
    QCString     brief;       // brief description (doc block)
    QCString     inside;      // name of the class in which documents are found
    QCString     exception;   // throw specification
    int          mGrpId;      // member group id
    QList<Entry>    *sublist; // entries that are children of this one
    QList<BaseInfo> *extends; // list of base classes
    QList<QCString> *groups;  // list of groups this entry belongs to
    QList<QCString> *anchors; // list of anchors defined in this entry
    QCString	fileName;     // file this entry was extracted from
    int		startLine;    // start line of entry in the source
    static int  num;          // counts the total number of entries
  private:
    Entry &operator=(const Entry &); 
} ;

typedef QList<Entry> EntryList;
typedef QListIterator<Entry> EntryListIterator;

#endif
