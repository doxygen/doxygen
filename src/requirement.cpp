/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
 * The implementation of the requirements feature is loosely  based on
 * https://github.com/doxygen/doxygen/pull/11839 provided by
 * Rob Mellor (https://github.com/dropbearrob)
 */

// own header
#include "requirement.h"

// standard includes
#include <algorithm>

// other includes
#include "config.h"
#include "doxygen.h"
#include "entry.h"
#include "language.h"
#include "linkedmap.h"
#include "message.h"
#include "outputlist.h"
#include "pagedef.h"
#include "section.h"
#include "trace.h"
#include "util.h"

struct Requirement : RequirementIntf
{
  public:
    Requirement(const DString &id_,const DString &file_,int line_,const DString &title_,const DString &doc_,const DString &tagFile_,const DString &extPage_) :
      m_id(id_), m_file(file_), m_line(line_), m_title(title_), m_doc(doc_), m_tagFile(tagFile_), m_extPage(extPage_) {}
    DString id()                const override { return m_id;       }
    DString file()              const override { return m_file;     }
    int     line()              const override { return m_line;     }
    DString title()             const override { return m_title;    }
    DString doc()               const override { return m_doc;      }
    DString getTagFile()        const override { return m_tagFile;  }
    DString getExtPage()        const override { return m_extPage;  }
    DString getOutputFileBase() const override { return ::convertNameToFile("requirements",false,true); }
    const DefinitionVector &satisfiedBy() const override { return m_satisfiedBy; }
    const DefinitionVector &verifiedBy()  const override { return m_verifiedBy;  }

    void sortReferences()
    {
      auto comp = [](const Definition *c1,const Definition *c2)
      {
        return Config_getBool(SORT_BY_SCOPE_NAME)                   ?
          dstricmp_sort(c1->qualifiedName(), c2->qualifiedName())<0 :
          dstricmp_sort(c1->name(), c2->name())<0;
      };
      std::stable_sort(m_satisfiedBy.begin(), m_satisfiedBy.end(), comp);
      std::stable_sort(m_verifiedBy.begin(),  m_verifiedBy.end(),  comp);
    }

    void addSatisfiedBy(const Definition *def)
    {
      if (auto it = std::find(m_satisfiedBy.begin(),m_satisfiedBy.end(),def); it==m_satisfiedBy.end())
      {
        m_satisfiedBy.push_back(def);
      }
    }

    void addVerifiedBy(const Definition *def)
    {
      if (auto it = std::find(m_verifiedBy.begin(),m_verifiedBy.end(),def); it==m_verifiedBy.end())
      {
        m_verifiedBy.push_back(def);
      }
    }

  private:
    DString         m_id;
    DString         m_file;
    int              m_line;
    DString         m_title;
    DString         m_doc;
    DString         m_tagFile;
    DString         m_extPage;
    DefinitionVector m_satisfiedBy;
    DefinitionVector m_verifiedBy;
};

class RequirementManager::Private
{
  public:
    LinkedMap<Requirement> requirements;
    PageDef *reqPageDef = nullptr;
};

RequirementManager &RequirementManager::instance()
{
  static RequirementManager s_instance;
  return s_instance;
}

RequirementManager::RequirementManager() : p(std::make_unique<Private>())
{
}

const PageDef *RequirementManager::requirementsPage() const
{
  return p->reqPageDef;
}

void RequirementManager::addRequirement(Entry *e)
{
  DString tagFile;
  DString extPage;
  if (!Config_getBool(GENERATE_REQUIREMENTS)) // still record the requirement, so we can refer to it.
  {
    if (e->tagInfo())
    {
      //printf("External requirement %s title=%s fileName=%s tagName=%s anchor=%s\n",
      //    qPrint(e->name),qPrint(e->type),qPrint(e->tagInfo()->fileName),qPrint(e->tagInfo()->tagName),qPrint(e->tagInfo()->anchor));
      tagFile = e->tagInfo()->tagName;
      extPage = e->tagInfo()->fileName;
      // register requirement id as anchor; for non-external links this is
      // done in commentscan.l in the comment block containing the @requirement command
      SectionManager::instance().add(e->name,"requirements",1,e->type,SectionType::Requirement,1);
    }
    p->requirements.add(e->name, e->fileName, e->startLine, e->type, e->doc, tagFile, extPage);
  }
  else
  {
    if (p->reqPageDef==nullptr)
    {
      p->reqPageDef = addRelatedPage("requirements",                      // name
          theTranslator->trRequirements(),     // ptitle
          DString(),                          // doc
          "requirements",                      // fileName
          1,                                   // docLine
          1                                    // startLine
          );
    }
    //printf("requirement ID=%s title='%s' file=%s line=%d brief='%s' doc='%s'\n",
    //    qPrint(e->name), qPrint(e->type), qPrint(e->fileName), e->startLine, qPrint(e->brief), qPrint(e->doc));
    DString title = parseCommentAsText(p->reqPageDef,nullptr,e->type,e->fileName,e->startLine);
    DString doc = e->doc;
    if (e->tagInfo())
    {
      //printf("External requirement %s title=%s fileName=%s tagName=%s anchor=%s\n",
      //    qPrint(e->name),qPrint(e->type),qPrint(e->tagInfo()->fileName),qPrint(e->tagInfo()->tagName),qPrint(e->tagInfo()->anchor));
      tagFile = e->tagInfo()->tagName;
      extPage = e->tagInfo()->fileName;
      // register requirement id as anchor; for non-external links this is
      // done in commentscan.l in the comment block containing the @requirement command
      SectionManager::instance().add(e->name,"requirements",1,title,SectionType::Anchor,1);
    }
    if (!e->brief.empty())
    {
      doc.prepend(e->brief+"\n<p>");
    }
    p->requirements.add(e->name, e->fileName, e->startLine, title, doc, tagFile, extPage);

    p->reqPageDef->setRefItems(e->sli);
  }
}

bool RequirementManager::hasRequirements() const
{
  return !p->requirements.empty();
}

RequirementIntfList RequirementManager::requirements() const
{
  RequirementIntfList result;
  for (const auto &req : p->requirements)
  {
    result.push_back(req.get());
  }
  return result;
}

void RequirementManager::generatePage()
{
  AUTO_TRACE("#requirements={}",p->requirements.size());
  if (!Config_getBool(GENERATE_REQUIREMENTS) || p->requirements.empty()) return;
  std::vector<const SectionInfo*> anchors;
  std::stable_sort(p->requirements.begin(),p->requirements.end(),
                   [](const auto &left,const auto &right) { return dstricmp(left->id(),right->id()) < 0; });
  DString doc = "<table class=\"doxtable reqlist\">";
  doc.reserve(10*1024); // prevent too many reallocs
  doc += "<tr><th>";
  doc += theTranslator->trRequirementID();
  doc += "</th><th>";
  doc += "</th>";
  using RequirementPtrVector = std::vector<const Requirement*>;
  RequirementPtrVector missingSatisfiedRef, missingVerifiedRef;
  for (const auto &req : p->requirements) // TODO: filter out external references?
  {
    if (const SectionInfo *si = SectionManager::instance().find(req->id()); si!=nullptr)
    {
      anchors.push_back(si);
    }
    doc += "<tr><td valign=\"top\">";
    doc += " \\ifile \""+req->file()+"\" \\iline "+DString().setNum(req->line())+" ";
    doc += "\\anchor ";
    doc += req->id();
    doc += " ";
    doc += "<span class=\"req_id\">";
    if (DString tagFile = req->getTagFile(); !tagFile.empty())
    {
      //printf("tagFile=%s extPage=%s\n",qPrint(tagFile),qPrint(req->getExtPage()));
      doc += "<a href=\"";
      doc += createHtmlUrl(DString(),tagFile,false,req->getExtPage(),req->id());
      doc +="\">"+req->id()+"</a>";
    }
    else
    {
      doc += req->id();
    }
    doc += "</span> ";
    doc += "</td><td>";
    doc += "<div class=\"req_title\">"+req->title()+"</div>";
    doc += "<p/><div class=\"req_docs\">";
    doc += req->doc();
    req->sortReferences();
    auto symToString = [](const Definition *sym)
    {
      DString symName = sym->qualifiedName();
      if (sym->definitionType()==Definition::TypeMember)
      {
        const MemberDef *md = toMemberDef(sym);
        if (!md->isObjCMethod() && md->isFunctionOrSignalSlot()) symName += "()";
      }
      return symName;
    };
    int numSatisfiedBy = static_cast<int>(req->satisfiedBy().size());
    if (numSatisfiedBy>0)
    {
      doc += "<p><div class=\"satisfiedby\">";
      doc += theTranslator->trSatisfiedBy(
                 writeMarkerList(theTranslator->trWriteList(numSatisfiedBy).str(),
                     numSatisfiedBy,
                     [&symToString, &refs = req->satisfiedBy()](size_t entryIndex) {
                       return symToString(refs[entryIndex]);
                     }));
      doc += "</div></p>";
    }
    else
    {
      missingSatisfiedRef.push_back(req.get());
    }
    int numVerifiedBy = static_cast<int>(req->verifiedBy().size());
    if (numVerifiedBy>0)
    {
      doc += "<p><div class=\"verifiedby\">";
      doc += theTranslator->trVerifiedBy(
                 writeMarkerList(theTranslator->trWriteList(numVerifiedBy).str(),
                     numVerifiedBy,
                     [&symToString, &refs = req->verifiedBy()](size_t entryIndex) {
                       return symToString(refs[entryIndex]);
                     }));
      doc += "</div></p>";
    }
    else
    {
      missingVerifiedRef.push_back(req.get());
    }
    doc += "\n</div></td></tr>\n";
  }
  doc += "</table>\n";

  //------------
  doc += " \\ifile \"requirements\" \\iline 1\n";

  auto writeMissingRef = [&doc,&anchors](const RequirementPtrVector &reqs,
                                         const char *label,const DString &section,const DString &text)
  {
    if (!reqs.empty())
    {
      SectionManager &sm = SectionManager::instance();
      const SectionInfo *si = sm.add(DString("missing_")+label,"requirements",1,section,SectionType::Section,1);
      anchors.push_back(si);
      doc += "\\htmlonly <div class=\"missing_";
      doc += label;
      doc += "\">\\endhtmlonly\n";
      doc += DString("@section missing_")+label+" "+section+"\n"+text+"\n";
      doc += "\\htmlonly </div>\\endhtmlonly\n";
    }
  };

  // write list of requirements that do not have a satisfies relation
  auto traceInfo = Config_getEnum(REQ_TRACEABILITY_INFO);
  int numMissingSatisfied = static_cast<int>(missingSatisfiedRef.size());
  if ((traceInfo==REQ_TRACEABILITY_INFO_t::YES || traceInfo==REQ_TRACEABILITY_INFO_t::UNSATISFIED_ONLY) && numMissingSatisfied>0)
  {
    writeMissingRef(missingSatisfiedRef,
                    "satisfies",
                    theTranslator->trUnsatisfiedRequirements(),
                    theTranslator->trUnsatisfiedRequirementsText(numMissingSatisfied==1,
                     writeMarkerList(theTranslator->trWriteList(numMissingSatisfied).str(),
                       numMissingSatisfied,
                       [&missingSatisfiedRef](size_t entryIndex) {
                         DString id = missingSatisfiedRef[entryIndex]->id();
                         return "@ref "+id + " \""+id+"\"";
                       })));
  }

  // write list of requirements that do not have a verifies relation
  int numMissingVerified = static_cast<int>(missingVerifiedRef.size());
  if ((traceInfo==REQ_TRACEABILITY_INFO_t::YES || traceInfo==REQ_TRACEABILITY_INFO_t::UNVERIFIED_ONLY) && numMissingVerified>0)
  {
    writeMissingRef(missingVerifiedRef,
                    "verifies",
                    theTranslator->trUnverifiedRequirements(),
                    theTranslator->trUnverifiedRequirementsText(numMissingVerified==1,
                     writeMarkerList(theTranslator->trWriteList(numMissingVerified).str(),
                       numMissingVerified,
                       [&missingVerifiedRef](size_t entryIndex) {
                         DString id = missingVerifiedRef[entryIndex]->id();
                         return "@ref "+id + " \""+id+"\"";
                       })));
  }

  AUTO_TRACE_ADD("doc=[[\n{}\n]]\n",doc);
  p->reqPageDef->setDocumentation(doc,"requirements",1,false);
  p->reqPageDef->addSectionsToDefinition(anchors);
}

const RequirementIntf *RequirementManager::find(const DString &reqId) const
{
  return p->requirements.find(reqId);
}

void RequirementManager::addRequirementRefsForSymbol(const Definition *symbol)
{
  for (const auto &ref : symbol->requirementReferences())
  {
    Requirement *req = p->requirements.find(ref.reqId());
    if (req)
    {
      //printf("adding reference from %s to requirement %s\n",qPrint(symbol->name()),qPrint(ref.reqId()));
      switch (ref.type())
      {
        case RequirementRefType::Satisfies: req->addSatisfiedBy(symbol); break;
        case RequirementRefType::Verifies:  req->addVerifiedBy(symbol);   break;
      }
    }
    else
    {
      warn(ref.file(),ref.line(),"Reference to unknown requirement '{}' found",ref.reqId());
      // invalid reference (file, and line needed)
    }
  }
}


void RequirementManager::writeRef(OutputList &ol,const RequirementRef &ref)
{
  if (!Config_getBool(GENERATE_REQUIREMENTS)) return;
  if (const RequirementIntf *req = RequirementManager::instance().find(ref.reqId()); req!=nullptr)
  {
    DString title = ref.reqId();
    if (!ref.title().empty())
    {
      title +=" (";
      title += parseCommentAsText(p->reqPageDef,nullptr,ref.title(),ref.file(),ref.line());
      title +=")";
    }
    else if (!req->title().empty())
    {
      title += " (";
      title += req->title();
      title += ")";
    }
    else
    {
      title = ref.reqId();
    }
    ol.writeObjectLink(DString(),req->getOutputFileBase(),ref.reqId(),title);
  }
  else
  {
    ol.docify(ref.reqId());
  }
}

void RequirementManager::writeTagFile(TextStream &tagFile)
{
  if (!Config_getBool(GENERATE_REQUIREMENTS)) return;
  if (!p->requirements.empty())
  {
    for (const auto &req : p->requirements)
    {
      tagFile << "  <compound kind=\"requirement\">\n";
      tagFile << "    <id>" << req->id() << "</id>\n";
      tagFile << "    <title>" << convertToXML(req->title()) << "</title>\n";
      DString fn = req->getOutputFileBase();
      addHtmlExtensionIfMissing(fn);
      tagFile << "    <filename>" << fn << "</filename>\n";
      tagFile << "  </compound>\n";
    }
  }
}

void splitRequirementRefs(const RequirementRefs &inputReqRefs,RequirementRefs &satisfiesRefs,RequirementRefs &verifiesRefs)
{
  auto makeUnique = [](RequirementRefs &uniqueRefs)
  {
    // sort results on itemId
    std::stable_sort(uniqueRefs.begin(),uniqueRefs.end(),
        [](const auto &left,const auto &right)
        { return  left.reqId()< right.reqId() ||
        (left.reqId()==right.reqId() &&
         dstricmp(left.title(),right.title())<0);
        });

    // filter out duplicates
    auto last = std::unique(uniqueRefs.begin(),uniqueRefs.end(),
        [](const auto &left,const auto &right)
        { return left.reqId()==right.reqId() &&
        dstricmp(left.title(),right.title())==0;
        });

    // remove unused part
    uniqueRefs.erase(last, uniqueRefs.end());
  };

  // split into satisfied and verifies references
  std::partition_copy(
      inputReqRefs.begin(),
      inputReqRefs.end(),
      std::back_inserter(satisfiesRefs),
      std::back_inserter(verifiesRefs),
      [](const auto &ref) { return ref.type()==RequirementRefType::Satisfies; }
     );

  // remove duplicates
  makeUnique(satisfiesRefs);
  makeUnique(verifiesRefs);
}

