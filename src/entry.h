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
#include <functional>

#include "types.h"
#include "arguments.h"
#include "reflist.h"
#include "requirement.h"
#include "textstream.h"
#include "configvalues.h"

class SectionInfo;
class FileDef;

//--------------------------------------------------------------

#define COMMAND_OVERRIDES                                       \
  OVERRIDE_ENTRY(bool,          bool, 1, callGraph            ) \
  OVERRIDE_ENTRY(bool,          bool, 1, callerGraph          ) \
  OVERRIDE_ENTRY(bool,          bool, 1, referencedByRelation ) \
  OVERRIDE_ENTRY(bool,          bool, 1, referencesRelation   ) \
  OVERRIDE_ENTRY(bool,          bool, 1, inlineSource         ) \
  OVERRIDE_ENTRY(bool,          bool, 1, includeGraph         ) \
  OVERRIDE_ENTRY(bool,          bool, 1, includedByGraph      ) \
  OVERRIDE_ENTRY(bool,          bool, 1, directoryGraph       ) \
  OVERRIDE_ENTRY(bool,          bool, 1, collaborationGraph   ) \
  OVERRIDE_ENTRY(bool,          bool, 1, groupGraph           ) \
  OVERRIDE_ENTRY(bool,          bool, 1, enumValues           ) \
  OVERRIDE_ENTRY(CLASS_GRAPH_t, int,  3, inheritanceGraph     )

class CommandOverrides
{
  private:
#define OVERRIDE_ENTRY(type,store_type,bits,name)  \
    store_type m_##name          : bits;       \
    bool m_##name##ExplicitlySet : 1;
    COMMAND_OVERRIDES
#undef OVERRIDE_ENTRY

  public:
    CommandOverrides() { reset(); }
    void reset() { std::memset(this, 0, sizeof(*this)); }

    // conversions between type and store_type
    bool          to_store_type(bool t)           const { return t;                             }
    int           to_store_type(CLASS_GRAPH_t t)  const { return static_cast<int>(t);           }
    bool          from_store_type(bool t)         const { return t;                             }
    CLASS_GRAPH_t from_store_type(int t)          const { return static_cast<CLASS_GRAPH_t>(t); }

#define OVERRIDE_ENTRY(type,store_type,bits,name)              \
    void override_##name(type value) {                         \
      m_##name = to_store_type(value);                         \
      m_##name##ExplicitlySet = true;                          \
      /* printf("overrule_%s(%d) isSet=%d\n",#name,value,m_##name##ExplicitlySet); */ \
    }                                                          \
    void apply_##name(std::function<void(type)> func) const {  \
      /* printf("apply_%s(%d) isSet=%d\n",#name,m_##name,m_##name##ExplicitlySet); */ \
      if (m_##name##ExplicitlySet) func(from_store_type(m_##name)); \
    }
    COMMAND_OVERRIDES
#undef OVERRIDE_ENTRY

};

//--------------------------------------------------------------

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

    enum GroupDocType
    {
      GROUPDOC_NORMAL,        //!< defgroup
      GROUPDOC_ADD,           //!< addtogroup
      GROUPDOC_WEAK           //!< weakgroup
    };                        //!< kind of group

    Entry();
    Entry(const Entry &);
    Entry &operator=(const Entry &) = delete;
    Entry(Entry &&) = delete;
    Entry &operator=(Entry &&) = delete;
   ~Entry();

    /*! Returns the parent for this Entry or nullptr if this entry has no parent. */
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

    void markAsProcessed() const { (const_cast<Entry*>(this))->section = EntryType::makeEmpty(); }
    void setFileDef(FileDef *fd);
    FileDef *fileDef() const { return m_fileDef; }

    // identification
    EntryType    section;     //!< entry type (see Sections);
    QCString	 type;        //!< member type
    QCString	 name;        //!< member name
    bool         hasTagInfo;  //!< is tag info valid
    TagInfo      tagInfoData; //!< tag file info data
    const TagInfo *tagInfo() const { return hasTagInfo ? &tagInfoData : nullptr; }

    // content
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    TypeSpecifier spec;       //!< class/member specifiers
    VhdlSpecifier vhdlSpec;   //!< VHDL specifiers
    int  initLines;           //!< define/variable initializer lines to show
    bool isStatic;            //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    bool subGrouping;         //!< automatically group class members?
    bool exported;            //!< is the symbol exported from a C++20 module
    CommandOverrides commandOverrides; //!< store info for commands whose default can be overridden
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
    RequirementRefs rqli;     //!< references to requirements
    SrcLangExt  lang;         //!< programming language in which this entry was found
    bool        hidden;       //!< does this represent an entity that is hidden from the output
    bool        artificial;   //!< Artificially introduced item
    GroupDocType groupDocType;
    QCString    id;           //!< libclang id
    LocalToc    localToc;
    QCString    metaData;     //!< Slice metadata
    QCString    req;          //!< C++20 requires clause
    std::vector<std::string> qualifiers;  //!< qualifiers specified with the qualifier command

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
      if( !section.isGroupDoc() )
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
    FileDef       *m_fileDef;
};

typedef std::vector< std::shared_ptr<Entry> > EntryList;

#endif
