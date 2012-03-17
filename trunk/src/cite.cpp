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

static QCString getListOfBibFiles(const QCString &sep,bool stripExtension)
{
  QCString result;
  QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    int i;
    QCString bibFile = bibdata;
    if (stripExtension && bibFile.right(4)==".bib")
    {
      bibFile = bibFile.left(bibFile.length()-4);
    }
    if (stripExtension && (i=bibFile.findRev('/'))!=-1)
    {
      bibFile = bibFile.mid(i+1);
    }
    if (!bibFile.isEmpty())
    {
      result+=bibFile;
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
  if (m_entries.count()==0) return;
  QCString style = Config_getString("LATEX_BIB_STYLE");
  if (style.isEmpty()) style="plain";
  t << "\\newpage \\bibliographystyle{" << style << "}" << endl;
  t << "\\bibliography{" << getListOfBibFiles(",",TRUE) << "}" << endl;
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
    err("error: could not open file %s for writing\n",citeListFile.data());
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
    err("error: could not open file %s for writing\n",bib2xhtmlFile.data());
  }
  f.writeBlock(bib2xhtml, bib2xhtml.length());
  f.close();

  // 3. generate doxygen.bst
  QCString doxygenBstFile = outputDir+"/doxygen.bst";
  QCString bstData = doxygen_bst;
  f.setName(doxygenBstFile);
  if (!f.open(IO_WriteOnly)) 
  {
    err("error: could not open file %s for writing\n",doxygenBstFile.data());
  }
  f.writeBlock(bstData, bstData.length());
  f.close();

  // 4. run bib2xhtml perl script on the generated file which will insert the
  //    bibliography in citelist.doc
  portable_system("perl",bib2xhtmlFile+" "+getListOfBibFiles(" ",FALSE)+" "+
                         citeListFile);

  // 5. read back the file
  f.setName(citeListFile);
  if (!f.open(IO_ReadOnly)) 
  {
    err("error: could not open file %s/citelist.doc for reading\n",outputDir.data());
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

  // 6. add it as a page
  addRelatedPage(CiteConsts::fileName,
       theTranslator->trCiteReferences(),doc,0,CiteConsts::fileName,1,0,0,0);

  // 7. for latex we just copy the bib files to the output and let 
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
        err("Error: bib file %s not found!\n",bibFile.data());
      }
      bibdata = citeDataList.next();
    }
  }

  // 8. Remove temporary files
  QDir thisDir;
  thisDir.remove(citeListFile);
  thisDir.remove(doxygenBstFile);
  thisDir.remove(bib2xhtmlFile);

}

