/******************************************************************************
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

#include <qregexp.h>

#include "pagedef.h"
#include "groupdef.h"
#include "docparser.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "doxygen.h"
#include "language.h"
#include "namespacedef.h"
#include "reflist.h"

PageDef::PageDef(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : Definition(f,l,1,n), m_title(t)
{
  setDocumentation(d,f,l);
  m_subPageDict = new PageSDict(7);
  m_pageScope = 0;
  m_nestingLevel = 0;
  m_fileName = ::convertNameToFile(n,FALSE,TRUE);
  m_showToc = FALSE;
}

PageDef::~PageDef()
{
  delete m_subPageDict;
}

void PageDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
}

GroupDef *PageDef::getGroupDef() const 
{ 
  GroupList *groups = partOfGroups();
  return groups!=0 ? groups->getFirst() : 0; 
}

QCString PageDef::getOutputFileBase() const 
{ 
  if (getGroupDef()) 
    return getGroupDef()->getOutputFileBase();
  else 
    return m_fileName; 
}

void PageDef::setFileName(const char *name)
{
  m_fileName = name;
}

void PageDef::addInnerCompound(Definition *def)
{
  if (def->definitionType()==Definition::TypePage)
  {
    PageDef *pd = (PageDef*)def;
    m_subPageDict->append(pd->name(),pd);
    def->setOuterScope(this);
    if (this==Doxygen::mainPage)
    {
      pd->setNestingLevel(m_nestingLevel);
    }
    else
    {
      pd->setNestingLevel(m_nestingLevel+1);
    }
  }
}

bool PageDef::hasParentPage() const
{
  return getOuterScope() && 
         getOuterScope()->definitionType()==Definition::TypePage;
}

void PageDef::writeTagFile(FTextStream &tagFile)
{
  bool found = name()=="citelist";
  QDictIterator<RefList> rli(*Doxygen::xrefLists);
  RefList *rl;
  for (rli.toFirst();(rl=rli.current()) && !found;++rli)
  {
    if (rl->listName()==name())
    {
      found=TRUE;
      break;
    }
  }
  if (!found) // not one of the generated related pages
  {
    tagFile << "  <compound kind=\"page\">" << endl;
    tagFile << "    <name>" << name() << "</name>" << endl;
    tagFile << "    <title>" << convertToXML(title()) << "</title>" << endl;
    tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << "</filename>" << endl;
    writeDocAnchorsToTagFile(tagFile);
    tagFile << "  </compound>" << endl;
  }
}

void PageDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);

  //outputList->disable(OutputGenerator::Man);
  QCString pageName,manPageName;
  pageName    = escapeCharsInString(name(),FALSE,TRUE);
  manPageName = escapeCharsInString(name(),TRUE,TRUE);

  //printf("PageDef::writeDocumentation: %s\n",getOutputFileBase().data());

  ol.pushGeneratorState();
  //1.{ 

  if (m_nestingLevel>0 
      //&& // a sub page
      //(Doxygen::mainPage==0 || getOuterScope()!=Doxygen::mainPage) // and not a subpage of the mainpage
     )
  {
    // do not generate sub page output for RTF and LaTeX, as these are
    // part of their parent page
    ol.disableAll();
    ol.enable(OutputGenerator::Man);
    ol.enable(OutputGenerator::Html);
  }

  ol.pushGeneratorState();
  //2.{ 
  ol.disableAllBut(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),manPageName,title(),HLI_Pages,!generateTreeView);
  ol.enableAll();
  ol.disable(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),pageName,title(),HLI_Pages,!generateTreeView);
  ol.popGeneratorState();
  //2.} 

  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope && !Config_getBool(DISABLE_INDEX))
    {
      getOuterScope()->writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }
  SectionInfo *si=Doxygen::sectionDict->find(name());

  // save old generator state and write title only to Man generator
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputGenerator::Man);
  ol.startTitleHead(manPageName);
  ol.endTitleHead(manPageName, manPageName);
  if (si)
  {
    ol.generateDoc(docFile(),docLine(),this,0,si->title,TRUE,FALSE,0,TRUE,FALSE);
    ol.endSection(si->label,si->type);
  }
  ol.popGeneratorState();
  //2.}

  // for Latex the section is already generated as a chapter in the index!
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  ol.disable(OutputGenerator::Man);
  if (!title().isEmpty() && !name().isEmpty() && si!=0)
  {
    //ol.startSection(si->label,si->title,si->type);
    startTitle(ol,getOutputFileBase(),this);
    ol.generateDoc(docFile(),docLine(),this,0,si->title,TRUE,FALSE,0,TRUE,FALSE);
    //stringToSearchIndex(getOutputFileBase(),
    //                    theTranslator->trPage(TRUE,TRUE)+" "+si->title,
    //                    si->title);
    //ol.endSection(si->label,si->type);
    endTitle(ol,getOutputFileBase(),name());
  }
  ol.startContents();
  ol.popGeneratorState();
  //2.}

  if (m_showToc && hasSections())
  {
    writeToc(ol);
  }

  writePageDocumentation(ol);

  if (generateTreeView && getOuterScope()!=Doxygen::globalScope && !Config_getBool(DISABLE_INDEX))
  {
    ol.endContents();
    endFileWithNavPath(getOuterScope(),ol);
  }
  else
  {
    endFile(ol);
  }

  ol.popGeneratorState();
  //1.}

  Doxygen::indexList->addIndexItem(this,0,0,filterTitle(title()));
}

void PageDef::writePageDocumentation(OutputList &ol)
{

  bool markdownEnabled = Doxygen::markdownSupport;
  if (getLanguage()==SrcLangExt_Markdown)
  {
    Doxygen::markdownSupport = TRUE;
  }

  ol.startTextBlock();
  ol.generateDoc(
      docFile(),           // fileName
      docLine(),           // startLine
      this,                // context
      0,                   // memberdef
      documentation()+inbodyDocumentation(), // docStr
      TRUE,                // index words
      FALSE                // not an example
      );
  ol.endTextBlock();

  Doxygen::markdownSupport = markdownEnabled;

  if (hasSubPages())
  {
    // for printed documentation we write subpages as section's of the
    // parent page.
    ol.pushGeneratorState();
    ol.disableAll();
    ol.enable(OutputGenerator::Latex);
    ol.enable(OutputGenerator::RTF);

    PageSDict::Iterator pdi(*m_subPageDict);
    PageDef *subPage=pdi.toFirst();
    for (pdi.toFirst();(subPage=pdi.current());++pdi)
    {
      SectionInfo::SectionType sectionType = SectionInfo::Paragraph;
      switch (m_nestingLevel)
      {
        case  0: sectionType = SectionInfo::Page;          break;
        case  1: sectionType = SectionInfo::Section;       break;
        case  2: sectionType = SectionInfo::Subsection;    break;
        case  3: sectionType = SectionInfo::Subsubsection; break;
        default: sectionType = SectionInfo::Paragraph;     break;
      }
      QCString title = subPage->title();
      if (title.isEmpty()) title = subPage->name();
      ol.startSection(subPage->name(),title,sectionType);
      ol.parseText(title);
      ol.endSection(subPage->name(),sectionType);
      Doxygen::subpageNestingLevel++;
      subPage->writePageDocumentation(ol);
      Doxygen::subpageNestingLevel--;
    }

    ol.popGeneratorState();
  }
}

bool PageDef::visibleInIndex() const
{
  static bool externalPages = Config_getBool(EXTERNAL_PAGES);
  return // not part of a group
         !getGroupDef() && 
         // not an externally defined page
         (!isReference() || externalPages) 
         ;
}

bool PageDef::documentedPage() const
{
   return // not part of a group
          !getGroupDef() && 
          // not an externally defined page
          !isReference();
}

bool PageDef::hasSubPages() const
{
  return m_subPageDict->count()>0;
}

void PageDef::setNestingLevel(int l)
{
  m_nestingLevel = l;
}

void PageDef::setShowToc(bool b)
{
  m_showToc = b;
}

