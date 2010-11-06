/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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

#include <qxml.h>
#include <qfile.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qtextstream.h>

static const char layout_default[] =
#include "layout_default.h"
;

static bool elemIsVisible(const QXmlAttributes &attrib,bool defVal=TRUE)
{
  QCString visible = convertToQCString(attrib.value("visible"));
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

LayoutNavEntry *LayoutNavEntry::find(LayoutNavEntry::Kind kind) const
{
  LayoutNavEntry *result=0;
  QListIterator<LayoutNavEntry> li(m_children);
  LayoutNavEntry *entry;
  for (li.toFirst();(entry=li.current());++li)
  {
    // depth first search, needed to find the entry furthest from the 
    // root in case an entry is in the tree twice
    result = entry->find(kind);
    if (result) return result;
    if (entry->kind()==kind)
    {
      return entry;
    }
  }
  return result;
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
                                              MemberList::ListType type,
                                              const QCString &title,
                                              const QCString &subtitle); 
      public:
        StartElementHandlerMember(LayoutParser *parent, 
                                  Handler h,
                                  MemberList::ListType type,
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
        MemberList::ListType m_type;
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

      bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");

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
                                         vhdlOpt ? VhdlDocGen::trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE) :
                                         fortranOpt ? theTranslator->trDataTypes() :
                                         theTranslator->trCompounds() 
                                         ));
      m_sHandler.insert("class/memberdecl/publictypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubTypes,theTranslator->trPublicTypes()));
      m_sHandler.insert("class/memberdecl/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubSlots,theTranslator->trPublicSlots())); 
      m_sHandler.insert("class/memberdecl/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::signals,theTranslator->trSignals())); 
      m_sHandler.insert("class/memberdecl/publicmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubMethods,theTranslator->trPublicMembers())); 
      m_sHandler.insert("class/memberdecl/publicstaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubStaticMethods,theTranslator->trStaticPublicMembers())); 
      m_sHandler.insert("class/memberdecl/publicattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubAttribs,theTranslator->trPublicAttribs())); 
      m_sHandler.insert("class/memberdecl/publicstaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pubStaticAttribs,theTranslator->trStaticPublicAttribs())); 
      m_sHandler.insert("class/memberdecl/protectedtypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proTypes,theTranslator->trProtectedTypes())); 
      m_sHandler.insert("class/memberdecl/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proSlots,theTranslator->trProtectedSlots())); 
      m_sHandler.insert("class/memberdecl/protectedmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proMethods,theTranslator->trProtectedMembers())); 
      m_sHandler.insert("class/memberdecl/protectedstaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proStaticMethods,theTranslator->trStaticProtectedMembers()));
      m_sHandler.insert("class/memberdecl/protectedattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proAttribs,theTranslator->trProtectedAttribs())); 
      m_sHandler.insert("class/memberdecl/protectedstaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::proStaticAttribs,theTranslator->trStaticProtectedAttribs())); 
      m_sHandler.insert("class/memberdecl/packagetypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pacTypes,theTranslator->trPackageTypes())); 
      m_sHandler.insert("class/memberdecl/packagemethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pacMethods,theTranslator->trPackageMembers())); 
      m_sHandler.insert("class/memberdecl/packagestaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pacStaticMethods,theTranslator->trStaticPackageMembers())); 
      m_sHandler.insert("class/memberdecl/packageattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pacAttribs,theTranslator->trPackageAttribs())); 
      m_sHandler.insert("class/memberdecl/packagestaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::pacStaticAttribs,theTranslator->trStaticPackageAttribs())); 
      m_sHandler.insert("class/memberdecl/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::properties,theTranslator->trProperties())); 
      m_sHandler.insert("class/memberdecl/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::events,theTranslator->trEvents())); 
      m_sHandler.insert("class/memberdecl/privatetypes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priTypes,theTranslator->trPrivateTypes())); 
      m_sHandler.insert("class/memberdecl/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priSlots,theTranslator->trPrivateSlots())); 
      m_sHandler.insert("class/memberdecl/privatemethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priMethods,theTranslator->trPrivateMembers())); 
      m_sHandler.insert("class/memberdecl/privatestaticmethods", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priStaticMethods,theTranslator->trStaticPrivateMembers())); 
      m_sHandler.insert("class/memberdecl/privateattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priAttribs,theTranslator->trPrivateAttribs())); 
      m_sHandler.insert("class/memberdecl/privatestaticattributes", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::priStaticAttribs,theTranslator->trStaticPrivateAttribs())); 
      m_sHandler.insert("class/memberdecl/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::friends,theTranslator->trFriends()));
      m_sHandler.insert("class/memberdecl/related", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::related,theTranslator->trRelatedFunctions(),
                                        theTranslator->trRelatedSubscript())); 
      m_eHandler.insert("class/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("class/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("class/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::typedefMembers,theTranslator->trMemberTypedefDocumentation()));
      m_sHandler.insert("class/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::enumMembers,theTranslator->trMemberEnumerationDocumentation()));
      m_sHandler.insert("class/memberdef/constructors", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::constructors,theTranslator->trConstructorDocumentation()));
      m_sHandler.insert("class/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::functionMembers,
                                        fortranOpt ?
                                        theTranslator->trMemberFunctionDocumentationFortran() :
                                        theTranslator->trMemberFunctionDocumentation()));
      m_sHandler.insert("class/memberdef/related", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::relatedMembers,theTranslator->trRelatedFunctionDocumentation()));
      m_sHandler.insert("class/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::variableMembers,theTranslator->trMemberDataDocumentation()));
      m_sHandler.insert("class/memberdef/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::propertyMembers,theTranslator->trPropertyDocumentation()));
      m_sHandler.insert("class/memberdef/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::eventMembers,theTranslator->trEventDocumentation()));
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
                                         javaOpt ? theTranslator->trPackages() :
                                         fortranOpt ? theTranslator->trModules() :
                                         theTranslator->trNamespaces()
                                         ));
      m_sHandler.insert("namespace/memberdecl/classes", 
          new StartElementHandlerSection(this,LayoutDocEntry::NamespaceClasses,&LayoutParser::startSectionEntry,
                                         vhdlOpt ? VhdlDocGen::trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE) :
                                         fortranOpt ? theTranslator->trDataTypes() :
                                         theTranslator->trCompounds() 
                                         ));
      m_sHandler.insert("namespace/memberdecl/membergroups", 
          new StartElementHandlerKind(this,LayoutDocEntry::MemberGroups,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("namespace/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("namespace/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("namespace/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decFuncMembers,
                                        fortranOpt ? theTranslator->trSubprograms()  : 
                                        vhdlOpt    ? VhdlDocGen::trFunctionAndProc() :
                                        theTranslator->trFunctions()));
      m_sHandler.insert("namespace/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decVarMembers,theTranslator->trVariables()));
      m_eHandler.insert("namespace/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("namespace/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("namespace/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("namespace/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docEnumMembers,theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("namespace/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docFuncMembers,
                                        fortranOpt ? 
                                        theTranslator->trSubprogramDocumentation() :
                                        theTranslator->trFunctionDocumentation()));
      m_sHandler.insert("namespace/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docVarMembers,theTranslator->trVariableDocumentation()));
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
                                         vhdlOpt ? VhdlDocGen::trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE) :
                                         fortranOpt ? theTranslator->trDataTypes() :
                                         theTranslator->trCompounds() 
                                         ));
      m_sHandler.insert("file/memberdecl/namespaces", 
          new StartElementHandlerSection(this,LayoutDocEntry::FileNamespaces,&LayoutParser::startSectionEntry,
                                         javaOpt ? theTranslator->trPackages() :
                                         fortranOpt ? theTranslator->trModules() :
                                         theTranslator->trNamespaces()
                                         ));
      m_sHandler.insert("file/memberdecl/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decDefineMembers,theTranslator->trDefines()));
      m_sHandler.insert("file/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("file/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("file/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decFuncMembers,
                                        fortranOpt ? theTranslator->trSubprograms()  : 
                                        vhdlOpt    ? VhdlDocGen::trFunctionAndProc() :
                                        theTranslator->trFunctions())) ;
      m_sHandler.insert("file/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decVarMembers,theTranslator->trVariables()));

      m_eHandler.insert("file/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("file/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("file/memberdef/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docDefineMembers,theTranslator->trDefineDocumentation()));
      m_sHandler.insert("file/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("file/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docEnumMembers,theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("file/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docFuncMembers,theTranslator->trFunctionDocumentation()));
      m_sHandler.insert("file/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docVarMembers,theTranslator->trVariableDocumentation()));
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
                                         vhdlOpt ? VhdlDocGen::trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE) :
                                         fortranOpt ? theTranslator->trDataTypes() :
                                         theTranslator->trCompounds() 
                                         ));
      m_sHandler.insert("group/memberdecl/namespaces", 
          new StartElementHandlerSection(this,LayoutDocEntry::GroupNamespaces,&LayoutParser::startSectionEntry,
                                         javaOpt ? theTranslator->trPackages() :
                                         fortranOpt ? theTranslator->trModules() :
                                         theTranslator->trNamespaces()
                                         ));
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
                                        MemberList::decDefineMembers,theTranslator->trDefines()));
      m_sHandler.insert("group/memberdecl/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decTypedefMembers,theTranslator->trTypedefs()));
      m_sHandler.insert("group/memberdecl/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decEnumMembers,theTranslator->trEnumerations()));
      m_sHandler.insert("group/memberdecl/enumvalues", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decEnumValMembers,theTranslator->trEnumerationValues()));
      m_sHandler.insert("group/memberdecl/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decFuncMembers,
                                            fortranOpt ? theTranslator->trSubprograms()  : 
                                            vhdlOpt    ? VhdlDocGen::trFunctionAndProc() :
                                            theTranslator->trFunctions()));
      m_sHandler.insert("group/memberdecl/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decVarMembers,theTranslator->trVariables()));
      m_sHandler.insert("group/memberdecl/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decSignalMembers,theTranslator->trSignals()));
      m_sHandler.insert("group/memberdecl/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decPubSlotMembers,theTranslator->trPublicSlots()));
      m_sHandler.insert("group/memberdecl/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decProSlotMembers,theTranslator->trProtectedSlots()));
      m_sHandler.insert("group/memberdecl/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decPriSlotMembers,theTranslator->trPrivateSlots()));
      m_sHandler.insert("group/memberdecl/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decEventMembers,theTranslator->trEvents()));
      m_sHandler.insert("group/memberdecl/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decPropMembers,theTranslator->trProperties()));
      m_sHandler.insert("group/memberdecl/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,
                                        MemberList::decFriendMembers,theTranslator->trFriends()));
      m_eHandler.insert("group/memberdecl", 
          new EndElementHandler(this,&LayoutParser::endMemberDecl));
      m_sHandler.insert("group/memberdef", 
          new StartElementHandler(this,&LayoutParser::startMemberDef));
      m_sHandler.insert("group/memberdef/pagedocs", 
          new StartElementHandlerKind(this,LayoutDocEntry::GroupPageDocs,&LayoutParser::startSimpleEntry));
      m_sHandler.insert("group/memberdef/defines", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docDefineMembers,theTranslator->trDefineDocumentation()));
      m_sHandler.insert("group/memberdef/typedefs", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docTypedefMembers,theTranslator->trTypedefDocumentation()));
      m_sHandler.insert("group/memberdef/enums", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docEnumMembers,theTranslator->trEnumerationTypeDocumentation()));
      m_sHandler.insert("group/memberdef/enumvalues", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docEnumValMembers,theTranslator->trEnumerationValueDocumentation()));
      m_sHandler.insert("group/memberdef/functions", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docFuncMembers,
                                        fortranOpt?
                                        theTranslator->trSubprogramDocumentation():
                                        theTranslator->trFunctionDocumentation()));
      m_sHandler.insert("group/memberdef/variables", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docVarMembers,theTranslator->trVariableDocumentation()));
      m_sHandler.insert("group/memberdef/signals", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docSignalMembers,theTranslator->trSignals())); 
      m_sHandler.insert("group/memberdef/publicslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docPubSlotMembers,theTranslator->trPublicSlots()));
      m_sHandler.insert("group/memberdef/protectedslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docProSlotMembers,theTranslator->trProtectedSlots()));
      m_sHandler.insert("group/memberdef/privateslots", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docPriSlotMembers,theTranslator->trPrivateSlots()));
      m_sHandler.insert("group/memberdef/events", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docEventMembers,theTranslator->trEvents()));
      m_sHandler.insert("group/memberdef/properties", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docPropMembers,theTranslator->trProperties()));
      m_sHandler.insert("group/memberdef/friends", 
          new StartElementHandlerMember(this,&LayoutParser::startMemberDefEntry,
                                        MemberList::docFriendMembers,theTranslator->trFriends()));
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
      QCString userTitle = convertToQCString(attrib.value("title"));
      //printf("startSectionEntry: title='%s' userTitle='%s'\n",
      //    title.data(),userTitle.data());
      if (userTitle.isEmpty())  userTitle = title;
      if (m_part!=-1 && isVisible)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntrySection(k,userTitle));
      }
    }


    void startMemberDeclEntry(const QXmlAttributes &attrib,MemberList::ListType type,
                              const QCString &title,const QCString &subscript)
    {
      //QCString visible = convertToQCString(attrib.value("visible"));
      //bool isVisible = visible.isEmpty() || (visible!="no" && visible!="0");
      QCString userTitle     = convertToQCString(attrib.value("title"));
      QCString userSubscript = convertToQCString(attrib.value("subtitle"));
      if (userTitle.isEmpty())     userTitle     = title;
      if (userSubscript.isEmpty()) userSubscript = subscript;
      //printf("memberdecl: %s\n",userTitle.data());
      if (m_part!=-1 /*&& isVisible*/)
      {
        LayoutDocManager::instance().addEntry((LayoutDocManager::LayoutPart)m_part,
                                              new LayoutDocEntryMemberDecl(type,userTitle,userSubscript));
      }
    }

    void startMemberDefEntry(const QXmlAttributes &attrib,MemberList::ListType type,
                             const QCString &title,const QCString &)
    {
      QCString userTitle = convertToQCString(attrib.value("title"));
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
      m_rootNav->clear();
    }

    void endNavIndex()
    {
      m_scope="";
      if (!m_rootNav->find(LayoutNavEntry::MainPage))
      {
        // no MainPage node... add one the first item of the root node...
        new LayoutNavEntry(m_rootNav,LayoutNavEntry::MainPage, TRUE, 
            Config_getBool("GENERATE_TREEVIEW") ? "main" : "index",
            theTranslator->trMainPage(),TRUE);
      }
    }

    void startNavEntry(const QXmlAttributes &attrib)
    {
      bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
      bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
      bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
      bool hasGraphicalHierarchy = Config_getBool("HAVE_DOT") &&
                                   Config_getBool("GRAPHICAL_HIERARCHY");
      static struct NavEntryMap
      {
        const char *typeStr;       // type attribute name in the XML file
        LayoutNavEntry::Kind kind; // corresponding enum name
        QCString mainName;         // default title for an item if it has children
        QCString subName;          // optional name for an item if it is rendered as a child
        QCString baseFile;         // base name of the file containing the index page
      } mapping[] =
      {
        { "mainpage",         LayoutNavEntry::MainPage,         theTranslator->trMainPage(),     QCString(), Config_getBool("GENERATE_TREEVIEW") ? "main" : "index" },
        { "pages",            LayoutNavEntry::Pages,            theTranslator->trRelatedPages(), QCString(), "pages"  },
        { "modules",          LayoutNavEntry::Modules,          theTranslator->trModules(),      QCString(), "modules" },
        { "namespaces",       LayoutNavEntry::Namespaces,       javaOpt    ? theTranslator->trPackages() : 
                                                                fortranOpt ? theTranslator->trModules() : 
                                                                theTranslator->trNamespaces(),        
                                                                javaOpt    ? theTranslator->trPackages() :
                                                                fortranOpt ? theTranslator->trModulesList() :
                                                                theTranslator->trNamespaceList(), "namespaces" },
        { "namespacemembers", LayoutNavEntry::NamespaceMembers, javaOpt    ? theTranslator->trPackageMembers() :
                                                                fortranOpt ? theTranslator->trModulesMembers() :
                                                                theTranslator->trNamespaceMembers(), QCString(), "namespacemembers" },
        { "classindex",       LayoutNavEntry::Classes,          fortranOpt ? theTranslator->trDataTypes() :
                                                                vhdlOpt    ? VhdlDocGen::trDesignUnits() :
                                                                theTranslator->trCompoundIndex(),
                                                                QCString(), "classes" },
        { "classes",          LayoutNavEntry::ClassAnnotated,   fortranOpt ? theTranslator->trCompoundListFortran() :
                                                                vhdlOpt    ? VhdlDocGen::trDesignUnitList() :
                                                                theTranslator->trClasses(), 
                                                                theTranslator->trCompoundList(), "annotated" },
        { "hierarchy",        LayoutNavEntry::ClassHierarchy,   vhdlOpt    ? VhdlDocGen::trDesignUnitHierarchy() :
                                                                theTranslator->trClassHierarchy(), QCString(), 
                                                                hasGraphicalHierarchy ? "inherits" : "hierarchy" },
        { "classmembers",     LayoutNavEntry::ClassMembers,     fortranOpt ? theTranslator->trCompoundMembersFortran() :
                                                                vhdlOpt    ? VhdlDocGen::trDesignUnitMembers() :
                                                                theTranslator->trCompoundMembers(), QCString(), "functions" },
        { "files",            LayoutNavEntry::Files,            theTranslator->trFile(TRUE,FALSE), 
                                                                theTranslator->trFileList(), "files" },
        { "globals",          LayoutNavEntry::FileGlobals,      theTranslator->trFileMembers(), QCString(), "globals" },
        { "dirs",             LayoutNavEntry::Dirs,             theTranslator->trDirectories(), QCString(), "dirs" },
        { "examples",         LayoutNavEntry::Examples,         theTranslator->trExamples(), QCString(), "examples" },
        { 0,                 (LayoutNavEntry::Kind)0,           QCString(), QCString(), QCString() }
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
          err("error: the type '%s' is not supported for the entry tag within a navindex! Check your layout file!\n");
        }
        return;
      }
      QCString baseFile = mapping[i].baseFile;
      QCString title = convertToQCString(attrib.value("title"));
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
      // create new item and make it the new root
      m_rootNav = new LayoutNavEntry(m_rootNav,kind,kind==LayoutNavEntry::MainPage?TRUE:isVisible,baseFile,title);
    }

    void endNavEntry()
    {
      // set the root back to the parent
      m_rootNav = m_rootNav->parent();
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
      StartElementHandler *handler = m_sHandler[m_scope+name];
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
      if (!m_scope.isEmpty() && m_scope.right(name.length()+1)==name+"/")
      { // element ends current scope
        handler = m_eHandler[m_scope.left(m_scope.length()-1)];
      }
      else // continue with current scope
      {
        handler = m_eHandler[m_scope+name];
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
    LayoutParser() : m_sHandler(163), m_eHandler(17) { }

    QDict<StartElementHandler> m_sHandler;
    QDict<EndElementHandler>   m_eHandler;
    QString m_scope;
    int m_part;
    LayoutNavEntry *m_rootNav;
};

//---------------------------------------------------------------------------------

class LayoutErrorHandler : public QXmlErrorHandler
{
  public:
    bool warning( const QXmlParseException &exception )
    {
      err("error: at line %d column %d: %s\n",
          exception.lineNumber(),exception.columnNumber(),
          exception.message().data());
      return FALSE;
    }
    bool error( const QXmlParseException &exception )
    {
      err("error: at line %d column %d: %s\n",
          exception.lineNumber(),exception.columnNumber(),
          exception.message().data());
      return FALSE;
    }
    bool fatalError( const QXmlParseException &exception )
    {
      err("error: at line %d column %d: %s\n",
          exception.lineNumber(),exception.columnNumber(),
          exception.message().data());
      return FALSE;
    }
    QString errorString() { return ""; }

  private:
    QString errorMsg;
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
  LayoutErrorHandler errorHandler;
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

void LayoutDocManager::parse(QTextStream &t)
{
  LayoutErrorHandler errorHandler;
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
  t << layout_default;
}
