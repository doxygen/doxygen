/******************************************************************************
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

//------------------------------------------------------------------

NamespaceDef::NamespaceDef(const char *df,int dl,int dc,
                           const char *name,const char *lref,
                           const char *fName) : 
   Definition(df,dl,dc,name)
{
  if (fName)
  {
    fileName = stripExtension(fName);
  }
  else
  {
    fileName="namespace";
    fileName+=name;
  }
  classSDict = new ClassSDict(17);
  namespaceSDict = new NamespaceSDict(17);
  m_innerCompounds = new SDict<Definition>(17);
  usingDirList = 0;
  usingDeclList = 0;
  m_allMembersDict = 0;
  setReference(lref);
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);
  visited=FALSE;
  m_subGrouping=Config_getBool("SUBGROUPING");
}

NamespaceDef::~NamespaceDef()
{
  delete classSDict;
  delete namespaceSDict;
  delete m_innerCompounds;
  delete usingDirList;
  delete usingDeclList;
  delete memberGroupSDict;
  delete m_allMembersDict;
}

void NamespaceDef::distributeMemberGroupDocumentation()
{
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void NamespaceDef::findSectionsInDocumentation()
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

void NamespaceDef::insertUsedFile(const char *f)
{
  if (files.find(f)==-1) 
  {
    if (Config_getBool("SORT_MEMBER_DOCS"))
      files.inSort(f);
    else
      files.append(f);
  }
}

void NamespaceDef::addInnerCompound(Definition *d)
{
  m_innerCompounds->append(d->localName(),d);
  if (d->definitionType()==Definition::TypeNamespace)
  {
    insertNamespace((NamespaceDef *)d);
  }
  else if (d->definitionType()==Definition::TypeClass)
  {
    insertClass((ClassDef *)d);
  }
}

void NamespaceDef::insertClass(ClassDef *cd)
{
  if (classSDict->find(cd->name())==0)
  {
    if (Config_getBool("SORT_BRIEF_DOCS"))
      classSDict->inSort(cd->name(),cd);
    else
      classSDict->append(cd->name(),cd);
  }
}

void NamespaceDef::insertNamespace(NamespaceDef *nd)
{
  if (namespaceSDict->find(nd->name())==0)
  {
    if (Config_getBool("SORT_MEMBER_DOCS"))
      namespaceSDict->inSort(nd->name(),nd);
    else
      namespaceSDict->append(nd->name(),nd);
  }
}


void NamespaceDef::addMembersToMemberGroup()
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

void NamespaceDef::insertMember(MemberDef *md)
{
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
  //static bool sortBriefDocs=Config_getBool("SORT_BRIEF_DOCS");
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
      err("NamespaceDef::insertMembers(): "
           "member `%s' with class scope `%s' inserted in namespace scope `%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
}

void NamespaceDef::computeAnchors()
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList) setAnchors(allMemberList);
}

void NamespaceDef::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().isEmpty()
     )
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
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF") &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.disable(OutputGenerator::RTF);
        //ol.newParagraph(); // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
    ol.endTextBlock();
  }
}

void NamespaceDef::writeBriefDescription(OutputList &ol)
{
  if (!briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
  {
    ol.startParagraph();
    ol.parseDoc(briefFile(),briefLine(),this,0,
                briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::RTF);
    ol.writeString(" \n");
    ol.enable(OutputGenerator::RTF);

    if (Config_getBool("REPEAT_BRIEF") ||
        !documentation().isEmpty()
       )
    {
      ol.disableAllBut(OutputGenerator::Html);
      ol.startTextLink(0,"details");
      ol.parseText(theTranslator->trMore());
      ol.endTextLink();
    }
    ol.popGeneratorState();
    ol.endParagraph();

    // FIXME:PARA
    //ol.pushGeneratorState();
    //ol.disable(OutputGenerator::RTF);
    //ol.newParagraph();
    //ol.popGeneratorState();
  }
  ol.writeSynopsis();
}

void NamespaceDef::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void NamespaceDef::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void NamespaceDef::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void NamespaceDef::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void NamespaceDef::writeClassDeclarations(OutputList &ol,const QCString &title)
{
  if (classSDict) classSDict->writeDeclaration(ol,0,title,TRUE);
}

void NamespaceDef::writeInlineClasses(OutputList &ol)
{
  if (classSDict) classSDict->writeDocumentation(ol,this);
}

void NamespaceDef::writeNamespaceDeclarations(OutputList &ol,const QCString &title)
{
  if (namespaceSDict) namespaceSDict->writeDeclaration(ol,title,TRUE);
}

void NamespaceDef::writeMemberGroups(OutputList &ol)
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
  
void NamespaceDef::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));
  ol.popGeneratorState();
}

void NamespaceDef::writeSummaryLinks(OutputList &ol)
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
    if ((lde->kind()==LayoutDocEntry::NamespaceClasses && classSDict && classSDict->declVisible()) || 
        (lde->kind()==LayoutDocEntry::NamespaceNestedNamespaces && namespaceSDict && namespaceSDict->declVisible())
       )
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = lde->kind()==LayoutDocEntry::NamespaceClasses ? "nested-classes" : "namespaces";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()== LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        ol.writeSummaryLink(0,ml->listTypeAsString(),lmd->title(lang),first);
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

void NamespaceDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  //static bool outputJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  //static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  SrcLangExt lang = getLanguage();

  QCString pageTitle;
  if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
  {
    pageTitle = theTranslator->trPackage(displayName());
  }
  else if (lang==SrcLangExt_Fortran)
  {
    pageTitle = theTranslator->trModuleReference(displayName());
  }
  else
  {
    pageTitle = theTranslator->trNamespaceReference(displayName());
  }
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
  endTitle(ol,getOutputFileBase(),displayName());
  ol.startContents();
  
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }

  bool generateTagFile = !Config_getString("GENERATE_TAGFILE").isEmpty();
  if (generateTagFile)
  {
    Doxygen::tagFile << "  <compound kind=\"namespace\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }

  Doxygen::indexList->addIndexItem(this,0);

  //---------------------------------------- start flexible part -------------------------------

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
          writeClassDeclarations(ol,ls->title(lang));
        }
        break; 
      case LayoutDocEntry::NamespaceNestedNamespaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaceDeclarations(ol,ls->title(lang));
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
      case LayoutDocEntry::FileNamespaces:
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

  if (generateTagFile)
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
    if (allMemberList) allMemberList->sort();
    writeMemberPages(ol);
  }
}

void NamespaceDef::writeMemberPages(OutputList &ol)
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

void NamespaceDef::writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool("CREATE_SUBDIRS");

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList)
  {
    MemberListIterator mli(*allMemberList);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      if (md->getNamespaceDef()==this && md->isLinkable())
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

int NamespaceDef::countMembers()
{
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList) allMemberList->countDocMembers();
  return (allMemberList ? allMemberList->numDocMembers() : 0)+classSDict->count();
}

void NamespaceDef::addUsingDirective(NamespaceDef *nd)
{
  if (usingDirList==0)
  {
    usingDirList = new NamespaceSDict;
  }
  if (usingDirList->find(nd->qualifiedName())==0)
  {
    usingDirList->append(nd->qualifiedName(),nd);
  }
  //printf("%p: NamespaceDef::addUsingDirective: %s:%d\n",this,name().data(),usingDirList->count());
}

NamespaceSDict *NamespaceDef::getUsedNamespaces() const 
{ 
  //printf("%p: NamespaceDef::getUsedNamespace: %s:%d\n",this,name().data(),usingDirList?usingDirList->count():0);
  return usingDirList; 
}

void NamespaceDef::addUsingDeclaration(Definition *d)
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

QCString NamespaceDef::getOutputFileBase() const 
{ 
  if (isReference())
  {
    return fileName;
  }
  else
  {
    return convertNameToFile(fileName); 
  }
}

Definition *NamespaceDef::findInnerCompound(const char *n)
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

void NamespaceDef::addListReferences()
{
  //bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
        qualifiedName(),
        getLanguage()==SrcLangExt_Fortran ? 
          theTranslator->trModule(TRUE,TRUE) : 
          theTranslator->trNamespace(TRUE,TRUE),
        getOutputFileBase(),displayName(),
        0
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

QCString NamespaceDef::displayName(bool includeScope) const
{
  QCString result=includeScope ? name() : localName();
  SrcLangExt lang = getLanguage();
  QCString sep = getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    result = substitute(result,"::",sep);
  }
  //printf("NamespaceDef::displayName() %s->%s lang=%d\n",name().data(),result.data(),lang);
  return result; 
}

QCString NamespaceDef::localName() const
{
  QCString result=name();
  int i=result.findRev("::");
  if (i!=-1)
  {
    result=result.mid(i+2);
  }
  return result;
}

void NamespaceDef::combineUsingRelations()
{
  if (visited) return; // already done
  visited=TRUE;
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

void NamespaceSDict::writeDeclaration(OutputList &ol,const char *title,bool localName)
{
 

  if (count()==0) return; // no namespaces in the list

  if (Config_getBool("OPTIMIZE_OUTPUT_VHDL")) return;
 

  SDict<NamespaceDef>::Iterator ni(*this);
  NamespaceDef *nd;
  bool found=FALSE;
  for (ni.toFirst();(nd=ni.current()) && !found;++ni)
  {
    if (nd->isLinkable()) found=TRUE;
  }
  if (!found) return; // no linkable namespaces in the list

  // write list of namespaces
  ol.startMemberHeader("namespaces");
  //bool javaOpt    = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  //bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  ol.parseText(title);
  ol.endMemberHeader();
  ol.startMemberList();
  for (ni.toFirst();(nd=ni.current());++ni)
  {
    if (nd->isLinkable())
    {
      SrcLangExt lang = nd->getLanguage();
      ol.startMemberDeclaration();
      ol.startMemberItem(nd->getOutputFileBase(),0);
      if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
      {
        ol.docify("package ");
      }
      else if (lang==SrcLangExt_Fortran)
      {
        ol.docify("module ");
      }
      else
      {
        ol.docify("namespace ");
      }
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty() && nd->isLinkableInProject()) 
      {
        Doxygen::tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
      }
      ol.endMemberItem();
      if (!nd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription(nd->getOutputFileBase());
        ol.parseDoc(nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription(),FALSE,FALSE,0,TRUE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
    }
  }
  ol.endMemberList();
}

MemberList *NamespaceDef::createMemberList(MemberListType lt)
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

void NamespaceDef::addMemberToList(MemberListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  static bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->append(md);

#if 0
  if (ml->needsSorting())
    ml->inSort(md);
  else
    ml->append(md);
#endif

  if (ml->listType()&MemberListType_declarationLists) md->setSectionList(this,ml);
}

void NamespaceDef::sortMemberLists()
{
  MemberList *ml = m_memberLists.first();
  while (ml)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
    ml = m_memberLists.next();
  }
}



MemberList *NamespaceDef::getMemberList(MemberListType lt) const
{
  NamespaceDef *that = (NamespaceDef*)this;
  MemberList *ml = that->m_memberLists.first();
  while (ml)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
    ml = that->m_memberLists.next();
  }
  return 0;
}

void NamespaceDef::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,0,this,0,0,title,0);
}

void NamespaceDef::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,displayName(),this,title);
}


bool NamespaceDef::isLinkableInProject() const
{
  int i = name().findRev("::");
  if (i==-1) i=0; else i+=2;
  static bool extractAnonNs = Config_getBool("EXTRACT_ANON_NSPACES");
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

bool NamespaceDef::isLinkable() const
{
  return isLinkableInProject() || isReference();
}

MemberDef * NamespaceDef::getMemberByName(const QCString &n) const
{
  MemberDef *md = 0;
  if (m_allMembersDict && !n.isEmpty())
  {
    md = m_allMembersDict->find(n);
    //printf("%s::m_allMembersDict->find(%s)=%p\n",name().data(),n.data(),md);
  }
  return md;
}

