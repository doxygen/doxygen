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

#include <map>
#include <string>
#include <fstream>

const char *bibTmpFile = "bibTmpFile_";
const char *bibTmpDir  = "bibTmpDir/";

class CiteInfoImpl : public CiteInfo
{
  public:
    CiteInfoImpl(const QCString &label, const QCString &text=QCString())
    : m_label(label), m_text(text) { }

    virtual QCString label()    const { return m_label;    }
    virtual QCString text()     const { return m_text;     }

    void setText(const QCString &s) { m_text = s; }

  private:
    QCString m_label;
    QCString m_text;
};

struct CitationManager::Private
{
  std::map< std::string,std::unique_ptr<CiteInfoImpl> > entries;
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
  p->entries.insert(
      std::make_pair(
        label.str(),
        std::make_unique<CiteInfoImpl>(label)
      ));
}

const CiteInfo *CitationManager::find(const QCString &label) const
{
  auto it = p->entries.find(label.str());
  if (it!=p->entries.end())
  {
    return it->second.get();
  }
  return 0;
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
  std::ifstream f(bibFile.str(), std::ifstream::in);
  if (!f.is_open())
  {
    err("could not open file %s for reading\n",qPrint(bibFile));
    return;
  }

  // search for citation cross references
  QCString citeName;

  std::string lineStr;
  while (getline(f,lineStr))
  {
    int i;
    QCString line(lineStr);
    if (line.stripWhiteSpace().startsWith("@"))
    {
      // assumption entry like: "@book { name," or "@book { name" (spaces optional)
      int j = line.find('{');
      // when no {, go hunting for it
      while (j==-1 && getline(f,lineStr))
      {
        line = lineStr;
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
            citeName = line.mid((uint)(j),(uint)(k-j));
          }
          else
          {
            citeName = line.mid((uint)(j));
          }
          citeName = citeName.stripWhiteSpace();
          j = 0;
          if (citeName.isEmpty() && getline(f,lineStr))
          {
            line = lineStr;
            k = line.find(',');
          }
        }
      }
      //printf("citeName = #%s#\n",qPrint(citeName));
    }
    else if ((i=line.find("crossref"))!=-1 && !citeName.isEmpty()) /* assumption cross reference is on one line and the only item */
    {
      int j = line.find('{',i);
      int k = line.find('}',i);
      if (j>i && k>j)
      {
        QCString crossrefName = line.mid((uint)(j+1),(uint)(k-j-1));
        // check if the reference with the cross reference is used
        // insert cross reference when cross reference has not yet been added.
        if ((p->entries.find(citeName.str())!=p->entries.end()) &&
            (p->entries.find(crossrefName.str())==p->entries.end())) // not found yet
        {
          insert(crossrefName);
        }
      }
    }
  }
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
    QCString bibFile = bibdata.c_str();
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    insertCrossReferencesForBibFile(bibFile);
  }

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QCString outputDir = Config_getString(OUTPUT_DIRECTORY);
  QCString citeListFile = outputDir+"/citelist.doc";
  {
    std::ofstream t(citeListFile.str(),std::ofstream::out | std::ofstream::binary);
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
    QCString bibFile = bibdata.c_str();
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    FileInfo fi(bibFile.str());
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        ++i;
        copyFile(bibFile,bibOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        bibOutputFiles = bibOutputFiles + " " + bibTmpDir + bibTmpFile + QCString().setNum(i) + ".bib";
      }
    }
  }

  std::string oldDir = Dir::currentDirPath();
  Dir::setCurrent(outputDir.str());

  // 5. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  int exitCode;
  Portable::sysTimerStop();
  QCString perlArgs = "\""+bib2xhtmlFile+"\" "+bibOutputFiles+" \""+ citeListFile+"\"";
  if (citeDebug) perlArgs+=" -d";
  if ((exitCode=Portable::system("perl",perlArgs)) != 0)
  {
    err("Problems running bibtex. Verify that the command 'perl --version' works from the command line. Exit code: %d\n",
        exitCode);
  }
  Portable::sysTimerStop();

  Dir::setCurrent(oldDir);

  // 6. read back the file
  QCString doc;
  {
    std::ifstream f(citeListFile.str(),std::ifstream::in);
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
          uint ui=(uint)i;
          uint uj=(uint)j;
          uint uk=(uint)k;
          QCString label = line.mid(ui+14,uj-ui-14);
          QCString number = line.mid(uj+2,uk-uj-1);
          line = line.left(ui+14) + label + line.right(line.length()-uj);
          auto it = p->entries.find(label.str());
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

  // 7. add it as a page
  addRelatedPage(fileName(),theTranslator->trCiteReferences(),doc,fileName(),1,1);

  // 8. for latex we just copy the bib files to the output and let
  //    latex do this work.
  if (Config_getBool(GENERATE_LATEX))
  {
    // copy bib files to the latex output dir
    QCString latexOutputDir = Config_getString(LATEX_OUTPUT)+"/";
    i = 0;
    for (const auto &bibdata : citeDataList)
    {
      QCString bibFile = bibdata.c_str();
      // Note: file can now have multiple dots
      if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
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

  // 9. Remove temporary files
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
      QCString bibFile = bibOutputDir + bibTmpFile + QCString().setNum(static_cast<ulong>(j)) + ".bib";
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
    QCString bibFile = bibdata.c_str();
    // Note: file can now have multiple dots
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
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
