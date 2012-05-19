#include "pagedef.h"
#include "groupdef.h"
#include "docparser.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "doxygen.h"
#include "language.h"
#include <qregexp.h>


PageDef::PageDef(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : Definition(f,l,n), m_title(t)
{
  setDocumentation(d,f,l);
  m_subPageDict = new PageSDict(7);
  m_pageScope = 0;
  m_nestingLevel = 0;
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
  LockingPtr<GroupList> groups = partOfGroups();
  return groups!=0 ? groups->getFirst() : 0; 
}

QCString PageDef::getOutputFileBase() const 
{ 
  if (getGroupDef()) 
    return getGroupDef()->getOutputFileBase();
  else 
    return m_fileName; 
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

void PageDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");

  //outputList->disable(OutputGenerator::Man);
  QCString pageName;
  pageName=escapeCharsInString(name(),FALSE,TRUE);

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

  startFile(ol,getOutputFileBase(),pageName,title(),HLI_Pages,!generateTreeView);

  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope && !Config_getBool("DISABLE_INDEX"))
    {
      getOuterScope()->writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }
  SectionInfo *si=Doxygen::sectionDict.find(name());

  // save old generator state and write title only to Man generator
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputGenerator::Man);
  ol.startTitleHead(pageName);
  ol.endTitleHead(pageName, pageName);
  if (si)
  {
    ol.parseDoc(docFile(),docLine(),this,0,si->title,TRUE,FALSE,0,TRUE,FALSE);
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
    ol.parseDoc(docFile(),docLine(),this,0,si->title,TRUE,FALSE,0,TRUE,FALSE);
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

  if (generateTreeView && getOuterScope()!=Doxygen::globalScope && !Config_getBool("DISABLE_INDEX"))
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

  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    bool found=FALSE;
    QDictIterator<RefList> rli(*Doxygen::xrefLists);
    RefList *rl;
    for (rli.toFirst();(rl=rli.current());++rli)
    {
      if (rl->listName()==name())
      {
        found=TRUE;
        break;
      }
    }
    if (!found) // not one of the generated related pages
    {
      Doxygen::tagFile << "  <compound kind=\"page\">" << endl;
      Doxygen::tagFile << "    <name>" << name() << "</name>" << endl;
      Doxygen::tagFile << "    <title>" << convertToXML(title()) << "</title>" << endl;
      Doxygen::tagFile << "    <filename>" << getOutputFileBase() << "</filename>" << endl;
      writeDocAnchorsToTagFile();
      Doxygen::tagFile << "  </compound>" << endl;
    }
  }

  Doxygen::indexList.addIndexItem(this,0,filterTitle(title()));
}

void PageDef::writePageDocumentation(OutputList &ol)
{

  bool markdownEnabled = Doxygen::markdownSupport;
  if (getLanguage()==SrcLangExt_Markdown)
  {
    Doxygen::markdownSupport = TRUE;
  }

  ol.startTextBlock();
  ol.parseDoc(
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
  static bool allExternals = Config_getBool("ALLEXTERNALS");
  return // not part of a group
         !getGroupDef() && 
         // not an externally defined page
         (!isReference() || allExternals) 
         // &&
         // not a subpage
         //(getOuterScope()==0 || 
         // getOuterScope()->definitionType()!=Definition::TypePage
         //)
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

