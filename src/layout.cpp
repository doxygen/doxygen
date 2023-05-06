/******************************************************************************
 *
 *
 *
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

#include <array>

#include <assert.h>

#include "layout.h"
#include "message.h"
#include "language.h"
#include "vhdldocgen.h"
#include "util.h"
#include "doxygen.h"
#include "version.h"
#include "config.h"
#include "xml.h"
#include "resourcemgr.h"
#include "debug.h"

inline QCString compileOptions(const QCString &def)
{
  return def;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1)
{
  return compileOptions(def)+"|"+QCString().setNum(langId1)+"="+value1;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2)
{
  return compileOptions(def,langId1,value1)+
         "|"+QCString().setNum(langId2)+"="+value2;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3)
{
  return compileOptions(def,langId1,value1,langId2,value2)+
         "|"+QCString().setNum(langId3)+"="+value3;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3,
                                                   SrcLangExt langId4,const QCString &value4)
{
  return compileOptions(def,langId1,value1,langId2,value2,langId3,value3)+
         "|"+QCString().setNum(langId4)+"="+value4;
}

inline QCString compileOptions(const QCString &def,SrcLangExt langId1,const QCString &value1,
                                                   SrcLangExt langId2,const QCString &value2,
                                                   SrcLangExt langId3,const QCString &value3,
                                                   SrcLangExt langId4,const QCString &value4,
                                                   SrcLangExt langId5,const QCString &value5)
{
  return compileOptions(def,langId1,value1,langId2,value2,langId3,value3,langId4,value4)+
         "|"+QCString().setNum(langId5)+"="+value5;
}

static bool elemIsVisible(const XMLHandlers::Attributes &attrib,bool defVal=TRUE)
{
  QCString visible = XMLHandlers::value(attrib,"visible");
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
      err("found unsupported value '%s' for visible attribute in layout file, reverting to '%s'\n",
          qPrint(visible),(defVal?"yes":"no"));
      return defVal;
    }
  }
  QCString visibleLow = visible.lower();
  if (visibleLow=="no" || visibleLow=="false" || visibleLow=="0") return FALSE;
  else if (visibleLow=="yes" || visibleLow=="true" || visibleLow=="1") return TRUE;
  else
  {
    err("found unsupported value '%s' for visible attribute in layout file, reverting to '%s'\n",
        qPrint(visible),(defVal?"yes":"no"));
    return defVal;
  }
}

static bool parentIsVisible(LayoutNavEntry *parent)
{
  return parent==0 || parent->visible();
}

//---------------------------------------------------------------------------------

LayoutNavEntry *LayoutNavEntry::find(LayoutNavEntry::Kind kind,
    const QCString &file) const
{
  LayoutNavEntry *result=0;
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
    const Definition *d = 0;
    QCString anchor;
    bool found=FALSE;
    if (resolveLink(QCString(),url.mid(5).stripWhiteSpace(),TRUE,&d,anchor))
    {
      if (d && d->isLinkable())
      {
        url = d->getOutputFileBase();
        addHtmlExtensionIfMissing(url);
        if (!anchor.isEmpty())
        {
          url+="#"+anchor;
        }
        found=TRUE;
      }
    }
    if (!found)
    {
      msg("explicit link request to '%s' in layout file '%s' could not be resolved\n",qPrint(url.mid(5)),qPrint(Config_getString(LAYOUT_FILE)));
    }
  }
  //printf("LayoutNavEntry::url()=%s\n",qPrint(url));
  return url;
}

//---------------------------------------------------------------------------------

class LayoutParser
{
  public:
    static LayoutParser &instance()
    {
      static LayoutParser theInstance;
      return theInstance;
    }

    // =========== XMLHandler events
    void setDocumentLocator(const XMLLocator *locator)
    {
      m_locator = locator;
    }
    void error( const std::string &fileName,int lineNr,const std::string &msg)
    {
      ::warn(fileName.c_str(),lineNr,"%s",msg.c_str());
    }
    void startElement( const std::string &name, const XMLHandlers::Attributes& attrib );
    void endElement( const std::string &name );

    void startSimpleEntry(LayoutDocEntry::Kind k,const XMLHandlers::Attributes &attrib)
    {
      bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(k,isVisible));
      }
    }

    // ============ Specific callbacks

    void startSectionEntry(LayoutDocEntry::Kind k,const XMLHandlers::Attributes &attrib,
                           const QCString &title)
    {
      bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      QCString userTitle = XMLHandlers::value(attrib,"title");
      //printf("startSectionEntry: title='%s' userTitle='%s'\n",
      //    qPrint(title),qPrint(userTitle));
      if (userTitle.isEmpty())  userTitle = title;
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySection>(k,userTitle,isVisible));
      }
    }


    void startMemberDeclEntry(const XMLHandlers::Attributes &attrib,MemberListType type,
                              const QCString &title,const QCString &subscript)
    {
      QCString userTitle     = XMLHandlers::value(attrib,"title");
      QCString userSubscript = XMLHandlers::value(attrib,"subtitle");
      if (userTitle.isEmpty())     userTitle     = title;
      if (userSubscript.isEmpty()) userSubscript = subscript;
      bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntryMemberDecl>(type,userTitle,userSubscript));
      }
    }

    void startMemberDefEntry(const XMLHandlers::Attributes &attrib,MemberListType type,
                             const QCString &title,const QCString &)
    {
      QCString userTitle = XMLHandlers::value(attrib,"title");
      if (userTitle.isEmpty()) userTitle = title;
      //printf("memberdef: %s\n",qPrint(userTitle));
      bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntryMemberDef>(type,userTitle));
      }
    }

    void startLayout(const XMLHandlers::Attributes &)
    {
    }

    void startNavIndex(const XMLHandlers::Attributes &)
    {
      m_scope="navindex/";
      m_rootNav = LayoutDocManager::instance().rootNavEntry();
      if (m_rootNav)
      {
        m_rootNav->clear();
      }
    }

    void endNavIndex()
    {
      m_scope="";
      if (m_rootNav && !m_rootNav->find(LayoutNavEntry::MainPage))
      {
        // no MainPage node... add one as the first item of the root node...
        m_rootNav->prependChild(std::make_unique<LayoutNavEntry>(m_rootNav,LayoutNavEntry::MainPage, TRUE,
                                                   "index",theTranslator->trMainPage(),""));
      }
    }

    void startNavEntry(const XMLHandlers::Attributes &attrib)
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
        { "modules",
          LayoutNavEntry::Modules,
          theTranslator->trModules(),
          QCString(),
          theTranslator->trModulesDescription(),
          "modules"
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
        { 0, // end of list
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
      if (mapping[i].typeStr==0)
      {
        std::string fileName = m_locator->fileName();
        if (type.isEmpty())
        {
          ::warn(fileName.c_str(),m_locator->lineNr(),"an entry tag within a navindex has no type attribute! Check your layout file!\n");
        }
        else
        {
          ::warn(fileName.c_str(),m_locator->lineNr(),"the type '%s' is not supported for the entry tag within a navindex! Check your layout file!\n",qPrint(type));
        }
        m_invalidEntry=TRUE;
        return;
      }
      LayoutNavEntry::Kind kind = mapping[i].kind;
      QCString baseFile = mapping[i].baseFile;
      QCString title = XMLHandlers::value(attrib,"title");
      bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      if (title.isEmpty()) // use default title
      {
        title = mapping[i].mainName; // use title for main row
        if (m_rootNav!=LayoutDocManager::instance().rootNavEntry() && !mapping[i].subName.isEmpty())
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
      m_rootNav = LayoutDocManager::instance().createChildNavEntry(m_rootNav,kind,isVisible,baseFile,title,intro);
    }

    void endNavEntry()
    {
      // set the root back to the parent
      if (m_rootNav && !m_invalidEntry) m_rootNav = m_rootNav->parent();
      m_invalidEntry=FALSE;
    }

    void recurseNavEntryChildren(LayoutNavEntry &lne)
    {
      bool vis = lne.visible();
      for (const auto &lne1 : lne.children())
      {
        lne1->setVisible(vis && lne1->visible());
        recurseNavEntryChildren(*lne1);
      }
    }

    void startTop(const XMLHandlers::Attributes &attrib,LayoutDocManager::LayoutPart part,
                  const QCString &scope, LayoutNavEntry::Kind nav)
    {
      LayoutDocManager::instance().clear(part);
      m_scope = scope;
      m_part = part;
      m_visible = elemIsVisible(attrib);
      auto *lne = LayoutDocManager::instance().rootNavEntry()->find(nav);
      if (lne)
      {
        m_visible = m_visible && lne->visible();
        lne->setVisible(m_visible);
        recurseNavEntryChildren(*lne);
      }
    }

    void endTop()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startMemberDef(const XMLHandlers::Attributes &attrib)
    {
      m_scope+="memberdef/";
      if (m_part!=LayoutDocManager::Undefined)
      {
        bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDefStart,isVisible));
      }
    }

    void endMemberDef()
    {
      QCString scopeOrg = m_scope;
      int i=m_scope.findRev("memberdef/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        if (m_part!=LayoutDocManager::Undefined)
        {
          bool isVisible = true;
          for (const auto &lde : LayoutDocManager::instance().docEntries(m_part))
          {
            if (lde->kind() == LayoutDocEntry::MemberDefStart)
            {
               isVisible = static_cast<const LayoutDocEntrySimple*>(lde.get())->visible();
            }
          }
          LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDefEnd,isVisible));
        }
      }
    }

    void startMemberDecl(const XMLHandlers::Attributes &attrib)
    {
      m_scope+="memberdecl/";
      if (m_part!=LayoutDocManager::Undefined)
      {
        bool isVisible = m_visible && elemIsVisible(attrib) && parentIsVisible(m_rootNav);
        LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDeclStart,isVisible));
      }
    }

    void endMemberDecl()
    {
      int i=m_scope.findRev("memberdecl/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        if (m_part!=LayoutDocManager::Undefined)
        {
          bool isVisible = true;
          for (const auto &lde : LayoutDocManager::instance().docEntries(m_part))
          {
            if (lde->kind() == LayoutDocEntry::MemberDeclStart)
            {
               isVisible = static_cast<const LayoutDocEntrySimple*>(lde.get())->visible();
            }
          }
          LayoutDocManager::instance().addEntry(m_part,std::make_unique<LayoutDocEntrySimple>(LayoutDocEntry::MemberDeclEnd,isVisible));
        }
      }
    }

  private:
    QCString m_scope;
    LayoutDocManager::LayoutPart m_part = LayoutDocManager::Undefined;
    LayoutNavEntry *m_rootNav = 0;
    bool m_invalidEntry = false;
    bool m_visible = true;
    static int m_userGroupCount;
    const XMLLocator *m_locator = nullptr;
};

//---------------------------------------------------------------------------------

namespace {

struct ElementCallbacks
{
  using StartCallback = std::function<void(LayoutParser&,const XMLHandlers::Attributes&)>;
  using EndCallback   = std::function<void(LayoutParser&)>;

  StartCallback startCb;
  EndCallback   endCb = [](LayoutParser &){};
};

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...))
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocEntry::Kind kind
                   )
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,attr); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocEntry::Kind kind,
                    const std::function<QCString()> &title
                   )
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,attr,title()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    MemberListType type,
                    const std::function<QCString()> &title
                   )
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr,type,title(),QCString()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    MemberListType type,
                    const std::function<QCString()> &title,
                    const std::function<QCString()> &subtitle
                   )
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr,type,title(),subtitle()); };
}

template<class...Args>
static auto startCb(void (LayoutParser::*fn)(Args...),
                    LayoutDocManager::LayoutPart part,
                    const QCString &scope,
                    LayoutNavEntry::Kind nav
                   )
{
  return [=](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr,part,scope,nav); };
}

static auto endCb(void (LayoutParser::*fn)())
{
  return [=](LayoutParser &parser) { (parser.*fn)(); };
}

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
                                                                                         SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran,theTranslator->trDataTypes()); })
                                                  } },
  { "class/memberdecl/services",                  { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_services,
                                                            []() { return compileOptions(theTranslator->trServices()); })
                                                  } },
  { "class/memberdecl/interfaces",                { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_interfaces,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdecl/publictypes",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_pubTypes,
                                                            []() { return compileOptions(theTranslator->trPublicTypes()); })
                                                  } },
  { "class/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_pubSlots,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "class/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_signals,
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "class/memberdecl/publicmethods",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubMethods,
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicMembers(),
                                                                                         SrcLangExt_ObjC, theTranslator->trInstanceMethods(),
                                                                                         SrcLangExt_Slice,theTranslator->trOperations()); })
                                                  } },
  { "class/memberdecl/publicstaticmethods",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubStaticMethods,
                                                            []() { return compileOptions(/* default */    theTranslator->trStaticPublicMembers(),
                                                                                         SrcLangExt_ObjC, theTranslator->trClassMethods()); })
                                                  } },
  { "class/memberdecl/publicattributes",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubAttribs,
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicAttribs(),
                                                                                         SrcLangExt_Slice,theTranslator->trDataMembers()); })
                                                  } },
  { "class/memberdecl/publicstaticattributes",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPublicAttribs()); })
                                                  } },
  { "class/memberdecl/protectedtypes",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proTypes,
                                                            []() { return compileOptions(theTranslator->trProtectedTypes()); })
                                                  } },
  { "class/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proSlots,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "class/memberdecl/protectedmethods",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proMethods,
                                                            []() { return compileOptions(theTranslator->trProtectedMembers()); })
                                                  } },
  { "class/memberdecl/protectedstaticmethods",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticProtectedMembers()); })
                                                  } },
  { "class/memberdecl/protectedattributes",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proAttribs,
                                                            []() { return compileOptions(theTranslator->trProtectedAttribs()); })
                                                  } },
  { "class/memberdecl/protectedstaticattributes", { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticProtectedAttribs()); })
                                                  } },
  { "class/memberdecl/packagetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacTypes,
                                                            []() { return compileOptions(theTranslator->trPackageTypes()); })
                                                  } },
  { "class/memberdecl/packagemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacMethods,
                                                            []() { return compileOptions(theTranslator->trPackageFunctions()); })
                                                  } },
  { "class/memberdecl/packagestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticPackageFunctions()); })
                                                  } },
  { "class/memberdecl/packageattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacAttribs,
                                                            []() { return compileOptions(theTranslator->trPackageAttribs()); })
                                                  } },
  { "class/memberdecl/packagestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPackageAttribs()); })
                                                  } },
  { "class/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_properties,
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "class/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_events,
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "class/memberdecl/privatetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priTypes,
                                                            []() { return compileOptions(theTranslator->trPrivateTypes()); })
                                                  } },
  { "class/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priSlots,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "class/memberdecl/privatemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priMethods,
                                                            []() { return compileOptions(theTranslator->trPrivateMembers()); })
                                                  } },
  { "class/memberdecl/privatestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticPrivateMembers()); })
                                                  } },
  { "class/memberdecl/privateattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priAttribs,
                                                            []() { return compileOptions(theTranslator->trPrivateAttribs()); })
                                                  } },
  { "class/memberdecl/privatestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPrivateAttribs()); })
                                                  } },
  { "class/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_friends,
                                                            []() { return compileOptions(theTranslator->trFriends()); })
                                                  } },
  { "class/memberdecl/related",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_related,
                                                            []() { return compileOptions(theTranslator->trRelatedSymbols()); },
                                                            []() { return compileOptions(theTranslator->trRelatedSymbolsSubscript()); })
                                                  } },
  { "class/memberdef",                            { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "class/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ClassInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "class/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_typedefMembers,
                                                            []() { return compileOptions(theTranslator->trMemberTypedefDocumentation()); })
                                                  } },
  { "class/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType_enumMembers,
                                                            []() { return compileOptions(theTranslator->trMemberEnumerationDocumentation()); })
                                                  } },
  { "class/memberdef/services",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_serviceMembers,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdef/interfaces",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_interfaceMembers,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); })
                                                  } },
  { "class/memberdef/constructors",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_constructors,
                                                            []() { return compileOptions(theTranslator->trConstructorDocumentation()); })
                                                  } },
  { "class/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_functionMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberFunctionDocumentation(), SrcLangExt_ObjC,   theTranslator->trMethodDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trMemberFunctionDocumentationFortran(),
                                                                           SrcLangExt_Slice,  theTranslator->trOperationDocumentation()); })
                                                  } },
  { "class/memberdef/related",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_relatedMembers,
                                                            []() { return compileOptions(theTranslator->trRelatedSymbolDocumentation()); })
                                                  } },
  { "class/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_variableMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberDataDocumentation(),
                                                                           SrcLangExt_Slice,  theTranslator->trDataMemberDocumentation()); })
                                                  } },
  { "class/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_propertyMembers,
                                                            []() { return compileOptions(theTranslator->trPropertyDocumentation()); })
                                                  } },
  { "class/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_eventMembers,
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
                                                                           SrcLangExt_Java,   theTranslator->trPackages(),
                                                                           SrcLangExt_VHDL,   theTranslator->trPackages(),
                                                                           SrcLangExt_IDL,    theTranslator->trModules(),
                                                                           SrcLangExt_Fortran,theTranslator->trModules(),
                                                                           SrcLangExt_Slice,(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
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
                                                                           SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                           SrcLangExt_Fortran,theTranslator->trDataTypes()); })
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
  { "namespace/memberdecl/typedefs",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "namespace/memberdecl/sequences",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "namespace/memberdecl/dictionaries",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "namespace/memberdecl/enums",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "namespace/memberdecl/functions",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                           SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                           SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "namespace/memberdecl/variables",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstants() :
                                                                                              theTranslator->trVariables()); })
                                                  } },
  { "namespace/memberdef",                        { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "namespace/memberdef/inlineclasses",          { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "namespace/memberdef/typedefs",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "namespace/memberdef/sequences",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "namespace/memberdef/dictionaries",           { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "namespace/memberdef/enums",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "namespace/memberdef/functions",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "namespace/memberdef/variables",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstantDocumentation() :
                                                                                              theTranslator->trVariableDocumentation()); })
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
                                                                                         SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran,theTranslator->trDataTypes()); })
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
                                                                                         SrcLangExt_Java,   theTranslator->trPackages(),
                                                                                         SrcLangExt_IDL,    theTranslator->trModules(),
                                                                                         SrcLangExt_Fortran,theTranslator->trModules(),
                                                                                         SrcLangExt_Slice,  theTranslator->trModules()); })
                                                  } },
  { "file/memberdecl/constantgroups",             { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileConstantGroups,
                                                            []() { return compileOptions(theTranslator->trConstantGroups()); })
                                                  } },
  { "file/memberdecl/defines",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefines()); })
                                                  } },
  { "file/memberdecl/typedefs",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "file/memberdecl/sequences",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "file/memberdecl/dictionaries",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "file/memberdecl/enums",                      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "file/memberdecl/functions",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "file/memberdecl/variables",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                                            theTranslator->trConstants() :
                                                                                                            theTranslator->trVariables()); })
                                                  } },
  { "file/memberdef",                             { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },

  { "file/memberdef/inlineclasses",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileInlineClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt_Fortran, theTranslator->trTypeDocumentation()); })
                                                  } },
  { "file/memberdef/defines",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); })
                                                  } },
  { "file/memberdef/typedefs",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "file/memberdef/sequences",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "file/memberdef/dictionaries",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "file/memberdef/enums",                       { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "file/memberdef/functions",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */       theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt_Fortran, theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "file/memberdef/variables",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); })
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
  { "group/memberdecl/membergroups",              { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups)  } },
  { "group/memberdecl",                           { startCb(&LayoutParser::startMemberDecl), endCb(&LayoutParser::endMemberDecl) } },
  { "group/memberdecl/classes",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trCompounds(),
                                                                                         SrcLangExt_VHDL,    theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran, theTranslator->trDataTypes()); })
                                                  } },
  { "group/memberdecl/concepts",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); })
                                                  } },
  { "group/memberdecl/namespaces",                { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNamespaces,
                                                            []() { return compileOptions(/* default */       theTranslator->trNamespaces(),
                                                                                         SrcLangExt_Java,    theTranslator->trPackages(),
                                                                                         SrcLangExt_Fortran, theTranslator->trModules()); })
                                                  } },
  { "group/memberdecl/dirs",                      { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupDirs,
                                                            []() { return compileOptions(theTranslator->trDirectories()); })
                                                  } },
  { "group/memberdecl/nestedgroups",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNestedGroups,
                                                            []() { return compileOptions(theTranslator->trModules()); })
                                                  } },
  { "group/memberdecl/files",                     { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupFiles,
                                                            []() { return compileOptions(theTranslator->trFile(TRUE,FALSE)); })
                                                  } },
  { "group/memberdecl/defines",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefines()); })
                                                  } },
  { "group/memberdecl/typedefs",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); })
                                                  } },
  { "group/memberdecl/sequences",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); })
                                                  } },
  { "group/memberdecl/dictionaries",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); })
                                                  } },
  { "group/memberdecl/enums",                     { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); })
                                                  } },
  { "group/memberdecl/enumvalues",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumValMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationValues()); })
                                                  } },
  { "group/memberdecl/functions",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); })
                                                  } },
  { "group/memberdecl/variables",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariables()); })
                                                  } },
  { "group/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSignalMembers,
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "group/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPubSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "group/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decProSlotMembers,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "group/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPriSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "group/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEventMembers,
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "group/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPropMembers,
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "group/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFriendMembers,
                                                            []() { return compileOptions(theTranslator->trFriends()); })
                                                  } },
  { "group/memberdef",                            { startCb(&LayoutParser::startMemberDef), endCb(&LayoutParser::endMemberDef) } },
  { "group/memberdef/pagedocs",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::GroupPageDocs)    } },
  { "group/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); })
                                                  } },
  { "group/memberdef/defines",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); })
                                                  } },
  { "group/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); })
                                                  } },
  { "group/memberdef/sequences",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); })
                                                  } },
  { "group/memberdef/dictionaries",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); })
                                                  } },
  { "group/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); })
                                                  } },
  { "group/memberdef/enumvalues",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumValMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationValueDocumentation()); })
                                                  } },
  { "group/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()); })
                                                  } },
  { "group/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); })
                                                  } },
  { "group/memberdef/signals",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSignalMembers,
                                                            []() { return compileOptions(theTranslator->trSignals()); })
                                                  } },
  { "group/memberdef/publicslots",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPubSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); })
                                                  } },
  { "group/memberdef/protectedslots",             { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docProSlotMembers,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); })
                                                  } },
  { "group/memberdef/privateslots",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPriSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); })
                                                  } },
  { "group/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEventMembers,
                                                            []() { return compileOptions(theTranslator->trEvents()); })
                                                  } },
  { "group/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPropMembers,
                                                            []() { return compileOptions(theTranslator->trProperties()); })
                                                  } },
  { "group/memberdef/friends",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFriendMembers,
                                                            []() { return compileOptions(theTranslator->trFriends()); })
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

} // namespace

void LayoutParser::startElement( const std::string &name, const XMLHandlers::Attributes& attrib )
{
  //printf("startElement [%s]::[%s]\n",qPrint(m_scope),qPrint(name));
  auto it = g_elementHandlers.find(m_scope.str()+name);
  if (it!=g_elementHandlers.end())
  {
    it->second.startCb(*this,attrib);
  }
  else
  {
    std::string fileName = m_locator->fileName();
    ::warn(fileName.c_str(),m_locator->lineNr(),"Unexpected start tag '%s' found in scope='%s'!\n",
        qPrint(name),qPrint(m_scope));
  }
}

void LayoutParser::endElement( const std::string &name )
{
  //printf("endElement [%s]::[%s]\n",qPrint(m_scope),qPrint(name));
  auto it=g_elementHandlers.end();

  if (!m_scope.isEmpty() && m_scope.right(static_cast<uint32_t>(name.length())+1)==name+"/")
  { // element ends current scope
    it = g_elementHandlers.find(m_scope.left(m_scope.length()-1).str());
  }
  else // continue with current scope
  {
    it = g_elementHandlers.find(m_scope.str()+name);
  }
  if (it!=g_elementHandlers.end())
  {
    it->second.endCb(*this);
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
};

LayoutDocManager::LayoutDocManager() : d(std::make_unique<Private>())
{
}

LayoutDocManager::~LayoutDocManager()
{
}

void LayoutDocManager::init()
{
  LayoutParser &layoutParser = LayoutParser::instance();
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

LayoutNavEntry *LayoutDocManager::createChildNavEntry(LayoutNavEntry *parent,LayoutNavEntry::Kind k,bool vs,const QCString &bf,
                                                      const QCString &tl,const QCString &intro)
{
  if (parent==0) parent = &d->rootNav;
  auto ptr = std::make_unique<LayoutNavEntry>(parent,k,vs,bf,tl,intro);
  auto child = ptr.get();
  parent->addChild(std::move(ptr));
  return child;
}

void LayoutDocManager::addEntry(LayoutDocManager::LayoutPart p,LayoutDocEntryPtr &&e)
{
  d->docEntries[static_cast<int>(p)].push_back(std::move(e));
}

void LayoutDocManager::clear(LayoutDocManager::LayoutPart p)
{
  d->docEntries[static_cast<int>(p)].clear();
}

void LayoutDocManager::parse(const QCString &fileName, const char *data)
{
  LayoutParser &layoutParser = LayoutParser::instance();
  XMLHandlers handlers;
  handlers.startElement = [&layoutParser](const std::string &name,const XMLHandlers::Attributes &attrs) { layoutParser.startElement(name,attrs); };
  handlers.endElement   = [&layoutParser](const std::string &name) { layoutParser.endElement(name); };
  handlers.error        = [&layoutParser](const std::string &fn,int lineNr,const std::string &msg) { layoutParser.error(fn,lineNr,msg); };
  XMLParser parser(handlers);
  layoutParser.setDocumentLocator(&parser);
  parser.parse(fileName.data(),data ? data : fileToString(fileName).data(),Debug::isFlagSet(Debug::Lex_xml),
               [&]() { DebugLex::print(Debug::Lex_xml,"Entering","libxml/xml.l",qPrint(fileName)); },
               [&]() { DebugLex::print(Debug::Lex_xml,"Finished", "libxml/xml.l",qPrint(fileName)); }
              );
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
    err("Failed to open file %s for writing!\n",qPrint(fileName));
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
  int i,s=0,e=input.find('|');
  if (e==-1) return input; // simple title case
  int e1=e;
  while (e!=-1) // look for 'number=title' pattern separated by '|'
  {
    s=e+1;
    e=input.find('|',s);
    i=input.find('=',s);
    assert(i>s);
    SrcLangExt key= static_cast<SrcLangExt>(input.mid(s,i-s).toUInt());
    if (key==lang) // found matching key
    {
      if (e==-1) e=input.length();
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




