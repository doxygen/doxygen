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
class TranslateContext::Private : public PropertyMapper
{
  public:

    static TemplateVariant generatedAtFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->generatedAt(args);
    }
    static TemplateVariant inheritanceDiagramForFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->inheritanceDiagramFor(args);
    }
    static TemplateVariant inheritsListFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->inheritsList(args);
    }
    static TemplateVariant inheritedByListFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->inheritedByList(args);
    }
    static TemplateVariant collaborationDiagramForFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->collaborationDiagramFor(args);
    }
    static TemplateVariant writeListFunc(const void *obj,const QValueList<TemplateVariant> &args)
    {
      return ((TranslateContext::Private*)obj)->writeList(args);
    }
    TemplateVariant generatedAt(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==2)
      {
        return theTranslator->trGeneratedAt(args[0].toString(),args[1].toString());
      }
      else
      {
        err("tr.generateAt should take two parameters, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant inheritanceDiagramFor(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trClassDiagram(args[0].toString());
      }
      else
      {
        err("tr.inheritanceDiagramFor should take one parameter, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant collaborationDiagramFor(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trCollaborationDiagram(args[0].toString());
      }
      else
      {
        err("tr.collaborationDiagramFor should take one parameter, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    TemplateVariant inheritsList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trInheritsList(args[0].toInt());
      }
      else
      {
        err("tr.inheritsList should take one integer parameter, got %d!\n",args.count());
      }
      return TemplateVariant();
    }
    QCString inheritedByList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trInheritedByList(args[0].toInt());
      }
      else
      {
        err("tr.inheritedByList should take one integer parameter, got %d!\n",args.count());
      }
      return QCString();
    }
    QCString writeList(const QValueList<TemplateVariant> &args) const
    {
      if (args.count()==1)
      {
        return theTranslator->trWriteList(args[0].toInt());
      }
      else
      {
        err("tr.*List should take one integer parameter, got %d!\n",args.count());
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
    TemplateVariant inheritanceDiagramFor() const
    {
      return TemplateVariant(this,&Private::inheritanceDiagramForFunc);
    }
    TemplateVariant collaborationDiagramFor() const
    {
      return TemplateVariant(this,&Private::collaborationDiagramForFunc);
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
    TemplateVariant inheritsList() const
    {
      return TemplateVariant(this,&Private::inheritsListFunc);
    }
    TemplateVariant inheritedByList() const
    {
      return TemplateVariant(this,&Private::inheritedByListFunc);
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
      return TemplateVariant(this,&Private::writeListFunc);
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
    result = TemplateVariant(docs,TRUE);
  delete root;
  return result;
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

      if (m_def && !m_def->getSourceFileBase().isEmpty())
      {
        m_sourceDef.append(&m_lineLink);
        m_sourceDef.append(&m_fileLink);
        m_lineLink.set("text",m_def->getStartBodyLine());
        m_lineLink.set("isLinkable",m_def->isLinkable());
        m_lineLink.set("fileName",m_def->getSourceFileBase());
        m_lineLink.set("anchor",m_def->getSourceAnchor());
        if (m_def->definitionType()==Definition::TypeFile)
        {
          m_fileLink.set("text",m_def->name());
        }
        else if (m_def->getBodyDef())
        {
          m_fileLink.set("text",m_def->getBodyDef()->name());
        }
        else
        {
          m_fileLink.set("text",name());
        }
        m_fileLink.set("isLinkable",m_def->isLinkable());
        m_fileLink.set("fileName",m_def->getSourceFileBase());
        m_fileLink.set("anchor",QCString());
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
      if (m_sourceDef.count()==2)
      {
        return &m_sourceDef;
      }
      else
      {
        return FALSE;
      }
    }

  private:
    Definition      *m_def;
    struct Cachable
    {
      ScopedPtr<TemplateVariant> details;
      ScopedPtr<TemplateVariant> brief;
    };
    mutable Cachable m_cache;
    TemplateList m_sourceDef;
    TemplateStruct m_fileLink;
    TemplateStruct m_lineLink;
};
//%% }

//------------------------------------------------------------------------

//%% struct IncludeInfo: include file information
//%% {
class IncludeInfoContext::Private : public PropertyMapper
{
  public:
    Private(IncludeInfo *info,SrcLangExt lang) :
      m_info(info),
      m_fileContext(info && info->fileDef ? info->fileDef : 0),
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
      if (m_info->fileDef)
      {
        return &m_fileContext;
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
    IncludeInfo *m_info;
    FileContext m_fileContext;
    SrcLangExt m_lang;
};

IncludeInfoContext::IncludeInfoContext(IncludeInfo *info,SrcLangExt lang)
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

//%% struct Class(Symbol): class information
//%% {
class ClassContext::Private : public DefinitionContext<ClassContext::Private>
{
  public:
    Private(ClassDef *cd) : DefinitionContext(cd) ,
       m_classDef(cd), m_usedFiles(cd),
       m_includeInfo(cd ? cd->includeInfo() : 0, cd ? cd->getLanguage() : SrcLangExt_Unknown)
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
      addProperty("includeStatement",          this,&Private::includeStatement);
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
      addProperty("relatedDecls",              this,&Private::relatedDecls);

      addProperty("typedefs",                  this,&Private::typedefs);
      addProperty("methods",                   this,&Private::methods);
      addProperty("relatedDefs",               this,&Private::relatedDefs);

      addProperty("nestedClasses",             this,&Private::nestedClasses);
      addProperty("compoundType",              this,&Private::compoundType);
      addProperty("templateDecls",             this,&Private::templateDecls);
      addProperty("typeConstraints",           this,&Private::typeConstraints);
      addProperty("examples",                  this,&Private::examples);
      addProperty("allMembersList",            this,&Private::allMembersList);
      addProperty("allMembersFileName",        this,&Private::allMembersFileName);
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
      return TemplateVariant(&m_usedFiles);
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
                       m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
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
            m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
            relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
            );
      }
      g_globals.dynSectionId++;
      return TemplateVariant(result.data(),TRUE);
    }

    TemplateVariant includeInfo() const
    {
      if (m_classDef->includeInfo())
      {
        return TemplateVariant(&m_includeInfo);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant includeStatement() const
    {
      return m_classDef->includeStatement();
    }
    TemplateVariant inherits() const
    {
      if (!m_cache.inheritsList)
      {
        m_cache.inheritsList.reset(new InheritanceListContext(m_classDef->baseClasses(),TRUE));
      }
      return m_cache.inheritsList.get();
    }
    TemplateVariant inheritedBy() const
    {
      if (!m_cache.inheritedByList)
      {
        m_cache.inheritedByList.reset(new InheritanceListContext(m_classDef->subClasses(),FALSE));
      }
      return m_cache.inheritedByList.get();
    }
    TemplateVariant getMemberList(ScopedPtr<MemberListInfoContext> &list,
                                  MemberListType type,const char *title) const
    {
      if (!list)
      {
        MemberList *ml = m_classDef->getMemberList(type);
        if (ml)
        {
          list.reset(new MemberListInfoContext(ml,title));
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
    TemplateVariant relatedDecls() const
    {
      return getMemberList(m_cache.relatedDecls,MemberListType_related,theTranslator->trRelatedFunctions());
    }
    TemplateVariant typedefs() const
    {
      return getMemberList(m_cache.typedefs,MemberListType_typedefMembers,theTranslator->trMemberTypedefDocumentation());
    }
    TemplateVariant methods() const
    {
      return getMemberList(m_cache.methods,MemberListType_functionMembers,theTranslator->trMemberFunctionDocumentation());
    }
    TemplateVariant relatedDefs() const
    {
      return getMemberList(m_cache.relatedDefs,MemberListType_relatedMembers,theTranslator->trRelatedFunctionDocumentation());
    }
    TemplateVariant nestedClasses() const
    {
      static bool extractPrivate      = Config_getBool("EXTRACT_PRIVATE");
      static bool hideUndocClasses    = Config_getBool("HIDE_UNDOC_CLASSES");
      static bool extractLocalClasses = Config_getBool("EXTRACT_LOCAL_CLASSES");
      if (!m_cache.nestedClasses)
      {
        if (m_classDef->getClassSDict())
        {
          NestedClassListContext *classList = new NestedClassListContext;
          ClassSDict::Iterator sdi(*m_classDef->getClassSDict());
          ClassDef *cd;
          for (sdi.toFirst();(cd=sdi.current());++sdi)
          {
            bool linkable = cd->isLinkable();
            if (cd->name().find('@')==-1 && !cd->isExtension() &&
                (cd->protection()!=::Private || extractPrivate) &&
                (linkable ||
                 (!hideUndocClasses && (!cd->isLocal() || extractLocalClasses)))
               )
            {
              classList->append(cd);
            }
          }
          m_cache.nestedClasses.reset(classList);
        }
      }
      if (m_cache.nestedClasses)
      {
        return m_cache.nestedClasses.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
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
          ArgumentListContext *al = new ArgumentListContext(cd->templateArguments(),cd,relPathAsString());
          // since a TemplateVariant does take ownership of the object, we add it
          // a separate list just to be able to delete it and avoid a memory leak
          m_cache.templateArgList.append(al);
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
          TemplateStruct *s = new TemplateStruct;
          m_cache.exampleList.append(s);
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
        TemplateList *tl = new TemplateList;
        addTemplateDecls(m_classDef,tl);
        m_cache.templateDecls.reset(tl);
      }
      if (m_cache.templateDecls)
      {
        return m_cache.templateDecls.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant typeConstraints() const
    {
      if (!m_cache.typeConstraints && m_classDef->typeConstraints())
      {
        m_cache.typeConstraints.reset(new ArgumentListContext(m_classDef->typeConstraints(),m_classDef,relPathAsString()));
      }
      if (m_cache.typeConstraints)
      {
        return m_cache.typeConstraints.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant examples() const
    {
      if (!m_cache.examples)
      {
        TemplateList *exampleList = new TemplateList;
        addExamples(exampleList);
        m_cache.examples.reset(exampleList);
      }
      if (m_cache.examples)
      {
        return m_cache.examples.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant allMembersList() const
    {
      if (!m_cache.allMembersList && m_classDef->memberNameInfoSDict())
      {
        AllMembersListContext *ml = new AllMembersListContext(m_classDef->memberNameInfoSDict());
        m_cache.allMembersList.reset(ml);
      }
      if (m_cache.allMembersList)
      {
        return m_cache.allMembersList.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant allMembersFileName() const
    {
      return m_classDef->getMemberListFileName();
    }

  private:
    ClassDef *m_classDef;
    UsedFilesContext m_usedFiles;
    IncludeInfoContext m_includeInfo;
    struct Cachable
    {
      Cachable() : inheritanceNodes(-1)
      {
        templateArgList.setAutoDelete(TRUE);
        exampleList.setAutoDelete(TRUE);
      }
      ScopedPtr<InheritanceListContext> inheritsList;
      ScopedPtr<InheritanceListContext> inheritedByList;
      ScopedPtr<DotClassGraph> classGraph;
      ScopedPtr<DotClassGraph> collaborationGraph;
      ScopedPtr<NestedClassListContext> nestedClasses;
      ScopedPtr<MemberListInfoContext> publicTypes;
      ScopedPtr<MemberListInfoContext> publicMethods;
      ScopedPtr<MemberListInfoContext> publicStaticMethods;
      ScopedPtr<MemberListInfoContext> publicAttributes;
      ScopedPtr<MemberListInfoContext> publicStaticAttributes;
      ScopedPtr<MemberListInfoContext> publicSlots;
      ScopedPtr<MemberListInfoContext> protectedTypes;
      ScopedPtr<MemberListInfoContext> protectedMethods;
      ScopedPtr<MemberListInfoContext> protectedStaticMethods;
      ScopedPtr<MemberListInfoContext> protectedAttributes;
      ScopedPtr<MemberListInfoContext> protectedStaticAttributes;
      ScopedPtr<MemberListInfoContext> protectedSlots;
      ScopedPtr<MemberListInfoContext> privateTypes;
      ScopedPtr<MemberListInfoContext> privateMethods;
      ScopedPtr<MemberListInfoContext> privateStaticMethods;
      ScopedPtr<MemberListInfoContext> privateAttributes;
      ScopedPtr<MemberListInfoContext> privateStaticAttributes;
      ScopedPtr<MemberListInfoContext> privateSlots;
      ScopedPtr<MemberListInfoContext> packageTypes;
      ScopedPtr<MemberListInfoContext> packageMethods;
      ScopedPtr<MemberListInfoContext> packageStaticMethods;
      ScopedPtr<MemberListInfoContext> packageAttributes;
      ScopedPtr<MemberListInfoContext> packageStaticAttributes;
      ScopedPtr<MemberListInfoContext> unoIDLServices;
      ScopedPtr<MemberListInfoContext> unoIDLInterfaces;
      ScopedPtr<MemberListInfoContext> signals;
      ScopedPtr<MemberListInfoContext> properties;
      ScopedPtr<MemberListInfoContext> events;
      ScopedPtr<MemberListInfoContext> friends;
      ScopedPtr<MemberListInfoContext> relatedDecls;
      ScopedPtr<MemberListInfoContext> typedefs;
      ScopedPtr<MemberListInfoContext> methods;
      ScopedPtr<MemberListInfoContext> relatedDefs;
      ScopedPtr<AllMembersListContext> allMembersList;
      ScopedPtr<ArgumentListContext>   typeConstraints;
      ScopedPtr<TemplateList>          examples;
      ScopedPtr<TemplateList>          templateDecls;
      QList<ArgumentListContext> templateArgList;
      int inheritanceNodes;
      QList<TemplateStruct>      exampleList;
    };
    mutable Cachable m_cache;
};
//%% }

ClassContext::ClassContext(ClassDef *cd)
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
    Private(NamespaceDef *nd) : DefinitionContext(nd) , m_namespaceDef(nd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
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
    Private(FileDef *fd) : DefinitionContext(fd) , m_fileDef(fd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
      addProperty("versionInfo",this,&Private::versionInfo);
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
    Private(DirDef *dd) : DefinitionContext(dd) , m_dirDef(dd)
    {
      addProperty("title",this,&Private::title);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subHighlight);
      addProperty("dirName",this,&Private::dirName);
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
    Private(PageDef *pd) : DefinitionContext(pd) , m_pageDef(pd)
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
      m_ts << "<br/>";
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
    Private(MemberDef *md) : DefinitionContext(md) , m_memberDef(md)
    {
      addProperty("declType",            this,&Private::declType);
      addProperty("declArgs",            this,&Private::declArgs);
      addProperty("isStatic",            this,&Private::isStatic);
      addProperty("isObjCMethod",        this,&Private::isObjCMethod);
      addProperty("isObjCProperty",      this,&Private::isObjCProperty);
      addProperty("isDefine",            this,&Private::isDefine);
      addProperty("isImplementation",    this,&Private::isImplementation);
      addProperty("isEvent",             this,&Private::isEvent);
      addProperty("isProperty",          this,&Private::isProperty);
      addProperty("isEnumeration",       this,&Private::isEnumeration);
      addProperty("isEnumValue",         this,&Private::isEnumValue);
      addProperty("isAnonymous",         this,&Private::isAnonymous);
      addProperty("hasDetails",          this,&Private::hasDetails);
      addProperty("exception",           this,&Private::exception);
      addProperty("bitfields",           this,&Private::bitfields);
      addProperty("initializer",         this,&Private::initializer);
      addProperty("oneLineInitializer",  this,&Private::oneLineInitializer);
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

      if (md && md->isProperty())
      {
        if (md->isGettable()) m_propertyAttrs.append("get");
        if (md->isSettable()) m_propertyAttrs.append("set");
      }
      if (md && md->isEvent())
      {
        if (md->isAddable())   m_eventAttrs.append("add");
        if (md->isRemovable()) m_eventAttrs.append("remove");
        if (md->isRaisable())  m_eventAttrs.append("raise");
      }
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
    TemplateVariant isEvent() const
    {
      return m_memberDef->isEvent();
    }
    TemplateVariant isProperty() const
    {
      return m_memberDef->isProperty();
    }
    TemplateVariant isEnumValue() const
    {
      return m_memberDef->isEnumValue();
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
    TemplateVariant isDefine() const
    {
      return m_memberDef->isDefine();
    }
    TemplateVariant isAnonymous() const
    {
      QCString name = m_memberDef->name();
      return !name.isEmpty() && name.at(0)=='@';
    }
    TemplateVariant enumBaseType() const
    {
      return m_memberDef->enumBaseType();
    }
    TemplateVariant oneLineInitializer() const
    {
      return m_memberDef->hasOneLineInitializer();
    }
    TemplateVariant enumValues() const
    {
      if (!m_cache.enumValues)
      {
        MemberList *ml = m_memberDef->enumFieldList();
        if (ml)
        {
          m_cache.enumValues.reset(new MemberListContext(ml));
        }
      }
      if (m_cache.enumValues)
      {
        return m_cache.enumValues.get();
      }
      else
      {
        return FALSE;
      }
    }
    TemplateVariant templateArgs() const
    {
      if (!m_cache.templateArgs)
      {
        m_cache.templateArgs.reset(new ArgumentListContext(m_memberDef->templateArguments(),m_memberDef,relPathAsString()));
      }
      return m_cache.templateArgs.get();
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
      return &m_propertyAttrs;
    }
    TemplateVariant eventAttrs() const
    {
      return &m_eventAttrs;
    }
    TemplateVariant getClass() const
    {
      if (!m_cache.classDef && m_memberDef->getClassDef())
      {
        m_cache.classDef.reset(new ClassContext(m_memberDef->getClassDef()));
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
          m_cache.arguments.reset(new ArgumentListContext(defArgList,m_memberDef,relPathAsString()));
        }
      }
      if (m_cache.arguments)
      {
        return m_cache.arguments.get();
      }
      else
      {
        return FALSE;
      }
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
            ArgumentListContext *al = new ArgumentListContext(tal,m_memberDef,relPathAsString());
            // since a TemplateVariant does take ownership of the object, we add it
            // a separate list just to be able to delete it and avoid a memory leak
            m_cache.templateArgList.append(al);
            tl->append(al);
          }
        }
      }
      else
      {
        if (cd && !m_memberDef->isTemplateSpecialization())
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
              ArgumentListContext *al = new ArgumentListContext(tal,m_memberDef,relPathAsString());
              // since a TemplateVariant does take ownership of the object, we add it
              // a separate list just to be able to delete it and avoid a memory leak
              m_cache.templateArgList.append(al);
              tl->append(al);
            }
          }
        }
        if (m_memberDef->templateArguments()) // function template prefix
        {
          ArgumentListContext *al = new ArgumentListContext(
              m_memberDef->templateArguments(),m_memberDef,relPathAsString());
          // since a TemplateVariant does take ownership of the object, we add it
          // a separate list just to be able to delete it and avoid a memory leak
          m_cache.templateArgList.append(al);
          tl->append(al);
        }
      }
    }
    TemplateVariant templateDecls() const
    {
      if (!m_cache.templateDecls)
      {
        TemplateList *tl = new TemplateList;
        addTemplateDecls(tl);
        m_cache.templateDecls.reset(tl);
      }
      if (m_cache.templateDecls)
      {
        return m_cache.templateDecls.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant labels() const
    {
      if (!m_cache.labels)
      {
        QStrList sl;
        m_memberDef->getLabels(sl,m_memberDef->getOuterScope());
        if (sl.count()>0)
        {
          TemplateList *tl = new TemplateList;
          QStrListIterator it(sl);
          for (;it.current();++it)
          {
            tl->append(*it);
          }
          m_cache.labels.reset(tl);
        }
      }
      if (m_cache.labels)
      {
        return m_cache.labels.get();
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
  private:
    MemberDef *m_memberDef;
    struct Cachable
    {
      ScopedPtr<ArgumentListContext> templateArgs;
      ScopedPtr<ArgumentListContext> arguments;
      ScopedPtr<MemberListContext>   enumValues;
      ScopedPtr<ClassContext>        classDef;
      ScopedPtr<TemplateList>        templateDecls;
      QList<ArgumentListContext>     templateArgList;
      ScopedPtr<TemplateList>        labels;
    };
    mutable Cachable m_cache;
    TemplateList m_propertyAttrs;
    TemplateList m_eventAttrs;
};
//%% }

MemberContext::MemberContext(MemberDef *md)
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
    Private(GroupDef *gd) : DefinitionContext(gd) , m_groupDef(gd)
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

//%% list NestedClassList[Class] : list of nested classes
class NestedClassListContext::Private : public GenericNodeListContext<ClassContext>
{
};

NestedClassListContext::NestedClassListContext()
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
    p->append(new ClassContext(cd));
  }
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
class ClassInheritanceNodeContext::Private : public PropertyMapper
{
  public:
    Private(ClassDef *cd) : m_classContext(cd)
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
class ClassHierarchyContext::Private : public PropertyMapper
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
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
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
class NestingNodeContext::Private : public PropertyMapper
{
  public:
    Private(Definition *d,bool addCls) : m_def(d),
       m_classContext(m_def->definitionType()==Definition::TypeClass?(ClassDef*)d:0),
       m_namespaceContext(m_def->definitionType()==Definition::TypeNamespace?(NamespaceDef*)d:0)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% Nesting children: list of nested classes/namespaces
      addProperty("children",this,&Private::children);
      //%% [optional] Class class: class info (if this node represents a class)
      addProperty("class",this,&Private::getClass);
      //%% [optional] Namespace namespace: namespace info (if this node represents a namespace)
      addProperty("namespace",this,&Private::getNamespace);
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
class ClassTreeContext::Private : public PropertyMapper
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
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
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
      if (Doxygen::namespaceSDict)
      {
        m_namespaceTree.addNamespaces(*Doxygen::namespaceSDict,TRUE,FALSE);
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

//%% list UsedFiles[File] : list of files
class UsedFilesContext::Private : public GenericNodeListContext<FileContext>
{
  public:
    void addFile(FileDef *fd)
    {
      append(new FileContext(fd));
    }
};

UsedFilesContext::UsedFilesContext(ClassDef *cd)
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


//%% struct DirFileNode: node is a directory hierarchy
//%% {
class DirFileNodeContext::Private : public PropertyMapper
{
  public:
    Private(Definition *d) : m_def(d),
       m_dirContext (m_def->definitionType()==Definition::TypeDir  ? (DirDef*)d  : 0),
       m_fileContext(m_def->definitionType()==Definition::TypeFile ? (FileDef*)d : 0)
    {
      //%% bool is_leaf_node: true if this node does not have any children
      addProperty("is_leaf_node",this,&Private::isLeafNode);
      //%% DirFile children: list of nested classes/namespaces
      addProperty("children",this,&Private::children);
      //%% [optional] Dir dir: directory info (if this node represents a directory)
      addProperty("dir",this,&Private::getDir);
      //%% [optional] File file: file info (if this node represents a file)
      addProperty("file",this,&Private::getFile);
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
class FileTreeContext::Private : public PropertyMapper
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
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
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
class PageNodeContext::Private : public PropertyMapper
{
  public:
    Private(PageDef *pd) : m_pageDef(pd), m_pageContext(pd)
    {
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
class PageTreeContext::Private : public PropertyMapper
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
      addProperty("tree",this,&Private::tree);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
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
class PageListContext::Private : public PropertyMapper
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
      addProperty("items",this,&Private::items);
      addProperty("fileName",this,&Private::fileName);
      addProperty("relPath",this,&Private::relPath);
      addProperty("highlight",this,&Private::highlight);
      addProperty("subhighlight",this,&Private::subhighlight);
      addProperty("title",this,&Private::title);
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
class ModuleNodeContext::Private : public PropertyMapper
{
  public:
    Private(GroupDef *gd) : m_groupDef(gd), m_moduleContext(gd)
    {
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
      // Add modules
      if (Doxygen::groupSDict)
      {
        m_moduleList.addModules(*Doxygen::groupSDict);
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
      // Add pages
      if (Doxygen::exampleSDict)
      {
        m_pageList.addPages(*Doxygen::exampleSDict,FALSE);
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

//%% struct InheritanceNode: a class in the inheritance list
//%% {
class InheritanceNodeContext::Private : public PropertyMapper
{
  public:
    Private(ClassDef *cd,const QCString &name) : m_classContext(cd), m_name(name)
    {
      addProperty("class",this,&Private::getClass);
      addProperty("name",this,&Private::name);
    }
    TemplateVariant getClass() const
    {
      return &m_classContext;
    }
    TemplateVariant name() const
    {
      return m_name;
    }
  private:
    ClassContext m_classContext;
    QCString m_name;
};
//%% }

InheritanceNodeContext::InheritanceNodeContext(ClassDef *cd,const QCString &name)
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
class InheritanceListContext::Private : public GenericNodeListContext<InheritanceNodeContext>
{
  public:
    void addClass(ClassDef *cd,const QCString &name)
    {
      append(new InheritanceNodeContext(cd,name));
    }
};

InheritanceListContext::InheritanceListContext(const BaseClassList *list, bool baseClasses)
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
class MemberListContext::Private : public GenericNodeListContext<MemberContext>
{
  public:
    void addMember(MemberDef *md)
    {
      append(new MemberContext(md));
    }
};

MemberListContext::MemberListContext(const MemberList *list)
{
  p = new Private;
  if (list)
  {
    MemberListIterator mli(*list);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      if (md->isBriefSectionVisible())
      {
        p->addMember(md);
      }
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
    }
    TemplateVariant virtualness() const
    {
      switch (m_memberInfo->virt)
      {
        case ::Normal:   return "normal";
        case ::Virtual:  return "virtual";
        case ::Pure:     return "pure";
      }
    }
    TemplateVariant ambiguityScope() const
    {
      return m_memberInfo->ambiguityResolutionScope;
    }
    TemplateVariant member() const
    {
      if (!m_member && m_memberInfo->memberDef)
      {
        m_member.reset(new MemberContext(m_memberInfo->memberDef));
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
    mutable ScopedPtr<MemberContext> m_member;
};
//%% }

MemberInfoContext::MemberInfoContext(const MemberInfo *mi)
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
class AllMembersListContext::Private : public GenericNodeListContext<MemberInfoContext>
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
                append(new MemberInfoContext(mi));
              }
            }
          }
        }
      }
    }
};

AllMembersListContext::AllMembersListContext(const MemberNameInfoSDict *ml)
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

//%% struct MemberListInfo: member list information
//%% {
class MemberListInfoContext::Private : public PropertyMapper
{
  public:
    Private(const MemberList *ml,const QCString &title,const QCString &subtitle) :
      m_memberListContext(ml), m_memberList(ml), m_title(title), m_subtitle(subtitle)
    {
      addProperty("members", this,&Private::members);
      addProperty("title",   this,&Private::title);
      addProperty("subtitle",this,&Private::subtitle);
      addProperty("anchor",  this,&Private::anchor);
      // TODO: member groups
    }
    TemplateVariant members() const
    {
      return &m_memberListContext;
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
      return m_memberList->listTypeAsString(m_memberList->listType());
    }
  private:
    MemberListContext m_memberListContext;
    const MemberList *m_memberList;
    QCString m_title;
    QCString m_subtitle;
};
//%% }

MemberListInfoContext::MemberListInfoContext(const MemberList *ml,
           const QCString &title,const QCString &subtitle)
{
  p = new Private(ml,title,subtitle);
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

ArgumentContext::ArgumentContext(const Argument *al,Definition *def,const QCString &relPath)
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
class ArgumentListContext::Private : public GenericNodeListContext<ArgumentContext>
{
  public:
    void addArgument(const Argument *arg,Definition *def,const QCString &relPath)
    {
      append(new ArgumentContext(arg,def,relPath));
    }
};

ArgumentListContext::ArgumentListContext(const ArgumentList *list,
                        Definition *def,const QCString &relPath)
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

void generateOutputViaTemplate()
{
  TemplateEngine e;
  TemplateContext *ctx = e.createContext();
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
      g_globals.outputFormat = ContextGlobals::Html;
      g_globals.dynSectionId    = 0;
      g_globals.outputDir    = Config_getString("HTML_OUTPUT");
      HtmlEscaper esc;
      ctx->setEscapeIntf(&esc);
      HtmlSpaceless spl;
      ctx->setSpacelessIntf(&spl);
      ctx->setOutputDirectory(g_globals.outputDir);
      FTextStream ts;
      tpl->render(ts,ctx);
    }

    // TODO: render other outputs
  }
}

