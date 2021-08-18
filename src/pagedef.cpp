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

class PageDefImpl : public DefinitionMixin<PageDef>
{
  public:
    PageDefImpl(const QCString &f,int l,const QCString &n,const QCString &d,const QCString &t);
    virtual ~PageDefImpl();

    virtual void setFileName(const QCString &name);
    virtual void setLocalToc(const LocalToc &tl);
    virtual void setShowLineNo(bool);
    virtual DefType definitionType() const { return TypePage; }
    virtual CodeSymbolType codeSymbolType() const { return CodeSymbolType::Default; }
    virtual bool isLinkableInProject() const { return /*hasDocumentation() &&*/ !isReference(); }
    virtual bool isLinkable() const { return isLinkableInProject() || isReference(); }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual void findSectionsInDocumentation();
    virtual QCString title() const { return m_title; }
    virtual const GroupDef * getGroupDef() const;
    virtual const PageLinkedRefMap &getSubPages() const { return m_subPages; }
    virtual void addInnerCompound(const Definition *d);
    virtual bool visibleInIndex() const;
    virtual bool documentedPage() const;
    virtual bool hasSubPages() const;
    virtual bool hasParentPage() const;
    virtual bool hasTitle() const;
    virtual LocalToc localToc() const { return m_localToc; }
    virtual void setPageScope(Definition *d){ m_pageScope = d; }
    virtual Definition *getPageScope() const { return m_pageScope; }
    virtual QCString displayName(bool=TRUE) const { return hasTitle() ? m_title : DefinitionMixin::name(); }
    virtual bool showLineNo() const;
    virtual void setTitle(const QCString &title);
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeTagFile(TextStream &);
    virtual void setNestingLevel(int l);
    virtual void writePageDocumentation(OutputList &ol) const;

  private:
    QCString m_fileName;
    QCString m_title;
    PageLinkedRefMap m_subPages;                 // list of pages in the group
    Definition *m_pageScope;
    int m_nestingLevel;
    LocalToc m_localToc;
    bool m_showLineNo;
};

PageDef *createPageDef(const QCString &f,int l,const QCString &n,const QCString &d,const QCString &t)
{
  return new PageDefImpl(f,l,n,d,t);
}

//------------------------------------------------------------------------------------------

PageDefImpl::PageDefImpl(const QCString &f,int l,const QCString &n,
                 const QCString &d,const QCString &t)
 : DefinitionMixin(f,l,1,n), m_title(!t.isEmpty() ? t : n)
{
  setDocumentation(d,f,l);
  m_pageScope = 0;
  m_nestingLevel = 0;
  m_fileName = ::convertNameToFile(n,FALSE,TRUE);
  m_showLineNo = FALSE;
}

PageDefImpl::~PageDefImpl()
{
}

void PageDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
}

const GroupDef *PageDefImpl::getGroupDef() const
{
  return !partOfGroups().empty() ? partOfGroups().front() : 0;
}

QCString PageDefImpl::getOutputFileBase() const
{
  if (getGroupDef())
    return getGroupDef()->getOutputFileBase();
  else
    return m_fileName;
}

void PageDefImpl::setFileName(const QCString &name)
{
  m_fileName = name;
}

void PageDefImpl::addInnerCompound(const Definition *def)
{
  if (def->definitionType()==Definition::TypePage)
  {
    PageDef *pd = const_cast<PageDef*>(toPageDef(def));
    if (pd)
    {
      m_subPages.add(pd->name(),pd);
      pd->setOuterScope(this);
      if (this==Doxygen::mainPage.get())
      {
        pd->setNestingLevel(m_nestingLevel);
      }
      else
      {
        pd->setNestingLevel(m_nestingLevel+1);
      }
    }
  }
}

bool PageDefImpl::hasParentPage() const
{
  return getOuterScope() &&
         getOuterScope()->definitionType()==Definition::TypePage;
}

void PageDefImpl::writeTagFile(TextStream &tagFile)
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
    tagFile << "  <compound kind=\"page\">\n";
    tagFile << "    <name>" << name() << "</name>\n";
    tagFile << "    <title>" << convertToXML(title()) << "</title>\n";
    tagFile << "    <filename>" << convertToXML(getOutputFileBase())<< Doxygen::htmlFileExtension << "</filename>\n";
    writeDocAnchorsToTagFile(tagFile);
    tagFile << "  </compound>\n";
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
      DefinitionMutable *outerScope = toDefinitionMutable(getOuterScope());
      if (outerScope)
      {
        outerScope->writeNavigationPath(ol);
      }
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
      ol.generateDoc(docFile(),getStartBodyLine(),this,0,si->title(),TRUE,FALSE,
                     QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
    ol.generateDoc(docFile(),getStartBodyLine(),this,0,si->title(),TRUE,FALSE,
                   QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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

  Doxygen::indexList->addIndexItem(this,0,QCString(),filterTitle(title().str()));
}

void PageDefImpl::writePageDocumentation(OutputList &ol) const
{
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
      FALSE,               // not an example
      QCString(),                   // exampleName
      FALSE,               // singleLine
      FALSE,               // linkFromIndex
      TRUE                 // markdown support
      );
  ol.endTextBlock();

  if (hasSubPages())
  {
    // for printed documentation we write subpages as section's of the
    // parent page.
    ol.pushGeneratorState();
    ol.disableAll();
    ol.enable(OutputGenerator::Latex);
    ol.enable(OutputGenerator::Docbook);
    ol.enable(OutputGenerator::RTF);

    for (const auto &subPage : m_subPages)
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
  return !m_subPages.empty();
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

void PageDefImpl::setTitle(const QCString &title)
{
  m_title = title;
}

// --- Cast functions

PageDef *toPageDef(Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(PageDefImpl))
  {
    return static_cast<PageDef*>(d);
  }
  else
  {
    return 0;
  }
}

const PageDef *toPageDef(const Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(PageDefImpl))
  {
    return static_cast<const PageDef*>(d);
  }
  else
  {
    return 0;
  }
}

