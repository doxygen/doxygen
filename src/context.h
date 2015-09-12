/******************************************************************************
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"
#include "template.h"
#include <qlist.h>
#include <stdio.h>

class Definition;
class ClassDef;
class ClassSDict;
class BaseClassList;
class PageDef;
class GroupDef;
class NamespaceDef;
class BaseClassList;
class NamespaceSDict;
class FileDef;
class FileList;
class FileNameList;
class DirSDict;
class DirList;
class DirDef;
class PageSDict;
class GroupSDict;
class GroupDef;
class GroupList;
struct IncludeInfo;
class MemberList;
class MemberSDict;
class MemberDef;
struct Argument;
class ArgumentList;
class MemberNameInfoSDict;
struct MemberInfo;
class MemberGroup;
class MemberGroupSDict;
class MemberGroupList;
class DotNode;
class DotGfxHierarchyTable;
struct SearchIndexInfo;
class SearchIndexList;
class SearchDefinitionList;

//----------------------------------------------------

#define DEBUG_REF 0

/** @brief Helper class to support reference counting */
#if DEBUG_REF
class RefCountedContext
{
  public:
    RefCountedContext(const char *className) : m_refCount(0)
    {
      m_className=className;
      m_insideRelease = FALSE;
    }
    virtual ~RefCountedContext()
    {
      if (!m_insideRelease) abort();
    }
    int addRef()
    {
      ++s_totalCount;
      printf("%p:%s::addRef()=%d\n",this,m_className.data(),m_refCount);
      return ++m_refCount;
    }
    int release()
    {
      --s_totalCount;
      printf("%p:%s::release()=%d\n",this,m_className.data(),m_refCount-1);
      int count = --m_refCount;
      if (count<=0)
      {
        m_insideRelease=TRUE;
        delete this;
      }
      return count;
    }
  private:
    int m_refCount;
    QCString m_className;
    bool m_insideRelease;
  public:
    static int s_totalCount;
};

#else // release version

class RefCountedContext
{
  public:
    RefCountedContext(const char *) : m_refCount(0) {}
    virtual ~RefCountedContext() {}
    int addRef() { return ++m_refCount; }
    int release()
    {
      int count = --m_refCount;
      if (count<=0)
      {
        delete this;
      }
      return count;
    }
  private:
    int m_refCount;
};
#endif


//----------------------------------------------------

class ConfigContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ConfigContext *alloc() { return new ConfigContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ConfigContext();
   ~ConfigContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class DoxygenContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static DoxygenContext *alloc() { return new DoxygenContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DoxygenContext();
    ~DoxygenContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class TranslateContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static TranslateContext *alloc() { return new TranslateContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    TranslateContext();
   ~TranslateContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class UsedFilesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static UsedFilesContext *alloc(ClassDef *cd) { return new UsedFilesContext(cd); }

    // TemplateListIntf
    virtual int count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addFile(FileDef *fd);

  private:
    UsedFilesContext(ClassDef *cd);
   ~UsedFilesContext();

    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static IncludeInfoContext *alloc(const IncludeInfo *info,SrcLangExt lang)
    { return new IncludeInfoContext(info,lang); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    IncludeInfoContext(const IncludeInfo *,SrcLangExt lang);
   ~IncludeInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static IncludeInfoListContext *alloc(const QList<IncludeInfo> &list,SrcLangExt lang)
    { return new IncludeInfoListContext(list,lang); }

    // TemplateListIntf
    virtual int count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    IncludeInfoListContext(const QList<IncludeInfo> &list,SrcLangExt lang);
   ~IncludeInfoListContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class ClassContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassContext *alloc(ClassDef *cd) { return new ClassContext(cd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassContext(ClassDef *);
   ~ClassContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceContext *alloc(NamespaceDef *nd) { return new NamespaceContext(nd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceContext(NamespaceDef *);
   ~NamespaceContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static FileContext *alloc(FileDef *fd) { return new FileContext(fd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileContext(FileDef *);
   ~FileContext();
    class Private;
    Private *p;
};
//----------------------------------------------------

class DirContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static DirContext *alloc(DirDef *dd) { return new DirContext(dd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DirContext(DirDef *);
   ~DirContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class PageContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static PageContext *alloc(PageDef *pd,bool isMainPage,bool isExample) { return new PageContext(pd,isMainPage,isExample); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    PageContext(PageDef *,bool isMainPage,bool isExample);
   ~PageContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberContext *alloc(MemberDef *md) { return new MemberContext(md); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberContext(MemberDef *);
   ~MemberContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class ModuleContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleContext *alloc(GroupDef *gd) { return new ModuleContext(gd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleContext(GroupDef *);
   ~ModuleContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ClassListContext *alloc() { return new ClassListContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassListContext();
   ~ClassListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassIndexContext *alloc() { return new ClassIndexContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassIndexContext();
   ~ClassIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritanceGraphContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritanceGraphContext *alloc(DotGfxHierarchyTable *hierarchy,DotNode *n,int id)
    { return new InheritanceGraphContext(hierarchy,n,id); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceGraphContext(DotGfxHierarchyTable *hierarchy,DotNode *n,int id);
   ~InheritanceGraphContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassInheritanceNodeContext *alloc(ClassDef *cd)
    { return new ClassInheritanceNodeContext(cd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addChildren(const BaseClassList *bcl,bool hideSuper);

  private:
    ClassInheritanceNodeContext(ClassDef *);
   ~ClassInheritanceNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ClassInheritanceContext *alloc() { return new ClassInheritanceContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassInheritanceContext();
   ~ClassInheritanceContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassHierarchyContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassHierarchyContext *alloc() { return new ClassHierarchyContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassHierarchyContext();
   ~ClassHierarchyContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestingNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NestingNodeContext *alloc(const NestingNodeContext *parent,Definition *def,
                                     int index,int level,bool addClasses,bool inherit,bool hideSuper)
    { return new NestingNodeContext(parent,def,index,level,addClasses,inherit,hideSuper); }

    QCString id() const;

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NestingNodeContext(const NestingNodeContext *parent,
                       Definition *,int index,int level,bool addClasses,bool inherit,bool hideSuper);
   ~NestingNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestingContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static NestingContext *alloc(const NestingNodeContext *parent,int level)
    { return new NestingContext(parent,level); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addNamespaces(const NamespaceSDict &nsDict,bool rootOnly,bool addClasses);
    void addClasses(const ClassSDict &clDict,bool rootOnly);
    void addDirs(const DirSDict &);
    void addDirs(const DirList &);
    void addFiles(const FileNameList &);
    void addFiles(const FileList &);
    void addPages(const PageSDict &pages,bool rootOnly);
    void addModules(const GroupSDict &modules);
    void addModules(const GroupList &modules);
    void addClassHierarchy(const ClassSDict &clDict,bool rootOnly);
    void addDerivedClasses(const BaseClassList *bcl,bool hideSuper);

  private:
    NestingContext(const NestingNodeContext *parent,int level);
   ~NestingContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassTreeContext *alloc() { return new ClassTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassTreeContext();
   ~ClassTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static NamespaceListContext *alloc() { return new NamespaceListContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceListContext();
   ~NamespaceListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceTreeContext *alloc() { return new NamespaceTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceTreeContext();
   ~NamespaceTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class DirListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static DirListContext *alloc() { return new DirListContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DirListContext();
   ~DirListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static FileListContext *alloc() { return new FileListContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileListContext();
   ~FileListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static FileTreeContext *alloc() { return new FileTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileTreeContext();
   ~FileTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static PageListContext *alloc(const PageSDict *pages) { return new PageListContext(pages); }

    // TemplateListIntf methods
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addPages(const PageSDict &pages);

  private:
    PageListContext(const PageSDict *pages);
   ~PageListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static PageTreeContext *alloc(const PageSDict *pages) { return new PageTreeContext(pages); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    PageTreeContext(const PageSDict *pages);
   ~PageTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleNodeContext *alloc(GroupDef *gd) { return new ModuleNodeContext(gd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleNodeContext(GroupDef *);
   ~ModuleNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ModuleListContext *alloc() { return new ModuleListContext(); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addModules(const GroupSDict &);
    void addModules(const GroupList &);

  private:
    ModuleListContext();
   ~ModuleListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleTreeContext *alloc() { return new ModuleTreeContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleTreeContext();
   ~ModuleTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ExampleListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ExampleListContext *alloc() { return new ExampleListContext; }

    // TemplateListIntf methods
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ExampleListContext();
   ~ExampleListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------


class ExampleTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ExampleTreeContext *alloc() { return new ExampleTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ExampleTreeContext();
   ~ExampleTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class GlobalsIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static GlobalsIndexContext *alloc() { return new GlobalsIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    GlobalsIndexContext();
   ~GlobalsIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassMembersIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassMembersIndexContext *alloc() { return new ClassMembersIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassMembersIndexContext();
   ~ClassMembersIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceMembersIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceMembersIndexContext *alloc() { return new NamespaceMembersIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceMembersIndexContext();
   ~NamespaceMembersIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NavPathElemContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NavPathElemContext *alloc(Definition *def) { return new NavPathElemContext(def); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NavPathElemContext(Definition *def);
   ~NavPathElemContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class InheritanceNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritanceNodeContext *alloc(ClassDef *cd,const QCString &name)
    { return new InheritanceNodeContext(cd,name); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceNodeContext(ClassDef *cd,const QCString &name);
   ~InheritanceNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritanceListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static InheritanceListContext *alloc(const BaseClassList *list,bool baseClasses)
    { return new InheritanceListContext(list,baseClasses); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceListContext(const BaseClassList *list,bool baseClasses);
   ~InheritanceListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static MemberListContext *alloc()
    { return new MemberListContext; }
    static MemberListContext *alloc(const MemberList *ml)
    { return new MemberListContext(ml); }
    static MemberListContext *alloc(MemberSDict *ml,bool doSort)
    { return new MemberListContext(ml,doSort); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberListContext();
    MemberListContext(const MemberList *ml);
    MemberListContext(MemberSDict *ml,bool doSort);
   ~MemberListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberGroupInfoContext *alloc(Definition *def,const QCString &relPath,const MemberGroup *mg)
    { return new MemberGroupInfoContext(def,relPath,mg); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberGroupInfoContext(Definition *def,const QCString &relPath,const MemberGroup *mg);
   ~MemberGroupInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static MemberGroupListContext *alloc()
    { return new MemberGroupListContext; }
    static MemberGroupListContext *alloc(Definition *def,const QCString &relPath,const MemberGroupList *list)
    { return new MemberGroupListContext(def,relPath,list); }
    static MemberGroupListContext *alloc(Definition *def,const QCString &relPath,const MemberGroupSDict *dict,bool subGrouping)
    { return new MemberGroupListContext(def,relPath,dict,subGrouping); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberGroupListContext();
    MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupList *list);
    MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupSDict *mgDict,bool subGrouping);
   ~MemberGroupListContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class MemberListInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberListInfoContext *alloc(Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString())
    { return new MemberListInfoContext(def,relPath,ml,title,subtitle); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberListInfoContext(Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString());
   ~MemberListInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberInfoContext *alloc(const MemberInfo *mi) { return new MemberInfoContext(mi); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberInfoContext(const MemberInfo *mi);
   ~MemberInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritedMemberInfoContext *alloc(ClassDef *cd,MemberList *ml,const QCString &title)
    { return new InheritedMemberInfoContext(cd,ml,title); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritedMemberInfoContext(ClassDef *cd,MemberList *ml,const QCString &title);
   ~InheritedMemberInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static InheritedMemberInfoListContext *alloc() { return new InheritedMemberInfoListContext; }
    void addMemberList(ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList=TRUE);

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritedMemberInfoListContext();
   ~InheritedMemberInfoListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class AllMembersListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static AllMembersListContext *alloc()
    { return new AllMembersListContext; }
    static AllMembersListContext *alloc(const MemberNameInfoSDict *ml)
    { return new AllMembersListContext(ml); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    AllMembersListContext();
    AllMembersListContext(const MemberNameInfoSDict *ml);
   ~AllMembersListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ArgumentContext *alloc(const Argument *arg,Definition *def,const QCString &relPath)
    { return new ArgumentContext(arg,def,relPath); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ArgumentContext(const Argument *arg,Definition *def,const QCString &relPath);
   ~ArgumentContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ArgumentListContext *alloc() { return new ArgumentListContext; }
    static ArgumentListContext *alloc(const ArgumentList *al,Definition *def,const QCString &relPath)
    { return new ArgumentListContext(al,def,relPath); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ArgumentListContext();
    ArgumentListContext(const ArgumentList *al,Definition *def,const QCString &relPath);
   ~ArgumentListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolContext *alloc(const Definition *def,const Definition *prev,const Definition *next)
    { return new SymbolContext(def,prev,next); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolContext(const Definition *def,const Definition *prev,const Definition *next);
   ~SymbolContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolListContext *alloc(const SearchDefinitionList *sdl)
    { return new SymbolListContext(sdl); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolListContext(const SearchDefinitionList *sdl);
   ~SymbolListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolGroupContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolGroupContext *alloc(const SearchDefinitionList *sdl)
    { return new SymbolGroupContext(sdl); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolGroupContext(const SearchDefinitionList *sdl);
   ~SymbolGroupContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolGroupListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolGroupListContext *alloc(const SearchIndexList *sil)
    { return new SymbolGroupListContext(sil); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolGroupListContext(const SearchIndexList *sil);
   ~SymbolGroupListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolIndexContext *alloc(const SearchIndexList *sl,const QCString &name)
    { return new SymbolIndexContext(sl,name); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolIndexContext(const SearchIndexList *sl,const QCString &name);
   ~SymbolIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolIndicesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolIndicesContext *alloc(const SearchIndexInfo *info)
    { return new SymbolIndicesContext(info); }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolIndicesContext(const SearchIndexInfo *info);
   ~SymbolIndicesContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SearchIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SearchIndexContext *alloc(const SearchIndexInfo *info)
    { return new SearchIndexContext(info); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SearchIndexContext(const SearchIndexInfo *info);
   ~SearchIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SearchIndicesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SearchIndicesContext *alloc() { return new SearchIndicesContext; }

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SearchIndicesContext();
   ~SearchIndicesContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

void generateOutputViaTemplate();

#endif
