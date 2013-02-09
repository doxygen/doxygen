/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include "types.h"

#include <qlist.h>
#include <qgstring.h>

struct SectionInfo;
class QFile;
class EntryNav;
class FileDef;
class FileStorage;
class StorageIntf;
class ArgumentList;
struct ListItemInfo;

/** This class stores information about an inheritance relation
 */ 
struct BaseInfo 
{
  /*! Creates an object representing an inheritance relation */
  BaseInfo(const char *n,Protection p,Specifier v) : 
    name(n),prot(p),virt(v) {}
  QCString   name; //!< the name of the base class
  Protection prot; //!< inheritance type
  Specifier  virt; //!< virtualness
};

/** This struct is used to capture the tag file information 
 *  for an Entry. 
 */
struct TagInfo 
{
  QCString tagName;
  QCString fileName;
  QCString anchor;
};

/** Represents an unstructured piece of information, about an
 *  entity found in the sources. 
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
      NAMESPACE_SEC    = 0x00000010,
      COMPOUND_MASK    = CLASS_SEC,
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
      OBJCIMPL_SEC     = 0x17000000,
      DIRDOC_SEC       = 0x18000000
    };
    enum MemberSpecifier
    {
      Inline      = 0x00000001,
      Explicit    = 0x00000002,
      Mutable     = 0x00000004,
      Settable    = 0x00000008,
      Gettable    = 0x00000010,
      Readable    = 0x00000020,
      Writable    = 0x00000040,
      Final       = 0x00000080,
      Abstract    = 0x00000100,
      Addable     = 0x00000200,
      Removable   = 0x00000400,
      Raisable    = 0x00000800,
      Override    = 0x00001000,
      New         = 0x00002000,
      Sealed      = 0x00004000,
      Initonly    = 0x00008000,
      Optional    = 0x00010000,
      Required    = 0x00020000,
      NonAtomic   = 0x00040000,
      Copy        = 0x00080000,
      Retain      = 0x00100000,
      Assign      = 0x00200000,
      Strong      = 0x00400000,
      Weak        = 0x00800000,
      Unretained  = 0x01000000,
      Alias       = 0x02000000,
      ConstExp    = 0x04000000,
      Default     = 0x08000000,
      Delete      = 0x10000000,
      NoExcept    = 0x20000000
    };
    enum ClassSpecifier
    {
      Template       = 0x0001,
      Generic        = 0x0002,
      Ref            = 0x0004,
      Value          = 0x0008,
      Interface      = 0x0010,
      Struct         = 0x0020,
      Union          = 0x0040,
      Exception      = 0x0080,
      Protocol       = 0x0100,
      Category       = 0x0200,
      SealedClass    = 0x0400,
      AbstractClass  = 0x0800,
      Enum           = 0x1000  // for Java-style enums
    };
    enum GroupDocType
    {
      GROUPDOC_NORMAL,        //!< defgroup
      GROUPDOC_ADD,           //!< addgroup
      GROUPDOC_WEAK           //!< weakgroup
    };                        //!< kind of group

    Entry();
    Entry(const Entry &);
   ~Entry();

    /*! Returns the static size of the Entry (so excluding any dynamic memory) */
    int getSize();

    void addSpecialListItem(const char *listName,int index);
    void createNavigationIndex(EntryNav *rootNav,FileStorage *storage,FileDef *fd);

    // while parsing a file these function can be used to navigate/build the tree
    void setParent(Entry *parent) { m_parent = parent; }

    /*! Returns the parent for this Entry or 0 if this entry has no parent. */
    Entry *parent() const { return m_parent; }

    /*! Returns the list of children for this Entry 
     *  @see addSubEntry() and removeSubEntry()
     */
    const QList<Entry> *children() const { return m_sublist; }

    /*! Adds entry \a e as a child to this entry */
    void addSubEntry (Entry* e) ;

    /*! Removes entry \a e from the list of children. 
     *  Returns a pointer to the entry or 0 if the entry was not a child. 
     *  Note the entry will not be deleted.
     */ 
    Entry *removeSubEntry(Entry *e);

    /*! Restore the state of this Entry to the default value it has
     *  at construction time. 
     */
    void reset();

    /*! Serialize this entry to a persistent storage stream. */
    void marshall(StorageIntf *);

    /*! Reinitialize this entry from a persistent storage stream. */
    void unmarshall(StorageIntf *);

  public:

    // identification
    int          section;     //!< entry type (see Sections);
    QCString	 type;        //!< member type 
    QCString	 name;        //!< member name
    TagInfo     *tagInfo;     //!< tag file info

    // content
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    int  spec;                //!< class/member specifiers
    int  initLines;           //!< define/variable initializer lines to show 
    bool stat;                //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    bool subGrouping;         //!< automatically group class members?
    bool callGraph;           //!< do we need to draw the call graph?
    bool callerGraph;         //!< do we need to draw the caller graph?
    Specifier    virt;        //!< virtualness of the entry 
    QCString     args;        //!< member argument string
    QCString     bitfields;   //!< member's bit fields
    ArgumentList *argList;    //!< member arguments as a list
    QList<ArgumentList> *tArgLists; //!< template argument declarations
    QGString	 program;     //!< the program text
    QGString     initializer; //!< initial value (for variables)
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
    RelatesType  relatesType; //!< how relates is handled
    QCString     read;        //!< property read accessor
    QCString     write;       //!< property write accessor
    QCString     inside;      //!< name of the class in which documents are found
    QCString     exception;   //!< throw specification
    ArgumentList *typeConstr; //!< where clause (C#) for type constraints
    int          bodyLine;    //!< line number of the definition in the source
    int          endBodyLine; //!< line number where the definition ends
    int          mGrpId;      //!< member group id
    QList<BaseInfo> *extends; //!< list of base classes    
    QList<Grouping> *groups;  //!< list of groups this entry belongs to
    QList<SectionInfo> *anchors; //!< list of anchors defined in this entry
    QCString	fileName;     //!< file this entry was extracted from
    int		startLine;    //!< start line of entry in the source
    int		startColumn;  //!< start column of entry in the source
    QList<ListItemInfo> *sli; //!< special lists (test/todo/bug/deprecated/..) this entry is in
    SrcLangExt  lang;         //!< programming language in which this entry was found
    bool        hidden;       //!< does this represent an entity that is hidden from the output
    bool        artificial;   //!< Artificially introduced item
    GroupDocType groupDocType;


    static int  num;          //!< counts the total number of entries

    /// return the command name used to define GROUPDOC_SEC
    const char *groupDocCmd() const
    {
      switch( groupDocType ) 
      {
        case GROUPDOC_NORMAL: return "\\defgroup";
        case GROUPDOC_ADD: return "\\addgroup";
        case GROUPDOC_WEAK: return "\\weakgroup";
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
        case GROUPDOC_NORMAL: return Grouping::GROUPING_AUTO_DEF;
        case GROUPDOC_ADD:    return Grouping::GROUPING_AUTO_ADD;
        case GROUPDOC_WEAK:   return Grouping::GROUPING_AUTO_WEAK;
        default: return Grouping::GROUPING_LOWEST;
      }
    }

  private:  
    void createSubtreeIndex(EntryNav *nav,FileStorage *storage,FileDef *fd);
    Entry         *m_parent;    //!< parent node in the tree
    QList<Entry>  *m_sublist;   //!< entries that are children of this one
    Entry &operator=(const Entry &); 
};

/** Wrapper for a node in the Entry tree.
 *
 *  Allows navigating through the Entry tree and load and storing Entry
 *  objects persistently to disk.
 */
class EntryNav
{
  public:
    EntryNav(EntryNav *parent,Entry *e);
   ~EntryNav();
    void addChild(EntryNav *);
    bool loadEntry(FileStorage *storage);
    bool saveEntry(Entry *e,FileStorage *storage);
    void setEntry(Entry *e);
    void releaseEntry();
    void changeSection(int section) { m_section = section; }
    void setFileDef(FileDef *fd) { m_fileDef = fd; }

    Entry *entry() const { return m_info; }
    int section() const { return m_section; }
    SrcLangExt lang() const { return m_lang; }
    const QCString &type() const { return m_type; }
    const QCString &name() const { return m_name; }
    TagInfo *tagInfo() const { return m_tagInfo; }
    const QList<EntryNav> *children() const { return m_subList; }
    EntryNav *parent() const { return m_parent; }
    FileDef *fileDef() const { return m_fileDef; }

  private:

    // navigation 
    EntryNav        *m_parent;    //!< parent node in the tree
    QList<EntryNav> *m_subList;   //!< entries that are children of this one

    // identification
    int          m_section;     //!< entry type (see Sections);
    QCString	 m_type;        //!< member type 
    QCString	 m_name;        //!< member name
    TagInfo     *m_tagInfo;      //!< tag file info
    FileDef     *m_fileDef;
    SrcLangExt   m_lang;         //!< programming language in which this entry was found

    Entry       *m_info;
    int64        m_offset;
    bool         m_noLoad;
};


typedef QList<Entry> EntryList;
typedef QListIterator<Entry> EntryListIterator;

typedef QList<EntryNav> EntryNavList;
typedef QListIterator<EntryNav> EntryNavListIterator;

#endif
