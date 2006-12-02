/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

#include "qtbc.h"
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

NamespaceDef::NamespaceDef(const char *df,int dl,
                           const char *name,const char *lref,
                           const char *fName) : 
   Definition(df,dl,name)
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
    if (ml->listType()&MemberList::declarationLists)
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
    if (ml->listType()&MemberList::declarationLists)
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
  MemberList *allMemberList = getMemberList(MemberList::allMembersList);
  if (allMemberList==0)
  {
    allMemberList = new MemberList(MemberList::allMembersList);
    m_memberLists.append(allMemberList);
  }
  allMemberList->append(md); 
  ::addNamespaceMemberNameToIndex(md);
  //static bool sortBriefDocs=Config_getBool("SORT_BRIEF_DOCS");
  switch(md->memberType())
  {
    case MemberDef::Variable:     
      addMemberToList(MemberList::decVarMembers,md);
      addMemberToList(MemberList::docVarMembers,md);
      break;
    case MemberDef::Function: 
      addMemberToList(MemberList::decFuncMembers,md);
      addMemberToList(MemberList::docFuncMembers,md);
      break;
    case MemberDef::Typedef:      
      addMemberToList(MemberList::decTypedefMembers,md);
      addMemberToList(MemberList::docTypedefMembers,md);
      break;
    case MemberDef::Enumeration:  
      addMemberToList(MemberList::decEnumMembers,md);
      addMemberToList(MemberList::docEnumMembers,md);
      break;
    case MemberDef::EnumValue:    
      break;
    case MemberDef::Prototype:    
      addMemberToList(MemberList::decProtoMembers,md);
      addMemberToList(MemberList::docProtoMembers,md);
      break;
    case MemberDef::Define:       
      addMemberToList(MemberList::decDefineMembers,md);
      addMemberToList(MemberList::docDefineMembers,md);
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
  MemberList *allMemberList = getMemberList(MemberList::allMembersList);
  if (allMemberList) setAnchors(0,'a',allMemberList);
}

void NamespaceDef::writeDetailedDocumentation(OutputList &ol)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.writeAnchor(0,"_details"); 
    //if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(theTranslator->trDetailedDescription());
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
        ol.newParagraph();
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
      ol.newParagraph();
    }
    ol.endTextBlock();
  }
}
  
void NamespaceDef::writeDocumentation(OutputList &ol)
{
  QCString pageTitle;
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    pageTitle = theTranslator->trPackage(displayName());
  }
  else
  {
    pageTitle = theTranslator->trNamespaceReference(displayName());
  }
  startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_NamespaceVisible);
  if (getOuterScope()!=Doxygen::globalScope)
  {
    writeNavigationPath(ol);
  }
  startTitle(ol,getOutputFileBase());
  ol.parseText(pageTitle);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),displayName());
  
  if (Config_getBool("SEARCHENGINE"))
  {
    Doxygen::searchIndex->setCurrentDoc(pageTitle,getOutputFileBase());
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }

  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    Doxygen::tagFile << "  <compound kind=\"namespace\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  
  ol.startTextBlock();
    
  if (Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
    ol.newParagraph();
  }
  else if (!briefDescription().isEmpty()) 
  {
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
    ol.disableAllBut(OutputGenerator::Man);
    ol.newParagraph();
    ol.popGeneratorState();
  }
  ol.disable(OutputGenerator::Man);
  ol.newParagraph();
  ol.enable(OutputGenerator::Man);
  ol.writeSynopsis();

  ol.endTextBlock();
  
  ol.startMemberSections();
  classSDict->writeDeclaration(ol,0,0,TRUE);

  namespaceSDict->writeDeclaration(ol,TRUE);

  /* write user defined member groups */
  if (memberGroupSDict)
  {
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

  writeMemberDeclarations(ol,MemberList::decDefineMembers,theTranslator->trDefines());
  writeMemberDeclarations(ol,MemberList::decProtoMembers,theTranslator->trFuncProtos());
  writeMemberDeclarations(ol,MemberList::decTypedefMembers,theTranslator->trTypedefs());
  writeMemberDeclarations(ol,MemberList::decEnumMembers,theTranslator->trEnumerations());
  writeMemberDeclarations(ol,MemberList::decFuncMembers,theTranslator->trFunctions());
  writeMemberDeclarations(ol,MemberList::decVarMembers,theTranslator->trVariables());
  ol.endMemberSections();
  
  if (!Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
  }

  writeMemberDocumentation(ol);

  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  ol.popGeneratorState();
  endFile(ol);

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    MemberList *allMemberList = getMemberList(MemberList::allMembersList);
    if (allMemberList) allMemberList->sort();
    writeMemberPages(ol);
  }
}

void NamespaceDef::writeMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
  }
  
  writeMemberDocumentation(ol,MemberList::docDefineMembers,theTranslator->trDefineDocumentation());
  writeMemberDocumentation(ol,MemberList::docProtoMembers,theTranslator->trFunctionPrototypeDocumentation());
  writeMemberDocumentation(ol,MemberList::docTypedefMembers,theTranslator->trTypedefDocumentation());
  writeMemberDocumentation(ol,MemberList::docEnumMembers,theTranslator->trEnumerationTypeDocumentation());
  writeMemberDocumentation(ol,MemberList::docFuncMembers,theTranslator->trFunctionDocumentation());
  writeMemberDocumentation(ol,MemberList::docVarMembers,theTranslator->trVariableDocumentation());

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
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
    if (ml->listType()&MemberList::documentationLists)
    {
      ml->writeDocumentationPage(ol,name(),this);
    }
  }
  ol.popGeneratorState();
}

void NamespaceDef::writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool("CREATE_SUBDIRS");

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberList::allMembersList);
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
          ol.writeString(md->localName());
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
  MemberList *allMemberList = getMemberList(MemberList::allMembersList);
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
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
        theTranslator->trNamespace(TRUE,TRUE),
        getOutputFileBase(),displayName()
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
    if (ml->listType()&MemberList::documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

QCString NamespaceDef::displayName() const
{
  QCString result=name();
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    result = substitute(result,"::",".");
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

void NamespaceSDict::writeDeclaration(OutputList &ol,bool localName)
{
  if (count()==0) return; // no namespaces in the list

  SDict<NamespaceDef>::Iterator ni(*this);
  NamespaceDef *nd;
  bool found=FALSE;
  for (ni.toFirst();(nd=ni.current()) && !found;++ni)
  {
    if (nd->isLinkable()) found=TRUE;
  }
  if (!found) return; // no linkable namespaces in the list

  // write list of namespaces
  ol.startMemberHeader();
  bool javaOpt = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  if (javaOpt)
  {
    ol.parseText(theTranslator->trPackages());
  }
  else
  {
    ol.parseText(theTranslator->trNamespaces());
  }
  ol.endMemberHeader();
  ol.startMemberList();
  for (ni.toFirst();(nd=ni.current());++ni)
  {
    if (nd->isLinkable())
    {
      ol.startMemberItem(0);
      if (javaOpt)
      {
        ol.docify("package ");
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
      }
      ol.endMemberItem();
      if (!nd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
    }
  }
  ol.endMemberList();
}

MemberList *NamespaceDef::createMemberList(MemberList::ListType lt)
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

void NamespaceDef::addMemberToList(MemberList::ListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  static bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  MemberList *ml = createMemberList(lt);
  if (((ml->listType()&MemberList::declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberList::documentationLists) && sortMemberDocs)
     )
    ml->inSort(md);
  else
    ml->append(md);
  if (ml->listType()&MemberList::declarationLists) md->setSectionList(this,ml);
}

MemberList *NamespaceDef::getMemberList(MemberList::ListType lt) const
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

void NamespaceDef::writeMemberDeclarations(OutputList &ol,MemberList::ListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,0,this,0,0,title,0);
}

void NamespaceDef::writeMemberDocumentation(OutputList &ol,MemberList::ListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}


bool NamespaceDef::isLinkableInProject() const
{
  int i = name().findRev("::");
  if (i==-1) i=0; else i+=2;
  return !name().isEmpty() && name().at(i)!='@' &&
    hasDocumentation() && !isReference() && !isHidden();
}

bool NamespaceDef::isLinkable() const
{
  return isLinkableInProject() || isReference();
}
