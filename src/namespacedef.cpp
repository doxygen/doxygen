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

//------------------------------------------------------------------

class NamespaceDefImpl : public DefinitionImpl, public NamespaceDef
{
  public:
    NamespaceDefImpl(const char *defFileName,int defLine,int defColumn,
                 const char *name,const char *ref=0,
                 const char *refFile=0,const char*type=0,
                 bool isPublished=false);
    virtual ~NamespaceDefImpl();
    virtual DefType definitionType() const { return TypeNamespace; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual void insertUsedFile(FileDef *fd);
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeMemberPages(OutputList &ol);
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const;
    virtual void writeTagFile(FTextStream &);
    virtual void insertClass(const ClassDef *cd);
    virtual void insertNamespace(const NamespaceDef *nd);
    virtual void insertMember(MemberDef *md);
    virtual void computeAnchors();
    virtual void countMembers();
    virtual int  numDocMembers() const;
    virtual void addUsingDirective(const NamespaceDef *nd);
    virtual const NamespaceSDict *getUsedNamespaces() const;
    virtual void addUsingDeclaration(const Definition *def);
    virtual const SDict<Definition> *getUsedClasses() const { return usingDeclList; }
    virtual void combineUsingRelations();
    virtual QCString displayName(bool=TRUE) const;
    virtual QCString localName() const;
    virtual void setInline(bool isInline) { m_inline = isInline; }
    virtual bool isConstantGroup() const { return CONSTANT_GROUP == m_type; }
    virtual bool isModule()        const { return MODULE == m_type; }
    virtual bool isLibrary() const { return LIBRARY == m_type; }
    virtual bool isInline() const { return m_inline; }
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual bool hasDetailedDescription() const;
    virtual void addMembersToMemberGroup();
    virtual void distributeMemberGroupDocumentation();
    virtual void findSectionsInDocumentation();
    virtual void sortMemberLists();
    virtual Definition *findInnerCompound(const char *name) const;
    virtual void addInnerCompound(const Definition *d);
    virtual void addListReferences();
    virtual void setFileName(const QCString &fn);
    virtual bool subGrouping() const { return m_subGrouping; }
    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const QList<MemberList> &getMemberLists() const { return m_memberLists; }
    virtual MemberDef    *getMemberByName(const QCString &) const;
    virtual MemberGroupSDict *getMemberGroupSDict() const { return memberGroupSDict; }
    virtual ClassSDict *getClassSDict() const { return classSDict; }
    virtual ClassSDict *getInterfaceSDict() const { return interfaceSDict; }
    virtual ClassSDict *getStructSDict() const { return structSDict; }
    virtual ClassSDict *getExceptionSDict() const { return exceptionSDict; }
    virtual const NamespaceSDict *getNamespaceSDict() const { return namespaceSDict; }

    virtual QCString title() const;
    virtual QCString compoundTypeString() const;

    virtual void setMetaData(const QCString &m);
    void setVisited(bool v) { m_visited = v; }
    bool isVisited() const { return m_visited; }

  private:
    bool m_visited;
    MemberList *createMemberList(MemberListType lt);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void writeClassDeclarations(OutputList &ol,const QCString &title,ClassSDict *d);
    void writeInlineClasses(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeSummaryLinks(OutputList &ol) const;
    void addNamespaceAttributes(OutputList &ol);
    void writeClassesToTagFile(FTextStream &,ClassSDict *d);

    void writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool isConstantGroup=false);

    QCString              fileName;
    FileList              files;

    NamespaceSDict       *usingDirList;
    SDict<Definition> *usingDeclList;
    SDict<Definition>    *m_innerCompounds;

    MemberSDict          *m_allMembersDict;
    QList<MemberList>     m_memberLists;
    MemberGroupSDict     *memberGroupSDict;
    ClassSDict           *classSDict;
    ClassSDict           *interfaceSDict;
    ClassSDict           *structSDict;
    ClassSDict           *exceptionSDict;
    NamespaceSDict       *namespaceSDict;
    bool                  m_subGrouping;
    enum { NAMESPACE, MODULE, CONSTANT_GROUP, LIBRARY } m_type;
    bool m_isPublished;
    QCString              metaData;
    bool                  m_inline;
};

NamespaceDef *createNamespaceDef(const char *defFileName,int defLine,int defColumn,
                 const char *name,const char *ref,
                 const char *refFile,const char*type,
                 bool isPublished)
{
  return new NamespaceDefImpl(defFileName,defLine,defColumn,name,ref,refFile,type,isPublished);
}

//------------------------------------------------------------------

class NamespaceDefAliasImpl : public DefinitionAliasImpl, public NamespaceDef
{
  public:
    NamespaceDefAliasImpl(const Definition *newScope,const NamespaceDef *nd) : DefinitionAliasImpl(newScope,nd) {}
    virtual ~NamespaceDefAliasImpl() {}
    virtual DefType definitionType() const { return TypeNamespace; }

    const NamespaceDef *getNSAlias() const { return dynamic_cast<const NamespaceDef*>(getAlias()); }

    // ---- getters
    virtual QCString getOutputFileBase() const
    { return getNSAlias()->getOutputFileBase(); }
    virtual QCString anchor() const
    { return getNSAlias()->anchor(); }
    virtual int numDocMembers() const
    { return getNSAlias()->numDocMembers(); }
    virtual void addUsingDirective(const NamespaceDef *nd) {}
    virtual const NamespaceSDict *getUsedNamespaces() const
    { return getNSAlias()->getUsedNamespaces(); }
    virtual void addUsingDeclaration(const Definition *def) {}
    virtual const SDict<Definition> *getUsedClasses() const
    { return getNSAlias()->getUsedClasses(); }
    virtual void combineUsingRelations() {}
    virtual QCString displayName(bool b=TRUE) const
    { return getNSAlias()->displayName(b); }
    virtual QCString localName() const
    { return getNSAlias()->localName(); }
    virtual void setInline(bool isInline) { }
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
    virtual Definition *findInnerCompound(const char *name) const
    { return getNSAlias()->findInnerCompound(name); }
    virtual bool subGrouping() const
    { return getNSAlias()->subGrouping(); }
    virtual MemberList *getMemberList(MemberListType lt) const
    { return getNSAlias()->getMemberList(lt); }
    virtual const QList<MemberList> &getMemberLists() const
    { return getNSAlias()->getMemberLists(); }
    virtual MemberDef    *getMemberByName(const QCString &name) const
    { return getNSAlias()->getMemberByName(name); }
    virtual MemberGroupSDict *getMemberGroupSDict() const
    { return getNSAlias()->getMemberGroupSDict(); }
    virtual ClassSDict *getClassSDict() const
    { return getNSAlias()->getClassSDict(); }
    virtual ClassSDict *getInterfaceSDict() const
    { return getNSAlias()->getInterfaceSDict(); }
    virtual ClassSDict *getStructSDict() const
    { return getNSAlias()->getStructSDict(); }
    virtual ClassSDict *getExceptionSDict() const
    { return getNSAlias()->getExceptionSDict(); }
    virtual const NamespaceSDict *getNamespaceSDict() const
    { return getNSAlias()->getNamespaceSDict(); }
    virtual QCString title() const
    { return getNSAlias()->title(); }
    virtual QCString compoundTypeString() const
    { return getNSAlias()->compoundTypeString(); }

    // --- setters/actions
    virtual void setMetaData(const QCString &m) {}
    virtual void insertUsedFile(FileDef *fd) { }
    virtual void writeDocumentation(OutputList &ol) {}
    virtual void writeMemberPages(OutputList &ol) {}
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const {}
    virtual void writeTagFile(FTextStream &) {}
    virtual void insertClass(const ClassDef *cd) {}
    virtual void insertNamespace(const NamespaceDef *nd) {}
    virtual void insertMember(MemberDef *md) {}
    virtual void computeAnchors() {}
    virtual void countMembers() {}
    virtual void addMembersToMemberGroup() {}
    virtual void distributeMemberGroupDocumentation() {}
    virtual void findSectionsInDocumentation() {}
    virtual void sortMemberLists() {}
    virtual void addInnerCompound(const Definition *d) {}
    virtual void addListReferences() {}
    virtual void setFileName(const QCString &fn) {}

    void setVisited(bool v) { m_visited = v; }
    bool isVisited() const { return m_visited; }

  private:
    bool m_visited;
};

NamespaceDef *createNamespaceDefAlias(const Definition *newScope,const NamespaceDef *nd)
{
  return new NamespaceDefAliasImpl(newScope,nd);
}

//------------------------------------------------------------------

NamespaceDefImpl::NamespaceDefImpl(const char *df,int dl,int dc,
                           const char *name,const char *lref,
                           const char *fName, const char*type,
                           bool isPublished) :
   DefinitionImpl(df,dl,dc,name)
  ,m_isPublished(isPublished)
{
  if (fName)
  {
    if (lref)
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
  classSDict = new ClassSDict(17);
  interfaceSDict = new ClassSDict(17);
  structSDict = new ClassSDict(17);
  exceptionSDict = new ClassSDict(17);
  namespaceSDict = new NamespaceSDict(17);
  m_innerCompounds = new SDict<Definition>(17);
  usingDirList = 0;
  usingDeclList = 0;
  m_allMembersDict = 0;
  setReference(lref);
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);
  m_visited=FALSE;
  m_inline=FALSE;
  m_subGrouping=Config_getBool(SUBGROUPING);
  if (type && !strcmp("module", type))
  {
    m_type = MODULE;
  }
  else if (type && !strcmp("constants", type))
  {
    m_type = CONSTANT_GROUP;
  }
  else if (type && !strcmp("library", type))
  {
    m_type = LIBRARY;
  }
  else
  {
    m_type = NAMESPACE;
  }
}

NamespaceDefImpl::~NamespaceDefImpl()
{
  delete classSDict;
  delete interfaceSDict;
  delete structSDict;
  delete exceptionSDict;
  delete namespaceSDict;
  delete m_innerCompounds;
  delete usingDirList;
  delete usingDeclList;
  delete memberGroupSDict;
  delete m_allMembersDict;
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
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void NamespaceDefImpl::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->findSectionsInDocumentation();
  }
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ml->findSectionsInDocumentation();
    }
  }
}

void NamespaceDefImpl::insertUsedFile(FileDef *fd)
{
  if (fd==0) return;
  if (files.find(fd)==-1) 
  {
    if (Config_getBool(SORT_MEMBER_DOCS))
      files.inSort(fd);
    else
      files.append(fd);
  }
}

void NamespaceDefImpl::addInnerCompound(const Definition *d)
{
  m_innerCompounds->append(d->localName(),d);
  if (d->definitionType()==Definition::TypeNamespace)
  {
    insertNamespace(dynamic_cast<const NamespaceDef *>(d));
  }
  else if (d->definitionType()==Definition::TypeClass)
  {
    insertClass(dynamic_cast<const ClassDef *>(d));
  }
}

void NamespaceDefImpl::insertClass(const ClassDef *cd)
{
  ClassSDict *d = classSDict;

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      d = interfaceSDict;
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      d = structSDict;
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      d = exceptionSDict;
    }
  }

  if (d->find(cd->name())==0)
  {
    if (Config_getBool(SORT_BRIEF_DOCS))
    {
      d->inSort(cd->name(),cd);
    }
    else
    {
      d->append(cd->name(),cd);
    }
  }
}

void NamespaceDefImpl::insertNamespace(const NamespaceDef *nd)
{
  if (namespaceSDict->find(nd->name())==0)
  {
    if (Config_getBool(SORT_MEMBER_DOCS))
      namespaceSDict->inSort(nd->name(),nd);
    else
      namespaceSDict->append(nd->name(),nd);
  }
}


void NamespaceDefImpl::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ::addMembersToMemberGroup(ml,&memberGroupSDict,this);
    }
  }

  // add members inside sections to their groups
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if (mg->allMembersInSameSection() && m_subGrouping)
      {
        //printf("----> addToDeclarationSection(%s)\n",mg->header().data());
        mg->addToDeclarationSection();
      }
    }
  }
}

void NamespaceDefImpl::insertMember(MemberDef *md)
{
  //printf("%s::insertMember(%s)\n",qPrint(name()),qPrint(md->name()));
  if (md->isHidden()) return;
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList==0)
  {
    allMemberList = new MemberList(MemberListType_allMembersList);
    m_memberLists.append(allMemberList);
  }
  allMemberList->append(md); 
  if (m_allMembersDict==0)
  {
    m_allMembersDict = new MemberSDict;
  }
  //printf("%s::m_allMembersDict->append(%s)\n",name().data(),md->localName().data());
  m_allMembersDict->append(md->localName(),md); 
  //::addNamespaceMemberNameToIndex(md);
  //static bool sortBriefDocs=Config_getBool(SORT_BRIEF_DOCS);
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
           "member '%s' with class scope '%s' inserted in namespace scope '%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
  // if this is an inline namespace, then insert an alias of this member in the outer scope.
  if (isInline())
  {
    Definition *outerScope = getOuterScope();
    if (outerScope)
    {
      MemberDef *aliasMd = 0;
      if (outerScope->definitionType()==Definition::TypeNamespace)
      {
        aliasMd = createMemberDefAlias(outerScope,md);
        dynamic_cast<NamespaceDef*>(outerScope)->insertMember(aliasMd);
      }
      else if (outerScope->definitionType()==Definition::TypeFile)
      {
        aliasMd = createMemberDefAlias(outerScope,md);
        dynamic_cast<FileDef*>(outerScope)->insertMember(aliasMd);
      }
      if (aliasMd)
      {
        MemberName *mn;
        QCString name = md->name();
        if ((mn=Doxygen::functionNameSDict->find(name)))
        {
          mn->append(aliasMd);
        }
        else
        {
          mn = new MemberName(name);
          mn->append(aliasMd);
          Doxygen::functionNameSDict->append(name,mn);
        }
      }
    }
  }
}

void NamespaceDefImpl::computeAnchors()
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList) setAnchors(allMemberList);
}

bool NamespaceDefImpl::hasDetailedDescription() const
{
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().isEmpty());
}

void NamespaceDefImpl::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"namespace\">" << endl;
  tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>" << endl;
  }
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::NamespaceNestedNamespaces:
        {
          if (namespaceSDict)
          {
            SDict<NamespaceDef>::Iterator ni(*namespaceSDict);
            NamespaceDef *nd;
            for (ni.toFirst();(nd=ni.current());++ni)
            {
              if (nd->isLinkableInProject())
              {
                tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::NamespaceClasses:
        {
          if (classSDict)
            writeClassesToTagFile(tagFile, classSDict);
        }
        break;
      case LayoutDocEntry::NamespaceInterfaces:
        {
          if (interfaceSDict)
            writeClassesToTagFile(tagFile, interfaceSDict);
        }
        break;
      case LayoutDocEntry::NamespaceStructs:
        {
          if (structSDict)
            writeClassesToTagFile(tagFile, structSDict);
        }
        break;
      case LayoutDocEntry::NamespaceExceptions:
        {
          if (exceptionSDict)
            writeClassesToTagFile(tagFile, exceptionSDict);
        }
        break;
      case LayoutDocEntry::MemberDecl:
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          MemberList * ml = getMemberList(lmd->type);
          if (ml)
          {
            ml->writeTagFile(tagFile);
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          if (memberGroupSDict)
          {
            MemberGroupSDict::Iterator mgli(*memberGroupSDict);
            MemberGroup *mg;
            for (;(mg=mgli.current());++mgli)
            {
              mg->writeTagFile(tagFile);
            }
          }
        }
        break;
      default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>" << endl;
}

void NamespaceDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.writeRuler();
    ol.popGeneratorState();
    ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeAnchor(0,"details"); 
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();

    ol.startTextBlock();
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.disable(OutputGenerator::RTF);
        //ol.newParagraph(); // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.enable(OutputGenerator::Latex);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
    ol.endTextBlock();
  }
}

void NamespaceDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),this,0,
                        briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(rootNode,this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputGenerator::Html);
        ol.startTextLink(getOutputFileBase(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
    delete rootNode;

    // FIXME:PARA
    //ol.pushGeneratorState();
    //ol.disable(OutputGenerator::RTF);
    //ol.newParagraph();
    //ol.popGeneratorState();
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
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void NamespaceDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void NamespaceDefImpl::writeClassDeclarations(OutputList &ol,const QCString &title,ClassSDict *d)
{
  if (d) d->writeDeclaration(ol,0,title,TRUE);
}

void NamespaceDefImpl::writeInlineClasses(OutputList &ol)
{
  if (classSDict) classSDict->writeDocumentation(ol,this);
}

void NamespaceDefImpl::writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool const isConstantGroup)
{
  if (namespaceSDict) namespaceSDict->writeDeclaration(ol,title,isConstantGroup,TRUE);
}

void NamespaceDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  if (memberGroupSDict)
  {
    memberGroupSDict->sort();
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if ((!mg->allMembersInSameSection() || !m_subGrouping) 
          && mg->header()!="[NOHEADER]")
      {
        mg->writeDeclarations(ol,0,this,0,0);
      }
    }
  }
}
  
void NamespaceDefImpl::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void NamespaceDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace));
  LayoutDocEntry *lde;
  bool first=TRUE;
  SrcLangExt lang = getLanguage();
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if (lde->kind()==LayoutDocEntry::NamespaceClasses && classSDict && classSDict->declVisible())
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = "nested-classes";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceInterfaces && interfaceSDict && interfaceSDict->declVisible())
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = "interfaces";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceStructs && structSDict && structSDict->declVisible())
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = "structs";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceExceptions && exceptionSDict && exceptionSDict->declVisible())
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = "exceptions";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceNestedNamespaces && namespaceSDict && namespaceSDict->declVisible())
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = "namespaces";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()== LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        ol.writeSummaryLink(0,MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
        first=FALSE;
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
    ol.disableAllBut(OutputGenerator::Html);
    ol.startLabels();
    ol.writeLabel("published",false);
    ol.endLabels();
    ol.popGeneratorState();
  }
}

void NamespaceDefImpl::writeClassesToTagFile(FTextStream &tagFile,ClassSDict *d)
{
  SDict<ClassDef>::Iterator ci(*d);
  ClassDef *cd;
  for (ci.toFirst();(cd=ci.current());++ci)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <class kind=\"" << cd->compoundTypeString()
              << "\">" << convertToXML(cd->name()) << "</class>" << endl;
    }
  }
}

void NamespaceDefImpl::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //static bool outputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);

  QCString pageTitle = title();
  startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_NamespaceVisible,!generateTreeView);

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
  
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }

  Doxygen::indexList->addIndexItem(this,0);

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
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
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClassDeclarations(ol,ls->title(lang),classSDict);
        }
        break; 
      case LayoutDocEntry::NamespaceInterfaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClassDeclarations(ol,ls->title(lang),interfaceSDict);
        }
        break; 
      case LayoutDocEntry::NamespaceStructs: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClassDeclarations(ol,ls->title(lang),structSDict);
        }
        break; 
      case LayoutDocEntry::NamespaceExceptions: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClassDeclarations(ol,ls->title(lang),exceptionSDict);
        }
        break; 
      case LayoutDocEntry::NamespaceNestedNamespaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaceDeclarations(ol,ls->title(lang),false);
        }
        break; 
      case LayoutDocEntry::NamespaceNestedConstantGroups:
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaceDeclarations(ol,ls->title(lang),true);
        }
        break;
      case LayoutDocEntry::MemberGroups: 
        writeMemberGroups(ol);
        break; 
      case LayoutDocEntry::MemberDecl: 
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break; 
      case LayoutDocEntry::MemberDeclEnd: 
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDetailedDescription(ol,ls->title(lang));
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
          LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
          writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
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
      case LayoutDocEntry::FileClasses:
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

  endFileWithNavPath(this,ol);

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
  ol.disableAllBut(OutputGenerator::Html);

  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
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
  static bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList)
  {
    MemberListIterator mli(*allMemberList);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      if (md->getNamespaceDef()==this && md->isLinkable() && !md->isEnumValue())
      {
        ol.writeString("          <tr><td class=\"navtab\">");
        if (md->isLinkableInProject())
        {
          if (md==currentMd) // selected item => highlight
          {
            ol.writeString("<a class=\"qindexHL\" ");
          }
          else
          {
            ol.writeString("<a class=\"qindex\" ");
          }
          ol.writeString("href=\"");
          if (createSubDirs) ol.writeString("../../");
          ol.writeString(md->getOutputFileBase()+Doxygen::htmlFileExtension+"#"+md->anchor());
          ol.writeString("\">");
          ol.writeString(convertToHtml(md->localName()));
          ol.writeString("</a>");
        }
        ol.writeString("</td></tr>\n");
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}

void NamespaceDefImpl::countMembers()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDecMembers();
      mg->countDocMembers();
    }
  }
}

int NamespaceDefImpl::numDocMembers() const
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  return (allMemberList ? allMemberList->numDocMembers() : 0) + m_innerCompounds->count();
}

void NamespaceDefImpl::addUsingDirective(const NamespaceDef *nd)
{
  if (usingDirList==0)
  {
    usingDirList = new NamespaceSDict;
  }
  if (usingDirList->find(nd->qualifiedName())==0)
  {
    usingDirList->append(nd->qualifiedName(),nd);
  }
  //printf("%p: NamespaceDefImpl::addUsingDirective: %s:%d\n",this,name().data(),usingDirList->count());
}

const NamespaceSDict *NamespaceDefImpl::getUsedNamespaces() const 
{ 
  //printf("%p: NamespaceDefImpl::getUsedNamespace: %s:%d\n",this,name().data(),usingDirList?usingDirList->count():0);
  return usingDirList; 
}

void NamespaceDefImpl::addUsingDeclaration(const Definition *d)
{
  if (usingDeclList==0)
  {
    usingDeclList = new SDict<Definition>(17);
  }
  if (usingDeclList->find(d->qualifiedName())==0)
  {
    usingDeclList->append(d->qualifiedName(),d);
  }
}

QCString NamespaceDefImpl::getOutputFileBase() const
{
  return fileName;
}

Definition *NamespaceDefImpl::findInnerCompound(const char *n) const
{
  if (n==0) return 0;
  Definition *d = m_innerCompounds->find(n);
  if (d==0)
  {
    if (usingDirList)
    {
      d = usingDirList->find(n);
    }
    if (d==0 && usingDeclList)
    {
      d = usingDeclList->find(n);
    }
  }
  return d;
}

void NamespaceDefImpl::addListReferences()
{
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  {
    QList<ListItemInfo> *xrefItems = xrefListItems();
    addRefItem(xrefItems,
        qualifiedName(),
        getLanguage()==SrcLangExt_Fortran ? 
          theTranslator->trModule(TRUE,TRUE) : 
          theTranslator->trNamespace(TRUE,TRUE),
        getOutputFileBase(),displayName(),
        0,
        this
        );
  }
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->addListReferences(this);
  }
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

QCString NamespaceDefImpl::displayName(bool includeScope) const
{
  QCString result=includeScope ? name() : localName();
  SrcLangExt lang = getLanguage();
  QCString sep = getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    result = substitute(result,"::",sep);
  }
  //printf("NamespaceDefImpl::displayName() %s->%s lang=%d\n",name().data(),result.data(),lang);
  return result; 
}

QCString NamespaceDefImpl::localName() const
{
  QCString result=name();
  int i=result.findRev("::");
  if (i!=-1)
  {
    result=result.mid(i+2);
  }
  return result;
}

void NamespaceDefImpl::combineUsingRelations()
{
  if (m_visited) return; // already done
  m_visited=TRUE;
  if (usingDirList)
  {
    NamespaceSDict::Iterator nli(*usingDirList);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      nd->combineUsingRelations();
    }
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      // add used namespaces of namespace nd to this namespace
      if (nd->getUsedNamespaces())
      {
        NamespaceSDict::Iterator unli(*nd->getUsedNamespaces());
        NamespaceDef *und;
        for (unli.toFirst();(und=unli.current());++unli)
        {
          //printf("Adding namespace %s to the using list of %s\n",und->qualifiedName().data(),qualifiedName().data());
          addUsingDirective(und);
        }
      }
      // add used classes of namespace nd to this namespace
      if (nd->getUsedClasses())
      {
        SDict<Definition>::Iterator cli(*nd->getUsedClasses());
        Definition *ucd;
        for (cli.toFirst();(ucd=cli.current());++cli)
        {
          //printf("Adding class %s to the using list of %s\n",cd->qualifiedName().data(),qualifiedName().data());
          addUsingDeclaration(ucd);
        }
      }
    }
  }
}

bool NamespaceSDict::declVisible() const
{
  SDict<NamespaceDef>::Iterator ni(*this);
  NamespaceDef *nd;
  for (ni.toFirst();(nd=ni.current());++ni)
  {
    if (nd->isLinkable())
    {
      return TRUE;
    }
  }
  return FALSE;
}

void NamespaceSDict::writeDeclaration(OutputList &ol,const char *title,
        bool const isConstantGroup,bool localName)
{
 

  if (count()==0) return; // no namespaces in the list

  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL)) return;
 

  SDict<NamespaceDef>::Iterator ni(*this);
  NamespaceDef *nd;
  bool found=FALSE;
  for (ni.toFirst();(nd=ni.current()) && !found;++ni)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (SrcLangExt_IDL==lang)
      {
        if (isConstantGroup == nd->isConstantGroup())
        {
          found=TRUE;
          break;
        }
      }
      else if (!isConstantGroup) // ensure we only get extra section in IDL
      {
        if (nd->isConstantGroup())
        {
          err("Internal inconsistency: constant group but not IDL?\n");
        }
        found=TRUE;
        break;
      }
    }
  }
  if (!found) return; // no linkable namespaces in the list

  // write list of namespaces
  ol.startMemberHeader("namespaces");
  //bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  ol.parseText(title);
  ol.endMemberHeader();
  ol.startMemberList();
  for (ni.toFirst();(nd=ni.current());++ni)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (lang==SrcLangExt_IDL && (isConstantGroup != nd->isConstantGroup()))
          continue; // will be output in another pass, see layout_default.xml
      ol.startMemberDeclaration();
      ol.startMemberItem(nd->getOutputFileBase(),0);
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
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),0,name);
      ol.endMemberItem();
      if (!nd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(nd->getOutputFileBase());
        ol.generateDoc(nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription(),FALSE,FALSE,0,TRUE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
    }
  }
  ol.endMemberList();
}

MemberList *NamespaceDefImpl::createMemberList(MemberListType lt)
{
  m_memberLists.setAutoDelete(TRUE);
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  // not found, create a new member list
  ml = new MemberList(lt);
  m_memberLists.append(ml);
  return ml;
}

void NamespaceDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  static bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->append(md);

  if (ml->listType()&MemberListType_declarationLists)
  {
    md->setSectionList(this,ml);
  }
}

void NamespaceDefImpl::sortMemberLists()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }
  if (classSDict)
  {
    classSDict->sort();
  }
  if (interfaceSDict)
  {
    interfaceSDict->sort();
  }
  if (structSDict)
  {
    structSDict->sort();
  }
  if (exceptionSDict)
  {
    exceptionSDict->sort();
  }
  if (namespaceSDict)
  {
    namespaceSDict->sort();
  }
}

MemberList *NamespaceDefImpl::getMemberList(MemberListType lt) const
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  return 0;
}

void NamespaceDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,0,this,0,0,title,0);
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
  static bool extractAnonNs = Config_getBool(EXTRACT_ANON_NSPACES);
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

MemberDef * NamespaceDefImpl::getMemberByName(const QCString &n) const
{
  MemberDef *md = 0;
  if (m_allMembersDict && !n.isEmpty())
  {
    md = m_allMembersDict->find(n);
    //printf("%s::m_allMembersDict->find(%s)=%p\n",name().data(),n.data(),md);
  }
  return md;
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
  return "";
}

void NamespaceDefImpl::setMetaData(const QCString &m)
{
  metaData = m;
}
