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

#include <assert.h>
#include <sstream>

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
#include "htmlhelp.h"
#include "latexgen.h"
#include "latexdocvisitor.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "dotclassgraph.h"
#include "dotdirdeps.h"
#include "dotgfxhierarchytable.h"
#include "dotgroupcollaboration.h"
#include "dotincldepgraph.h"
#include "diagram.h"
#include "example.h"
#include "membername.h"
#include "parserintf.h"
#include "portable.h"
#include "arguments.h"
#include "groupdef.h"
#include "searchindex.h"
#include "resourcemgr.h"
#include "dir.h"

// TODO: pass the current file to Dot*::writeGraph, so the user can put dot graphs in other
//       files as well

enum ContextOutputFormat
{
  ContextOutputFormat_Unspecified=0,
  ContextOutputFormat_Html,
  ContextOutputFormat_Latex,
  ContextOutputFormat_Rtf,
  ContextOutputFormat_ManPage,
  ContextOutputFormat_DocBook,
  ContextOutputFormat_Xml,
  ContextOutputFormat_TagFile
};

struct ContextGlobals
{
  int                 dynSectionId;
  QCString            outputDir;
  ContextOutputFormat outputFormat;
} g_globals;


/** @brief Template List iterator support */
class GenericConstIterator : public TemplateListIntf::ConstIterator
{
  public:
    GenericConstIterator(const std::vector<TemplateVariant> &list) : m_list(list) {}
    virtual ~GenericConstIterator() {}
    void toFirst()
    {
      m_index=0;
    }
    void toLast()
    {
      m_index=static_cast<int>(m_list.size())-1;
    }
    void toNext()
    {
      if (m_index < static_cast<int>(m_list.size())) ++m_index;
    }
    void toPrev()
    {
      if (m_index>=0) --m_index;
    }
    bool current(TemplateVariant &v) const
    {
      if (m_index>=0 && m_index < static_cast<int>(m_list.size()))
      {
        v = m_list[m_index];
        return true;
      }
      else
      {
        v = TemplateVariant();
        return false;
      }
    }
  private:
    const std::vector<TemplateVariant> &m_list;
    int m_index=0;
};

//------------------------------------------------------------------------

/** @brief standard template list implementation */
class GenericNodeListContext : public TemplateListIntf
{
  public:
    GenericNodeListContext()
    {
    }
    static TemplateListIntfPtr alloc()
    {
      return std::static_pointer_cast<TemplateListIntf>(std::make_shared<GenericNodeListContext>());
    }

    // TemplateListIntf methods
    uint count() const
    {
      return static_cast<uint>(m_children.size());
    }
    TemplateVariant at(uint index) const
    {
      TemplateVariant result;
      if (index<count())
      {
        result = m_children[index];
      }
      return result;
    }
    TemplateListIntf::ConstIteratorPtr createIterator() const
    {
      return std::make_unique<GenericConstIterator>(m_children);
    }

    void append(const TemplateVariant &ctn)
    {
      m_children.emplace_back(ctn);
    }
    bool isEmpty() const
    {
      return m_children.empty();
    }
  private:
    std::vector< TemplateVariant > m_children;
};

//------------------------------------------------------------------------

/** @brief Helper class to map a property name to a handler member function */
template<typename T>
class PropertyMapper
{
  private:
    struct PropertyFuncIntf
    {
      virtual ~PropertyFuncIntf() {}
      virtual TemplateVariant operator()(const T *obj) const = 0;
    };
    struct PropertyFunc : public PropertyFuncIntf
    {
      typedef TemplateVariant (T::*Handler)() const;
      PropertyFunc(Handler h) : handler(h) {}
      TemplateVariant operator()(const T *obj) const
      {
        return (obj->*handler)();
      }
      Handler handler;
    };

  public:
    /** Add a property to the map
     *  @param[in] name   The name of the property to add.
     *  @param[in] handle The method to call when the property is accessed.
     */
    void addProperty(const char *name,typename PropertyFunc::Handler handle)
    {
      auto it = m_map.find(name);
      if (it!=m_map.end())
      {
        err("adding property '%s' more than once\n",name);
      }
      else
      {
        m_map.insert(std::make_pair(name,std::make_unique<PropertyFunc>(handle)));
      }
    }

    /** Gets the value of a property.
     *  @param[in] obj  The object handling access to the property.
     *  @param[in] name The name of the property.
     *  @returns A variant representing the properties value or an
     *  invalid variant if it was not found.
     */
    TemplateVariant get(const T *obj,const QCString &name) const
    {
      //printf("PropertyMapper::get(%s)\n",name);
      TemplateVariant result;
      auto it = m_map.find(name.str());
      return it!=m_map.end() ? (*it->second)(obj) : TemplateVariant();
    }

    StringVector fields() const
    {
      StringVector result;
      for (const auto &kv : m_map)
      {
        result.push_back(kv.first);
      }
      std::sort(result.begin(),result.end());
      return result;
    }

  private:
    std::unordered_map<std::string,std::unique_ptr<PropertyFuncIntf>> m_map;
};


//------------------------------------------------------------------------

//%% struct Config : configuration options
//%% {
class ConfigContext::Private
{
  public:
    Private() { }
    virtual ~Private() { }
    TemplateVariant fetchList(const QCString &name,const StringVector &list)
    {
      auto it = m_cachedLists.find(name.str());
      if (it==m_cachedLists.end())
      {
        TemplateListPtr tlist = TemplateList::alloc();
        m_cachedLists.insert(std::make_pair(name.str(),TemplateVariant(std::static_pointer_cast<TemplateListIntf>(tlist))));
        for (const auto &s : list)
        {
          tlist->append(s.c_str());
        }
        return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(tlist));
      }
      else
      {
        return it->second;
      }
    }
  private:
    std::unordered_map<std::string,TemplateVariant> m_cachedLists;
};
//%% }

ConfigContext::ConfigContext() : p(std::make_unique<Private>())
{
}

ConfigContext::~ConfigContext()
{
}

TemplateVariant ConfigContext::get(const QCString &name) const
{
  TemplateVariant result;
  if (!name.isEmpty())
  {
    const ConfigValues::Info *option = ConfigValues::instance().get(name);
    if (option)
    {
      switch (option->type)
      {
        case ConfigValues::Info::Bool:
          {
            bool b = ConfigValues::instance().*(option->value.b);
            return TemplateVariant(b);
          }
        case ConfigValues::Info::Int:
          {
            int i = ConfigValues::instance().*(option->value.i);
            return TemplateVariant(i);
          }
        case ConfigValues::Info::String:
          {
            QCString s = ConfigValues::instance().*(option->value.s);
            return TemplateVariant(s);
          }
        case ConfigValues::Info::List:
          {
            const StringVector &l = ConfigValues::instance().*(option->value.l);
            return p->fetchList(name,l);
          }
        default:
          break;
      }
    }
  }
  return result;
}

StringVector ConfigContext::fields() const
{
  return ConfigValues::instance().fields();
}

//------------------------------------------------------------------------

//%% struct Doxygen: global information
//%% {
class DoxygenContext::Private
{
  public:
    TemplateVariant version() const
    {
      return getDoxygenVersion();
    }
    TemplateVariant date() const
    {
      return dateToString(TRUE);
    }
    TemplateVariant mathJaxCodeFile() const
    {
      return m_cache.mathJaxCodeFile;
    }
    TemplateVariant mathJaxMacros() const
    {
      return m_cache.mathJaxMacros;
    }
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        //%% string version
        s_inst.addProperty("version",         &Private::version);
        //%% string date
        s_inst.addProperty("date",            &Private::date);
        //%% string maxJaxCodeFile
        s_inst.addProperty("mathJaxCodeFile", &Private::mathJaxCodeFile);
        //%% string maxJaxMacros
        s_inst.addProperty("mathJaxMacros",   &Private::mathJaxMacros);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
  private:
    struct Cachable
    {
      Cachable() {
        mathJaxCodeFile=fileToString(Config_getString(MATHJAX_CODEFILE));
        mathJaxMacros=HtmlGenerator::getMathJaxMacros();
      }
      QCString mathJaxCodeFile;
      QCString mathJaxMacros;
    };
    mutable Cachable m_cache;
    static PropertyMapper<DoxygenContext::Private> s_inst;
};
//%% }

PropertyMapper<DoxygenContext::Private> DoxygenContext::Private::s_inst;
//                                (PropertyMapper<DoxygenContext::Private>::instance());

DoxygenContext::DoxygenContext() : p(std::make_unique<Private>())
{
}

DoxygenContext::~DoxygenContext()
{
}

TemplateVariant DoxygenContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector DoxygenContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Translator: translation methods
//%% {
class TranslateContext::Private
{
  public:

    TemplateVariant handleGeneratedAt(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==2)
      {
        return theTranslator->trGeneratedAt(args[0].toString(),args[1].toString());
      }
      else
      {
        err("tr.generateAt should take two arguments, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritanceDiagramFor(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trClassDiagram(args[0].toString());
      }
      else
      {
        err("tr.inheritanceDiagramFor should take one argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleCollaborationDiagramFor(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trCollaborationDiagram(args[0].toString());
      }
      else
      {
        err("tr.collaborationDiagramFor should take one argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleDirDependencyGraphFor(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trDirDepGraph(args[0].toString());
      }
      else
      {
        err("tr.dirDependencyGraphFor should take one argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritsList(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trInheritsList(args[0].toInt());
      }
      else
      {
        err("tr.inheritsList should take one integer argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleInheritedByList(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trInheritedByList(args[0].toInt());
      }
      else
      {
        err("tr.inheritedByList should take one integer argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleWriteList(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trWriteList(args[0].toInt());
      }
      else
      {
        err("tr.*List should take one integer argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleImplementedBy(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trImplementedInList(args[0].toInt());
      }
      else
      {
        err("tr.implementedBy should take one integer argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleReimplementedBy(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trReimplementedInList(args[0].toInt());
      }
      else
      {
        err("tr.reimplementedBy should take one integer argument, got %zu!\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleSourceRefs(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trReferences()+" "+theTranslator->trWriteList(args[0].toInt())+".";
      }
      else
      {
        err("tr.sourceRefs should take one integer argument, got %zu\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleSourceRefBys(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trReferencedBy()+" "+theTranslator->trWriteList(args[0].toInt())+".";
      }
      else
      {
        err("tr.sourceRefBys should take one integer argument, got %zu\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant handleIncludeDependencyGraph(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        return theTranslator->trInclDepGraph(args[0].toString());
      }
      else
      {
        err("tr.includeDependencyGraph should take one string argument, got %zu\n",args.size());
      }
      return TemplateVariant();
    }



    TemplateVariant generatedBy() const
    {
      return theTranslator->trGeneratedBy();
    }
    TemplateVariant generatedAt() const
    {
      return TemplateVariant(std::bind(&Private::handleGeneratedAt,this,std::placeholders::_1));
    }
    TemplateVariant inheritanceDiagramFor() const
    {
      return TemplateVariant(std::bind(&Private::handleInheritanceDiagramFor,this,std::placeholders::_1));
    }
    TemplateVariant collaborationDiagramFor() const
    {
      return TemplateVariant(std::bind(&Private::handleCollaborationDiagramFor,this,std::placeholders::_1));
    }
    TemplateVariant dirDependencyGraphFor() const
    {
      return TemplateVariant(std::bind(&Private::handleDirDependencyGraphFor,this,std::placeholders::_1));
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
      // TODO: VHDL: theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE)
      // TODO: Fortran: trDataTypes()
    }
    TemplateVariant concepts() const
    {
      return theTranslator->trConcept(true,false);
    }
    TemplateVariant conceptDefinition() const
    {
      return theTranslator->trConceptDefinition();
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
    TemplateVariant namespaceIndex() const
    {
      return theTranslator->trNamespaceIndex();
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
    TemplateVariant moduleIndex() const
    {
      return theTranslator->trModuleIndex();
    }
    TemplateVariant namespaces() const
    {
      if (m_javaOpt || m_vhdlOpt)
      {
        return theTranslator->trPackages();
      }
      else if (m_fortranOpt || m_sliceOpt)
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
    TemplateVariant fileIndex() const
    {
      return theTranslator->trFileIndex();
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
      else if (m_fortranOpt || m_sliceOpt)
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
      else if (m_fortranOpt || m_sliceOpt)
      {
        return theTranslator->trModulesMembers();
      }
      else
      {
        return theTranslator->trNamespaceMembers();
      }
    }
    TemplateVariant moduleDocumentation() const
    {
      return theTranslator->trModuleDocumentation();
    }
    TemplateVariant fileDocumentation() const
    {
      return theTranslator->trFileDocumentation();
    }
    TemplateVariant fileList() const
    {
      return theTranslator->trFileList();
    }
    TemplateVariant fileMembers() const
    {
      return theTranslator->trFileMembers();
    }
    TemplateVariant fileMembersDescription() const
    {
      static bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trFileMembersDescription(extractAll);
    }
    TemplateVariant namespaceMembersDescription() const
    {
      static bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trNamespaceMemberDescription(extractAll);
    }
    TemplateVariant classHierarchyDescription() const
    {
      return theTranslator->trClassHierarchyDescription();
    }
    TemplateVariant gotoGraphicalHierarchy() const
    {
      return theTranslator->trGotoGraphicalHierarchy();
    }
    TemplateVariant gotoTextualHierarchy() const
    {
      return theTranslator->trGotoTextualHierarchy();
    }
    TemplateVariant classMembersDescription() const
    {
      static bool extractAll = Config_getBool(EXTRACT_ALL);
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      if (fortranOpt)
      {
        return theTranslator->trCompoundMembersDescriptionFortran(extractAll);
      }
      else
      {
        return theTranslator->trCompoundMembersDescription(extractAll);
      }
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
      return TemplateVariant(std::bind(&Private::handleInheritsList,this,std::placeholders::_1));
    }
    TemplateVariant inheritedByList() const
    {
      return TemplateVariant(std::bind(&Private::handleInheritedByList,this,std::placeholders::_1));
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
      return TemplateVariant(std::bind(&Private::handleWriteList,this,std::placeholders::_1));
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
      return TemplateVariant(std::bind(&Private::handleImplementedBy,this,std::placeholders::_1));
    }
    TemplateVariant reimplementedBy() const
    {
      return TemplateVariant(std::bind(&Private::handleReimplementedBy,this,std::placeholders::_1));
    }
    TemplateVariant sourceRefs() const
    {
      return TemplateVariant(std::bind(&Private::handleSourceRefs,this,std::placeholders::_1));
    }
    TemplateVariant sourceRefBys() const
    {
      return TemplateVariant(std::bind(&Private::handleSourceRefBys,this,std::placeholders::_1));
    }
    TemplateVariant callGraph() const
    {
      return theTranslator->trCallGraph();
    }
    TemplateVariant callerGraph() const
    {
      return theTranslator->trCallerGraph();
    }
    TemplateVariant referencedByRelation() const
    {
      return theTranslator->trReferencedBy();
    }
    TemplateVariant referencesRelation() const
    {
      return theTranslator->trReferences();
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
      return TemplateVariant(std::bind(&Private::handleIncludeDependencyGraph,this,std::placeholders::_1));
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
    TemplateVariant namespaceDocumentation() const
    {
      return theTranslator->trNamespaceDocumentation();
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
      bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trFileListDescription(extractAll);
    }
    TemplateVariant modulesDescription() const
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trModulesListDescription(extractAll);
    }
    TemplateVariant namespaceListDescription() const
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trNamespaceListDescription(extractAll);
    }
    TemplateVariant conceptListDescription() const
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      return theTranslator->trConceptListDescription(extractAll);
    }
    TemplateVariant directories() const
    {
      return theTranslator->trDirectories();
    }
    TemplateVariant all() const
    {
      return theTranslator->trAll();
    }
    TemplateVariant functions() const
    {
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      return fortranOpt ? theTranslator->trSubprograms()     :
             vhdlOpt    ? theTranslator->trFunctionAndProc() :
                          theTranslator->trFunctions();
    }
    TemplateVariant variables() const
    {
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      return sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables();
    }
    TemplateVariant typedefs() const
    {
      return theTranslator->trTypedefs();
    }
    TemplateVariant enums() const
    {
      return theTranslator->trEnumerations();
    }
    TemplateVariant properties() const
    {
      return theTranslator->trProperties();
    }
    TemplateVariant events() const
    {
      return theTranslator->trEvents();
    }
    TemplateVariant related() const
    {
      return theTranslator->trRelatedFunctions();
    }
    TemplateVariant macros() const
    {
      return theTranslator->trDefines();
    }
    TemplateVariant loading() const
    {
      return theTranslator->trLoading();
    }
    TemplateVariant searching() const
    {
      return theTranslator->trSearching();
    }
    TemplateVariant noMatches() const
    {
      return theTranslator->trNoMatches();
    }
    TemplateVariant enumName() const
    {
      return theTranslator->trEnumName();
    }
    TemplateVariant enumValue() const
    {
      return theTranslator->trEnumValue();
    }
    TemplateVariant referenceManual() const
    {
      return theTranslator->trReferenceManual();
    }
    TemplateVariant index() const
    {
      return theTranslator->trRTFGeneralIndex();
    }
    TemplateVariant panelSyncOn() const
    {
      return theTranslator->trPanelSynchronisationTooltip(FALSE);
    }
    TemplateVariant panelSyncOff() const
    {
      return theTranslator->trPanelSynchronisationTooltip(TRUE);
    }
    TemplateVariant providedByCategory() const
    {
      return theTranslator->trProvidedByCategory();
    }
    TemplateVariant extendsClass() const
    {
      return theTranslator->trExtendsClass();
    }
    TemplateVariant examplesDescription() const
    {
      return theTranslator->trExamplesDescription();
    }
    TemplateVariant langString() const
    {
      return HtmlHelp::getLanguageString();
    }
    TemplateVariant code() const
    {
      return theTranslator->trCode();
    }
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        //%% string generatedBy
        s_inst.addProperty("generatedBy",       &Private::generatedBy);
        //%% string generatedAt
        s_inst.addProperty("generatedAt",       &Private::generatedAt);
        //%% string search
        s_inst.addProperty("search",            &Private::search);
        //%% string mainPage
        s_inst.addProperty("mainPage",          &Private::mainPage);
        //%% string classes
        s_inst.addProperty("classes",           &Private::classes);
        //%% string classList
        s_inst.addProperty("classList",         &Private::classList);
        //%% string classListDescription
        s_inst.addProperty("classListDescription", &Private::classListDescription);
        //%% string classIndex
        s_inst.addProperty("classIndex",        &Private::classIndex);
        //%% string concepts
        s_inst.addProperty("concepts",          &Private::concepts);
        //%% string conceptDefintion
        s_inst.addProperty("conceptDefinition", &Private::conceptDefinition);
        //%% string namespaceIndex
        s_inst.addProperty("namespaceIndex",    &Private::namespaceIndex);
        //%% string classHierarchy
        s_inst.addProperty("classHierarchy",    &Private::classHierarchy);
        //%% string classMembers
        s_inst.addProperty("classMembers",      &Private::classMembers);
        //%% string classMembersDescription
        s_inst.addProperty("classMembersDescription",&Private::classMembersDescription);
        //%% string modules
        s_inst.addProperty("modules",           &Private::modules);
        //%% string moduleIndex
        s_inst.addProperty("moduleIndex",       &Private::moduleIndex);
        //%% string namespaces
        s_inst.addProperty("namespaces",        &Private::namespaces);
        //%% string fileIndex
        s_inst.addProperty("fileIndex",         &Private::fileIndex);
        //%% string files
        s_inst.addProperty("files",             &Private::files);
        //%% string pages
        s_inst.addProperty("pages",             &Private::pages);
        //%% string examples
        s_inst.addProperty("examples",          &Private::examples);
        //%% string namespaceList
        s_inst.addProperty("namespaceList",     &Private::namespaceList);
        //%% string namespaceMembers
        s_inst.addProperty("namespaceMembers",  &Private::namespaceMembers);
        //%% string fileList
        s_inst.addProperty("fileList",          &Private::fileList);
        //%% string fileMembers
        s_inst.addProperty("fileMembers",       &Private::fileMembers);
        //%% string fileMembersDescription
        s_inst.addProperty("fileMembersDescription", &Private::fileMembersDescription);
        //%% string relatedPagesDescription
        s_inst.addProperty("relatedPagesDesc",  &Private::relatedPagesDesc);
        //%% string more
        s_inst.addProperty("more",              &Private::more);
        //%% string detailedDescription
        s_inst.addProperty("detailedDesc",      &Private::detailedDesc);
        //%% string inheritanceDiagramFor
        s_inst.addProperty("inheritanceDiagramFor", &Private::inheritanceDiagramFor);
        //%% string collaborationDiagramFor
        s_inst.addProperty("collaborationDiagramFor", &Private::collaborationDiagramFor);
        //%% markerstring inheritsList
        s_inst.addProperty("inheritsList",      &Private::inheritsList);
        //%% markerstring inheritedByList
        s_inst.addProperty("inheritedByList",   &Private::inheritedByList);
        //%% markerstring definedAtLineInSourceFile
        s_inst.addProperty("definedAtLineInSourceFile", &Private::definedAtLineInSourceFile);
        //%% string typeConstraints
        s_inst.addProperty("typeConstraints",   &Private::typeConstraints);
        //%% string exampleList
        s_inst.addProperty("exampleList",       &Private::exampleList);
        //%% string listOfAllMembers
        s_inst.addProperty("listOfAllMembers",  &Private::listOfAllMembers);
        //%% string memberList
        s_inst.addProperty("memberList",        &Private::memberList);
        //%% string theListOfAllMembers
        s_inst.addProperty("theListOfAllMembers",&Private::theListOfAllMembers);
        //%% string incInheritedMembers
        s_inst.addProperty("incInheritedMembers",&Private::incInheritedMembers);
        //%% string defineValue
        s_inst.addProperty("defineValue",        &Private::defineValue);
        //%% string initialValue
        s_inst.addProperty("initialValue",       &Private::initialValue);
        //%% markerstring implements
        s_inst.addProperty("implements",         &Private::implements);
        //%% markerstring reimplements
        s_inst.addProperty("reimplements",       &Private::reimplements);
        //%% markerstring implementedBy
        s_inst.addProperty("implementedBy",      &Private::implementedBy);
        //%% markerstring reimplementedBy
        s_inst.addProperty("reimplementedBy",    &Private::reimplementedBy);
        //%% markerstring sourceRefs
        s_inst.addProperty("sourceRefs",         &Private::sourceRefs);
        //%% markerstring sourceRefBys
        s_inst.addProperty("sourceRefBys",       &Private::sourceRefBys);
        //%% string callGraph
        s_inst.addProperty("callGraph",          &Private::callGraph);
        //%% string callerGraph
        s_inst.addProperty("callerGraph",        &Private::callerGraph);
        //%% string referencedByRelation
        s_inst.addProperty("referencedByRelation", &Private::referencedByRelation);
        //%% string referencesRelation
        s_inst.addProperty("referencesRelation",   &Private::referencesRelation);
        //%% markerstring inheritedFrom
        s_inst.addProperty("inheritedFrom",      &Private::inheritedFrom);
        //%% string additionalInheritedMembers
        s_inst.addProperty("additionalInheritedMembers",&Private::additionalInheritedMembers);
        //%% string includeDependencyGraph:container_name
        s_inst.addProperty("includeDependencyGraph",&Private::includeDependencyGraph);
        //%% string includedByDependencyGraph
        s_inst.addProperty("includedByDependencyGraph",&Private::includedByDependencyGraph);
        //%% string gotoSourceCode
        s_inst.addProperty("gotoSourceCode",     &Private::gotoSourceCode);
        //%% string gotoDocumentation
        s_inst.addProperty("gotoDocumentation",  &Private::gotoDocumentation);
        //%% string constantgroups
        s_inst.addProperty("constantgroups",     &Private::constantgroups);
        //%% string classDocumentation
        s_inst.addProperty("classDocumentation", &Private::classDocumentation);
        //%% string namespaceDocumentation
        s_inst.addProperty("namespaceDocumentation", &Private::namespaceDocumentation);
        //%% string moduleDocumentation
        s_inst.addProperty("moduleDocumentation",&Private::moduleDocumentation);
        //%% string fileDocumentation
        s_inst.addProperty("fileDocumentation",  &Private::fileDocumentation);
        //%% string compoundMembers
        s_inst.addProperty("compoundMembers",    &Private::compoundMembers);
        //%% string detailLevel
        s_inst.addProperty("detailLevel",        &Private::detailLevel);
        //%% string fileListDescription
        s_inst.addProperty("fileListDescription",&Private::fileListDescription);
        //%% string namespaceListDescription
        s_inst.addProperty("namespaceListDescription",&Private::namespaceListDescription);
        //%% string conceptListDescription
        s_inst.addProperty("conceptListDescription",&Private::conceptListDescription);
        //%% string directories
        s_inst.addProperty("directories",        &Private::directories);
        //%% string moduleDescription
        s_inst.addProperty("modulesDescription", &Private::modulesDescription);
        //%% string all
        s_inst.addProperty("all",                &Private::all);
        //%% string functions
        s_inst.addProperty("functions",          &Private::functions);
        //%% string variables
        s_inst.addProperty("variables",          &Private::variables);
        //%% string typedefs
        s_inst.addProperty("typedefs",           &Private::typedefs);
        //%% string enums
        s_inst.addProperty("enums",              &Private::enums);
        //%% string enumValues
        s_inst.addProperty("enumValues",         &Private::enumerationValues);
        //%% string properties
        s_inst.addProperty("properties",         &Private::properties);
        //%% string events
        s_inst.addProperty("events",             &Private::events);
        //%% string related
        s_inst.addProperty("related",            &Private::related);
        //%% string macros
        s_inst.addProperty("macros",             &Private::macros);
        //%% string namespaceMembersDescription
        s_inst.addProperty("namespaceMembersDescription",&Private::namespaceMembersDescription);
        //%% string classHierarchyDescription
        s_inst.addProperty("classHierarchyDescription",&Private::classHierarchyDescription);
        //%% string gotoGraphicalHierarchy
        s_inst.addProperty("gotoGraphicalHierarchy",&Private::gotoGraphicalHierarchy);
        //%% string gotoTextualHierarchy
        s_inst.addProperty("gotoTextualHierarchy",&Private::gotoTextualHierarchy);
        //%% string loading
        s_inst.addProperty("loading",            &Private::loading);
        //%% string searching
        s_inst.addProperty("searching",          &Private::searching);
        //%% string noMatches
        s_inst.addProperty("noMatches",          &Private::noMatches);
        //%% string enumValue
        s_inst.addProperty("enumValue",          &Private::enumValue);
        //%% string enumName
        s_inst.addProperty("enumName",           &Private::enumName);
        //%% string referenceManual
        s_inst.addProperty("referenceManual",    &Private::referenceManual);
        //%% string index
        s_inst.addProperty("index",              &Private::index);
        //%% string panelSyncOn
        s_inst.addProperty("panelSyncOn",        &Private::panelSyncOn);
        //%% string panelSyncOff
        s_inst.addProperty("panelSyncOff",       &Private::panelSyncOff);
        //%% string dirDependencyGraph
        s_inst.addProperty("dirDependencyGraphFor", &Private::dirDependencyGraphFor);
        //%% string providedByCategory
        s_inst.addProperty("providedByCategory", &Private::providedByCategory);
        //%% string extendsClass
        s_inst.addProperty("extendsClass",       &Private::extendsClass);
        //%% string examplesDescription
        s_inst.addProperty("examplesDescription",&Private::examplesDescription);
        //%% string langstring
        s_inst.addProperty("langString",         &Private::langString);
        //%% string code
        s_inst.addProperty("code",               &Private::code);

        init=TRUE;
      }

      m_javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
      m_fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      m_vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      m_sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
  private:
    bool m_javaOpt;
    bool m_fortranOpt;
    bool m_vhdlOpt;
    bool m_sliceOpt;
    static PropertyMapper<TranslateContext::Private> s_inst;
};
//%% }

PropertyMapper<TranslateContext::Private> TranslateContext::Private::s_inst;

TranslateContext::TranslateContext() : p(std::make_unique<Private>())
{
}

TranslateContext::~TranslateContext()
{
}

TemplateVariant TranslateContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector TranslateContext::fields() const
{
  return p->fields();
}

static TemplateVariant parseDoc(const Definition *def,const QCString &file,int line,
                                const QCString &relPath,const QCString &docStr,bool isBrief)
{
  TemplateVariant result;
  std::unique_ptr<IDocParser> parser { createDocParser() };
  std::unique_ptr<DocRoot>    root   { validatingParseDoc(
                                       *parser.get(),file,line,def,0,docStr,TRUE,FALSE,
                                       QCString(),isBrief,FALSE,Config_getBool(MARKDOWN_SUPPORT))
                                     };
  TextStream ts;
  switch (g_globals.outputFormat)
  {
    case ContextOutputFormat_Html:
      {
        HtmlCodeGenerator codeGen(ts,relPath);
        HtmlDocVisitor visitor(ts,codeGen,def);
        root->accept(&visitor);
      }
      break;
    case ContextOutputFormat_Latex:
      {
        LatexCodeGenerator codeGen(ts,relPath,file);
        LatexDocVisitor visitor(ts,codeGen,def->getDefFileExtension(),FALSE);
        root->accept(&visitor);
      }
      break;
    // TODO: support other generators
    default:
      err("context.cpp: output format not yet supported\n");
      break;
  }
  bool isEmpty = root->isEmpty();
  if (isEmpty)
    result = "";
  else
    result = TemplateVariant(ts.str().c_str(),TRUE);
  return result;
}

static TemplateVariant parseCode(const Definition *d,const QCString &scopeName,const QCString &relPath,
                                 const QCString &code,int startLine=-1,int endLine=-1,bool showLineNumbers=FALSE)
{
  auto intf = Doxygen::parserManager->getCodeParser(d->getDefFileExtension());
  intf->resetCodeParserState();
  TextStream t;
  switch (g_globals.outputFormat)
  {
    case ContextOutputFormat_Html:
      {
        HtmlCodeGenerator codeGen(t,relPath);
        intf->parseCode(codeGen,scopeName,code,d->getLanguage(),FALSE,QCString(),d->getBodyDef(),
            startLine,endLine,TRUE,toMemberDef(d),showLineNumbers,d);
      }
      break;
    case ContextOutputFormat_Latex:
      {
        LatexCodeGenerator codeGen(t,relPath,d->docFile());
        intf->parseCode(codeGen,scopeName,code,d->getLanguage(),FALSE,QCString(),d->getBodyDef(),
            startLine,endLine,TRUE,toMemberDef(d),showLineNumbers,d);
      }
      break;
    // TODO: support other generators
    default:
      err("context.cpp: output format not yet supported\n");
      break;
  }
  return TemplateVariant(t.str(),TRUE);
}

static TemplateVariant parseCode(const FileDef *fd,const QCString &relPath)
{
  static bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  auto intf = Doxygen::parserManager->getCodeParser(fd->getDefFileExtension());
  intf->resetCodeParserState();
  TextStream t;
  switch (g_globals.outputFormat)
  {
    case ContextOutputFormat_Html:
      {
        HtmlCodeGenerator codeGen(t,relPath);
        intf->parseCode(codeGen,QCString(),
              fileToString(fd->absFilePath(),filterSourceFiles,TRUE), // the sources
              fd->getLanguage(),  // lang
              FALSE,              // isExampleBlock
              QCString(),         // exampleName
              const_cast<FileDef*>(fd),  // fileDef, TODO: should be const
              -1,                 // startLine
              -1,                 // endLine
              FALSE,              // inlineFragment
              0,                  // memberDef
              TRUE,               // showLineNumbers
              0,                  // searchCtx
              TRUE                // collectXRefs, TODO: should become FALSE
              );
      }
      break;
    case ContextOutputFormat_Latex:
      {
        LatexCodeGenerator codeGen(t,relPath,fd->docFile());
        intf->parseCode(codeGen,QCString(),
              fileToString(fd->absFilePath(),filterSourceFiles,TRUE), // the sources
              fd->getLanguage(),  // lang
              FALSE,              // isExampleBlock
              QCString(),         // exampleName
              const_cast<FileDef*>(fd),  // fileDef, TODO: should be const
              -1,                 // startLine
              -1,                 // endLine
              FALSE,              // inlineFragment
              0,                  // memberDef
              TRUE,               // showLineNumbers
              0,                  // searchCtx
              TRUE                // collectXRefs, TODO: should become FALSE
              );
      }
      break;
    // TODO: support other generators
    default:
      err("context.cpp: output format not yet supported\n");
      break;
  }
  return TemplateVariant(t.str().c_str(),TRUE);
}

//------------------------------------------------------------------------

//%% struct Symbol: shared info for all symbols
//%% {
template<typename T>
class DefinitionContext
{
  public:
    DefinitionContext(const Definition *d) : m_def(d)
    {
      assert(d!=0);
    }
    virtual ~DefinitionContext() {}
    void addBaseProperties(PropertyMapper<T> &inst)
    {
      //%% string name: the name of the symbol
      inst.addProperty("name",&DefinitionContext::name);
      //%% string bareName: the bare name of the symbol with scope info
      inst.addProperty("bareName",&DefinitionContext::bareName);
      //%% string relPath: the relative path to the root of the output (CREATE_SUBDIRS)
      inst.addProperty("relPath",&DefinitionContext::relPath);
      //%% string fileName: the file name of the output file associated with the symbol (without extension)
      inst.addProperty("fileName",&DefinitionContext::fileName);
      //%% string anchor: anchor within the page
      inst.addProperty("anchor",&DefinitionContext::anchor);
      //%% string details: the detailed documentation for this symbol
      inst.addProperty("details",&DefinitionContext::details);
      //%% string brief: the brief description for this symbol
      inst.addProperty("brief",&DefinitionContext::brief);
      //%% string inbodyDocs: the documentation found in the body
      inst.addProperty("inbodyDocs",&DefinitionContext::inbodyDocs);
      //%% string sourceFileName: the file name of the source file (without extension)
      inst.addProperty("sourceFileName",&DefinitionContext::sourceFileName);
      //%% bool isLinkable: can the symbol be linked to?
      inst.addProperty("isLinkable",&DefinitionContext::isLinkable);
      //%% bool isLinkableInProject: can the symbol be linked within this project?
      inst.addProperty("isLinkableInProject",&DefinitionContext::isLinkableInProject);
      //%% int dynSectionId: identifier that can be used for collapsible sections
      inst.addProperty("dynSectionId",&DefinitionContext::dynSectionId);
      //%% string language: the programming language in which the symbol is written
      inst.addProperty("language",&DefinitionContext::language);
      //%% string sourceDef: A link to the source definition
      inst.addProperty("sourceDef",&DefinitionContext::sourceDef);
      //%% list[Definition] navigationPath: Breadcrumb navigation path to this item
      inst.addProperty("navigationPath",&DefinitionContext::navigationPath);
      //%% string kind: Kind of compound object: class, namespace, module, package, page, dir
      inst.addProperty("compoundKind",&DefinitionContext::compoundKind);
      //%% bool isReference: is this definition imported via a tag file
      inst.addProperty("isReference",&DefinitionContext::isReference);
      //%% string externalReference: the link to the element in the remote documentation
      inst.addProperty("externalReference",&DefinitionContext::externalReference);
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
      static bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
      return createSubdirs ? QCString("../../") : QCString("");
    }
    virtual TemplateVariant relPath() const
    {
      return relPathAsString();
    }
    TemplateVariant details() const
    {
      Cachable &cache = getCache();
      if (!cache.details || g_globals.outputFormat!=cache.detailsOutputFormat)
      {
        cache.details.reset(new TemplateVariant(parseDoc(m_def,m_def->docFile(),m_def->docLine(),
                                            relPathAsString(),m_def->documentation(),FALSE)));
        cache.detailsOutputFormat = g_globals.outputFormat;
      }
      return *cache.details;
    }
    TemplateVariant brief() const
    {
      Cachable &cache = getCache();
      if (!cache.brief || g_globals.outputFormat!=cache.briefOutputFormat)
      {
        if (m_def->hasBriefDescription())
        {
          cache.brief.reset(new TemplateVariant(parseDoc(m_def,m_def->briefFile(),m_def->briefLine(),
                             relPathAsString(),m_def->briefDescription(),TRUE)));
          cache.briefOutputFormat = g_globals.outputFormat;
        }
        else
        {
          cache.brief.reset(new TemplateVariant(""));
        }
      }
      return *cache.brief;
    }
    TemplateVariant inbodyDocs() const
    {
      Cachable &cache = getCache();
      if (!cache.inbodyDocs || g_globals.outputFormat!=cache.inbodyDocsOutputFormat)
      {
        if (!m_def->inbodyDocumentation().isEmpty())
        {
          cache.inbodyDocs.reset(new TemplateVariant(parseDoc(m_def,m_def->inbodyFile(),m_def->inbodyLine(),
                                           relPathAsString(),m_def->inbodyDocumentation(),FALSE)));
          cache.inbodyDocsOutputFormat = g_globals.outputFormat;
        }
        else
        {
          cache.inbodyDocs.reset(new TemplateVariant(""));
        }
      }
      return *cache.inbodyDocs;
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
        case SrcLangExt_SQL:      result="sql";      break;
        case SrcLangExt_Markdown: result="markdown"; break;
        case SrcLangExt_Slice:    result="slice";    break;
        case SrcLangExt_Lex:      result="lex";      break;
      }
      return result;
    }
    TemplateVariant compoundKind() const
    {
      QCString result = "unspecified";
      switch (m_def->definitionType())
      {
        case Definition::TypeClass:     result="class";     break;
        case Definition::TypeFile:      result="file";      break;
        case Definition::TypeNamespace: result="namespace"; break;
        case Definition::TypeGroup:     result="module";    break;
        case Definition::TypePackage:   result="package";   break;
        case Definition::TypePage:      result="page";      break;
        case Definition::TypeDir:       result="dir";       break;
        case Definition::TypeConcept:   result="concept";   break;
        case Definition::TypeMember:                        break;
      }
      return result;
    }
    TemplateVariant sourceDef() const
    {
      Cachable &cache = getCache();
      if (cache.sourceDef->count()==2)
      {
        return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.sourceDef));
      }
      else
      {
        return FALSE;
      }
    }
    void fillPath(const Definition *def,TemplateListPtr list) const
    {
      const Definition *outerScope = def->getOuterScope();
      Definition::DefType type = def->definitionType();
      if (outerScope && outerScope!=Doxygen::globalScope)
      {
        fillPath(outerScope,list);
      }
      else if (type==Definition::TypeFile && (toFileDef(def))->getDirDef())
      {
        fillPath((toFileDef(def))->getDirDef(),list);
      }
      list->append(NavPathElemContext::alloc(def));
    }
    TemplateVariant navigationPath() const
    {
      Cachable &cache = getCache();
      if (!cache.navPath)
      {
        TemplateListPtr list = TemplateList::alloc();
        if (m_def->getOuterScope() && m_def->getOuterScope()!=Doxygen::globalScope)
        {
          fillPath(m_def,list);
        }
        else if (m_def->definitionType()==Definition::TypeFile && (toFileDef(m_def))->getDirDef())
        {
          fillPath((toFileDef(m_def))->getDirDef(),list);
        }
        cache.navPath = list;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.navPath));
    }
    TemplateVariant isReference() const
    {
      return m_def->isReference();
    }
    TemplateVariant externalReference() const
    {
      return m_def->externalReference(relPathAsString());
    }

  protected:
    struct Cachable : public Definition::Cookie
    {
      Cachable(const Definition *def) : detailsOutputFormat(ContextOutputFormat_Unspecified),
                                  briefOutputFormat(ContextOutputFormat_Unspecified),
                                  inbodyDocsOutputFormat(ContextOutputFormat_Unspecified)
      {
        sourceDef = TemplateList::alloc();
        lineLink = TemplateStruct::alloc();
        fileLink = TemplateStruct::alloc();

        if (def && !def->getSourceFileBase().isEmpty())
        {
          lineLink->set("text",def->getStartBodyLine());
          lineLink->set("isLinkable",TRUE);
          lineLink->set("fileName",def->getSourceFileBase());
          lineLink->set("anchor",def->getSourceAnchor());
          lineLink->set("isReference",FALSE);
          lineLink->set("externalReference","");
          if (def->definitionType()==Definition::TypeFile)
          {
            fileLink->set("text",def->name());
          }
          else if (def->getBodyDef())
          {
            fileLink->set("text",def->getBodyDef()->name());
          }
          else
          {
            fileLink->set("text",def->displayName(TRUE));
          }
          fileLink->set("isLinkable",TRUE);
          fileLink->set("fileName",def->getSourceFileBase());
          fileLink->set("anchor",QCString());
          fileLink->set("isReference",FALSE);
          fileLink->set("externalReference","");
          sourceDef->append(std::static_pointer_cast<TemplateStructIntf>(lineLink));
          sourceDef->append(std::static_pointer_cast<TemplateStructIntf>(fileLink));
        }
      }
      std::unique_ptr<TemplateVariant> details;
      ContextOutputFormat        detailsOutputFormat;
      std::unique_ptr<TemplateVariant> brief;
      ContextOutputFormat        briefOutputFormat;
      std::unique_ptr<TemplateVariant> inbodyDocs;
      ContextOutputFormat        inbodyDocsOutputFormat;
      TemplateListPtr            navPath;
      TemplateListPtr            sourceDef;
      TemplateStructPtr          fileLink;
      TemplateStructPtr          lineLink;
    };

  private:
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_def->cookie());
      assert(c!=0);
      return *c;
    }
    const Definition *m_def;
};
//%% }

//------------------------------------------------------------------------

//%% struct IncludeInfo: include file information
//%% {
class IncludeInfoContext::Private
{
  public:
    Private(const IncludeInfo *info,SrcLangExt lang) :
      m_info(info),
      m_lang(lang)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("file",&Private::file);
        s_inst.addProperty("name",&Private::name);
        s_inst.addProperty("isImport",&Private::isImport);
        s_inst.addProperty("isLocal",&Private::isLocal);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant isLocal() const
    {
      bool isIDLorJava = m_lang==SrcLangExt_IDL || m_lang==SrcLangExt_Java;
      return m_info->local || isIDLorJava;
    }
    TemplateVariant isImport() const
    {
      return m_info->imported || m_lang==SrcLangExt_ObjC;
    }
    TemplateVariant file() const
    {
      if (!m_fileContext && m_info && m_info->fileDef)
      {
        m_fileContext = FileContext::alloc(m_info->fileDef);
      }
      if (m_fileContext)
      {
        return m_fileContext;
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
    mutable TemplateStructIntfPtr m_fileContext;
    SrcLangExt m_lang;
    static PropertyMapper<IncludeInfoContext::Private> s_inst;
};

PropertyMapper<IncludeInfoContext::Private> IncludeInfoContext::Private::s_inst;

IncludeInfoContext::IncludeInfoContext(const IncludeInfo *info,SrcLangExt lang) : p(std::make_unique<Private>(info,lang))
{
}

IncludeInfoContext::~IncludeInfoContext()
{
}

TemplateVariant IncludeInfoContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector IncludeInfoContext::fields() const
{
  return p->fields();
}

//%% }

//------------------------------------------------------------------------

//%% list IncludeInfoList[Class] : list of nested classes
class IncludeInfoListContext::Private : public GenericNodeListContext
{
  public:
    Private(const IncludeInfoList &list,SrcLangExt lang)
    {
      for (const auto &ii : list)
      {
        append(IncludeInfoContext::alloc(&ii,lang));
      }
    }
};

IncludeInfoListContext::IncludeInfoListContext(const IncludeInfoList &list,SrcLangExt lang) : p(std::make_unique<Private>(list,lang))
{
}

IncludeInfoListContext::~IncludeInfoListContext()
{
}

// TemplateListIntf
uint IncludeInfoListContext::count() const
{
  return p->count();
}

TemplateVariant IncludeInfoListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr IncludeInfoListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct Class(Symbol): class information
//%% {
class ClassContext::Private : public DefinitionContext<ClassContext::Private>
{
  public:
    Private(const ClassDef *cd) : DefinitionContext<ClassContext::Private>(cd),
       m_classDef(cd)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",                     &Private::title);
        s_inst.addProperty("highlight",                 &Private::highlight);
        s_inst.addProperty("subhighlight",              &Private::subHighlight);
        s_inst.addProperty("hasDetails",                &Private::hasDetails);
        s_inst.addProperty("generatedFromFiles",        &Private::generatedFromFiles);
        s_inst.addProperty("usedFiles",                 &Private::usedFiles);
        s_inst.addProperty("hasInheritanceDiagram",     &Private::hasInheritanceDiagram);
        s_inst.addProperty("inheritanceDiagram",        &Private::inheritanceDiagram);
        s_inst.addProperty("hasCollaborationDiagram",   &Private::hasCollaborationDiagram);
        s_inst.addProperty("collaborationDiagram",      &Private::collaborationDiagram);
        s_inst.addProperty("includeInfo",               &Private::includeInfo);
        s_inst.addProperty("inherits",                  &Private::inherits);
        s_inst.addProperty("inheritedBy",               &Private::inheritedBy);
        s_inst.addProperty("unoIDLServices",            &Private::unoIDLServices);
        s_inst.addProperty("unoIDLInterfaces",          &Private::unoIDLInterfaces);
        s_inst.addProperty("signals",                   &Private::signals);
        s_inst.addProperty("publicTypes",               &Private::publicTypes);
        s_inst.addProperty("publicMethods",             &Private::publicMethods);
        s_inst.addProperty("publicStaticMethods",       &Private::publicStaticMethods);
        s_inst.addProperty("publicAttributes",          &Private::publicAttributes);
        s_inst.addProperty("publicStaticAttributes",    &Private::publicStaticAttributes);
        s_inst.addProperty("publicSlots",               &Private::publicSlots);
        s_inst.addProperty("protectedTypes",            &Private::protectedTypes);
        s_inst.addProperty("protectedMethods",          &Private::protectedMethods);
        s_inst.addProperty("protectedStaticMethods",    &Private::protectedStaticMethods);
        s_inst.addProperty("protectedAttributes",       &Private::protectedAttributes);
        s_inst.addProperty("protectedStaticAttributes", &Private::protectedStaticAttributes);
        s_inst.addProperty("protectedSlots",            &Private::protectedSlots);
        s_inst.addProperty("privateTypes",              &Private::privateTypes);
        s_inst.addProperty("privateMethods",            &Private::privateMethods);
        s_inst.addProperty("privateStaticMethods",      &Private::privateStaticMethods);
        s_inst.addProperty("privateAttributes",         &Private::privateAttributes);
        s_inst.addProperty("privateStaticAttributes",   &Private::privateStaticAttributes);
        s_inst.addProperty("privateSlots",              &Private::privateSlots);
        s_inst.addProperty("packageTypes",              &Private::packageTypes);
        s_inst.addProperty("packageMethods",            &Private::packageMethods);
        s_inst.addProperty("packageStaticMethods",      &Private::packageStaticMethods);
        s_inst.addProperty("packageAttributes",         &Private::packageAttributes);
        s_inst.addProperty("packageStaticAttributes",   &Private::packageStaticAttributes);
        s_inst.addProperty("properties",                &Private::properties);
        s_inst.addProperty("events",                    &Private::events);
        s_inst.addProperty("friends",                   &Private::friends);
        s_inst.addProperty("related",                   &Private::related);
        s_inst.addProperty("detailedTypedefs",          &Private::detailedTypedefs);
        s_inst.addProperty("detailedEnums",             &Private::detailedEnums);
        s_inst.addProperty("detailedServices",          &Private::detailedServices);
        s_inst.addProperty("detailedInterfaces",        &Private::detailedInterfaces);
        s_inst.addProperty("detailedConstructors",      &Private::detailedConstructors);
        s_inst.addProperty("detailedMethods",           &Private::detailedMethods);
        s_inst.addProperty("detailedRelated",           &Private::detailedRelated);
        s_inst.addProperty("detailedVariables",         &Private::detailedVariables);
        s_inst.addProperty("detailedProperties",        &Private::detailedProperties);
        s_inst.addProperty("detailedEvents",            &Private::detailedEvents);
        s_inst.addProperty("classes",                   &Private::classes);
        s_inst.addProperty("innerClasses",              &Private::innerClasses);
        s_inst.addProperty("compoundType",              &Private::compoundType);
        s_inst.addProperty("templateDecls",             &Private::templateDecls);
        s_inst.addProperty("typeConstraints",           &Private::typeConstraints);
        s_inst.addProperty("examples",                  &Private::examples);
        s_inst.addProperty("members",                   &Private::members);
        s_inst.addProperty("allMembersList",            &Private::allMembersList);
        s_inst.addProperty("allMembersFileName",        &Private::allMembersFileName);
        s_inst.addProperty("memberGroups",              &Private::memberGroups);
        s_inst.addProperty("additionalInheritedMembers",&Private::additionalInheritedMembers);
        s_inst.addProperty("isSimple",                  &Private::isSimple);
        s_inst.addProperty("categoryOf",                &Private::categoryOf);
        init=TRUE;
      }
      if (!cd->cookie()) { cd->setCookie(new ClassContext::Private::Cachable(cd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
      Cachable &cache = getCache();
      if (!cache.usedFiles)
      {
        cache.usedFiles = UsedFilesContext::alloc(m_classDef);
      }
      return cache.usedFiles;
    }
    DotClassGraph *getClassGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.classGraph)
      {
        cache.classGraph.reset(new DotClassGraph(m_classDef,Inheritance));
      }
      return cache.classGraph.get();
    }
    int numInheritanceNodes() const
    {
      Cachable &cache = getCache();
      if (cache.inheritanceNodes==-1)
      {
        cache.inheritanceNodes=m_classDef->countInheritanceNodes();
      }
      return cache.inheritanceNodes>0;
    }
    TemplateVariant hasInheritanceDiagram() const
    {
      bool result=FALSE;
      static bool haveDot    = Config_getBool(HAVE_DOT);
      static auto classGraph = Config_getEnum(CLASS_GRAPH);
      bool classGraphEnabled = classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH;

      if (haveDot && classGraphEnabled)
      {
        DotClassGraph *cg = getClassGraph();
        result = !cg->isTrivial() && !cg->isTooBig();
      }
      else if (classGraphEnabled)
      {
        result = numInheritanceNodes()>0;
      }
      return result;
    }
    TemplateVariant inheritanceDiagram() const
    {
      TextStream t;
      static bool haveDot    = Config_getBool(HAVE_DOT);
      static auto classGraph = Config_getEnum(CLASS_GRAPH);
      bool classGraphEnabled = classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH;

      if (haveDot && classGraphEnabled)
      {
        DotClassGraph *cg = getClassGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                             g_globals.outputDir,
                             g_globals.outputDir+Portable::pathSeparator()+m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                             relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
                            );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                             g_globals.outputDir,
                             g_globals.outputDir+Portable::pathSeparator()+m_classDef->getOutputFileBase()+".tex",
                             relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
                            );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      else if (classGraphEnabled)
      {
        ClassDiagram d(m_classDef);
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              TextStream tt;

              QCString name = convertToHtml(m_classDef->displayName());
              d.writeImage(tt,g_globals.outputDir,
                           relPathAsString(),
                           m_classDef->getOutputFileBase());
              if (!tt.empty())
              {
                t << "<div class=\"center\">\n";
                t << "  <img src=\"";
                t << relPathAsString() << m_classDef->getOutputFileBase();
                t << ".png\" usemap=\"#" << convertToId(name) << "_map\" alt=\"\"/>\n";
                t << "  <map id=\"" << convertToId(name) << "_map\" name=\"" << convertToId(name) << "_map\">\n";
	        t << tt.str();
	        t << "  </map>\n";
                t << "</div>";
              }
              else
              {
                t << "<div class=\"center\">\n";
                t << "  <img src=\"";
                t << relPathAsString() << m_classDef->getOutputFileBase();
                t << ".png\" alt=\"\"/>\n";
                t << "</div>";
              }
            }
            break;
          case ContextOutputFormat_Latex:
            {
              d.writeFigure(t,g_globals.outputDir,m_classDef->getOutputFileBase());
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }
    DotClassGraph *getCollaborationGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.collaborationGraph)
      {
        cache.collaborationGraph.reset(new DotClassGraph(m_classDef,Collaboration));
      }
      return cache.collaborationGraph.get();
    }
    TemplateVariant hasCollaborationDiagram() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      return haveDot && !getCollaborationGraph()->isTrivial();
    }
    TemplateVariant collaborationDiagram() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      TextStream t;
      if (haveDot)
      {
        DotClassGraph *cg = getCollaborationGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                             g_globals.outputDir,
                             g_globals.outputDir+Portable::pathSeparator()+m_classDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                             relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
                            );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                             g_globals.outputDir,
                             g_globals.outputDir+Portable::pathSeparator()+m_classDef->getOutputFileBase()+".tex",
                             relPathAsString(),TRUE,TRUE,g_globals.dynSectionId
                            );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }

    TemplateVariant includeInfo() const
    {
      Cachable &cache = getCache();
      if (!cache.includeInfo && m_classDef->includeInfo())
      {
        cache.includeInfo = IncludeInfoContext::alloc(m_classDef->includeInfo(),m_classDef->getLanguage());
      }
      if (cache.includeInfo)
      {
        return cache.includeInfo;
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant inherits() const
    {
      Cachable &cache = getCache();
      if (!cache.inheritsList)
      {
        cache.inheritsList = InheritanceListContext::alloc(m_classDef->baseClasses(),TRUE);
      }
      return cache.inheritsList;
    }
    TemplateVariant inheritedBy() const
    {
      Cachable &cache = getCache();
      if (!cache.inheritedByList)
      {
        cache.inheritedByList = InheritanceListContext::alloc(m_classDef->subClasses(),FALSE);
      }
      return cache.inheritedByList;
    }
    TemplateVariant getMemberList(TemplateStructIntfPtr &list,
                                  MemberListType type,const QCString &title,bool=FALSE) const
    {
      if (!list)
      {
        const MemberList *ml = m_classDef->getMemberList(type);
        if (ml)
        {
          list = MemberListInfoContext::alloc(m_classDef,relPathAsString(),ml,title,"");
        }
      }
      if (list)
      {
        return list;
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant unoIDLServices() const
    {
      return getMemberList(getCache().unoIDLServices,MemberListType_services,theTranslator->trServices());
    }
    TemplateVariant unoIDLInterfaces() const
    {
      return getMemberList(getCache().unoIDLInterfaces,MemberListType_interfaces,theTranslator->trInterfaces());
    }
    TemplateVariant signals() const
    {
      return getMemberList(getCache().signals,MemberListType_signals,theTranslator->trSignals());
    }
    TemplateVariant publicTypes() const
    {
      return getMemberList(getCache().publicTypes,MemberListType_pubTypes,theTranslator->trPublicTypes());
    }
    TemplateVariant publicMethods() const
    {
      return getMemberList(getCache().publicMethods,MemberListType_pubMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trInstanceMethods()
                                                     : theTranslator->trPublicMembers());
    }
    TemplateVariant publicStaticMethods() const
    {
      return getMemberList(getCache().publicStaticMethods,MemberListType_pubStaticMethods,
          m_classDef->getLanguage()==SrcLangExt_ObjC ? theTranslator->trClassMethods()
                                                     : theTranslator->trStaticPublicMembers());
    }
    TemplateVariant publicAttributes() const
    {
      return getMemberList(getCache().publicAttributes,MemberListType_pubAttribs,theTranslator->trPublicAttribs());
    }
    TemplateVariant publicStaticAttributes() const
    {
      return getMemberList(getCache().publicStaticAttributes,MemberListType_pubStaticAttribs,theTranslator->trStaticPublicAttribs());
    }
    TemplateVariant publicSlots() const
    {
      return getMemberList(getCache().publicSlots,MemberListType_pubSlots,theTranslator->trPublicSlots());
    }
    TemplateVariant protectedTypes() const
    {
      return getMemberList(getCache().protectedTypes,MemberListType_proTypes,theTranslator->trProtectedTypes());
    }
    TemplateVariant protectedMethods() const
    {
      return getMemberList(getCache().protectedMethods,MemberListType_proMethods,theTranslator->trProtectedMembers());
    }
    TemplateVariant protectedStaticMethods() const
    {
      return getMemberList(getCache().protectedStaticMethods,MemberListType_proStaticMethods,theTranslator->trStaticProtectedMembers());
    }
    TemplateVariant protectedAttributes() const
    {
      return getMemberList(getCache().protectedAttributes,MemberListType_proAttribs,theTranslator->trProtectedAttribs());
    }
    TemplateVariant protectedStaticAttributes() const
    {
      return getMemberList(getCache().protectedStaticAttributes,MemberListType_proStaticAttribs,theTranslator->trStaticProtectedAttribs());
    }
    TemplateVariant protectedSlots() const
    {
      return getMemberList(getCache().protectedSlots,MemberListType_proSlots,theTranslator->trProtectedSlots());
    }
    TemplateVariant privateTypes() const
    {
      return getMemberList(getCache().privateTypes,MemberListType_priTypes,theTranslator->trPrivateTypes());
    }
    TemplateVariant privateSlots() const
    {
      return getMemberList(getCache().privateSlots,MemberListType_priSlots,theTranslator->trPrivateSlots());
    }
    TemplateVariant privateMethods() const
    {
      return getMemberList(getCache().privateMethods,MemberListType_priMethods,theTranslator->trPrivateMembers());
    }
    TemplateVariant privateStaticMethods() const
    {
      return getMemberList(getCache().privateStaticMethods,MemberListType_priStaticMethods,theTranslator->trStaticPrivateMembers());
    }
    TemplateVariant privateAttributes() const
    {
      return getMemberList(getCache().privateAttributes,MemberListType_priAttribs,theTranslator->trPrivateAttribs());
    }
    TemplateVariant privateStaticAttributes() const
    {
      return getMemberList(getCache().privateStaticAttributes,MemberListType_priStaticAttribs,theTranslator->trStaticPrivateAttribs());
    }
    TemplateVariant packageTypes() const
    {
      return getMemberList(getCache().packageTypes,MemberListType_pacTypes,theTranslator->trPackageTypes());
    }
    TemplateVariant packageMethods() const
    {
      return getMemberList(getCache().packageMethods,MemberListType_pacMethods,theTranslator->trPackageMembers());
    }
    TemplateVariant packageStaticMethods() const
    {
      return getMemberList(getCache().packageStaticMethods,MemberListType_pacStaticMethods,theTranslator->trStaticPackageMembers());
    }
    TemplateVariant packageAttributes() const
    {
      return getMemberList(getCache().packageAttributes,MemberListType_pacAttribs,theTranslator->trPackageAttribs());
    }
    TemplateVariant packageStaticAttributes() const
    {
      return getMemberList(getCache().packageStaticAttributes,MemberListType_pacStaticAttribs,theTranslator->trStaticPackageAttribs());
    }
    TemplateVariant properties() const
    {
      return getMemberList(getCache().properties,MemberListType_properties,theTranslator->trProperties());
    }
    TemplateVariant events() const
    {
      return getMemberList(getCache().events,MemberListType_events,theTranslator->trEvents());
    }
    TemplateVariant friends() const
    {
      return getMemberList(getCache().friends,MemberListType_friends,theTranslator->trFriends());
    }
    TemplateVariant related() const
    {
      return getMemberList(getCache().related,MemberListType_related,theTranslator->trRelatedFunctions());
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(getCache().detailedTypedefs,MemberListType_typedefMembers,theTranslator->trMemberTypedefDocumentation(),TRUE);
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(getCache().detailedEnums,MemberListType_enumMembers,theTranslator->trMemberEnumerationDocumentation(),TRUE);
    }
    TemplateVariant detailedServices() const
    {
      return getMemberList(getCache().detailedServices,MemberListType_serviceMembers,theTranslator->trServices(),TRUE);
    }
    TemplateVariant detailedInterfaces() const
    {
      return getMemberList(getCache().detailedInterfaces,MemberListType_interfaceMembers,theTranslator->trInterfaces(),TRUE);
    }
    TemplateVariant detailedConstructors() const
    {
      return getMemberList(getCache().detailedConstructors,MemberListType_constructors,theTranslator->trConstructorDocumentation(),TRUE);
    }
    TemplateVariant detailedMethods() const
    {
      return getMemberList(getCache().detailedMethods,MemberListType_functionMembers,theTranslator->trMemberFunctionDocumentation(),TRUE);
    }
    TemplateVariant detailedRelated() const
    {
      return getMemberList(getCache().detailedRelated,MemberListType_relatedMembers,theTranslator->trRelatedFunctionDocumentation(),TRUE);
    }
    TemplateVariant detailedVariables() const
    {
      return getMemberList(getCache().detailedVariables,MemberListType_variableMembers,theTranslator->trMemberDataDocumentation(),TRUE);
    }
    TemplateVariant detailedProperties() const
    {
      return getMemberList(getCache().detailedProperties,MemberListType_propertyMembers,theTranslator->trPropertyDocumentation(),TRUE);
    }
    TemplateVariant detailedEvents() const
    {
      return getMemberList(getCache().detailedEvents,MemberListType_eventMembers,theTranslator->trEventDocumentation(),TRUE);
    }
    TemplateVariant classes() const
    {
      Cachable &cache = getCache();
      if (!cache.classes)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_classDef->getClasses())
        {
          if (cd->visibleInParentsDeclList())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.classes = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.classes));
    }
    TemplateVariant innerClasses() const
    {
      Cachable &cache = getCache();
      if (!cache.innerClasses)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_classDef->getClasses())
        {
          if (!cd->isAnonymous() &&
              cd->isLinkableInProject() &&
              cd->isEmbeddedInOuterScope() &&
              cd->partOfGroups().empty()
             )
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.innerClasses = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.innerClasses));
    }
    TemplateVariant compoundType() const
    {
      return m_classDef->compoundTypeString();
    }
    void addTemplateDecls(const Definition *d,TemplateListPtr tl) const
    {
      if (d->definitionType()==Definition::TypeClass)
      {
        const Definition *parent = d->getOuterScope();
        if (parent)
        {
          addTemplateDecls(parent,tl);
        }
        const ClassDef *cd=toClassDef(d);
        if (!cd->templateArguments().empty())
        {
          TemplateListIntfPtr al = ArgumentListContext::alloc(cd->templateArguments(),cd,relPathAsString());
          // since a TemplateVariant does take ownership of the object, we add it
          // a separate list just to be able to delete it and avoid a memory leak
          tl->append(al);
        }
      }
    }
    void addExamples(TemplateListPtr list) const
    {
      if (m_classDef->hasExamples())
      {
        for (const auto &ex : m_classDef->getExamples())
        {
          TemplateStructPtr s = TemplateStruct::alloc();
          s->set("text",ex.name);
          s->set("isLinkable",TRUE);
          s->set("anchor",ex.anchor);
          s->set("fileName",ex.file);
          s->set("isReference",FALSE);
          s->set("externalReference","");
          list->append(std::static_pointer_cast<TemplateStructIntf>(s));
        }
      }
    }
    TemplateVariant templateDecls() const
    {
      Cachable &cache = getCache();
      if (!cache.templateDecls)
      {
        TemplateListPtr tl = TemplateList::alloc();
        addTemplateDecls(m_classDef,tl);
        cache.templateDecls = tl;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.templateDecls));
    }
    TemplateVariant typeConstraints() const
    {
      if (!m_classDef->typeConstraints().empty())
      {
        Cachable &cache = getCache();
        if (!cache.typeConstraints && !m_classDef->typeConstraints().empty())
        {
          cache.typeConstraints = ArgumentListContext::alloc(m_classDef->typeConstraints(),m_classDef,relPathAsString());
        }
        return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.typeConstraints));
      }
      return FALSE;
    }
    TemplateVariant examples() const
    {
      Cachable &cache = getCache();
      if (!cache.examples)
      {
        TemplateListPtr exampleList = TemplateList::alloc();
        addExamples(exampleList);
        cache.examples = exampleList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.examples));
    }
    void addMembers(const ClassDef *cd,MemberListType lt) const
    {
      const MemberList *ml = cd->getMemberList(lt);
      if (ml)
      {
        Cachable &cache = getCache();
        for (const auto &md : *ml)
        {
          if (md->isBriefSectionVisible())
          {
            cache.allMembers.push_back(md);
          }
        }
      }
    }
    TemplateVariant members() const
    {
      Cachable &cache = getCache();
      if (!cache.members)
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
        cache.members = MemberListContext::alloc(&cache.allMembers);
      }
      return TemplateVariant(cache.members);
    }
    TemplateVariant allMembersList() const
    {
      Cachable &cache = getCache();
      if (!cache.allMembersList)
      {
        TemplateListIntfPtr ml = AllMembersListContext::alloc(m_classDef->memberNameInfoLinkedMap());
        cache.allMembersList = ml;
      }
      return cache.allMembersList;
    }
    TemplateVariant allMembersFileName() const
    {
      return m_classDef->getMemberListFileName();
    }
    TemplateVariant memberGroups() const
    {
      Cachable &cache = getCache();
      if (!cache.memberGroups)
      {
        if (!m_classDef->getMemberGroups().empty())
        {
          cache.memberGroups = MemberGroupListContext::alloc(m_classDef,relPathAsString(),m_classDef->getMemberGroups(),m_classDef->subGrouping());
        }
        else
        {
          cache.memberGroups = MemberGroupListContext::alloc();
        }
      }
      return cache.memberGroups;
    }
    TemplateVariant additionalInheritedMembers() const
    {
      Cachable &cache = getCache();
      if (!cache.additionalInheritedMembers)
      {
        cache.additionalInheritedMembers = InheritedMemberInfoListContext::alloc();
        auto ctx = std::dynamic_pointer_cast<InheritedMemberInfoListContext>(cache.additionalInheritedMembers);
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
      }
      return cache.additionalInheritedMembers;
    }
    TemplateVariant isSimple() const
    {
      return m_classDef->isSimple();
    }
    TemplateVariant categoryOf() const
    {
      Cachable &cache = getCache();
      if (!cache.categoryOf && m_classDef->categoryOf())
      {
        cache.categoryOf = ClassContext::alloc(m_classDef->categoryOf());
      }
      if (cache.categoryOf)
      {
        return cache.categoryOf;
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }

  private:
    const ClassDef *m_classDef;
    struct Cachable : public DefinitionContext<ClassContext::Private>::Cachable
    {
      Cachable(const ClassDef *cd) : DefinitionContext<ClassContext::Private>::Cachable(cd),
                               inheritanceNodes(-1),
                               allMembers(MemberListType_allMembersList,MemberListContainer::Class) { }
      TemplateStructIntfPtr                   includeInfo;
      TemplateListIntfPtr                     inheritsList;
      TemplateListIntfPtr                     inheritedByList;
      std::unique_ptr<DotClassGraph>          classGraph;
      std::unique_ptr<DotClassGraph>          collaborationGraph;
      TemplateListPtr                         classes;
      TemplateListPtr                         innerClasses;
      TemplateStructIntfPtr                   publicTypes;
      TemplateStructIntfPtr                   publicMethods;
      TemplateStructIntfPtr                   publicStaticMethods;
      TemplateStructIntfPtr                   publicAttributes;
      TemplateStructIntfPtr                   publicStaticAttributes;
      TemplateStructIntfPtr                   publicSlots;
      TemplateStructIntfPtr                   protectedTypes;
      TemplateStructIntfPtr                   protectedMethods;
      TemplateStructIntfPtr                   protectedStaticMethods;
      TemplateStructIntfPtr                   protectedAttributes;
      TemplateStructIntfPtr                   protectedStaticAttributes;
      TemplateStructIntfPtr                   protectedSlots;
      TemplateStructIntfPtr                   privateTypes;
      TemplateStructIntfPtr                   privateMethods;
      TemplateStructIntfPtr                   privateStaticMethods;
      TemplateStructIntfPtr                   privateAttributes;
      TemplateStructIntfPtr                   privateStaticAttributes;
      TemplateStructIntfPtr                   privateSlots;
      TemplateStructIntfPtr                   packageTypes;
      TemplateStructIntfPtr                   packageMethods;
      TemplateStructIntfPtr                   packageStaticMethods;
      TemplateStructIntfPtr                   packageAttributes;
      TemplateStructIntfPtr                   packageStaticAttributes;
      TemplateStructIntfPtr                   unoIDLServices;
      TemplateStructIntfPtr                   unoIDLInterfaces;
      TemplateStructIntfPtr                   signals;
      TemplateStructIntfPtr                   properties;
      TemplateStructIntfPtr                   events;
      TemplateStructIntfPtr                   friends;
      TemplateStructIntfPtr                   related;
      TemplateStructIntfPtr                   detailedTypedefs;
      TemplateStructIntfPtr                   detailedEnums;
      TemplateStructIntfPtr                   detailedServices;
      TemplateStructIntfPtr                   detailedInterfaces;
      TemplateStructIntfPtr                   detailedConstructors;
      TemplateStructIntfPtr                   detailedMethods;
      TemplateStructIntfPtr                   detailedRelated;
      TemplateStructIntfPtr                   detailedVariables;
      TemplateStructIntfPtr                   detailedProperties;
      TemplateStructIntfPtr                   detailedEvents;
      TemplateListIntfPtr                     memberGroups;
      TemplateListIntfPtr                     allMembersList;
      TemplateListIntfPtr                     typeConstraints;
      TemplateListPtr                         examples;
      TemplateListPtr                         templateDecls;
      TemplateListIntfPtr                     additionalInheritedMembers;
      TemplateListIntfPtr                     members;
      TemplateListIntfPtr                     usedFiles;
      TemplateListPtr                         exampleList;
      TemplateStructIntfPtr                   categoryOf;
      int                                     inheritanceNodes;
      MemberList                              allMembers;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_classDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<ClassContext::Private> s_inst;
};
//%% }

PropertyMapper<ClassContext::Private> ClassContext::Private::s_inst;

ClassContext::ClassContext(const ClassDef *cd) : p(std::make_unique<Private>(cd))
{
}

ClassContext::~ClassContext()
{
}

TemplateVariant ClassContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector ClassContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Namespace(Symbol): namespace information
//%% {
class NamespaceContext::Private : public DefinitionContext<NamespaceContext::Private>
{
  public:
    Private(const NamespaceDef *nd) : DefinitionContext<NamespaceContext::Private>(nd),
                                m_namespaceDef(nd)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",                &Private::title);
        s_inst.addProperty("highlight",            &Private::highlight);
        s_inst.addProperty("subhighlight",         &Private::subHighlight);
        s_inst.addProperty("compoundType",         &Private::compoundType);
        s_inst.addProperty("hasDetails",           &Private::hasDetails);
        s_inst.addProperty("classes",              &Private::classes);
        //s_inst.addProperty("interfaces",           &Private::interfaces);
        s_inst.addProperty("namespaces",           &Private::namespaces);
        s_inst.addProperty("constantgroups",       &Private::constantgroups);
        s_inst.addProperty("typedefs",             &Private::typedefs);
        s_inst.addProperty("sequences",            &Private::sequences);
        s_inst.addProperty("dictionaries",         &Private::dictionaries);
        s_inst.addProperty("enums",                &Private::enums);
        s_inst.addProperty("functions",            &Private::functions);
        s_inst.addProperty("variables",            &Private::variables);
        s_inst.addProperty("memberGroups",         &Private::memberGroups);
        s_inst.addProperty("detailedTypedefs",     &Private::detailedTypedefs);
        s_inst.addProperty("detailedSequences",    &Private::detailedSequences);
        s_inst.addProperty("detailedDictionaries", &Private::detailedDictionaries);
        s_inst.addProperty("detailedEnums",        &Private::detailedEnums);
        s_inst.addProperty("detailedFunctions",    &Private::detailedFunctions);
        s_inst.addProperty("detailedVariables",    &Private::detailedVariables);
        s_inst.addProperty("inlineClasses",        &Private::inlineClasses);
        init=TRUE;
      }
      if (!nd->cookie()) { nd->setCookie(new NamespaceContext::Private::Cachable(nd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
    TemplateVariant classes() const
    {
      Cachable &cache = getCache();
      if (!cache.classes)
      {
        static bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_namespaceDef->getClasses())
        {
          if (sliceOpt && (cd->compoundType()==ClassDef::Struct    ||
                           cd->compoundType()==ClassDef::Interface ||
                           cd->compoundType()==ClassDef::Exception))
          {
            continue; // These types appear in their own sections.
          }
          if (cd->visibleInParentsDeclList())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.classes = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.classes));
    }
    TemplateVariant namespaces() const
    {
      Cachable &cache = getCache();
      if (!cache.namespaces)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_namespaceDef->getNamespaces())
        {
          if (nd->isLinkable() && !nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.namespaces = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.namespaces));
    }
    TemplateVariant constantgroups() const
    {
      Cachable &cache = getCache();
      if (!cache.constantgroups)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_namespaceDef->getNamespaces())
        {
          if (nd->isLinkable() && nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.constantgroups = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.constantgroups));
    }
    TemplateVariant getMemberList(TemplateStructIntfPtr &list,
                                  MemberListType type,const QCString &title,bool=FALSE) const
    {
      if (!list)
      {
        const MemberList *ml = m_namespaceDef->getMemberList(type);
        if (ml)
        {
          list = MemberListInfoContext::alloc(m_namespaceDef,relPathAsString(),ml,title,"");
        }
      }
      if (list)
      {
        return TemplateVariant(list);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant typedefs() const
    {
      return getMemberList(getCache().typedefs,MemberListType_decTypedefMembers,theTranslator->trTypedefs());
    }
    TemplateVariant sequences() const
    {
      return getMemberList(getCache().sequences,MemberListType_decSequenceMembers,theTranslator->trSequences());
    }
    TemplateVariant dictionaries() const
    {
      return getMemberList(getCache().dictionaries,MemberListType_decDictionaryMembers,theTranslator->trDictionaries());
    }
    TemplateVariant enums() const
    {
      return getMemberList(getCache().enums,MemberListType_decEnumMembers,theTranslator->trEnumerations());
    }
    TemplateVariant functions() const
    {
      QCString title = theTranslator->trFunctions();
      SrcLangExt lang = m_namespaceDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprograms();
      else if (lang==SrcLangExt_VHDL) title=theTranslator->trFunctionAndProc();
      return getMemberList(getCache().functions,MemberListType_decFuncMembers,title);
    }
    TemplateVariant variables() const
    {
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      return getMemberList(getCache().variables,MemberListType_decVarMembers,
                           sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables());
    }
    TemplateVariant memberGroups() const
    {
      Cachable &cache = getCache();
      if (!cache.memberGroups)
      {
        if (!m_namespaceDef->getMemberGroups().empty())
        {
          cache.memberGroups = MemberGroupListContext::alloc(m_namespaceDef,relPathAsString(),m_namespaceDef->getMemberGroups(),m_namespaceDef->subGrouping());
        }
        else
        {
          cache.memberGroups = MemberGroupListContext::alloc();
        }
      }
      return cache.memberGroups;
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(getCache().detailedTypedefs,MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation());
    }
    TemplateVariant detailedSequences() const
    {
      return getMemberList(getCache().detailedSequences,MemberListType_docSequenceMembers,theTranslator->trSequenceDocumentation());
    }
    TemplateVariant detailedDictionaries() const
    {
      return getMemberList(getCache().detailedDictionaries,MemberListType_docDictionaryMembers,theTranslator->trDictionaryDocumentation());
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(getCache().detailedEnums,MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation());
    }
    TemplateVariant detailedFunctions() const
    {
      QCString title = theTranslator->trFunctionDocumentation();
      SrcLangExt lang = m_namespaceDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprogramDocumentation();
      return getMemberList(getCache().detailedFunctions,MemberListType_docFuncMembers,title);
    }
    TemplateVariant detailedVariables() const
    {
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      return getMemberList(getCache().detailedVariables,MemberListType_docVarMembers,
                           sliceOpt ? theTranslator->trConstantDocumentation() :
                           theTranslator->trVariableDocumentation());
    }
    TemplateVariant inlineClasses() const
    {
      Cachable &cache = getCache();
      if (!cache.inlineClasses)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_namespaceDef->getClasses())
        {
          if (!cd->isAnonymous() &&
              cd->isLinkableInProject() &&
              cd->isEmbeddedInOuterScope() &&
              cd->partOfGroups().empty())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.inlineClasses = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.inlineClasses));
    }
  private:
    const NamespaceDef *m_namespaceDef;
    struct Cachable : public DefinitionContext<NamespaceContext::Private>::Cachable
    {
      Cachable(const NamespaceDef *nd) : DefinitionContext<NamespaceContext::Private>::Cachable(nd) {}
      TemplateListPtr                             classes;
      TemplateListPtr                             interfaces;
      TemplateListPtr                             namespaces;
      TemplateListPtr                             constantgroups;
      TemplateStructIntfPtr                       typedefs;
      TemplateStructIntfPtr                       sequences;
      TemplateStructIntfPtr                       dictionaries;
      TemplateStructIntfPtr                       enums;
      TemplateStructIntfPtr                       functions;
      TemplateStructIntfPtr                       variables;
      TemplateListIntfPtr                         memberGroups;
      TemplateStructIntfPtr                       detailedTypedefs;
      TemplateStructIntfPtr                       detailedSequences;
      TemplateStructIntfPtr                       detailedDictionaries;
      TemplateStructIntfPtr                       detailedEnums;
      TemplateStructIntfPtr                       detailedFunctions;
      TemplateStructIntfPtr                       detailedVariables;
      TemplateListPtr                             inlineClasses;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_namespaceDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<NamespaceContext::Private> s_inst;
};
//%% }

PropertyMapper<NamespaceContext::Private> NamespaceContext::Private::s_inst;

NamespaceContext::NamespaceContext(const NamespaceDef *nd) : p(std::make_unique<Private>(nd))
{
}

NamespaceContext::~NamespaceContext()
{
}

TemplateVariant NamespaceContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector NamespaceContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct File(Symbol): file information
//%% {
class FileContext::Private : public DefinitionContext<FileContext::Private>
{
  public:
    Private(const FileDef *fd) : DefinitionContext<FileContext::Private>(fd) , m_fileDef(fd)
    {
      if (fd==0) abort();
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",                     &Private::title);
        s_inst.addProperty("highlight",                 &Private::highlight);
        s_inst.addProperty("subhighlight",              &Private::subHighlight);
        s_inst.addProperty("versionInfo",               &Private::versionInfo);
        s_inst.addProperty("includeList",               &Private::includeList);
        s_inst.addProperty("hasIncludeGraph",           &Private::hasIncludeGraph);
        s_inst.addProperty("hasIncludedByGraph",        &Private::hasIncludedByGraph);
        s_inst.addProperty("includeGraph",              &Private::includeGraph);
        s_inst.addProperty("includedByGraph",           &Private::includedByGraph);
        s_inst.addProperty("hasDetails",                &Private::hasDetails);
        s_inst.addProperty("hasSourceFile",             &Private::hasSourceFile);
        s_inst.addProperty("sources",                   &Private::sources);
        s_inst.addProperty("version",                   &Private::version);
        s_inst.addProperty("classes",                   &Private::classes);
        s_inst.addProperty("namespaces",                &Private::namespaces);
        s_inst.addProperty("constantgroups",            &Private::constantgroups);
        s_inst.addProperty("macros",                    &Private::macros);
        s_inst.addProperty("typedefs",                  &Private::typedefs);
        s_inst.addProperty("sequences",                 &Private::sequences);
        s_inst.addProperty("dictionaries",              &Private::dictionaries);
        s_inst.addProperty("enums",                     &Private::enums);
        s_inst.addProperty("functions",                 &Private::functions);
        s_inst.addProperty("variables",                 &Private::variables);
        s_inst.addProperty("memberGroups",              &Private::memberGroups);
        s_inst.addProperty("detailedMacros",            &Private::detailedMacros);
        s_inst.addProperty("detailedTypedefs",          &Private::detailedTypedefs);
        s_inst.addProperty("detailedSequences",         &Private::detailedSequences);
        s_inst.addProperty("detailedDictionaries",      &Private::detailedDictionaries);
        s_inst.addProperty("detailedEnums",             &Private::detailedEnums);
        s_inst.addProperty("detailedFunctions",         &Private::detailedFunctions);
        s_inst.addProperty("detailedVariables",         &Private::detailedVariables);
        s_inst.addProperty("inlineClasses",             &Private::inlineClasses);
        s_inst.addProperty("compoundType",              &Private::compoundType);
        init=TRUE;
      }
      if (!fd->cookie()) { fd->setCookie(new FileContext::Private::Cachable(fd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
      Cachable &cache = getCache();
      if (!cache.includeInfoList && !m_fileDef->includeFileList().empty())
      {
        cache.includeInfoList = IncludeInfoListContext::alloc(
              m_fileDef->includeFileList(),m_fileDef->getLanguage());
      }
      if (cache.includeInfoList)
      {
        return TemplateVariant(cache.includeInfoList);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    DotInclDepGraph *getIncludeGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.includeGraph)
      {
        cache.includeGraph.reset(new DotInclDepGraph(m_fileDef,FALSE));
      }
      return cache.includeGraph.get();
    }
    TemplateVariant hasIncludeGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      DotInclDepGraph *incGraph = getIncludeGraph();
      return (haveDot && !incGraph->isTooBig() && !incGraph->isTrivial());
    }
    TemplateVariant includeGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      TextStream t;
      if (haveDot)
      {
        DotInclDepGraph *cg = getIncludeGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_fileDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_fileDef->getOutputFileBase()+".tex",
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }
    DotInclDepGraph *getIncludedByGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.includedByGraph)
      {
        cache.includedByGraph.reset(new DotInclDepGraph(m_fileDef,TRUE));
      }
      return cache.includedByGraph.get();
    }
    TemplateVariant hasIncludedByGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      DotInclDepGraph *incGraph = getIncludedByGraph();
      return (haveDot && !incGraph->isTooBig() && !incGraph->isTrivial());
    }
    TemplateVariant includedByGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      TextStream t;
      if (haveDot)
      {
        DotInclDepGraph *cg = getIncludedByGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_fileDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_fileDef->getOutputFileBase()+".tex",
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
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
      Cachable &cache = getCache();
      if (!cache.sources)
      {
        if (m_fileDef->generateSourceFile())
        {
          cache.sources.reset(new TemplateVariant(parseCode(m_fileDef,relPathAsString())));
        }
        else
        {
          cache.sources.reset(new TemplateVariant(""));
        }
      }
      return *cache.sources;
    }
    TemplateVariant version() const
    {
      return m_fileDef->fileVersion();
    }
    TemplateVariant classes() const
    {
      Cachable &cache = getCache();
      if (!cache.classes)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_fileDef->getClasses())
        {
          if (cd->visibleInParentsDeclList())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.classes = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.classes));
    }
    TemplateVariant namespaces() const
    {
      Cachable &cache = getCache();
      if (!cache.namespaces)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_fileDef->getNamespaces())
        {
          if (nd->isLinkable() && !nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.namespaces = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.namespaces));
    }
    TemplateVariant constantgroups() const
    {
      Cachable &cache = getCache();
      if (!cache.constantgroups)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_fileDef->getNamespaces())
        {
          if (nd->isLinkable() && nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.constantgroups = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.constantgroups));
    }
    TemplateVariant getMemberList(TemplateStructIntfPtr &list,
                                  MemberListType type,const QCString &title,bool=FALSE) const
    {
      if (!list)
      {
        const MemberList *ml = m_fileDef->getMemberList(type);
        if (ml)
        {
          list = MemberListInfoContext::alloc(m_fileDef,relPathAsString(),ml,title,"");
        }
      }
      if (list)
      {
        return TemplateVariant(list);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant macros() const
    {
      return getMemberList(getCache().macros,MemberListType_decDefineMembers,theTranslator->trDefines());
    }
    TemplateVariant typedefs() const
    {
      return getMemberList(getCache().typedefs,MemberListType_decTypedefMembers,theTranslator->trTypedefs());
    }
    TemplateVariant sequences() const
    {
      return getMemberList(getCache().sequences,MemberListType_decSequenceMembers,theTranslator->trSequences());
    }
    TemplateVariant dictionaries() const
    {
      return getMemberList(getCache().dictionaries,MemberListType_decDictionaryMembers,theTranslator->trDictionaries());
    }
    TemplateVariant enums() const
    {
      return getMemberList(getCache().enums,MemberListType_decEnumMembers,theTranslator->trEnumerations());
    }
    TemplateVariant functions() const
    {
      QCString title = theTranslator->trFunctions();
      SrcLangExt lang = m_fileDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprograms();
      else if (lang==SrcLangExt_VHDL) title=theTranslator->trFunctionAndProc();
      return getMemberList(getCache().functions,MemberListType_decFuncMembers,title);
    }
    TemplateVariant variables() const
    {
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      return getMemberList(getCache().variables,MemberListType_decVarMembers,
                           sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables());
    }
    TemplateVariant memberGroups() const
    {
      Cachable &cache = getCache();
      if (!cache.memberGroups)
      {
        if (!m_fileDef->getMemberGroups().empty())
        {
          cache.memberGroups = MemberGroupListContext::alloc(m_fileDef,relPathAsString(),m_fileDef->getMemberGroups(),m_fileDef->subGrouping());
        }
        else
        {
          cache.memberGroups = MemberGroupListContext::alloc();
        }
      }
      return TemplateVariant(cache.memberGroups);
    }
    TemplateVariant detailedMacros() const
    {
      return getMemberList(getCache().detailedMacros,MemberListType_docDefineMembers,theTranslator->trDefineDocumentation());
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(getCache().detailedTypedefs,MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation());
    }
    TemplateVariant detailedSequences() const
    {
      return getMemberList(getCache().detailedSequences,MemberListType_docSequenceMembers,theTranslator->trSequenceDocumentation());
    }
    TemplateVariant detailedDictionaries() const
    {
      return getMemberList(getCache().detailedDictionaries,MemberListType_docDictionaryMembers,theTranslator->trDictionaryDocumentation());
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(getCache().detailedEnums,MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation());
    }
    TemplateVariant detailedFunctions() const
    {
      QCString title = theTranslator->trFunctionDocumentation();
      SrcLangExt lang = m_fileDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprogramDocumentation();
      return getMemberList(getCache().detailedFunctions,MemberListType_docFuncMembers,title);
    }
    TemplateVariant detailedVariables() const
    {
      return getMemberList(getCache().detailedVariables,MemberListType_docVarMembers,theTranslator->trVariableDocumentation());
    }
    TemplateVariant inlineClasses() const
    {
      Cachable &cache = getCache();
      if (!cache.inlineClasses)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_fileDef->getClasses())
        {
          if (!cd->isAnonymous() &&
              cd->isLinkableInProject() &&
              cd->isEmbeddedInOuterScope() &&
              cd->partOfGroups().empty())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.inlineClasses = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.inlineClasses));
    }
    TemplateVariant compoundType() const
    {
      return theTranslator->trFile(FALSE,TRUE);
    }

  private:
    const FileDef *m_fileDef;
    struct Cachable : public DefinitionContext<FileContext::Private>::Cachable
    {
      Cachable(const FileDef *fd) : DefinitionContext<FileContext::Private>::Cachable(fd) {}
      TemplateListIntfPtr                   includeInfoList;
      std::unique_ptr<DotInclDepGraph>      includeGraph;
      std::unique_ptr<DotInclDepGraph>      includedByGraph;
      std::unique_ptr<TemplateVariant>      sources;
      TemplateListPtr                       classes;
      TemplateListPtr                       namespaces;
      TemplateListPtr                       constantgroups;
      TemplateStructIntfPtr                 macros;
      TemplateStructIntfPtr                 typedefs;
      TemplateStructIntfPtr                 sequences;
      TemplateStructIntfPtr                 dictionaries;
      TemplateStructIntfPtr                 enums;
      TemplateStructIntfPtr                 functions;
      TemplateStructIntfPtr                 variables;
      TemplateListIntfPtr                   memberGroups;
      TemplateStructIntfPtr                 detailedMacros;
      TemplateStructIntfPtr                 detailedTypedefs;
      TemplateStructIntfPtr                 detailedSequences;
      TemplateStructIntfPtr                 detailedDictionaries;
      TemplateStructIntfPtr                 detailedEnums;
      TemplateStructIntfPtr                 detailedFunctions;
      TemplateStructIntfPtr                 detailedVariables;
      TemplateListPtr                       inlineClasses;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_fileDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<FileContext::Private> s_inst;
};
//%% }

PropertyMapper<FileContext::Private> FileContext::Private::s_inst;

FileContext::FileContext(const FileDef *fd) : p(std::make_unique<Private>(fd))
{
}

FileContext::~FileContext()
{
}

TemplateVariant FileContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector FileContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Dir(Symbol): directory information
//%% {
class DirContext::Private : public DefinitionContext<DirContext::Private>
{
  public:
    Private(const DirDef *dd) : DefinitionContext<DirContext::Private>(dd) , m_dirDef(dd)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subHighlight);
        s_inst.addProperty("dirName",       &Private::dirName);
        s_inst.addProperty("dirs",          &Private::dirs);
        s_inst.addProperty("files",         &Private::files);
        s_inst.addProperty("hasDetails",    &Private::hasDetails);
        s_inst.addProperty("hasDirGraph",   &Private::hasDirGraph);
        s_inst.addProperty("dirGraph",      &Private::dirGraph);
        s_inst.addProperty("compoundType",  &Private::compoundType);
        init=TRUE;
      }
      if (!dd->cookie()) { dd->setCookie(new DirContext::Private::Cachable(dd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
      Cachable &cache = getCache();
      if (!cache.dirs)
      {
        cache.dirs = TemplateList::alloc();
        for(const auto dd : m_dirDef->subDirs())
        {
          cache.dirs->append(DirContext::alloc(dd));
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.dirs));
    }
    TemplateVariant files() const
    {
      Cachable &cache = getCache();
      if (!cache.files)
      {
        cache.files = TemplateList::alloc();
        for (const auto &fd : m_dirDef->getFiles())
        {
          cache.files->append(FileContext::alloc(fd));
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.files));
    }
    TemplateVariant hasDetails() const
    {
      return m_dirDef->hasDetailedDescription();
    }
    TemplateVariant compoundType() const
    {
      return theTranslator->trDir(FALSE,TRUE);
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    DotDirDeps *getDirDepsGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.dirDepsGraph)
      {
        cache.dirDepsGraph.reset(new DotDirDeps(m_dirDef));
      }
      return cache.dirDepsGraph.get();
    }
    TemplateVariant hasDirGraph() const
    {
      bool result=FALSE;
      static bool haveDot  = Config_getBool(HAVE_DOT);
      static bool dirGraph = Config_getBool(DIRECTORY_GRAPH);
      if (haveDot && dirGraph)
      {
        DotDirDeps *graph = getDirDepsGraph();
        result = !graph->isTrivial();
      }
      return result;
    }
    TemplateVariant dirGraph() const
    {
      TextStream t;
      static bool haveDot  = Config_getBool(HAVE_DOT);
      static bool dirGraph = Config_getBool(DIRECTORY_GRAPH);
      if (haveDot && dirGraph)
      {
        DotDirDeps *graph = getDirDepsGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              graph->writeGraph(t,GOF_BITMAP,
                                EOF_Html,
                                g_globals.outputDir,
                                g_globals.outputDir+Portable::pathSeparator()+m_dirDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                                relPathAsString(),
                                TRUE,
                                g_globals.dynSectionId,
                                FALSE);
            }
            break;
          case ContextOutputFormat_Latex:
            {
              graph->writeGraph(t,GOF_EPS,
                                EOF_LaTeX,
                                g_globals.outputDir,
                                g_globals.outputDir+Portable::pathSeparator()+m_dirDef->getOutputFileBase()+".tex",
                                relPathAsString(),
                                TRUE,
                                g_globals.dynSectionId,
                                FALSE);
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }

  private:
    const DirDef *m_dirDef;
    struct Cachable : public DefinitionContext<DirContext::Private>::Cachable
    {
      Cachable(const DirDef *dd) : DefinitionContext<DirContext::Private>::Cachable(dd) {}
      TemplateListPtr                dirs;
      TemplateListPtr                files;
      std::unique_ptr<DotDirDeps>    dirDepsGraph;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_dirDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<DirContext::Private> s_inst;
};
//%% }

PropertyMapper<DirContext::Private> DirContext::Private::s_inst;

DirContext::DirContext(const DirDef *fd) : p(std::make_unique<Private>(fd))
{
}

DirContext::~DirContext()
{
}

TemplateVariant DirContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector DirContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Page(Symbol): page information
//%% {
class PageContext::Private : public DefinitionContext<PageContext::Private>
{
  public:
    Private(const PageDef *pd,bool isMainPage,bool isExample)
      : DefinitionContext<PageContext::Private>(pd) , m_pageDef(pd), m_isMainPage(isMainPage),
        m_isExample(isExample)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",       &Private::title);
        s_inst.addProperty("highlight",   &Private::highlight);
        s_inst.addProperty("subhighlight",&Private::subHighlight);
        s_inst.addProperty("example",     &Private::example);
        init=TRUE;
      }
      if (!pd->cookie()) { pd->setCookie(new PageContext::Private::Cachable(pd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant title() const
    {
      if (m_isMainPage)
      {
        QCString projectName = Config_getString(PROJECT_NAME);
        if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty())
        {
          if (Doxygen::mainPage->title().lower()!="notitle")
          {
            return m_pageDef->title();
          }
          else
          {
            return "";
          }
        }
        else if (!projectName.isEmpty())
        {
          return projectName+" "+theTranslator->trDocumentation();
        }
        else
        {
          return theTranslator->trMainPage();
        }
      }
      else if (m_isExample)
      {
        return m_pageDef->name();
      }
      else
      {
        return m_pageDef->title();
      }
    }
    TemplateVariant relPath() const
    {
      if (m_isMainPage)
      {
        return "";
      }
      else
      {
        return DefinitionContext<PageContext::Private>::relPath();
      }
    }
    TemplateVariant highlight() const
    {
      if (m_isMainPage)
      {
        return "main";
      }
      else
      {
        return "pages";
      }
    }
    TemplateVariant subHighlight() const
    {
      return "";
    }
    TemplateVariant example() const
    {
      if (m_isExample)
      {
        Cachable &cache = getCache();
        if (!cache.example || g_globals.outputFormat!=cache.exampleOutputFormat)
        {
          cache.example.reset(new TemplateVariant(
                parseDoc(m_pageDef,m_pageDef->docFile(),m_pageDef->docLine(),
                  relPathAsString(),"\\include "+m_pageDef->name(),FALSE)));
          cache.exampleOutputFormat = g_globals.outputFormat;
        }
        return *cache.example;
      }
      else
      {
        return TemplateVariant("");
      }
    }
  private:
    const PageDef *m_pageDef;
    struct Cachable : public DefinitionContext<PageContext::Private>::Cachable
    {
      Cachable(const PageDef *pd) : DefinitionContext<PageContext::Private>::Cachable(pd),
                              exampleOutputFormat(ContextOutputFormat_Unspecified) { }
      std::unique_ptr<TemplateVariant> example;
      ContextOutputFormat        exampleOutputFormat;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_pageDef->cookie());
      assert(c!=0);
      return *c;
    }
    bool m_isMainPage;
    bool m_isExample;
    static PropertyMapper<PageContext::Private> s_inst;
};
//%% }

PropertyMapper<PageContext::Private> PageContext::Private::s_inst;

PageContext::PageContext(const PageDef *pd,bool isMainPage,bool isExample) : p(std::make_unique<Private>(pd,isMainPage,isExample))
{
}

PageContext::~PageContext()
{
}

TemplateVariant PageContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector PageContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

class TextGeneratorHtml : public TextGeneratorIntf
{
  public:
    TextGeneratorHtml(TextStream &ts,const QCString &relPath)
       : m_ts(ts), m_relPath(relPath) {}
    void writeString(const QCString &s,bool keepSpaces) const
    {
      static const char *hex="0123456789ABCDEF";
      if (s.isEmpty()) return;
      if (keepSpaces)
      {
        const char *p=s.data();
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
            default:
              {
                uchar uc = static_cast<uchar>(c);
                if (uc<32 && !isspace(c)) // non-printable control characters
                {
                  m_ts << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
                }
                else
                {
                  m_ts << c;
                }
              }
              break;
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

    void writeLink(const QCString &ref,const QCString &f,
                   const QCString &anchor,const QCString &name
                  ) const
    {
      if (!ref.isEmpty())
      {
        m_ts << "<a class=\"elRef\" ";
        m_ts << externalLinkTarget();
      }
      else
      {
        m_ts << "<a class=\"el\" ";
      }
      m_ts << "href=\"";
      m_ts << externalRef(m_relPath,ref,TRUE);
      if (!f.isEmpty()) m_ts << f << Doxygen::htmlFileExtension;
      if (!anchor.isEmpty()) m_ts << "#" << anchor;
      m_ts << "\">";
      m_ts << convertToHtml(name);
      m_ts << "</a>";
    }

  private:
    TextStream &m_ts;
    QCString m_relPath;
};

//------------------------------------------------------------------------

class TextGeneratorLatex : public TextGeneratorIntf
{
  public:
    TextGeneratorLatex(TextStream &ts) : m_ts(ts) {}
    void writeString(const QCString &s,bool keepSpaces) const
    {
      if (s==0) return;
      m_ts << convertToLaTeX(s,FALSE,keepSpaces);
    }
    void writeBreak(int indent) const
    {
      m_ts << "\\\\*\n";
      for (int i=0;i<indent;i++)
      {
        m_ts << "~";
      }
    }
    void writeLink(const QCString &ref,const QCString &f,
                   const QCString &anchor,const QCString &text
                  ) const
    {
      static bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
      if (ref.isEmpty() && pdfHyperlinks)
      {
        m_ts << "\\mbox{\\hyperlink{";
        if (!f.isEmpty()) m_ts << stripPath(f);
        if (!f.isEmpty() && !anchor.isEmpty()) m_ts << "_";
        if (!anchor.isEmpty()) m_ts << anchor;
        m_ts << "}{";
        filterLatexString(m_ts,text,
                          false, // insideTabbing
                          false, // insidePre
                          false, // insideItem
                          false, // insideTable
                          false  // keepSpaces
                         );
        m_ts << "}}";
      }
      else
      {
        m_ts << "\\textbf{ ";
        filterLatexString(m_ts,text,
                          false, // insideTabbing
                          false, // insidePre
                          false, // insideItem
                          false, // insideTable
                          false  // keepSpaces
                         );
        m_ts << "}";
      }
    }

  private:
    TextStream &m_ts;
};

//------------------------------------------------------------------------

class TextGeneratorFactory
{
  public:
    static TextGeneratorFactory *instance()
    {
      static TextGeneratorFactory *instance = 0;
      if (instance==0) instance = new TextGeneratorFactory;
      return instance;
    }
    TextGeneratorIntf *create(TextStream &ts,const QCString &relPath)
    {
      switch (g_globals.outputFormat)
      {
        case ContextOutputFormat_Html:
          return new TextGeneratorHtml(ts,relPath);
        case ContextOutputFormat_Latex:
          return new TextGeneratorLatex(ts);
        default:
          break;
      }
      return 0;
    }
  private:
    TextGeneratorFactory() {}
    virtual ~TextGeneratorFactory() {}
};

TemplateVariant createLinkedText(const Definition *def,const QCString &relPath,const QCString &text)
{
  TextStream ts;
  TextGeneratorIntf *tg = TextGeneratorFactory::instance()->create(ts,relPath);
  if (tg)
  {
    linkifyText(*tg,def->getOuterScope(),def->getBodyDef(),def,text);
    delete tg;
    return TemplateVariant(ts.str().c_str(),true);
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
    Private(const MemberDef *md) : DefinitionContext<MemberContext::Private>(md) , m_memberDef(md)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("isSignal",            &Private::isSignal);
        s_inst.addProperty("isSlot",              &Private::isSlot);
        s_inst.addProperty("isVariable",          &Private::isVariable);
        s_inst.addProperty("isEnumeration",       &Private::isEnumeration);
        s_inst.addProperty("isEnumValue",         &Private::isEnumValue);
        s_inst.addProperty("isTypedef",           &Private::isTypedef);
        s_inst.addProperty("isFunction",          &Private::isFunction);
        s_inst.addProperty("isFunctionPtr",       &Private::isFunctionPtr);
        s_inst.addProperty("isDefine",            &Private::isDefine);
        s_inst.addProperty("isFriend",            &Private::isFriend);
        s_inst.addProperty("isProperty",          &Private::isProperty);
        s_inst.addProperty("isEvent",             &Private::isEvent);
        s_inst.addProperty("isRelated",           &Private::isRelated);
        s_inst.addProperty("isForeign",           &Private::isForeign);
        s_inst.addProperty("isStatic",            &Private::isStatic);
        s_inst.addProperty("isInline",            &Private::isInline);
        s_inst.addProperty("isExplicit",          &Private::isExplicit);
        s_inst.addProperty("isMutable",           &Private::isMutable);
        s_inst.addProperty("isGettable",          &Private::isGettable);
        s_inst.addProperty("isPrivateGettable",   &Private::isPrivateGettable);
        s_inst.addProperty("isProtectedGettable", &Private::isProtectedGettable);
        s_inst.addProperty("isSettable",          &Private::isSettable);
        s_inst.addProperty("isPrivateSettable",   &Private::isPrivateSettable);
        s_inst.addProperty("isProtectedSettable", &Private::isProtectedSettable);
        s_inst.addProperty("isReadable",          &Private::isReadable);
        s_inst.addProperty("isWritable",          &Private::isWritable);
        s_inst.addProperty("isAddable",           &Private::isAddable);
        s_inst.addProperty("isRemovable",         &Private::isRemovable);
        s_inst.addProperty("isRaisable",          &Private::isRaisable);
        s_inst.addProperty("isFinal",             &Private::isFinal);
        s_inst.addProperty("isAbstract",          &Private::isAbstract);
        s_inst.addProperty("isOverride",          &Private::isOverride);
        s_inst.addProperty("isInitonly",          &Private::isInitonly);
        s_inst.addProperty("isOptional",          &Private::isOptional);
        s_inst.addProperty("isRequired",          &Private::isRequired);
        s_inst.addProperty("isNonAtomic",         &Private::isNonAtomic);
        s_inst.addProperty("isCopy",              &Private::isCopy);
        s_inst.addProperty("isAssign",            &Private::isAssign);
        s_inst.addProperty("isRetain",            &Private::isRetain);
        s_inst.addProperty("isWeak",              &Private::isWeak);
        s_inst.addProperty("isStrong",            &Private::isStrong);
        s_inst.addProperty("isEnumStruct",        &Private::isEnumStruct);
        s_inst.addProperty("isUnretained",        &Private::isUnretained);
        s_inst.addProperty("isNew",               &Private::isNew);
        s_inst.addProperty("isSealed",            &Private::isSealed);
        s_inst.addProperty("isImplementation",    &Private::isImplementation);
        s_inst.addProperty("isExternal",          &Private::isExternal);
        s_inst.addProperty("isTypeAlias",         &Private::isTypeAlias);
        s_inst.addProperty("isDefault",           &Private::isDefault);
        s_inst.addProperty("isDelete",            &Private::isDelete);
        s_inst.addProperty("isNoExcept",          &Private::isNoExcept);
        s_inst.addProperty("isAttribute",         &Private::isAttribute);
        s_inst.addProperty("isUNOProperty",       &Private::isUNOProperty);
        s_inst.addProperty("isReadonly",          &Private::isReadonly);
        s_inst.addProperty("isBound",             &Private::isBound);
        s_inst.addProperty("isConstrained",       &Private::isConstrained);
        s_inst.addProperty("isTransient",         &Private::isTransient);
        s_inst.addProperty("isMaybeVoid",         &Private::isMaybeVoid);
        s_inst.addProperty("isMaybeDefault",      &Private::isMaybeDefault);
        s_inst.addProperty("isMaybeAmbiguous",    &Private::isMaybeAmbiguous);
        s_inst.addProperty("isPublished",         &Private::isPublished);
        s_inst.addProperty("isTemplateSpecialization",&Private::isTemplateSpecialization);
        s_inst.addProperty("isObjCMethod",        &Private::isObjCMethod);
        s_inst.addProperty("isObjCProperty",      &Private::isObjCProperty);
        s_inst.addProperty("isAnonymous",         &Private::isAnonymous);
        s_inst.addProperty("hasParameters",       &Private::hasParameters);
        s_inst.addProperty("declType",            &Private::declType);
        s_inst.addProperty("declArgs",            &Private::declArgs);
        s_inst.addProperty("anonymousType",       &Private::anonymousType);
        s_inst.addProperty("anonymousMember",     &Private::anonymousMember);
        s_inst.addProperty("hasDetails",          &Private::hasDetails);
        s_inst.addProperty("exception",           &Private::exception);
        s_inst.addProperty("bitfields",           &Private::bitfields);
        s_inst.addProperty("initializer",         &Private::initializer);
        s_inst.addProperty("initializerAsCode",   &Private::initializerAsCode);
        s_inst.addProperty("hasOneLineInitializer",   &Private::hasOneLineInitializer);
        s_inst.addProperty("hasMultiLineInitializer", &Private::hasMultiLineInitializer);
        s_inst.addProperty("templateArgs",        &Private::templateArgs);
        s_inst.addProperty("templateAlias",       &Private::templateAlias);
        s_inst.addProperty("propertyAttrs",       &Private::propertyAttrs);
        s_inst.addProperty("eventAttrs",          &Private::eventAttrs);
        s_inst.addProperty("category",            &Private::category);
        s_inst.addProperty("categoryRelation",    &Private::categoryRelation);
        s_inst.addProperty("class",               &Private::getClass);
        s_inst.addProperty("file",                &Private::getFile);
        s_inst.addProperty("namespace",           &Private::getNamespace);
        s_inst.addProperty("definition",          &Private::definition);
        s_inst.addProperty("parameters",          &Private::parameters);
        s_inst.addProperty("hasConstQualifier",   &Private::hasConstQualifier);
        s_inst.addProperty("hasVolatileQualifier",&Private::hasVolatileQualifier);
        s_inst.addProperty("hasRefQualifierLValue", &Private::hasRefQualifierLValue);
        s_inst.addProperty("hasRefQualifierRValue", &Private::hasRefQualifierRValue);
        s_inst.addProperty("trailingReturnType",  &Private::trailingReturnType);
        s_inst.addProperty("extraTypeChars",      &Private::extraTypeChars);
        s_inst.addProperty("templateDecls",       &Private::templateDecls);
        s_inst.addProperty("labels",              &Private::labels);
        s_inst.addProperty("enumBaseType",        &Private::enumBaseType);
        s_inst.addProperty("enumValues",          &Private::enumValues);
        s_inst.addProperty("paramDocs",           &Private::paramDocs);
        s_inst.addProperty("reimplements",        &Private::reimplements);
        s_inst.addProperty("implements",          &Private::implements);
        s_inst.addProperty("reimplementedBy",     &Private::reimplementedBy);
        s_inst.addProperty("implementedBy",       &Private::implementedBy);
        s_inst.addProperty("examples",            &Private::examples);
        s_inst.addProperty("typeConstraints",     &Private::typeConstraints);
        s_inst.addProperty("functionQualifier",   &Private::functionQualifier);
        s_inst.addProperty("sourceRefs",          &Private::sourceRefs);
        s_inst.addProperty("sourceRefBys",        &Private::sourceRefBys);
        s_inst.addProperty("hasSources",          &Private::hasSources);
        s_inst.addProperty("sourceCode",          &Private::sourceCode);
        s_inst.addProperty("hasCallGraph",        &Private::hasCallGraph);
        s_inst.addProperty("callGraph",           &Private::callGraph);
        s_inst.addProperty("hasCallerGraph",      &Private::hasCallerGraph);
        s_inst.addProperty("callerGraph",         &Private::callerGraph);
        s_inst.addProperty("hasReferencedByRelation", &Private::hasReferencedByRelation);
        s_inst.addProperty("referencedByRelation",    &Private::referencedByRelation);
        s_inst.addProperty("hasReferencesRelation",   &Private::hasReferencesRelation);
        s_inst.addProperty("referencesRelation",      &Private::referencesRelation);
        s_inst.addProperty("fieldType",           &Private::fieldType);
        s_inst.addProperty("type",                &Private::type);
        s_inst.addProperty("detailsVisibleFor",   &Private::detailsVisibleFor);
        s_inst.addProperty("nameWithContextFor",  &Private::nameWithContextFor);
        init=TRUE;
      }
      if (!md->cookie()) { md->setCookie(new MemberContext::Private::Cachable(md)); }

      m_propertyAttrs = TemplateList::alloc();
      if (md->isProperty())
      {
        if (md->isGettable())           m_propertyAttrs->append("get");
        if (md->isPrivateGettable())    m_propertyAttrs->append("private get");
        if (md->isProtectedGettable())  m_propertyAttrs->append("protected get");
        if (md->isSettable())           m_propertyAttrs->append("set");
        if (md->isPrivateSettable())    m_propertyAttrs->append("private set");
        if (md->isProtectedSettable())  m_propertyAttrs->append("protected set");
      }
      m_eventAttrs = TemplateList::alloc();
      if (md->isEvent())
      {
        if (md->isAddable())   m_eventAttrs->append("add");
        if (md->isRemovable()) m_eventAttrs->append("remove");
        if (md->isRaisable())  m_eventAttrs->append("raise");
      }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant fieldType() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->fieldType());
    }
    TemplateVariant declType() const
    {
      Cachable &cache = getCache();
      if (!cache.declTypeParsed)
      {
        cache.declType=createLinkedText(m_memberDef,relPathAsString(),m_memberDef->getDeclType());
        cache.declTypeParsed = TRUE;
        return cache.declType;
      }
      else
      {
        return cache.declType;
      }
    }
    TemplateVariant declArgs() const
    {
      Cachable &cache = getCache();
      if (!cache.declArgsParsed)
      {
        cache.declArgs=createLinkedText(m_memberDef,relPathAsString(),m_memberDef->argsString());
        cache.declArgsParsed = TRUE;
        return cache.declArgs;
      }
      else
      {
        return cache.declArgs;
      }
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
      return m_memberDef->isGettable();
    }
    TemplateVariant isPrivateGettable() const
    {
      return m_memberDef->isPrivateGettable();
    }
    TemplateVariant isProtectedGettable() const
    {
      return m_memberDef->isProtectedGettable();
    }
    TemplateVariant isSettable() const
    {
      return m_memberDef->isSettable();
    }
    TemplateVariant isPrivateSettable() const
    {
      return m_memberDef->isPrivateSettable();
    }
    TemplateVariant isProtectedSettable() const
    {
      return m_memberDef->isProtectedSettable();
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
    TemplateVariant isEnumStruct() const
    {
      return m_memberDef->isEnumStruct();
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
    TemplateVariant isTypeAlias() const
    {
      return m_memberDef->isTypeAlias();
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
      return m_memberDef->hasDetailedDescription();
    }
    TemplateVariant initializer() const
    {
      return createLinkedText(m_memberDef,relPathAsString(),m_memberDef->initializer());
    }
    TemplateVariant initializerAsCode() const
    {
      Cachable &cache = getCache();
      if (!cache.initializerParsed)
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
        cache.initializer = parseCode(m_memberDef,
                                      scopeName,relPathAsString(),
                                      m_memberDef->initializer());
        cache.initializerParsed = TRUE;
      }
      return cache.initializer;
    }
    TemplateVariant isDefine() const
    {
      return m_memberDef->isDefine();
    }
    TemplateVariant isAnonymous() const
    {
      return m_memberDef->isAnonymous();
    }
    TemplateVariant anonymousType() const
    {
      Cachable &cache = getCache();
      if (!cache.anonymousType)
      {
        const ClassDef *cd = m_memberDef->getClassDefOfAnonymousType();
        if (cd)
        {
          cache.anonymousType = ClassContext::alloc(cd);
        }
      }
      if (cache.anonymousType)
      {
        return TemplateVariant(cache.anonymousType);
      }
      else
      {
        return FALSE;
      }
    }
    TemplateVariant anonymousMember() const
    {
      Cachable &cache = getCache();
      if (!cache.anonymousMember)
      {
        MemberDef *md = m_memberDef->fromAnonymousMember();
        if (md)
        {
          cache.anonymousMember = MemberContext::alloc(md);
        }
      }
      if (cache.anonymousMember)
      {
        return TemplateVariant(cache.anonymousMember);
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
      if (m_memberDef->isEnumerate())
      {
        Cachable &cache = getCache();
        if (!cache.enumValues)
        {
          cache.enumValues = MemberListContext::alloc(m_memberDef->enumFieldList());
        }
        return TemplateVariant(cache.enumValues);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant templateArgs() const
    {
      Cachable &cache = getCache();
      if (!cache.templateArgs && !m_memberDef->templateArguments().empty())
      {
        cache.templateArgs = ArgumentListContext::alloc(m_memberDef->templateArguments(),m_memberDef,relPathAsString());
      }
      if (cache.templateArgs)
      {
        return TemplateVariant(cache.templateArgs);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant templateAlias() const
    {
      if (m_memberDef->isTypeAlias())
      {
        return createLinkedText(m_memberDef,relPathAsString(),
                                QCString(" = ")+m_memberDef->typeString());
      }
      return "";
    }
    TemplateVariant propertyAttrs() const
    {
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_propertyAttrs));
    }
    TemplateVariant eventAttrs() const
    {
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_eventAttrs));
    }
    TemplateVariant getClass() const
    {
      Cachable &cache = getCache();
      if (!cache.classDef && m_memberDef->getClassDef())
      {
        cache.classDef = ClassContext::alloc(m_memberDef->getClassDef());
      }
      if (cache.classDef)
      {
        return TemplateVariant(cache.classDef);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant category() const
    {
      Cachable &cache = getCache();
      if (!cache.category && m_memberDef->category())
      {
        cache.category = ClassContext::alloc(m_memberDef->category());
      }
      if (cache.category)
      {
        return TemplateVariant(cache.category);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant categoryRelation() const
    {
      Cachable &cache = getCache();
      if (!cache.categoryRelation && m_memberDef->categoryRelation())
      {
        cache.categoryRelation = MemberContext::alloc(m_memberDef->categoryRelation());
      }
      if (cache.categoryRelation)
      {
        return TemplateVariant(cache.categoryRelation);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getFile() const
    {
      Cachable &cache = getCache();
      if (!cache.fileDef && m_memberDef->getFileDef())
      {
        cache.fileDef = FileContext::alloc(m_memberDef->getFileDef());
      }
      if (cache.fileDef)
      {
        return TemplateVariant(cache.fileDef);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getNamespace() const
    {
      Cachable &cache = getCache();
      if (!cache.namespaceDef && m_memberDef->getNamespaceDef())
      {
        cache.namespaceDef = NamespaceContext::alloc(m_memberDef->getNamespaceDef());
      }
      if (cache.namespaceDef)
      {
        return TemplateVariant(cache.namespaceDef);
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
    const ArgumentList &getDefArgList() const
    {
      return (m_memberDef->isDocsForDefinition()) ?
              m_memberDef->argumentList() : m_memberDef->declArgumentList();
    }
    TemplateVariant parameters() const
    {
      Cachable &cache = getCache();
      if (!cache.arguments)
      {
        const ArgumentList &defArgList = getDefArgList();
        if (!m_memberDef->isProperty())
        {
          cache.arguments = ArgumentListContext::alloc(defArgList,m_memberDef,relPathAsString());
        }
        else
        {
          cache.arguments = ArgumentListContext::alloc();
        }
      }
      return TemplateVariant(cache.arguments);
    }
    TemplateVariant hasParameters() const
    {
      return !getDefArgList().empty();
    }
    TemplateVariant hasConstQualifier() const
    {
      return getDefArgList().constSpecifier();
    }
    TemplateVariant hasVolatileQualifier() const
    {
      return getDefArgList().volatileSpecifier();
    }
    TemplateVariant hasRefQualifierLValue() const
    {
      return getDefArgList().refQualifier()==RefQualifierLValue;
    }
    TemplateVariant hasRefQualifierRValue() const
    {
      return getDefArgList().refQualifier()==RefQualifierRValue;
    }
    TemplateVariant trailingReturnType() const
    {
      const ArgumentList &al = getDefArgList();
      if (!al.trailingReturnType().isEmpty())
      {
        return createLinkedText(m_memberDef,relPathAsString(),
                                al.trailingReturnType());
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
    void addTemplateDecls(TemplateListPtr tl) const
    {
      const ClassDef *cd=m_memberDef->getClassDef();
      if (!m_memberDef->definitionTemplateParameterLists().empty())
      {
        for (const ArgumentList &tal : m_memberDef->definitionTemplateParameterLists())
        {
          if (!tal.empty())
          {
            tl->append(ArgumentListContext::alloc(tal,m_memberDef,relPathAsString()));
          }
        }
      }
      else
      {
        if (cd && !m_memberDef->isRelated() && !m_memberDef->isTemplateSpecialization())
        {
          for (const ArgumentList &tal : cd->getTemplateParameterLists())
          {
            if (!tal.empty())
            {
              tl->append(ArgumentListContext::alloc(tal,m_memberDef,relPathAsString()));
            }
          }
        }
        if (!m_memberDef->templateArguments().empty()) // function template prefix
        {
          tl->append(ArgumentListContext::alloc(
              m_memberDef->templateArguments(),m_memberDef,relPathAsString()));
        }
      }
    }
    TemplateVariant templateDecls() const
    {
      Cachable &cache = getCache();
      if (!cache.templateDecls)
      {
        cache.templateDecls = TemplateList::alloc();
        addTemplateDecls(cache.templateDecls);
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.templateDecls));
    }
    TemplateVariant labels() const
    {
      Cachable &cache = getCache();
      if (!cache.labels)
      {
        StringVector sl = m_memberDef->getLabels(m_memberDef->getOuterScope());
        TemplateListPtr tl = TemplateList::alloc();
        for (const auto &s : sl)
        {
          tl->append(s.c_str());
        }
        cache.labels = tl;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.labels));
    }
    TemplateVariant paramDocs() const
    {
      Cachable &cache = getCache();
      if (!cache.paramDocs)
      {
        if (m_memberDef->argumentList().hasDocumentation())
        {
          QCString paramDocs;
          for (const Argument &a : m_memberDef->argumentList())
          {
            if (a.hasDocumentation())
            {
              QCString docs = a.docs;
              QCString direction = extractDirection(docs);
              paramDocs+="@param"+direction+" "+a.name+" "+docs;
            }
          }
          cache.paramDocs.reset(new TemplateVariant(parseDoc(m_memberDef,
                                           m_memberDef->docFile(),m_memberDef->docLine(),
                                           relPathAsString(),paramDocs,FALSE)));
        }
        else
        {
          cache.paramDocs.reset(new TemplateVariant(""));
        }
      }
      return *cache.paramDocs;
    }
    TemplateVariant implements() const
    {
      Cachable &cache = getCache();
      if (!cache.implements)
      {
        const MemberDef *md = m_memberDef->reimplements();
        cache.implements = TemplateList::alloc();
        if (md)
        {
          const ClassDef *cd = md->getClassDef();
          // filter on pure virtual/interface methods
          if (cd && (md->virtualness()==Pure || cd->compoundType()==ClassDef::Interface))
          {
            cache.implements->append(MemberContext::alloc(md));
          }
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.implements));
    }
    TemplateVariant reimplements() const
    {
      Cachable &cache = getCache();
      if (!cache.reimplements)
      {
        const MemberDef *md = m_memberDef->reimplements();
        cache.reimplements = TemplateList::alloc();
        if (md)
        {
          const ClassDef *cd = md->getClassDef();
          // filter on non-pure virtual & non interface methods
          if (cd && md->virtualness()!=Pure && cd->compoundType()!=ClassDef::Interface)
          {
            cache.reimplements->append(MemberContext::alloc(md));
          }
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.reimplements));
    }
    TemplateVariant implementedBy() const
    {
      Cachable &cache = getCache();
      if (!cache.implementedBy)
      {
        cache.implementedBy = TemplateList::alloc();
        for (const auto &md : m_memberDef->reimplementedBy())
        {
          const ClassDef *cd = md->getClassDef();
          // filter on pure virtual/interface methods
          if (cd && md->virtualness()==Pure && cd->compoundType()==ClassDef::Interface)
          {
            cache.implementedBy->append(MemberContext::alloc(md));
          }
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.implementedBy));
    }
    TemplateVariant reimplementedBy() const
    {
      Cachable &cache = getCache();
      if (!cache.reimplementedBy)
      {
        cache.reimplementedBy = TemplateList::alloc();
        for (const auto &md : m_memberDef->reimplementedBy())
        {
          const ClassDef *cd = md->getClassDef();
          // filter on non-pure virtual & non interface methods
          if (cd && md->virtualness()!=Pure && cd->compoundType()!=ClassDef::Interface)
          {
            cache.reimplementedBy->append(MemberContext::alloc(md));
          }
        }
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.reimplementedBy));
    }
    void addExamples(TemplateListPtr list) const
    {
      if (m_memberDef->hasExamples())
      {
        for (const auto &ex : m_memberDef->getExamples())
        {
          TemplateStructPtr s = TemplateStruct::alloc();
          s->set("text",ex.name);
          s->set("isLinkable",TRUE);
          s->set("anchor",ex.anchor);
          s->set("fileName",ex.file);
          s->set("isReference",FALSE);
          s->set("externalReference","");
          list->append(std::static_pointer_cast<TemplateStructIntf>(s));
        }
      }
    }
    TemplateVariant examples() const
    {
      Cachable &cache = getCache();
      if (!cache.examples)
      {
        cache.examples = TemplateList::alloc();
        addExamples(cache.examples);
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.examples));
    }
    TemplateVariant typeConstraints() const
    {
      Cachable &cache = getCache();
      if (cache.typeConstraints && !m_memberDef->typeConstraints().empty())
      {
        cache.typeConstraints = ArgumentListContext::alloc(m_memberDef->typeConstraints(),m_memberDef,relPathAsString());
      }
      else
      {
        cache.typeConstraints = ArgumentListContext::alloc();
      }
      return TemplateVariant(cache.typeConstraints);
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
      Cachable &cache = getCache();
      if (!cache.sourceRefs)
      {
        cache.sourceRefs = MemberListContext::alloc(m_memberDef->getReferencesMembers());
      }
      return TemplateVariant(cache.sourceRefs);
    }
    TemplateVariant sourceRefBys() const
    {
      Cachable &cache = getCache();
      if (!cache.sourceRefBys)
      {
        cache.sourceRefBys = MemberListContext::alloc(m_memberDef->getReferencedByMembers());
      }
      return TemplateVariant(cache.sourceRefBys);
    }
    TemplateVariant hasSources() const
    {
      return TemplateVariant(m_memberDef->hasSources());
    }
    TemplateVariant sourceCode() const
    {
      Cachable &cache = getCache();
      if (!cache.sourceCodeParsed)
      {
        QCString codeFragment;
        const FileDef *fd   = m_memberDef->getBodyDef();
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
          cache.sourceCode = parseCode(m_memberDef,
                                       scopeName,relPathAsString(),
                                       codeFragment,startLine,endLine,TRUE);
          cache.sourceCodeParsed = TRUE;
        }
      }
      return cache.sourceCode;
    }
    DotCallGraph *getCallGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.callGraph)
      {
        cache.callGraph.reset(new DotCallGraph(m_memberDef,FALSE));
      }
      return cache.callGraph.get();
    }
    TemplateVariant hasCallGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      if (m_memberDef->hasCallGraph() && haveDot &&
          (m_memberDef->isFunction() || m_memberDef->isSlot() || m_memberDef->isSignal()))
      {
        DotCallGraph *cg = getCallGraph();
        return !cg->isTooBig() && !cg->isTrivial();
      }
      return TemplateVariant(FALSE);
    }
    TemplateVariant hasReferencedByRelation() const
    {
      return TemplateVariant(m_memberDef->hasReferencedByRelation());
    }
    TemplateVariant callGraph() const
    {
      if (hasCallGraph().toBool())
      {
        DotCallGraph *cg = getCallGraph();
        TextStream t;
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_memberDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_memberDef->getOutputFileBase()+".tex",
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
        return TemplateVariant(t.str().c_str(),TRUE);
      }
      else
      {
        return TemplateVariant("");
      }
    }
    TemplateVariant referencedByRelation() const
    {
      if (hasReferencedByRelation().toBool())
      {
        err("context.cpp: output format not yet supported\n");
      }
      return TemplateVariant("");
    }
    DotCallGraph *getCallerGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.callerGraph)
      {
        cache.callerGraph.reset(new DotCallGraph(m_memberDef,TRUE));
      }
      return cache.callerGraph.get();
    }
    TemplateVariant hasCallerGraph() const
    {
      static bool haveDot = Config_getBool(HAVE_DOT);
      if (m_memberDef->hasCallerGraph() && haveDot &&
          (m_memberDef->isFunction() || m_memberDef->isSlot() || m_memberDef->isSignal()))
      {
        DotCallGraph *cg = getCallerGraph();
        return !cg->isTooBig() && !cg->isTrivial();
      }
      return TemplateVariant(FALSE);
    }
    TemplateVariant hasReferencesRelation() const
    {
      return TemplateVariant(m_memberDef->hasReferencesRelation());
    }
    TemplateVariant callerGraph() const
    {
      if (hasCallerGraph().toBool())
      {
        DotCallGraph *cg = getCallerGraph();
        TextStream t;
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              cg->writeGraph(t,GOF_BITMAP,EOF_Html,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_memberDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
          case ContextOutputFormat_Latex:
            {
              cg->writeGraph(t,GOF_EPS,EOF_LaTeX,
                  g_globals.outputDir,
                  g_globals.outputDir+Portable::pathSeparator()+m_memberDef->getOutputFileBase()+".tex",
                  relPathAsString(),TRUE,g_globals.dynSectionId
                  );
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
        return TemplateVariant(t.str().c_str(),TRUE);
      }
      else
      {
        return TemplateVariant("");
      }
    }
    TemplateVariant referencesRelation() const
    {
      if (hasReferencesRelation().toBool())
      {
         err("context.cpp: output format not yet supported\n");
      }
      return TemplateVariant("");
    }
    TemplateVariant type() const
    {
      return m_memberDef->typeString();
    }
    TemplateVariant handleDetailsVisibleFor(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        QCString containerStr = args[0].toString();
        MemberListContainer                 container = MemberListContainer::Class;
        if      (containerStr=="module")    container = MemberListContainer::Group;
        else if (containerStr=="file")      container = MemberListContainer::File;
        else if (containerStr=="namespace") container = MemberListContainer::Namespace;
        return m_memberDef->isDetailedSectionVisible(container);
      }
      else
      {
        err(".detailsVisibleFor should take one string argument, got %zu\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant detailsVisibleFor() const
    {
      return TemplateVariant(std::bind(&Private::handleDetailsVisibleFor,this,std::placeholders::_1));
    }
    TemplateVariant handleNameWithContextFor(const std::vector<TemplateVariant> &args) const
    {
      if (args.size()==1)
      {
        SrcLangExt lang = m_memberDef->getLanguage();
        QCString n = m_memberDef->name();
        QCString ctx = args[0].toString();
        QCString sep = getLanguageSpecificSeparator(lang,TRUE);
        if (m_memberDef->getEnumScope() && m_memberDef->livesInsideEnum())
        {
          n.prepend(m_memberDef->getEnumScope()->displayName()+sep);
        }
        if (ctx=="module" && m_memberDef->getClassDef() && !m_memberDef->isRelated())
        {
          n.prepend(m_memberDef->getClassDef()->displayName()+sep);
        }
        else if ((ctx=="module" || ctx=="file") && m_memberDef->getNamespaceDef())
        {
          n.prepend(m_memberDef->getNamespaceDef()->displayName()+sep);
        }
        return n;
      }
      else
      {
        err(".nameWithContextFor should take one string argument, got %zu\n",args.size());
      }
      return TemplateVariant();
    }
    TemplateVariant nameWithContextFor() const
    {
      return TemplateVariant(std::bind(&Private::handleNameWithContextFor,this,std::placeholders::_1));
    }
  private:
    const MemberDef *m_memberDef;
    struct Cachable : public DefinitionContext<MemberContext::Private>::Cachable
    {
      Cachable(const MemberDef *md) : DefinitionContext<MemberContext::Private>::Cachable(md),
                                initializerParsed(FALSE), sourceCodeParsed(FALSE),
                                declArgsParsed(FALSE), declTypeParsed(FALSE) { }
      TemplateListIntfPtr                  templateArgs;
      TemplateListIntfPtr                  arguments;
      TemplateListIntfPtr                  enumValues;
      TemplateStructIntfPtr                fileDef;
      TemplateStructIntfPtr                namespaceDef;
      TemplateStructIntfPtr                category;
      TemplateStructIntfPtr                categoryRelation;
      TemplateStructIntfPtr                classDef;
      TemplateStructIntfPtr                anonymousType;
      TemplateListPtr                      templateDecls;
      std::unique_ptr<TemplateVariant>     paramDocs;
      TemplateListPtr                      implements;
      TemplateListPtr                      reimplements;
      TemplateListPtr                      implementedBy;
      TemplateListPtr                      reimplementedBy;
      TemplateListIntfPtr                  sourceRefs;
      TemplateListIntfPtr                  sourceRefBys;
      std::unique_ptr<DotCallGraph>        callGraph;
      std::unique_ptr<DotCallGraph>        callerGraph;
      TemplateStructIntfPtr                anonymousMember;
      TemplateListPtr                      labels;
      TemplateVariant                      initializer;
      bool                                 initializerParsed;
      TemplateVariant                      sourceCode;
      bool                                 sourceCodeParsed;
      TemplateVariant                      declArgs;
      bool                                 declArgsParsed;
      TemplateVariant                      declType;
      bool                                 declTypeParsed;
      TemplateListPtr                      examples;
      TemplateListIntfPtr                  typeConstraints;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_memberDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<MemberContext::Private> s_inst;
    TemplateListPtr                               m_propertyAttrs;
    TemplateListPtr                               m_eventAttrs;
};
//%% }

PropertyMapper<MemberContext::Private> MemberContext::Private::s_inst;

MemberContext::MemberContext(const MemberDef *md) : p(std::make_unique<Private>(md))
{
}

MemberContext::~MemberContext()
{
}

TemplateVariant MemberContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector MemberContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Concept(Symbol): class information
//%% {
class ConceptContext::Private : public DefinitionContext<ConceptContext::Private>
{
  public:
    Private(const ConceptDef *cd) : DefinitionContext<ConceptContext::Private>(cd),
       m_conceptDef(cd)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",                     &Private::title);
        s_inst.addProperty("highlight",                 &Private::highlight);
        s_inst.addProperty("subhighlight",              &Private::subHighlight);
        s_inst.addProperty("hasDetails",                &Private::hasDetails);
        s_inst.addProperty("includeInfo",               &Private::includeInfo);
        s_inst.addProperty("templateDecls",             &Private::templateDecls);
        s_inst.addProperty("initializer",               &Private::initializer);
        s_inst.addProperty("initializerAsCode",         &Private::initializerAsCode);
        init=TRUE;
      }
      if (!cd->cookie()) { cd->setCookie(new ConceptContext::Private::Cachable(cd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant title() const
    {
      return TemplateVariant(m_conceptDef->title());
    }
    TemplateVariant highlight() const
    {
      return TemplateVariant("concepts");
    }
    TemplateVariant subHighlight() const
    {
      return TemplateVariant("");
    }
    TemplateVariant hasDetails() const
    {
      return m_conceptDef->hasDetailedDescription();
    }
    TemplateVariant includeInfo() const
    {
      Cachable &cache = getCache();
      if (!cache.includeInfo && m_conceptDef->includeInfo())
      {
        cache.includeInfo = IncludeInfoContext::alloc(m_conceptDef->includeInfo(),m_conceptDef->getLanguage());
      }
      if (cache.includeInfo)
      {
        return TemplateVariant(cache.includeInfo);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    void addTemplateDecls(const ConceptDef *cd,TemplateListPtr tl) const
    {
      if (!cd->getTemplateParameterList().empty())
      {
        // since a TemplateVariant does take ownership of the object, we add it
        // a separate list just to be able to delete it and avoid a memory leak
        tl->append(ArgumentListContext::alloc(cd->getTemplateParameterList(),cd,relPathAsString()));
      }
    }
    TemplateVariant templateDecls() const
    {
      Cachable &cache = getCache();
      if (!cache.templateDecls)
      {
        cache.templateDecls = TemplateList::alloc();
        addTemplateDecls(m_conceptDef,cache.templateDecls);
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.templateDecls));
    }
    TemplateVariant initializer() const
    {
      return createLinkedText(m_conceptDef,relPathAsString(),m_conceptDef->initializer());
    }
    TemplateVariant initializerAsCode() const
    {
      Cachable &cache = getCache();
      if (!cache.initializerParsed)
      {
        QCString scopeName;
        if (m_conceptDef->getOuterScope()!=Doxygen::globalScope)
        {
          scopeName = m_conceptDef->getOuterScope()->name();
        }
        cache.initializer = parseCode(m_conceptDef,
                                      scopeName,relPathAsString(),
                                      m_conceptDef->initializer());
        cache.initializerParsed = TRUE;
      }
      return cache.initializer;
    }

  private:
    const ConceptDef *m_conceptDef;
    struct Cachable : public DefinitionContext<ConceptContext::Private>::Cachable
    {
      Cachable(const ConceptDef *cd) : DefinitionContext<ConceptContext::Private>::Cachable(cd) {}
      TemplateStructIntfPtr             includeInfo;
      TemplateListPtr                   templateDecls;
      TemplateVariant                   initializer;
      bool                              initializerParsed = false;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_conceptDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<ConceptContext::Private> s_inst;
};
//%% }

PropertyMapper<ConceptContext::Private> ConceptContext::Private::s_inst;

ConceptContext::ConceptContext(const ConceptDef *cd) : p(std::make_unique<Private>(cd))
{
}

ConceptContext::~ConceptContext()
{
}

TemplateVariant ConceptContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector ConceptContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct Module(Symbol): group information
//%% {
class ModuleContext::Private : public DefinitionContext<ModuleContext::Private>
{
  public:
    Private(const GroupDef *gd) : DefinitionContext<ModuleContext::Private>(gd) , m_groupDef(gd)
    {
      static bool init=FALSE;
      if (!init)
      {
        addBaseProperties(s_inst);
        s_inst.addProperty("title",                     &Private::title);
        s_inst.addProperty("highlight",                 &Private::highlight);
        s_inst.addProperty("subhighlight",              &Private::subHighlight);
        s_inst.addProperty("hasGroupGraph",             &Private::hasGroupGraph);
        s_inst.addProperty("groupGraph",                &Private::groupGraph);
        s_inst.addProperty("hasDetails",                &Private::hasDetails);
        s_inst.addProperty("modules",                   &Private::modules);
        s_inst.addProperty("dirs",                      &Private::dirs);
        s_inst.addProperty("files",                     &Private::files);
        s_inst.addProperty("namespaces",                &Private::namespaces);
        s_inst.addProperty("classes",                   &Private::classes);
        s_inst.addProperty("constantgroups",            &Private::constantgroups);
        s_inst.addProperty("examples",                  &Private::examples);
        s_inst.addProperty("macros",                    &Private::macros);
        s_inst.addProperty("typedefs",                  &Private::typedefs);
        s_inst.addProperty("enums",                     &Private::enums);
        s_inst.addProperty("enumvalues",                &Private::enumValues);
        s_inst.addProperty("functions",                 &Private::functions);
        s_inst.addProperty("variables",                 &Private::variables);
        s_inst.addProperty("signals",                   &Private::signals);
        s_inst.addProperty("publicSlots",               &Private::publicSlots);
        s_inst.addProperty("protectedSlots",            &Private::protectedSlots);
        s_inst.addProperty("privateSlots",              &Private::privateSlots);
        s_inst.addProperty("events",                    &Private::events);
        s_inst.addProperty("properties",                &Private::properties);
        s_inst.addProperty("friends",                   &Private::friends);
        s_inst.addProperty("memberGroups",              &Private::memberGroups);
        s_inst.addProperty("detailedMacros",            &Private::detailedMacros);
        s_inst.addProperty("detailedTypedefs",          &Private::detailedTypedefs);
        s_inst.addProperty("detailedEnums",             &Private::detailedEnums);
        s_inst.addProperty("detailedEnumValues",        &Private::detailedEnumValues);
        s_inst.addProperty("detailedFunctions",         &Private::detailedFunctions);
        s_inst.addProperty("detailedVariables",         &Private::detailedVariables);
        s_inst.addProperty("detailedSignals",           &Private::detailedSignals);
        s_inst.addProperty("detailedPublicSlots",       &Private::detailedPublicSlots);
        s_inst.addProperty("detailedProtectedSlots",    &Private::detailedProtectedSlots);
        s_inst.addProperty("detailedPrivateSlots",      &Private::detailedPrivateSlots);
        s_inst.addProperty("detailedEvents",            &Private::detailedEvents);
        s_inst.addProperty("detailedProperties",        &Private::detailedProperties);
        s_inst.addProperty("detailedFriends",           &Private::detailedFriends);
        s_inst.addProperty("inlineClasses",             &Private::inlineClasses);
        s_inst.addProperty("compoundType",              &Private::compoundType);
        init=TRUE;
      }
      if (!gd->cookie()) { gd->setCookie(new ModuleContext::Private::Cachable(gd)); }
    }
    virtual ~Private() {}
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
    DotGroupCollaboration *getGroupGraph() const
    {
      Cachable &cache = getCache();
      if (!cache.groupGraph)
      {
        cache.groupGraph.reset(new DotGroupCollaboration(m_groupDef));
      }
      return cache.groupGraph.get();
    }
    TemplateVariant hasGroupGraph() const
    {
      bool result=FALSE;
      static bool haveDot     = Config_getBool(HAVE_DOT);
      static bool groupGraphs = Config_getBool(GROUP_GRAPHS);
      if (haveDot && groupGraphs)
      {
        DotGroupCollaboration *graph = getGroupGraph();
        result = !graph->isTrivial();
      }
      return result;
    }
    TemplateVariant groupGraph() const
    {
      TextStream t;
      static bool haveDot     = Config_getBool(HAVE_DOT);
      static bool groupGraphs = Config_getBool(GROUP_GRAPHS);
      if (haveDot && groupGraphs)
      {
        DotGroupCollaboration *graph = getGroupGraph();
        switch (g_globals.outputFormat)
        {
          case ContextOutputFormat_Html:
            {
              graph->writeGraph(t,GOF_BITMAP,
                                EOF_Html,
                                g_globals.outputDir,
                                g_globals.outputDir+Portable::pathSeparator()+m_groupDef->getOutputFileBase()+Doxygen::htmlFileExtension,
                                relPathAsString(),
                                TRUE,
                                g_globals.dynSectionId);
            }
            break;
          case ContextOutputFormat_Latex:
            {
              graph->writeGraph(t,GOF_EPS,
                                EOF_LaTeX,
                                g_globals.outputDir,
                                g_globals.outputDir+Portable::pathSeparator()+m_groupDef->getOutputFileBase()+".tex",
                                relPathAsString(),
                                TRUE,
                                g_globals.dynSectionId);
            }
            break;
            // TODO: support other generators
          default:
            err("context.cpp: output format not yet supported\n");
            break;
        }
        g_globals.dynSectionId++;
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }
    TemplateVariant hasDetails() const
    {
      return m_groupDef->hasDetailedDescription();
    }
    TemplateVariant modules() const
    {
      Cachable &cache = getCache();
      if (!cache.modules)
      {
        TemplateListPtr moduleList = TemplateList::alloc();
        for (const auto &gd : m_groupDef->getSubGroups())
        {
          if (gd->isVisible())
          {
            moduleList->append(ModuleContext::alloc(gd));
          }
        }
        cache.modules = moduleList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.modules));
    }
    TemplateVariant examples() const
    {
      Cachable &cache = getCache();
      if (!cache.examples)
      {
        TemplateListPtr exampleList = TemplateList::alloc();
        for (const auto &ex : m_groupDef->getExamples())
        {
          exampleList->append(PageContext::alloc(ex,FALSE,TRUE));
        }
        cache.examples = exampleList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.examples));
    }
    TemplateVariant pages() const
    {
      Cachable &cache = getCache();
      if (!cache.pages)
      {
        TemplateListPtr pageList = TemplateList::alloc();
        for (const auto &ex : m_groupDef->getPages())
        {
          pageList->append(PageContext::alloc(ex,FALSE,TRUE));
        }
        cache.pages = pageList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.pages));
    }
    TemplateVariant dirs() const
    {
      Cachable &cache = getCache();
      if (!cache.dirs)
      {
        TemplateListPtr dirList = TemplateList::alloc();
        for(const auto dd : m_groupDef->getDirs())
        {
          dirList->append(DirContext::alloc(dd));
        }
        cache.dirs = dirList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.dirs));
    }
    TemplateVariant files() const
    {
      Cachable &cache = getCache();
      if (!cache.files)
      {
        TemplateListPtr fileList = TemplateList::alloc();
        for (const auto &fd : m_groupDef->getFiles())
        {
          fileList->append(FileContext::alloc(fd));
        }
        cache.files = fileList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.files));
    }
    TemplateVariant classes() const
    {
      Cachable &cache = getCache();
      if (!cache.classes)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_groupDef->getClasses())
        {
          if (cd->visibleInParentsDeclList())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.classes = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.classes));
    }
    TemplateVariant namespaces() const
    {
      Cachable &cache = getCache();
      if (!cache.namespaces)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_groupDef->getNamespaces())
        {
          if (nd->isLinkable() && !nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.namespaces = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.namespaces));
    }
    TemplateVariant constantgroups() const
    {
      Cachable &cache = getCache();
      if (!cache.constantgroups)
      {
        TemplateListPtr namespaceList = TemplateList::alloc();
        for (const auto &nd : m_groupDef->getNamespaces())
        {
          if (nd->isLinkable() && nd->isConstantGroup())
          {
            namespaceList->append(NamespaceContext::alloc(nd));
          }
        }
        cache.constantgroups = namespaceList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.constantgroups));
    }

    TemplateVariant getMemberList(TemplateStructIntfPtr &list,
                                  MemberListType type,const QCString &title,bool=FALSE) const
    {
      if (!list)
      {
        MemberList *ml = m_groupDef->getMemberList(type);
        if (ml)
        {
          list = MemberListInfoContext::alloc(m_groupDef,relPathAsString(),ml,title,"");
        }
      }
      if (list)
      {
        return TemplateVariant(list);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant macros() const
    {
      return getMemberList(getCache().macros,MemberListType_decDefineMembers,theTranslator->trDefines());
    }
    TemplateVariant typedefs() const
    {
      return getMemberList(getCache().typedefs,MemberListType_decTypedefMembers,theTranslator->trTypedefs());
    }
    TemplateVariant enums() const
    {
      return getMemberList(getCache().enums,MemberListType_decEnumMembers,theTranslator->trEnumerations());
    }
    TemplateVariant enumValues() const
    {
      return getMemberList(getCache().enums,MemberListType_decEnumValMembers,theTranslator->trEnumerationValues());
    }
    TemplateVariant functions() const
    {
      QCString title = theTranslator->trFunctions();
      SrcLangExt lang = m_groupDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprograms();
      else if (lang==SrcLangExt_VHDL) title=theTranslator->trFunctionAndProc();
      return getMemberList(getCache().functions,MemberListType_decFuncMembers,title);
    }
    TemplateVariant variables() const
    {
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      return getMemberList(getCache().variables,MemberListType_decVarMembers,
                           sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables());
    }
    TemplateVariant signals() const
    {
      return getMemberList(getCache().signals,MemberListType_signals,theTranslator->trSignals());
    }
    TemplateVariant publicSlots() const
    {
      return getMemberList(getCache().publicSlots,MemberListType_pubSlots,theTranslator->trPublicSlots());
    }
    TemplateVariant protectedSlots() const
    {
      return getMemberList(getCache().protectedSlots,MemberListType_proSlots,theTranslator->trProtectedSlots());
    }
    TemplateVariant privateSlots() const
    {
      return getMemberList(getCache().privateSlots,MemberListType_priSlots,theTranslator->trPrivateSlots());
    }
    TemplateVariant events() const
    {
      return getMemberList(getCache().events,MemberListType_events,theTranslator->trEvents());
    }
    TemplateVariant properties() const
    {
      return getMemberList(getCache().properties,MemberListType_properties,theTranslator->trProperties());
    }
    TemplateVariant friends() const
    {
      return getMemberList(getCache().friends,MemberListType_friends,theTranslator->trFriends());
    }
    TemplateVariant memberGroups() const
    {
      Cachable &cache = getCache();
      if (!cache.memberGroups)
      {
        if (!m_groupDef->getMemberGroups().empty())
        {
          cache.memberGroups = MemberGroupListContext::alloc(
              m_groupDef,relPathAsString(),m_groupDef->getMemberGroups(),m_groupDef->subGrouping());
        }
        else
        {
          cache.memberGroups = MemberGroupListContext::alloc();
        }
      }
      return TemplateVariant(cache.memberGroups);
    }
    TemplateVariant detailedMacros() const
    {
      return getMemberList(getCache().detailedMacros,MemberListType_docDefineMembers,theTranslator->trDefineDocumentation());
    }
    TemplateVariant detailedTypedefs() const
    {
      return getMemberList(getCache().detailedTypedefs,MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation());
    }
    TemplateVariant detailedEnums() const
    {
      return getMemberList(getCache().detailedEnums,MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation());
    }
    TemplateVariant detailedEnumValues() const
    {
      return getMemberList(getCache().detailedEnumValues,MemberListType_docEnumValMembers,theTranslator->trEnumerationValueDocumentation());
    }
    TemplateVariant detailedFunctions() const
    {
      QCString title = theTranslator->trFunctionDocumentation();
      SrcLangExt lang = m_groupDef->getLanguage();
      if (lang==SrcLangExt_Fortran) title=theTranslator->trSubprogramDocumentation();
      return getMemberList(getCache().detailedFunctions,MemberListType_docFuncMembers,title);
    }
    TemplateVariant detailedVariables() const
    {
      return getMemberList(getCache().detailedVariables,MemberListType_docVarMembers,theTranslator->trVariableDocumentation());
    }
    TemplateVariant detailedSignals() const
    {
      return getMemberList(getCache().detailedSignals,MemberListType_docSignalMembers,theTranslator->trSignals());
    }
    TemplateVariant detailedPublicSlots() const
    {
      return getMemberList(getCache().detailedPublicSlots,MemberListType_docPubSlotMembers,theTranslator->trPublicSlots());
    }
    TemplateVariant detailedProtectedSlots() const
    {
      return getMemberList(getCache().detailedProtectedSlots,MemberListType_docProSlotMembers,theTranslator->trProtectedSlots());
    }
    TemplateVariant detailedPrivateSlots() const
    {
      return getMemberList(getCache().detailedPrivateSlots,MemberListType_docPriSlotMembers,theTranslator->trPrivateSlots());
    }
    TemplateVariant detailedEvents() const
    {
      return getMemberList(getCache().detailedEvents,MemberListType_docEventMembers,theTranslator->trEventDocumentation(),TRUE);
    }
    TemplateVariant detailedProperties() const
    {
      return getMemberList(getCache().detailedProperties,MemberListType_docPropMembers,theTranslator->trPropertyDocumentation(),TRUE);
    }
    TemplateVariant detailedFriends() const
    {
      return getMemberList(getCache().detailedFriends,MemberListType_docFriendMembers,theTranslator->trFriends(),TRUE);
    }
    TemplateVariant inlineClasses() const
    {
      Cachable &cache = getCache();
      if (!cache.inlineClasses)
      {
        TemplateListPtr classList = TemplateList::alloc();
        for (const auto &cd : m_groupDef->getClasses())
        {
          if (!cd->isAnonymous() &&
              cd->isLinkableInProject() &&
              cd->isEmbeddedInOuterScope() &&
              cd->partOfGroups().empty())
          {
            classList->append(ClassContext::alloc(cd));
          }
        }
        cache.inlineClasses = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(cache.inlineClasses));
    }
    TemplateVariant compoundType() const
    {
      return "module"; //theTranslator->trGroup(FALSE,TRUE);
    }
  private:
    const GroupDef *m_groupDef;
    struct Cachable : public DefinitionContext<ModuleContext::Private>::Cachable
    {
      Cachable(const GroupDef *gd) : DefinitionContext<ModuleContext::Private>::Cachable(gd) {}
      TemplateListPtr       modules;
      TemplateListPtr       dirs;
      TemplateListPtr       files;
      TemplateListPtr       classes;
      TemplateListPtr       namespaces;
      TemplateListPtr       constantgroups;
      TemplateListPtr       examples;
      TemplateListPtr       pages;
      TemplateStructIntfPtr macros;
      TemplateStructIntfPtr typedefs;
      TemplateStructIntfPtr enums;
      TemplateStructIntfPtr enumValues;
      TemplateStructIntfPtr functions;
      TemplateStructIntfPtr variables;
      TemplateStructIntfPtr signals;
      TemplateStructIntfPtr publicSlots;
      TemplateStructIntfPtr protectedSlots;
      TemplateStructIntfPtr privateSlots;
      TemplateStructIntfPtr events;
      TemplateStructIntfPtr properties;
      TemplateStructIntfPtr friends;
      TemplateListIntfPtr   memberGroups;
      TemplateStructIntfPtr detailedMacros;
      TemplateStructIntfPtr detailedTypedefs;
      TemplateStructIntfPtr detailedEnums;
      TemplateStructIntfPtr detailedEnumValues;
      TemplateStructIntfPtr detailedFunctions;
      TemplateStructIntfPtr detailedVariables;
      TemplateStructIntfPtr detailedSignals;
      TemplateStructIntfPtr detailedPublicSlots;
      TemplateStructIntfPtr detailedProtectedSlots;
      TemplateStructIntfPtr detailedPrivateSlots;
      TemplateStructIntfPtr detailedEvents;
      TemplateStructIntfPtr detailedProperties;
      TemplateStructIntfPtr detailedFriends;
      TemplateListPtr       inlineClasses;
      std::unique_ptr<DotGroupCollaboration>      groupGraph;
    };
    Cachable &getCache() const
    {
      Cachable *c = static_cast<Cachable*>(m_groupDef->cookie());
      assert(c!=0);
      return *c;
    }
    static PropertyMapper<ModuleContext::Private> s_inst;
};
//%% }

PropertyMapper<ModuleContext::Private> ModuleContext::Private::s_inst;

ModuleContext::ModuleContext(const GroupDef *gd) : p(std::make_unique<Private>(gd))
{
}

ModuleContext::~ModuleContext()
{
}

TemplateVariant ModuleContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector ModuleContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list ClassList[Class] : list of classes
class ClassListContext::Private : public GenericNodeListContext
{
  public:
    void addClasses(const ClassLinkedMap &classLinkedMap)
    {
      for (const auto &cd : classLinkedMap)
      {
        if (cd->getLanguage()==SrcLangExt_VHDL &&
            ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS ||
             (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
           ) // no architecture
        {
          continue;
        }
        if (cd->isLinkableInProject() && cd->templateMaster()==0 &&
            !cd->isHidden() && !cd->isEmbeddedInOuterScope())
        {
          append(ClassContext::alloc(cd.get()));
        }
      }
    }
};

ClassListContext::ClassListContext() : p(std::make_unique<Private>())
{
  p->addClasses(*Doxygen::classLinkedMap);
  p->addClasses(*Doxygen::hiddenClassLinkedMap);
}

ClassListContext::~ClassListContext()
{
}

// TemplateListIntf
uint ClassListContext::count() const
{
  return p->count();
}

TemplateVariant ClassListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr ClassListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list ClassIndex[Class] : list of classes
class ClassIndexContext::Private
{
  public:
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("list",        &Private::list);
        s_inst.addProperty("fileName",    &Private::fileName);
        s_inst.addProperty("relPath",     &Private::relPath);
        s_inst.addProperty("highlight",   &Private::highlight);
        s_inst.addProperty("subhighlight",&Private::subhighlight);
        s_inst.addProperty("title",       &Private::title);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant list() const
    {
      if (!m_cache.classes)
      {
        TemplateListPtr classList = TemplateList::alloc();
        if (Doxygen::classLinkedMap)
        {
          for (const auto &cd : *Doxygen::classLinkedMap)
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
              classList->append(ClassContext::alloc(cd.get()));
            }
          }
        }
        m_cache.classes = classList;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_cache.classes));
    }
    TemplateVariant fileName() const
    {
      return "classes";
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
      return "classindex";
    }
    TemplateVariant title() const
    {
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      if (fortranOpt)
      {
        return theTranslator->trDataTypes();
      }
      else if (vhdlOpt)
      {
        return theTranslator->trDesignUnits();
      }
      else
      {
        return theTranslator->trCompoundIndex();
      }
    }
  private:
    struct Cachable
    {
      TemplateListPtr classes;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ClassIndexContext::Private> s_inst;
};

PropertyMapper<ClassIndexContext::Private> ClassIndexContext::Private::s_inst;

ClassIndexContext::ClassIndexContext() : p(std::make_unique<Private>())
{
}

ClassIndexContext::~ClassIndexContext()
{
}

// TemplateStructIntf
TemplateVariant ClassIndexContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector ClassIndexContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

static int computeMaxDepth(const TemplateListIntfPtr list)
{
  int maxDepth=0;
  if (list)
  {
    TemplateListIntf::ConstIteratorPtr it = list->createIterator();
    TemplateVariant v;
    for (it->toFirst();it->current(v);it->toNext())
    {
      const TemplateStructIntfPtr s = v.toStruct();
      TemplateVariant child = s->get("children");
      int d = computeMaxDepth(child.toList())+1;
      if (d>maxDepth) maxDepth=d;
    }
  }
  return maxDepth;
}

static int computeNumNodesAtLevel(const TemplateStructIntfPtr s,int level,int maxLevel)
{
  int num=0;
  if (level<maxLevel)
  {
    num++;
    TemplateVariant child = s->get("children");
    if (child.toList())
    {
      TemplateListIntf::ConstIteratorPtr it = child.toList()->createIterator();
      TemplateVariant v;
      for (it->toFirst();it->current(v);it->toNext())
      {
        num+=computeNumNodesAtLevel(v.toStruct(),level+1,maxLevel);
      }
    }
  }
  return num;
}

static int computePreferredDepth(const TemplateListIntfPtr list,int maxDepth)
{
  int preferredNumEntries = Config_getInt(HTML_INDEX_NUM_ENTRIES);
  int preferredDepth=1;
  if (preferredNumEntries>0)
  {
    int depth = maxDepth;
    for (int i=1;i<=depth;i++)
    {
      int num=0;
      TemplateListIntf::ConstIteratorPtr it = list->createIterator();
      TemplateVariant v;
      for (it->toFirst();it->current(v);it->toNext())
      {
        num+=computeNumNodesAtLevel(v.toStruct(),0,i);
      }
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

//------------------------------------------------------------------------

//%% struct ClassHierarchy: inheritance tree
//%% {
class ClassHierarchyContext::Private
{
  public:
    Private()
    {
      m_classTree = NestingContext::alloc(0,ContextTreeType::ClassInheritance,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_classTree);
      ClassDefSet visitedClasses;
      ctx->addClassHierarchy(*Doxygen::classLinkedMap,visitedClasses);
      ctx->addClassHierarchy(*Doxygen::hiddenClassLinkedMap,visitedClasses);
      //%% ClassInheritance tree
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",            &Private::tree);
        s_inst.addProperty("fileName",        &Private::fileName);
        s_inst.addProperty("relPath",         &Private::relPath);
        s_inst.addProperty("highlight",       &Private::highlight);
        s_inst.addProperty("subhighlight",    &Private::subhighlight);
        s_inst.addProperty("title",           &Private::title);
        s_inst.addProperty("preferredDepth",  &Private::preferredDepth);
        s_inst.addProperty("maxDepth",        &Private::maxDepth);
        s_inst.addProperty("diagrams",        &Private::diagrams);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_classTree);
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
    DotGfxHierarchyTable *getHierarchy() const
    {
      if (!m_cache.hierarchy)
      {
        m_cache.hierarchy.reset(new DotGfxHierarchyTable());
      }
      return m_cache.hierarchy.get();
    }
    TemplateVariant diagrams() const
    {
      if (!m_cache.diagrams)
      {
        TemplateListPtr diagrams = TemplateList::alloc();
        DotGfxHierarchyTable *hierarchy = getHierarchy();
        int id=0;
        for (auto n : hierarchy->subGraphs())
        {
          diagrams->append(InheritanceGraphContext::alloc(hierarchy,n,id++));
        }
        m_cache.diagrams = diagrams;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_cache.diagrams));
    }
    TemplateVariant title() const
    {
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      if (vhdlOpt)
      {
        return theTranslator->trDesignUnitHierarchy();
      }
      else
      {
        return theTranslator->trClassHierarchy();
      }
    }
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_classTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_classTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_classTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
      TemplateListPtr diagrams;
      std::unique_ptr<DotGfxHierarchyTable> hierarchy;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ClassHierarchyContext::Private> s_inst;
};
//%% }

PropertyMapper<ClassHierarchyContext::Private> ClassHierarchyContext::Private::s_inst;

ClassHierarchyContext::ClassHierarchyContext() : p(std::make_unique<Private>())
{
}

ClassHierarchyContext::~ClassHierarchyContext()
{
}

TemplateVariant ClassHierarchyContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ClassHierarchyContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct NestingNode: node is a nesting relation tree
//%% {
class NestingNodeContext::Private
{
  public:
    Private(const NestingNodeContext *parent,const NestingNodeContext *thisNode,
        ContextTreeType type,
        const Definition *d,int index,int level,
        bool addCls,bool addCps,bool inherit, bool hideSuper,
        ClassDefSet &visitedClasses)
      : m_parent(parent), m_type(type), m_def(d), m_level(level), m_index(index)
    {
      m_children = std::dynamic_pointer_cast<NestingContext>(NestingContext::alloc(thisNode,m_type,level+1));
      m_members = std::dynamic_pointer_cast<NestingContext>(NestingContext::alloc(thisNode,m_type,level+1));
      static bool init=FALSE;
      if (!init)
      {
        //%% bool is_leaf_node: true if this node does not have any children
        s_inst.addProperty("is_leaf_node",&Private::isLeafNode);
        //%% Nesting children: list of nested classes/namespaces
        s_inst.addProperty("children",&Private::children);
        //%% Nesting children: list of nested classes/namespaces
        s_inst.addProperty("members",&Private::members);
        //%% [optional] Class class: class info (if this node represents a class)
        s_inst.addProperty("class",&Private::getClass);
        //%% [optional] Namespace namespace: namespace info (if this node represents a namespace)
        s_inst.addProperty("namespace",&Private::getNamespace);
        //%% [optional] File file: file info (if this node represents a file)
        s_inst.addProperty("file",&Private::getFile);
        //%% [optional] Dir dir: directory info (if this node represents a directory)
        s_inst.addProperty("dir",&Private::getDir);
        //%% [optional] Page page: page info (if this node represents a page)
        s_inst.addProperty("page",&Private::getPage);
        //%% [optional] Module module: module info (if this node represents a module)
        s_inst.addProperty("module",&Private::getModule);
        //%% [optional] Member member: member info (if this node represents a member)
        s_inst.addProperty("member",&Private::getMember);
        //%% int id
        s_inst.addProperty("id",&Private::id);
        //%% string level
        s_inst.addProperty("level",&Private::level);
        //%% string name
        s_inst.addProperty("name",&Private::name);
        //%% string brief
        s_inst.addProperty("brief",&Private::brief);
        //%% bool isLinkable
        s_inst.addProperty("isLinkable",&Private::isLinkable);
        s_inst.addProperty("partOfGroup",&Private::partOfGroup);
        s_inst.addProperty("anchor",&Private::anchor);
        s_inst.addProperty("fileName",&Private::fileName);
        s_inst.addProperty("isReference",&Private::isReference);
        s_inst.addProperty("externalReference",&Private::externalReference);
        init=TRUE;
      }

      addNamespaces(addCls,addCps,visitedClasses);
      addClasses(inherit,hideSuper,visitedClasses);
      addDirFiles(visitedClasses);
      addPages(visitedClasses);
      addModules(visitedClasses);
      addMembers(visitedClasses);
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant isLeafNode() const
    {
      return m_children->count()==0;
    }
    TemplateVariant children() const
    {
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_children));
    }
    TemplateVariant members() const
    {
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_members));
    }
    TemplateVariant getClass() const
    {
      if (!m_cache.classContext && m_def->definitionType()==Definition::TypeClass)
      {
        m_cache.classContext = ClassContext::alloc(toClassDef(m_def));
      }
      if (m_cache.classContext)
      {
        return TemplateVariant(m_cache.classContext);
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
        m_cache.namespaceContext = NamespaceContext::alloc(toNamespaceDef(m_def));
      }
      if (m_cache.namespaceContext)
      {
        return TemplateVariant(m_cache.namespaceContext);
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
        m_cache.dirContext = DirContext::alloc(toDirDef(m_def));
      }
      if (m_cache.dirContext)
      {
        return TemplateVariant(m_cache.dirContext);
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
        m_cache.fileContext = FileContext::alloc(toFileDef(m_def));
      }
      if (m_cache.fileContext)
      {
        return TemplateVariant(m_cache.fileContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getPage() const
    {
      if (!m_cache.pageContext && m_def->definitionType()==Definition::TypePage)
      {
        m_cache.pageContext = PageContext::alloc(toPageDef(m_def),FALSE,FALSE);
      }
      if (m_cache.pageContext)
      {
        return TemplateVariant(m_cache.pageContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getModule() const
    {
      if (!m_cache.moduleContext && m_def->definitionType()==Definition::TypeGroup)
      {
        m_cache.moduleContext = ModuleContext::alloc(toGroupDef(m_def));
      }
      if (m_cache.moduleContext)
      {
        return TemplateVariant(m_cache.moduleContext);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
    TemplateVariant getMember() const
    {
      if (!m_cache.memberContext && m_def->definitionType()==Definition::TypeMember)
      {
        m_cache.memberContext = MemberContext::alloc(toMemberDef(m_def));
      }
      if (m_cache.memberContext)
      {
        return TemplateVariant(m_cache.memberContext);
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
      if (m_def->definitionType()==Definition::TypeMember && m_type==ContextTreeType::Module)
      {
        return toMemberDef(m_def)->qualifiedName();
      }
      else
      {
        return m_def->displayName(m_type==ContextTreeType::Module);
      }
    }
    TemplateVariant partOfGroup() const
    {
      return m_def->definitionType()==Definition::TypeMember ? toMemberDef(m_def)->getGroupDef()!=0 :
             !m_def->partOfGroups().empty();
    }
    QCString relPathAsString() const
    {
      static bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
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
    TemplateVariant isReference() const
    {
      return m_def->isReference();
    }
    TemplateVariant externalReference() const
    {
      return m_def->externalReference(relPathAsString());
    }

    //------------------------------------------------------------------

    void addClasses(bool inherit, bool hideSuper,ClassDefSet &visitedClasses)
    {
      const ClassDef *cd = toClassDef(m_def);
      if (cd)
      {
        if (inherit)
        {
          bool hasChildren = visitedClasses.find(cd)==visitedClasses.end() &&
            !hideSuper && classHasVisibleChildren(cd);
          if (hasChildren)
          {
            visitedClasses.insert(cd);
            if (cd->getLanguage()==SrcLangExt_VHDL)
            {
              m_children->addDerivedClasses(cd->baseClasses(),false,visitedClasses);
            }
            else
            {
              m_children->addDerivedClasses(cd->subClasses(),false,visitedClasses);
            }
          }
        }
        else
        {
          m_children->addClasses(cd->getClasses(),FALSE,visitedClasses);
        }
      }
      const GroupDef *gd = toGroupDef(m_def);
      if (gd)
      {
        m_children->addClasses(gd->getClasses(),FALSE,visitedClasses);
      }
    }
    void addConcepts(ClassDefSet &visitedClasses)
    {
      const GroupDef *gd = toGroupDef(m_def);
      if (gd)
      {
        m_children->addConcepts(gd->getConcepts(),FALSE,visitedClasses);
      }
    }
    void addNamespaces(bool addClasses,bool addConcepts,ClassDefSet &visitedClasses)
    {
      const NamespaceDef *nd = toNamespaceDef(m_def);
      if (nd)
      {
        if (!nd->getNamespaces().empty())
        {
          m_children->addNamespaces(nd->getNamespaces(),FALSE,addClasses,addConcepts,visitedClasses);
        }
        if (addClasses)
        {
          m_children->addClasses(nd->getClasses(),FALSE,visitedClasses);
        }
        if (addConcepts)
        {
          m_children->addConcepts(nd->getConcepts(),FALSE,visitedClasses);
        }
      }
      const GroupDef *gd = toGroupDef(m_def);
      if (gd)
      {
        m_children->addConcepts(gd->getConcepts(),false,visitedClasses);
      }
    }
    void addDirFiles(ClassDefSet &visitedClasses)
    {
      const DirDef *dd = toDirDef(m_def);
      if (dd)
      {
        m_children->addDirs(dd->subDirs(),visitedClasses);
        m_children->addFiles(dd->getFiles(),visitedClasses);
      }
      const GroupDef *gd = toGroupDef(m_def);
      if (gd)
      {
        m_children->addDirs(gd->getDirs(),visitedClasses);
      }
    }
    void addPages(ClassDefSet &visitedClasses)
    {
      const PageDef *pd = toPageDef(m_def);
      if (pd && !pd->getSubPages().empty())
      {
        m_children->addPages(pd->getSubPages(),FALSE,visitedClasses);
      }
      const GroupDef *gd = toGroupDef(m_def);
      if (gd)
      {
        m_children->addPages(gd->getPages(),FALSE,visitedClasses);
      }
    }
    void addModules(ClassDefSet &visitedClasses)
    {
      const GroupDef *gd = toGroupDef(m_def);
      if (gd && !gd->getSubGroups().empty())
      {
        m_children->addModules(gd->getSubGroups(),visitedClasses);
      }
    }
    void addMembers(ClassDefSet &visitedClasses)
    {
      if (m_type!=ContextTreeType::Module && m_def->partOfGroups().empty()) // show outside of group
      {
        if (m_def->definitionType()==Definition::TypeNamespace)
        {
          // add namespace members
          for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
          {
            if (lde->kind()==LayoutDocEntry::MemberDef)
            {
              const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
              const MemberList *ml = toNamespaceDef(m_def)->getMemberList(lmd->type);
              if (ml)
              {
                m_members->addMembers(*ml,visitedClasses);
              }
            }
          }
        }
        else if (m_def->definitionType()==Definition::TypeClass)
        {
          // add class members
          for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
          {
            if (lde->kind()==LayoutDocEntry::MemberDef)
            {
              const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
              const MemberList *ml = toClassDef(m_def)->getMemberList(lmd->type);
              if (ml)
              {
                m_members->addMembers(*ml,visitedClasses);
              }
            }
          }
        }
        else if (m_def->definitionType()==Definition::TypeFile)
        {
          // add class members
          for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
          {
            if (lde->kind()==LayoutDocEntry::MemberDef)
            {
              const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
              const MemberList *ml = toFileDef(m_def)->getMemberList(lmd->type);
              if (ml)
              {
                m_members->addMembers(*ml,visitedClasses);
              }
            }
          }
        }
      }
      else if (m_def->definitionType()==Definition::TypeGroup && m_type==ContextTreeType::Module) // show as part of group
      {
        // add class members
        for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
        {
          if (lde->kind()==LayoutDocEntry::MemberDef)
          {
            const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
            const MemberList *ml = toGroupDef(m_def)->getMemberList(lmd->type);
            if (ml)
            {
              m_members->addMembers(*ml,visitedClasses);
            }
          }
        }
      }
      if (m_def->definitionType()==Definition::TypeMember)
      {
        const MemberDef *md = toMemberDef(m_def);
        if (md->isEnumerate() && md->isStrong())
        {
          m_members->addMembers(md->enumFieldList(),visitedClasses);
        }
      }
    }
  private:
    const NestingNodeContext *m_parent;
    ContextTreeType m_type;
    const Definition *m_def;
    std::shared_ptr<NestingContext> m_children;
    std::shared_ptr<NestingContext> m_members;
    int m_level;
    int m_index;
    struct Cachable
    {
      TemplateStructIntfPtr classContext;
      TemplateStructIntfPtr namespaceContext;
      TemplateStructIntfPtr dirContext;
      TemplateStructIntfPtr fileContext;
      TemplateStructIntfPtr pageContext;
      TemplateStructIntfPtr moduleContext;
      TemplateStructIntfPtr memberContext;
      std::unique_ptr<TemplateVariant>  brief;
    };
    mutable Cachable m_cache;
    static PropertyMapper<NestingNodeContext::Private> s_inst;
};
//%% }

PropertyMapper<NestingNodeContext::Private> NestingNodeContext::Private::s_inst;

NestingNodeContext::NestingNodeContext(const NestingNodeContext *parent,
                                       ContextTreeType type,
                                       const Definition *d,int index,int level,
                                       bool addClass,bool addConcepts,
                                       bool inherit,bool hideSuper,
                                       ClassDefSet &visitedClasses)
  : p(std::make_unique<Private>(parent,this,type,d,index,level,addClass,addConcepts,inherit,hideSuper,visitedClasses))
{
}

NestingNodeContext::~NestingNodeContext()
{
}

TemplateVariant NestingNodeContext::get(const QCString &n) const
{
  return p->get(n);
}

StringVector NestingNodeContext::fields() const
{
  return p->fields();
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
    Private(const NestingNodeContext *parent,ContextTreeType type,int level)
      : m_parent(parent), m_type(type), m_level(level) {}

    void addNamespace(const NamespaceDef *nd,bool rootOnly,bool addClasses,bool addConcepts,
                      ClassDefSet &visitedClasses)
    {
      if (!nd->isAnonymous() &&
          (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
      {
        bool hasChildren = namespaceHasNestedNamespace(nd) ||
                           (addClasses  && namespaceHasNestedClass(nd,false,ClassDef::Class)) ||
                           (addConcepts && namespaceHasNestedConcept(nd)) ||
                           (m_type==ContextTreeType::Namespace && countVisibleMembers(nd));
        bool isLinkable  = nd->isLinkableInProject();
        if (isLinkable && hasChildren)
        {
          append(NestingNodeContext::alloc(m_parent,m_type,nd,m_index,m_level,
                                           addClasses,addConcepts,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addNamespaces(const NamespaceLinkedMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses)
    {
      for (const auto &nd : nsLinkedMap)
      {
        addNamespace(nd.get(),rootOnly,addClasses,addConcepts,visitedClasses);
      }
    }
    void addNamespaces(const NamespaceLinkedRefMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses)
    {
      for (const auto &nd : nsLinkedMap)
      {
        addNamespace(nd,rootOnly,addClasses,addConcepts,visitedClasses);
      }
    }
    void addClass(const ClassDef *cd,bool rootOnly,ClassDefSet &visitedClasses)
    {
      if (cd->getLanguage()==SrcLangExt_VHDL)
      {
        if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS ||
            (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS
           )// no architecture
        {
          return;
        }
      }
      if (!rootOnly ||
          cd->getOuterScope()==0 ||
          cd->getOuterScope()==Doxygen::globalScope
         )
      {
        if (classVisibleInIndex(cd) && cd->templateMaster()==0)
        {
          append(NestingNodeContext::alloc(m_parent,m_type,cd,m_index,m_level,
                                           TRUE,FALSE,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addConcept(const ConceptDef *cd,bool rootOnly,ClassDefSet &visitedClasses)
    {
      if (!rootOnly || cd->getOuterScope()==0 || cd->getOuterScope()==Doxygen::globalScope)
      {
        if (cd->isLinkable())
        {
          append(NestingNodeContext::alloc(m_parent,m_type,cd,m_index,m_level,
                                           FALSE,TRUE,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addClasses(const ClassLinkedRefMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &cd : clLinkedMap)
      {
        addClass(cd,rootOnly,visitedClasses);
      }
    }
    void addClasses(const ClassLinkedMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &cd : clLinkedMap)
      {
        addClass(cd.get(),rootOnly,visitedClasses);
      }
    }
    void addConcepts(const ConceptLinkedRefMap &cpLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &cd : cpLinkedMap)
      {
        addConcept(cd,rootOnly,visitedClasses);
      }
    }
    void addConcepts(const ConceptLinkedMap &cpLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &cd : cpLinkedMap)
      {
        addConcept(cd.get(),rootOnly,visitedClasses);
      }
    }
    void addDirs(const DirLinkedMap &dirLinkedMap,ClassDefSet &visitedClasses)
    {
      for (const auto &dd : dirLinkedMap)
      {
        if (dd->getOuterScope()==Doxygen::globalScope)
        {
          append(NestingNodeContext::alloc(m_parent,m_type,dd.get(),m_index,m_level,
                                           FALSE,FALSE,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addDirs(const DirList &dirList,ClassDefSet &visitedClasses)
    {
      for(const auto dd : dirList)
      {
        append(NestingNodeContext::alloc(m_parent,m_type,dd,m_index,m_level,
                                         FALSE,FALSE,FALSE,FALSE,visitedClasses));
        m_index++;
      }
    }
    void addFiles(const FileNameLinkedMap &fnList,ClassDefSet &visitedClasses)
    {
      for (const FileNameLinkedMap::Ptr &fn : fnList)
      {
        for (const auto &fd : *fn)
        {
          if (fd->getDirDef()==0) // top level file
          {
            append(NestingNodeContext::alloc(m_parent,m_type,fd.get(),m_index,m_level,
                                             FALSE,FALSE,FALSE,FALSE,visitedClasses));
            m_index++;
          }
        }
      }
    }
    void addFiles(const FileList &fList,ClassDefSet &visitedClasses)
    {
      for (const auto &fd : fList)
      {
        append(NestingNodeContext::alloc(m_parent,m_type,fd,m_index,m_level,
                                         FALSE,FALSE,FALSE,FALSE,visitedClasses));
        m_index++;
      }
    }
    void addPage(const PageDef *pd,bool rootOnly,ClassDefSet &visitedClasses)
    {
      if (!rootOnly ||
          pd->getOuterScope()==0 ||
          pd->getOuterScope()->definitionType()!=Definition::TypePage)
      {
        append(NestingNodeContext::alloc(m_parent,m_type,pd,m_index,m_level,
                                          FALSE,FALSE,FALSE,FALSE,visitedClasses));
        m_index++;
      }
    }
    void addPages(const PageLinkedMap &pages,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &pd : pages)
      {
        addPage(pd.get(),rootOnly,visitedClasses);
      }
    }
    void addPages(const PageLinkedRefMap &pages,bool rootOnly,ClassDefSet &visitedClasses)
    {
      for (const auto &pd : pages)
      {
        addPage(pd,rootOnly,visitedClasses);
      }
    }
    void addModules(const GroupLinkedMap &groups,ClassDefSet &visitedClasses)
    {
      for (const auto &gd : groups)
      {
        bool externalGroups = Config_getBool(EXTERNAL_GROUPS);
        if (!gd->isASubGroup() && gd->isVisible() &&
            (!gd->isReference() || externalGroups)
           )
        {
          append(NestingNodeContext::alloc(m_parent,m_type,gd.get(),m_index,m_level,
                                           FALSE,FALSE,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addModules(const GroupList &groups,ClassDefSet &visitedClasses)
    {
      for (const auto &gd : groups)
      {
        if (gd->isVisible())
        {
          append(NestingNodeContext::alloc(m_parent,m_type,gd,m_index,m_level,
                                           FALSE,FALSE,FALSE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }
    void addDerivedClasses(const BaseClassList &bcl,bool hideSuper,ClassDefSet &visitedClasses)
    {
      for (const auto &bcd : bcl)
      {
        const ClassDef *cd=bcd.classDef;
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

        if (cd->isVisibleInHierarchy() && b)
        {
          append(NestingNodeContext::alloc(m_parent,m_type,cd,m_index,m_level,
                                           TRUE,FALSE,TRUE,hideSuper,visitedClasses));
          m_index++;
        }
      }
    }
    void addClassHierarchy(const ClassLinkedMap &classLinkedMap,ClassDefSet &visitedClasses)
    {
      for (const auto &cd : classLinkedMap)
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
            append(NestingNodeContext::alloc(m_parent,m_type,cd.get(),m_index,m_level,
                                             TRUE,FALSE,TRUE,FALSE,visitedClasses));
            m_index++;
          }
        }
      }
    }
    void addMembers(const MemberVector &mv,ClassDefSet &visitedClasses)
    {
      for (const auto &md : mv)
      {
        if (md->visibleInIndex())
        {
          append(NestingNodeContext::alloc(m_parent,m_type,md,m_index,m_level+1,
                                           TRUE,FALSE,TRUE,FALSE,visitedClasses));
          m_index++;
        }
      }
    }

  private:
    const NestingNodeContext *m_parent;
    ContextTreeType m_type;
    int m_level;
    int m_index = 0;
};

NestingContext::NestingContext(const NestingNodeContext *parent,ContextTreeType type,int level) : p(std::make_unique<Private>(parent,type,level))
{
}

NestingContext::~NestingContext()
{
}

// TemplateListIntf
uint NestingContext::count() const
{
  return p->count();
}

TemplateVariant NestingContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr NestingContext::createIterator() const
{
  return p->createIterator();
}

void NestingContext::addClasses(const ClassLinkedRefMap &clLinkedRefMap,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addClasses(clLinkedRefMap,rootOnly,visitedClasses);
}

void NestingContext::addClasses(const ClassLinkedMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addClasses(clLinkedMap,rootOnly,visitedClasses);
}

void NestingContext::addConcepts(const ConceptLinkedRefMap &cpLinkedRefMap,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addConcepts(cpLinkedRefMap,rootOnly,visitedClasses);
}

void NestingContext::addConcepts(const ConceptLinkedMap &cpLinkedMap,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addConcepts(cpLinkedMap,rootOnly,visitedClasses);
}

void NestingContext::addNamespaces(const NamespaceLinkedMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses)
{
  p->addNamespaces(nsLinkedMap,rootOnly,addClasses,addConcepts,visitedClasses);
}

void NestingContext::addNamespaces(const NamespaceLinkedRefMap &nsLinkedRefMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses)
{
  p->addNamespaces(nsLinkedRefMap,rootOnly,addClasses,addConcepts,visitedClasses);
}

void NestingContext::addDirs(const DirLinkedMap &dirs,ClassDefSet &visitedClasses)
{
  p->addDirs(dirs,visitedClasses);
}

void NestingContext::addDirs(const DirList &dirs,ClassDefSet &visitedClasses)
{
  p->addDirs(dirs,visitedClasses);
}

void NestingContext::addFiles(const FileNameLinkedMap &files,ClassDefSet &visitedClasses)
{
  p->addFiles(files,visitedClasses);
}

void NestingContext::addFiles(const FileList &files,ClassDefSet &visitedClasses)
{
  p->addFiles(files,visitedClasses);
}

void NestingContext::addPages(const PageLinkedMap &pages,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addPages(pages,rootOnly,visitedClasses);
}

void NestingContext::addPages(const PageLinkedRefMap &pages,bool rootOnly,ClassDefSet &visitedClasses)
{
  p->addPages(pages,rootOnly,visitedClasses);
}

void NestingContext::addModules(const GroupLinkedMap &modules,ClassDefSet &visitedClasses)
{
  p->addModules(modules,visitedClasses);
}

void NestingContext::addModules(const GroupList &modules,ClassDefSet &visitedClasses)
{
  p->addModules(modules,visitedClasses);
}

void NestingContext::addClassHierarchy(const ClassLinkedMap &classLinkedMap,ClassDefSet &visitedClasses)
{
  p->addClassHierarchy(classLinkedMap,visitedClasses);
}

void NestingContext::addDerivedClasses(const BaseClassList &bcl,bool hideSuper,ClassDefSet &visitedClasses)
{
  p->addDerivedClasses(bcl,hideSuper,visitedClasses);
}

void NestingContext::addMembers(const MemberVector &mv,ClassDefSet &visitedClasses)
{
  p->addMembers(mv,visitedClasses);
}


//------------------------------------------------------------------------

//%% struct ClassTree: Class nesting relations
//%% {
class ClassTreeContext::Private
{
  public:
    Private()
    {
      m_classTree = NestingContext::alloc(0,ContextTreeType::ClassNesting,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_classTree);
      ClassDefSet visitedClasses;
      ctx->addNamespaces(*Doxygen::namespaceLinkedMap,TRUE,TRUE,FALSE,visitedClasses);
      ctx->addClasses(*Doxygen::classLinkedMap,TRUE,visitedClasses);
      //%% Nesting tree
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_classTree);
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
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      if (fortranOpt)
      {
        return theTranslator->trCompoundListFortran();
      }
      else if (vhdlOpt)
      {
        return theTranslator->trDesignUnitList();
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
        m_cache.maxDepth = computeMaxDepth(m_classTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_classTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_classTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ClassTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<ClassTreeContext::Private> ClassTreeContext::Private::s_inst;

ClassTreeContext::ClassTreeContext() : p(std::make_unique<Private>())
{
}

ClassTreeContext::~ClassTreeContext()
{
}

TemplateVariant ClassTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ClassTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list ConceptList[Concept] : list of namespaces
class ConceptListContext::Private : public GenericNodeListContext
{
  public:
    void addConcepts(const ConceptLinkedMap &nsLinkedMap)
    {
      for (const auto &cd : nsLinkedMap)
      {
        if (cd->isLinkableInProject())
        {
          append(ConceptContext::alloc(cd.get()));
        }
      }
    }
};

ConceptListContext::ConceptListContext() : p(std::make_unique<Private>())
{
  p->addConcepts(*Doxygen::conceptLinkedMap);
}

ConceptListContext::~ConceptListContext()
{
}

// TemplateListIntf
uint ConceptListContext::count() const
{
  return p->count();
}

TemplateVariant ConceptListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr ConceptListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list NamespaceList[Namespace] : list of namespaces
class NamespaceListContext::Private : public GenericNodeListContext
{
  public:
    void addNamespaces(const NamespaceLinkedMap &nsLinkedMap)
    {
      for (const auto &nd : nsLinkedMap)
      {
        if (nd->isLinkableInProject())
        {
          append(NamespaceContext::alloc(nd.get()));
        }
      }
    }
};

NamespaceListContext::NamespaceListContext() : p(std::make_unique<Private>())
{
  p->addNamespaces(*Doxygen::namespaceLinkedMap);
}

NamespaceListContext::~NamespaceListContext()
{
}

// TemplateListIntf
uint NamespaceListContext::count() const
{
  return p->count();
}

TemplateVariant NamespaceListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr NamespaceListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct NamespaceTree: tree of nested namespace
//%% {
class NamespaceTreeContext::Private
{
  public:
    Private()
    {
      m_namespaceTree = NestingContext::alloc(0,ContextTreeType::Namespace,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_namespaceTree);
      ClassDefSet visitedClasses;
      ctx->addNamespaces(*Doxygen::namespaceLinkedMap,TRUE,FALSE,TRUE,visitedClasses);
      //%% Nesting tree
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_namespaceTree);
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
      static bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      if (javaOpt || vhdlOpt)
      {
        return theTranslator->trPackages();
      }
      else if (fortranOpt || sliceOpt)
      {
        return theTranslator->trModulesList();
      }
      else
      {
        return theTranslator->trNamespaceList();
      }
    }
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_namespaceTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_namespaceTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_namespaceTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<NamespaceTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<NamespaceTreeContext::Private> NamespaceTreeContext::Private::s_inst;

NamespaceTreeContext::NamespaceTreeContext() : p(std::make_unique<Private>())
{
}

NamespaceTreeContext::~NamespaceTreeContext()
{
}

TemplateVariant NamespaceTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector NamespaceTreeContext::fields() const
{
  return p->fields();
}


//------------------------------------------------------------------------

//%% list FileList[File] : list of files
class FileListContext::Private : public GenericNodeListContext
{
  public:
    void addFiles(const FileNameLinkedMap &fnMap)
    {
      // TODO: if FULL_PATH_NAMES is enabled, the ordering should be dir+file
      for (const auto &fn : fnMap)
      {
        for (const auto &fd : *fn)
        {
          bool doc = fd->isLinkableInProject();
          bool src = fd->generateSourceFile();
          bool nameOk = !fd->isDocumentationFile();
          if (nameOk && (doc || src) && !fd->isReference())
          {
            append(FileContext::alloc(fd.get()));
          }
        }
      }
    }
};

FileListContext::FileListContext() : p(std::make_unique<Private>())
{
  if (Doxygen::inputNameLinkedMap) p->addFiles(*Doxygen::inputNameLinkedMap);
}

FileListContext::~FileListContext()
{
}

// TemplateListIntf
uint FileListContext::count() const
{
  return p->count();
}

TemplateVariant FileListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr FileListContext::createIterator() const
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
      for (const auto &dir : *Doxygen::dirLinkedMap)
      {
        append(DirContext::alloc(dir.get()));
      }
    }
};

DirListContext::DirListContext() : p(std::make_unique<Private>())
{
}

DirListContext::~DirListContext()
{
}

// TemplateListIntf
uint DirListContext::count() const
{
  return p->count();
}

TemplateVariant DirListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr DirListContext::createIterator() const
{
  return p->createIterator();
}


//------------------------------------------------------------------------

//%% list UsedFiles[File] : list of files
class UsedFilesContext::Private : public GenericNodeListContext
{
  public:
    void addFile(const FileDef *fd)
    {
      append(FileContext::alloc(fd));
    }
};

UsedFilesContext::UsedFilesContext(const ClassDef *cd) : p(std::make_unique<Private>())
{
  if (cd)
  {
    for (const auto &fd : cd->usedFiles())
    {
      p->addFile(fd);
    }
  }
}

UsedFilesContext::~UsedFilesContext()
{
}

// TemplateListIntf
uint UsedFilesContext::count() const
{
  return p->count();
}

TemplateVariant UsedFilesContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr UsedFilesContext::createIterator() const
{
  return p->createIterator();
}

void UsedFilesContext::addFile(const FileDef *fd)
{
  p->addFile(fd);
}

//------------------------------------------------------------------------

//%% struct FileTree: tree of directories and files
//%% {
class FileTreeContext::Private
{
  public:
    Private()
    {
      // Add dirs tree
      m_dirFileTree = NestingContext::alloc(0,ContextTreeType::File,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_dirFileTree);
      ClassDefSet visitedClasses;
      ctx->addDirs(*Doxygen::dirLinkedMap,visitedClasses);
      if (Doxygen::inputNameLinkedMap)
      {
        ctx->addFiles(*Doxygen::inputNameLinkedMap,visitedClasses);
      }
      //%% DirFile tree:
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_dirFileTree);
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
        m_cache.maxDepth = computeMaxDepth(m_dirFileTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_dirFileTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_dirFileTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<FileTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<FileTreeContext::Private> FileTreeContext::Private::s_inst;

FileTreeContext::FileTreeContext() : p(std::make_unique<Private>())
{
}

FileTreeContext::~FileTreeContext()
{
}

TemplateVariant FileTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector FileTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct PageTree: tree of related pages
//%% {
class PageTreeContext::Private
{
  public:
    Private(const PageLinkedMap &pages)
    {
      m_pageTree = NestingContext::alloc(0,ContextTreeType::Page,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_pageTree);
      ClassDefSet visitedClasses;
      // Add pages
      ctx->addPages(pages,TRUE,visitedClasses);

      //%% PageNodeList tree:
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_pageTree);
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
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_pageTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_pageTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_pageTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<PageTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<PageTreeContext::Private> PageTreeContext::Private::s_inst;

PageTreeContext::PageTreeContext(const PageLinkedMap &pages) : p(std::make_unique<Private>(pages))
{
}

PageTreeContext::~PageTreeContext()
{
}

TemplateVariant PageTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector PageTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list PageList[Page]: list of pages
class PageListContext::Private : public GenericNodeListContext
{
  public:
    void addPages(const PageLinkedMap &pages)
    {
      for (const auto &pd : pages)
      {
        if (!pd->getGroupDef() && !pd->isReference())
        {
          append(PageContext::alloc(pd.get(),FALSE,FALSE));
        }
      }
    }
};

PageListContext::PageListContext(const PageLinkedMap &pages) : p(std::make_unique<Private>())
{
  p->addPages(pages);
}

PageListContext::~PageListContext()
{
}

// TemplateListIntf
uint PageListContext::count() const
{
  return p->count();
}

TemplateVariant PageListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr PageListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list ExampleList[Page]: list of pages
class ExampleListContext::Private : public GenericNodeListContext
{
  public:
    Private()
    {
      for (const auto &pd : *Doxygen::exampleLinkedMap)
      {
        if (!pd->getGroupDef() && !pd->isReference())
        {
          append(PageContext::alloc(pd.get(),FALSE,TRUE));
        }
      }
    }
};

ExampleListContext::ExampleListContext() : p(std::make_unique<Private>())
{
}

ExampleListContext::~ExampleListContext()
{
}

// TemplateListIntf
uint ExampleListContext::count() const
{
  return p->count();
}

TemplateVariant ExampleListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr ExampleListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list ModuleList[ModuleNode]: list of directories and/or files
class ModuleListContext::Private : public GenericNodeListContext
{
  public:
    void addModules()
    {
      for (const auto &gd : *Doxygen::groupLinkedMap)
      {
        if (!gd->isReference())
        {
          append(ModuleContext::alloc(gd.get()));
        }
      }
    }
};

ModuleListContext::ModuleListContext() : p(std::make_unique<Private>())
{
  p->addModules();
}

ModuleListContext::~ModuleListContext()
{
}

// TemplateListIntf
uint ModuleListContext::count() const
{
  return p->count();
}

TemplateVariant ModuleListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr ModuleListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct ConceptTree: tree of modules
//%% {
class ConceptTreeContext::Private
{
  public:
    Private()
    {
      m_conceptTree = NestingContext::alloc(0,ContextTreeType::Concept,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_conceptTree);
      ClassDefSet visitedClasses;
      // Add concepts
      ctx->addNamespaces(*Doxygen::namespaceLinkedMap,TRUE,FALSE,TRUE,visitedClasses);
      ctx->addConcepts(*Doxygen::conceptLinkedMap,TRUE,visitedClasses);

      //%% ConceptList tree:
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_conceptTree);
    }
    TemplateVariant fileName() const
    {
      return "concepts";
    }
    TemplateVariant relPath() const
    {
      return "";
    }
    TemplateVariant highlight() const
    {
      return "concepts";
    }
    TemplateVariant subhighlight() const
    {
      return "";
    }
    TemplateVariant title() const
    {
      return theTranslator->trConcept(true,false);
    }
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_conceptTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_conceptTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_conceptTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ConceptTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<ConceptTreeContext::Private> ConceptTreeContext::Private::s_inst;

ConceptTreeContext::ConceptTreeContext() : p(std::make_unique<Private>())
{
}

ConceptTreeContext::~ConceptTreeContext()
{
}

TemplateVariant ConceptTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ConceptTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct ModuleTree: tree of modules
//%% {
class ModuleTreeContext::Private
{
  public:
    Private()
    {
      m_moduleTree = NestingContext::alloc(0,ContextTreeType::Module,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_moduleTree);
      ClassDefSet visitedClasses;
      // Add modules
      ctx->addModules(*Doxygen::groupLinkedMap,visitedClasses);

      //%% ModuleList tree:
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_moduleTree);
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
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_moduleTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_moduleTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_moduleTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ModuleTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<ModuleTreeContext::Private> ModuleTreeContext::Private::s_inst;

ModuleTreeContext::ModuleTreeContext() : p(std::make_unique<Private>())
{
}

ModuleTreeContext::~ModuleTreeContext()
{
}

TemplateVariant ModuleTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ModuleTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct NavPathElem: list of examples page
//%% {
class NavPathElemContext::Private
{
  public:
    Private(const Definition *def) : m_def(def)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("isLinkable",       &Private::isLinkable);
        s_inst.addProperty("fileName",         &Private::fileName);
        s_inst.addProperty("anchor",           &Private::anchor);
        s_inst.addProperty("text",             &Private::text);
        s_inst.addProperty("isReference",      &Private::isReference);
        s_inst.addProperty("externalReference",&Private::externalReference);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
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
        text = (toGroupDef(m_def))->groupTitle();
      }
      else if (type==Definition::TypePage && ((toPageDef(m_def))->hasTitle()))
      {
        text = (toPageDef(m_def))->title();
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
    TemplateVariant isReference() const
    {
      return m_def->isReference();
    }
    QCString relPathAsString() const
    {
      static bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
      return createSubdirs ? QCString("../../") : QCString("");
    }
    TemplateVariant externalReference() const
    {
      return m_def->externalReference(relPathAsString());
    }
  private:
    const Definition *m_def;
    static PropertyMapper<NavPathElemContext::Private> s_inst;
};
//%% }

PropertyMapper<NavPathElemContext::Private> NavPathElemContext::Private::s_inst;

NavPathElemContext::NavPathElemContext(const Definition *def) : p(std::make_unique<Private>(def))
{
}

NavPathElemContext::~NavPathElemContext()
{
}

TemplateVariant NavPathElemContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector NavPathElemContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct ExampleList: list of examples page
//%% {
class ExampleTreeContext::Private
{
  public:
    Private()
    {
      m_exampleTree = NestingContext::alloc(0,ContextTreeType::Example,0);
      auto ctx = std::dynamic_pointer_cast<NestingContext>(m_exampleTree);
      ClassDefSet visitedClasses;
      // Add pages
      ctx->addPages(*Doxygen::exampleLinkedMap,TRUE,visitedClasses);

      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("tree",          &Private::tree);
        s_inst.addProperty("fileName",      &Private::fileName);
        s_inst.addProperty("relPath",       &Private::relPath);
        s_inst.addProperty("highlight",     &Private::highlight);
        s_inst.addProperty("subhighlight",  &Private::subhighlight);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("preferredDepth",&Private::preferredDepth);
        s_inst.addProperty("maxDepth",      &Private::maxDepth);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant tree() const
    {
      return TemplateVariant(m_exampleTree);
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
    TemplateVariant maxDepth() const
    {
      if (!m_cache.maxDepthComputed)
      {
        m_cache.maxDepth = computeMaxDepth(m_exampleTree);
        m_cache.maxDepthComputed=TRUE;
      }
      return m_cache.maxDepth;
    }
    TemplateVariant preferredDepth() const
    {
      if (!m_cache.preferredDepthComputed)
      {
        m_cache.preferredDepth = computePreferredDepth(m_exampleTree,maxDepth().toInt());
        m_cache.preferredDepthComputed=TRUE;
      }
      return m_cache.preferredDepth;
    }
  private:
    TemplateListIntfPtr m_exampleTree;
    struct Cachable
    {
      Cachable() : maxDepth(0), maxDepthComputed(FALSE),
                   preferredDepth(0), preferredDepthComputed(FALSE) {}
      int   maxDepth;
      bool  maxDepthComputed;
      int   preferredDepth;
      bool  preferredDepthComputed;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ExampleTreeContext::Private> s_inst;
};
//%% }

PropertyMapper<ExampleTreeContext::Private> ExampleTreeContext::Private::s_inst;

ExampleTreeContext::ExampleTreeContext() : p(std::make_unique<Private>())
{
}

ExampleTreeContext::~ExampleTreeContext()
{
}

TemplateVariant ExampleTreeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ExampleTreeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct GlobalsIndex: list of examples page
//%% {
class GlobalsIndexContext::Private
{
  public:
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("all",         &Private::all);
        s_inst.addProperty("functions",   &Private::functions);
        s_inst.addProperty("variables",   &Private::variables);
        s_inst.addProperty("typedefs",    &Private::typedefs);
        s_inst.addProperty("enums",       &Private::enums);
        s_inst.addProperty("enumValues",  &Private::enumValues);
        s_inst.addProperty("macros",      &Private::macros);
        s_inst.addProperty("properties",  &Private::properties);
        s_inst.addProperty("events",      &Private::events);
        s_inst.addProperty("related",     &Private::related);
        s_inst.addProperty("fileName",    &Private::fileName);
        s_inst.addProperty("relPath",     &Private::relPath);
        s_inst.addProperty("highlight",   &Private::highlight);
        s_inst.addProperty("subhighlight",&Private::subhighlight);
        s_inst.addProperty("title",       &Private::title);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    typedef bool (MemberDef::*MemberFunc)() const;
    TemplateVariant getMembersFiltered(TemplateListPtr &listRef,MemberFunc filter) const
    {
      if (!listRef) // fill cache
      {
        TemplateListPtr list = TemplateList::alloc();
        for (const auto &mn : *Doxygen::functionNameLinkedMap)
        {
          for (const auto &md : *mn)
          {
            const FileDef *fd=md->getFileDef();
            if (fd && fd->isLinkableInProject() &&
                !md->name().isEmpty() && !md->getNamespaceDef() && md->isLinkableInProject())
            {
              if (filter==0 || (md.get()->*filter)())
              {
                list->append(MemberContext::alloc(md.get()));
              }
            }
          }
        }
        listRef = list;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(listRef));
    }
    TemplateVariant all() const
    {
      return getMembersFiltered(m_cache.all,0);
    }
    TemplateVariant functions() const
    {
      return getMembersFiltered(m_cache.functions,&MemberDef::isFunction);
    }
    TemplateVariant variables() const
    {
      return getMembersFiltered(m_cache.variables,&MemberDef::isVariable);
    }
    TemplateVariant typedefs() const
    {
      return getMembersFiltered(m_cache.typedefs,&MemberDef::isTypedef);
    }
    TemplateVariant enums() const
    {
      return getMembersFiltered(m_cache.enums,&MemberDef::isEnumerate);
    }
    TemplateVariant enumValues() const
    {
      return getMembersFiltered(m_cache.enumValues,&MemberDef::isEnumValue);
    }
    TemplateVariant macros() const
    {
      return getMembersFiltered(m_cache.macros,&MemberDef::isDefine);
    }
    TemplateVariant properties() const
    {
      return FALSE;
    }
    TemplateVariant events() const
    {
      return FALSE;
    }
    TemplateVariant related() const
    {
      return FALSE;
    }
    TemplateVariant fileName() const
    {
      return "globals";
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
      return "filemembers";
    }
    TemplateVariant title() const
    {
      return theTranslator->trFileMembers();
    }
  private:
    struct Cachable
    {
      Cachable() {}
      TemplateListPtr all;
      TemplateListPtr functions;
      TemplateListPtr variables;
      TemplateListPtr typedefs;
      TemplateListPtr enums;
      TemplateListPtr enumValues;
      TemplateListPtr macros;
    };
    mutable Cachable m_cache;
    static PropertyMapper<GlobalsIndexContext::Private> s_inst;
};
//%% }

PropertyMapper<GlobalsIndexContext::Private> GlobalsIndexContext::Private::s_inst;

GlobalsIndexContext::GlobalsIndexContext() : p(std::make_unique<Private>())
{
}

GlobalsIndexContext::~GlobalsIndexContext()
{
}

TemplateVariant GlobalsIndexContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector GlobalsIndexContext::fields() const
{
  return p->fields();
}


//------------------------------------------------------------------------

//%% struct ClassMembersIndex: list of examples page
//%% {
class ClassMembersIndexContext::Private
{
  public:
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("all",         &Private::all);
        s_inst.addProperty("functions",   &Private::functions);
        s_inst.addProperty("variables",   &Private::variables);
        s_inst.addProperty("typedefs",    &Private::typedefs);
        s_inst.addProperty("enums",       &Private::enums);
        s_inst.addProperty("enumValues",  &Private::enumValues);
        s_inst.addProperty("macros",      &Private::macros);
        s_inst.addProperty("properties",  &Private::properties);
        s_inst.addProperty("events",      &Private::events);
        s_inst.addProperty("related",     &Private::related);
        s_inst.addProperty("fileName",    &Private::fileName);
        s_inst.addProperty("relPath",     &Private::relPath);
        s_inst.addProperty("highlight",   &Private::highlight);
        s_inst.addProperty("subhighlight",&Private::subhighlight);
        s_inst.addProperty("title",       &Private::title);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    typedef bool (MemberDef::*MemberFunc)() const;
    TemplateVariant getMembersFiltered(TemplateListPtr &listRef,MemberFunc filter) const
    {
      if (!listRef) // fill cache
      {
        TemplateListPtr list = TemplateList::alloc();
        for (const auto &mn : *Doxygen::memberNameLinkedMap)
        {
          for (const auto &md : *mn)
          {
            const ClassDef *cd = md->getClassDef();
            if (cd && cd->isLinkableInProject() && cd->templateMaster()==0 &&
                md->isLinkableInProject() && !md->name().isEmpty())
            {
              if (filter==0 || (md.get()->*filter)())
              {
                list->append(MemberContext::alloc(md.get()));
              }
            }
          }
        }
        listRef = list;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(listRef));
    }
    TemplateVariant all() const
    {
      return getMembersFiltered(m_cache.all,&MemberDef::isNotFriend);
    }
    TemplateVariant functions() const
    {
      return getMembersFiltered(m_cache.functions,&MemberDef::isFunctionOrSignalSlot);
    }
    TemplateVariant variables() const
    {
      return getMembersFiltered(m_cache.variables,&MemberDef::isVariable);
    }
    TemplateVariant typedefs() const
    {
      return getMembersFiltered(m_cache.typedefs,&MemberDef::isTypedef);
    }
    TemplateVariant enums() const
    {
      return getMembersFiltered(m_cache.enums,&MemberDef::isEnumerate);
    }
    TemplateVariant enumValues() const
    {
      return getMembersFiltered(m_cache.enumValues,&MemberDef::isEnumValue);
    }
    TemplateVariant macros() const
    {
      return FALSE;
    }
    TemplateVariant properties() const
    {
      return getMembersFiltered(m_cache.properties,&MemberDef::isProperty);
    }
    TemplateVariant events() const
    {
      return getMembersFiltered(m_cache.events,&MemberDef::isEvent);
    }
    TemplateVariant related() const
    {
      return getMembersFiltered(m_cache.related,&MemberDef::isRelated);
    }
    TemplateVariant fileName() const
    {
      return "functions";
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
      return "classmembers";
    }
    TemplateVariant title() const
    {
      return theTranslator->trCompoundMembers();
    }
  private:
    struct Cachable
    {
      Cachable() {}
      TemplateListPtr all;
      TemplateListPtr functions;
      TemplateListPtr variables;
      TemplateListPtr typedefs;
      TemplateListPtr enums;
      TemplateListPtr enumValues;
      TemplateListPtr properties;
      TemplateListPtr events;
      TemplateListPtr related;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ClassMembersIndexContext::Private> s_inst;
};
//%% }

PropertyMapper<ClassMembersIndexContext::Private> ClassMembersIndexContext::Private::s_inst;

ClassMembersIndexContext::ClassMembersIndexContext() : p(std::make_unique<Private>())
{
}

ClassMembersIndexContext::~ClassMembersIndexContext()
{
}

TemplateVariant ClassMembersIndexContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ClassMembersIndexContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct NamespaceMembersIndex: list of examples page
//%% {
class NamespaceMembersIndexContext::Private
{
  public:
    Private()
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("all",         &Private::all);
        s_inst.addProperty("functions",   &Private::functions);
        s_inst.addProperty("variables",   &Private::variables);
        s_inst.addProperty("typedefs",    &Private::typedefs);
        s_inst.addProperty("enums",       &Private::enums);
        s_inst.addProperty("enumValues",  &Private::enumValues);
        s_inst.addProperty("macros",      &Private::macros);
        s_inst.addProperty("properties",  &Private::properties);
        s_inst.addProperty("events",      &Private::events);
        s_inst.addProperty("related",     &Private::related);
        s_inst.addProperty("fileName",    &Private::fileName);
        s_inst.addProperty("relPath",     &Private::relPath);
        s_inst.addProperty("highlight",   &Private::highlight);
        s_inst.addProperty("subhighlight",&Private::subhighlight);
        s_inst.addProperty("title",       &Private::title);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    typedef bool (MemberDef::*MemberFunc)() const;
    TemplateVariant getMembersFiltered(TemplateListPtr &listRef,MemberFunc filter) const
    {
      if (!listRef) // fill the cache
      {
        TemplateListPtr list = TemplateList::alloc();
        for (const auto &mn : *Doxygen::functionNameLinkedMap)
        {
          for (const auto &md : *mn)
          {
            const NamespaceDef *nd=md->getNamespaceDef();
            if (nd && nd->isLinkableInProject() &&
                !md->name().isEmpty() && md->isLinkableInProject())
            {
              if (filter==0 || (md.get()->*filter)())
              {
                list->append(MemberContext::alloc(md.get()));
              }
            }
          }
        }
        listRef = list;
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(listRef));
    }
    TemplateVariant all() const
    {
      return getMembersFiltered(m_cache.all,0);
    }
    TemplateVariant functions() const
    {
      return getMembersFiltered(m_cache.functions,&MemberDef::isFunction);
    }
    TemplateVariant variables() const
    {
      return getMembersFiltered(m_cache.variables,&MemberDef::isVariable);
    }
    TemplateVariant typedefs() const
    {
      return getMembersFiltered(m_cache.typedefs,&MemberDef::isTypedef);
    }
    TemplateVariant enums() const
    {
      return getMembersFiltered(m_cache.enums,&MemberDef::isEnumerate);
    }
    TemplateVariant enumValues() const
    {
      return getMembersFiltered(m_cache.enumValues,&MemberDef::isEnumValue);
    }
    TemplateVariant macros() const
    {
      return FALSE;
    }
    TemplateVariant properties() const
    {
      return FALSE;
    }
    TemplateVariant events() const
    {
      return FALSE;
    }
    TemplateVariant related() const
    {
      return FALSE;
    }
    TemplateVariant fileName() const
    {
      return "namespacemembers";
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
      return "namespacemembers";
    }
    TemplateVariant title() const
    {
      return theTranslator->trNamespaceMembers();
    }
  private:
    struct Cachable
    {
      Cachable() {}
      TemplateListPtr all;
      TemplateListPtr functions;
      TemplateListPtr variables;
      TemplateListPtr typedefs;
      TemplateListPtr enums;
      TemplateListPtr enumValues;
    };
    mutable Cachable m_cache;
    static PropertyMapper<NamespaceMembersIndexContext::Private> s_inst;
};
//%% }

PropertyMapper<NamespaceMembersIndexContext::Private> NamespaceMembersIndexContext::Private::s_inst;

NamespaceMembersIndexContext::NamespaceMembersIndexContext() : p(std::make_unique<Private>())
{
}

NamespaceMembersIndexContext::~NamespaceMembersIndexContext()
{
}

TemplateVariant NamespaceMembersIndexContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector NamespaceMembersIndexContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct InheritanceGraph: a connected graph representing part of the overall inheritance tree
//%% {
class InheritanceGraphContext::Private
{
  public:
    Private(DotGfxHierarchyTable *hierarchy,DotNode *n,int id) : m_hierarchy(hierarchy), m_node(n), m_id(id)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("graph",&Private::graph);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant graph() const
    {
      TextStream t;
      static bool haveDot            = Config_getBool(HAVE_DOT);
      static bool graphicalHierarchy = Config_getBool(GRAPHICAL_HIERARCHY);
      if (haveDot && graphicalHierarchy)
      {
        m_hierarchy->createGraph(m_node,t,
                          /*GOF_BITMAP,
                          EOF_Html,*/
                          g_globals.outputDir,
                          g_globals.outputDir+Portable::pathSeparator()+"inherits"+Doxygen::htmlFileExtension,
                          m_id);
      }
      return TemplateVariant(t.str().c_str(),TRUE);
    }
  private:
    DotGfxHierarchyTable *m_hierarchy;
    DotNode *m_node;
    int m_id;
    static PropertyMapper<InheritanceGraphContext::Private> s_inst;
};

PropertyMapper<InheritanceGraphContext::Private> InheritanceGraphContext::Private::s_inst;

InheritanceGraphContext::InheritanceGraphContext(DotGfxHierarchyTable *hierarchy,DotNode *n,int id) : p(std::make_unique<Private>(hierarchy,n,id))
{
}

InheritanceGraphContext::~InheritanceGraphContext()
{
}

TemplateVariant InheritanceGraphContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector InheritanceGraphContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct InheritanceNode: a class in the inheritance list
//%% {
class InheritanceNodeContext::Private
{
  public:
    Private(const ClassDef *cd,const QCString &name) : m_classDef(cd), m_name(name)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("class",&Private::getClass);
        s_inst.addProperty("name", &Private::name);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant getClass() const
    {
      if (!m_classContext)
      {
        m_classContext = ClassContext::alloc(m_classDef);
      }
      return TemplateVariant(m_classContext);
    }
    TemplateVariant name() const
    {
      return m_name;
    }
  private:
    const ClassDef *m_classDef;
    mutable TemplateStructIntfPtr m_classContext;
    QCString m_name;
    static PropertyMapper<InheritanceNodeContext::Private> s_inst;
};
//%% }

PropertyMapper<InheritanceNodeContext::Private> InheritanceNodeContext::Private::s_inst;

InheritanceNodeContext::InheritanceNodeContext(const ClassDef *cd,const QCString &name) : p(std::make_unique<Private>(cd,name))
{
}

InheritanceNodeContext::~InheritanceNodeContext()
{
}

TemplateVariant InheritanceNodeContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector InheritanceNodeContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list InheritanceList[InheritanceNode] : list of inherited classes
class InheritanceListContext::Private : public GenericNodeListContext
{
  public:
    void addClass(const ClassDef *cd,const QCString &name)
    {
      append(InheritanceNodeContext::alloc(cd,name));
    }
};

InheritanceListContext::InheritanceListContext(const BaseClassList &list, bool baseClasses) : p(std::make_unique<Private>())
{
  for (const auto &bcd : list)
  {
    const ClassDef *cd=bcd.classDef;
    QCString name;
    if (baseClasses)
    {
      name = insertTemplateSpecifierInScope(
          cd->displayName(),bcd.templSpecifiers);
    }
    else
    {
      name = cd->displayName();
    }
    //printf("InheritanceListContext: adding %s baseClass=%d\n",qPrint(name),baseClasses);
    p->addClass(cd,name);
  }
}

InheritanceListContext::~InheritanceListContext()
{
}

// TemplateListIntf
uint InheritanceListContext::count() const
{
  return p->count();
}

TemplateVariant InheritanceListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr InheritanceListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% list MemberList[Member] : list of inherited classes
class MemberListContext::Private : public GenericNodeListContext
{
  public:
    void addMember(const MemberDef *md)
    {
      append(MemberContext::alloc(md));
    }
};

MemberListContext::MemberListContext() : p(std::make_unique<Private>())
{
}

MemberListContext::MemberListContext(const MemberList *list) : p(std::make_unique<Private>())
{
  if (list)
  {
    bool details = list->listType()&MemberListType_detailedLists;
    for (const auto &md : *list)
    {
      if ((md->isBriefSectionVisible() && !details) ||
          (md->hasDetailedDescription() && details)
         )
      {
        p->addMember(md);
      }
    }
  }
}

MemberListContext::MemberListContext(const MemberVector &ml) : p(std::make_unique<Private>())
{
  for (const auto &md : ml)
  {
    p->addMember(md);
  }
}

MemberListContext::~MemberListContext()
{
}

// TemplateListIntf
uint MemberListContext::count() const
{
  return p->count();
}

TemplateVariant MemberListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr MemberListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct MemberInfo: member information
//%% {
class MemberInfoContext::Private
{
  public:
    Private(const MemberInfo *mi) : m_memberInfo(mi)
    {
      static bool init=FALSE;
      if (!init)
      {
        //%% string protection
        s_inst.addProperty("protection",    &Private::protection);
        //%% string virtualness
        s_inst.addProperty("virtualness",   &Private::virtualness);
        //%% string ambiguityScope
        s_inst.addProperty("ambiguityScope",&Private::ambiguityScope);
        //%% Member member
        s_inst.addProperty("member",        &Private::member);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant protection() const
    {
      switch (m_memberInfo->prot())
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
      switch (m_memberInfo->virt())
      {
        case ::Normal:   return "normal";
        case ::Virtual:  return "virtual";
        case ::Pure:     return "pure";
      }
      return "";
    }
    TemplateVariant ambiguityScope() const
    {
      return m_memberInfo->ambiguityResolutionScope();
    }
    TemplateVariant member() const
    {
      if (!m_member && m_memberInfo->memberDef())
      {
        m_member = MemberContext::alloc(m_memberInfo->memberDef());
      }
      if (m_member)
      {
        return TemplateVariant(m_member);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
  private:
    const MemberInfo *m_memberInfo;
    mutable TemplateStructIntfPtr m_member;
    static PropertyMapper<MemberInfoContext::Private> s_inst;
};
//%% }

PropertyMapper<MemberInfoContext::Private> MemberInfoContext::Private::s_inst;

MemberInfoContext::MemberInfoContext(const MemberInfo *mi) : p(std::make_unique<Private>(mi))
{
}

MemberInfoContext::~MemberInfoContext()
{
}

TemplateVariant MemberInfoContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector MemberInfoContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list AllMembersList[MemberList] : list of inherited classes
class AllMembersListContext::Private : public GenericNodeListContext
{
  public:
    Private(const MemberNameInfoLinkedMap &ml)
    {
      static bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
      for (auto &mni : ml)
      {
        for (auto &mi : *mni)
        {
          const MemberDef *md=mi->memberDef();
          const ClassDef  *cd=md->getClassDef();
          if (cd && !md->isAnonymous())
          {
            if ((cd->isLinkable() && md->isLinkable()) ||
                (!cd->isArtificial() && !hideUndocMembers &&
                 (protectionLevelVisible(md->protection()) || md->isFriend())
                )
               )
            {
              append(MemberInfoContext::alloc(mi.get()));
            }
          }
        }
      }
    }
};

AllMembersListContext::AllMembersListContext(const MemberNameInfoLinkedMap &ml) : p(std::make_unique<Private>(ml))
{
}

AllMembersListContext::~AllMembersListContext()
{
}

// TemplateListIntf
uint AllMembersListContext::count() const
{
  return p->count();
}

TemplateVariant AllMembersListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr AllMembersListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct MemberGroupInfo: member group information
//%% {
class MemberGroupInfoContext::Private
{
  public:
    Private(const Definition *def,const QCString &relPath,const MemberGroup *mg) :
      m_def(def),
      m_relPath(relPath),
      m_memberGroup(mg)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("members",      &Private::members);
        s_inst.addProperty("title",        &Private::groupTitle);
        s_inst.addProperty("subtitle",     &Private::groupSubtitle);
        s_inst.addProperty("anchor",       &Private::groupAnchor);
        s_inst.addProperty("memberGroups", &Private::memberGroups);
        s_inst.addProperty("docs",         &Private::docs);
        s_inst.addProperty("inherited",    &Private::inherited);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant members() const
    {
      if (!m_cache.memberListContext)
      {
        m_cache.memberListContext = MemberListContext::alloc(&m_memberGroup->members());
      }
      return TemplateVariant(m_cache.memberListContext);
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
        m_cache.memberGroups = MemberGroupListContext::alloc();
      }
      return TemplateVariant(m_cache.memberGroups);
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
    const Definition *m_def;
    QCString m_relPath;
    const MemberGroup *m_memberGroup;
    struct Cachable
    {
      TemplateListIntfPtr              memberListContext;
      TemplateListIntfPtr              memberGroups;
      std::unique_ptr<TemplateVariant> docs;
    };
    mutable Cachable m_cache;
    static PropertyMapper<MemberGroupInfoContext::Private> s_inst;
};
//%% }

PropertyMapper<MemberGroupInfoContext::Private> MemberGroupInfoContext::Private::s_inst;

MemberGroupInfoContext::MemberGroupInfoContext(const Definition *def,
       const QCString &relPath,const MemberGroup *mg) : p(std::make_unique<Private>(def,relPath,mg))
{
}

MemberGroupInfoContext::~MemberGroupInfoContext()
{
}

TemplateVariant MemberGroupInfoContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector MemberGroupInfoContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list MemberGroupList[MemberGroupInfo] : list of member groups
class MemberGroupListContext::Private : public GenericNodeListContext
{
  public:
    void addMemberGroup(const Definition *def,const QCString &relPath,const MemberGroup *mg)
    {
      append(MemberGroupInfoContext::alloc(def,relPath,mg));
    }
};

MemberGroupListContext::MemberGroupListContext() : p(std::make_unique<Private>())
{
}

MemberGroupListContext::MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupRefList &list) : p(std::make_unique<Private>())
{
  for (const auto &mg : list)
  {
    p->addMemberGroup(def,relPath,mg);
  }
}

MemberGroupListContext::MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupList &list,bool subGrouping) : p(std::make_unique<Private>())
{
  for (const auto &mg : list)
  {
    if (!mg->allMembersInSameSection() || !subGrouping)
    {
      p->addMemberGroup(def,relPath,mg.get());
    }
  }
}

MemberGroupListContext::~MemberGroupListContext()
{
}

// TemplateListIntf
uint MemberGroupListContext::count() const
{
  return p->count();
}

TemplateVariant MemberGroupListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr MemberGroupListContext::createIterator() const
{
  return p->createIterator();
}


//------------------------------------------------------------------------

//%% struct MemberListInfo: member list information
//%% {
class MemberListInfoContext::Private
{
  public:
    Private(const Definition *def,const QCString &relPath,const MemberList *ml,const QCString &title,const QCString &subtitle) :
      m_def(def),
      m_memberList(ml),
      m_relPath(relPath),
      m_title(title),
      m_subtitle(subtitle)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("members",      &Private::members);
        s_inst.addProperty("title",        &Private::title);
        s_inst.addProperty("subtitle",     &Private::subtitle);
        s_inst.addProperty("anchor",       &Private::anchor);
        s_inst.addProperty("memberGroups", &Private::memberGroups);
        s_inst.addProperty("inherited",    &Private::inherited);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant members() const
    {
      if (!m_cache.memberListContext)
      {
        m_cache.memberListContext = MemberListContext::alloc(m_memberList);
      }
      return TemplateVariant(m_cache.memberListContext);
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
        m_cache.memberGroups = MemberGroupListContext::alloc(m_def,m_relPath,m_memberList->getMemberGroupList());
      }
      return TemplateVariant(m_cache.memberGroups);
    }
    TemplateVariant inherited() const
    {
      if (!m_cache.inherited && (m_memberList->listType()&MemberListType_detailedLists)==0 &&
          m_def->definitionType()==Definition::TypeClass)
      {
        m_cache.inherited = InheritedMemberInfoListContext::alloc();
        auto ctx = std::dynamic_pointer_cast<InheritedMemberInfoListContext>(m_cache.inherited);
        ctx->addMemberList(toClassDef(m_def),m_memberList->listType(),m_title,FALSE);
      }
      if (m_cache.inherited)
      {
        return TemplateVariant(m_cache.inherited);
      }
      else
      {
        return TemplateVariant(FALSE);
      }
    }
  private:
    const Definition *m_def;
    const MemberList *m_memberList;
    QCString m_relPath;
    QCString m_title;
    QCString m_subtitle;
    struct Cachable
    {
      TemplateListIntfPtr memberListContext;
      TemplateListIntfPtr memberGroups;
      TemplateListIntfPtr inherited;
    };
    mutable Cachable m_cache;
    static PropertyMapper<MemberListInfoContext::Private> s_inst;
};
//%% }

PropertyMapper<MemberListInfoContext::Private> MemberListInfoContext::Private::s_inst;

MemberListInfoContext::MemberListInfoContext(
           const Definition *def,const QCString &relPath,const MemberList *ml,
           const QCString &title,const QCString &subtitle) : p(std::make_unique<Private>(def,relPath,ml,title,subtitle))
{
}

MemberListInfoContext::~MemberListInfoContext()
{
}

TemplateVariant MemberListInfoContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector MemberListInfoContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% struct InheritedMemberInfo: inherited member information
//%% {
class InheritedMemberInfoContext::Private
{
  public:
    Private(const ClassDef *cd,std::unique_ptr<MemberList> &&ml,const QCString &title)
      : m_class(cd), m_memberList(std::move(ml)), m_title(title)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("class",         &Private::getClass);
        s_inst.addProperty("title",         &Private::title);
        s_inst.addProperty("members",       &Private::members);
        s_inst.addProperty("id",            &Private::id);
        s_inst.addProperty("inheritedFrom", &Private::inheritedFrom);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant getClass() const
    {
      if (!m_classCtx)
      {
        m_classCtx = ClassContext::alloc(m_class);
      }
      return TemplateVariant(m_classCtx);
    }
    TemplateVariant title() const
    {
      return m_title;
    }
    TemplateVariant members() const
    {
      if (!m_memberListCtx)
      {
        m_memberListCtx = MemberListContext::alloc(m_memberList.get());
      }
      return TemplateVariant(m_memberListCtx);
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
        m_inheritedFrom = TemplateList::alloc();
        m_inheritedFrom->append(title());
        m_inheritedFrom->append(getClass());
      }
      return TemplateVariant(std::static_pointer_cast<TemplateListIntf>(m_inheritedFrom));
    }

  private:
    const ClassDef *  m_class;
    std::unique_ptr<const MemberList> m_memberList;
    QCString    m_title;
    mutable TemplateStructIntfPtr m_classCtx;
    mutable TemplateListIntfPtr   m_memberListCtx;
    mutable TemplateListPtr m_inheritedFrom;
    static PropertyMapper<InheritedMemberInfoContext::Private> s_inst;
};
//%% }

PropertyMapper<InheritedMemberInfoContext::Private> InheritedMemberInfoContext::Private::s_inst;

InheritedMemberInfoContext::InheritedMemberInfoContext(const ClassDef *cd,std::unique_ptr<MemberList> &&ml,const QCString &title)
  : p(std::make_unique<Private>(cd,std::move(ml),title))
{
}

InheritedMemberInfoContext::~InheritedMemberInfoContext()
{
}

TemplateVariant InheritedMemberInfoContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector InheritedMemberInfoContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list InheritedMemberList[InheritedMemberInfo] : list of inherited classes
class InheritedMemberInfoListContext::Private : public GenericNodeListContext
{
  public:
    void addMemberList(const ClassDef *inheritedFrom,const MemberList &ml,MemberList *combinedList)
    {
      for (const auto &md : ml)
      {
        if (md->isBriefSectionVisible() && !md->isReimplementedBy(inheritedFrom))
        {
          combinedList->push_back(md);
        }
      }
    }
    void addMemberListIncludingGrouped(const ClassDef *inheritedFrom,const MemberList *ml,MemberList *combinedList)
    {
      if (ml)
      {
        addMemberList(inheritedFrom,*ml,combinedList);
        for (const auto *mg : ml->getMemberGroupList())
        {
          addMemberList(inheritedFrom,mg->members(),combinedList);
        }
      }
    }
    void addMemberGroupsOfClass(const ClassDef *inheritedFrom,
                                const ClassDef *cd,MemberListType lt,MemberList *combinedList)
    {
      // TODO: why this there no addMemberGroupsOfNamespace, addMembersGroupsOfFile,
      // addMemberGroupsOfGroup?
      for (const auto &mg: cd->getMemberGroups())
      {
        if (!mg->members().empty() && (!mg->allMembersInSameSection() || !cd->subGrouping())) // group is in its own section
        {
          for (const auto &md : mg->members())
          {
            if (lt==md->getSectionList(mg->container())->listType() &&
                !md->isReimplementedBy(inheritedFrom) &&
                md->isBriefSectionVisible())
            {
              combinedList->push_back(md);
            }
          }
        }
      }
    }
    void addInheritedMembers(const ClassDef *inheritedFrom,const ClassDef *cd,MemberListType lt,
                             MemberListType lt1,int lt2,const QCString &title,bool additionalList)
    {
      int count = cd->countMembersIncludingGrouped(lt1,inheritedFrom,additionalList);
      if (lt2!=-1) count += cd->countMembersIncludingGrouped((MemberListType)lt2,inheritedFrom,additionalList);
      if (count>0)
      {
        const MemberList *ml  = cd->getMemberList(lt1);
        const MemberList *ml2 = lt2!=-1 ? cd->getMemberList((MemberListType)lt2) : 0;
        std::unique_ptr<MemberList> combinedList = std::make_unique<MemberList>(lt,MemberListContainer::Class);
        addMemberListIncludingGrouped(inheritedFrom,ml,combinedList.get());
        addMemberListIncludingGrouped(inheritedFrom,ml2,combinedList.get());
        addMemberGroupsOfClass(inheritedFrom,cd,lt,combinedList.get());
        if (lt2!=-1) addMemberGroupsOfClass(inheritedFrom,cd,(MemberListType)lt2,combinedList.get());
        append(InheritedMemberInfoContext::alloc(cd,std::move(combinedList),title));
      }
    }
    void findInheritedMembers(const ClassDef *inheritedFrom,const ClassDef *cd,MemberListType lt,
                              int lt2, const QCString &title,bool additionalList,
                              ClassDefSet &visitedClasses)
    {
      for (const auto &ibcd : cd->baseClasses())
      {
        ClassDef *icd=ibcd.classDef;
        if (icd->isLinkable())
        {
          int lt1,lt3;
          convertProtectionLevel(lt,ibcd.prot,&lt1,&lt3);
          if (lt2==-1 && lt3!=-1)
          {
            lt2=lt3;
          }
          if (visitedClasses.find(icd)==visitedClasses.end())
          {
            visitedClasses.insert(icd); // guard for multiple virtual inheritance
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
};

InheritedMemberInfoListContext::InheritedMemberInfoListContext() : p(std::make_unique<Private>())
{
}

void InheritedMemberInfoListContext::addMemberList(
    const ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList)
{
  ClassDefSet visited;
  bool memberInSection = cd->countMembersIncludingGrouped(lt,cd,FALSE)>0;
  bool show = (additionalList && !memberInSection) || // inherited member to show in the additional inherited members list
              (!additionalList && memberInSection);   // inherited member to show in a member list of the class
  //printf("%s:%s show=%d\n",qPrint(cd->name()),qPrint(MemberList::listTypeAsString(lt)),show);
  if (show)
  {
    p->findInheritedMembers(cd,cd,lt,-1,title,additionalList,visited);
  }
}

InheritedMemberInfoListContext::~InheritedMemberInfoListContext()
{
}

// TemplateListIntf
uint InheritedMemberInfoListContext::count() const
{
  return p->count();
}

TemplateVariant InheritedMemberInfoListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr InheritedMemberInfoListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct Argument: parameter information
//%% {
class ArgumentContext::Private
{
  public:
    Private(const Argument &arg,const Definition *def,const QCString &relPath) :
      m_argument(arg), m_def(def), m_relPath(relPath)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("type",     &Private::type);
        s_inst.addProperty("name",     &Private::name);
        s_inst.addProperty("defVal",   &Private::defVal);
        s_inst.addProperty("docs",     &Private::docs);
        s_inst.addProperty("attrib",   &Private::attrib);
        s_inst.addProperty("array",    &Private::array);
        s_inst.addProperty("namePart", &Private::namePart);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant type() const
    {
      return createLinkedText(m_def,m_relPath,m_argument.type);
    }
    TemplateVariant attrib() const
    {
      return m_argument.attrib;
    }
    TemplateVariant name() const
    {
      return m_argument.name;
    }
    TemplateVariant defVal() const
    {
      return createLinkedText(m_def,m_relPath,m_argument.defval);
    }
    TemplateVariant array() const
    {
      return m_argument.array;
    }
    TemplateVariant docs() const
    {
      if (!m_cache.docs && m_def)
      {
        if (!m_argument.docs.isEmpty())
        {
          m_cache.docs.reset(new TemplateVariant(
                             parseDoc(m_def,m_def->docFile(),m_def->docLine(),
                             m_relPath,m_argument.docs,TRUE)));
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
      QCString result = m_argument.attrib;
      uint l = result.length();
      if (l>2 && result.at(0)=='[' && result.at(l-1)==']')
      {
        result = result.mid(1,l-2);
        if (result!=",") result+=":"; // for normal keywords add colon
      }
      return result;
    }
  private:
    Argument m_argument;
    const Definition *m_def;
    QCString m_relPath;
    struct Cachable
    {
      std::unique_ptr<TemplateVariant> docs;
    };
    mutable Cachable m_cache;
    static PropertyMapper<ArgumentContext::Private> s_inst;
};
//%% }

PropertyMapper<ArgumentContext::Private> ArgumentContext::Private::s_inst;

ArgumentContext::ArgumentContext(const Argument &al,const Definition *def,const QCString &relPath) : p(std::make_unique<Private>(al,def,relPath))
{
}

ArgumentContext::~ArgumentContext()
{
}

TemplateVariant ArgumentContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector ArgumentContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list ArgumentList[Argument] : list of inherited classes
class ArgumentListContext::Private : public GenericNodeListContext
{
  public:
    void addArgument(const Argument &arg,const Definition *def,const QCString &relPath)
    {
      append(ArgumentContext::alloc(arg,def,relPath));
    }
};

ArgumentListContext::ArgumentListContext() : p(std::make_unique<Private>())
{
}

ArgumentListContext::ArgumentListContext(const ArgumentList &list,
                        const Definition *def,const QCString &relPath) : p(std::make_unique<Private>())
{
  for (const Argument &arg : list)
  {
    p->addArgument(arg,def,relPath);
  }
}

ArgumentListContext::~ArgumentListContext()
{
}

// TemplateListIntf
uint ArgumentListContext::count() const
{
  return p->count();
}

TemplateVariant ArgumentListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr ArgumentListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

// SymbolIndex
//  - name: string
//  - letter: string
//  - symbolGroups: SymbolGroupList
// SymbolGroupList: list of SymbolGroups
// SymbolGroup
//  - id
//  - name
//  - symbols: SymbolList
// SymbolList: list of Symbols
// Symbol
//  - obj
//  - scope
//  - relPath

//------------------------------------------------------------------------

//%% struct SymbolGroup: search group of similar symbols
//%% {
class SymbolContext::Private
{
  public:
    Private(const Definition *d,const Definition *prev,
            const Definition *next) : m_def(d), m_prevDef(prev), m_nextDef(next)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("fileName",&Private::fileName);
        s_inst.addProperty("anchor",  &Private::anchor);
        s_inst.addProperty("scope",   &Private::scope);
        s_inst.addProperty("relPath", &Private::relPath);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant fileName() const
    {
      return m_def->getOutputFileBase();
    }
    TemplateVariant anchor() const
    {
      return m_def->anchor();
    }
    TemplateVariant scope() const
    {
      const Definition *scope     = m_def->getOuterScope();
      const Definition *next      = m_nextDef;
      const Definition *prev      = m_prevDef;
      const Definition *nextScope = next ? next->getOuterScope() : 0;
      const Definition *prevScope = prev ? prev->getOuterScope() : 0;
      const MemberDef  *md        = toMemberDef(m_def);
      bool isFunctionLike   = md && (md->isFunction() || md->isSlot() || md->isSignal());
      bool overloadedFunction = ((prevScope!=0 && scope==prevScope) || (scope && scope==nextScope)) &&
                                md && (md->isFunction() || md->isSlot());

      QCString name;
      if (prev==0 && next==0) // unique name
      {
        if (scope!=Doxygen::globalScope)
        {
          name = scope->name();
        }
        else if (md)
        {
          const FileDef *fd = md->getBodyDef();
          if (fd==0) fd = md->getFileDef();
          if (fd)
          {
            name = fd->localName();
          }
        }
      }
      else
      {

        QCString prefix;
        if (md) prefix=md->localName();
        if (overloadedFunction) // overloaded member function
        {
          prefix+=md->argsString();
          // show argument list to disambiguate overloaded functions
        }
        else if (md && isFunctionLike) // unique member function
        {
          prefix+="()"; // only to show it is a function
        }
        bool found=FALSE;
        if (m_def->definitionType()==Definition::TypeClass)
        {
          name = m_def->displayName();
          found = TRUE;
        }
        else if (m_def->definitionType()==Definition::TypeNamespace)
        {
          name = m_def->displayName();
          found = TRUE;
        }
        else if (scope==0 || scope==Doxygen::globalScope) // in global scope
        {
          if (md)
          {
            const FileDef *fd = md->getBodyDef();
            if (fd==0) fd = md->getFileDef();
            if (fd)
            {
              if (!prefix.isEmpty()) prefix+=": ";
              name = prefix + convertToXML(fd->localName());
              found = TRUE;
            }
          }
        }
        else if (md && (md->resolveAlias()->getClassDef() || md->resolveAlias()->getNamespaceDef()))
          // member in class or namespace scope
        {
          SrcLangExt lang = md->getLanguage();
          name = m_def->getOuterScope()->qualifiedName()
            + getLanguageSpecificSeparator(lang) + prefix;
          found = TRUE;
        }
        else if (scope) // some thing else? -> show scope
        {
          name = prefix + convertToXML(scope->name());
          found = TRUE;
        }
        if (!found) // fallback
        {
          name = prefix + "("+theTranslator->trGlobalNamespace()+")";
        }
      }
      return name;
    }
    TemplateVariant relPath() const
    {
      return externalRef("../",m_def->getReference(),TRUE);
    }
  private:
    const Definition *m_def;
    const Definition *m_prevDef;
    const Definition *m_nextDef;
    static PropertyMapper<SymbolContext::Private> s_inst;
};
//%% }

PropertyMapper<SymbolContext::Private> SymbolContext::Private::s_inst;

SymbolContext::SymbolContext(const Definition *def,const Definition *prevDef,const Definition *nextDef) : p(std::make_unique<Private>(def,prevDef,nextDef))
{
}

SymbolContext::~SymbolContext()
{
}

TemplateVariant SymbolContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector SymbolContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list SymbolList[Symbol] : list of search symbols with the same name
class SymbolListContext::Private : public GenericNodeListContext
{
  public:
    Private(const SearchIndexList::const_iterator &start,
            const SearchIndexList::const_iterator &end)
    {
      const Definition *prev = 0;
      for (auto it = start; it!=end;)
      {
        const Definition *def = *it;
        ++it;
        const Definition *next = it!=end ? *it : 0;
        append(SymbolContext::alloc(def,prev,next));
        prev = def;
      }
    }
};

SymbolListContext::SymbolListContext(const SearchIndexList::const_iterator &start,
                                     const SearchIndexList::const_iterator &end) : p(std::make_unique<Private>(start,end))
{
}

SymbolListContext::~SymbolListContext()
{
}

// TemplateListIntf
uint SymbolListContext::count() const
{
  return p->count();
}

TemplateVariant SymbolListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr SymbolListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct SymbolGroup: search group of similar symbols
//%% {
class SymbolGroupContext::Private
{
  public:
    Private(const SearchIndexList::const_iterator &start,
            const SearchIndexList::const_iterator &end) : m_start(start), m_end(end)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("id",     &Private::id);
        s_inst.addProperty("name",   &Private::name);
        s_inst.addProperty("symbols",&Private::symbolList);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant id() const
    {
      return searchId(*m_start);
    }
    TemplateVariant name() const
    {
      return searchName(*m_start);
    }
    TemplateVariant symbolList() const
    {
      if (!m_cache.symbolList)
      {
        m_cache.symbolList = SymbolListContext::alloc(m_start,m_end);
      }
      return TemplateVariant(m_cache.symbolList);
    }
  private:
    SearchIndexList::const_iterator m_start;
    SearchIndexList::const_iterator m_end;
    struct Cachable
    {
      TemplateListIntfPtr symbolList;
    };
    mutable Cachable m_cache;
    static PropertyMapper<SymbolGroupContext::Private> s_inst;
};
//%% }

PropertyMapper<SymbolGroupContext::Private> SymbolGroupContext::Private::s_inst;

SymbolGroupContext::SymbolGroupContext(const SearchIndexList::const_iterator &start,
                                       const SearchIndexList::const_iterator &end) : p(std::make_unique<Private>(start,end))
{
}

SymbolGroupContext::~SymbolGroupContext()
{
}

TemplateVariant SymbolGroupContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector SymbolGroupContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list SymbolGroupList[SymbolGroup] : list of search groups one per by name
class SymbolGroupListContext::Private : public GenericNodeListContext
{
  public:
    Private(const SearchIndexList &sil)
    {
      QCString lastName;
      auto it = sil.begin();
      auto it_begin = it;
      while (it!=sil.end())
      {
        QCString name = searchName(*it);
        if (name!=lastName)
        {
          if (it!=it_begin)
          {
            append(SymbolGroupContext::alloc(it_begin,it));
          }
          it_begin = it;
          lastName = name;
        }
        ++it;
      }
      if (it_begin!=sil.end())
      {
        append(SymbolGroupContext::alloc(it_begin,sil.end()));
      }
    }
};

SymbolGroupListContext::SymbolGroupListContext(const SearchIndexList &sil) : p(std::make_unique<Private>(sil))
{
}

SymbolGroupListContext::~SymbolGroupListContext()
{
}

// TemplateListIntf
uint SymbolGroupListContext::count() const
{
  return p->count();
}

TemplateVariant SymbolGroupListContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr SymbolGroupListContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct SymbolIndex: search index
//%% {
class SymbolIndexContext::Private
{
  public:
    Private(const std::string &letter,
            const SearchIndexList &sl,
            const QCString &name) : m_letter(letter), m_searchList(sl), m_name(name)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("name",        &Private::name);
        s_inst.addProperty("letter",      &Private::letter);
        s_inst.addProperty("symbolGroups",&Private::symbolGroups);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant name() const
    {
      return m_name;
    }
    TemplateVariant letter() const
    {
      return m_letter;
    }
    TemplateVariant symbolGroups() const
    {
      if (!m_cache.symbolGroups)
      {
        m_cache.symbolGroups = SymbolGroupListContext::alloc(m_searchList);
      }
      return TemplateVariant(m_cache.symbolGroups);
    }
  private:
    QCString m_letter;
    const SearchIndexList &m_searchList;
    QCString m_name;
    struct Cachable
    {
      TemplateListIntfPtr symbolGroups;
    };
    mutable Cachable m_cache;
    static PropertyMapper<SymbolIndexContext::Private> s_inst;
};
//%% }

PropertyMapper<SymbolIndexContext::Private> SymbolIndexContext::Private::s_inst;

SymbolIndexContext::SymbolIndexContext(const std::string &letter,const SearchIndexList &sl,const QCString &name) : p(std::make_unique<Private>(letter,sl,name))
{
}

SymbolIndexContext::~SymbolIndexContext()
{
}

TemplateVariant SymbolIndexContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector SymbolIndexContext::fields() const
{
  return p->fields();
}

//------------------------------------------------------------------------

//%% list SymbolIndices[SymbolIndex] : list of search indices one per by type
class SymbolIndicesContext::Private : public GenericNodeListContext
{
  public:
    Private(const SearchIndexInfo &info)
    {
      // use info->symbolList to populate the list
      for (const auto &kv : info.symbolMap)
      {
        append(SymbolIndexContext::alloc(kv.first,kv.second,info.name));
      }
    }
};

SymbolIndicesContext::SymbolIndicesContext(const SearchIndexInfo &info) : p(std::make_unique<Private>(info))
{
}

SymbolIndicesContext::~SymbolIndicesContext()
{
}

// TemplateListIntf
uint SymbolIndicesContext::count() const
{
  return p->count();
}

TemplateVariant SymbolIndicesContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr SymbolIndicesContext::createIterator() const
{
  return p->createIterator();
}

//------------------------------------------------------------------------

//%% struct SearchIndex: search index
//%% {
class SearchIndexContext::Private
{
  public:
    Private(const SearchIndexInfo &info) : m_info(info)
    {
      static bool init=FALSE;
      if (!init)
      {
        s_inst.addProperty("name",         &Private::name);
        s_inst.addProperty("text",         &Private::text);
        s_inst.addProperty("symbolIndices",&Private::symbolIndices);
        init=TRUE;
      }
    }
    TemplateVariant get(const QCString &n) const
    {
      return s_inst.get(this,n);
    }
    StringVector fields() const
    {
      return s_inst.fields();
    }
    TemplateVariant name() const
    {
      return m_info.name;
    }
    TemplateVariant text() const
    {
      return m_info.getText();
    }
    TemplateVariant symbolIndices() const
    {
      if (!m_cache.symbolIndices)
      {
        m_cache.symbolIndices = SymbolIndicesContext::alloc(m_info);
      }
      return TemplateVariant(m_cache.symbolIndices);
    }
  private:
    const SearchIndexInfo &m_info;
    struct Cachable
    {
      TemplateListIntfPtr symbolIndices;
    };
    mutable Cachable m_cache;
    static PropertyMapper<SearchIndexContext::Private> s_inst;
};
//%% }

PropertyMapper<SearchIndexContext::Private> SearchIndexContext::Private::s_inst;

SearchIndexContext::SearchIndexContext(const SearchIndexInfo &info) : p(std::make_unique<Private>(info))
{
}

SearchIndexContext::~SearchIndexContext()
{
}

TemplateVariant SearchIndexContext::get(const QCString &name) const
{
  return p->get(name);
}

StringVector SearchIndexContext::fields() const
{
  return p->fields();
}


//------------------------------------------------------------------------

//%% list SearchIndices[SearchIndex] : list of search indices one per by type
class SearchIndicesContext::Private : public GenericNodeListContext
{
  public:
    Private()
    {
      for (const auto &si : getSearchIndices())
      {
        append(SearchIndexContext::alloc(si));
      }
    }
};

SearchIndicesContext::SearchIndicesContext() : p(std::make_unique<Private>())
{
}

SearchIndicesContext::~SearchIndicesContext()
{
}

// TemplateListIntf
uint SearchIndicesContext::count() const
{
  return p->count();
}

TemplateVariant SearchIndicesContext::at(uint index) const
{
  return p->at(index);
}

TemplateListIntf::ConstIteratorPtr SearchIndicesContext::createIterator() const
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
    void enableTabbing(bool) {}
};

//------------------------------------------------------------------------

class LatexSpaceless : public TemplateSpacelessIntf
{
  public:
    LatexSpaceless() { reset(); }
    void reset() { }
    QCString remove(const QCString &s)
    {
      if (s.isEmpty()) return s;
      TextStream result;
      const char *p = s.data();
      char c;
      while ((c=*p++))
      {
        switch(c)
        {
          case '\t': case ' ': case '\n':
            break;
          default:
            result << c;
            break;
        }
      }
      return result.str();
    }
  private:
};

//------------------------------------------------------------------------

class HtmlSpaceless : public TemplateSpacelessIntf
{
  public:
    HtmlSpaceless() { reset(); }
    void reset()
    {
      m_insideTag = FALSE;
      m_insideString = '\0';
      m_removeSpaces = TRUE;
    }
    QCString remove(const QCString &s)
    {
      if (s.isEmpty()) return s;
      TextStream result;
      const char *p = s.data();
      char c;
      while ((c=*p++))
      {
        switch(c)
        {
          case '<': // start of a tag
            if (!m_insideString) m_insideTag=TRUE,m_removeSpaces=FALSE;
            result << c;
            break;
          case '>': // end of a tag
            if (!m_insideString) m_insideTag=FALSE,m_removeSpaces=TRUE;
            result << c;
            break;
          case '\\': // escaped character in a string
            result << c;
            if (m_insideString && *p) result << *p++;
            break;
          case '"': case '\'':
            if (m_insideTag)
            {
              if (m_insideString==c) // end of string
              {
                m_insideString='\0';
              }
              else if (m_insideString=='\0') // start of string
              {
                m_insideString=c;
              }
            }
            result << c;
            break;
          case ' ': case '\t': case '\n': // whitespace
            if (!m_insideTag) // outside tags strip consecutive whitespace
            {
              m_removeSpaces=TRUE;
            }
            else
            {
              result << ' ';
            }
            break;
          default:
            //if (m_removeSpaces) result+=' ';
            result << c;
            m_removeSpaces=FALSE;
            break;
        }
      }
      //printf("HtmlSpaceless::remove({%s})={%s} m_insideTag=%d m_insideString=%c (%d) removeSpaces=%d\n",qPrint(s),qPrint(result),
      //    m_insideTag,m_insideString,m_insideString,m_removeSpaces);
      return result.str();
    }
  private:
    bool m_insideTag;
    char m_insideString;
    bool m_removeSpaces;
};

//------------------------------------------------------------------------

class LatexEscaper : public TemplateEscapeIntf
{
  public:
    LatexEscaper() : m_tabbing(FALSE) {}
    QCString escape(const QCString &s)
    {
      return convertToLaTeX(s,m_tabbing);
    }
    void enableTabbing(bool b) { m_tabbing=b; }
  private:
    bool m_tabbing;
};


//------------------------------------------------------------------------

void generateOutputViaTemplate()
{
  msg("Generating output via template engine...\n");
  {
    TemplateEngine e;
    auto ctx = e.createContext();
    if (ctx)
    {
      TemplateStructIntfPtr doxygen              (DoxygenContext::alloc());
      TemplateStructIntfPtr config               (ConfigContext::alloc());
      TemplateStructIntfPtr tr                   (TranslateContext::alloc());
      TemplateListIntfPtr   classList            (ClassListContext::alloc());
      TemplateStructIntfPtr classIndex           (ClassIndexContext::alloc());
      TemplateStructIntfPtr classTree            (ClassTreeContext::alloc());
      TemplateStructIntfPtr classHierarchy       (ClassHierarchyContext::alloc());
      TemplateListIntfPtr   conceptList          (ConceptListContext::alloc());
      TemplateStructIntfPtr conceptTree          (ConceptTreeContext::alloc());
      TemplateListIntfPtr   namespaceList        (NamespaceListContext::alloc());
      TemplateStructIntfPtr namespaceTree        (NamespaceTreeContext::alloc());
      TemplateListIntfPtr   dirList              (DirListContext::alloc());
      TemplateListIntfPtr   fileList             (FileListContext::alloc());
      TemplateStructIntfPtr fileTree             (FileTreeContext::alloc());
      TemplateStructIntfPtr pageTree             (PageTreeContext::alloc(*Doxygen::pageLinkedMap));
      TemplateListIntfPtr   pageList             (PageListContext::alloc(*Doxygen::pageLinkedMap));
      TemplateStructIntfPtr exampleTree          (ExampleTreeContext::alloc());
      TemplateListIntfPtr   exampleList          (ExampleListContext::alloc());
      TemplateStructIntfPtr moduleTree           (ModuleTreeContext::alloc());
      TemplateListIntfPtr   moduleList           (ModuleListContext::alloc());
      TemplateStructIntfPtr globalsIndex         (GlobalsIndexContext::alloc());
      TemplateStructIntfPtr classMembersIndex    (ClassMembersIndexContext::alloc());
      TemplateStructIntfPtr namespaceMembersIndex(NamespaceMembersIndexContext::alloc());
      TemplateListIntfPtr   searchIndices        (SearchIndicesContext::alloc());

      //%% Doxygen doxygen:
      ctx->set("doxygen",doxygen);
      //%% Translator tr:
      ctx->set("tr",tr);
      //%% Config config:
      ctx->set("config",config);
      //%% ClassList classList:
      ctx->set("classList",classList); // not used for standard HTML
      //%% ClassTree classTree:
      ctx->set("classTree",classTree);
      //%% ClassIndex classIndex:
      ctx->set("classIndex",classIndex);
      //%% ClassHierarchy classHierarchy:
      ctx->set("classHierarchy",classHierarchy);
      //%% ConceptList conceptList:
      ctx->set("conceptList",conceptList);
      //%% ConceptTree conceptTree:
      ctx->set("conceptTree",conceptTree);
      //%% NamespaceList namespaceList:
      ctx->set("namespaceList",namespaceList);
      //%% NamespaceTree namespaceTree:
      ctx->set("namespaceTree",namespaceTree);
      //%% FileList fileList:
      ctx->set("fileList",fileList);
      //%% FileTree fileTree:
      ctx->set("fileTree",fileTree);
      //%% PageList pageList
      ctx->set("pageList",pageList);
      //%% PageTree pageTree
      ctx->set("pageTree",pageTree);
      //%% ExampleTree exampleTree
      ctx->set("exampleTree",exampleTree);
      //%% ExampleList exampleList
      ctx->set("exampleList",exampleList);
      //%% ModuleTree moduleTree
      ctx->set("moduleTree",moduleTree);
      //%% ModuleList moduleList
      ctx->set("moduleList",moduleList);
      //%% DirList dirList
      ctx->set("dirList",dirList);
      //%% Page mainPage
      if (Doxygen::mainPage)
      {
        TemplateStructIntfPtr mainPage = PageContext::alloc(Doxygen::mainPage.get(),TRUE,FALSE);
        ctx->set("mainPage",mainPage);
      }
      else
      {
        // TODO: for LaTeX output index should be main... => solve in template
        Doxygen::mainPage.reset(createPageDef("[generated]",1,"index","",Config_getString(PROJECT_NAME)));
        Doxygen::mainPage->setFileName("index");
        TemplateStructIntfPtr mainPage = PageContext::alloc(Doxygen::mainPage.get(),TRUE,FALSE);
        ctx->set("mainPage",mainPage);
      }
      //%% GlobalsIndex globalsIndex:
      ctx->set("globalsIndex",globalsIndex);
      //%% ClassMembersIndex classMembersIndex:
      ctx->set("classMembersIndex",classMembersIndex);
      //%% NamespaceMembersIndex namespaceMembersIndex:
      ctx->set("namespaceMembersIndex",namespaceMembersIndex);
      //%% SearchIndices searchIndices
      ctx->set("searchIndices",searchIndices);
      //%% string space
      ctx->set("space"," ");

      //if (Config_getBool(GENERATE_HTML))
      { // render HTML output
        e.setTemplateDir("templates/html"); // TODO: make template part user configurable
        Template *tpl = e.loadByName("htmllayout.tpl",1);
        if (tpl)
        {
          g_globals.outputFormat = ContextOutputFormat_Html;
          g_globals.dynSectionId = 0;
          g_globals.outputDir    = Config_getString(HTML_OUTPUT);
          Dir dir(g_globals.outputDir.str());
          createSubDirs(dir);
          HtmlEscaper htmlEsc;
          ctx->setEscapeIntf(Config_getString(HTML_FILE_EXTENSION),&htmlEsc);
          HtmlSpaceless spl;
          ctx->setSpacelessIntf(&spl);
          ctx->setOutputDirectory(g_globals.outputDir);
          TextStream ts;
          tpl->render(ts,ctx.get());
          e.unload(tpl);
        }
      }

      // TODO: clean index before each run...

      //if (Config_getBool(GENERATE_LATEX))
      if (0)
      { // render LaTeX output
        e.setTemplateDir("templates/latex"); // TODO: make template part user configurable
        Template *tpl = e.loadByName("latexlayout.tpl",1);
        if (tpl)
        {
          g_globals.outputFormat = ContextOutputFormat_Latex;
          g_globals.dynSectionId = 0;
          g_globals.outputDir    = Config_getString(LATEX_OUTPUT);
          Dir dir(g_globals.outputDir.str());
          createSubDirs(dir);
          LatexEscaper latexEsc;
          ctx->setEscapeIntf(".tex",&latexEsc);
          LatexSpaceless spl;
          ctx->setSpacelessIntf(&spl);
          ctx->setOutputDirectory(g_globals.outputDir);
          TextStream ts;
          tpl->render(ts,ctx.get());
          e.unload(tpl);
        }
      }

      // clear all cached data in Definition objects.
      for (const auto &kv : *Doxygen::symbolMap)
      {
        kv.second->setCookie(0);
      }
    }
  }
}

void generateTemplateFiles(const QCString &templateDir)
{
  if (templateDir.isEmpty()) return;
  Dir thisDir;
  if (!thisDir.exists(templateDir.str()) && !thisDir.mkdir(templateDir.str()))
  {
    err("Failed to create output directory '%s'\n",qPrint(templateDir));
    return;
  }
  std::string outDir = templateDir.str()+"/html";
  if (!thisDir.exists(outDir) && !thisDir.mkdir(outDir))
  {
    err("Failed to create output directory '%s'\n",outDir.c_str());
    return;
  }
  ResourceMgr::instance().writeCategory("html",outDir.c_str());
}
