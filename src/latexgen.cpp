/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <cstdlib>
#include <sstream>

#include "latexgen.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "version.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "dotclassgraph.h"
#include "dotdirdeps.h"
#include "dotgroupcollaboration.h"
#include "dotincldepgraph.h"
#include "pagedef.h"
#include "docparser.h"
#include "latexdocvisitor.h"
#include "dirdef.h"
#include "cite.h"
#include "groupdef.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"
#include "resourcemgr.h"
#include "portable.h"
#include "fileinfo.h"
#include "utf8.h"

static QCString g_header;
static QCString g_footer;

LatexCodeGenerator::LatexCodeGenerator(std::ostream &t,const QCString &relPath,const QCString &sourceFileName)
  : m_t(nullptr), m_relPath(relPath), m_sourceFileName(sourceFileName)
{
  m_prettyCode=Config_getBool(LATEX_SOURCE_CODE);
  setTextStream(t);
}

LatexCodeGenerator::LatexCodeGenerator() : m_t(nullptr)
{
  m_prettyCode=Config_getBool(LATEX_SOURCE_CODE);
}

void LatexCodeGenerator::setTextStream(std::ostream &t)
{
  m_t.rdbuf(t.rdbuf());
}

void LatexCodeGenerator::setRelativePath(const QCString &path)
{
  m_relPath = path;
}

void LatexCodeGenerator::setSourceFileName(const QCString &name)
{
  m_sourceFileName = name;
}

void LatexCodeGenerator::codify(const char *str)
{
  if (str)
  {
    const signed char *p=(const signed char*)str;
    signed char c;
    //char cs[5];
    int spacesToNextTabStop;
    int tabSize = Config_getInt(TAB_SIZE);
    static THREAD_LOCAL signed char *result = NULL;
    static THREAD_LOCAL int lresult = 0;
    int i;
    while ((c=*p))
    {
      switch(c)
      {
        case 0x0c: p++;  // remove ^L
                   break;
        case ' ':  m_t <<" ";
                   m_col++;
                   p++;
                   break;
        case '^':  m_t <<"\\string^";
                   m_col++;
                   p++;
                   break;
        case '\t': spacesToNextTabStop =
                         tabSize - (m_col%tabSize);
                   for (i = 0; i < spacesToNextTabStop; i++) m_t <<" ";
                   m_col+=spacesToNextTabStop;
                   p++;
                   break;
        case '\n': m_t << '\n';
                   m_col=0;
                   p++;
                   break;
        default:
                   i=0;

#undef  COPYCHAR
// helper macro to copy a single utf8 character, dealing with multibyte chars.
#define COPYCHAR() do {                                           \
                     int bytes = getUTF8CharNumBytes(c);          \
                     if (lresult < (i + bytes + 1))               \
                     {                                            \
                       lresult += 512;                            \
                       result = (signed char *)realloc(result, lresult); \
                     }                                            \
                     for (int j=0; j<bytes && *p; j++)            \
                     {                                            \
                       result[i++]=*p++;                          \
                     }                                            \
                     m_col++;                                     \
                   } while(0)

                   // gather characters until we find whitespace or another special character
                   COPYCHAR();
                   while ((c=*p) &&
                          c!=0x0c && c!='\t' && c!='\n' && c!=' ' && c!='^'
                         )
                   {
                     COPYCHAR();
                   }
                   result[i]=0; // add terminator
                   filterLatexString(m_t,(const char *)result,
                                     false, // insideTabbing
                                     true,  // insidePre
                                     false, // insideItem
                                     m_usedTableLevel>0, // insideTable
                                     false  // keepSpaces
                                    );
                   break;
      }
    }
  }
}


void LatexCodeGenerator::writeCodeLink(const char *ref,const char *f,
                                   const char *anchor,const char *name,
                                   const char *)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  int l = qstrlen(name);
  if (!ref && usePDFLatex && pdfHyperlinks)
  {
    m_t << "\\mbox{\\hyperlink{";
    if (f) m_t << stripPath(f);
    if (f && anchor) m_t << "_";
    if (anchor) m_t << anchor;
    m_t << "}{";
    codify(name);
    m_t << "}}";
  }
  else
  {
    codify(name);
  }
  m_col+=l;
}

void LatexCodeGenerator::writeLineNumber(const char *ref,const char *fileName,const char *anchor,int l)
{
  bool usePDFLatex = Config_getBool(USE_PDFLATEX);
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_doxyCodeLineOpen)
  {
    m_t << "\\DoxyCodeLine{";
    m_doxyCodeLineOpen = TRUE;
  }
  if (m_prettyCode)
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    if (fileName && !m_sourceFileName.isEmpty())
    {
      QCString lineAnchor;
      lineAnchor.sprintf("_l%05d",l);
      lineAnchor.prepend(stripExtensionGeneral(m_sourceFileName, ".tex"));
      //if (!m_prettyCode) return;
      if (usePDFLatex && pdfHyperlinks)
      {
        m_t << "\\Hypertarget{" << stripPath(lineAnchor) << "}";
      }
      writeCodeLink(ref,fileName,anchor,lineNumber,0);
    }
    else
    {
      codify(lineNumber);
    }
    m_t << " ";
  }
  else
  {
    m_t << l << " ";
  }
  m_col=0;
}


void LatexCodeGenerator::startCodeLine(bool)
{
  m_col=0;
  if (!m_doxyCodeLineOpen)
  {
    m_t << "\\DoxyCodeLine{";
    m_doxyCodeLineOpen = TRUE;
  }
}

void LatexCodeGenerator::endCodeLine()
{
  if (m_doxyCodeLineOpen)
  {
    m_t << "}";
    m_doxyCodeLineOpen = FALSE;
  }
  codify("\n");
}

void LatexCodeGenerator::startFontClass(const char *name)
{
  m_t << "\\textcolor{" << name << "}{";
}

void LatexCodeGenerator::endFontClass()
{
  m_t << "}";
}

void LatexCodeGenerator::startCodeFragment(const char *style)
{
  m_t << "\n\\begin{" << style << "}{" << m_usedTableLevel << "}\n";
}

void LatexCodeGenerator::endCodeFragment(const char *style)
{
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

  m_t << "\\end{" << style << "}\n";
}


//-------------------------------

LatexGenerator::LatexGenerator() : OutputGenerator(Config_getString(LATEX_OUTPUT))
{
  //printf("LatexGenerator::LatexGenerator() m_insideTabbing=FALSE\n");
}

LatexGenerator::LatexGenerator(const LatexGenerator &og) : OutputGenerator(og)
{
}

LatexGenerator &LatexGenerator::operator=(const LatexGenerator &og)
{
  OutputGenerator::operator=(og);
  return *this;
}

std::unique_ptr<OutputGenerator> LatexGenerator::clone() const
{
  return std::make_unique<LatexGenerator>(*this);
}

LatexGenerator::~LatexGenerator()
{
}

static void writeLatexMakefile()
{
  bool generateBib = !CitationManager::instance().isEmpty();
  QCString fileName=Config_getString(LATEX_OUTPUT)+"/Makefile";
  std::ofstream t(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!t.is_open())
  {
    term("Could not open file %s for writing\n",fileName.data());
  }
  // inserted by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  QCString latex_command = theTranslator->latexCommandName();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME);
  // end insertion by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "LATEX_CMD=" << latex_command << "\n"
      << "\n"
      << "all: refman.dvi\n"
      << "\n"
      << "ps: refman.ps\n"
      << "\n"
      << "pdf: refman.pdf\n"
      << "\n"
      << "ps_2on1: refman_2on1.ps\n"
      << "\n"
      << "pdf_2on1: refman_2on1.pdf\n"
      << "\n"
      << "refman.ps: refman.dvi\n"
      << "\tdvips -o refman.ps refman.dvi\n"
      << "\n";
    t << "refman.pdf: refman.ps\n";
    t << "\tps2pdf refman.ps refman.pdf\n\n";
    t << "refman.dvi: clean refman.tex doxygen.sty\n"
      << "\techo \"Running latex...\"\n"
      << "\t$(LATEX_CMD) refman.tex\n"
      << "\techo \"Running makeindex...\"\n"
      << "\t" << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "\techo \"Running bibtex...\"\n";
      t << "\tbibtex refman\n";
      t << "\techo \"Rerunning latex....\"\n";
      t << "\t$(LATEX_CMD) refman.tex\n";
    }
    t << "\techo \"Rerunning latex....\"\n"
      << "\t$(LATEX_CMD) refman.tex\n"
      << "\tlatex_count=8 ; \\\n"
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\\n"
      << "\t    do \\\n"
      << "\t      echo \"Rerunning latex....\" ;\\\n"
      << "\t      $(LATEX_CMD) refman.tex ; \\\n"
      << "\t      latex_count=`expr $$latex_count - 1` ;\\\n"
      << "\t    done\n"
      << "\t" << mkidx_command << " refman.idx\n"
      << "\t$(LATEX_CMD) refman.tex\n\n"
      << "refman_2on1.ps: refman.ps\n"
      << "\tpsnup -2 refman.ps >refman_2on1.ps\n"
      << "\n"
      << "refman_2on1.pdf: refman_2on1.ps\n"
      << "\tps2pdf refman_2on1.ps refman_2on1.pdf\n";
  }
  else // use pdflatex for higher quality output
  {
    t << "LATEX_CMD=" << latex_command << "\n"
      << "\n";
    t << "all: refman.pdf\n\n"
      << "pdf: refman.pdf\n\n";
    t << "refman.pdf: clean refman.tex\n";
    t << "\t$(LATEX_CMD) refman\n";
    t << "\t" << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "\tbibtex refman\n";
      t << "\t$(LATEX_CMD) refman\n";
    }
    t << "\t$(LATEX_CMD) refman\n"
      << "\tlatex_count=8 ; \\\n"
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\\n"
      << "\t    do \\\n"
      << "\t      echo \"Rerunning latex....\" ;\\\n"
      << "\t      $(LATEX_CMD) refman ;\\\n"
      << "\t      latex_count=`expr $$latex_count - 1` ;\\\n"
      << "\t    done\n"
      << "\t" << mkidx_command << " refman.idx\n"
      << "\t$(LATEX_CMD) refman\n\n";
  }

  t << "\n"
    << "clean:\n"
    << "\trm -f "
    << "*.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf\n";
}

static void writeMakeBat()
{
#if defined(_MSC_VER)
  QCString dir=Config_getString(LATEX_OUTPUT);
  QCString fileName=dir+"/make.bat";
  QCString latex_command = theTranslator->latexCommandName();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME);
  bool generateBib = !CitationManager::instance().isEmpty();
  std::ofstream t(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!t.is_open())
  {
    term("Could not open file %s for writing\n",fileName.data());
  }
  t << "set Dir_Old=%cd%\r\n";
  t << "cd /D %~dp0\r\n\r\n";
  t << "del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf\r\n\r\n";
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "set LATEX_CMD=" << latex_command << "\r\n";
    t << "%LATEX_CMD% refman.tex\r\n";
    t << "echo ----\r\n";
    t << mkidx_command << " refman.idx\r\n";
    if (generateBib)
    {
      t << "bibtex refman\r\n";
      t << "echo ----\r\n";
      t << "\t%LATEX_CMD% refman.tex\r\n";
    }
    t << "setlocal enabledelayedexpansion\r\n";
    t << "set count=8\r\n";
    t << ":repeat\r\n";
    t << "set content=X\r\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X goto :skip\r\n";
    t << "set /a count-=1\r\n";
    t << "if !count! EQU 0 goto :skip\r\n\r\n";
    t << "echo ----\r\n";
    t << "%LATEX_CMD% refman.tex\r\n";
    t << "goto :repeat\r\n";
    t << ":skip\r\n";
    t << "endlocal\r\n";
    t << mkidx_command << " refman.idx\r\n";
    t << "%LATEX_CMD% refman.tex\r\n";
    t << "dvips -o refman.ps refman.dvi\r\n";
    t << Portable::ghostScriptCommand();
    t << " -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
         "-sOutputFile=refman.pdf -c save pop -f refman.ps\r\n";
  }
  else // use pdflatex
  {
    t << "set LATEX_CMD=" << latex_command << "\r\n";
    t << "%LATEX_CMD% refman\r\n";
    t << "echo ----\r\n";
    t << mkidx_command << " refman.idx\r\n";
    if (generateBib)
    {
      t << "bibtex refman\r\n";
      t << "%LATEX_CMD% refman\r\n";
    }
    t << "echo ----\r\n";
    t << "%LATEX_CMD% refman\r\n\r\n";
    t << "setlocal enabledelayedexpansion\r\n";
    t << "set count=8\r\n";
    t << ":repeat\r\n";
    t << "set content=X\r\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X goto :skip\r\n";
    t << "set /a count-=1\r\n";
    t << "if !count! EQU 0 goto :skip\r\n\r\n";
    t << "echo ----\r\n";
    t << "%LATEX_CMD% refman\r\n";
    t << "goto :repeat\r\n";
    t << ":skip\r\n";
    t << "endlocal\r\n";
    t << mkidx_command << " refman.idx\r\n";
    t << "%LATEX_CMD% refman\r\n";
    t << "cd /D %Dir_Old%\r\n";
    t << "set Dir_Old=\r\n";
  }
#endif
}

void LatexGenerator::init()
{
  QCString dname = Config_getString(LATEX_OUTPUT);
  Dir d(dname.str());
  if (!d.exists() && !d.mkdir(dname.str()))
  {
    term("Could not create output directory %s\n",dname.data());
  }

  if (!Config_getString(LATEX_HEADER).isEmpty())
  {
    g_header=fileToString(Config_getString(LATEX_HEADER));
    //printf("g_header='%s'\n",g_header.data());
  }
  else
  {
    g_header = ResourceMgr::instance().getAsString("header.tex");
  }
  if (!Config_getString(LATEX_FOOTER).isEmpty())
  {
    g_footer=fileToString(Config_getString(LATEX_FOOTER));
    //printf("g_footer='%s'\n",g_footer.data());
  }
  else
  {
    g_footer = ResourceMgr::instance().getAsString("footer.tex");
  }

  writeLatexMakefile();
  writeMakeBat();

  createSubDirs(d);
}

static void writeDefaultStyleSheet(std::ostream &t)
{
  t << ResourceMgr::instance().getAsString("doxygen.sty");
}

void LatexGenerator::writeHeaderFile(std::ostream &t)
{
  t << "% Latex header for doxygen " << getDoxygenVersion() << "\n";
  t << ResourceMgr::instance().getAsString("header.tex");
}

void LatexGenerator::writeFooterFile(std::ostream &t)
{
  t << "% Latex footer for doxygen " << getDoxygenVersion() << "\n";
  t << ResourceMgr::instance().getAsString("footer.tex");
}

void LatexGenerator::writeStyleSheetFile(std::ostream &t)
{
  t << "% stylesheet for doxygen " << getDoxygenVersion() << "\n";
  writeDefaultStyleSheet(t);
}

void LatexGenerator::startFile(const char *name,const char *,const char *,int)
{
#if 0
  setEncoding(Config_getString(LATEX_OUTPUT_ENCODING));
#endif
  QCString fileName=name;
  m_relPath = relativePathToRoot(fileName);
  if (fileName.right(4)!=".tex" && fileName.right(4)!=".sty") fileName+=".tex";
  startPlainFile(fileName);
  m_codeGen.setTextStream(t);
  m_codeGen.setRelativePath(m_relPath);
  m_codeGen.setSourceFileName(stripPath(fileName));
}

void LatexGenerator::endFile()
{
  endPlainFile();
  m_codeGen.setSourceFileName("");
}

//void LatexGenerator::writeIndex()
//{
//  startFile("refman.tex");
//}

void LatexGenerator::startProjectNumber()
{
  t << "\\\\[1ex]\\large ";
}

static QCString extraLatexStyleSheet()
{
  QCString result;
  const StringVector &extraLatexStyles = Config_getList(LATEX_EXTRA_STYLESHEET);
  for (const auto &fileName : extraLatexStyles)
  {
    if (!fileName.empty())
    {
      FileInfo fi(fileName);
      if (fi.exists())
      {
        result += "\\usepackage{";
        if (checkExtension(fi.fileName().c_str(), LATEX_STYLE_EXTENSION))
        {
          // strip the extension, it will be added by the usepackage in the tex conversion process
          result += stripExtensionGeneral(fi.fileName().c_str(), LATEX_STYLE_EXTENSION);
        }
        else
        {
          result += fi.fileName();
        }
        result += "}\n";
      }
    }
  }
  return result;
}

static QCString makeIndex()
{
  QCString result;
  QCString latex_mkidx_command = Config_getString(LATEX_MAKEINDEX_CMD);
  if (!latex_mkidx_command.isEmpty())
  {
    if (latex_mkidx_command[0] == '\\')
      result += latex_mkidx_command;
    else
      result += '\\'+latex_mkidx_command;
  }
  else
  {
    result += "\\makeindex";
  }
  return result;
}

static QCString substituteLatexKeywords(const QCString &str,
                                        const QCString &title)
{
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePdfLatex = Config_getBool(USE_PDFLATEX);
  bool latexBatchmode = Config_getBool(LATEX_BATCHMODE);
  QCString paperType = Config_getString(PAPER_TYPE);

  QCString style = Config_getString(LATEX_BIB_STYLE);
  if (style.isEmpty())
  {
    style="plain";
  }

  std::ostringstream tg(std::ios_base::ate);
  bool timeStamp = Config_getBool(LATEX_TIMESTAMP);
  QCString generatedBy;
  if (timeStamp)
  {
    generatedBy = theTranslator->trGeneratedAt(dateToString(TRUE), Config_getString(PROJECT_NAME));
  }
  else
  {
    generatedBy = theTranslator->trGeneratedBy();
  }
  filterLatexString(tg, generatedBy,
                    false, // insideTabbing
                    false, // insidePre
                    false, // insideItem
                    false, // insideTable
                    false  // keepSpaces
                   );
  generatedBy = tg.str();

  QCString latexFontenc = theTranslator->latexFontenc();

  QCString latexEmojiDirectory = Config_getString(LATEX_EMOJI_DIRECTORY);
  if (latexEmojiDirectory.isEmpty()) latexEmojiDirectory = ".";
  latexEmojiDirectory = substitute(latexEmojiDirectory,"\\","/");

  std::ostringstream tg1(std::ios_base::ate);
  writeExtraLatexPackages(tg1);
  QCString extraLatexPackages = tg1.str();

  std::ostringstream tg2(std::ios_base::ate);
  writeLatexSpecialFormulaChars(tg2);
  QCString latexSpecialFormulaChars = tg2.str();

  QCString formulaMacrofile = Config_getString(FORMULA_MACROFILE);
  if (!formulaMacrofile.isEmpty())
  {
    FileInfo fi(formulaMacrofile.str());
    formulaMacrofile=fi.absFilePath();
    QCString stripMacroFile = fi.fileName().data();
    copyFile(formulaMacrofile,Config_getString(LATEX_OUTPUT) + "/" + stripMacroFile);
  }

  // first substitute generic keywords
  QCString result = substituteKeywords(str,title,
    convertToLaTeX(Config_getString(PROJECT_NAME)),
    convertToLaTeX(Config_getString(PROJECT_NUMBER)),
        convertToLaTeX(Config_getString(PROJECT_BRIEF)));

  // additional LaTeX only keywords
  result = substitute(result,"$latexdocumentpre",theTranslator->latexDocumentPre());
  result = substitute(result,"$latexdocumentpost",theTranslator->latexDocumentPost());
  result = substitute(result,"$generatedby",generatedBy);
  result = substitute(result,"$latexbibstyle",style);
  result = substitute(result,"$latexcitereference",theTranslator->trCiteReferences());
  result = substitute(result,"$latexbibfiles",CitationManager::instance().latexBibFiles());
  result = substitute(result,"$papertype",paperType+"paper");
  result = substitute(result,"$extralatexstylesheet",extraLatexStyleSheet());
  result = substitute(result,"$languagesupport",theTranslator->latexLanguageSupportCommand());
  result = substitute(result,"$latexfontenc",latexFontenc);
  result = substitute(result,"$latexfont",theTranslator->latexFont());
  result = substitute(result,"$latexemojidirectory",latexEmojiDirectory);
  result = substitute(result,"$makeindex",makeIndex());
  result = substitute(result,"$extralatexpackages",extraLatexPackages);
  result = substitute(result,"$latexspecialformulachars",latexSpecialFormulaChars);
  result = substitute(result,"$formulamacrofile",formulaMacrofile);

  // additional LaTeX only conditional blocks
  result = selectBlock(result,"CITATIONS_PRESENT", !CitationManager::instance().isEmpty(),OutputGenerator::Latex);
  result = selectBlock(result,"COMPACT_LATEX",compactLatex,OutputGenerator::Latex);
  result = selectBlock(result,"PDF_HYPERLINKS",pdfHyperlinks,OutputGenerator::Latex);
  result = selectBlock(result,"USE_PDFLATEX",usePdfLatex,OutputGenerator::Latex);
  result = selectBlock(result,"LATEX_TIMESTAMP",timeStamp,OutputGenerator::Latex);
  result = selectBlock(result,"LATEX_BATCHMODE",latexBatchmode,OutputGenerator::Latex);
  result = selectBlock(result,"LATEX_FONTENC",!latexFontenc.isEmpty(),OutputGenerator::Latex);
  result = selectBlock(result,"FORMULA_MACROFILE",!formulaMacrofile.isEmpty(),OutputGenerator::Latex);

  result = removeEmptyLines(result);

  return result;
}

void LatexGenerator::startIndexSection(IndexSections is)
{
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  switch (is)
  {
    case isTitlePageStart:
      t << substituteLatexKeywords(g_header,convertToLaTeX(Config_getString(PROJECT_NAME)));
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Introduction}\n"
      break;
    //case isPackageIndex:
    //  if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
    //  t << "{"; //Package Index}\n"
    //  break;
    case isModuleIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Module Index}\n"
      break;
    case isDirIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Directory Index}\n"
      break;
    case isNamespaceIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Namespace Index}\n"
      break;
    case isClassHierarchyIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Hierarchical Index}\n"
      break;
    case isCompoundIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Annotated Compound Index}\n"
      break;
    case isFileIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Annotated File Index}\n"
      break;
    case isPageIndex:
      if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
      t << "{"; //Annotated Page Index}\n"
      break;
    case isModuleDocumentation:
      {
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            break;
          }
        }
      }
      break;
    case isDirDocumentation:
      {
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            break;
          }
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; // Namespace Documentation}\n":
            break;
          }
        }
      }
      break;
    case isClassDocumentation:
      {
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
              !cd->isEmbeddedInOuterScope() &&
              !cd->isAlias()
             )
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; //Compound Documentation}\n";
            break;
          }
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
                t << "{"; //File Documentation}\n";
                isFirst=FALSE;
                break;
              }
            }
          }
        }
      }
      break;
    case isExampleDocumentation:
      {
        if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
        t << "{"; //Example Documentation}\n";
      }
      break;
    case isPageDocumentation:
      {
        if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
        t << "{"; //Page Documentation}\n";
      }
      break;
    case isPageDocumentation2:
      break;
    case isEndIndex:
      break;
  }
}

void LatexGenerator::endIndexSection(IndexSections is)
{
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      {
        //QCString indexName=Config_getBool(GENERATE_TREEVIEW)?"main":"index";
        QCString indexName="index";
        t << "}\n\\label{index}";
        if (Config_getBool(PDF_HYPERLINKS)) t << "\\hypertarget{index}{}";
        t << "\\input{" << indexName << "}\n";
      }
      break;
    case isModuleIndex:
      t << "}\n\\input{modules}\n";
      break;
    case isDirIndex:
      t << "}\n\\input{dirs}\n";
      break;
    case isNamespaceIndex:
      t << "}\n\\input{namespaces}\n";
      break;
    case isClassHierarchyIndex:
      t << "}\n\\input{hierarchy}\n";
      break;
    case isCompoundIndex:
      t << "}\n\\input{annotated}\n";
      break;
    case isFileIndex:
      t << "}\n\\input{files}\n";
      break;
    case isPageIndex:
      t << "}\n\\input{pages}\n";
      break;
    case isModuleDocumentation:
      {
        bool found=FALSE;
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            if (!found)
            {
              t << "}\n";
              found=TRUE;
            }
            t << "\\input{" << gd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isDirDocumentation:
      {
        bool found=FALSE;
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            if (!found)
            {
              t << "}\n";
              found = TRUE;
            }
            t << "\\input{" << dd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        bool found=FALSE;
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            if (!found)
            {
              t << "}\n";
              found=true;
            }
            t << "\\input{" << nd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isClassDocumentation:
      {
        bool found=FALSE;
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope() &&
             !cd->isAlias()
             )
          {
            if (!found)
            {
              t << "}\n"; // end doxysection or chapter title
              found=TRUE;
            }
            t << "\\input{" << cd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                t << "}\n"; // end doxysection or chapter title
              }
              isFirst=FALSE;
              t << "\\input{" << fd->getOutputFileBase() << "}\n";
              if (sourceBrowser && m_prettyCode && fd->generateSourceFile())
              {
                //t << "\\include{" << fd->getSourceFileBase() << "}\n";
                t << "\\input{" << fd->getSourceFileBase() << "}\n";
              }
            }
          }
        }
      }
      break;
    case isExampleDocumentation:
      {
        t << "}\n";
        for (const auto &pd : *Doxygen::exampleLinkedMap)
        {
          t << "\\input{" << pd->getOutputFileBase() << "}\n";
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "}\n";
#if 0
        bool first=TRUE;
        for (const auto *pd : Doxygen::pageLinkedMap)
        {
          if (!pd->getGroupDef() && !pd->isReference())
          {
             if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
             t << "{" << pd->title();
             t << "}\n";

            if (compactLatex || first) t << "\\input" ; else t << "\\include";
            t << "{" << pd->getOutputFileBase() << "}\n";
            first=FALSE;
          }
        }
#endif
      }
      break;
    case isPageDocumentation2:
      break;
    case isEndIndex:
      t << substituteLatexKeywords(g_footer,convertToLaTeX(Config_getString(PROJECT_NAME)));
      break;
  }
}

void LatexGenerator::writePageLink(const char *name, bool /*first*/)
{
  //bool &compactLatex = Config_getBool(COMPACT_LATEX);
  // next is remove for bug615957
  //if (compactLatex || first) t << "\\input" ; else t << "\\include";
  t << "\\input" ;
  t << "{" << name << "}\n";
}


void LatexGenerator::writeStyleInfo(int part)
{
  if (part > 0)
    return;

  startPlainFile("doxygen.sty");
  writeDefaultStyleSheet(t);
  endPlainFile();

  // workaround for the problem caused by change in LaTeX in version 2019
  // in the unmaintained tabu package
  startPlainFile("tabu_doxygen.sty");
  t << ResourceMgr::instance().getAsString("tabu_doxygen.sty");
  endPlainFile();
  startPlainFile("longtable_doxygen.sty");
  t << ResourceMgr::instance().getAsString("longtable_doxygen.sty");
  endPlainFile();
}

void LatexGenerator::newParagraph()
{
  t << "\n" << "\n";
}

void LatexGenerator::startParagraph(const char *)
{
  t << "\n" << "\n";
}

void LatexGenerator::endParagraph()
{
  t << "\n" << "\n";
}

void LatexGenerator::writeString(const char *text)
{
  t << text;
}

void LatexGenerator::startIndexItem(const char *ref,const char *fn)
{
  t << "\\item ";
  if (!ref && fn)
  {
    t << "\\contentsline{section}{";
  }
}

void LatexGenerator::endIndexItem(const char *ref,const char *fn)
{
  if (!ref && fn)
  {
    t << "}{\\pageref{" << stripPath(fn) << "}}{}\n";
  }
}

//void LatexGenerator::writeIndexFileItem(const char *,const char *text)
//{
//  t << "\\item\\contentsline{section}{";
//  docify(text);
//  t << "}{\\pageref{" << stripPath(text) << "}}\n";
//}


void LatexGenerator::startHtmlLink(const char *url)
{
  if (Config_getBool(PDF_HYPERLINKS))
  {
    t << "\\href{";
    t << latexFilterURL(url);
    t << "}";
  }
  t << "{\\texttt{ ";
}

void LatexGenerator::endHtmlLink()
{
  t << "}}";
}

//void LatexGenerator::writeMailLink(const char *url)
//{
//  if (Config_getBool(PDF_HYPERLINKS))
//  {
//    t << "\\href{mailto:";
//    t << url;
//    t << "}";
//  }
//  t << "\\texttt{ ";
//  docify(url);
//  t << "}";
//}

void LatexGenerator::writeStartAnnoItem(const char *,const char *,
                                        const char *path,const char *name)
{
  t << "\\item\\contentsline{section}\\textbf{ ";
  if (path) docify(path);
  docify(name);
  t << "} ";
}

void LatexGenerator::writeEndAnnoItem(const char *name)
{
  t << "}{\\pageref{" << stripPath(name) << "}}{}\n";
}

void LatexGenerator::startIndexKey()
{
  t << "\\item\\contentsline{section}{";
}

void LatexGenerator::endIndexKey()
{
}

void LatexGenerator::startIndexValue(bool hasBrief)
{
  t << " ";
  if (hasBrief) t << "\\\\*";
}

void LatexGenerator::endIndexValue(const char *name,bool /*hasBrief*/)
{
  //if (hasBrief) t << ")";
  t << "}{\\pageref{" << stripPath(name) << "}}{}\n";
}

//void LatexGenerator::writeClassLink(const char *,const char *,
//                                    const char *,const char *name)
//{
//  t << "\\textbf{ ";
//  docify(name);
//  t << "}";
//}

void LatexGenerator::startTextLink(const char *f,const char *anchor)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_disableLinks && pdfHyperlinks)
  {
    t << "\\mbox{\\hyperlink{";
    if (f) t << stripPath(f);
    if (anchor) t << "_" << anchor;
    t << "}{";
  }
  else
  {
    t << "\\textbf{ ";
  }
}

void LatexGenerator::endTextLink()
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_disableLinks && pdfHyperlinks)
  {
    t << "}";
  }
  t << "}";
}

void LatexGenerator::writeObjectLink(const char *ref, const char *f,
                                     const char *anchor, const char *text)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_disableLinks && !ref && pdfHyperlinks)
  {
    t << "\\mbox{\\hyperlink{";
    if (f) t << stripPath(f);
    if (f && anchor) t << "_";
    if (anchor) t << anchor;
    t << "}{";
    docify(text);
    t << "}}";
  }
  else
  {
    t << "\\textbf{ ";
    docify(text);
    t << "}";
  }
}

void LatexGenerator::startPageRef()
{
  t << " \\doxyref{}{";
}

void LatexGenerator::endPageRef(const char *clname, const char *anchor)
{
  t << "}{";
  if (clname) t << clname;
  if (anchor) t << "_" << anchor;
  t << "}";
}


void LatexGenerator::startTitleHead(const char *fileName)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks && fileName)
  {
    t << "\\hypertarget{" << stripPath(fileName) << "}{}";
  }
  if (Config_getBool(COMPACT_LATEX))
  {
    t << "\\doxysubsection{";
  }
  else
  {
    t << "\\doxysection{";
  }
}

void LatexGenerator::endTitleHead(const char *fileName,const char *name)
{
  t << "}\n";
  if (name)
  {
    t << "\\label{" << stripPath(fileName) << "}\\index{";
    t << latexEscapeLabelName(name);
    t << "@{";
    t << latexEscapeIndexChars(name);
    t << "}}\n";
  }
}

void LatexGenerator::startTitle()
{
  if (Config_getBool(COMPACT_LATEX))
  {
    t << "\\doxysubsection{";
  }
  else
  {
    t << "\\doxysection{";
  }
}

void LatexGenerator::startGroupHeader(int extraIndentLevel)
{
  if (Config_getBool(COMPACT_LATEX))
  {
    extraIndentLevel++;
  }

  if (extraIndentLevel==3)
  {
    t << "\\doxysubparagraph*{";
  }
  else if (extraIndentLevel==2)
  {
    t << "\\doxyparagraph{";
  }
  else if (extraIndentLevel==1)
  {
    t << "\\doxysubsubsection{";
  }
  else // extraIndentLevel==0
  {
    t << "\\doxysubsection{";
  }
  m_disableLinks=TRUE;
}

void LatexGenerator::endGroupHeader(int)
{
  m_disableLinks=FALSE;
  t << "}\n";
}

void LatexGenerator::startMemberHeader(const char *,int)
{
  if (Config_getBool(COMPACT_LATEX))
  {
    t << "\\doxysubsubsection*{";
  }
  else
  {
    t << "\\doxysubsection*{";
  }
  m_disableLinks=TRUE;
}

void LatexGenerator::endMemberHeader()
{
  m_disableLinks=FALSE;
  t << "}\n";
}

void LatexGenerator::startMemberDoc(const char *clname,
                                    const char *memname,
                                    const char *,
                                    const char *title,
                                    int memCount,
                                    int memTotal,
                                    bool showInline)
{
  if (memname && memname[0]!='@')
  {
    t << "\\index{";
    if (clname)
    {
      t << latexEscapeLabelName(clname);
      t << "@{";
      t << latexEscapeIndexChars(clname);
      t << "}!";
    }
    t << latexEscapeLabelName(memname);
    t << "@{";
    t << latexEscapeIndexChars(memname);
    t << "}}\n";

    t << "\\index{";
    t << latexEscapeLabelName(memname);
    t << "@{";
    t << latexEscapeIndexChars(memname);
    t << "}";
    if (clname)
    {
      t << "!";
      t << latexEscapeLabelName(clname);
      t << "@{";
      t << latexEscapeIndexChars(clname);
      t << "}";
    }
    t << "}\n";
  }
  static const char *levelLab[] = { "doxysubsubsection","doxyparagraph","doxysubparagraph", "doxysubparagraph" };
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  int level=0;
  if (showInline) level+=2;
  if (compactLatex) level++;
  t << "\\" << levelLab[level];

  t << "{";
  if (pdfHyperlinks)
  {
    t << "\\texorpdfstring{";
  }
  t << latexEscapeIndexChars(title);
  if (pdfHyperlinks)
  {
    t << "}{" << latexEscapePDFString(title) << "}";
  }
  if (memTotal>1)
  {
    t << "\\hspace{0.1cm}{\\footnotesize\\ttfamily [" << memCount << "/" << memTotal << "]}";
  }
  t << "}";
  t << "\n{\\footnotesize\\ttfamily ";
  //m_disableLinks=TRUE;
}

void LatexGenerator::endMemberDoc(bool)
{
  m_disableLinks=FALSE;
  t << "}\n\n";
  //if (Config_getBool(COMPACT_LATEX)) t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const char *fName,const char *,
                                     const char *anchor, const char *,
                                     const char *)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  t << "\\mbox{";
  if (usePDFLatex && pdfHyperlinks)
  {
    t << "\\Hypertarget{";
    if (fName) t << stripPath(fName);
    if (anchor) t << "_" << anchor;
    t << "}";
  }
  t << "\\label{";
  if (fName) t << stripPath(fName);
  if (anchor) t << "_" << anchor;
  t << "}} \n";
}

void LatexGenerator::endDoxyAnchor(const char *fName,const char *anchor)
{
}

void LatexGenerator::writeAnchor(const char *fName,const char *name)
{
  //printf("LatexGenerator::writeAnchor(%s,%s)\n",fName,name);
  t << "\\label{" << stripPath(name) << "}\n";
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks)
  {
    if (fName)
    {
      t << "\\Hypertarget{" << stripPath(fName) << "_" << stripPath(name) << "}\n";
    }
    else
    {
      t << "\\Hypertarget{" << stripPath(name) << "}\n";
    }
  }
}


//void LatexGenerator::writeLatexLabel(const char *clName,const char *anchor)
//{
//  writeDoxyAnchor(0,clName,anchor,0);
//}

void LatexGenerator::addIndexItem(const char *s1,const char *s2)
{
  if (s1)
  {
    t << "\\index{";
    t << latexEscapeLabelName(s1);
    t << "@{";
    t << latexEscapeIndexChars(s1);
    t << "}";
    if (s2)
    {
      t << "!";
      t << latexEscapeLabelName(s2);
      t << "@{";
      t << latexEscapeIndexChars(s2);
      t << "}";
    }
    t << "}";
  }
}


void LatexGenerator::startSection(const char *lab,const char *,SectionType type)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks)
  {
    t << "\\hypertarget{" << stripPath(lab) << "}{}";
  }
  t << "\\";
  if (Config_getBool(COMPACT_LATEX))
  {
    switch(type)
    {
      case SectionType::Page:          t << "doxysubsection"; break;
      case SectionType::Section:       t << "doxysubsubsection"; break;
      case SectionType::Subsection:    t << "doxyparagraph"; break;
      case SectionType::Subsubsection: t << "doxysubparagraph"; break;
      case SectionType::Paragraph:     t << "doxysubparagraph"; break;
      default: ASSERT(0); break;
    }
    t << "{";
  }
  else
  {
    switch(type)
    {
      case SectionType::Page:          t << "doxysection"; break;
      case SectionType::Section:       t << "doxysubsection"; break;
      case SectionType::Subsection:    t << "doxysubsubsection"; break;
      case SectionType::Subsubsection: t << "doxyparagraph"; break;
      case SectionType::Paragraph:     t << "doxysubparagraph"; break;
      default: ASSERT(0); break;
    }
    t << "{";
  }
}

void LatexGenerator::endSection(const char *lab,SectionType)
{
  t << "}\\label{" << lab << "}\n";
}


void LatexGenerator::docify(const char *str)
{
  filterLatexString(t,str,
                    m_insideTabbing, // insideTabbing
                    false,           // insidePre
                    false,           // insideItem
                    m_codeGen.usedTableLevel()>0,  // insideTable
                    false            // keepSpaces
                   );
}

void LatexGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

void LatexGenerator::startClassDiagram()
{
  //if (Config_getBool(COMPACT_LATEX)) t << "\\doxysubsubsection"; else t << "\\doxysubsection";
  //t << "{";
}

void LatexGenerator::endClassDiagram(const ClassDiagram &d,
                                       const char *fileName,const char *)
{
  d.writeFigure(t,dir(),fileName);
}


void LatexGenerator::startAnonTypeScope(int indent)
{
  if (indent==0)
  {
    t << "\\begin{tabbing}\n";
    t << "xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=\\kill\n";
    m_insideTabbing=TRUE;
  }
  m_indent=indent;
}

void LatexGenerator::endAnonTypeScope(int indent)
{
  if (indent==0)
  {
    t << "\n" << "\\end{tabbing}";
    m_insideTabbing=FALSE;
  }
  m_indent=indent;
}

void LatexGenerator::startMemberTemplateParams()
{
  if (templateMemberItem)
  {
    t << "{\\footnotesize ";
  }
}

void LatexGenerator::endMemberTemplateParams(const char *,const char *)
{
  if (templateMemberItem)
  {
    t << "}\\\\";
  }
}

void LatexGenerator::startMemberItem(const char *,int annoType,const char *)
{
  //printf("LatexGenerator::startMemberItem(%d)\n",annType);
  if (!m_insideTabbing)
  {
    t << "\\item \n";
    templateMemberItem = (annoType == 3);
  }
}

void LatexGenerator::endMemberItem()
{
  if (m_insideTabbing)
  {
    t << "\\\\";
  }
  templateMemberItem = FALSE;
  t << "\n";
}

void LatexGenerator::startMemberDescription(const char *,const char *,bool)
{
  if (!m_insideTabbing)
  {
    t << "\\begin{DoxyCompactList}\\small\\item\\em ";
  }
  else
  {
    for (int i=0;i<m_indent+2;i++) t << "\\>";
    t << "{\\em ";
  }
}

void LatexGenerator::endMemberDescription()
{
  if (!m_insideTabbing)
  {
    //t << "\\item\\end{DoxyCompactList}";
    t << "\\end{DoxyCompactList}";
  }
  else
  {
    t << "}\\\\\n";
  }
}


void LatexGenerator::writeNonBreakableSpace(int)
{
  //printf("writeNonBreakableSpace()\n");
  if (m_insideTabbing)
  {
    t << "\\>";
  }
  else
  {
    t << "~";
  }
}

// ----------------------------------------------
// nesting of functions below:
// startDescTable()
// - startDescTableRow()
//   - startDescTableTitle()
//   - endDescTableTitle()
//   - startDescTableData()
//   - endDescTableData()
// - endDescTableRow()
// - startDescTableRow()
//   - ...
// - endDescTableRow()
// endDescTable()

void LatexGenerator::startDescTable(const char *title)
{
  m_codeGen.incUsedTableLevel();
  t << "\\begin{DoxyEnumFields}{" << title << "}\n";
}

void LatexGenerator::endDescTable()
{
  m_codeGen.decUsedTableLevel();
  t << "\\end{DoxyEnumFields}\n";
}

void LatexGenerator::startDescTableRow()
{
  // this is needed to prevent the \hypertarget, \label, and \index commands from messing up
  // the row height (based on http://tex.stackexchange.com/a/186102)
  t << "\\raisebox{\\heightof{T}}[0pt][0pt]{";
}

void LatexGenerator::endDescTableRow()
{
}

void LatexGenerator::startDescTableTitle()
{
  t << "}";
}

void LatexGenerator::endDescTableTitle()
{
}

void LatexGenerator::startDescTableData()
{
  t << "&";
}

void LatexGenerator::endDescTableData()
{
  t << "\\\\\n\\hline\n\n";
}

void LatexGenerator::lastIndexPage()
{
}


void LatexGenerator::startMemberList()
{
  if (!m_insideTabbing)
  {
    t << "\\begin{DoxyCompactItemize}\n";
  }
}

void LatexGenerator::endMemberList()
{
  //printf("LatexGenerator::endMemberList(%d)\n",m_insideTabbing);
  if (!m_insideTabbing)
  {
    t << "\\end{DoxyCompactItemize}\n";
  }
}


void LatexGenerator::startMemberGroupHeader(bool hasHeader)
{
  if (hasHeader) t << "\\begin{Indent}";
  t << "\\textbf{ ";
  // changed back to rev 756 due to bug 660501
  //if (Config_getBool(COMPACT_LATEX))
  //{
  //  t << "\\doxysubparagraph*{";
  //}
  //else
  //{
  //  t << "\\doxyparagraph*{";
  //}
}

void LatexGenerator::endMemberGroupHeader()
{
  // changed back to rev 756 due to bug 660501
  t << "}\\par\n";
  //t << "}\n";
}

void LatexGenerator::startMemberGroupDocs()
{
  t << "{\\em ";
}

void LatexGenerator::endMemberGroupDocs()
{
  t << "}";
}

void LatexGenerator::startMemberGroup()
{
}

void LatexGenerator::endMemberGroup(bool hasHeader)
{
  if (hasHeader)t << "\\end{Indent}";
  t << "\n";
}

void LatexGenerator::startDotGraph()
{
  newParagraph();
}

void LatexGenerator::endDotGraph(DotClassGraph &g)
{
  g.writeGraph(t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startInclDepGraph()
{
}

void LatexGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  g.writeGraph(t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startGroupCollaboration()
{
}

void LatexGenerator::endGroupCollaboration(DotGroupCollaboration &g)
{
  g.writeGraph(t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startCallGraph()
{
}

void LatexGenerator::endCallGraph(DotCallGraph &g)
{
  g.writeGraph(t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startDirDepGraph()
{
}

void LatexGenerator::endDirDepGraph(DotDirDeps &g)
{
  g.writeGraph(t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startDescription()
{
  t << "\\begin{description}\n";
}

void LatexGenerator::endDescription()
{
  t << "\\end{description}\n";
  m_firstDescItem=TRUE;
}

void LatexGenerator::startDescItem()
{
  m_firstDescItem=TRUE;
  t << "\\item[";
}

void LatexGenerator::endDescItem()
{
  if (m_firstDescItem)
  {
    t << "]\n";
    m_firstDescItem=FALSE;
  }
  else
  {
    lineBreak();
  }
}

void LatexGenerator::startExamples()
{
  t << "\\begin{Desc}\n\\item[";
  docify(theTranslator->trExamples());
  t << "]";
}

void LatexGenerator::endExamples()
{
  t << "\\end{Desc}\n";
}

void LatexGenerator::startParamList(ParamListTypes,const char *title)
{
  t << "\\begin{Desc}\n\\item[";
  docify(title);
  t << "]";
}

void LatexGenerator::endParamList()
{
  t << "\\end{Desc}\n";
}

void LatexGenerator::startParameterList(bool openBracket)
{
  /* start of ParameterType ParameterName list */
  if (openBracket) t << "(";
  t << "\\begin{DoxyParamCaption}";
}

void LatexGenerator::endParameterList()
{
}

void LatexGenerator::startParameterType(bool first,const char *key)
{
  t << "\\item[{";
  if (!first && key) docify(key);
}

void LatexGenerator::endParameterType()
{
  t << "}]";
}

void LatexGenerator::startParameterName(bool /*oneArgOnly*/)
{
  t << "{";
}

void LatexGenerator::endParameterName(bool last,bool /*emptyList*/,bool closeBracket)
{
  t << " }";
  if (last)
  {
    t << "\\end{DoxyParamCaption}";
    if (closeBracket) t << ")";
  }
}

void LatexGenerator::exceptionEntry(const char* prefix,bool closeBracket)
{
  if (prefix)
      t << " " << prefix << "(";
  else if (closeBracket)
      t << ")";
  t << " ";
}

void LatexGenerator::writeDoc(DocNode *n,const Definition *ctx,const MemberDef *,int)
{
  LatexDocVisitor *visitor =
    new LatexDocVisitor(t,m_codeGen,ctx?ctx->getDefFileExtension():QCString(""),m_insideTabbing);
  n->accept(visitor);
  delete visitor;
}

void LatexGenerator::startConstraintList(const char *header)
{
  t << "\\begin{Desc}\n\\item[";
  docify(header);
  t << "]";
  t << "\\begin{description}\n";
}

void LatexGenerator::startConstraintParam()
{
  t << "\\item[{\\em ";
}

void LatexGenerator::endConstraintParam()
{
}

void LatexGenerator::startConstraintType()
{
  t << "} : {\\em ";
}

void LatexGenerator::endConstraintType()
{
  t << "}]";
}

void LatexGenerator::startConstraintDocs()
{
}

void LatexGenerator::endConstraintDocs()
{
}

void LatexGenerator::endConstraintList()
{
  t << "\\end{description}\n";
  t << "\\end{Desc}\n";
}

void LatexGenerator::startInlineHeader()
{
  if (Config_getBool(COMPACT_LATEX))
  {
    t << "\\doxyparagraph*{";
  }
  else
  {
    t << "\\doxysubsubsection*{";
  }
}

void LatexGenerator::endInlineHeader()
{
  t << "}\n";
}

void LatexGenerator::lineBreak(const char *)
{
  if (m_insideTabbing)
  {
    t << "\\\\\n";
  }
  else
  {
    t << "\\newline\n";
  }
}

void LatexGenerator::startMemberDocSimple(bool isEnum)
{
  m_codeGen.incUsedTableLevel();
  if (isEnum)
  {
    t << "\\begin{DoxyEnumFields}{";
    docify(theTranslator->trEnumerationValues());
  }
  else
  {
    t << "\\begin{DoxyFields}{";
    docify(theTranslator->trCompoundMembers());
  }
  t << "}\n";
}

void LatexGenerator::endMemberDocSimple(bool isEnum)
{
  m_codeGen.decUsedTableLevel();
  if (isEnum)
  {
    t << "\\end{DoxyEnumFields}\n";
  }
  else
  {
    t << "\\end{DoxyFields}\n";
  }
}

void LatexGenerator::startInlineMemberType()
{
  m_insideTabbing = TRUE; // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberType()
{
  t << "&\n";
  m_insideTabbing = FALSE;
}

void LatexGenerator::startInlineMemberName()
{
  m_insideTabbing = TRUE; // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberName()
{
  t << "&\n";
  m_insideTabbing = FALSE;
}

void LatexGenerator::startInlineMemberDoc()
{
}

void LatexGenerator::endInlineMemberDoc()
{
  t << "\\\\\n\\hline\n\n";
}

void LatexGenerator::startLabels()
{
  t << "\\hspace{0.3cm}";
}

void LatexGenerator::writeLabel(const char *l,bool isLast)
{
  t << "{\\ttfamily [" << l << "]}";
  if (!isLast) t << ", ";
}

void LatexGenerator::endLabels()
{
}
