#include "context.h"
#include "config.h"
#include "index.h"
#include "classlist.h"
#include "doxygen.h"
#include "namespacedef.h"
#include "filedef.h"
#include "pagedef.h"
#include "groupdef.h"
#include "util.h"
#include "version.h"
#include "language.h"
#include "message.h"
#include "vhdldocgen.h"
#include "filename.h"
#include "dirdef.h"
#include "docparser.h"
#include "htmlgen.h"
#include "htmldocvisitor.h"

// iterator support
template<class T>
class GenericConstIterator : public TemplateListIntf::ConstIterator
{
  public:
    GenericConstIterator(const QList<T> &list) 
      : m_it(list) { }
    virtual ~GenericConstIterator() {}
    void toFirst() 
    {
      m_it.toFirst();
    }
    void toLast()
    {
      m_it.toLast();
    }
    void toNext()
    {
      if (m_it.current()) ++m_it;
    }
    void toPrev()
    {
      if (m_it.current()) --m_it;
    }
    bool current(TemplateVariant &v) const
    {
      if (m_it.current())
      {
        v = m_it.current();
        return TRUE;
      }
      else
      {
        v = TemplateVariant();
        return FALSE;
      }
    }
  private:
    QListIterator<T> m_it;
};

//------------------------------------------------------------------------

// standard list implementation
template<class T>
class GenericNodeListContext : public TemplateListIntf
{
  public:
    GenericNodeListContext()
    {
      m_children.setAutoDelete(TRUE);
    }

    // TemplateListIntf methods
    int count() const
    {
      return (int)m_children.count();
    }
    TemplateVariant at(int index) const
    {
      TemplateVariant result;
      if (index>=0 && index<count())
      {
        result = m_children.at(index);
      }
      return result;
    }
    TemplateListIntf::ConstIterator *createIterator() const
    {
      return new GenericConstIterator<T>(m_children);
    }

    void append(T *ctn)
    {
      m_children.append(ctn);
    }
    bool isEmpty() const
    {
      return m_children.isEmpty();
    }
  private:
    mutable QList<T> m_children;
};

//------------------------------------------------------------------------

/** @brief Helper class to map a property name to a handler member function */
template<typename T> 
class PropertyMapper
{
  public:
    struct PropertyFunc
    {
      typedef TemplateVariant (T::*Handler)() const;
      PropertyFunc(const T *o,Handler h) : obj(o), handler(h) {}
      TemplateVariant operator()() const
      {
        return (obj->*handler)();
      }
      const T *obj;
      Handler handler;
    };
    PropertyMapper() { m_map.setAutoDelete(TRUE); }
    TemplateVariant get(const char *n)
    {
      //printf("PropertyMapper::get(%s)\n",n);
      TemplateVariant result;
      PropertyFunc *func = m_map.find(n);
      if (func)
      {
        result = (*func)();
      }
      return result;
    }
    void insert(const char *name,const PropertyFunc *func)
    {
      m_map.insert(name,func);
    }
  private:
    QDict<PropertyFunc> m_map;
};


//------------------------------------------------------------------------

//%% struct Config : configuration options
//%% {
class ConfigContext::Private
{
  public:
    Private() { cachedLists.setAutoDelete(TRUE); }
    TemplateVariant fetchList(const QCString &name,const QStrList *list)
    {
      TemplateList *tlist = cachedLists.find(name);
      if (tlist==0)
      {
        tlist = new TemplateList;
        cachedLists.insert(name,tlist);
        QStrListIterator li(*list);
        char *s;
        for (li.toFirst();(s=li.current());++li)
        {
          tlist->append(s);
        }
      }
      return tlist;
    }
  private:
    QDict<TemplateList> cachedLists;
};
//%% }

ConfigContext::ConfigContext()
{
  p = new Private;
}

ConfigContext::~ConfigContext()
{
  delete p;
}

TemplateVariant ConfigContext::get(const char *name) const
{
  TemplateVariant result;
  if (name)
  {
    ConfigOption *option = Config::instance()->get(name);
    if (option)
    {
      switch (option->kind())
      {
        case ConfigOption::O_Bool:
          return TemplateVariant(*((ConfigBool*)option)->valueRef());
        case ConfigOption::O_Int:
          return TemplateVariant(*((ConfigInt*)option)->valueRef());
        case ConfigOption::O_Enum:
          return TemplateVariant(*((ConfigEnum*)option)->valueRef());
        case ConfigOption::O_String:
          return TemplateVariant(*((ConfigString*)option)->valueRef());
        case ConfigOption::O_List:
          return p->fetchList(name,((ConfigList*)option)->valueRef());
          break;
        default:
          break;
      }
    }
  }
  return result;
}

//------------------------------------------------------------------------

//%% struct Doxygen: global information
//%% {
class DoxygenContext::Private : public PropertyMapper<DoxygenContext::Private>
{
  public:
    TemplateVariant version() const
    {
      return versionString;
    }
    TemplateVariant date() const
    {
      return TemplateVariant(dateToString(TRUE));
    }
    Private()
    {
      //%% string version
      insert("version",new PropertyFunc(this,&Private::version));
      //%% string date
      insert("date",   new PropertyFunc(this,&Private::date));
    }
};
//%% }

DoxygenContext::DoxygenContext()
{
  p = new Private;
}

DoxygenContext::~DoxygenContext()
{
  delete p;
}

TemplateVariant DoxygenContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% struct Translator: translation methods
//%% {
class TranslateContext::Private : public PropertyMapper<TranslateContext::Private>
{
  public:
    static QCString generatedAtFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->generatedAt(args);
    }
    QCString generatedAt(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==2)
      {
        return theTranslator->trGeneratedAt(args[0].toString(),args[1].toString());
      }
      else
      {
        err("tr.generateAt should take two parameters!\n");
      }
      return QCString();
    }

    TemplateVariant generatedBy() const
    {
      return theTranslator->trGeneratedBy();
    }
    TemplateVariant generatedAt() const
    {
      return TemplateVariant(this,&Private::generatedAtFunc);
    }
    TemplateVariant search() const
    {
      return theTranslator->trSearch();
    }
    TemplateVariant mainPage() const
    {
      return theTranslator->trMainPage();
    }
    TemplateVariant classes() const
    {
      return theTranslator->trClasses();
    }
    TemplateVariant classList() const
    {
      return theTranslator->trCompoundList();
    }
    TemplateVariant classIndex() const
    {
      return theTranslator->trCompoundIndex();
    }
    TemplateVariant classHierarchy() const
    {
      return theTranslator->trClassHierarchy();
    }
    TemplateVariant classMembers() const
    {
      return theTranslator->trCompoundMembers();
    }
    TemplateVariant modules() const
    {
      return theTranslator->trModules();
    }
    TemplateVariant namespaces() const
    {
      if (m_javaOpt || m_vhdlOpt)
      {
        return theTranslator->trPackages();
      }
      else if (m_fortranOpt)
      {
        return theTranslator->trModules();
      }
      else
      {
        return theTranslator->trNamespaces();
      }
    }
    TemplateVariant files() const
    {
      return theTranslator->trFile(TRUE,FALSE);
    }
    TemplateVariant pages() const
    {
      return theTranslator->trRelatedPages();
    }
    TemplateVariant examples() const
    {
      return theTranslator->trExamples();
    }
    TemplateVariant namespaceList() const
    {
      if (m_javaOpt || m_vhdlOpt)
      {
        return theTranslator->trPackages();
      }
      else if (m_fortranOpt)
      {
        return theTranslator->trModulesList();
      }
      else
      {
        return theTranslator->trNamespaceList();
      }
    }
    TemplateVariant namespaceMembers() const
    {
      if (m_javaOpt || m_vhdlOpt)
      {
        return theTranslator->trPackageMembers();
      }
      else if (m_fortranOpt)
      {
        return theTranslator->trModulesMembers();
      }
      else
      { 
        return theTranslator->trNamespaceMembers();
      }
    }
    TemplateVariant fileList() const
    {
      return theTranslator->trFileList();
    }
    TemplateVariant fileMembers() const
    {
      return theTranslator->trFileMembers();
    }
    TemplateVariant relatedPagesDesc() const
    {
      return theTranslator->trRelatedPagesDescription();
    }
    TemplateVariant more() const
    {
      return theTranslator->trMore();
    }
    TemplateVariant detailedDesc() const
    {
      return theTranslator->trDetailedDescription();
    }
    Private()
    {
      //%% string generatedBy
      insert("generatedby",     new PropertyFunc(this,&Private::generatedBy));
      //%% string generatedAt
      insert("generatedAt",     new PropertyFunc(this,&Private::generatedAt));
      //%% string search
      insert("search",          new PropertyFunc(this,&Private::search));
      //%% string mainPage
      insert("mainPage",        new PropertyFunc(this,&Private::mainPage));
      //%% string classes
      insert("classes",         new PropertyFunc(this,&Private::classes));
      //%% string classList
      insert("classList",       new PropertyFunc(this,&Private::classList));
      //%% string classIndex
      insert("classIndex",      new PropertyFunc(this,&Private::classIndex));
      //%% string classHierarchy
      insert("classHierarchy",  new PropertyFunc(this,&Private::classHierarchy));
      //%% string classMembers
      insert("classMembers",    new PropertyFunc(this,&Private::classMembers));
      //%% string modules
      insert("modules",         new PropertyFunc(this,&Private::modules));
      //%% string namespaces
      insert("namespaces",      new PropertyFunc(this,&Private::namespaces));
      //%% string files
      insert("files",           new PropertyFunc(this,&Private::files));
      //%% string pages
      insert("pages",           new PropertyFunc(this,&Private::pages));
      //%% string examples
      insert("examples",        new PropertyFunc(this,&Private::examples));
      //%% string namespaceList
      insert("namespaceList",   new PropertyFunc(this,&Private::namespaceList));
      //%% string namespaceMembers
      insert("namespaceMembers",new PropertyFunc(this,&Private::namespaceMembers));
      //%% srting fileList
      insert("fileList",        new PropertyFunc(this,&Private::fileList));
      //%% string fileMembers
      insert("fileMembers",     new PropertyFunc(this,&Private::fileMembers));
      //%% string relatedPagesDescripiton
      insert("relatedPagesDesc",new PropertyFunc(this,&Private::relatedPagesDesc));
      //%% string more
      insert("more",            new PropertyFunc(this,&Private::more));
      //%% string detailedDescription
      insert("detailedDesc",    new PropertyFunc(this,&Private::detailedDesc));

      m_javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
      m_fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      m_vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
    }
  private:
    bool m_javaOpt;
    bool m_fortranOpt;
    bool m_vhdlOpt;
};
//%% }

TranslateContext::TranslateContext()
{
  p = new Private;
}

TranslateContext::~TranslateContext()
{
  delete p;
}

TemplateVariant TranslateContext::get(const char *n) const
{
  return p->get(n);
}

static TemplateVariant parseDoc(Definition *def,const QCString &file,int line,
                                const QCString &relPath,const QCString &docStr,bool isBrief)
{
  TemplateVariant result;
  DocRoot *root = validatingParseDoc(file,line,def,0,docStr,TRUE,FALSE,0,isBrief,FALSE);
  QGString docs;
  {
    FTextStream ts(&docs);
    // TODO: support other generators
    HtmlCodeGenerator codeGen(ts,relPath);
    HtmlDocVisitor visitor(ts,codeGen,def);
    root->accept(&visitor);
  }
  bool isEmpty = root->isEmpty();
  if (isEmpty) 
    result = "";
  else 
    result = TemplateVariant(docs);
  result.setRaw(TRUE);
  delete root;
  return result;
}

//------------------------------------------------------------------------

//%% struct Symbol: shared info for all symbols
//%% {
template<typename T> 
class DefinitionContext : public PropertyMapper<T>
{
  public:
    DefinitionContext(const T *super,Definition *d) : m_def(d), m_detailsCached(FALSE)
    {
      //%% string name: the name of the symbol
      PropertyMapper<T>::insert("name",      new typename PropertyMapper<T>::PropertyFunc(super,&DefinitionContext::name));
      //%% string relPath: the relative path to the root of the output (CREATE_SUBDIRS)
      PropertyMapper<T>::insert("relPath",   new typename PropertyMapper<T>::PropertyFunc(super,&DefinitionContext::relPath));
      //%% string fileName: the file name of the output file associated with the symbol (without extension)
      PropertyMapper<T>::insert("fileName",  new typename PropertyMapper<T>::PropertyFunc(super,&DefinitionContext::fileName));
      //%% string details: the detailed documentation for this symbol
      PropertyMapper<T>::insert("details",   new typename PropertyMapper<T>::PropertyFunc(super,&DefinitionContext::details));
      //%% string brief: the brief description for this symbol
      PropertyMapper<T>::insert("brief",     new typename PropertyMapper<T>::PropertyFunc(super,&DefinitionContext::brief));
    }
    TemplateVariant fileName() const
    {
      return TemplateVariant(m_def->getOutputFileBase());
    }
    TemplateVariant name() const
    {
      return m_def->displayName();
    }
    TemplateVariant relPath() const 
    {
      static bool createSubdirs = Config_getBool("CREATE_SUBDIRS");
      return createSubdirs ? TemplateVariant("../../") : TemplateVariant("");
    }
    TemplateVariant details() const
    {
      if (!m_detailsCached)
      {
        m_details = parseDoc(m_def,m_def->docFile(),m_def->docLine(),
                             relPath().toString(),m_def->documentation(),FALSE);
        m_detailsCached = TRUE;
      }
      return m_details;
    }
    TemplateVariant brief() const
    {
      if (!m_briefCached)
      {
        m_brief = parseDoc(m_def,m_def->briefFile(),m_def->briefLine(),
                             relPath().toString(),m_def->briefDescription(),TRUE);
        m_briefCached = TRUE;
      }
      return m_brief;
    }

  private:
    Definition      *m_def;
    mutable bool     m_detailsCached;
    mutable TemplateVariant m_details;
    mutable bool     m_briefCached;
    mutable TemplateVariant m_brief;
};
//%% }

//------------------------------------------------------------------------

//%% struct Class(Symbol): class information
//%% {
class ClassContext::Private : public DefinitionContext<ClassContext::Private>
{
  public:
    Private(ClassDef *cd) : DefinitionContext(this,cd) , m_classDef(cd)
    {
      insert("title",     new PropertyFunc(this,&Private::title));
      insert("highlight", new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
      insert("hasBrief", new PropertyFunc(this,&Private::hasBrief));
      insert("hasDetails", new PropertyFunc(this,&Private::hasDetails));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_classDef->title());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("classes");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
    TemplateVariant hasBrief() const
    {
      return m_classDef->hasBriefDescription();
    }
    TemplateVariant hasDetails() const
    {
      return m_classDef->hasDetailedDescription();
    }
  private:
    ClassDef *m_classDef;
};
//%% }

ClassContext::ClassContext(ClassDef *cd)
{
  p = new Private(cd);
}

ClassContext::~ClassContext()
{
  delete p;
}

TemplateVariant ClassContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% struct Namespace(Symbol): namespace information
//%% {
class NamespaceContext::Private : public DefinitionContext<NamespaceContext::Private>
{
  public:
    Private(NamespaceDef *nd) : DefinitionContext(this,nd) , m_namespaceDef(nd)
    {
      insert("title",     new PropertyFunc(this,&Private::title));
      insert("highlight", new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_namespaceDef->title());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("namespaces");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
  private:
    NamespaceDef *m_namespaceDef;
};
//%% }

NamespaceContext::NamespaceContext(NamespaceDef *nd)
{
  p = new Private(nd);
}

NamespaceContext::~NamespaceContext()
{
  delete p;
}

TemplateVariant NamespaceContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% struct File(Symbol): file information
//%% {
class FileContext::Private : public DefinitionContext<FileContext::Private>
{
  public:
    Private(FileDef *fd) : DefinitionContext(this,fd) , m_fileDef(fd)
    {
      insert("title",     new PropertyFunc(this,&Private::title));
      insert("highlight", new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_fileDef->title());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("files");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
  private:
    FileDef *m_fileDef;
};
//%% }

FileContext::FileContext(FileDef *fd)
{
  p = new Private(fd);
}

FileContext::~FileContext()
{
  delete p;
}

TemplateVariant FileContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% struct Dir(Symbol): directory information
//%% {
class DirContext::Private : public DefinitionContext<DirContext::Private>
{
  public:
    Private(DirDef *dd) : DefinitionContext(this,dd) , m_dirDef(dd)
    {
      insert("title",        new PropertyFunc(this,&Private::title));
      insert("highlight",    new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
      insert("dirName",      new PropertyFunc(this,&Private::dirName));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_dirDef->shortTitle());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("files");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
    TemplateVariant dirName() const
    {
      return TemplateVariant(m_dirDef->shortName());
    }
  private:
    DirDef *m_dirDef;
};
//%% }

DirContext::DirContext(DirDef *fd)
{
  p = new Private(fd);
}

DirContext::~DirContext()
{
  delete p;
}

TemplateVariant DirContext::get(const char *n) const
{
  return p->get(n);
}


//------------------------------------------------------------------------

//%% struct Page(Symbol): page information
//%% {
class PageContext::Private : public DefinitionContext<PageContext::Private>
{
  public:
    Private(PageDef *pd) : DefinitionContext(this,pd) , m_pageDef(pd)
    {
      insert("title",     new PropertyFunc(this,&Private::title));
      insert("highlight", new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_pageDef->title());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("pages");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
  private:
    PageDef *m_pageDef;
};
//%% }

PageContext::PageContext(PageDef *pd)
{
  p = new Private(pd);
}

PageContext::~PageContext()
{
  delete p;
}

TemplateVariant PageContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% struct Module(Symbol): group information
//%% {
class ModuleContext::Private : public DefinitionContext<ModuleContext::Private>
{
  public:
    Private(GroupDef *gd) : DefinitionContext(this,gd) , m_groupDef(gd)
    {
      insert("title",     new PropertyFunc(this,&Private::title));
      insert("highlight", new PropertyFunc(this,&Private::highlight));
      insert("subhighlight", new PropertyFunc(this,&Private::subHighlight));
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_groupDef->groupTitle());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("modules");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
  private:
    GroupDef *m_groupDef;
};
//%% }

ModuleContext::ModuleContext(GroupDef *gd)
{
  p = new Private(gd);
}

ModuleContext::~ModuleContext()
{
  delete p;
}

TemplateVariant ModuleContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% list ClassList[Class] : list of classes
class ClassListContext::Private : public GenericNodeListContext<ClassContext>
{
  public:
    void addClasses(const ClassSDict &classSDict)
    {
      ClassSDict::Iterator cli(classSDict);
      ClassDef *cd;
      for (cli.toFirst() ; (cd=cli.current()) ; ++cli )
      {
        if (cd->getLanguage()==SrcLangExt_VHDL && 
            ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS || 
             (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
           ) // no architecture
        {
          continue;
        }
        if (cd->isLinkableInProject() && cd->templateMaster()==0)
        {
          append(new ClassContext(cd));
        }
      }
    }
};

ClassListContext::ClassListContext()
{
  p = new Private;
  p->addClasses(*Doxygen::classSDict);
  p->addClasses(*Doxygen::hiddenClasses);
}

ClassListContext::~ClassListContext()
{
  delete p;
}

// TemplateListIntf
int ClassListContext::count() const
{
  return p->count();
}

TemplateVariant ClassListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *ClassListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct ClassInheritanceNode: node in inheritance tree 
//%% {
class ClassInheritanceNodeContext::Private : public PropertyMapper<ClassInheritanceNodeContext::Private>
{
  public:
    Private(ClassDef *cd) : m_classContext(cd) 
    {
      //%% bool is_leaf_node: true if this node does not have any children
      insert("is_leaf_node",  new PropertyFunc(this,&Private::isLeafNode));
      //%% ClassInheritance children: list of nested classes/namespaces
      insert("children",      new PropertyFunc(this,&Private::children));
      //%% Class class: class info
      insert("class",         new PropertyFunc(this,&Private::getClass));
    }
    void addChildren(const BaseClassList *bcl,bool hideSuper)
    {
      if (bcl==0) return;
      BaseClassListIterator bcli(*bcl);
      BaseClassDef *bcd;
      for (bcli.toFirst() ; (bcd=bcli.current()) ; ++bcli)
      {    
        ClassDef *cd=bcd->classDef;
        if (cd->getLanguage()==SrcLangExt_VHDL && (VhdlDocGen::VhdlClasses)cd->protection()!=VhdlDocGen::ENTITYCLASS)
        {
          continue;
        }

        bool b;
        if (cd->getLanguage()==SrcLangExt_VHDL)
        {
          b=hasVisibleRoot(cd->subClasses());
        }
        else
        {
          b=hasVisibleRoot(cd->baseClasses());
        }

        if (cd->isVisibleInHierarchy() && b) // hasVisibleRoot(cd->baseClasses()))
        {
          bool hasChildren = !cd->visited && !hideSuper && classHasVisibleChildren(cd);
          ClassInheritanceNodeContext *tnc = new ClassInheritanceNodeContext(cd);
          m_children.append(tnc);
          if (hasChildren)
          {
            //printf("Class %s at %p visited=%d\n",cd->name().data(),cd,cd->visited);
            bool wasVisited=cd->visited;
            cd->visited=TRUE;
            if (cd->getLanguage()==SrcLangExt_VHDL)	
            {
              tnc->addChildren(cd->baseClasses(),wasVisited);
            }
            else       
            {
              tnc->addChildren(cd->subClasses(),wasVisited);
            }
          }
        }
      }
    }
    TemplateVariant isLeafNode() const
    {
      return m_children.isEmpty();
    }
    TemplateVariant children() const
    {
      return TemplateVariant(&m_children);
    }
    TemplateVariant getClass() const
    {
      return TemplateVariant(&m_classContext);
    }
  private:
    GenericNodeListContext<ClassInheritanceNodeContext> m_children;
    ClassContext m_classContext;
};
//%% }

ClassInheritanceNodeContext::ClassInheritanceNodeContext(ClassDef *cd) 
{
  p = new Private(cd);
}

ClassInheritanceNodeContext::~ClassInheritanceNodeContext()
{
  delete p;
}

TemplateVariant ClassInheritanceNodeContext::get(const char *n) const
{
  return p->get(n);
}

void ClassInheritanceNodeContext::addChildren(const BaseClassList *bcl,bool hideSuper)
{
  p->addChildren(bcl,hideSuper);
}

//------------------------------------------------------------------------

//%% list ClassInheritance[ClassInheritanceNode]: list of classes
class ClassInheritanceContext::Private : public
    GenericNodeListContext<ClassInheritanceNodeContext>
{
  public:
    void addClasses(const ClassSDict &classSDict)
    {
      ClassSDict::Iterator cli(classSDict);
      ClassDef *cd;
      for (cli.toFirst();(cd=cli.current());++cli)
      {
        bool b;
        if (cd->getLanguage()==SrcLangExt_VHDL)
        {
          if (!(VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS)      
          {
            continue;
          }
          b=!hasVisibleRoot(cd->subClasses());
        }
        else
        {
          b=!hasVisibleRoot(cd->baseClasses());
        }
        if (b)
        {
          if (cd->isVisibleInHierarchy()) // should it be visible
          {
            // new root level class
            ClassInheritanceNodeContext *tnc = new ClassInheritanceNodeContext(cd);
            append(tnc);
            bool hasChildren = !cd->visited && classHasVisibleChildren(cd); 
            if (cd->getLanguage()==SrcLangExt_VHDL && hasChildren) 
            {
              tnc->addChildren(cd->baseClasses(),cd->visited);
              cd->visited=TRUE;
            }
            else if (hasChildren)
            {
              tnc->addChildren(cd->subClasses(),cd->visited);
              cd->visited=TRUE;
            }
          }
        }
      }
    }
};

ClassInheritanceContext::ClassInheritanceContext()
{
  p = new Private;
  initClassHierarchy(Doxygen::classSDict);
  initClassHierarchy(Doxygen::hiddenClasses);
  p->addClasses(*Doxygen::classSDict);
  p->addClasses(*Doxygen::hiddenClasses);
}

ClassInheritanceContext::~ClassInheritanceContext()
{
  delete p;
}

// TemplateListIntf
int ClassInheritanceContext::count() const
{
  return (int)p->count();
}

TemplateVariant ClassInheritanceContext::at(int index) const
{
  TemplateVariant result;
  if (index>=0 && index<count())
  {
    result = p->at(index);
  }
  return result;
}

TemplateListIntf::ConstIterator *ClassInheritanceContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct ClassHierarchy: inheritance tree 
//%% {
class ClassHierarchyContext::Private : public PropertyMapper<ClassHierarchyContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_classTree);
    }
    TemplateVariant fileName() const
    {
      return "hierarchy";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "classes";
    }
    TemplateVariant subhighlight() const
    {
      return "classhierarchy";
    }
    TemplateVariant title() const
    {
      static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      if (vhdlOpt)
      {
        return VhdlDocGen::trDesignUnitHierarchy();
      }
      else
      {
        return theTranslator->trClassHierarchy();
      }
    }
    Private()
    {
      //%% ClassInheritance tree
      insert("tree",new PropertyFunc(this,&Private::tree));
      insert("fileName",new PropertyFunc(this,&Private::fileName));
      insert("relPath",new PropertyFunc(this,&Private::relPath));
      insert("highlight",new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",new PropertyFunc(this,&Private::title));
    }
  private:
    ClassInheritanceContext m_classTree;
};
//%% }

ClassHierarchyContext::ClassHierarchyContext()
{
  p = new Private;
}

ClassHierarchyContext::~ClassHierarchyContext()
{
  delete p;
}

TemplateVariant ClassHierarchyContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% struct NestingNode: node is a nesting relation tree
//%% {
class NestingNodeContext::Private : public PropertyMapper<NestingNodeContext::Private>
{
  public:
    Private(Definition *d,bool addCls) : m_def(d), 
       m_classContext(m_def->definitionType()==Definition::TypeClass?(ClassDef*)d:0),
       m_namespaceContext(m_def->definitionType()==Definition::TypeNamespace?(NamespaceDef*)d:0)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      insert("is_leaf_node",  new PropertyFunc(this,&Private::isLeafNode));
      //%% Nesting children: list of nested classes/namespaces
      insert("children",      new PropertyFunc(this,&Private::children));
      //%% [optional] Class class: class info (if this node represents a class)
      insert("class",         new PropertyFunc(this,&Private::getClass));
      //%% [optional] Namespace namespace: namespace info (if this node represents a namespace)
      insert("namespace",     new PropertyFunc(this,&Private::getNamespace));
      addNamespaces(addCls);
      addClasses();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children.count()==0;
    }
    TemplateVariant children() const
    {
      return TemplateVariant(&m_children);
    }
    TemplateVariant getClass() const
    {
      if (m_def->definitionType()==Definition::TypeClass)
      {
        return TemplateVariant(&m_classContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getNamespace() const
    {
      if (m_def->definitionType()==Definition::TypeNamespace)
      {
        return TemplateVariant(&m_namespaceContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    void addClasses()
    {
      ClassDef *cd = m_def->definitionType()==Definition::TypeClass ? (ClassDef*)m_def : 0;
      if (cd && cd->getClassSDict())
      {
        m_children.addClasses(*cd->getClassSDict(),FALSE);
      }
    }
    void addNamespaces(bool addClasses)
    {
      NamespaceDef *nd = m_def->definitionType()==Definition::TypeNamespace ? (NamespaceDef*)m_def : 0;
      if (nd && nd->getNamespaceSDict())
      {
        m_children.addNamespaces(*nd->getNamespaceSDict(),FALSE,addClasses);
      }
      if (addClasses && nd && nd->getClassSDict())
      {
        m_children.addClasses(*nd->getClassSDict(),FALSE);
      }
    }
    Definition *m_def;
  private:
    NestingContext m_children;
    ClassContext m_classContext;
    NamespaceContext m_namespaceContext;
};
//%% }

NestingNodeContext::NestingNodeContext(Definition *d,bool addClass) 
{
  p = new Private(d,addClass);
}

NestingNodeContext::~NestingNodeContext()
{
  delete p;
}

TemplateVariant NestingNodeContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% list Nesting[NestingNode]: namespace and class nesting relations
class NestingContext::Private : public GenericNodeListContext<NestingNodeContext>
{
  public:
    void addNamespaces(const NamespaceSDict &nsDict,bool rootOnly,bool addClasses)
    {
      NamespaceSDict::Iterator nli(nsDict);
      NamespaceDef *nd;
      for (nli.toFirst();(nd=nli.current());++nli)
      {
        if (nd->localName().find('@')==-1 &&
            (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
        {
          bool hasChildren = namespaceHasVisibleChild(nd,addClasses);
          bool isLinkable  = nd->isLinkableInProject();
          if (isLinkable || hasChildren)
          {
            NestingNodeContext *nnc = new NestingNodeContext(nd,addClasses);
            append(nnc);
          }
        }
      }
    }
    void addClasses(const ClassSDict &clDict,bool rootOnly)
    {
      ClassSDict::Iterator cli(clDict);
      ClassDef *cd;
      for (;(cd=cli.current());++cli)
      {
        if (cd->getLanguage()==SrcLangExt_VHDL) 
        {
          if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS || 
              (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS
             )// no architecture
          {
            continue;
          }
        }
        if (!rootOnly || 
            cd->getOuterScope()==0 || 
            cd->getOuterScope()==Doxygen::globalScope 
           ) 
        {
          if (classVisibleInIndex(cd) && cd->templateMaster()==0) 
          {
            NestingNodeContext *nnc = new NestingNodeContext(cd,TRUE);
            append(nnc);
          }
        }
      }
    }
};

NestingContext::NestingContext()
{
  p = new Private;
}

NestingContext::~NestingContext()
{
  delete p;
}

// TemplateListIntf
int NestingContext::count() const
{
  return p->count();
}

TemplateVariant NestingContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *NestingContext::createIterator() const
{
  return p->createIterator();
}

void NestingContext::addClasses(const ClassSDict &clDict,bool rootOnly)
{
  p->addClasses(clDict,rootOnly);
}

void NestingContext::addNamespaces(const NamespaceSDict &nsDict,bool rootOnly,bool addClasses)
{
  p->addNamespaces(nsDict,rootOnly,addClasses);
}

//------------------------------------------------------------------------

//%% struct ClassTree: Class nesting relations
//%% {
class ClassTreeContext::Private : public PropertyMapper<ClassTreeContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_classTree);
    }
    TemplateVariant fileName() const
    {
      return "annotated";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "classes";
    }
    TemplateVariant subhighlight() const
    {
      return "classlist";
    }
    TemplateVariant title() const
    {
      static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      if (fortranOpt) 
      {
        return theTranslator->trCompoundListFortran();
      }
      else if (vhdlOpt)
      {
        return VhdlDocGen::trDesignUnitList();
      }
      else
      {
        return theTranslator->trClasses();
      }
    }
    Private()
    {
      if (Doxygen::namespaceSDict)
      {
        m_classTree.addNamespaces(*Doxygen::namespaceSDict,TRUE,TRUE);
      }
      if (Doxygen::classSDict)
      {
        m_classTree.addClasses(*Doxygen::classSDict,TRUE);
      }
      //%% Nesting tree
      insert("tree",        new PropertyFunc(this,&Private::tree));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    NestingContext m_classTree;
};
//%% }

ClassTreeContext::ClassTreeContext()
{
  p = new Private;
}

ClassTreeContext::~ClassTreeContext()
{
  delete p;
}

TemplateVariant ClassTreeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list NamespaceList[Namespace] : list of namespaces
class NamespaceListContext::Private : public GenericNodeListContext<NamespaceContext>
{
  public:
    void addNamespaces(const NamespaceSDict &nsDict)
    {
      NamespaceSDict::Iterator nli(nsDict);
      NamespaceDef *nd;
      for (nli.toFirst();(nd=nli.current());++nli)
      {
        if (nd->isLinkableInProject())
        {
          append(new NamespaceContext(nd));
        }
      }
    }
};

NamespaceListContext::NamespaceListContext()
{
  p = new Private;
  p->addNamespaces(*Doxygen::namespaceSDict);
}

NamespaceListContext::~NamespaceListContext()
{
  delete p;
}

// TemplateListIntf
int NamespaceListContext::count() const
{
  return p->count();
}

TemplateVariant NamespaceListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *NamespaceListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct NamespaceTree: tree of nested namespace
//%% {
class NamespaceTreeContext::Private : public PropertyMapper<NamespaceTreeContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_namespaceTree);
    }
    TemplateVariant fileName() const
    {
      return "namespaces";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "namespaces";
    }
    TemplateVariant subhighlight() const
    {
      return "namespacelist";
    }
    TemplateVariant title() const
    {
      static bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
      static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      if (javaOpt || vhdlOpt) 
      {
        return theTranslator->trPackages();
      }
      else if (fortranOpt)
      {
        return theTranslator->trModulesList();
      }
      else
      {
        return theTranslator->trNamespaceList();
      }
    }
    Private()
    {
      if (Doxygen::namespaceSDict)
      {
        m_namespaceTree.addNamespaces(*Doxygen::namespaceSDict,TRUE,FALSE);
      }
      //%% Nesting tree
      insert("tree",        new PropertyFunc(this,&Private::tree));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    NestingContext m_namespaceTree;
};
//%% }

NamespaceTreeContext::NamespaceTreeContext()
{
  p = new Private;
}

NamespaceTreeContext::~NamespaceTreeContext()
{
  delete p;
}

TemplateVariant NamespaceTreeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list FileList[File] : list of files
class FileListContext::Private : public GenericNodeListContext<FileContext>
{
  public:
    void addFiles(const FileNameList &fnList)
    {
      // TODO: if FULL_PATH_NAMES is enabled, the ordering should be dir+file
      FileNameListIterator fnli(fnList);
      FileName *fn;
      for (fnli.toFirst();(fn=fnli.current());++fnli)
      {
        FileNameIterator fni(*fn);
        FileDef *fd;
        for (fni.toFirst();(fd=fni.current());++fni)
        {
          bool doc = fd->isLinkableInProject();
          bool src = fd->generateSourceFile();
          bool nameOk = !fd->isDocumentationFile();
          if (nameOk && (doc || src) && !fd->isReference())
          {
            append(new FileContext(fd));
          }
        }
      }
    }
};

FileListContext::FileListContext()
{
  p = new Private;
  if (Doxygen::inputNameList) p->addFiles(*Doxygen::inputNameList);
}

FileListContext::~FileListContext()
{
  delete p;
}

// TemplateListIntf
int FileListContext::count() const
{
  return p->count();
}

TemplateVariant FileListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *FileListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct DirFileNode: node is a directory hierarchy
//%% {
class DirFileNodeContext::Private : public PropertyMapper<DirFileNodeContext::Private>
{
  public:
    Private(Definition *d) : m_def(d), 
       m_dirContext (m_def->definitionType()==Definition::TypeDir  ? (DirDef*)d  : 0),
       m_fileContext(m_def->definitionType()==Definition::TypeFile ? (FileDef*)d : 0)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      insert("is_leaf_node",  new PropertyFunc(this,&Private::isLeafNode));
      //%% DirFile children: list of nested classes/namespaces
      insert("children",      new PropertyFunc(this,&Private::children));
      //%% [optional] Dir dir: directory info (if this node represents a directory)
      insert("dir",           new PropertyFunc(this,&Private::getDir));
      //%% [optional] File file: file info (if this node represents a file)
      insert("file",          new PropertyFunc(this,&Private::getFile));
      addDirFiles();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children.count()==0;
    }
    TemplateVariant children() const
    {
      return TemplateVariant(&m_children);
    }
    TemplateVariant getDir() const
    {
      if (m_def->definitionType()==Definition::TypeDir)
      {
        return TemplateVariant(&m_dirContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getFile() const
    {
      if (m_def->definitionType()==Definition::TypeFile)
      {
        return TemplateVariant(&m_fileContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    void addDirFiles()
    {
      DirDef *dd = m_def->definitionType()==Definition::TypeDir ? (DirDef*)m_def : 0;
      if (dd)
      {
        m_children.addDirs(dd->subDirs());
        if (dd && dd->getFiles())
        {
          m_children.addFiles(*dd->getFiles());
        }
      }
    }
  private:
    Definition *m_def;
    DirFileContext m_children;
    DirContext m_dirContext;
    FileContext m_fileContext;
};
//%% }

DirFileNodeContext::DirFileNodeContext(Definition *d) 
{
  p = new Private(d);
}

DirFileNodeContext::~DirFileNodeContext()
{
  delete p;
}

TemplateVariant DirFileNodeContext::get(const char *n) const
{
  return p->get(n);
}


//------------------------------------------------------------------------

//%% list DirFile[DirFileNode]: list of directories and/or files
class DirFileContext::Private : public GenericNodeListContext<DirFileNodeContext>
{
  public:
    void addDirs(const DirSDict &dirDict)
    {
      SDict<DirDef>::Iterator dli(dirDict);
      DirDef *dd;
      for (dli.toFirst();(dd=dli.current());++dli)
      {
        if (dd->getOuterScope()==Doxygen::globalScope) 
        {
          append(new DirFileNodeContext(dd));
        }
      }
    }
    void addDirs(const DirList &dirList)
    {
      QListIterator<DirDef> li(dirList);
      DirDef *dd;
      for (li.toFirst();(dd=li.current());++li)
      {
        append(new DirFileNodeContext(dd));
      }
    }
    void addFiles(const FileNameList &fnList)
    {
      FileNameListIterator fnli(fnList); 
      FileName *fn;
      for (fnli.toFirst();(fn=fnli.current());++fnli)
      {
        FileNameIterator fni(*fn);
        FileDef *fd;
        for (;(fd=fni.current());++fni)
        {
          if (fd->getDirDef()==0) // top level file
          {
            append(new DirFileNodeContext(fd));
          }
        }
      }
    }
    void addFiles(const FileList &fList)
    {
      QListIterator<FileDef> li(fList);
      FileDef *fd;
      for (li.toFirst();(fd=li.current());++li)
      {
        append(new DirFileNodeContext(fd));
      }
    }
};

DirFileContext::DirFileContext()
{
  p = new Private;
}

DirFileContext::~DirFileContext()
{
  delete p;
}

// TemplateListIntf
int DirFileContext::count() const
{
  return p->count();
}

TemplateVariant DirFileContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *DirFileContext::createIterator() const
{
  return p->createIterator();
}

void DirFileContext::addDirs(const DirSDict &dirs)
{
  p->addDirs(dirs);
}

void DirFileContext::addDirs(const DirList &dirs)
{
  p->addDirs(dirs);
}

void DirFileContext::addFiles(const FileNameList &files)
{
  p->addFiles(files);
}

void DirFileContext::addFiles(const FileList &files)
{
  p->addFiles(files);
}


//------------------------------------------------------------------------

//%% struct FileTree: tree of directories and files
//%% {
class FileTreeContext::Private : public PropertyMapper<FileTreeContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_dirFileTree);
    }
    TemplateVariant fileName() const
    {
      return "files";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "files";
    }
    TemplateVariant subhighlight() const
    {
      return "filelist";
    }
    TemplateVariant title() const
    {
      return theTranslator->trFileList();
    }
    Private()
    {
      // Add dirs tree
      if (Doxygen::directories)
      {
        m_dirFileTree.addDirs(*Doxygen::directories);
      }
      if (Doxygen::inputNameList)
      {
        m_dirFileTree.addFiles(*Doxygen::inputNameList);
      }
      //%% DirFile tree:
      insert("tree",        new PropertyFunc(this,&Private::tree));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    DirFileContext m_dirFileTree;
};
//%% }

FileTreeContext::FileTreeContext()
{
  p = new Private;
}

FileTreeContext::~FileTreeContext()
{
  delete p;
}

TemplateVariant FileTreeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% struct PageNode: node is a directory hierarchy
//%% {
class PageNodeContext::Private : public PropertyMapper<PageNodeContext::Private>
{
  public:
    Private(PageDef *pd) : m_pageDef(pd), m_pageContext(pd)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      insert("is_leaf_node",  new PropertyFunc(this,&Private::isLeafNode));
      //%% PageList children: list of nested classes/namespaces
      insert("children",      new PropertyFunc(this,&Private::children));
      //%% Page page: page info
      insert("page",          new PropertyFunc(this,&Private::getPage));
      addPages();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children.count()==0;
    }
    TemplateVariant children() const
    {
      return TemplateVariant(&m_children);
    }
    TemplateVariant getPage() const
    {
      return TemplateVariant(&m_pageContext);
    }
    void addPages()
    {
      if (m_pageDef->getSubPages())
      {
        m_children.addPages(*m_pageDef->getSubPages(),FALSE);
      }
    }
  private:
    PageDef *m_pageDef;
    PageNodeListContext m_children;
    PageContext m_pageContext;
};
//%% }

PageNodeContext::PageNodeContext(PageDef *pd) 
{
  p = new Private(pd);
}

PageNodeContext::~PageNodeContext()
{
  delete p;
}

TemplateVariant PageNodeContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% list PageList[PageNode]: list of directories and/or files
class PageNodeListContext::Private : public GenericNodeListContext<PageNodeContext>
{
  public:
    void addPages(const PageSDict &pages,bool rootOnly)
    {
      SDict<PageDef>::Iterator pli(pages);
      PageDef *pd;
      for (pli.toFirst();(pd=pli.current());++pli)
      {
        if (!rootOnly ||
            pd->getOuterScope()==0 || 
            pd->getOuterScope()->definitionType()!=Definition::TypePage)
        {
          append(new PageNodeContext(pd));
        }
      }
    }
};

PageNodeListContext::PageNodeListContext()
{
  p = new Private;
}

PageNodeListContext::~PageNodeListContext()
{
  delete p;
}

// TemplateListIntf
int PageNodeListContext::count() const
{
  return p->count();
}

TemplateVariant PageNodeListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *PageNodeListContext::createIterator() const
{
  return p->createIterator();
}

void PageNodeListContext::addPages(const PageSDict &pages,bool rootOnly)
{
  p->addPages(pages,rootOnly);
}

//------------------------------------------------------------------------

//%% struct PageTree: tree of related pages
//%% {
class PageTreeContext::Private : public PropertyMapper<PageTreeContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_pageList);
    }
    TemplateVariant fileName() const
    {
      return "pages";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "pages";
    }
    TemplateVariant subhighlight() const
    {
      return "";
    }
    TemplateVariant title() const
    {
      return theTranslator->trRelatedPages();
    }
    Private()
    {
      // Add pages 
      if (Doxygen::pageSDict)
      {
        m_pageList.addPages(*Doxygen::pageSDict,TRUE);
      }

      //%% PageNodeList tree:
      insert("tree",        new PropertyFunc(this,&Private::tree));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    PageNodeListContext m_pageList;
};
//%% }

PageTreeContext::PageTreeContext()
{
  p = new Private;
}

PageTreeContext::~PageTreeContext()
{
  delete p;
}

TemplateVariant PageTreeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% struct PageList: list of related pages
//%% {
class PageListContext::Private : public PropertyMapper<PageListContext::Private>
{
  public:
    TemplateVariant items() const
    {
      return TemplateVariant(&m_pageList);
    }
    TemplateVariant fileName() const
    {
      return "pages";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "pages";
    }
    TemplateVariant subhighlight() const
    {
      return "";
    }
    TemplateVariant title() const
    {
      return theTranslator->trRelatedPages();
    }
    Private()
    {
      // Add pages 
      PageSDict::Iterator pdi(*Doxygen::pageSDict);
      PageDef *pd=0;
      for (pdi.toFirst();(pd=pdi.current());++pdi)
      {
        if (!pd->getGroupDef() && !pd->isReference())
        {
          m_pageList.append(new PageContext(pd));
        }
      }

      //%% list[Page] items:
      insert("items",       new PropertyFunc(this,&Private::items));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    GenericNodeListContext<PageContext> m_pageList;
};
//%% }

PageListContext::PageListContext()
{
  p = new Private;
}

PageListContext::~PageListContext()
{
  delete p;
}

TemplateVariant PageListContext::get(const char *name) const
{
  return p->get(name);
}


//------------------------------------------------------------------------

//%% struct ModuleNode: node is a directory hierarchy
//%% {
class ModuleNodeContext::Private : public PropertyMapper<ModuleNodeContext::Private>
{
  public:
    Private(GroupDef *gd) : m_groupDef(gd), m_moduleContext(gd)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      insert("is_leaf_node",  new PropertyFunc(this,&Private::isLeafNode));
      //%% ModuleList children: list of submodules
      insert("children",      new PropertyFunc(this,&Private::children));
      //%% Module module: module info
      insert("module",        new PropertyFunc(this,&Private::getModule));
      addModules();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children.count()==0;
    }
    TemplateVariant children() const
    {
      return TemplateVariant(&m_children);
    }
    TemplateVariant getModule() const
    {
      return TemplateVariant(&m_moduleContext);
    }
    void addModules()
    {
      if (m_groupDef->getSubGroups())
      {
        m_children.addModules(*m_groupDef->getSubGroups());
      }
    }
  private:
    GroupDef *m_groupDef;
    ModuleListContext m_children;
    ModuleContext m_moduleContext;
};
//%% }

ModuleNodeContext::ModuleNodeContext(GroupDef *gd) 
{
  p = new Private(gd);
}

ModuleNodeContext::~ModuleNodeContext()
{
  delete p;
}

TemplateVariant ModuleNodeContext::get(const char *n) const
{
  return p->get(n);
}

//------------------------------------------------------------------------

//%% list ModuleList[ModuleNode]: list of directories and/or files
class ModuleListContext::Private : public GenericNodeListContext<ModuleNodeContext>
{
  public:
    void addModules(const GroupSDict &modules)
    {
      static bool externalGroups = Config_getBool("EXTERNAL_GROUPS");
      GroupSDict::Iterator gli(modules);
      GroupDef *gd;
      for (gli.toFirst();(gd=gli.current());++gli)
      {
        if (!gd->isASubGroup() && gd->isVisible() && (!gd->isReference() || externalGroups))
        {
          append(new ModuleNodeContext(gd));
        }
      }
    }
    void addModules(const GroupList &list)
    {
      QListIterator<GroupDef> gli(list);
      GroupDef *gd;
      for (gli.toFirst();(gd=gli.current());++gli)
      {
        append(new ModuleNodeContext(gd));
      }
    }
};

ModuleListContext::ModuleListContext()
{
  p = new Private;
}

ModuleListContext::~ModuleListContext()
{
  delete p;
}

// TemplateListIntf
int ModuleListContext::count() const
{
  return p->count();
}

TemplateVariant ModuleListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *ModuleListContext::createIterator() const
{
  return p->createIterator();
}

void ModuleListContext::addModules(const GroupSDict &modules)
{
  p->addModules(modules);
}

void ModuleListContext::addModules(const GroupList &modules)
{
  p->addModules(modules);
}


//------------------------------------------------------------------------

//%% struct ModuleTree: tree of modules
//%% {
class ModuleTreeContext::Private : public PropertyMapper<ModuleTreeContext::Private>
{
  public:
    TemplateVariant tree() const
    {
      return TemplateVariant(&m_moduleList);
    }
    TemplateVariant fileName() const
    {
      return "modules";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "modules";
    }
    TemplateVariant subhighlight() const
    {
      return "";
    }
    TemplateVariant title() const
    {
      return theTranslator->trModules();
    }
    Private()
    {
      // Add modules 
      if (Doxygen::groupSDict)
      {
        m_moduleList.addModules(*Doxygen::groupSDict);
      }

      //%% ModuleList tree:
      insert("tree",        new PropertyFunc(this,&Private::tree));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    ModuleListContext m_moduleList;
};
//%% }

ModuleTreeContext::ModuleTreeContext()
{
  p = new Private;
}

ModuleTreeContext::~ModuleTreeContext()
{
  delete p;
}

TemplateVariant ModuleTreeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% struct ExampleList: list of examples page
//%% {
class ExampleListContext::Private : public PropertyMapper<ExampleListContext::Private>
{
  public:
    TemplateVariant items() const
    {
      return TemplateVariant(&m_pageList);
    }
    TemplateVariant fileName() const
    {
      return "examples";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "examples";
    }
    TemplateVariant subhighlight() const
    {
      return "";
    }
    TemplateVariant title() const
    {
      return theTranslator->trExamples();
    }
    Private()
    {
      // Add pages 
      if (Doxygen::exampleSDict)
      {
        m_pageList.addPages(*Doxygen::exampleSDict,FALSE);
      }

      //%% PageNodeList items:
      insert("items",       new PropertyFunc(this,&Private::items));
      insert("fileName",    new PropertyFunc(this,&Private::fileName));
      insert("relPath",     new PropertyFunc(this,&Private::relPath));
      insert("highlight",   new PropertyFunc(this,&Private::highlight));
      insert("subhighlight",new PropertyFunc(this,&Private::subhighlight));
      insert("title",       new PropertyFunc(this,&Private::title));
    }
  private:
    PageNodeListContext m_pageList;
};
//%% }

ExampleListContext::ExampleListContext()
{
  p = new Private;
}

ExampleListContext::~ExampleListContext()
{
  delete p;
}

TemplateVariant ExampleListContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

class HtmlEscaper : public TemplateEscapeIntf
{
  public:
    QCString escape(const QCString &s)
    {
      return convertToHtml(s,TRUE);
    }
};

//------------------------------------------------------------------------

void generateOutputViaTemplate()
{
  TemplateEngine e;
  TemplateContext *ctx = e.createContext();
  HtmlEscaper esc;
  ctx->setEscapeIntf(&esc);
  if (ctx)
  {
    DoxygenContext        doxygen;
    ConfigContext         config;
    TranslateContext      tr;
    ClassListContext      classList;
    ClassTreeContext      classTree;
    ClassHierarchyContext classHierarchy;
    NamespaceListContext  namespaceList;
    NamespaceTreeContext  namespaceTree;
    FileListContext       fileList;
    FileTreeContext       fileTree;
    PageTreeContext       pageTree;
    PageListContext       pageList;
    ModuleTreeContext     moduleTree;
    ExampleListContext    exampleList;

    //%% Doxygen doxygen:
    ctx->set("doxygen",&doxygen);
    //%% Translator tr:
    ctx->set("tr",&tr);
    //%% Config config:
    ctx->set("config",&config);
    //%% ClassList classList:
    ctx->set("classList",&classList); // not used for standard HTML
    //%% ClassTree classTree:
    ctx->set("classTree",&classTree);
    // classIndex
    //%% ClassHierarchy classHierarchy:
    ctx->set("classHierarchy",&classHierarchy);
    //%% NamespaceList namespaceList:
    ctx->set("namespaceList",&namespaceList);
    //%% NamespaceTree namespaceTree:
    ctx->set("namespaceTree",&namespaceTree);
    //%% FileList fileList:
    ctx->set("fileList",&fileList);
    //%% FileTree fileTree:
    ctx->set("fileTree",&fileTree);
    //%% PageList pageList
    ctx->set("pageList",&pageList);
    //%% PageTree pageTree
    ctx->set("pageTree",&pageTree);
    //%% ModuleTree moduleTree
    ctx->set("moduleTree",&moduleTree);
    //%% ExampleList exampleList
    ctx->set("exampleList",&exampleList);

    // render HTML output
    Template *tpl = e.loadByName("htmllayout.tpl");
    if (tpl)
    {
      ctx->setOutputDirectory(Config_getString("HTML_OUTPUT"));
      FTextStream ts;
      tpl->render(ts,ctx);
    }

    // TODO: render other outputs
  }
}

