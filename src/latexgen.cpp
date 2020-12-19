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

#include <stdlib.h>

#include <qdir.h>
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

LatexCodeGenerator::LatexCodeGenerator(FTextStream &t,const QCString &relPath,const QCString &sourceFileName)
  : m_relPath(relPath), m_sourceFileName(sourceFileName)
{
  m_prettyCode=Config_getBool(LATEX_SOURCE_CODE);
  setTextStream(t);
}

LatexCodeGenerator::LatexCodeGenerator()
{
  m_prettyCode=Config_getBool(LATEX_SOURCE_CODE);
}

void LatexCodeGenerator::setTextStream(FTextStream &t)
{
  m_streamSet = t.device()!=0;
  m_t.setDevice(t.device());
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
                     if (lresult < (i + 5))                       \
                     {                                            \
                       lresult += 512;                            \
                       result = (signed char *)realloc(result, lresult); \
                     }                                            \
                     result[i++]=c; p++;                          \
                     if (c<0) /* multibyte utf-8 character */     \
                     {                                            \
                       /* 1xxx.xxxx: >=2 byte character */        \
                       result[i++]=*p++;                          \
                       if (((uchar)c&0xE0)==0xE0)                 \
                       {                                          \
                         /* 111x.xxxx: >=3 byte character */      \
                         result[i++]=*p++;                        \
                       }                                          \
                       if (((uchar)c&0xF0)==0xF0)                 \
                       {                                          \
                         /* 1111.xxxx: 4 byte character */        \
                         result[i++]=*p++;                        \
                       }                                          \
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
  QFile file(fileName);
  if (!file.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",fileName.data());
  }
  // inserted by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  QCString latex_command = theTranslator->latexCommandName();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME);
  // end insertion by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  FTextStream t(&file);
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "LATEX_CMD=" << latex_command << endl
      << endl
      << "all: refman.dvi" << endl
      << endl
      << "ps: refman.ps" << endl
      << endl
      << "pdf: refman.pdf" << endl
      << endl
      << "ps_2on1: refman_2on1.ps" << endl
      << endl
      << "pdf_2on1: refman_2on1.pdf" << endl
      << endl
      << "refman.ps: refman.dvi" << endl
      << "\tdvips -o refman.ps refman.dvi" << endl
      << endl;
    t << "refman.pdf: refman.ps" << endl;
    t << "\tps2pdf refman.ps refman.pdf" << endl << endl;
    t << "refman.dvi: clean refman.tex doxygen.sty" << endl
      << "\techo \"Running latex...\"" << endl
      << "\t$(LATEX_CMD) refman.tex" << endl
      << "\techo \"Running makeindex...\"" << endl
      << "\t" << mkidx_command << " refman.idx" << endl;
    if (generateBib)
    {
      t << "\techo \"Running bibtex...\"" << endl;
      t << "\tbibtex refman" << endl;
      t << "\techo \"Rerunning latex....\"" << endl;
      t << "\t$(LATEX_CMD) refman.tex" << endl;
    }
    t << "\techo \"Rerunning latex....\"" << endl
      << "\t$(LATEX_CMD) refman.tex" << endl
      << "\tlatex_count=8 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      $(LATEX_CMD) refman.tex ; \\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl
      << "\t" << mkidx_command << " refman.idx" << endl
      << "\t$(LATEX_CMD) refman.tex" << endl << endl
      << "refman_2on1.ps: refman.ps" << endl
      << "\tpsnup -2 refman.ps >refman_2on1.ps" << endl
      << endl
      << "refman_2on1.pdf: refman_2on1.ps" << endl
      << "\tps2pdf refman_2on1.ps refman_2on1.pdf" << endl;
  }
  else // use pdflatex for higher quality output
  {
    t << "LATEX_CMD=" << latex_command << endl
      << endl;
    t << "all: refman.pdf" << endl << endl
      << "pdf: refman.pdf" << endl << endl;
    t << "refman.pdf: clean refman.tex" << endl;
    t << "\t$(LATEX_CMD) refman" << endl;
    t << "\t" << mkidx_command << " refman.idx" << endl;
    if (generateBib)
    {
      t << "\tbibtex refman" << endl;
      t << "\t$(LATEX_CMD) refman" << endl;
    }
    t << "\t$(LATEX_CMD) refman" << endl
      << "\tlatex_count=8 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      $(LATEX_CMD) refman ;\\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl
      << "\t" << mkidx_command << " refman.idx" << endl
      << "\t$(LATEX_CMD) refman" << endl << endl;
  }

  t << endl
    << "clean:" << endl
    << "\trm -f "
    << "*.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf" << endl;
}

static void writeMakeBat()
{
#if defined(_MSC_VER)
  QCString dir=Config_getString(LATEX_OUTPUT);
  QCString fileName=dir+"/make.bat";
  QCString latex_command = theTranslator->latexCommandName();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME);
  QFile file(fileName);
  bool generateBib = !CitationManager::instance().isEmpty();
  if (!file.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",fileName.data());
  }
  FTextStream t(&file);
  t << "set Dir_Old=%cd%\n";
  t << "cd /D %~dp0\n\n";
  t << "del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf\n\n";
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "set LATEX_CMD=" << latex_command << "\n";
    t << "%LATEX_CMD% refman.tex\n";
    t << "echo ----\n";
    t << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "bibtex refman\n";
      t << "echo ----\n";
      t << "\t%LATEX_CMD% refman.tex\n";
    }
    t << "setlocal enabledelayedexpansion\n";
    t << "set count=8\n";
    t << ":repeat\n";
    t << "set content=X\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X goto :skip\n";
    t << "set /a count-=1\n";
    t << "if !count! EQU 0 goto :skip\n\n";
    t << "echo ----\n";
    t << "%LATEX_CMD% refman.tex\n";
    t << "goto :repeat\n";
    t << ":skip\n";
    t << "endlocal\n";
    t << mkidx_command << " refman.idx\n";
    t << "%LATEX_CMD% refman.tex\n";
    t << "dvips -o refman.ps refman.dvi\n";
    t << Portable::ghostScriptCommand();
    t << " -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
         "-sOutputFile=refman.pdf -c save pop -f refman.ps\n";
  }
  else // use pdflatex
  {
    t << "set LATEX_CMD=" << latex_command << "\n";
    t << "%LATEX_CMD% refman\n";
    t << "echo ----\n";
    t << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "bibtex refman" << endl;
      t << "%LATEX_CMD% refman" << endl;
    }
    t << "echo ----\n";
    t << "%LATEX_CMD% refman\n\n";
    t << "setlocal enabledelayedexpansion\n";
    t << "set count=8\n";
    t << ":repeat\n";
    t << "set content=X\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X goto :skip\n";
    t << "set /a count-=1\n";
    t << "if !count! EQU 0 goto :skip\n\n";
    t << "echo ----\n";
    t << "%LATEX_CMD% refman\n";
    t << "goto :repeat\n";
    t << ":skip\n";
    t << "endlocal\n";
    t << mkidx_command << " refman.idx\n";
    t << "%LATEX_CMD% refman\n";
    t << "cd /D %Dir_Old%\n";
    t << "set Dir_Old=\n";
  }
#endif
}

void LatexGenerator::init()
{
  QCString dname = Config_getString(LATEX_OUTPUT);
  QDir d(dname);
  if (!d.exists() && !d.mkdir(dname))
  {
    term("Could not create output directory %s\n",dname.data());
  }

  writeLatexMakefile();
  writeMakeBat();

  createSubDirs(d);
}

static void writeDefaultHeaderPart1(FTextStream &t)
{
  // part 1

  // Handle batch mode
  if (Config_getBool(LATEX_BATCHMODE))
    t << "\\batchmode\n";

  // to overcome problems with too many open files
  t << "\\let\\mypdfximage\\pdfximage"
       "\\def\\pdfximage{\\immediate\\mypdfximage}";

  // Set document class depending on configuration
  QCString documentClass;
  if (Config_getBool(COMPACT_LATEX))
    documentClass = "article";
  else
    documentClass = "book";
  t << "\\documentclass[twoside]{" << documentClass << "}\n"
       "\n";
  t << "%% moved from doxygen.sty due to workaround for LaTex 2019 version and unmaintained tabu package\n"
       "\\usepackage{ifthen}\n"
       "\\ifx\\requestedLaTeXdate\\undefined\n"
       "\\usepackage{array}\n"
       "\\else\n"
       "\\usepackage{array}[=2016-10-06]\n"
       "\\fi\n"
       "%%\n";

  // Load required packages
  t << "% Packages required by doxygen\n"
       "\\usepackage{fixltx2e}\n" // for \textsubscript
       "\\usepackage{doxygen}\n";
  const StringVector &extraLatexStyles = Config_getList(LATEX_EXTRA_STYLESHEET);
  for (const auto &extraStyle : extraLatexStyles)
  {
    QCString fileName = extraStyle.c_str();
    if (!fileName.isEmpty())
    {
      QFileInfo fi(fileName);
      if (fi.exists())
      {
        if (checkExtension(fi.fileName().data(), LATEX_STYLE_EXTENSION))
        {
          // strip the extension, it will be added by the usepackage in the tex conversion process
          t << "\\usepackage{" << stripExtensionGeneral(fi.fileName().data(), LATEX_STYLE_EXTENSION) << "}\n";
        }
        else
        {
          t << "\\usepackage{" << fi.fileName().utf8() << "}\n";
        }
      }
    }
  }
  t << "\\usepackage{graphicx}\n"
       "\\usepackage[utf8]{inputenc}\n"
       "\\usepackage{makeidx}\n"
       "\\PassOptionsToPackage{warn}{textcomp}\n"
       "\\usepackage{textcomp}\n"
       "\\usepackage[nointegrals]{wasysym}\n"
       "\\usepackage{ifxetex}\n"
       "\n";

  // Language support
  QCString languageSupport = theTranslator->latexLanguageSupportCommand();
  if (!languageSupport.isEmpty())
  {
    t << "% NLS support packages\n"
      << languageSupport
      << "\n";
  }

  // Define default fonts
  t << "% Font selection\n";
  QCString fontenc = theTranslator->latexFontenc();
  if (!fontenc.isEmpty())
  {
    t << "\\usepackage[" << fontenc << "]{fontenc}\n";
  }
  QCString font = theTranslator->latexFont();
  if (!font.isEmpty())
  {
    t << font;
  }
  t << "\\usepackage{sectsty}\n"
       "\\allsectionsfont{%\n"
       "  \\fontseries{bc}\\selectfont%\n"
       "  \\color{darkgray}%\n"
       "}\n"
       "\\renewcommand{\\DoxyLabelFont}{%\n"
       "  \\fontseries{bc}\\selectfont%\n"
       "  \\color{darkgray}%\n"
       "}\n"
       "\\newcommand{\\+}{\\discretionary{\\mbox{\\scriptsize$\\hookleftarrow$}}{}{}}\n"
       "\n";

   QCString emojiDir=Config_getString(LATEX_EMOJI_DIRECTORY);
   if (emojiDir.isEmpty()) emojiDir = ".";
   emojiDir = substitute(emojiDir,"\\","/");
   t << "% Arguments of doxygenemoji:\n"
        "% 1) ':<text>:' form of the emoji, already \"LaTeX\"-escaped\n"
        "% 2) file with the name of the emoji without the .png extension\n"
	"% in case image exist use this otherwise use the ':<text>:' form\n";
   t << "\\newcommand{\\doxygenemoji}[2]{%\n"
        "  \\IfFileExists{" << emojiDir << "/#2.png}{\\raisebox{-0.1em}{\\includegraphics[height=0.9em]{" << emojiDir << "/#2.png}}}{#1}%\n"
        "}\n";

  // Define page & text layout
  QCString paperName=Config_getEnum(PAPER_TYPE);
  // "a4wide" package is obsolete (see bug 563698)
  t << "% Page & text layout\n"
       "\\usepackage{geometry}\n"
       "\\geometry{%\n"
       "  " << paperName << "paper,%\n"
       "  top=2.5cm,%\n"
       "  bottom=2.5cm,%\n"
       "  left=2.5cm,%\n"
       "  right=2.5cm%\n"
       "}\n";
  // \sloppy is obsolete (see bug 563698)
  // Allow a bit of overflow to go unnoticed by other means
  t << "\\tolerance=750\n"
       "\\hfuzz=15pt\n"
       "\\hbadness=750\n"
       "\\setlength{\\emergencystretch}{15pt}\n"
       "\\setlength{\\parindent}{0cm}\n"
       "\\newcommand{\\doxynormalparskip}{\\setlength{\\parskip}{3ex plus 2ex minus 2ex}}\n"
       "\\newcommand{\\doxytocparskip}{\\setlength{\\parskip}{1ex plus 0ex minus 0ex}}\n"
       "\\doxynormalparskip\n";
  // Redefine paragraph/subparagraph environments, using sectsty fonts
  t << "\\makeatletter\n"
       "\\renewcommand{\\paragraph}{%\n"
       "  \\@startsection{paragraph}{4}{0ex}{-1.0ex}{1.0ex}{%\n"
       "    \\normalfont\\normalsize\\bfseries\\SS@parafont%\n"
       "  }%\n"
       "}\n"
       "\\renewcommand{\\subparagraph}{%\n"
       "  \\@startsection{subparagraph}{5}{0ex}{-1.0ex}{1.0ex}{%\n"
       "    \\normalfont\\normalsize\\bfseries\\SS@subparafont%\n"
       "  }%\n"
       "}\n"
       "\\makeatother\n"
       "\n";
  //
  t << "\\makeatletter\n"
       "\\newcommand\\hrulefilll{\\leavevmode\\leaders\\hrule\\hskip 0pt plus 1filll\\kern\\z@}\n"
       "\\makeatother\n"
       "\n";

  // Headers & footers
  QGString genString;
  QCString generatedBy;
  bool timeStamp = Config_getBool(LATEX_TIMESTAMP);
  FTextStream tg(&genString);
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
  t << "% Headers & footers\n"
       "\\usepackage{fancyhdr}\n"
       "\\pagestyle{fancyplain}\n"
       "\\renewcommand{\\footrulewidth}{0.4pt}\n"
       "%\n"
       "\\fancypagestyle{fancyplain}{\n"
       "\\fancyhf{}\n"
       "\\fancyhead[LE, RO]{\\bfseries\\thepage}\n"
       "\\fancyhead[LO]{\\bfseries\\rightmark}\n"
       "\\fancyhead[RE]{\\bfseries\\leftmark}\n"
       "\\fancyfoot[LO, RE]{\\bfseries\\scriptsize " << genString << " Doxygen }\n"
       "}\n"
       "%\n"
       "\\fancypagestyle{plain}{\n"
       "\\fancyhf{}\n"
       "\\fancyfoot[LO, RE]{\\bfseries\\scriptsize " << genString << " Doxygen }\n"
       "\\renewcommand{\\headrulewidth}{0pt}}\n"
       "%\n"
       "\\pagestyle{fancyplain}\n"
       "%\n";

  if (!Config_getBool(COMPACT_LATEX))
  {
    t << "\\renewcommand{\\chaptermark}[1]{%\n"
         "  \\markboth{#1}{}%\n"
         "}\n";
  }
  t << "\\renewcommand{\\sectionmark}[1]{%\n"
       "  \\markright{\\thesection\\ #1}%\n"
       "}\n"
       "\n";

  // ToC, LoF, LoT, bibliography, and index
  t << "% Indices & bibliography\n"
       "\\usepackage{natbib}\n"
       "\\usepackage[titles]{tocloft}\n"
       "\\setcounter{tocdepth}{3}\n"
       "\\setcounter{secnumdepth}{5}\n";

  QCString latex_mkidx_command = Config_getString(LATEX_MAKEINDEX_CMD);
  if (!latex_mkidx_command.isEmpty())
  {
    if (latex_mkidx_command[0] == '\\')
      t << latex_mkidx_command << "\n";
    else
      t << '\\' << latex_mkidx_command << "\n";
  }
  else
  {
    t << "\\makeindex\n";
  }
  t << "\n";

  writeExtraLatexPackages(t);
  writeLatexSpecialFormulaChars(t);
  QCString macroFile = Config_getString(FORMULA_MACROFILE);
  if (!macroFile.isEmpty())
  {
    QFileInfo fi(macroFile);
    macroFile=fi.absFilePath().utf8();
    QCString stripMacroFile = fi.fileName().data();
    copyFile(macroFile,Config_getString(LATEX_OUTPUT) + "/" + stripMacroFile);
    t << "\\input{" << stripMacroFile << "}" << endl;
  }

  // Hyperlinks
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (pdfHyperlinks)
  {
    t << "% Hyperlinks (required, but should be loaded last)\n"
         "\\ifpdf\n"
         "  \\usepackage[pdftex,pagebackref=true]{hyperref}\n"
         "\\else\n"
         "  \\ifxetex\n"
         "    \\usepackage[pagebackref=true]{hyperref}\n"
         "  \\else\n"
         "    \\usepackage[ps2pdf,pagebackref=true]{hyperref}\n"
         "  \\fi\n"
         "\\fi\n"
         "\n"
         "\\hypersetup{%\n"
         "  colorlinks=true,%\n"
         "  linkcolor=blue,%\n"
         "  citecolor=blue,%\n"
         "  unicode%\n"
         "}\n"
         "\n";
  }

  // Custom commands used by the header
  t << "% Custom commands\n"
       "\\newcommand{\\clearemptydoublepage}{%\n"
       "  \\newpage{\\pagestyle{empty}\\cleardoublepage}%\n"
       "}\n"
       "\n";

  // caption style definition
  t << "\\usepackage{caption}\n"
    << "\\captionsetup{labelsep=space,justification=centering,font={bf},singlelinecheck=off,skip=4pt,position=top}\n\n";


  // in page table of contents
  t << "\\usepackage{etoc}\n"
       "\\etocsettocstyle{\\doxytocparskip}{\\doxynormalparskip}\n";

  // prevent numbers overlap the titles in toc
  t << "\\renewcommand{\\numberline}[1]{#1~}\n";

  // End of preamble, now comes the document contents
  t << "%===== C O N T E N T S =====\n"
       "\n"
       "\\begin{document}\n"
       "\\raggedbottom\n";
  QCString documentPre = theTranslator->latexDocumentPre();
  if (!documentPre.isEmpty())
  {
    t << documentPre;
  }
  t << "\n";

  // Front matter
  t << "% Titlepage & ToC\n";
  bool usePDFLatex = Config_getBool(USE_PDFLATEX);
  if (pdfHyperlinks && usePDFLatex)
  {
    // To avoid duplicate page anchors due to reuse of same numbers for
    // the index (be it as roman numbers)
    t << "\\hypersetup{pageanchor=false,\n"
    //  << "             bookmarks=true,\n" // commented out to prevent warning
      << "             bookmarksnumbered=true,\n"
      << "             pdfencoding=unicode\n"
      << "            }\n";
  }
  t << "\\pagenumbering{alph}\n"
       "\\begin{titlepage}\n"
       "\\vspace*{7cm}\n"
       "\\begin{center}%\n"
       "{\\Large ";
}

static void writeDefaultHeaderPart2(FTextStream &t)
{
  // part 2
  // Finalize project name
  t << "}\\\\\n"
       "\\vspace*{1cm}\n"
       "{\\large ";
}

static void writeDefaultHeaderPart3(FTextStream &t)
{
  // part 3
  // Finalize project number
  t << " Doxygen " << getDoxygenVersion() << "}\\\\\n";
  if (Config_getBool(LATEX_TIMESTAMP))
    t << "\\vspace*{0.5cm}\n"
         "{\\small " << dateToString(TRUE) << "}\\\\\n";
  t << "\\end{center}\n"
       "\\end{titlepage}\n";
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  if (!compactLatex)
    t << "\\clearemptydoublepage\n";
  t << "\\pagenumbering{roman}\n";

  // ToC
  t << "\\tableofcontents\n";
  if (!compactLatex)
    t << "\\clearemptydoublepage\n";
  t << "\\pagenumbering{arabic}\n";
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (pdfHyperlinks && usePDFLatex)
  {
    // re-enable anchors again
    t << "\\hypersetup{pageanchor=true}\n";
  }
  t << "\n"
       "%--- Begin generated contents ---\n";
}

static void writeDefaultStyleSheet(FTextStream &t)
{
  t << ResourceMgr::instance().getAsString("doxygen.sty");
}

static void writeDefaultFooter(FTextStream &t)
{
  t << "%--- End generated contents ---\n"
       "\n";

  // Bibliography
  CitationManager::instance().writeLatexBibliography(t);

  // Index
  t << "% Index\n";
  QCString unit;
  if (Config_getBool(COMPACT_LATEX))
  {
    unit = "section";
  }
  else
  {
    unit = "chapter";
    t << "\\backmatter\n";
  }
  t << "\\newpage\n"
       "\\phantomsection\n"
       "\\clearemptydoublepage\n"
       "\\addcontentsline{toc}{" << unit << "}{\\indexname}\n"
       "\\printindex\n"
       "\n";
  QCString documentPost = theTranslator->latexDocumentPost();
  if (!documentPost.isEmpty())
  {
    t << documentPost;
  }
  t << "\\end{document}\n";
}

void LatexGenerator::writeHeaderFile(QFile &f)
{
  FTextStream t(&f);
  t << "% Latex header for doxygen " << getDoxygenVersion() << endl;
  writeDefaultHeaderPart1(t);
  t << "Your title here";
  writeDefaultHeaderPart2(t);
  t << "Generated by";
  writeDefaultHeaderPart3(t);
}

void LatexGenerator::writeFooterFile(QFile &f)
{
  FTextStream t(&f);
  t << "% Latex footer for doxygen " << getDoxygenVersion() << endl;
  writeDefaultFooter(t);
}

void LatexGenerator::writeStyleSheetFile(QFile &f)
{
  FTextStream t(&f);
  t << "% stylesheet for doxygen " << getDoxygenVersion() << endl;
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

void LatexGenerator::startIndexSection(IndexSections is)
{
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  QCString latexHeader = Config_getString(LATEX_HEADER);
  switch (is)
  {
    case isTitlePageStart:
      {
        if (latexHeader.isEmpty())
        {
          writeDefaultHeaderPart1(t);
        }
        else
        {
          QCString header = fileToString(latexHeader);
          t << substituteKeywords(header,"",
                   convertToLaTeX(Config_getString(PROJECT_NAME)),
                   convertToLaTeX(Config_getString(PROJECT_NUMBER)),
                   convertToLaTeX(Config_getString(PROJECT_BRIEF)));
        }
      }
      break;
    case isTitlePageAuthor:
      if (latexHeader.isEmpty())
      {
        writeDefaultHeaderPart2(t);
      }
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
      t << "{"; //Namespace Index}\"
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
        GroupSDict::Iterator gli(*Doxygen::groupSDict);
        GroupDef *gd;
        bool found=FALSE;
        for (gli.toFirst();(gd=gli.current()) && !found;++gli)
        {
          if (!gd->isReference())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            found=TRUE;
          }
        }
      }
      break;
    case isDirDocumentation:
      {
        SDict<DirDef>::Iterator dli(*Doxygen::directories);
        DirDef *dd;
        bool found=FALSE;
        for (dli.toFirst();(dd=dli.current()) && !found;++dli)
        {
          if (dd->isLinkableInProject())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            found=TRUE;
          }
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
        NamespaceDef *nd;
        bool found=FALSE;
        for (nli.toFirst();(nd=nli.current()) && !found;++nli)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            if (compactLatex) t << "\\doxysection"; else t << "\\chapter";
            t << "{"; // Namespace Documentation}\n":
            found=TRUE;
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
  //bool compactLatex = Config_getBool(COMPACT_LATEX);
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  QCString latexHeader = Config_getString(LATEX_HEADER);
  QCString latexFooter = Config_getString(LATEX_FOOTER);
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      if (latexHeader.isEmpty())
      {
        writeDefaultHeaderPart3(t);
      }
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
        GroupSDict::Iterator gli(*Doxygen::groupSDict);
        GroupDef *gd;
        bool found=FALSE;
        for (gli.toFirst();(gd=gli.current()) && !found;++gli)
        {
          if (!gd->isReference())
          {
            t << "}\n\\input{" << gd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        for (;(gd=gli.current());++gli)
        {
          if (!gd->isReference())
          {
            //if (compactLatex) t << "\\input"; else t << "\\include";
            t << "\\include";
            t << "{" << gd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isDirDocumentation:
      {
        SDict<DirDef>::Iterator dli(*Doxygen::directories);
        DirDef *dd;
        bool found=FALSE;
        for (dli.toFirst();(dd=dli.current()) && !found;++dli)
        {
          if (dd->isLinkableInProject())
          {
            t << "}\n\\input{" << dd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        for (;(dd=dli.current());++dli)
        {
          if (dd->isLinkableInProject())
          {
            //if (compactLatex) t << "\\input"; else t << "\\include";
            t << "\\input";
            t << "{" << dd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
        NamespaceDef *nd;
        bool found=FALSE;
        for (nli.toFirst();(nd=nli.current()) && !found;++nli)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            t << "}\n\\input{" << nd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        while ((nd=nli.current()))
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            //if (compactLatex) t << "\\input"; else t << "\\include";
            t << "\\input";
            t << "{" << nd->getOutputFileBase() << "}\n";
          }
          ++nli;
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
        PageSDict::Iterator pdi(*Doxygen::exampleSDict);
        PageDef *pd=pdi.toFirst();
        if (pd)
        {
          t << "\\input{" << pd->getOutputFileBase() << "}\n";
        }
        for (++pdi;(pd=pdi.current());++pdi)
        {
          //if (compactLatex) t << "\\input" ; else t << "\\include";
          t << "\\input";
          t << "{" << pd->getOutputFileBase() << "}\n";
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "}\n";
#if 0
        PageSDict::Iterator pdi(*Doxygen::pageSDict);
        PageDef *pd=pdi.toFirst();
        bool first=TRUE;
        for (pdi.toFirst();(pd=pdi.current());++pdi)
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
      if (latexFooter.isEmpty())
      {
        writeDefaultFooter(t);
      }
      else
      {
        QCString footer = fileToString(latexFooter);
        t << substituteKeywords(footer,"",
                   convertToLaTeX(Config_getString(PROJECT_NAME)),
                   convertToLaTeX(Config_getString(PROJECT_NUMBER)),
                   convertToLaTeX(Config_getString(PROJECT_BRIEF)));
      }
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
  t << endl << endl;
}

void LatexGenerator::startParagraph(const char *)
{
  t << endl << endl;
}

void LatexGenerator::endParagraph()
{
  t << endl << endl;
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
    t << "}{\\pageref{" << stripPath(fn) << "}}{}" << endl;
  }
}

//void LatexGenerator::writeIndexFileItem(const char *,const char *text)
//{
//  t << "\\item\\contentsline{section}{";
//  docify(text);
//  t << "}{\\pageref{" << stripPath(text) << "}}" << endl;
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
  t << "}{\\pageref{" << stripPath(name) << "}}{}" << endl;
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
  t << "}{\\pageref{" << stripPath(name) << "}}{}" << endl;
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
  t << "}" << endl;
  if (name)
  {
    t << "\\label{" << stripPath(fileName) << "}\\index{";
    t << latexEscapeLabelName(name);
    t << "@{";
    t << latexEscapeIndexChars(name);
    t << "}}" << endl;
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
  t << "}" << endl;
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
  t << "}" << endl;
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
    t << "}}" << endl;

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
    t << "}" << endl;
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
  t << "}} " << endl;
}

void LatexGenerator::endDoxyAnchor(const char *fName,const char *anchor)
{
}

void LatexGenerator::writeAnchor(const char *fName,const char *name)
{
  //printf("LatexGenerator::writeAnchor(%s,%s)\n",fName,name);
  t << "\\label{" << stripPath(name) << "}" << endl;
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks)
  {
    if (fName)
    {
      t << "\\Hypertarget{" << stripPath(fName) << "_" << stripPath(name) << "}" << endl;
    }
    else
    {
      t << "\\Hypertarget{" << stripPath(name) << "}" << endl;
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
  t << "}\\label{" << lab << "}" << endl;
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
    t << "\\begin{tabbing}" << endl;
    t << "xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=\\kill" << endl;
    m_insideTabbing=TRUE;
  }
  m_indent=indent;
}

void LatexGenerator::endAnonTypeScope(int indent)
{
  if (indent==0)
  {
    t << endl << "\\end{tabbing}";
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
    t << "\\item " << endl;
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
  t << endl;
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
  t << "\\begin{DoxyEnumFields}{" << title << "}" << endl;
}

void LatexGenerator::endDescTable()
{
  m_codeGen.decUsedTableLevel();
  t << "\\end{DoxyEnumFields}" << endl;
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
  t << "\\\\\n\\hline\n" << endl;
}

void LatexGenerator::lastIndexPage()
{
}


void LatexGenerator::startMemberList()
{
  if (!m_insideTabbing)
  {
    t << "\\begin{DoxyCompactItemize}" << endl;
  }
}

void LatexGenerator::endMemberList()
{
  //printf("LatexGenerator::endMemberList(%d)\n",m_insideTabbing);
  if (!m_insideTabbing)
  {
    t << "\\end{DoxyCompactItemize}"   << endl;
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
  t << "}\\par" << endl;
  //t << "}" << endl;
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
  t << endl;
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
  t << "\\begin{description}" << endl;
}

void LatexGenerator::endDescription()
{
  t << "\\end{description}" << endl;
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
    t << "]" << endl;
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
  t << "\\end{Desc}" << endl;
}

void LatexGenerator::startParamList(ParamListTypes,const char *title)
{
  t << "\\begin{Desc}\n\\item[";
  docify(title);
  t << "]";
}

void LatexGenerator::endParamList()
{
  t << "\\end{Desc}" << endl;
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
  t << "\\begin{description}" << endl;
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
  t << "\\end{description}" << endl;
  t << "\\end{Desc}" << endl;
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
  t << "}" << endl;
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
  t << "}" << endl;
}

void LatexGenerator::endMemberDocSimple(bool isEnum)
{
  m_codeGen.decUsedTableLevel();
  if (isEnum)
  {
    t << "\\end{DoxyEnumFields}" << endl;
  }
  else
  {
    t << "\\end{DoxyFields}" << endl;
  }
}

void LatexGenerator::startInlineMemberType()
{
  m_insideTabbing = TRUE; // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberType()
{
  t << "&" << endl;
  m_insideTabbing = FALSE;
}

void LatexGenerator::startInlineMemberName()
{
  m_insideTabbing = TRUE; // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberName()
{
  t << "&" << endl;
  m_insideTabbing = FALSE;
}

void LatexGenerator::startInlineMemberDoc()
{
}

void LatexGenerator::endInlineMemberDoc()
{
  t << "\\\\\n\\hline\n" << endl;
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


