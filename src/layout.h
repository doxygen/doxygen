/******************************************************************************
 *
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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <memory>
#include <vector>
#include "types.h"

class LayoutParser;
struct LayoutNavEntry;
class MemberList;

/** @brief Base class representing a piece of a documentation page */
struct LayoutDocEntry
{
  virtual ~LayoutDocEntry() = default;
  enum Kind {
              // Generic items for all pages
              MemberGroups,
              MemberDeclStart, MemberDeclEnd, MemberDecl,
              MemberDefStart, MemberDefEnd, MemberDef,
              BriefDesc, DetailedDesc,
              AuthorSection,

              // Class specific items
              ClassIncludes, ClassInlineClasses,
              ClassInheritanceGraph, ClassNestedClasses,
              ClassCollaborationGraph, ClassAllMembersLink,
              ClassUsedFiles,

              // Concept specific items
              ConceptDefinition,

              // Namespace specific items
              NamespaceNestedNamespaces, NamespaceNestedConstantGroups,
              NamespaceClasses, NamespaceConcepts, NamespaceInterfaces, NamespaceStructs, NamespaceExceptions,
              NamespaceInlineClasses,

              // File specific items
              FileClasses, FileConcepts, FileInterfaces, FileStructs, FileExceptions, FileConstantGroups, FileNamespaces,
              FileIncludes, FileIncludeGraph,
              FileIncludedByGraph, FileSourceLink,
              FileInlineClasses,

              // Group specific items
              GroupClasses, GroupConcepts, GroupInlineClasses, GroupNamespaces,
              GroupDirs, GroupNestedGroups, GroupFiles,
              GroupGraph, GroupPageDocs,

              // Directory specific items
              DirSubDirs, DirFiles, DirGraph

            };
  virtual Kind kind() const = 0;
};

/** @brief Represents of a piece of a documentation page without configurable parts */
struct LayoutDocEntrySimple : LayoutDocEntry
{
  public:
    LayoutDocEntrySimple(Kind k) : m_kind(k) {}
    Kind kind() const { return m_kind; }
  private:
    Kind m_kind;
};

struct LayoutDocEntrySection: public LayoutDocEntrySimple
{
  LayoutDocEntrySection(Kind k,const QCString &tl) :
    LayoutDocEntrySimple(k), m_title(tl) {}
  QCString title(SrcLangExt lang) const;
private:
  QCString m_title;
};

/** @brief Represents of a member declaration list with configurable title and subtitle. */
struct LayoutDocEntryMemberDecl: public LayoutDocEntry
{
  LayoutDocEntryMemberDecl(MemberListType tp,
                           const QCString &tl,const QCString &ss)
    : type(tp), m_title(tl), m_subscript(ss) {}

  Kind kind() const { return MemberDecl; }
  MemberListType type;
  QCString title(SrcLangExt lang) const;
  QCString subtitle(SrcLangExt lang) const;
private:
  QCString m_title;
  QCString m_subscript;
};

/** @brief Represents of a member definition list with configurable title. */
struct LayoutDocEntryMemberDef: public LayoutDocEntry
{
  LayoutDocEntryMemberDef(MemberListType tp,const QCString &tl)
    : type(tp), m_title(tl) {}

  Kind kind() const { return MemberDef; }
  MemberListType type;
  QCString title(SrcLangExt lang) const;
private:
  QCString m_title;
};

using LayoutNavEntryList = std::vector< std::unique_ptr<LayoutNavEntry> >;

/** @brief Base class for the layout of a navigation item at the top of the HTML pages. */
struct LayoutNavEntry
{
  public:
    enum Kind {
      None = -1,
      MainPage,
      Pages,
      Modules,
      Namespaces,
      NamespaceList,
      NamespaceMembers,
      Concepts,
      Classes,
      ClassList,
      ClassIndex,
      ClassHierarchy,
      ClassMembers,
      Interfaces,
      InterfaceList,
      InterfaceIndex,
      InterfaceHierarchy,
      Structs,
      StructList,
      StructIndex,
      Exceptions,
      ExceptionList,
      ExceptionIndex,
      ExceptionHierarchy,
      Files,
      FileList,
      FileGlobals,
      Examples,
      User,
      UserGroup
    };
    LayoutNavEntry(LayoutNavEntry *parent,Kind k,bool vs,const QCString &bf,
                   const QCString &tl,const QCString &intro,bool prepend=false)
      : m_parent(parent), m_kind(k), m_visible(vs), m_baseFile(bf), m_title(tl), m_intro(intro)
    {
      if (parent)
      {
        if (prepend) parent->prependChild(this); else parent->addChild(this);
      }
    }
    LayoutNavEntry *parent() const   { return m_parent; }
    Kind kind() const                { return m_kind; }
    QCString baseFile() const        { return m_baseFile; }
    QCString title() const           { return m_title; }
    QCString intro() const           { return m_intro; }
    QCString url() const;
    bool visible()                   { return m_visible; }
    void clear()                     { m_children.clear(); }
    void addChild(LayoutNavEntry *e) { m_children.push_back(std::unique_ptr<LayoutNavEntry>(e)); }
    void prependChild(LayoutNavEntry *e) { m_children.insert(m_children.begin(),std::unique_ptr<LayoutNavEntry>(e)); }
    const LayoutNavEntryList &children() const { return m_children; }
    LayoutNavEntry *find(LayoutNavEntry::Kind k,const QCString &file=QCString()) const;

  private:
    LayoutNavEntry() : m_parent(0), m_kind(None), m_visible(true) {}
    LayoutNavEntry *m_parent;
    Kind m_kind;
    bool m_visible;
    QCString m_baseFile;
    QCString m_title;
    QCString m_intro;
    LayoutNavEntryList m_children;
    friend class LayoutDocManager;
};

using LayoutDocEntryList = std::vector< std::unique_ptr<LayoutDocEntry> >;

/** @brief Singleton providing access to the (user configurable) layout of the documentation */
class LayoutDocManager
{
    class Private;
  public:
    enum LayoutPart
    {
      Undefined = -1,
      Class, Concept, Namespace, File, Group, Directory,
      NrParts
    };
    /** Returns a reference to this singleton. */
    static LayoutDocManager &instance();

    /** Returns the list of LayoutDocEntry's in representation order for a given page identified by @a part. */
    const LayoutDocEntryList &docEntries(LayoutPart part) const;

    /** returns the (invisible) root of the navigation tree. */
    LayoutNavEntry *rootNavEntry() const;

    /** Parses a user provided layout */
    void parse(const QCString &fileName);
    void init();
  private:
    void addEntry(LayoutPart p,LayoutDocEntry*e);
    void clear(LayoutPart p);
    LayoutDocManager();
    ~LayoutDocManager();
    std::unique_ptr<Private> d;
    friend class LayoutParser;
};

void writeDefaultLayoutFile(const QCString &fileName);

#endif

