/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"
#include "template.h"
#include "classdef.h"
#include "searchindex_js.h"
#include "memberlist.h"
#include "dotgfxhierarchytable.h"

class Definition;

class PageDef;
class PageLinkedMap;
class PageLinkedRefMap;

class GroupDef;
class GroupLinkedMap;
class GroupList;

class ConceptDef;
class ConceptLinkedMap;
class ConceptLinkedRefMap;

class NamespaceDef;
class NamespaceLinkedMap;
class NamespaceLinkedRefMap;

class FileNameLinkedMap;
class ClassLinkedMap;
class MemberNameInfoLinkedMap;

class DirDef;
class DirLinkedMap;
class DirList;

class FileDef;
class FileList;

struct IncludeInfo;
class IncludeInfoList;

class MemberList;
class MemberDef;
struct Argument;
class ArgumentList;
class MemberInfo;
class DotNode;

class MemberGroup;
class MemberGroupList;
class MemberGroupRefList;
class MemberVector;

//----------------------------------------------------

class ConfigContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ConfigContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ConfigContext();
    virtual ~ConfigContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class DoxygenContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<DoxygenContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    DoxygenContext();
    virtual ~DoxygenContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class TranslateContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<TranslateContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    TranslateContext();
    virtual ~TranslateContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class UsedFilesContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const ClassDef *cd)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<UsedFilesContext>(cd)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    void addFile(const FileDef *fd);
    UsedFilesContext(const ClassDef *cd);
    virtual ~UsedFilesContext();

  private:

    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class IncludeInfoContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const IncludeInfo *info,SrcLangExt lang)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<IncludeInfoContext>(info,lang)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    IncludeInfoContext(const IncludeInfo *,SrcLangExt lang);
    virtual ~IncludeInfoContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class IncludeInfoListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const IncludeInfoList &list,SrcLangExt lang)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<IncludeInfoListContext>(list,lang)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    IncludeInfoListContext(const IncludeInfoList &list,SrcLangExt lang);
    virtual ~IncludeInfoListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const ClassDef *cd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassContext>(cd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ClassContext(const ClassDef *);
    virtual ~ClassContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};


//----------------------------------------------------

class ConceptContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const ConceptDef *cd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ConceptContext>(cd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ConceptContext(const ConceptDef *);
    virtual ~ConceptContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NamespaceContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const NamespaceDef *nd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<NamespaceContext>(nd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    NamespaceContext(const NamespaceDef *);
    virtual ~NamespaceContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class FileContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const FileDef *fd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<FileContext>(fd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    FileContext(const FileDef *);
    virtual ~FileContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};
//----------------------------------------------------

class DirContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const DirDef *dd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<DirContext>(dd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    DirContext(const DirDef *);
    virtual ~DirContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};


//----------------------------------------------------

class PageContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const PageDef *pd,bool isMainPage,bool isExample)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<PageContext>(pd,isMainPage,isExample)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    PageContext(const PageDef *,bool isMainPage,bool isExample);
    virtual ~PageContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class MemberContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const MemberDef *md)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<MemberContext>(md)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    MemberContext(const MemberDef *);
    virtual ~MemberContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};


//----------------------------------------------------

class ModuleContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const GroupDef *gd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ModuleContext>(gd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ModuleContext(const GroupDef *);
    virtual ~ModuleContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ClassListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    ClassListContext();
    virtual ~ClassListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassIndexContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ClassIndexContext();
    virtual ~ClassIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class InheritanceGraphContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(DotGfxHierarchyTablePtr hierarchy,DotNode *n,int id)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<InheritanceGraphContext>(hierarchy,n,id)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    InheritanceGraphContext(DotGfxHierarchyTablePtr hierarchy,DotNode *n,int id);
    virtual ~InheritanceGraphContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassInheritanceNodeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const ClassDef *cd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassInheritanceNodeContext>(cd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    void addChildren(const BaseClassList &bcl,bool hideSuper);
    ClassInheritanceNodeContext(const ClassDef *);
    virtual ~ClassInheritanceNodeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassInheritanceContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ClassInheritanceContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    ClassInheritanceContext();
    virtual ~ClassInheritanceContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassHierarchyContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassHierarchyContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ClassHierarchyContext();
    virtual ~ClassHierarchyContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

enum class ContextTreeType
{
  Namespace,           // NamespaceTreeContext
  ClassInheritance,    // ClassHierarchyContext
  ClassNesting,        // ClassTreeContext
  Module,              // ModuleTreeContext
  File,                // FileTreeContext
  Page,                // PageTreeContext
  Concept,             // ContextTreeContext
  Example              // ExampleTreeContext
};

class NestingNodeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const NestingNodeContext *parent,ContextTreeType type,
                                     const Definition *def,
                                     int index,int level,
                                     bool addClasses,bool addConcepts,
                                     bool inherit,bool hideSuper,
                                     ClassDefSet &visitedClasses)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<NestingNodeContext>
             (parent,type,def,index,level,addClasses,addConcepts,inherit,hideSuper,visitedClasses)); }

    QCString id() const;

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    NestingNodeContext(const NestingNodeContext *parent,
                       ContextTreeType type,
                       const Definition *,int index,int level,
                       bool addClasses,bool addConcepts,
                       bool inherit,bool hideSuper,
                       ClassDefSet &visitedClasses);
    virtual ~NestingNodeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NestingContext : public TemplateListIntf
{
  public:

    static TemplateListIntfPtr alloc(const NestingNodeContext *parent,ContextTreeType type,int level)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<NestingContext>(parent,type,level)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    void addNamespaces(const NamespaceLinkedMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses);
    void addNamespaces(const NamespaceLinkedRefMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses);
    void addClasses(const ClassLinkedMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addClasses(const ClassLinkedRefMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addConcepts(const ConceptLinkedMap &cnLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addConcepts(const ConceptLinkedRefMap &cnLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addDirs(const DirLinkedMap &,ClassDefSet &visitedClasses);
    void addDirs(const DirList &,ClassDefSet &visitedClasses);
    void addFiles(const FileNameLinkedMap &,ClassDefSet &visitedClasses);
    void addFiles(const FileList &,ClassDefSet &visitedClasses);
    void addPages(const PageLinkedMap &pages,bool rootOnly,ClassDefSet &visitedClasses);
    void addPages(const PageLinkedRefMap &pages,bool rootOnly,ClassDefSet &visitedClasses);
    void addModules(const GroupLinkedMap &modules,ClassDefSet &visitedClasses);
    void addModules(const GroupList &modules,ClassDefSet &visitedClasses);
    void addClassHierarchy(const ClassLinkedMap &clLinkedMap,ClassDefSet &visitedClasses);
    void addDerivedClasses(const BaseClassList &bcl,bool hideSuper,ClassDefSet &visitedClasses);
    void addMembers(const MemberVector &mv,ClassDefSet &visitedClasses);

    NestingContext(const NestingNodeContext *parent,ContextTreeType type,int level);
    virtual ~NestingContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ClassTreeContext();
    virtual ~ClassTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ConceptListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ConceptListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    ConceptListContext();
    virtual ~ConceptListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NamespaceListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<NamespaceListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    NamespaceListContext();
    virtual ~NamespaceListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NamespaceTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<NamespaceTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    NamespaceTreeContext();
    virtual ~NamespaceTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class DirListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<DirListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    DirListContext();
    virtual ~DirListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class FileListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<FileListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    FileListContext();
    virtual ~FileListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class FileTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<FileTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    FileTreeContext();
    virtual ~FileTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class PageListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const PageLinkedMap &pages)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<PageListContext>(pages)); }

    // TemplateListIntf methods
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    void addPages(const PageLinkedMap &pages);

    PageListContext(const PageLinkedMap &pages);
    virtual ~PageListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class PageTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const PageLinkedMap &pages)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<PageTreeContext>(pages)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    PageTreeContext(const PageLinkedMap &pages);
    virtual ~PageTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ModuleNodeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const GroupDef *gd)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ModuleNodeContext>(gd)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ModuleNodeContext(const GroupDef *);
    virtual ~ModuleNodeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ModuleListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ModuleListContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    void addModules(const GroupLinkedMap &);
    void addModules(const GroupList &);

    ModuleListContext();
    virtual ~ModuleListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ConceptTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ConceptTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ConceptTreeContext();
    virtual ~ConceptTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ModuleTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ModuleTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ModuleTreeContext();
    virtual ~ModuleTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ExampleListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ExampleListContext>()); }

    // TemplateListIntf methods
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    ExampleListContext();
    virtual ~ExampleListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------


class ExampleTreeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ExampleTreeContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ExampleTreeContext();
    virtual ~ExampleTreeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class GlobalsIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<GlobalsIndexContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    GlobalsIndexContext();
    virtual ~GlobalsIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ClassMembersIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ClassMembersIndexContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ClassMembersIndexContext();
    virtual ~ClassMembersIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NamespaceMembersIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc()
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<NamespaceMembersIndexContext>()); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    NamespaceMembersIndexContext();
    virtual ~NamespaceMembersIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class NavPathElemContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const Definition *def)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<NavPathElemContext>(def)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    NavPathElemContext(const Definition *def);
    virtual ~NavPathElemContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};


//----------------------------------------------------

class InheritanceNodeContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const ClassDef *cd,const QCString &name)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<InheritanceNodeContext>(cd,name)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    InheritanceNodeContext(const ClassDef *cd,const QCString &name);
    virtual ~InheritanceNodeContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class InheritanceListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const BaseClassList &list,bool baseClasses)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<InheritanceListContext>(list,baseClasses)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    InheritanceListContext(const BaseClassList &list,bool baseClasses);
    virtual ~InheritanceListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class MemberListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberListContext>()); }
    static TemplateListIntfPtr alloc(const MemberList *ml)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberListContext>(ml)); }
    static TemplateListIntfPtr alloc(const MemberVector &ml)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberListContext>(ml)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    MemberListContext();
    MemberListContext(const MemberList *ml);
    MemberListContext(const MemberVector &ml);
    virtual ~MemberListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class MemberGroupInfoContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const Definition *def,const QCString &relPath,const MemberGroup *mg)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<MemberGroupInfoContext>(def,relPath,mg)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    MemberGroupInfoContext(const Definition *def,const QCString &relPath,const MemberGroup *mg);
    virtual ~MemberGroupInfoContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class MemberGroupListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberGroupListContext>()); }
    static TemplateListIntfPtr alloc(const Definition *def,const QCString &relPath,const MemberGroupRefList &list)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberGroupListContext>(def,relPath,list)); }
    static TemplateListIntfPtr alloc(const Definition *def,const QCString &relPath,const MemberGroupList &list,bool subGrouping)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<MemberGroupListContext>(def,relPath,list,subGrouping)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    MemberGroupListContext();
    MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupRefList &list);
    MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupList &list,bool subGrouping);
    virtual ~MemberGroupListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};


//----------------------------------------------------

class MemberListInfoContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString())
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<MemberListInfoContext>(def,relPath,ml,title,subtitle)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    MemberListInfoContext(const Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString());
    virtual ~MemberListInfoContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class MemberInfoContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const MemberInfo *mi)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<MemberInfoContext>(mi)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    MemberInfoContext(const MemberInfo *mi);
    virtual ~MemberInfoContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class InheritedMemberInfoContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const ClassDef *cd,std::unique_ptr<MemberList> &&ml,const QCString &title)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<InheritedMemberInfoContext>(cd,std::move(ml),title)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    InheritedMemberInfoContext(const ClassDef *cd,std::unique_ptr<MemberList> &&ml,const QCString &title);
    virtual ~InheritedMemberInfoContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class InheritedMemberInfoListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<InheritedMemberInfoListContext>()); }
    void addMemberList(const ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList=TRUE);

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    InheritedMemberInfoListContext();
    virtual ~InheritedMemberInfoListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class AllMembersListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const MemberNameInfoLinkedMap &ml)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<AllMembersListContext>(ml)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    AllMembersListContext(const MemberNameInfoLinkedMap &ml);
    virtual ~AllMembersListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ArgumentContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const Argument &arg,const Definition *def,const QCString &relPath)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<ArgumentContext>(arg,def,relPath)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    ArgumentContext(const Argument &arg,const Definition *def,const QCString &relPath);
    virtual ~ArgumentContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class ArgumentListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ArgumentListContext>()); }
    static TemplateListIntfPtr alloc(const ArgumentList &al,const Definition *def,const QCString &relPath)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<ArgumentListContext>(al,def,relPath)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    ArgumentListContext();
    ArgumentListContext(const ArgumentList &al,const Definition *def,const QCString &relPath);
    virtual ~ArgumentListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const Definition *def,const Definition *prev,const Definition *next)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<SymbolContext>(def,prev,next)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    SymbolContext(const Definition *def,const Definition *prev,const Definition *next);
    virtual ~SymbolContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const SearchIndexList::const_iterator &start,
                                    const SearchIndexList::const_iterator &end)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<SymbolListContext>(start,end)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    SymbolListContext(const SearchIndexList::const_iterator &start,
                      const SearchIndexList::const_iterator &end);
    virtual ~SymbolListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolGroupContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const SearchIndexList::const_iterator &start,
                                     const SearchIndexList::const_iterator &end)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<SymbolGroupContext>(start,end)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    SymbolGroupContext(const SearchIndexList::const_iterator &start,
                       const SearchIndexList::const_iterator &end);
    virtual ~SymbolGroupContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolGroupListContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const SearchIndexList &sil)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<SymbolGroupListContext>(sil)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    SymbolGroupListContext(const SearchIndexList &sil);
    virtual ~SymbolGroupListContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const std::string &letter,
                                     const SearchIndexList &sl,const QCString &name)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<SymbolIndexContext>(letter,sl,name)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    SymbolIndexContext(const std::string &letter,const SearchIndexList &sl,const QCString &name);
    virtual ~SymbolIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SymbolIndicesContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc(const SearchIndexInfo &info)
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<SymbolIndicesContext>(info)); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    SymbolIndicesContext(const SearchIndexInfo &info);
    virtual ~SymbolIndicesContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SearchIndexContext : public TemplateStructIntf
{
  public:
    static TemplateStructIntfPtr alloc(const SearchIndexInfo &info)
    { return std::static_pointer_cast<TemplateStructIntf>(std::make_shared<SearchIndexContext>(info)); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    SearchIndexContext(const SearchIndexInfo &info);
    virtual ~SearchIndexContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

class SearchIndicesContext : public TemplateListIntf
{
  public:
    static TemplateListIntfPtr alloc()
    { return std::static_pointer_cast<TemplateListIntf>(std::make_shared<SearchIndicesContext>()); }

    // TemplateListIntf
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    SearchIndicesContext();
    virtual ~SearchIndicesContext();

  private:
    class Private;
    std::unique_ptr<Private> p;
};

//----------------------------------------------------

void generateOutputViaTemplate();
void generateTemplateFiles(const QCString &templateDir);

#endif
