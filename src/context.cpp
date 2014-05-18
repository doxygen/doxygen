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
#include "dot.h"
#include "diagram.h"
#include "example.h"
#include "membername.h"
#include "parserintf.h"
#include "portable.h"
#include "arguments.h"

// TODO: pass the current file to Dot*::writeGraph, so the user can put dot graphs in other
//       files as well

#define ADD_PROPERTY(name) addProperty(#name,this,&Private::name);

struct ContextGlobals
{
  enum OutputFormat
  {
    Html,
    LateX,
    Rtf,
    ManPage,
    DocBook,
    Xml,
    TagFile
  };
  int          dynSectionId;
  QCString     outputDir;
  OutputFormat outputFormat;
} g_globals;

/** @brief Scoped smart pointer */
template<class T> class ScopedPtr
{
  private:
    T *m_ptr;
    ScopedPtr(const ScopedPtr &);
    ScopedPtr &operator=(const ScopedPtr &);
    void operator==(const ScopedPtr &) const;
    void operator!=(const ScopedPtr &) const;

  public:
    typedef T Type;
    explicit ScopedPtr(T *p=0) : m_ptr(p) {}
    ~ScopedPtr() { delete m_ptr; };
    T &operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
    T *get() const { return m_ptr; }
    operator bool() const { return m_ptr!=0; }
    void reset(T *p=0) { if (p!=m_ptr) { delete m_ptr; m_ptr = p; } }
};

/** @brief Reference counting smart pointer */
template<class T> class SharedPtr
{
  private:
    T *m_ptr;
    SharedPtr(const SharedPtr &);
    SharedPtr &operator=(const SharedPtr &p);
    void operator==(const SharedPtr &) const;
    void operator!=(const SharedPtr &) const;

  public:
    typedef T Type;
    explicit SharedPtr(T *p=0) : m_ptr(p) { if (m_ptr) m_ptr->addRef(); }
    ~SharedPtr() { if (m_ptr) m_ptr->release(); };
    T &operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
    T *get() const { return m_ptr; }
    operator bool() const { return m_ptr!=0; }
    void reset(T *p=0)
    {
      if (p) p->addRef();
      if (m_ptr) m_ptr->release();
      m_ptr = p;
    }
};

/** @brief Template List iterator support */
class GenericConstIterator : public TemplateListIntf::ConstIterator
{
  public:
    GenericConstIterator(const QList<TemplateVariant> &list)
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
        v = *m_it.current();
        return TRUE;
      }
      else
      {
        v = TemplateVariant();
        return FALSE;
      }
    }
  private:
    QListIterator<TemplateVariant> m_it;
};

//------------------------------------------------------------------------

/** @brief standard template list implementation */
class GenericNodeListContext : public TemplateListIntf
{
  public:
    GenericNodeListContext() : m_refCount(0)
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
        result = *m_children.at(index);
      }
      return result;
    }
    TemplateListIntf::ConstIterator *createIterator() const
    {
      return new GenericConstIterator(m_children);
    }

    void append(const TemplateVariant &ctn)
    {
      m_children.append(new TemplateVariant(ctn));
    }
    bool isEmpty() const
    {
      return m_children.isEmpty();
    }
    int addRef()
    {
      return ++m_refCount;
    }
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
    mutable QList<TemplateVariant> m_children;
    int m_refCount;
};

//------------------------------------------------------------------------

/** @brief Helper class to map a property name to a handler member function */
class PropertyMapper 
{
  private:
    struct PropertyFuncIntf
    {
      virtual ~PropertyFuncIntf() {}
      virtual TemplateVariant operator()() const = 0;
    };
    template<typename T>
    struct PropertyFunc : public PropertyFuncIntf
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

  public:
    PropertyMapper() { m_map.setAutoDelete(TRUE); }

    /** Add a property to the map
     *  @param[in] name   The name of the property to add.
     *  @param[in] obj    The object handling access to the property.
     *  @param[in] handle The method to call when the property is accessed.
     */
    template<typename T>
    void addProperty(const char *name,const T* obj,
                     typename PropertyFunc<T>::Handler handle)
    {
      if (m_map.find(name))
      {
        err("Error: adding property '%s' more than once",name);
      }
      else
      {
        m_map.insert(name,new PropertyFunc<T>(obj,handle));
      }
    }

    /** Gets the value of a property.
     *  @param[in] name The name of the property.
     *  @returns A variant representing the properties value or an
     *  invalid variant if it was not found.
     */
    TemplateVariant get(const char *name) const
    {
      //printf("PropertyMapper::get(%s)\n",name);
      TemplateVariant result;
      PropertyFuncIntf *func = m_map.find(name);
      if (func)
      {
        result = (*func)();
      }
      return result;
    }

  private:
    QDict<PropertyFuncIntf> m_map;
};


//------------------------------------------------------------------------

//%% struct Config : configuration options
//%% {
class ConfigContext::Private
{
  public:
    Private() { m_cachedLists.setAutoDelete(TRUE); }
   ~Private() { }
    TemplateVariant fetchList(const QCString &name,const QStrList *list)
    {
      TemplateVariant *v = m_cachedLists.find(name);
      if (v==0)
      {
        TemplateList *tlist = TemplateList::alloc();
        m_cachedLists.insert(name,new TemplateVariant(tlist));
        QStrListIterator li(*list);
        char *s;
        for (li.toFirst();(s=li.current());++li)
        {
          tlist->append(s);
        }
        return tlist;
      }
      else
      {
        return *v;
      }
    }
  private:
    QDict<TemplateVariant> m_cachedLists;
};
//%% }

ConfigContext::ConfigContext() : RefCountedContext("ConfigContext")
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
class DoxygenContext::Private : public PropertyMapper
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
      addProperty("version",this,&Private::version); //makeProperty(this,&Private::version));
      //%% string date
      addProperty("date",   this,&Private::date);
    }
};
//%% }

DoxygenContext::DoxygenContext() : RefCountedContext("DoxygenContext")
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
class TranslateContext::Private : public PropertyMapper
{
  public:

    TemplateVariant handleGeneratedAt(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==2)
      {
        return theTranslator->trGeneratedAt(args[0].toString(),args[1].toString());
      }
      else
      {
        err("tr.generateAt should take two arguments, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritanceDiagramFor(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trClassDiagram(args[0].toString());
      }
      else
      {
        err("tr.inheritanceDiagramFor should take one argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleCollaborationDiagramFor(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trCollaborationDiagram(args[0].toString());
      }
      else
      {
        err("tr.collaborationDiagramFor should take one argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritsList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trInheritsList(args[0].toInt());
      }
      else
      {
        err("tr.inheritsList should take one integer argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritedByList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trInheritedByList(args[0].toInt());
      }
      else
      {
        err("tr.inheritedByList should take one integer argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleWriteList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trWriteList(args[0].toInt());
      }
      else
      {
        err("tr.*List should take one integer argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleImplementedBy(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trImplementedInList(args[0].toInt());
      }
      else
      {
        err("tr.implementedBy should take one integer argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleReimplementedBy(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trReimplementedInList(args[0].toInt());
      }
      else
      {
        err("tr.reimplementedBy should take one integer argument, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleSourceRefs(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trReferences()+" "+theTranslator->trWriteList(args[0].toInt())+".";
      }
      else
      {
        err("tr.sourceRefs should take one integer argument, got %d\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleSourceRefBys(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trReferencedBy()+" "+theTranslator->trWriteList(args[0].toInt())+".";
      }
      else
      {
        err("tr.sourceRefBys should take one integer argument, got %d\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant handleIncludeDependencyGraph(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trInclDepGraph(args[0].toString());
      }
      else
      {
        err("tr.includeDependencyGraph should take one string argument, got %d\n",args.count());
      }
      return TemplateVariant();
    }



    TemplateVariant generatedBy() const
    {
      return theTranslator->trGeneratedBy();
    }
    TemplateVariant generatedAt() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleGeneratedAt>(this);
    }
    TemplateVariant inheritanceDiagramFor() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleInheritanceDiagramFor>(this);
    }
    TemplateVariant collaborationDiagramFor() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleCollaborationDiagramFor>(this);
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
      // TODO: VHDL: trVhdlType(VhdlDocGen::ENTITY,FALSE)
      // TODO: Fortran: trDataTypes()
    }
    TemplateVariant classList() const
    {
      return theTranslator->trCompoundList();
    }
    TemplateVariant classListDescription() const
    {
      return theTranslator->trCompoundListDescription();
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
    TemplateVariant inheritsList() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleInheritsList>(this);
    }
    TemplateVariant inheritedByList() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleInheritedByList>(this);
    }
    TemplateVariant definedAtLineInSourceFile() const
    {
      return theTranslator->trDefinedAtLineInSourceFile();
    }
    TemplateVariant typeConstraints() const
    {
      return theTranslator->trTypeConstraints();
    }
    TemplateVariant exampleList() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleWriteList>(this);
    }
    TemplateVariant listOfAllMembers() const
    {
      return theTranslator->trListOfAllMembers();
    }
    TemplateVariant memberList() const
    {
      return theTranslator->trMemberList();
    }
    TemplateVariant theListOfAllMembers() const
    {
      return theTranslator->trThisIsTheListOfAllMembers();
    }
    TemplateVariant incInheritedMembers() const
    {
      return theTranslator->trIncludingInheritedMembers();
    }
    TemplateVariant defineValue() const
    {
      return theTranslator->trDefineValue();
    }
    TemplateVariant initialValue() const
    {
      return theTranslator->trInitialValue();
    }
    TemplateVariant enumerationValues() const
    {
      return theTranslator->trEnumerationValues();
    }
    TemplateVariant implements() const
    {
      return theTranslator->trImplementedFromList(1);
    }
    TemplateVariant reimplements() const
    {
      return theTranslator->trReimplementedFromList(1);
    }
    TemplateVariant implementedBy() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleImplementedBy>(this);
    }
    TemplateVariant reimplementedBy() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleReimplementedBy>(this);
    }
    TemplateVariant sourceRefs() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleSourceRefs>(this);
    }
    TemplateVariant sourceRefBys() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleSourceRefBys>(this);
    }
    TemplateVariant callGraph() const
    {
      return theTranslator->trCallGraph();
    }
    TemplateVariant callerGraph() const
    {
      return theTranslator->trCallerGraph();
    }
    TemplateVariant inheritedFrom() const
    {
      return theTranslator->trInheritedFrom("@0","@1");
    }
    TemplateVariant additionalInheritedMembers() const
    {
      return theTranslator->trAdditionalInheritedMembers();
    }
    TemplateVariant includeDependencyGraph() const
    {
      return TemplateVariant::Delegate::fromMethod<Private,&Private::handleIncludeDependencyGraph>(this);
    }
    TemplateVariant includedByDependencyGraph() const
    {
      return theTranslator->trInclByDepGraph();
    }
    TemplateVariant gotoSourceCode() const
    {
      return theTranslator->trGotoSourceCode();
    }
    TemplateVariant gotoDocumentation() const
    {
      return theTranslator->trGotoDocumentation();
    }
    TemplateVariant constantgroups() const
    {
      return theTranslator->trConstantGroups();
    }
    TemplateVariant classDocumentation() const
    {
      return theTranslator->trClassDocumentation();
    }
    TemplateVariant compoundMembers() const
    {
      return theTranslator->trCompoundMembers();
    }
    TemplateVariant detailLevel() const
    {
      return theTranslator->trDetailLevel();
    }
    TemplateVariant fileListDescription() const
    {
      bool extractAll = Config_getBool("EXTRACT_ALL");
      return theTranslator->trFileListDescription(extractAll);
    }
    TemplateVariant directories() const
    {
      return theTranslator->trDirectories();
    }
    Private()
    {
      //%% string generatedBy
      addProperty("generatedby",       this,&Private::generatedBy);
      //%% string generatedAt
      addProperty("generatedAt",       this,&Private::generatedAt);
      //%% string search
      addProperty("search",            this,&Private::search);
      //%% string mainPage
      addProperty("mainPage",          this,&Private::mainPage);
      //%% string classes
      addProperty("classes",           this,&Private::classes);
      //%% string classList
      addProperty("classList",         this,&Private::classList);
      //%% string classListDescription
      addProperty("classListDescription", this,&Private::classListDescription);
      //%% string classIndex
      addProperty("classIndex",        this,&Private::classIndex);
      //%% string classHierarchy
      addProperty("classHierarchy",    this,&Private::classHierarchy);
      //%% string classMembers
      addProperty("classMembers",      this,&Private::classMembers);
      //%% string modules
      addProperty("modules",           this,&Private::modules);
      //%% string namespaces
      addProperty("namespaces",        this,&Private::namespaces);
      //%% string files
      addProperty("files",             this,&Private::files);
      //%% string pages
      addProperty("pages",             this,&Private::pages);
      //%% string examples
      addProperty("examples",          this,&Private::examples);
      //%% string namespaceList
      addProperty("namespaceList",     this,&Private::namespaceList);
      //%% string namespaceMembers
      addProperty("namespaceMembers",  this,&Private::namespaceMembers);
      //%% srting fileList
      addProperty("fileList",          this,&Private::fileList);
      //%% string fileMembers
      addProperty("fileMembers",       this,&Private::fileMembers);
      //%% string relatedPagesDescripiton
      addProperty("relatedPagesDesc",  this,&Private::relatedPagesDesc);
      //%% string more
      addProperty("more",              this,&Private::more);
      //%% string detailedDescription
      addProperty("detailedDesc",      this,&Private::detailedDesc);
      //%% string inheritanceDiagramFor
      addProperty("inheritanceDiagramFor", this,&Private::inheritanceDiagramFor);
      //%% string collaborationDiagramFor
      addProperty("collaborationDiagramFor", this,&Private::collaborationDiagramFor);
      //%% markerstring inheritsList
      addProperty("inheritsList",      this,&Private::inheritsList);
      //%% markerstring inheritedByList
      addProperty("inheritedByList",   this,&Private::inheritedByList);
      //%% markerstring definedAtLineInSourceFile
      addProperty("definedAtLineInSourceFile", this,&Private::definedAtLineInSourceFile);
      //%% string typeConstraints
      addProperty("typeConstraints",   this,&Private::typeConstraints);
      //%% string exampleList
      addProperty("exampleList",       this,&Private::exampleList);
      //%% string listOfAllMembers
      addProperty("listOfAllMembers",  this,&Private::listOfAllMembers);
      //%% string memberList
      addProperty("memberList",        this,&Private::memberList);
      //%% string theListOfAllMembers
      addProperty("theListOfAllMembers",this,&Private::theListOfAllMembers);
      //%% string incInheritedMembers
      addProperty("incInheritedMembers",this,&Private::incInheritedMembers);
      //%% string defineValue
      addProperty("defineValue",        this,&Private::defineValue);
      //%% string initialValue
      addProperty("initialValue",       this,&Private::initialValue);
      //%% string enumerationValues
      addProperty("enumerationValues",  this,&Private::enumerationValues);
      //%% markerstring implements
      addProperty("implements",         this,&Private::implements);
      //%% markerstring reimplements
      addProperty("reimplements",       this,&Private::reimplements);
      //%% markerstring implementedBy
      addProperty("implementedBy",      this,&Private::implementedBy);
      //%% markerstring reimplementedBy
      addProperty("reimplementedBy",    this,&Private::reimplementedBy);
      //%% markerstring sourceRefs
      addProperty("sourceRefs",         this,&Private::sourceRefs);
      //%% markerstring sourceRefBys
      addProperty("sourceRefBys",       this,&Private::sourceRefBys);
      //%% string callGraph
      addProperty("callGraph",          this,&Private::callGraph);
      //%% string callerGraph
      addProperty("callerGraph",        this,&Private::callerGraph);
      //%% markerstring inheritedFrom
      addProperty("inheritedFrom",      this,&Private::inheritedFrom);
      //%% string addtionalInheritedMembers
      addProperty("additionalInheritedMembers",this,&Private::additionalInheritedMembers);
      //%% string includeDependencyGraph:container_name
      addProperty("includeDependencyGraph",this,&Private::includeDependencyGraph);
      //%% string includedByDependencyGraph
      addProperty("includedByDependencyGraph",this,&Private::includedByDependencyGraph);
      //%% string gotoSourceCode
      addProperty("gotoSourceCode",     this,&Private::gotoSourceCode);
      //%% string gotoDocumentation
      addProperty("gotoDocumentation",  this,&Private::gotoDocumentation);
      //%% string constantgroups
      addProperty("constantgroups",     this,&Private::constantgroups);
      //%% string classDocumentation
      addProperty("classDocumentation", this,&Private::classDocumentation);
      //%% string compoundMembers
      addProperty("compoundMembers",    this,&Private::compoundMembers);
      //%% string detailLevel
      addProperty("detailLevel",        this,&Private::detailLevel);
      //%% string fileListDescription
      addProperty("fileListDescription",this,&Private::fileListDescription);
      //%% string directories
      addProperty("directories",        this,&Private::directories);

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

TranslateContext::TranslateContext() : RefCountedContext("TranslateContext")
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
    result = TemplateVariant(docs,TRUE);
  delete root;
  return result;
}

static TemplateVariant parseCode(MemberDef *md,const QCString &scopeName,const QCString &relPath,
                                 const QCString &code,int startLine=-1,int endLine=-1,bool showLineNumbers=FALSE)
{
  ParserInterface *pIntf = Doxygen::parserManager->getParser(md->getDefFileExtension());
  pIntf->resetCodeParserState();
  QGString s;
  FTextStream t(&s);
  HtmlCodeGenerator codeGen(t,relPath);
  pIntf->parseCode(codeGen,scopeName,code,md->getLanguage(),FALSE,0,md->getBodyDef(),
                   startLine,endLine,TRUE,md,showLineNumbers,md);
  return TemplateVariant(s.data(),TRUE);
}

static TemplateVariant parseCode(FileDef *fd,const QCString &relPath)
{
  static bool filterSourceFiles = Config_getBool("FILTER_SOURCE_FILES");
  ParserInterface *pIntf = Doxygen::parserManager->getParser(fd->getDefFileExtension());
  pIntf->resetCodeParserState();
  QGString s;
  FTextStream t(&s);
  HtmlCodeGenerator codeGen(t,relPath);
  pIntf->parseCode(codeGen,0,
        fileToString(fd->absFilePath(),filterSourceFiles,TRUE), // the sources
        fd->getLanguage(),  // lang
        FALSE,              // isExampleBlock
        0,                  // exampleName
        fd,                 // fileDef
        -1,                 // startLine
        -1,                 // endLine
        FALSE,              // inlineFragment
        0,                  // memberDef
        TRUE,               // showLineNumbers
        0,                  // searchCtx
        TRUE                // collectXRefs, TODO: should become FALSE
        );
  return TemplateVariant(s.data(),TRUE);
}

//------------------------------------------------------------------------

//%% struct Symbol: shared info for all symbols
//%% {
template<typename T>
class DefinitionContext : public PropertyMapper
{
  public:
    DefinitionContext(Definition *d) : m_def(d)
    {
      //%% string name: the name of the symbol
      addProperty("name",this,&DefinitionContext::name);
      //%% string bareName: the bare name of the symbol with scope info
      addProperty("bareName",this,&DefinitionContext::bareName);
      //%% string relPath: the relative path to the root of the output (CREATE_SUBDIRS)
      addProperty("relPath",this,&DefinitionContext::relPath);
      //%% string fileName: the file name of the output file associated with the symbol (without extension)
      addProperty("fileName",this,&DefinitionContext::fileName);
      //%% string anchor: anchor within the page
      addProperty("anchor",this,&DefinitionContext::anchor);
      //%% string details: the detailed documentation for this symbol
      addProperty("details",this,&DefinitionContext::details);
      //%% string brief: the brief description for this symbol
      addProperty("brief",this,&DefinitionContext::brief);
      //%% string inbodyDocs: the documentation found in the body
      addProperty("inbodyDocs",this,&DefinitionContext::inbodyDocs);
      //%% string sourceFileName: the file name of the source file (without extension)
      addProperty("sourceFileName",this,&DefinitionContext::sourceFileName);
      //%% bool isLinkable: can the symbol be linked to?
      addProperty("isLinkable",this,&DefinitionContext::isLinkable);
      //%% bool isLinkableInProject: can the symbol be linked within this project?
      addProperty("isLinkableInProject",this,&DefinitionContext::isLinkableInProject);
      //%% int dynSectionId: identifier that can be used for collapsable sections
      addProperty("dynSectionId",this,&DefinitionContext::dynSectionId);
      //%% string language: the programming language in which the symbol is written
      addProperty("language",this,&DefinitionContext::language);
      //%% string sourceDef: A link to the source definition
      addProperty("sourceDef",this,&DefinitionContext::sourceDef);
      //%% list[Definition] navigationPath: Breadcrumb navigation path to this item
      addProperty("navigationPath",this,&DefinitionContext::navigationPath);

      m_cache.sourceDef.reset(TemplateList::alloc());
      m_cache.lineLink.reset(TemplateStruct::alloc());
      m_cache.fileLink.reset(TemplateStruct::alloc());

      if (m_def && !m_def->getSourceFileBase().isEmpty())
      {
        m_cache.lineLink->set("text",m_def->getStartBodyLine());
        m_cache.lineLink->set("isLinkable",TRUE);
        m_cache.lineLink->set("fileName",m_def->getSourceFileBase());
        m_cache.lineLink->set("anchor",m_def->getSourceAnchor());
        if (m_def->definitionType()==Definition::TypeFile)
        {
          m_cache.fileLink->set("text",m_def->name());
        }
        else if (m_def->getBodyDef())
        {
          m_cache.fileLink->set("text",m_def->getBodyDef()->name());
        }
        else
        {
          m_cache.fileLink->set("text",name());
        }
        m_cache.fileLink->set("isLinkable",TRUE);
        m_cache.fileLink->set("fileName",m_def->getSourceFileBase());
        m_cache.fileLink->set("anchor",QCString());
        m_cache.sourceDef->append(m_cache.lineLink.get());
        m_cache.sourceDef->append(m_cache.fileLink.get());
      }
    }
    TemplateVariant fileName() const
    {
      return m_def->getOutputFileBase();
    }
    TemplateVariant anchor() const
    {
      return m_def->anchor();
    }
    TemplateVariant sourceFileName() const
    {
      return m_def->getSourceFileBase();
    }
    TemplateVariant isLinkable() const
    {
      return m_def->isLinkable();
    }
    TemplateVariant isLinkableInProject() const
    {
      return m_def->isLinkableInProject();
    }
    TemplateVariant name() const
    {
      return m_def->displayName(TRUE);
    }
    TemplateVariant bareName() const
    {
      return m_def->displayName(FALSE);
    }
    QCString relPathAsString() const
    {
      static bool createSubdirs = Config_getBool("CREATE_SUBDIRS");
      return createSubdirs ? QCString("../../") : QCString("");
    }
    TemplateVariant relPath() const
    {
      return relPathAsString();
    }
    TemplateVariant details() const
    {
      if (!m_cache.details)
      {
        m_cache.details.reset(new TemplateVariant(parseDoc(m_def,m_def->docFile(),m_def->docLine(),
                                            relPathAsString(),m_def->documentation(),FALSE)));
      }
      return *m_cache.details;
    }
    TemplateVariant brief() const
    {
      if (!m_cache.brief)
      {
        if (m_def->hasBriefDescription())
        {
          m_cache.brief.reset(new TemplateVariant(parseDoc(m_def,m_def->briefFile(),m_def->briefLine(),
                             relPathAsString(),m_def->briefDescription(),TRUE)));
        }
        else
        {
          m_cache.brief.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.brief;
    }
    TemplateVariant inbodyDocs() const
    {
      if (!m_cache.inbodyDocs)
      {
        if (!m_def->inbodyDocumentation().isEmpty())
        {
          m_cache.inbodyDocs.reset(new TemplateVariant(parseDoc(m_def,m_def->inbodyFile(),m_def->inbodyLine(),
                                           relPathAsString(),m_def->inbodyDocumentation(),FALSE)));
        }
        else
        {
          m_cache.inbodyDocs.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.inbodyDocs;
    }
    TemplateVariant dynSectionId() const
    {
      return g_globals.dynSectionId;
    }
    TemplateVariant language() const
    {
      SrcLangExt lang = m_def->getLanguage();
      QCString result = "unknown";
      switch (lang)
      {
        case SrcLangExt_Unknown:  break;
        case SrcLangExt_IDL:      result="idl";      break;
        case SrcLangExt_Java:     result="java";     break;
        case SrcLangExt_CSharp:   result="csharp";   break;
        case SrcLangExt_D:        result="d";        break;
        case SrcLangExt_PHP:      result="php";      break;
        case SrcLangExt_ObjC:     result="objc";     break;
        case SrcLangExt_Cpp:      result="cpp";      break;
        case SrcLangExt_JS:       result="js";       break;
        case SrcLangExt_Python:   result="python";   break;
        case SrcLangExt_Fortran:  result="fortran";  break;
        case SrcLangExt_VHDL:     result="vhdl";     break;
        case SrcLangExt_XML:      result="xml";      break;
        case SrcLangExt_Tcl:      result="tcl";      break;
        case SrcLangExt_Markdown: result="markdown"; break;
      }
      return result;
    }
    TemplateVariant sourceDef() const
    {
      if (m_cache.sourceDef->count()==2)
      {
        return m_cache.sourceDef.get();
      }
      else
      {
        return FALSE;
      }
    }
    void fillPath(Definition *def,TemplateList *list) const
    {
      Definition *outerScope = def->getOuterScope();
      Definition::DefType type = def->definitionType();
      if (outerScope && outerScope!=Doxygen::globalScope)
      {
        fillPath(outerScope,list);
      }
      else if (type==Definition::TypeFile && ((const FileDef*)def)->getDirDef())
      {
        fillPath(((const FileDef*)def)->getDirDef(),list);
      }
      list->append(NavPathElemContext::alloc(def));
    }
    TemplateVariant navigationPath() const
    {
      if (!m_cache.navPath)
      {
        TemplateList *list = TemplateList::alloc();
        if (m_def->getOuterScope() && m_def->getOuterScope()!=Doxygen::globalScope)
        {
          fillPath(m_def->getOuterScope(),list);
        }
        else if (m_def->definitionType()==Definition::TypeFile && ((const FileDef *)m_def)->getDirDef())
        {
          fillPath(((const FileDef *)m_def)->getDirDef(),list);
        }
        m_cache.navPath.reset(list);
      }
      return m_cache.navPath.get();
    }

  private:
    Definition      *m_def;
    struct Cachable
    {
      Cachable() { }
      ScopedPtr<TemplateVariant> details;
      ScopedPtr<TemplateVariant> brief;
      ScopedPtr<TemplateVariant> inbodyDocs;
      SharedPtr<TemplateList>    navPath;
      SharedPtr<TemplateList>    sourceDef;
      SharedPtr<TemplateStruct>  fileLink;
      SharedPtr<TemplateStruct>  lineLink;
    };
    mutable Cachable m_cache;
};
//%% }

//------------------------------------------------------------------------

//%% struct IncludeInfo: include file information
//%% {
class IncludeInfoContext::Private : public PropertyMapper
{
  public:
    Private(const IncludeInfo *info,SrcLangExt lang) :
      m_info(info),
      m_lang(lang)
    {
      if (m_info)
      {
        addProperty("file",this,&Private::file);
        addProperty("name",this,&Private::name);
        addProperty("isImport",this,&Private::isImport);
        addProperty("isLocal",this,&Private::isLocal);
      }
    }
    TemplateVariant isLocal() const
    {
      bool isIDLorJava = m_lang==SrcLangExt_IDL || m_lang==SrcLangExt_Java;
      return m_info->local || isIDLorJava;
    }
    TemplateVariant isImport() const
    {
      return m_info->imported;
    }
    TemplateVariant file() const
    {
      if (!m_fileContext && m_info && m_info->fileDef)
      {
        m_fileContext.reset(FileContext::alloc(m_info->fileDef));
      }
      if (m_fileContext)
      {
        return m_fileContext.get();
      }
      else
      {
        return FALSE;
      }
    }
    TemplateVariant name() const
    {
      return m_info->includeName;
    }
  private:
    const IncludeInfo *m_info;
    mutable SharedPtr<FileContext> m_fileContext;
    SrcLangExt m_lang;
};

IncludeInfoContext::IncludeInfoContext(const IncludeInfo *info,SrcLangExt lang) : RefCountedContext("IncludeContext")
{
  p = new Private(info,lang);
}

IncludeInfoContext::~IncludeInfoContext()
{
  delete p;
}

TemplateVariant IncludeInfoContext::get(const char *n) const
{
  return p->get(n);
}
//%% }

//------------------------------------------------------------------------

//%% list IncludeInfoList[Class] : list of nested classes
class IncludeInfoListContext::Private : public GenericNodeListContext
{
  public:
    Private(const QList<IncludeInfo> &list,SrcLangExt lang)
    {
      QListIterator<IncludeInfo> li(list);
      IncludeInfo *ii;
      for (li.toFirst();(ii=li.current());++li)
      {
        if (!ii->indirect)
        {
          append(IncludeInfoContext::alloc(ii,lang));
        }
      }
    }
};

IncludeInfoListContext::IncludeInfoListContext(const QList<IncludeInfo> &list,SrcLangExt lang) : RefCountedContext("IncludeListContext")
{
  p = new Private(list,lang);
}

IncludeInfoListContext::~IncludeInfoListContext()
{
  delete p;
}

// TemplateListIntf
int IncludeInfoListContext::count() const
{
  return p->count();
}

TemplateVariant IncludeInfoListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *IncludeInfoListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct Class(Symbol): class information
//%% {
class ClassContext::Private : public DefinitionContext<ClassContext::Private>
{
  public:
    Private(ClassDef *cd) : DefinitionContext<ClassContext::Private>(cd),
       m_classDef(cd)
    {
      addProperty("title",                     this,&Private::title);
      addProperty("highlight",                 this,&Private::highlight);
      addProperty("subhighlight",              this,&Private::subHighlight);
      addProperty("hasDetails",                this,&Private::hasDetails);
      addProperty("generatedFromFiles",        this,&Private::generatedFromFiles);
      addProperty("usedFiles",                 this,&Private::usedFiles);
      addProperty("hasInheritanceDiagram",     this,&Private::hasInheritanceDiagram);
      addProperty("inheritanceDiagram",        this,&Private::inheritanceDiagram);
      addProperty("hasCollaborationDiagram",   this,&Private::hasCollaborationDiagram);
      addProperty("collaborationDiagram",      this,&Private::collaborationDiagram);
      addProperty("includeInfo",               this,&Private::includeInfo);
      addProperty("inherits",                  this,&Private::inherits);
      addProperty("inheritedBy",               this,&Private::inheritedBy);
      addProperty("unoIDLServices",            this,&Private::unoIDLServices);
      addProperty("unoIDLInterfaces",          this,&Private::unoIDLInterfaces);
      addProperty("signals",                   this,&Private::signals);
      addProperty("publicTypes",               this,&Private::publicTypes);
      addProperty("publicMethods",             this,&Private::publicMethods);
      addProperty("publicStaticMethods",       this,&Private::publicStaticMethods);
      addProperty("publicAttributes",          this,&Private::publicAttributes);
      addProperty("publicStaticAttributes",    this,&Private::publicStaticAttributes);
      addProperty("publicSlots",               this,&Private::publicSlots);
      addProperty("protectedTypes",            this,&Private::protectedTypes);
      addProperty("protectedMethods",          this,&Private::protectedMethods);
      addProperty("protectedStaticMethods",    this,&Private::protectedStaticMethods);
      addProperty("protectedAttributes",       this,&Private::protectedAttributes);
      addProperty("protectedStaticAttributes", this,&Private::protectedStaticAttributes);
      addProperty("protectedSlots",            this,&Private::protectedSlots);
      addProperty("privateTypes",              this,&Private::privateTypes);
      addProperty("privateMethods",            this,&Private::privateMethods);
      addProperty("privateStaticMethods",      this,&Private::privateStaticMethods);
      addProperty("privateAttributes",         this,&Private::privateAttributes);
      addProperty("privateStaticAttributes",   this,&Private::privateStaticAttributes);
      addProperty("privateSlots",              this,&Private::privateSlots);
      addProperty("packageTypes",              this,&Private::packageTypes);
      addProperty("packageMethods",            this,&Private::packageMethods);
      addProperty("packageStaticMethods",      this,&Private::packageStaticMethods);
      addProperty("packageAttributes",         this,&Private::packageAttributes);
      addProperty("packageStaticAttributes",   this,&Private::packageStaticAttributes);
      addProperty("properties",                this,&Private::properties);
      addProperty("events",                    this,&Private::events);
      addProperty("friends",                   this,&Private::friends);
      addProperty("related",                   this,&Private::related);
      addProperty("detailedTypedefs",          this,&Private::detailedTypedefs);
      addProperty("detailedEnums",             this,&Private::detailedEnums);
      addProperty("detailedServices",          this,&Private::detailedServices);
      addProperty("detailedInterfaces",        this,&Private::detailedInterfaces);
      addProperty("detailedConstructors",      this,&Private::detailedConstructors);
      addProperty("detailedMethods",           this,&Private::detailedMethods);
      addProperty("detailedRelated",           this,&Private::detailedRelated);
      addProperty("detailedVariables",         this,&Private::detailedVariables);
      addProperty("detailedProperties",        this,&Private::detailedProperties);
      addProperty("detailedEvents",            this,&Private::detailedEvents);
      addProperty("classes",                   this,&Private::classes);
      addProperty("compoundType",              this,&Private::compoundType);
      addProperty("templateDecls",             this,&Private::templateDecls);
      addProperty("typeConstraints",           this,&Private::typeConstraints);
      addProperty("examples",                  this,&Private::examples);
      addProperty("members",                   this,&Private::members);
      addProperty("allMembersList",            this,&Private::allMembersList);
      addProperty("allMembersFileName",        this,&Private::allMembersFileName);
      addProperty("memberGroups",              this,&Private::memberGroups);
      addProperty("additionalInheritedMembers",this,&Private::additionalInheritedMembers);
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
    TemplateVariant hasDetails() const
    {
      return m_classDef->hasDetailedDescription();
    }
    TemplateVariant generatedFromFiles() const
    {
      return m_classDef->generatedFromFiles();
    }
    TemplateVariant usedFiles() const
    {
      if (!m_cache.usedFiles)
      {
        m_cache.usedFiles.reset(UsedFilesContext::alloc(m_classDef));
      }
      return m_cache.usedFiles.get();
    }
    DotClassGraph *getClassGraph() const
    {
      if (!m_cache.classGraph)
      {
        m_cache.classGraph.reset(new DotClassGraph(m_classDef,DotNode::Inheritance));
      }
      return m_cache.classGraph.get();
    }
    int numInheritanceNodes() const
    {
      if (m_cache.inheritanceNodes==-1)
      {
        m_cache.inheritanceNodes=m_classDef->countInheritanceNodes();
      }
      return m_cache.inheritanceNodes>0;
    }
    TemplateVariant hasInheritanceDiagram() const
    {
      bool result=FALSE;
      static bool haveDot       = Config_getBool("HAVE_DOT");
      static bool classDiagrams = Config_getBool("CLASS_DIAGRAMS");
      static bool classGraph    = Config_getBool("CLASS_GRAPH");
      if (haveDot && (classDiagrams || classGraph))
      {
        DotClassGraph *cg = getClassGraph();
        result = !cg->isTrivial() && !cg->isTooBig();
      }
      else if (classDiagrams)
      {
        result = numInheritanceNodes()>0;
      }
      return result;
    }
    TemplateVariant inheritanceDiagram() const
    {
      QGString result;
      static bool haveDot       = Config_getBool("HAVE_DOT");
      static bool classDiagrams = Config_getBool("CLASS_DIAGRAMS");
      static bool classGraph    = Config_getBool("CLASS_GRAPH");
      if (haveDot && (classDiagrams || classGraph))
      {
        DotClassGraph *cg = getClassGraph();
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
                       g_globals.outputDir,
                       g_globals.outputDir+portable_pathSeparator()+m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                       relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
                      );
      }
      else if (classDiagrams)
      {
        ClassDiagram d(m_classDef);
        FTextStream t(&result);
        QCString name = convertToHtml(m_classDef->displayName());
        t << "<div class=\"center\">" << endl;
        t << "<img src=\"";
        t << relPathAsString() << m_classDef->getOutputFileBase();
        t << ".png\" usemap=\"#" << name << "_map\" alt=\"\"/>" << endl;
        t << "<map id=\"" << name << "_map\" name=\"" << name << "_map\">" << endl;
        d.writeImage(t,g_globals.outputDir,
                     relPathAsString(),
                     m_classDef->getOutputFileBase());
        t << "</div>";
      }
      g_globals.dynSectionId++;
      return TemplateVariant(result.data(),TRUE);
    }
    DotClassGraph *getCollaborationGraph() const
    {
      if (!m_cache.collaborationGraph)
      {
        m_cache.collaborationGraph.reset(new DotClassGraph(m_classDef,DotNode::Collaboration));
      }
      return m_cache.collaborationGraph.get();
    }
    TemplateVariant hasCollaborationDiagram() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      return haveDot && !getCollaborationGraph()->isTrivial();
    }
    TemplateVariant collaborationDiagram() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      QGString result;
      if (haveDot)
      {
        DotClassGraph *cg = getCollaborationGraph();
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
            g_globals.outputDir,
            g_globals.outputDir+portable_pathSeparator()+m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
            );
      }
      g_globals.dynSectionId++;
      return TemplateVariant(result.data(),TRUE);
    }

    TemplateVariant includeInfo() const
    {
      if (!m_cache.includeInfo && m_classDef->includeInfo())
      {
        m_cache.includeInfo.reset(IncludeInfoContext::alloc(m_classDef->includeInfo(),m_classDef->getLanguage()));
      }
      if (m_cache.includeInfo)
      {
        return m_cache.includeInfo.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant inherits() const
    {
      if (!m_cache.inheritsList)
      {
        m_cache.inheritsList.reset(InheritanceListContext::alloc(m_classDef->baseClasses(),TRUE));
      }
      return m_cache.inheritsList.get();
    }
    TemplateVariant inheritedBy() const
    {
      if (!m_cache.inheritedByList)
      {
        m_cache.inheritedByList.reset(InheritanceListContext::alloc(m_classDef->subClasses(),FALSE));
      }
      return m_cache.inheritedByList.get();
    }
    TemplateVariant getMemberList(SharedPtr<MemberListInfoContext> &list,
                                  MemberListType type,const char *title,bool detailed=FALSE) const
    {
      if (!list)
      {
        MemberList *ml = m_classDef->getMemberList(type);
        if (ml)
        {
          list.reset(MemberListInfoContext::alloc(m_classDef,relPathAsString(),ml,title,detailed));
        }
      }
      if (list)
      {
        return list.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant unoIDLServices() const
    {
      return getMemberList(m_cache.unoIDLServices,MemberListType_services,theTranslator->trServices());
    }
    TemplateVariant unoIDLInterfaces() const
    {
      return getMemberList(m_cache.unoIDLInterfaces,MemberListType_interfaces,theTranslator->trInterfaces());
    }
    TemplateVariant signals() const
    {
      return getMemberList(m_cache.signals,MemberListType_signals,theTranslator->trSignals());
    }
    TemplateVariant publicTypes() const
    {
      return getMemberList(m_cache.publicTypes,MemberListType_pubTypes,theTranslator->trPublicTypes());
    }
    TemplateVariant publicMethods() const
    {
      return getMemberList(m_cache.publicMethods,MemberListType_pubMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trInstanceMethods()
                                                     : theTranslator->trPublicMembers());
    }
    TemplateVariant publicStaticMethods() const
    {
      return getMemberList(m_cache.publicStaticMethods,MemberListType_pubStaticMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trClassMethods()
                                                     : theTranslator->trStaticPublicMembers());
    }
    TemplateVariant publicAttributes() const
    {
      return getMemberList(m_cache.publicAttributes,MemberListType_pubAttribs,theTranslator->trPublicAttribs());
    }
    TemplateVariant publicStaticAttributes() const
    {
      return getMemberList(m_cache.publicStaticAttributes,MemberListType_pubStaticAttribs,theTranslator->trStaticPublicAttribs());
    }
    TemplateVariant publicSlots() const
    {
      return getMemberList(m_cache.publicSlots,MemberListType_pubSlots,theTranslator->trPublicSlots());
    }
    TemplateVariant protectedTypes() const
    {
      return getMemberList(m_cache.protectedTypes,MemberListType_proTypes,theTranslator->trProtectedTypes());
    }
    TemplateVariant protectedMethods() const
    {
      return getMemberList(m_cache.protectedMethods,MemberListType_proMethods,theTranslator->trProtectedMembers());
    }
    TemplateVariant protectedStaticMethods() const
    {
      return getMemberList(m_cache.protectedStaticMethods,MemberListType_proStaticMethods,theTranslator->trStaticProtectedMembers());
    }
    TemplateVariant protectedAttributes() const
    {
      return getMemberList(m_cache.protectedAttributes,MemberListType_proAttribs,theTranslator->trProtectedAttribs());
    }
    TemplateVariant protectedStaticAttributes() const
    {
      return getMemberList(m_cache.protectedStaticAttributes,MemberListType_proStaticAttribs,theTranslator->trStaticProtectedAttribs());
    }
    TemplateVariant protectedSlots() const
    {
      return getMemberList(m_cache.protectedSlots,MemberListType_proSlots,theTranslator->trProtectedSlots());
    }
    TemplateVariant privateTypes() const
    {
      return getMemberList(m_cache.privateTypes,MemberListType_priTypes,theTranslator->trPrivateTypes());
    }
    TemplateVariant privateSlots() const
    {
      return getMemberList(m_cache.privateSlots,MemberListType_priSlots,theTranslator->trPrivateSlots());
    }
    TemplateVariant privateMethods() const
    {
      return getMemberList(m_cache.privateMethods,MemberListType_priMethods,theTranslator->trPrivateMembers());
    }
    TemplateVariant privateStaticMethods() const
    {
      return getMemberList(m_cache.privateStaticMethods,MemberListType_priStaticMethods,theTranslator->trStaticPrivateMembers());
    }
    TemplateVariant privateAttributes() const
    {
      return getMemberList(m_cache.privateAttributes,MemberListType_priAttribs,theTranslator->trPrivateAttribs());
    }
    TemplateVariant privateStaticAttributes() const
    {
      return getMemberList(m_cache.privateStaticAttributes,MemberListType_priStaticAttribs,theTranslator->trStaticPrivateAttribs());
    }
    TemplateVariant packageTypes() const
    {
      return getMemberList(m_cache.packageTypes,MemberListType_pacTypes,theTranslator->trPackageTypes());
    }
    TemplateVariant packageMethods() const
    {
      return getMemberList(m_cache.packageMethods,MemberListType_pacMethods,theTranslator->trPackageMembers());
    }
    TemplateVariant packageStaticMethods() const
    {
      return getMemberList(m_cache.packageStaticMethods,MemberListType_pacStaticMethods,theTranslator->trStaticPackageMembers());
    }
    TemplateVariant packageAttributes() const
    {
      return getMemberList(m_cache.packageAttributes,MemberListType_pacAttribs,theTranslator->trPackageAttribs());
    }
    TemplateVariant packageStaticAttributes() const
    {
      return getMemberList(m_cache.packageStaticAttributes,MemberListType_pacStaticAttribs,theTranslator->trStaticPackageAttribs());
    }
    TemplateVariant properties() const
    {
      return getMemberList(m_cache.properties,MemberListType_properties,theTranslator->trProperties());
    }
    TemplateVariant events() const
    {
      return getMemberList(m_cache.events,MemberListType_events,theTranslator->trEvents());
    }
    TemplateVariant friends() const
    {
      return getMemberList(m_cache.friends,MemberListType_friends,theTranslator->trFriends());
    }
    TemplateVariant related() const
    {
      return getMemberList(m_cache.related,MemberListType_related,theTranslator->trRelatedFunctions());
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(m_cache.detailedTypedefs,MemberListType_typedefMembers,theTranslator->trMemberTypedefDocumentation(),TRUE);
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(m_cache.detailedEnums,MemberListType_enumMembers,theTranslator->trMemberEnumerationDocumentation(),TRUE);
    }
    TemplateVariant detailedServices() const
    {
      return getMemberList(m_cache.detailedServices,MemberListType_serviceMembers,theTranslator->trServices(),TRUE);
    }
    TemplateVariant detailedInterfaces() const
    {
      return getMemberList(m_cache.detailedInterfaces,MemberListType_interfaceMembers,theTranslator->trInterfaces(),TRUE);
    }
    TemplateVariant detailedConstructors() const
    {
      return getMemberList(m_cache.detailedConstructors,MemberListType_constructors,theTranslator->trConstructorDocumentation(),TRUE);
    }
    TemplateVariant detailedMethods() const
    {
      return getMemberList(m_cache.detailedMethods,MemberListType_functionMembers,theTranslator->trMemberFunctionDocumentation(),TRUE);
    }
    TemplateVariant detailedRelated() const
    {
      return getMemberList(m_cache.detailedRelated,MemberListType_relatedMembers,theTranslator->trRelatedFunctionDocumentation(),TRUE);
    }
    TemplateVariant detailedVariables() const
    {
      return getMemberList(m_cache.detailedVariables,MemberListType_variableMembers,theTranslator->trMemberDataDocumentation(),TRUE);
    }
    TemplateVariant detailedProperties() const
    {
      return getMemberList(m_cache.detailedProperties,MemberListType_propertyMembers,theTranslator->trPropertyDocumentation(),TRUE);
    }
    TemplateVariant detailedEvents() const
    {
      return getMemberList(m_cache.detailedEvents,MemberListType_eventMembers,theTranslator->trEventDocumentation(),TRUE);
    }
    TemplateVariant classes() const
    {
      if (!m_cache.classes)
      {
        NestedClassListContext *classList = NestedClassListContext::alloc();
        if (m_classDef->getClassSDict())
        {
          ClassSDict::Iterator sdi(*m_classDef->getClassSDict());
          ClassDef *cd;
          for (sdi.toFirst();(cd=sdi.current());++sdi)
          {
            if (cd->visibleInParentsDeclList())
            {
              classList->append(cd);
            }
          }
        }
        m_cache.classes.reset(classList);
      }
      return m_cache.classes.get();
    }
    TemplateVariant compoundType() const
    {
      return m_classDef->compoundTypeString();
    }
    void addTemplateDecls(Definition *d,TemplateList *tl) const
    {
      if (d->definitionType()==Definition::TypeClass)
      {
        Definition *parent = d->getOuterScope();
        if (parent)
        {
          addTemplateDecls(parent,tl);
        }
        ClassDef *cd=(ClassDef *)d;
        if (cd->templateArguments())
        {
          ArgumentListContext *al = ArgumentListContext::alloc(cd->templateArguments(),cd,relPathAsString());
          // since a TemplateVariant does take ownership of the object, we add it
          // a separate list just to be able to delete it and avoid a memory leak
          tl->append(al);
        }
      }
    }
    void addExamples(TemplateList *list) const
    {
      if (m_classDef->hasExamples())
      {
        ExampleSDict::Iterator it(*m_classDef->exampleList());
        Example *ex;
        for (it.toFirst();(ex=it.current());++it)
        {
          TemplateStruct *s = TemplateStruct::alloc();
          s->set("text",ex->name);
          s->set("isLinkable",TRUE);
          s->set("anchor",ex->anchor);
          s->set("fileName",ex->file);
          list->append(s);
        }
      }
    }
    TemplateVariant templateDecls() const
    {
      if (!m_cache.templateDecls)
      {
        TemplateList *tl = TemplateList::alloc();
        addTemplateDecls(m_classDef,tl);
        m_cache.templateDecls.reset(tl);
      }
      return m_cache.templateDecls.get();
    }
    TemplateVariant typeConstraints() const
    {
      if (!m_cache.typeConstraints && m_classDef->typeConstraints())
      {
        m_cache.typeConstraints.reset(ArgumentListContext::alloc(m_classDef->typeConstraints(),m_classDef,relPathAsString()));
      }
      else
      {
        m_cache.typeConstraints.reset(ArgumentListContext::alloc());
      }
      return m_cache.typeConstraints.get();
    }
    TemplateVariant examples() const
    {
      if (!m_cache.examples)
      {
        TemplateList *exampleList = TemplateList::alloc();
        addExamples(exampleList);
        m_cache.examples.reset(exampleList);
      }
      return m_cache.examples.get();
    }
    void addMembers(ClassDef *cd,MemberListType lt) const
    {
      MemberList *ml = cd->getMemberList(lt);
      if (ml)
      {
        MemberListIterator li(*ml);
        const MemberDef *md;
        for (li.toFirst();(md=li.current());++li)
        {
          if (md->isBriefSectionVisible())
          {
            m_cache.allMembers.append(md);
          }
        }
      }
    }
    TemplateVariant members() const
    {
      if (!m_cache.members)
      {
        addMembers(m_classDef,MemberListType_pubTypes);
        addMembers(m_classDef,MemberListType_services);
        addMembers(m_classDef,MemberListType_interfaces);
        addMembers(m_classDef,MemberListType_pubSlots);
        addMembers(m_classDef,MemberListType_signals);
        addMembers(m_classDef,MemberListType_pubMethods);
        addMembers(m_classDef,MemberListType_pubStaticMethods);
        addMembers(m_classDef,MemberListType_pubAttribs);
        addMembers(m_classDef,MemberListType_pubStaticAttribs);
        addMembers(m_classDef,MemberListType_proTypes);
        addMembers(m_classDef,MemberListType_proSlots);
        addMembers(m_classDef,MemberListType_proMethods);
        addMembers(m_classDef,MemberListType_proStaticMethods);
        addMembers(m_classDef,MemberListType_proAttribs);
        addMembers(m_classDef,MemberListType_proStaticAttribs);
        addMembers(m_classDef,MemberListType_pacTypes);
        addMembers(m_classDef,MemberListType_pacMethods);
        addMembers(m_classDef,MemberListType_pacStaticMethods);
        addMembers(m_classDef,MemberListType_pacAttribs);
        addMembers(m_classDef,MemberListType_pacStaticAttribs);
        addMembers(m_classDef,MemberListType_properties);
        addMembers(m_classDef,MemberListType_events);
        addMembers(m_classDef,MemberListType_priTypes);
        addMembers(m_classDef,MemberListType_priSlots);
        addMembers(m_classDef,MemberListType_priMethods);
        addMembers(m_classDef,MemberListType_priStaticMethods);
        addMembers(m_classDef,MemberListType_priAttribs);
        addMembers(m_classDef,MemberListType_priStaticAttribs);
        addMembers(m_classDef,MemberListType_related);
        m_cache.members.reset(MemberListContext::alloc(&m_cache.allMembers));
      }
      return m_cache.members.get();
    }
    TemplateVariant allMembersList() const
    {
      if (!m_cache.allMembersList)
      {
        if (m_classDef->memberNameInfoSDict())
        {
          AllMembersListContext *ml = AllMembersListContext::alloc(m_classDef->memberNameInfoSDict());
          m_cache.allMembersList.reset(ml);
        }
        else
        {
          m_cache.allMembersList.reset(AllMembersListContext::alloc());
        }
      }
      return m_cache.allMembersList.get();
    }
    TemplateVariant allMembersFileName() const
    {
      return m_classDef->getMemberListFileName();
    }
    TemplateVariant memberGroups() const
    {
      if (!m_cache.memberGroups)
      {
        if (m_classDef->getMemberGroupSDict())
        {
          m_cache.memberGroups.reset(MemberGroupListContext::alloc(m_classDef,relPathAsString(),m_classDef->getMemberGroupSDict(),m_classDef->subGrouping()));
        }
        else
        {
          m_cache.memberGroups.reset(MemberGroupListContext::alloc());
        }
      }
      return m_cache.memberGroups.get();
    }
    TemplateVariant additionalInheritedMembers() const
    {
      if (!m_cache.additionalInheritedMembers)
      {
        InheritedMemberInfoListContext *ctx = InheritedMemberInfoListContext::alloc();
        ctx->addMemberList(m_classDef,MemberListType_pubTypes,theTranslator->trPublicTypes());
        ctx->addMemberList(m_classDef,MemberListType_services,theTranslator->trServices());
        ctx->addMemberList(m_classDef,MemberListType_interfaces,theTranslator->trInterfaces());
        ctx->addMemberList(m_classDef,MemberListType_pubSlots,theTranslator->trPublicSlots());
        ctx->addMemberList(m_classDef,MemberListType_signals,theTranslator->trSignals());
        ctx->addMemberList(m_classDef,MemberListType_pubMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trInstanceMethods()
                                                     : theTranslator->trPublicMembers());
        ctx->addMemberList(m_classDef,MemberListType_pubStaticMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trClassMethods()
                                                     : theTranslator->trStaticPublicMembers());
        ctx->addMemberList(m_classDef,MemberListType_pubAttribs,theTranslator->trPublicAttribs());
        ctx->addMemberList(m_classDef,MemberListType_pubStaticAttribs,theTranslator->trStaticPublicAttribs());
        ctx->addMemberList(m_classDef,MemberListType_proTypes,theTranslator->trProtectedTypes());
        ctx->addMemberList(m_classDef,MemberListType_proSlots,theTranslator->trProtectedSlots());
        ctx->addMemberList(m_classDef,MemberListType_proMethods,theTranslator->trProtectedMembers());
        ctx->addMemberList(m_classDef,MemberListType_proStaticMethods,theTranslator->trStaticProtectedMembers());
        ctx->addMemberList(m_classDef,MemberListType_proAttribs,theTranslator->trProtectedAttribs());
        ctx->addMemberList(m_classDef,MemberListType_proStaticAttribs,theTranslator->trStaticProtectedAttribs());
        ctx->addMemberList(m_classDef,MemberListType_pacTypes,theTranslator->trPackageTypes());
        ctx->addMemberList(m_classDef,MemberListType_pacMethods,theTranslator->trPackageMembers());
        ctx->addMemberList(m_classDef,MemberListType_pacStaticMethods,theTranslator->trStaticPackageMembers());
        ctx->addMemberList(m_classDef,MemberListType_pacAttribs,theTranslator->trPackageAttribs());
        ctx->addMemberList(m_classDef,MemberListType_pacStaticAttribs,theTranslator->trStaticPackageAttribs());
        ctx->addMemberList(m_classDef,MemberListType_properties,theTranslator->trProperties());
        ctx->addMemberList(m_classDef,MemberListType_events,theTranslator->trEvents());
        ctx->addMemberList(m_classDef,MemberListType_priTypes,theTranslator->trPrivateTypes());
        ctx->addMemberList(m_classDef,MemberListType_priSlots,theTranslator->trPrivateSlots());
        ctx->addMemberList(m_classDef,MemberListType_priMethods,theTranslator->trPrivateMembers());
        ctx->addMemberList(m_classDef,MemberListType_priStaticMethods,theTranslator->trStaticPrivateMembers());
        ctx->addMemberList(m_classDef,MemberListType_priAttribs,theTranslator->trPrivateAttribs());
        ctx->addMemberList(m_classDef,MemberListType_priStaticAttribs,theTranslator->trStaticPrivateAttribs());
        ctx->addMemberList(m_classDef,MemberListType_related,theTranslator->trRelatedFunctions());
        m_cache.additionalInheritedMembers.reset(ctx);
      }
      return m_cache.additionalInheritedMembers.get();
    }

  private:
    ClassDef *m_classDef;
    struct Cachable
    {
      Cachable() : inheritanceNodes(-1) { }
      SharedPtr<IncludeInfoContext>     includeInfo;
      SharedPtr<InheritanceListContext> inheritsList;
      SharedPtr<InheritanceListContext> inheritedByList;
      ScopedPtr<DotClassGraph>          classGraph;
      ScopedPtr<DotClassGraph>          collaborationGraph;
      SharedPtr<NestedClassListContext> classes;
      SharedPtr<MemberListInfoContext>  publicTypes;
      SharedPtr<MemberListInfoContext>  publicMethods;
      SharedPtr<MemberListInfoContext>  publicStaticMethods;
      SharedPtr<MemberListInfoContext>  publicAttributes;
      SharedPtr<MemberListInfoContext>  publicStaticAttributes;
      SharedPtr<MemberListInfoContext>  publicSlots;
      SharedPtr<MemberListInfoContext>  protectedTypes;
      SharedPtr<MemberListInfoContext>  protectedMethods;
      SharedPtr<MemberListInfoContext>  protectedStaticMethods;
      SharedPtr<MemberListInfoContext>  protectedAttributes;
      SharedPtr<MemberListInfoContext>  protectedStaticAttributes;
      SharedPtr<MemberListInfoContext>  protectedSlots;
      SharedPtr<MemberListInfoContext>  privateTypes;
      SharedPtr<MemberListInfoContext>  privateMethods;
      SharedPtr<MemberListInfoContext>  privateStaticMethods;
      SharedPtr<MemberListInfoContext>  privateAttributes;
      SharedPtr<MemberListInfoContext>  privateStaticAttributes;
      SharedPtr<MemberListInfoContext>  privateSlots;
      SharedPtr<MemberListInfoContext>  packageTypes;
      SharedPtr<MemberListInfoContext>  packageMethods;
      SharedPtr<MemberListInfoContext>  packageStaticMethods;
      SharedPtr<MemberListInfoContext>  packageAttributes;
      SharedPtr<MemberListInfoContext>  packageStaticAttributes;
      SharedPtr<MemberListInfoContext>  unoIDLServices;
      SharedPtr<MemberListInfoContext>  unoIDLInterfaces;
      SharedPtr<MemberListInfoContext>  signals;
      SharedPtr<MemberListInfoContext>  properties;
      SharedPtr<MemberListInfoContext>  events;
      SharedPtr<MemberListInfoContext>  friends;
      SharedPtr<MemberListInfoContext>  related;
      SharedPtr<MemberListInfoContext>  detailedTypedefs;
      SharedPtr<MemberListInfoContext>  detailedEnums;
      SharedPtr<MemberListInfoContext>  detailedServices;
      SharedPtr<MemberListInfoContext>  detailedInterfaces;
      SharedPtr<MemberListInfoContext>  detailedConstructors;
      SharedPtr<MemberListInfoContext>  detailedMethods;
      SharedPtr<MemberListInfoContext>  detailedRelated;
      SharedPtr<MemberListInfoContext>  detailedVariables;
      SharedPtr<MemberListInfoContext>  detailedProperties;
      SharedPtr<MemberListInfoContext>  detailedEvents;
      SharedPtr<MemberGroupListContext> memberGroups;
      SharedPtr<AllMembersListContext>  allMembersList;
      SharedPtr<ArgumentListContext>    typeConstraints;
      SharedPtr<TemplateList>           examples;
      SharedPtr<TemplateList>           templateDecls;
      SharedPtr<InheritedMemberInfoListContext> additionalInheritedMembers;
      SharedPtr<MemberListContext>      members;
      SharedPtr<UsedFilesContext>       usedFiles;
      SharedPtr<TemplateList>           exampleList;
      int                               inheritanceNodes;
      MemberList                        allMembers;
    };
    mutable Cachable m_cache;
};
//%% }

ClassContext::ClassContext(ClassDef *cd) : RefCountedContext("ClassContext")
{
  //printf("ClassContext::ClassContext(%s)\n",cd?cd->name().data():"<none>");
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
    Private(NamespaceDef *nd) : DefinitionContext<NamespaceContext::Private>(nd) , m_namespaceDef(nd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
      addProperty("compoundType",this,&Private::compoundType);
      addProperty("hasDetails",this,&Private::hasDetails);
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
    TemplateVariant compoundType() const
    {
      return m_namespaceDef->compoundTypeString();
    }
    TemplateVariant hasDetails() const
    {
      return m_namespaceDef->hasDetailedDescription();
    }
  private:
    NamespaceDef *m_namespaceDef;
};
//%% }

NamespaceContext::NamespaceContext(NamespaceDef *nd) : RefCountedContext("NamespaceContext")
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
    Private(FileDef *fd) : DefinitionContext<FileContext::Private>(fd) , m_fileDef(fd)
    {
      if (fd==0) abort();
      addProperty("title",                     this,&Private::title);
      addProperty("highlight",                 this,&Private::highlight);
      addProperty("subhighlight",              this,&Private::subHighlight);
      addProperty("versionInfo",               this,&Private::versionInfo);
      addProperty("includeList",               this,&Private::includeList);
      addProperty("hasIncludeGraph",           this,&Private::hasIncludeGraph);
      addProperty("hasIncludedByGraph",        this,&Private::hasIncludedByGraph);
      addProperty("includeGraph",              this,&Private::includeGraph);
      addProperty("includedByGraph",           this,&Private::includedByGraph);
      addProperty("hasDetails",                this,&Private::hasDetails);
      addProperty("hasSourceFile",             this,&Private::hasSourceFile);
      addProperty("sources",                   this,&Private::sources);
      addProperty("version",                   this,&Private::version);
      addProperty("classes",                   this,&Private::classes);
      addProperty("namespaces",                this,&Private::namespaces);
      addProperty("constantgroups",            this,&Private::constantgroups);
      addProperty("macros",                    this,&Private::macros);
      addProperty("typedefs",                  this,&Private::typedefs);
      addProperty("enums",                     this,&Private::enums);
      addProperty("functions",                 this,&Private::functions);
      addProperty("variables",                 this,&Private::variables);
      addProperty("memberGroups",              this,&Private::memberGroups);
      addProperty("detailedMacros",            this,&Private::detailedMacros);
      addProperty("detailedTypedefs",          this,&Private::detailedTypedefs);
      addProperty("detailedEnums",             this,&Private::detailedEnums);
      addProperty("detailedFunctions",         this,&Private::detailedFunctions);
      addProperty("detailedVariables",         this,&Private::detailedVariables);
      addProperty("inlineClasses",             this,&Private::inlineClasses);
      addProperty("compoundType",              this,&Private::compoundType);
    }
    TemplateVariant title() const
    {
      return m_fileDef->title();
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("files");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
    TemplateVariant versionInfo() const
    {
      return m_fileDef->getVersion();
    }
    TemplateVariant includeList() const
    {
      if (!m_cache.includeInfoList && m_fileDef->includeFileList())
      {
        m_cache.includeInfoList.reset(IncludeInfoListContext::alloc(
              *m_fileDef->includeFileList(),m_fileDef->getLanguage()));
      }
      if (m_cache.includeInfoList)
      {
        return m_cache.includeInfoList.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    DotInclDepGraph *getIncludeGraph() const
    {
      if (!m_cache.includeGraph)
      {
        m_cache.includeGraph.reset(new DotInclDepGraph(m_fileDef,FALSE));
      }
      return m_cache.includeGraph.get();
    }
    TemplateVariant hasIncludeGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      DotInclDepGraph *incGraph = getIncludeGraph();
      return (haveDot && !incGraph->isTooBig() && !incGraph->isTrivial());
    }
    TemplateVariant includeGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      QGString result;
      if (haveDot)
      {
        DotInclDepGraph *cg = getIncludeGraph();
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
            g_globals.outputDir,
            g_globals.outputDir+portable_pathSeparator()+m_fileDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,g_globals.dynSectionId
            );
      }
      g_globals.dynSectionId++;
      return TemplateVariant(result.data(),TRUE);
    }
    DotInclDepGraph *getIncludedByGraph() const
    {
      if (!m_cache.includedByGraph)
      {
        m_cache.includedByGraph.reset(new DotInclDepGraph(m_fileDef,TRUE));
      }
      return m_cache.includedByGraph.get();
    }
    TemplateVariant hasIncludedByGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      DotInclDepGraph *incGraph = getIncludedByGraph();
      return (haveDot && !incGraph->isTooBig() && !incGraph->isTrivial());
    }
    TemplateVariant includedByGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      QGString result;
      if (haveDot)
      {
        DotInclDepGraph *cg = getIncludedByGraph();
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
            g_globals.outputDir,
            g_globals.outputDir+portable_pathSeparator()+m_fileDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,g_globals.dynSectionId
            );
      }
      g_globals.dynSectionId++;
      return TemplateVariant(result.data(),TRUE);
    }
    TemplateVariant hasDetails() const
    {
      return m_fileDef->hasDetailedDescription();
    }
    TemplateVariant hasSourceFile() const
    {
      return m_fileDef->generateSourceFile();
    }
    TemplateVariant sources() const
    {
      if (!m_cache.sources)
      {
        if (m_fileDef->generateSourceFile())
        {
          m_cache.sources.reset(new TemplateVariant(parseCode(m_fileDef,relPathAsString())));
        }
        else
        {
          m_cache.sources.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.sources;
    }
    TemplateVariant version() const
    {
      return m_fileDef->fileVersion();
    }
    TemplateVariant classes() const
    {
      if (!m_cache.classes)
      {
        NestedClassListContext *classList = NestedClassListContext::alloc();
        if (m_fileDef->getClassSDict())
        {
          ClassSDict::Iterator sdi(*m_fileDef->getClassSDict());
          ClassDef *cd;
          for (sdi.toFirst();(cd=sdi.current());++sdi)
          {
            if (cd->visibleInParentsDeclList())
            {
              classList->append(cd);
            }
          }
        }
        m_cache.classes.reset(classList);
      }
      return m_cache.classes.get();
    }
    TemplateVariant namespaces() const
    {
      if (!m_cache.namespaces)
      {
        NestedNamespaceListContext *namespaceList = NestedNamespaceListContext::alloc();
        if (m_fileDef->getNamespaceSDict())
        {
          NamespaceSDict::Iterator sdi(*m_fileDef->getNamespaceSDict());
          NamespaceDef *nd;
          for (sdi.toFirst();(nd=sdi.current());++sdi)
          {
            if (nd->isLinkable() && !nd->isConstantGroup())
            {
              namespaceList->append(nd);
            }
          }
        }
        m_cache.namespaces.reset(namespaceList);
      }
      return m_cache.namespaces.get();
    }
    TemplateVariant constantgroups() const
    {
      if (!m_cache.constantgroups)
      {
        NestedNamespaceListContext *namespaceList = NestedNamespaceListContext::alloc();
        if (m_fileDef->getNamespaceSDict())
        {
          NamespaceSDict::Iterator sdi(*m_fileDef->getNamespaceSDict());
          NamespaceDef *nd;
          for (sdi.toFirst();(nd=sdi.current());++sdi)
          {
            if (nd->isLinkable() && nd->isConstantGroup())
            {
              namespaceList->append(nd);
            }
          }
        }
        m_cache.constantgroups.reset(namespaceList);
      }
      return m_cache.constantgroups.get();
    }
    TemplateVariant getMemberList(SharedPtr<MemberListInfoContext> &list,
                                  MemberListType type,const char *title,bool detailed=FALSE) const
    {
      if (!list)
      {
        MemberList *ml = m_fileDef->getMemberList(type);
        if (ml)
        {
          list.reset(MemberListInfoContext::alloc(m_fileDef,relPathAsString(),ml,title,detailed));
        }
      }
      if (list)
      {
        return list.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant macros() const
    {
      return getMemberList(m_cache.macros,MemberListType_decDefineMembers,theTranslator->trDefines());
    }
    TemplateVariant typedefs() const
    {
      return getMemberList(m_cache.typedefs,MemberListType_decTypedefMembers,theTranslator->trTypedefs());
    }
    TemplateVariant enums() const
    {
      return getMemberList(m_cache.enums,MemberListType_decEnumMembers,theTranslator->trEnumerations());
    }
    TemplateVariant functions() const
    {
      // TODO: Fortran: trSubprograms()
      // TODO: VHDL:    VhdlDocGen::trFunctionAndProc()
      return getMemberList(m_cache.functions,MemberListType_decFuncMembers,theTranslator->trFunctions());
    }
    TemplateVariant variables() const
    {
      return getMemberList(m_cache.variables,MemberListType_decVarMembers,theTranslator->trVariables());
    }
    TemplateVariant memberGroups() const
    {
      if (!m_cache.memberGroups)
      {
        if (m_fileDef->getMemberGroupSDict())
        {
          m_cache.memberGroups.reset(MemberGroupListContext::alloc(m_fileDef,relPathAsString(),m_fileDef->getMemberGroupSDict(),m_fileDef->subGrouping()));
        }
        else
        {
          m_cache.memberGroups.reset(MemberGroupListContext::alloc());
        }
      }
      return m_cache.memberGroups.get();
    }
    TemplateVariant detailedMacros() const
    {
      return getMemberList(m_cache.detailedMacros,MemberListType_docDefineMembers,theTranslator->trDefineDocumentation());
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(m_cache.detailedTypedefs,MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation());
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(m_cache.detailedEnums,MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation());
    }
    TemplateVariant detailedFunctions() const
    {
      // TODO: Fortran: trSubprogramDocumentation()
      return getMemberList(m_cache.detailedFunctions,MemberListType_docFuncMembers,theTranslator->trFunctionDocumentation());
    }
    TemplateVariant detailedVariables() const
    {
      return getMemberList(m_cache.detailedVariables,MemberListType_docVarMembers,theTranslator->trVariableDocumentation());
    }
    TemplateVariant inlineClasses() const
    {
      if (!m_cache.inlineClasses)
      {
        NestedClassListContext *classList = NestedClassListContext::alloc();
        if (m_fileDef->getClassSDict())
        {
          ClassSDict::Iterator sdi(*m_fileDef->getClassSDict());
          ClassDef *cd;
          for (sdi.toFirst();(cd=sdi.current());++sdi)
          {
            if (cd->name().find('@')==-1 &&
                cd->isLinkableInProject() &&
                cd->isEmbeddedInOuterScope() &&
                cd->partOfGroups()==0)
            {
              classList->append(cd);
            }
          }
        }
        m_cache.inlineClasses.reset(classList);
      }
      return m_cache.inlineClasses.get();
    }
    TemplateVariant compoundType() const
    {
      return theTranslator->trFile(FALSE,TRUE);
    }

  private:
    FileDef *m_fileDef;
    struct Cachable
    {
      SharedPtr<IncludeInfoListContext>     includeInfoList;
      ScopedPtr<DotInclDepGraph>            includeGraph;
      ScopedPtr<DotInclDepGraph>            includedByGraph;
      ScopedPtr<TemplateVariant>            sources;
      SharedPtr<NestedClassListContext>     classes;
      SharedPtr<NestedNamespaceListContext> namespaces;
      SharedPtr<NestedNamespaceListContext> constantgroups;
      SharedPtr<MemberListInfoContext>      macros;
      SharedPtr<MemberListInfoContext>      typedefs;
      SharedPtr<MemberListInfoContext>      enums;
      SharedPtr<MemberListInfoContext>      functions;
      SharedPtr<MemberListInfoContext>      variables;
      SharedPtr<MemberGroupListContext>     memberGroups;
      SharedPtr<MemberListInfoContext>      detailedMacros;
      SharedPtr<MemberListInfoContext>      detailedTypedefs;
      SharedPtr<MemberListInfoContext>      detailedEnums;
      SharedPtr<MemberListInfoContext>      detailedFunctions;
      SharedPtr<MemberListInfoContext>      detailedVariables;
      SharedPtr<NestedClassListContext>     inlineClasses;
    };
    mutable Cachable m_cache;
};
//%% }

FileContext::FileContext(FileDef *fd) : RefCountedContext("FileContext")
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
    Private(DirDef *dd) : DefinitionContext<DirContext::Private>(dd) , m_dirDef(dd)
    {
      addProperty("title",         this,&Private::title);
      addProperty("highlight",     this,&Private::highlight);
      addProperty("subhighlight",  this,&Private::subHighlight);
      addProperty("dirName",       this,&Private::dirName);
      addProperty("dirs",          this,&Private::dirs);
      addProperty("files",         this,&Private::files);
      addProperty("hasDetails",    this,&Private::hasDetails);
      addProperty("compoundType",  this,&Private::compoundType);
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
    TemplateVariant dirs() const
    {
      if (!m_cache.dirs)
      {
        m_cache.dirs.reset(TemplateList::alloc());
        const DirList &subDirs = m_dirDef->subDirs();
        QListIterator<DirDef> it(subDirs);
        DirDef *dd;
        for (it.toFirst();(dd=it.current());++it)
        {
          DirContext *dc = new DirContext(dd);
          m_cache.dirs->append(dc);
        }
      }
      return m_cache.dirs.get();
    }
    TemplateVariant files() const
    {
      // FileList *list = m_dirDef->getFiles();
      if (!m_cache.files)
      {
        m_cache.files.reset(TemplateList::alloc());
        FileList *files = m_dirDef->getFiles();
        if (files)
        {
          QListIterator<FileDef> it(*files);
          FileDef *fd;
          for (it.toFirst();(fd=it.current());++it)
          {
            FileContext *fc = FileContext::alloc(fd);
            m_cache.files->append(fc);
          }
        }
      }
      return m_cache.files.get();
    }
    TemplateVariant hasDetails() const
    {
      return m_dirDef->hasDetailedDescription();
    }
    TemplateVariant compoundType() const
    {
      return theTranslator->trDir(FALSE,TRUE);
    }

  private:
    DirDef *m_dirDef;
    struct Cachable
    {
      Cachable() {}
      SharedPtr<TemplateList>  dirs;
      SharedPtr<TemplateList>  files;
    };
    mutable Cachable m_cache;
};
//%% }

DirContext::DirContext(DirDef *fd) : RefCountedContext("DirContext")
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
    Private(PageDef *pd) : DefinitionContext<PageContext::Private>(pd) , m_pageDef(pd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
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

PageContext::PageContext(PageDef *pd) : RefCountedContext("PageContext")
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

class TextGeneratorHtml : public TextGeneratorIntf
{
  public:
    TextGeneratorHtml(FTextStream &ts,const QCString &relPath)
       : m_ts(ts), m_relPath(relPath) {}
    void writeString(const char *s,bool keepSpaces) const
    {
      if (s==0) return;
      //printf("TextGeneratorOlImpl::writeString('%s',%d)\n",s,keepSpaces);
      if (keepSpaces)
      {
        const char *p=s;
        char c;
        while ((c=*p++))
        {
          switch(c)
          {
            case '<':  m_ts << "&lt;"; break;
            case '>':  m_ts << "&gt;"; break;
            case '\'': m_ts << "&#39;"; break;
            case '"':  m_ts << "&quot;"; break;
            case '&':  m_ts << "&amp;"; break;
            case ' ':  m_ts << "&#160;"; break;
          }
        }
      }
      else
      {
        m_ts << convertToHtml(s);
      }
    }

    void writeBreak(int indent) const
    {
      m_ts << "<br />";
      for (int i=0;i<indent;i++)
      {
        m_ts << "&#160;";
      }
    }

    void writeLink(const char *ref,const char *f,
                   const char *anchor,const char *name
                  ) const
    {
      if (ref)
      {
        m_ts << "<a class=\"elRef\" ";
        m_ts << externalLinkTarget() << externalRef(m_relPath,ref,FALSE);
      }
      else
      {
        m_ts << "<a class=\"el\" ";
      }
      m_ts << "href=\"";
      m_ts << externalRef(m_relPath,ref,TRUE);
      if (f) m_ts << f << Doxygen::htmlFileExtension;
      if (anchor) m_ts << "#" << anchor;
      m_ts << "\">";
      m_ts << convertToHtml(name);
      m_ts << "</a>";
    }

  private:
    FTextStream &m_ts;
    QCString m_relPath;
};

class TextGeneratorFactory
{
  public:
    static TextGeneratorFactory *instance()
    {
      static TextGeneratorFactory *instance = 0;
      if (instance==0) instance = new TextGeneratorFactory;
      return instance;
    }
    TextGeneratorIntf *create(FTextStream &ts,const QCString &relPath)
    {
      switch (g_globals.outputFormat)
      {
        case ContextGlobals::Html:
          return new TextGeneratorHtml(ts,relPath);
          break;
        default:
          break;
      }
      return 0;
    }
  private:
    TextGeneratorFactory() {}
    virtual ~TextGeneratorFactory() {}
};

TemplateVariant createLinkedText(Definition *def,const QCString &relPath,const QCString &text)
{
  QGString s;
  FTextStream ts(&s);
  TextGeneratorIntf *tg = TextGeneratorFactory::instance()->create(ts,relPath);
  if (tg)
  {
    linkifyText(*tg,def->getOuterScope(),def->getBodyDef(),def,text);
    return TemplateVariant(s.data(),TRUE);
  }
  else
  {
    return text;
  }
}

//%% struct Member(Symbol): member information
//%% {
class MemberContext::Private : public DefinitionContext<MemberContext::Private>
{
  public:
    Private(MemberDef *md) : DefinitionContext<MemberContext::Private>(md) , m_memberDef(md)
    {
      addProperty("isSignal",            this,&Private::isSignal);
      addProperty("isSlot",              this,&Private::isSlot);
      addProperty("isVariable",          this,&Private::isVariable);
      addProperty("isEnumeration",       this,&Private::isEnumeration);
      addProperty("isEnumValue",         this,&Private::isEnumValue);
      addProperty("isTypedef",           this,&Private::isTypedef);
      addProperty("isFunction",          this,&Private::isFunction);
      addProperty("isFunctionPtr",       this,&Private::isFunctionPtr);
      addProperty("isDefine",            this,&Private::isDefine);
      addProperty("isFriend",            this,&Private::isFriend);
      addProperty("isProperty",          this,&Private::isProperty);
      addProperty("isEvent",             this,&Private::isEvent);
      addProperty("isRelated",           this,&Private::isRelated);
      addProperty("isForeign",           this,&Private::isForeign);
      addProperty("isStatic",            this,&Private::isStatic);
      addProperty("isInline",            this,&Private::isInline);
      addProperty("isExplicit",          this,&Private::isExplicit);
      addProperty("isMutable",           this,&Private::isMutable);
      addProperty("isGettable",          this,&Private::isGettable);
      addProperty("isSettable",          this,&Private::isSettable);
      addProperty("isReadable",          this,&Private::isReadable);
      addProperty("isWritable",          this,&Private::isWritable);
      addProperty("isAddable",           this,&Private::isAddable);
      addProperty("isRemovable",         this,&Private::isRemovable);
      addProperty("isRaisable",          this,&Private::isRaisable);
      addProperty("isFinal",             this,&Private::isFinal);
      addProperty("isAbstract",          this,&Private::isAbstract);
      addProperty("isOverride",          this,&Private::isOverride);
      addProperty("isInitonly",          this,&Private::isInitonly);
      addProperty("isOptional",          this,&Private::isOptional);
      addProperty("isRequired",          this,&Private::isRequired);
      addProperty("isNonAtomic",         this,&Private::isNonAtomic);
      addProperty("isCopy",              this,&Private::isCopy);
      addProperty("isAssign",            this,&Private::isAssign);
      addProperty("isRetain",            this,&Private::isRetain);
      addProperty("isWeak",              this,&Private::isWeak);
      addProperty("isStrong",            this,&Private::isStrong);
      addProperty("isUnretained",        this,&Private::isUnretained);
      addProperty("isNew",               this,&Private::isNew);
      addProperty("isSealed",            this,&Private::isSealed);
      addProperty("isImplementation",    this,&Private::isImplementation);
      addProperty("isExternal",          this,&Private::isExternal);
      addProperty("isAlias",             this,&Private::isAlias);
      addProperty("isDefault",           this,&Private::isDefault);
      addProperty("isDelete",            this,&Private::isDelete);
      addProperty("isNoExcept",          this,&Private::isNoExcept);
      addProperty("isAttribute",         this,&Private::isAttribute);
      addProperty("isUNOProperty",       this,&Private::isUNOProperty);
      addProperty("isReadonly",          this,&Private::isReadonly);
      addProperty("isBound",             this,&Private::isBound);
      addProperty("isConstrained",       this,&Private::isConstrained);
      addProperty("isTransient",         this,&Private::isTransient);
      addProperty("isMaybeVoid",         this,&Private::isMaybeVoid);
      addProperty("isMaybeDefault",      this,&Private::isMaybeDefault);
      addProperty("isMaybeAmbiguous",    this,&Private::isMaybeAmbiguous);
      addProperty("isPublished",         this,&Private::isPublished);
      addProperty("isTemplateSpecialization",this,&Private::isTemplateSpecialization);
      addProperty("isObjCMethod",        this,&Private::isObjCMethod);
      addProperty("isObjCProperty",      this,&Private::isObjCProperty);
      addProperty("isAnonymous",         this,&Private::isAnonymous);
      addProperty("declType",            this,&Private::declType);
      addProperty("declArgs",            this,&Private::declArgs);
      addProperty("anonymousType",       this,&Private::anonymousType);
      addProperty("anonymousMember",     this,&Private::anonymousMember);
      addProperty("hasDetails",          this,&Private::hasDetails);
      addProperty("exception",           this,&Private::exception);
      addProperty("bitfields",           this,&Private::bitfields);
      addProperty("initializer",         this,&Private::initializer);
      addProperty("initializerAsCode",   this,&Private::initializerAsCode);
      addProperty("hasOneLineInitializer",   this,&Private::hasOneLineInitializer);
      addProperty("hasMultiLineInitializer", this,&Private::hasMultiLineInitializer);
      addProperty("templateArgs",        this,&Private::templateArgs);
      addProperty("templateAlias",       this,&Private::templateAlias);
      addProperty("propertyAttrs",       this,&Private::propertyAttrs);
      addProperty("eventAttrs",          this,&Private::eventAttrs);
      addProperty("class",               this,&Private::getClass);
      addProperty("definition",          this,&Private::definition);
      addProperty("parameters",          this,&Private::parameters);
      addProperty("hasParameterList",    this,&Private::hasParameterList);
      addProperty("hasConstQualifier",   this,&Private::hasConstQualifier);
      addProperty("hasVolatileQualifier",this,&Private::hasVolatileQualifier);
      addProperty("trailingReturnType",  this,&Private::trailingReturnType);
      addProperty("extraTypeChars",      this,&Private::extraTypeChars);
      addProperty("templateDecls",       this,&Private::templateDecls);
      addProperty("labels",              this,&Private::labels);
      addProperty("enumBaseType",        this,&Private::enumBaseType);
      addProperty("enumValues",          this,&Private::enumValues);
      addProperty("paramDocs",           this,&Private::paramDocs);
      addProperty("reimplements",        this,&Private::reimplements);
      addProperty("implements",          this,&Private::implements);
      addProperty("reimplementedBy",     this,&Private::reimplementedBy);
      addProperty("implementedBy",       this,&Private::implementedBy);
      addProperty("examples",            this,&Private::examples);
      addProperty("typeConstraints",     this,&Private::typeConstraints);
      addProperty("functionQualifier",   this,&Private::functionQualifier);
      addProperty("sourceRefs",          this,&Private::sourceRefs);
      addProperty("sourceRefBys",        this,&Private::sourceRefBys);
      addProperty("hasSources",          this,&Private::hasSources);
      addProperty("sourceCode",          this,&Private::sourceCode);
      addProperty("hasCallGraph",        this,&Private::hasCallGraph);
      addProperty("callGraph",           this,&Private::callGraph);
      addProperty("hasCallerGraph",      this,&Private::hasCallerGraph);
      addProperty("callerGraph",         this,&Private::callerGraph);
      addProperty("fieldType",           this,&Private::fieldType);

      m_cache.propertyAttrs.reset(TemplateList::alloc());
      if (md && md->isProperty())
      {
        if (md->isGettable()) m_cache.propertyAttrs->append("get");
        if (md->isSettable()) m_cache.propertyAttrs->append("set");
      }
      m_cache.eventAttrs.reset(TemplateList::alloc());
      if (md && md->isEvent())
      {
        if (md->isAddable())   m_cache.eventAttrs->append("add");
        if (md->isRemovable()) m_cache.eventAttrs->append("remove");
        if (md->isRaisable())  m_cache.eventAttrs->append("raise");
      }
    }
    TemplateVariant fieldType() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->fieldType());
    }
    TemplateVariant declType() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->getDeclType());
    }
    TemplateVariant declArgs() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->argsString());
    }
    TemplateVariant exception() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->excpString());
    }
    TemplateVariant bitfields() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->bitfieldString());
    }
    TemplateVariant isStatic() const
    {
      return m_memberDef->isStatic();
    }
    TemplateVariant isObjCMethod() const
    {
      return m_memberDef->isObjCMethod();
    }
    TemplateVariant isObjCProperty() const
    {
      return m_memberDef->isObjCProperty();
    }
    TemplateVariant isImplementation() const
    {
      return m_memberDef->isImplementation();
    }
    TemplateVariant isSignal() const
    {
      return m_memberDef->isSignal();
    }
    TemplateVariant isSlot() const
    {
      return m_memberDef->isSlot();
    }
    TemplateVariant isTypedef() const
    {
      return m_memberDef->isTypedef();
    }
    TemplateVariant isFunction() const
    {
      return m_memberDef->isFunction();
    }
    TemplateVariant isFunctionPtr() const
    {
      return m_memberDef->isFunctionPtr();
    }
    TemplateVariant isFriend() const
    {
      return m_memberDef->isFriend();
    }
    TemplateVariant isForeign() const
    {
      return m_memberDef->isForeign();
    }
    TemplateVariant isEvent() const
    {
      return m_memberDef->isEvent();
    }
    TemplateVariant isInline() const
    {
      return m_memberDef->isInline();
    }
    TemplateVariant isExplicit() const
    {
      return m_memberDef->isExplicit();
    }
    TemplateVariant isMutable() const
    {
      return m_memberDef->isMutable();
    }
    TemplateVariant isGettable() const
    {
      return m_memberDef->isSettable();
    }
    TemplateVariant isSettable() const
    {
      return m_memberDef->isSettable();
    }
    TemplateVariant isReadable() const
    {
      return m_memberDef->isReadable();
    }
    TemplateVariant isWritable() const
    {
      return m_memberDef->isWritable();
    }
    TemplateVariant isAddable() const
    {
      return m_memberDef->isAddable();
    }
    TemplateVariant isRemovable() const
    {
      return m_memberDef->isRemovable();
    }
    TemplateVariant isRaisable() const
    {
      return m_memberDef->isRaisable();
    }
    TemplateVariant isFinal() const
    {
      return m_memberDef->isFinal();
    }
    TemplateVariant isAbstract() const
    {
      return m_memberDef->isAbstract();
    }
    TemplateVariant isOverride() const
    {
      return m_memberDef->isOverride();
    }
    TemplateVariant isInitonly() const
    {
      return m_memberDef->isInitonly();
    }
    TemplateVariant isOptional() const
    {
      return m_memberDef->isOptional();
    }
    TemplateVariant isRequired() const
    {
      return m_memberDef->isRequired();
    }
    TemplateVariant isNonAtomic() const
    {
      return m_memberDef->isNonAtomic();
    }
    TemplateVariant isCopy() const
    {
      return m_memberDef->isCopy();
    }
    TemplateVariant isAssign() const
    {
      return m_memberDef->isAssign();
    }
    TemplateVariant isRetain() const
    {
      return m_memberDef->isRetain();
    }
    TemplateVariant isWeak() const
    {
      return m_memberDef->isWeak();
    }
    TemplateVariant isStrong() const
    {
      return m_memberDef->isStrong();
    }
    TemplateVariant isUnretained() const
    {
      return m_memberDef->isUnretained();
    }
    TemplateVariant isNew() const
    {
      return m_memberDef->isNew();
    }
    TemplateVariant isSealed() const
    {
      return m_memberDef->isSealed();
    }
    TemplateVariant isExternal() const
    {
      return m_memberDef->isExternal();
    }
    TemplateVariant isAlias() const
    {
      return m_memberDef->isAlias();
    }
    TemplateVariant isDefault() const
    {
      return m_memberDef->isDefault();
    }
    TemplateVariant isDelete() const
    {
      return m_memberDef->isDelete();
    }
    TemplateVariant isNoExcept() const
    {
      return m_memberDef->isNoExcept();
    }
    TemplateVariant isAttribute() const
    {
      return m_memberDef->isAttribute();
    }
    TemplateVariant isUNOProperty() const
    {
      return m_memberDef->isUNOProperty();
    }
    TemplateVariant isReadonly() const
    {
      return m_memberDef->isReadonly();
    }
    TemplateVariant isBound() const
    {
      return m_memberDef->isBound();
    }
    TemplateVariant isConstrained() const
    {
      return m_memberDef->isConstrained();
    }
    TemplateVariant isTransient() const
    {
      return m_memberDef->isTransient();
    }
    TemplateVariant isMaybeVoid() const
    {
      return m_memberDef->isMaybeVoid();
    }
    TemplateVariant isMaybeDefault() const
    {
      return m_memberDef->isMaybeDefault();
    }
    TemplateVariant isMaybeAmbiguous() const
    {
      return m_memberDef->isMaybeAmbiguous();
    }
    TemplateVariant isPublished() const
    {
      return m_memberDef->isPublished();
    }
    TemplateVariant isTemplateSpecialization() const
    {
      return m_memberDef->isTemplateSpecialization();
    }
    TemplateVariant isProperty() const
    {
      return m_memberDef->isProperty();
    }
    TemplateVariant isEnumValue() const
    {
      return m_memberDef->isEnumValue();
    }
    TemplateVariant isVariable() const
    {
      return m_memberDef->isVariable();
    }
    TemplateVariant isEnumeration() const
    {
      return m_memberDef->isEnumerate();
    }
    TemplateVariant hasDetails() const
    {
      return m_memberDef->isDetailedSectionLinkable();
    }
    TemplateVariant initializer() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->initializer());
    }
    TemplateVariant initializerAsCode() const
    {
      if (!m_cache.initializerParsed)
      {
        QCString scopeName;
        if (m_memberDef->getClassDef())
        {
          scopeName = m_memberDef->getClassDef()->name();
        }
        else if (m_memberDef->getNamespaceDef())
        {
          scopeName = m_memberDef->getNamespaceDef()->name();
        }
        m_cache.initializer = parseCode(m_memberDef,scopeName,relPathAsString(),
                                        m_memberDef->initializer());
        m_cache.initializerParsed = TRUE;
      }
      return m_cache.initializer;
    }
    TemplateVariant isDefine() const
    {
      return m_memberDef->isDefine();
    }
    TemplateVariant isAnonymous() const
    {
      QCString name = m_memberDef->name();
      return !name.isEmpty() && name.at(0)=='@';
    }
    TemplateVariant anonymousType() const
    {
      if (!m_cache.anonymousType)
      {
        ClassDef *cd = m_memberDef->getClassDefOfAnonymousType();
        if (cd)
        {
          m_cache.anonymousType.reset(ClassContext::alloc(cd));
        }
      }
      if (m_cache.anonymousType)
      {
        return m_cache.anonymousType.get();
      }
      else
      {
        return FALSE;
      }
    }
    TemplateVariant anonymousMember() const
    {
      if (!m_cache.anonymousMember)
      {
        MemberDef *md = m_memberDef->fromAnonymousMember();
        if (md)
        {
          m_cache.anonymousMember.reset(MemberContext::alloc(md));
        }
      }
      if (m_cache.anonymousMember)
      {
        return m_cache.anonymousMember.get();
      }
      else
      {
        return FALSE;
      }
    }
    TemplateVariant isRelated() const
    {
      return m_memberDef->isRelated();
    }
    TemplateVariant enumBaseType() const
    {
      return m_memberDef->enumBaseType();
    }
    TemplateVariant hasOneLineInitializer() const
    {
      return m_memberDef->hasOneLineInitializer();
    }
    TemplateVariant hasMultiLineInitializer() const
    {
      return m_memberDef->hasMultiLineInitializer();
    }
    TemplateVariant enumValues() const
    {
      if (!m_cache.enumValues)
      {
        MemberList *ml = m_memberDef->enumFieldList();
        if (ml)
        {
          m_cache.enumValues.reset(MemberListContext::alloc(ml));
        }
        else
        {
          m_cache.enumValues.reset(MemberListContext::alloc());
        }
      }
      return m_cache.enumValues.get();
    }
    TemplateVariant templateArgs() const
    {
      if (!m_cache.templateArgs && m_memberDef->templateArguments())
      {
        m_cache.templateArgs.reset(ArgumentListContext::alloc(m_memberDef->templateArguments(),m_memberDef,relPathAsString()));
      }
      if (m_cache.templateArgs)
      {
        return m_cache.templateArgs.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant templateAlias() const
    {
      if (m_memberDef->isAlias())
      {
        return createLinkedText(m_memberDef,relPathAsString(),
                                QCString(" = ")+m_memberDef->typeString());
      }
      return "";
    }
    TemplateVariant propertyAttrs() const
    {
      return m_cache.propertyAttrs.get();
    }
    TemplateVariant eventAttrs() const
    {
      return m_cache.eventAttrs.get();
    }
    TemplateVariant getClass() const
    {
      if (!m_cache.classDef && m_memberDef->getClassDef())
      {
        m_cache.classDef.reset(ClassContext::alloc(m_memberDef->getClassDef()));
      }
      if (m_cache.classDef)
      {
        return m_cache.classDef.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant definition() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),
                              m_memberDef->displayDefinition());
    }
    ArgumentList *getDefArgList() const
    {
      return (m_memberDef->isDocsForDefinition()) ?
              m_memberDef->argumentList() : m_memberDef->declArgumentList();
    }
    TemplateVariant parameters() const
    {
      if (!m_cache.arguments)
      {
        ArgumentList *defArgList = getDefArgList();
        if (defArgList && !m_memberDef->isProperty())
        {
          m_cache.arguments.reset(ArgumentListContext::alloc(defArgList,m_memberDef,relPathAsString()));
        }
        else
        {
          m_cache.arguments.reset(ArgumentListContext::alloc());
        }
      }
      return m_cache.arguments.get();
    }
    TemplateVariant hasParameterList() const
    {
      return getDefArgList()!=0;
    }
    TemplateVariant hasConstQualifier() const
    {
      ArgumentList *al = getDefArgList();
      return al ? al->constSpecifier : FALSE;
    }
    TemplateVariant hasVolatileQualifier() const
    {
      ArgumentList *al = getDefArgList();
      return al ? al->volatileSpecifier : FALSE;
    }
    TemplateVariant trailingReturnType() const
    {
      ArgumentList *al = getDefArgList();
      if (al && !al->trailingReturnType.isEmpty())
      {
        return createLinkedText(m_memberDef,relPathAsString(),
                                al->trailingReturnType);
      }
      else
      {
        return "";
      }
    }
    TemplateVariant extraTypeChars() const
    {
      return m_memberDef->extraTypeChars();
    }
    void addTemplateDecls(TemplateList *tl) const
    {
      ClassDef *cd=m_memberDef->getClassDef();
      if (m_memberDef->definitionTemplateParameterLists())
      {
        QListIterator<ArgumentList> ali(*m_memberDef->definitionTemplateParameterLists());
        ArgumentList *tal;
        for (ali.toFirst();(tal=ali.current());++ali)
        {
          if (tal->count()>0)
          {
            ArgumentListContext *al = ArgumentListContext::alloc(tal,m_memberDef,relPathAsString());
            tl->append(al);
          }
        }
      }
      else
      {
        if (cd && !m_memberDef->isRelated() && !m_memberDef->isTemplateSpecialization())
        {
          QList<ArgumentList> tempParamLists;
          cd->getTemplateParameterLists(tempParamLists);
          //printf("#tempParamLists=%d\n",tempParamLists.count());
          QListIterator<ArgumentList> ali(tempParamLists);
          ArgumentList *tal;
          for (ali.toFirst();(tal=ali.current());++ali)
          {
            if (tal->count()>0)
            {
              ArgumentListContext *al = ArgumentListContext::alloc(tal,m_memberDef,relPathAsString());
              tl->append(al);
            }
          }
        }
        if (m_memberDef->templateArguments()) // function template prefix
        {
          ArgumentListContext *al = ArgumentListContext::alloc(
              m_memberDef->templateArguments(),m_memberDef,relPathAsString());
          tl->append(al);
        }
      }
    }
    TemplateVariant templateDecls() const
    {
      if (!m_cache.templateDecls)
      {
        TemplateList *tl = TemplateList::alloc();
        addTemplateDecls(tl);
        m_cache.templateDecls.reset(tl);
      }
      return m_cache.templateDecls.get();
    }
    TemplateVariant labels() const
    {
      if (!m_cache.labels)
      {
        QStrList sl;
        m_memberDef->getLabels(sl,m_memberDef->getOuterScope());
        TemplateList *tl = TemplateList::alloc();
        if (sl.count()>0)
        {
          QStrListIterator it(sl);
          for (;it.current();++it)
          {
            tl->append(*it);
          }
        }
        m_cache.labels.reset(tl);
      }
      return m_cache.labels.get();
    }
    TemplateVariant paramDocs() const
    {
      if (!m_cache.paramDocs)
      {
        if (m_memberDef->argumentList() && m_memberDef->argumentList()->hasDocumentation())
        {
          QCString paramDocs;
          ArgumentListIterator ali(*m_memberDef->argumentList());
          Argument *a;
          // convert the parameter documentation into a list of @param commands
          for (ali.toFirst();(a=ali.current());++ali)
          {
            if (a->hasDocumentation())
            {
              QCString direction = extractDirection(a->docs);
              paramDocs+="@param"+direction+" "+a->name+" "+a->docs;
            }
          }
          m_cache.paramDocs.reset(new TemplateVariant(parseDoc(m_memberDef,
                                           m_memberDef->docFile(),m_memberDef->docLine(),
                                           relPathAsString(),paramDocs,FALSE)));
        }
        else
        {
          m_cache.paramDocs.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.paramDocs;
    }
    TemplateVariant implements() const
    {
      if (!m_cache.implements)
      {
        MemberDef *md = m_memberDef->reimplements();
        m_cache.implements.reset(TemplateList::alloc());
        if (md)
        {
          ClassDef *cd = md->getClassDef();
          if (cd && (md->virtualness()==Pure || cd->compoundType()==ClassDef::Interface))
          {
            MemberContext *mc = MemberContext::alloc(md);
            m_cache.implements->append(mc);
          }
        }
      }
      return m_cache.implements.get();
    }
    TemplateVariant reimplements() const
    {
      if (!m_cache.reimplements)
      {
        MemberDef *md = m_memberDef->reimplements();
        m_cache.reimplements.reset(TemplateList::alloc());
        if (md)
        {
          ClassDef *cd = md->getClassDef();
          if (cd && md->virtualness()!=Pure && cd->compoundType()!=ClassDef::Interface)
          {
            MemberContext *mc = MemberContext::alloc(md);
            m_cache.reimplements->append(mc);
          }
        }
      }
      return m_cache.reimplements.get();
    }
    TemplateVariant implementedBy() const
    {
      if (!m_cache.implementedBy)
      {
        MemberList *ml = m_memberDef->reimplementedBy();
        m_cache.implementedBy.reset(TemplateList::alloc());
        if (ml)
        {
          MemberListIterator mli(*ml);
          MemberDef *md=0;
          for (mli.toFirst();(md=mli.current());++mli)
          {
            ClassDef *cd = md->getClassDef();
            if (cd && (md->virtualness()==Pure || cd->compoundType()==ClassDef::Interface))
            {
              MemberContext *mc = new MemberContext(md);
              m_cache.implementedBy->append(mc);
            }
          }
        }
      }
      return m_cache.implementedBy.get();
    }
    TemplateVariant reimplementedBy() const
    {
      if (!m_cache.reimplementedBy)
      {
        m_cache.reimplementedBy.reset(TemplateList::alloc());
        MemberList *ml = m_memberDef->reimplementedBy();
        if (ml)
        {
          MemberListIterator mli(*ml);
          MemberDef *md=0;
          for (mli.toFirst();(md=mli.current());++mli)
          {
            ClassDef *cd = md->getClassDef();
            if (cd && md->virtualness()!=Pure && cd->compoundType()!=ClassDef::Interface)
            {
              MemberContext *mc = new MemberContext(md);
              m_cache.reimplementedBy->append(mc);
            }
          }
        }
      }
      return m_cache.reimplementedBy.get();
    }
    void addExamples(TemplateList *list) const
    {
      if (m_memberDef->hasExamples())
      {
        ExampleSDict::Iterator it(*m_memberDef->getExamples());
        Example *ex;
        for (it.toFirst();(ex=it.current());++it)
        {
          TemplateStruct *s = TemplateStruct::alloc();
          s->set("text",ex->name);
          s->set("isLinkable",TRUE);
          s->set("anchor",ex->anchor);
          s->set("fileName",ex->file);
          list->append(s);
        }
      }
    }
    TemplateVariant examples() const
    {
      if (!m_cache.examples)
      {
        TemplateList *exampleList = TemplateList::alloc();
        addExamples(exampleList);
        m_cache.examples.reset(exampleList);
      }
      return m_cache.examples.get();
    }
    TemplateVariant typeConstraints() const
    {
      if (!m_cache.typeConstraints && m_memberDef->typeConstraints())
      {
        m_cache.typeConstraints.reset(ArgumentListContext::alloc(m_memberDef->typeConstraints(),m_memberDef,relPathAsString()));
      }
      else
      {
        m_cache.typeConstraints.reset(ArgumentListContext::alloc());
      }
      return m_cache.typeConstraints.get();
    }
    TemplateVariant functionQualifier() const
    {
      if (!m_memberDef->isObjCMethod() &&
          (m_memberDef->isFunction()  || m_memberDef->isSlot() ||
           m_memberDef->isPrototype() || m_memberDef->isSignal()
          )
         )
      {
        return "()";
      }
      else
      {
        return "";
      }
    }
    TemplateVariant sourceRefs() const
    {
      if (!m_cache.sourceRefs)
      {
        m_cache.sourceRefs.reset(MemberListContext::alloc(m_memberDef->getReferencesMembers(),TRUE));
      }
      return m_cache.sourceRefs.get();
    }
    TemplateVariant sourceRefBys() const
    {
      if (!m_cache.sourceRefBys)
      {
        m_cache.sourceRefBys.reset(MemberListContext::alloc(m_memberDef->getReferencedByMembers(),TRUE));
      }
      return m_cache.sourceRefBys.get();
    }
    TemplateVariant hasSources() const
    {
      return TemplateVariant(m_memberDef->hasSources());
    }
    TemplateVariant sourceCode() const
    {
      if (!m_cache.sourceCodeParsed)
      {
        QCString codeFragment;
        FileDef *fd   = m_memberDef->getBodyDef();
        int startLine = m_memberDef->getStartBodyLine();
        int endLine   = m_memberDef->getEndBodyLine();
        if (fd && readCodeFragment(fd->absFilePath(),
              startLine,endLine,codeFragment)
           )
        {
          QCString scopeName;
          if (m_memberDef->getClassDef())
          {
            scopeName = m_memberDef->getClassDef()->name();
          }
          else if (m_memberDef->getNamespaceDef())
          {
            scopeName = m_memberDef->getNamespaceDef()->name();
          }
          m_cache.sourceCode = parseCode(m_memberDef,scopeName,relPathAsString(),codeFragment,startLine,endLine,TRUE);
          m_cache.sourceCodeParsed = TRUE;
        }
      }
      return m_cache.sourceCode;
    }
    DotCallGraph *getCallGraph() const
    {
      if (!m_cache.callGraph)
      {
        m_cache.callGraph.reset(new DotCallGraph(m_memberDef,FALSE));
      }
      return m_cache.callGraph.get();
    }
    TemplateVariant hasCallGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      static bool callGraph = Config_getBool("CALL_GRAPH");
      if ((callGraph || m_memberDef->hasCallGraph()) && haveDot &&
          (m_memberDef->isFunction() || m_memberDef->isSlot() || m_memberDef->isSignal()))
      {
        DotCallGraph *cg = getCallGraph();
        return !cg->isTooBig() && !cg->isTrivial();
      }
      return TemplateVariant(FALSE);
    }
    TemplateVariant callGraph() const
    {
      if (hasCallGraph().toBool())
      {
        DotCallGraph *cg = getCallGraph();
        QGString result;
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
            g_globals.outputDir,
            g_globals.outputDir+portable_pathSeparator()+m_memberDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,g_globals.dynSectionId
            );
        g_globals.dynSectionId++;
        return TemplateVariant(result.data(),TRUE);
      }
      else
      {
        return TemplateVariant("");
      }
    }
    DotCallGraph *getCallerGraph() const
    {
      if (!m_cache.callerGraph)
      {
        m_cache.callerGraph.reset(new DotCallGraph(m_memberDef,TRUE));
      }
      return m_cache.callerGraph.get();
    }
    TemplateVariant hasCallerGraph() const
    {
      static bool haveDot = Config_getBool("HAVE_DOT");
      static bool callerGraph = Config_getBool("CALLER_GRAPH");
      if ((callerGraph || m_memberDef->hasCallerGraph()) && haveDot &&
          (m_memberDef->isFunction() || m_memberDef->isSlot() || m_memberDef->isSignal()))
      {
        DotCallGraph *cg = getCallerGraph();
        return !cg->isTooBig() && !cg->isTrivial();
      }
      return TemplateVariant(FALSE);
    }
    TemplateVariant callerGraph() const
    {
      if (hasCallerGraph().toBool())
      {
        DotCallGraph *cg = getCallerGraph();
        QGString result;
        FTextStream t(&result);
        cg->writeGraph(t,BITMAP,
            g_globals.outputDir,
            g_globals.outputDir+portable_pathSeparator()+m_memberDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,g_globals.dynSectionId
            );
        g_globals.dynSectionId++;
        return TemplateVariant(result.data(),TRUE);
      }
      else
      {
        return TemplateVariant("");
      }
    }
  private:
    MemberDef *m_memberDef;
    struct Cachable
    {
      Cachable() : initializerParsed(FALSE), sourceCodeParsed(FALSE)
      {
      }
      SharedPtr<ArgumentListContext> templateArgs;
      SharedPtr<ArgumentListContext> arguments;
      SharedPtr<MemberListContext>   enumValues;
      SharedPtr<ClassContext>        classDef;
      SharedPtr<ClassContext>        anonymousType;
      SharedPtr<TemplateList>        templateDecls;
      ScopedPtr<TemplateVariant>     paramDocs;
      SharedPtr<TemplateList>        implements;
      SharedPtr<TemplateList>        reimplements;
      SharedPtr<TemplateList>        implementedBy;
      SharedPtr<MemberListContext>   sourceRefs;
      SharedPtr<MemberListContext>   sourceRefBys;
      ScopedPtr<DotCallGraph>        callGraph;
      ScopedPtr<DotCallGraph>        callerGraph;
      SharedPtr<MemberContext>       anonymousMember;
      SharedPtr<TemplateList>        reimplementedBy;
      SharedPtr<TemplateList>        labels;
      TemplateVariant                initializer;
      bool                           initializerParsed;
      TemplateVariant                sourceCode;
      bool                           sourceCodeParsed;
      SharedPtr<TemplateList>        examples;
      SharedPtr<TemplateList>        exampleList;
      SharedPtr<ArgumentListContext> typeConstraints;
      SharedPtr<TemplateList>        propertyAttrs;
      SharedPtr<TemplateList>        eventAttrs;
    };
    mutable Cachable m_cache;
};
//%% }

MemberContext::MemberContext(MemberDef *md) : RefCountedContext("MemberContext")
{
  p = new Private(md);
}

MemberContext::~MemberContext()
{
  delete p;
}

TemplateVariant MemberContext::get(const char *n) const
{
  return p->get(n);
}


//------------------------------------------------------------------------

//%% struct Module(Symbol): group information
//%% {
class ModuleContext::Private : public DefinitionContext<ModuleContext::Private>
{
  public:
    Private(GroupDef *gd) : DefinitionContext<ModuleContext::Private>(gd) , m_groupDef(gd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
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

ModuleContext::ModuleContext(GroupDef *gd) : RefCountedContext("ModuleContext")
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

//%% list NestedClassList[Class] : list of nested classes
class NestedClassListContext::Private : public GenericNodeListContext
{
};

NestedClassListContext::NestedClassListContext() : RefCountedContext("NestedClassListContext")
{
  p = new Private;
}

NestedClassListContext::~NestedClassListContext()
{
  delete p;
}

// TemplateListIntf
int NestedClassListContext::count() const
{
  return p->count();
}

TemplateVariant NestedClassListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *NestedClassListContext::createIterator() const
{
  return p->createIterator();
}

void NestedClassListContext::append(ClassDef *cd)
{
  if (cd)
  {
    p->append(ClassContext::alloc(cd));
  }
}

//------------------------------------------------------------------------

//%% list NestedClassList[Class] : list of nested namespaces
class NestedNamespaceListContext::Private : public GenericNodeListContext
{
};

NestedNamespaceListContext::NestedNamespaceListContext() : RefCountedContext("NestedNamespaceListContext")
{
  p = new Private;
}

NestedNamespaceListContext::~NestedNamespaceListContext()
{
  delete p;
}

// TemplateListIntf
int NestedNamespaceListContext::count() const
{
  return p->count();
}

TemplateVariant NestedNamespaceListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *NestedNamespaceListContext::createIterator() const
{
  return p->createIterator();
}

void NestedNamespaceListContext::append(NamespaceDef *cd)
{
  if (cd)
  {
    p->append(NamespaceContext::alloc(cd));
  }
}

//------------------------------------------------------------------------

//%% list ClassList[Class] : list of classes
class ClassListContext::Private : public GenericNodeListContext
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
          append(ClassContext::alloc(cd));
        }
      }
    }
};

ClassListContext::ClassListContext() : RefCountedContext("ClassListContext")
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
class ClassInheritanceNodeContext::Private : public PropertyMapper
{
  public:
    Private(ClassDef *cd) : m_classDef(cd)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% ClassInheritance children: list of nested classes/namespaces
      addProperty("children",this,&Private::children);
      //%% Class class: class info
      addProperty("class",this,&Private::getClass);
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
      if (!m_cache.classContext)
      {
        m_cache.classContext.reset(ClassContext::alloc(m_classDef));
      }
      return m_cache.classContext.get();
    }
  private:
    ClassDef *m_classDef;
    GenericNodeListContext m_children;
    struct Cachable
    {
      SharedPtr<ClassContext> classContext;
    };
    mutable Cachable m_cache;
};
//%% }

ClassInheritanceNodeContext::ClassInheritanceNodeContext(ClassDef *cd) : RefCountedContext("ClassInheritanceNodeContext")
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
class ClassInheritanceContext::Private : public GenericNodeListContext
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
          if ((VhdlDocGen::VhdlClasses)cd->protection()!=VhdlDocGen::ENTITYCLASS)
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
            ClassInheritanceNodeContext *tnc = ClassInheritanceNodeContext::alloc(cd);
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

ClassInheritanceContext::ClassInheritanceContext() : RefCountedContext("ClassInheritanceContext")
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
class ClassHierarchyContext::Private : public PropertyMapper
{
  public:
    TemplateVariant tree() const
    {
      if (!m_cache.classTree)
      {
        m_cache.classTree.reset(ClassInheritanceContext::alloc());
      }
      return m_cache.classTree.get();
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
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    struct Cachable
    {
      SharedPtr<ClassInheritanceContext> classTree;
    };
    mutable Cachable m_cache;
};
//%% }

ClassHierarchyContext::ClassHierarchyContext() : RefCountedContext("ClassHierarchyContext")
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
class NestingNodeContext::Private : public PropertyMapper
{
  public:
    Private(const NestingNodeContext *parent,const NestingNodeContext *thisNode,
        Definition *d,int index,int level,bool addCls)
      : m_parent(parent), m_def(d), m_level(level), m_index(index)
    {
      m_children.reset(NestingContext::alloc(thisNode,level+1));
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% Nesting children: list of nested classes/namespaces
      addProperty("children",this,&Private::children);
      //%% [optional] Class class: class info (if this node represents a class)
      addProperty("class",this,&Private::getClass);
      //%% [optional] Namespace namespace: namespace info (if this node represents a namespace)
      addProperty("namespace",this,&Private::getNamespace);
      //%% [optional] File file: file info (if this node represents a file)
      addProperty("file",this,&Private::getFile);
      //%% [optional] Dir dir: directory info (if this node represents a directory)
      addProperty("dir",this,&Private::getDir);
      //%% int id
      addProperty("id",this,&Private::id);
      //%% string level
      addProperty("level",this,&Private::level);
      //%% string name
      addProperty("name",this,&Private::name);
      //%% string brief
      addProperty("brief",this,&Private::brief);
      //%% bool isLinkable
      addProperty("isLinkable",this,&Private::isLinkable);
      addProperty("anchor",this,&Private::anchor);
      addProperty("fileName",this,&Private::fileName);

      addNamespaces(addCls);
      addClasses();
      addDirFiles();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children->count()==0;
    }
    TemplateVariant children() const
    {
      return m_children.get();
    }
    TemplateVariant getClass() const
    {
      if (!m_cache.classContext && m_def->definitionType()==Definition::TypeClass)
      {
        m_cache.classContext.reset(ClassContext::alloc((ClassDef*)m_def));
      }
      if (m_cache.classContext)
      {
        return m_cache.classContext.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getNamespace() const
    {
      if (!m_cache.namespaceContext && m_def->definitionType()==Definition::TypeNamespace)
      {
        m_cache.namespaceContext.reset(NamespaceContext::alloc((NamespaceDef*)m_def));
      }
      if (m_cache.namespaceContext)
      {
        return m_cache.namespaceContext.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getDir() const
    {
      if (!m_cache.dirContext && m_def->definitionType()==Definition::TypeDir)
      {
        m_cache.dirContext.reset(DirContext::alloc((DirDef*)m_def));
      }
      if (m_cache.dirContext)
      {
        return m_cache.dirContext.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getFile() const
    {
      if (!m_cache.fileContext && m_def->definitionType()==Definition::TypeFile)
      {
        m_cache.fileContext.reset(FileContext::alloc((FileDef*)m_def));
      }
      if (m_cache.fileContext)
      {
        return m_cache.fileContext.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant level() const
    {
      return m_level;
    }
    TemplateVariant id() const
    {
      QCString result;
      if (m_parent) result=m_parent->id();
      result+=QCString().setNum(m_index)+"_";
      return result;
    }
    TemplateVariant name() const
    {
      return m_def->displayName(FALSE);
    }
    QCString relPathAsString() const
    {
      static bool createSubdirs = Config_getBool("CREATE_SUBDIRS");
      return createSubdirs ? QCString("../../") : QCString("");
    }
    TemplateVariant brief() const
    {
      if (!m_cache.brief)
      {
        if (m_def->hasBriefDescription())
        {
          m_cache.brief.reset(new TemplateVariant(parseDoc(m_def,m_def->briefFile(),m_def->briefLine(),
                              "",m_def->briefDescription(),TRUE)));
        }
        else
        {
          m_cache.brief.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.brief;
    }
    TemplateVariant isLinkable() const
    {
      return m_def->isLinkable();
    }
    TemplateVariant anchor() const
    {
      return m_def->anchor();
    }
    TemplateVariant fileName() const
    {
      return m_def->getOutputFileBase();
    }

    void addClasses()
    {
      ClassDef *cd = m_def->definitionType()==Definition::TypeClass ? (ClassDef*)m_def : 0;
      if (cd && cd->getClassSDict())
      {
        m_children->addClasses(*cd->getClassSDict(),FALSE);
      }
    }
    void addNamespaces(bool addClasses)
    {
      NamespaceDef *nd = m_def->definitionType()==Definition::TypeNamespace ? (NamespaceDef*)m_def : 0;
      if (nd && nd->getNamespaceSDict())
      {
        m_children->addNamespaces(*nd->getNamespaceSDict(),FALSE,addClasses);
      }
      if (addClasses && nd && nd->getClassSDict())
      {
        m_children->addClasses(*nd->getClassSDict(),FALSE);
      }
    }
    void addDirFiles()
    {
      DirDef *dd = m_def->definitionType()==Definition::TypeDir ? (DirDef*)m_def : 0;
      if (dd)
      {
        m_children->addDirs(dd->subDirs());
        if (dd && dd->getFiles())
        {
          m_children->addFiles(*dd->getFiles());
        }
      }
    }
  private:
    const NestingNodeContext *m_parent;
    Definition *m_def;
    SharedPtr<NestingContext> m_children;
    int m_level;
    int m_index;
    struct Cachable
    {
      SharedPtr<ClassContext>     classContext;
      SharedPtr<NamespaceContext> namespaceContext;
      SharedPtr<DirContext>       dirContext;
      SharedPtr<FileContext>      fileContext;
      ScopedPtr<TemplateVariant>  brief;
    };
    mutable Cachable m_cache;
};
//%% }

NestingNodeContext::NestingNodeContext(const NestingNodeContext *parent,
                                       Definition *d,int index,int level,bool addClass) : RefCountedContext("NestingNodeContext")
{
  p = new Private(parent,this,d,index,level,addClass);
}

NestingNodeContext::~NestingNodeContext()
{
  delete p;
}

TemplateVariant NestingNodeContext::get(const char *n) const
{
  return p->get(n);
}

QCString NestingNodeContext::id() const
{
  return p->id().toString();
}

//------------------------------------------------------------------------

//%% list Nesting[NestingNode]: namespace and class nesting relations
class NestingContext::Private : public GenericNodeListContext
{
  public:
    Private(const NestingNodeContext *parent,int level)
      : m_parent(parent), m_level(level), m_index(0) {}

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
            NestingNodeContext *nnc = NestingNodeContext::alloc(m_parent,nd,m_index,m_level,addClasses);
            append(nnc);
            m_index++;
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
            NestingNodeContext *nnc = NestingNodeContext::alloc(m_parent,cd,m_index,m_level,TRUE);
            append(nnc);
            m_index++;
          }
        }
      }
    }
    void addDirs(const DirSDict &dirDict)
    {
      SDict<DirDef>::Iterator dli(dirDict);
      DirDef *dd;
      for (dli.toFirst();(dd=dli.current());++dli)
      {
        if (dd->getOuterScope()==Doxygen::globalScope)
        {
          append(NestingNodeContext::alloc(m_parent,dd,m_index,m_level,FALSE));
          m_index++;
        }
      }
    }
    void addDirs(const DirList &dirList)
    {
      QListIterator<DirDef> li(dirList);
      DirDef *dd;
      for (li.toFirst();(dd=li.current());++li)
      {
        append(NestingNodeContext::alloc(m_parent,dd,m_index,m_level,FALSE));
        m_index++;
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
            append(NestingNodeContext::alloc(m_parent,fd,m_index,m_level,FALSE));
            m_index++;
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
        append(NestingNodeContext::alloc(m_parent,fd,m_index,m_level,FALSE));
        m_index++;
      }
    }
  private:
    const NestingNodeContext *m_parent;
    int m_level;
    int m_index;
};

NestingContext::NestingContext(const NestingNodeContext *parent,int level) : RefCountedContext("NestingContext")
{
  p = new Private(parent,level);
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

void NestingContext::addDirs(const DirSDict &dirs)
{
  p->addDirs(dirs);
}

void NestingContext::addDirs(const DirList &dirs)
{
  p->addDirs(dirs);
}

void NestingContext::addFiles(const FileNameList &files)
{
  p->addFiles(files);
}

void NestingContext::addFiles(const FileList &files)
{
  p->addFiles(files);
}


//------------------------------------------------------------------------

static int computeMaxDepth(const TemplateListIntf *list)
{
  int maxDepth=0;
  if (list)
  {
    TemplateListIntf::ConstIterator *it = list->createIterator();
    TemplateVariant v;
    for (it->toFirst();it->current(v);it->toNext())
    {
      const TemplateStructIntf *s = v.toStruct();
      TemplateVariant child = s->get("children");
      int d = computeMaxDepth(child.toList())+1;
      if (d>maxDepth) maxDepth=d;
    }
    delete it;
  }
  return maxDepth;
}

static int computeNumNodesAtLevel(const TemplateStructIntf *s,int level,int maxLevel)
{
  int num=0;
  if (level<maxLevel)
  {
    num++;
    TemplateVariant child = s->get("children");
    if (child.toList())
    {
      TemplateListIntf::ConstIterator *it = child.toList()->createIterator();
      TemplateVariant v;
      for (it->toFirst();it->current(v);it->toNext())
      {
        num+=computeNumNodesAtLevel(v.toStruct(),level+1,maxLevel);
      }
      delete it;
    }
  }
  return num;
}

static int computePreferredDepth(const TemplateListIntf *list,int maxDepth)
{
  int preferredNumEntries = Config_getInt("HTML_INDEX_NUM_ENTRIES");
  int preferredDepth=1;
  if (preferredNumEntries>0)
  {
    int depth = maxDepth;
    for (int i=1;i<=depth;i++)
    {
      int num=0;
      TemplateListIntf::ConstIterator *it = list->createIterator();
      TemplateVariant v;
      for (it->toFirst();it->current(v);it->toNext())
      {
        num+=computeNumNodesAtLevel(v.toStruct(),0,i);
      }
      delete it;
      if (num<=preferredNumEntries)
      {
        preferredDepth=i;
      }
      else
      {
        break;
      }
    }
  }
  return preferredDepth;
}


//%% struct ClassTree: Class nesting relations
//%% {
class ClassTreeContext::Private : public PropertyMapper
{
  public:
    Private()
    {
      m_classTree.reset(NestingContext::alloc(0,0));
      if (Doxygen::namespaceSDict)
      {
        m_classTree->addNamespaces(*Doxygen::namespaceSDict,TRUE,TRUE);
      }
      if (Doxygen::classSDict)
      {
        m_classTree->addClasses(*Doxygen::classSDict,TRUE);
      }
      //%% Nesting tree
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
      addProperty("preferredDepth",this,&Private::preferredDepth);
      addProperty("maxDepth",this,&Private::maxDepth);
    }
    TemplateVariant tree() const
    {
      return m_classTree.get();
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
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_classTree.get());
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_classTree.get(),maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    SharedPtr<NestingContext> m_classTree;
    struct Cachable
    {
      Cachable() : maxDepthComputed(FALSE), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
};
//%% }

ClassTreeContext::ClassTreeContext() : RefCountedContext("ClassTreeContext")
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
class NamespaceListContext::Private : public GenericNodeListContext
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
          append(NamespaceContext::alloc(nd));
        }
      }
    }
};

NamespaceListContext::NamespaceListContext() : RefCountedContext("NamespaceListContext")
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
class NamespaceTreeContext::Private : public PropertyMapper
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
      m_namespaceTree.reset(NestingContext::alloc(0,0));
      if (Doxygen::namespaceSDict)
      {
        m_namespaceTree->addNamespaces(*Doxygen::namespaceSDict,TRUE,FALSE);
      }
      //%% Nesting tree
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    SharedPtr<NestingContext> m_namespaceTree;
};
//%% }

NamespaceTreeContext::NamespaceTreeContext() : RefCountedContext("NamespaceTreeContext")
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
class FileListContext::Private : public GenericNodeListContext
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
            append(FileContext::alloc(fd));
          }
        }
      }
    }
};

FileListContext::FileListContext() : RefCountedContext("FileListContext")
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

//%% list DirList[Dir] : list of files
class DirListContext::Private : public GenericNodeListContext
{
  public:
    Private()
    {
      DirDef *dir;
      DirSDict::Iterator sdi(*Doxygen::directories);
      for (sdi.toFirst();(dir=sdi.current());++sdi)
      {
        append(DirContext::alloc(dir));
      }
    }
};

DirListContext::DirListContext() : RefCountedContext("DirListContext")
{
  p = new Private;
}

DirListContext::~DirListContext()
{
  delete p;
}

// TemplateListIntf
int DirListContext::count() const
{
  return p->count();
}

TemplateVariant DirListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *DirListContext::createIterator() const
{
  return p->createIterator();
}


//------------------------------------------------------------------------

//%% list UsedFiles[File] : list of files
class UsedFilesContext::Private : public GenericNodeListContext
{
  public:
    void addFile(FileDef *fd)
    {
      append(FileContext::alloc(fd));
    }
};

UsedFilesContext::UsedFilesContext(ClassDef *cd) : RefCountedContext("UsedFilesContext")
{
  p = new Private;
  if (cd)
  {
    QListIterator<FileDef> li(cd->usedFiles());
    FileDef *fd;
    for (li.toFirst();(fd=li.current());++li)
    {
      p->addFile(fd);
    }
  }
}

UsedFilesContext::~UsedFilesContext()
{
  delete p;
}

// TemplateListIntf
int UsedFilesContext::count() const
{
  return p->count();
}

TemplateVariant UsedFilesContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *UsedFilesContext::createIterator() const
{
  return p->createIterator();
}

void UsedFilesContext::addFile(FileDef *fd)
{
  p->addFile(fd);
}

//------------------------------------------------------------------------

//%% struct FileTree: tree of directories and files
//%% {
class FileTreeContext::Private : public PropertyMapper
{
  public:
    Private()
    {
      // Add dirs tree
      m_dirFileTree.reset(NestingContext::alloc(0,0));
      if (Doxygen::directories)
      {
        m_dirFileTree->addDirs(*Doxygen::directories);
      }
      if (Doxygen::inputNameList)
      {
        m_dirFileTree->addFiles(*Doxygen::inputNameList);
      }
      //%% DirFile tree:
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
      addProperty("preferredDepth",this,&Private::preferredDepth);
      addProperty("maxDepth",this,&Private::maxDepth);
    }
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
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_dirFileTree.get());
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_dirFileTree.get(),maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    SharedPtr<NestingContext> m_dirFileTree;
    struct Cachable
    {
      Cachable() : maxDepthComputed(FALSE), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
};
//%% }

FileTreeContext::FileTreeContext() : RefCountedContext("FileTreeContext")
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
class PageNodeContext::Private : public PropertyMapper
{
  public:
    Private(PageDef *pd) : m_pageDef(pd)
    {
      m_children.reset(PageNodeListContext::alloc());
      m_pageContext.reset(PageContext::alloc(pd));
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% PageList children: list of nested classes/namespaces
      addProperty("children",this,&Private::children);
      //%% Page page: page info
      addProperty("page",this,&Private::getPage);
      addPages();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children->count()==0;
    }
    TemplateVariant children() const
    {
      return m_children.get();
    }
    TemplateVariant getPage() const
    {
      return m_pageContext.get();
    }
    void addPages()
    {
      if (m_pageDef->getSubPages())
      {
        m_children->addPages(*m_pageDef->getSubPages(),FALSE);
      }
    }
  private:
    PageDef                       *m_pageDef;
    SharedPtr<PageNodeListContext> m_children;
    SharedPtr<PageContext>         m_pageContext;
};
//%% }

PageNodeContext::PageNodeContext(PageDef *pd) : RefCountedContext("PageNodeContext")
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
class PageNodeListContext::Private : public GenericNodeListContext
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
          append(PageNodeContext::alloc(pd));
        }
      }
    }
};

PageNodeListContext::PageNodeListContext() : RefCountedContext("PageNodeListContext")
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
class PageTreeContext::Private : public PropertyMapper
{
  public:
    TemplateVariant tree() const
    {
      return m_pageList.get();
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
      m_pageList.reset(PageNodeListContext::alloc());
      // Add pages
      if (Doxygen::pageSDict)
      {
        m_pageList->addPages(*Doxygen::pageSDict,TRUE);
      }

      //%% PageNodeList tree:
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    SharedPtr<PageNodeListContext> m_pageList;
};
//%% }

PageTreeContext::PageTreeContext() : RefCountedContext("PageTreeContext")
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
class PageListContext::Private : public PropertyMapper
{
  public:
    TemplateVariant items() const
    {
      return m_pageList.get();
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
      m_pageList.reset(new GenericNodeListContext);
      // Add pages
      PageSDict::Iterator pdi(*Doxygen::pageSDict);
      PageDef *pd=0;
      for (pdi.toFirst();(pd=pdi.current());++pdi)
      {
        if (!pd->getGroupDef() && !pd->isReference())
        {
          m_pageList->append(PageContext::alloc(pd));
        }
      }

      //%% list[Page] items:
      addProperty("items",this,&Private::items);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    SharedPtr<GenericNodeListContext> m_pageList;
};
//%% }

PageListContext::PageListContext() : RefCountedContext("PageListContext")
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
class ModuleNodeContext::Private : public PropertyMapper
{
  public:
    Private(GroupDef *gd) : m_groupDef(gd)
    {
      m_children.reset(ModuleListContext::alloc());
      m_moduleContext.reset(ModuleContext::alloc(gd));
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% ModuleList children: list of submodules
      addProperty("children",this,&Private::children);
      //%% Module module: module info
      addProperty("module",this,&Private::getModule);
      addModules();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children->count()==0;
    }
    TemplateVariant children() const
    {
      return m_children.get();
    }
    TemplateVariant getModule() const
    {
      return m_moduleContext.get();
    }
    void addModules()
    {
      if (m_groupDef->getSubGroups())
      {
        m_children->addModules(*m_groupDef->getSubGroups());
      }
    }
  private:
    GroupDef                    *m_groupDef;
    SharedPtr<ModuleListContext> m_children;
    SharedPtr<ModuleContext>     m_moduleContext;
};
//%% }

ModuleNodeContext::ModuleNodeContext(GroupDef *gd) : RefCountedContext("ModuleNodeContext")
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
class ModuleListContext::Private : public GenericNodeListContext
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
          append(ModuleNodeContext::alloc(gd));
        }
      }
    }
    void addModules(const GroupList &list)
    {
      QListIterator<GroupDef> gli(list);
      GroupDef *gd;
      for (gli.toFirst();(gd=gli.current());++gli)
      {
        append(ModuleNodeContext::alloc(gd));
      }
    }
};

ModuleListContext::ModuleListContext() : RefCountedContext("ModuleListContext")
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
class ModuleTreeContext::Private : public PropertyMapper
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
      m_moduleList.reset(ModuleListContext::alloc());
      // Add modules
      if (Doxygen::groupSDict)
      {
        m_moduleList->addModules(*Doxygen::groupSDict);
      }

      //%% ModuleList tree:
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    SharedPtr<ModuleListContext> m_moduleList;
};
//%% }

ModuleTreeContext::ModuleTreeContext() : RefCountedContext("ModuleTreeContext")
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

//%% struct NavPathElem: list of examples page
//%% {
class NavPathElemContext::Private : public PropertyMapper
{
  public:
    Private(Definition *def) : m_def(def)
    {
      addProperty("isLinkable",this,&Private::isLinkable);
      addProperty("fileName",this,&Private::fileName);
      addProperty("anchor",this,&Private::anchor);
      addProperty("text",this,&Private::text);
    }
    TemplateVariant isLinkable() const
    {
      return m_def->isLinkable();
    }
    TemplateVariant anchor() const
    {
      return m_def->anchor();
    }
    TemplateVariant fileName() const
    {
      return m_def->getOutputFileBase();
    }
    TemplateVariant text() const
    {
      Definition::DefType type = m_def->definitionType();
      QCString text = m_def->localName();
      if (type==Definition::TypeGroup)
      {
        text = ((const GroupDef*)m_def)->groupTitle();
      }
      else if (type==Definition::TypePage && !(((const PageDef*)this)->title().isEmpty()))
      {
        text = ((const PageDef*)m_def)->title();
      }
      else if (type==Definition::TypeClass)
      {
        if (text.right(2)=="-p")
        {
          text = text.left(text.length()-2);
        }
      }
      return text;
    }
  private:
    Definition *m_def;
};
//%% }

NavPathElemContext::NavPathElemContext(Definition *def) : RefCountedContext("NavPathElemContext")
{
  p = new Private(def);
}

NavPathElemContext::~NavPathElemContext()
{
  delete p;
}

TemplateVariant NavPathElemContext::get(const char *name) const
{
  return p->get(name);
}


//------------------------------------------------------------------------

//%% struct ExampleList: list of examples page
//%% {
class ExampleListContext::Private : public PropertyMapper
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
      m_pageList.reset(PageNodeListContext::alloc());
      // Add pages
      if (Doxygen::exampleSDict)
      {
        m_pageList->addPages(*Doxygen::exampleSDict,FALSE);
      }

      //%% PageNodeList items:
      addProperty("items",this,&Private::items);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
    }
  private:
    SharedPtr<PageNodeListContext> m_pageList;
};
//%% }

ExampleListContext::ExampleListContext() : RefCountedContext("ExampleListContext")
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

//%% struct InheritanceNode: a class in the inheritance list
//%% {
class InheritanceNodeContext::Private : public PropertyMapper
{
  public:
    Private(ClassDef *cd,const QCString &name) : m_classDef(cd), m_name(name)
    {
      addProperty("class",this,&Private::getClass);
      addProperty("name",this,&Private::name);
    }
    TemplateVariant getClass() const
    {
      if (!m_classContext)
      {
        m_classContext.reset(ClassContext::alloc(m_classDef));
      }
      return m_classContext.get();
    }
    TemplateVariant name() const
    {
      return m_name;
    }
  private:
    ClassDef *m_classDef;
    mutable SharedPtr<ClassContext> m_classContext;
    QCString m_name;
};
//%% }

InheritanceNodeContext::InheritanceNodeContext(ClassDef *cd,const QCString &name) : RefCountedContext("InheritanceNodeContext")
{
  p = new Private(cd,name);
}

InheritanceNodeContext::~InheritanceNodeContext()
{
  delete p;
}

TemplateVariant InheritanceNodeContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list InheritanceList[InheritanceNode] : list of inherited classes
class InheritanceListContext::Private : public GenericNodeListContext
{
  public:
    void addClass(ClassDef *cd,const QCString &name)
    {
      append(InheritanceNodeContext::alloc(cd,name));
    }
};

InheritanceListContext::InheritanceListContext(const BaseClassList *list, bool baseClasses) : RefCountedContext("InheritanceListContext")
{
  p = new Private;
  if (list)
  {
    BaseClassListIterator li(*list);
    BaseClassDef *bcd;
    for (li.toFirst();(bcd=li.current());++li)
    {
      ClassDef *cd=bcd->classDef;
      QCString name;
      if (baseClasses)
      {
        name = insertTemplateSpecifierInScope(
                     cd->displayName(),bcd->templSpecifiers);
      }
      else
      {
        name = cd->displayName();
      }
      //printf("InheritanceListContext: adding %s baseClass=%d\n",name.data(),baseClasses);
      p->addClass(cd,name);
    }
  }
}

InheritanceListContext::~InheritanceListContext()
{
  delete p;
}

// TemplateListIntf
int InheritanceListContext::count() const
{
  return p->count();
}

TemplateVariant InheritanceListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *InheritanceListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list MemberList[Member] : list of inherited classes
class MemberListContext::Private : public GenericNodeListContext
{
  public:
    void addMember(MemberDef *md)
    {
      append(MemberContext::alloc(md));
    }
};

MemberListContext::MemberListContext() : RefCountedContext("MemberListContext")
{
  p = new Private;
}

MemberListContext::MemberListContext(const MemberList *list) : RefCountedContext("MemberListContext")
{
  p = new Private;
  if (list)
  {
    bool details = list->listType()&MemberListType_detailedLists;
    MemberListIterator mli(*list);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      if ((md->isBriefSectionVisible() && !details) ||
          (md->isDetailedSectionLinkable() && details)
         )
      {
        p->addMember(md);
      }
    }
  }
}

MemberListContext::MemberListContext(MemberSDict *list,bool doSort) : RefCountedContext("MemberListContext")
{
  p = new Private;
  if (list)
  {
    if (doSort)
    {
      list->sort();
    }
    MemberSDict::Iterator it(*list);
    MemberDef *md;
    for (it.toFirst();(md=it.current());++it)
    {
      p->addMember(md);
    }
  }
}

MemberListContext::~MemberListContext()
{
  delete p;
}

// TemplateListIntf
int MemberListContext::count() const
{
  return p->count();
}

TemplateVariant MemberListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *MemberListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct MemberInfo: member information
//%% {
class MemberInfoContext::Private : public PropertyMapper
{
  public:
    Private(const MemberInfo *mi) : m_memberInfo(mi)
    {
      //%% string protection
      addProperty("protection",this,&Private::protection);
      //%% string virtualness
      addProperty("virtualness",this,&Private::virtualness);
      //%% string ambiguityScope
      addProperty("ambiguityScope",this,&Private::ambiguityScope);
      //%% Member member
      addProperty("member",this,&Private::member);
    }
    TemplateVariant protection() const
    {
      switch (m_memberInfo->prot)
      {
        case ::Public:    return "public";
        case ::Protected: return "protected";
        case ::Private:   return "private";
        case ::Package:   return "package";
      }
      return "";
    }
    TemplateVariant virtualness() const
    {
      switch (m_memberInfo->virt)
      {
        case ::Normal:   return "normal";
        case ::Virtual:  return "virtual";
        case ::Pure:     return "pure";
      }
      return "";
    }
    TemplateVariant ambiguityScope() const
    {
      return m_memberInfo->ambiguityResolutionScope;
    }
    TemplateVariant member() const
    {
      if (!m_member && m_memberInfo->memberDef)
      {
        m_member.reset(MemberContext::alloc(m_memberInfo->memberDef));
      }
      if (m_member)
      {
        return m_member.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
  private:
    const MemberInfo *m_memberInfo;
    mutable SharedPtr<MemberContext> m_member;
};
//%% }

MemberInfoContext::MemberInfoContext(const MemberInfo *mi) : RefCountedContext("MemberInfoContext")
{
  p = new Private(mi);
}

MemberInfoContext::~MemberInfoContext()
{
  delete p;
}

TemplateVariant MemberInfoContext::get(const char *name) const
{
  return p->get(name);
}


//------------------------------------------------------------------------

//%% list AllMembersList[MemberList] : list of inherited classes
class AllMembersListContext::Private : public GenericNodeListContext
{
  public:
    Private(const MemberNameInfoSDict *ml)
    {
      if (ml)
      {
        static bool hideUndocMembers = Config_getBool("HIDE_UNDOC_MEMBERS");
        MemberNameInfoSDict::Iterator mnii(*ml);
        MemberNameInfo *mni;
        for (mnii.toFirst();(mni=mnii.current());++mnii)
        {
          MemberNameInfoIterator mnii2(*mni);
          MemberInfo *mi;
          for (mnii2.toFirst();(mi=mnii2.current());++mnii2)
          {
            MemberDef *md=mi->memberDef;
            ClassDef  *cd=md->getClassDef();
            if (cd && !md->name().isEmpty() && md->name()[0]!='@')
            {
              if ((cd->isLinkable() && md->isLinkable()) ||
                  (!cd->isArtificial() && !hideUndocMembers &&
                   (protectionLevelVisible(md->protection()) || md->isFriend())
                  )
                 )
              {
                append(MemberInfoContext::alloc(mi));
              }
            }
          }
        }
      }
    }
};

AllMembersListContext::AllMembersListContext() : RefCountedContext("AllMembersListContext")
{
  p = new Private(0);
}

AllMembersListContext::AllMembersListContext(const MemberNameInfoSDict *ml) : RefCountedContext("AllMembersListContext")
{
  p = new Private(ml);
}

AllMembersListContext::~AllMembersListContext()
{
  delete p;
}

// TemplateListIntf
int AllMembersListContext::count() const
{
  return p->count();
}

TemplateVariant AllMembersListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *AllMembersListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct MemberGroupInfo: member group information
//%% {
class MemberGroupInfoContext::Private : public PropertyMapper
{
  public:
    Private(Definition *def,const QCString &relPath,const MemberGroup *mg) :
      m_def(def),
      m_relPath(relPath),
      m_memberGroup(mg)
    {
      addProperty("members",      this,&Private::members);
      addProperty("title",        this,&Private::groupTitle);
      addProperty("subtitle",     this,&Private::groupSubtitle);
      addProperty("anchor",       this,&Private::groupAnchor);
      addProperty("memberGroups", this,&Private::memberGroups);
      addProperty("docs",         this,&Private::docs);
      addProperty("inherited",    this,&Private::inherited);
    }
    TemplateVariant members() const
    {
      if (!m_cache.memberListContext)
      {
        m_cache.memberListContext.reset(MemberListContext::alloc(m_memberGroup->members()));
      }
      return m_cache.memberListContext.get();
    }
    TemplateVariant groupTitle() const
    {
      return m_memberGroup->header();
    }
    TemplateVariant groupSubtitle() const
    {
      return "";
    }
    TemplateVariant groupAnchor() const
    {
      return m_memberGroup->anchor();
    }
    TemplateVariant memberGroups() const
    {
      if (!m_cache.memberGroups)
      {
        m_cache.memberGroups.reset(MemberGroupListContext::alloc(m_def,m_relPath,0));
      }
      return m_cache.memberGroups.get();
    }
    TemplateVariant docs() const
    {
      if (!m_cache.docs)
      {
        QCString docs = m_memberGroup->documentation();
        if (!docs.isEmpty())
        {
          m_cache.docs.reset(new TemplateVariant(
                           parseDoc(m_def,"[@name docs]",-1, // TODO store file & line
                                    m_relPath,
                                    m_memberGroup->documentation()+"\n",FALSE)));
        }
        else
        {
          m_cache.docs.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.docs;
    }
    TemplateVariant inherited() const
    {
      return FALSE;
    }
  private:
    Definition *m_def;
    QCString m_relPath;
    const MemberGroup *m_memberGroup;
    struct Cachable
    {
      SharedPtr<MemberListContext>      memberListContext;
      SharedPtr<MemberGroupListContext> memberGroups;
      ScopedPtr<TemplateVariant>        docs;
    };
    mutable Cachable m_cache;
};
//%% }

MemberGroupInfoContext::MemberGroupInfoContext(Definition *def,
       const QCString &relPath,const MemberGroup *mg) : RefCountedContext("MemberGroupInfoContext")
{
  p = new Private(def,relPath,mg);
}

MemberGroupInfoContext::~MemberGroupInfoContext()
{
  delete p;
}

TemplateVariant MemberGroupInfoContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list MemberGroupList[MemberGroupInfo] : list of member groups
class MemberGroupListContext::Private : public GenericNodeListContext
{
  public:
    void addMemberGroup(Definition *def,const QCString &relPath,const MemberGroup *mg)
    {
      append(MemberGroupInfoContext::alloc(def,relPath,mg));
    }
};

MemberGroupListContext::MemberGroupListContext() : RefCountedContext("MemberGroupListContext")
{
  p = new Private;
}

MemberGroupListContext::MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupList *list) : RefCountedContext("MemberGroupListContext")
{
  p = new Private;
  if (list)
  {
    MemberGroupListIterator mgli(*list);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      p->addMemberGroup(def,relPath,mg);
    }
  }
}

MemberGroupListContext::MemberGroupListContext(Definition *def,const QCString &relPath,const MemberGroupSDict *dict,bool subGrouping) : RefCountedContext("MemberGroupListContext")
{
  p = new Private;
  if (dict)
  {
    MemberGroupSDict::Iterator di(*dict);
    const MemberGroup *mg;
    for (di.toFirst();(mg=di.current());++di)
    {
      if (!mg->allMembersInSameSection() || !subGrouping)
      {
        p->addMemberGroup(def,relPath,mg);
      }
    }
  }
}

MemberGroupListContext::~MemberGroupListContext()
{
  delete p;
}

// TemplateListIntf
int MemberGroupListContext::count() const
{
  return p->count();
}

TemplateVariant MemberGroupListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *MemberGroupListContext::createIterator() const
{
  return p->createIterator();
}


//------------------------------------------------------------------------

//%% struct MemberListInfo: member list information
//%% {
class MemberListInfoContext::Private : public PropertyMapper
{
  public:
    Private(Definition *def,const QCString &relPath,const MemberList *ml,const QCString &title,const QCString &subtitle) :
      m_def(def),
      m_memberList(ml),
      m_relPath(relPath),
      m_title(title),
      m_subtitle(subtitle)
    {
      addProperty("members",      this,&Private::members);
      addProperty("title",        this,&Private::title);
      addProperty("subtitle",     this,&Private::subtitle);
      addProperty("anchor",       this,&Private::anchor);
      addProperty("memberGroups", this,&Private::memberGroups);
      addProperty("inherited",    this,&Private::inherited);
    }
    TemplateVariant members() const
    {
      if (!m_cache.memberListContext)
      {
        m_cache.memberListContext.reset(MemberListContext::alloc(m_memberList));
      }
      return m_cache.memberListContext.get();
    }
    TemplateVariant title() const
    {
      return m_title;
    }
    TemplateVariant subtitle() const
    {
      return m_subtitle;
    }
    TemplateVariant anchor() const
    {
      return MemberList::listTypeAsString(m_memberList->listType());
    }
    TemplateVariant memberGroups() const
    {
      if (!m_cache.memberGroups)
      {
        m_cache.memberGroups.reset(MemberGroupListContext::alloc(m_def,m_relPath,m_memberList->getMemberGroupList()));
      }
      return m_cache.memberGroups.get();
    }
    TemplateVariant inherited() const
    {
      if (!m_cache.inherited && (m_memberList->listType()&MemberListType_detailedLists)==0 &&
          m_def->definitionType()==Definition::TypeClass)
      {
        InheritedMemberInfoListContext *ctx = InheritedMemberInfoListContext::alloc();
        ctx->addMemberList((ClassDef*)m_def,m_memberList->listType(),m_title,FALSE);
        m_cache.inherited.reset(ctx);
      }
      if (m_cache.inherited)
      {
        return m_cache.inherited.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
  private:
    Definition *m_def;
    const MemberList *m_memberList;
    QCString m_relPath;
    QCString m_title;
    QCString m_subtitle;
    struct Cachable
    {
      SharedPtr<MemberListContext> memberListContext;
      SharedPtr<MemberGroupListContext> memberGroups;
      SharedPtr<InheritedMemberInfoListContext> inherited;
    };
    mutable Cachable m_cache;
};
//%% }

MemberListInfoContext::MemberListInfoContext(
           Definition *def,const QCString &relPath,const MemberList *ml,
           const QCString &title,const QCString &subtitle) : RefCountedContext("MemberListInfoContext")
{
  p = new Private(def,relPath,ml,title,subtitle);
}

MemberListInfoContext::~MemberListInfoContext()
{
  delete p;
}

TemplateVariant MemberListInfoContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% struct InheritedMemberInfo: inherited member information
//%% {
class InheritedMemberInfoContext::Private : public PropertyMapper
{
  public:
    Private(ClassDef *cd,MemberList *ml,const QCString &title)
      : m_class(cd), m_memberList(ml), m_title(title)
    {
      addProperty("class",         this,&Private::getClass);
      addProperty("title",         this,&Private::title);
      addProperty("members",       this,&Private::members);
      addProperty("id",            this,&Private::id);
      addProperty("inheritedFrom", this,&Private::inheritedFrom);
    }
    ~Private()
    {
      delete m_memberList;
    }
    TemplateVariant getClass() const
    {
      if (!m_classCtx)
      {
        m_classCtx.reset(ClassContext::alloc(m_class));
      }
      return m_classCtx.get();
    }
    TemplateVariant title() const
    {
      return m_title;
    }
    TemplateVariant members() const
    {
      if (!m_memberListCtx)
      {
        m_memberListCtx.reset(MemberListContext::alloc(m_memberList));
      }
      return m_memberListCtx.get();
    }
    TemplateVariant id() const
    {
      return substitute(MemberList::listTypeAsString(m_memberList->listType()),"-","_")+"_"+
                        stripPath(m_class->getOutputFileBase());
    }
    TemplateVariant inheritedFrom() const
    {
      if (!m_inheritedFrom)
      {
        m_inheritedFrom.reset(TemplateList::alloc());
        m_inheritedFrom->append(title());
        m_inheritedFrom->append(getClass());
      }
      return m_inheritedFrom.get();
    }

  private:
    ClassDef *  m_class;
    MemberList *m_memberList;
    QCString    m_title;
    mutable SharedPtr<ClassContext> m_classCtx;
    mutable SharedPtr<MemberListContext> m_memberListCtx;
    mutable SharedPtr<TemplateList> m_inheritedFrom;
};
//%% }

InheritedMemberInfoContext::InheritedMemberInfoContext(ClassDef *cd,MemberList *ml,
                                                       const QCString &title) : RefCountedContext("InheritedMemberInfoContext")
{
  p = new Private(cd,ml,title);
}

InheritedMemberInfoContext::~InheritedMemberInfoContext()
{
  delete p;
}

TemplateVariant InheritedMemberInfoContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list InheritedMemberList[InheritedMemberInfo] : list of inherited classes
class InheritedMemberInfoListContext::Private : public GenericNodeListContext
{
  public:
    void addMemberList(ClassDef *inheritedFrom,MemberList *ml,MemberList *combinedList)
    {
      if (ml)
      {
        MemberListIterator li(*ml);
        MemberDef *md;
        for (li.toFirst();(md=li.current());++li)
        {
          if (md->isBriefSectionVisible() && !md->isReimplementedBy(inheritedFrom))
          {
            combinedList->append(md);
          }
        }
      }
    }
    void addMemberListIncludingGrouped(ClassDef *inheritedFrom,MemberList *ml,MemberList *combinedList)
    {
      if (ml)
      {
        addMemberList(inheritedFrom,ml,combinedList);
        if (ml->getMemberGroupList())
        {
          MemberGroupListIterator mgli(*ml->getMemberGroupList());
          MemberGroup *mg;
          for (mgli.toFirst();(mg=mgli.current());++mgli)
          {
            addMemberList(inheritedFrom,mg->members(),combinedList);
          }
        }
      }
    }
    void addMemberGroupsOfClass(ClassDef *inheritedFrom,
                                ClassDef *cd,MemberListType lt,MemberList *combinedList)
    {
      if (cd->getMemberGroupSDict())
      {
        MemberGroupSDict::Iterator mgli(*cd->getMemberGroupSDict());
        MemberGroup *mg;
        for (;(mg=mgli.current());++mgli)
        {
          if (mg->members() && (!mg->allMembersInSameSection() || !cd->subGrouping())) // group is in its own section
          {
            MemberListIterator li(*mg->members());
            MemberDef *md;
            for (li.toFirst();(md=li.current());++li)
            {
              if (lt==md->getSectionList(mg->parent())->listType() &&
                  !md->isReimplementedBy(inheritedFrom) &&
                  md->isBriefSectionVisible())
              {
                combinedList->append(md);
              }
            }
          }
        }
      }
    }
    void addInheritedMembers(ClassDef *inheritedFrom,ClassDef *cd,MemberListType lt,
                             MemberListType lt1,int lt2,const QCString &title,bool additionalList)
    {
      int count = cd->countMembersIncludingGrouped(lt1,inheritedFrom,additionalList);
      if (lt2!=-1) count += cd->countMembersIncludingGrouped((MemberListType)lt2,inheritedFrom,additionalList);
      if (count>0)
      {
        MemberList *ml  = cd->getMemberList(lt1);
        MemberList *ml2 = lt2!=-1 ? cd->getMemberList((MemberListType)lt2) : 0;
        MemberList *combinedList = new MemberList(lt);
        addMemberListIncludingGrouped(inheritedFrom,ml,combinedList);
        addMemberListIncludingGrouped(inheritedFrom,ml2,combinedList);
        addMemberGroupsOfClass(inheritedFrom,cd,lt,combinedList);
        if (lt2!=-1) addMemberGroupsOfClass(inheritedFrom,cd,(MemberListType)lt2,combinedList);
        append(InheritedMemberInfoContext::alloc(cd,combinedList,title));
      }
    }
    void findInheritedMembers(ClassDef *inheritedFrom,ClassDef *cd,MemberListType lt,
                              int lt2, const QCString &title,bool additionalList,
                              QPtrDict<void> *visitedClasses)
    {
      if (cd->baseClasses())
      {
        BaseClassListIterator it(*cd->baseClasses());
        BaseClassDef *ibcd;
        for (it.toFirst();(ibcd=it.current());++it)
        {
          ClassDef *icd=ibcd->classDef;
          if (icd->isLinkable())
          {
            int lt1,lt3;
            convertProtectionLevel(lt,ibcd->prot,&lt1,&lt3);
            if (lt2==-1 && lt3!=-1)
            {
              lt2=lt3;
            }
            if (visitedClasses->find(icd)==0)
            {
              visitedClasses->insert(icd,icd); // guard for multiple virtual inheritance
              if (lt1!=-1)
              {
                // add member info for members of cd with list type lt
                addInheritedMembers(inheritedFrom,icd,lt,(MemberListType)lt1,lt2,title,additionalList);
                // recurse down the inheritance tree
                findInheritedMembers(inheritedFrom,icd,(MemberListType)lt1,lt2,title,additionalList,visitedClasses);
              }
            }
          }
        }
      }
    }
};

InheritedMemberInfoListContext::InheritedMemberInfoListContext() : RefCountedContext("InheritedMemberInfoListContext")
{
  p = new Private;
}

void InheritedMemberInfoListContext::addMemberList(
    ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList)
{
  QPtrDict<void> visited(17);
  bool memberInSection = cd->countMembersIncludingGrouped(lt,cd,FALSE)>0;
  bool show = (additionalList && !memberInSection) || // inherited member to show in the additional inherited members list
              (!additionalList && memberInSection);   // inherited member to show in a member list of the class
  //printf("%s:%s show=%d\n",cd->name().data(),MemberList::listTypeAsString(lt).data(),show);
  if (show)
  {
    p->findInheritedMembers(cd,cd,lt,-1,title,additionalList,&visited);
  }
}

InheritedMemberInfoListContext::~InheritedMemberInfoListContext()
{
  delete p;
}

// TemplateListIntf
int InheritedMemberInfoListContext::count() const
{
  return p->count();
}

TemplateVariant InheritedMemberInfoListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *InheritedMemberInfoListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct Argument: parameter information
//%% {
class ArgumentContext::Private : public PropertyMapper
{
  public:
    Private(const Argument *arg,Definition *def,const QCString &relPath) :
      m_argument(arg), m_def(def), m_relPath(relPath)
    {
      addProperty("type",     this,&Private::type);
      addProperty("name",     this,&Private::name);
      addProperty("defVal",   this,&Private::defVal);
      addProperty("docs",     this,&Private::docs);
      addProperty("attrib",   this,&Private::attrib);
      addProperty("array",    this,&Private::array);
      addProperty("namePart", this,&Private::namePart);
    }
    TemplateVariant type() const
    {
      return createLinkedText(m_def,m_relPath,m_argument->type);
    }
    TemplateVariant attrib() const
    {
      return m_argument->attrib;
    }
    TemplateVariant name() const
    {
      return m_argument->name;
    }
    TemplateVariant defVal() const
    {
      return createLinkedText(m_def,m_relPath,m_argument->defval);
    }
    TemplateVariant array() const
    {
      return m_argument->array;
    }
    TemplateVariant docs() const
    {
      if (!m_cache.docs && m_def)
      {
        if (!m_argument->docs.isEmpty())
        {
          m_cache.docs.reset(new TemplateVariant(
                             parseDoc(m_def,m_def->docFile(),m_def->docLine(),
                             m_relPath,m_argument->docs,TRUE)));
        }
        else
        {
          m_cache.docs.reset(new TemplateVariant(""));
        }
      }
      return *m_cache.docs;
    }
    TemplateVariant namePart() const
    {
      QCString result = m_argument->attrib;
      int l = result.length();
      if (l>2 && result.at(0)=='[' && result.at(l-1)==']')
      {
        result = result.mid(1,l-2);
        if (result!=",") result+=":"; // for normal keywords add colon
      }
      return result;
    }
  private:
    const Argument *m_argument;
    Definition *m_def;
    QCString m_relPath;
    struct Cachable
    {
      ScopedPtr<TemplateVariant> docs;
    };
    mutable Cachable m_cache;
};
//%% }

ArgumentContext::ArgumentContext(const Argument *al,Definition *def,const QCString &relPath) : RefCountedContext("ArgumentContext")
{
  p = new Private(al,def,relPath);
}

ArgumentContext::~ArgumentContext()
{
  delete p;
}

TemplateVariant ArgumentContext::get(const char *name) const
{
  return p->get(name);
}

//------------------------------------------------------------------------

//%% list ArgumentList[Argument] : list of inherited classes
class ArgumentListContext::Private : public GenericNodeListContext
{
  public:
    void addArgument(const Argument *arg,Definition *def,const QCString &relPath)
    {
      append(ArgumentContext::alloc(arg,def,relPath));
    }
};

ArgumentListContext::ArgumentListContext() : RefCountedContext("ArgumentListContext")
{
  p = new Private;
}

ArgumentListContext::ArgumentListContext(const ArgumentList *list,
                        Definition *def,const QCString &relPath) : RefCountedContext("ArgumentListContext")
{
  p = new Private;
  if (list)
  {
    ArgumentListIterator ali(*list);
    const Argument *arg;
    for (ali.toFirst();(arg=ali.current());++ali)
    {
      p->addArgument(arg,def,relPath);
    }
  }
}

ArgumentListContext::~ArgumentListContext()
{
  delete p;
}

// TemplateListIntf
int ArgumentListContext::count() const
{
  return p->count();
}

TemplateVariant ArgumentListContext::at(int index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIterator *ArgumentListContext::createIterator() const
{
  return p->createIterator();
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

class HtmlSpaceless : public TemplateSpacelessIntf
{
  public:
    HtmlSpaceless() : m_insideTag(FALSE), m_insideString('\0'), m_removeSpaces(TRUE) {}
    QCString remove(const QCString &s)
    {
      QGString result;
      const char *p = s.data();
      char c;
      while ((c=*p++))
      {
        switch(c)
        {
          case '<': // start of a tag
            if (!m_insideString) m_insideTag=TRUE,m_removeSpaces=FALSE;
            result+=c;
            break;
          case '>': // end of a tag
            if (!m_insideString) m_insideTag=FALSE,m_removeSpaces=TRUE;
            result+=c;
            break;
          case '\\': // escaped character in a string
            result+=c;
            if (m_insideString && *p) result+=*p++;
            break;
          case '"': case '\'':
            if (m_insideTag)
            {
              if (m_insideString==c) // end of string
              {
                m_insideString='\0';
              }
              else // start of string
              {
                m_insideString=c;
              }
            }
            result+=c;
            break;
          case ' ': case '\t': case '\n': // whitespace
            if (!m_insideTag) // outside tags strip consecutive whitespace
            {
              m_removeSpaces=TRUE;
            }
            else
            {
              result+=' ';
            }
            break;
          default:
            //if (m_removeSpaces) result+=' ';
            result+=c;
            m_removeSpaces=FALSE;
            break;
        }
      }
      result+='\0';
      //printf("HtmlSpaceless::remove('%s')='%s' m_insideTag=%d m_insideString=%d removeSpaces=%d\n",s.data(),result.data(),
      //    m_insideTag,m_insideString,m_removeSpaces);
      return result.data();
    }
  private:
    bool m_insideTag;
    char m_insideString;
    bool m_removeSpaces;
};

//------------------------------------------------------------------------

#if DEBUG_REF
int RefCountedContext::s_totalCount;
#endif

void generateOutputViaTemplate()
{
  {
  TemplateEngine e;
  TemplateContext *ctx = e.createContext();
  if (ctx)
  {
    SharedPtr<DoxygenContext>        doxygen         (DoxygenContext::alloc());
    SharedPtr<ConfigContext>         config          (ConfigContext::alloc());
    SharedPtr<TranslateContext>      tr              (TranslateContext::alloc());
    SharedPtr<ClassListContext>      classList       (ClassListContext::alloc());
    SharedPtr<ClassTreeContext>      classTree       (ClassTreeContext::alloc());
    SharedPtr<ClassHierarchyContext> classHierarchy  (ClassHierarchyContext::alloc());
    SharedPtr<NamespaceListContext>  namespaceList   (NamespaceListContext::alloc());
    SharedPtr<NamespaceTreeContext>  namespaceTree   (NamespaceTreeContext::alloc());
    SharedPtr<DirListContext>        dirList         (DirListContext::alloc());
    SharedPtr<FileListContext>       fileList        (FileListContext::alloc());
    SharedPtr<FileTreeContext>       fileTree        (FileTreeContext::alloc());
    SharedPtr<PageTreeContext>       pageTree        (PageTreeContext::alloc());
    SharedPtr<PageListContext>       pageList        (PageListContext::alloc());
    SharedPtr<ModuleTreeContext>     moduleTree      (ModuleTreeContext::alloc());
    SharedPtr<ExampleListContext>    exampleList     (ExampleListContext::alloc());

    //%% Doxygen doxygen:
    ctx->set("doxygen",doxygen.get());
    //%% Translator tr:
    ctx->set("tr",tr.get());
    //%% Config config:
    ctx->set("config",config.get());
    //%% ClassList classList:
    ctx->set("classList",classList.get()); // not used for standard HTML
    //%% ClassTree classTree:
    ctx->set("classTree",classTree.get());
    // classIndex
    //%% ClassHierarchy classHierarchy:
    ctx->set("classHierarchy",classHierarchy.get());
    //%% NamespaceList namespaceList:
    ctx->set("namespaceList",namespaceList.get());
    //%% NamespaceTree namespaceTree:
    ctx->set("namespaceTree",namespaceTree.get());
    //%% FileList fileList:
    ctx->set("fileList",fileList.get());
    //%% FileTree fileTree:
    ctx->set("fileTree",fileTree.get());
    //%% PageList pageList
    ctx->set("pageList",pageList.get());
    //%% PageTree pageTree
    ctx->set("pageTree",pageTree.get());
    //%% ModuleTree moduleTree
    ctx->set("moduleTree",moduleTree.get());
    //%% ExampleList exampleList
    ctx->set("exampleList",exampleList.get());
    //%% DirList dirList
    ctx->set("dirList",dirList.get());

    // render HTML output
    Template *tpl = e.loadByName("htmllayout.tpl",1);
    if (tpl)
    {
      g_globals.outputFormat = ContextGlobals::Html;
      g_globals.dynSectionId    = 0;
      g_globals.outputDir    = Config_getString("HTML_OUTPUT");
      HtmlEscaper htmlEsc;
      ctx->setEscapeIntf(Config_getString("HTML_FILE_EXTENSION"),&htmlEsc);
      HtmlSpaceless spl;
      ctx->setSpacelessIntf(&spl);
      ctx->setOutputDirectory(g_globals.outputDir);
      FTextStream ts;
      tpl->render(ts,ctx);
    }
    e.unload(tpl);

    // TODO: render other outputs
  }
  delete ctx;
  }
#if DEBUG_REF
  printf("==== total ref count %d\n",RefCountedContext::s_totalCount);
#endif
}

