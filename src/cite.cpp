/******************************************************************************
 *
 * Copyright (C) 2020 by Dimitri van Heesch
 * Based on a patch by David Munger
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

#include "cite.h"
#include "config.h"
#include "language.h"
#include "message.h"
#include "portable.h"
#include "resourcemgr.h"
#include "util.h"
#include "debug.h"
#include "fileinfo.h"
#include "dir.h"
#include "entry.h"
#include "commentscan.h"
#include "linkedmap.h"

#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

const char *bibTmpFile = "bibTmpFile_";
const char *bibTmpDir  = "bibTmpDir/";

//! class that provide information about the p[osition of a citation name
class CitePosition
{
  public:
    CitePosition(const QCString &fn, int l) : fileName(fn), lineNr(l) {}

    QCString    fileName;
    int         lineNr;
};

static QCString getBibFile(const QCString &inFile)
{
  QCString name = inFile;
  if (!name.empty() && !name.endsWith(".bib")) name+=".bib";
  return name;
}

class CiteInfoImpl : public CiteInfo
{
  public:
    CiteInfoImpl(const QCString &label, const QCString &text=QCString())
    : m_label(label), m_text(text), m_shortAuthor(QCString()), m_year(QCString()) { }

    QCString label() const override { return m_label;    }
    QCString text()  const override { return m_text;     }
    QCString shortAuthor()  const override { return m_shortAuthor;     }
    QCString year()  const override { return m_year;     }

    void setText(const QCString &s) { m_text = s; }
    void setShortAuthor(const QCString &s) { m_shortAuthor = s; }
    void setYear(const QCString &s) { m_year = s; }

  private:
    QCString m_label;
    QCString m_text;
    QCString m_shortAuthor;
    QCString m_year;
};

struct CitationManager::Private
{
  std::map< std::string,std::unique_ptr<CiteInfoImpl> > entries;
  std::unordered_map< int,std::string > formulaCite;
  std::unordered_map< std::string, CitePosition > citePosition;
};

CitationManager &CitationManager::instance()
{
  static CitationManager ct;
  return ct;
}

CitationManager::CitationManager() : p(new Private)
{
}

void CitationManager::insert(const QCString &label)
{
  QCString lowerCaseLabel = label.lower();
  p->entries.emplace(lowerCaseLabel.str(),std::make_unique<CiteInfoImpl>(lowerCaseLabel));
}

const CiteInfo *CitationManager::find(const QCString &label) const
{
  if (auto it = p->entries.find(label.lower().str()); it != p->entries.end())
  {
    return it->second.get();
  }
  return nullptr;
}

void CitationManager::clear()
{
  p->entries.clear();
}

bool CitationManager::empty() const
{
  size_t numFiles = Config_getList(CITE_BIB_FILES).size();
  return (numFiles==0 || p->entries.empty());
}

QCString CitationManager::fileName() const
{
  return "citelist";
}

QCString CitationManager::anchorPrefix() const
{
  return "CITEREF_";
}

void CitationManager::insertCrossReferencesForBibFile(const QCString &bibFile)
{
  // sanity checks
  if (bibFile.empty())
  {
    return;
  }
  FileInfo fi(bibFile.str());
  if (!fi.exists())
  {
    err("bib file {} not found!\n",bibFile);
    return;
  }
  std::ifstream f = Portable::openInputStream(bibFile);
  if (!f.is_open())
  {
    err("could not open file {} for reading\n",bibFile);
    return;
  }

  // search for citation cross references
  QCString citeName;

  std::string lineStr;
  int lineCount = 0;
  while (getline(f,lineStr))
  {
    size_t i = QCString::npos;
    QCString line(lineStr);
    lineCount++;
    if (line.stripWhiteSpace().startsWith("@"))
    {
      // assumption entry like: "@book { name," or "@book { name" (spaces optional)
      size_t j = line.find('{');
      // when no {, go hunting for it
      while (j==QCString::npos && getline(f,lineStr))
      {
        line = lineStr;
        lineCount++;
        j = line.find('{');
      }
      // search for the name
      citeName = "";
      if (!f.eof() && j!=QCString::npos) // to prevent something like "@manual ," and no { found
      {
        size_t k = line.find(',',j);
        j++;
        // found a line "@....{.....,...." or "@.....{....."
        //                     ^=j  ^=k               ^=j   k=-1
        while (!f.eof() && citeName.empty())
        {
          if (k!=QCString::npos)
          {
            citeName = line.mid(j,k-j);
          }
          else
          {
            citeName = line.mid(j);
          }
          citeName = citeName.stripWhiteSpace();
          j = 0;
          if (citeName.empty() && getline(f,lineStr))
          {
            line = lineStr;
            lineCount++;
            k = line.find(',');
          }
        }
      }
      //printf("citeName = #%s#\n",qPrint(citeName));
      if (!citeName.empty())
      {
        std::string lCiteName = citeName.lower().str();
        auto it = p->citePosition.find(lCiteName);
        if (it != p->citePosition.end())
        {
          warn(bibFile,lineCount,"multiple use of citation name '{}', (first occurrence: {}, line {})",
               lCiteName,it->second.fileName,it->second.lineNr);
        }
        else
        {
          p->citePosition.emplace(lCiteName,CitePosition(bibFile,lineCount));
        }
      }
    }
    else if ((i=line.find("crossref"))!=QCString::npos && !citeName.empty()) /* assumption cross reference is on one line and the only item */
    {
      size_t j = line.find('{',i);
      size_t k = line.find('}',i);
      if (j!=QCString::npos && k!=QCString::npos && j>i && k>j)
      {
        QCString crossrefName = line.mid(j+1,k-j-1);
        // check if the reference with the cross reference is used
        // insert cross reference when cross reference has not yet been added.
        if (find(citeName) && !find(crossrefName)) // not found yet
        {
          insert(crossrefName);
        }
      }
    }
  }
}

static const std::string g_formulaMarker = "CITE_FORMULA_";

QCString CitationManager::getFormulas(const QCString &s)
{
  if (s.empty()) return s;
  QCString result;
  result.reserve(s.length()+32);
  QCString formula;
  formula.reserve(256);
  bool insideFormula = false;
  int citeFormulaCnt = 1;
  const char *ps=s.data();
  char c = 0;
  while ((c=*ps++))
  {
    if (insideFormula)
    {
      switch (c)
      {
        case '\\':
          formula+=c;
          c = *ps++;
          formula+=c;
          break;
        case '\n':
          formula+=c;
          result+='$';
          result+=formula;
          insideFormula = false;
          formula.clear();
          break;
        case '$':
          {
            const size_t idLen = 30;
            char id[idLen];
            qsnprintf(id,idLen,"%s%06d",g_formulaMarker.c_str(),citeFormulaCnt);
            p->formulaCite.emplace(citeFormulaCnt,std::string("\\f$") + formula.str() + "\\f$");
            citeFormulaCnt++;
            // need { and } due to the capitalization rules of bibtex.
            result+='{';
            result+=id;
            result+='}';
            insideFormula = false;
            formula.clear();
          }
          break;
        default:
          formula+=c;
          break;
      }
    }
    else
    {
      switch (c)
      {
        case '\\':
          result+=c;
          c = *ps++;
          result+=c;
          break;
        case '$':
          insideFormula = true;
          break;
        default:
          result+=c;
          break;
      }
    }
  }
  if (insideFormula)
  {
    result+=formula;
    formula.clear();
  }
  return result;
}

QCString CitationManager::replaceFormulas(const QCString &s)
{
  if (s.empty()) return s;
  QCString t;
  size_t pos=0;
  size_t i = QCString::npos;
  while ((i=s.find(g_formulaMarker,pos))!=QCString::npos)
  {
    t += s.mid(pos,i-pos);
    int markerSize = static_cast<int>( g_formulaMarker.length());
    int markerId = atoi(s.mid(i+markerSize,6).data());
    auto it = p->formulaCite.find(markerId);
    if (it != p->formulaCite.end()) t += it->second;
    pos = i + markerSize+6;
  }
  t += s.mid(pos);
  //printf("replaceFormulas(%s)=%s\n",qPrint(s),qPrint(t));
  return t;
}

void CitationManager::generatePage()
{
  //printf("** CitationManager::generatePage() count=%d\n",m_ordering.count());

  // do not generate an empty citations page
  if (empty()) return; // nothing to cite

  bool citeDebug = Debug::isFlagSet(Debug::Cite);

  // 0. add cross references from the bib files to the cite dictionary
  const StringVector &citeDataList = Config_getList(CITE_BIB_FILES);
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = getBibFile(bibdata);
    insertCrossReferencesForBibFile(bibFile);
  }

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QCString outputDir = Config_getString(OUTPUT_DIRECTORY);
  QCString citeListFile = outputDir+"/citelist.doc";
  {
    std::ofstream t = Portable::openOutputStream(citeListFile);
    if (!t.is_open())
    {
      err("could not open file {} for writing\n",citeListFile);
    }
    t << "<!-- BEGIN CITATIONS -->\n";
    t << "<!--\n";
    for (const auto &it : p->entries)
    {
      t << "\\citation{" << it.second->label() << "}\n";
    }
    t << "-->\n";
    t << "<!-- END CITATIONS -->\n";
    t << "<!-- BEGIN BIBLIOGRAPHY -->\n";
    t << "<!-- END BIBLIOGRAPHY -->\n";
    t.close();
  }

  // 2. generate bib2xhtml
  QCString bib2xhtmlFile  = outputDir+"/bib2xhtml.pl";
  ResourceMgr::instance().copyResource("bib2xhtml.pl",outputDir);

  // 3. generate doxygen.bst
  QCString doxygenBstFile = outputDir+"/doxygen.bst";
  ResourceMgr::instance().copyResource("doxygen.bst",outputDir);

  // 4. for all formats we just copy the bib files to as special output directory
  //    so bibtex can find them without path (bibtex doesn't support paths or
  //    filenames with spaces!)
  //    Strictly not required when only latex is generated
  QCString bibOutputDir = outputDir+"/"+bibTmpDir;
  QCString bibOutputFiles = "";
  Dir thisDir;
  if (!thisDir.exists(bibOutputDir.str()) && !thisDir.mkdir(bibOutputDir.str()))
  {
    err("Failed to create temporary output directory '{}', skipping citations\n",bibOutputDir);
    return;
  }
  size_t i = 0;
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = getBibFile(bibdata);
    FileInfo fi(bibFile.str());
    if (fi.exists())
    {
      if (!bibFile.empty())
      {
        ++i;
        std::ifstream f_org = Portable::openInputStream(bibFile);
        if (!f_org.is_open())
        {
          err("could not open file {} for reading\n",bibFile);
        }
        std::ofstream f_out = Portable::openOutputStream(bibOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        if (!f_out.is_open())
        {
          err("could not open file {}{}{:d}{} for reading\n",bibOutputDir,bibTmpFile,i,".bib");
        }
        QCString docs;
        std::string lineStr;
        while (getline(f_org,lineStr))
        {
          docs += lineStr + "\n";
        }
        docs = getFormulas(docs);
        f_out << docs;
        if (f_org.is_open()) f_org.close();
        if (f_out.is_open()) f_out.close();
        bibOutputFiles = bibOutputFiles + " " + bibTmpDir + bibTmpFile + QCString().setNum(i) + ".bib";
      }
    }
  }

  std::string oldDir = Dir::currentDirPath();
  Dir::setCurrent(outputDir.str());

  // 5. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  QCString perlArgs = "\""+bib2xhtmlFile+"\" "+bibOutputFiles+" \""+ citeListFile+"\"";
  if (citeDebug) perlArgs+=" -d";
  int exitCode = Portable::system("perl",perlArgs);
  if (exitCode!=0)
  {
    err("Problems running bibtex. Verify that the command 'perl --version' works from the command line. Exit code: {}\n",
        exitCode);
  }

  Dir::setCurrent(oldDir);

  // 6. read back the file
  QCString doc;
  {
    std::ifstream f = Portable::openInputStream(citeListFile);
    if (!f.is_open())
    {
      err("could not open file {} for reading\n",citeListFile);
    }

    bool insideBib=false;
    //printf("input=[%s]\n",qPrint(input));
    std::string lineStr;
    while (getline(f,lineStr))
    {
      QCString line(lineStr);
      //printf("pos=%d s=%d line=[%s]\n",pos,s,qPrint(line));

      if      (line.find("<!-- BEGIN BIBLIOGRAPHY")!=QCString::npos) insideBib=true;
      else if (line.find("<!-- END BIBLIOGRAPH")!=QCString::npos)    insideBib=false;
      // determine text to use at the location of the @cite command
      if (insideBib && ((i=line.find("name=\"CITEREF_"))!=QCString::npos || (i=line.find("name=\"#CITEREF_"))!=QCString::npos))
      {
        size_t j=line.find("\">[");
        size_t j1=line.find("<!--[");
        size_t k=line.find("]<!--");
        size_t k1=line.find("]-->");
        if (j!=QCString::npos && k!=QCString::npos)
        {
          QCString label       = line.mid(i+14,j-i-14);
          StringVector optList = split(line.mid(j1+5,k1-j1-5).str(),",");
          QCString number      = optList[0];
          QCString shortAuthor = optList[1];
          QCString year;
          if (optList.size() == 3)
          {
            year = optList[2];
          }
          line = line.left(i+14) + label + line.mid(j);
          auto it = p->entries.find(label.lower().str());
          //printf("label='%s' number='%s' => %p\n",qPrint(label),qPrint(number),it->second.get());
          if (it!=p->entries.end())
          {
            it->second->setText(number);
            it->second->setShortAuthor(shortAuthor);
            it->second->setYear(year.stripWhiteSpace());
          }
        }
      }
      if (insideBib) doc+=line+"\n";
    }
    //printf("doc=[%s]\n",qPrint(doc));
  }

  // 7. place formulas back and run the conversion of \f$ ... \f$ to the internal required format
  {
    doc = replaceFormulas(doc);
    Entry            current;
    bool             needsEntry = false;
    CommentScanner   commentScanner;
    int              lineNr = 0;
    int              pos = 0;
    GuardedSectionStack guards;
    Protection       prot = Protection::Public;
    commentScanner.parseCommentBlock(
        nullptr,
        &current,
        doc,          // text
        fileName(),   // file
        lineNr,       // line of block start
        false,        // isBrief
        false,        // isJavaDocStyle
        false,        // isInBody
        prot,         // protection
        pos,          // position,
        needsEntry,
        false,
        &guards
        );
    doc = current.doc;
  }

  // 8. add it as a page
  addRelatedPage(fileName(),theTranslator->trCiteReferences(),doc,fileName(),1,1);

  // 9. for latex we just copy the bib files to the output and let
  //    latex do this work.
  if (Config_getBool(GENERATE_LATEX))
  {
    // copy bib files to the latex output dir
    QCString latexOutputDir = Config_getString(LATEX_OUTPUT)+"/";
    i = 0;
    for (const auto &bibdata : citeDataList)
    {
      QCString bibFile = getBibFile(bibdata);
      FileInfo fi(bibFile.str());
      if (fi.exists())
      {
        if (!bibFile.empty())
        {
          // bug_700510, multiple times the same name were overwriting; creating new names
          // also for names with spaces
          ++i;
          copyFile(bibFile,latexOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        }
      }
      else
      {
        err("bib file {} not found!\n",bibFile);
      }
    }
  }

  // 10. Remove temporary files
  if (!citeDebug)
  {
    thisDir.remove(citeListFile.str());
    thisDir.remove(doxygenBstFile.str());
    thisDir.remove(bib2xhtmlFile.str());
    // we might try to remove too many files as empty files didn't get a corresponding new file
    // but the remove function does not emit an error for it and we don't catch the error return
    // so no problem.
    for (size_t j = 1; j <= citeDataList.size(); j++)
    {
      QCString bibFile = bibOutputDir + bibTmpFile + QCString().setNum(static_cast<int>(j)) + ".bib";
      thisDir.remove(bibFile.str());
    }
    thisDir.rmdir(bibOutputDir.str());
  }
}

QCString CitationManager::latexBibFiles()
{
  QCString result;
  const StringVector &citeDataList = Config_getList(CITE_BIB_FILES);
  int i = 0;
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = getBibFile(bibdata);
    FileInfo fi(bibFile.str());
    if (fi.exists() && !bibFile.empty())
    {
      if (i) result += ",";
      i++;
      result += bibTmpFile;
      result += QCString().setNum(i);
    }
  }
  return result;
}
