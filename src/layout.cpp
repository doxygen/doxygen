/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include "layout.h"
#include "message.h"
#include "language.h"
#include "vhdldocgen.h"
#include "util.h"
#include "doxygen.h"
#include "version.h"
#include "config.h"

#include <assert.h>
#include <qxml.h>
#include <qfile.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qtextstream.h>

static const char layout_default[] =
#include "layout_default.h"
;

#define ADD_OPTION(langId,text) "|"+QCString().setNum(langId)+"="+text

#define COMPILE_FOR_1_OPTION(def,langId1,text1) \
  def+ADD_OPTION(langId1,text1)

#define COMPILE_FOR_2_OPTIONS(def,langId1,text1,langId2,text2) \
  COMPILE_FOR_1_OPTION(def,langId1,text1)+ADD_OPTION(langId2,text2)

#define COMPILE_FOR_3_OPTIONS(def,langId1,text1,langId2,text2,langId3,text3) \
  COMPILE_FOR_2_OPTIONS(def,langId1,text1,langId2,text2)+ADD_OPTION(langId3,text3)

#define COMPILE_FOR_4_OPTIONS(def,langId1,text1,langId2,text2,langId3,text3,langId4,text4) \
  COMPILE_FOR_3_OPTIONS(def,langId1,text1,langId2,text2,langId3,text3)+ADD_OPTION(langId4,text4)

static bool elemIsVisible(const QXmlAttributes &attrib,bool defVal=TRUE)
{
  QCString visible = attrib.value("visible").utf8();
  if (visible.isEmpty()) return defVal;
  if (visible.at(0)=='$' && visible.length()>1)
  {
    QCString id = visible.mid(1);
    ConfigOption *opt = Config::instance()->get(id);
    if (opt && opt->kind()==ConfigOption::O_Bool)
    {
      return *((ConfigBool *)opt)->valueRef();
    }
    else if (!opt)
    {
      err("error: found unsupported value %s for visible attribute in layout file\n",
          visible.data());
    }
  }
  return visible!="no" && visible!="0";
}

//---------------------------------------------------------------------------------

LayoutNavEntry *LayoutNavEntry::find(LayoutNavEntry::Kind kind,
    const char *file) const
{
  LayoutNavEntry *result=0;
  QListIterator<LayoutNavEntry> li(m_children);
  LayoutNavEntry *entry;
  for (li.toFirst();(entry=li.current());++li)
  {
    // depth first search, needed to find the entry furthest from the 
    // root in case an entry is in the tree twice
    result = entry->find(kind,file);
    if (result) return result;
    if (entry->kind()==kind && (file==0 || entry->baseFile()==file))
    {
      return entry;
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
    url+=Doxygen::htmlFileExtension;
  }
  else if (url.left(5)=="@ref " || url.left(5)=="\\ref ")
  {
    Definition *d;
    QCString anchor;
    bool found=FALSE;
    if (resolveLink(0,url.mid(5).stripWhiteSpace(),TRUE,&d,anchor))
    {
      if (d && d->isLinkable()) 
      {
        url=d->getOutputFileBase()+Doxygen::htmlFileExtension;
        if (!anchor.isEmpty())
        {
          url+="#"+anchor;
        }
        found=TRUE;
      }
    }
    if (!found)
    {
      msg("warning: explicit link request to '%s' in layout file '%s' could not be resolved\n",qPrint(url.mid(5)),qPrint(Config_getString("LAYOUT_FILE")));
    }
  }
  //printf("LayoutNavEntry::url()=%s\n",url.data());
  return url;
}

//---------------------------------------------------------------------------------

class LayoutParser : public QXmlDefaultHandler
{
  private:
    class StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(const QXmlAttributes &attrib); 
      public:
        StartElementHandler(LayoutParser *parent, Handler h) 
          : m_parent(parent), m_handler(h) {}
        virtual ~StartElementHandler() {}
        virtual void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(attrib); 
        }
      protected:
        StartElementHandler() : m_parent(0), m_handler(0) {}
      private:
        LayoutParser *m_parent;
        Handler m_handler;
    };

    class StartElementHandlerKind : public StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind,
                                              const QXmlAttributes &attrib); 
      public:
        StartElementHandlerKind(LayoutParser *parent, LayoutDocEntry::Kind k,Handler h) 
          : m_parent(parent), m_kind(k), m_handler(h) {}
        void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(m_kind,attrib); 
        }
      private:
        LayoutParser *m_parent;
        LayoutDocEntry::Kind m_kind;
        Handler m_handler;
    };

    class StartElementHandlerSection : public StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind,
                                              const QXmlAttributes &attrib,
                                              const QCString &title); 
      public:
        StartElementHandlerSection(LayoutParser *parent, LayoutDocEntry::Kind k,Handler h,
                                const QCString &title) 
          : m_parent(parent), m_kind(k), m_handler(h), m_title(title) {}
        void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(m_kind,attrib,m_title); 
        }
      private:
        LayoutParser *m_parent;
        LayoutDocEntry::Kind m_kind;
        Handler m_handler;
        QCString m_title;
    };

    class StartElementHandlerMember : public StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(const QXmlAttributes &attrib,
                                              MemberListType type,
                                              const QCString &title,
                                              const QCString &subtitle); 
      public:
        StartElementHandlerMember(LayoutParser *parent, 
                                  Handler h,
                                  MemberListType type,
                                  const QCString &tl,
                                  const QCString &ss = QCString()
                                 ) 
          : m_parent(parent), m_handler(h), m_type(type),
            m_title(tl), m_subscript(ss) {}
        void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(attrib,m_type,m_title,m_subscript); 
        }
      private:
        LayoutParser *m_parent;
        Handler m_handler;
        MemberListType m_type;
        QCString m_title;
        QCString m_subscript;
    };

    class StartElementHandlerNavEntry : public StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind,
                                              const QXmlAttributes &attrib,
                                              const QCString &title); 
      public:
        StartElementHandlerNavEntry(LayoutParser *parent,
                               LayoutNavEntry::Kind kind, 
                               Handler h,
                               const QCString &tl
                              ) 
          : m_parent(parent), m_kind(kind), m_handler(h), m_title(tl) {}
        void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(m_kind,attrib,m_title); 
        }
      private:
        LayoutParser *m_parent;
        LayoutNavEntry::Kind m_kind;
        Handler m_handler;
        QCString m_title;
    };

    class EndElementHandler
    {
        typedef void (LayoutParser::*Handler)(); 
      public:
        EndElementHandler(LayoutParser *parent, Handler h) : m_parent(parent), m_handler(h) {}
        void operator()() { (m_parent->*m_handler)(); }
      private:
        LayoutParser *m_parent;
        Handler m_handler;
    };


  public:
    static LayoutParser &instance()
    {
      static LayoutParser *theInstance = new LayoutParser;
      return *theInstance;
    }
    void init()
    {
      m_sHandler.setAutoDelete(TRUE);
      m_eHandler.setAutoDelete(TRUE);
      m_part = -1; // invalid
      m_rootNav = 0;

      //bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      //bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      //bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");

      // start & end handlers
      m_sHandler.insert("doxygenlayout", 
          new StartElementHandler(this,&LayoutParser::startLayout));
      m_eHandler.insert("doxygenlayout", 
          new EndElementHandler(this,&LayoutParser::endLayout));

      // class layout handlers
      m_sHandler.insert("navindex", 
          new StartElementHandler(this,&LayoutParser::startNavIndex));
      m_sHandler.insert("navindex/tab", 
          new StartElementHandler(this,&LayoutParser::startNavEntry));
      m_eHandler.insert("navindex/tab", 
          new EndElementHandler(this,&LayoutParser::endNavEntry));
      m_eHandler.insert("navindex", 
          new EndElementHandler(this,&LayoutParser::endNavIndex));

      // class layout handlers
      m_sHandler.insert("class", 
          new StartElementHandler(this,&LayoutParser::startClass));
      m_sHandler.insert("class/briefdescription", 
          new StartElementHandlerKind(this,LayoutDocEntry::BriefDesc,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/detaileddescription", 
          new StartElementHandlerSection(this,LayoutDocEntry::DetailedDesc,&LayoutParser::startSectionEntry,
                                         theTranslator->trDetailedDescription()));
      m_sHandler.insert("class/authorsection", 
          new StartElementHandlerKind(this,LayoutDocEntry::AuthorSection,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/includes", 
          new StartElementHandlerKind(this,LayoutDocEntry::ClassIncludes,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/inheritancegraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::ClassInheritanceGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/collaborationgraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::ClassCollaborationGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/allmemberslink", 
          new StartElementHandlerKind(this,LayoutDocEntry::ClassAllMembersLink,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/usedfiles", 
          new StartElementHandlerKind(this,LayoutDocEntry::ClassUsedFiles,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/memberdecl", 
          new StartElementHandler(this,&LayoutParser::startMemberDecl));
      m_sHandler.insert("class/memberdecl/membergroups", 
          new StartElementHandlerKind(this,LayoutDocEntry::MemberGroups,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("class/memberdecl/nestedclasses", 
          new StartElementHandlerSection(this,LayoutDocEntry::ClassNestedClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_2_OPTIONS(
                                           theTranslator->trCompounds(),
                                           SrcLangExt_VHDL,VhdlDocGen::trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                           SrcLangExt_Fortran,theTranslator->trDataTypes()
                                         )));
      m_sHandler.insert("class/memberdecl/services",
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_services,theTranslator->trServices()));
      m_sHandler.insert("class/memberdecl/interfaces",
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_interfaces,theTranslator->trInterfaces()));
      m_sHandler.insert("class/memberdecl/publictypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubTypes,theTranslator->trPublicTypes()));
      m_sHandler.insert("class/memberdecl/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubSlots,theTranslator->trPublicSlots())); 
      m_sHandler.insert("class/memberdecl/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_signals,theTranslator->trSignals())); 
      m_sHandler.insert("class/memberdecl/publicmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubMethods,
                                        COMPILE_FOR_1_OPTION(
                                          theTranslator->trPublicMembers(),
                                          SrcLangExt_ObjC,theTranslator->trInstanceMethods()
                                        ))); 
      m_sHandler.insert("class/memberdecl/publicstaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubStaticMethods,
                                        COMPILE_FOR_1_OPTION(
                                          theTranslator->trStaticPublicMembers(),
                                          SrcLangExt_ObjC,theTranslator->trClassMethods()
                                        ))); 
      m_sHandler.insert("class/memberdecl/publicattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubAttribs,theTranslator->trPublicAttribs())); 
      m_sHandler.insert("class/memberdecl/publicstaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pubStaticAttribs,theTranslator->trStaticPublicAttribs())); 
      m_sHandler.insert("class/memberdecl/protectedtypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proTypes,theTranslator->trProtectedTypes())); 
      m_sHandler.insert("class/memberdecl/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proSlots,theTranslator->trProtectedSlots())); 
      m_sHandler.insert("class/memberdecl/protectedmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proMethods,theTranslator->trProtectedMembers())); 
      m_sHandler.insert("class/memberdecl/protectedstaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proStaticMethods,theTranslator->trStaticProtectedMembers()));
      m_sHandler.insert("class/memberdecl/protectedattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proAttribs,theTranslator->trProtectedAttribs())); 
      m_sHandler.insert("class/memberdecl/protectedstaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_proStaticAttribs,theTranslator->trStaticProtectedAttribs())); 
      m_sHandler.insert("class/memberdecl/packagetypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pacTypes,theTranslator->trPackageTypes())); 
      m_sHandler.insert("class/memberdecl/packagemethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pacMethods,theTranslator->trPackageMembers())); 
      m_sHandler.insert("class/memberdecl/packagestaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pacStaticMethods,theTranslator->trStaticPackageMembers())); 
      m_sHandler.insert("class/memberdecl/packageattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pacAttribs,theTranslator->trPackageAttribs())); 
      m_sHandler.insert("class/memberdecl/packagestaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_pacStaticAttribs,theTranslator->trStaticPackageAttribs())); 
      m_sHandler.insert("class/memberdecl/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_properties,theTranslator->trProperties())); 
      m_sHandler.insert("class/memberdecl/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_events,theTranslator->trEvents())); 
      m_sHandler.insert("class/memberdecl/privatetypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priTypes,theTranslator->trPrivateTypes())); 
      m_sHandler.insert("class/memberdecl/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priSlots,theTranslator->trPrivateSlots())); 
      m_sHandler.insert("class/memberdecl/privatemethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priMethods,theTranslator->trPrivateMembers())); 
      m_sHandler.insert("class/memberdecl/privatestaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priStaticMethods,theTranslator->trStaticPrivateMembers())); 
      m_sHandler.insert("class/memberdecl/privateattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priAttribs,theTranslator->trPrivateAttribs())); 
      m_sHandler.insert("class/memberdecl/privatestaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_priStaticAttribs,theTranslator->trStaticPrivateAttribs())); 
      m_sHandler.insert("class/memberdecl/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_friends,theTranslator->trFriends()));
      m_sHandler.insert("class/memberdecl/related", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_related,theTranslator->trRelatedFunctions(),
                                        theTranslator->trRelatedSubscript())); 
      m_eHandler.insert("class/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("class/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("class/memberdef/inlineclasses", 
          new StartElementHandlerSection(this,LayoutDocEntry::ClassInlineClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_1_OPTION(
                                           theTranslator->trClassDocumentation(),
                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()
                                         )));
      m_sHandler.insert("class/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_typedefMembers,theTranslator->trMemberTypedefDocumentation()));
      m_sHandler.insert("class/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_enumMembers,theTranslator->trMemberEnumerationDocumentation()));
      m_sHandler.insert("class/memberdef/services",
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_serviceMembers,theTranslator->trInterfaces()));
      m_sHandler.insert("class/memberdef/interfaces",
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_interfaceMembers,theTranslator->trInterfaces()));
      m_sHandler.insert("class/memberdef/constructors", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_constructors,theTranslator->trConstructorDocumentation()));
      m_sHandler.insert("class/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_functionMembers,
                                        COMPILE_FOR_2_OPTIONS(
                                          theTranslator->trMemberFunctionDocumentation(),
                                          SrcLangExt_ObjC,theTranslator->trMethodDocumentation(),
                                          SrcLangExt_Fortran,theTranslator->trMemberFunctionDocumentationFortran()
                                        )));
      m_sHandler.insert("class/memberdef/related", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_relatedMembers,theTranslator->trRelatedFunctionDocumentation()));
      m_sHandler.insert("class/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_variableMembers,theTranslator->trMemberDataDocumentation()));
      m_sHandler.insert("class/memberdef/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_propertyMembers,theTranslator->trPropertyDocumentation()));
      m_sHandler.insert("class/memberdef/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_eventMembers,theTranslator->trEventDocumentation()));
      m_eHandler.insert("class/memberdef", 
          new EndElementHandler(this,&LayoutParser::endMemberDef));
      m_eHandler.insert("class", 
          new EndElementHandler(this,&LayoutParser::endClass));


      // namespace layout handlers
      m_sHandler.insert("namespace", 
          new StartElementHandler(this,&LayoutParser::startNamespace));
      m_sHandler.insert("namespace/briefdescription", 
          new StartElementHandlerKind(this,LayoutDocEntry::BriefDesc,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("namespace/detaileddescription", 
          new StartElementHandlerSection(this,LayoutDocEntry::DetailedDesc,&LayoutParser::startSectionEntry,
                                         theTranslator->trDetailedDescription()));
      m_sHandler.insert("namespace/authorsection", 
          new StartElementHandlerKind(this,LayoutDocEntry::AuthorSection,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("namespace/memberdecl", 
          new StartElementHandler(this,&LayoutParser::startMemberDecl));
      m_sHandler.insert("namespace/memberdecl/nestednamespaces", 
          new StartElementHandlerSection(this,LayoutDocEntry::NamespaceNestedNamespaces,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_4_OPTIONS(
                                           theTranslator->trNamespaces(),
                                           SrcLangExt_Java,theTranslator->trPackages(),
                                           SrcLangExt_VHDL,theTranslator->trPackages(),
                                           SrcLangExt_IDL,theTranslator->trModules(),
                                           SrcLangExt_Fortran,theTranslator->trModules()
                                         )));
      m_sHandler.insert("namespace/memberdecl/constantgroups",
          new StartElementHandlerSection(this,LayoutDocEntry::NamespaceNestedConstantGroups,&LayoutParser::startSectionEntry,
                                         theTranslator->trConstantGroups()));
      m_sHandler.insert("namespace/memberdecl/classes", 
          new StartElementHandlerSection(this,LayoutDocEntry::NamespaceClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_2_OPTIONS(
                                           theTranslator->trCompounds(),
                                           SrcLangExt_VHDL,VhdlDocGen::trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                           SrcLangExt_Fortran,theTranslator->trDataTypes()
                                         )));
      m_sHandler.insert("namespace/memberdecl/membergroups", 
          new StartElementHandlerKind(this,LayoutDocEntry::MemberGroups,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("namespace/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("namespace/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("namespace/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decFuncMembers,
                                        COMPILE_FOR_2_OPTIONS(
                                          theTranslator->trFunctions(),
                                          SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                          SrcLangExt_VHDL,VhdlDocGen::trFunctionAndProc()
                                        )));
      m_sHandler.insert("namespace/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decVarMembers,theTranslator->trVariables()));
      m_eHandler.insert("namespace/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("namespace/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("namespace/memberdef/inlineclasses", 
          new StartElementHandlerSection(this,LayoutDocEntry::NamespaceInlineClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_1_OPTION(
                                           theTranslator->trClassDocumentation(),
                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()
                                         )));
      m_sHandler.insert("namespace/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("namespace/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("namespace/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docFuncMembers,
                                        COMPILE_FOR_1_OPTION(
                                          theTranslator->trFunctionDocumentation(),
                                          SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()
                                        )));
      m_sHandler.insert("namespace/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docVarMembers,theTranslator->trVariableDocumentation()));
      m_eHandler.insert("namespace/memberdef", 
          new EndElementHandler(this,&LayoutParser::endMemberDef));
      m_eHandler.insert("namespace", 
          new EndElementHandler(this,&LayoutParser::endNamespace));

      // file layout handlers
      m_sHandler.insert("file", 
          new StartElementHandler(this,&LayoutParser::startFile));
      m_sHandler.insert("file/briefdescription", 
          new StartElementHandlerKind(this,LayoutDocEntry::BriefDesc,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/detaileddescription", 
          new StartElementHandlerSection(this,LayoutDocEntry::DetailedDesc,&LayoutParser::startSectionEntry,
                                         theTranslator->trDetailedDescription()));
      m_sHandler.insert("file/authorsection", 
          new StartElementHandlerKind(this,LayoutDocEntry::AuthorSection,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/includes", 
          new StartElementHandlerKind(this,LayoutDocEntry::FileIncludes,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/includegraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::FileIncludeGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/includedbygraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::FileIncludedByGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/sourcelink", 
          new StartElementHandlerKind(this,LayoutDocEntry::FileSourceLink,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/memberdecl/membergroups", 
          new StartElementHandlerKind(this,LayoutDocEntry::MemberGroups,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("file/memberdecl", 
          new StartElementHandler(this,&LayoutParser::startMemberDecl));
      m_sHandler.insert("file/memberdecl/classes", 
          new StartElementHandlerSection(this,LayoutDocEntry::FileClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_2_OPTIONS(
                                           theTranslator->trCompounds(),
                                           SrcLangExt_VHDL,VhdlDocGen::trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                           SrcLangExt_Fortran,theTranslator->trDataTypes()
                                         )));
      m_sHandler.insert("file/memberdecl/namespaces", 
          new StartElementHandlerSection(this,LayoutDocEntry::FileNamespaces,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_3_OPTIONS(
                                           theTranslator->trNamespaces(),
                                           SrcLangExt_Java,theTranslator->trPackages(),
                                           SrcLangExt_IDL,theTranslator->trModules(),
                                           SrcLangExt_Fortran,theTranslator->trModules()
                                         )));
      m_sHandler.insert("file/memberdecl/constantgroups",
          new StartElementHandlerSection(this,LayoutDocEntry::FileNamespaces,&LayoutParser::startSectionEntry,
                                         theTranslator->trConstantGroups()));
      m_sHandler.insert("file/memberdecl/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decDefineMembers,theTranslator->trDefines()));
      m_sHandler.insert("file/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("file/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("file/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decFuncMembers,
                                        COMPILE_FOR_2_OPTIONS(
                                          theTranslator->trFunctions(),
                                          SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                          SrcLangExt_VHDL,VhdlDocGen::trFunctionAndProc()
                                        )));
      m_sHandler.insert("file/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decVarMembers,theTranslator->trVariables()));

      m_eHandler.insert("file/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("file/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("file/memberdef/inlineclasses", 
          new StartElementHandlerSection(this,LayoutDocEntry::FileInlineClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_1_OPTION(
                                           theTranslator->trClassDocumentation(),
                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()
                                         )));
      m_sHandler.insert("file/memberdef/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docDefineMembers,theTranslator->trDefineDocumentation()));
      m_sHandler.insert("file/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("file/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docEnumMembers,
                                        theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("file/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docFuncMembers,
                                        COMPILE_FOR_1_OPTION(
                                          theTranslator->trFunctionDocumentation(),
                                          SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()
                                        )));
      m_sHandler.insert("file/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docVarMembers,theTranslator->trVariableDocumentation()));
      m_eHandler.insert("file/memberdef", 
          new EndElementHandler(this,&LayoutParser::endMemberDef));
      m_eHandler.insert("file", 
          new EndElementHandler(this,&LayoutParser::endFile));

      // group layout handlers
      m_sHandler.insert("group", 
          new StartElementHandler(this,&LayoutParser::startGroup));
      m_sHandler.insert("group/briefdescription", 
          new StartElementHandlerKind(this,LayoutDocEntry::BriefDesc,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/detaileddescription", 
          new StartElementHandlerSection(this,LayoutDocEntry::DetailedDesc,&LayoutParser::startSectionEntry,
                                         theTranslator->trDetailedDescription()));
      m_sHandler.insert("group/authorsection", 
          new StartElementHandlerKind(this,LayoutDocEntry::AuthorSection,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/groupgraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::GroupGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/memberdecl/membergroups", 
          new StartElementHandlerKind(this,LayoutDocEntry::MemberGroups,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/memberdecl", 
          new StartElementHandler(this,&LayoutParser::startMemberDecl));
      m_sHandler.insert("group/memberdecl/classes", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_2_OPTIONS(
                                           theTranslator->trCompounds(),
                                           SrcLangExt_VHDL,VhdlDocGen::trVhdlType(VhdlDocGen::ENTITY,FALSE),
                                           SrcLangExt_Fortran,theTranslator->trDataTypes()
                                         )));
      m_sHandler.insert("group/memberdecl/namespaces", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupNamespaces,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_2_OPTIONS(
                                           theTranslator->trNamespaces(),
                                           SrcLangExt_Java,theTranslator->trPackages(),
                                           SrcLangExt_Fortran,theTranslator->trModules()
                                         )));
      m_sHandler.insert("group/memberdecl/dirs", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupDirs,&LayoutParser::startSectionEntry,
                                         theTranslator->trDirectories()
                                         ));
      m_sHandler.insert("group/memberdecl/nestedgroups", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupNestedGroups,&LayoutParser::startSectionEntry,
                                         theTranslator->trModules()
                                         ));
      m_sHandler.insert("group/memberdecl/files", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupFiles,&LayoutParser::startSectionEntry,
                                         theTranslator->trFile(TRUE,FALSE)
                                         ));

      m_sHandler.insert("group/memberdecl/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decDefineMembers,theTranslator->trDefines()));
      m_sHandler.insert("group/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("group/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("group/memberdecl/enumvalues", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decEnumValMembers,theTranslator->trEnumerationValues()));
      m_sHandler.insert("group/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decFuncMembers,
                                        COMPILE_FOR_2_OPTIONS(
                                          theTranslator->trFunctions(),
                                          SrcLangExt_Fortran,theTranslator->trSubprograms(),
                                          SrcLangExt_VHDL,VhdlDocGen::trFunctionAndProc()
                                        )));
      m_sHandler.insert("group/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decVarMembers,theTranslator->trVariables()));
      m_sHandler.insert("group/memberdecl/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decSignalMembers,theTranslator->trSignals()));
      m_sHandler.insert("group/memberdecl/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decPubSlotMembers,theTranslator->trPublicSlots()));
      m_sHandler.insert("group/memberdecl/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decProSlotMembers,theTranslator->trProtectedSlots()));
      m_sHandler.insert("group/memberdecl/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decPriSlotMembers,theTranslator->trPrivateSlots()));
      m_sHandler.insert("group/memberdecl/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decEventMembers,theTranslator->trEvents()));
      m_sHandler.insert("group/memberdecl/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decPropMembers,theTranslator->trProperties()));
      m_sHandler.insert("group/memberdecl/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberListType_decFriendMembers,theTranslator->trFriends()));
      m_eHandler.insert("group/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("group/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("group/memberdef/pagedocs", 
          new StartElementHandlerKind(this,LayoutDocEntry::GroupPageDocs,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/memberdef/inlineclasses", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupInlineClasses,&LayoutParser::startSectionEntry,
                                         COMPILE_FOR_1_OPTION(
                                           theTranslator->trClassDocumentation(),
                                           SrcLangExt_Fortran,theTranslator->trTypeDocumentation()
                                         )));
      m_sHandler.insert("group/memberdef/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docDefineMembers,theTranslator->trDefineDocumentation()));
      m_sHandler.insert("group/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("group/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docEnumMembers,theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("group/memberdef/enumvalues", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docEnumValMembers,theTranslator->trEnumerationValueDocumentation()));
      m_sHandler.insert("group/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docFuncMembers,
                                        COMPILE_FOR_1_OPTION(
                                          theTranslator->trFunctionDocumentation(),
                                          SrcLangExt_Fortran,theTranslator->trSubprogramDocumentation()
                                       )));
      m_sHandler.insert("group/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docVarMembers,theTranslator->trVariableDocumentation()));
      m_sHandler.insert("group/memberdef/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docSignalMembers,theTranslator->trSignals())); 
      m_sHandler.insert("group/memberdef/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docPubSlotMembers,theTranslator->trPublicSlots()));
      m_sHandler.insert("group/memberdef/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docProSlotMembers,theTranslator->trProtectedSlots()));
      m_sHandler.insert("group/memberdef/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docPriSlotMembers,theTranslator->trPrivateSlots()));
      m_sHandler.insert("group/memberdef/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docEventMembers,theTranslator->trEvents()));
      m_sHandler.insert("group/memberdef/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docPropMembers,theTranslator->trProperties()));
      m_sHandler.insert("group/memberdef/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberListType_docFriendMembers,theTranslator->trFriends()));
      m_eHandler.insert("group/memberdef", 
          new EndElementHandler(this,&LayoutParser::endMemberDef));
      m_eHandler.insert("group", 
          new EndElementHandler(this,&LayoutParser::endGroup));

      // directory layout handlers
      m_sHandler.insert("directory", 
          new StartElementHandler(this,&LayoutParser::startDirectory));
      m_sHandler.insert("directory/briefdescription", 
          new StartElementHandlerKind(this,LayoutDocEntry::BriefDesc,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("directory/detaileddescription", 
          new StartElementHandlerSection(this,LayoutDocEntry::DetailedDesc,&LayoutParser::startSectionEntry,
                                         theTranslator->trDetailedDescription()));
      m_sHandler.insert("directory/directorygraph", 
          new StartElementHandlerKind(this,LayoutDocEntry::DirGraph,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("directory/memberdecl", 
          new StartElementHandler(this,&LayoutParser::startMemberDecl));
      m_sHandler.insert("directory/memberdecl/dirs", 
          new StartElementHandlerKind(this,LayoutDocEntry::DirSubDirs,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("directory/memberdecl/files", 
          new StartElementHandlerKind(this,LayoutDocEntry::DirFiles,&LayoutParser::startSimpleEntry));
      m_eHandler.insert("directory/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_eHandler.insert("directory", 
          new EndElementHandler(this,&LayoutParser::endDirectory));
    }

    void startSimpleEntry(LayoutDocEntry::Kind k,const QXmlAttributes &attrib)
    {
      bool isVisible = elemIsVisible(attrib);
      if (m_part!=-1 && isVisible)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySimple(k));
      }
    }

    void startSectionEntry(LayoutDocEntry::Kind k,const QXmlAttributes &attrib,
                           const QCString &title)
    {
      bool isVisible = elemIsVisible(attrib);
      QCString userTitle = attrib.value("title").utf8();
      //printf("startSectionEntry: title='%s' userTitle='%s'\n",
      //    title.data(),userTitle.data());
      if (userTitle.isEmpty())  userTitle = title;
      if (m_part!=-1 && isVisible)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySection(k,userTitle));
      }
    }


    void startMemberDeclEntry(const QXmlAttributes &attrib,MemberListType type,
                              const QCString &title,const QCString &subscript)
    {
      //QCString visible = convertToQCString(attrib.value("visible"));
      //bool isVisible = visible.isEmpty() || (visible!="no" && visible!="0");
      QCString userTitle     = attrib.value("title").utf8();
      QCString userSubscript = attrib.value("subtitle").utf8();
      if (userTitle.isEmpty())     userTitle     = title;
      if (userSubscript.isEmpty()) userSubscript = subscript;
      //printf("memberdecl: %s\n",userTitle.data());
      if (m_part!=-1 /*&& isVisible*/)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntryMemberDecl(type,userTitle,userSubscript));
      }
    }

    void startMemberDefEntry(const QXmlAttributes &attrib,MemberListType type,
                             const QCString &title,const QCString &)
    {
      QCString userTitle = attrib.value("title").utf8();
      if (userTitle.isEmpty()) userTitle = title;
      //printf("memberdef: %s\n",userTitle.data());
      if (m_part!=-1 /*&& isVisible*/)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntryMemberDef(type,userTitle));
      }
    }

    void startLayout(const QXmlAttributes &)
    {
    }

    void endLayout()
    {
    }

    void startNavIndex(const QXmlAttributes &)
    {
      m_scope="navindex/";
      m_rootNav = LayoutDocManager::instance().rootNavEntry();
      if (m_rootNav) m_rootNav->clear();
    }

    void endNavIndex()
    {
      m_scope="";
      if (m_rootNav && !m_rootNav->find(LayoutNavEntry::MainPage))
      {
        // no MainPage node... add one as the first item of the root node...
        new LayoutNavEntry(m_rootNav,LayoutNavEntry::MainPage, TRUE, 
            /*Config_getBool("GENERATE_TREEVIEW") ? "main" :*/ "index",
            theTranslator->trMainPage(),TRUE);
      }
    }

    void startNavEntry(const QXmlAttributes &attrib)
    {
      static bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
      static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      static bool hasGraphicalHierarchy = Config_getBool("HAVE_DOT") &&
                                          Config_getBool("GRAPHICAL_HIERARCHY");
      static bool extractAll = Config_getBool("EXTRACT_ALL");
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
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt ? theTranslator->trModules() : theTranslator->trNamespaces(),
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageListDescription() : fortranOpt ? theTranslator->trModulesListDescription(extractAll) : theTranslator->trNamespaceListDescription(extractAll),
          "namespaces"
        },
        { "namespacelist",
          LayoutNavEntry::NamespaceList,
          javaOpt || vhdlOpt   ? theTranslator->trPackages() : fortranOpt ? theTranslator->trModulesList() : theTranslator->trNamespaceList(),
          QCString(),
          javaOpt || vhdlOpt   ? theTranslator->trPackageListDescription() : fortranOpt ? theTranslator->trModulesListDescription(extractAll) : theTranslator->trNamespaceListDescription(extractAll),
          "namespaces"
        },
        { "namespacemembers",
          LayoutNavEntry::NamespaceMembers,
          javaOpt || vhdlOpt   ? theTranslator->trPackageMembers() : fortranOpt ? theTranslator->trModulesMembers() : theTranslator->trNamespaceMembers(),
          QCString(),
          fortranOpt ? theTranslator->trModulesMemberDescription(extractAll) : theTranslator->trNamespaceMemberDescription(extractAll),
          "namespacemembers"
        },
        { "classindex",
          LayoutNavEntry::ClassIndex,
          fortranOpt ? theTranslator->trDataTypes() : vhdlOpt ? VhdlDocGen::trDesignUnits() : theTranslator->trCompoundIndex(),
          QCString(),
          QCString(),
          "classes"
        },
        { "classes",
          LayoutNavEntry::Classes,
          fortranOpt ? theTranslator->trCompoundListFortran() : vhdlOpt ? VhdlDocGen::trDesignUnitList() : theTranslator->trClasses(),
          theTranslator->trCompoundList(),
          fortranOpt ? theTranslator->trCompoundListDescriptionFortran() : vhdlOpt ? VhdlDocGen::trDesignUnitListDescription() : theTranslator->trCompoundListDescription(),
          "annotated"
        },
        { "classlist",
          LayoutNavEntry::ClassList,
          fortranOpt ? theTranslator->trCompoundListFortran() : vhdlOpt ? VhdlDocGen::trDesignUnitList() : theTranslator->trCompoundList(),
          QCString(),
          fortranOpt ? theTranslator->trCompoundListDescriptionFortran() : vhdlOpt ? VhdlDocGen::trDesignUnitListDescription() : theTranslator->trCompoundListDescription(),
          "annotated"
        },
        { "hierarchy",
          LayoutNavEntry::ClassHierarchy,
          vhdlOpt    ? VhdlDocGen::trDesignUnitHierarchy() : theTranslator->trClassHierarchy(),
          QCString(),
          theTranslator->trClassHierarchyDescription(),
          hasGraphicalHierarchy ? "inherits" : "hierarchy"
        },
        { "classmembers",
          LayoutNavEntry::ClassMembers,
          fortranOpt ? theTranslator->trCompoundMembersFortran() : vhdlOpt ? VhdlDocGen::trDesignUnitMembers() : theTranslator->trCompoundMembers(),
          QCString(),
          fortranOpt ? theTranslator->trCompoundMembersDescriptionFortran(extractAll) : theTranslator->trCompoundMembersDescription(extractAll),
          "functions"
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
        //{ "dirs",
        //  LayoutNavEntry::Dirs,
        //  theTranslator->trDirectories(),
        //  QCString(),
        //  theTranslator->trDirDescription(),
        //  "dirs"
        //},
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
          (LayoutNavEntry::Kind)0,
          QCString(),
          QCString(),
          QCString(),
          QCString()
        }
      };
      LayoutNavEntry::Kind kind;
      // find type in the table
      int i=0;
      QString type = attrib.value("type");
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
        if (type.isEmpty())
        {
          err("error: an entry tag within a navindex has no type attribute! Check your layout file!\n");
        }
        else
        {
          err("error: the type '%s' is not supported for the entry tag within a navindex! Check your layout file!\n",type.data());
        }
        m_invalidEntry=TRUE;
        return;
      }
      QCString baseFile = mapping[i].baseFile;
      QCString title = attrib.value("title").utf8();
      bool isVisible = elemIsVisible(attrib);
      if (title.isEmpty()) // use default title
      { 
        title = mapping[i].mainName; // use title for main row
        if (m_rootNav!=LayoutDocManager::instance().rootNavEntry() && !mapping[i].subName.isEmpty())
        {
          title = mapping[i].subName; // if this is a child of another row, use the subName if available
                                      // this is mainly done to get compatible naming with older versions.
        }
      }
      QCString intro = attrib.value("intro").utf8();
      if (intro.isEmpty()) // use default intro text
      {
        intro = mapping[i].intro;
      }
      QCString url = attrib.value("url").utf8();
      if (mapping[i].kind==LayoutNavEntry::User && !url.isEmpty())
      {
        baseFile=url;
      }
      else if (kind==LayoutNavEntry::UserGroup)
      {
        if (!url.isEmpty())
        {
          baseFile=url;
        }
        else
        {
          baseFile+=QCString().sprintf("%d",m_userGroupCount++);
        }
      }
      // create new item and make it the new root
      m_rootNav = new LayoutNavEntry(m_rootNav,kind,kind==LayoutNavEntry::MainPage?TRUE:isVisible,baseFile,title,intro);
    }

    void endNavEntry()
    {
      // set the root back to the parent
      if (m_rootNav && !m_invalidEntry) m_rootNav = m_rootNav->parent();
      m_invalidEntry=FALSE;
    }

    void startClass(const QXmlAttributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Class);
      m_scope="class/";
      m_part = (int)LayoutDocManager::Class;
    }

    void endClass()
    {
      m_scope="";
      m_part = -1;
    }

    void startNamespace(const QXmlAttributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Namespace);
      m_scope="namespace/";
      m_part = (int)LayoutDocManager::Namespace;
    }

    void endNamespace()
    {
      m_scope="";
      m_part = -1;
    }

    void startFile(const QXmlAttributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::File);
      m_scope="file/";
      m_part = (int)LayoutDocManager::File;
    }

    void endFile()
    {
      m_scope="";
      m_part = -1;
    }

    void startGroup(const QXmlAttributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Group);
      m_scope="group/";
      m_part = (int)LayoutDocManager::Group;
    }

    void endGroup()
    {
      m_scope="";
      m_part = -1;
    }

    void startDirectory(const QXmlAttributes &)
    {
      LayoutDocManager::instance().clear(LayoutDocManager::Directory);
      m_scope="directory/";
      m_part = (int)LayoutDocManager::Directory;
    }

    void endDirectory()
    {
      m_scope="";
      m_part = -1;
    }

    void startMemberDef(const QXmlAttributes &)
    {
      m_scope+="memberdef/";
      if (m_part!=-1)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySimple(LayoutDocEntry::MemberDefStart));
      }
    }

    void endMemberDef()
    {
      int i=m_scope.findRev("memberdef/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        if (m_part!=-1)
        {
          LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySimple(LayoutDocEntry::MemberDefEnd));
        }
      }
    }

    void startMemberDecl(const QXmlAttributes &)
    {
      m_scope+="memberdecl/";
      if (m_part!=-1)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySimple(LayoutDocEntry::MemberDeclStart));
      }
    }

    void endMemberDecl()
    {
      int i=m_scope.findRev("memberdecl/");
      if (i!=-1)
      {
        m_scope=m_scope.left(i);
        if (m_part!=-1)
        {
          LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySimple(LayoutDocEntry::MemberDeclEnd));
        }
      }
    }

    // reimplemented from QXmlDefaultHandler
    bool startElement( const QString&, const QString&, 
                       const QString& name, const QXmlAttributes& attrib )
    {
      //printf("startElement [%s]::[%s]\n",m_scope.data(),name.data());
      StartElementHandler *handler = m_sHandler[m_scope+name.utf8()];
      if (handler)
      {
        (*handler)(attrib);
      }
      else
      {
        err("error: Unexpected start tag `%s' found in scope='%s'!\n",
            name.data(),m_scope.data());
      }
      return TRUE;
    }
    bool endElement( const QString&, const QString&, const QString& name )
    {
      //printf("endElement [%s]::[%s]\n",m_scope.data(),name.data());
      EndElementHandler *handler;
      if (!m_scope.isEmpty() && m_scope.right(name.length()+1)==name.utf8()+"/")
      { // element ends current scope
        handler = m_eHandler[m_scope.left(m_scope.length()-1)];
      }
      else // continue with current scope
      {
        handler = m_eHandler[m_scope+name.utf8()];
      }
      if (handler)
      {
        (*handler)();
      }
      return TRUE;
    }
    bool startDocument()
    {
      return TRUE;
    }

  private:
    LayoutParser() : m_sHandler(163), m_eHandler(17), m_invalidEntry(FALSE) { }

    QDict<StartElementHandler> m_sHandler;
    QDict<EndElementHandler>   m_eHandler;
    QCString m_scope;
    int m_part;
    LayoutNavEntry *m_rootNav;
    bool m_invalidEntry;
    static int m_userGroupCount;
};

int LayoutParser::m_userGroupCount=0;

//---------------------------------------------------------------------------------

class LayoutErrorHandler : public QXmlErrorHandler
{
  public:
    LayoutErrorHandler(const char *fn) : fileName(fn) {}
    bool warning( const QXmlParseException &exception )
    {
      err("warning: at line %d column %d of %s: %s\n",
          exception.lineNumber(),exception.columnNumber(),fileName.data(),
          exception.message().data());
      return FALSE;
    }
    bool error( const QXmlParseException &exception )
    {
      err("error: at line %d column %d of %s: %s\n",
          exception.lineNumber(),exception.columnNumber(),fileName.data(),
          exception.message().data());
      return FALSE;
    }
    bool fatalError( const QXmlParseException &exception )
    {
      err("fatal error: at line %d column %d of %s: %s\n",
          exception.lineNumber(),exception.columnNumber(),fileName.data(),
          exception.message().data());
      return FALSE;
    }
    QString errorString() { return ""; }

  private:
    QString errorMsg;
    QString fileName;
};

//---------------------------------------------------------------------------------

class LayoutDocManager::Private
{
  public:
    QList<LayoutDocEntry> docEntries[LayoutDocManager::NrParts];
    LayoutNavEntry *rootNav;
};

LayoutDocManager::LayoutDocManager()
{
  d = new Private;
  int i;
  for (i=0;i<LayoutDocManager::NrParts;i++)
  {
    d->docEntries[i].setAutoDelete(TRUE);
  }
  d->rootNav = new LayoutNavEntry;
  LayoutParser::instance().init();
}


void LayoutDocManager::init()
{
  // parse the default layout
  LayoutErrorHandler errorHandler( "layout_default.xml" );
  QXmlInputSource source;
  source.setData( layout_default );
  QXmlSimpleReader reader;
  reader.setContentHandler( &LayoutParser::instance() );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
}

LayoutDocManager::~LayoutDocManager()
{
  delete d->rootNav;
  delete d;
}

LayoutDocManager & LayoutDocManager::instance()
{
  static LayoutDocManager *theInstance = new LayoutDocManager;
  return *theInstance;
}

const QList<LayoutDocEntry> &LayoutDocManager::docEntries(LayoutDocManager::LayoutPart part) const
{
  return d->docEntries[(int)part];
}

LayoutNavEntry* LayoutDocManager::rootNavEntry() const
{
  return d->rootNav;
}

void LayoutDocManager::addEntry(LayoutDocManager::LayoutPart p,LayoutDocEntry *e)
{
  d->docEntries[(int)p].append(e);
}

void LayoutDocManager::clear(LayoutDocManager::LayoutPart p)
{
  d->docEntries[(int)p].clear();
}

void LayoutDocManager::parse(QTextStream &t,const char *fileName)
{
  LayoutErrorHandler errorHandler(fileName);
  QXmlInputSource source( t );
  QXmlSimpleReader reader;
  reader.setContentHandler( &LayoutParser::instance() );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
}

//---------------------------------------------------------------------------------

void writeDefaultLayoutFile(const char *fileName)
{
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Failed to open file %s for writing!\n",fileName);
    return;
  }
  QTextStream t(&f);
  t << substitute(layout_default,"$doxygenversion",versionString);
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
    int key=input.mid(s,i-s).toInt();
    if (key==(int)lang) // found matching key
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




