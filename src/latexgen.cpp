/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
#include "docnode.h"
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
#include "datetime.h"
#include "portable.h"
#include "outputlist.h"

static QCString g_header;
static QCString g_footer;

LatexCodeGenerator::LatexCodeGenerator(TextStream *t,const QCString &relPath,const QCString &sourceFileName)
  : m_t(t), m_relPath(relPath), m_sourceFileName(sourceFileName)
{
}

LatexCodeGenerator::LatexCodeGenerator(TextStream *t) : m_t(t)
{
}

void LatexCodeGenerator::setRelativePath(const QCString &path)
{
  m_relPath = path;
}

void LatexCodeGenerator::setSourceFileName(const QCString &name)
{
  m_sourceFileName = name;
}

void LatexCodeGenerator::codify(const QCString &str)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
    char c;
    //char cs[5];
    int spacesToNextTabStop;
    int tabSize = Config_getInt(TAB_SIZE);
    static THREAD_LOCAL char *result = NULL;
    static THREAD_LOCAL int lresult = 0;
    int i;
    while ((c=*p))
    {
      switch(c)
      {
        case 0x0c: p++;  // remove ^L
                   break;
        case ' ':  *m_t << (m_doxyCodeLineOpen ? "\\ " : " ");
                   m_col++;
                   p++;
                   break;
        case '^':  *m_t <<"\\string^";
                   m_col++;
                   p++;
                   break;
        case '`':  *m_t <<"\\`{}";
                   m_col++;
                   p++;
                   break;
        case '\t': spacesToNextTabStop =
                         tabSize - (m_col%tabSize);
                   for (i = 0; i < spacesToNextTabStop; i++) *m_t << (m_doxyCodeLineOpen ? "\\ " : " ");
                   m_col+=spacesToNextTabStop;
                   p++;
                   break;
        case '\n': *m_t << '\n';
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
                       result = static_cast<char *>(realloc(result, lresult)); \
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
                   filterLatexString(*m_t,result,
                                     m_insideTabbing, // insideTabbing
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


void LatexCodeGenerator::writeCodeLink(CodeSymbolType,
                                   const QCString &ref,const QCString &f,
                                   const QCString &anchor,const QCString &name,
                                   const QCString &)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  uint32_t l = name.length();
  if (ref.isEmpty() && usePDFLatex && pdfHyperlinks)
  {
    *m_t << "\\mbox{\\hyperlink{";
    if (!f.isEmpty()) *m_t << stripPath(f);
    if (!f.isEmpty() && !anchor.isEmpty()) *m_t << "_";
    if (!anchor.isEmpty()) *m_t << anchor;
    *m_t << "}{";
    codify(name);
    *m_t << "}}";
  }
  else
  {
    codify(name);
  }
  m_col+=l;
}

void LatexCodeGenerator::writeLineNumber(const QCString &ref,const QCString &fileName,const QCString &anchor,int l,bool writeLineAnchor)
{
  bool usePDFLatex = Config_getBool(USE_PDFLATEX);
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_doxyCodeLineOpen)
  {
    *m_t << "\\DoxyCodeLine{";
    m_doxyCodeLineOpen = TRUE;
  }
  if (Config_getBool(SOURCE_BROWSER))
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    QCString lineAnchor;
    if (!m_sourceFileName.isEmpty())
    {
      lineAnchor.sprintf("_l%05d",l);
      lineAnchor.prepend(stripExtensionGeneral(m_sourceFileName, ".tex"));
    }
    bool showTarget = usePDFLatex && pdfHyperlinks && !lineAnchor.isEmpty() && writeLineAnchor;
    if (showTarget)
    {
      *m_t << "\\Hypertarget{" << stripPath(lineAnchor) << "}";
    }
    if (!fileName.isEmpty())
    {
      writeCodeLink(CodeSymbolType::Default,ref,fileName,anchor,lineNumber,QCString());
    }
    else
    {
      codify(lineNumber);
    }
    *m_t << "\\ ";
  }
  else
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);
    codify(lineNumber);
    *m_t << "\\ ";
  }
  m_col=0;
}


void LatexCodeGenerator::startCodeLine(bool)
{
  m_col=0;
  if (!m_doxyCodeLineOpen)
  {
    *m_t << "\\DoxyCodeLine{";
    m_doxyCodeLineOpen = TRUE;
  }
}

void LatexCodeGenerator::endCodeLine()
{
  if (m_doxyCodeLineOpen)
  {
    *m_t << "}";
    m_doxyCodeLineOpen = FALSE;
  }
  codify("\n");
}

void LatexCodeGenerator::startFontClass(const QCString &name)
{
  *m_t << "\\textcolor{" << name << "}{";
}

void LatexCodeGenerator::endFontClass()
{
  *m_t << "}";
}

void LatexCodeGenerator::startCodeFragment(const QCString &style)
{
  *m_t << "\n\\begin{" << style << "}{" << m_usedTableLevel << "}\n";
}

void LatexCodeGenerator::endCodeFragment(const QCString &style)
{
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

  *m_t << "\\end{" << style << "}\n";
}


//-------------------------------

LatexGenerator::LatexGenerator()
  : OutputGenerator(Config_getString(LATEX_OUTPUT))
  , m_codeList(std::make_unique<OutputCodeList>())
{
  m_codeGen = m_codeList->add<LatexCodeGenerator>(&m_t);
}

LatexGenerator::LatexGenerator(const LatexGenerator &og) : OutputGenerator(og.m_dir)
{
  m_codeList           = std::make_unique<OutputCodeList>(*og.m_codeList);
  m_codeGen            = m_codeList->get<LatexCodeGenerator>();
  m_codeGen->setTextStream(&m_t);
  m_firstDescItem      = og.m_firstDescItem;
  m_disableLinks       = og.m_disableLinks;
  m_relPath            = og.m_relPath;
  m_indent             = og.m_indent;
  m_templateMemberItem = og.m_templateMemberItem;
}

LatexGenerator &LatexGenerator::operator=(const LatexGenerator &og)
{
  if (this!=&og)
  {
    m_dir                = og.m_dir;
    m_codeList           = std::make_unique<OutputCodeList>(*og.m_codeList);
    m_codeGen            = m_codeList->get<LatexCodeGenerator>();
    m_codeGen->setTextStream(&m_t);
    m_firstDescItem      = og.m_firstDescItem;
    m_disableLinks       = og.m_disableLinks;
    m_relPath            = og.m_relPath;
    m_indent             = og.m_indent;
    m_templateMemberItem = og.m_templateMemberItem;
  }
  return *this;
}

LatexGenerator::LatexGenerator(LatexGenerator &&og)
  : OutputGenerator(std::move(og))
{
  m_codeList           = std::exchange(og.m_codeList,std::unique_ptr<OutputCodeList>());
  m_codeGen            = m_codeList->get<LatexCodeGenerator>();
  m_codeGen->setTextStream(&m_t);
  m_firstDescItem      = std::exchange(og.m_firstDescItem,true);
  m_disableLinks       = std::exchange(og.m_disableLinks,false);
  m_relPath            = std::exchange(og.m_relPath,QCString());
  m_indent             = std::exchange(og.m_indent,0);
  m_templateMemberItem = std::exchange(og.m_templateMemberItem,false);
}

LatexGenerator::~LatexGenerator()
{
}

void LatexGenerator::addCodeGen(OutputCodeList &list)
{
  list.add(OutputCodeList::OutputCodeVariant(LatexCodeGeneratorDefer(m_codeGen)));
}

static void writeLatexMakefile()
{
  bool generateBib = !CitationManager::instance().isEmpty();
  QCString fileName=Config_getString(LATEX_OUTPUT)+"/Makefile";
  std::ofstream f = Portable::openOutputStream(fileName);
  if (!f.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(fileName));
  }
  TextStream t(&f);
  // inserted by KONNO Akihisa <konno@researchers.jp> 2002-03-05
  QCString latex_command = theTranslator->latexCommandName().quoted();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME).quoted();
  QCString bibtex_command = "bibtex";
  QCString manual_file = "refman";
  const int latex_count = 8;
  // end insertion by KONNO Akihisa <konno@researchers.jp> 2002-03-05
    t << "LATEX_CMD?=" << latex_command << "\n"
      << "MKIDX_CMD?=" << mkidx_command << "\n"
      << "BIBTEX_CMD?=" << bibtex_command << "\n"
      << "LATEX_COUNT?=" << latex_count << "\n"
      << "MANUAL_FILE?=" << manual_file << "\n"
      << "\n";
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "all: $(MANUAL_FILE).dvi\n"
      << "\n"
      << "ps: $(MANUAL_FILE).ps\n"
      << "\n"
      << "pdf: $(MANUAL_FILE).pdf\n"
      << "\n"
      << "ps_2on1: $(MANUAL_FILE).ps\n"
      << "\n"
      << "pdf_2on1: $(MANUAL_FILE).pdf\n"
      << "\n"
      << "$(MANUAL_FILE).ps: $(MANUAL_FILE).dvi\n"
      << "\tdvips -o $(MANUAL_FILE).ps $(MANUAL_FILE).dvi\n"
      << "\n";
    t << "$(MANUAL_FILE).pdf: $(MANUAL_FILE).ps\n";
    t << "\tps2pdf $(MANUAL_FILE).ps $(MANUAL_FILE).pdf\n\n";
    t << "$(MANUAL_FILE).dvi: clean $(MANUAL_FILE).tex doxygen.sty\n"
      << "\techo \"Running latex...\"\n"
      << "\t$(LATEX_CMD) $(MANUAL_FILE).tex\n"
      << "\techo \"Running makeindex...\"\n"
      << "\t$(MKIDX_CMD) $(MANUAL_FILE).idx\n";
    if (generateBib)
    {
      t << "\techo \"Running bibtex...\"\n";
      t << "\t$(BIBTEX_CMD) $(MANUAL_FILE)\n";
      t << "\techo \"Rerunning latex....\"\n";
      t << "\t$(LATEX_CMD) $(MANUAL_FILE).tex\n";
    }
    t << "\techo \"Rerunning latex....\"\n"
      << "\t$(LATEX_CMD) $(MANUAL_FILE).tex\n"
      << "\tlatex_count=$(LATEX_COUNT) ; \\\n"
      << "\twhile grep -E -s 'Rerun (LaTeX|to get cross-references right|to get bibliographical references right)' $(MANUAL_FILE).log && [ $$latex_count -gt 0 ] ;\\\n"
      << "\t    do \\\n"
      << "\t      echo \"Rerunning latex....\" ;\\\n"
      << "\t      $(LATEX_CMD) $(MANUAL_FILE).tex ; \\\n"
      << "\t      latex_count=`expr $$latex_count - 1` ;\\\n"
      << "\t    done\n"
      << "\t$(MKIDX_CMD) $(MANUAL_FILE).idx\n"
      << "\t$(LATEX_CMD) $(MANUAL_FILE).tex\n\n"
      << "$(MANUAL_FILE).ps: $(MANUAL_FILE).ps\n"
      << "\tpsnup -2 $(MANUAL_FILE).ps >$(MANUAL_FILE).ps\n"
      << "\n"
      << "$(MANUAL_FILE).pdf: $(MANUAL_FILE).ps\n"
      << "\tps2pdf $(MANUAL_FILE).ps $(MANUAL_FILE).pdf\n";
  }
  else // use pdflatex for higher quality output
  {
    t << "all: $(MANUAL_FILE).pdf\n\n"
      << "pdf: $(MANUAL_FILE).pdf\n\n";
    t << "$(MANUAL_FILE).pdf: clean $(MANUAL_FILE).tex\n";
    t << "\t$(LATEX_CMD) $(MANUAL_FILE)\n";
    t << "\t$(MKIDX_CMD) $(MANUAL_FILE).idx\n";
    if (generateBib)
    {
      t << "\t$(BIBTEX_CMD) $(MANUAL_FILE)\n";
      t << "\t$(LATEX_CMD) $(MANUAL_FILE)\n";
    }
    t << "\t$(LATEX_CMD) $(MANUAL_FILE)\n"
      << "\tlatex_count=$(LATEX_COUNT) ; \\\n"
      << "\twhile grep -E -s 'Rerun (LaTeX|to get cross-references right|to get bibliographical references right)' $(MANUAL_FILE).log && [ $$latex_count -gt 0 ] ;\\\n"
      << "\t    do \\\n"
      << "\t      echo \"Rerunning latex....\" ;\\\n"
      << "\t      $(LATEX_CMD) $(MANUAL_FILE) ;\\\n"
      << "\t      latex_count=`expr $$latex_count - 1` ;\\\n"
      << "\t    done\n"
      << "\t$(MKIDX_CMD) $(MANUAL_FILE).idx\n"
      << "\t$(LATEX_CMD) $(MANUAL_FILE)\n\n";
  }

  t << "\n"
    << "clean:\n"
    << "\trm -f "
    << "*.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl $(MANUAL_FILE).pdf\n";
}

static void writeMakeBat()
{
#if defined(_MSC_VER)
  QCString dir=Config_getString(LATEX_OUTPUT);
  QCString fileName=dir+"/make.bat";
  QCString latex_command = theTranslator->latexCommandName().quoted();
  QCString mkidx_command = Config_getString(MAKEINDEX_CMD_NAME).quoted();
  QCString bibtex_command = "bibtex";
  QCString manual_file = "refman";
  const int latex_count = 8;
  bool generateBib = !CitationManager::instance().isEmpty();
  std::ofstream t = Portable::openOutputStream(fileName);
  if (!t.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(fileName));
  }
  t << "pushd %~dp0\r\n";
  t << "if not %errorlevel% == 0 goto :end\r\n";
  t << "\r\n";
  t << "set ORG_LATEX_CMD=%LATEX_CMD%\r\n";
  t << "set ORG_MKIDX_CMD=%MKIDX_CMD%\r\n";
  t << "set ORG_BIBTEX_CMD=%BIBTEX_CMD%\r\n";
  t << "set ORG_LATEX_COUNT=%LATEX_COUNT%\r\n";
  t << "set ORG_MANUAL_FILE=%MANUAL_FILE%\r\n";
  t << "if \"X\"%LATEX_CMD% == \"X\" set LATEX_CMD=" << latex_command << "\r\n";
  t << "if \"X\"%MKIDX_CMD% == \"X\" set MKIDX_CMD=" << mkidx_command << "\r\n";
  t << "if \"X\"%BIBTEX_CMD% == \"X\" set BIBTEX_CMD=" << bibtex_command << "\r\n";
  t << "if \"X\"%LATEX_COUNT% == \"X\" set LATEX_COUNT=" << latex_count << "\r\n";
  t << "if \"X\"%MANUAL_FILE% == \"X\" set MANUAL_FILE=" << manual_file << "\r\n";
  t << "\r\n";
  t << "del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl %MANUAL_FILE%.pdf\r\n\r\n";
  t << "\r\n";
  if (!Config_getBool(USE_PDFLATEX)) // use plain old latex
  {
    t << "%LATEX_CMD% %MANUAL_FILE%.tex\r\n";
    t << "echo ----\r\n";
    t << "%MKIDX_CMD% %MANUAL_FILE%.idx\r\n";
    if (generateBib)
    {
      t << "%BIBTEX_CMD% %MANUAL_FILE%\r\n";
      t << "echo ----\r\n";
      t << "\t%LATEX_CMD% %MANUAL_FILE%.tex\r\n";
    }
    t << "setlocal enabledelayedexpansion\r\n";
    t << "set count=%LAT#EX_COUNT%\r\n";
    t << ":repeat\r\n";
    t << "set content=X\r\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get bibliographical references right\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X goto :skip\r\n";
    t << "set /a count-=1\r\n";
    t << "if !count! EQU 0 goto :skip\r\n\r\n";
    t << "echo ----\r\n";
    t << "%LATEX_CMD% %MANUAL_FILE%.tex\r\n";
    t << "goto :repeat\r\n";
    t << ":skip\r\n";
    t << "endlocal\r\n";
    t << "%MKIDX_CMD% %MANUAL_FILE%.idx\r\n";
    t << "%LATEX_CMD% %MANUAL_FILE%.tex\r\n";
    t << "dvips -o %MANUAL_FILE%.ps %MANUAL_FILE%.dvi\r\n";
    t << Portable::ghostScriptCommand();
    t << " -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite "
         "-sOutputFile=%MANUAL_FILE%.pdf -c save pop -f %MANUAL_FILE%.ps\r\n";
  }
  else // use pdflatex
  {
    t << "%LATEX_CMD% %MANUAL_FILE%\r\n";
    t << "echo ----\r\n";
    t << "%MKIDX_CMD% %MANUAL_FILE%.idx\r\n";
    if (generateBib)
    {
      t << "%BIBTEX_CMD% %MANUAL_FILE%\r\n";
      t << "%LATEX_CMD% %MANUAL_FILE%\r\n";
    }
    t << "echo ----\r\n";
    t << "%LATEX_CMD% %MANUAL_FILE%\r\n\r\n";
    t << "setlocal enabledelayedexpansion\r\n";
    t << "set count=%LATEX_COUNT%\r\n";
    t << ":repeat\r\n";
    t << "set content=X\r\n";
    t << "for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun LaTeX\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get cross-references right\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X for /F \"tokens=*\" %%T in ( 'findstr /C:\"Rerun to get bibliographical references right\" %MANUAL_FILE%.log' ) do set content=\"%%~T\"\r\n";
    t << "if !content! == X goto :skip\r\n";
    t << "set /a count-=1\r\n";
    t << "if !count! EQU 0 goto :skip\r\n\r\n";
    t << "echo ----\r\n";
    t << "%LATEX_CMD% %MANUAL_FILE%\r\n";
    t << "goto :repeat\r\n";
    t << ":skip\r\n";
    t << "endlocal\r\n";
    t << "%MKIDX_CMD% %MANUAL_FILE%.idx\r\n";
    t << "%LATEX_CMD% %MANUAL_FILE%\r\n";
  }
  t<< "\r\n";
  t<< "@REM reset environment\r\n";
  t<< "popd\r\n";
  t<< "set LATEX_CMD=%ORG_LATEX_CMD%\r\n";
  t<< "set ORG_LATEX_CMD=\r\n";
  t<< "set MKIDX_CMD=%ORG_MKIDX_CMD%\r\n";
  t<< "set ORG_MKIDX_CMD=\r\n";
  t<< "set BIBTEX_CMD=%ORG_BIBTEX_CMD%\r\n";
  t<< "set ORG_BIBTEX_CMD=\r\n";
  t<< "set MANUAL_FILE=%ORG_MANUAL_FILE%\r\n";
  t<< "set ORG_MANUAL_FILE=\r\n";
  t<< "set LATEX_COUNT=%ORG_LATEX_COUNT%\r\n";
  t<< "set ORG_LATEX_COUNT=\r\n";
  t<< "\r\n";
  t<< ":end\r\n";
#endif
}

void LatexGenerator::init()
{
  QCString dname = Config_getString(LATEX_OUTPUT);
  Dir d(dname.str());
  if (!d.exists() && !d.mkdir(dname.str()))
  {
    term("Could not create output directory %s\n",qPrint(dname));
  }

  if (!Config_getString(LATEX_HEADER).isEmpty())
  {
    g_header=fileToString(Config_getString(LATEX_HEADER));
    //printf("g_header='%s'\n",qPrint(g_header));
  }
  else
  {
    g_header = ResourceMgr::instance().getAsString("header.tex");
  }
  if (!Config_getString(LATEX_FOOTER).isEmpty())
  {
    g_footer=fileToString(Config_getString(LATEX_FOOTER));
    //printf("g_footer='%s'\n",qPrint(g_footer));
  }
  else
  {
    g_footer = ResourceMgr::instance().getAsString("footer.tex");
  }

  writeLatexMakefile();
  writeMakeBat();

  createSubDirs(d);
}

void LatexGenerator::cleanup()
{
  QCString dname = Config_getString(LATEX_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}

static void writeDefaultStyleSheet(TextStream &t)
{
  t << ResourceMgr::instance().getAsString("doxygen.sty");
}

void LatexGenerator::writeHeaderFile(TextStream &t)
{
  t << "% Latex header for doxygen " << getDoxygenVersion() << "\n";
  t << ResourceMgr::instance().getAsString("header.tex");
}

void LatexGenerator::writeFooterFile(TextStream &t)
{
  t << "% Latex footer for doxygen " << getDoxygenVersion() << "\n";
  t << ResourceMgr::instance().getAsString("footer.tex");
}

void LatexGenerator::writeStyleSheetFile(TextStream &t)
{
  t << "% stylesheet for doxygen " << getDoxygenVersion() << "\n";
  writeDefaultStyleSheet(t);
}

void LatexGenerator::startFile(const QCString &name,const QCString &,const QCString &,int)
{
#if 0
  setEncoding(Config_getString(LATEX_OUTPUT_ENCODING));
#endif
  QCString fileName=name;
  m_relPath = relativePathToRoot(fileName);
  if (!fileName.endsWith(".tex") && !fileName.endsWith(".sty")) fileName+=".tex";
  startPlainFile(fileName);
  m_codeGen->setRelativePath(m_relPath);
  m_codeGen->setSourceFileName(stripPath(fileName));
}

void LatexGenerator::endFile()
{
  endPlainFile();
  m_codeGen->setSourceFileName("");
}

//void LatexGenerator::writeIndex()
//{
//  startFile("refman.tex");
//}

void LatexGenerator::startProjectNumber()
{
  m_t << "\\\\[1ex]\\large ";
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
      result += "\\"+latex_mkidx_command;
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
  QCString paperType = Config_getEnumAsString(PAPER_TYPE);

  QCString style = Config_getString(LATEX_BIB_STYLE);
  if (style.isEmpty())
  {
    style="plain";
  }

  TextStream tg;
  bool timeStamp = Config_getBool(LATEX_TIMESTAMP);
  QCString generatedBy;
  if (timeStamp)
  {
    generatedBy = theTranslator->trGeneratedAt(dateToString(DateTimeType::DateTime).data(),
                                               Config_getString(PROJECT_NAME).data());
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

  TextStream tg1;
  writeExtraLatexPackages(tg1);
  QCString extraLatexPackages = tg1.str();

  TextStream tg2;
  writeLatexSpecialFormulaChars(tg2);
  QCString latexSpecialFormulaChars = tg2.str();

  QCString formulaMacrofile = Config_getString(FORMULA_MACROFILE);
  QCString stripMacroFile;
  if (!formulaMacrofile.isEmpty())
  {
    FileInfo fi(formulaMacrofile.str());
    formulaMacrofile=fi.absFilePath();
    stripMacroFile = fi.fileName();
    copyFile(formulaMacrofile,Config_getString(LATEX_OUTPUT) + "/" + stripMacroFile);
  }

  QCString projectNumber = Config_getString(PROJECT_NUMBER);

  // first substitute generic keywords
  QCString result = substituteKeywords(str,title,
        convertToLaTeX(Config_getString(PROJECT_NAME),false),
        convertToLaTeX(projectNumber,false),
        convertToLaTeX(Config_getString(PROJECT_BRIEF),false));

  // additional LaTeX only keywords
  result = substituteKeywords(result,
  {
    // keyword                     value getter
    { "$latexdocumentpre",         [&]() { return theTranslator->latexDocumentPre();            } },
    { "$latexdocumentpost",        [&]() { return theTranslator->latexDocumentPost();           } },
    { "$generatedby",              [&]() { return generatedBy;                                  } },
    { "$latexbibstyle",            [&]() { return style;                                        } },
    { "$latexcitereference",       [&]() { return theTranslator->trCiteReferences();            } },
    { "$latexbibfiles",            [&]() { return CitationManager::instance().latexBibFiles();  } },
    { "$papertype",                [&]() { return paperType+"paper";                            } },
    { "$extralatexstylesheet",     [&]() { return extraLatexStyleSheet();                       } },
    { "$languagesupport",          [&]() { return theTranslator->latexLanguageSupportCommand(); } },
    { "$latexfontenc",             [&]() { return latexFontenc;                                 } },
    { "$latexfont",                [&]() { return theTranslator->latexFont();                   } },
    { "$latexemojidirectory",      [&]() { return latexEmojiDirectory;                          } },
    { "$makeindex",                [&]() { return makeIndex();                                  } },
    { "$extralatexpackages",       [&]() { return extraLatexPackages;                           } },
    { "$latexspecialformulachars", [&]() { return latexSpecialFormulaChars;                     } },
    { "$formulamacrofile",         [&]() { return stripMacroFile;                               } }
  });

  static const SelectionMarkerInfo latexMarkerInfo = { '%', "%%BEGIN ",8 ,"%%END ",6, "",0 };

  // remove conditional blocks
  result = selectBlocks(result,
  {
    // marker              is enabled
    { "CITATIONS_PRESENT", !CitationManager::instance().isEmpty() },
    { "COMPACT_LATEX",     compactLatex                           },
    { "PDF_HYPERLINKS",    pdfHyperlinks                          },
    { "USE_PDFLATEX",      usePdfLatex                            },
    { "LATEX_TIMESTAMP",   timeStamp                              },
    { "LATEX_BATCHMODE",   latexBatchmode                         },
    { "LATEX_FONTENC",     !latexFontenc.isEmpty()                },
    { "FORMULA_MACROFILE", !formulaMacrofile.isEmpty()            },
    { "PROJECT_NUMBER",    !projectNumber.isEmpty()               }
  },latexMarkerInfo);

  result = removeEmptyLines(result);

  return result;
}

void LatexGenerator::startIndexSection(IndexSection is)
{
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  switch (is)
  {
    case IndexSection::isTitlePageStart:
      m_t << substituteLatexKeywords(g_header,convertToLaTeX(Config_getString(PROJECT_NAME),m_codeGen->insideTabbing()));
      break;
    case IndexSection::isTitlePageAuthor:
      break;
    case IndexSection::isMainPage:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Introduction}\n"
      break;
    case IndexSection::isModuleIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Module Index}\n"
      break;
    case IndexSection::isDirIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Directory Index}\n"
      break;
    case IndexSection::isNamespaceIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Namespace Index}\n"
      break;
    case IndexSection::isConceptIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Concept Index}\n"
      break;
    case IndexSection::isClassHierarchyIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Hierarchical Index}\n"
      break;
    case IndexSection::isCompoundIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Annotated Compound Index}\n"
      break;
    case IndexSection::isFileIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Annotated File Index}\n"
      break;
    case IndexSection::isPageIndex:
      if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
      m_t << "{"; //Annotated Page Index}\n"
      break;
    case IndexSection::isModuleDocumentation:
      {
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
            m_t << "{"; //Module Documentation}\n";
            break;
          }
        }
      }
      break;
    case IndexSection::isDirDocumentation:
      {
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
            m_t << "{"; //Module Documentation}\n";
            break;
          }
        }
      }
      break;
    case IndexSection::isNamespaceDocumentation:
      {
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
            m_t << "{"; // Namespace Documentation}\n":
            break;
          }
        }
      }
      break;
    case IndexSection::isConceptDocumentation:
      {
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject() && !cd->isAlias())
          {
            if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
            m_t << "{"; // Concept Documentation}\n":
            break;
          }
        }
      }
      break;
    case IndexSection::isClassDocumentation:
      {
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
              !cd->isEmbeddedInOuterScope() &&
              !cd->isAlias()
             )
          {
            if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
            m_t << "{"; //Compound Documentation}\n";
            break;
          }
        }
      }
      break;
    case IndexSection::isFileDocumentation:
      {
        bool isFirst=TRUE;
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject() || fd->generateSourceFile())
            {
              if (isFirst)
              {
                if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
                m_t << "{"; //File Documentation}\n";
                isFirst=FALSE;
                break;
              }
            }
          }
        }
      }
      break;
    case IndexSection::isExampleDocumentation:
      {
        if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
        m_t << "{"; //Example Documentation}\n";
      }
      break;
    case IndexSection::isPageDocumentation:
      {
        if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
        m_t << "{"; //Page Documentation}\n";
      }
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      break;
  }
}

void LatexGenerator::endIndexSection(IndexSection is)
{
  switch (is)
  {
    case IndexSection::isTitlePageStart:
      break;
    case IndexSection::isTitlePageAuthor:
      break;
    case IndexSection::isMainPage:
      {
        //QCString indexName=Config_getBool(GENERATE_TREEVIEW)?"main":"index";
        QCString indexName="index";
        m_t << "}\n\\label{index}";
        if (Config_getBool(PDF_HYPERLINKS)) m_t << "\\hypertarget{index}{}";
        m_t << "\\input{" << indexName << "}\n";
      }
      break;
    case IndexSection::isModuleIndex:
      m_t << "}\n\\input{modules}\n";
      break;
    case IndexSection::isDirIndex:
      m_t << "}\n\\input{dirs}\n";
      break;
    case IndexSection::isNamespaceIndex:
      m_t << "}\n\\input{namespaces}\n";
      break;
    case IndexSection::isConceptIndex:
      m_t << "}\n\\input{concepts}\n";
      break;
    case IndexSection::isClassHierarchyIndex:
      m_t << "}\n\\input{hierarchy}\n";
      break;
    case IndexSection::isCompoundIndex:
      m_t << "}\n\\input{annotated}\n";
      break;
    case IndexSection::isFileIndex:
      m_t << "}\n\\input{files}\n";
      break;
    case IndexSection::isPageIndex:
      m_t << "}\n\\input{pages}\n";
      break;
    case IndexSection::isModuleDocumentation:
      {
        bool found=FALSE;
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            if (!found)
            {
              m_t << "}\n";
              found=TRUE;
            }
            m_t << "\\input{" << gd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case IndexSection::isDirDocumentation:
      {
        bool found=FALSE;
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            if (!found)
            {
              m_t << "}\n";
              found = TRUE;
            }
            m_t << "\\input{" << dd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case IndexSection::isNamespaceDocumentation:
      {
        bool found=FALSE;
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            if (!found)
            {
              m_t << "}\n";
              found=true;
            }
            m_t << "\\input{" << nd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case IndexSection::isConceptDocumentation:
      {
        bool found=FALSE;
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject() && !cd->isAlias())
          {
            if (!found)
            {
              m_t << "}\n";
              found=true;
            }
            m_t << "\\input{" << cd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case IndexSection::isClassDocumentation:
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
              m_t << "}\n"; // end doxysection or chapter title
              found=TRUE;
            }
            m_t << "\\input{" << cd->getOutputFileBase() << "}\n";
          }
        }
      }
      break;
    case IndexSection::isFileDocumentation:
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
                m_t << "}\n"; // end doxysection or chapter title
              }
              isFirst=FALSE;
              m_t << "\\input{" << fd->getOutputFileBase() << "}\n";
            }
            if (fd->generateSourceFile())
            {
              if (isFirst)
              {
                m_t << "}\n"; // end doxysection or chapter title
              }
              isFirst=FALSE;
              m_t << "\\input{" << fd->getSourceFileBase() << "}\n";
            }
          }
        }
      }
      break;
    case IndexSection::isExampleDocumentation:
      {
        m_t << "}\n";
        for (const auto &pd : *Doxygen::exampleLinkedMap)
        {
          m_t << "\\input{" << pd->getOutputFileBase() << "}\n";
        }
      }
      break;
    case IndexSection::isPageDocumentation:
      {
        m_t << "}\n";
#if 0
        bool first=TRUE;
        for (const auto *pd : Doxygen::pageLinkedMap)
        {
          if (!pd->getGroupDef() && !pd->isReference())
          {
             if (compactLatex) m_t << "\\doxysection"; else m_t << "\\chapter";
             m_t << "{" << pd->title();
             m_t << "}\n";

            if (compactLatex || first) m_t << "\\input" ; else m_t << "\\include";
            m_t << "{" << pd->getOutputFileBase() << "}\n";
            first=FALSE;
          }
        }
#endif
      }
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      m_t << substituteLatexKeywords(g_footer,convertToLaTeX(Config_getString(PROJECT_NAME),m_codeGen->insideTabbing()));
      break;
  }
}

void LatexGenerator::writePageLink(const QCString &name, bool /*first*/)
{
  //bool &compactLatex = Config_getBool(COMPACT_LATEX);
  // next is remove for bug615957
  //if (compactLatex || first) m_t << "\\input" ; else m_t << "\\include";
  m_t << "\\input" ;
  m_t << "{" << name << "}\n";
}


void LatexGenerator::writeStyleInfo(int part)
{
  if (part > 0)
    return;

  startPlainFile("doxygen.sty");
  writeDefaultStyleSheet(m_t);
  endPlainFile();

  // workaround for the problem caused by change in LaTeX in version 2019
  // in the unmaintained tabu package
  startPlainFile("tabu_doxygen.sty");
  m_t << ResourceMgr::instance().getAsString("tabu_doxygen.sty");
  endPlainFile();
  startPlainFile("longtable_doxygen.sty");
  m_t << ResourceMgr::instance().getAsString("longtable_doxygen.sty");
  endPlainFile();
}

void LatexGenerator::startParagraph(const QCString &)
{
  m_t << "\n" << "\n";
}

void LatexGenerator::endParagraph()
{
  m_t << "\n" << "\n";
}

void LatexGenerator::writeString(const QCString &text)
{
  m_t << text;
}

void LatexGenerator::startIndexItem(const QCString &ref,const QCString &fn)
{
  m_t << "\\item ";
  if (ref.isEmpty() && !fn.isEmpty())
  {
    m_t << "\\contentsline{section}{";
  }
}

void LatexGenerator::endIndexItem(const QCString &ref,const QCString &fn)
{
  if (ref.isEmpty() && !fn.isEmpty())
  {
    m_t << "}{\\pageref{" << stripPath(fn) << "}}{}\n";
  }
}

void LatexGenerator::writeStartAnnoItem(const QCString &,const QCString &,
                                        const QCString &path,const QCString &name)
{
  m_t << "\\item\\contentsline{section}\\textbf{ ";
  if (!path.isEmpty()) docify(path);
  docify(name);
  m_t << "} ";
}

void LatexGenerator::startIndexKey()
{
  m_t << "\\item\\contentsline{section}{";
}

void LatexGenerator::endIndexKey()
{
}

void LatexGenerator::startIndexValue(bool hasBrief)
{
  m_t << " ";
  if (hasBrief) m_t << "\\\\*";
}

void LatexGenerator::endIndexValue(const QCString &name,bool /*hasBrief*/)
{
  //if (hasBrief) m_t << ")";
  m_t << "}{\\pageref{" << stripPath(name) << "}}{}\n";
}

//void LatexGenerator::writeClassLink(const QCString &,const QCString &,
//                                    const QCString &,const QCString &name)
//{
//  m_t << "\\textbf{ ";
//  docify(name);
//  m_t << "}";
//}

void LatexGenerator::startTextLink(const QCString &f,const QCString &anchor)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_disableLinks && pdfHyperlinks)
  {
    m_t << "\\mbox{\\hyperlink{";
    if (!f.isEmpty()) m_t << stripPath(f);
    if (!anchor.isEmpty()) m_t << "_" << anchor;
    m_t << "}{";
  }
  else
  {
    m_t << "\\textbf{ ";
  }
}

void LatexGenerator::endTextLink()
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (!m_disableLinks && pdfHyperlinks)
  {
    m_t << "}";
  }
  m_t << "}";
}

static QCString objectLinkToString(const QCString &ref, const QCString &f,
                                    const QCString &anchor, const QCString &text,
                                    bool insideTabbing,bool disableLinks)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  QCString result;
  if (!disableLinks && ref.isEmpty() && pdfHyperlinks)
  {
    result += "\\mbox{\\hyperlink{";
    if (!f.isEmpty()) result += stripPath(f);
    if (!f.isEmpty() && !anchor.isEmpty()) result += "_";
    if (!anchor.isEmpty()) result += anchor;
    result += "}{";
    result += convertToLaTeX(text,insideTabbing);
    result += "}}";
  }
  else
  {
    result += "\\textbf{ ";
    result += convertToLaTeX(text,insideTabbing);
    result += "}";
  }
  return result;
}

void LatexGenerator::writeObjectLink(const QCString &ref, const QCString &f,
                                     const QCString &anchor, const QCString &text)
{
  m_t << objectLinkToString(ref,f,anchor,text,m_codeGen->insideTabbing(),m_disableLinks);
}

void LatexGenerator::startPageRef()
{
  m_t << " \\doxyref{}{";
}

void LatexGenerator::endPageRef(const QCString &clname, const QCString &anchor)
{
  m_t << "}{";
  if (!clname.isEmpty()) m_t << clname;
  if (!anchor.isEmpty()) m_t << "_" << anchor;
  m_t << "}";
}


void LatexGenerator::startTitleHead(const QCString &fileName)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks && !fileName.isEmpty())
  {
    m_t << "\\hypertarget{" << stripPath(fileName) << "}{}";
  }
  if (Config_getBool(COMPACT_LATEX))
  {
    m_t << "\\doxysubsection{";
  }
  else
  {
    m_t << "\\doxysection{";
  }
}

void LatexGenerator::endTitleHead(const QCString &fileName,const QCString &name)
{
  m_t << "}\n";
  if (!name.isEmpty())
  {
    m_t << "\\label{" << stripPath(fileName) << "}\\index{";
    m_t << latexEscapeLabelName(name);
    m_t << "@{";
    m_t << latexEscapeIndexChars(name);
    m_t << "}}\n";
  }
}

void LatexGenerator::startTitle()
{
  if (Config_getBool(COMPACT_LATEX))
  {
    m_t << "\\doxysubsection{";
  }
  else
  {
    m_t << "\\doxysection{";
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
    m_t << "\\doxysubparagraph*{";
  }
  else if (extraIndentLevel==2)
  {
    m_t << "\\doxyparagraph{";
  }
  else if (extraIndentLevel==1)
  {
    m_t << "\\doxysubsubsection{";
  }
  else // extraIndentLevel==0
  {
    m_t << "\\doxysubsection{";
  }
  m_disableLinks=TRUE;
}

void LatexGenerator::endGroupHeader(int)
{
  m_disableLinks=FALSE;
  m_t << "}\n";
}

void LatexGenerator::startMemberHeader(const QCString &,int)
{
  if (Config_getBool(COMPACT_LATEX))
  {
    m_t << "\\doxysubsubsection*{";
  }
  else
  {
    m_t << "\\doxysubsection*{";
  }
  m_disableLinks=TRUE;
}

void LatexGenerator::endMemberHeader()
{
  m_disableLinks=FALSE;
  m_t << "}\n";
}

void LatexGenerator::startMemberDoc(const QCString &clname,
                                    const QCString &memname,
                                    const QCString &,
                                    const QCString &title,
                                    int memCount,
                                    int memTotal,
                                    bool showInline)
{
  if (!memname.isEmpty() && memname[0]!='@')
  {
    m_t << "\\index{";
    if (!clname.isEmpty())
    {
      m_t << latexEscapeLabelName(clname);
      m_t << "@{";
      m_t << latexEscapeIndexChars(clname);
      m_t << "}!";
    }
    m_t << latexEscapeLabelName(memname);
    m_t << "@{";
    m_t << latexEscapeIndexChars(memname);
    m_t << "}}\n";

    m_t << "\\index{";
    m_t << latexEscapeLabelName(memname);
    m_t << "@{";
    m_t << latexEscapeIndexChars(memname);
    m_t << "}";
    if (!clname.isEmpty())
    {
      m_t << "!";
      m_t << latexEscapeLabelName(clname);
      m_t << "@{";
      m_t << latexEscapeIndexChars(clname);
      m_t << "}";
    }
    m_t << "}\n";
  }
  static const char *levelLab[] = { "doxysubsubsection","doxyparagraph","doxysubparagraph", "doxysubparagraph" };
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  int level=0;
  if (showInline) level+=2;
  if (compactLatex) level++;
  m_t << "\\" << levelLab[level];

  m_t << "{";
  if (pdfHyperlinks)
  {
    m_t << "\\texorpdfstring{";
  }
  m_t << latexEscapeIndexChars(title);
  if (pdfHyperlinks)
  {
    m_t << "}{" << latexEscapePDFString(title) << "}";
  }
  if (memTotal>1)
  {
    m_t << "\\hspace{0.1cm}{\\footnotesize\\ttfamily [" << memCount << "/" << memTotal << "]}";
  }
  m_t << "}";
  m_t << "\n{\\footnotesize\\ttfamily ";
  //m_disableLinks=TRUE;
}

void LatexGenerator::endMemberDoc(bool)
{
  m_disableLinks=FALSE;
  m_t << "}\n\n";
  //if (Config_getBool(COMPACT_LATEX)) m_t << "\\hfill";
}

void LatexGenerator::startDoxyAnchor(const QCString &fName,const QCString &,
                                     const QCString &anchor, const QCString &,
                                     const QCString &)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (m_insideTableEnv) m_t << "\\mbox{"; // see issue #6093
  if (usePDFLatex && pdfHyperlinks)
  {
    m_t << "\\Hypertarget{";
    if (!fName.isEmpty()) m_t << stripPath(fName);
    if (!anchor.isEmpty()) m_t << "_" << anchor;
    m_t << "}";
  }
  m_t << "\\label{";
  if (!fName.isEmpty()) m_t << stripPath(fName);
  if (!anchor.isEmpty()) m_t << "_" << anchor;
  if (m_insideTableEnv) m_t << "}";
  m_t << "} \n";
}

void LatexGenerator::endDoxyAnchor(const QCString &/* fName */,const QCString &/* anchor */)
{
}

void LatexGenerator::writeAnchor(const QCString &fName,const QCString &name)
{
  //printf("LatexGenerator::writeAnchor(%s,%s)\n",fName,name);
  m_t << "\\label{" << stripPath(name) << "}\n";
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks)
  {
    if (!fName.isEmpty())
    {
      m_t << "\\Hypertarget{" << stripPath(fName) << "_" << stripPath(name) << "}\n";
    }
    else
    {
      m_t << "\\Hypertarget{" << stripPath(name) << "}\n";
    }
  }
}


//void LatexGenerator::writeLatexLabel(const QCString &clName,const QCString &anchor)
//{
//  writeDoxyAnchor(0,clName,anchor,0);
//}

void LatexGenerator::addIndexItem(const QCString &s1,const QCString &s2)
{
  if (!s1.isEmpty())
  {
    m_t << "\\index{";
    m_t << latexEscapeLabelName(s1);
    m_t << "@{";
    m_t << latexEscapeIndexChars(s1);
    m_t << "}";
    if (!s2.isEmpty())
    {
      m_t << "!";
      m_t << latexEscapeLabelName(s2);
      m_t << "@{";
      m_t << latexEscapeIndexChars(s2);
      m_t << "}";
    }
    m_t << "}";
  }
}


void LatexGenerator::startSection(const QCString &lab,const QCString &,SectionType type)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);
  if (usePDFLatex && pdfHyperlinks)
  {
    m_t << "\\hypertarget{" << stripPath(lab) << "}{}";
  }
  m_t << "\\";
  if (Config_getBool(COMPACT_LATEX))
  {
    switch(type)
    {
      case SectionType::Page:          m_t << "doxysubsection"; break;
      case SectionType::Section:       m_t << "doxysubsubsection"; break;
      case SectionType::Subsection:    m_t << "doxyparagraph"; break;
      case SectionType::Subsubsection: m_t << "doxysubparagraph"; break;
      case SectionType::Paragraph:     m_t << "doxysubparagraph"; break;
      default: ASSERT(0); break;
    }
    m_t << "{";
  }
  else
  {
    switch(type)
    {
      case SectionType::Page:          m_t << "doxysection"; break;
      case SectionType::Section:       m_t << "doxysubsection"; break;
      case SectionType::Subsection:    m_t << "doxysubsubsection"; break;
      case SectionType::Subsubsection: m_t << "doxyparagraph"; break;
      case SectionType::Paragraph:     m_t << "doxysubparagraph"; break;
      default: ASSERT(0); break;
    }
    m_t << "{";
  }
}

void LatexGenerator::endSection(const QCString &lab,SectionType)
{
  m_t << "}\\label{" << lab << "}\n";
}


void LatexGenerator::docify(const QCString &str)
{
  filterLatexString(m_t,str,
                    m_codeGen->insideTabbing(), // insideTabbing
                    false,           // insidePre
                    false,           // insideItem
                    m_codeGen->usedTableLevel()>0,  // insideTable
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
  //if (Config_getBool(COMPACT_LATEX)) m_t << "\\doxysubsubsection"; else m_t << "\\doxysubsection";
  //m_t << "{";
}

void LatexGenerator::endClassDiagram(const ClassDiagram &d,
                                       const QCString &fileName,const QCString &)
{
  d.writeFigure(m_t,dir(),fileName);
}


void LatexGenerator::startAnonTypeScope(int indent)
{
  if (indent==0)
  {
    m_t << "\\begin{tabbing}\n";
    m_t << "xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=xx\\=\\kill\n";
    m_codeGen->setInsideTabbing(true);
  }
  m_indent=indent;
}

void LatexGenerator::endAnonTypeScope(int indent)
{
  if (indent==0)
  {
    m_t << "\n" << "\\end{tabbing}";
    m_codeGen->setInsideTabbing(false);
  }
  m_indent=indent;
}

void LatexGenerator::startMemberTemplateParams()
{
  if (m_templateMemberItem)
  {
    m_t << "{\\footnotesize ";
  }
}

void LatexGenerator::endMemberTemplateParams(const QCString &,const QCString &)
{
  if (m_templateMemberItem)
  {
    m_t << "}\\\\";
  }
}

void LatexGenerator::startMemberItem(const QCString &,MemberItemType type,const QCString &)
{
  //printf("LatexGenerator::startMemberItem(%d)\n",annType);
  if (!m_codeGen->insideTabbing())
  {
    m_t << "\\item \n";
    m_templateMemberItem = (type == MemberItemType::Templated);
  }
}

void LatexGenerator::endMemberItem(MemberItemType)
{
  if (m_codeGen->insideTabbing())
  {
    m_t << "\\\\";
  }
  m_templateMemberItem = FALSE;
  m_t << "\n";
}

void LatexGenerator::startMemberDescription(const QCString &,const QCString &,bool)
{
  if (!m_codeGen->insideTabbing())
  {
    m_t << "\\begin{DoxyCompactList}\\small\\item\\em ";
  }
  else
  {
    for (int i=0;i<m_indent+2;i++) m_t << "\\>";
    m_t << "{\\em ";
  }
}

void LatexGenerator::endMemberDescription()
{
  if (!m_codeGen->insideTabbing())
  {
    //m_t << "\\item\\end{DoxyCompactList}";
    m_t << "\\end{DoxyCompactList}";
  }
  else
  {
    m_t << "}\\\\\n";
  }
}


void LatexGenerator::writeNonBreakableSpace(int)
{
  //printf("writeNonBreakableSpace()\n");
  if (m_codeGen->insideTabbing())
  {
    m_t << "\\>";
  }
  else
  {
    m_t << "~";
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

void LatexGenerator::startDescTable(const QCString &title)
{
  m_codeGen->incUsedTableLevel();
  m_t << "\\begin{DoxyEnumFields}{" << title << "}\n";
}

void LatexGenerator::endDescTable()
{
  m_codeGen->decUsedTableLevel();
  m_t << "\\end{DoxyEnumFields}\n";
}

void LatexGenerator::startDescTableRow()
{
  // this is needed to prevent the \hypertarget, \label, and \index commands from messing up
  // the row height (based on http://tex.stackexchange.com/a/186102)
  m_t << "\\raisebox{\\heightof{T}}[0pt][0pt]{";
}

void LatexGenerator::endDescTableRow()
{
}

void LatexGenerator::startDescTableTitle()
{
  m_t << "}";
}

void LatexGenerator::endDescTableTitle()
{
}

void LatexGenerator::startDescTableData()
{
  m_t << "&";
}

void LatexGenerator::endDescTableData()
{
  m_t << "\\\\\n\\hline\n\n";
}

void LatexGenerator::lastIndexPage()
{
}


void LatexGenerator::startMemberList()
{
  if (!m_codeGen->insideTabbing())
  {
    m_t << "\\begin{DoxyCompactItemize}\n";
  }
}

void LatexGenerator::endMemberList()
{
  //printf("LatexGenerator::endMemberList(%d)\n",m_codeGen->InsideTabbing());
  if (!m_codeGen->insideTabbing())
  {
    m_t << "\\end{DoxyCompactItemize}\n";
  }
}


void LatexGenerator::startMemberGroupHeader(bool hasHeader)
{
  if (hasHeader) m_t << "\\begin{Indent}";
  m_t << "\\textbf{ ";
  // changed back to rev 756 due to bug 660501
  //if (Config_getBool(COMPACT_LATEX))
  //{
  //  m_t << "\\doxysubparagraph*{";
  //}
  //else
  //{
  //  m_t << "\\doxyparagraph*{";
  //}
}

void LatexGenerator::endMemberGroupHeader()
{
  // changed back to rev 756 due to bug 660501
  m_t << "}\\par\n";
  //m_t << "}\n";
}

void LatexGenerator::startMemberGroupDocs()
{
  m_t << "{\\em ";
}

void LatexGenerator::endMemberGroupDocs()
{
  m_t << "}";
}

void LatexGenerator::startMemberGroup()
{
}

void LatexGenerator::endMemberGroup(bool hasHeader)
{
  if (hasHeader)m_t << "\\end{Indent}";
  m_t << "\n";
}

void LatexGenerator::startDotGraph()
{
  m_t << "\n" << "\n";
}

void LatexGenerator::endDotGraph(DotClassGraph &g)
{
  g.writeGraph(m_t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startInclDepGraph()
{
}

void LatexGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  g.writeGraph(m_t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startGroupCollaboration()
{
}

void LatexGenerator::endGroupCollaboration(DotGroupCollaboration &g)
{
  g.writeGraph(m_t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startCallGraph()
{
}

void LatexGenerator::endCallGraph(DotCallGraph &g)
{
  g.writeGraph(m_t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startDirDepGraph()
{
}

void LatexGenerator::endDirDepGraph(DotDirDeps &g)
{
  g.writeGraph(m_t,GOF_EPS,EOF_LaTeX,dir(),fileName(),m_relPath);
}

void LatexGenerator::startExamples()
{
  m_t << "\\begin{Desc}\n\\item[";
  docify(theTranslator->trExamples());
  m_t << "]";
}

void LatexGenerator::endExamples()
{
  m_t << "\\end{Desc}\n";
}

void LatexGenerator::startParameterList(bool openBracket)
{
  /* start of ParameterType ParameterName list */
  if (openBracket) m_t << "(";
  m_t << "\\begin{DoxyParamCaption}";
}

void LatexGenerator::endParameterList()
{
}

void LatexGenerator::startParameterType(bool first,const QCString &key)
{
  m_t << "\\item[{";
  if (!first && !key.isEmpty()) docify(key);
}

void LatexGenerator::endParameterType()
{
  m_t << "}]";
}

void LatexGenerator::startParameterName(bool /*oneArgOnly*/)
{
  m_t << "{";
}

void LatexGenerator::endParameterName(bool last,bool /*emptyList*/,bool closeBracket)
{
  m_t << " }";
  if (last)
  {
    m_t << "\\end{DoxyParamCaption}";
    if (closeBracket) m_t << ")";
  }
}

void LatexGenerator::exceptionEntry(const QCString &prefix,bool closeBracket)
{
  if (!prefix.isEmpty())
  {
    m_t << " " << prefix << "(";
  }
  else if (closeBracket)
  {
    m_t << ")";
  }
  m_t << " ";
}

void LatexGenerator::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *,int)
{
  const DocNodeAST *astImpl = dynamic_cast<const DocNodeAST*>(ast);
  if (astImpl)
  {
    LatexDocVisitor visitor(m_t,*m_codeList,*m_codeGen,
                            ctx?ctx->getDefFileExtension():QCString(""));
    std::visit(visitor,astImpl->root);
  }
}

void LatexGenerator::startConstraintList(const QCString &header)
{
  m_t << "\\begin{Desc}\n\\item[";
  docify(header);
  m_t << "]";
  m_t << "\\begin{description}\n";
}

void LatexGenerator::startConstraintParam()
{
  m_t << "\\item[{\\em ";
}

void LatexGenerator::endConstraintParam()
{
}

void LatexGenerator::startConstraintType()
{
  m_t << "} : {\\em ";
}

void LatexGenerator::endConstraintType()
{
  m_t << "}]";
}

void LatexGenerator::startConstraintDocs()
{
}

void LatexGenerator::endConstraintDocs()
{
}

void LatexGenerator::endConstraintList()
{
  m_t << "\\end{description}\n";
  m_t << "\\end{Desc}\n";
}

void LatexGenerator::startInlineHeader()
{
  if (Config_getBool(COMPACT_LATEX))
  {
    m_t << "\\doxyparagraph*{";
  }
  else
  {
    m_t << "\\doxysubsubsection*{";
  }
}

void LatexGenerator::endInlineHeader()
{
  m_t << "}\n";
}

void LatexGenerator::lineBreak(const QCString &)
{
  if (m_codeGen->insideTabbing())
  {
    m_t << "\\\\\n";
  }
  else
  {
    m_t << "\\newline\n";
  }
}

void LatexGenerator::startMemberDocSimple(bool isEnum)
{
  m_codeGen->incUsedTableLevel();
  if (isEnum)
  {
    m_t << "\\begin{DoxyEnumFields}{";
    docify(theTranslator->trEnumerationValues());
  }
  else
  {
    m_t << "\\begin{DoxyFields}{";
    docify(theTranslator->trCompoundMembers());
  }
  m_t << "}\n";
  m_insideTableEnv=true;
}

void LatexGenerator::endMemberDocSimple(bool isEnum)
{
  m_insideTableEnv=false;
  m_codeGen->decUsedTableLevel();
  if (isEnum)
  {
    m_t << "\\end{DoxyEnumFields}\n";
  }
  else
  {
    m_t << "\\end{DoxyFields}\n";
  }
}

void LatexGenerator::startInlineMemberType()
{
  m_codeGen->setInsideTabbing(true); // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberType()
{
  m_t << "&\n";
  m_codeGen->setInsideTabbing(false);
}

void LatexGenerator::startInlineMemberName()
{
  m_codeGen->setInsideTabbing(true); // to prevent \+ from causing unwanted breaks
}

void LatexGenerator::endInlineMemberName()
{
  m_t << "&\n";
  m_codeGen->setInsideTabbing(false);
}

void LatexGenerator::startInlineMemberDoc()
{
}

void LatexGenerator::endInlineMemberDoc()
{
  m_t << "\\\\\n\\hline\n\n";
}

void LatexGenerator::startLabels()
{
  m_t << "\\hspace{0.3cm}";
}

void LatexGenerator::writeLabel(const QCString &l,bool isLast)
{
  m_t << "{\\ttfamily [" << l << "]}";
  if (!isLast) m_t << ", ";
}

void LatexGenerator::endLabels()
{
}

void LatexGenerator::writeInheritedSectionTitle(
                  const QCString &/*id*/,const QCString &ref,
                  const QCString &file,  const QCString &anchor,
                  const QCString &title, const QCString &name)
{
  if (Config_getBool(COMPACT_LATEX))
  {
    m_t << "\\doxyparagraph*{";
  }
  else
  {
    m_t << "\\doxysubsubsection*{";
  }
  m_t << theTranslator->trInheritedFrom(convertToLaTeX(title,m_codeGen->insideTabbing()),
                                        objectLinkToString(ref, file, anchor, name, m_codeGen->insideTabbing(), m_disableLinks));
  m_t << "}\n";
}

void LatexGenerator::writeLocalToc(const SectionRefs &,const LocalToc &localToc)
{
  if (localToc.isLatexEnabled())
  {
    int maxLevel = localToc.latexLevel();
    m_t << "\\etocsetnexttocdepth{" << maxLevel << "}\n";
    m_t << "\\localtableofcontents\n";
  }
}

//--------------------------------------------------------------------------------------------------

void writeExtraLatexPackages(TextStream &t)
{
  // User-specified packages
  const StringVector &extraPackages = Config_getList(EXTRA_PACKAGES);
  if (!extraPackages.empty())
  {
    t << "% Packages requested by user\n";
    for (const auto &pkgName : extraPackages)
    {
      if ((pkgName[0] == '[') || (pkgName[0] == '{'))
        t << "\\usepackage" << pkgName.c_str() << "\n";
      else
        t << "\\usepackage{" << pkgName.c_str() << "}\n";
    }
    t << "\n";
  }
}

void writeLatexSpecialFormulaChars(TextStream &t)
{
    unsigned char minus[4]; // Superscript minus
    unsigned char sup2[3]; // Superscript two
    unsigned char sup3[3];
    minus[0]= 0xE2;
    minus[1]= 0x81;
    minus[2]= 0xBB;
    minus[3]= 0;
    sup2[0]= 0xC2;
    sup2[1]= 0xB2;
    sup2[2]= 0;
    sup3[0]= 0xC2;
    sup3[1]= 0xB3;
    sup3[2]= 0;

    t << "\\usepackage{newunicodechar}\n"
         "  \\newunicodechar{" << minus << "}{${}^{-}$}% Superscript minus\n"
         "  \\newunicodechar{" << sup2  << "}{${}^{2}$}% Superscript two\n"
         "  \\newunicodechar{" << sup3  << "}{${}^{3}$}% Superscript three\n"
         "\n";
}

void filterLatexString(TextStream &t,const QCString &str,
    bool insideTabbing,bool insidePre,bool insideItem,bool insideTable,bool keepSpaces, const bool retainNewline)
{
  if (str.isEmpty()) return;
  //printf("filterLatexString(%s) insideTabbing=%d\n",qPrint(str),insideTabbing);
  const char *p=str.data();
  const char *q;
  int cnt;
  unsigned char c;
  unsigned char pc='\0';
  while (*p)
  {
    c=static_cast<unsigned char>(*p++);

    if (insidePre)
    {
      switch(c)
      {
        case 0xef: // handle U+FFFD i.e. "Replacement character" caused by octal: 357 277 275 / hexadecimal 0xef 0xbf 0xbd
                   // the LaTeX command \ucr has been defined in doxygen.sty
          if (static_cast<unsigned char>(*(p)) == 0xbf && static_cast<unsigned char>(*(p+1)) == 0xbd)
          {
            t << "{\\ucr}";
            p += 2;
          }
          else
            t << static_cast<char>(c);
          break;
        case '\\': t << "\\(\\backslash\\)"; break;
        case '{':  t << "\\{"; break;
        case '}':  t << "\\}"; break;
        case '_':  t << "\\_"; break;
        case '&':  t << "\\&"; break;
        case '%':  t << "\\%"; break;
        case '#':  t << "\\#"; break;
        case '$':  t << "\\$"; break;
        case '"':  t << "\"{}"; break;
        case '-':  t << "-\\/"; break;
        case '^':  insideTable ? t << "\\string^" : t << static_cast<char>(c);    break;
        case '~':  t << "\\string~";    break;
        case '\n':  if (retainNewline) t << "\\newline"; else t << ' ';
                   break;
        case ' ':  if (keepSpaces) t << "~"; else t << ' ';
                   break;
        default:
                   if (c<32) t << ' '; // non printable control character
                   else t << static_cast<char>(c);
                   break;
      }
    }
    else
    {
      switch(c)
      {
        case 0xef: // handle U+FFFD i.e. "Replacement character" caused by octal: 357 277 275 / hexadecimal 0xef 0xbf 0xbd
                   // the LaTeX command \ucr has been defined in doxygen.sty
          if (static_cast<unsigned char>(*(p)) == 0xbf && static_cast<unsigned char>(*(p+1)) == 0xbd)
          {
            t << "{\\ucr}";
            p += 2;
          }
          else
            t << static_cast<char>(c);
          break;
        case '#':  t << "\\#";           break;
        case '$':  t << "\\$";           break;
        case '%':  t << "\\%";           break;
        case '^':  t << "$^\\wedge$";    break;
        case '&':  // possibility to have a special symbol
                   q = p;
                   cnt = 2; // we have to count & and ; as well
                   while ((*q >= 'a' && *q <= 'z') || (*q >= 'A' && *q <= 'Z') || (*q >= '0' && *q <= '9'))
                   {
                     cnt++;
                     q++;
                   }
                   if (*q == ';')
                   {
                      --p; // we need & as well
                      HtmlEntityMapper::SymType res = HtmlEntityMapper::instance().name2sym(QCString(p).left(cnt));
                      if (res == HtmlEntityMapper::Sym_Unknown)
                      {
                        p++;
                        t << "\\&";
                      }
                      else
                      {
                        t << HtmlEntityMapper::instance().latex(res);
                        q++;
                        p = q;
                      }
                   }
                   else
                   {
                     t << "\\&";
                   }
                   break;
        case '*':  t << "$\\ast$";       break;
        case '_':  if (!insideTabbing) t << "\\+";
                   t << "\\_";
                   if (!insideTabbing) t << "\\+";
                   break;
        case '{':  t << "\\{";           break;
        case '}':  t << "\\}";           break;
        case '<':  t << "$<$";           break;
        case '>':  t << "$>$";           break;
        case '|':  t << "$\\vert$";      break;
        case '~':  t << "$\\sim$";       break;
        case '[':  if (Config_getBool(PDF_HYPERLINKS) || insideItem)
                     t << "\\mbox{[}";
                   else
                     t << "[";
                   break;
        case ']':  if (pc=='[') t << "$\\,$";
                     if (Config_getBool(PDF_HYPERLINKS) || insideItem)
                       t << "\\mbox{]}";
                     else
                       t << "]";
                   break;
        case '-':  t << "-\\/";
                   break;
        case '\\': t << "\\textbackslash{}";
                   break;
        case '"':  t << "\\char`\\\"{}";
                   break;
        case '`':  t << "\\`{}";
                   break;
        case '\'': t << "\\textquotesingle{}";
                   break;
        case '\n':  if (retainNewline) t << "\\newline"; else t << ' ';
                   break;
        case ' ':  if (keepSpaces) { if (insideTabbing) t << "\\>"; else t << '~'; } else t << ' ';
                   break;

        default:
                   //if (!insideTabbing && forceBreaks && c!=' ' && *p!=' ')
                   if (!insideTabbing &&
                       ((c>='A' && c<='Z' && pc!=' ' && !(pc>='A' && pc <= 'Z') && pc!='\0' && *p) || (c==':' && pc!=':') || (pc=='.' && isId(c)))
                      )
                   {
                     t << "\\+";
                   }
                   if (c<32)
                   {
                     t << ' '; // non-printable control character
                   }
                   else
                   {
                     t << static_cast<char>(c);
                   }
      }
    }
    pc = c;
  }
}

QCString convertToLaTeX(const QCString &s,bool insideTabbing,bool keepSpaces)
{
  TextStream t;
  filterLatexString(t,s,insideTabbing,false,false,false,keepSpaces);
  return t.str();
}

QCString latexEscapeLabelName(const QCString &s)
{
  //printf("latexEscapeLabelName(%s)\n",qPrint(s));
  if (s.isEmpty()) return s;
  QCString tmp(s.length()+1);
  TextStream t;
  const char *p=s.data();
  char c;
  int i;
  while ((c=*p++))
  {
    switch (c)
    {
      case '|': t << "\\texttt{\"|}"; break;
      case '!': t << "\"!"; break;
      case '@': t << "\"@"; break;
      case '%': t << "\\%";       break;
      case '{': t << "\\lcurly{}"; break;
      case '}': t << "\\rcurly{}"; break;
      case '~': t << "````~"; break; // to get it a bit better in index together with other special characters
      // NOTE: adding a case here, means adding it to while below as well!
      default:
        i=0;
        // collect as long string as possible, before handing it to docify
        tmp[i++]=c;
        while ((c=*p) && c!='@' && c!='[' && c!=']' && c!='!' && c!='{' && c!='}' && c!='|')
        {
          tmp[i++]=c;
          p++;
        }
        tmp[i]=0;
        filterLatexString(t,tmp,
                          true,  // insideTabbing
                          false, // insidePre
                          false, // insideItem
                          false, // insideTable
                          false  // keepSpaces
                         );
        break;
    }
  }
  return t.str();
}

QCString latexEscapeIndexChars(const QCString &s)
{
  //printf("latexEscapeIndexChars(%s)\n",qPrint(s));
  if (s.isEmpty()) return s;
  QCString tmp(s.length()+1);
  TextStream t;
  const char *p=s.data();
  char c;
  int i;
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
      case '{': t << "\\lcurly{}"; break;
      case '}': t << "\\rcurly{}"; break;
      // NOTE: adding a case here, means adding it to while below as well!
      default:
        i=0;
        // collect as long string as possible, before handing it to docify
        tmp[i++]=c;
        while ((c=*p) && c!='"' && c!='@' && c!='[' && c!=']' && c!='!' && c!='{' && c!='}' && c!='|')
        {
          tmp[i++]=c;
          p++;
        }
        tmp[i]=0;
        filterLatexString(t,tmp,
                          true,   // insideTabbing
                          false,  // insidePre
                          false,  // insideItem
                          false,  // insideTable
                          false   // keepSpaces
                         );
        break;
    }
  }
  return t.str();
}

QCString latexEscapePDFString(const QCString &s)
{
  if (s.isEmpty()) return s;
  TextStream t;
  const char *p=s.data();
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '\\': t << "\\textbackslash{}"; break;
      case '{':  t << "\\{"; break;
      case '}':  t << "\\}"; break;
      case '_':  t << "\\_"; break;
      case '%':  t << "\\%"; break;
      case '&':  t << "\\&"; break;
      default:
        t << c;
        break;
    }
  }
  return t.str();
}

QCString latexFilterURL(const QCString &s)
{
  constexpr auto hex = "0123456789ABCDEF";
  if (s.isEmpty()) return s;
  TextStream t;
  const char *p=s.data();
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '#':  t << "\\#"; break;
      case '%':  t << "\\%"; break;
      case '\\':  t << "\\\\"; break;
      default:
        if (c<0)
        {
          unsigned char id = static_cast<unsigned char>(c);
          t << "\\%" << hex[id>>4] << hex[id&0xF];
        }
        else
        {
          t << c;
        }
        break;
    }
  }
  return t.str();
}


