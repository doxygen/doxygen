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
#include <qdir.h>

//--------------------------------------------------------------------------

static const char *doxygen_bst =
#include "doxygen_bst.h"
;

static const char *bib2xhtml_pl =
#include "bib2xhtml.h"
;

//--------------------------------------------------------------------------

const QCString CiteConsts::fileName("citelist");
const QCString CiteConsts::anchorPrefix("CITEREF_");

//--------------------------------------------------------------------------

CiteDict::CiteDict(int size) : m_entries(size, FALSE)
{ 
  m_entries.setAutoDelete(TRUE);
}

static QCString getListOfBibFiles(const QCString &sep,bool namesOnly)
{
  QCString result;
  QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    int i;
    QCString bibFile = bibdata;
    if (namesOnly && bibFile.right(4)==".bib") // strip extension
    {
      bibFile = bibFile.left(bibFile.length()-4);
    }
    if ((i=bibFile.findRev('/'))!=-1) // strip path
    {
      bibFile = bibFile.mid(i+1);
    }
    if (!bibFile.isEmpty())
    {
      if (namesOnly) // bare names
      {
        result+=bibFile;
      }
      else // add quotes for paths with spaces
      {
        result+="\""+bibFile+"\""; 
      }
      bibdata = citeDataList.next();
      if (bibdata)
      {
        result+=sep;
      }
    }
    else
    {
      bibdata = citeDataList.next();
    }
  }
  return result;
}

void CiteDict::writeLatexBibliography(FTextStream &t)
{
  if (m_entries.isEmpty())
    return;

  QCString style = Config_getString("LATEX_BIB_STYLE");
  if (style.isEmpty())
    style="plain";
  t << "% Bibliography\n"
       "\\newpage\n"
       "\\phantomsection\n"
       "\\addcontentsline{toc}{part}{" << theTranslator->trCiteReferences() << "}\n"
       "\\bibliographystyle{" << style << "}\n"
       "\\bibliography{" << getListOfBibFiles(",",TRUE) << "}\n"
       "\n";
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
  QStrList &citeBibFiles = Config_getList("CITE_BIB_FILES");
  return (citeBibFiles.count()==0 || m_entries.isEmpty());
}

void CiteDict::generatePage() const
{
  //printf("** CiteDict::generatePage() count=%d\n",m_ordering.count());

  // do not generate an empty citations page
  if (isEmpty()) return; // nothing to cite

  // 1. generate file with markers and citations to OUTPUT_DIRECTORY
  QFile f;
  QCString outputDir = Config_getString("OUTPUT_DIRECTORY");
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
  QCString bib2xhtmlFile = outputDir+"/bib2xhtml.pl";
  f.setName(bib2xhtmlFile);
  QCString bib2xhtml = bib2xhtml_pl;
  if (!f.open(IO_WriteOnly)) 
  {
    err("could not open file %s for writing\n",bib2xhtmlFile.data());
  }
  f.writeBlock(bib2xhtml, bib2xhtml.length());
  f.close();

  // 3. generate doxygen.bst
  QCString doxygenBstFile = outputDir+"/doxygen.bst";
  QCString bstData = doxygen_bst;
  f.setName(doxygenBstFile);
  if (!f.open(IO_WriteOnly)) 
  {
    err("could not open file %s for writing\n",doxygenBstFile.data());
  }
  f.writeBlock(bstData, bstData.length());
  f.close();

  // 4. for html we just copy the bib files to the output so that
  //    bibtex can find them without path (bibtex doesn't support path's
  //    with spaces!)
  QList<QCString> tempFiles;
  tempFiles.setAutoDelete(TRUE);
  QDir thisDir;
  if (Config_getBool("GENERATE_HTML"))
  {
    // copy bib files to the latex output dir
    QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
    QCString bibOutputDir = outputDir+"/";
    QFileInfo fo(bibOutputDir);
    const char *bibdata = citeDataList.first();
    while (bibdata)
    {
      QCString bibFile = bibdata;
      if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
      QFileInfo fi(bibFile);
      if (fi.exists() && fi.dirPath(TRUE)!=fo.absFilePath())
      {
        if (!bibFile.isEmpty())
        {
          QCString destFile=bibOutputDir+fi.fileName().data();
          copyFile(bibFile,destFile);
          tempFiles.append(new QCString(destFile));
        }
      }
      else if (!fi.exists())
      {
        err("bib file %s not found!\n",bibFile.data());
      }
      bibdata = citeDataList.next();
    }
  }

  QCString oldDir = QDir::currentDirPath().utf8();
  QDir::setCurrent(outputDir);

  // 5. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  portable_system("perl","\""+bib2xhtmlFile+"\" "+getListOfBibFiles(" ",FALSE)+" \""+
                         citeListFile+"\"");

  QDir::setCurrent(oldDir);

  // 6. read back the file
  f.setName(citeListFile);
  if (!f.open(IO_ReadOnly)) 
  {
    err("could not open file %s/citelist.doc for reading\n",outputDir.data());
  }
  bool insideBib=FALSE;
  
  QCString doc;
  QFileInfo fi(citeListFile);
  QCString input(fi.size()+1);
  f.readBlock(input.data(),fi.size());
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
    if (insideBib && (i=line.find("<a name=\"CITEREF_"))!=-1)
    {
      int j=line.find("\">[");
      int k=line.find("]</a>");
      if (j!=-1 && k!=-1)
      {
        QCString label = line.mid(i+17,j-i-17);
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
  if (Config_getBool("GENERATE_LATEX"))
  {
    // copy bib files to the latex output dir
    QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
    QCString latexOutputDir = Config_getString("LATEX_OUTPUT")+"/";
    const char *bibdata = citeDataList.first();
    while (bibdata)
    {
      QCString bibFile = bibdata;
      if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
      QFileInfo fi(bibFile);
      if (fi.exists())
      {
        if (!bibFile.isEmpty())
        {
          copyFile(bibFile,latexOutputDir+fi.fileName().data());
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
  while (!tempFiles.isEmpty()) 
  {
    QCString *s=tempFiles.take();
    thisDir.remove(*s);
  }
}

