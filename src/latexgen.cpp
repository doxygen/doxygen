/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
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

static QCString filterTitle(const char *s)
{
  QCString tmp=s,result;
  uint i;for (i=0;i<tmp.length();i++)
  {
    char c=tmp.at(i);
    switch(c)
    {
      case '#': result+="\\#";  break;
      case '"': result+="\\\""; break;
      case '%': result+="\\%";  break;
      case '[': result+="{";    break;
      case ']': result+="}";    break;
      default:  result+=c;      break;
    }
  }
  return result;  
}

//static QCString escapeLabelName(const QCString &s)
//{
//  QCString result;
//  uint i;
//  for (i=0;i<s.length();i++)
//  {
//    char c=s.at(i);
//    switch (c)
//    {
//      case '~': result+=".1"; break;
//      case '%': result+=".2"; break;
//      default: result+=c;
//    }
//  }
//  return result;
//}


LatexGenerator::LatexGenerator() : OutputGenerator()
{
  dir=Config::latexOutputDir;
  col=0;
  //printf("LatexGenerator::LatexGenerator() insideTabbing=FALSE\n");
  insideTabbing=FALSE;
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
  //printf("LatexGenerator::append(%s) insideTabbing=%s\n", g->getContents().data(),
  //    insideTabbing ? "TRUE" : "FALSE" );
}

OutputGenerator *LatexGenerator::copy()
{
  LatexGenerator *result = new LatexGenerator;
  result->insideTabbing=insideTabbing;
  return result;
}

void LatexGenerator::init()
{
  QCString dir=Config::latexOutputDir;
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
    << "pdf: ps" << endl
    << "\tps2pdf refman.ps refman.pdf" << endl
    << endl
    << "refman.ps: refman.dvi" << endl
    << "\tdvips -o refman.ps refman.dvi" << endl
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

void LatexGenerator::startFile(const char *name,const char *,bool)
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
  QCString paperName;
  switch (is)
  {
    case isTitlePageStart:
      {
        if (Config::latexHeaderFile.isEmpty())
        {
          if (Config::latexBatchModeFlag) t << "\\batchmode" << endl;
          if (Config::paperType=="a4wide") paperName="a4"; else paperName=Config::paperType;
          t << "\\documentclass[" << paperName << "paper";
          if (Config::pdfHyperFlag) t << ",ps2pdf";
          t << "]{";
          if (Config::compactLatexFlag) t << "article"; else t << "book";
          t << "}\n";
          if (Config::paperType=="a4wide") t << "\\usepackage{a4wide}\n";
          t << "\\usepackage{makeidx}\n"
               "\\usepackage{fancyheadings}\n"
               "\\usepackage{epsfig}\n"
               "\\usepackage{float}\n"
               "\\usepackage{doxygen}\n";
          if (Config::pdfHyperFlag) 
          {
            t << "\\usepackage{times}" << endl
              << "\\usepackage[backref=true," << endl
              << "            pagebackref=true," << endl
              << "            colorlinks=true," << endl
              << "            linkcolor=blue" << endl
              << "           ]{hyperref}" << endl;
          }
          if (!theTranslator->latexBabelPackage().isEmpty())
          {
            if (theTranslator->latexBabelPackage()=="russian")
            {
              t << "\\usepackage{inputenc}\n";
            }
            t << "\\usepackage[" << theTranslator->latexBabelPackage() << "]{babel}\n";
          }

          const char *s=Config::extraPackageList.first();
          while (s)
          {
            t << "\\usepackage{" << s << "}\n";
            s=Config::extraPackageList.next();
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
        else
        {
          QCString header = fileToString(Config::latexHeaderFile);
          t << substituteKeywords(header,0);
        }
      }
      break;
    case isTitlePageAuthor:
      if (Config::latexHeaderFile.isEmpty())
      {
        t << "}\\\\" << endl
          << "\\vspace*{1cm}" << endl
          << "{\\large ";
      }
      break;
    case isMainPage:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Introduction}\n"
      break;
    case isModuleIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Module Index}\n"
      break;
    case isNamespaceIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Namespace Index}\"
      break;
    case isClassHierarchyIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Hierarchical Index}\n"
      break;
    case isCompoundIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Annotated Compound Index}\n"
      break;
    case isFileIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Annotated File Index}\n"
      break;
    case isPageIndex:
      if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
      t << "{"; //Annotated Page Index}\n"
      break;
    case isModuleDocumentation:
      {
        GroupDef *gd=groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
            t << "{"; //Module Documentation}\n";
            found=TRUE;
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
            t << "{"; // Namespace Documentation}\n":
            found=TRUE;
          }
          nd=namespaceList.next();
        } 
      }
      break;
    case isClassDocumentation:
      {
        ClassDef *cd=classList.first();
        bool found=FALSE;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
            t << "{"; //Compound Documentation}\n";
            found=TRUE;
          }
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
                t << "{"; //File Documentation}\n";
                isFirst=FALSE;
                break;
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
        t << "{"; //Example Documentation}\n";
      }
      break;
    case isPageDocumentation:
      {
        if (Config::compactLatexFlag) t << "\\section"; else t << "\\chapter";
        t << "{"; //Page Documentation}\n";
      }
      break;
    case isEndIndex:
      break;
  }
}

void LatexGenerator::endIndexSection(IndexSections is)
{
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      if (Config::latexHeaderFile.isEmpty())
      {
        t << " Doxygen " << versionString << "}\\\\" << endl
          << "\\vspace*{0.5cm}" << endl
          << "{\\small " << dateToString(TRUE) << "}\\\\" << endl
          << "\\end{center}" << endl
          << "\\end{titlepage}" << endl;
        if (!Config::compactLatexFlag) t << "\\clearemptydoublepage\n";
        t << "\\pagenumbering{roman}\n";
        t << "\\tableofcontents\n";
        if (!Config::compactLatexFlag) t << "\\clearemptydoublepage\n";
        t << "\\pagenumbering{arabic}\n";
      }
      break;
    case isMainPage:
      t << "}\n\\input{index}\n";
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
    case isModuleDocumentation:
      {
        GroupDef *gd=groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            t << "}\n\\input{" << gd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
          gd=groupList.next();
        }
        while (gd)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            if (Config::compactLatexFlag) t << "\\input"; else t << "\\include";
            t << "{" << gd->getOutputFileBase() << "}\n";
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject() || nd->countMembers()>0)
          {
            t << "}\n\\input{" << nd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
          nd=namespaceList.next();
        }
        while (nd)
        {
          if (nd->isLinkableInProject() || nd->countMembers()>0)
          {
            if (Config::compactLatexFlag) t << "\\input"; else t << "\\include";
            t << "{" << nd->getOutputFileBase() << "}\n";
          }
          nd=namespaceList.next();
        }
      }
      break;
    case isClassDocumentation:
      {
        ClassDef *cd=classList.first();
        bool found=FALSE;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            t << "}\n\\input{" << cd->getOutputFileBase() << "}\n";
            found=TRUE;
          }
          cd=classList.next();
        }
        while (cd)
        {
          if (cd->isLinkableInProject())
          {
            if (Config::compactLatexFlag) t << "\\input"; else t << "\\include";
            t << "{" << cd->getOutputFileBase() << "}\n";
          } 
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();
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
                if (Config::compactLatexFlag) t << "\\input" ; else t << "\\include";
                t << "{" << fd->getOutputFileBase() << "}\n";
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        t << "}\n";
        PageInfo *pi=exampleList.first();
        if (pi)
        {
          t << "\\input{" << convertSlashes(pi->name,TRUE) << "-example}\n";
          pi=exampleList.next();
        }
        while (pi)
        {
          if (Config::compactLatexFlag) t << "\\input" ; else t << "\\include";
          t << "{" << convertSlashes(pi->name,TRUE) << "-example}\n";
          pi=exampleList.next();
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "}\n";
        PageInfo *pi=pageList.first();
        if (pi)
        {
          QCString pageName;
          if (Config::caseSensitiveNames)
            pageName=pi->name.copy();
          else
            pageName=pi->name.lower();
          t << "\\input{" << pageName << "}\n";
          pi=pageList.next();
        }
        while (pi)
        {
          if (Config::compactLatexFlag) t << "\\input" ; else t << "\\include";
          QCString pageName;
          if (Config::caseSensitiveNames)
            pageName=pi->name.copy();
          else
            pageName=pi->name.lower();
          t << "{" << pageName << "}\n";
          pi=pageList.next();
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
        QCString pname=Config::projectName.stripWhiteSpace();
        startPlainFile("doxygen.sty");
        t << "\\NeedsTeXFormat{LaTeX2e}\n";
        t << "\\ProvidesPackage{doxygen}\n";
        t << "\\RequirePackage{calc}\n";
        t << "\\RequirePackage{array}\n";
        t << "\\pagestyle{fancyplain}\n";
        t << "\\addtolength{\\headwidth}{\\marginparsep}\n";
        t << "\\addtolength{\\headwidth}{\\marginparwidth}\n";
        t << "\\newcommand{\\clearemptydoublepage}{\\newpage{\\pagestyle{empty}";
        t << "\\cleardoublepage}}\n";
        if (!Config::compactLatexFlag) 
          t << "\\renewcommand{\\chaptermark}[1]{\\markboth{#1}{}}\n";
        t << "\\renewcommand{\\sectionmark}[1]{\\markright{\\thesection\\ #1}}\n";
        t << "\\lhead[\\fancyplain{}{\\bfseries\\thepage}]\n";
        t << "        {\\fancyplain{}{\\bfseries\\rightmark}}\n";
        t << "\\rhead[\\fancyplain{}{\\bfseries\\leftmark}]\n";
        t << "        {\\fancyplain{}{\\bfseries\\thepage}}\n";
        t << "\\rfoot[\\fancyplain{}{\\bfseries\\scriptsize ";
      }
      break;
    case 1:
    case 3:
      t << " Doxygen ";
      break;
    case 2:
      {
        t << " Dimitri van Heesch (c) 1997-2000}]{}\n";
        //QCString dtString=dateToString(FALSE);
        t << "\\lfoot[]{\\fancyplain{}{\\bfseries\\scriptsize ";
      }
      break;
    case 4:
      {
        t << " Dimitri van Heesch (c) 1997-2000}}\n";
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
        t << "  \\setlength{\\itemsep}{-4pt}\n";
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
        t << "\\setlength{\\parindent}{0cm}\n";
        t << "\\setlength{\\parskip}{0.2cm}\n";
        t << "\\addtocounter{secnumdepth}{1}\n";
        //if (!Config::compactLatexFlag)
        //{
        //  t << "\\renewcommand{\\subsection}{\\@startsection{subsection}{2}{\\z@}\n";
        //  t << "                             {-3.25ex \\@plus -1ex \\@minus -.2ex}\n";
        //  t << "                             {1.5ex \\@plus.2ex}\n";
        //  t << "                             {\\normalfont\\large\\bfseries\\medskip\n";
        //  t << "                              \\vspace{0.6cm}\\hrule\\vspace{-0.6cm}}\n";
        //  t << "                            }\n";
        //}
        //else
        //{
        //  t << "\\renewcommand{\\subsubsection}{\\@startsection{subsubsection}{3}{\\z@}\n";
        //  t << "                             {-3.25ex \\@plus -1ex \\@minus -.2ex}\n";
        //  t << "                             {1.5ex \\@plus.2ex}\n";
        //  t << "                             {\\normalfont\\normalsize\\bfseries\\medskip\n";
        //  t << "                              \\vspace{0.6cm}\\hrule\\vspace{-0.6cm}}\n";
        //  t << "                            }\n";
        //}
        t << "\\sloppy\n";
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
    t << "}{\\pageref{" << name << "}}{}" << endl;
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


void LatexGenerator::writeHtmlLink(const char *url,const char *text)
{
  if (Config::pdfHyperFlag)
  {
    t << "\\href{";
    t << url;
    t << "}";
  }
  t << "{\\tt ";
  docify(text);
  t << "}";
}

void LatexGenerator::writeMailLink(const char *url)
{
  if (Config::pdfHyperFlag)
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
  if (Config::pdfHyperFlag)
  {
    t << "\\hyperlink{";
    if (f) t << f;
    if (anchor) t << "_" << anchor; 
    t << "}{";
  }
}

void LatexGenerator::endTextLink()
{
  if (Config::pdfHyperFlag)
  {
    t << "}";
  }
}

void LatexGenerator::writeObjectLink(const char *ref, const char *f,
                                     const char *anchor, const char *text)
{
  if (!ref && Config::pdfHyperFlag)
  {
    t << "\\hyperlink{";
    if (f) t << f;
    if (anchor) t << "_" << anchor; 
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
  t << "~\\pageref{";
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
  if (Config::pdfHyperFlag && fileName)
  {
    t << "\\hypertarget{" << fileName << "}{" << endl;
  }
  if (Config::compactLatexFlag) t << "\\subsection{"; else t << "\\section{"; 
}

void LatexGenerator::endTitleHead(const char *fileName,const char *name)
{
  t << "}" << endl;
  if (name)
  {
    t << "\\label{" << name << "}\\index{" 
      << name << "@{";
    docify(name);
    t << "}}" << endl;
  }
  if (Config::pdfHyperFlag && fileName)
  {
    t << "}" << endl;
  }
}

void LatexGenerator::startTitle()
{
  if (Config::compactLatexFlag) t << "\\subsection{"; else t << "\\section{"; 
}

void LatexGenerator::startGroupHeader()
{
  if (Config::compactLatexFlag) t << "\\subsubsection{"; else t << "\\subsection{";
}

void LatexGenerator::endGroupHeader()
{
  t << "}" << endl;
}

void LatexGenerator::startMemberHeader()
{
  if (Config::compactLatexFlag) t << "\\subsubsection*{"; else t << "\\subsection*{";
}

void LatexGenerator::endMemberHeader()
{
  t << "}" << endl;
}

void LatexGenerator::startMemberDoc(const char *clname,
                                    const char *memname,
                                    const char *,
                                    const char *title)
{ 
  t << "\\index{";
  if (clname)
  {
    t << clname << "@{";
    docify(clname);
    t << "}!";
  }
  t << memname << "@{";
  docify(memname);
  t << "}}" << endl;
  
  t << "\\index{" << memname << "@{";
  docify(memname);
  t << "}";
  if (clname)
  {
    t << "!" << clname << "@{";
    docify(clname);
    t << "}"; 
  }
  t << "}" << endl;
  //
  if (Config::compactLatexFlag) t << "\\paragraph"; else t << "\\subsubsection";
  if (Config::pdfHyperFlag && title) t << "[" << filterTitle(title) << "]";
  t << "{\\setlength{\\rightskip}{0pt plus 5cm}";
}

void LatexGenerator::endMemberDoc() 
{ 
  t << "}";
  if (Config::compactLatexFlag) t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const char *fName,const char *clname,
                                     const char *anchor,const char *)
{
  t << "\\label{";
  if (clname) t << clname;
  if (anchor) t << "_" << anchor;
  t << "}" << endl;
  if (Config::pdfHyperFlag)
  {
    t << "\\hypertarget{";
    if (fName) t << fName;
    if (anchor) t << "_" << anchor;
    t << "}{" << endl;
  }
}

void LatexGenerator::endDoxyAnchor()
{
  if (Config::pdfHyperFlag)
  {
    t << "}" << endl;
  }
}


//void LatexGenerator::writeLatexLabel(const char *clName,const char *anchor)
//{
//  writeDoxyAnchor(0,clName,anchor,0);
//}

void LatexGenerator::addToIndex(const char *s1,const char *s2)
{
  if (s1)
  {
    t << "\\index{" << s1 << "@{";
    docify(s1);
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

void LatexGenerator::writeSection(const char *lab,const char *title,bool sub)
{
  t << "\\";
  if (sub) t << "subsection{"; else t << "section{";
  docify(title);
  t << "}\\label{" << lab << "}" << endl;
}

void LatexGenerator::writeSectionRef(const char *,const char *lab,
                                     const char *)
{
  t << "\\ref{" << lab << "}";
}

void LatexGenerator::writeSectionRefItem(const char *,const char *lab,
                                     const char *title)
{
  t << "\\contentsline{section}{";
  docify(title);
  t << "}{\\ref{" << lab << "}}{}" << endl;
}

void LatexGenerator::writeSectionRefAnchor(const char *,const char *lab,
                                     const char *title)
{
  startBold();
  docify(title);
  endBold();
  t << " (p.~\\pageref{" << lab << "})" << endl;
}

//void LatexGenerator::docify(const char *str)
//{
//  docifyStatic(t,str);
//}

//void LatexGenerator::docifyStatic(QTextStream &t,const char *str)
void LatexGenerator::docify(const char *str)
{
  static bool isJapanese = theTranslator->latexBabelPackage()=="a4j";
  static bool isRussian  = theTranslator->latexBabelPackage()=="russian";
  if (str)
  {
    const unsigned char *p=(const unsigned char *)str;
    unsigned char c;
    unsigned char pc='\0';
    while (*p)
    {
      c=*p++;
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
        case '[':  if (Config::pdfHyperFlag) 
                     t << "\\mbox{[}"; 
                   else 
                     t << "[";
                   break;
        case ']':  if (pc=='[') t << "$\\,$";
                   if (Config::pdfHyperFlag)
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
        
        default:   
          if (isJapanese)
          { // language is japanese
            if (c>=128) // wide character
            {
              t << (char)c;
              if (*p)  
              {
                c = *p++;
                t << (char)c;
              }
            }
            else // ascii char => see if we can insert hypenation hint
            {
              if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
              t << (char)c;    
            } 
          }
          else if (isRussian)
          {
            if (c>=128)
            {
              t << (char)c;
            }
            else
            {
              if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
              t << (char)c;
            }
          }
          else // language is other than russian or japanese
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
              case 197: t << "\\AA ";         break;
              case 198: t << "\\AE ";         break;
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
              case 223: t << "\"s ";          break; // assumes german package     
              case 224: t << "\\`{a}";        break;
              case 225: t << "\\'{a}";        break;
              case 226: t << "\\^{a}";        break;
              case 227: t << "\\~{a}";        break;
              case 228: t << "\\\"{a}";       break;
              case 229: t << "\\aa ";         break;
              case 230: t << "\\ae ";         break;
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
              case 248: t << "\\o ";          break;
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
      pc = c;
    }
  }
}

void LatexGenerator::codify(const char *str)
{
  static char spaces[]="        ";
  if (str)
  { 
    const char *p=str;
    char c;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case 0x0c: break; // remove ^L
        case '\t': t << &spaces[col&7]; col+=8-(col&7); break; 
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
  //if (Config::compactLatexFlag) t << "\\subsubsection"; else t << "\\subsection";
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

void LatexGenerator::writeNonBreakableSpace() 
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

void LatexGenerator::writeImage(const char *name,const char *w,const char *h)
{
  t << "\\mbox{\\epsfig{file=" << name;
  if (w)
    t << "," << w; 
  else if (h)
    t << "," << h;
  t << "}}" << endl;
}
