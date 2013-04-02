/******************************************************************************
 *
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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <qlist.h>
#include "types.h"

class LayoutParser;
class MemberList;
class QTextStream;

/** @brief Base class representing a piece of a documentation page */
struct LayoutDocEntry
{
  virtual ~LayoutDocEntry() {}
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

              // Namespace specific items
              NamespaceNestedNamespaces, NamespaceNestedConstantGroups,
              NamespaceClasses, NamespaceInlineClasses,

              // File specific items
              FileClasses, FileNamespaces, FileConstantGroups,
              FileIncludes, FileIncludeGraph, 
              FileIncludedByGraph, FileSourceLink,
              FileInlineClasses,

              // Group specific items
              GroupClasses, GroupInlineClasses, GroupNamespaces,
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

/** @brief Base class for the layout of a navigation item at the top of the HTML pages. */
struct LayoutNavEntry 
{
  public:
    enum Kind { 
      MainPage, 
      Pages,
      Modules, 
      Namespaces, 
      NamespaceList,
      NamespaceMembers,
      Classes,
      ClassList, 
      ClassIndex, 
      ClassHierarchy, 
      ClassMembers,
      Files, 
      FileList,
      FileGlobals,
      //Dirs, 
      Examples,
      User,
      UserGroup
    };
    LayoutNavEntry(LayoutNavEntry *parent,Kind k,bool vs,const QCString &bf, 
                   const QCString &tl,const QCString &intro,bool prepend=FALSE) 
      : m_parent(parent), m_kind(k), m_visible(vs), m_baseFile(bf), m_title(tl), m_intro(intro)
    { m_children.setAutoDelete(TRUE); 
      if (parent) { if (prepend) parent->prependChild(this); else parent->addChild(this); }
    }
    LayoutNavEntry *parent() const   { return m_parent; }
    Kind kind() const                { return m_kind; }
    QCString baseFile() const        { return m_baseFile; }
    QCString title() const           { return m_title; }
    QCString intro() const           { return m_intro; }
    QCString url() const;
    bool visible()                   { return m_visible; }
    void clear()                     { m_children.clear(); }
    void addChild(LayoutNavEntry *e) { m_children.append(e); }
    void prependChild(LayoutNavEntry *e) { m_children.prepend(e); }
    const QList<LayoutNavEntry> &children() const { return m_children; }
    LayoutNavEntry *find(LayoutNavEntry::Kind k,const char *file=0) const;

  private:
    LayoutNavEntry() : m_parent(0) {}
    LayoutNavEntry *m_parent;
    Kind m_kind;
    bool m_visible;
    QCString m_baseFile;
    QCString m_title;
    QCString m_intro;
    QList<LayoutNavEntry> m_children;
    friend class LayoutDocManager;
};

/** @brief Singleton providing access to the (user configurable) layout of the documentation */
class LayoutDocManager
{
    class Private;
  public:
    enum LayoutPart
    {
      Class, Namespace, File, Group, Directory,
      NrParts
    };
    /** Returns a reference to this singleton. */
    static LayoutDocManager &instance();

    /** Returns the list of LayoutDocEntry's in representation order for a given page identified by @a part. */
    const QList<LayoutDocEntry> &docEntries(LayoutPart part) const;

    /** returns the (invisible) root of the navigation tree. */
    LayoutNavEntry *rootNavEntry() const;

    /** Parses a user provided layout */
    void parse(QTextStream &t,const char *fileName);
    void init();
  private:
    void addEntry(LayoutPart p,LayoutDocEntry*e);
    void clear(LayoutPart p);
    LayoutDocManager();
    ~LayoutDocManager();
    Private *d;
    friend class LayoutParser;
};

void writeDefaultLayoutFile(const char *fileName);

#endif

