/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include "qtbc.h"
#include "memberlist.h"
#include "classlist.h"
#include "filedef.h"
#include "scanner.h"
#include "doxygen.h"
#include "memberdef.h"
#include "classdef.h"
#include "namespacedef.h"
#include "util.h"
#include "language.h"
#include "outputlist.h"
#include "dot.h"
#include "message.h"

/*! create a new file definition, where \a p is the file path, 
    \a the file name, and \a ref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDef::FileDef(const char *p,const char *nm,const char *ref)
   : Definition(nm)
{
  path=p;
  filepath=path+nm;
  filename=nameToFile(nm);
  diskname=filename.copy();
  setReference(ref);
  classList     = new ClassList;
  includeList   = new QList<IncludeInfo>;
  includeList->setAutoDelete(TRUE);
  includeDict   = new QDict<IncludeInfo>(61);
  namespaceList = new NamespaceList;
  namespaceDict = new NamespaceDict(7);
  srcDefDict = 0;
  srcAnchorDict = 0;
  usingList = 0;
  isSource = FALSE; 
  docname = nm;
  if (Config::fullPathNameFlag)
  {
    docname.prepend(stripFromPath(path.copy()));
  }
  memberGroupList = new MemberGroupList;
  memberGroupList->setAutoDelete(TRUE);
  memberGroupDict = new MemberGroupDict(1009);
}

/*! destroy the file definition */
FileDef::~FileDef()
{
  delete classList;
  delete includeDict;
  delete includeList;
  delete namespaceList;
  delete namespaceDict;
  delete srcDefDict;
  delete srcAnchorDict;
  delete usingList;
  delete memberGroupList;
  delete memberGroupDict;
}

/*! Compute the HTML anchor names for all members in the class */ 
void FileDef::computeAnchors()
{
  setAnchors('a',&allMemberList);
}

/*! Write the documentation page for this file to the file of output
    generators \a ol. 
*/
void FileDef::writeDocumentation(OutputList &ol)
{
  //funcList->countDecMembers();
  
  //QCString fn = name();
  //if (Config::fullPathNameFlag)
  //{
  //  fn.prepend(stripFromPath(getPath().copy()));
  //}
  
  QCString pageTitle=name()+" File Reference";
  startFile(ol,diskname,pageTitle);
  startTitle(ol,getOutputFileBase());
  parseText(ol,theTranslator->trFileReference(docname));
  endTitle(ol,getOutputFileBase(),getOutputFileBase());
  //ol.newParagraph();
  
  if (!Config::genTagFile.isEmpty()) tagFile << "&" << name() << ":\n";
  
  ol.startTextBlock();
  //brief=brief.stripWhiteSpace();
  //int bl=brief.length();
  OutputList briefOutput(&ol); 
  if (briefDescription()) 
  {
    //if (brief.at(bl-1)!='.' && brief.at(bl-1)!='!' && brief.at(bl!='?')) 
    //  brief+='.';
    parseDoc(briefOutput,0,0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    parseText(ol,theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
    ol.disable(OutputGenerator::Man);
    ol.newParagraph();
    ol.enable(OutputGenerator::Man);
  }
  ol.writeSynopsis();
 
  if (Config::showIncFileFlag)
  {
    ol.startTextBlock(TRUE);
    QListIterator<IncludeInfo> ili(*includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      FileDef *fd=ii->fileDef;
      ol.startTypewriter();
      ol.docify("#include ");
      if (ii->local)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.disable(OutputGenerator::Html);
      ol.docify(ii->includeName);
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::Html);
      if (fd && fd->isLinkable() && 
          (fd->generateSource() || Config::sourceBrowseFlag)
         )
      {
        ol.writeObjectLink(fd->getReference(),fd->includeName(),0,ii->includeName);
      }
      else
      {
        ol.docify(ii->includeName);
      }
      ol.enableAll();
      if (ii->local)
        ol.docify("\"");
      else
        ol.docify(">");
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
  
  if (Config::haveDotFlag && Config::includeGraphFlag)
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this);
    if (!incDepGraph.isTrivial())
    {
      ol.disableAllBut(OutputGenerator::Html);
      ol.newParagraph();
      ol.startInclDepGraph();
      parseText(ol,theTranslator->trInclDepGraph(name()));
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
    }
    //incDepGraph.writeGraph(Config::htmlOutputDir,fd->getOutputFileBase());
  }

  if (generateSource() || (!isReference() && Config::sourceBrowseFlag))
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.newParagraph();
    ol.startTextLink(includeName(),0);
    parseText(ol,theTranslator->trGotoSourceCode());
    ol.endTextLink();
    ol.enableAll();
  }
  
  //ol.disableAllBut(OutputGenerator::Html);
  //ol.writeString((QCString)"<p>Interface collaboration diagram for "
  //               "<a href=\"usage_intf_graph_"+name()+".html\">here</a>");
  //ol.writeString((QCString)"<p>Include dependency diagram for "+fn+" can be found "+
  //               "<a href=\""+diskname+"_incldep.html\">here</a>.");
  //ol.enableAll();

  ol.endTextBlock();
  
  ol.startMemberSections();

  if (namespaceList->count()>0)
  {
    NamespaceDef *nd=namespaceList->first();
    bool found=FALSE;
    while (nd)
    {
      if (nd->name().find('@')==-1)
      {
        if (!found)
        {
          ol.startMemberHeader();
          parseText(ol,theTranslator->trNamespaces());
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
              nd->name()
                            );
        }
        else
        {
          ol.startBold();
          ol.docify(nd->name());
          ol.endBold();
        }
        ol.endMemberItem(FALSE);
      }
      nd=namespaceList->next();
    }
    if (found) ol.endMemberList();
  }
  if (classList->count()>0)
  {
    ClassDef *cd=classList->first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->name().find('@')==-1)
      {
        if (!found)
        {
          ol.startMemberHeader();
          parseText(ol,theTranslator->trCompounds());
          ol.endMemberHeader();
          ol.startMemberList();
          found=TRUE;
        }
        ol.startMemberItem(FALSE);
        switch (cd->compoundType())
        {
          case ClassDef::Class:  ol.writeString("class");  break;
          case ClassDef::Struct: ol.writeString("struct"); break;
          case ClassDef::Union:  ol.writeString("union");  break;
          case ClassDef::Interface:  ol.writeString("interface");  break;
          case ClassDef::Exception:  ol.writeString("exception");  break;
        }
        ol.writeString(" ");
        ol.insertMemberAlign();
        if (cd->isLinkable()) 
        {
          ol.writeObjectLink(cd->getReference(),
              cd->getOutputFileBase(),
              0,
              cd->name()
                            );
        }
        else
        {
          ol.startBold();
          ol.docify(cd->name());
          ol.endBold();
        }
        ol.endMemberItem(FALSE);
      }
      cd=classList->next();
    }
    if (found) ol.endMemberList();
  }
  
  /* write user defined member groups */
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->writeDeclarations(ol,0,0,this,0);
  }

  allMemberList.writeDeclarations(ol,0,0,this,0,0,0);
  ol.endMemberSections();

  //doc=doc.stripWhiteSpace();
  //int bl=brief.length();
  //int dl=doc.length();
  if (!briefDescription().isEmpty() || !documentation().isEmpty() || 
      startBodyLine!=-1)
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.writeAnchor("_details"); 
    //if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    if (!briefDescription().isEmpty())
    {
      ol+=briefOutput;
    }
    if (!briefDescription().isEmpty() && !documentation().isEmpty())
    {
      ol.newParagraph();
    }
    if (!documentation().isEmpty())
    {
      //if (doc.at(dl-1)!='.' && doc.at(dl-1)!='!' && doc.at(dl-1)!='?') 
      //  doc+='.';
      parseDoc(ol,0,0,documentation()+"\n");
    }
    //printf("Writing source ref for file %s\n",name().data());
    if (Config::sourceBrowseFlag) 
    {
      ol.newParagraph();
      QCString refText = theTranslator->trDefinedInSourceFile();
      int fileMarkerPos = refText.find("@0");
      if (fileMarkerPos!=-1) // should always pass this.
      {
        parseText(ol,refText.left(fileMarkerPos)); //text left from marker 1
        ol.writeObjectLink(0,sourceName(),
            0,name());
        parseText(ol,refText.right(
              refText.length()-fileMarkerPos-2)); // text right from marker 2
      }
    }
  }

  defineMembers.countDocMembers();
  if (defineMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDefineDocumentation());
    ol.endGroupHeader();
    defineMembers.writeDocumentation(ol,name());
  }
  
  protoMembers.countDocMembers(); 
  if (protoMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    protoMembers.writeDocumentation(ol,name());
  }

  typedefMembers.countDocMembers();
  if (typedefMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    typedefMembers.writeDocumentation(ol,name());
  }
  
  enumMembers.countDocMembers();
  if (enumMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    enumMembers.writeDocumentation(ol,name());
  }

  enumValMembers.countDocMembers();
  if (enumValMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
    enumValMembers.writeDocumentation(ol,name());
  }

  funcMembers.countDocMembers();
  if (funcMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionDocumentation());
    ol.endGroupHeader();
    funcMembers.writeDocumentation(ol,name());
  }
  
  varMembers.countDocMembers();
  if (varMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    varMembers.writeDocumentation(ol,name());
  }
  
  // write Author section (Man only)
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  parseText(ol,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseText(ol,theTranslator->trGeneratedAutomatically(Config::projectName));
  ol.enableAll();
  endFile(ol);
}

/*! Write a source listing of this file to the output */
void FileDef::writeSource(OutputList &ol)
{
  //QCString fn=name();
  //if (Config::fullPathNameFlag)
  //{
  //  fn.prepend(stripFromPath(getPath().copy()));
  //}
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,sourceName(),docname+" Source File");
  startTitle(ol,0);
  parseText(ol,docname);
  endTitle(ol,0,0);

  if (isLinkable())
  {
    ol.startTextLink(getOutputFileBase(),0);
    parseText(ol,theTranslator->trGotoDocumentation());
    ol.endTextLink();
  }

  //parseText(ol,theTranslator->trVerbatimText(incFile->name()));
  //ol.writeRuler();
  initParseCodeContext();
  ol.startCodeFragment();
  parseCode(ol,0,fileToString(absFilePath()),FALSE,0,this);
  ol.endCodeFragment();
  endFile(ol);
  ol.enableAll();
}

/*! Adds a member \a md to the member group with id \a groupId.
 */ 
void FileDef::addMemberToGroup(MemberDef *md,int groupId)
{
  if (groupId!=-1)
  {
    QCString *pGrpHeader = memberHeaderDict[groupId];
    QCString *pDocs      = memberDocDict[groupId];
    if (pGrpHeader)
    {
      MemberGroup *mg = memberGroupDict->find(groupId);
      if (mg==0)
      {
        mg = new MemberGroup(groupId,*pGrpHeader,pDocs ? pDocs->data() : 0);
        memberGroupDict->insert(groupId,mg);
        memberGroupList->append(mg);
      }
      mg->insertMember(md);
      md->setMemberGroup(mg);
    }
  }
}

/*! Adds member definition \a md to the list of all members of this file */
void FileDef::insertMember(MemberDef *md,int groupId)
{
  allMemberList.append(md); 
  switch(md->memberType())
  {
    case MemberDef::Variable:     varMembers.inSort(md); break;
    case MemberDef::Function:     funcMembers.inSort(md); break;
    case MemberDef::Typedef:      typedefMembers.inSort(md); break;
    case MemberDef::Enumeration:  enumMembers.inSort(md); break;
    case MemberDef::EnumValue:    enumValMembers.inSort(md); break;
    case MemberDef::Prototype:    protoMembers.inSort(md); break;
    case MemberDef::Define:       defineMembers.inSort(md); break;
    default:
       err("FileDef::insertMembers(): unexpected member insert in file!\n");
  }
  addMemberToGroup(md,groupId);
}

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDef::insertClass(ClassDef *cd)
{
  classList->inSort(cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (!nd->name().isEmpty() && namespaceDict->find(nd->name())==0)
  {
    namespaceList->inSort(nd);
    namespaceDict->insert(nd->name(),nd);
  }
}

void FileDef::addSourceRef(int line,Definition *d,const char *anchor)
{
  if (d)
  {
    if (srcDefDict==0) srcDefDict = new QIntDict<Definition>(257);
    if (srcAnchorDict==0) 
    {
      srcAnchorDict = new QIntDict<QCString>(257);
      srcAnchorDict->setAutoDelete(TRUE);
    }
    srcDefDict->insert(line,d);
    srcAnchorDict->insert(line,new QCString(anchor));
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

QCString FileDef::getSourceAnchor(int lineNr)
{
  QCString result;
  if (srcAnchorDict)
  {
    QCString *pstr = srcAnchorDict->find(lineNr);
    if (pstr) result=*pstr;
  }
  return result;
}

void FileDef::addUsingDirective(NamespaceDef *nd)
{
  if (usingList==0)
  {
    usingList = new NamespaceList;
  }
  usingList->append(nd);
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

//-----------------------------------------------------------------------------

/*! Creates a file list. */
FileList::FileList() : QList<FileDef>()
{
}

/*! Destroys a file list */
FileList::~FileList()
{
}

/*! Compares two files by name. */
int FileList::compareItems(GCI item1, GCI item2)
{
  FileDef *f1=(FileDef *)item1;
  FileDef *f2=(FileDef *)item2;
  ASSERT(f1!=0 && f2!=0);
  return Config::fullPathNameFlag ? 
         stricmp(f1->absFilePath(),f2->absFilePath()) : 
         stricmp(f1->name(),f2->name());
}

/*! Create a file list iterator. */
FileListIterator::FileListIterator(const FileList &cllist) :
  QListIterator<FileDef>(cllist)
{
}

