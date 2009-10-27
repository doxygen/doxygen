#include "pagedef.h"
#include "groupdef.h"
#include "docparser.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "doxygen.h"
#include "language.h"


PageDef::PageDef(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : Definition(f,l,n), m_title(t)
{
  setDocumentation(d,f,l);
  m_subPageDict = new PageSDict(7);
  m_pageScope = 0;
  m_nestingLevel = 0;
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
  //outputList->disable(OutputGenerator::Man);
  QCString pageName;
  pageName=escapeCharsInString(name(),FALSE);

  startFile(ol,pageName,pageName,title(),HLI_Pages,TRUE);

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

  if (getOuterScope()!=Doxygen::globalScope && !Config_getBool("DISABLE_INDEX"))
  {
    getOuterScope()->writeNavigationPath(ol);
  }

  ol.endQuickIndices();
  ol.startContents();

  // save old generator state and write title only to Man generator
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputGenerator::Man);
  ol.startTitleHead(pageName);
  ol.endTitleHead(pageName, pageName);
  ol.popGeneratorState();
  //2.}

  // for Latex the section is already generated as a chapter in the index!
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  SectionInfo *si=0;
  if (!title().isEmpty() && !name().isEmpty() &&
      (si=Doxygen::sectionDict.find(pageName))!=0)
  {
    ol.startSection(si->label,si->title,si->type);
    ol.parseDoc(docFile(),docLine(),this,0,si->title,TRUE,FALSE,0,TRUE,FALSE);
    //stringToSearchIndex(getOutputFileBase(),
    //                    theTranslator->trPage(TRUE,TRUE)+" "+si->title,
    //                    si->title);
    ol.endSection(si->label,si->type);
  }
  ol.popGeneratorState();
  //2.}

  writePageDocumentation(ol);

  ol.popGeneratorState();
  //1.}

  endFile(ol);

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
}

void PageDef::writePageDocumentation(OutputList &ol)
{
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
   return // not part of a group
          !getGroupDef() && 
          // not an externally defined page
          (!isReference() || Config_getBool("ALLEXTERNALS")) &&
          // not a subpage
          (getOuterScope()==0 || 
           getOuterScope()->definitionType()!=Definition::TypePage
          );
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

