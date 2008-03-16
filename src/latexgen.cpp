/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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

#include "qtbc.h"
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
}

LatexGenerator::~LatexGenerator()
{
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
  QTextStream t(&file);
  if (!Config_getBool("USE_PDFLATEX")) // use plain old latex
  {
    t << "all: clean refman.dvi" << endl
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
#if defined(_MSC_VER)
    // ps2pdf.bat does not work properly from a makefile using GNU make!
    t << "\tgswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
      "-sOutputFile=refman.pdf -c save pop -f refman.ps" << endl << endl;
#else
    t << "\tps2pdf refman.ps refman.pdf" << endl << endl;
#endif
    t << "refman.dvi: refman.tex doxygen.sty" << endl
      << "\techo \"Running latex...\"" << endl
      << "\t" << latex_command << " refman.tex" << endl
      << "\techo \"Running makeindex...\"" << endl
      << "\t" << mkidx_command << " refman.idx" << endl
      << "\techo \"Rerunning latex....\"" << endl
      << "\t" << latex_command << " refman.tex" << endl
      << "\tlatex_count=5 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      " << latex_command << " refman.tex ;\\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl << endl
      << "refman_2on1.ps: refman.ps" << endl
      << "\tpsnup -2 refman.ps >refman_2on1.ps" << endl
      << endl
      << "refman_2on1.pdf: refman_2on1.ps" << endl
#if defined(_MSC_VER)
      // ps2pdf.bat does not work properly from a makefile using GNU make!
      << "\tgswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
         "-sOutputFile=refman_2on1.pdf -c save pop -f refman_2on1.ps" << endl;
#else
      << "\tps2pdf refman_2on1.ps refman_2on1.pdf" << endl;
#endif
  }
  else // use pdflatex for higher quality output
  {
    t << "all: clean refman.pdf" << endl << endl
      << "pdf: refman.pdf" << endl << endl;
    t << "refman.pdf: refman.tex" << endl;
    t << "\tpdflatex refman.tex" << endl;
    t << "\tmakeindex refman.idx" << endl;
    t << "\tpdflatex refman.tex" << endl
      << "\tlatex_count=5 ; \\" << endl
      << "\twhile egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\\" << endl
      << "\t    do \\" << endl
      << "\t      echo \"Rerunning latex....\" ;\\" << endl
      << "\t      pdflatex refman.tex ;\\" << endl
      << "\t      latex_count=`expr $$latex_count - 1` ;\\" << endl
      << "\t    done" << endl << endl;
  }

  t << endl
    << "clean:" << endl
    << "\trm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out refman.pdf" << endl;

  createSubDirs(d);
}

static void writeDefaultHeaderPart1(QTextStream &t)
{
  // part 1

  QCString paperName;
  if (Config_getBool("LATEX_BATCHMODE")) t << "\\batchmode" << endl;
  QCString &paperType=Config_getEnum("PAPER_TYPE");
  if (paperType=="a4wide") 
    paperName="a4"; 
  else 
    paperName=paperType;
  t << "\\documentclass[" << paperName << "paper";
  //if (Config_getBool("PDF_HYPERLINKS")) t << ",ps2pdf";
  t << "]{";
  if (Config_getBool("COMPACT_LATEX")) t << "article"; else t << "book";
  t << "}\n";
  if (paperType=="a4wide") t << "\\usepackage{a4wide}\n";
  t << "\\usepackage{makeidx}\n"
    "\\usepackage{fancyhdr}\n"
    "\\usepackage{graphicx}\n"
    "\\usepackage{multicol}\n"
    "\\usepackage{float}\n"
    "\\usepackage{textcomp}\n"
    "\\usepackage{alltt}\n"
    //"\\usepackage{ae,aecompl,aeguill}\n"
    ;
  if (Config_getBool("USE_PDFLATEX"))
  {
    t << "\\usepackage{times}" << endl;
  }
  if (Config_getBool("PDF_HYPERLINKS")) 
  {
    t << "\\usepackage{ifpdf}" << endl
      << "\\ifpdf" << endl
      << "\\usepackage[pdftex," << endl
      << "            pagebackref=true," << endl
      << "            colorlinks=true," << endl
      << "            linkcolor=blue," << endl
      << "            unicode" << endl
      << "           ]{hyperref}" << endl
      << "\\else" << endl
      << "\\usepackage[ps2pdf," << endl
      << "            pagebackref=true," << endl
      << "            colorlinks=true," << endl
      << "            linkcolor=blue," << endl
      << "            unicode" << endl
      << "           ]{hyperref}" << endl
      << "\\usepackage{pspicture}" << endl
      << "\\fi" << endl;
  }
  // Try to get the command for switching on the language
  // support
  t << "\\usepackage[utf8]{inputenc}" << endl;
  QCString sLanguageSupportCommand(
      theTranslator->latexLanguageSupportCommand());

  if (!sLanguageSupportCommand.isEmpty())
  {
    // The command is not empty.  Put it to the output.
    // if the command is empty, no output is needed.
    t << sLanguageSupportCommand << endl;
  }
  t << "\\usepackage{doxygen}\n";
  QStrList &extraPackages = Config_getList("EXTRA_PACKAGES");
  const char *s=extraPackages.first();
  while (s)
  {
    t << "\\usepackage{" << s << "}\n";
    s=extraPackages.next();
  }
  t << "\\makeindex\n"
    "\\setcounter{tocdepth}{3}\n"
    "\\renewcommand{\\footrulewidth}{0.4pt}\n"
    "\\begin{document}\n";
  if (theTranslator->idLanguage()=="greek") t << "\\selectlanguage{greek}\n";
  t << "\\begin{titlepage}\n"
    "\\vspace*{7cm}\n"
    "\\begin{center}\n"
    "{\\Large ";
}

static void writeDefaultHeaderPart2(QTextStream &t)
{
  // part 2
  t << "}\\\\" << endl
    << "\\vspace*{1cm}" << endl
    << "{\\large ";
}

static void writeDefaultHeaderPart3(QTextStream &t)
{
  // part 3
  t << " Doxygen " << versionString << "}\\\\" << endl
    << "\\vspace*{0.5cm}" << endl
    << "{\\small " << dateToString(TRUE) << "}\\\\" << endl
    << "\\end{center}" << endl
    << "\\end{titlepage}" << endl;
  if (!Config_getBool("COMPACT_LATEX")) t << "\\clearemptydoublepage\n";
  t << "\\pagenumbering{roman}\n";
  t << "\\tableofcontents\n";
  if (!Config_getBool("COMPACT_LATEX")) t << "\\clearemptydoublepage\n";
  t << "\\pagenumbering{arabic}\n";
}

static void writeDefaultStyleSheetPart1(QTextStream &t)
{
  // part 1
  t << "\\NeedsTeXFormat{LaTeX2e}\n";
  t << "\\ProvidesPackage{doxygen}\n";
  t << "\\RequirePackage{calc}\n";
  t << "\\RequirePackage{array}\n";
  t << "\\pagestyle{fancyplain}\n";
  //t << "\\addtolength{\\headwidth}{\\marginparsep}\n";
  //t << "\\addtolength{\\headwidth}{\\marginparwidth}\n";
  t << "\\newcommand{\\clearemptydoublepage}{\\newpage{\\pagestyle{empty}";
  t << "\\cleardoublepage}}\n";
  if (!Config_getBool("COMPACT_LATEX")) 
    t << "\\renewcommand{\\chaptermark}[1]{\\markboth{#1}{}}\n";
  t << "\\renewcommand{\\sectionmark}[1]{\\markright{\\thesection\\ #1}}\n";
  t << "\\lhead[\\fancyplain{}{\\bfseries\\thepage}]\n";
  t << "        {\\fancyplain{}{\\bfseries\\rightmark}}\n";
  t << "\\rhead[\\fancyplain{}{\\bfseries\\leftmark}]\n";
  t << "        {\\fancyplain{}{\\bfseries\\thepage}}\n";
  t << "\\rfoot[\\fancyplain{}{\\bfseries\\scriptsize ";
}

static void writeDefaultStyleSheetPart2(QTextStream &t)
{
  t << "\\lfoot[]{\\fancyplain{}{\\bfseries\\scriptsize ";
}

static void writeDefaultStyleSheetPart3(QTextStream &t)
{
  t << "}}\n";
  t << "\\cfoot{}\n";
  t << "\\newenvironment{Code}\n";
  t << "{\\footnotesize}\n";
  t << "{\\normalsize}\n";
  t << "\\newcommand{\\doxyref}[3]{\\textbf{#1} (\\textnormal{#2}\\,\\pageref{#3})}\n";
  t << "\\newenvironment{DocInclude}\n";
  t << "{\\footnotesize}\n";
  t << "{\\normalsize}\n";
  t << "\\newenvironment{VerbInclude}\n";
  t << "{\\footnotesize}\n";
  t << "{\\normalsize}\n";
  t << "\\newenvironment{Image}\n";
  t << "{\\begin{figure}[H]}\n";
  t << "{\\end{figure}}\n";
  t << "\\newenvironment{ImageNoCaption}{}{}\n";
  t << "\\newenvironment{CompactList}\n";
  t << "{\\begin{list}{}{\n";
  t << "  \\setlength{\\leftmargin}{0.5cm}\n";
  t << "  \\setlength{\\itemsep}{0pt}\n";
  t << "  \\setlength{\\parsep}{0pt}\n";
  t << "  \\setlength{\\topsep}{0pt}\n";
  t << "  \\renewcommand{\\makelabel}{\\hfill}}}\n";
  t << "{\\end{list}}\n";
  t << "\\newenvironment{CompactItemize}\n";
  t << "{\n";
  t << "  \\begin{itemize}\n";
  t << "  \\setlength{\\itemsep}{-3pt}\n";
  t << "  \\setlength{\\parsep}{0pt}\n";
  t << "  \\setlength{\\topsep}{0pt}\n";
  t << "  \\setlength{\\partopsep}{0pt}\n";
  t << "}\n";
  t << "{\\end{itemize}}\n";
  t << "\\newcommand{\\PBS}[1]{\\let\\temp=\\\\#1\\let\\\\=\\temp}\n";
  t << "\\newlength{\\tmplength}\n";
  t << "\\newenvironment{TabularC}[1]\n";
  t << "{\n";
  t << "\\setlength{\\tmplength}\n";
  t << "     {\\linewidth/(#1)-\\tabcolsep*2-\\arrayrulewidth*(#1+1)/(#1)}\n";
  t << "      \\par\\begin{tabular*}{\\linewidth}\n";
  t << "             {*{#1}{|>{\\PBS\\raggedright\\hspace{0pt}}p{\\the\\tmplength}}|}\n";
  t << "}\n";
  t << "{\\end{tabular*}\\par}\n";
  t << "\\newcommand{\\entrylabel}[1]{\n";
  t << "   {\\parbox[b]{\\labelwidth-4pt}{\\makebox[0pt][l]{\\textbf{#1}}\\vspace{1.5\\baselineskip}}}}\n";
  t << "\\newenvironment{Desc}\n";
  t << "{\\begin{list}{}\n";
  t << "  {\n";
  t << "    \\settowidth{\\labelwidth}{40pt}\n";
  t << "    \\setlength{\\leftmargin}{\\labelwidth}\n";
  t << "    \\setlength{\\parsep}{0pt}\n";
  t << "    \\setlength{\\itemsep}{-4pt}\n";
  t << "    \\renewcommand{\\makelabel}{\\entrylabel}\n";
  t << "  }\n";
  t << "}\n";
  t << "{\\end{list}}\n";

  t << "\\newenvironment{Indent}\n";
  t << "  {\\begin{list}{}{\\setlength{\\leftmargin}{0.5cm}}\n";
  t << "      \\item[]\\ignorespaces}\n";
  t << "  {\\unskip\\end{list}}\n";

  t << "\\setlength{\\parindent}{0cm}\n";
  t << "\\setlength{\\parskip}{0.2cm}\n";
  t << "\\addtocounter{secnumdepth}{1}\n";
  t << "\\sloppy\n";
  t << "\\usepackage[T1]{fontenc}\n";
}

void LatexGenerator::writeHeaderFile(QFile &f)
{
  QTextStream t(&f);
  writeDefaultHeaderPart1(t);
  t << "Your title here";
  writeDefaultHeaderPart2(t);
  t << "Generated by";
  writeDefaultHeaderPart3(t);
}

void LatexGenerator::writeStyleSheetFile(QFile &f)
{
  QTextStream t(&f);
  t.setEncoding(QTextStream::UnicodeUTF8);

  writeDefaultStyleSheetPart1(t);
  QCString &projectName = Config_getString("PROJECT_NAME");

  t << theTranslator->trGeneratedAt( dateToString(TRUE), projectName );
  t << " doxygen";
  //t << " " << theTranslator->trWrittenBy() << " ";
  //t << "Dimitri van Heesch \\copyright~1997-2008";
  writeDefaultStyleSheetPart2(t);
  t << theTranslator->trGeneratedAt( dateToString(TRUE), projectName );
  t << " doxygen";
  //t << " << theTranslator->trWrittenBy() << " ";
  //t << "Dimitri van Heesch \\copyright~1997-2008";
  writeDefaultStyleSheetPart3(t);
}

void LatexGenerator::startFile(const char *name,const char *,const char *)
{
#if 0
  setEncoding(Config_getString("LATEX_OUTPUT_ENCODING"));
#endif
  QCString fileName=name;
  relPath = relativePathToRoot(fileName);
  if (fileName.right(4)!=".tex" && fileName.right(4)!=".sty") fileName+=".tex";
  startPlainFile(fileName);
}

void LatexGenerator::endFile()
{
  endPlainFile();
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
          t << substituteKeywords(header,0);
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
          if (cd->isLinkableInProject() && cd->templateMaster()==0)
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
  bool &compactLatex = Config_getBool("COMPACT_LATEX");
  QCString &latexHeader = Config_getString("LATEX_HEADER");
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
        QCString indexName=usingTreeIndex()?"main":"index";
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
            if (compactLatex) t << "\\input"; else t << "\\include";
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
            if (compactLatex) t << "\\input"; else t << "\\include";
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
            if (compactLatex) t << "\\input"; else t << "\\include";
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
          if (cd->isLinkableInProject() && cd->templateMaster()==0)
          {
            t << "}\n\\input{" << cd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        for (;(cd=cli.current());++cli)
        {
          if (cd->isLinkableInProject() && cd->templateMaster()==0)
          {
            if (compactLatex) t << "\\input"; else t << "\\include";
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
                isFirst=FALSE;
              }
              else
              {
                if (compactLatex) t << "\\input" ; else t << "\\include";
                t << "{" << fd->getOutputFileBase() << "}\n";
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
          if (compactLatex) t << "\\input" ; else t << "\\include";
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
      t << "\\printindex\n";
      t << "\\end{document}\n";
      break;
  }
}

void LatexGenerator::writePageLink(const char *name, bool first)
{
  bool &compactLatex = Config_getBool("COMPACT_LATEX");
  if (compactLatex || first) t << "\\input" ; else t << "\\include";
  t << "{" << name << "}\n";
}


void LatexGenerator::writeStyleInfo(int part)
{
  switch(part)
  {
    case 0:
      {
        //QCString pname=Config_getString("PROJECT_NAME").stripWhiteSpace();
        startPlainFile("doxygen.sty");
        writeDefaultStyleSheetPart1(t);
      }
      break;
    case 1:
    case 3:
      t << " Doxygen ";
      break;
    case 2:
      {
        //t << " Dimitri van Heesch \\copyright~1997-2008";
        t << "}]{}\n";
        writeDefaultStyleSheetPart2(t);
      }
      break;
    case 4:
      {
        //t << " Dimitri van Heesch \\copyright~1997-2008";
        writeDefaultStyleSheetPart3(t);
        endPlainFile();
      }
      break;
  }
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
  if (hasBrief) t << "(";
}

void LatexGenerator::endIndexValue(const char *name,bool hasBrief)
{
  if (hasBrief) t << ")";
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

void LatexGenerator::writeCodeLink(const char *,const char *,
                                   const char *,const char *name,
                                   const char *)
{
  t << name;
  col+=strlen(name);
}

void LatexGenerator::startTitleHead(const char *fileName)
{
  if (Config_getBool("PDF_HYPERLINKS") && fileName)
  {
    t << "\\hypertarget{" << stripPath(fileName) << "}{" << endl;
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
  t << "}" << endl;
  if (name)
  {
    t << "\\label{" << fileName << "}\\index{";
    escapeLabelName(name);
    t << "@{";
    escapeMakeIndexChars(name);
    t << "}}" << endl;
  }
  if (Config_getBool("PDF_HYPERLINKS") && fileName)
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

void LatexGenerator::startGroupHeader()
{
  if (Config_getBool("COMPACT_LATEX")) 
  {
    t << "\\subsubsection{"; 
  }
  else 
  {
    t << "\\subsection{";
  }
  disableLinks=TRUE;
}

void LatexGenerator::endGroupHeader()
{
  disableLinks=FALSE;
  t << "}" << endl;
}

void LatexGenerator::startMemberHeader()
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
                                    const char *title)
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
  if (Config_getBool("COMPACT_LATEX")) t << "\\paragraph"; else t << "\\subsubsection";
  //if (Config_getBool("PDF_HYPERLINKS") && memname) 
  //{
  //  t << "["; 
  //  escapeMakeIndexChars(this,t,memname);
  //  t << "]";
  //}
  t << "[";
  escapeMakeIndexChars(title);
  t << "]";
  t << "{\\setlength{\\rightskip}{0pt plus 5cm}";
  disableLinks=TRUE;
}

void LatexGenerator::endMemberDoc(bool) 
{ 
  disableLinks=FALSE;
  t << "}";
  if (Config_getBool("COMPACT_LATEX")) t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const char *fName,const char *,
                                     const char *anchor, const char *,
                                     const char *)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hypertarget{";
    if (fName) t << stripPath(fName);
    if (anchor) t << "_" << anchor;
    t << "}{" << endl;
  }
}

void LatexGenerator::endDoxyAnchor(const char *fName,const char *anchor)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "}" << endl;
  }
  t << "\\label{";
  if (fName) t << fName;
  if (anchor) t << "_" << anchor;
  t << "}" << endl;
}

void LatexGenerator::writeAnchor(const char *fName,const char *name)
{ 
  //printf("LatexGenerator::writeAnchor(%s,%s)\n",fName,name);
  t << "\\label{" << name << "}" << endl; 
  if (Config_getBool("PDF_HYPERLINKS"))
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
  if (Config_getBool("PDF_HYPERLINKS"))
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


//void LatexGenerator::docifyStatic(QTextStream &t,const char *str)
void LatexGenerator::docify(const char *str)
{
  filterLatexString(t,str,insideTabbing,FALSE);
}

void LatexGenerator::codify(const char *str)
{
  static bool isJapanese  = theTranslator->idLanguage()=="japanese" || 
                            theTranslator->idLanguage()=="japanese-en";

  if (str)
  { 
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    int &tabSize = Config_getInt("TAB_SIZE");
    while (*p)
    {
      static bool MultiByte = FALSE;
      c=*p++;

      if( isJapanese )
      {
        if ( MultiByte )
        {
          t << (char)c;
          MultiByte = FALSE;
          continue;
        }
        if ((uchar)c>=0x80) // char in range [0x80..0xff]
        {
          t << (char)c;
          MultiByte = TRUE;
          continue;
        }
      }

      switch(c)
      {
        case 0x0c: break; // remove ^L
        case '\t': spacesToNextTabStop =
                         tabSize - (col%tabSize);
                   t << spaces.left(spacesToNextTabStop); 
                   col+=spacesToNextTabStop;
                   break; 
        case '\n': t << '\n'; col=0;                    break;
        default:   t << c;    col++;                    break;
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

void LatexGenerator::endMemberTemplateParams()
{
  if (templateMemberItem)
  {
    t << "}\\\\";
  }
}

void LatexGenerator::startMemberItem(int annoType) 
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

void LatexGenerator::startMemberDescription() 
{
  if (!insideTabbing)
  { 
    t << "\\begin{CompactList}\\small\\item\\em "; 
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
    t << "\\item\\end{CompactList}"; 
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
    t << "~"; 
}

void LatexGenerator::startMemberList()  
{ 
  if (!insideTabbing)
  {
    t << "\\begin{CompactItemize}" << endl; 
  }
}

void LatexGenerator::endMemberList()    
{
  //printf("LatexGenerator::endMemberList(%d)\n",insideTabbing);
  if (!insideTabbing)
  {
    t << "\\end{CompactItemize}"   << endl; 
  }
}


void LatexGenerator::startMemberGroupHeader(bool hasHeader)
{
  if (hasHeader) t << "\\begin{Indent}";
  t << "{\\bf ";
}

void LatexGenerator::endMemberGroupHeader()
{
  t << "}\\par" << endl;
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
}

void LatexGenerator::endDotGraph(const DotClassGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),relPath);
}

void LatexGenerator::startInclDepGraph() 
{
}

void LatexGenerator::endInclDepGraph(const DotInclDepGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),relPath);
}

void LatexGenerator::startGroupCollaboration() 
{
}

void LatexGenerator::endGroupCollaboration(const DotGroupCollaboration &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),relPath);
}

void LatexGenerator::startCallGraph() 
{
}

void LatexGenerator::endCallGraph(const DotCallGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),relPath);
}

void LatexGenerator::startDirDepGraph() 
{
}

void LatexGenerator::endDirDepGraph(const DotDirDeps &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"),relPath);
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

void LatexGenerator::startParameterType(bool first,const char *key)
{
  if (!first)
  {
    t << "\\/ " << key << " ";
  }
}

void LatexGenerator::printDoc(DocNode *n,const char *langExt)
{
  LatexDocVisitor *visitor = new LatexDocVisitor(t,*this,langExt,insideTabbing);
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
  const char *p=s;
  char str[2];
  str[1]=0;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '%': t << "\\%";       break;
      case '|': t << "\\tt{\"|}"; break;
      case '!': t << "\"!";       break;
      default:  str[0]=c; docify(str); break;
    }
  }
}

void LatexGenerator::escapeMakeIndexChars(const char *s)
{
  const char *p=s;
  char str[2];
  str[1]=0;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '!': t << "\"!"; break;
      case '"': t << "\"\""; break;
      case '@': t << "\"@"; break;
      case '|': t << "\\tt{\"|}"; break;
      case '[': t << "["; break;
      case ']': t << "]"; break;
      default:  str[0]=c; docify(str); break;
    }
  }
}


