/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
#include "ftvhelp.h"
#include "searchindex.h"
//#include "xml.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

class DevNullCodeDocInterface : public BaseCodeDocInterface
{
  public:
    virtual void codify(const char *) {}
    virtual void writeCodeLink(const char *ref,const char *file,
                               const char *anchor,const char *name) 
    { ref=ref; file=file; anchor=anchor; name=name; }
    virtual void writeLineNumber(const char *,const char *,
                                 const char *,int) {}
    virtual void startCodeLine() {}
    virtual void endCodeLine() {}
    virtual void startCodeAnchor(const char *) {}
    virtual void endCodeAnchor() {}
    virtual void startFontClass(const char *) {}
    virtual void endFontClass() {}
    virtual void writeCodeAnchor(const char *) {}
};



/*! create a new file definition, where \a p is the file path, 
    \a nm the file name, and \a ref is an HTML anchor name if the
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
  dir = 0;
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    docname.prepend(stripFromPath(path.copy()));
  }
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);
  acquireFileVersion();
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
  docFindSections(documentation(),this,0,docFile());
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
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.writeAnchor(0,"_details"); 
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(theTranslator->trDetailedDescription());
    ol.endGroupHeader();
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
      //if (doc.at(dl-1)!='.' && doc.at(dl-1)!='!' && doc.at(dl-1)!='?') 
      //  doc+='.';
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
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
  
  QCString versionTitle;
  if (!fileVersion.isEmpty())
  {
    versionTitle=("("+fileVersion+")");
  }
  QCString title = docname+versionTitle;
  QCString pageTitle=theTranslator->trFileReference(docname);
  startFile(ol,getOutputFileBase(),name(),pageTitle);

  if (Config_getBool("SHOW_DIRECTORIES") && getDirDef())
  {
    getDirDef()->writeNavigationPath(ol);
    QCString pageTitleShort=theTranslator->trFileReference(name());
    startTitle(ol,getOutputFileBase());
    ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.parseText(pageTitleShort); // Html only
      ol.enableAll();
      ol.disable(OutputGenerator::Html);
      ol.parseText(pageTitle); // other output formats
    ol.popGeneratorState();
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }
  else
  {
    startTitle(ol,getOutputFileBase());
    ol.parseText(pageTitle);
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }
  if (!fileVersion.isEmpty())
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.startProjectNumber();
    ol.docify(versionTitle);
    ol.endProjectNumber();
    ol.enableAll();
  }
  
  if (Config_getBool("SEARCHENGINE"))
  {
    Doxygen::searchIndex->setCurrentDoc(pageTitle,getOutputFileBase());
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }
  
  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"file\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <path>" << convertToXML(getPath()) << "</path>" << endl;
    Doxygen::tagFile << "    <filename>" 
                     << convertToXML(getOutputFileBase()) 
                     << "</filename>" << endl;
  }
  
  if (Config_getBool("DETAILS_AT_TOP"))
  {
    ol.startTextBlock(); 
    writeDetailedDocumentation(ol);
    ol.endTextBlock();

    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::RTF);
    ol.newParagraph();
    ol.popGeneratorState();
  }
  else if (!briefDescription().isEmpty()) 
  {
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
    //ol.writeString(" \n");
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();

    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::RTF);
    ol.newParagraph();
    ol.popGeneratorState();
  }
  ol.writeSynopsis();
 
  if (Config_getBool("SHOW_INCLUDE_FILES") && includeList->count()>0)
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
                      fd->name().right(5)==".pidl" ||
                      fd->name().right(5)==".java";
      }
      ol.startTypewriter();
      if (isIDLorJava) // IDL/Java include
      {
        ol.docify("import ");
      }
      else if (ii->imported) // Objective-C include
      {
        ol.docify("#import ");
      }
      else // C/C++ include
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
          const char *locStr = (ii->local    || isIDLorJava) ? "yes" : "no";
          const char *impStr = (ii->imported || isIDLorJava) ? "yes" : "no";
          Doxygen::tagFile << "    <includes id=\"" 
                           << convertToXML(fd->getOutputFileBase()) << "\" "
                           << "name=\"" << convertToXML(fd->name()) << "\" "
                           << "local=\"" << locStr << "\" "
                           << "imported=\"" << impStr << "\">"
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
      ol.lineBreak();
    }
    ol.endTextBlock();
  }
  
  if (Config_getBool("HAVE_DOT") && Config_getBool("INCLUDE_GRAPH"))
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,Config_getInt("MAX_DOT_GRAPH_DEPTH"),FALSE);
    if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock(); 
      ol.disable(OutputGenerator::Man);
      ol.newParagraph();
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclDepGraph(name()));
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString("HTML_OUTPUT"),fd->getOutputFileBase());
  }

  if (Config_getBool("HAVE_DOT") && Config_getBool("INCLUDED_BY_GRAPH"))
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,Config_getInt("MAX_DOT_GRAPH_DEPTH"),TRUE);
    if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock(); 
      ol.disable(OutputGenerator::Man);
      ol.newParagraph();
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclByDepGraph());
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
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
  QCString title = docname;
  if (!fileVersion.isEmpty())
  {
    title+=(" ("+fileVersion+")");
  }
  QCString pageTitle = theTranslator->trSourceFile(title);
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,getSourceFileBase(),0,pageTitle);

  if (Config_getBool("SHOW_DIRECTORIES") && getDirDef())
  {
    getDirDef()->writeNavigationPath(ol);
    startTitle(ol,getOutputFileBase());
    ol.parseText(name());
    endTitle(ol,getOutputFileBase(),title);
  }
  else
  {
    startTitle(ol,0);
    ol.parseText(title);
    endTitle(ol,0,0);
  }

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

void FileDef::parseSource()
{
  DevNullCodeDocInterface devNullIntf;
  parseCode(devNullIntf,0,
            fileToString(absFilePath(),Config_getBool("FILTER_SOURCE_FILES")),
            FALSE,0,this
           );
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
  bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  switch(md->memberType())
  {
    case MemberDef::Variable:     
    case MemberDef::Property:     
      if (sortBriefDocs)  decVarMembers.inSort(md); else decVarMembers.append(md);
      if (sortMemberDocs) docVarMembers.inSort(md); else docVarMembers.append(md);
      break;
    case MemberDef::Function: 
      if (sortBriefDocs)  decFuncMembers.inSort(md); else decFuncMembers.append(md);
      if (sortMemberDocs) docFuncMembers.inSort(md); else docFuncMembers.append(md);
      break;
    case MemberDef::Typedef:      
      if (sortBriefDocs)  decTypedefMembers.inSort(md); else decTypedefMembers.append(md);
      if (sortMemberDocs) docTypedefMembers.inSort(md); else docTypedefMembers.append(md);
      break;
    case MemberDef::Enumeration:  
      if (sortBriefDocs)  decEnumMembers.inSort(md); else decEnumMembers.append(md);
      if (sortMemberDocs) docEnumMembers.inSort(md); else docEnumMembers.append(md);
      break;
    case MemberDef::EnumValue:    // enum values are shown inside their enums
      break;
    case MemberDef::Prototype:    
      if (sortBriefDocs)  decProtoMembers.inSort(md); else decProtoMembers.append(md);
      if (sortMemberDocs) docProtoMembers.inSort(md); else docProtoMembers.append(md);
      break;
    case MemberDef::Define:       
      if (sortBriefDocs)  decDefineMembers.inSort(md); else decDefineMembers.append(md);
      if (sortMemberDocs) docDefineMembers.inSort(md); else docDefineMembers.append(md);
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
  if (Config_getBool("SORT_BRIEF_DOCS"))
    classSDict->inSort(cd->name(),cd);
  else
    classSDict->append(cd->name(),cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (!nd->name().isEmpty() && namespaceSDict->find(nd->name())==0)
  {
    if (Config_getBool("SORT_BRIEF_DOCS"))
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
    usingDirList = new NamespaceSDict;
  }
  if (usingDirList->find(nd->qualifiedName())==0)
  {
    usingDirList->append(nd->qualifiedName(),nd);
  }
}

void FileDef::addUsingDeclaration(Definition *d)
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

void FileDef::addIncludeDependency(FileDef *fd,const char *incName,bool local,
                                   bool imported)
{
  //printf("FileDef::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && includeDict->find(iName)==0)
  {
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    includeList->append(ii);
    includeDict->insert(iName,ii);
  }
}

void FileDef::addIncludedUsingDirectives()
{
  if (visited) return;
  visited=TRUE;
  //printf("( FileDef::addIncludedUsingDirectives for file %s\n",name().data());

  NamespaceList nl;
  if (includeList) // file contains #includes
  {
    {
      QListIterator<IncludeInfo> iii(*includeList);
      IncludeInfo *ii;
      for (iii.toFirst();(ii=iii.current());++iii) // foreach #include...
      {
        if (ii->fileDef && !ii->fileDef->visited) // ...that is a known file
        {
          // recurse into this file
          ii->fileDef->addIncludedUsingDirectives();
        }
      }
    }
    {
      QListIterator<IncludeInfo> iii(*includeList);
      IncludeInfo *ii;
      // iterate through list from last to first
      for (iii.toLast();(ii=iii.current());--iii)
      {
        if (ii->fileDef && ii->fileDef!=this)
        {
          NamespaceSDict *unl = ii->fileDef->usingDirList;
          if (unl)
          {
            NamespaceSDict::Iterator nli(*unl);
            NamespaceDef *nd;
            for (nli.toLast();(nd=nli.current());--nli)
            {
              // append each using directive found in a #include file
              if (usingDirList==0) usingDirList = new NamespaceSDict;
              //printf("Prepending used namespace %s to the list of file %s\n",
              //    nd->name().data(),name().data());
              usingDirList->prepend(nd->qualifiedName(),nd);
            }
          }
        }
      }
    }
  }
  //printf(") end FileDef::addIncludedUsingDirectives for file %s\n",name().data());
}


void FileDef::addIncludedByDependency(FileDef *fd,const char *incName,
                                      bool local,bool imported)
{
  //printf("FileDef::addIncludedByDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && includedByDict->find(iName)==0)
  {
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
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
  addRefItem(xrefListItems(),
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

//-------------------------------------------------------------------

static int findMatchingPart(const QCString &path,const QCString dir)
{
  int si1;
  int pos1=0,pos2=0;
  while ((si1=path.find('/',pos1))!=-1)
  {
    int si2=dir.find('/',pos2);
    //printf("  found slash at pos %d in path %d: %s<->%s\n",si1,si2,
    //    path.mid(pos1,si1-pos2).data(),dir.mid(pos2).data());
    if (si2==-1 && path.mid(pos1,si1-pos2)==dir.mid(pos2)) // match at end
    {
      return dir.length();
    }
    if (si1!=si2 || path.mid(pos1,si1-pos2)!=dir.mid(pos2,si2-pos2)) // no match in middle
    {
      return QMAX(pos1-1,0);
    }
    pos1=si1+1;
    pos2=si2+1;
  }
  return 0;
}

static Directory *findDirNode(Directory *root,const QCString &name)
{
  QListIterator<DirEntry> dli(root->children());
  DirEntry *de;
  for (dli.toFirst();(de=dli.current());++dli)
  {
    if (de->kind()==DirEntry::Dir)
    {
      Directory *dir = (Directory *)de;
      QCString dirName=dir->name();
      int sp=findMatchingPart(name,dirName);
      //printf("findMatchingPart(%s,%s)=%d\n",name.data(),dirName.data(),sp);
      if (sp>0) // match found
      {
        if ((uint)sp==dirName.length()) // whole directory matches
        {
          // recurse into the directory
          return findDirNode(dir,name.mid(dirName.length()+1));
        } 
        else // partial match => we need to split the path into three parts
        {
          QCString baseName     =dirName.left(sp);
          QCString oldBranchName=dirName.mid(sp+1);
          QCString newBranchName=name.mid(sp+1);
          // strip file name from path
          int newIndex=newBranchName.findRev('/');
          if (newIndex>0) newBranchName=newBranchName.left(newIndex);

          //printf("Splitting off part in new branch \n"
          //    "base=%s old=%s new=%s\n",
          //    baseName.data(),
          //    oldBranchName.data(),
          //    newBranchName.data()
          //      );
          Directory *base = new Directory(root,baseName);
          Directory *newBranch = new Directory(base,newBranchName);
          dir->reParent(base);
          dir->rename(oldBranchName);
          base->addChild(dir);
          base->addChild(newBranch);
          dir->setLast(FALSE);
          // remove DirEntry container from list (without deleting it)
          root->children().setAutoDelete(FALSE);
          root->children().removeRef(dir);
          root->children().setAutoDelete(TRUE);
          // add new branch to the root
          if (!root->children().isEmpty())
          {
            root->children().last()->setLast(FALSE); 
          }
          root->addChild(base);
          return newBranch;
        }
      }
    }
  }
  int si=name.findRev('/');
  if (si==-1) // no subdir
  {
    return root; // put the file under the root node.
  }
  else // need to create a subdir 
  {
    QCString baseName = name.left(si);
    //printf("new subdir %s\n",baseName.data());
    Directory *newBranch = new Directory(root,baseName);
    if (!root->children().isEmpty())
    {
      root->children().last()->setLast(FALSE); 
    }
    root->addChild(newBranch);
    return newBranch;
  }
}

static void mergeFileDef(Directory *root,FileDef *fd)
{
  QCString rootPath = root->name();
  QCString filePath = fd->absFilePath();
  //printf("merging %s\n",filePath.data());
  Directory *dirNode = findDirNode(root,filePath);
  if (!dirNode->children().isEmpty())
  {
    dirNode->children().last()->setLast(FALSE); 
  }
  DirEntry *e=new DirEntry(dirNode,fd);
  dirNode->addChild(e);
}

#if 0
static void generateIndent(QTextStream &t,DirEntry *de,int level)
{
  if (de->parent())
  {
    generateIndent(t,de->parent(),level+1);
  }
  // from the root up to node n do...
  if (level==0) // item before a dir or document
  {
    if (de->isLast())
    {
      if (de->kind()==DirEntry::Dir)
      {
        t << "<img " << FTV_IMGATTRIBS(plastnode) << "/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(lastnode) << "/>";
      }
    }
    else
    {
      if (de->kind()==DirEntry::Dir)
      {
        t << "<img " << FTV_IMGATTRIBS(pnode) << "/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(node) << "/>";
      }
    }
  }
  else // item at another level
  {
    if (de->isLast())
    {
      t << "<img " << FTV_IMGATTRIBS(blank) << "/>";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(vertline) << "/>";
    }
  }
}

static void writeDirTreeNode(QTextStream &t,Directory *root,int level)
{
  QCString indent;
  indent.fill(' ',level*2);
  QListIterator<DirEntry> dli(root->children());
  DirEntry *de;
  for (dli.toFirst();(de=dli.current());++dli)
  {
    t << indent << "<p>";
    generateIndent(t,de,0);
    if (de->kind()==DirEntry::Dir)
    {
      Directory *dir=(Directory *)de;
      //printf("%s [dir]: %s (last=%d,dir=%d)\n",indent.data(),dir->name().data(),dir->isLast(),dir->kind()==DirEntry::Dir);
      t << "<img " << FTV_IMGATTRIBS(folderclosed) << "/>";
      t << dir->name();
      t << "</p>\n";
      t << indent << "<div>\n";
      writeDirTreeNode(t,dir,level+1);
      t << indent << "</div>\n";
    }
    else
    {
      //printf("%s [file]: %s (last=%d,dir=%d)\n",indent.data(),de->file()->name().data(),de->isLast(),de->kind()==DirEntry::Dir);
      t << "<img " << FTV_IMGATTRIBS(doc) << "/>";
      t << de->file()->name();
      t << "</p>\n";
    }
  }
}
#endif

static void addDirsAsGroups(Directory *root,GroupDef *parent,int level)
{
  GroupDef *gd=0;
  if (root->kind()==DirEntry::Dir)
  {
    gd = new GroupDef("<generated>",
                      1,
                      root->path(), // name
                      root->name()  // title
                     );
    if (parent) 
    {
      parent->addGroup(gd);
      gd->makePartOfGroup(parent);
    }
    else
    {
      Doxygen::groupSDict.append(root->path(),gd);
    }
  }
  QListIterator<DirEntry> dli(root->children());
  DirEntry *de;
  for (dli.toFirst();(de=dli.current());++dli)
  {
    if (de->kind()==DirEntry::Dir)
    {
      addDirsAsGroups((Directory *)de,gd,level+1);
    }
  }
}

void generateFileTree()
{
  FTVHelp::generateTreeViewImages();
  
  Directory *root=new Directory(0,"root");
  root->setLast(TRUE);
  FileNameListIterator fnli(Doxygen::inputNameList); 
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      mergeFileDef(root,fd);
    }
  }
  //t << "<div class=\"directory\">\n";
  //writeDirTreeNode(t,root,0);
  //t << "</div>\n";
  addDirsAsGroups(root,0,0);
  delete root;
}

//-------------------------------------------------------------------

void FileDef::combineUsingRelations()
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

bool FileDef::isDocumentationFile() const
{
  return name().right(4)==".doc" ||
         name().right(4)==".txt" ||
         name().right(4)==".dox";
}

void FileDef::acquireFileVersion()
{
  QCString vercmd = Config_getString("FILE_VERSION_FILTER");
  if (!vercmd.isEmpty()) 
  {
    msg("Version of %s : ",filepath.data());
    FILE *f=popen(vercmd+" "+filepath,"r");
    if (!f)
    {
      err("Error: could not execute %s\n",vercmd.data());
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead = fread(buf,1,bufSize,f);
    pclose(f);
    if (numRead > 0) 
    {
      fileVersion = QCString(buf,numRead).stripWhiteSpace();
      msg("%s\n",fileVersion.data());
    }
    else 
    {
      msg("no version available\n");
    }
  }
}


