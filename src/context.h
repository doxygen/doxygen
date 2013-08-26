#ifndef CONTEXT_H
#define CONTEXT_H

#include "template.h"

class Definition;
class ClassDef;
class ClassSDict;
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
    NestingNodeContext(Definition *,bool addClasses);
   ~NestingNodeContext();

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
    NestingContext();
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
    DirFileNodeContext(Definition *);
   ~DirFileNodeContext();

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
    DirFileContext();
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

void generateOutputViaTemplate();

#endif
