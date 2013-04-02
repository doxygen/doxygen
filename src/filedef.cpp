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
#include "docparser.h"
#include "searchindex.h"
#include "htags.h"
#include "parserintf.h"
#include "portable.h"
#include "vhdldocgen.h"
#include "debug.h"
#include "layout.h"
#include "entry.h"
#include "groupdef.h"
#include "filename.h"
#include "membergroup.h"
#include "dirdef.h"
#include "config.h"
#include "clangparser.h"
#include "settings.h"

//---------------------------------------------------------------------------

/** Class implementing CodeOutputInterface by throwing away everything. */
class DevNullCodeDocInterface : public CodeOutputInterface
{
  public:
    virtual void codify(const char *) {}
    virtual void writeCodeLink(const char *,const char *,
                               const char *,const char *,
                               const char *) {}
    virtual void writeLineNumber(const char *,const char *,
                                 const char *,int) {}
    virtual void startCodeLine(bool) {}
    virtual void endCodeLine() {}
    virtual void startCodeAnchor(const char *) {}
    virtual void endCodeAnchor() {}
    virtual void startFontClass(const char *) {}
    virtual void endFontClass() {}
    virtual void writeCodeAnchor(const char *) {}
    virtual void linkableSymbol(int, const char *,Definition *,Definition *) {}
    virtual void setCurrentDoc(Definition *,const char *,bool) {}
    virtual void addWord(const char *,bool) {}
};

//---------------------------------------------------------------------------

/*! create a new file definition, where \a p is the file path, 
    \a nm the file name, and \a lref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDef::FileDef(const char *p,const char *nm,
                 const char *lref,const char *dn)
   : Definition((QCString)p+nm,1,1,nm)
{
  path=p;
  filepath=path+nm;
  filename=nm;
  diskname=dn;
  if (diskname.isEmpty()) diskname=nm;
  setReference(lref);
  classSDict        = 0;
  includeList       = 0;
  includeDict       = 0; 
  includedByList    = 0;
  includedByDict    = 0; 
  namespaceSDict    = 0; 
  srcDefDict        = 0;
  srcMemberDict     = 0;
  usingDirList      = 0;
  usingDeclList     = 0;
  package           = 0;
  isSource          = FALSE; 
  docname           = nm;
  dir               = 0;
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    docname.prepend(stripFromPath(path.copy()));
  }
  SrcLangExt lang = getLanguageFromFileName(name());
  setLanguage(lang);
  //m_isJava          = lang==SrcLangExt_Java;
  //m_isCSharp        = lang==SrcLangExt_CSharp;
  memberGroupSDict = 0;
  acquireFileVersion();
  m_subGrouping=Config_getBool("SUBGROUPING");
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
  MemberList *ml = getMemberList(MemberListType_allMembersList);
  if (ml) setAnchors(ml);
}

void FileDef::distributeMemberGroupDocumentation()
{
  //printf("FileDef::distributeMemberGroupDocumentation()\n");
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->distributeMemberGroupDocumentation();
    }
  }
}

void FileDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->findSectionsInDocumentation();
    }
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

void FileDef::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().stripWhiteSpace().isEmpty() || // avail empty section
      (Config_getBool("SOURCE_BROWSER") && getStartBodyLine()!=-1 && getBodyDef())
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
        // ol.newParagraph(); // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
    //printf("Writing source ref for file %s\n",name().data());
    if (Config_getBool("SOURCE_BROWSER")) 
    {
      ol.startParagraph();
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
      ol.endParagraph();
    }
    ol.endTextBlock();
  }
}

void FileDef::writeBriefDescription(OutputList &ol)
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

    //ol.pushGeneratorState();
    //ol.disable(OutputGenerator::RTF);
    //ol.newParagraph();
    //ol.popGeneratorState();
  }
  ol.writeSynopsis();
}

void FileDef::writeIncludeFiles(OutputList &ol)
{
  if (/*Config_getBool("SHOW_INCLUDE_FILES") &&*/ includeList && 
      includeList->count()>0)
  {
    ol.startTextBlock(TRUE);
    QListIterator<IncludeInfo> ili(*includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      if (!ii->indirect)
      {
        FileDef *fd=ii->fileDef;
        bool isIDLorJava = FALSE;
        if (fd)
        {
          SrcLangExt lang   = fd->getLanguage();
          isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
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
    }
    ol.endTextBlock();
  }
}

void FileDef::writeIncludeGraph(OutputList &ol)
{
  if (Config_getBool("HAVE_DOT") /*&& Config_getBool("INCLUDE_GRAPH")*/)
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,FALSE);
    if (incDepGraph.isTooBig())
    {
       err("warning: Include graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",name().data());
    }
    else if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock(); 
      ol.disable(OutputGenerator::Man);
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclDepGraph(name()));
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString("HTML_OUTPUT"),fd->getOutputFileBase());
  }
}

void FileDef::writeIncludedByGraph(OutputList &ol)
{
  if (Config_getBool("HAVE_DOT") /*&& Config_getBool("INCLUDED_BY_GRAPH")*/)
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,TRUE);
    if (incDepGraph.isTooBig())
    {
       err("warning: Included by graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",name().data());
    }
    if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock(); 
      ol.disable(OutputGenerator::Man);
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclByDepGraph());
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString("HTML_OUTPUT"),fd->getOutputFileBase());
  }
}


void FileDef::writeSourceLink(OutputList &ol)
{
  //printf("%s: generateSourceFile()=%d\n",name().data(),generateSourceFile());
  if (generateSourceFile())
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.startParagraph();
    ol.startTextLink(includeName(),0);
    ol.parseText(theTranslator->trGotoSourceCode());
    ol.endTextLink();
    ol.endParagraph();
    ol.enableAll();
  }
}

void FileDef::writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool const isConstantGroup)
{
  // write list of namespaces
  if (namespaceSDict) namespaceSDict->writeDeclaration(ol,title,isConstantGroup);
}

void FileDef::writeClassDeclarations(OutputList &ol,const QCString &title)
{
  // write list of classes
  if (classSDict) classSDict->writeDeclaration(ol,0,title,FALSE);
}

void FileDef::writeInlineClasses(OutputList &ol)
{
  if (classSDict) classSDict->writeDocumentation(ol,this);
}

void FileDef::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void FileDef::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void FileDef::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void FileDef::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void FileDef::writeMemberGroups(OutputList &ol)
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
        mg->writeDeclarations(ol,0,0,this,0);
      }
    }
  }
}

void FileDef::writeAuthorSection(OutputList &ol)
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

void FileDef::writeSummaryLinks(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::File));
  LayoutDocEntry *lde;
  bool first=TRUE;
  SrcLangExt lang=getLanguage();
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if ((lde->kind()==LayoutDocEntry::FileClasses && classSDict && classSDict->declVisible()) || 
        (lde->kind()==LayoutDocEntry::FileNamespaces && namespaceSDict && namespaceSDict->declVisible())
       )
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = lde->kind()==LayoutDocEntry::FileClasses ? "nested-classes" : "namespaces";
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
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

/*! Write the documentation page for this file to the file of output
    generators \a ol. 
*/
void FileDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
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

  if (getDirDef())
  {
    startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_FileVisible,!generateTreeView);
    if (!generateTreeView)
    {
      getDirDef()->writeNavigationPath(ol);
      ol.endQuickIndices();
    }
    QCString pageTitleShort=theTranslator->trFileReference(name());
    startTitle(ol,getOutputFileBase(),this);
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
    startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_FileVisible,!generateTreeView);
    if (!generateTreeView)
    {
      ol.endQuickIndices();
    }
    startTitle(ol,getOutputFileBase(),this);
    ol.parseText(pageTitle);
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }

  ol.startContents();

  if (!fileVersion.isEmpty())
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.startProjectNumber();
    ol.docify(versionTitle);
    ol.endProjectNumber();
    ol.enableAll();
  }
  
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
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

  //---------------------------------------- start flexible part -------------------------------
  
  SrcLangExt lang = getLanguage();
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::File));
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
      case LayoutDocEntry::FileIncludes:
        writeIncludeFiles(ol);
        break;
      case LayoutDocEntry::FileIncludeGraph:
        writeIncludeGraph(ol);
        break;
      case LayoutDocEntry::FileIncludedByGraph:
        writeIncludedByGraph(ol);
        break;
      case LayoutDocEntry::FileSourceLink:
        writeSourceLink(ol);
        break;
      case LayoutDocEntry::FileClasses: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClassDeclarations(ol,ls->title(lang));
        }
        break; 
      case LayoutDocEntry::FileNamespaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaceDeclarations(ol,ls->title(lang),false);
        }
        break; 
      case LayoutDocEntry::FileConstantGroups:
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
      case LayoutDocEntry::FileInlineClasses:
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
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInlineClasses:
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
            "LayoutDocManager::File entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  ol.endContents();

  endFileWithNavPath(this,ol);

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    MemberList *ml = getMemberList(MemberListType_allMembersList);
    if (ml) ml->sort();
    writeMemberPages(ol);
  }
}

void FileDef::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->writeDocumentationPage(ol,name(),this);
    }
  }

  ol.popGeneratorState();
}

void FileDef::writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const
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
      if (md->getFileDef()==this && md->getNamespaceDef()==0 && md->isLinkable())
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

/*! Write a source listing of this file to the output */
void FileDef::writeSource(OutputList &ol)
{
  static bool generateTreeView  = Config_getBool("GENERATE_TREEVIEW");
  static bool filterSourceFiles = Config_getBool("FILTER_SOURCE_FILES");
  static bool latexSourceCode   = Config_getBool("LATEX_SOURCE_CODE");
  QCString title = docname;
  if (!fileVersion.isEmpty())
  {
    title+=(" ("+fileVersion+")");
  }
  QCString pageTitle = theTranslator->trSourceFile(title);
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::RTF);
  if (!latexSourceCode) ol.disable(OutputGenerator::Latex);

  bool isDocFile = isDocumentationFile();
  bool genSourceFile = !isDocFile && generateSourceFile();
  if (getDirDef())
  {
    startFile(ol,getSourceFileBase(),0,pageTitle,HLI_FileVisible,
        !generateTreeView,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase());
    if (!generateTreeView)
    {
      getDirDef()->writeNavigationPath(ol);
      ol.endQuickIndices();
    }
    startTitle(ol,getOutputFileBase());
    ol.parseText(name());
    endTitle(ol,getOutputFileBase(),title);
  }
  else
  {
    startFile(ol,getSourceFileBase(),0,pageTitle,HLI_FileVisible,FALSE,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase());
    startTitle(ol,getSourceFileBase());
    ol.parseText(title);
    endTitle(ol,getSourceFileBase(),0);
  }

  ol.startContents();

  if (isLinkable())
  {
    if (latexSourceCode) ol.disable(OutputGenerator::Latex);
    ol.startTextLink(getOutputFileBase(),0);
    ol.parseText(theTranslator->trGotoDocumentation());
    ol.endTextLink();
    if (latexSourceCode) ol.enable(OutputGenerator::Latex);
  }

#if USE_LIBCLANG
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  if (clangAssistedParsing && 
      (getLanguage()==SrcLangExt_Cpp || getLanguage()==SrcLangExt_ObjC))
  {
    ol.startCodeFragment();
    ClangParser::instance()->start(absFilePath());
    ClangParser::instance()->writeSources(ol,this);
    ClangParser::instance()->finish();
    ol.endCodeFragment();
  }
  else
#endif
  {
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    ol.startCodeFragment();
    pIntf->parseCode(ol,0,
        fileToString(absFilePath(),filterSourceFiles,TRUE),
        FALSE,0,this
        );
    ol.endCodeFragment();
  }
  ol.endContents();
  endFileWithNavPath(this,ol);
  ol.enableAll();
}

void FileDef::parseSource()
{
  static bool filterSourceFiles = Config_getBool("FILTER_SOURCE_FILES");
  DevNullCodeDocInterface devNullIntf;
  ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
  pIntf->resetCodeParserState();
  pIntf->parseCode(
            devNullIntf,0,
            fileToString(absFilePath(),filterSourceFiles,TRUE),
            FALSE,0,this
           );
}

void FileDef::addMembersToMemberGroup()
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

/*! Adds member definition \a md to the list of all members of this file */
void FileDef::insertMember(MemberDef *md)
{
  if (md->isHidden()) return;
  //printf("%s:FileDef::insertMember(%s (=%p) list has %d elements)\n",
  //    name().data(),md->name().data(),md,allMemberList.count());
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList && allMemberList->findRef(md)!=-1)  // TODO optimize the findRef!
  { 
    return;
  }

  if (allMemberList==0)
  {
    allMemberList = new MemberList(MemberListType_allMembersList);
    m_memberLists.append(allMemberList);
  }
  allMemberList->append(md); 
  //::addFileMemberNameToIndex(md);
  switch (md->memberType())
  {
    case MemberType_Variable:     
    case MemberType_Property:     
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
    case MemberType_EnumValue:    // enum values are shown inside their enums
      break;
    case MemberType_Define:       
      addMemberToList(MemberListType_decDefineMembers,md);
      addMemberToList(MemberListType_docDefineMembers,md);
      break;
    default:
       err("FileDef::insertMembers(): "
           "member `%s' with class scope `%s' inserted in file scope `%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "<global>",
           name().data());
  }
  //addMemberToGroup(md,groupId);
}

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDef::insertClass(ClassDef *cd)
{
  if (cd->isHidden()) return;
  if (classSDict==0)
  {
    classSDict = new ClassSDict(17);
  }
  if (Config_getBool("SORT_BRIEF_DOCS"))
    classSDict->inSort(cd->name(),cd);
  else
    classSDict->append(cd->name(),cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (nd->isHidden()) return;
  if (!nd->name().isEmpty() && 
      (namespaceSDict==0 || namespaceSDict->find(nd->name())==0))
  {
    if (namespaceSDict==0)
    {
      namespaceSDict = new NamespaceSDict;
    }
    if (Config_getBool("SORT_BRIEF_DOCS"))
      namespaceSDict->inSort(nd->name(),nd);
    else
      namespaceSDict->append(nd->name(),nd);
  }
}

QCString FileDef::name() const 
{ 
  if (Config_getBool("FULL_PATH_NAMES")) 
    return filename; 
  else 
    return Definition::name(); 
} 

void FileDef::addSourceRef(int line,Definition *d,MemberDef *md)
{
  //printf("FileDef::addSourceDef(%d,%p,%p)\n",line,d,md);
  if (d)
  {
    if (srcDefDict==0)    srcDefDict    = new QIntDict<Definition>(257);
    if (srcMemberDict==0) srcMemberDict = new QIntDict<MemberDef>(257);
    srcDefDict->insert(line,d);
    if (md) srcMemberDict->insert(line,md);
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    md?md->name().data():"<none>",md?md->anchor().data():"<none>",line,name().data());
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
  //printf("%p: FileDef::addUsingDirective: %s:%d\n",this,name().data(),usingDirList->count());
}

NamespaceSDict *FileDef::getUsedNamespaces() const 
{ 
  //printf("%p: FileDef::getUsedNamespace: %s:%d\n",this,name().data(),usingDirList?usingDirList->count():0);
  return usingDirList; 
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
                                   bool imported,bool indirect)
{
  //printf("FileDef::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && (!includeDict || includeDict->find(iName)==0))
  {
    if (includeDict==0)
    {
      includeDict   = new QDict<IncludeInfo>(61);
      includeList   = new QList<IncludeInfo>;
      includeList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = indirect;
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
          // add using directives
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
              if (usingDirList->find(nd->qualifiedName())==0) // not yet added
              {
                usingDirList->prepend(nd->qualifiedName(),nd);
              }
            }
          }
          // add using declarations
          SDict<Definition> *udl = ii->fileDef->usingDeclList;
          if (udl)
          {
            SDict<Definition>::Iterator udi(*udl);
            Definition *d;
            for (udi.toLast();(d=udi.current());--udi)
            {
              //printf("Adding using declaration %s\n",d->name().data());
              if (usingDeclList==0)
              {
                usingDeclList = new SDict<Definition>(17);
              }
              if (usingDeclList->find(d->qualifiedName())==0)
              {
                usingDeclList->prepend(d->qualifiedName(),d);
              }
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
  if (!iName.isEmpty() && (includedByDict==0 || includedByDict->find(iName)==0))
  {
    if (includedByDict==0)
    {
      includedByDict = new QDict<IncludeInfo>(61);
      includedByList = new QList<IncludeInfo>;
      includedByList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = FALSE;
    includedByList->append(ii);
    includedByDict->insert(iName,ii);
  }
}

bool FileDef::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return includeDict!=0 && includeDict->find(name)!=0;
}

bool FileDef::generateSourceFile() const 
{ 
  static bool sourceBrowser = Config_getBool("SOURCE_BROWSER");
  static bool verbatimHeaders = Config_getBool("VERBATIM_HEADERS");
  QCString extension = name().right(4);
  return !isReference() && 
         (sourceBrowser || 
           (verbatimHeaders && guessSection(name())==Entry::HEADER_SEC) 
         ) &&
         extension!=".doc" && extension!=".txt" && extension!=".dox" &&
         extension!=".md" && name().right(9)!=".markdown";  
}


void FileDef::addListReferences()
{
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
               getOutputFileBase(),
               theTranslator->trFile(TRUE,TRUE),
               getOutputFileBase(),name(),
               0
              );
  }
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->addListReferences(this);
    }
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
    gd = new GroupDef("[generated]",
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
      Doxygen::groupSDict->append(root->path(),gd);
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
  Directory *root=new Directory(0,"root");
  root->setLast(TRUE);
  FileNameListIterator fnli(*Doxygen::inputNameList); 
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
         name().right(4)==".dox" ||
         name().right(3)==".md"  ||
         name().right(9)==".markdown";
}

void FileDef::acquireFileVersion()
{
  QCString vercmd = Config_getString("FILE_VERSION_FILTER");
  if (!vercmd.isEmpty() && !filepath.isEmpty() && filepath!="generated") 
  {
    msg("Version of %s : ",filepath.data());
    QCString cmd = vercmd+" \""+filepath+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",cmd.data());
    FILE *f=portable_popen(cmd,"r");
    if (!f)
    {
      err("error: could not execute %s\n",vercmd.data());
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead = (int)fread(buf,1,bufSize,f);
    portable_pclose(f);
    if (numRead>0 && !(fileVersion=QCString(buf,numRead).stripWhiteSpace()).isEmpty())
    {
      msg("%s\n",fileVersion.data());
    }
    else 
    {
      msg("no version available\n");
    }
  }
}


QCString FileDef::getSourceFileBase() const
{ 
  if (Htags::useHtags)
  {
    return Htags::path2URL(filepath);
  }
  else
  {
    return convertNameToFile(diskname)+"_source"; 
  }
}

/*! Returns the name of the verbatim copy of this file (if any). */
QCString FileDef::includeName() const 
{ 
  if (Htags::useHtags)
  {
    return Htags::path2URL(filepath);
  }
  else
  {
    return convertNameToFile(diskname)+"_source"; 
  }
}

MemberList *FileDef::createMemberList(MemberListType lt)
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

void FileDef::addMemberToList(MemberListType lt,MemberDef *md)
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
  if (lt&MemberListType_documentationLists)
  {
    ml->setInFile(TRUE);
  }
  if (ml->listType()&MemberListType_declarationLists) md->setSectionList(this,ml);
}

void FileDef::sortMemberLists()
{
  MemberList *ml = m_memberLists.first();
  while (ml)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
    ml = m_memberLists.next();
  }
}

MemberList *FileDef::getMemberList(MemberListType lt) const
{
  FileDef *that = (FileDef*)this;
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

void FileDef::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  static bool optVhdl = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  MemberList * ml = getMemberList(lt);
  if (ml) 
  {
    if (optVhdl) // use specific declarations function
    {

      VhdlDocGen::writeVhdlDeclarations(ml,ol,0,0,this,0);
    }
    else
    {
      ml->writeDeclarations(ol,0,0,this,0,title,0,definitionType());
    }
  }
}

void FileDef::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

bool FileDef::isLinkableInProject() const
{
  static bool showFiles = Config_getBool("SHOW_FILES");
  return hasDocumentation() && !isReference() && showFiles;
}

#if 0
bool FileDef::includes(FileDef *incFile,QDict<FileDef> *includedFiles) const
{
  //printf("%s::includes(%s)\n",name().data(),incFile->name().data());
  if (incFile==this) return TRUE;
  includedFiles->insert(absFilePath(),this);
  if (includeList)
  {
    QListIterator<IncludeInfo> ili(*includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      //printf("ii=%s\n",ii->includeName.data());
      if ((ii->fileDef && 
           includedFiles->find(ii->fileDef->absFilePath())==0 &&
           ii->fileDef->includes(incFile,includedFiles)
          ) ||
          incFile->absFilePath()==ii->includeName
         ) return TRUE;
    }
  }
  return FALSE;
}

bool FileDef::includesByName(const QCString &fileName) const
{
  if (includeList)
  {
    QListIterator<IncludeInfo> ili(*includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      //printf("ii=%s\n",ii->includeName.data());
      if (fileName==ii->includeName) return TRUE;
    }
  }
  return FALSE;
}
#endif
