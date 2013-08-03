/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "pagedef.h"
#include "docparser.h"
#include "latexdocvisitor.h"
#include "dirdef.h"
#include "cite.h"
#include "groupdef.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"

static const char doxygenLatexStyle[] =
#include "doxygen_sty.h"
;

//static QCString filterTitle(const char *s)
//{
//  QCString tmp=s,result;
//  uint i;for (i=0;i<tmp.length();i++)
//  {
//    char c=tmp.at(i);
//    switch(c)
//    {
//      case '#': result+="\\#";  break;
//      case '"': result+="\\\""; break;
//      case '%': result+="\\%";  break;
//      case '[': result+="{";    break;
//      case ']': result+="}";    break;
//      default:  result+=c;      break;
//    }
//  }
//  return result;  
//}



LatexGenerator::LatexGenerator() : OutputGenerator()
{
  dir=Config_getString("LATEX_OUTPUT");
  col=0;
  //printf("LatexGenerator::LatexGenerator() insideTabbing=FALSE\n");
  insideTabbing=FALSE;
  firstDescItem=TRUE;
  disableLinks=FALSE;
  m_indent=0;
  templateMemberItem = FALSE;
  m_prettyCode=Config_getBool("LATEX_SOURCE_CODE");
}

LatexGenerator::~LatexGenerator()
{
}

static void writeLatexMakefile()
{
  bool generateBib = !Doxygen::citeDict->isEmpty();
  QCString dir=Config_getString("LATEX_OUTPUT");
  QCString fileName=dir+"/Makefile";
  QFile file(fileName);
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",fileName.data());
    exit(1);
  }
  // inserted by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  QCString latex_command = Config_getString("LATEX_CMD_NAME");
  QCString mkidx_command = Config_getString("MAKEINDEX_CMD_NAME");
  // end insertion by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  FTextStream t(&file);
  if (!Config_getBool("USE_PDFLATEX")) // use plain old latex
  {
    t << "all: refman.dvi" << endl
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
      << "\t" << latex_command << " refman.tex" << endl
      << "\techo \"Running makeindex...\"" << endl
      << "\t" << mkidx_command << " refman.idx" << endl;
    if (generateBib)
    {
      t << "\techo \"Running bibtex...\"" << endl;
      t << "\tbibtex refman" << endl;
      t << "\techo \"Rerunning latex....\"" << endl;
      t << "\t" << latex_command << " refman.tex" << endl;
    }
    t << "\techo \"Rerunning latex....\"" << endl
      << "\t" << latex_command << " refman.tex" << endl
      << "\tlatex_count=5 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      " << latex_command << " refman.tex ;\\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl
      << "\t" << mkidx_command << " refman.idx" << endl
      << "\t" << latex_command << " refman.tex" << endl << endl
      << "refman_2on1.ps: refman.ps" << endl
      << "\tpsnup -2 refman.ps >refman_2on1.ps" << endl
      << endl
      << "refman_2on1.pdf: refman_2on1.ps" << endl
      << "\tps2pdf refman_2on1.ps refman_2on1.pdf" << endl;
  }
  else // use pdflatex for higher quality output
  {
    t << "all: refman.pdf" << endl << endl
      << "pdf: refman.pdf" << endl << endl;
    t << "refman.pdf: clean refman.tex" << endl;
    t << "\tpdflatex refman" << endl;
    t << "\t" << mkidx_command << " refman.idx" << endl;
    if (generateBib)
    {
      t << "\tbibtex refman" << endl;
      t << "\tpdflatex refman" << endl;
    }
    t << "\tpdflatex refman" << endl
      << "\tlatex_count=5 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      pdflatex refman ;\\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl
      << "\t" << mkidx_command << " refman.idx" << endl
      << "\tpdflatex refman" << endl << endl;
  }

  t << endl
    << "clean:" << endl
    << "\trm -f " 
    << "*.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf" << endl;
}

static void writeMakeBat()
{
#if defined(_MSC_VER)
  QCString dir=Config_getString("LATEX_OUTPUT");
  QCString fileName=dir+"/make.bat";
  QCString latex_command = Config_getString("LATEX_CMD_NAME");
  QCString mkidx_command = Config_getString("MAKEINDEX_CMD_NAME");
  QFile file(fileName);
  bool generateBib = !Doxygen::citeDict->isEmpty();
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",fileName.data());
    exit(1);
  }
  FTextStream t(&file);
  t << "del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf\n\n";
  if (!Config_getBool("USE_PDFLATEX")) // use plain old latex
  {
    t << latex_command << " refman.tex\n";
    t << "echo ----\n";
    t << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "bibtex refman\n";
      t << "echo ----\n";
      t << latex_command << " refman.tex\n";
    }
    t << "setlocal enabledelayedexpansion\n";
    t << "set count=5\n";
    t << ":repeat\n";
    t << "set content=X\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X goto :skip\n";
    t << "set /a count-=1\n";
    t << "if !count! EQU 0 goto :skip\n\n";
    t << "echo ----\n";
    t << latex_command << " refman.tex\n";
    t << "goto :repeat\n";
    t << ":skip\n";
    t << "endlocal\n";
    t << mkidx_command << " refman.idx\n";
    t << latex_command << " refman.tex\n";
    t << "dvips -o refman.ps refman.dvi\n";
    t << "gswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
         "-sOutputFile=refman.pdf -c save pop -f refman.ps\n";
  }
  else // use pdflatex
  {
    t << "pdflatex refman\n";
    t << "echo ----\n";
    t << mkidx_command << " refman.idx\n";
    if (generateBib)
    {
      t << "bibtex refman" << endl;
      t << "pdflatex refman" << endl;
    }
    t << "echo ----\n";
    t << "pdflatex refman\n\n";
    t << "setlocal enabledelayedexpansion\n";
    t << "set count=5\n";
    t << ":repeat\n";
    t << "set content=X\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" refman.log' ) do set content=\"%%~T\"\n";
    t << "if !content! == X goto :skip\n";
    t << "set /a count-=1\n";
    t << "if !count! EQU 0 goto :skip\n\n";
    t << "echo ----\n";
    t << "pdflatex refman\n";
    t << "goto :repeat\n";
    t << ":skip\n";
    t << "endlocal\n";
    t << mkidx_command << " refman.idx\n";
    t << "pdflatex refman\n";
  }
#endif
}

void LatexGenerator::init()
{

  QCString dir=Config_getString("LATEX_OUTPUT");
  QDir d(dir);
  if (!d.exists() && !d.mkdir(dir))
  {
    err("Could not create output directory %s\n",dir.data());
    exit(1);
  }

  writeLatexMakefile();
  writeMakeBat();

  createSubDirs(d);
}

static void writeDefaultHeaderPart1(FTextStream &t)
{
  // part 1

  // Handle batch mode
  if (Config_getBool("LATEX_BATCHMODE"))
    t << "\\batchmode\n";

  // Set document class depending on configuration
  QCString documentClass;
  if (Config_getBool("COMPACT_LATEX"))
    documentClass = "article";
  else
    documentClass = "book";
  t << "\\documentclass[twoside]{" << documentClass << "}\n"
       "\n";

  // Load required packages
  t << "% Packages required by doxygen\n"
       "\\usepackage{calc}\n"
       "\\usepackage{doxygen}\n"
       "\\usepackage{graphicx}\n"
       "\\usepackage[utf8]{inputenc}\n"
       "\\usepackage{makeidx}\n"
       "\\usepackage{multicol}\n"
       "\\usepackage{multirow}\n"
       "\\usepackage{textcomp}\n"
       "\\usepackage[table]{xcolor}\n"
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
  t << "% Font selection\n"
       "\\usepackage[T1]{fontenc}\n"
       "\\usepackage{mathptmx}\n"
       "\\usepackage[scaled=.90]{helvet}\n"
       "\\usepackage{courier}\n"
       "\\usepackage{amssymb}\n"
       "\\usepackage{sectsty}\n"
       "\\renewcommand{\\familydefault}{\\sfdefault}\n"
       "\\allsectionsfont{%\n"
       "  \\fontseries{bc}\\selectfont%\n"
       "  \\color{darkgray}%\n"
       "}\n"
       "\\renewcommand{\\DoxyLabelFont}{%\n"
       "  \\fontseries{bc}\\selectfont%\n"
       "  \\color{darkgray}%\n"
       "}\n"
       "\n";

  // Define page & text layout
  QCString paperName;
  QCString &paperType=Config_getEnum("PAPER_TYPE");
  // "a4wide" package is obsolete (see bug 563698)
  if (paperType=="a4wide")
    paperName="a4";
  else
    paperName=paperType;
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
       "\\setlength{\\parskip}{0.2cm}\n";
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

  // Headers & footers
  QGString genString;
  FTextStream tg(&genString);
  filterLatexString(tg,
                    theTranslator->trGeneratedAt(dateToString(TRUE),
                       Config_getString("PROJECT_NAME")),
                    FALSE,FALSE,FALSE);
  t << "% Headers & footers\n"
       "\\usepackage{fancyhdr}\n"
       "\\pagestyle{fancyplain}\n"
       "\\fancyhead[LE]{\\fancyplain{}{\\bfseries\\thepage}}\n"
       "\\fancyhead[CE]{\\fancyplain{}{}}\n"
       "\\fancyhead[RE]{\\fancyplain{}{\\bfseries\\leftmark}}\n"
       "\\fancyhead[LO]{\\fancyplain{}{\\bfseries\\rightmark}}\n"
       "\\fancyhead[CO]{\\fancyplain{}{}}\n"
       "\\fancyhead[RO]{\\fancyplain{}{\\bfseries\\thepage}}\n"
       "\\fancyfoot[LE]{\\fancyplain{}{}}\n"
       "\\fancyfoot[CE]{\\fancyplain{}{}}\n"
       "\\fancyfoot[RE]{\\fancyplain{}{\\bfseries\\scriptsize " << genString << " Doxygen }}\n"
       "\\fancyfoot[LO]{\\fancyplain{}{\\bfseries\\scriptsize " << genString << " Doxygen }}\n"
       "\\fancyfoot[CO]{\\fancyplain{}{}}\n"
       "\\fancyfoot[RO]{\\fancyplain{}{}}\n"
       "\\renewcommand{\\footrulewidth}{0.4pt}\n";
  if (!Config_getBool("COMPACT_LATEX"))
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
       "\\setcounter{secnumdepth}{5}\n"
       "\\makeindex\n"
       "\n";

  // User-specified packages
  QStrList &extraPackages = Config_getList("EXTRA_PACKAGES");
  if (!extraPackages.isEmpty()) {
    t << "% Packages requested by user\n";
    const char *pkgName=extraPackages.first();
    while (pkgName)
    {
      t << "\\usepackage{" << pkgName << "}\n";
      pkgName=extraPackages.next();
    }
    t << "\n";
  }

  // Hyperlinks
  bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  if (pdfHyperlinks)
  {
    t << "% Hyperlinks (required, but should be loaded last)\n"
         "\\usepackage{ifpdf}\n"
         "\\ifpdf\n"
         "  \\usepackage[pdftex,pagebackref=true]{hyperref}\n"
         "\\else\n"
         "  \\usepackage[ps2pdf,pagebackref=true]{hyperref}\n"
         "\\fi\n"
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
       "\n"
       "\n";

  // End of preamble, now comes the document contents
  t << "%===== C O N T E N T S =====\n"
       "\n"
       "\\begin{document}\n";
  if (theTranslator->idLanguage()=="greek")
    t << "\\selectlanguage{greek}\n";
  t << "\n";

  // Front matter
  t << "% Titlepage & ToC\n";
  bool usePDFLatex = Config_getBool("USE_PDFLATEX");
  if (pdfHyperlinks && usePDFLatex)
  {
    // To avoid duplicate page anchors due to reuse of same numbers for
    // the index (be it as roman numbers)
    t << "\\hypersetup{pageanchor=false}\n";
  }
  t << "\\pagenumbering{roman}\n"
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
  t << " Doxygen " << versionString << "}\\\\\n"
       "\\vspace*{0.5cm}\n"
       "{\\small " << dateToString(TRUE) << "}\\\\\n"
       "\\end{center}\n"
       "\\end{titlepage}\n";
  bool compactLatex = Config_getBool("COMPACT_LATEX");
  if (!compactLatex)
    t << "\\clearemptydoublepage\n";

  // ToC
  t << "\\tableofcontents\n";
  if (!compactLatex)
    t << "\\clearemptydoublepage\n";
  t << "\\pagenumbering{arabic}\n";
  bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
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
  t << doxygenLatexStyle;
}

static void writeDefaultFooter(FTextStream &t)
{
  t << "%--- End generated contents ---\n"
       "\n";

  // Bibliography
  Doxygen::citeDict->writeLatexBibliography(t);

  // Index
  t << "% Index\n"
       "\\newpage\n"
       "\\phantomsection\n"
       "\\addcontentsline{toc}{part}{" << theTranslator->trRTFGeneralIndex() << "}\n"
       "\\printindex\n"
       "\n"
       "\\end{document}\n";
}

void LatexGenerator::writeHeaderFile(QFile &f)
{
  FTextStream t(&f);
  t << "% Latex header for doxygen " << versionString << endl;
  writeDefaultHeaderPart1(t);
  t << "Your title here";
  writeDefaultHeaderPart2(t);
  t << "Generated by";
  writeDefaultHeaderPart3(t);
}

void LatexGenerator::writeFooterFile(QFile &f)
{
  FTextStream t(&f);
  t << "% Latex footer for doxygen " << versionString << endl;
  writeDefaultFooter(t);
}

void LatexGenerator::writeStyleSheetFile(QFile &f)
{
  FTextStream t(&f);
  t << "% stylesheet for doxygen " << versionString << endl;
  writeDefaultStyleSheet(t);
}

void LatexGenerator::startFile(const char *name,const char *,const char *)
{
#if 0
  setEncoding(Config_getString("LATEX_OUTPUT_ENCODING"));
#endif
  QCString fileName=name;
  relPath = relativePathToRoot(fileName);
  sourceFileName = stripPath(fileName);
  if (fileName.right(4)!=".tex" && fileName.right(4)!=".sty") fileName+=".tex";
  startPlainFile(fileName);
}

void LatexGenerator::endFile()
{
  endPlainFile();
  sourceFileName.resize(0);
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
  bool &compactLatex = Config_getBool("COMPACT_LATEX");
  QCString &latexHeader = Config_getString("LATEX_HEADER");
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
          t << substituteKeywords(header,0,
              Config_getString("PROJECT_NAME"),
              Config_getString("PROJECT_NUMBER"),
              Config_getString("PROJECT_BRIEF"));
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
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Introduction}\n"
      break;
    //case isPackageIndex:
    //  if (compactLatex) t << "\\section"; else t << "\\chapter";
    //  t << "{"; //Package Index}\n"
    //  break;
    case isModuleIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Module Index}\n"
      break;
    case isDirIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Directory Index}\n"
      break;
    case isNamespaceIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Namespace Index}\"
      break;
    case isClassHierarchyIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Hierarchical Index}\n"
      break;
    case isCompoundIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Annotated Compound Index}\n"
      break;
    case isFileIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Annotated File Index}\n"
      break;
    case isPageIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
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
            if (compactLatex) t << "\\section"; else t << "\\chapter";
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
            if (compactLatex) t << "\\section"; else t << "\\chapter";
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
          if (nd->isLinkableInProject())
          {
            if (compactLatex) t << "\\section"; else t << "\\chapter";
            t << "{"; // Namespace Documentation}\n":
            found=TRUE;
          }
        } 
      }
      break;
    case isClassDocumentation:
      {
        ClassSDict::Iterator cli(*Doxygen::classSDict);
        ClassDef *cd=0;
        bool found=FALSE;
        for (cli.toFirst();(cd=cli.current()) && !found;++cli)
        {
          if (cd->isLinkableInProject() && 
              cd->templateMaster()==0 &&
              !cd->isEmbeddedInOuterScope()
             )
          {
            if (compactLatex) t << "\\section"; else t << "\\chapter";
            t << "{"; //Compound Documentation}\n";
            found=TRUE;
          }
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=Doxygen::inputNameList->first();
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                if (compactLatex) t << "\\section"; else t << "\\chapter";
                t << "{"; //File Documentation}\n";
                isFirst=FALSE;
                break;
              }
            }
            fd=fn->next();
          }
          fn=Doxygen::inputNameList->next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        if (compactLatex) t << "\\section"; else t << "\\chapter";
        t << "{"; //Example Documentation}\n";
      }
      break;
    case isPageDocumentation:
      {
        if (compactLatex) t << "\\section"; else t << "\\chapter";
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
  //static bool compactLatex = Config_getBool("COMPACT_LATEX");
  static bool sourceBrowser = Config_getBool("SOURCE_BROWSER");
  static QCString latexHeader = Config_getString("LATEX_HEADER");
  static QCString latexFooter = Config_getString("LATEX_FOOTER");
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
        //QCString indexName=Config_getBool("GENERATE_TREEVIEW")?"main":"index";
        QCString indexName="index";
        t << "}\n\\label{index}";
        if (Config_getBool("PDF_HYPERLINKS")) t << "\\hypertarget{index}{}";
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
          if (nd->isLinkableInProject())
          {
            t << "}\n\\input{" << nd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        while ((nd=nli.current()))
        {
          if (nd->isLinkableInProject())
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
        ClassSDict::Iterator cli(*Doxygen::classSDict);
        ClassDef *cd=0;
        bool found=FALSE;
        for (cli.toFirst();(cd=cli.current()) && !found;++cli)
        {
          if (cd->isLinkableInProject() && 
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope()
             )
          {
            t << "}\n\\input{" << cd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        for (;(cd=cli.current());++cli)
        {
          if (cd->isLinkableInProject() && 
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope()
             )
          {
            //if (compactLatex) t << "\\input"; else t << "\\include";
            t << "\\input"; 
            t << "{" << cd->getOutputFileBase() << "}\n";
          } 
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=Doxygen::inputNameList->first();
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                t << "}\n\\input{" << fd->getOutputFileBase() << "}\n";
                if (sourceBrowser && m_prettyCode && fd->generateSourceFile())
                {
                  //t << "\\include{" << fd->getSourceFileBase() << "}\n";
                  t << "\\input{" << fd->getSourceFileBase() << "}\n";
                }
                isFirst=FALSE;
              }
              else
              {
                //if (compactLatex) t << "\\input" ; else t << "\\include";
                t << "\\input" ; 
                t << "{" << fd->getOutputFileBase() << "}\n";
                if (sourceBrowser && m_prettyCode && fd->generateSourceFile())
                {
                  //t << "\\include{" << fd->getSourceFileBase() << "}\n";
                  t << "\\input{" << fd->getSourceFileBase() << "}\n";
                }
              }
            }
            fd=fn->next();
          }
          fn=Doxygen::inputNameList->next();
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
             if (compactLatex) t << "\\section"; else t << "\\chapter";
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
        t << substituteKeywords(footer,0,
              Config_getString("PROJECT_NAME"),
              Config_getString("PROJECT_NUMBER"),
              Config_getString("PROJECT_BRIEF"));
      }
      break;
  }
}

void LatexGenerator::writePageLink(const char *name, bool /*first*/)
{
  //bool &compactLatex = Config_getBool("COMPACT_LATEX");
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
}

void LatexGenerator::newParagraph()
{
  t << endl << endl;
}

void LatexGenerator::startParagraph()
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
    t << "}{\\pageref{" << fn << "}}{}" << endl;
  }
}

//void LatexGenerator::writeIndexFileItem(const char *,const char *text)
//{
//  t << "\\item\\contentsline{section}{";
//  docify(text);
//  t << "}{\\pageref{" << text << "}}" << endl;
//}


void LatexGenerator::startHtmlLink(const char *url)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\href{";
    t << url;
    t << "}";
  }
  t << "{\\tt ";
}

void LatexGenerator::endHtmlLink()
{
  t << "}";
}

//void LatexGenerator::writeMailLink(const char *url)
//{
//  if (Config_getBool("PDF_HYPERLINKS"))
//  {
//    t << "\\href{mailto:";
//    t << url;
//    t << "}";
//  }
//  t << "{\\tt "; 
//  docify(url);
//  t << "}";
//}

void LatexGenerator::writeStartAnnoItem(const char *,const char *,
                                        const char *path,const char *name)
{
  t << "\\item\\contentsline{section}{\\bf ";
  if (path) docify(path);
  docify(name); 
  t << "} ";
}

void LatexGenerator::writeEndAnnoItem(const char *name)
{
  t << "}{\\pageref{" << name << "}}{}" << endl;
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
  t << "}{\\pageref{" << name << "}}{}" << endl;
}

//void LatexGenerator::writeClassLink(const char *,const char *,
//                                    const char *,const char *name)
//{
//  t << "{\\bf ";
//  docify(name);
//  t << "}"; 
//}

void LatexGenerator::startTextLink(const char *f,const char *anchor)
{
  if (!disableLinks && Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hyperlink{";
    if (f) t << stripPath(f);
    if (anchor) t << "_" << anchor; 
    t << "}{";
  }
  else
  {
    t << "{\\bf ";
  }
}

void LatexGenerator::endTextLink()
{
  t << "}";
}

void LatexGenerator::writeObjectLink(const char *ref, const char *f,
                                     const char *anchor, const char *text)
{
  if (!disableLinks && !ref && Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hyperlink{";
    if (f) t << stripPath(f);
    if (f && anchor) t << "_"; 
    if (anchor) t << anchor; 
    t << "}{";
    docify(text);
    t << "}";
  }
  else
  {
    t << "{\\bf ";
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

void LatexGenerator::writeCodeLink(const char *ref,const char *f,
                                   const char *anchor,const char *name,
                                   const char *)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  int l = qstrlen(name);
  if (col+l>80)
  {
    t << "\n      ";
    col=0;
  }
  if (/*m_prettyCode &&*/ !disableLinks && !ref && usePDFLatex && pdfHyperlinks)
  {
    t << "\\hyperlink{";
    if (f) t << stripPath(f);
    if (f && anchor) t << "_"; 
    if (anchor) t << anchor; 
    t << "}{";
    codify(name);
    t << "}";
  }
  else
  {
    t << name;
  }
  col+=l;
}

void LatexGenerator::startTitleHead(const char *fileName)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  if (usePDFLatex && pdfHyperlinks && fileName)
  {
    t << "\\hypertarget{" << stripPath(fileName) << "}{";
  }
  if (Config_getBool("COMPACT_LATEX")) 
  {
    t << "\\subsection{"; 
  }
  else 
  {
    t << "\\section{"; 
  }
}

void LatexGenerator::endTitleHead(const char *fileName,const char *name)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  t << "}" << endl;
  if (name)
  {
    t << "\\label{" << stripPath(fileName) << "}\\index{";
    escapeLabelName(name);
    t << "@{";
    escapeMakeIndexChars(name);
    t << "}}" << endl;
  }
  if (usePDFLatex && pdfHyperlinks && fileName)
  {
    t << "}" << endl;
  }
}

void LatexGenerator::startTitle()
{
  if (Config_getBool("COMPACT_LATEX")) 
  {
    t << "\\subsection{"; 
  }
  else 
  {
    t << "\\section{"; 
  }
}

void LatexGenerator::startGroupHeader(int extraIndentLevel)
{
  if (Config_getBool("COMPACT_LATEX")) 
  {
    extraIndentLevel++;
  }

  if (extraIndentLevel==3)
  {
    t << "\\subparagraph*{"; 
  }
  else if (extraIndentLevel==2)
  {
    t << "\\paragraph{";
  }
  else if (extraIndentLevel==1)
  {
    t << "\\subsubsection{";
  }
  else // extraIndentLevel==0
  {
    t << "\\subsection{";
  }
  disableLinks=TRUE;
}

void LatexGenerator::endGroupHeader(int)
{
  disableLinks=FALSE;
  t << "}" << endl;
}

void LatexGenerator::startMemberHeader(const char *)
{
  if (Config_getBool("COMPACT_LATEX")) 
  {
    t << "\\subsubsection*{"; 
  }
  else 
  {
    t << "\\subsection*{";
  }
  disableLinks=TRUE;
}

void LatexGenerator::endMemberHeader()
{
  disableLinks=FALSE;
  t << "}" << endl;
}

void LatexGenerator::startMemberDoc(const char *clname,
                                    const char *memname,
                                    const char *,
                                    const char *title,
                                    bool showInline)
{ 
  if (memname && memname[0]!='@')
  {
    t << "\\index{";
    if (clname)
    {
      escapeLabelName(clname);
      t << "@{";
      escapeMakeIndexChars(clname);
      t << "}!";
    }
    escapeLabelName(memname);
    t << "@{";
    escapeMakeIndexChars(memname);
    t << "}}" << endl;

    t << "\\index{";
    escapeLabelName(memname);
    t << "@{";
    escapeMakeIndexChars(memname);
    t << "}";
    if (clname)
    {
      t << "!" << clname << "@{";
      docify(clname);
      t << "}"; 
    }
    t << "}" << endl;
  }
  static const char *levelLab[] = { "subsubsection","paragraph","subparagraph", "subparagraph" };
  static bool compactLatex = Config_getBool("COMPACT_LATEX");
  int level=0;
  if (showInline) level+=2;
  if (compactLatex) level++;
  t << "\\" << levelLab[level]; 

  //if (Config_getBool("PDF_HYPERLINKS") && memname) 
  //{
  //  t << "["; 
  //  escapeMakeIndexChars(this,t,memname);
  //  t << "]";
  //}
  t << "[{";
  escapeMakeIndexChars(title);
  t << "}]";
  t << "{\\setlength{\\rightskip}{0pt plus 5cm}";
  disableLinks=TRUE;
}

void LatexGenerator::endMemberDoc(bool) 
{ 
  disableLinks=FALSE;
  t << "}";
  //if (Config_getBool("COMPACT_LATEX")) t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const char *fName,const char *,
                                     const char *anchor, const char *,
                                     const char *)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  if (usePDFLatex && pdfHyperlinks)
  {
    t << "\\hypertarget{";
    if (fName) t << stripPath(fName);
    if (anchor) t << "_" << anchor;
    t << "}{";
  }
}

void LatexGenerator::endDoxyAnchor(const char *fName,const char *anchor)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  if (usePDFLatex && pdfHyperlinks)
  {
    t << "}";
  }
  t << "\\label{";
  if (fName) t << stripPath(fName);
  if (anchor) t << "_" << anchor;
  t << "}" << endl;
}

void LatexGenerator::writeAnchor(const char *fName,const char *name)
{ 
  //printf("LatexGenerator::writeAnchor(%s,%s)\n",fName,name);
  t << "\\label{" << name << "}" << endl; 
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  if (usePDFLatex && pdfHyperlinks)
  {
    if (fName)
    {
      t << "\\hypertarget{" << stripPath(fName) << "_" << name << "}{}" << endl;
    }
    else
    {
      t << "\\hypertarget{" << name << "}{}" << endl;
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
    escapeLabelName(s1);
    t << "@{";
    escapeMakeIndexChars(s1);
    t << "}";
    if (s2)
    {
      t << "!";
      escapeLabelName(s2);
      t << "@{";
      escapeMakeIndexChars(s2);
      t << "}";
    }
    t << "}";
  }
}


void LatexGenerator::startSection(const char *lab,const char *,SectionInfo::SectionType type)
{
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");
  if (usePDFLatex && pdfHyperlinks)
  {
    t << "\\hypertarget{" << stripPath(lab) << "}{}";
  }
  t << "\\";
  if (Config_getBool("COMPACT_LATEX"))
  {
    switch(type)
    {
      case SectionInfo::Page:          t << "subsection"; break;
      case SectionInfo::Section:       t << "subsubsection"; break;
      case SectionInfo::Subsection:    t << "paragraph"; break;
      case SectionInfo::Subsubsection: t << "subparagraph"; break;
      case SectionInfo::Paragraph:     t << "subparagraph"; break;
      default: ASSERT(0); break;
    }
    t << "{";
  }
  else
  {
    switch(type)
    {
      case SectionInfo::Page:          t << "section"; break;
      case SectionInfo::Section:       t << "subsection"; break;
      case SectionInfo::Subsection:    t << "subsubsection"; break;
      case SectionInfo::Subsubsection: t << "paragraph"; break;
      case SectionInfo::Paragraph:     t << "subparagraph"; break;
      default: ASSERT(0); break;
    }
    t << "{";
  }
}

void LatexGenerator::endSection(const char *lab,SectionInfo::SectionType)
{
  t << "}\\label{" << lab << "}" << endl;
}


void LatexGenerator::docify(const char *str)
{
  filterLatexString(t,str,insideTabbing,FALSE,FALSE);
}

void LatexGenerator::codify(const char *str)
{
  if (str)
  { 
    const char *p=str;
    char c;
    //char cs[5];
    int spacesToNextTabStop;
    static int tabSize = Config_getInt("TAB_SIZE");
    const int maxLineLen = 108;
    QCString result(4*maxLineLen+1); // worst case for 1 line of 4-byte chars
    int i;
    while ((c=*p))
    {
      switch(c)
      {
        case 0x0c: p++;  // remove ^L
                   break;
        case '\t': spacesToNextTabStop =
                         tabSize - (col%tabSize);
                   t << Doxygen::spaces.left(spacesToNextTabStop); 
                   col+=spacesToNextTabStop;
                   p++;
                   break; 
        case '\n': t << '\n'; col=0; p++;
                   break;
        default:   
                   i=0;

#undef  COPYCHAR
// helper macro to copy a single utf8 character, dealing with multibyte chars.
#define COPYCHAR() do {                                           \
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
                     col++;                                       \
                   } while(0)

                   // gather characters until we find whitespace or are at
                   // the end of a line
                   COPYCHAR();
                   if (col>=maxLineLen) // force line break
                   {
                     t << "\n      ";
                     col=0;
                   }
                   else // copy more characters
                   {
                     while (col<maxLineLen && (c=*p) && 
                            c!=0x0c && c!='\t' && c!='\n' && c!=' '
                           )
                     {
                       COPYCHAR();
                     }
                     if (col>=maxLineLen) // force line break
                     {
                       t << "\n      ";
                       col=0;
                     }
                   }
                   result[i]=0; // add terminator
                   //if (m_prettyCode)
                   //{
                     filterLatexString(t,result,insideTabbing,TRUE);
                   //}
                   //else
                   //{
                   //  t << result;
                   //}
                   break;
      }
    }
  }
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
  //if (Config_getBool("COMPACT_LATEX")) t << "\\subsubsection"; else t << "\\subsection";
  //t << "{";
}

void LatexGenerator::endClassDiagram(const ClassDiagram &d,
                                       const char *fileName,const char *)
{
  d.writeFigure(t,dir,fileName);
}


void LatexGenerator::startAnonTypeScope(int indent)
{
  if (indent==0)
  {
    t << "\\begin{tabbing}" << endl;
    t << "xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=\\kill" << endl;
    insideTabbing=TRUE;
  }
  m_indent=indent;
}

void LatexGenerator::endAnonTypeScope(int indent)
{
  if (indent==0)
  {
    t << endl << "\\end{tabbing}";
    insideTabbing=FALSE;
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
  if (!insideTabbing)
  {
    t << "\\item " << endl; 
    templateMemberItem = (annoType == 3);
  }
}

void LatexGenerator::endMemberItem() 
{
  if (insideTabbing)
  {
    t << "\\\\";
  } 
  templateMemberItem = FALSE;
  t << endl; 
}

void LatexGenerator::startMemberDescription(const char *,const char *) 
{
  if (!insideTabbing)
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
  if (!insideTabbing)
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
  //printf("writeNonBreakbleSpace()\n");
  if (insideTabbing)
  {
    t << "\\>";
  }
  else
  {
    t << "~"; 
  }
}

void LatexGenerator::startMemberList()  
{ 
  if (!insideTabbing)
  {
    t << "\\begin{DoxyCompactItemize}" << endl; 
  }
}

void LatexGenerator::endMemberList()    
{
  //printf("LatexGenerator::endMemberList(%d)\n",insideTabbing);
  if (!insideTabbing)
  {
    t << "\\end{DoxyCompactItemize}"   << endl; 
  }
}


void LatexGenerator::startMemberGroupHeader(bool hasHeader)
{
  if (hasHeader) t << "\\begin{Indent}";
  t << "{\\bf ";
  // changed back to rev 756 due to bug 660501
  //if (Config_getBool("COMPACT_LATEX")) 
  //{
  //  t << "\\subparagraph*{";
  //}
  //else
  //{
  //  t << "\\paragraph*{";
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

void LatexGenerator::endDotGraph(const DotClassGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),fileName,relPath);
}

void LatexGenerator::startInclDepGraph() 
{
}

void LatexGenerator::endInclDepGraph(const DotInclDepGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),fileName,relPath);
}

void LatexGenerator::startGroupCollaboration() 
{
}

void LatexGenerator::endGroupCollaboration(const DotGroupCollaboration &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),fileName,relPath);
}

void LatexGenerator::startCallGraph() 
{
}

void LatexGenerator::endCallGraph(const DotCallGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),fileName,relPath);
}

void LatexGenerator::startDirDepGraph() 
{
}

void LatexGenerator::endDirDepGraph(const DotDirDeps &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),fileName,relPath);
}

void LatexGenerator::startDescription() 
{ 
  t << "\\begin{description}" << endl; 
}

void LatexGenerator::endDescription()   
{ 
  t << "\\end{description}" << endl; 
  firstDescItem=TRUE;
}

void LatexGenerator::startDescItem()    
{ 
  firstDescItem=TRUE;
  t << "\\item["; 
}

void LatexGenerator::endDescItem()      
{ 
  if (firstDescItem) 
  {
    t << "]" << endl;
    firstDescItem=FALSE;
  } 
  else
  {
    lineBreak();
  }
}

void LatexGenerator::startSimpleSect(SectionTypes,const char *file,
                                     const char *anchor,const char *title)
{
  t << "\\begin{Desc}\n\\item[";
  if (file)
  {
    writeObjectLink(0,file,anchor,title);
  }
  else
  {
    docify(title);
  }
  t << "]";
}

void LatexGenerator::endSimpleSect()
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
  t << endl << "\\begin{DoxyParamCaption}" << endl;
}

void LatexGenerator::endParameterList()
{
}

void LatexGenerator::startParameterType(bool first,const char *key)
{
  t << "\\item[{";
  if (!first && key) t << key;
}

void LatexGenerator::endParameterType()
{
  t << "}]";
}

void LatexGenerator::startParameterName(bool /*oneArgOnly*/)
{
  t << "{";
}

void LatexGenerator::endParameterName(bool last,bool /* emptyList */,bool closeBracket)
{
  t << "}" << endl;

  if (last)
  {
    t << "\\end{DoxyParamCaption}" << endl;
    if (closeBracket) t << ")";
  }
}

void LatexGenerator::exceptionEntry(const char* prefix,bool closeBracket)
{
  if (prefix)
      t << " " << prefix;
  else if (closeBracket)
      t << ")";
  t << " ";
}

void LatexGenerator::writeDoc(DocNode *n,Definition *ctx,MemberDef *)
{
  LatexDocVisitor *visitor = 
    new LatexDocVisitor(t,*this,ctx?ctx->getDefFileExtension():QCString(""),insideTabbing);
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

void LatexGenerator::escapeLabelName(const char *s)
{
  if (s==0) return;
  const char *p=s;
  char c;
  QCString result(qstrlen(s)+1); // worst case allocation
  int i;
  while ((c=*p++))
  {
    switch (c)
    {
      case '%': t << "\\%";       break;
      // NOTE: adding a case here, means adding it to while below as well!
      default:  
        i=0;
        // collect as long string as possible, before handing it to docify
        result[i++]=c;
        while ((c=*p) && c!='%')
        {
          result[i++]=c;
          p++;
        }
        result[i]=0;
        docify(result); 
        break;
    }
  }
}

void LatexGenerator::escapeMakeIndexChars(const char *s)
{
  if (s==0) return;
  const char *p=s;
  char c;
  QCString result(qstrlen(s)+1); // worst case allocation
  int i;
  while ((c=*p++))
  {
    switch (c)
    {
      case '"': t << "\"\""; break;
      case '@': t << "\"@"; break;
      case '[': t << "["; break;
      case ']': t << "]"; break;
      // NOTE: adding a case here, means adding it to while below as well!
      default:  
        i=0;
        // collect as long string as possible, before handing it to docify
        result[i++]=c;
        while ((c=*p) && c!='"' && c!='@' && c!='[' && c!=']')
        {
          result[i++]=c;
          p++;
        }
        result[i]=0;
        docify(result); 
        break;
    }
  }
}

void LatexGenerator::startCodeFragment()
{
  t << "\n\\begin{DoxyCode}\n";
}

void LatexGenerator::endCodeFragment()
{
  t << "\\end{DoxyCode}\n";
}

void LatexGenerator::writeLineNumber(const char *ref,const char *fileName,const char *anchor,int l)
{
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  if (m_prettyCode)
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    if (fileName && !sourceFileName.isEmpty())
    {
      QCString lineAnchor;
      lineAnchor.sprintf("_l%05d",l);
      lineAnchor.prepend(sourceFileName);
      //if (!m_prettyCode) return;
      if (usePDFLatex && pdfHyperlinks)
      {
        t << "\\hypertarget{" << stripPath(lineAnchor) << "}{}";
      }
      writeCodeLink(ref,fileName,anchor,lineNumber,0);
    }
    else
    { 
      codify(lineNumber);
    }
    t << " ";
  }
  else
  {
    t << l << " ";
  }
}

void LatexGenerator::startCodeLine(bool)
{
  col=0;
}

void LatexGenerator::endCodeLine()
{
  codify("\n");
}

void LatexGenerator::startFontClass(const char *name)
{
  //if (!m_prettyCode) return;
  t << "\\textcolor{" << name << "}{";
}

void LatexGenerator::endFontClass()
{
  //if (!m_prettyCode) return;
  t << "}";
}

void LatexGenerator::startInlineHeader()
{
  if (Config_getBool("COMPACT_LATEX")) 
  {
    t << "\\paragraph*{"; 
  }
  else 
  {
    t << "\\subsubsection*{";
  }
}

void LatexGenerator::endInlineHeader()
{
  t << "}" << endl;
}

void LatexGenerator::lineBreak(const char *)
{
  if (insideTabbing)
  {
    t << "\\\\\n";
  }
  else
  {
    t << "\\\\*\n";
  }
}

void LatexGenerator::startMemberDocSimple()
{
  t << "\\begin{DoxyFields}{";
  docify(theTranslator->trCompoundMembers());
  t << "}" << endl;
}

void LatexGenerator::endMemberDocSimple()
{
  t << "\\end{DoxyFields}" << endl;
}

void LatexGenerator::startInlineMemberType()
{
}

void LatexGenerator::endInlineMemberType()
{
  t << "&" << endl;
}

void LatexGenerator::startInlineMemberName()
{
}

void LatexGenerator::endInlineMemberName()
{
  t << "&" << endl;
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

