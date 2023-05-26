/******************************************************************************
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

#include "namespacedef.h"
#include "outputlist.h"
#include "util.h"
#include "language.h"
#include "classdef.h"
#include "classlist.h"
#include "memberlist.h"
#include "doxygen.h"
#include "message.h"
#include "docparser.h"
#include "searchindex.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "membergroup.h"
#include "config.h"
#include "definitionimpl.h"
#include "membername.h"
#include "conceptdef.h"

//------------------------------------------------------------------
static QCString makeDisplayName(const NamespaceDef *nd,bool includeScope)
{
  QCString result=includeScope ? nd->name() : nd->localName();
  SrcLangExt lang = nd->getLanguage();
  QCString sep = getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    result = substitute(result,"::",sep);
  }
  //printf("makeDisplayName() %s->%s lang=%d\n",qPrint(name()),qPrint(result),lang);
  return result;
}
//------------------------------------------------------------------

class NamespaceDefImpl : public DefinitionMixin<NamespaceDefMutable>
{
  public:
    NamespaceDefImpl(const QCString &defFileName,int defLine,int defColumn,
                 const QCString &name,const QCString &ref=QCString(),
                 const QCString &refFile=QCString(),const QCString &type=QCString(),
                 bool isPublished=false);
    virtual ~NamespaceDefImpl();
    virtual DefType definitionType() const { return TypeNamespace; }
    virtual CodeSymbolType codeSymbolType() const
    { return getLanguage()==SrcLangExt_Java ? CodeSymbolType::Package : CodeSymbolType::Namespace; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual void insertUsedFile(FileDef *fd);
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeMemberPages(OutputList &ol);
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const;
    virtual void writeTagFile(TextStream &);
    virtual void insertClass(ClassDef *cd);
    virtual void insertConcept(ConceptDef *cd);
    virtual void insertNamespace(NamespaceDef *nd);
    virtual void insertMember(MemberDef *md);
    virtual void computeAnchors();
    virtual void countMembers();
    virtual int  numDocMembers() const;
    virtual void addUsingDirective(NamespaceDef *nd);
    virtual const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const { return m_usingDirList; }
    virtual void addUsingDeclaration(ClassDef *cd);
    virtual const LinkedRefMap<ClassDef> &getUsedClasses() const { return m_usingDeclList; }
    virtual void combineUsingRelations(NamespaceDefSet &visitedNamespace);
    virtual QCString displayName(bool=TRUE) const;
    virtual void setInline(bool isInline) { m_inline = isInline; }
    virtual bool isConstantGroup() const { return CONSTANT_GROUP == m_type; }
    virtual bool isModule()        const { return NAMESPACE == m_type || MODULE == m_type; }
    virtual bool isLibrary() const { return LIBRARY == m_type; }
    virtual bool isInline() const { return m_inline; }
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual bool hasDetailedDescription() const;
    virtual void addMembersToMemberGroup();
    virtual void distributeMemberGroupDocumentation();
    virtual void findSectionsInDocumentation();
    virtual void sortMemberLists();
    virtual const Definition *findInnerCompound(const QCString &name) const;
    virtual void addInnerCompound(Definition *d);
    virtual void addListReferences();
    virtual void setFileName(const QCString &fn);
    virtual bool subGrouping() const { return m_subGrouping; }
    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const MemberLists &getMemberLists() const { return m_memberLists; }
    virtual const MemberDef *getMemberByName(const QCString &) const;
    virtual const MemberGroupList &getMemberGroups() const { return m_memberGroups; }
    virtual ClassLinkedRefMap getClasses() const { return classes; }
    virtual ClassLinkedRefMap getInterfaces() const { return interfaces; }
    virtual ClassLinkedRefMap getStructs() const { return structs; }
    virtual ClassLinkedRefMap getExceptions() const { return exceptions; }
    virtual NamespaceLinkedRefMap getNamespaces() const { return namespaces; }
    virtual ConceptLinkedRefMap getConcepts() const { return m_concepts; }
    virtual void setName(const QCString &name);

    virtual QCString title() const;
    virtual QCString compoundTypeString() const;

    virtual void setMetaData(const QCString &m);
    virtual int countVisibleMembers() const;

  private:
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void writeClassDeclarations(OutputList &ol,const QCString &title,const ClassLinkedRefMap &d);
    void writeConcepts(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeSummaryLinks(OutputList &ol) const;
    void addNamespaceAttributes(OutputList &ol);
    void writeClassesToTagFile(TextStream &,const ClassLinkedRefMap &d);
    void writeConceptsToTagFile(TextStream &);

    void writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool isConstantGroup=false);
    void updateLocalName();

    QCString              fileName;
    FileList              files;

    LinkedRefMap<NamespaceDef> m_usingDirList;
    LinkedRefMap<ClassDef> m_usingDeclList;
    LinkedRefMap<const Definition> m_innerCompounds;

    MemberLinkedRefMap    m_allMembers;
    MemberLists           m_memberLists;
    MemberGroupList       m_memberGroups;
    ClassLinkedRefMap     classes;
    ClassLinkedRefMap     interfaces;
    ClassLinkedRefMap     structs;
    ClassLinkedRefMap     exceptions;
    ConceptLinkedRefMap   m_concepts;
    NamespaceLinkedRefMap namespaces;
    bool                  m_subGrouping = false;
    enum { NAMESPACE, MODULE, CONSTANT_GROUP, LIBRARY } m_type;
    bool m_isPublished = false;
    QCString              metaData;
    bool                  m_inline = false;
};

std::unique_ptr<NamespaceDef> createNamespaceDef(const QCString &defFileName,int defLine,int defColumn,
                 const QCString &name,const QCString &ref,
                 const QCString &refFile,const QCString &type,
                 bool isPublished)
{
  //printf("createNamespaceDef(%s)\n",qPrint(name));
  return std::make_unique<NamespaceDefImpl>(defFileName,defLine,defColumn,name,ref,refFile,type,isPublished);
}

//------------------------------------------------------------------

class NamespaceDefAliasImpl : public DefinitionAliasMixin<NamespaceDef>
{
  public:
    NamespaceDefAliasImpl(const Definition *newScope,const NamespaceDef *nd)
      : DefinitionAliasMixin(newScope,nd) { init(); }
    virtual ~NamespaceDefAliasImpl() { deinit(); }
    virtual DefType definitionType() const { return TypeNamespace; }

    const NamespaceDef *getNSAlias() const { return toNamespaceDef(getAlias()); }

    // ---- getters
    virtual CodeSymbolType codeSymbolType() const
    { return getNSAlias()->codeSymbolType(); }
    virtual QCString getOutputFileBase() const
    { return getNSAlias()->getOutputFileBase(); }
    virtual QCString anchor() const
    { return getNSAlias()->anchor(); }
    virtual int numDocMembers() const
    { return getNSAlias()->numDocMembers(); }
    virtual const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const
    { return getNSAlias()->getUsedNamespaces(); }
    virtual const LinkedRefMap<ClassDef> &getUsedClasses() const
    { return getNSAlias()->getUsedClasses(); }
    virtual QCString displayName(bool b=TRUE) const
    { return makeDisplayName(this,b); }
    virtual const QCString &localName() const
    { return getNSAlias()->localName(); }
    virtual bool isConstantGroup() const
    { return getNSAlias()->isConstantGroup(); }
    virtual bool isModule()        const
    { return getNSAlias()->isModule(); }
    virtual bool isLibrary() const
    { return getNSAlias()->isLibrary(); }
    virtual bool isInline() const
    { return getNSAlias()->isInline(); }
    virtual bool isLinkableInProject() const
    { return getNSAlias()->isLinkableInProject(); }
    virtual bool isLinkable() const
    { return getNSAlias()->isLinkable(); }
    virtual bool hasDetailedDescription() const
    { return getNSAlias()->hasDetailedDescription(); }
    virtual const Definition *findInnerCompound(const QCString &name) const
    { return getNSAlias()->findInnerCompound(name); }
    virtual bool subGrouping() const
    { return getNSAlias()->subGrouping(); }
    virtual MemberList *getMemberList(MemberListType lt) const
    { return getNSAlias()->getMemberList(lt); }
    virtual const MemberLists &getMemberLists() const
    { return getNSAlias()->getMemberLists(); }
    virtual const MemberDef *getMemberByName(const QCString &name) const
    { return getNSAlias()->getMemberByName(name); }
    virtual const MemberGroupList &getMemberGroups() const
    { return getNSAlias()->getMemberGroups(); }
    virtual ClassLinkedRefMap getClasses() const
    { return getNSAlias()->getClasses(); }
    virtual ClassLinkedRefMap getInterfaces() const
    { return getNSAlias()->getInterfaces(); }
    virtual ClassLinkedRefMap getStructs() const
    { return getNSAlias()->getStructs(); }
    virtual ClassLinkedRefMap getExceptions() const
    { return getNSAlias()->getExceptions(); }
    virtual NamespaceLinkedRefMap getNamespaces() const
    { return getNSAlias()->getNamespaces(); }
    virtual ConceptLinkedRefMap getConcepts() const
    { return getNSAlias()->getConcepts(); }
    virtual QCString title() const
    { return getNSAlias()->title(); }
    virtual QCString compoundTypeString() const
    { return getNSAlias()->compoundTypeString(); }
    virtual int countVisibleMembers() const
    { return getNSAlias()->countVisibleMembers(); }
};

std::unique_ptr<NamespaceDef> createNamespaceDefAlias(const Definition *newScope,const NamespaceDef *nd)
{
  auto alnd = std::make_unique<NamespaceDefAliasImpl>(newScope,nd);
  //printf("alnd name=%s localName=%s qualifiedName=%s displayName()=%s\n",
  //    qPrint(alnd->name()),qPrint(alnd->localName()),qPrint(alnd->qualifiedName()),
  //    qPrint(alnd->displayName()));
  return alnd;
}

//------------------------------------------------------------------

NamespaceDefImpl::NamespaceDefImpl(const QCString &df,int dl,int dc,
                           const QCString &name,const QCString &lref,
                           const QCString &fName, const QCString &type,
                           bool isPublished) :
   DefinitionMixin(df,dl,dc,name)
  ,m_isPublished(isPublished)
{
  if (!fName.isEmpty())
  {
    if (!lref.isEmpty())
    {
      fileName = stripExtension(fName);
    }
    else
    {
      fileName = convertNameToFile(stripExtension(fName));
    }
  }
  else
  {
    setFileName(name);
  }
  setReference(lref);
  m_inline=FALSE;
  m_subGrouping=Config_getBool(SUBGROUPING);
  if (type=="module")
  {
    m_type = MODULE;
  }
  else if (type=="constants")
  {
    m_type = CONSTANT_GROUP;
  }
  else if (type=="library")
  {
    m_type = LIBRARY;
  }
  else
  {
    m_type = NAMESPACE;
  }

  updateLocalName();
}

void NamespaceDefImpl::updateLocalName()
{
  QCString locName=name();
  int i=locName.findRev("::");
  if (i!=-1)
  {
    locName=locName.mid(i+2);
  }
  setLocalName(locName);
}

void NamespaceDefImpl::setName(const QCString &name)
{
  DefinitionMixin<NamespaceDefMutable>::setName(name);
  updateLocalName();
}

NamespaceDefImpl::~NamespaceDefImpl()
{
}

void NamespaceDefImpl::setFileName(const QCString &fn)
{
  if (isReference())
  {
    fileName = "namespace"+fn;
  }
  else
  {
    fileName = convertNameToFile("namespace"+fn);
  }
}

void NamespaceDefImpl::distributeMemberGroupDocumentation()
{
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void NamespaceDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  for (const auto &mg : m_memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}

void NamespaceDefImpl::insertUsedFile(FileDef *fd)
{
  if (fd==0) return;
  auto it = std::find(files.begin(),files.end(),fd);
  if (it==files.end())
  {
    files.push_back(fd);
  }
}

void NamespaceDefImpl::addInnerCompound(Definition *d)
{
  //printf("%s:NamespaceDefImpl::addInnerCompound(%s)\n",qPrint(name()),qPrint(d->name()));
  m_innerCompounds.add(d->localName(),d);
  if (d->definitionType()==Definition::TypeNamespace)
  {
    insertNamespace(toNamespaceDef(d));
  }
  else if (d->definitionType()==Definition::TypeClass)
  {
    insertClass(toClassDef(d));
  }
  else if (d->definitionType()==Definition::TypeConcept)
  {
    insertConcept(toConceptDef(d));
  }
}

void NamespaceDefImpl::insertClass(ClassDef *cd)
{
  ClassLinkedRefMap &d = classes;

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      d = interfaces;
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      d = structs;
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      d = exceptions;
    }
  }

  d.add(cd->name(),cd);
}

void NamespaceDefImpl::insertConcept(ConceptDef *cd)
{
  m_concepts.add(cd->name(),cd);
}

void NamespaceDefImpl::insertNamespace(NamespaceDef *nd)
{
  namespaces.add(nd->name(),nd);
}


void NamespaceDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ::addMembersToMemberGroup(ml.get(),&m_memberGroups,this);
    }
  }

  // add members inside sections to their groups
  for (const auto &mg : m_memberGroups)
  {
    if (mg->allMembersInSameSection() && m_subGrouping)
    {
      //printf("----> addToDeclarationSection(%s)\n",qPrint(mg->header()));
      mg->addToDeclarationSection();
    }
  }
}

void NamespaceDefImpl::insertMember(MemberDef *md)
{
  //printf("%s::insertMember(%s) isInline=%d hasDocs=%d\n",qPrint(name()),qPrint(md->name()),
  //    isInline(),hasDocumentation());
  if (md->isHidden()) return;
  MemberDefMutable *mdm = toMemberDefMutable(md);

  // if this is an inline namespace that is not documented, then insert the
  // member in the parent scope instead
  if (isInline() && !hasDocumentation())
  {
    Definition *outerScope = getOuterScope();
    if (outerScope)
    {
      if (outerScope->definitionType()==Definition::TypeNamespace)
      {
        NamespaceDefMutable *nd = toNamespaceDefMutable(outerScope);
        if (nd)
        {
          nd->insertMember(md);
          if (mdm)
          {
            mdm->setNamespace(nd);
          }
        }
      }
      else if (outerScope->definitionType()==Definition::TypeFile)
      {
        FileDef *fd = toFileDef(outerScope);
        fd->insertMember(md);
        if (mdm)
        {
          mdm->setFileDef(fd);
          mdm->setOuterScope(fd);
        }
      }
    }
  }
  else // member is a non-inline namespace or a documented inline namespace
  {
    MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
    if (allMemberList==0)
    {
      m_memberLists.emplace_back(std::make_unique<MemberList>(MemberListType_allMembersList,MemberListContainer::Namespace));
      allMemberList = m_memberLists.back().get();
    }
    allMemberList->push_back(md);
    //printf("%s::m_allMembersDict->append(%s)\n",qPrint(name()),qPrint(md->localName()));
    m_allMembers.add(md->localName(),md);
    switch(md->memberType())
    {
      case MemberType_Variable:
        addMemberToList(MemberListType_decVarMembers,md);
        addMemberToList(MemberListType_docVarMembers,md);
        break;
      case MemberType_Function:
        addMemberToList(MemberListType_decFuncMembers,md);
        addMemberToList(MemberListType_docFuncMembers,md);
        break;
      case MemberType_Typedef:
        addMemberToList(MemberListType_decTypedefMembers,md);
        addMemberToList(MemberListType_docTypedefMembers,md);
        break;
      case MemberType_Sequence:
        addMemberToList(MemberListType_decSequenceMembers,md);
        addMemberToList(MemberListType_docSequenceMembers,md);
        break;
      case MemberType_Dictionary:
        addMemberToList(MemberListType_decDictionaryMembers,md);
        addMemberToList(MemberListType_docDictionaryMembers,md);
        break;
      case MemberType_Enumeration:
        addMemberToList(MemberListType_decEnumMembers,md);
        addMemberToList(MemberListType_docEnumMembers,md);
        break;
      case MemberType_EnumValue:
        break;
      case MemberType_Define:
        addMemberToList(MemberListType_decDefineMembers,md);
        addMemberToList(MemberListType_docDefineMembers,md);
        break;
      default:
        err("NamespaceDefImpl::insertMembers(): "
            "member '%s' with unexpected type id %d and class scope '%s' inserted in namespace scope '%s'!\n",
            qPrint(md->name()),
            md->memberType(),
            md->getClassDef() ? qPrint(md->getClassDef()->name()) : "",
            qPrint(name()));
    }
    // if this is an inline namespace, then insert an alias of this member in the outer scope.
    if (isInline())
    {
      Definition *outerScope = getOuterScope();
      if (outerScope)
      {
        std::unique_ptr<MemberDef> aliasMd;
        if (outerScope->definitionType()==Definition::TypeNamespace)
        {
          aliasMd = createMemberDefAlias(outerScope,md);
          NamespaceDefMutable *ndm = toNamespaceDefMutable(outerScope);
          if (ndm)
          {
            ndm->insertMember(aliasMd.get());
          }
        }
        else if (outerScope->definitionType()==Definition::TypeFile)
        {
          aliasMd = createMemberDefAlias(outerScope,md);
          toFileDef(outerScope)->insertMember(aliasMd.get());
        }
        if (aliasMd)
        {
          MemberName *mn;
          QCString name = md->name();
          mn = Doxygen::functionNameLinkedMap->add(name);
          mn->push_back(std::move(aliasMd));
        }
      }
    }
  }
}

void NamespaceDefImpl::computeAnchors()
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList) allMemberList->setAnchors();
}

bool NamespaceDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().isEmpty());
}

void NamespaceDefImpl::writeTagFile(TextStream &tagFile)
{
  QCString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "  <compound kind=\"namespace\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <filename>" << fn << "</filename>\n";
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::NamespaceNestedNamespaces:
        {
          for (const auto &nd : namespaces)
          {
            if (nd->isLinkableInProject())
            {
              tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::NamespaceClasses:
        {
          writeClassesToTagFile(tagFile, classes);
        }
        break;
      case LayoutDocEntry::NamespaceInterfaces:
        {
          writeClassesToTagFile(tagFile, interfaces);
        }
        break;
      case LayoutDocEntry::NamespaceStructs:
        {
          writeClassesToTagFile(tagFile, structs);
        }
        break;
      case LayoutDocEntry::NamespaceExceptions:
        {
          writeClassesToTagFile(tagFile, exceptions);
        }
        break;
      case LayoutDocEntry::NamespaceConcepts:
        {
          writeConceptsToTagFile(tagFile);
        }
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            MemberList * ml = getMemberList(lmd->type);
            if (ml)
            {
              ml->writeTagFile(tagFile);
            }
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          for (const auto &mg : m_memberGroups)
          {
            mg->writeTagFile(tagFile);
          }
        }
        break;
      default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>\n";
}

void NamespaceDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.writeRuler();
    ol.popGeneratorState();
    ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.writeAnchor(QCString(),"details");
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();

    ol.startTextBlock();
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputType::Man);
        ol.disable(OutputType::RTF);
        ol.enableAll();
        ol.disableAllBut(OutputType::Man);
        ol.enable(OutputType::Latex);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    ol.endTextBlock();
  }
}

void NamespaceDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),briefLine(),this,0,
                                     briefDescription(),TRUE,FALSE,
                                     QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    if (!ast->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(ast.get(),this,0);
      ol.pushGeneratorState();
      ol.disable(OutputType::RTF);
      ol.writeString(" \n");
      ol.enable(OutputType::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputType::Html);
        ol.startTextLink(getOutputFileBase(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
  }

  // Write a summary of the Slice definition including metadata.
  if (getLanguage() == SrcLangExt_Slice)
  {
    ol.startParagraph();
    ol.startTypewriter();
    if (!metaData.isEmpty())
    {
      ol.docify(metaData);
      ol.lineBreak();
    }
    ol.docify("module ");
    ol.docify(stripScope(name()));
    ol.docify(" { ... }");
    ol.endTypewriter();
    ol.endParagraph();
  }

  ol.writeSynopsis();
}

void NamespaceDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void NamespaceDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void NamespaceDefImpl::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputType::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void NamespaceDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputType::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void NamespaceDefImpl::writeClassDeclarations(OutputList &ol,const QCString &title,const ClassLinkedRefMap &d)
{
  d.writeDeclaration(ol,0,title,TRUE);
}

void NamespaceDefImpl::writeConcepts(OutputList &ol,const QCString &title)
{
  m_concepts.writeDeclaration(ol,title,TRUE);
}

void NamespaceDefImpl::writeInlineClasses(OutputList &ol)
{
  classes.writeDocumentation(ol,this);
}

void NamespaceDefImpl::writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool const isConstantGroup)
{
  namespaces.writeDeclaration(ol,title,isConstantGroup,TRUE);
}

void NamespaceDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    if (!mg->allMembersInSameSection() || !m_subGrouping)
    {
      mg->writeDeclarations(ol,0,this,0,0);
    }
  }
}

void NamespaceDefImpl::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void NamespaceDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    if (lde->kind()==LayoutDocEntry::NamespaceClasses && classes.declVisible() && ls)
    {
      QCString label = "nested-classes";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceInterfaces && interfaces.declVisible() && ls)
    {
      QCString label = "interfaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceStructs && structs.declVisible() && ls)
    {
      QCString label = "structs";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceExceptions && exceptions.declVisible() && ls)
    {
      QCString label = "exceptions";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceNestedNamespaces && namespaces.declVisible(false) && ls)
    {
      QCString label = "namespaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceNestedConstantGroups && namespaces.declVisible(true) && ls)
    {
      QCString label = "constantgroups";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceConcepts && m_concepts.declVisible() && ls)
    {
      QCString label = "concepts";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()== LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        MemberList * ml = getMemberList(lmd->type);
        if (ml && ml->declVisible())
        {
          ol.writeSummaryLink(QCString(),MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
          first=FALSE;
        }
      }
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void NamespaceDefImpl::addNamespaceAttributes(OutputList &ol)
{
  // UNO IDL constant groups may be published
  if (getLanguage()==SrcLangExt_IDL && isConstantGroup() && m_isPublished)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.startLabels();
    ol.writeLabel("published",false);
    ol.endLabels();
    ol.popGeneratorState();
  }
}

void NamespaceDefImpl::writeClassesToTagFile(TextStream &tagFile,const ClassLinkedRefMap &list)
{
  for (const auto &cd : list)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <class kind=\"" << cd->compoundTypeString()
              << "\">" << convertToXML(cd->name()) << "</class>\n";
    }
  }
}

void NamespaceDefImpl::writeConceptsToTagFile(TextStream &tagFile)
{
  for (const auto &cd : m_concepts)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <concept>" << convertToXML(cd->name()) << "</concept>\n";
    }
  }
}

void NamespaceDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //bool outputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);

  QCString pageTitle = title();
  startFile(ol,getOutputFileBase(),name(),pageTitle,HighlightedItem::NamespaceVisible,!generateTreeView);

  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase(),this);
  ol.parseText(pageTitle);
  addGroupListToTitle(ol,this);
  addNamespaceAttributes(ol);
  endTitle(ol,getOutputFileBase(),displayName());
  ol.startContents();

  //if (Doxygen::searchIndex)
  //{
  //  Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
  //  Doxygen::searchIndex->addWord(localName(),TRUE);
  //}
  //Doxygen::indexList->addIndexItem(this,0);

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
        startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::NamespaceClasses:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),classes);
        }
        break;
      case LayoutDocEntry::NamespaceInterfaces:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),interfaces);
        }
        break;
      case LayoutDocEntry::NamespaceStructs:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),structs);
        }
        break;
      case LayoutDocEntry::NamespaceExceptions:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),exceptions);
        }
        break;
      case LayoutDocEntry::NamespaceConcepts:
        {
          if (ls) writeConcepts(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::NamespaceNestedNamespaces:
        {
          if (ls) writeNamespaceDeclarations(ol,ls->title(lang),false);
        }
        break;
      case LayoutDocEntry::NamespaceNestedConstantGroups:
        {
          if (ls) writeNamespaceDeclarations(ol,ls->title(lang),true);
        }
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd) writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          if (ls) writeDetailedDescription(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::NamespaceInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
          if (lmd) writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::AuthorSection:
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::ClassIncludes:
      case LayoutDocEntry::ClassInheritanceGraph:
      case LayoutDocEntry::ClassNestedClasses:
      case LayoutDocEntry::ClassCollaborationGraph:
      case LayoutDocEntry::ClassAllMembersLink:
      case LayoutDocEntry::ClassUsedFiles:
      case LayoutDocEntry::ClassInlineClasses:
      case LayoutDocEntry::ConceptDefinition:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileConcepts:
      case LayoutDocEntry::FileInterfaces:
      case LayoutDocEntry::FileStructs:
      case LayoutDocEntry::FileExceptions:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileConstantGroups:
      case LayoutDocEntry::FileIncludes:
      case LayoutDocEntry::FileIncludeGraph:
      case LayoutDocEntry::FileIncludedByGraph:
      case LayoutDocEntry::FileSourceLink:
      case LayoutDocEntry::FileInlineClasses:
      case LayoutDocEntry::GroupClasses:
      case LayoutDocEntry::GroupConcepts:
      case LayoutDocEntry::GroupInlineClasses:
      case LayoutDocEntry::GroupNamespaces:
      case LayoutDocEntry::GroupDirs:
      case LayoutDocEntry::GroupNestedGroups:
      case LayoutDocEntry::GroupFiles:
      case LayoutDocEntry::GroupGraph:
      case LayoutDocEntry::GroupPageDocs:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Namespace entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(ol,this);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
    if (allMemberList) allMemberList->sort();
    writeMemberPages(ol);
  }
}

void NamespaceDefImpl::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  for (const auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->writeDocumentationPage(ol,displayName(),this);
    }
  }
  ol.popGeneratorState();
}

void NamespaceDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList)
  {
    for (const auto &md : *allMemberList)
    {
      if (md->getNamespaceDef()==this && md->isLinkable() && !md->isEnumValue())
      {
        if (md->isLinkableInProject())
        {
          QCString fn = md->getOutputFileBase();
          addHtmlExtensionIfMissing(fn);
          if (md==currentMd) // selected item => highlight
          {
            ol.writeString("          <tr><td class=\"navtabHL\">");
          }
          else
          {
            ol.writeString("          <tr><td class=\"navtab\">");
          }
          ol.writeString("<a class=\"navtab\" ");
          ol.writeString("href=\"");
          if (createSubDirs) ol.writeString("../../");
          ol.writeString(fn+"#"+md->anchor());
          ol.writeString("\">");
          ol.writeString(convertToHtml(md->localName()));
          ol.writeString("</a>");
          ol.writeString("</td></tr>\n");
        }
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}

void NamespaceDefImpl::countMembers()
{
  for (auto &ml : m_memberLists)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->countDecMembers();
    mg->countDocMembers();
  }
}

int NamespaceDefImpl::numDocMembers() const
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  return (allMemberList ? allMemberList->numDocMembers() : 0) + static_cast<int>(m_innerCompounds.size());
}

void NamespaceDefImpl::addUsingDirective(NamespaceDef *nd)
{
  m_usingDirList.add(nd->qualifiedName(),nd);
  //printf("%s: NamespaceDefImpl::addUsingDirective: %s:%zu\n",qPrint(name()),qPrint(nd->qualifiedName()),m_usingDirList.size());
}

void NamespaceDefImpl::addUsingDeclaration(ClassDef *cd)
{
  m_usingDeclList.add(cd->qualifiedName(),cd);
}

QCString NamespaceDefImpl::getOutputFileBase() const
{
  return fileName;
}

const Definition *NamespaceDefImpl::findInnerCompound(const QCString &n) const
{
  if (n==0) return 0;
  const Definition *d = m_innerCompounds.find(n);
  if (d==0)
  {
    if (!m_usingDirList.empty())
    {
      d = m_usingDirList.find(n);
    }
    if (d==0 && !m_usingDeclList.empty())
    {
      d = m_usingDeclList.find(n);
    }
  }
  return d;
}

void NamespaceDefImpl::addListReferences()
{
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  {
    const RefItemVector &xrefItems = xrefListItems();
    addRefItem(xrefItems,
        qualifiedName(),
        getLanguage()==SrcLangExt_Fortran ?
          theTranslator->trModule(TRUE,TRUE) :
          theTranslator->trNamespace(TRUE,TRUE),
        getOutputFileBase(),displayName(),
        QCString(),
        this
        );
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

QCString NamespaceDefImpl::displayName(bool includeScope) const
{
  return makeDisplayName(this,includeScope);
}

void NamespaceDefImpl::combineUsingRelations(NamespaceDefSet &visitedNamespaces)
{
  if (visitedNamespaces.find(this)!=visitedNamespaces.end()) return; // already processed
  visitedNamespaces.insert(this);

  LinkedRefMap<NamespaceDef> usingDirList = m_usingDirList;
  for (auto &nd : usingDirList)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd);
    if (ndm)
    {
      ndm->combineUsingRelations(visitedNamespaces);
    }
  }

  for (auto &nd : usingDirList)
  {
    // add used namespaces of namespace nd to this namespace
    for (const auto &und : nd->getUsedNamespaces())
    {
      addUsingDirective(und);
    }
    // add used classes of namespace nd to this namespace
    for (const auto &ucd : nd->getUsedClasses())
    {
      addUsingDeclaration(ucd);
    }
  }
}

int NamespaceDefImpl::countVisibleMembers() const
{
  int count=0;
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    if (lde->kind()==LayoutDocEntry::MemberDef)
    {
      const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
      if (lmd)
      {
        MemberList *ml = getMemberList(lmd->type);
        if (ml)
        {
          for (const auto &md : *ml)
          {
            if (md->visibleInIndex())
            {
              count++;
            }
          }
        }
      }
    }
  }
  return count;
}



//-------------------------------------------------------------------------------

bool NamespaceLinkedRefMap::declVisible(bool isConstantGroup) const
{
  bool found=false;
  for (const auto &nd : *this)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (SrcLangExt_IDL==lang)
      {
        if (isConstantGroup == nd->isConstantGroup())
        {
          found=true;
          break;
        }
      }
      else if (!isConstantGroup) // ensure we only get extra section in IDL
      {
        if (nd->isConstantGroup())
        {
          err("Internal inconsistency: constant group but not IDL?\n");
        }
        found=true;
        break;
      }
    }
  }
  return found;
}

void NamespaceLinkedRefMap::writeDeclaration(OutputList &ol,const QCString &title,
        bool const isConstantGroup,bool localName)
{


  if (empty()) return; // no namespaces in the list

  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL)) return;

  if (!declVisible(isConstantGroup)) return;

  // write list of namespaces
  ol.startMemberHeader(isConstantGroup ? "constantgroups" : "namespaces");
  //bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  ol.parseText(title);
  ol.endMemberHeader();
  ol.startMemberList();
  for (const auto &nd : *this)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (lang==SrcLangExt_IDL && (isConstantGroup != nd->isConstantGroup()))
          continue; // will be output in another pass, see layout_default.xml
      ol.startMemberDeclaration();
      ol.startMemberItem(nd->getOutputFileBase(),OutputGenerator::MemberItemType::Normal);
      QCString ct = nd->compoundTypeString();
      ol.docify(ct);
      ol.docify(" ");
      ol.insertMemberAlign();
      QCString name;
      if (localName)
      {
        name = nd->localName();
      }
      else
      {
        name = nd->displayName();
      }
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),QCString(),name);
      ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
      if (!nd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(nd->getOutputFileBase());
        ol.generateDoc(nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription(),FALSE,FALSE,
                       QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(QCString(),QCString());
    }
  }
  ol.endMemberList();
}

//-------------------------------------------------------------------------------

void NamespaceDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt,MemberListContainer::Namespace);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->push_back(md);

  if (ml->listType()&MemberListType_declarationLists)
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->setSectionList(this,ml.get());
    }
  }
}

void NamespaceDefImpl::sortMemberLists()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }

  auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
  {
    return Config_getBool(SORT_BY_SCOPE_NAME)     ?
      qstricmp(c1->name(), c2->name())<0          :
      qstricmp(c1->className(), c2->className())<0;
  };

  std::sort(classes.begin(),   classes.end(),   classComp);
  std::sort(interfaces.begin(),interfaces.end(),classComp);
  std::sort(structs.begin(),   structs.end(),   classComp);
  std::sort(exceptions.begin(),exceptions.end(),classComp);


  auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
  {
    return qstricmp(n1->name(),n2->name())<0;
  };

  std::sort(namespaces.begin(),namespaces.end(),namespaceComp);
}

MemberList *NamespaceDefImpl::getMemberList(MemberListType lt) const
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()==lt)
    {
      return ml.get();
    }
  }
  return 0;
}

void NamespaceDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,0,this,0,0,title,QCString());
}

void NamespaceDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,displayName(),this,title);
}


bool NamespaceDefImpl::isLinkableInProject() const
{
  int i = name().findRev("::");
  if (i==-1) i=0; else i+=2;
  bool extractAnonNs = Config_getBool(EXTRACT_ANON_NSPACES);
  if (extractAnonNs &&                             // extract anonymous ns
      name().mid(i,20)=="anonymous_namespace{"     // correct prefix
     )                                             // not disabled by config
  {
    return TRUE;
  }
  return !name().isEmpty() && name().at(i)!='@' && // not anonymous
    (hasDocumentation() || getLanguage()==SrcLangExt_CSharp) &&  // documented
    !isReference() &&      // not an external reference
    !isHidden() &&         // not hidden
    !isArtificial();       // or artificial
}

bool NamespaceDefImpl::isLinkable() const
{
  return isLinkableInProject() || isReference();
}

const MemberDef * NamespaceDefImpl::getMemberByName(const QCString &n) const
{
  return m_allMembers.find(n);
}

QCString NamespaceDefImpl::title() const
{
  SrcLangExt lang = getLanguage();
  QCString pageTitle;
  if (lang==SrcLangExt_Java)
  {
    pageTitle = theTranslator->trPackage(displayName());
  }
  else if (lang==SrcLangExt_Fortran || lang==SrcLangExt_Slice)
  {
    pageTitle = theTranslator->trModuleReference(displayName());
  }
  else if (lang==SrcLangExt_IDL)
  {
    pageTitle = isConstantGroup()
        ? theTranslator->trConstantGroupReference(displayName())
        : theTranslator->trModuleReference(displayName());
  }
  else
  {
    pageTitle = theTranslator->trNamespaceReference(displayName());
  }
  return pageTitle;
}

QCString NamespaceDefImpl::compoundTypeString() const
{
  SrcLangExt lang = getLanguage();
  if (lang==SrcLangExt_Java)
  {
    return "package";
  }
  else if(lang==SrcLangExt_CSharp)
  {
    return "namespace";
  }
  else if (lang==SrcLangExt_Fortran)
  {
    return "module";
  }
  else if (lang==SrcLangExt_IDL)
  {
    if (isModule())
    {
      return "module";
    }
    else if (isConstantGroup())
    {
      return "constants";
    }
    else if (isLibrary())
    {
      return "library";
    }
    else
    {
      err_full(getDefFileName(),getDefLine(),"Internal inconsistency: namespace in IDL not module, library or constant group");
    }
  }
  return "namespace";
}

void NamespaceDefImpl::setMetaData(const QCString &m)
{
  metaData = m;
}

// --- Cast functions
//
NamespaceDef *toNamespaceDef(Definition *d)
{
  if (d && (typeid(*d)==typeid(NamespaceDefImpl) || typeid(*d)==typeid(NamespaceDefAliasImpl)))
  {
    return static_cast<NamespaceDef*>(d);
  }
  else
  {
    return 0;
  }
}

NamespaceDef *toNamespaceDef(DefinitionMutable *md)
{
  Definition *d = toDefinition(md);
  if (d && typeid(*d)==typeid(NamespaceDefImpl))
  {
    return static_cast<NamespaceDef*>(d);
  }
  else
  {
    return 0;
  }
}


const NamespaceDef *toNamespaceDef(const Definition *d)
{
  if (d && (typeid(*d)==typeid(NamespaceDefImpl) || typeid(*d)==typeid(NamespaceDefAliasImpl)))
  {
    return static_cast<const NamespaceDef*>(d);
  }
  else
  {
    return 0;
  }
}

NamespaceDefMutable *toNamespaceDefMutable(Definition *d)
{
  if (d && typeid(*d)==typeid(NamespaceDefImpl))
  {
    return static_cast<NamespaceDefMutable*>(d);
  }
  else
  {
    return 0;
  }
}

// --- Helpers


NamespaceDef *getResolvedNamespace(const QCString &name)
{
  if (name.isEmpty()) return 0;
  auto it = Doxygen::namespaceAliasMap.find(name.str());
  if (it!=Doxygen::namespaceAliasMap.end())
  {
    int count=0; // recursion detection guard
    StringUnorderedMap::iterator it2;
    while ((it2=Doxygen::namespaceAliasMap.find(it->second))!=Doxygen::namespaceAliasMap.end() &&
           count<10)
    {
      it=it2;
      count++;
    }
    if (count==10)
    {
      warn_uncond("possible recursive namespace alias detected for %s!\n",qPrint(name));
    }
    return Doxygen::namespaceLinkedMap->find(it->second);
  }
  else
  {
    return Doxygen::namespaceLinkedMap->find(name);
  }
}

//--------------------------------------------------------------------------------------
//
bool namespaceHasNestedNamespace(const NamespaceDef *nd)
{
  for (const auto &cnd : nd->getNamespaces())
  {
    if (cnd->isLinkableInProject() && !cnd->isAnonymous())
    {
      return true;
    }
  }
  return false;
}

bool namespaceHasNestedConcept(const NamespaceDef *nd)
{
  //printf(">namespaceHasNestedConcept(%s)\n",qPrint(nd->name()));
  for (const auto &cnd : nd->getNamespaces())
  {
    if (namespaceHasNestedConcept(cnd))
    {
      //printf("<namespaceHasNestedConcept(%s): case1\n",qPrint(nd->name()));
      return true;
    }
  }
  for (const auto &cnd : nd->getConcepts())
  {
    //printf("candidate %s isLinkableInProject()=%d\n",qPrint(cnd->name()),cnd->isLinkableInProject());
    if (cnd->isLinkableInProject())
    {
      //printf("<namespaceHasNestedConcept(%s): case2\n",qPrint(nd->name()));
      return true;
    }
  }
  //printf("<namespaceHasNestedConcept(%s): case3\n",qPrint(nd->name()));
  return false;
}

bool namespaceHasNestedClass(const NamespaceDef *nd,bool filterClasses,ClassDef::CompoundType ct)
{
  //printf(">namespaceHasNestedClass(%s,filterClasses=%d)\n",qPrint(nd->name()),filterClasses);
  for (const auto &cnd : nd->getNamespaces())
  {
    if (namespaceHasNestedClass(cnd,filterClasses,ct))
    {
      //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case1\n",qPrint(nd->name()),filterClasses);
      return true;
    }
  }

  ClassLinkedRefMap list = nd->getClasses();
  if (filterClasses)
  {
    switch (ct)
    {
      case ClassDef::Interface:
        list = nd->getInterfaces();
        break;
      case ClassDef::Struct:
        list = nd->getStructs();
        break;
      case ClassDef::Exception:
        list = nd->getExceptions();
        break;
      default:
        break;
    }
  }

  for (const auto &cd : list)
  {
    //printf("candidate %s isLinkableInProject()=%d\n",qPrint(cd->name()),cd->isLinkableInProject());
    if (cd->isLinkableInProject())
    {
      //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case2\n",qPrint(nd->name()),filterClasses);
      return true;
    }
  }
  //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case3\n",qPrint(nd->name()),filterClasses);
  return false;
}


