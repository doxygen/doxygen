/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
#include "page.h"

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

static QCString escapeLabelName(const char *s)
{
  QCString result;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '%': result+="\\%"; break;
      case '|': result+="\\texttt{\"|}"; break;
      case '!': result+="\"!"; break;
      default: result+=c;
    }
  }
  return result;
}

static QCString escapeMakeIndexChars(LatexGenerator *g,QTextStream &t,const char *s)
{
  QCString result;
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
      case '|': t << "\\texttt{\"|}"; break;
      case '[': t << "["; break;
      case ']': t << "]"; break;
      default:  str[0]=c; g->docify(str); break;
    }
  }
  return result;
}


LatexGenerator::LatexGenerator() : OutputGenerator()
{
  dir=Config_getString("LATEX_OUTPUT");
  col=0;
  //printf("LatexGenerator::LatexGenerator() insideTabbing=FALSE\n");
  insideTabbing=FALSE;
  firstDescItem=TRUE;
  insidePre=FALSE;
}

LatexGenerator::~LatexGenerator()
{
}

//LatexGenerator::LatexGenerator(const LatexGenerator &g)
//  : OutputGenerator(g)
//{
//  col=g.col;
//}

void LatexGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
  col+=((LatexGenerator *)g)->col;
  insideTabbing=insideTabbing || ((LatexGenerator *)g)->insideTabbing;
  firstDescItem = ((LatexGenerator *)g)->firstDescItem;
  insidePre = insidePre || ((LatexGenerator *)g)->insidePre;
  //printf("LatexGenerator::append(%s) insideTabbing=%s\n", g->getContents().data(),
  //    insideTabbing ? "TRUE" : "FALSE" );
}

OutputGenerator *LatexGenerator::copy()
{
  LatexGenerator *result = new LatexGenerator;
  result->insideTabbing=insideTabbing;
  result->insidePre=insidePre;
  return result;
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
  QTextStream t(&file);
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
    << endl
    << "refman.pdf: refman.ps" << endl;
    if (Config_getBool("USE_PDFLATEX")) // use pdflatex instead of latex
    {
      t << "\tpdflatex refman.tex" << endl;
      t << "\tmakeindex refman.idx" << endl;
      t << "\tpdflatex refman.tex" << endl << endl;
    }
    else // otherwise use ps2pdf: not as nice :(
    {
#if defined(_MSC_VER)
      // ps2pdf.bat does not work properly from a makefile using GNU make!
      t << "\tgswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
           "-sOutputFile=refman.pdf -c save pop -f refman.ps" << endl << endl;
#else
      t << "\tps2pdf refman.ps refman.pdf" << endl << endl;
#endif
    }
    
  t << "refman_2on1.ps: refman.ps" << endl
    << "\tpsnup -2 refman.ps >refman_2on1.ps" << endl
    << endl
    << "refman_2on1.pdf: refman_2on1.ps" << endl
#if defined(_MSC_VER)
    // ps2pdf.bat does not work properly from a makefile using GNU make!
    << "\tgswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
           "-sOutputFile=refman_2on1.pdf -c save pop -f refman_2on1.ps" << endl
#else
    << "\tps2pdf refman_2on1.ps refman_2on1.pdf" << endl
#endif
    << endl
    << "refman.dvi: refman.tex doxygen.sty" << endl
    << "\techo \"Running latex...\"" << endl
    << "\tlatex refman.tex" << endl
    << "\techo \"Running makeindex...\"" << endl
    << "\tmakeindex refman.idx" << endl
    << "\techo \"Rerunning latex....\"" << endl
    << "\tlatex refman.tex" << endl
    << "clean:" << endl
    << "\trm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out" << endl;
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
    "\\usepackage{float}\n"
    "\\usepackage{alltt}\n"
    "\\usepackage{doxygen}\n";
  if (Config_getBool("PDF_HYPERLINKS")) 
  {
    t << "\\usepackage{times}" << endl;
    t << "\\ifx\\pdfoutput\\undefined" << endl
      << "\\usepackage[ps2pdf," << endl
      << "            pagebackref=true," << endl
      << "            colorlinks=true," << endl
      << "            linkcolor=blue" << endl
      << "           ]{hyperref}" << endl
      << "\\else" << endl
      << "\\usepackage[pdftex," << endl
      << "            pagebackref=true," << endl
      << "            colorlinks=true," << endl
      << "            linkcolor=blue" << endl
      << "           ]{hyperref}" << endl
      << "\\fi" << endl;
  }
  // Try to get the command for switching on the language
  // support
  QCString sLanguageSupportCommand(
      theTranslator->latexLanguageSupportCommand());

  if (!sLanguageSupportCommand.isEmpty())
  {
    // The command is not empty.  Put it to the output.
    // if the command is empty, no output is needed.
    t << sLanguageSupportCommand;
  }

  QStrList &extraPackages = Config_getList("EXTRA_PACKAGES");
  const char *s=extraPackages.first();
  while (s)
  {
    t << "\\usepackage{" << s << "}\n";
    s=extraPackages.next();
  }
  t << "\\makeindex\n"
    "\\setcounter{tocdepth}{1}\n"
    "\\setlength{\\footrulewidth}{0.4pt}\n"
    "\\begin{document}\n"
    "\\begin{titlepage}\n"
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
  t << "\\addtolength{\\headwidth}{\\marginparsep}\n";
  t << "\\addtolength{\\headwidth}{\\marginparwidth}\n";
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
  t << "\\newenvironment{CompactList}\n";
  t << "{\\begin{list}{}{\n";
  t << "  \\setlength{\\leftmargin}{0.5cm}\n";
  t << "  \\setlength{\\itemsep}{0pt}\n";
  t << "  \\setlength{\\parsep}{0pt}\n";
  t << "  \\setlength{\\topsep}{0pt}\n";
  t << "  \\renewcommand{\\makelabel}{}}}\n";
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
  t << "   {\\parbox[b]{\\labelwidth-4pt}{\\makebox[0pt][l]{\\textbf{#1}}\\\\}}}\n";
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
#if QT_VERSION >= 200
  t.setEncoding(QTextStream::Latin1);
#endif

  writeDefaultStyleSheetPart1(t);
  QCString &projectName = Config_getString("PROJECT_NAME");

  t << theTranslator->trGeneratedAt( dateToString(TRUE), projectName );
  t << " doxygen " << theTranslator->trWrittenBy() << " ";
  t << "Dimitri van Heesch \\copyright~1997-2001";
  writeDefaultStyleSheetPart2(t);
  t << theTranslator->trGeneratedAt( dateToString(TRUE), projectName );
  t << " doxygen " << theTranslator->trWrittenBy() << " ";
  t << "Dimitri van Heesch \\copyright~1997-2001";
  writeDefaultStyleSheetPart3(t);
}

void LatexGenerator::startFile(const char *name,const char *,const char *,bool)
{
  QCString fileName=name;
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
    case isPackageIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Package Index}\n"
      break;
    case isModuleIndex:
      if (compactLatex) t << "\\section"; else t << "\\chapter";
      t << "{"; //Module Index}\n"
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
    case isPackageDocumentation:
      {
        PackageSDict::Iterator pdi(Doxygen::packageDict);
        PackageDef *pd=pdi.toFirst();
        bool found=FALSE;
        while (pd && !found)
        {
          if (compactLatex) t << "\\section"; else t << "\\chapter";
          t << "{"; 
          found=TRUE;
          ++pdi;
          pd=pdi.current();
        }
      }
      break;
    case isModuleDocumentation:
      {
        GroupDef *gd=Doxygen::groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (!gd->isReference())
          {
            if (compactLatex) t << "\\section"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            found=TRUE;
          }
          gd=Doxygen::groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=Doxygen::namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            if (compactLatex) t << "\\section"; else t << "\\chapter";
            t << "{"; // Namespace Documentation}\n":
            found=TRUE;
          }
          nd=Doxygen::namespaceList.next();
        } 
      }
      break;
    case isClassDocumentation:
      {
        ClassSDict::Iterator cli(Doxygen::classSDict);
        ClassDef *cd=0;
        bool found=FALSE;
        for (cli.toFirst();(cd=cli.current()) && !found;++cli)
        {
          if (cd->isLinkableInProject())
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
        FileName *fn=Doxygen::inputNameList.first();
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
          fn=Doxygen::inputNameList.next();
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
        QCString indexName="index";
        if (Config_getBool("GENERATE_TREEVIEW")) indexName="main";
        t << "}\n\\label{index}";
        if (Config_getBool("PDF_HYPERLINKS")) t << "\\hypertarget{index}{}";
        t << "\\input{" << indexName << "}\n";
      }
      break;
    case isPackageIndex:
      t << "}\n\\input{packages}\n";
      break;
    case isModuleIndex:
      t << "}\n\\input{modules}\n";
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
    case isPackageDocumentation:
      {
        PackageSDict::Iterator pdi(Doxygen::packageDict);
        PackageDef *pd=pdi.toFirst();
        bool found=FALSE;
        while (pd && !found)
        {
          t << "}\n\\input{" << pd->getOutputFileBase() << "}\n";
          found=TRUE;
          ++pdi;
          pd=pdi.current();
        }
        while (pd)
        {
          if (compactLatex) t << "\\input"; else t << "\\include";
          t << "{" << pd->getOutputFileBase() << "}\n";
          ++pdi;
          pd=pdi.current();
        }
      }
      break;
    case isModuleDocumentation:
      {
        GroupDef *gd=Doxygen::groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (!gd->isReference())
          {
            t << "}\n\\input{" << gd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
          gd=Doxygen::groupList.next();
        }
        while (gd)
        {
          if (!gd->isReference())
          {
            if (compactLatex) t << "\\input"; else t << "\\include";
            t << "{" << gd->getOutputFileBase() << "}\n";
          }
          gd=Doxygen::groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=Doxygen::namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            t << "}\n\\input{" << nd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
          nd=Doxygen::namespaceList.next();
        }
        while (nd)
        {
          if (nd->isLinkableInProject())
          {
            if (compactLatex) t << "\\input"; else t << "\\include";
            t << "{" << nd->getOutputFileBase() << "}\n";
          }
          nd=Doxygen::namespaceList.next();
        }
      }
      break;
    case isClassDocumentation:
      {
        ClassSDict::Iterator cli(Doxygen::classSDict);
        ClassDef *cd=0;
        bool found=FALSE;
        for (cli.toFirst();(cd=cli.current()) && !found;++cli)
        {
          if (cd->isLinkableInProject())
          {
            t << "}\n\\input{" << cd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
        }
        for (;(cd=cli.current());++cli)
        {
          if (cd->isLinkableInProject())
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
        FileName *fn=Doxygen::inputNameList.first();
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
          fn=Doxygen::inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        t << "}\n";
        PageSDictIterator pdi(*Doxygen::exampleSDict);
        PageInfo *pi=pdi.toFirst();
        if (pi)
        {
          t << "\\input{" << convertNameToFile(pi->name+"-example") << "}\n";
        }
        for (++pdi;(pi=pdi.current());++pdi)
        {
          if (compactLatex) t << "\\input" ; else t << "\\include";
          t << "{" << convertNameToFile(pi->name+"-example") << "}\n";
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "}\n";
        PageSDictIterator pdi(*Doxygen::pageSDict);
        PageInfo *pi=pdi.toFirst();
        bool first=TRUE;
        for (pdi.toFirst();(pi=pdi.current());++pdi)
        {
          if (!pi->inGroup && !pi->isReference())
          {
            QCString pageName;
            if (Config_getBool("CASE_SENSE_NAMES"))
              pageName=pi->name.copy();
            else
              pageName=pi->name.lower();
            if (compactLatex || first) t << "\\input" ; else t << "\\include";
            t << "{" << pageName << "}\n";
            first=FALSE;
          }
        }
      }
      break;
    case isEndIndex:
      t << "\\printindex\n";
      t << "\\end{document}\n";
      break;
  }
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
        t << " Dimitri van Heesch \\copyright~1997-2001";
        t << "}]{}\n";
        writeDefaultStyleSheetPart2(t);
      }
      break;
    case 4:
      {
        t << " Dimitri van Heesch \\copyright~1997-2001";
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

void LatexGenerator::writeString(const char *text)
{
  t << text;
}

void LatexGenerator::writeIndexItem(const char *ref,const char *fn,
                                    const char *name)
{
  t << "\\item ";
  if (!ref && fn)
  {
    t << "\\contentsline{section}{";
    docify(name);
    t << "}{\\pageref{" << fn << "}}{}" << endl;
  }
  else
    docify(name);
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

void LatexGenerator::writeMailLink(const char *url)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\href{mailto:";
    t << url;
    t << "}";
  }
  t << "{\\tt "; 
  docify(url);
  t << "}";
}

void LatexGenerator::writeStartAnnoItem(const char *,const char *,
                                        const char *path,const char *name)
{
  t << "\\item\\contentsline{section}{{\\bf ";
  if (path) docify(path);
  docify(name); 
  t << "} ";
}

void LatexGenerator::writeEndAnnoItem(const char *name)
{
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
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hyperlink{";
    if (f) t << f;
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
  if (!ref && Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hyperlink{";
    if (f) t << f;
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
  t << " {\\rm (";
}

void LatexGenerator::endPageRef(const char *clname, const char *anchor)
{
  t << "\\,\\pageref{";
  if (clname) t << clname; 
  if (anchor) t << "_" << anchor;
  t << "})}";
}

void LatexGenerator::writeCodeLink(const char *,const char *,
                                   const char *,const char *name)
{
  t << name;
  col+=strlen(name);
}

void LatexGenerator::startTitleHead(const char *fileName)
{
  if (Config_getBool("PDF_HYPERLINKS") && fileName)
  {
    t << "\\hypertarget{" << fileName << "}{" << endl;
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
    t << "\\label{" << fileName << "}\\index{" 
      << name << "@{";
    docify(name);
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
}

void LatexGenerator::endGroupHeader()
{
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
}

void LatexGenerator::endMemberHeader()
{
  t << "}" << endl;
}

void LatexGenerator::startMemberDoc(const char *clname,
                                    const char *memname,
                                    const char *,
                                    const char *)
{ 
  if (memname && memname[0]!='@')
  {
    t << "\\index{";
    if (clname)
    {
      t << clname << "@{";
      docify(clname);
      t << "}!";
    }
    t << escapeLabelName(memname) << "@{";
    escapeMakeIndexChars(this,t,memname);
    t << "}}" << endl;

    t << "\\index{" << escapeLabelName(memname) << "@{";
    escapeMakeIndexChars(this,t,memname);
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
  if (Config_getBool("PDF_HYPERLINKS") && memname) 
  {
    t << "["; 
    escapeMakeIndexChars(this,t,memname);
    //filterTitle(title) 
    t << "]";
  }
  t << "{\\setlength{\\rightskip}{0pt plus 5cm}";
}

void LatexGenerator::endMemberDoc() 
{ 
  t << "}";
  if (Config_getBool("COMPACT_LATEX")) t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const char *fName,const char *,
                                     const char *anchor, const char *)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hypertarget{";
    if (fName) t << fName;
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
  t << "\\label{" << name << "}" << endl; 
  if (fName && Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hypertarget{" << fName << "_" << name << "}{}" << endl;
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
    t << "\\index{" << escapeLabelName(s1) << "@{";
    escapeMakeIndexChars(this,t,s1);
    t << "}";
    if (s2)
    {
      t << "!" << s2 << "@{";
      docify(s2);
      t << "}";
    }
    t << "}";
  }
}


void LatexGenerator::startSection(const char *lab,const char *,bool sub)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    t << "\\hypertarget{" << lab << "}{}";
  }
  t << "\\";
  if (Config_getBool("COMPACT_LATEX"))
  {
    if (sub) t << "subsubsection{"; else t << "subsection{";
  }
  else
  {
    if (sub) t << "subsection{"; else t << "section{";
  }
}

void LatexGenerator::endSection(const char *lab,bool)
{
  t << "}\\label{" << lab << "}" << endl;
}

void LatexGenerator::writeSectionRef(const char *ref,const char *,
                                     const char *lab,const char *text)
{
  if (ref) // external reference
  {
    docify(text);
  }
  else // local reference
  {
    if (text && Config_getBool("PDF_HYPERLINKS"))
    {
      t << "\\hyperlink{";
      if (lab) t << lab; 
      t << "}{";
      docify(text);
      t << "}";
      //t << " {\\rm (p.\\,\\pageref{" << lab << "})}";
    }
    else
    {
      if (strcmp(lab,text)!=0) // lab!=text
      {
        // todo: don't hardcode p. here!
        t << "{\\bf ";
        docify(text);
        t << "} {\\rm (p.\\,\\pageref{" << lab << "})}";
      }
      else
      {
        t << "\\ref{" << lab << "}";
      }
    }
  }
}

void LatexGenerator::writeSectionRefItem(const char *,const char *lab,
                                     const char *title)
{
  t << "\\contentsline{section}{";
  docify(title);
  t << "}{\\ref{" << lab << "}}{}" << endl;
}

// TODO: remove this function
//void LatexGenerator::writeSectionRefAnchor(const char *,const char *lab,
//                                     const char *title)
//{
//  startBold();
//  docify(title);
//  endBold();
//  t << " (p.\\,\\pageref{" << lab << "})" << endl;
//}

//void LatexGenerator::docify(const char *str)
//{
//  docifyStatic(t,str);
//}

//void LatexGenerator::docifyStatic(QTextStream &t,const char *str)
void LatexGenerator::docify(const char *str)
{
  static bool isCzech     = theTranslator->idLanguage()=="czech";
  static bool isJapanese  = theTranslator->idLanguage()=="japanese";
  static bool isKorean    = theTranslator->idLanguage()=="korean";
  static bool isRussian   = theTranslator->idLanguage()=="russian";
  static bool isUkrainian = theTranslator->idLanguage()=="ukrainian";
  if (str)
  {
    const unsigned char *p=(const unsigned char *)str;
    unsigned char c;
    unsigned char pc='\0';
    while (*p)
    {
      c=*p++;
      if (insidePre)
      {
        switch(c)
        {
          case '\\': t << "\\(\\backslash\\)"; break;
          case '{':  t << "\\{"; break;
          case '}':  t << "\\}"; break;
          default: t << (char)c; break;
        }
      }
      else
      {
        switch(c)
        {
          case '#':  t << "\\#";           break;
          case '$':  t << "\\$";           break;
          case '%':  t << "\\%";           break;
          case '^':  t << "$^\\wedge$";    break;
          case '&':  t << "\\&";           break;
          case '*':  t << "$\\ast$";       break;
          case '_':  t << "\\_"; 
                     if (!insideTabbing) t << "\\-";  
                     break;
          case '{':  t << "\\{";           break;
          case '}':  t << "\\}";           break;
          case '<':  t << "$<$";           break;
          case '>':  t << "$>$";           break;
          case '|':  t << "$|$";           break;
          case '~':  t << "$\\sim$";       break;
          case '[':  if (Config_getBool("PDF_HYPERLINKS")) 
                       t << "\\mbox{[}"; 
                     else 
                       t << "[";
                     break;
          case ']':  if (pc=='[') t << "$\\,$";
                       if (Config_getBool("PDF_HYPERLINKS"))
                         t << "\\mbox{]}";
                       else
                         t << "]";             
                     break;
          case '-':  if (*p=='>') 
                       { t << " $\\rightarrow$ "; p++; }
                     else
                     { t << (char)c; }
                     break;
          case '\\': if (*p=='<') 
                       { t << "$<$"; p++; }
                     else if (*p=='>')
                     { t << "$>$"; p++; } 
                     else  
                     { t << "$\\backslash$"; }
                     break;           
          case '"':  { t << "\\char`\\\"{}"; }
                     break;

          default:   

             if (isJapanese || isKorean)
             { // Japanese language uses wide characters
               if (c>=128) 
               {
                 t << (char)c;
                 if (*p)  
                 {
                   c = *p++;
                   t << (char)c;
                 }
               }
               else // ascii char => see if we can insert a hypenation hint
               {
                 if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                 t << (char)c;    
               } 
             }
             else if (isCzech || isRussian || isUkrainian)
             {
               if (c>=128)
               {
                 t << (char)c;
               }
               else // ascii char => see if we can insert a hypenation hint
               {
                 if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                 t << (char)c;
               }
             }
             else // language is other than Czech, Russian or Japanese
             {
               switch(c)
               {
                 // the Latin-1 characters
                 case 161: t << "!`";            break;
                 case 181: t << "$\\mu$";        break;
                 case 191: t << "?`";            break;
                 case 192: t << "\\`{A}";        break;
                 case 193: t << "\\'{A}";        break;
                 case 194: t << "\\^{A}";        break;
                 case 195: t << "\\~{A}";        break;
                 case 196: t << "\\\"{A}";       break;
                 case 197: t << "\\AA{}";         break;
                 case 198: t << "\\AE{}";         break;
                 case 199: t << "\\c{C}";        break;
                 case 200: t << "\\`{E}";        break;
                 case 201: t << "\\'{E}";        break;
                 case 202: t << "\\^{E}";        break;
                 case 203: t << "\\\"{E}";       break;
                 case 204: t << "\\`{I}";        break;
                 case 205: t << "\\'{I}";        break;
                 case 206: t << "\\^{I}";        break;
                 case 207: t << "\\\"{I}";       break;
                 case 208: t << "D ";            break; // anyone know the real code?
                 case 209: t << "\\~{N}";        break;
                 case 210: t << "\\`{O}";        break;
                 case 211: t << "\\'{O}";        break;
                 case 212: t << "\\^{O}";        break;
                 case 213: t << "\\~{O}";        break;
                 case 214: t << "\\\"{O}";       break;
                 case 215: t << "$\\times$";     break;
                 case 216: t << "\\O";           break;
                 case 217: t << "\\`{U}";        break;
                 case 218: t << "\\'{U}";        break;
                 case 219: t << "\\^{U}";        break;
                 case 220: t << "\\\"{U}";       break;
                 case 221: t << "\\'{Y}";        break;
                 case 223: t << "\\ss{}";         break; 
                 case 224: t << "\\`{a}";        break;
                 case 225: t << "\\'{a}";        break;
                 case 226: t << "\\^{a}";        break;
                 case 227: t << "\\~{a}";        break;
                 case 228: t << "\\\"{a}";       break;
                 case 229: t << "\\aa{}";         break;
                 case 230: t << "\\ae{}";         break;
                 case 231: t << "\\c{c}";        break;
                 case 232: t << "\\`{e}";        break;
                 case 233: t << "\\'{e}";        break;
                 case 234: t << "\\^{e}";        break;
                 case 235: t << "\\\"{e}";       break;
                 case 236: t << "\\`{\\i}";      break;
                 case 237: t << "\\'{\\i}";      break;
                 case 238: t << "\\^{\\i}";      break;
                 case 239: t << "\\\"{\\i}";     break;
                 case 241: t << "\\~{n}";        break;
                 case 242: t << "\\`{o}";        break;
                 case 243: t << "\\'{o}";        break;
                 case 244: t << "\\^{o}";        break;
                 case 245: t << "\\~{o}";        break;
                 case 246: t << "\\\"{o}";       break;
                 case 248: t << "\\o{}";          break;
                 case 249: t << "\\`{u}";        break;
                 case 250: t << "\\'{u}";        break;
                 case 251: t << "\\^{u}";        break;
                 case 252: t << "\\\"{u}";       break;
                 case 253: t << "\\'{y}";        break;
                 case 255: t << "\\\"{y}";       break;           
                 default: // normal ascii char 
                           { 
                             // see if we can insert an hyphenation hint
                             if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                             t << (char)c;    
                           }
               }
             }
        }
      }
      pc = c;
    }
  }
}

void LatexGenerator::codify(const char *str)
{
  if (str)
  { 
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    int &tabSize = Config_getInt("TAB_SIZE");
    while (*p)
    {
      c=*p++;
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

void LatexGenerator::endClassDiagram(ClassDiagram &d,
                                       const char *fileName,const char *)
{
  d.writeFigure(t,dir,fileName);
}

void LatexGenerator::writeFormula(const char *,const char *text)
{
  t << text;
}

void LatexGenerator::startMemberItem(int annType) 
{ 
  if (!insideTabbing)
  {
    t << "\\item " << endl; 
    switch(annType)
    {
      case 0: break;
      case 1:
      default: 
        t << "\\begin{tabbing}" << endl;
        t << "xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=\\kill" << endl;
        //printf("LatexGenerator::startMemberItem() insideTabbing=TRUE\n");
        insideTabbing=TRUE;
        break;
    }
  }
}

void LatexGenerator::endMemberItem(bool endItem) 
{
  if (insideTabbing && endItem)
  {
    t << endl << "\\end{tabbing}";
    //printf("LatexGenerator::endMemberItem() insideTabbing=FALSE\n");
    insideTabbing=FALSE;
  }
  if (insideTabbing)
  {
    t << "\\\\";
  } 
  t << endl; 
}

void LatexGenerator::writeNonBreakableSpace(int) 
{
  if (insideTabbing)
    t << "\\>";
  else
    t << "\\ "; 
}

void LatexGenerator::startMemberList()  
{ 
  if (!insideTabbing)
    t << "\\begin{CompactItemize}" << endl; 
}

void LatexGenerator::endMemberList()    
{
  if (!insideTabbing)
    t << "\\end{CompactItemize}"   << endl; 
}

void LatexGenerator::startImage(const char *name,const char *size,bool hasCaption)
{
  if (hasCaption)
    t << "\\begin{figure}[H]" << endl;
  else
    t << "\\mbox{";
  QCString gfxName = name;
  if (gfxName.right(4)==".eps") gfxName.left(gfxName.length()-4);
  //     "\\epsfig{file=" << name;
  t << "\\includegraphics";
  if (size) t << "[" << size << "]";
  t << "{" << gfxName << "}";
  if (hasCaption) 
    t << "\\caption{";
  else
    t << "}" << endl;
}

void LatexGenerator::endImage(bool hasCaption)
{
  if (hasCaption)
  {
    t << "}" << endl;
    t << "\\end{figure}" << endl;
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

void LatexGenerator::endDotGraph(DotClassGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"));
}

void LatexGenerator::startInclDepGraph() 
{
}

void LatexGenerator::endInclDepGraph(DotInclDepGraph &g) 
{
  g.writeGraph(t,EPS,Config_getString("LATEX_OUTPUT"));
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

