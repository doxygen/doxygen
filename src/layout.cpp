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
      return ConfigValues::instance().*(opt->value.s) != "NO";
    }
    else if (!opt)
    {
      err("found unsupported value %s for visible attribute in layout file\n",
          qPrint(visible));
    }
  }
  return visible!="no" && visible!="0";
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
      (kind()==LayoutNavEntry::UserGroup && url.left(9)=="usergroup"))
  {
    url = addHtmlExtensionIfMissing(url);
  }
  else if (url.left(5)=="@ref " || url.left(5)=="\\ref ")
  {
    const Definition *d = 0;
    QCString anchor;
    bool found=FALSE;
    if (resolveLink(QCString(),url.mid(5).stripWhiteSpace(),TRUE,&d,anchor))
    {
      if (d && d->isLinkable())
      {
        url=addHtmlExtensionIfMissing(d->getOutputFileBase());
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
      static LayoutParser *theInstance = new LayoutParser;
      return *theInstance;
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
      bool isVisible = elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySimple(k));
      }
    }

    // ============ Specific callbacks

    void startSectionEntry(LayoutDocEntry::Kind k,const XMLHandlers::Attributes &attrib,
                           const QCString &title)
    {
      bool isVisible = elemIsVisible(attrib) && parentIsVisible(m_rootNav);
      QCString userTitle = XMLHandlers::value(attrib,"title");
      //printf("startSectionEntry: title='%s' userTitle='%s'\n",
      //    qPrint(title),qPrint(userTitle));
      if (userTitle.isEmpty())  userTitle = title;
      if (m_part!=LayoutDocManager::Undefined && isVisible)
      {
        LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySection(k,userTitle));
      }
    }


    void startMemberDeclEntry(const XMLHandlers::Attributes &attrib,MemberListType type,
                              const QCString &title,const QCString &subscript)
    {
      //QCString visible = convertToQCString(XMLHandlers::value(attrib,"visible"));
      //bool isVisible = visible.isEmpty() || (visible!="no" && visible!="0");
      QCString userTitle     = XMLHandlers::value(attrib,"title");
      QCString userSubscript = XMLHandlers::value(attrib,"subtitle");
      if (userTitle.isEmpty())     userTitle     = title;
      if (userSubscript.isEmpty()) userSubscript = subscript;
      //printf("memberdecl: %s\n",qPrint(userTitle));
      if (m_part!=LayoutDocManager::Undefined)
      {
        LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntryMemberDecl(type,userTitle,userSubscript));
      }
    }

    void startMemberDefEntry(const XMLHandlers::Attributes &attrib,MemberListType type,
                             const QCString &title,const QCString &)
    {
      QCString userTitle = XMLHandlers::value(attrib,"title");
      if (userTitle.isEmpty()) userTitle = title;
      //printf("memberdef: %s\n",qPrint(userTitle));
      if (m_part!=LayoutDocManager::Undefined)
      {
        LayoutDocManager::instance().addEntry(m_part,
                                              new LayoutDocEntryMemberDef(type,userTitle));
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
        new LayoutNavEntry(m_rootNav,LayoutNavEntry::MainPage, TRUE,
            /*Config_getBool(GENERATE_TREEVIEW) ? "main" :*/ "index",
            theTranslator->trMainPage(),"",TRUE);
      }
    }

    void startNavEntry(const XMLHandlers::Attributes &attrib)
    {
      static bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
      static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
      static bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      static bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
      static bool hasGraphicalHierarchy = Config_getBool(HAVE_DOT) &&
                                          Config_getBool(GRAPHICAL_HIERARCHY);
      static bool extractAll = Config_getBool(EXTRACT_ALL);
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
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt || sliceOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageListDescription() : fortranOpt || sliceOpt ? theTranslator->trModulesListDescription(extractAll) : theTranslator->trNamespaceListDescription(extractAll),
          "namespaces"
        },
        { "namespacelist",
          LayoutNavEntry::NamespaceList,
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt || sliceOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
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
          fortranOpt ? theTranslator->trDataTypes() : vhdlOpt ? theTranslator->trDesignUnits() : theTranslator->trCompoundIndex(),
          QCString(),
          QCString(),
          "classes"
        },
        { "classes",
          LayoutNavEntry::Classes,
          fortranOpt ? theTranslator->trCompoundListFortran() : vhdlOpt ? theTranslator->trDesignUnitList() : theTranslator->trClasses(),
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
      LayoutNavEntry::Kind kind;
      // find type in the table
      int i=0;
      QCString type = XMLHandlers::value(attrib,"type");
      while (mapping[i].typeStr)
      {
        if (mapping[i].typeStr==type)
        {
          kind = mapping[i].kind;
          break;
        }
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
      QCString baseFile = mapping[i].baseFile;
      QCString title = XMLHandlers::value(attrib,"title");
      bool isVisible = elemIsVisible(attrib) && parentIsVisible(m_rootNav);
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
      m_rootNav = new LayoutNavEntry(m_rootNav,kind,isVisible,baseFile,title,intro);
    }

    void endNavEntry()
    {
      // set the root back to the parent
      if (m_rootNav && !m_invalidEntry) m_rootNav = m_rootNav->parent();
      m_invalidEntry=FALSE;
    }

    void startClass(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Class);
      m_scope="class/";
      m_part = LayoutDocManager::Class;
    }

    void endClass()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startNamespace(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Namespace);
      m_scope="namespace/";
      m_part = LayoutDocManager::Namespace;
    }

    void endNamespace()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startConcept(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Concept);
      m_scope="concept/";
      m_part = LayoutDocManager::Concept;
    }

    void endConcept()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startFile(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::File);
      m_scope="file/";
      m_part = LayoutDocManager::File;
    }

    void endFile()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startGroup(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Group);
      m_scope="group/";
      m_part = LayoutDocManager::Group;
    }

    void endGroup()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startDirectory(const XMLHandlers::Attributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Directory);
      m_scope="directory/";
      m_part = LayoutDocManager::Directory;
    }

    void endDirectory()
    {
      m_scope="";
      m_part = LayoutDocManager::Undefined;
    }

    void startMemberDef(const XMLHandlers::Attributes &)
    {
      m_scope+="memberdef/";
      if (m_part!=LayoutDocManager::Undefined)
      {
        LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySimple(LayoutDocEntry::MemberDefStart));
      }
    }

    void endMemberDef()
    {
      int i=m_scope.findRev("memberdef/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        if (m_part!=LayoutDocManager::Undefined)
        {
          LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySimple(LayoutDocEntry::MemberDefEnd));
        }
      }
    }

    void startMemberDecl(const XMLHandlers::Attributes &)
    {
      m_scope+="memberdecl/";
      if (m_part!=LayoutDocManager::Undefined)
      {
        LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySimple(LayoutDocEntry::MemberDeclStart));
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
          LayoutDocManager::instance().addEntry(m_part,new LayoutDocEntrySimple(LayoutDocEntry::MemberDeclEnd));
        }
      }
    }


  private:
    LayoutParser() { }
   ~LayoutParser() { delete m_rootNav; }

    QCString m_scope;
    LayoutDocManager::LayoutPart m_part = LayoutDocManager::Undefined;
    LayoutNavEntry *m_rootNav = 0;
    bool m_invalidEntry = false;
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
  EndCallback   endCb;
};

static ElementCallbacks::StartCallback startCb(void (LayoutParser::*fn)(const XMLHandlers::Attributes &))
{
  return [fn](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr); };
}

static ElementCallbacks::StartCallback startCb(void (LayoutParser::*fn)(LayoutDocEntry::Kind,const XMLHandlers::Attributes &),LayoutDocEntry::Kind kind)
{
  return [fn,kind](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,attr); };
}

static ElementCallbacks::StartCallback startCb(void (LayoutParser::*fn)(LayoutDocEntry::Kind,const XMLHandlers::Attributes &,const QCString &),
                                        LayoutDocEntry::Kind kind,
                                        std::function<QCString()> title
                                       )
{
  return [fn,kind,title](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(kind,attr,title()); };
}

static ElementCallbacks::StartCallback startCb(void (LayoutParser::*fn)(const XMLHandlers::Attributes &,MemberListType,const QCString &,const QCString &),
                                        MemberListType type,
                                        std::function<QCString()> title
                                       )
{
  return [fn,type,title](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr,type,title(),QCString()); };
}

static ElementCallbacks::StartCallback startCb(void (LayoutParser::*fn)(const XMLHandlers::Attributes &,MemberListType,const QCString &,const QCString &),
                                        MemberListType type,
                                        std::function<QCString()> title,
                                        std::function<QCString()> subtitle
                                       )
{
  return [fn,type,title,subtitle](LayoutParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr,type,title(),subtitle()); };
}


static ElementCallbacks::EndCallback endCb(void (LayoutParser::*fn)())
{
  return [fn](LayoutParser &parser) { (parser.*fn)(); };
}

static ElementCallbacks::EndCallback endCb()
{
  return [](LayoutParser &){};
}

static const std::map< std::string, ElementCallbacks > g_elementHandlers =
{
  // path/name
  { "doxygenlayout",                              { startCb(&LayoutParser::startLayout),
                                                    endCb()
                                                  } },
  { "navindex",                                   { startCb(&LayoutParser::startNavIndex),
                                                    endCb(&LayoutParser::endNavIndex)
                                                  } },
  { "navindex/tab",                               { startCb(&LayoutParser::startNavEntry),
                                                    endCb(&LayoutParser::endNavEntry)
                                                  } },

  // class layout handlers
  { "class",                                      { startCb(&LayoutParser::startClass),
                                                    endCb(&LayoutParser::endClass)
                                                  } },
  { "class/briefdescription",                     { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "class/detaileddescription",                  { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            [](){ return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "class/authorsection",                        { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::AuthorSection),
                                                    endCb()
                                                  } },
  { "class/includes",                             { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassIncludes),
                                                    endCb()
                                                  } },
  { "class/inheritancegraph",                     { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassInheritanceGraph),
                                                    endCb()
                                                  } },
  { "class/collaborationgraph",                   { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassCollaborationGraph),
                                                    endCb()
                                                  } },
  { "class/allmemberslink",                       { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassAllMembersLink),
                                                    endCb()
                                                  } },
  { "class/usedfiles",                            { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::ClassUsedFiles),
                                                    endCb()
                                                  } },
  { "class/memberdecl",                           { startCb(&LayoutParser::startMemberDecl),
                                                    endCb(&LayoutParser::endMemberDecl)
                                                  } },
  { "class/memberdecl/membergroups",              { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::MemberGroups),
                                                    endCb()
                                                  } },
  { "class/memberdecl/nestedclasses",             { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::ClassNestedClasses,
                                                            []() { return compileOptions(/*default*/        theTranslator->trCompounds(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran,theTranslator->trDataTypes()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/services",                  { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_services,
                                                            []() { return compileOptions(theTranslator->trServices()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/interfaces",                { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_interfaces,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/publictypes",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_pubTypes,
                                                            []() { return compileOptions(theTranslator->trPublicTypes()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_pubSlots,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry,MemberListType_signals,
                                                            []() { return compileOptions(theTranslator->trSignals()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/publicmethods",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubMethods,
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicMembers(),
                                                                                         SrcLangExt_ObjC, theTranslator->trInstanceMethods(),
                                                                                         SrcLangExt_Slice,theTranslator->trOperations()); }),
                                                     endCb()
                                                  } },
  { "class/memberdecl/publicstaticmethods",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubStaticMethods,
                                                            []() { return compileOptions(/* default */    theTranslator->trStaticPublicMembers(),
                                                                                         SrcLangExt_ObjC, theTranslator->trClassMethods()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/publicattributes",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubAttribs,
                                                            []() { return compileOptions(/* default */    theTranslator->trPublicAttribs(),
                                                                                         SrcLangExt_Slice,theTranslator->trDataMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/publicstaticattributes",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pubStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPublicAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedtypes",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proTypes,
                                                            []() { return compileOptions(theTranslator->trProtectedTypes()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proSlots,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedmethods",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proMethods,
                                                            []() { return compileOptions(theTranslator->trProtectedMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedstaticmethods",    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticProtectedMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedattributes",       { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proAttribs,
                                                            []() { return compileOptions(theTranslator->trProtectedAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/protectedstaticattributes", { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_proStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticProtectedAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/packagetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacTypes,
                                                            []() { return compileOptions(theTranslator->trPackageTypes()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/packagemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacMethods,
                                                            []() { return compileOptions(theTranslator->trPackageMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/packagestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticPackageMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/packageattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacAttribs,
                                                            []() { return compileOptions(theTranslator->trPackageAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/packagestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_pacStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPackageAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_properties,
                                                            []() { return compileOptions(theTranslator->trProperties()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_events,
                                                            []() { return compileOptions(theTranslator->trEvents()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privatetypes",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priTypes,
                                                            []() { return compileOptions(theTranslator->trPrivateTypes()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priSlots,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privatemethods",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priMethods,
                                                            []() { return compileOptions(theTranslator->trPrivateMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privatestaticmethods",      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priStaticMethods,
                                                            []() { return compileOptions(theTranslator->trStaticPrivateMembers()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privateattributes",         { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priAttribs,
                                                            []() { return compileOptions(theTranslator->trPrivateAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/privatestaticattributes",   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_priStaticAttribs,
                                                            []() { return compileOptions(theTranslator->trStaticPrivateAttribs()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_friends,
                                                            []() { return compileOptions(theTranslator->trFriends()); }),
                                                    endCb()
                                                  } },
  { "class/memberdecl/related",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_related,
                                                            []() { return compileOptions(theTranslator->trRelatedFunctions()); },
                                                            []() { return compileOptions(theTranslator->trRelatedSubscript()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef",                            { startCb(&LayoutParser::startMemberDef),
                                                    endCb(&LayoutParser::endMemberDef)
                                                  } },
  { "class/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ClassInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_typedefMembers,
                                                            []() { return compileOptions(theTranslator->trMemberTypedefDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType_enumMembers,
                                                            []() { return compileOptions(theTranslator->trMemberEnumerationDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/services",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_serviceMembers,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/interfaces",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_interfaceMembers,
                                                            []() { return compileOptions(theTranslator->trInterfaces()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/constructors",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_constructors,
                                                            []() { return compileOptions(theTranslator->trConstructorDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_functionMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberFunctionDocumentation(), SrcLangExt_ObjC,   theTranslator->trMethodDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trMemberFunctionDocumentationFortran(),
                                                                           SrcLangExt_Slice,  theTranslator->trOperationDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/related",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_relatedMembers,
                                                            []() { return compileOptions(theTranslator->trRelatedFunctionDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_variableMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trMemberDataDocumentation(),
                                                                           SrcLangExt_Slice,  theTranslator->trDataMemberDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_propertyMembers,
                                                            []() { return compileOptions(theTranslator->trPropertyDocumentation()); }),
                                                    endCb()
                                                  } },
  { "class/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_eventMembers,
                                                            []() { return compileOptions(theTranslator->trEventDocumentation()); }),
                                                    endCb()
                                                  } },

  // concept layout handlers
  { "concept",                                    { startCb(&LayoutParser::startConcept),
                                                    endCb(&LayoutParser::endConcept)
                                                  } },

  { "concept/briefdescription",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "concept/definition",                         { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::ConceptDefinition,
                                                            []() { return compileOptions(theTranslator->trConceptDefinition()); }),
                                                    endCb()
                                                  } },
  { "concept/includes",                           { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::ClassIncludes),
                                                    endCb()
                                                  } },
  { "concept/sourcelink",                         { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileSourceLink),
                                                    endCb()
                                                  } },
  { "concept/detaileddescription",                { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "concept/authorsection",                      { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection),
                                                    endCb()
                                                  } },
  // namespace layout handlers
  { "namespace",                                  { startCb(&LayoutParser::startNamespace),
                                                    endCb(&LayoutParser::endNamespace)
                                                  } },

  { "namespace/briefdescription",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "namespace/detaileddescription",              { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "namespace/authorsection",                    { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection),
                                                    endCb()
                                                  } },
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
                                                                                              theTranslator->trNamespaces())); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/constantgroups",        { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceNestedConstantGroups,
                                                            []() { return compileOptions(theTranslator->trConstantGroups()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/interfaces",            { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceInterfaces,
                                                            []() { return compileOptions(theTranslator->trSliceInterfaces()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/classes",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trCompounds(),
                                                                           SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                           SrcLangExt_Fortran,theTranslator->trDataTypes()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/concepts",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/structs",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceStructs,
                                                            []() { return compileOptions(theTranslator->trStructs()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/exceptions",            { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::NamespaceExceptions,
                                                            []() { return compileOptions(theTranslator->trExceptions()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/membergroups",          { startCb(&LayoutParser::startSimpleEntry,LayoutDocEntry::MemberGroups),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/typedefs",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/sequences",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/dictionaries",          { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/enums",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/functions",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                           SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                           SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdecl/variables",             { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstants() :
                                                                                              theTranslator->trVariables()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef",                        { startCb(&LayoutParser::startMemberDef),
                                                    endCb(&LayoutParser::endMemberDef)
                                                  } },
  { "namespace/memberdef/inlineclasses",          { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::NamespaceInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/typedefs",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/sequences",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/dictionaries",           { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/enums",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/functions",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                           SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()); }),
                                                    endCb()
                                                  } },
  { "namespace/memberdef/variables",              { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                              theTranslator->trConstantDocumentation() :
                                                                                              theTranslator->trVariableDocumentation()); }),
                                                    endCb()
                                                  } },

  // file layout handlers
  { "file",                                       { startCb(&LayoutParser::startFile),
                                                    endCb(&LayoutParser::endFile)
                                                  } },

  { "file/briefdescription",                      { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "file/detaileddescription",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "file/authorsection",                         { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection),
                                                    endCb()
                                                  } },
  { "file/includes",                              { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludes),
                                                    endCb()
                                                  } },
  { "file/includegraph",                          { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludeGraph),
                                                    endCb()
                                                  } },
  { "file/includedbygraph",                       { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileIncludedByGraph),
                                                    endCb()
                                                  } },
  { "file/sourcelink",                            { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::FileSourceLink),
                                                    endCb()
                                                  } },
  { "file/memberdecl/membergroups",               { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups),
                                                    endCb()
                                                  } },
  { "file/memberdecl",                            { startCb(&LayoutParser::startMemberDecl),
                                                    endCb(&LayoutParser::endMemberDecl)
                                                  } },

  { "file/memberdecl/interfaces",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileInterfaces,
                                                            []() { return compileOptions(theTranslator->trSliceInterfaces()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/classes",                    { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trCompounds(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran,theTranslator->trDataTypes()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/concepts",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::FileConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/structs",                    { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileStructs,
                                                            []() { return compileOptions(theTranslator->trStructs()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/exceptions",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileExceptions,
                                                            []() { return compileOptions(theTranslator->trExceptions()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/namespaces",                 { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileNamespaces,
                                                            []() { return compileOptions(/* default */      theTranslator->trNamespaces(),
                                                                                         SrcLangExt_Java,   theTranslator->trPackages(),
                                                                                         SrcLangExt_IDL,    theTranslator->trModules(),
                                                                                         SrcLangExt_Fortran,theTranslator->trModules(),
                                                                                         SrcLangExt_Slice,  theTranslator->trModules()); }),
                                                     endCb()
                                                  } },
  { "file/memberdecl/constantgroups",             { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileConstantGroups,
                                                            []() { return compileOptions(theTranslator->trConstantGroups()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/defines",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefines()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/typedefs",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/sequences",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/dictionaries",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/enums",                      { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/functions",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); }),
                                                    endCb()
                                                  } },
  { "file/memberdecl/variables",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                                                            theTranslator->trConstants() :
                                                                                                            theTranslator->trVariables()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef",                             { startCb(&LayoutParser::startMemberDef),
                                                    endCb(&LayoutParser::endMemberDef)
                                                  } },

  { "file/memberdef/inlineclasses",               { startCb(&LayoutParser::startSectionEntry,LayoutDocEntry::FileInlineClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt_Fortran, theTranslator->trTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/defines",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/typedefs",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/sequences",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/dictionaries",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/enums",                       { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/functions",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */       theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt_Fortran, theTranslator->trSubprogramDocumentation()); }),
                                                    endCb()
                                                  } },
  { "file/memberdef/variables",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); }),
                                                    endCb()
                                                  } },

  // group layout handlers
  { "group",                                      { startCb(&LayoutParser::startGroup),
                                                    endCb(&LayoutParser::endGroup)
                                                  } },
  { "group/briefdescription",                     { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "group/detaileddescription",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "group/authorsection",                        { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::AuthorSection),
                                                    endCb()
                                                  } },
  { "group/groupgraph",                           { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::GroupGraph),
                                                    endCb()
                                                  } },
  { "group/memberdecl/membergroups",              { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::MemberGroups),
                                                    endCb()
                                                  } },

  { "group/memberdecl",                           { startCb(&LayoutParser::startMemberDecl),
                                                    endCb(&LayoutParser::endMemberDecl)
                                                  } },

  { "group/memberdecl/classes",                   { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupClasses,
                                                            []() { return compileOptions(/* default */       theTranslator->trCompounds(),
                                                                                         SrcLangExt_VHDL,    theTranslator->trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                                                                         SrcLangExt_Fortran, theTranslator->trDataTypes()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/concepts",                  { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupConcepts,
                                                            []() { return compileOptions(theTranslator->trConcept(true,false)); }),
                                                   endCb()
                                                  } },
  { "group/memberdecl/namespaces",                { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNamespaces,
                                                            []() { return compileOptions(/* default */       theTranslator->trNamespaces(),
                                                                                         SrcLangExt_Java,    theTranslator->trPackages(),
                                                                                         SrcLangExt_Fortran, theTranslator->trModules()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/dirs",                      { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupDirs,
                                                            []() { return compileOptions(theTranslator->trDirectories()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/nestedgroups",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupNestedGroups,
                                                            []() { return compileOptions(theTranslator->trModules()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/files",                     { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupFiles,
                                                            []() { return compileOptions(theTranslator->trFile(TRUE,FALSE)); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/defines",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefines()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/typedefs",                  { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefs()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/sequences",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequences()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/dictionaries",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaries()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/enums",                     { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerations()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/enumvalues",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEnumValMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationValues()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/functions",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctions(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                                                                         SrcLangExt_VHDL,   theTranslator->trFunctionAndProc()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/variables",                 { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariables()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/signals",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decSignalMembers,
                                                            []() { return compileOptions(theTranslator->trSignals()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/publicslots",               { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPubSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/protectedslots",            { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decProSlotMembers,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/privateslots",              { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPriSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/events",                    { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decEventMembers,
                                                            []() { return compileOptions(theTranslator->trEvents()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/properties",                { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decPropMembers,
                                                            []() { return compileOptions(theTranslator->trProperties()); }),
                                                    endCb()
                                                  } },
  { "group/memberdecl/friends",                   { startCb(&LayoutParser::startMemberDeclEntry, MemberListType_decFriendMembers,
                                                            []() { return compileOptions(theTranslator->trFriends()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef",                            { startCb(&LayoutParser::startMemberDef),
                                                    endCb(&LayoutParser::endMemberDef)
                                                  } },

  { "group/memberdef/pagedocs",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::GroupPageDocs),
                                                    endCb()
                                                  } },
  { "group/memberdef/inlineclasses",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::GroupInlineClasses,
                                                            []() { return compileOptions(/* default */      theTranslator->trClassDocumentation(),
                                                                                         SrcLangExt_Fortran,theTranslator->trTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/defines",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDefineMembers,
                                                            []() { return compileOptions(theTranslator->trDefineDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/typedefs",                   { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docTypedefMembers,
                                                            []() { return compileOptions(theTranslator->trTypedefDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/sequences",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSequenceMembers,
                                                            []() { return compileOptions(theTranslator->trSequenceDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/dictionaries",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docDictionaryMembers,
                                                            []() { return compileOptions(theTranslator->trDictionaryDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/enums",                      { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationTypeDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/enumvalues",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEnumValMembers,
                                                            []() { return compileOptions(theTranslator->trEnumerationValueDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/functions",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFuncMembers,
                                                            []() { return compileOptions(/* default */      theTranslator->trFunctionDocumentation(),
                                                                                         SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/variables",                  { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docVarMembers,
                                                            []() { return compileOptions(theTranslator->trVariableDocumentation()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/signals",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docSignalMembers,
                                                            []() { return compileOptions(theTranslator->trSignals()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/publicslots",                { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPubSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPublicSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/protectedslots",             { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docProSlotMembers,
                                                            []() { return compileOptions(theTranslator->trProtectedSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/privateslots",               { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPriSlotMembers,
                                                            []() { return compileOptions(theTranslator->trPrivateSlots()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/events",                     { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docEventMembers,
                                                            []() { return compileOptions(theTranslator->trEvents()); }),
                                                    endCb()
                                                  } },
  { "group/memberdef/properties",                 { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docPropMembers,
                                                            []() { return compileOptions(theTranslator->trProperties()); }),
                                                    endCb()
                                                  } }, { "group/memberdef/friends",                    { startCb(&LayoutParser::startMemberDefEntry, MemberListType_docFriendMembers,
                                                            []() { return compileOptions(theTranslator->trFriends()); }),
                                                    endCb()
                                                  } },

  // directory layout handlers
  { "directory",                                  { startCb(&LayoutParser::startDirectory),
                                                    endCb(&LayoutParser::endDirectory)
                                                  } },

  { "directory/briefdescription",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::BriefDesc),
                                                    endCb()
                                                  } },
  { "directory/detaileddescription",              { startCb(&LayoutParser::startSectionEntry, LayoutDocEntry::DetailedDesc,
                                                            []() { return compileOptions(theTranslator->trDetailedDescription()); }),
                                                    endCb()
                                                  } },
  { "directory/directorygraph",                   { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirGraph),
                                                    endCb()
                                                  } },
  { "directory/memberdecl",                       { startCb(&LayoutParser::startMemberDecl),
                                                    endCb(&LayoutParser::endMemberDecl)
                                                  } },

  { "directory/memberdecl/dirs",                  { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirSubDirs),
                                                    endCb()
                                                  } },
  { "directory/memberdecl/files",                 { startCb(&LayoutParser::startSimpleEntry, LayoutDocEntry::DirFiles),
                                                    endCb()
                                                  } },

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

  if (!m_scope.isEmpty() && m_scope.right(static_cast<uint>(name.length())+1)==name+"/")
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
    LayoutNavEntry *rootNav;
};

LayoutDocManager::LayoutDocManager() : d(std::make_unique<Private>())
{
  d->rootNav = new LayoutNavEntry;
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
  QCString layout_default = ResourceMgr::instance().getAsString("layout_default.xml");
  parser.parse("layout_default.xml",layout_default.data(),Debug::isFlagSet(Debug::Lex));
}

LayoutDocManager::~LayoutDocManager()
{
  delete d->rootNav;
}

LayoutDocManager & LayoutDocManager::instance()
{
  static LayoutDocManager *theInstance = new LayoutDocManager;
  return *theInstance;
}

const LayoutDocEntryList &LayoutDocManager::docEntries(LayoutDocManager::LayoutPart part) const
{
  return d->docEntries[static_cast<int>(part)];
}

LayoutNavEntry* LayoutDocManager::rootNavEntry() const
{
  return d->rootNav;
}

void LayoutDocManager::addEntry(LayoutDocManager::LayoutPart p,LayoutDocEntry *e)
{
  d->docEntries[static_cast<int>(p)].push_back(std::unique_ptr<LayoutDocEntry>(e));
}

void LayoutDocManager::clear(LayoutDocManager::LayoutPart p)
{
  d->docEntries[static_cast<int>(p)].clear();
}

void LayoutDocManager::parse(const QCString &fileName)
{
  LayoutParser &layoutParser = LayoutParser::instance();
  XMLHandlers handlers;
  handlers.startElement = [&layoutParser](const std::string &name,const XMLHandlers::Attributes &attrs) { layoutParser.startElement(name,attrs); };
  handlers.endElement   = [&layoutParser](const std::string &name) { layoutParser.endElement(name); };
  handlers.error        = [&layoutParser](const std::string &fn,int lineNr,const std::string &msg) { layoutParser.error(fn,lineNr,msg); };
  XMLParser parser(handlers);
  layoutParser.setDocumentLocator(&parser);
  parser.parse(fileName.data(),fileToString(fileName).data(),Debug::isFlagSet(Debug::Lex));
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
    size_t key=input.mid(s,i-s).toUInt();
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




