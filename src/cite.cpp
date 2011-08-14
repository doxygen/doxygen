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

//--------------------------------------------------------------------------

const QCString CiteConsts::fileName("citelist");
const QCString CiteConsts::anchorPrefix("CITEREF_");

//--------------------------------------------------------------------------

CiteDict::CiteDict(int size) : m_entries(size, FALSE)
{ 
  m_ordering.setAutoDelete(TRUE); 
  m_entries.setAutoDelete(TRUE);
}

bool CiteDict::writeAux()
{
  //msg("..writing aux file\n");
  QCString auxFileName(m_baseFileName + ".aux");
  QFile auxFile(auxFileName);
  if (!auxFile.open(IO_WriteOnly)) 
    // point it to something valid, because warn() relies on it
  {
    err("Error opening file %s for output\n", auxFileName.data()); 
    return FALSE;
  }
  FTextStream t(&auxFile);

  QDictIterator<CiteInfo> cdi(m_entries);
  for (CiteInfo *ci = 0; (ci=cdi.current()); ++cdi)
  {
    t << "\\citation{" << ci->label << "}\n";
  }

  t << "\\bibstyle{" << m_baseFileName << "}\n";

  t << "\\bibdata{";
  QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    QCString bibFile = bibdata;
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    if (!bibFile.isEmpty())
    {
      QFileInfo fi(bibFile);
      if (fi.exists())
      {
        if (!copyFile(bibFile,m_baseFileName+"_"+bibFile))
        {
          return FALSE;
        }
        t << m_baseFileName+"_"+bibFile;
        bibdata = citeDataList.next();
        if (bibdata)
        {
          t << ",";
        }
      }
      else
      {
        err("The file %s specified at CITE_BIB_FILES could not be read!\n",bibdata);
        return FALSE;
      }
    }
    else
    {
      bibdata = citeDataList.next();
    }
  }
  t << "}\n";
  return TRUE;
}

bool CiteDict::writeBst()
{
  //msg("..writing bst file\n");
  QCString fileName = m_baseFileName + ".bst";
  QCString bstData = doxygen_bst;
  QFile f(fileName);
  if (!f.open(IO_WriteOnly)) 
  {
    err("error: could not open file %s for writing\n",fileName.data());
    return FALSE;
  }
  f.writeBlock(bstData, bstData.length());
  return TRUE;
}

void CiteDict::writeLatexBibliography(FTextStream &t)
{
  if (m_entries.count()==0) return;
  QCString style = Config_getString("LATEX_BIB_STYLE");
  if (style.isEmpty()) style="plain";
  t << "\\newpage \\bibliographystyle{" << style << "}" << endl;
  t << "\\bibliography{";
  QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    QCString bibFile = bibdata;
    if (!bibFile.isEmpty())
    {
      t << bibFile;
      bibdata = citeDataList.next();
      if (bibdata)
      {
        t << ",";
      }
    }
    else
    {
      bibdata = citeDataList.next();
    }
  }
  t << "}" << endl;
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
  m_ordering.clear();
}

bool CiteDict::execute()
{
  //msg("..running bibtex\n");
  bool result=TRUE;
  QCString auxFileName(m_baseFileName + ".aux");
  int splitPoint = auxFileName.findRev('/');
  QCString dirname = auxFileName.left(splitPoint);
  QCString basename = auxFileName.mid(splitPoint + 1);
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  QDir::setCurrent(dirname);
  QCString args;
  args += "-terse ";
  args += basename;
  portable_system("bibtex", args);
  int exitCode;
  if ((exitCode=portable_system("bibtex",args))!=0)
  {
    err("Problems running bibtex: exit code=%d, command='bibtex', arguments='%s'\n",
      exitCode,args.data());
    result=FALSE;
  }
  QDir::setCurrent(oldDir);
  return result;
}

void CiteDict::parse()
{
  //msg("..parsing bbl file\n");
  QCString bblFileName(m_baseFileName + ".bbl");
  QFile f(bblFileName);
  if (!f.open(IO_ReadOnly))
  {
    err("error: could not open file %s\n",bblFileName.data());
    return;
  }

  m_ordering.clear();

  QTextStream t(&f);
  QCString label;
  QCString line;
  while (!t.eof())
  {
    label=t.readLine();
    if (label.isEmpty())
    {
      continue;
    }

    CiteInfo* ci = m_entries.find(label);
    if (!ci) insert(label); 

    // BibTeX has its own way of sorting references,
    // depending on the .bst file
    m_ordering.append(new QCString(label));

    ci->text=t.readLine();

    while (!(line=t.readLine()).isEmpty())
    {
      ci->fullText += line + '\n';
    }

    // FIXME: process LaTeX-style accents properly
    ci->fullText = substitute(substitute(substitute(ci->fullText,
            "{", ""),
          "}", ""),
        "~", " ");
    
    //printf("BIB ENTRY: %s -> %s: %s\n", ci->label.data(), ci->text.data(), ci->fullText.data());
  }
}

void CiteDict::clean()
{
  // clean
  QDir thisDir;
  QStrList &citeDataList = Config_getList("CITE_BIB_FILES");
  const char *bibdata = citeDataList.first();
  while (bibdata)
  {
    QCString bibFile = bibdata;
    if (!bibFile.isEmpty() && bibFile.right(4)!=".bib") bibFile+=".bib";
    if (!bibFile.isEmpty()) thisDir.remove(m_baseFileName+"_"+bibFile);
    bibdata = citeDataList.next();
  }

  thisDir.remove(m_baseFileName + ".aux");
  thisDir.remove(m_baseFileName + ".bbl");
  thisDir.remove(m_baseFileName + ".blg");
  thisDir.remove(m_baseFileName + ".tmp");
  thisDir.remove(m_baseFileName + ".bst");

  QCString &outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty()) 
  {
    outputDirectory=QDir::currentDirPath();
  }
  QDir d(outputDirectory);
  d.rmdir("bib");
}

void CiteDict::resolve()
{
  QStrList &citeBibFiles = Config_getList("CITE_BIB_FILES");
  if (citeBibFiles.count()==0 || m_entries.count()==0) return; // nothing to cite

  QCString &outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty()) 
  {
    outputDirectory=QDir::currentDirPath();
  }
  QDir d(outputDirectory);
  d.mkdir("bib");

  uint pid = portable_pid();
  m_baseFileName.sprintf("doxygen_bibtex_%d",pid);
  m_baseFileName.prepend(outputDirectory+"/bib/");

  if (writeAux() && writeBst() && execute()) 
  {
    parse();
    clean();
  }

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
      if (!bibFile.isEmpty())
      {
        copyFile(bibFile,latexOutputDir+bibFile);
      }
      bibdata = citeDataList.next();
    }
  }
}

bool CiteDict::isEmpty() const
{
  QStrList &citeBibFiles = Config_getList("CITE_BIB_FILES");
  return (citeBibFiles.count()==0 || m_ordering.isEmpty());
}

void CiteDict::generatePage() const
{
  //printf("** CiteDict::generatePage() count=%d\n",m_ordering.count());

  // do not generate an empty citations page
  if (isEmpty()) return; // nothing to cite

  QCString doc;
  doc += "<dl class=\"citelist\">";

  QListIterator<QCString> it(m_ordering);
  QCString *s;
  for (it.toFirst();(s=it.current());++it)
  {
    CiteInfo* ci = m_entries.find(*s);
    doc += " <dt>";
    doc +=  "\\anchor ";
    doc += CiteConsts::anchorPrefix + ci->label;
    doc += "\n";
    doc += ci->text;
    doc += "</dt><dd> ";
    doc += ci->fullText;
    doc += "</dd>";
  }
  doc += "</dl>\n";
  //printf("addRelatedPage with doc='%s'\n",doc.data());
  addRelatedPage(CiteConsts::fileName,
       theTranslator->trCiteReferences(),doc,0,CiteConsts::fileName,1,0,0,0);
}

