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
#include "definitionimpl.h"

//------------------------------------------------------------------------------------------

class PageDefImpl : public DefinitionImpl, public PageDef
{
  public:
    PageDefImpl(const char *f,int l,const char *n,const char *d,const char *t);
    virtual ~PageDefImpl();

    virtual void setFileName(const char *name);
    virtual void setLocalToc(const LocalToc &tl);
    virtual void setShowLineNo(bool);
    virtual DefType definitionType() const { return TypePage; }
    virtual bool isLinkableInProject() const { return /*hasDocumentation() &&*/ !isReference(); }
    virtual bool isLinkable() const { return isLinkableInProject() || isReference(); }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual void findSectionsInDocumentation();
    virtual QCString title() const { return m_title; }
    virtual GroupDef *  getGroupDef() const;
    virtual PageSDict * getSubPages() const { return m_subPageDict; }
    virtual void addInnerCompound(Definition *d);
    virtual bool visibleInIndex() const;
    virtual bool documentedPage() const;
    virtual bool hasSubPages() const;
    virtual bool hasParentPage() const;
    virtual bool hasTitle() const;
    virtual LocalToc localToc() const { return m_localToc; }
    virtual void setPageScope(Definition *d){ m_pageScope = d; }
    virtual Definition *getPageScope() const { return m_pageScope; }
    virtual QCString displayName(bool=TRUE) const { return hasTitle() ? m_title : DefinitionImpl::name(); }
    virtual bool showLineNo() const;
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeTagFile(FTextStream &);
    virtual void setNestingLevel(int l);
    virtual void writePageDocumentation(OutputList &ol);

  private:
    QCString m_fileName;
    QCString m_title;
    PageSDict *m_subPageDict;                 // list of pages in the group
    Definition *m_pageScope;
    int m_nestingLevel;
    LocalToc m_localToc;
    bool m_showLineNo;
};

PageDef *createPageDef(const char *f,int l,const char *n,const char *d,const char *t)
{
  return new PageDefImpl(f,l,n,d,t);
}

//------------------------------------------------------------------------------------------

PageDefImpl::PageDefImpl(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : DefinitionImpl(f,l,1,n), m_title(t)
{
  setDocumentation(d,f,l);
  m_subPageDict = new PageSDict(7);
  m_pageScope = 0;
  m_nestingLevel = 0;
  m_fileName = ::convertNameToFile(n,FALSE,TRUE);
  m_showLineNo = FALSE;
}

PageDefImpl::~PageDefImpl()
{
  delete m_subPageDict;
}

void PageDefImpl::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,docFile());
}

GroupDef *PageDefImpl::getGroupDef() const 
{ 
  GroupList *groups = partOfGroups();
  return groups!=0 ? groups->getFirst() : 0; 
}

QCString PageDefImpl::getOutputFileBase() const 
{ 
  if (getGroupDef()) 
    return getGroupDef()->getOutputFileBase();
  else 
    return m_fileName; 
}

void PageDefImpl::setFileName(const char *name)
{
  m_fileName = name;
}

void PageDefImpl::addInnerCompound(Definition *def)
{
  if (def->definitionType()==Definition::TypePage)
  {
    PageDef *pd = dynamic_cast<PageDef*>(def);
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

bool PageDefImpl::hasParentPage() const
{
  return getOuterScope() && 
         getOuterScope()->definitionType()==Definition::TypePage;
}

void PageDefImpl::writeTagFile(FTextStream &tagFile)
{
  bool found = name()=="citelist";
  for (RefListManager::Ptr &rl : RefListManager::instance())
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

void PageDefImpl::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);

  //outputList->disable(OutputGenerator::Man);
  QCString pageName,manPageName;
  pageName    = escapeCharsInString(name(),FALSE,TRUE);
  manPageName = escapeCharsInString(name(),TRUE,TRUE);

  //printf("PageDefImpl::writeDocumentation: %s\n",getOutputFileBase().data());

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
  const SectionInfo *si=SectionManager::instance().find(name());

  // save old generator state and write title only to Man generator
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputGenerator::Man);
  ol.startTitleHead(manPageName);
  ol.endTitleHead(manPageName, manPageName);
  if (si)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Man);
    ol.writeString(" - ");
    ol.popGeneratorState();

    if (si->title() != manPageName)
    {
      ol.generateDoc(docFile(),docLine(),this,0,si->title(),TRUE,FALSE,0,TRUE,FALSE);
      ol.endSection(si->label(),si->type());
    }
  }
  ol.popGeneratorState();
  //2.}

  // for Latex the section is already generated as a chapter in the index!
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::Docbook);
  ol.disable(OutputGenerator::RTF);
  ol.disable(OutputGenerator::Man);
  if (hasTitle() && !name().isEmpty() && si!=0)
  {
    ol.startPageDoc(si->title());
    //ol.startSection(si->label,si->title,si->type);
    startTitle(ol,getOutputFileBase(),this);
    ol.generateDoc(docFile(),docLine(),this,0,si->title(),TRUE,FALSE,0,TRUE,FALSE);
    //stringToSearchIndex(getOutputFileBase(),
    //                    theTranslator->trPage(TRUE,TRUE)+" "+si->title,
    //                    si->title);
    //ol.endSection(si->label,si->type);
    endTitle(ol,getOutputFileBase(),name());
  }
  else
    ol.startPageDoc("");
  ol.startContents();
  ol.popGeneratorState();
  //2.}

  if ((m_localToc.isHtmlEnabled() || m_localToc.isLatexEnabled() || m_localToc.isDocbookEnabled()) && hasSections())
  {
    writeToc(ol, m_localToc);
  }

  writePageDocumentation(ol);
  ol.endContents();
  ol.endPageDoc();

  if (generateTreeView && getOuterScope()!=Doxygen::globalScope && !Config_getBool(DISABLE_INDEX))
  {
    endFileWithNavPath(getOuterScope(),ol);
  }
  else
  {
    endFile(ol,FALSE,TRUE);
  }

  ol.popGeneratorState();
  //1.}

  Doxygen::indexList->addIndexItem(this,0,0,filterTitle(title()));
}

void PageDefImpl::writePageDocumentation(OutputList &ol)
{

  bool markdownEnabled = Doxygen::markdownSupport;
  if (getLanguage()==SrcLangExt_Markdown)
  {
    Doxygen::markdownSupport = TRUE;
  }

  ol.startTextBlock();
  QCString docStr = documentation()+inbodyDocumentation();
  if (hasBriefDescription() && !SectionManager::instance().find(name()))
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Man);
    ol.writeString(" - ");
    ol.popGeneratorState();
  }
  ol.generateDoc(
      docFile(),           // fileName
      docLine(),           // startLine
      this,                // context
      0,                   // memberdef
      docStr,              // docStr
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
    ol.enable(OutputGenerator::Docbook);
    ol.enable(OutputGenerator::RTF);

    PageSDict::Iterator pdi(*m_subPageDict);
    PageDef *subPage=pdi.toFirst();
    for (pdi.toFirst();(subPage=pdi.current());++pdi)
    {
      SectionType sectionType = SectionType::Paragraph;
      switch (m_nestingLevel)
      {
        case  0: sectionType = SectionType::Page;          break;
        case  1: sectionType = SectionType::Section;       break;
        case  2: sectionType = SectionType::Subsection;    break;
        case  3: sectionType = SectionType::Subsubsection; break;
        default: sectionType = SectionType::Paragraph;     break;
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

bool PageDefImpl::visibleInIndex() const
{
  static bool externalPages = Config_getBool(EXTERNAL_PAGES);
  return // not part of a group
         !getGroupDef() && 
         // not an externally defined page
         (!isReference() || externalPages) 
         ;
}

bool PageDefImpl::documentedPage() const
{
   return // not part of a group
          !getGroupDef() && 
          // not an externally defined page
          !isReference();
}

bool PageDefImpl::hasSubPages() const
{
  return m_subPageDict->count()>0;
}

void PageDefImpl::setNestingLevel(int l)
{
  m_nestingLevel = l;
}

void PageDefImpl::setLocalToc(const LocalToc &lt)
{
  m_localToc = lt;
}

void PageDefImpl::setShowLineNo(bool b)
{
  m_showLineNo = b;
}

bool PageDefImpl::showLineNo() const
{
  return m_showLineNo;
}

bool PageDefImpl::hasTitle() const
{
  return !m_title.isEmpty() && m_title.lower()!="notitle";
}

