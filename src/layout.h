/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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
#include <utility>

#include "types.h"
#include "construct.h"

class LayoutParser;
struct LayoutNavEntry;
class MemberList;

/** @brief Base class representing a piece of a documentation page */
struct LayoutDocEntry
{
  ABSTRACT_BASE_CLASS(LayoutDocEntry)

#define ENTRY_SPECIFICATIONS \
              /* Generic items for all pages */ \
              ESPEC(MemberGroups) \
              ESPEC(MemberDeclStart) ESPEC(MemberDeclEnd) ESPEC(MemberDecl) \
              ESPEC(MemberDefStart) ESPEC(MemberDefEnd) ESPEC(MemberDef) \
              ESPEC(BriefDesc) ESPEC(DetailedDesc) \
              ESPEC(AuthorSection) \
              /* Class specific items */ \
              ESPEC(ClassIncludes) ESPEC(ClassInlineClasses) \
              ESPEC(ClassInheritanceGraph) ESPEC(ClassNestedClasses) \
              ESPEC(ClassCollaborationGraph) ESPEC(ClassAllMembersLink) \
              ESPEC(ClassUsedFiles) \
              /* Concept specific items */ \
              ESPEC(ConceptDefinition) \
              /* Namespace specific items */ \
              ESPEC(NamespaceNestedNamespaces) ESPEC(NamespaceNestedConstantGroups) \
              ESPEC(NamespaceClasses) ESPEC(NamespaceConcepts) ESPEC(NamespaceInterfaces) ESPEC(NamespaceStructs) ESPEC(NamespaceExceptions) \
              ESPEC(NamespaceInlineClasses) \
              /* File specific items */ \
              ESPEC(FileClasses) ESPEC(FileConcepts) ESPEC(FileInterfaces) ESPEC(FileStructs) ESPEC(FileExceptions) ESPEC(FileConstantGroups) ESPEC(FileNamespaces) \
              ESPEC(FileIncludes) ESPEC(FileIncludeGraph) \
              ESPEC(FileIncludedByGraph) ESPEC(FileSourceLink) \
              ESPEC(FileInlineClasses) \
              /* C++20 Modules */ \
              ESPEC(ModuleExports) ESPEC(ModuleClasses) ESPEC(ModuleConcepts) ESPEC(ModuleUsedFiles) \
              /* Group specific items */ \
              ESPEC(GroupClasses) ESPEC(GroupConcepts) ESPEC(GroupModules) ESPEC(GroupInlineClasses) ESPEC(GroupNamespaces) \
              ESPEC(GroupDirs) ESPEC(GroupNestedGroups) ESPEC(GroupFiles) \
              ESPEC(GroupGraph) ESPEC(GroupPageDocs) \
              /* Directory specific items */ \
              ESPEC(DirSubDirs) ESPEC(DirFiles) ESPEC(DirGraph)

  enum Kind {
#define ESPEC(x) x,
      ENTRY_SPECIFICATIONS
#undef ESPEC
            };
  virtual Kind kind() const = 0;
  std::string entryToString() const
  {
    switch (kind())
    {
#define ESPEC(x) case x: return #x; break;
      ENTRY_SPECIFICATIONS
#undef ESPEC
      default: return "unknown"; // to satisfy compiler
    }
  }
  virtual std::string id() const = 0;
  virtual bool visible() const = 0;
};

/** @brief Represents of a piece of a documentation page without configurable parts */
struct LayoutDocEntrySimple : LayoutDocEntry
{
  public:
    LayoutDocEntrySimple(Kind k,const std::string &id, bool v) : m_kind(k), m_id(id), m_visible(v) {}
    Kind kind() const override { return m_kind; }
    bool visible() const override { return m_visible; }
    std::string id() const override { return m_id; }
  private:
    Kind m_kind;
    std::string m_id;
    bool m_visible;
};

struct LayoutDocEntrySection: public LayoutDocEntrySimple
{
  LayoutDocEntrySection(Kind k,const std::string &id,const QCString &tl,bool v) :
    LayoutDocEntrySimple(k,id,v), m_title(tl) {}
  QCString title(SrcLangExt lang) const;
private:
  QCString m_title;
};

/** @brief Represents of a member declaration list with configurable title and subtitle. */
struct LayoutDocEntryMemberDecl: public LayoutDocEntry
{
  LayoutDocEntryMemberDecl(MemberListType tp,const std::string &id,
                           const QCString &tl,const QCString &ss, bool v)
    : type(tp), m_id(id), m_title(tl), m_subscript(ss), m_visible(v) {}

  Kind kind() const override { return MemberDecl; }
  MemberListType type;
  QCString title(SrcLangExt lang) const;
  QCString subtitle(SrcLangExt lang) const;
  bool visible() const override { return m_visible; }
  std::string id() const override { return m_id; }
private:
  std::string m_id;
  QCString m_title;
  QCString m_subscript;
  bool m_visible;
};

/** @brief Represents of a member definition list with configurable title. */
struct LayoutDocEntryMemberDef: public LayoutDocEntry
{
  LayoutDocEntryMemberDef(MemberListType tp,const std::string &id, const QCString &tl,bool v)
    : type(tp), m_id(id), m_title(tl), m_visible(v) {}

  Kind kind() const override { return MemberDef; }
  MemberListType type;
  QCString title(SrcLangExt lang) const;
  bool visible() const override { return m_visible; }
  std::string id() const override { return m_id; }
private:
  std::string m_id;
  QCString m_title;
  bool m_visible;
};

using LayoutDocEntryPtr  = std::unique_ptr<LayoutDocEntry>;
using LayoutDocEntryList = std::vector<LayoutDocEntryPtr>;

//-------------------------------------------------------------------------------------------------

using LayoutNavEntryList = std::vector< std::unique_ptr<LayoutNavEntry> >;

/** @brief Base class for the layout of a navigation item at the top of the HTML pages. */
struct LayoutNavEntry
{
#define NAV_SPECIFICATIONS \
      NSPEC(None, = -1) \
      NSPEC(MainPage,) \
      NSPEC(Pages,) \
      NSPEC(Modules,) \
      NSPEC(ModuleList,) \
      NSPEC(ModuleMembers,) \
      NSPEC(Topics,) \
      NSPEC(Namespaces,) \
      NSPEC(NamespaceList,) \
      NSPEC(NamespaceMembers,) \
      NSPEC(Concepts,) \
      NSPEC(Classes,) \
      NSPEC(ClassList,) \
      NSPEC(ClassIndex,) \
      NSPEC(ClassHierarchy,) \
      NSPEC(ClassMembers,) \
      NSPEC(Interfaces,) \
      NSPEC(InterfaceList,) \
      NSPEC(InterfaceIndex,) \
      NSPEC(InterfaceHierarchy,) \
      NSPEC(Structs,) \
      NSPEC(StructList,) \
      NSPEC(StructIndex,) \
      NSPEC(Exceptions,) \
      NSPEC(ExceptionList,) \
      NSPEC(ExceptionIndex,) \
      NSPEC(ExceptionHierarchy,) \
      NSPEC(Files,) \
      NSPEC(FileList,) \
      NSPEC(FileGlobals,) \
      NSPEC(Examples,) \
      NSPEC(User,) \
      NSPEC(UserGroup,)

  public:
    enum Kind {
#define NSPEC(x,y) x y,
      NAV_SPECIFICATIONS
#undef NSPEC
    };
    std::string navToString() const
    {
      switch (kind())
      {
#define NSPEC(x,y) case x: return #x; break;
      NAV_SPECIFICATIONS
#undef NSPEC
        default: return "unknown"; // to satisfy compiler
      }
    }

    LayoutNavEntry(LayoutNavEntry *parent,Kind k,bool vs,const QCString &bf,
                   const QCString &tl,const QCString &intro)
      : m_parent(parent), m_kind(k), m_visible(vs), m_baseFile(bf), m_title(tl), m_intro(intro) {}
    LayoutNavEntry *parent() const   { return m_parent; }
    Kind kind() const                { return m_kind; }
    QCString baseFile() const        { return m_baseFile; }
    std::string id() const           { return navToString()+":"+m_baseFile.str(); }
    QCString title() const           { return m_title; }
    QCString intro() const           { return m_intro; }
    QCString url() const;
    const LayoutNavEntryList &children() const { return m_children; }
    LayoutNavEntryList &children()   { return m_children; }
    void setVisible(bool v)          { m_visible = v; }
    bool visible() const             { return m_visible; }
    void insertChild(size_t pos,std::unique_ptr<LayoutNavEntry> &&e);
    void appendChild(std::unique_ptr<LayoutNavEntry> &&e);
    void updateVisibility(LayoutNavEntry *parent);
    LayoutNavEntry *find(LayoutNavEntry::Kind k,const QCString &file=QCString()) const;
    void swap(LayoutNavEntry &other)
    {
      std::swap(m_parent,other.m_parent);
      std::swap(m_kind,other.m_kind);
      std::swap(m_visible,other.m_visible);
      std::swap(m_baseFile,other.m_baseFile);
      std::swap(m_title,other.m_title);
      std::swap(m_intro,other.m_intro);
      std::swap(m_children,other.m_children);
      // reparent children
      for (auto &child : m_children)
      {
        child->m_parent = this;
      }
    }

  private:
    LayoutNavEntry() : m_parent(nullptr), m_kind(None), m_visible(true) {}
    LayoutNavEntry *m_parent;
    Kind m_kind;
    bool m_visible;
    QCString m_baseFile;
    QCString m_title;
    QCString m_intro;
    LayoutNavEntryList m_children;
    friend class LayoutDocManager;
};

#define PART_SPECIFICATIONS \
  PSPEC(Undefined, = -1)     \
  PSPEC(Class,) PSPEC(Concept,) PSPEC(Namespace,) PSPEC(File,) PSPEC(Group,) PSPEC(Directory,) PSPEC(Module,)       \
  PSPEC(NrParts,)
/** @brief Singleton providing access to the (user configurable) layout of the documentation */
class LayoutDocManager
{
    class Private;
  public:
    enum LayoutPart
    {
#define PSPEC(x,y) x y,
      PART_SPECIFICATIONS
#undef PSPEC
    };
    /** Returns a reference to this singleton. */
    static LayoutDocManager &instance();
    static std::string partToString(int k)
    {
      switch (k)
      {
#define PSPEC(x,y) case x: return #x; break;
        PART_SPECIFICATIONS
#undef PSPEC
        default: return "unknown"; // to satisfy compiler
      }
    }
    /** Returns the list of LayoutDocEntry's in representation order for a given page identified by @a part. */
    const LayoutDocEntryList &docEntries(LayoutPart part) const;

    /** returns the (invisible) root of the navigation tree. */
    LayoutNavEntry *rootNavEntry() const;
    /** append a new node as a child to root. */
    LayoutNavEntry *createChildNavEntry(LayoutNavEntry *root,
                                        LayoutNavEntry::Kind k,bool vs,const QCString &bf,const QCString &tl,const QCString &intro);

    /** Parses a user provided layout */
    void parse(const QCString &fileName, const char* data = nullptr);
    void init();
    int majorVersion() const;
    int minorVersion() const;
  private:
    void addEntry(LayoutPart p,LayoutDocEntryPtr &&e);
    void mergeNavEntries(LayoutDocManager &manager);
    void mergeDocEntries(const QCString &fileName,LayoutDocManager &manager);
    void removeInvisibleDocEntries();
    LayoutDocManager();
    ~LayoutDocManager();
    NON_COPYABLE(LayoutDocManager)
    std::unique_ptr<Private> d;
    friend class LayoutParser;
};

void writeDefaultLayoutFile(const QCString &fileName);
void printLayout();

#endif

