/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

struct SectionInfo;

enum Protection { Public, Protected, Private, Package } ;
enum Specifier { Normal, Virtual, Pure } ;
enum MethodTypes { Method, Signal, Slot, DCOP, Property, Event };

struct ListItemInfo
{
  QCString type;
  int itemId;
};

/*! \brief This class stores information about an inheritance relation
 */ 
struct BaseInfo 
{
  /*! Creates an object representing an inheritance relation */
  BaseInfo(const char *n,Protection p,Specifier v) : name(n),prot(p),virt(v) {}
  QCString   name; //!< the name of the base class
  Protection prot; //!< inheritance type
  Specifier  virt; //!< virtualness
};

/*! \brief This class contains the information about the argument of a
 *         function or template
 *
 */
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
  /*! Assignment of an argument (does a deep copy of all strings). */
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

/*! \brief This class represents an function or template argument list. 
 *
 *  This class also stores some information about member that is typically
 *  put after the argument list, such as wether the member is const, 
 *  volatile or pure virtual.
 */
class ArgumentList : public QList<Argument> 
{
  public:
    /*! Creates an empty argument list */
    ArgumentList() : QList<Argument>(), 
                     constSpecifier(FALSE),
                     volatileSpecifier(FALSE),
                     pureSpecifier(FALSE) { setAutoDelete(TRUE); }
    /*! Destroys the argument list */
   ~ArgumentList() {}
    bool hasDocumentation() const;
    /*! Does the member modify the state of the class? default: FALSE. */
    bool constSpecifier;
    /*! Is the member volatile? default: FALSE. */
    bool volatileSpecifier;
    /*! Is this a pure virtual member? default: FALSE */
    bool pureSpecifier;
};

typedef QListIterator<Argument> ArgumentListIterator;

/*! \brief This struct is used to capture the tag file information 
 *         for an Entry. 
 */
struct TagInfo 
{
  QCString tagName;
  QCString fileName;
  QCString anchor;
};

struct Grouping 
{
  enum GroupPri_t 
  {
    GROUPING_LOWEST,
    GROUPING_AUTO_WEAK = 
      GROUPING_LOWEST,   //!< membership in group was defined via \@weakgroup 
    GROUPING_AUTO_ADD,    //!< membership in group was defined via \@add[to]group 
    GROUPING_AUTO_DEF,    //!< membership in group was defined via \@defgroup
    GROUPING_AUTO_HIGHEST = GROUPING_AUTO_DEF,
    GROUPING_INGROUP,      //!< membership in group was defined by \@ingroup
    GROUPING_HIGHEST = GROUPING_INGROUP
  };

  static const char *getGroupPriName( GroupPri_t priority )
  {
    switch( priority )
    {
      case GROUPING_AUTO_WEAK:
        return "@weakgroup";
      case GROUPING_AUTO_ADD:
        return "@addtogroup";
      case GROUPING_AUTO_DEF:
        return "@defgroup";
      case GROUPING_INGROUP:
        return "@ingroup";
    }	    
    return "???";
  }

  Grouping( const char *gn, GroupPri_t p ) : groupname(gn), pri(p) {}
  Grouping( const Grouping &g ) : groupname(g.groupname), pri(g.pri) {}
  QCString groupname;   //!< name of the group
  GroupPri_t pri;       //!< priority of this definition

};

/*! \brief Represents an unstructured piece of information, about an
 *         entity found in the sources. 
 *
 *  parseMain() in scanner.l will generate a tree of these
 *  entries.
 */
class Entry
{
  public:

    /*! Kind of entries that are supported */
    enum Sections { 
      CLASS_SEC        = 0x00000001, 
      STRUCT_SEC       = 0x00000002,
      UNION_SEC        = 0x00000004, 
      EXCEPTION_SEC    = 0x00000008,
      NAMESPACE_SEC    = 0x00000010,
      INTERFACE_SEC    = 0x00000020,
      PROTOCOL_SEC     = 0x00000040,
      CATEGORY_SEC     = 0x00000080,
      COMPOUND_MASK    = CLASS_SEC | STRUCT_SEC | UNION_SEC | 
                         INTERFACE_SEC | EXCEPTION_SEC | 
                         PROTOCOL_SEC | CATEGORY_SEC,
      SCOPE_MASK       = COMPOUND_MASK | NAMESPACE_SEC,
      
      CLASSDOC_SEC     = 0x00000800, 
      STRUCTDOC_SEC    = 0x00001000,
      UNIONDOC_SEC     = 0x00002000,
      EXCEPTIONDOC_SEC = 0x00004000,
      NAMESPACEDOC_SEC = 0x00008000,
      INTERFACEDOC_SEC = 0x00010000,
      PROTOCOLDOC_SEC  = 0x00020000,
      CATEGORYDOC_SEC  = 0x00040000,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC | 
                         INTERFACEDOC_SEC | EXCEPTIONDOC_SEC | PROTOCOLDOC_SEC |
                         CATEGORYDOC_SEC,

      SOURCE_SEC       = 0x00400000,
      HEADER_SEC       = 0x00800000,
      FILE_MASK        = SOURCE_SEC | HEADER_SEC,

      ENUMDOC_SEC      = 0x01000000,
      ENUM_SEC         = 0x02000000,
      EMPTY_SEC        = 0x03000000, 
      PAGEDOC_SEC      = 0x04000000, 
      VARIABLE_SEC     = 0x05000000,
      FUNCTION_SEC     = 0x06000000,
      TYPEDEF_SEC      = 0x07000000,
      MEMBERDOC_SEC    = 0x08000000, 
      OVERLOADDOC_SEC  = 0x09000000,
      EXAMPLE_SEC      = 0x0a000000, 
      VARIABLEDOC_SEC  = 0x0b000000,
      FILEDOC_SEC      = 0x0c000000,
      DEFINEDOC_SEC    = 0x0d000000,
      INCLUDE_SEC      = 0x0e000000,
      DEFINE_SEC       = 0x0f000000,
      GROUPDOC_SEC     = 0x10000000,
      USINGDIR_SEC     = 0x11000000,
      MAINPAGEDOC_SEC  = 0x12000000,
      MEMBERGRP_SEC    = 0x13000000,
      USINGDECL_SEC    = 0x14000000,
      PACKAGE_SEC      = 0x15000000,
      PACKAGEDOC_SEC   = 0x16000000,
      OBJCIMPL_SEC     = 0x17000000
    };
    enum MemberSpecifier
    {
      Inline    = 0x01,
      Explicit  = 0x02,
      Mutable   = 0x04,
      Settable  = 0x08,
      Gettable  = 0x10,
      Readable  = 0x20,
      Writable  = 0x40
    };

    Entry();
    Entry(const Entry &);
    ~Entry();
    int getSize();
    void addSpecialListItem(const char *listName,int index);

    /*! Adds entry \e as a child to this entry */
    void	addSubEntry (Entry* e) ;
    /*! Restore the state of this Entry to the default value it has
     *  at construction time. 
     */
    void        reset();

    int        section;       //!< entry type (see Sections);
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    bool stat;                //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    int  memSpec;             //!< member specifiers
    int  initLines;           //!< define/variable initializer lines to show 
    bool subGrouping;         //!< automatically group class members?
    bool callGraph;           //!< do we need to draw the call graph?
    Specifier    virt;        //!< virtualness of the entry 
    Entry       *parent;      //!< parent node in the tree
    QCString	 type;        //!< member type 
    QCString	 name;        //!< member name
    QCString     args;        //!< member argument string
    QCString     bitfields;   //!< member's bit fields
    ArgumentList *argList;    //!< member arguments as a list
    QList<ArgumentList> *tArgLists; //!< template argument declarations
    QCString	 program;     //!< the program text
    QCString     initializer; //!< initial value (for variables)
    QCString     includeFile; //!< include file (2 arg of \\class, must be unique)
    QCString     includeName; //!< include name (3 arg of \\class)
    QCString     doc;         //!< documentation block (partly parsed)
    int          docLine;     //!< line number at which the documentation was found
    QCString     docFile;     //!< file in which the documentation was found
    QCString     brief;       //!< brief description (doc block)
    int          briefLine;   //!< line number at which the brief desc. was found
    QCString     briefFile;   //!< file in which the brief desc. was found
    QCString     inbodyDocs;  //!< documentation inside the body of a function
    int          inbodyLine;  //!< line number at which the body doc was found
    QCString     inbodyFile;  //!< file in which the body doc was found
    QCString     relates;     //!< related class (doc block)
    bool         relatesDup;  //!< keep duplicate doc in original file also
    QCString     inside;      //!< name of the class in which documents are found
    QCString     exception;   //!< throw specification
    int          bodyLine;    //!< line number of the definition in the source
    int          endBodyLine; //!< line number where the definition ends
    int          mGrpId;      //!< member group id
    QList<Entry>    *sublist; //!< entries that are children of this one
    QList<BaseInfo> *extends; //!< list of base classes    
    QList<Grouping> *groups;  //!< list of groups this entry belongs to
    QList<SectionInfo> *anchors; //!< list of anchors defined in this entry
    QCString	fileName;     //!< file this entry was extracted from
    int		startLine;    //!< start line of entry in the source
    QList<ListItemInfo> *sli; //!< special lists (test/todo/bug/deprecated/..) this entry is in
    TagInfo    *tagInfo;      //!< tag file info
    static int  num;          //!< counts the total number of entries
    bool        objc;         //!< Objective-C construct
    enum 
    {
      GROUPDOC_NORMAL,        //<! defgroup
      GROUPDOC_ADD,           //<! addgroup
      GROUPDOC_WEAK           //<! weakgroup
    } groupDocType;           //!< kind of group
    /// return the command name used to define GROUPDOC_SEC
    const char *groupDocCmd() const
    {
      switch( groupDocType ) 
      {
        case GROUPDOC_NORMAL: return "\\defgroup"; break;
        case GROUPDOC_ADD: return "\\addgroup"; break;
        case GROUPDOC_WEAK: return "\\weakgroup"; break;
        default: return "unknown group command";
      }
    }
    Grouping::GroupPri_t groupingPri() const
    {
      if( section != GROUPDOC_SEC ) 
      {
        return Grouping::GROUPING_LOWEST;
      }
      switch( groupDocType ) 
      {
        case GROUPDOC_NORMAL: return Grouping::GROUPING_AUTO_DEF;  break;
        case GROUPDOC_ADD:    return Grouping::GROUPING_AUTO_ADD;  break;
        case GROUPDOC_WEAK:   return Grouping::GROUPING_AUTO_WEAK; break;
        default: return Grouping::GROUPING_LOWEST;
      }
    }
  private:
    Entry &operator=(const Entry &); 
} ;

typedef QList<Entry> EntryList;
typedef QListIterator<Entry> EntryListIterator;

#endif
