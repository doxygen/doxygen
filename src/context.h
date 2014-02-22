#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"
#include "template.h"
#include <qlist.h>

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

//----------------------------------------------------

class ConfigContext : public TemplateStructIntf
{
  public:
    ConfigContext();
   ~ConfigContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class DoxygenContext : public TemplateStructIntf
{
  public:
    DoxygenContext();
   ~DoxygenContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class TranslateContext : public TemplateStructIntf
{
  public:
    TranslateContext();
   ~TranslateContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class UsedFilesContext : public TemplateListIntf
{
  public:
    UsedFilesContext(ClassDef *cd);
   ~UsedFilesContext();

    // TemplateListIntf
    virtual int count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void addFile(FileDef *fd);

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoContext : public TemplateStructIntf
{
  public:
    IncludeInfoContext(const IncludeInfo *,SrcLangExt lang);
   ~IncludeInfoContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoListContext : public TemplateListIntf
{
  public:
    IncludeInfoListContext(const QList<IncludeInfo> &list,SrcLangExt lang);
   ~IncludeInfoListContext();

    // TemplateListIntf
    virtual int count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};


//----------------------------------------------------

class ClassContext : public TemplateStructIntf
{
  public:
    ClassContext(ClassDef *);
   ~ClassContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceContext : public TemplateStructIntf
{
  public:
    NamespaceContext(NamespaceDef *);
   ~NamespaceContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileContext : public TemplateStructIntf
{
  public:
    FileContext(FileDef *);
   ~FileContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};
//----------------------------------------------------

class DirContext : public TemplateStructIntf
{
  public:
    DirContext(DirDef *);
   ~DirContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};


//----------------------------------------------------

class PageContext : public TemplateStructIntf
{
  public:
    PageContext(PageDef *);
   ~PageContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberContext : public TemplateStructIntf
{
  public:
    MemberContext(MemberDef *);
   ~MemberContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};


//----------------------------------------------------

class ModuleContext : public TemplateStructIntf
{
  public:
    ModuleContext(GroupDef *);
   ~ModuleContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestedClassListContext : public TemplateListIntf
{
  public:
    NestedClassListContext();
   ~NestedClassListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void append(ClassDef *cd);

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestedNamespaceListContext : public TemplateListIntf
{
  public:
    NestedNamespaceListContext();
   ~NestedNamespaceListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void append(NamespaceDef *cd);

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassListContext : public TemplateListIntf
{
  public:
    ClassListContext();
   ~ClassListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceNodeContext : public TemplateStructIntf
{
  public:
    ClassInheritanceNodeContext(ClassDef *);
   ~ClassInheritanceNodeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

    void addChildren(const BaseClassList *bcl,bool hideSuper);
  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceContext : public TemplateListIntf
{
  public:
    ClassInheritanceContext();
   ~ClassInheritanceContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassHierarchyContext : public TemplateStructIntf
{
  public:
    ClassHierarchyContext();
   ~ClassHierarchyContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestingNodeContext : public TemplateStructIntf
{
  public:
    NestingNodeContext(const NestingNodeContext *parent,
                       Definition *,int index,int level,bool addClasses);
   ~NestingNodeContext();
    QCString id() const;

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestingContext : public TemplateListIntf
{
  public:
    NestingContext(const NestingNodeContext *parent,int level);
   ~NestingContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void addNamespaces(const NamespaceSDict &nsDict,bool rootOnly,bool addClasses);
    void addClasses(const ClassSDict &clDict,bool rootOnly);
  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassTreeContext : public TemplateStructIntf
{
  public:
    ClassTreeContext();
   ~ClassTreeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceListContext : public TemplateListIntf
{
  public:
    NamespaceListContext();
   ~NamespaceListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceTreeContext : public TemplateStructIntf
{
  public:
    NamespaceTreeContext();
   ~NamespaceTreeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class DirFileNodeContext : public TemplateStructIntf
{
  public:
    DirFileNodeContext(const DirFileNodeContext *parent,
                       Definition *,int index,int level);
   ~DirFileNodeContext();
    QCString id() const;

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class DirFileContext : public TemplateListIntf
{
  public:
    DirFileContext(const DirFileNodeContext *parent,int level);
   ~DirFileContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void addDirs(const DirSDict &);
    void addDirs(const DirList &);
    void addFiles(const FileNameList &);
    void addFiles(const FileList &);
  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileListContext : public TemplateListIntf
{
  public:
    FileListContext();
   ~FileListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileTreeContext : public TemplateStructIntf
{
  public:
    FileTreeContext();
   ~FileTreeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageNodeContext : public TemplateStructIntf
{
  public:
    PageNodeContext(PageDef *);
   ~PageNodeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageNodeListContext : public TemplateListIntf
{
  public:
    PageNodeListContext();
   ~PageNodeListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void addPages(const PageSDict &,bool rootOnly);
  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageListContext : public TemplateStructIntf
{
  public:
    PageListContext();
   ~PageListContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageTreeContext : public TemplateStructIntf
{
  public:
    PageTreeContext();
   ~PageTreeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleNodeContext : public TemplateStructIntf
{
  public:
    ModuleNodeContext(GroupDef *);
   ~ModuleNodeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleListContext : public TemplateListIntf
{
  public:
    ModuleListContext();
   ~ModuleListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

    void addModules(const GroupSDict &);
    void addModules(const GroupList &);
  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleTreeContext : public TemplateStructIntf
{
  public:
    ModuleTreeContext();
   ~ModuleTreeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ExampleListContext : public TemplateStructIntf
{
  public:
    ExampleListContext();
   ~ExampleListContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class NavPathElemContext : public TemplateStructIntf
{
  public:
    NavPathElemContext(Definition *def);
   ~NavPathElemContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};


//----------------------------------------------------

class InheritanceNodeContext : public TemplateStructIntf
{
  public:
    InheritanceNodeContext(ClassDef *cd,const QCString &name);
   ~InheritanceNodeContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritanceListContext : public TemplateListIntf
{
  public:
    InheritanceListContext(const BaseClassList *list,bool baseClasses);
   ~InheritanceListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberListContext : public TemplateListIntf
{
  public:
    MemberListContext();
    MemberListContext(const MemberList *ml);
    MemberListContext(MemberSDict *ml,bool doSort);
   ~MemberListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupInfoContext : public TemplateStructIntf
{
  public:
    MemberGroupInfoContext(Definition *def,const QCString &relPath,const MemberGroup *mg);
   ~MemberGroupInfoContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupListContext : public TemplateListIntf
{
  public:
    MemberGroupListContext();
    MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupList *list);
    MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupSDict *mgDict,bool subGrouping);
   ~MemberGroupListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};


//----------------------------------------------------

class MemberListInfoContext : public TemplateStructIntf
{
  public:
    MemberListInfoContext(Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString());
   ~MemberListInfoContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberInfoContext : public TemplateStructIntf
{
  public:
    MemberInfoContext(const MemberInfo *mi);
   ~MemberInfoContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoContext : public TemplateStructIntf
{
  public:
    InheritedMemberInfoContext(ClassDef *cd,MemberList *ml,const QCString &title);
   ~InheritedMemberInfoContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoListContext : public TemplateListIntf
{
  public:
    InheritedMemberInfoListContext();
    void addMemberList(ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList=TRUE);
   ~InheritedMemberInfoListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class AllMembersListContext : public TemplateListIntf
{
  public:
    AllMembersListContext();
    AllMembersListContext(const MemberNameInfoSDict *ml);
   ~AllMembersListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentContext : public TemplateStructIntf
{
  public:
    ArgumentContext(const Argument *arg,Definition *def,const QCString &relPath);
   ~ArgumentContext();

    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentListContext : public TemplateListIntf
{
  public:
    ArgumentListContext();
    ArgumentListContext(const ArgumentList *al,Definition *def,const QCString &relPath);
   ~ArgumentListContext();

    // TemplateListIntf
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;

  private:
    class Private;
    Private *p;
};

//----------------------------------------------------

void generateOutputViaTemplate();

#endif
