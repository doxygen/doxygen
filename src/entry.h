/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <vector>
#include <memory>
#include <sstream>

#include "types.h"
#include "arguments.h"
#include "reflist.h"
#include "textstream.h"

class SectionInfo;
class FileDef;

/** This class stores information about an inheritance relation
 */
struct BaseInfo
{
  /*! Creates an object representing an inheritance relation */
  BaseInfo(const QCString &n,Protection p,Specifier v) :
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
      CONCEPT_SEC      = 0x00000020,
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
      SERVICEDOC_SEC   = 0x00080000,
      SINGLETONDOC_SEC = 0x00100000,
      CONCEPTDOC_SEC   = 0x00200000,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC |
                         INTERFACEDOC_SEC | EXCEPTIONDOC_SEC | PROTOCOLDOC_SEC |
                         CATEGORYDOC_SEC | SERVICEDOC_SEC | SINGLETONDOC_SEC,

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
      DIRDOC_SEC       = 0x18000000,
      EXPORTED_INTERFACE_SEC = 0x19000000,
      INCLUDED_SERVICE_SEC   = 0x1A000000,
      EXAMPLE_LINENO_SEC     = 0x1B000000,
    };

    enum GroupDocType
    {
      GROUPDOC_NORMAL,        //!< defgroup
      GROUPDOC_ADD,           //!< addtogroup
      GROUPDOC_WEAK           //!< weakgroup
    };                        //!< kind of group

    Entry();
    Entry(const Entry &);
   ~Entry();

    /*! Returns the parent for this Entry or 0 if this entry has no parent. */
    Entry *parent() const { return m_parent; }

    /*! Returns the list of children for this Entry
     *  @see addSubEntry() and removeSubEntry()
     */
    const std::vector< std::shared_ptr<Entry> > &children() const { return m_sublist; }

    /*! @name add entry as a child and pass ownership.
     *  @note This makes the entry passed invalid!
     *  @{
     */
    void moveToSubEntryAndKeep(Entry* e);
    void moveToSubEntryAndKeep(std::shared_ptr<Entry> e);
    /*! @} */

    /*! @name add entry as a child, pass ownership and reinitialize entry */
    void moveToSubEntryAndRefresh(Entry* &e);
    void moveToSubEntryAndRefresh(std::shared_ptr<Entry> &e);

    /*! make a copy of \a e and add it as a child to this entry */
    void copyToSubEntry (Entry* e);
    void copyToSubEntry (const std::shared_ptr<Entry> &e);

    /*! Removes entry \a e from the list of children.
     *  The entry will be deleted if found.
     */
    void removeSubEntry(const Entry *e);

    /*! Restore the state of this Entry to the default value it has
     *  at construction time.
     */
    void reset();

    void markAsProcessed() const { ((Entry*)(this))->section = Entry::EMPTY_SEC; }
    void setFileDef(FileDef *fd);
    FileDef *fileDef() const { return m_fileDef; }

    // identification
    int          section;     //!< entry type (see Sections);
    QCString	 type;        //!< member type
    QCString	 name;        //!< member name
    bool         hasTagInfo;  //!< is tag info valid
    TagInfo      tagInfoData; //!< tag file info data
    const TagInfo *tagInfo() const { return hasTagInfo ? &tagInfoData : 0; }

    // content
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    Spec spec;                //!< class/member specifiers
    int  initLines;           //!< define/variable initializer lines to show
    bool stat;                //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    bool subGrouping;         //!< automatically group class members?
    bool callGraph;           //!< do we need to draw the call graph?
    bool callerGraph;         //!< do we need to draw the caller graph?
    bool referencedByRelation;//!< do we need to show the referenced by relation?
    bool referencesRelation;  //!< do we need to show the references relation?
    Specifier    virt;        //!< virtualness of the entry
    QCString     args;        //!< member argument string
    QCString     bitfields;   //!< member's bit fields
    ArgumentList argList;     //!< member arguments as a list
    ArgumentLists tArgLists;  //!< template argument declarations
    TextStream   program;     //!< the program text
    TextStream   initializer; //!< initial value (for variables)
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
    ArgumentList typeConstr;  //!< where clause (C#) for type constraints
    int          bodyLine;    //!< line number of the body in the source
    int          bodyColumn;  //!< column of the body in the source
    int          endBodyLine; //!< line number where the definition ends
    int          mGrpId;      //!< member group id
    std::vector<BaseInfo> extends; //!< list of base classes
    std::vector<Grouping> groups;  //!< list of groups this entry belongs to
    std::vector<const SectionInfo*> anchors; //!< list of anchors defined in this entry
    QCString	fileName;     //!< file this entry was extracted from
    int		startLine;    //!< start line of entry in the source
    int		startColumn;  //!< start column of entry in the source
    RefItemVector sli;        //!< special lists (test/todo/bug/deprecated/..) this entry is in
    SrcLangExt  lang;         //!< programming language in which this entry was found
    bool        hidden;       //!< does this represent an entity that is hidden from the output
    bool        artificial;   //!< Artificially introduced item
    GroupDocType groupDocType;
    QCString    id;           //!< libclang id
    LocalToc    localToc;
    QCString    metaData;     //!< Slice metadata
    QCString    req;          //!< C++20 requires clause

    /// return the command name used to define GROUPDOC_SEC
    const char *groupDocCmd() const
    {
      switch( groupDocType )
      {
        case GROUPDOC_NORMAL: return "\\defgroup";
        case GROUPDOC_ADD: return "\\addtogroup";
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
    Entry         *m_parent;    //!< parent node in the tree
    std::vector< std::shared_ptr<Entry> > m_sublist;
    Entry &operator=(const Entry &);
    FileDef       *m_fileDef;
};

typedef std::vector< std::shared_ptr<Entry> > EntryList;

#endif
