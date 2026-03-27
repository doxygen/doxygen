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
#include "indexlist.h"

//------------------------------------------------------------------------------------------

class PageDefImpl : public DefinitionMixin<PageDef>
{
  public:
    PageDefImpl(const QCString &f,int l,const QCString &n,const QCString &d,const QCString &t);
    ~PageDefImpl() override;
    NON_COPYABLE(PageDefImpl)

    void setFileName(const QCString &name) override;
    void setLocalToc(const LocalToc &tl) override;
    void setShowLineNo(bool) override;
    DefType definitionType() const override { return TypePage; }
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Default; }
    bool isLinkableInProject() const override { return /*hasDocumentation() &&*/ !isReference(); }
    bool isLinkable() const override { return isLinkableInProject() || isReference(); }
    QCString getOutputFileBase() const override;
    QCString anchor() const override { return QCString(); }
    void findSectionsInDocumentation() override;
    QCString title() const override { return m_title; }
    const GroupDef * getGroupDef() const override;
    const PageLinkedRefMap &getSubPages() const override { return m_subPages; }
    void addInnerCompound(Definition *d) override;
    bool visibleInIndex() const override;
    bool documentedPage() const override;
    bool hasSubPages() const override;
    bool hasParentPage() const override;
    bool hasTitle() const override;
    LocalToc localToc() const override { return m_localToc; }
    void setPageScope(Definition *d) override { m_pageScope = d; }
    Definition *getPageScope() const override { return m_pageScope; }
    QCString displayName(bool=TRUE) const override { return hasTitle() ? m_title : DefinitionMixin::name(); }
    bool showLineNo() const override;
    void setTitle(const QCString &title) override;
    void writeDocumentation(OutputList &ol) override;
    void writeTagFile(TextStream &) override;
    void setNestingLevel(int l) override;
    void writePageDocumentation(OutputList &ol) const override;
    void addSectionsToIndex() override;
    void writePageNavigation(OutputList &ol) const override;
    void addListReferences() override;
    void addRequirementReferences() override;

  private:
    QCString m_fileName;
    QCString m_title;
    PageLinkedRefMap m_subPages;                 // list of pages in the group
    Definition *m_pageScope;
    int m_nestingLevel;
    LocalToc m_localToc;
    bool m_showLineNo;
};

std::unique_ptr<PageDef> createPageDef(const QCString &f,int l,const QCString &n,const QCString &d,const QCString &t)
{
  return std::make_unique<PageDefImpl>(f,l,n,d,t);
}

//------------------------------------------------------------------------------------------

PageDefImpl::PageDefImpl(const QCString &f,int l,const QCString &n,
                 const QCString &d,const QCString &t)
 : DefinitionMixin(f,l,1,n), m_title(!t.isEmpty() ? t : n)
{
  setDocumentation(d,f,l);
  m_pageScope = nullptr;
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
  docFindSections(inbodyDocumentation(),this,docFile());
}

const GroupDef *PageDefImpl::getGroupDef() const
{
  return !partOfGroups().empty() ? partOfGroups().front() : nullptr;
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

void PageDefImpl::addInnerCompound(Definition *def)
{
  if (def->definitionType()==Definition::TypePage)
  {
    PageDef *pd = toPageDef(def);
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

void PageDefImpl::addSectionsToIndex()
{
  const SectionRefs &sectionRefs = getSectionRefs();
  if (sectionRefs.empty()) return;
  //printf("PageDefImpl::addSectionsToIndex()\n");
  int level=1;
  for (auto it = sectionRefs.begin(); it!=sectionRefs.end(); ++it)
  {
    const SectionInfo *si = *it;
    SectionType type = si->type();
    if (type.isSection())
    {
      //printf("  level=%d title=%s\n",level,qPrint(si->title));
      int nextLevel = type.level();
      if (nextLevel>level)
      {
        for (int i=level;i<nextLevel;i++)
        {
          Doxygen::indexList->incContentsDepth();
        }
      }
      else if (nextLevel<level)
      {
        for (int i=nextLevel;i<level;i++)
        {
          Doxygen::indexList->decContentsDepth();
        }
      }
      QCString title = si->title();
      if (title.isEmpty()) title = si->label();
      title = parseCommentAsText(this,nullptr,title,si->fileName(),si->lineNr());
      QCString titleAsHtml = parseCommentAsHtml(this,nullptr,si->title(),si->fileName(),si->lineNr());
      // determine if there is a next level inside this item, but be aware of the anchor and table section references.
      auto it_next = std::next(it);
      bool isDir = (it_next!=sectionRefs.end()) ?  ((*it_next)->type().isSection() && (*it_next)->type().level() > nextLevel) : false;
      Doxygen::indexList->addContentsItem(isDir,               // isDir
                                          title,               // name
                                          getReference(),      // ref
                                          getOutputFileBase(), // file
                                          si->label(),         // anchor
                                          false,               // separateIndex
                                          true,                // addToNavIndex
                                          nullptr,             // def
                                          titleAsHtml);        // nameAsHtml
      level = nextLevel;
    }
  }
  while (level>1)
  {
    Doxygen::indexList->decContentsDepth();
    level--;
  }
}

void PageDefImpl::addListReferences()
{
  QCString name = getOutputFileBase();
  if (getGroupDef())
  {
    name = getGroupDef()->getOutputFileBase();
  }
  addRefItem(xrefListItems(),name,
             theTranslator->trPage(TRUE,TRUE),
             name,title(),QCString(),nullptr);
}

void PageDefImpl::addRequirementReferences()
{
  RequirementManager::instance().addRequirementRefsForSymbol(this);
}

bool PageDefImpl::hasParentPage() const
{
  return getOuterScope() &&
         getOuterScope()->definitionType()==Definition::TypePage;
}

void PageDefImpl::writeTagFile(TextStream &tagFile)
{
  bool found = name()=="citelist" || name()=="requirements";
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
    QCString fn = getOutputFileBase();
    addHtmlExtensionIfMissing(fn);
    tagFile << "  <compound kind=\"page\">\n";
    tagFile << "    <name>" << name() << "</name>\n";
    tagFile << "    <title>" << convertToXML(title()) << "</title>\n";
    tagFile << "    <filename>" << fn << "</filename>\n";
    for (const auto &subPage : m_subPages)
    {
      QCString sfn = subPage->getOutputFileBase();
      addHtmlExtensionIfMissing(sfn);
      tagFile << "    <subpage>" << sfn << "</subpage>\n";
    }
    writeDocAnchorsToTagFile(tagFile);
    tagFile << "  </compound>\n";
  }
}

void PageDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  int hierarchyLevel = -1; // Pages start at the root
  PageDef *pd = this;
  while (pd && pd->hasParentPage())
  {
    pd = dynamic_cast<PageDef *>(pd->getOuterScope());
    ++hierarchyLevel;
  }

  //outputList->disable(OutputType::Man);
  QCString pageName,manPageName;
  pageName    = escapeCharsInString(name(),FALSE,TRUE);
  manPageName = escapeCharsInString(name(),TRUE,TRUE);

  //printf("PageDefImpl::writeDocumentation: %s\n",getOutputFileBase().data());

  ol.pushGeneratorState();
  //1.{

  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputType::Man);
  startFile(ol,getOutputFileBase(),false,manPageName,title(),HighlightedItem::Pages,!generateTreeView,
            QCString() /* altSidebarName */, hierarchyLevel);
  ol.enableAll();
  ol.disable(OutputType::Man);
  startFile(ol,getOutputFileBase(),false,pageName,title(),HighlightedItem::Pages,!generateTreeView,
            QCString() /* altSidebarName */, hierarchyLevel);
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
  ol.disableAllBut(OutputType::Man);
  ol.startTitleHead(manPageName);
  ol.endTitleHead(manPageName, manPageName);
  if (si)
  {
    ol.writeString(" - ");

    if (si->title() != manPageName)
    {
      ol.generateDoc(docFile(),
                     getStartBodyLine(),
                     this,
                     nullptr,
                     si->title(),
                     DocOptions()
                     .setSingleLine(true)
                     .setAutolinkSupport(false));
      ol.endSection(si->label(),si->type());
    }
  }
  ol.popGeneratorState();
  //2.}

  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputType::Man);
  QCString title;
  if (this == Doxygen::mainPage.get() && !hasTitle())
    title = theTranslator->trMainPage();
  else
    title = m_title;

  if (!title.isEmpty() && !name().isEmpty() && si!=nullptr)
  {
    ol.startPageDoc(si->title());
    ol.startHeaderSection();
    ol.startTitleHead(getOutputFileBase());
    ol.generateDoc(docFile(),
                   getStartBodyLine(),
                   this,
                   nullptr,
                   title,
                   DocOptions()
                   .setIndexWords(true)
                   .setSingleLine(true)
                   .setAutolinkSupport(false));
    ol.endTitleHead(getOutputFileBase(),title);
    ol.endHeaderSection();
  }
  else
  {
    ol.startPageDoc("");
  }
  ol.popGeneratorState();
  //2.}

  bool pageWithSections = hasSections();
  ol.startContents();
  if ((m_localToc.isHtmlEnabled() || m_localToc.isLatexEnabled() || m_localToc.isDocbookEnabled())
    && pageWithSections)
  {
    writeToc(ol, m_localToc);
  }

  writePageDocumentation(ol);
  ol.endContents();
  ol.endPageDoc();

  if (generateTreeView && getOuterScope()!=Doxygen::globalScope && !Config_getBool(DISABLE_INDEX))
  {
    endFileWithNavPath(ol,toDefinitionMutable(const_cast<Definition*>(getOuterScope())));
  }
  else
  {
    if (generateTreeView && Config_getBool(PAGE_OUTLINE_PANEL) && pageWithSections)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.writeString("</div><!-- doc-content -->\n");
      writePageNavigation(ol);
      ol.writeString("</div><!-- container -->\n");
      ol.popGeneratorState();
      endFile(ol,true);
    }
    else
    {
      endFile(ol,false,true);
    }
  }

  ol.popGeneratorState();
  //1.}
}

void PageDefImpl::writePageDocumentation(OutputList &ol) const
{
  ol.startTextBlock();
  QCString docStr = (briefDescription().isEmpty()?"":briefDescription()+"\n\n")+documentation()+inbodyDocumentation();
  if (hasBriefDescription() && !SectionManager::instance().find(name()))
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Man);
    ol.writeString(" - ");
    ol.popGeneratorState();
  }
  ol.generateDoc(
      docFile(),           // fileName
      docLine(),           // startLine
      this,                // context
      nullptr,             // memberdef
      docStr,              // docStr
      DocOptions()
      .setIndexWords(true));
  ol.endTextBlock();

  if (hasSubPages())
  {
    // for printed documentation we write subpages as section's of the
    // parent page.
    ol.pushGeneratorState();
    ol.disableAll();
    ol.enable(OutputType::Latex);
    ol.enable(OutputType::Docbook);
    ol.enable(OutputType::RTF);

    for (const auto &subPage : m_subPages)
    {
      ol.writePageLink(subPage->getOutputFileBase(), FALSE);
    }

    ol.popGeneratorState();
  }

  if (hasRequirementRefs()) writeRequirementRefs(ol);
}

void PageDefImpl::writePageNavigation(OutputList &ol) const
{
  ol.writePageOutline();
}

bool PageDefImpl::visibleInIndex() const
{
  bool externalPages = Config_getBool(EXTERNAL_PAGES);
  return // not part of a group
         !getGroupDef() &&
         // not an externally defined page
         (!isReference() || externalPages);
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
  return (d && typeid(*d) == typeid(PageDefImpl)) ? static_cast<PageDef*>(d) : nullptr;
}

const PageDef *toPageDef(const Definition *d)
{
  return (d && typeid(*d) == typeid(PageDefImpl)) ? static_cast<const PageDef*>(d) : nullptr;
}

