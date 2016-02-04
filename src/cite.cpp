/******************************************************************************
 *
 * Copyright (C) 2011 by Dimitri van Heesch
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
#include "portable.h"
#include "config.h"
#include "message.h"
#include "util.h"
#include "language.h"
#include "ftextstream.h"
#include "resourcemgr.h"
#include <qdir.h>

//--------------------------------------------------------------------------

const QCString CiteConsts::fileName("citelist");
const QCString CiteConsts::anchorPrefix("CITEREF_");
const QCString bibTmpFile("bibTmpFile_");
const QCString bibTmpDir("bibTmpDir/");

//--------------------------------------------------------------------------

CiteDict::CiteDict(int size) : m_entries(size, FALSE)
{ 
  m_entries.setAutoDelete(TRUE);
}

void CiteDict::writeLatexBibliography(FTextStream &t)
{
  if (m_entries.isEmpty())
    return;

  QCString style = Config_getString(LATEX_BIB_STYLE);
  if (style.isEmpty())
    style="plain";
  QCString unit;
  if (Config_getBool(COMPACT_LATEX))
    unit = "section";
  else
    unit = "chapter";
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
  QStrList &citeDataList = Config_getList(CITE_BIB_FILES);
  int i = 0;
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    QCString bibFile = bibdata;
    // Note: file can now have multiple dots
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    QFileInfo fi(bibFile);
    if (fi.exists())
    {
      if (!bibFile.isEmpty())
      {
        if (i) t << ",";
        i++;
        t << bibTmpFile << QString().setNum(i);
      }
    }
    bibdata = citeDataList.next();
  }
  t << "}\n";
  if (pdfHyperlinks)
  {
    t << "\\addcontentsline{toc}{" << unit << "}{" << theTranslator->trCiteReferences() << "}\n";
  }
  t << "\n";
}

void CiteDict::insert(const char *label)
{
  m_entries.insert(label,new CiteInfo(label));
}

CiteInfo *CiteDict::find(const char *label) const
{
  return label ? m_entries.find(label) : 0;
}

void CiteDict::clear()
{
  m_entries.clear();
}

bool CiteDict::isEmpty() const
{
  QStrList &citeBibFiles = Config_getList(CITE_BIB_FILES);
  return (citeBibFiles.count()==0 || m_entries.isEmpty());
}

void CiteDict::generatePage() const
{
  //printf("** CiteDict::generatePage() count=%d\n",m_ordering.count());

  // do not generate an empty citations page
  if (isEmpty()) return; // nothing to cite

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QFile f;
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
  QDictIterator<CiteInfo> it(m_entries);
  CiteInfo *ci;
  for (it.toFirst();(ci=it.current());++it)
  {
    t << "\\citation{" << ci->label << "}" << endl;
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
  QStrList &citeDataList = Config_getList(CITE_BIB_FILES);
  QCString bibOutputDir = outputDir+"/"+bibTmpDir;
  QCString bibOutputFiles = "";
  QDir thisDir;
  thisDir.mkdir(bibOutputDir);
  const char *bibdata = citeDataList.first();
  int i = 0;
  while (bibdata)
  {
    QCString bibFile = bibdata;
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
    else if (!fi.exists())
    {
      err("bib file %s not found!\n",bibFile.data());
    }
    bibdata = citeDataList.next();
  }

  QString oldDir = QDir::currentDirPath();
  QDir::setCurrent(outputDir);

  // 5. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  int exitCode;
  portable_sysTimerStop();
  if ((exitCode=portable_system("perl","\""+bib2xhtmlFile+"\" "+bibOutputFiles+" \""+
                         citeListFile+"\"")) != 0)
  {
    err("Problems running bibtex. Verify that the command 'perl --version' works from the command line. Exit code: %d\n",
        exitCode);
  }
  portable_sysTimerStop();

  QDir::setCurrent(oldDir);

  // 6. read back the file
  f.setName(citeListFile);
  if (!f.open(IO_ReadOnly)) 
  {
    err("could not open file %s for reading\n",citeListFile.data());
  }
  bool insideBib=FALSE;
  
  QCString doc;
  QFileInfo fi(citeListFile);
  QCString input(fi.size()+1);
  f.readBlock(input.rawData(),fi.size());
  f.close();
  input.at(fi.size())='\0';
  int p=0,s;
  //printf("input=[%s]\n",input.data());
  while ((s=input.find('\n',p))!=-1)
  {
    QCString line = input.mid(p,s-p);
    //printf("p=%d s=%d line=[%s]\n",p,s,line.data());
    p=s+1;

    if      (line.find("<!-- BEGIN BIBLIOGRAPHY")!=-1) insideBib=TRUE;
    else if (line.find("<!-- END BIBLIOGRAPH")!=-1)    insideBib=FALSE;
    else if (insideBib) doc+=line+"\n";
    int i;
    // determine text to use at the location of the @cite command
    if (insideBib && (i=line.find("name=\"CITEREF_"))!=-1)
    {
      int j=line.find("\">[");
      int k=line.find("]</a>");
      if (j!=-1 && k!=-1)
      {
        QCString label = line.mid(i+14,j-i-14);
        QCString number = line.mid(j+2,k-j-1);
        CiteInfo *ci = m_entries.find(label);
        //printf("label='%s' number='%s' => %p\n",label.data(),number.data(),ci);
        if (ci)
        {
          ci->text = number;
        }
      }
    }
  }
  //printf("doc=[%s]\n",doc.data());

  // 7. add it as a page
  addRelatedPage(CiteConsts::fileName,
       theTranslator->trCiteReferences(),doc,0,CiteConsts::fileName,1,0,0,0);

  // 8. for latex we just copy the bib files to the output and let 
  //    latex do this work.
  if (Config_getBool(GENERATE_LATEX))
  {
    // copy bib files to the latex output dir
    QStrList &citeDataList = Config_getList(CITE_BIB_FILES);
    QCString latexOutputDir = Config_getString(LATEX_OUTPUT)+"/";
    int i = 0;
    const char *bibdata = citeDataList.first();
    while (bibdata)
    {
      QCString bibFile = bibdata;
      // Note: file can now have multiple dots
      if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
      QFileInfo fi(bibFile);
      if (fi.exists())
      {
        if (!bibFile.isEmpty())
        {
          // bug_700510, multile times the same name were overwriting; creating new names
          // also for names with spaces
          ++i;
          copyFile(bibFile,latexOutputDir + bibTmpFile + QCString().setNum(i) + ".bib");
        }
      }
      else
      {
        err("bib file %s not found!\n",bibFile.data());
      }
      bibdata = citeDataList.next();
    }
  }

  // 9. Remove temporary files
  thisDir.remove(citeListFile);
  thisDir.remove(doxygenBstFile);
  thisDir.remove(bib2xhtmlFile);
  // we might try to remove too many files as empty files didn't get a coresponding new file
  // but the remove function does not emit an error for it and we don't catch the error return
  // so no problem.
  for (unsigned int j = 1; j <= citeDataList.count(); j++)
  {
    thisDir.remove(bibOutputDir + bibTmpFile + QCString().setNum(j) + ".bib");
  }
  thisDir.rmdir(bibOutputDir);
}

