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
#include "growbuf.h"
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
  if (!name.isEmpty() && !name.endsWith(".bib")) name+=".bib";
  return name;
}

class CiteInfoImpl : public CiteInfo
{
  public:
    CiteInfoImpl(const QCString &label, const QCString &text=QCString())
    : m_label(label), m_text(text) { }

    QCString label() const override { return m_label;    }
    QCString text()  const override { return m_text;     }

    void setText(const QCString &s) { m_text = s; }

  private:
    QCString m_label;
    QCString m_text;
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
  auto it = p->entries.find(label.lower().str());
  if (it!=p->entries.end())
  {
    return it->second.get();
  }
  return nullptr;
}

void CitationManager::clear()
{
  p->entries.clear();
}

bool CitationManager::isEmpty() const
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
  if (bibFile.isEmpty())
  {
    return;
  }
  FileInfo fi(bibFile.str());
  if (!fi.exists())
  {
    err("bib file %s not found!\n",qPrint(bibFile));
    return;
  }
  std::ifstream f = Portable::openInputStream(bibFile);
  if (!f.is_open())
  {
    err("could not open file %s for reading\n",qPrint(bibFile));
    return;
  }

  // search for citation cross references
  QCString citeName;

  std::string lineStr;
  int lineCount = 0;
  while (getline(f,lineStr))
  {
    int i = -1;
    QCString line(lineStr);
    lineCount++;
    if (line.stripWhiteSpace().startsWith("@"))
    {
      // assumption entry like: "@book { name," or "@book { name" (spaces optional)
      int j = line.find('{');
      // when no {, go hunting for it
      while (j==-1 && getline(f,lineStr))
      {
        line = lineStr;
        lineCount++;
        j = line.find('{');
      }
      // search for the name
      citeName = "";
      if (!f.eof() && j!=-1) // to prevent something like "@manual ," and no { found
      {
        int k = line.find(',',j);
        j++;
        // found a line "@....{.....,...." or "@.....{....."
        //                     ^=j  ^=k               ^=j   k=-1
        while (!f.eof() && citeName.isEmpty())
        {
          if (k!=-1)
          {
            citeName = line.mid(static_cast<size_t>(j),static_cast<size_t>(k-j));
          }
          else
          {
            citeName = line.mid(static_cast<size_t>(j));
          }
          citeName = citeName.stripWhiteSpace();
          j = 0;
          if (citeName.isEmpty() && getline(f,lineStr))
          {
            line = lineStr;
            lineCount++;
            k = line.find(',');
          }
        }
      }
      //printf("citeName = #%s#\n",qPrint(citeName));
      if (!citeName.isEmpty())
      {
        std::string lCiteName = citeName.lower().str();
        auto it = p->citePosition.find(lCiteName);
        if (it != p->citePosition.end())
        {
          warn(bibFile,lineCount,"multiple use of citation name '%s', (first occurrence: %s, line %d)",
               qPrint(lCiteName),qPrint(it->second.fileName),it->second.lineNr);
        }
        else
        {
          p->citePosition.emplace(lCiteName,CitePosition(bibFile,lineCount));
        }
      }
    }
    else if ((i=line.find("crossref"))!=-1 && !citeName.isEmpty()) /* assumption cross reference is on one line and the only item */
    {
      int j = line.find('{',i);
      int k = line.find('}',i);
      if (j>i && k>j)
      {
        QCString crossrefName = line.mid(static_cast<size_t>(j+1),static_cast<uint32_t>(k-j-1));
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

const std::string g_formulaMarker = "CITE_FORMULA_";

QCString CitationManager::getFormulas(const QCString &s)
{
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  GrowBuf formulaBuf;
  bool insideFormula = false;
  int citeFormulaCnt = 1;
  const size_t tmpLen = 30;
  char tmp[tmpLen];
  const char *ps=s.data();
  char c = 0;
  while ((c=*ps++))
  {
    if (insideFormula)
    {
      switch (c)
      {
        case '\\':
          formulaBuf.addChar(c);
          c = *ps++;
          formulaBuf.addChar(c);
          break;
        case '\n':
          formulaBuf.addChar(c);
          formulaBuf.addChar(0);
          growBuf.addChar('$');
          growBuf.addStr(formulaBuf.get());
          insideFormula = false;
          formulaBuf.clear();
          break;
        case '$':
          qsnprintf(tmp,tmpLen,"%s%06d",g_formulaMarker.c_str(),citeFormulaCnt);
          formulaBuf.addChar(0);
          p->formulaCite.emplace(citeFormulaCnt,std::string("\\f$") + formulaBuf.get() + "\\f$");
          citeFormulaCnt++;
          // need { and } due to the capitalization rules of bibtex.
          growBuf.addChar('{');
          growBuf.addStr(tmp);
          growBuf.addChar('}');
          insideFormula = false;
          formulaBuf.clear();
          break;
        default:
          formulaBuf.addChar(c);
          break;
      }
    }
    else
    {
      switch (c)
      {
        case '\\':
          growBuf.addChar(c);
          c = *ps++;
          growBuf.addChar(c);
          break;
        case '$':
          insideFormula = true;
          break;
        default:
          growBuf.addChar(c);
          break;
      }
    }
  }
  if (insideFormula)
  {
    formulaBuf.addChar(0);
    growBuf.addStr(formulaBuf.get());
    formulaBuf.clear();
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString CitationManager::replaceFormulas(const QCString &s)
{
  if (s.isEmpty()) return s;
  QCString t;
  int pos=0;
  int i = -1;
  while ((i=s.find(g_formulaMarker.c_str(),pos))!=-1)
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
  if (isEmpty()) return; // nothing to cite

  bool citeDebug = Debug::isFlagSet(Debug::Cite);

  // 0. add cross references from the bib files to the cite dictionary
  const StringVector &citeDataList = Config_getList(CITE_BIB_FILES);
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = getBibFile(QCString(bibdata));
    insertCrossReferencesForBibFile(bibFile);
  }

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QCString outputDir = Config_getString(OUTPUT_DIRECTORY);
  QCString citeListFile = outputDir+"/citelist.doc";
  {
    std::ofstream t = Portable::openOutputStream(citeListFile);
    if (!t.is_open())
    {
      err("could not open file %s for writing\n",qPrint(citeListFile));
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
    err("Failed to create temporary output directory '%s', skipping citations\n",qPrint(bibOutputDir));
    return;
  }
  int i = 0;
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = getBibFile(QCString(bibdata));
    FileInfo fi(bibFile.str());
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        ++i;
        std::ifstream f_org = Portable::openInputStream(bibFile);
        if (!f_org.is_open())
        {
          err("could not open file %s for reading\n",qPrint(bibFile));
        }
        std::ofstream f_out = Portable::openOutputStream(bibOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        if (!f_out.is_open())
        {
          err("could not open file %s for reading\n",qPrint(bibOutputDir + bibTmpFile + QCString().setNum(i) + ".bib"));
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
    err("Problems running bibtex. Verify that the command 'perl --version' works from the command line. Exit code: %d\n",
        exitCode);
  }

  Dir::setCurrent(oldDir);

  // 6. read back the file
  QCString doc;
  {
    std::ifstream f = Portable::openInputStream(citeListFile);
    if (!f.is_open())
    {
      err("could not open file %s for reading\n",qPrint(citeListFile));
    }

    bool insideBib=FALSE;
    //printf("input=[%s]\n",qPrint(input));
    std::string lineStr;
    while (getline(f,lineStr))
    {
      QCString line(lineStr);
      //printf("pos=%d s=%d line=[%s]\n",pos,s,qPrint(line));

      if      (line.find("<!-- BEGIN BIBLIOGRAPHY")!=-1) insideBib=TRUE;
      else if (line.find("<!-- END BIBLIOGRAPH")!=-1)    insideBib=FALSE;
      // determine text to use at the location of the @cite command
      if (insideBib && ((i=line.find("name=\"CITEREF_"))!=-1 || (i=line.find("name=\"#CITEREF_"))!=-1))
      {
        int j=line.find("\">[");
        int k=line.find("]</a>");
        if (j!=-1 && k!=-1)
        {
          size_t ui=static_cast<size_t>(i);
          size_t uj=static_cast<size_t>(j);
          size_t uk=static_cast<size_t>(k);
          QCString label = line.mid(ui+14,uj-ui-14);
          QCString number = line.mid(uj+2,uk-uj-1);
          line = line.left(ui+14) + label + line.right(line.length()-uj);
          auto it = p->entries.find(label.lower().str());
          //printf("label='%s' number='%s' => %p\n",qPrint(label),qPrint(number),it->second.get());
          if (it!=p->entries.end())
          {
            it->second->setText(number);
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
      QCString bibFile = getBibFile(QCString(bibdata));
      FileInfo fi(bibFile.str());
      if (fi.exists())
      {
        if (!bibFile.isEmpty())
        {
          // bug_700510, multiple times the same name were overwriting; creating new names
          // also for names with spaces
          ++i;
          copyFile(bibFile,latexOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        }
      }
      else
      {
        err("bib file %s not found!\n",qPrint(bibFile));
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
    QCString bibFile = getBibFile(QCString(bibdata));
    FileInfo fi(bibFile.str());
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        if (i) result += ",";
        i++;
        result += bibTmpFile;
        result += QCString().setNum(i);
      }
    }
  }
  return result;
}
