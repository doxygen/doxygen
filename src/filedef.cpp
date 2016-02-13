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
    virtual void writeTooltip(const char *, const DocLinkInfo &, const char *,
                              const char *, const SourceLinkInfo &, const SourceLinkInfo &
                             ) {}
    virtual void writeLineNumber(const char *,const char *,
                                 const char *,int) {}
    virtual void startCodeLine(bool) {}
    virtual void endCodeLine() {}
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
  m_path=p;
  m_filePath=m_path+nm;
  m_fileName=nm;
  setDiskName(dn?dn:nm);
  setReference(lref);
  m_classSDict        = 0;
  m_includeList       = 0;
  m_includeDict       = 0; 
  m_includedByList    = 0;
  m_includedByDict    = 0; 
  m_namespaceSDict    = 0; 
  m_srcDefDict        = 0;
  m_srcMemberDict     = 0;
  m_usingDirList      = 0;
  m_usingDeclList     = 0;
  m_package           = 0;
  m_isSource          = guessSection(nm)==Entry::SOURCE_SEC; 
  m_docname           = nm;
  m_dir               = 0;
  if (Config_getBool(FULL_PATH_NAMES))
  {
    m_docname.prepend(stripFromPath(m_path.copy()));
  }
  setLanguage(getLanguageFromFileName(name()));
  m_memberGroupSDict = 0;
  acquireFileVersion();
  m_subGrouping=Config_getBool(SUBGROUPING);
}

/*! destroy the file definition */
FileDef::~FileDef()
{
  delete m_classSDict;
  delete m_includeDict;
  delete m_includeList;
  delete m_includedByDict;
  delete m_includedByList;
  delete m_namespaceSDict;
  delete m_srcDefDict;
  delete m_srcMemberDict;
  delete m_usingDirList;
  delete m_usingDeclList;
  delete m_memberGroupSDict;
}

void FileDef::setDiskName(const QCString &name)
{
  m_outputDiskName = convertNameToFile(name);
  m_inclDepFileName = convertNameToFile(name+"_incl");
  m_inclByDepFileName = convertNameToFile(name+"_dep_incl");
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
  if (m_memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  if (m_memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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

bool FileDef::hasDetailedDescription() const
{
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  static bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  return ((!briefDescription().isEmpty() && repeatBrief) || 
          !documentation().stripWhiteSpace().isEmpty() || // avail empty section
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef())
         );
}

void FileDef::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"file\">" << endl;
  tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "    <path>" << convertToXML(getPath()) << "</path>" << endl;
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << "</filename>" << endl;
  if (m_includeList && m_includeList->count()>0)
  {
    QListIterator<IncludeInfo> ili(*m_includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      if (!ii->indirect)
      {
        FileDef *fd=ii->fileDef;
        if (fd && fd->isLinkable() && !fd->isReference()) 
        {
          bool isIDLorJava = FALSE;
          SrcLangExt lang = fd->getLanguage();
          isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
          const char *locStr = (ii->local    || isIDLorJava) ? "yes" : "no";
          const char *impStr = (ii->imported || isIDLorJava) ? "yes" : "no";
          tagFile << "    <includes id=\"" 
                  << convertToXML(fd->getOutputFileBase()) << "\" "
                  << "name=\"" << convertToXML(fd->name()) << "\" "
                  << "local=\"" << locStr << "\" "
                  << "imported=\"" << impStr << "\">"
                  << convertToXML(ii->includeName)
                  << "</includes>" 
                  << endl;
        }
      }
    }
  }
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::File));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::FileClasses:
        {
          if (m_classSDict)
          {
            SDict<ClassDef>::Iterator ci(*m_classSDict);
            ClassDef *cd;
            for (ci.toFirst();(cd=ci.current());++ci)
            {
              if (cd->isLinkableInProject())
              {
                tagFile << "    <class kind=\"" << cd->compoundTypeString() <<
                  "\">" << convertToXML(cd->name()) << "</class>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::FileNamespaces:
        {
          if (m_namespaceSDict)
          {
            SDict<NamespaceDef>::Iterator ni(*m_namespaceSDict);
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
          if (m_memberGroupSDict)
          {
            MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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

void FileDef::writeDetailedDescription(OutputList &ol,const QCString &title)
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
        // ol.newParagraph(); // FIXME:PARA
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
    //printf("Writing source ref for file %s\n",name().data());
    if (Config_getBool(SOURCE_BROWSER)) 
    {
      //if Latex enabled and LATEX_SOURCE_CODE isn't -> skip, bug_738548
      ol.pushGeneratorState();
      if (ol.isEnabled(OutputGenerator::Latex) && !Config_getBool(LATEX_SOURCE_CODE))
      { 
        ol.disable(OutputGenerator::Latex);
      }
      if (ol.isEnabled(OutputGenerator::RTF) && !Config_getBool(RTF_SOURCE_CODE))
      { 
        ol.disable(OutputGenerator::RTF);
      }

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
      //Restore settings, bug_738548
      ol.popGeneratorState();
    }
    ol.endTextBlock();
  }
}

void FileDef::writeBriefDescription(OutputList &ol)
{
  if (!briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),this,0,
                       briefDescription(),TRUE,FALSE,0,TRUE,FALSE);

    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.writeDoc(rootNode,this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (Config_getBool(REPEAT_BRIEF) ||
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
    }
    delete rootNode;
  }
  ol.writeSynopsis();
}

void FileDef::writeIncludeFiles(OutputList &ol)
{
  if (m_includeList && m_includeList->count()>0)
  {
    ol.startTextBlock(TRUE);
    QListIterator<IncludeInfo> ili(*m_includeList);
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
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(INCLUDE_GRAPH)*/)
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,FALSE);
    if (incDepGraph.isTooBig())
    {
       warn_uncond("Include graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",name().data());
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
    //incDepGraph.writeGraph(Config_getString(HTML_OUTPUT),fd->getOutputFileBase());
  }
}

void FileDef::writeIncludedByGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(INCLUDED_BY_GRAPH)*/)
  {
    //printf("Graph for file %s\n",name().data());
    DotInclDepGraph incDepGraph(this,TRUE);
    if (incDepGraph.isTooBig())
    {
       warn_uncond("Included by graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",name().data());
    }
    else if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock(); 
      ol.disable(OutputGenerator::Man);
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclByDepGraph());
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString(HTML_OUTPUT),fd->getOutputFileBase());
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
  if (m_namespaceSDict) m_namespaceSDict->writeDeclaration(ol,title,isConstantGroup);
}

void FileDef::writeClassDeclarations(OutputList &ol,const QCString &title)
{
  // write list of classes
  if (m_classSDict) m_classSDict->writeDeclaration(ol,0,title,FALSE);
}

void FileDef::writeInlineClasses(OutputList &ol)
{
  // temporarily undo the disbling could be done by startMemberDocumentation()
  // as a result of setting SEPARATE_MEMBER_PAGES to YES; see bug730512
  bool isEnabled = ol.isEnabled(OutputGenerator::Html);
  ol.enable(OutputGenerator::Html);

  if (m_classSDict) m_classSDict->writeDocumentation(ol,this);

  // restore the initial state if needed
  if (!isEnabled) ol.disable(OutputGenerator::Html);
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
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void FileDef::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void FileDef::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  if (m_memberGroupSDict)
  {
    m_memberGroupSDict->sort();
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
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
    if ((lde->kind()==LayoutDocEntry::FileClasses && 
         m_classSDict && m_classSDict->declVisible()) || 
        (lde->kind()==LayoutDocEntry::FileNamespaces && 
         m_namespaceSDict && m_namespaceSDict->declVisible())
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

/*! Write the documentation page for this file to the file of output
    generators \a ol. 
*/
void FileDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //funcList->countDecMembers();
  
  //QCString fn = name();
  //if (Config_getBool(FULL_PATH_NAMES))
  //{
  //  fn.prepend(stripFromPath(getPath().copy()));
  //}

  //printf("WriteDocumentation diskname=%s\n",diskname.data());
  
  QCString versionTitle;
  if (!m_fileVersion.isEmpty())
  {
    versionTitle=("("+m_fileVersion+")");
  }
  QCString title = m_docname+versionTitle;
  QCString pageTitle=theTranslator->trFileReference(m_docname);

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

  if (!m_fileVersion.isEmpty())
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

  ol.endContents();

  endFileWithNavPath(this,ol);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
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
      if (md->getFileDef()==this && md->getNamespaceDef()==0 && md->isLinkable() && !md->isEnumValue())
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
void FileDef::writeSource(OutputList &ol,bool sameTu,QStrList &filesInSameTu)
{
  static bool generateTreeView  = Config_getBool(GENERATE_TREEVIEW);
  static bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  static bool latexSourceCode   = Config_getBool(LATEX_SOURCE_CODE);
  static bool rtfSourceCode     = Config_getBool(RTF_SOURCE_CODE);
  DevNullCodeDocInterface devNullIntf;
  QCString title = m_docname;
  if (!m_fileVersion.isEmpty())
  {
    title+=(" ("+m_fileVersion+")");
  }
  QCString pageTitle = theTranslator->trSourceFile(title);
  ol.disable(OutputGenerator::Man);
  if (!latexSourceCode) ol.disable(OutputGenerator::Latex);
  if (!rtfSourceCode) ol.disable(OutputGenerator::RTF);

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
    startTitle(ol,getSourceFileBase());
    ol.parseText(name());
    endTitle(ol,getSourceFileBase(),title);
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
    if (rtfSourceCode) ol.disable(OutputGenerator::RTF);
    ol.startTextLink(getOutputFileBase(),0);
    ol.parseText(theTranslator->trGotoDocumentation());
    ol.endTextLink();
    if (latexSourceCode) ol.enable(OutputGenerator::Latex);
    if (rtfSourceCode) ol.enable(OutputGenerator::RTF);
  }

  (void)sameTu;
  (void)filesInSameTu;
#if USE_LIBCLANG
  static bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (clangAssistedParsing && 
      (getLanguage()==SrcLangExt_Cpp || getLanguage()==SrcLangExt_ObjC))
  {
    ol.startCodeFragment();
    if (!sameTu)
    {
      ClangParser::instance()->start(absFilePath(),filesInSameTu);
    }
    else
    {
      ClangParser::instance()->switchToFile(absFilePath());
    }
    ClangParser::instance()->writeSources(ol,this);
    ol.endCodeFragment();
  }
  else
#endif
  {
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    ol.startCodeFragment();
    bool needs2PassParsing = 
        Doxygen::parseSourcesNeeded &&                // we need to parse (filtered) sources for cross-references
        !filterSourceFiles &&                         // but user wants to show sources as-is
        !getFileFilter(absFilePath(),TRUE).isEmpty(); // and there is a filter used while parsing

    if (needs2PassParsing)
    {
      // parse code for cross-references only (see bug707641)
      pIntf->parseCode(devNullIntf,0,
                       fileToString(absFilePath(),TRUE,TRUE),
                       getLanguage(),
                       FALSE,0,this
                      );
    }
    pIntf->parseCode(ol,0,
        fileToString(absFilePath(),filterSourceFiles,TRUE),
        getLanguage(),      // lang
        FALSE,              // isExampleBlock
        0,                  // exampleName
        this,               // fileDef
        -1,                 // startLine
        -1,                 // endLine
        FALSE,              // inlineFragment
        0,                  // memberDef
        TRUE,               // showLineNumbers
        0,                  // searchCtx
        !needs2PassParsing  // collectXRefs
        );
    ol.endCodeFragment();
  }
  ol.endContents();
  endFileWithNavPath(this,ol);
  ol.enableAll();
}

void FileDef::parseSource(bool sameTu,QStrList &filesInSameTu)
{
  static bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  DevNullCodeDocInterface devNullIntf;
  (void)sameTu;
  (void)filesInSameTu;
#if USE_LIBCLANG
  static bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (clangAssistedParsing && 
      (getLanguage()==SrcLangExt_Cpp || getLanguage()==SrcLangExt_ObjC))
  {
    if (!sameTu)
    {
      ClangParser::instance()->start(absFilePath(),filesInSameTu);
    }
    else
    {
      ClangParser::instance()->switchToFile(absFilePath());
    }
    ClangParser::instance()->writeSources(devNullIntf,this);
  }
  else
#endif
  {
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    pIntf->parseCode(
            devNullIntf,0,
            fileToString(absFilePath(),filterSourceFiles,TRUE),
            getLanguage(),
            FALSE,0,this
           );
  }
}

void FileDef::startParsing()
{
}

void FileDef::finishParsing()
{
  ClangParser::instance()->finish();
}

void FileDef::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ::addMembersToMemberGroup(ml,&m_memberGroupSDict,this);
    }
  }

  // add members inside sections to their groups
  if (m_memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  if (m_classSDict==0)
  {
    m_classSDict = new ClassSDict(17);
  }
  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    m_classSDict->inSort(cd->name(),cd);
  }
  else
  {
    m_classSDict->append(cd->name(),cd);
  }
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (nd->isHidden()) return;
  if (!nd->name().isEmpty() && 
      (m_namespaceSDict==0 || m_namespaceSDict->find(nd->name())==0))
  {
    if (m_namespaceSDict==0)
    {
      m_namespaceSDict = new NamespaceSDict;
    }
    if (Config_getBool(SORT_BRIEF_DOCS))
    {
      m_namespaceSDict->inSort(nd->name(),nd);
    }
    else
    {
      m_namespaceSDict->append(nd->name(),nd);
    }
  }
}

QCString FileDef::name() const 
{ 
  if (Config_getBool(FULL_PATH_NAMES)) 
    return m_fileName; 
  else 
    return Definition::name(); 
} 

void FileDef::addSourceRef(int line,Definition *d,MemberDef *md)
{
  //printf("FileDef::addSourceDef(%d,%p,%p)\n",line,d,md);
  if (d)
  {
    if (m_srcDefDict==0)    m_srcDefDict    = new QIntDict<Definition>(257);
    if (m_srcMemberDict==0) m_srcMemberDict = new QIntDict<MemberDef>(257);
    m_srcDefDict->insert(line,d);
    if (md) m_srcMemberDict->insert(line,md);
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    md?md->name().data():"<none>",md?md->anchor().data():"<none>",line,name().data());
  }
}

Definition *FileDef::getSourceDefinition(int lineNr) const
{
  Definition *result=0;
  if (m_srcDefDict)
  {
    result = m_srcDefDict->find(lineNr);
  }
  //printf("%s::getSourceDefinition(%d)=%s\n",name().data(),lineNr,result?result->name().data():"none");
  return result;
}

MemberDef *FileDef::getSourceMember(int lineNr) const
{
  MemberDef *result=0;
  if (m_srcMemberDict)
  {
    result = m_srcMemberDict->find(lineNr);
  }
  //printf("%s::getSourceMember(%d)=%s\n",name().data(),lineNr,result?result->name().data():"none");
  return result;
}


void FileDef::addUsingDirective(NamespaceDef *nd)
{
  if (m_usingDirList==0)
  {
    m_usingDirList = new NamespaceSDict;
  }
  if (m_usingDirList->find(nd->qualifiedName())==0)
  {
    m_usingDirList->append(nd->qualifiedName(),nd);
  }
  //printf("%p: FileDef::addUsingDirective: %s:%d\n",this,name().data(),usingDirList->count());
}

NamespaceSDict *FileDef::getUsedNamespaces() const 
{ 
  //printf("%p: FileDef::getUsedNamespace: %s:%d\n",this,name().data(),usingDirList?usingDirList->count():0);
  return m_usingDirList; 
}

void FileDef::addUsingDeclaration(Definition *d)
{
  if (m_usingDeclList==0)
  {
    m_usingDeclList = new SDict<Definition>(17);
  }
  if (m_usingDeclList->find(d->qualifiedName())==0)
  {
    m_usingDeclList->append(d->qualifiedName(),d);
  }
}

void FileDef::addIncludeDependency(FileDef *fd,const char *incName,bool local,
                                   bool imported,bool indirect)
{
  //printf("FileDef::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && (!m_includeDict || m_includeDict->find(iName)==0))
  {
    if (m_includeDict==0)
    {
      m_includeDict   = new QDict<IncludeInfo>(61);
      m_includeList   = new QList<IncludeInfo>;
      m_includeList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = indirect;
    m_includeList->append(ii);
    m_includeDict->insert(iName,ii);
  }
}

void FileDef::addIncludedUsingDirectives()
{
  if (visited) return;
  visited=TRUE;
  //printf("( FileDef::addIncludedUsingDirectives for file %s\n",name().data());

  if (m_includeList) // file contains #includes
  {
    {
      QListIterator<IncludeInfo> iii(*m_includeList);
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
      QListIterator<IncludeInfo> iii(*m_includeList);
      IncludeInfo *ii;
      // iterate through list from last to first
      for (iii.toLast();(ii=iii.current());--iii)
      {
        if (ii->fileDef && ii->fileDef!=this)
        {
          // add using directives
          NamespaceSDict *unl = ii->fileDef->m_usingDirList;
          if (unl)
          {
            NamespaceSDict::Iterator nli(*unl);
            NamespaceDef *nd;
            for (nli.toLast();(nd=nli.current());--nli)
            {
              // append each using directive found in a #include file
              if (m_usingDirList==0) m_usingDirList = new NamespaceSDict;
              //printf("Prepending used namespace %s to the list of file %s\n",
              //    nd->name().data(),name().data());
              if (m_usingDirList->find(nd->qualifiedName())==0) // not yet added
              {
                m_usingDirList->prepend(nd->qualifiedName(),nd);
              }
            }
          }
          // add using declarations
          SDict<Definition> *udl = ii->fileDef->m_usingDeclList;
          if (udl)
          {
            SDict<Definition>::Iterator udi(*udl);
            Definition *d;
            for (udi.toLast();(d=udi.current());--udi)
            {
              //printf("Adding using declaration %s\n",d->name().data());
              if (m_usingDeclList==0)
              {
                m_usingDeclList = new SDict<Definition>(17);
              }
              if (m_usingDeclList->find(d->qualifiedName())==0)
              {
                m_usingDeclList->prepend(d->qualifiedName(),d);
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
  if (!iName.isEmpty() && (m_includedByDict==0 || m_includedByDict->find(iName)==0))
  {
    if (m_includedByDict==0)
    {
      m_includedByDict = new QDict<IncludeInfo>(61);
      m_includedByList = new QList<IncludeInfo>;
      m_includedByList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = FALSE;
    m_includedByList->append(ii);
    m_includedByDict->insert(iName,ii);
  }
}

bool FileDef::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return m_includeDict!=0 && m_includeDict->find(name)!=0;
}

bool FileDef::generateSourceFile() const 
{ 
  static bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  static bool verbatimHeaders = Config_getBool(VERBATIM_HEADERS);
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
    QList<ListItemInfo> *xrefItems = xrefListItems();
    addRefItem(xrefItems,
               getOutputFileBase(),
               theTranslator->trFile(TRUE,TRUE),
               getOutputFileBase(),name(),
               0,
               0
              );
  }
  if (m_memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
            root->children().getLast()->setLast(FALSE); 
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
      root->children().getLast()->setLast(FALSE); 
    }
    root->addChild(newBranch);
    return newBranch;
  }
}

static void mergeFileDef(Directory *root,FileDef *fd)
{
  QCString filePath = fd->absFilePath();
  //printf("merging %s\n",filePath.data());
  Directory *dirNode = findDirNode(root,filePath);
  if (!dirNode->children().isEmpty())
  {
    dirNode->children().getLast()->setLast(FALSE); 
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
  if (m_usingDirList)
  {
    NamespaceSDict::Iterator nli(*m_usingDirList);
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
  QCString vercmd = Config_getString(FILE_VERSION_FILTER);
  if (!vercmd.isEmpty() && !m_filePath.isEmpty() &&
      m_filePath!="generated" && m_filePath!="graph_legend")
  {
    msg("Version of %s : ",m_filePath.data());
    QCString cmd = vercmd+" \""+m_filePath+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
    FILE *f=portable_popen(cmd,"r");
    if (!f)
    {
      err("could not execute %s\n",vercmd.data());
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead = (int)fread(buf,1,bufSize-1,f);
    portable_pclose(f);
    if (numRead>0 && numRead<bufSize)
    {
      buf[numRead]='\0';
      m_fileVersion=QCString(buf,numRead).stripWhiteSpace();
      if (!m_fileVersion.isEmpty())
      {
        msg("%s\n",m_fileVersion.data());
        return;
      }
    }
    msg("no version available\n");
  }
}


QCString FileDef::getSourceFileBase() const
{
  if (Htags::useHtags)
  {
    return Htags::path2URL(m_filePath);
  }
  else
  {
    return m_outputDiskName+"_source";
  }
}

QCString FileDef::getOutputFileBase() const
{
  return m_outputDiskName;
}

/*! Returns the name of the verbatim copy of this file (if any). */
QCString FileDef::includeName() const 
{ 
  return getSourceFileBase();
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
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  static bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
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
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (;(ml=mli.current());++mli)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }
}

MemberList *FileDef::getMemberList(MemberListType lt) const
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (;(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  return 0;
}

void FileDef::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  static bool optVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  MemberList * ml = getMemberList(lt);
  if (ml) 
  {
    if (optVhdl) // use specific declarations function
    {

      VhdlDocGen::writeVhdlDeclarations(ml,ol,0,0,this,0);
    }
    else
    {
      ml->writeDeclarations(ol,0,0,this,0,title,0);
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
  static bool showFiles = Config_getBool(SHOW_FILES);
  return hasDocumentation() && !isReference() && showFiles;
}

static void getAllIncludeFilesRecursively(
    QDict<void> *filesVisited,const FileDef *fd,QStrList &incFiles)
{
  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> iii(*fd->includeFileList());
    IncludeInfo *ii;
    for (iii.toFirst();(ii=iii.current());++iii)
    {
      if (ii->fileDef && !ii->fileDef->isReference() &&
          !filesVisited->find(ii->fileDef->absFilePath()))
      {
        //printf("FileDef::addIncludeDependency(%s)\n",ii->fileDef->absFilePath().data());
        incFiles.append(ii->fileDef->absFilePath());
        filesVisited->insert(ii->fileDef->absFilePath(),(void*)0x8);
        getAllIncludeFilesRecursively(filesVisited,ii->fileDef,incFiles);
      }
    }
  }
}

void FileDef::getAllIncludeFilesRecursively(QStrList &incFiles) const
{
  QDict<void> includes(257);
  ::getAllIncludeFilesRecursively(&includes,this,incFiles);
}

QCString FileDef::title() const
{
  return theTranslator->trFileReference(name());
}

QCString FileDef::fileVersion() const
{
  return m_fileVersion;
}

QCString FileDef::includeDependencyGraphFileName() const
{
  return m_inclDepFileName;
}

QCString FileDef::includedByDependencyGraphFileName() const
{
  return m_inclByDepFileName;
}

