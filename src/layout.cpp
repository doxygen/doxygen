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

#include <array>

#include <assert.h>

#include "types.h"
#include "layout.h"
#include "message.h"
#include "language.h"
#include "util.h"
#include "doxygen.h"
#include "version.h"
#include "config.h"
#include "xml.h"
#include "resourcemgr.h"
#include "docparser.h"
#include "docnode.h"
#include "debug.h"
#include "regex.h"

inline QCString compileOptions(const QCString &def)
{
  return def;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1)
{
  return compileOptions(def)+"|"+QCString().setNum(static_cast<long>(langId1))+"="+value1;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2)
{
  return compileOptions(def,langId1,value1)+
         "|"+QCString().setNum(static_cast<long>(langId2))+"="+value2;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3)
{
  return compileOptions(def,langId1,value1,langId2,value2)+
         "|"+QCString().setNum(static_cast<long>(langId3))+"="+value3;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3,
                                                   SrcLangExt langId4,const QCString &value4)
{
  return compileOptions(def,langId1,value1,langId2,value2,langId3,value3)+
         "|"+QCString().setNum(static_cast<long>(langId4))+"="+value4;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3,
                                                   SrcLangExt langId4,const QCString &value4,
                                                   SrcLangExt langId5,const QCString &value5)
{
  return compileOptions(def,langId1,value1,langId2,value2,langId3,value3,langId4,value4)+
         "|"+QCString().setNum(static_cast<long>(langId5))+"="+value5;
}

static bool elemIsVisible(const XMLHandlers::Attributes &attrib,bool defVal=TRUE)
{
  QCString visible = XMLHandlers::value(attrib,"visible");
  //printf("visible_attribute=%s\n",qPrint(visible));
  if (visible.isEmpty()) return defVal;
  if (visible.at(0)=='$' && visible.length()>1)
  {
    QCString id = visible.mid(1);
    const ConfigValues::Info *opt = ConfigValues::instance().get(id);
    if (opt && opt->type==ConfigValues::Info::Bool)
    {
      return ConfigValues::instance().*(opt->value.b);
    }
    else if (opt && opt->type==ConfigValues::Info::String)
    {
      return opt->getBooleanRepresentation();
    }
    else if (!opt)
    {
      err("found unsupported value '{}' for visible attribute in layout file, reverting to '{}'\n",
          visible,(defVal?"yes":"no"));
      return defVal;
    }
  }
  QCString visibleLow = visible.lower();
  if (visibleLow=="no" || visibleLow=="false" || visibleLow=="0") return FALSE;
  else if (visibleLow=="yes" || visibleLow=="true" || visibleLow=="1") return TRUE;
  else
  {
    err("found unsupported value '{}' for visible attribute in layout file, reverting to '{}'\n",
        visible,(defVal?"yes":"no"));
    return defVal;
  }
}

//---------------------------------------------------------------------------------

void LayoutNavEntry::updateVisibility(LayoutNavEntry *parent)
{
  m_visible = m_visible && (parent==nullptr || parent->visible());
}

void LayoutNavEntry::appendChild(std::unique_ptr<LayoutNavEntry> &&e)
{
  e->updateVisibility(this);
  m_children.push_back(std::move(e));
}

void LayoutNavEntry::insertChild(size_t pos,std::unique_ptr<LayoutNavEntry> &&e)
{
  e->updateVisibility(this);
  m_children.insert(m_children.begin()+pos,std::move(e));
}

LayoutNavEntry *LayoutNavEntry::find(LayoutNavEntry::Kind kind,
    const QCString &file) const
{
  LayoutNavEntry *result=nullptr;
  for (const auto &entry : m_children)
  {
    // depth first search, needed to find the entry furthest from the
    // root in case an entry is in the tree twice
    result = entry->find(kind,file);
    if (result) return result;
    if (entry->kind()==kind && (file==QCString() || entry->baseFile()==file))
    {
      return entry.get();
    }
  }
  return result;
}

QCString LayoutNavEntry::url() const
{
  QCString url = baseFile().stripWhiteSpace();
  if ((kind()!=LayoutNavEntry::User && kind()!=LayoutNavEntry::UserGroup) ||
      (kind()==LayoutNavEntry::UserGroup && url.startsWith("usergroup")))
  {
    addHtmlExtensionIfMissing(url);
  }
  else if (url.startsWith("@ref ") || url.startsWith("\\ref "))
  {
    bool found=false;
    QCString relPath = "";
    QCString context = QCString();
    auto parser { createDocParser() };
    auto dfAst  { createRef( *parser.get(), url.mid(5).stripWhiteSpace(), context ) };
    auto dfAstImpl = dynamic_cast<const DocNodeAST*>(dfAst.get());
    const DocRef *df = std::get_if<DocRef>(&dfAstImpl->root);
    if (!df->file().isEmpty() || !df->anchor().isEmpty())
    {
      found = true;
      url=externalRef(relPath,df->ref(),TRUE);
      if (!df->file().isEmpty())
      {
        QCString fn = df->file();
        addHtmlExtensionIfMissing(fn);
        url += fn;
      }
      if (!df->anchor().isEmpty())
      {
        url += "#" + df->anchor();
      }
    }
    if (!found)
    {
      msg("explicit link request to '{}' in layout file '{}' could not be resolved\n",url.mid(5),Config_getString(LAYOUT_FILE));
    }
  }
  //printf("LayoutNavEntry::url()=%s\n",qPrint(url));
  return url;
}

//---------------------------------------------------------------------------------

class LayoutParser
{
  public:
    LayoutParser(LayoutDocManager &manager) : m_layoutDocManager(manager) {}

    // =========== XMLHandler events
    void setDocumentLocator(const XMLLocator *locator)
    {
      m_locator = locator;
    }
    void error( const std::string &fileName,int lineNr,const std::string &msg)
    {
      warn_layout(fileName,lineNr,"{}",msg);
    }
    void startElement( const std::string &name, const XMLHandlers::Attributes& attrib );
    void endElement( const std::string &name );

    void startSimpleEntry(LayoutDocEntry::Kind k,const std::string &id,const XMLHandlers::Attributes &attrib)
    {
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (m_part!=LayoutDocManager::Undefined)
      {
        auto elem = std::make_unique<LayoutDocEntrySimple>(k,id,isVisible);
        //printf("startSimpleEntry(%s) isVisible=%d visible=%d\n",qPrint(elem->entryToString()),isVisible,elem->visible());
        m_layoutDocManager.addEntry(m_part,std::move(elem));
      }
    }

    // ============ Specific callbacks

    void startSectionEntry(LayoutDocEntry::Kind k,const std::string &id,const XMLHandlers::Attributes &attrib,
                           const QCString &title)
    {
      bool isVisible = m_visible && elemIsVisible(attrib);
      QCString userTitle = XMLHandlers::value(attrib,"title");
      //printf("startSectionEntry: title='%s' userTitle='%s'\n",
      //    qPrint(title),qPrint(userTitle));
      if (userTitle.isEmpty())  userTitle = title;
      if (m_part!=LayoutDocManager::Undefined)
      {
        m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntrySection>(k,id,userTitle,isVisible));
      }
    }


    void startMemberDeclEntry(const std::string &id,const XMLHandlers::Attributes &attrib,MemberListType type,
                              const QCString &title,const QCString &subscript)
    {
      QCString userTitle     = XMLHandlers::value(attrib,"title");
      QCString userSubscript = XMLHandlers::value(attrib,"subtitle");
      if (userTitle.isEmpty())     userTitle     = title;
      if (userSubscript.isEmpty()) userSubscript = subscript;
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (m_part!=LayoutDocManager::Undefined)
      {
        m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntryMemberDecl>(type,id,userTitle,userSubscript,isVisible));
      }
    }

    void startMemberDefEntry(const std::string &id,const XMLHandlers::Attributes &attrib,MemberListType type,
                             const QCString &title,const QCString &)
    {
      QCString userTitle = XMLHandlers::value(attrib,"title");
      if (userTitle.isEmpty()) userTitle = title;
      //printf("memberdef: %s\n",qPrint(userTitle));
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (m_part!=LayoutDocManager::Undefined)
      {
        m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntryMemberDef>(type,id,userTitle,isVisible));
      }
    }

    void startLayout(const std::string &,const XMLHandlers::Attributes &attrib)
    {
      // extract and store version number
      QCString version = XMLHandlers::value(attrib,"version");
      static const reg::Ex re(R"((\d+)\.(\d+))");
      reg::Match match;
      if (reg::match(version.view(),match,re))
      {
        m_majorVersion = atoi(match[1].str().c_str());
        m_minorVersion = atoi(match[2].str().c_str());
        //printf("layout version %d.%d\n",m_versionMajor,m_versionMinor);
      }
    }

    void startNavIndex(const std::string &,const XMLHandlers::Attributes &)
    {
      m_scope="navindex/";
      m_rootNav = m_layoutDocManager.rootNavEntry();
    }

    void endNavIndex(const std::string &)
    {
      m_scope="";
      if (m_rootNav && !m_rootNav->find(LayoutNavEntry::MainPage))
      {
        // no MainPage node... add one as the first item of the root node...
        m_rootNav->insertChild(0,std::make_unique<LayoutNavEntry>(m_rootNav,LayoutNavEntry::MainPage, TRUE,
                                                   "index",theTranslator->trMainPage(),""));
      }
    }

    void startNavEntry(const std::string &,const XMLHandlers::Attributes &attrib)
    {
      bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
      bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      bool hasGraphicalHierarchy = Config_getBool(HAVE_DOT) &&
                                          Config_getBool(GRAPHICAL_HIERARCHY);
      bool extractAll = Config_getBool(EXTRACT_ALL);
      static struct NavEntryMap
      {
        const char *typeStr;       // type attribute name in the XML file
        LayoutNavEntry::Kind kind; // corresponding enum name
        QCString mainName;         // default title for an item if it has children
        QCString subName;          // optional name for an item if it is rendered as a child
        QCString intro;            // introduction text to be put on the index page
        QCString baseFile;         // base name of the file containing the index page
      } mapping[] =
      {
        { "mainpage",
          LayoutNavEntry::MainPage,
          theTranslator->trMainPage(),
          QCString(),
          QCString(),
          "index"
        },
        { "pages",
          LayoutNavEntry::Pages,
          theTranslator->trRelatedPages(),
          QCString(),
          theTranslator->trRelatedPagesDescription(),
          "pages"
        },
        { "topics",
          LayoutNavEntry::Topics,
          theTranslator->trTopics(),
          QCString(),
          theTranslator->trTopicListDescription(),
          "topics"
        },
        { "modules",
          LayoutNavEntry::Modules,
          theTranslator->trModules(),
          theTranslator->trModulesList(),
          theTranslator->trModulesDescription(),
          "modules"
        },
        { "modulelist",
          LayoutNavEntry::ModuleList,
          theTranslator->trModulesList(),
          QCString(),
          theTranslator->trModulesListDescription(extractAll),
          "modules"
        },
        { "modulemembers",
          LayoutNavEntry::ModuleMembers,
          theTranslator->trModulesMembers(),
          QCString(),
          theTranslator->trModulesMemberDescription(extractAll),
          "modulemembers"
        },
        { "namespaces",
          LayoutNavEntry::Namespaces,
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt || sliceOpt ? theTranslator->trModules() : theTranslator->trNamespaces(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageList() : fortranOpt || sliceOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageListDescription() : fortranOpt || sliceOpt ? theTranslator->trModulesListDescription(extractAll) : theTranslator->trNamespaceListDescription(extractAll),
          "namespaces"
        },
        { "namespacelist",
          LayoutNavEntry::NamespaceList,
          javaOpt || vhdlOpt   ? theTranslator->trPackageList() : fortranOpt || sliceOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
          QCString(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageListDescription() : fortranOpt || sliceOpt ? theTranslator->trModulesListDescription(extractAll) : theTranslator->trNamespaceListDescription(extractAll),
          "namespaces"
        },
        { "namespacemembers",
          LayoutNavEntry::NamespaceMembers,
          javaOpt || vhdlOpt   ? theTranslator->trPackageMembers() : fortranOpt || sliceOpt ? theTranslator->trModulesMembers() : theTranslator->trNamespaceMembers(),
          QCString(),
          fortranOpt || sliceOpt ? theTranslator->trModulesMemberDescription(extractAll) : theTranslator->trNamespaceMemberDescription(extractAll),
          "namespacemembers"
        },
        { "concepts",
          LayoutNavEntry::Concepts,
          theTranslator->trConcept(true,false),
          theTranslator->trConceptList(),
          theTranslator->trConceptListDescription(extractAll),
          "concepts"
        },
        { "classindex",
          LayoutNavEntry::ClassIndex,
          fortranOpt ? theTranslator->trCompoundIndexFortran() : vhdlOpt ? theTranslator->trDesignUnitIndex() : theTranslator->trCompoundIndex(),
          QCString(),
          QCString(),
          "classes"
        },
        { "classes",
          LayoutNavEntry::Classes,
          fortranOpt ? theTranslator->trDataTypes() : vhdlOpt ? theTranslator->trDesignUnits() : theTranslator->trClasses(),
          theTranslator->trCompoundList(),
          fortranOpt ? theTranslator->trCompoundListDescriptionFortran() : vhdlOpt ? theTranslator->trDesignUnitListDescription() : theTranslator->trCompoundListDescription(),
          "annotated"
        },
        { "classlist",
          LayoutNavEntry::ClassList,
          fortranOpt ? theTranslator->trCompoundListFortran() : vhdlOpt ? theTranslator->trDesignUnitList() : theTranslator->trCompoundList(),
          QCString(),
          fortranOpt ? theTranslator->trCompoundListDescriptionFortran() : vhdlOpt ? theTranslator->trDesignUnitListDescription() : theTranslator->trCompoundListDescription(),
          "annotated"
        },
        { "hierarchy",
          LayoutNavEntry::ClassHierarchy,
          vhdlOpt    ? theTranslator->trDesignUnitHierarchy() : theTranslator->trClassHierarchy(),
          QCString(),
          theTranslator->trClassHierarchyDescription(),
          hasGraphicalHierarchy ? "inherits" : "hierarchy"
        },
        { "classmembers",
          LayoutNavEntry::ClassMembers,
          fortranOpt ? theTranslator->trCompoundMembersFortran() : vhdlOpt ? theTranslator->trDesignUnitMembers() : theTranslator->trCompoundMembers(),
          QCString(),
          fortranOpt ? theTranslator->trCompoundMembersDescriptionFortran(extractAll) : theTranslator->trCompoundMembersDescription(extractAll),
          "functions"
        },
        { "interfaceindex",
          LayoutNavEntry::InterfaceIndex,
          theTranslator->trInterfaceIndex(),
          QCString(),
          QCString(),
          "interfaces"
        },
        { "interfaces",
          LayoutNavEntry::Interfaces,
          theTranslator->trSliceInterfaces(),
          theTranslator->trInterfaceList(),
          theTranslator->trInterfaceListDescription(),
          "annotatedinterfaces"
        },
        { "interfacelist",
          LayoutNavEntry::InterfaceList,
          theTranslator->trInterfaceList(),
          QCString(),
          theTranslator->trInterfaceListDescription(),
          "annotatedinterfaces"
        },
        { "interfacehierarchy",
          LayoutNavEntry::InterfaceHierarchy,
          theTranslator->trInterfaceHierarchy(),
          QCString(),
          theTranslator->trInterfaceHierarchyDescription(),
          hasGraphicalHierarchy ? "interfaceinherits" : "interfacehierarchy"
        },
        { "structindex",
          LayoutNavEntry::StructIndex,
          theTranslator->trStructIndex(),
          QCString(),
          QCString(),
          "structs"
        },
        { "structs",
          LayoutNavEntry::Structs,
          theTranslator->trStructs(),
          theTranslator->trStructList(),
          theTranslator->trStructListDescription(),
          "annotatedstructs"
        },
        { "structlist",
          LayoutNavEntry::StructList,
          theTranslator->trStructList(),
          QCString(),
          theTranslator->trStructListDescription(),
          "annotatedstructs"
        },
        { "exceptionindex",
          LayoutNavEntry::ExceptionIndex,
          theTranslator->trExceptionIndex(),
          QCString(),
          QCString(),
          "exceptions"
        },
        { "exceptions",
          LayoutNavEntry::Exceptions,
          theTranslator->trExceptions(),
          theTranslator->trExceptionList(),
          theTranslator->trExceptionListDescription(),
          "annotatedexceptions"
        },
        { "exceptionlist",
          LayoutNavEntry::ExceptionList,
          theTranslator->trExceptionList(),
          QCString(),
          theTranslator->trExceptionListDescription(),
          "annotatedexceptions"
        },
        { "exceptionhierarchy",
          LayoutNavEntry::ExceptionHierarchy,
          theTranslator->trExceptionHierarchy(),
          QCString(),
          theTranslator->trExceptionHierarchyDescription(),
          hasGraphicalHierarchy ? "exceptioninherits" : "exceptionhierarchy"
        },
        { "files",
          LayoutNavEntry::Files,
          theTranslator->trFile(TRUE,FALSE),
          theTranslator->trFileList(),
          theTranslator->trFileListDescription(extractAll),
          "files"
        },
        { "filelist",
          LayoutNavEntry::FileList,
          theTranslator->trFileList(),
          QCString(),
          theTranslator->trFileListDescription(extractAll),
          "files"
        },
        { "globals",
          LayoutNavEntry::FileGlobals,
          theTranslator->trFileMembers(),
          QCString(),
          theTranslator->trFileMembersDescription(extractAll),
          "globals"
        },
        { "examples",
          LayoutNavEntry::Examples,
          theTranslator->trExamples(),
          QCString(),
          theTranslator->trExamplesDescription(),
          "examples"
        },
        { "user",
          LayoutNavEntry::User,
          QCString(),
          QCString(),
          QCString(),
          "user"
        },
        { "usergroup",
          LayoutNavEntry::UserGroup,
          QCString(),
          QCString(),
          QCString(),
          "usergroup"
        },
        { nullptr, // end of list
          static_cast<LayoutNavEntry::Kind>(0),
          QCString(),
          QCString(),
          QCString(),
          QCString()
        }
      };
      // find type in the table
      int i=0;
      QCString type = XMLHandlers::value(attrib,"type");
      while (mapping[i].typeStr)
      {
        if (mapping[i].typeStr==type)
          break;
        i++;
      }
      if (mapping[i].typeStr==nullptr)
      {
        QCString fileName = m_locator->fileName();
        if (type.isEmpty())
        {
          warn_layout(fileName,m_locator->lineNr(),"an entry tag within a navindex has no type attribute! Check your layout file!");
        }
        else
        {
          warn_layout(fileName,m_locator->lineNr(),"the type '{}' is not supported for the entry tag within a navindex! Check your layout file!",type);
        }
        m_invalidEntry=TRUE;
        return;
      }
      LayoutNavEntry::Kind kind = mapping[i].kind;
      QCString baseFile = mapping[i].baseFile;
      QCString title = XMLHandlers::value(attrib,"title");
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (title.isEmpty()) // use default title
      {
        title = mapping[i].mainName; // use title for main row
        if (m_rootNav!=m_layoutDocManager.rootNavEntry() && !mapping[i].subName.isEmpty())
        {
          title = mapping[i].subName; // if this is a child of another row, use the subName if available
                                      // this is mainly done to get compatible naming with older versions.
        }
      }
      QCString intro = XMLHandlers::value(attrib,"intro");
      if (intro.isEmpty()) // use default intro text
      {
        intro = mapping[i].intro;
      }
      QCString url = XMLHandlers::value(attrib,"url");
      if (mapping[i].kind==LayoutNavEntry::User && !url.isEmpty())
      {
        baseFile=url;
      }
      else if (kind==LayoutNavEntry::UserGroup)
      {
        if (!url.isEmpty())
        {
          if (url == "[none]")
          {
            baseFile = QCString();
          }
          else
          {
            baseFile=url;
          }
        }
        else
        {
          baseFile+=QCString().sprintf("%d",m_userGroupCount++);
        }
      }
      // create new item and make it the new root
      m_rootNav = m_layoutDocManager.createChildNavEntry(m_rootNav,kind,isVisible,baseFile,title,intro);
    }

    void endNavEntry(const std::string &)
    {
      // set the root back to the parent
      if (m_rootNav && !m_invalidEntry) m_rootNav = m_rootNav->parent();
      m_invalidEntry=FALSE;
    }

    void startTop(const std::string &,const XMLHandlers::Attributes &attrib,LayoutDocManager::LayoutPart part,
                  const QCString &scope, LayoutNavEntry::Kind nav)
    {
      //printf("startTop(scope=%s)\n",qPrint(scope));
      m_scope = scope;
      m_part = part;
      m_visible = elemIsVisible(attrib);
    }

    void endTop(const std::string &)
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startMemberDef(const std::string &id,const XMLHandlers::Attributes &attrib)
    {
      m_scope+="memberdef/";
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (m_part!=LayoutDocManager::Undefined)
      {
        m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDefStart,id,isVisible));
      }
    }

    void endMemberDef(const std::string &id)
    {
      QCString scopeOrg = m_scope;
      int i=m_scope.findRev("memberdef/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        bool isVisible = true;
        for (const auto &lde : m_layoutDocManager.docEntries(m_part))
        {
          if (lde->kind() == LayoutDocEntry::MemberDefStart)
          {
             isVisible = static_cast<const LayoutDocEntrySimple*>(lde.get())->visible();
             break;
          }
        }
        if (m_part!=LayoutDocManager::Undefined)
        {
          m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDefEnd,id,isVisible));
        }
      }
    }

    void startMemberDecl(const std::string &id,const XMLHandlers::Attributes &attrib)
    {
      m_scope+="memberdecl/";
      bool isVisible = m_visible && elemIsVisible(attrib);
      if (m_part!=LayoutDocManager::Undefined)
      {
        m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDeclStart,id,isVisible));
      }
    }

    void endMemberDecl(const std::string &id)
    {
      int i=m_scope.findRev("memberdecl/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        bool isVisible = true;
        for (const auto &lde : m_layoutDocManager.docEntries(m_part))
        {
          if (lde->kind() == LayoutDocEntry::MemberDeclStart)
          {
             isVisible = static_cast<const LayoutDocEntrySimple*>(lde.get())->visible();
             break;
          }
        }
        if (m_part!=LayoutDocManager::Undefined)
        {
          m_layoutDocManager.addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDeclEnd,id,isVisible));
        }
      }
    }

    int majorVersion() const { return m_majorVersion; }
    int minorVersion() const { return m_minorVersion; }

  private:
    QCString m_scope;
    LayoutDocManager &m_layoutDocManager;
    LayoutDocManager::LayoutPart m_part = LayoutDocManager::Undefined;
    LayoutNavEntry *m_rootNav = nullptr;
    bool m_invalidEntry = false;
    bool m_visible = true;
    static int m_userGroupCount;
    const XMLLocator *m_locator = nullptr;
    int m_majorVersion = 1;
    int m_minorVersion = 0;
};

//---------------------------------------------------------------------------------

namespace {

struct ElementCallbacks
{
  using StartCallback = std::function<void(LayoutParser&,const std::string &,const XMLHandlers::Attributes&)>;
  using EndCallback   = std::function<void(LayoutParser&,const std::string &)>;

  StartCallback startCb;
  EndCallback   endCb = [](LayoutParser &,const std::string &){};
};

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...))
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(id,attr); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocEntry::Kind kind
                   )
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,id,attr); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocEntry::Kind kind,
                    const std::function<QCString()> &title
                   )
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,id,attr,title()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    MemberListType type,
                    const std::function<QCString()> &title
                   )
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(id,attr,type,title(),QCString()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    MemberListType type,
                    const std::function<QCString()> &title,
                    const std::function<QCString()> &subtitle
                   )
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(id,attr,type,title(),subtitle()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocManager::LayoutPart part,
                    const QCString &scope,
                    LayoutNavEntry::Kind nav
                   )
{
  return [=](LayoutParser &parser,const std::string &id,const XMLHandlers::Attributes &attr) { (parser.*fn)(id,attr,part,scope,nav); };
}

template<class...Args>
static auto endCb(void (LayoutParser::*fn)(Args...))
{
  return [=](LayoutParser &parser,const std::string &id) { (parser.*fn)(id); };
}

// clang-format off
static const std::map< std::string, ElementCallbacks > g_elementHandlers =
{
  // path/name
  { "doxygenlayout",                              { startCb(&LayoutParser::startLayout) } },
  { "navindex",                                   { startCb(&LayoutParser::startNavIndex), endCb(&LayoutParser::endNavIndex) } },
  { "navindex/tab",                               { startCb(&LayoutParser::startNavEntry),
                                                    endCb(&LayoutParser::endNavEntry)
                                                  } },

  // class layout handlers
  { "class",                                      { startCb(&LayoutParser::startTop,LayoutDocManager::Class,"class/",LayoutNavEntry::Classes),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "class/briefdescription",                     { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::BriefDesc) } },
  { "class/detaileddescription",                  { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            [](){ return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "class/authorsection",                        { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::AuthorSection) } },
  { "class/includes",                             { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassIncludes) } },
  { "class/inheritancegraph",                     { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassInheritanceGraph) } },
  { "class/collaborationgraph",                   { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassCollaborationGraph) } },
  { "class/allmemberslink",                       { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassAllMembersLink) } },
  { "class/usedfiles",                            { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassUsedFiles) } },
  { "class/memberdecl",                           { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl) } },
  { "class/memberdecl/membergroups",              { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::MemberGroups) } },
  { "class/memberdecl/nestedclasses",             { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::ClassNestedClasses,
                                                            []() { return compileOptions(/*default*/        theTranslator->trCompounds(),
                                                                                         SrcLangExt::VHDL,   theTranslator->trVhdlType(VhdlSpecifier::ENTITY,FALSE),
                                                                                         SrcLangExt::Fortran,theTranslator->trDataTypes()); })
                                                  } },
  { "class/memberdecl/services",                  { startCb(&LayoutParser::startMemberDeclEntry,MemberListType::Services(),
                                                            []() { return compileOptions(theTranslator->trServices()); })
                                                  } },
  { "class/memberdecl/interfaces",                { startCb(&LayoutParser::startMemberDeclEntry,MemberListType::Interfaces(),
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdecl/publictypes",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType::PubTypes(),
                                                            []() { return compileOptions(theTranslator->trPublicTypes()); })
                                                  } },
  { "class/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType::PubSlots(),
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "class/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry,MemberListType::Signals(),
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "class/memberdecl/publicmethods",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PubMethods(),
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicMembers(),
                                                                                         SrcLangExt::ObjC, theTranslator->trInstanceMethods(),
                                                                                         SrcLangExt::Slice,theTranslator->trOperations()); })
                                                  } },
  { "class/memberdecl/publicstaticmethods",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PubStaticMethods(),
                                                            []() { return compileOptions(/* default */    theTranslator->trStaticPublicMembers(),
                                                                                         SrcLangExt::ObjC, theTranslator->trClassMethods()); })
                                                  } },
  { "class/memberdecl/publicattributes",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PubAttribs(),
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicAttribs(),
                                                                                         SrcLangExt::Slice,theTranslator->trDataMembers()); })
                                                  } },
  { "class/memberdecl/publicstaticattributes",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PubStaticAttribs(),
                                                            []() { return compileOptions(theTranslator->trStaticPublicAttribs()); })
                                                  } },
  { "class/memberdecl/protectedtypes",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProTypes(),
                                                            []() { return compileOptions(theTranslator->trProtectedTypes()); })
                                                  } },
  { "class/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProSlots(),
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "class/memberdecl/protectedmethods",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProMethods(),
                                                            []() { return compileOptions(theTranslator->trProtectedMembers()); })
                                                  } },
  { "class/memberdecl/protectedstaticmethods",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProStaticMethods(),
                                                            []() { return compileOptions(theTranslator->trStaticProtectedMembers()); })
                                                  } },
  { "class/memberdecl/protectedattributes",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProAttribs(),
                                                            []() { return compileOptions(theTranslator->trProtectedAttribs()); })
                                                  } },
  { "class/memberdecl/protectedstaticattributes", { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::ProStaticAttribs(),
                                                            []() { return compileOptions(theTranslator->trStaticProtectedAttribs()); })
                                                  } },
  { "class/memberdecl/packagetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PacTypes(),
                                                            []() { return compileOptions(theTranslator->trPackageTypes()); })
                                                  } },
  { "class/memberdecl/packagemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PacMethods(),
                                                            []() { return compileOptions(theTranslator->trPackageFunctions()); })
                                                  } },
  { "class/memberdecl/packagestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PacStaticMethods(),
                                                            []() { return compileOptions(theTranslator->trStaticPackageFunctions()); })
                                                  } },
  { "class/memberdecl/packageattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PacAttribs(),
                                                            []() { return compileOptions(theTranslator->trPackageAttribs()); })
                                                  } },
  { "class/memberdecl/packagestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PacStaticAttribs(),
                                                            []() { return compileOptions(theTranslator->trStaticPackageAttribs()); })
                                                  } },
  { "class/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Properties(),
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "class/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Events(),
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "class/memberdecl/privatetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriTypes(),
                                                            []() { return compileOptions(theTranslator->trPrivateTypes()); })
                                                  } },
  { "class/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriSlots(),
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "class/memberdecl/privatemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriMethods(),
                                                            []() { return compileOptions(theTranslator->trPrivateMembers()); })
                                                  } },
  { "class/memberdecl/privatestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriStaticMethods(),
                                                            []() { return compileOptions(theTranslator->trStaticPrivateMembers()); })
                                                  } },
  { "class/memberdecl/privateattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriAttribs(),
                                                            []() { return compileOptions(theTranslator->trPrivateAttribs()); })
                                                  } },
  { "class/memberdecl/privatestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::PriStaticAttribs(),
                                                            []() { return compileOptions(theTranslator->trStaticPrivateAttribs()); })
                                                  } },
  { "class/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Friends(),
                                                            []() { return compileOptions(theTranslator->trFriends()); })
                                                  } },
  { "class/memberdecl/related",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Related(),
                                                            []() { return compileOptions(theTranslator->trRelatedSymbols()); },
                                                            []() { return compileOptions(theTranslator->trRelatedSymbolsSubscript()); })
                                                  } },
  { "class/memberdef",                            { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "class/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ClassInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt::Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "class/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::TypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trMemberTypedefDocumentation()); })
                                                  } },
  { "class/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType::EnumMembers(),
                                                            []() { return compileOptions(theTranslator->trMemberEnumerationDocumentation()); })
                                                  } },
  { "class/memberdef/services",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::ServiceMembers(),
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdef/interfaces",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType::InterfaceMembers(),
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdef/constructors",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType::Constructors(),
                                                            []() { return compileOptions(theTranslator->trConstructorDocumentation()); })
                                                  } },
  { "class/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::FunctionMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberFunctionDocumentation(), SrcLangExt::ObjC,   theTranslator->trMethodDocumentation(),
                                                                           SrcLangExt::Fortran,theTranslator->trMemberFunctionDocumentationFortran(),
                                                                           SrcLangExt::Slice,  theTranslator->trOperationDocumentation()); })
                                                  } },
  { "class/memberdef/related",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType::RelatedMembers(),
                                                            []() { return compileOptions(theTranslator->trRelatedSymbolDocumentation()); })
                                                  } },
  { "class/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::VariableMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberDataDocumentation(),
                                                                           SrcLangExt::Slice,  theTranslator->trDataMemberDocumentation()); })
                                                  } },
  { "class/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType::PropertyMembers(),
                                                            []() { return compileOptions(theTranslator->trPropertyDocumentation()); })
                                                  } },
  { "class/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType::EventMembers(),
                                                            []() { return compileOptions(theTranslator->trEventDocumentation()); })
                                                  } },

  // concept layout handlers
  { "concept",                                    { startCb(&LayoutParser::startTop,LayoutDocManager::Concept,"concept/",LayoutNavEntry::Concepts),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "concept/briefdescription",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc) } },
  { "concept/definition",                         { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ConceptDefinition,
                                                            []() { return compileOptions(theTranslator->trConceptDefinition()); }),
                                                  } },
  { "concept/includes",                           { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::ClassIncludes)  } },
  { "concept/sourcelink",                         { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileSourceLink) } },
  { "concept/detaileddescription",                { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "concept/authorsection",                      { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection) } },
  // namespace layout handlers
  { "namespace",                                  { startCb(&LayoutParser::startTop,LayoutDocManager::Namespace,"namespace/",LayoutNavEntry::Namespaces),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "namespace/briefdescription",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc) } },
  { "namespace/detaileddescription",              { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "namespace/authorsection",                    { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection) } },
  { "namespace/memberdecl",                       { startCb(&LayoutParser::startMemberDecl),
                                                    endCb(&LayoutParser::endMemberDecl)
                                                  } },
  { "namespace/memberdecl/nestednamespaces",      { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceNestedNamespaces,
                                                            []() { return compileOptions(/* default */      theTranslator->trNamespaces(),
                                                                           SrcLangExt::Java,   theTranslator->trPackages(),
                                                                           SrcLangExt::VHDL,   theTranslator->trPackages(),
                                                                           SrcLangExt::IDL,    theTranslator->trModules(),
                                                                           SrcLangExt::Fortran,theTranslator->trModules(),
                                                                           SrcLangExt::Slice,(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trModules() :
                                                                                              theTranslator->trNamespaces())); })
                                                  } },
  { "namespace/memberdecl/constantgroups",        { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceNestedConstantGroups,
                                                            []() { return compileOptions(theTranslator->trConstantGroups()); })
                                                  } },
  { "namespace/memberdecl/interfaces",            { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceInterfaces,
                                                            []() { return compileOptions(theTranslator->trSliceInterfaces()); })
                                                  } },
  { "namespace/memberdecl/classes",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trCompounds(),
                                                                           SrcLangExt::VHDL,   theTranslator->trVhdlType(VhdlSpecifier::ENTITY,FALSE),
                                                                           SrcLangExt::Fortran,theTranslator->trDataTypes()); })
                                                  } },
  { "namespace/memberdecl/concepts",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); })
                                                  } },
  { "namespace/memberdecl/structs",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceStructs,
                                                            []() { return compileOptions(theTranslator->trStructs()); })
                                                  } },
  { "namespace/memberdecl/exceptions",            { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceExceptions,
                                                            []() { return compileOptions(theTranslator->trExceptions()); })
                                                  } },
  { "namespace/memberdecl/membergroups",          { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::MemberGroups) } },
  { "namespace/memberdecl/typedefs",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "namespace/memberdecl/sequences",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "namespace/memberdecl/dictionaries",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "namespace/memberdecl/enums",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "namespace/memberdecl/functions",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecFuncMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                           SrcLangExt::Fortran,theTranslator->trSubprograms(),
                                                                           SrcLangExt::VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "namespace/memberdecl/variables",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecVarMembers(),
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstants() :
                                                                                              theTranslator->trVariables()); })
                                                  } },
  { "namespace/memberdecl/properties",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Properties(),
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "namespace/memberdef",                        { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "namespace/memberdef/inlineclasses",          { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt::Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "namespace/memberdef/typedefs",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "namespace/memberdef/sequences",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "namespace/memberdef/dictionaries",           { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "namespace/memberdef/enums",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "namespace/memberdef/functions",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocFuncMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                           SrcLangExt::Fortran,theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "namespace/memberdef/variables",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocVarMembers(),
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstantDocumentation() :
                                                                                              theTranslator->trVariableDocumentation()); })
                                                  } },
  { "namespace/memberdef/properties",             { startCb(&LayoutParser::startMemberDefEntry, MemberListType::PropertyMembers(),
                                                            []() { return compileOptions(theTranslator->trPropertyDocumentation()); })
                                                  } },

  // file layout handlers
  { "file",                                       { startCb(&LayoutParser::startTop,LayoutDocManager::File,"file/",LayoutNavEntry::Files),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "file/briefdescription",                      { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc) } },
  { "file/detaileddescription",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "file/authorsection",                         { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection)       } },
  { "file/includes",                              { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludes)        } },
  { "file/includegraph",                          { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludeGraph)    } },
  { "file/includedbygraph",                       { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludedByGraph) } },
  { "file/sourcelink",                            { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileSourceLink)      } },
  { "file/memberdecl/membergroups",               { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups)        } },
  { "file/memberdecl",                            { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl)  } },
  { "file/memberdecl/interfaces",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileInterfaces,
                                                            []() { return compileOptions(theTranslator->trSliceInterfaces()); })
                                                  } },
  { "file/memberdecl/classes",                    { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trCompounds(),
                                                                                         SrcLangExt::VHDL,   theTranslator->trVhdlType(VhdlSpecifier::ENTITY,FALSE),
                                                                                         SrcLangExt::Fortran,theTranslator->trDataTypes()); })
                                                  } },
  { "file/memberdecl/concepts",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::FileConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); })
                                                  } },
  { "file/memberdecl/structs",                    { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileStructs,
                                                            []() { return compileOptions(theTranslator->trStructs()); })
                                                  } },
  { "file/memberdecl/exceptions",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileExceptions,
                                                            []() { return compileOptions(theTranslator->trExceptions()); })
                                                  } },
  { "file/memberdecl/namespaces",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileNamespaces,
                                                            []() { return compileOptions(/* default */      theTranslator->trNamespaces(),
                                                                                         SrcLangExt::Java,   theTranslator->trPackages(),
                                                                                         SrcLangExt::IDL,    theTranslator->trModules(),
                                                                                         SrcLangExt::Fortran,theTranslator->trModules(),
                                                                                         SrcLangExt::Slice,  theTranslator->trModules()); })
                                                  } },
  { "file/memberdecl/constantgroups",             { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileConstantGroups,
                                                            []() { return compileOptions(theTranslator->trConstantGroups()); })
                                                  } },
  { "file/memberdecl/defines",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecDefineMembers(),
                                                            []() { return compileOptions(theTranslator->trDefines()); })
                                                  } },
  { "file/memberdecl/typedefs",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "file/memberdecl/sequences",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "file/memberdecl/dictionaries",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "file/memberdecl/enums",                      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "file/memberdecl/functions",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecFuncMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt::Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt::VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "file/memberdecl/variables",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecVarMembers(),
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                                            theTranslator->trConstants() :
                                                                                                            theTranslator->trVariables()); })
                                                  } },
  { "file/memberdecl/properties",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::Properties(),
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "file/memberdef",                             { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },

  { "file/memberdef/inlineclasses",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileInlineClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt::Fortran, theTranslator->trTypeDocumentation()); })
                                                  } },
  { "file/memberdef/defines",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocDefineMembers(),
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); })
                                                  } },
  { "file/memberdef/typedefs",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "file/memberdef/sequences",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "file/memberdef/dictionaries",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "file/memberdef/enums",                       { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "file/memberdef/functions",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocFuncMembers(),
                                                            []() { return compileOptions(/* default */       theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt::Fortran, theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "file/memberdef/variables",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocVarMembers(),
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); })
                                                  } },

  { "file/memberdef/properties",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::PropertyMembers(),
                                                            []() { return compileOptions(theTranslator->trPropertyDocumentation()); })
                                                  } },
  // group layout handlers
  { "group",                                      { startCb(&LayoutParser::startTop,LayoutDocManager::Group,"group/",LayoutNavEntry::None),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "group/briefdescription",                     { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc) } },
  { "group/detaileddescription",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "group/authorsection",                        { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection) } },
  { "group/groupgraph",                           { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::GroupGraph)    } },
  { "group/memberdecl",                           { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl) } },
  { "group/memberdecl/membergroups",              { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups)  } },
  { "group/memberdecl/classes",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trCompounds(),
                                                                                         SrcLangExt::VHDL,    theTranslator->trVhdlType(VhdlSpecifier::ENTITY,FALSE),
                                                                                         SrcLangExt::Fortran, theTranslator->trDataTypes()); })
                                                  } },
  { "group/memberdecl/concepts",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); })
                                                  } },
  { "group/memberdecl/modules",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupModules,
                                                            []() { return compileOptions(theTranslator->trModule(true,false)); })
                                                  } },
  { "group/memberdecl/namespaces",                { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNamespaces,
                                                            []() { return compileOptions(/* default */       theTranslator->trNamespaces(),
                                                                                         SrcLangExt::Java,    theTranslator->trPackages(),
                                                                                         SrcLangExt::Fortran, theTranslator->trModules()); })
                                                  } },
  { "group/memberdecl/dirs",                      { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupDirs,
                                                            []() { return compileOptions(theTranslator->trDirectories()); })
                                                  } },
  { "group/memberdecl/nestedgroups",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNestedGroups,
                                                            []() { return compileOptions(theTranslator->trTopics()); })
                                                  } },
  { "group/memberdecl/files",                     { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupFiles,
                                                            []() { return compileOptions(theTranslator->trFile(TRUE,FALSE)); })
                                                  } },
  { "group/memberdecl/defines",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecDefineMembers(),
                                                            []() { return compileOptions(theTranslator->trDefines()); })
                                                  } },
  { "group/memberdecl/typedefs",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "group/memberdecl/sequences",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "group/memberdecl/dictionaries",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "group/memberdecl/enums",                     { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "group/memberdecl/enumvalues",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEnumValMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerationValues()); })
                                                  } },
  { "group/memberdecl/functions",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecFuncMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt::Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt::VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "group/memberdecl/variables",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecVarMembers(),
                                                            []() { return compileOptions(theTranslator->trVariables()); })
                                                  } },
  { "group/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecSignalMembers(),
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "group/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecPubSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "group/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecProSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "group/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecPriSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "group/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEventMembers(),
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "group/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecPropMembers(),
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "group/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecFriendMembers(),
                                                            []() { return compileOptions(theTranslator->trFriends()); })
                                                  } },
  { "group/memberdef",                            { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "group/memberdef/pagedocs",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::GroupPageDocs)    } },
  { "group/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt::Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "group/memberdef/defines",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocDefineMembers(),
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); })
                                                  } },
  { "group/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "group/memberdef/sequences",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocSequenceMembers(),
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "group/memberdef/dictionaries",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocDictionaryMembers(),
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "group/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "group/memberdef/enumvalues",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocEnumValMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerationValueDocumentation()); })
                                                  } },
  { "group/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocFuncMembers(),
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt::Fortran,theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "group/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocVarMembers(),
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); })
                                                  } },
  { "group/memberdef/signals",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocSignalMembers(),
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "group/memberdef/publicslots",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocPubSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "group/memberdef/protectedslots",             { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocProSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "group/memberdef/privateslots",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocPriSlotMembers(),
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "group/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocEventMembers(),
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "group/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocPropMembers(),
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "group/memberdef/friends",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType::DocFriendMembers(),
                                                            []() { return compileOptions(theTranslator->trFriends()); })
                                                  } },

  // module layout handlers
  { "module",                                     { startCb(&LayoutParser::startTop,LayoutDocManager::Module,"module/",LayoutNavEntry::Modules),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "module/briefdescription",                    { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc) } },
  { "module/exportedmodules",                     { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ModuleExports,
                                                            []() { return compileOptions(theTranslator->trExportedModules()); })
                                                  } },
  { "module/detaileddescription",                 { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "module/authorsection",                       { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection) } },
  { "module/memberdecl",                          { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl) } },
  { "module/memberdecl/concepts",                 { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ModuleConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); })
                                                  } },
  { "module/memberdecl/classes",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ModuleClasses,
                                                            []() { return compileOptions(theTranslator->trCompounds()); })
                                                  } },
  { "module/memberdecl/enums",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecEnumMembers(),
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "module/memberdecl/typedefs",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecTypedefMembers(),
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "module/memberdecl/functions",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecFuncMembers(),
                                                            []() { return compileOptions(theTranslator->trFunctions()); })
                                                  } },
  { "module/memberdecl/variables",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType::DecVarMembers(),
                                                            []() { return compileOptions(theTranslator->trVariables()); })
                                                  } },
  { "module/memberdecl/membergroups",             { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups)  } },
  { "module/memberdecl/files",                    { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ModuleUsedFiles,
                                                            []() { return compileOptions(theTranslator->trFile(TRUE,FALSE)); })
                                                  } },

  // directory layout handlers
  { "directory",                                  { startCb(&LayoutParser::startTop,LayoutDocManager::Directory,"directory/",LayoutNavEntry::None),
                                                    endCb(&LayoutParser::endTop)
                                                  } },
  { "directory/briefdescription",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc)          } },
  { "directory/detaileddescription",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); })
                                                  } },
  { "directory/directorygraph",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirGraph)           } },
  { "directory/memberdecl",                       { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl) } },
  { "directory/memberdecl/dirs",                  { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirSubDirs)         } },
  { "directory/memberdecl/files",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirFiles)           } },
};
// clang-format on

} // namespace

void LayoutParser::startElement( const std::string &name, const XMLHandlers::Attributes& attrib )
{
  //printf("startElement [%s]::[%s]\n",qPrint(m_scope),qPrint(name));
  auto it = g_elementHandlers.find(m_scope.str()+name);
  if (it!=g_elementHandlers.end())
  {
    it->second.startCb(*this,it->first,attrib);
  }
  else
  {
    QCString fileName = m_locator->fileName();
    warn_layout(fileName,m_locator->lineNr(),"Unexpected start tag '{}' found in scope='{}'!",
        name,m_scope);
  }
}

void LayoutParser::endElement( const std::string &name )
{
  //printf("endElement [%s]::[%s]\n",qPrint(m_scope),qPrint(name));
  auto it=g_elementHandlers.end();

  if (!m_scope.isEmpty() && m_scope.right(name.length()+1)==name+"/")
  { // element ends current scope
    it = g_elementHandlers.find(m_scope.left(m_scope.length()-1).str());
  }
  else // continue with current scope
  {
    it = g_elementHandlers.find(m_scope.str()+name);
  }
  if (it!=g_elementHandlers.end())
  {
    it->second.endCb(*this,it->first+" end"); // added end to id to make it unique
  }
}

//---------------------------------------------------------------------------------

int LayoutParser::m_userGroupCount=0;

//---------------------------------------------------------------------------------

class LayoutDocManager::Private
{
  public:
    std::array<LayoutDocEntryList,LayoutDocManager::NrParts> docEntries;
    LayoutNavEntry rootNav;
    int majorVersion;
    int minorVersion;
};

LayoutDocManager::LayoutDocManager() : d(std::make_unique<Private>())
{
}

LayoutDocManager::~LayoutDocManager()
{
}

void LayoutDocManager::init()
{
  LayoutParser layoutParser(LayoutDocManager::instance());
  XMLHandlers handlers;
  handlers.startElement = [&layoutParser](const std::string &name,const XMLHandlers::Attributes &attrs) { layoutParser.startElement(name,attrs); };
  handlers.endElement   = [&layoutParser](const std::string &name) { layoutParser.endElement(name); };
  handlers.error        = [&layoutParser](const std::string &fileName,int lineNr,const std::string &msg) { layoutParser.error(fileName,lineNr,msg); };
  XMLParser parser(handlers);
  layoutParser.setDocumentLocator(&parser);
  constexpr auto layoutFile = "layout_default.xml";
  QCString layout_default = ResourceMgr::instance().getAsString(layoutFile);
  parser.parse(layoutFile,layout_default.data(),Debug::isFlagSet(Debug::Lex_xml),
               [&]() { DebugLex::print(Debug::Lex_xml,"Entering","libxml/xml.l",layoutFile); },
               [&]() { DebugLex::print(Debug::Lex_xml,"Finished", "libxml/xml.l",layoutFile); }
              );
  removeInvisibleDocEntries();
  d->majorVersion = layoutParser.majorVersion();
  d->minorVersion = layoutParser.minorVersion();
}

LayoutDocManager & LayoutDocManager::instance()
{
  static LayoutDocManager theInstance;
  return theInstance;
}

const LayoutDocEntryList &LayoutDocManager::docEntries(LayoutDocManager::LayoutPart part) const
{
  return d->docEntries[static_cast<int>(part)];
}

LayoutNavEntry* LayoutDocManager::rootNavEntry() const
{
  return &d->rootNav;
}

LayoutNavEntry *LayoutDocManager::createChildNavEntry(LayoutNavEntry *parent,
                                                      LayoutNavEntry::Kind k,bool vs,const QCString &bf,
                                                      const QCString &tl,const QCString &intro)
{
  if (parent==nullptr) parent = &d->rootNav;
  auto ptr = std::make_unique<LayoutNavEntry>(parent,k,vs,bf,tl,intro);
  auto child = ptr.get();
  parent->appendChild(std::move(ptr));
  return child;
}

void LayoutDocManager::addEntry(LayoutDocManager::LayoutPart p,LayoutDocEntryPtr &&e)
{
  auto &docEntry = d->docEntries[static_cast<int>(p)];
  docEntry.push_back(std::move(e)); // add
}

void LayoutDocManager::parse(const QCString &fileName, const char *data)
{
  //printf("============ LayoutDocManager::parse(%s)\n",qPrint(fileName));
  LayoutDocManager layoutDocManager;
  LayoutParser layoutParser(layoutDocManager);
  XMLHandlers handlers;
  handlers.startElement = [&layoutParser](const std::string &name,const XMLHandlers::Attributes &attrs) { layoutParser.startElement(name,attrs); };
  handlers.endElement   = [&layoutParser](const std::string &name) { layoutParser.endElement(name); };
  handlers.error        = [&layoutParser](const std::string &fn,int lineNr,const std::string &msg) { layoutParser.error(fn,lineNr,msg); };
  XMLParser parser(handlers);
  layoutParser.setDocumentLocator(&parser);
  parser.parse(fileName.data(),
               data ? data : fileToString(fileName).data(),
               Debug::isFlagSet(Debug::Lex_xml),
               [&]() { DebugLex::print(Debug::Lex_xml,"Entering","libxml/xml.l",qPrint(fileName)); },
               [&]() { DebugLex::print(Debug::Lex_xml,"Finished", "libxml/xml.l",qPrint(fileName)); },
               transcodeCharacterStringToUTF8
              );

  // version in the user defined layout overrides default one
  d->majorVersion = layoutParser.majorVersion();
  d->minorVersion = layoutParser.minorVersion();

  // merge missing parts of the default layout into the user defined layout
  // For now merging in defaults has been disabled for navigation entries
  // to avoid "extra entries" for projects that work with partial layout files.
  //mergeNavEntries(layoutDocManager);

  // for compatibility reasons we only merge defaults when the user defined layout has at least version 2.0 or higher
  if (d->majorVersion>=2)
  {
    mergeDocEntries(fileName,layoutDocManager);
  }

  layoutDocManager.removeInvisibleDocEntries();

  // replace default layout with merged layout
  d->docEntries.swap(layoutDocManager.d->docEntries);
  d->rootNav.swap(layoutDocManager.d->rootNav);
}

void LayoutDocManager::removeInvisibleDocEntries()
{
  // remove invisible entries
  for (auto &list : d->docEntries)
  {
    auto it = list.begin();
    while (it!=list.end())
    {
      if (*it==nullptr || !(*it)->visible())
      {
        it = list.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }
}

int LayoutDocManager::majorVersion() const
{
  return d->majorVersion;
}

int LayoutDocManager::minorVersion() const
{
  return d->minorVersion;
}

// search for candidate node in tree with root target. Returns the match target node if found, or nullptr otherwise.
static LayoutNavEntry *findNavEntryRec(LayoutNavEntry *root, const std::string &id)
{
  if (root)
  {
    if (root->id()==id) return root;
    for (auto &child : root->children())
    {
      LayoutNavEntry *childNavEntry = findNavEntryRec(child.get(),id);
      if (childNavEntry) return childNavEntry;
    }
  }
  return nullptr;
}

// merge the missing nodes
static void mergeNavTreeNodesRec(LayoutNavEntry *targetTree,LayoutNavEntry *sourceTree)
{
  using IdSet = std::unordered_set<std::string>;
  using IdMap = std::unordered_map<std::string,size_t>;

  auto prepareSet = [](const LayoutNavEntry *tree, IdSet &set) {
    for (const auto &e : tree->children())
    {
      set.insert(e->id());
    }
  };

  auto prepareMap = [](const LayoutNavEntry *tree, const IdSet &set, IdMap &map) {
    for (size_t i=0; i<tree->children().size(); i++)
    {
      std::string id = tree->children()[i]->id();
      auto it = set.find(id);
      if (it != set.end())
      {
        map[id]=i;
      }
    }
  };

  IdSet sourceSet, targetSet;
  prepareSet(sourceTree,sourceSet);
  prepareSet(targetTree,targetSet);

  IdMap sourceMap, targetMap;
  prepareMap(targetTree,sourceSet,targetMap);
  prepareMap(sourceTree,targetSet,sourceMap);

  // calculate list of insertion positions in the target list for each id in the source list
  std::vector<size_t> insertionList;
  for (size_t i=0; i<sourceTree->children().size(); i++)
  {
    std::string id = sourceTree->children()[i]->id();
    // If an id in the source list appears before a set of shared ids we want it also to
    // appear before these id in the target list. To do this find the lowest target index of all shared
    // ids that come after position i in the source list
    size_t minIdx = targetTree->children().size();
    for (const auto &kv : sourceMap) // iterator over shared ids
    {
      if (i < kv.second) // i appears before this shared id
      {
        size_t idx = targetMap[kv.first]; // get the corresponding index in the target list
        if (idx<minIdx) // update minimum
        {
          minIdx=idx;
        }
      }
    }
    insertionList.push_back(minIdx);
  }

  // Insert the missing elements of the source list into the target list.
  // Work backwards so the calculated insertion points don't change due to insertions.
  size_t idx = sourceTree->children().size()-1;
  for (auto it=insertionList.rbegin(); it!=insertionList.rend(); ++it, idx--)
  {
    std::string id = sourceTree->children()[idx]->id();
    if (targetSet.find(id)==targetSet.end()) // need to add id
    {
      // for efficiency we move the elements from the source list to the target list, thus modifying the source list!
      targetTree->insertChild(*it, std::move(sourceTree->children()[idx]));
    }
  }

  for (auto &targetChild : targetTree->children())
  {
    auto *node = findNavEntryRec(sourceTree,targetChild->id());
    if (node)
    {
      mergeNavTreeNodesRec(targetChild.get(),node);
    }
  }
}

void LayoutDocManager::mergeNavEntries(LayoutDocManager &other)
{
  mergeNavTreeNodesRec(other.rootNavEntry(),rootNavEntry());
}

static void mergeDocEntryLists(const QCString &fileName,LayoutDocEntryList &targetList,LayoutDocEntryList &sourceList)
{
  using IdSet = std::unordered_set<std::string>;
  using IdMap = std::unordered_map<std::string,size_t>;

  auto prepareSet = [](const LayoutDocEntryList &list, IdSet &set) {
    //size_t idx=0;
    for (const auto &e : list)
    {
      //printf("idx=%zu set(%s)\n",idx,qPrint(e->id()));
      set.insert(e->id());
      //idx++;
    }
  };

  auto prepareMap = [](const LayoutDocEntryList &list, const IdSet &set, IdMap &map) {
    for (size_t i=0; i<list.size(); i++)
    {
      std::string id = list[i]->id();
      auto it = set.find(id);
      if (it != set.end())
      {
        //printf("map %s->%zu\n",qPrint(id),i);
        map[id]=i;
      }
    }
  };

  IdSet sourceSet, targetSet;
  //printf("---- sourceSet\n");
  prepareSet(sourceList,sourceSet);
  //printf("---- targetSet\n");
  prepareSet(targetList,targetSet);

  IdMap sourceMap, targetMap;
  //printf("---- targetMap\n");
  prepareMap(targetList,sourceSet,targetMap);
  //printf("---- sourceMap\n");
  prepareMap(sourceList,targetSet,sourceMap);

  // calculate list of insertion positions in the target list for each id in the source list
  std::vector<size_t> insertionList;
  for (size_t i=0; i<sourceList.size(); i++)
  {
    std::string id = sourceList[i]->id();
    // If an id in the source list appears before a set of shared ids we want it also to
    // appear before these id in the target list. To do this find the lowest target index of all shared
    // ids that come after position i in the source list
    size_t minIdx = targetList.size();
    for (const auto &kv : sourceMap) // iterator over shared ids
    {
      if (i < kv.second) // i appears before this shared id
      {
        size_t idx = targetMap[kv.first]; // get the corresponding index in the target list
        //printf("  evaluating %s->%zu min=%zu\n",qPrint(kv.first),idx,minIdx);
        if (idx<minIdx) // update minimum
        {
          minIdx=idx;
        }
      }
    }
    //printf("insertion %s->%zu\n",qPrint(id),minIdx);
    insertionList.push_back(minIdx);
  }

  // Insert the missing elements of the source list into the target list.
  // Work backwards so the calculated insertion points don't change due to insertions.
  size_t idx = sourceList.size()-1;
  for (auto it=insertionList.rbegin(); it!=insertionList.rend(); ++it, idx--)
  {
    std::string id = sourceList[idx]->id();
    //printf("idx=%zu entry %s\n",idx,qPrint(id));
    if (targetSet.find(id)==targetSet.end()) // need to add id
    {
      // for efficiency we move the elements from the source list to the target list, thus modifying the source list!
      //printf("--> insert at %zu before %s\n",*it,qPrint(*it<targetList.size()?targetList[*it]->id():"none"));
      warn_layout(fileName,1,"User defined layout misses entry '{}'. Using default value.",id);
      targetList.insert(targetList.begin()+*it, std::move(sourceList[idx]));
    }
  }

}

void LayoutDocManager::mergeDocEntries(const QCString &fileName,LayoutDocManager &other)
{
   for (size_t i=0; i<d->docEntries.size(); i++)
   {
     //printf("========= part %zu\n",i);
     mergeDocEntryLists(fileName,other.d->docEntries[i],d->docEntries[i]);
   }
}

//---------------------------------------------------------------------------------

void writeDefaultLayoutFile(const QCString &fileName)
{
  std::ofstream f;
  if (openOutputFile(fileName,f))
  {
    TextStream t(&f);
    QCString layout_default = ResourceMgr::instance().getAsString("layout_default.xml");
    t << substitute(layout_default,"$doxygenversion",getDoxygenVersion());
  }
  else
  {
    err("Failed to open file {} for writing!\n",fileName);
    return;
  }
  f.close();
}

//----------------------------------------------------------------------------------

// Convert input to a title.
// The format of input can be a simple title "A title" or in case there are different
// titles for some programming languages they can take the following form:
// "A title|16=Another title|8=Yet Another title"
// where the number is a value of SrcLangExt in decimal notation (i.e. 16=Java, 8=IDL).
QCString extractLanguageSpecificTitle(const QCString &input,SrcLangExt lang)
{
  int s=0,e=input.find('|');
  if (e==-1) return input; // simple title case
  int e1=e;
  while (e!=-1) // look for 'number=title' pattern separated by '|'
  {
    s=e+1;
    e=input.find('|',s);
    int i=input.find('=',s);
    assert(i>s);
    SrcLangExt key= static_cast<SrcLangExt>(input.mid(s,i-s).toUInt());
    if (key==lang) // found matching key
    {
      if (e==-1) e=static_cast<int>(input.length());
      return input.mid(i+1,e-i-1);
    }
  }
  return input.left(e1); // fallback, no explicit language key found
}

//----------------------------------------------------------------------------------

QCString LayoutDocEntrySection::title(SrcLangExt lang) const
{
  return extractLanguageSpecificTitle(m_title,lang);
}

//----------------------------------------------------------------------------------

QCString LayoutDocEntryMemberDecl::title(SrcLangExt lang) const
{
  return extractLanguageSpecificTitle(m_title,lang);
}

QCString LayoutDocEntryMemberDecl::subtitle(SrcLangExt lang) const
{
  return extractLanguageSpecificTitle(m_subscript,lang);
}

//----------------------------------------------------------------------------------

QCString LayoutDocEntryMemberDef::title(SrcLangExt lang) const
{
  return extractLanguageSpecificTitle(m_title,lang);
}

//----------------------------------------------------------------------------------

static void printNavLayout(LayoutNavEntry *root,int indent)
{
  if (Debug::isFlagSet(Debug::Layout))
  {
    QCString indentStr;
    indentStr.fill(' ',indent);
    Debug::print(Debug::Layout,0,"{}kind={} visible={} title='{}'\n",
        indentStr, root->navToString(), root->visible(), root->title());
    for (const auto &e : root->children())
    {
      printNavLayout(e.get(),indent+2);
    }
  }
}

void printLayout()
{
  bool extraIndent = false;

  auto &mgr = LayoutDocManager::instance();
  Debug::print(Debug::Layout,0,"Layout version {}.{}\n",mgr.majorVersion(),mgr.minorVersion());

  Debug::print(Debug::Layout,0,"Part: Navigation index\n");
  for (const auto &e : mgr.rootNavEntry()->children())
  {
    printNavLayout(e.get(),2);
  }

  for (int i = 0; i < LayoutDocManager::NrParts; i++)
  {
     Debug::print(Debug::Layout,0,"\nPart: {}\n", LayoutDocManager::partToString(i));
     for (const auto &lde : mgr.docEntries(static_cast<LayoutDocManager::LayoutPart>(i)))
     {
       if (const LayoutDocEntrySimple *ldes = dynamic_cast<const LayoutDocEntrySimple*>(lde.get()))
       {
         if (lde->kind() == LayoutDocEntry::MemberDeclEnd || lde->kind() == LayoutDocEntry::MemberDefEnd) extraIndent = false;
         Debug::print(Debug::Layout,0,"  {}kind: {}, visible={}\n",
           extraIndent? "  " : "",lde->entryToString(), ldes->visible());
         if (lde->kind() == LayoutDocEntry::MemberDeclStart || lde->kind() == LayoutDocEntry::MemberDefStart) extraIndent = true;
       }
       else if (const LayoutDocEntryMemberDecl *lmdecl = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get()))
       {
         Debug::print(Debug::Layout,0,"  {}complex kind: {}, visible={}, type: {}\n",
           extraIndent? "  " : "",lde->entryToString(),lmdecl->visible(),lmdecl->type.to_string());
       }
       else if (const LayoutDocEntryMemberDef *lmdef = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get()))
       {
         Debug::print(Debug::Layout,0,"  {}complex kind: {}, visible={}, type: {}\n",
           extraIndent? "  " : "",lde->entryToString(),lmdef->visible(),lmdef->type.to_string());
       }
       else
       {
         // should not happen
         Debug::print(Debug::Layout,0,"  {}not handled kind: {}\n",extraIndent? "  " : "",lde->entryToString());
       }
     }
  }
}



