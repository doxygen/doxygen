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
#include "ftextstream.h"
#include "language.h"
#include "message.h"
#include "portable.h"
#include "resourcemgr.h"
#include "util.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <map>
#include <string>

// Remove the temporary files
#define RM_TMP_FILES (true)
//#define RM_TMP_FILES (false)

const char *bibTmpFile = "bibTmpFile_";
const char *bibTmpDir  = "bibTmpDir/";

class CiteInfoImpl : public CiteInfo
{
  public:
    CiteInfoImpl(const char *label, const char *text=0)
    : m_label(label), m_text(text) { }

    virtual QCString label()    const { return m_label;    }
    virtual QCString text()     const { return m_text;     }

    void setText(const char *s) { m_text = s; }

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

void CitationManager::insert(const char *label)
{
  p->entries.insert(
      std::make_pair(
        std::string(label),
        std::make_unique<CiteInfoImpl>(label)
      ));
}

const CiteInfo *CitationManager::find(const char *label) const
{
  auto it = p->entries.find(label);
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

const char *CitationManager::fileName() const
{
  return "citelist";
}

const char *CitationManager::anchorPrefix() const
{
  return "CITEREF_";
}

void CitationManager::generatePage()
{
  //printf("** CitationManager::generatePage() count=%d\n",m_ordering.count());

  // do not generate an empty citations page
  if (isEmpty()) return; // nothing to cite

  // 0. add cross references from the bib files to the cite dictionary
  QFile f;
  const StringVector &citeDataList = Config_getList(CITE_BIB_FILES);
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = bibdata.c_str();
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    QFileInfo fi(bibFile);
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        f.setName(bibFile);
        if (!f.open(IO_ReadOnly))
        {
          err("could not open file %s for reading\n",bibFile.data());
        }
        QCString doc;
        QCString input(fi.size()+1);
        f.readBlock(input.rawData(),fi.size());
        f.close();
        input.at(fi.size())='\0';
        int pos=0;
        int s;
        while ((s=input.find('\n',pos))!=-1)
        {
          QCString line = input.mid((uint)pos,(uint)(s-pos));
          pos=s+1;

	  int i;
          if ((i = line.find("crossref")) != -1) /* assumption cross reference is on one line and the only item */
          {
            int j=line.find("{",i);
            int k=line.find("}",i);
            if (j!=-1 && k!=-1)
            {
              QCString label = line.mid((uint)(j+1),(uint)(k-j-1));
              if (p->entries.find(label.data())==p->entries.end()) // not found yet
              {
                insert(label);
              }
            }
          }
        }
      }
    }
    else if (!fi.exists())
    {
      err("bib file %s not found!\n",bibFile.data());
    }
  }

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QCString outputDir = Config_getString(OUTPUT_DIRECTORY);
  QCString citeListFile = outputDir+"/citelist.doc";
  f.setName(citeListFile);
  if (!f.open(IO_WriteOnly))
  {
    err("could not open file %s for writing\n",citeListFile.data());
  }
  FTextStream t(&f);
  t << "<!-- BEGIN CITATIONS -->" << endl;
  t << "<!--" << endl;
  for (const auto &it : p->entries)
  {
    t << "\\citation{" << it.second->label() << "}" << endl;
  }
  t << "-->" << endl;
  t << "<!-- END CITATIONS -->" << endl;
  t << "<!-- BEGIN BIBLIOGRAPHY -->" << endl;
  t << "<!-- END BIBLIOGRAPHY -->" << endl;
  f.close();

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
  QDir thisDir;
  thisDir.mkdir(bibOutputDir);
  int i = 0;
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = bibdata.c_str();
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    QFileInfo fi(bibFile);
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

  QString oldDir = QDir::currentDirPath();
  QDir::setCurrent(outputDir);

  // 5. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  int exitCode;
  Portable::sysTimerStop();
  if ((exitCode=Portable::system("perl","\""+bib2xhtmlFile+"\" "+bibOutputFiles+" \""+
                         citeListFile+"\"")) != 0)
  {
    err("Problems running bibtex. Verify that the command 'perl --version' works from the command line. Exit code: %d\n",
        exitCode);
  }
  Portable::sysTimerStop();

  QDir::setCurrent(oldDir);

  // 6. read back the file
  f.setName(citeListFile);
  if (!f.open(IO_ReadOnly))
  {
    err("could not open file %s for reading\n",citeListFile.data());
  }

  QCString doc;
  QFileInfo fi(citeListFile);
  QCString input(fi.size()+1);
  f.readBlock(input.rawData(),fi.size());
  f.close();
  input.at(fi.size())='\0';

  bool insideBib=FALSE;
  int pos=0,s;
  //printf("input=[%s]\n",input.data());
  while ((s=input.find('\n',pos))!=-1)
  {
    QCString line = input.mid((uint)pos,(uint)(s-pos));
    //printf("pos=%d s=%d line=[%s]\n",pos,s,line.data());
    pos=s+1;

    if      (line.find("<!-- BEGIN BIBLIOGRAPHY")!=-1) insideBib=TRUE;
    else if (line.find("<!-- END BIBLIOGRAPH")!=-1)    insideBib=FALSE;
    // determine text to use at the location of the @cite command
    if (insideBib && (i=line.find("name=\"CITEREF_"))!=-1)
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
        label = substitute(substitute(label,"&ndash;","--"),"&mdash;","---");
        line = line.left(ui+14) + label + line.right(line.length()-uj);
        auto it = p->entries.find(label.data());
        //printf("label='%s' number='%s' => %p\n",label.data(),number.data(),it->second.get());
        if (it!=p->entries.end())
        {
          it->second->setText(number);
        }
      }
    }
    if (insideBib) doc+=line+"\n";
  }
  //printf("doc=[%s]\n",doc.data());

  // 7. add it as a page
  addRelatedPage(fileName(),theTranslator->trCiteReferences(),doc,fileName(),1);

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
      fi.setFile(bibFile);
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
        err("bib file %s not found!\n",bibFile.data());
      }
    }
  }

  // 9. Remove temporary files
  if (RM_TMP_FILES)
  {
    thisDir.remove(citeListFile);
    thisDir.remove(doxygenBstFile);
    thisDir.remove(bib2xhtmlFile);
    // we might try to remove too many files as empty files didn't get a corresponding new file
    // but the remove function does not emit an error for it and we don't catch the error return
    // so no problem.
    for (size_t j = 1; j <= citeDataList.size(); j++)
    {
      thisDir.remove(bibOutputDir + bibTmpFile + QCString().setNum(j) + ".bib");
    }
    thisDir.rmdir(bibOutputDir);
  }
}

void CitationManager::writeLatexBibliography(FTextStream &t) const
{
  if (p->entries.empty()) return;

  QCString style = Config_getString(LATEX_BIB_STYLE);
  if (style.isEmpty())
  {
    style="plain";
  }
  QCString unit;
  if (Config_getBool(COMPACT_LATEX))
  {
    unit = "section";
  }
  else
  {
    unit = "chapter";
  }
  t << "% Bibliography\n"
       "\\newpage\n"
       "\\phantomsection\n";
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!pdfHyperlinks)
  {
    t << "\\clearemptydoublepage\n";
    t << "\\addcontentsline{toc}{" << unit << "}{" << theTranslator->trCiteReferences() << "}\n";
  }
  t << "\\bibliographystyle{" << style << "}\n"
       "\\bibliography{";
  const StringVector &citeDataList = Config_getList(CITE_BIB_FILES);
  int i = 0;
  for (const auto &bibdata : citeDataList)
  {
    QCString bibFile = bibdata.c_str();
    // Note: file can now have multiple dots
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    QFileInfo fi(bibFile);
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        if (i) t << ",";
        i++;
        t << bibTmpFile << QCString().setNum(i);
      }
    }
  }
  t << "}\n";
  if (pdfHyperlinks)
  {
    t << "\\addcontentsline{toc}{" << unit << "}{" << theTranslator->trCiteReferences() << "}\n";
  }
  t << "\n";
}

