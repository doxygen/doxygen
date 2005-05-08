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
  subPageDict = new PageSDict(7);
}

PageDef::~PageDef()
{
  delete subPageDict;
}

void PageDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
}

GroupDef *PageDef::getGroupDef() const 
{ 
  return partOfGroups() ? partOfGroups()->getFirst() : 0; 
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
    subPageDict->append(pd->name(),pd);
    def->setOuterScope(this);
  }
}

void PageDef::writeDocumentation(OutputList &ol)
{
  //outputList->disable(OutputGenerator::Man);
  QCString pageName;
  if (Config_getBool("CASE_SENSE_NAMES"))
    pageName=name();
  else
    pageName=name().lower();

  startFile(ol,pageName,pageName,title());

  if (getOuterScope()!=Doxygen::globalScope && !Config_getBool("DISABLE_INDEX"))
  {
    getOuterScope()->writeNavigationPath(ol);
  }

  // save old generator state and write title only to Man generator
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startTitleHead(pageName);
  ol.endTitleHead(pageName, pageName);
  ol.popGeneratorState();

  SectionInfo *si=0;
  if (!title().isEmpty() && !name().isEmpty() &&
      (si=Doxygen::sectionDict.find(pageName))!=0)
  {
    ol.startSection(si->label,si->title,si->type);
    ol.docify(si->title);
    stringToSearchIndex(getOutputFileBase(),
                        theTranslator->trPage(TRUE,TRUE)+" "+si->title,
                        si->title);
    ol.endSection(si->label,si->type);
  }
  ol.startTextBlock();
  ol.parseDoc(docFile(),       // fileName
      docLine(),           // startLine
      this,                // context
      0,                   // memberdef
      documentation(),     // docStr
      TRUE,                // index words
      FALSE                // not an example
      );
  ol.endTextBlock();
  endFile(ol);
  //outputList->enable(OutputGenerator::Man);

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
  return subPageDict->count()>0;
}


