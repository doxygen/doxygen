/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#include "memberlist.h"
#include "classlist.h"
#include "filedef.h"
#include "doxygen.h"
#include "memberdef.h"
#include "classdef.h"
#include "namespacedef.h"
#include "util.h"
#include "language.h"
#include "outputlist.h"
#include "dot.h"
#include "message.h"
#include "code.h"
#include "docparser.h"
//#include "xml.h"

/*! create a new file definition, where \a p is the file path, 
    \a the file name, and \a ref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDef::FileDef(const char *p,const char *nm,
                 const char *lref,const char *dn)
   : Definition((QCString)p+nm,1,nm)
{
  path=p;
  filepath=path+nm;
  filename=nm;
  diskname=dn;
  if (diskname.isEmpty()) diskname=nm;
  setReference(lref);
  //printf("new FileDef(path=%s,name=%s,ref=%s)\n",p,nm,lref);
  classSDict     = new ClassSDict(17);
  includeList   = new QList<IncludeInfo>;
  includeList->setAutoDelete(TRUE);
  includeDict   = new QDict<IncludeInfo>(61);
  includedByList = new QList<IncludeInfo>;
  includedByList->setAutoDelete(TRUE);
  includedByDict = new QDict<IncludeInfo>(61);
  namespaceSDict = new NamespaceSDict;
  srcDefDict = 0;
  srcMemberDict = 0;
  usingDirList = 0;
  usingDeclList = 0;
  package = 0;
  isSource = FALSE; 
  docname = nm;
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    docname.prepend(stripFromPath(path.copy()));
  }
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);
}

/*! destroy the file definition */
FileDef::~FileDef()
{
  delete classSDict;
  delete includeDict;
  delete includeList;
  delete includedByDict;
  delete includedByList;
  delete namespaceSDict;
  delete srcDefDict;
  delete srcMemberDict;
  delete usingDirList;
  delete usingDeclList;
  delete memberGroupSDict;
}

/*! Compute the HTML anchor names for all members in the class */ 
void FileDef::computeAnchors()
{
  setAnchors(0,'a',&allMemberList);
}

void FileDef::distributeMemberGroupDocumentation()
{
  //printf("FileDef::distributeMemberGroupDocumentation()\n");
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void FileDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),0,this,0,docFile());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->findSectionsInDocumentation();
  }
  decDefineMembers.findSectionsInDocumentation();
  decProtoMembers.findSectionsInDocumentation();
  decTypedefMembers.findSectionsInDocumentation();
  decEnumMembers.findSectionsInDocumentation();
  decFuncMembers.findSectionsInDocumentation();
  decVarMembers.findSectionsInDocumentation();
}

void FileDef::writeDetailedDocumentation(OutputList &ol)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().isEmpty() 
     )
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.writeAnchor(0,"_details"); 
    //if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),0,0,briefDescription(),FALSE);
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
      //if (doc.at(dl-1)!='.' && doc.at(dl-1)!='!' && doc.at(dl-1)!='?') 
      //  doc+='.';
      ol.parseDoc(docFile(),docLine(),0,0,documentation()+"\n",FALSE);
    }
    //printf("Writing source ref for file %s\n",name().data());
    if (Config_getBool("SOURCE_BROWSER")) 
    {
      ol.newParagraph();
      QCString refText = theTranslator->trDefinedInSourceFile();
      int fileMarkerPos = refText.find("@0");
      if (fileMarkerPos!=-1) // should always pass this.
      {
        ol.parseText(refText.left(fileMarkerPos)); //text left from marker 1
        ol.writeObjectLink(0,getSourceFileBase(),
            0,name());
        ol.parseText(refText.right(
              refText.length()-fileMarkerPos-2)); // text right from marker 2
      }
    }
  }
}

/*! Write the documentation page for this file to the file of output
    generators \a ol. 
*/
void FileDef::writeDocumentation(OutputList &ol)
{
  //funcList->countDecMembers();
  
  //QCString fn = name();
  //if (Config_getBool("FULL_PATH_NAMES"))
  //{
  //  fn.prepend(stripFromPath(getPath().copy()));
  //}

  //printf("WriteDocumentation diskname=%s\n",diskname.data());
  
  QCString pageTitle=name()+" File Reference";
  startFile(ol,getOutputFileBase(),name(),pageTitle);
  startTitle(ol,getOutputFileBase());
  ol.parseText(theTranslator->trFileReference(docname));
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),docName());
  //ol.newParagraph();
  
  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"file\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <path>" << convertToXML(getPath()) << "</path>" << endl;
    Doxygen::tagFile << "    <filename>" 
                     << convertToXML(getOutputFileBase()) 
                     << "</filename>" << endl;
  }
  
  ol.startTextBlock();
  if (Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
    ol.newParagraph();
  }
  else if (briefDescription()) 
  {
    ol.parseDoc(briefFile(),briefLine(),0,0,briefDescription(),FALSE);
    ol.writeString(" \n");
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
    ol.disable(OutputGenerator::Man);
    ol.newParagraph();
    ol.enable(OutputGenerator::Man);
  }
  ol.writeSynopsis();
 
  if (Config_getBool("SHOW_INCLUDE_FILES"))
  {
    ol.startTextBlock(TRUE);
    QListIterator<IncludeInfo> ili(*includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      FileDef *fd=ii->fileDef;
      bool isIDLorJava = FALSE;
      if (fd)
      {
        isIDLorJava = fd->name().right(4)==".idl" || 
                      fd->name().right(5)==".java";
      }
      ol.startTypewriter();
      if (isIDLorJava)
      {
        ol.docify("import ");
      }
      else
      {
        ol.docify("#include ");
      }
      if (ii->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.disable(OutputGenerator::Html);
      ol.docify(ii->includeName);
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::Html);
      
      // Here we use the include file name as it appears in the file.
      // we could also we the name as it is used within doxygen,
      // then we should have used fd->docName() instead of ii->includeName
      if (fd && fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),
            fd->generateSourceFile() ? fd->includeName() : fd->getOutputFileBase(),
            0,ii->includeName);
        if (!Config_getString("GENERATE_TAGFILE").isEmpty() && !fd->isReference()) 
        {
          const char *locStr = (ii->local || isIDLorJava) ? "yes" : "no";
          Doxygen::tagFile << "    <includes id=\"" 
                           << convertToXML(fd->getOutputFileBase()) 
                           << "\" name=\"" 
                           << convertToXML(fd->name())
                           << "\" local=\"" << locStr << "\">" 
                           << convertToXML(ii->includeName)
                           << "</includes>" 
                           << endl;
        }
      }
      else
      {
        ol.docify(ii->includeName);
      }
      
      ol.enableAll();
      if (ii->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify(">");
      if (isIDLorJava) 
        ol.docify(";");
      ol.endTypewriter();
      ol.disable(OutputGenerator::RTF);
      ol.lineBreak();
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::RTF);
      ol.newParagraph();
      ol.enableAll();
    }
    ol.endTextBlock();
  }
  
  if (Config_getBool("HAVE_DOT") && Config_getBool("INCLUDE_GRAPH"))
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,FALSE);
    if (!incDepGraph.isTrivial())
    {
      ol.disable(OutputGenerator::Man);
      ol.newParagraph();
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclDepGraph(name()));
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
    }
    //incDepGraph.writeGraph(Config_getString("HTML_OUTPUT"),fd->getOutputFileBase());
  }

  if (Config_getBool("HAVE_DOT") && Config_getBool("INCLUDED_BY_GRAPH"))
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,TRUE);
    if (!incDepGraph.isTrivial())
    {
      ol.disable(OutputGenerator::Man);
      ol.newParagraph();
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclByDepGraph());
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
    }
    //incDepGraph.writeGraph(Config_getString("HTML_OUTPUT"),fd->getOutputFileBase());
  }

  //printf("%s: generateSourceFile()=%d\n",name().data(),generateSourceFile());
  if (generateSourceFile())
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.newParagraph();
    ol.startTextLink(includeName(),0);
    ol.parseText(theTranslator->trGotoSourceCode());
    ol.endTextLink();
    ol.enableAll();
  }
  
  ol.endTextBlock();
  
  ol.startMemberSections();

  if (namespaceSDict->count()>0)
  {
    NamespaceSDict::Iterator ndi(*namespaceSDict);
    NamespaceDef *nd;
    bool found=FALSE;
    for (ndi.toFirst();(nd=ndi.current());++ndi)
    {
      if (nd->name().find('@')==-1)
      {
        if (!found)
        {
          ol.startMemberHeader();
          ol.parseText(theTranslator->trNamespaces());
          ol.endMemberHeader();
          ol.startMemberList();
          found=TRUE;
        }
        ol.startMemberItem(0);
        ol.writeString("namespace ");
        ol.insertMemberAlign();
        if (nd->isLinkable()) 
        {
          ol.writeObjectLink(nd->getReference(),
              nd->getOutputFileBase(),
              0,
              nd->displayName()
          );
          if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
          {
            Doxygen::tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
          }
        }
        else
        {
          ol.startBold();
          ol.docify(nd->displayName());
          ol.endBold();
        }
        ol.endMemberItem();
      }
    }
    if (found) ol.endMemberList();
  }

  classSDict->writeDeclaration(ol);
  
  /* write user defined member groups */
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    if (mg->header()!="[NOHEADER]")
    {
      mg->writeDeclarations(ol,0,0,this,0);
    }
  }

  //allMemberList.writeDeclarations(ol,0,0,this,0,0,0);
  decDefineMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trDefines(),0);
  decProtoMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trFuncProtos(),0);
  decTypedefMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trTypedefs(),0);
  decEnumMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trEnumerations(),0);
  decFuncMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trFunctions(),0);
  decVarMembers.writeDeclarations(ol,0,0,this,0,theTranslator->trVariables(),0);
  ol.endMemberSections();

  if (!Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
  }

  docDefineMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trDefineDocumentation());
  
  docProtoMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trFunctionPrototypeDocumentation());

  docTypedefMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trTypedefDocumentation());
  
  docEnumMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trEnumerationTypeDocumentation());

  docFuncMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trFunctionDocumentation());
  
  docVarMembers.writeDocumentation(ol,name(),this,
                            theTranslator->trVariableDocumentation());
  
  // write Author section (Man only)
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));
  ol.enableAll();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  endFile(ol);
}

/*! Write a source listing of this file to the output */
void FileDef::writeSource(OutputList &ol)
{
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,getSourceFileBase(),0,docname+" Source File");
  startTitle(ol,0);
  ol.parseText(docname);
  endTitle(ol,0,0);

  if (isLinkable())
  {
    ol.startTextLink(getOutputFileBase(),0);
    ol.parseText(theTranslator->trGotoDocumentation());
    ol.endTextLink();
  }

  initParseCodeContext();
  ol.startCodeFragment();
  parseCode(ol,0,
            fileToString(absFilePath(),Config_getBool("FILTER_SOURCE_FILES")),
            FALSE,0,this
           );
  ol.endCodeFragment();
  endFile(ol);
  ol.enableAll();
}


void FileDef::addMembersToMemberGroup()
{
  ::addMembersToMemberGroup(&decDefineMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decProtoMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decTypedefMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decEnumMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decFuncMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decVarMembers,memberGroupSDict,this);
}

/*! Adds member definition \a md to the list of all members of this file */
void FileDef::insertMember(MemberDef *md)
{
  //printf("%s:FileDef::insertMember(%s)\n",name().data(),md->name().data());
  allMemberList.append(md); 
  bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  switch(md->memberType())
  {
    case MemberDef::Variable:     
    case MemberDef::Property:     
      decVarMembers.append(md);
      if (sortMemberDocs)
        docVarMembers.inSort(md); 
      else
        docVarMembers.append(md);
      break;
    case MemberDef::Function: 
      decFuncMembers.append(md);
      if (sortMemberDocs)    
        docFuncMembers.inSort(md); 
      else
        docFuncMembers.append(md);
      break;
    case MemberDef::Typedef:      
      decTypedefMembers.append(md);
      if (sortMemberDocs)
        docTypedefMembers.inSort(md); 
      else
        docTypedefMembers.append(md);
      break;
    case MemberDef::Enumeration:  
      decEnumMembers.append(md);
      if (sortMemberDocs)
        docEnumMembers.inSort(md); 
      else
        docEnumMembers.append(md);
      break;
    case MemberDef::EnumValue:    // enum values are shown inside their enums
      break;
    case MemberDef::Prototype:    
      decProtoMembers.append(md);
      if (sortMemberDocs)
        docProtoMembers.inSort(md); 
      else
        docProtoMembers.append(md);
      break;
    case MemberDef::Define:       
      decDefineMembers.append(md);
      if (sortMemberDocs)
        docDefineMembers.inSort(md); 
      else
        docDefineMembers.append(md);
      break;
    default:
       err("FileDef::insertMembers(): "
           "member `%s' with class scope `%s' inserted in file scope `%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
  //addMemberToGroup(md,groupId);
}

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDef::insertClass(ClassDef *cd)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    classSDict->inSort(cd->name(),cd);
  else
    classSDict->append(cd->name(),cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (!nd->name().isEmpty() && namespaceSDict->find(nd->name())==0)
  {
    if (Config_getBool("SORT_MEMBER_DOCS"))
      namespaceSDict->inSort(nd->name(),nd);
    else
      namespaceSDict->append(nd->name(),nd);
  }
}

void FileDef::addSourceRef(int line,Definition *d,MemberDef *md)
{
  if (d)
  {
    if (srcDefDict==0)    srcDefDict    = new QIntDict<Definition>(257);
    if (srcMemberDict==0) srcMemberDict = new QIntDict<MemberDef>(257);
    srcDefDict->insert(line,d);
    if (md) srcMemberDict->insert(line,md);
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    d->name().data(),anchor,line,name().data());
  }
}

Definition *FileDef::getSourceDefinition(int lineNr)
{
  Definition *result=0;
  if (srcDefDict)
  {
    result = srcDefDict->find(lineNr);
  }
  return result;
}

MemberDef *FileDef::getSourceMember(int lineNr)
{
  MemberDef *result=0;
  if (srcMemberDict)
  {
    result = srcMemberDict->find(lineNr);
  }
  return result;
}


void FileDef::addUsingDirective(NamespaceDef *nd)
{
  if (usingDirList==0)
  {
    usingDirList = new NamespaceList;
  }
  usingDirList->append(nd);
}

void FileDef::addUsingDeclaration(ClassDef *cd)
{
  if (usingDeclList==0)
  {
    usingDeclList = new ClassList;
  }
  usingDeclList->append(cd);
}

void FileDef::addIncludeDependency(FileDef *fd,const char *incName,bool local)
{
  //printf("FileDef::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && includeDict->find(iName)==0)
  {
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    includeList->append(ii);
    includeDict->insert(iName,ii);
  }
}

void FileDef::addIncludedUsingDirectives()
{
  if (!visited)
  {
    visited=TRUE;
    NamespaceList nl;
    if (includeList) // file contains #includes
    {
      QListIterator<IncludeInfo> iii(*includeList);
      IncludeInfo *ii;
      for (;(ii=iii.current());++iii) // foreach #include...
      {
        if (ii->fileDef) // ...that is a known file
        {
          // recurse into this file
          ii->fileDef->addIncludedUsingDirectives();
        }
      }
      // iterate through list from last to first
      for (iii.toLast();(ii=iii.current());--iii)
      {
        if (ii->fileDef)
        {
          NamespaceList *unl = ii->fileDef->usingDirList;
          if (unl)
          {
            NamespaceListIterator nli(*unl);
            NamespaceDef *nd;
            for (nli.toLast();(nd=nli.current());--nli)
            {
              // append each using directive found in a #include file
              if (usingDirList==0) usingDirList = new NamespaceList;
              usingDirList->prepend(nd);
            }
          }
        }
      }
    }
    // add elements of nl to usingDirList
  }
}


void FileDef::addIncludedByDependency(FileDef *fd,const char *incName,bool local)
{
  //printf("FileDef::addIncludedByDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && includedByDict->find(iName)==0)
  {
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    includedByList->append(ii);
    includedByDict->insert(iName,ii);
  }
}

bool FileDef::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return includeDict->find(name)!=0;
}

bool FileDef::generateSourceFile() const 
{ 
  QCString extension = name().right(4);
  return !isReference() && 
         (Config_getBool("SOURCE_BROWSER") || 
           (Config_getBool("VERBATIM_HEADERS") && guessSection(name())==Entry::HEADER_SEC) 
         ) &&
         extension!=".doc" && extension!=".txt" && extension!=".dox"; 
}


void FileDef::addListReferences()
{
  addRefItem(specialListItems(),
             theTranslator->trFile(TRUE,TRUE),
             getOutputFileBase(),name()
            );
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->addListReferences(this);
  }
  docDefineMembers.addListReferences(this);
  docProtoMembers.addListReferences(this);
  docTypedefMembers.addListReferences(this);
  docEnumMembers.addListReferences(this);
  docFuncMembers.addListReferences(this);
  docVarMembers.addListReferences(this);
}

