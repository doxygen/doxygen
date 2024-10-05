/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include <algorithm>
#include <array>

#include "htmlattrib.h"
#include "latexdocvisitor.h"
#include "latexgen.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "outputlist.h"
#include "dot.h"
#include "util.h"
#include "message.h"
#include "parserintf.h"
#include "msc.h"
#include "dia.h"
#include "cite.h"
#include "filedef.h"
#include "config.h"
#include "htmlentity.h"
#include "emoji.h"
#include "plantuml.h"
#include "fileinfo.h"
#include "regex.h"
#include "portable.h"
#include "codefragment.h"

static const int g_maxLevels = 7;
static const std::array<const char *,g_maxLevels> g_secLabels =
{ "doxysection",
  "doxysubsection",
  "doxysubsubsection",
  "doxysubsubsubsection",
  "doxysubsubsubsubsection",
  "doxysubsubsubsubsubsection",
  "doxysubsubsubsubsubsubsection"
};

static const char *g_paragraphLabel = "doxyparagraph";
static const char *g_subparagraphLabel = "doxysubparagraph";

const char *LatexDocVisitor::getSectionName(int level) const
{
  bool compactLatex = Config_getBool(COMPACT_LATEX);
  int l = level;
  if (compactLatex) l++;

  if (l < g_maxLevels)
  {
    l += m_hierarchyLevel; /* May be -1 if generating main page */
    // Sections get special treatment because they inherit the parent's level
    if (l >= g_maxLevels)
    {
      l = g_maxLevels - 1;
    }
    else if (l < 0)
    {
      /* Should not happen; level is always >= 1 and hierarchyLevel >= -1 */
      l = 0;
    }
    return g_secLabels[l];
  }
  else if (l == 7)
  {
    return g_paragraphLabel;
  }
  else
  {
    return g_subparagraphLabel;
  }
}

static void insertDimension(TextStream &t, QCString dimension, const char *orientationString)
{
  // dimensions for latex images can be a percentage, in this case they need some extra
  // handling as the % symbol is used for comments
  static const reg::Ex re(R"((\d+)%)");
  std::string s = dimension.str();
  reg::Match match;
  if (reg::search(s,match,re))
  {
    bool ok = false;
    double percent = QCString(match[1].str()).toInt(&ok);
    if (ok)
    {
      t << percent/100.0 << "\\text" << orientationString;
      return;
    }
  }
  t << dimension;
}

static void visitPreStart(TextStream &t, bool hasCaption, QCString name,  QCString width,  QCString height, bool inlineImage = FALSE)
{
    if (inlineImage)
    {
      t << "\n\\begin{DoxyInlineImage}\n";
    }
    else
    {
      if (hasCaption)
      {
        t << "\n\\begin{DoxyImage}\n";
      }
      else
      {
        t << "\n\\begin{DoxyImageNoCaption}\n"
               "  \\mbox{";
      }
    }

    t << "\\includegraphics";
    if (!width.isEmpty() || !height.isEmpty())
    {
      t << "[";
    }
    if (!width.isEmpty())
    {
      t << "width=";
      insertDimension(t, width, "width");
    }
    if (!width.isEmpty() && !height.isEmpty())
    {
      t << ",";
    }
    if (!height.isEmpty())
    {
      t << "height=";
      insertDimension(t, height, "height");
    }
    if (width.isEmpty() && height.isEmpty())
    {
      /* default setting */
      if (inlineImage)
      {
        t << "[height=\\baselineskip,keepaspectratio=true]";
      }
      else
      {
        t << "[width=\\textwidth,height=\\textheight/2,keepaspectratio=true]";
      }
    }
    else
    {
      t << "]";
    }

    t << "{" << name << "}";

    if (hasCaption)
    {
      if (!inlineImage)
      {
        if (Config_getBool(PDF_HYPERLINKS))
        {
          t << "\n\\doxyfigcaption{";
        }
        else
        {
          t << "\n\\doxyfigcaptionnolink{";
        }
      }
      else
      {
        t << "%"; // to catch the caption
      }
    }
}



static void visitPostEnd(TextStream &t, bool hasCaption, bool inlineImage = FALSE)
{
    if (inlineImage)
    {
      t << "\n\\end{DoxyInlineImage}\n";
    }
    else
    {
      t << "}\n"; // end mbox or caption
      if (hasCaption)
      {
        t << "\\end{DoxyImage}\n";
      }
      else
      {
        t << "\\end{DoxyImageNoCaption}\n";
      }
    }
}

static QCString makeShortName(const QCString &name)
{
  QCString shortName = name;
  int i = shortName.findRev('/');
  if (i!=-1)
  {
    shortName=shortName.mid(i+1);
  }
  return shortName;
}

static QCString makeBaseName(const QCString &name)
{
  QCString baseName = makeShortName(name);
  int i=baseName.find('.');
  if (i!=-1)
  {
    baseName=baseName.left(i);
  }
  return baseName;
}


void LatexDocVisitor::visitCaption(const DocNodeList &children)
{
  for (const auto &n : children)
  {
    std::visit(*this,n);
  }
}

QCString LatexDocVisitor::escapeMakeIndexChars(const char *s)
{
  QCString result;
  const char *p=s;
  char str[2]; str[1]=0;
  char c = 0;
  if (p)
  {
    while ((c=*p++))
    {
      switch (c)
      {
        case '!': m_t << "\"!"; break;
        case '"': m_t << "\"\""; break;
        case '@': m_t << "\"@"; break;
        case '|': m_t << "\\texttt{\"|}"; break;
        case '[': m_t << "["; break;
        case ']': m_t << "]"; break;
        case '{': m_t << "\\lcurly{}"; break;
        case '}': m_t << "\\rcurly{}"; break;
        default:  str[0]=c; filter(str); break;
      }
    }
  }
  return result;
}


LatexDocVisitor::LatexDocVisitor(TextStream &t,OutputCodeList &ci,LatexCodeGenerator &lcg,
                                 const QCString &langExt, int hierarchyLevel)
  : m_t(t), m_ci(ci), m_lcg(lcg), m_insidePre(FALSE),
    m_insideItem(FALSE), m_hide(FALSE),
    m_langExt(langExt), m_hierarchyLevel(hierarchyLevel)
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void LatexDocVisitor::operator()(const DocWord &w)
{
  if (m_hide) return;
  filter(w.word());
}

void LatexDocVisitor::operator()(const DocLinkedWord &w)
{
  if (m_hide) return;
  startLink(w.ref(),w.file(),w.anchor());
  filter(w.word());
  endLink(w.ref(),w.file(),w.anchor());
}

void LatexDocVisitor::operator()(const DocWhiteSpace &w)
{
  if (m_hide) return;
  if (m_insidePre)
  {
    m_t << w.chars();
  }
  else
  {
    m_t << " ";
  }
}

void LatexDocVisitor::operator()(const DocSymbol &s)
{
  if (m_hide) return;
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  const char *res = HtmlEntityMapper::instance().latex(s.symbol());
  if (res)
  {
    if (((s.symbol() == HtmlEntityMapper::Sym_lt) || (s.symbol() == HtmlEntityMapper::Sym_Less))&& (!m_insidePre))
    {
      if (pdfHyperlinks)
      {
        m_t << "\\texorpdfstring{$<$}{<}";
      }
      else
      {
        m_t << "$<$";
      }
    }
    else if (((s.symbol() == HtmlEntityMapper::Sym_gt) || (s.symbol() == HtmlEntityMapper::Sym_Greater)) && (!m_insidePre))
    {
      if (pdfHyperlinks)
      {
        m_t << "\\texorpdfstring{$>$}{>}";
      }
      else
      {
        m_t << "$>$";
      }
    }
    else
    {
      m_t << res;
    }
  }
  else
  {
    err("LaTeX: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
  }
}

void LatexDocVisitor::operator()(const DocEmoji &s)
{
  if (m_hide) return;
  QCString emojiName = EmojiEntityMapper::instance().name(s.index());
  if (!emojiName.isEmpty())
  {
    QCString imageName=emojiName.mid(1,emojiName.length()-2); // strip : at start and end
    if (m_texOrPdf != TexOrPdf::PDF) m_t << "\\doxygenemoji{";
    filter(emojiName);
    if (m_texOrPdf != TexOrPdf::PDF) m_t << "}{" << imageName << "}";
  }
  else
  {
    m_t << s.name();
  }
}

void LatexDocVisitor::operator()(const DocURL &u)
{
  if (m_hide) return;
  if (Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\href{";
    if (u.isEmail()) m_t << "mailto:";
    m_t << latexFilterURL(u.url()) << "}";
  }
  m_t << "{\\texttt{ ";
  filter(u.url());
  m_t << "}}";
}

void LatexDocVisitor::operator()(const DocLineBreak &)
{
  if (m_hide) return;
  m_t << "~\\newline\n";
}

void LatexDocVisitor::operator()(const DocHorRuler &)
{
  if (m_hide) return;
  if (insideTable())
    m_t << "\\DoxyHorRuler{1}\n";
  else
    m_t << "\\DoxyHorRuler{0}\n";
}

void LatexDocVisitor::operator()(const DocStyleChange &s)
{
  if (m_hide) return;
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) m_t << "{\\bfseries{";      else m_t << "}}";
      break;
    case DocStyleChange::S:
    case DocStyleChange::Strike:
    case DocStyleChange::Del:
      if (s.enable()) m_t << "\\sout{";     else m_t << "}";
      break;
    case DocStyleChange::Underline:
    case DocStyleChange::Ins:
      if (s.enable()) m_t << "\\uline{";     else m_t << "}";
      break;
    case DocStyleChange::Italic:
      if (s.enable()) m_t << "{\\itshape ";     else m_t << "}";
      break;
    case DocStyleChange::Code:
      if (s.enable()) m_t << "{\\ttfamily ";   else m_t << "}";
      break;
    case DocStyleChange::Subscript:
      if (s.enable()) m_t << "\\textsubscript{";    else m_t << "}";
      break;
    case DocStyleChange::Superscript:
      if (s.enable()) m_t << "\\textsuperscript{";    else m_t << "}";
      break;
    case DocStyleChange::Center:
      if (s.enable()) m_t << "\\begin{center}"; else m_t << "\\end{center} ";
      break;
    case DocStyleChange::Small:
      if (s.enable()) m_t << "\n\\footnotesize ";  else m_t << "\n\\normalsize ";
      break;
    case DocStyleChange::Cite:
      if (s.enable()) m_t << "{\\itshape ";     else m_t << "}";
      break;
    case DocStyleChange::Preformatted:
      if (s.enable())
      {
        m_t << "\n\\begin{DoxyPre}";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "\\end{DoxyPre}\n";
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void LatexDocVisitor::operator()(const DocVerbatim &s)
{
  if (m_hide) return;
  QCString lang = m_langExt;
  if (!s.language().isEmpty()) // explicit language setting
  {
    lang = s.language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch(s.type())
  {
    case DocVerbatim::Code:
      {
        m_ci.startCodeFragment("DoxyCode");
        getCodeParser(lang).parseCode(m_ci,s.context(),s.text(),langExt,
                                      Config_getBool(STRIP_CODE_COMMENTS),
                                      s.isExample(),s.exampleFile());
        m_ci.endCodeFragment("DoxyCode");
      }
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s.text(), true);
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "{\\ttfamily ";
      filter(s.text(), true);
      m_t << "}";
      break;
    case DocVerbatim::Verbatim:
      m_t << "\\begin{DoxyVerb}";
      m_t << s.text();
      m_t << "\\end{DoxyVerb}\n";
      break;
    case DocVerbatim::HtmlOnly:
    case DocVerbatim::XmlOnly:
    case DocVerbatim::ManOnly:
    case DocVerbatim::RtfOnly:
    case DocVerbatim::DocbookOnly:
      /* nothing */
      break;
    case DocVerbatim::LatexOnly:
      m_t << s.text();
      break;
    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString fileName(4096, QCString::ExplicitSize);

        fileName.sprintf("%s%d%s",
            qPrint(Config_getString(LATEX_OUTPUT)+"/inline_dotgraph_"),
            dotindex++,
            ".dot"
           );
        std::ofstream file = Portable::openOutputStream(fileName);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(fileName));
        }
        else
        {
          file.write( s.text().data(), s.text().length() );
          file.close();

          startDotFile(fileName,s.width(),s.height(),s.hasCaption(),s.srcFile(),s.srcLine());
          visitChildren(s);
          endDotFile(s.hasCaption());

          if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName.str());
        }
      }
      break;
    case DocVerbatim::Msc:
      {
        static int mscindex = 1;
        QCString baseName(4096, QCString::ExplicitSize);

        baseName.sprintf("%s%d",
            qPrint(Config_getString(LATEX_OUTPUT)+"/inline_mscgraph_"),
            mscindex++
           );
        QCString fileName = baseName+".msc";
        std::ofstream file = Portable::openOutputStream(fileName);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(fileName));
        }
        else
        {
          QCString text = "msc {";
          text+=s.text();
          text+="}";
          file.write( text.data(), text.length() );
          file.close();

          writeMscFile(baseName, s);

          if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName.str());
        }
      }
      break;
    case DocVerbatim::PlantUML:
      {
        QCString latexOutput = Config_getString(LATEX_OUTPUT);
        QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                              latexOutput,s.exampleFile(),s.text(),
                              s.useBitmap() ? PlantumlManager::PUML_BITMAP : PlantumlManager::PUML_EPS,
                              s.engine(),s.srcFile(),s.srcLine(),true);

        writePlantUMLFile(baseName, s);
      }
      break;
  }
}

void LatexDocVisitor::operator()(const DocAnchor &anc)
{
  if (m_hide) return;
  m_t << "\\label{" << stripPath(anc.file()) << "_" << anc.anchor() << "}%\n";
  if (!anc.file().isEmpty() && Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\Hypertarget{" << stripPath(anc.file()) << "_" << anc.anchor()
      << "}%\n";
  }
}

void LatexDocVisitor::operator()(const DocInclude &inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc.extension());
  switch(inc.type())
  {
    case DocInclude::IncWithLines:
      {
        m_ci.startCodeFragment("DoxyCodeInclude");
        FileInfo cfi( inc.file().str() );
        auto fd = createFileDef( cfi.dirPath(), cfi.fileName() );
        getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                                  inc.text(),
                                                  langExt,
                                                  inc.stripCodeComments(),
                                                  inc.isExample(),
                                                  inc.exampleFile(),
                                                  fd.get(),    // fileDef,
                                                  -1,    // start line
                                                  -1,    // end line
                                                  FALSE, // inline fragment
                                                  nullptr,     // memberDef
                                                  TRUE   // show line numbers
       				                 );
        m_ci.endCodeFragment("DoxyCodeInclude");
      }
      break;
    case DocInclude::Include:
      {
        m_ci.startCodeFragment("DoxyCodeInclude");
        getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                                  inc.text(),langExt,
                                                  inc.stripCodeComments(),
                                                  inc.isExample(),
                                                  inc.exampleFile(),
                                                  nullptr,     // fileDef
                                                  -1,    // startLine
                                                  -1,    // endLine
                                                  TRUE,  // inlineFragment
                                                  nullptr,     // memberDef
                                                  FALSE
                                                 );
        m_ci.endCodeFragment("DoxyCodeInclude");
      }
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
    case DocInclude::HtmlInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      break;
    case DocInclude::LatexInclude:
      m_t << inc.text();
      break;
    case DocInclude::VerbInclude:
      m_t << "\n\\begin{DoxyVerbInclude}\n";
      m_t << inc.text();
      m_t << "\\end{DoxyVerbInclude}\n";
      break;
    case DocInclude::Snippet:
    case DocInclude::SnippetWithLines:
      {
        m_ci.startCodeFragment("DoxyCodeInclude");
        CodeFragmentManager::instance().parseCodeFragment(m_ci,
                                         inc.file(),
                                         inc.blockId(),
                                         inc.context(),
                                         inc.type()==DocInclude::SnippetWithLines,
                                         inc.trimLeft(),
                                         inc.stripCodeComments()
                                        );
        m_ci.endCodeFragment("DoxyCodeInclude");
      }
      break;
  }
}

void LatexDocVisitor::operator()(const DocIncOperator &op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op.type(),op.isFirst(),op.isLast(),qPrint(op.text()));
  if (op.isFirst())
  {
    if (!m_hide) m_ci.startCodeFragment("DoxyCodeInclude");
    pushHidden(m_hide);
    m_hide = TRUE;
  }
  QCString locLangExt = getFileNameExtension(op.includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  if (op.type()!=DocIncOperator::Skip)
  {
    m_hide = popHidden();
    if (!m_hide)
    {
      std::unique_ptr<FileDef> fd;
      if (!op.includeFileName().isEmpty())
      {
        FileInfo cfi( op.includeFileName().str() );
        fd = createFileDef( cfi.dirPath(), cfi.fileName() );
      }

      getCodeParser(locLangExt).parseCode(m_ci,op.context(),op.text(),langExt,
                                          op.stripCodeComments(),
                                          op.isExample(),op.exampleFile(),
                                          fd.get(),     // fileDef
                                          op.line(),    // startLine
                                          -1,    // endLine
                                          FALSE, // inline fragment
                                          nullptr,     // memberDef
                                          op.showLineNo()  // show line numbers
                                         );
    }
    pushHidden(m_hide);
    m_hide=TRUE;
  }
  if (op.isLast())
  {
    m_hide=popHidden();
    if (!m_hide) m_ci.endCodeFragment("DoxyCodeInclude");
  }
  else
  {
    if (!m_hide) m_t << "\n";
  }
}

void LatexDocVisitor::operator()(const DocFormula &f)
{
  if (m_hide) return;
  QCString s = f.text();
  const char *p = s.data();
  char c = 0;
  if (p)
  {
    while ((c=*p++))
    {
      switch (c)
      {
        case '\'': m_t << "\\textnormal{\\textquotesingle}"; break;
        default:  m_t << c; break;
      }
    }
  }
}

void LatexDocVisitor::operator()(const DocIndexEntry &i)
{
  if (m_hide) return;
  m_t << "\\index{";
  m_t << latexEscapeLabelName(i.entry());
  m_t << "@{";
  m_t << latexEscapeIndexChars(i.entry());
  m_t << "}}";
}

void LatexDocVisitor::operator()(const DocSimpleSectSep &)
{
}

void LatexDocVisitor::operator()(const DocCite &cite)
{
  if (m_hide) return;
  if (!cite.file().isEmpty())
  {
    //startLink(cite.ref(),cite.file(),cite.anchor());
    QCString anchor = cite.anchor();
    QCString anchorPrefix = CitationManager::instance().anchorPrefix();
    anchor = anchor.mid(anchorPrefix.length()); // strip prefix
    m_t << "\\cite{" << anchor << "}";
  }
  else
  {
    m_t << "{\\bfseries [";
    filter(cite.text());
    m_t << "]}";
  }
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void LatexDocVisitor::operator()(const DocAutoList &l)
{
  if (m_hide) return;
  if (m_indentLevel>=maxIndentLevels-1) return;
  if (l.isEnumList())
  {
    m_t << "\n\\begin{DoxyEnumerate}";
    m_listItemInfo[indentLevel()].isEnum = true;
  }
  else
  {
    m_listItemInfo[indentLevel()].isEnum = false;
    m_t << "\n\\begin{DoxyItemize}";
  }
  visitChildren(l);
  if (l.isEnumList())
  {
    m_t << "\n\\end{DoxyEnumerate}";
  }
  else
  {
    m_t << "\n\\end{DoxyItemize}";
  }
}

void LatexDocVisitor::operator()(const DocAutoListItem &li)
{
  if (m_hide) return;
  switch (li.itemNumber())
  {
    case DocAutoList::Unchecked: // unchecked
      m_t << "\n\\item[\\DoxyUnchecked] ";
      break;
    case DocAutoList::Checked_x: // checked with x
    case DocAutoList::Checked_X: // checked with X
      m_t << "\n\\item[\\DoxyChecked] ";
      break;
    default:
      m_t << "\n\\item ";
      break;
  }
  incIndentLevel();
  visitChildren(li);
  decIndentLevel();
}

void LatexDocVisitor::operator()(const DocPara &p)
{
  if (m_hide) return;
  visitChildren(p);
  if (!p.isLast() &&            // omit <p> for last paragraph
      !(p.parent() &&           // and for parameter sections
        std::get_if<DocParamSect>(p.parent())
       )
     ) m_t << "\n\n";
}

void LatexDocVisitor::operator()(const DocRoot &r)
{
  visitChildren(r);
}

void LatexDocVisitor::operator()(const DocSimpleSect &s)
{
  if (m_hide) return;
  switch(s.type())
  {
    case DocSimpleSect::See:
      m_t << "\\begin{DoxySeeAlso}{";
      filter(theTranslator->trSeeAlso());
      break;
    case DocSimpleSect::Return:
      m_t << "\\begin{DoxyReturn}{";
      filter(theTranslator->trReturns());
      break;
    case DocSimpleSect::Author:
      m_t << "\\begin{DoxyAuthor}{";
      filter(theTranslator->trAuthor(TRUE,TRUE));
      break;
    case DocSimpleSect::Authors:
      m_t << "\\begin{DoxyAuthor}{";
      filter(theTranslator->trAuthor(TRUE,FALSE));
      break;
    case DocSimpleSect::Version:
      m_t << "\\begin{DoxyVersion}{";
      filter(theTranslator->trVersion());
      break;
    case DocSimpleSect::Since:
      m_t << "\\begin{DoxySince}{";
      filter(theTranslator->trSince());
      break;
    case DocSimpleSect::Date:
      m_t << "\\begin{DoxyDate}{";
      filter(theTranslator->trDate());
      break;
    case DocSimpleSect::Note:
      m_t << "\\begin{DoxyNote}{";
      filter(theTranslator->trNote());
      break;
    case DocSimpleSect::Warning:
      m_t << "\\begin{DoxyWarning}{";
      filter(theTranslator->trWarning());
      break;
    case DocSimpleSect::Pre:
      m_t << "\\begin{DoxyPrecond}{";
      filter(theTranslator->trPrecondition());
      break;
    case DocSimpleSect::Post:
      m_t << "\\begin{DoxyPostcond}{";
      filter(theTranslator->trPostcondition());
      break;
    case DocSimpleSect::Copyright:
      m_t << "\\begin{DoxyCopyright}{";
      filter(theTranslator->trCopyright());
      break;
    case DocSimpleSect::Invar:
      m_t << "\\begin{DoxyInvariant}{";
      filter(theTranslator->trInvariant());
      break;
    case DocSimpleSect::Remark:
      m_t << "\\begin{DoxyRemark}{";
      filter(theTranslator->trRemarks());
      break;
    case DocSimpleSect::Attention:
      m_t << "\\begin{DoxyAttention}{";
      filter(theTranslator->trAttention());
      break;
    case DocSimpleSect::Important:
      m_t << "\\begin{DoxyImportant}{";
      filter(theTranslator->trImportant());
      break;
    case DocSimpleSect::User:
      m_t << "\\begin{DoxyParagraph}{";
      break;
    case DocSimpleSect::Rcs:
      m_t << "\\begin{DoxyParagraph}{";
      break;
    case DocSimpleSect::Unknown:  break;
  }

  if (s.title())
  {
    m_insideItem=TRUE;
    std::visit(*this,*s.title());
    m_insideItem=FALSE;
  }
  m_t << "}\n";
  incIndentLevel();
  visitChildren(s);
  switch(s.type())
  {
    case DocSimpleSect::See:
      m_t << "\n\\end{DoxySeeAlso}\n";
      break;
    case DocSimpleSect::Return:
      m_t << "\n\\end{DoxyReturn}\n";
      break;
    case DocSimpleSect::Author:
      m_t << "\n\\end{DoxyAuthor}\n";
      break;
    case DocSimpleSect::Authors:
      m_t << "\n\\end{DoxyAuthor}\n";
      break;
    case DocSimpleSect::Version:
      m_t << "\n\\end{DoxyVersion}\n";
      break;
    case DocSimpleSect::Since:
      m_t << "\n\\end{DoxySince}\n";
      break;
    case DocSimpleSect::Date:
      m_t << "\n\\end{DoxyDate}\n";
      break;
    case DocSimpleSect::Note:
      m_t << "\n\\end{DoxyNote}\n";
      break;
    case DocSimpleSect::Warning:
      m_t << "\n\\end{DoxyWarning}\n";
      break;
    case DocSimpleSect::Pre:
      m_t << "\n\\end{DoxyPrecond}\n";
      break;
    case DocSimpleSect::Post:
      m_t << "\n\\end{DoxyPostcond}\n";
      break;
    case DocSimpleSect::Copyright:
      m_t << "\n\\end{DoxyCopyright}\n";
      break;
    case DocSimpleSect::Invar:
      m_t << "\n\\end{DoxyInvariant}\n";
      break;
    case DocSimpleSect::Remark:
      m_t << "\n\\end{DoxyRemark}\n";
      break;
    case DocSimpleSect::Attention:
      m_t << "\n\\end{DoxyAttention}\n";
      break;
    case DocSimpleSect::Important:
      m_t << "\n\\end{DoxyImportant}\n";
      break;
    case DocSimpleSect::User:
      m_t << "\n\\end{DoxyParagraph}\n";
      break;
    case DocSimpleSect::Rcs:
      m_t << "\n\\end{DoxyParagraph}\n";
      break;
    default:
      break;
  }
  decIndentLevel();
}

void LatexDocVisitor::operator()(const DocTitle &t)
{
  if (m_hide) return;
  visitChildren(t);
}

void LatexDocVisitor::operator()(const DocSimpleList &l)
{
  if (m_hide) return;
  m_t << "\\begin{DoxyItemize}\n";
  m_listItemInfo[indentLevel()].isEnum = false;
  visitChildren(l);
  m_t << "\\end{DoxyItemize}\n";
}

void LatexDocVisitor::operator()(const DocSimpleListItem &li)
{
  if (m_hide) return;
  m_t << "\\item ";
  incIndentLevel();
  if (li.paragraph())
  {
    visit(*this,*li.paragraph());
  }
  decIndentLevel();
}

void LatexDocVisitor::operator()(const DocSection &s)
{
  if (m_hide) return;
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (pdfHyperlinks)
  {
    m_t << "\\hypertarget{" << stripPath(s.file()) << "_" << s.anchor() << "}{}";
  }
  m_t << "\\" << getSectionName(s.level()) << "{";
  if (pdfHyperlinks)
  {
    m_t << "\\texorpdfstring{";
  }
  if (s.title())
  {
    if (pdfHyperlinks) m_texOrPdf = TexOrPdf::TEX;
    std::visit(*this,*s.title());
    m_texOrPdf = TexOrPdf::NO;
  }
  if (pdfHyperlinks)
  {
    m_t << "}{";
    if (s.title())
    {
      if (pdfHyperlinks) m_texOrPdf = TexOrPdf::PDF;
      std::visit(*this,*s.title());
      m_texOrPdf = TexOrPdf::NO;
    }
    m_t << "}";
  }
  m_t << "}\\label{" << stripPath(s.file()) << "_" << s.anchor() << "}\n";
  visitChildren(s);
}

void LatexDocVisitor::operator()(const DocHtmlList &s)
{
  if (m_hide) return;
  if (m_indentLevel>=maxIndentLevels-1) return;
  m_listItemInfo[indentLevel()].isEnum = s.type()==DocHtmlList::Ordered;
  if (s.type()==DocHtmlList::Ordered)
  {
    bool first = true;
    m_t << "\n\\begin{DoxyEnumerate}";
    for (const auto &opt : s.attribs())
    {
      if (opt.name=="type")
      {
        if (opt.value=="1")
        {
          m_t << (first ?  "[": ",");
          m_t << "label=\\arabic*";
          first = false;
        }
        else if (opt.value=="a")
        {
          m_t << (first ?  "[": ",");
          m_t << "label=\\enumalphalphcnt*";
          first = false;
        }
        else if (opt.value=="A")
        {
          m_t << (first ?  "[": ",");
          m_t << "label=\\enumAlphAlphcnt*";
          first = false;
        }
        else if (opt.value=="i")
        {
          m_t << (first ?  "[": ",");
          m_t << "label=\\roman*";
          first = false;
        }
        else if (opt.value=="I")
        {
          m_t << (first ?  "[": ",");
          m_t << "label=\\Roman*";
          first = false;
        }
      }
      else if (opt.name=="start")
      {
        m_t << (first ?  "[": ",");
        bool ok = false;
        int val = opt.value.toInt(&ok);
        if (ok) m_t << "start=" << val;
        first = false;
      }
    }
    if (!first) m_t << "]\n";
  }
  else
  {
    m_t << "\n\\begin{DoxyItemize}";
  }
  visitChildren(s);
  if (m_indentLevel>=maxIndentLevels-1) return;
  if (s.type()==DocHtmlList::Ordered)
    m_t << "\n\\end{DoxyEnumerate}";
  else
    m_t << "\n\\end{DoxyItemize}";
}

void LatexDocVisitor::operator()(const DocHtmlListItem &l)
{
  if (m_hide) return;
  if (m_listItemInfo[indentLevel()].isEnum)
  {
    for (const auto &opt : l.attribs())
    {
      if (opt.name=="value")
      {
        bool ok = false;
        int val = opt.value.toInt(&ok);
        if (ok)
        {
          m_t << "\n\\setcounter{DoxyEnumerate" << integerToRoman(indentLevel()+1,false) << "}{" << (val - 1) << "}";
        }
      }
    }
  }
  m_t << "\n\\item ";
  incIndentLevel();
  visitChildren(l);
  decIndentLevel();
}


static bool classEqualsReflist(const DocHtmlDescList &dl)
{
  HtmlAttribList attrs = dl.attribs();
  auto it = std::find_if(attrs.begin(),attrs.end(),
                      [](const auto &att) { return att.name=="class"; });
  if (it!=attrs.end() && it->value == "reflist") return true;
  return false;
}

static bool listIsNested(const DocHtmlDescList &dl)
{
  bool isNested=false;
  const DocNodeVariant *n = dl.parent();
  while (n && !isNested)
  {
    if (std::get_if<DocHtmlDescList>(n))
    {
      isNested = !classEqualsReflist(std::get<DocHtmlDescList>(*n));
    }
    n = ::parent(n);
  }
  return isNested;
}

void LatexDocVisitor::operator()(const DocHtmlDescList &dl)
{
  if (m_hide) return;
  bool eq = classEqualsReflist(dl);
  if (eq)
  {
    m_t << "\n\\begin{DoxyRefList}";
  }
  else
  {
    if (listIsNested(dl)) m_t << "\n\\hfill";
    m_t << "\n\\begin{DoxyDescription}";
  }
  visitChildren(dl);
  if (eq)
  {
    m_t << "\n\\end{DoxyRefList}";
  }
  else
  {
    m_t << "\n\\end{DoxyDescription}";
  }
}

void LatexDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
  if (m_hide) return;
  m_t << "\n\\item[";
  m_insideItem=TRUE;
  visitChildren(dt);
  m_insideItem=FALSE;
  m_t << "]";
}

void LatexDocVisitor::operator()(const DocHtmlDescData &dd)
{
  incIndentLevel();
  if (!m_insideItem) m_t << "\\hfill";
  m_t << " \\\\\n";
  visitChildren(dd);
  decIndentLevel();
}

static bool tableIsNested(const DocNodeVariant *n)
{
  bool isNested=FALSE;
  while (n && !isNested)
  {
    isNested = holds_one_of_alternatives<DocHtmlTable,DocParamSect>(*n);
    n = ::parent(n);
  }
  return isNested;
}

static void writeStartTableCommand(TextStream &t,const DocNodeVariant *n,size_t cols)
{
  if (tableIsNested(n))
  {
    t << "{\\begin{tabularx}{\\linewidth}{|*{" << cols << "}{>{\\raggedright\\arraybackslash}X|}}";
  }
  else
  {
    t << "\\tabulinesep=1mm\n\\begin{longtabu}spread 0pt [c]{*{" << cols << "}{|X[-1]}|}\n";
  }
  //return isNested ? "TabularNC" : "TabularC";
}

static void writeEndTableCommand(TextStream &t,const DocNodeVariant *n)
{
  if (tableIsNested(n))
  {
    t << "\\end{tabularx}}\n";
  }
  else
  {
    t << "\\end{longtabu}\n";
  }
  //return isNested ? "TabularNC" : "TabularC";
}

void LatexDocVisitor::operator()(const DocHtmlTable &t)
{
  if (m_hide) return;
  pushTableState();
  const DocHtmlCaption *c = t.caption() ? &std::get<DocHtmlCaption>(*t.caption()) : nullptr;
  if (c)
  {
    bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
    if (!c->file().isEmpty() && pdfHyperLinks)
    {
      m_t << "\\hypertarget{" << stripPath(c->file()) << "_" << c->anchor()
        << "}{}";
    }
    m_t << "\n";
  }

  writeStartTableCommand(m_t,t.parent(),t.numColumns());

  if (c)
  {
    m_t << "\\caption{";
    std::visit(*this, *t.caption());
    m_t << "}";
    m_t << "\\label{" << stripPath(c->file()) << "_" << c->anchor() << "}";
    m_t << "\\\\\n";
  }

  setNumCols(t.numColumns());
  m_t << "\\hline\n";

  // check if first row is a heading and then render the row already here
  // and end it with \endfirsthead (triggered via m_firstRow==TRUE)
  // then repeat the row as normal and end it with \endhead (m_firstRow==FALSE)
  const DocHtmlRow *firstRow = std::get_if<DocHtmlRow>(t.firstRow());
  if (firstRow && firstRow->isHeading())
  {
    setFirstRow(TRUE);
    if (!tableIsNested(t.parent()))
    {
      std::visit(*this,*t.firstRow());
    }
    setFirstRow(FALSE);
  }
  visitChildren(t);
  writeEndTableCommand(m_t,t.parent());
  popTableState();
}

void LatexDocVisitor::operator()(const DocHtmlCaption &c)
{
  if (m_hide) return;
  visitChildren(c);
}

void LatexDocVisitor::operator()(const DocHtmlRow &row)
{
  if (m_hide) return;
  setCurrentColumn(0);

  visitChildren(row);

  size_t c=currentColumn();
  while (c<=numCols()) // end of row while inside a row span?
  {
    for (const auto &span : rowSpans())
    {
      //printf("  found row span: column=%d rs=%d cs=%d rowIdx=%d cell->rowIdx=%d i=%d c=%d\n",
      //    span->column, span->rowSpan,span->colSpan,row.rowIndex(),span->cell->rowIndex(),i,c);
      if (span.rowSpan>0 && span.column==c &&  // we are at a cell in a row span
          row.rowIndex()>span.cell.rowIndex() // but not the row that started the span
         )
      {
        m_t << "&";
        if (span.colSpan>1) // row span is also part of a column span
        {
          m_t << "\\multicolumn{" << span.colSpan << "}{";
          m_t <<  "}|}{}";
        }
        else // solitary row span
        {
          m_t << "\\multicolumn{1}{c|}{}";
        }
      }
    }
    c++;
  }

  m_t << "\\\\";

  size_t col = 1;
  for (auto &span : rowSpans())
  {
    if (span.rowSpan>0) span.rowSpan--;
    if (span.rowSpan<=0)
    {
      // inactive span
    }
    else if (span.column>col)
    {
      m_t << "\\cline{" << col << "-" << (span.column-1) << "}";
      col = span.column+span.colSpan;
    }
    else
    {
      col = span.column+span.colSpan;
    }
  }

  if (col <= numCols())
  {
    m_t << "\\cline{" << col << "-" << numCols() << "}";
  }

  m_t << "\n";

  const DocNodeVariant *n = ::parent(row.parent());
  if (row.isHeading() && row.rowIndex()==1 && !tableIsNested(n))
  {
    if (firstRow())
    {
      m_t << "\\endfirsthead\n";
      m_t << "\\hline\n";
      m_t << "\\endfoot\n";
      m_t << "\\hline\n";
    }
    else
    {
      m_t << "\\endhead\n";
    }
  }
}

void LatexDocVisitor::operator()(const DocHtmlCell &c)
{
  if (m_hide) return;

  const DocHtmlRow *row = std::get_if<DocHtmlRow>(c.parent());

  setCurrentColumn(currentColumn()+1);

  //Skip columns that span from above.
  for (const auto &span : rowSpans())
  {
    if (span.rowSpan>0 && span.column==currentColumn())
    {
      if (row && span.colSpan>1)
      {
        m_t << "\\multicolumn{" << span.colSpan << "}{";
        if (currentColumn() /*c.columnIndex()*/==1) // add extra | for first column
        {
          m_t << "|";
        }
        m_t << "l|}{" << (c.isHeading()? "\\columncolor{\\tableheadbgcolor}" : "") << "}"; // alignment not relevant, empty column
        setCurrentColumn(currentColumn()+span.colSpan);
      }
      else
      {
        setCurrentColumn(currentColumn()+1);
      }
      m_t << "&";
    }
  }

  int cs = c.colSpan();
  int a = c.alignment();
  if (cs>1 && row)
  {
    setInColSpan(TRUE);
    m_t << "\\multicolumn{" << cs << "}{";
    if (c.columnIndex()==1) // add extra | for first column
    {
      m_t << "|";
    }
    switch (a)
    {
      case DocHtmlCell::Right:
        m_t << "r|}{";
        break;
      case DocHtmlCell::Center:
        m_t << "c|}{";
        break;
      default:
        m_t << "l|}{";
        break;
    }
  }
  int rs = c.rowSpan();
  int va = c.valignment();
  if (rs>0)
  {
    setInRowSpan(TRUE);
    m_t << "\\multirow";
    switch(va)
    {
      case DocHtmlCell::Top:
        m_t << "[t]";
        break;
      case DocHtmlCell::Bottom:
        m_t << "[b]";
        break;
      case DocHtmlCell::Middle:
        break; // No alignment option needed
      default:
        break;
    }
    //printf("adding row span: cell={r=%d c=%d rs=%d cs=%d} curCol=%d\n",
    //                       c.rowIndex(),c.columnIndex(),c.rowSpan(),c.colSpan(),
    //                       currentColumn());
    addRowSpan(ActiveRowSpan(c,rs,cs,currentColumn()));
    m_t << "{" << rs << "}{*}{";
  }
  if (a==DocHtmlCell::Center)
  {
    m_t << "\\PBS\\centering ";
  }
  else if (a==DocHtmlCell::Right)
  {
    m_t << "\\PBS\\raggedleft ";
  }
  if (c.isHeading())
  {
    m_t << "\\cellcolor{\\tableheadbgcolor}\\textbf{ ";
  }
  if (cs>1)
  {
    setCurrentColumn(currentColumn()+cs-1);
  }

  visitChildren(c);

  if (c.isHeading())
  {
    m_t << "}";
  }
  if (inRowSpan())
  {
    setInRowSpan(FALSE);
    m_t << "}";
  }
  if (inColSpan())
  {
    setInColSpan(FALSE);
    m_t << "}";
  }
  if (!c.isLast()) m_t << "&";
}

void LatexDocVisitor::operator()(const DocInternal &i)
{
  if (m_hide) return;
  visitChildren(i);
}

void LatexDocVisitor::operator()(const DocHRef &href)
{
  if (m_hide) return;
  if (Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\href{";
    m_t << latexFilterURL(href.url());
    m_t << "}";
  }
  m_t << "{\\texttt{ ";
  visitChildren(href);
  m_t << "}}";
}

void LatexDocVisitor::operator()(const DocHtmlSummary &d)
{
  if (m_hide) return;
  m_t << "{\\bfseries{";
  visitChildren(d);
  m_t << "}}";
}

void LatexDocVisitor::operator()(const DocHtmlDetails &d)
{
  if (m_hide) return;
  m_t << "\n\n";
  auto summary = d.summary();
  if (summary)
  {
    std::visit(*this,*summary);
    m_t << "\\begin{adjustwidth}{1em}{0em}\n";
  }
  visitChildren(d);
  if (summary)
  {
    m_t << "\\end{adjustwidth}\n";
  }
  else
  {
    m_t << "\n\n";
  }
}

void LatexDocVisitor::operator()(const DocHtmlHeader &header)
{
  if (m_hide) return;
  m_t << "\\" << getSectionName(header.level()) << "*{";
  visitChildren(header);
  m_t << "}";
}

void LatexDocVisitor::operator()(const DocImage &img)
{
  if (img.type()==DocImage::Latex)
  {
    if (m_hide) return;
    QCString gfxName = img.name();
    if (gfxName.endsWith(".eps") || gfxName.endsWith(".pdf"))
    {
      gfxName=gfxName.left(gfxName.length()-4);
    }

    visitPreStart(m_t,img.hasCaption(), gfxName, img.width(),  img.height(), img.isInlineImage());
    visitChildren(img);
    visitPostEnd(m_t,img.hasCaption(), img.isInlineImage());
  }
  else // other format -> skip
  {
  }
}

void LatexDocVisitor::operator()(const DocDotFile &df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(LATEX_OUTPUT)+"/"+stripPath(df.file()));
  startDotFile(df.file(),df.width(),df.height(),df.hasCaption(),df.srcFile(),df.srcLine());
  visitChildren(df);
  endDotFile(df.hasCaption());
}

void LatexDocVisitor::operator()(const DocMscFile &df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(LATEX_OUTPUT)+"/"+stripPath(df.file()));
  startMscFile(df.file(),df.width(),df.height(),df.hasCaption(),df.srcFile(),df.srcLine());
  visitChildren(df);
  endMscFile(df.hasCaption());
}

void LatexDocVisitor::operator()(const DocDiaFile &df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(LATEX_OUTPUT)+"/"+stripPath(df.file()));
  startDiaFile(df.file(),df.width(),df.height(),df.hasCaption(),df.srcFile(),df.srcLine());
  visitChildren(df);
  endDiaFile(df.hasCaption());
}

void LatexDocVisitor::operator()(const DocPlantUmlFile &df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(LATEX_OUTPUT)+"/"+stripPath(df.file()));
  startPlantUmlFile(df.file(),df.width(),df.height(),df.hasCaption(),df.srcFile(),df.srcLine());
  visitChildren(df);
  endPlantUmlFile(df.hasCaption());
}

void LatexDocVisitor::operator()(const DocLink &lnk)
{
  if (m_hide) return;
  startLink(lnk.ref(),lnk.file(),lnk.anchor());
  visitChildren(lnk);
  endLink(lnk.ref(),lnk.file(),lnk.anchor());
}

void LatexDocVisitor::operator()(const DocRef &ref)
{
  if (m_hide) return;
  // when ref.isSubPage()==TRUE we use ref.file() for HTML and
  // ref.anchor() for LaTeX/RTF
  if (ref.isSubPage())
  {
    startLink(ref.ref(),QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty()) startLink(ref.ref(),ref.file(),ref.anchor(),ref.refToTable(),ref.refToSection());
  }
  if (!ref.hasLinkText())
  {
    filter(ref.targetTitle());
  }
  visitChildren(ref);
  if (ref.isSubPage())
  {
    endLink(ref.ref(),QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty()) endLink(ref.ref(),ref.file(),ref.anchor(),ref.refToTable(),ref.refToSection(),ref.sectionType());
  }
}

void LatexDocVisitor::operator()(const DocSecRefItem &ref)
{
  if (m_hide) return;
  m_t << "\\item \\contentsline{section}{";
  if (ref.isSubPage())
  {
    startLink(ref.ref(),QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty())
    {
      startLink(ref.ref(),ref.file(),ref.anchor(),ref.refToTable());
    }
  }
  visitChildren(ref);
  if (ref.isSubPage())
  {
    endLink(ref.ref(),QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty()) endLink(ref.ref(),ref.file(),ref.anchor(),ref.refToTable());
  }
  m_t << "}{\\ref{";
  if (!ref.file().isEmpty()) m_t << stripPath(ref.file());
  if (!ref.file().isEmpty() && !ref.anchor().isEmpty()) m_t << "_";
  if (!ref.anchor().isEmpty()) m_t << ref.anchor();
  m_t << "}}{}\n";
}

void LatexDocVisitor::operator()(const DocSecRefList &l)
{
  if (m_hide) return;
  m_t << "\\footnotesize\n";
  m_t << "\\begin{multicols}{2}\n";
  m_t << "\\begin{DoxyCompactList}\n";
  incIndentLevel();
  visitChildren(l);
  decIndentLevel();
  m_t << "\\end{DoxyCompactList}\n";
  m_t << "\\end{multicols}\n";
  m_t << "\\normalsize\n";
}

void LatexDocVisitor::operator()(const DocParamSect &s)
{
  if (m_hide) return;
  bool hasInOutSpecs = s.hasInOutSpecifier();
  bool hasTypeSpecs  = s.hasTypeSpecifier();
  m_lcg.incUsedTableLevel();
  switch(s.type())
  {
    case DocParamSect::Param:
      m_t << "\n\\begin{DoxyParams}";
      if      (hasInOutSpecs && hasTypeSpecs) m_t << "[2]"; // 2 extra cols
      else if (hasInOutSpecs || hasTypeSpecs) m_t << "[1]"; // 1 extra col
      m_t << "{";
      filter(theTranslator->trParameters());
      break;
    case DocParamSect::RetVal:
      m_t << "\n\\begin{DoxyRetVals}{";
      filter(theTranslator->trReturnValues());
      break;
    case DocParamSect::Exception:
      m_t << "\n\\begin{DoxyExceptions}{";
      filter(theTranslator->trExceptions());
      break;
    case DocParamSect::TemplateParam:
      m_t << "\n\\begin{DoxyTemplParams}{";
      filter(theTranslator->trTemplateParameters());
      break;
    default:
      ASSERT(0);
      incIndentLevel();
  }
  m_t << "}\n";
  visitChildren(s);
  m_lcg.decUsedTableLevel();
  switch(s.type())
  {
    case DocParamSect::Param:
      m_t << "\\end{DoxyParams}\n";
      break;
    case DocParamSect::RetVal:
      m_t << "\\end{DoxyRetVals}\n";
      break;
    case DocParamSect::Exception:
      m_t << "\\end{DoxyExceptions}\n";
      break;
    case DocParamSect::TemplateParam:
      m_t << "\\end{DoxyTemplParams}\n";
      break;
    default:
      ASSERT(0);
      decIndentLevel();
  }
}

void LatexDocVisitor::operator()(const DocSeparator &sep)
{
  m_t << " " << sep.chars() << " ";
}

void LatexDocVisitor::operator()(const DocParamList &pl)
{
  if (m_hide) return;
  DocParamSect::Type parentType = DocParamSect::Unknown;
  const DocParamSect *sect = std::get_if<DocParamSect>(pl.parent());
  if (sect)
  {
    parentType = sect->type();
  }
  bool useTable = parentType==DocParamSect::Param ||
                  parentType==DocParamSect::RetVal ||
                  parentType==DocParamSect::Exception ||
                  parentType==DocParamSect::TemplateParam;
  if (!useTable)
  {
    m_t << "\\item[";
  }
  if (sect && sect->hasInOutSpecifier())
  {
    if (pl.direction()!=DocParamSect::Unspecified)
    {
      m_t << "\\mbox{\\texttt{ ";
      if (pl.direction()==DocParamSect::In)
      {
        m_t << "in";
      }
      else if (pl.direction()==DocParamSect::Out)
      {
        m_t << "out";
      }
      else if (pl.direction()==DocParamSect::InOut)
      {
        m_t << "in,out";
      }
      m_t << "}} ";
    }
    if (useTable) m_t << " & ";
  }
  if (sect && sect->hasTypeSpecifier())
  {
    for (const auto &type : pl.paramTypes())
    {
      std::visit(*this,type);
    }
    if (useTable) m_t << " & ";
  }
  m_t << "{\\em ";
  bool first=TRUE;
  for (const auto &param : pl.parameters())
  {
    if (!first) m_t << ","; else first=FALSE;
    m_insideItem=TRUE;
    std::visit(*this,param);
    m_insideItem=FALSE;
  }
  m_t << "}";
  if (useTable)
  {
    m_t << " & ";
  }
  else
  {
    m_t << "]";
  }
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  if (useTable)
  {
    m_t << "\\\\\n"
        << "\\hline\n";
  }
}

void LatexDocVisitor::operator()(const DocXRefItem &x)
{
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (m_hide) return;
  if (x.title().isEmpty()) return;
  incIndentLevel();
  m_t << "\\begin{DoxyRefDesc}{";
  filter(x.title());
  m_t << "}\n";
  bool anonymousEnum = x.file()=="@";
  m_t << "\\item[";
  if (pdfHyperlinks && !anonymousEnum)
  {
    m_t << "\\mbox{\\hyperlink{" << stripPath(x.file()) << "_" << x.anchor() << "}{";
  }
  else
  {
    m_t << "\\textbf{ ";
  }
  m_insideItem=TRUE;
  filter(x.title());
  m_insideItem=FALSE;
  if (pdfHyperlinks && !anonymousEnum)
  {
    m_t << "}";
  }
  m_t << "}]";
  visitChildren(x);
  if (x.title().isEmpty()) return;
  decIndentLevel();
  m_t << "\\end{DoxyRefDesc}\n";
}

void LatexDocVisitor::operator()(const DocInternalRef &ref)
{
  if (m_hide) return;
  startLink(QCString(),ref.file(),ref.anchor());
  visitChildren(ref);
  endLink(QCString(),ref.file(),ref.anchor());
}

void LatexDocVisitor::operator()(const DocText &t)
{
  if (m_hide) return;
  visitChildren(t);
}

void LatexDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
  if (m_hide) return;
  m_t << "\\begin{quote}\n";
  incIndentLevel();
  visitChildren(q);
  m_t << "\\end{quote}\n";
  decIndentLevel();
}

void LatexDocVisitor::operator()(const DocVhdlFlow &)
{
}

void LatexDocVisitor::operator()(const DocParBlock &pb)
{
  if (m_hide) return;
  visitChildren(pb);
}

void LatexDocVisitor::filter(const QCString &str, const bool retainNewLine)
{
  //printf("LatexDocVisitor::filter(%s) m_insideTabbing=%d\n",qPrint(str),m_ci.insideTabbing());
  filterLatexString(m_t,str,
                    m_lcg.insideTabbing(),
                    m_insidePre,
                    m_insideItem,
                    m_lcg.usedTableLevel()>0,  // insideTable
                    false, // keepSpaces
                    retainNewLine
                   );
}

void LatexDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor,
                                bool refToTable,bool refToSection)
{
  bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
  if (ref.isEmpty() && pdfHyperLinks) // internal PDF link
  {
    if (refToTable)
    {
      m_t << "\\doxytablelink{";
    }
    else if (refToSection)
    {
      if (m_texOrPdf == TexOrPdf::TEX) m_t << "\\protect";
      if (m_texOrPdf != TexOrPdf::PDF) m_t << "\\doxysectlink{";
    }
    else
    {
      if (m_texOrPdf == TexOrPdf::TEX) m_t << "\\protect";
      if (m_texOrPdf != TexOrPdf::PDF) m_t << "\\doxylink{";
    }
    if (refToTable || m_texOrPdf != TexOrPdf::PDF)
    {
      if (!file.isEmpty()) m_t << stripPath(file);
      if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
      if (!anchor.isEmpty()) m_t << anchor;
      m_t << "}";
    }
    m_t << "{";
  }
  else if (ref.isEmpty() && refToSection)
  {
    m_t << "\\doxysectref{";
  }
  else if (ref.isEmpty() && refToTable)
  {
    m_t << "\\doxytableref{";
  }
  else if (ref.isEmpty()) // internal non-PDF link
  {
    m_t << "\\doxyref{";
  }
  else // external link
  {
    m_t << "\\textbf{ ";
  }
}

void LatexDocVisitor::endLink(const QCString &ref,const QCString &file,const QCString &anchor,bool /*refToTable*/,bool refToSection, SectionType sectionType)
{
  m_t << "}";
  bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
  if (ref.isEmpty() && !pdfHyperLinks)
  {
    m_t << "{";
    filter(theTranslator->trPageAbbreviation());
    m_t << "}{" << file;
    if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
    m_t << anchor << "}";
    if (refToSection)
    {
      m_t << "{" << sectionType.level() << "}";
    }
  }
  if (ref.isEmpty() && pdfHyperLinks) // internal PDF link
  {
    if (refToSection)
    {
      if (m_texOrPdf != TexOrPdf::PDF) m_t << "{" << sectionType.level() << "}";
    }
  }
}

void LatexDocVisitor::startDotFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption,
                                   const QCString &srcFile,
                                   int srcLine
                                  )
{
  QCString baseName=makeBaseName(fileName);
  baseName.prepend("dot_");
  QCString outDir = Config_getString(LATEX_OUTPUT);
  QCString name = fileName;
  writeDotGraphFromFile(name,outDir,baseName,GraphOutputFormat::EPS,srcFile,srcLine);
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endDotFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}

void LatexDocVisitor::startMscFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption,
                                   const QCString &srcFile,
                                   int srcLine
                                  )
{
  QCString baseName=makeBaseName(fileName);
  baseName.prepend("msc_");

  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MscOutputFormat::EPS,srcFile,srcLine);
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endMscFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}


void LatexDocVisitor::writeMscFile(const QCString &baseName, const DocVerbatim &s)
{
  QCString shortName = makeShortName(baseName);
  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MscOutputFormat::EPS,s.srcFile(),s.srcLine());
  visitPreStart(m_t, s.hasCaption(), shortName, s.width(),s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}


void LatexDocVisitor::startDiaFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption,
                                   const QCString &srcFile,
                                   int srcLine
                                  )
{
  QCString baseName=makeBaseName(fileName);
  baseName.prepend("dia_");

  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeDiaGraphFromFile(fileName,outDir,baseName,DiaOutputFormat::EPS,srcFile,srcLine);
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endDiaFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}


void LatexDocVisitor::writeDiaFile(const QCString &baseName, const DocVerbatim &s)
{
  QCString shortName = makeShortName(baseName);
  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeDiaGraphFromFile(baseName+".dia",outDir,shortName,DiaOutputFormat::EPS,s.srcFile(),s.srcLine());
  visitPreStart(m_t, s.hasCaption(), shortName, s.width(), s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void LatexDocVisitor::writePlantUMLFile(const QCString &baseName, const DocVerbatim &s)
{
  QCString shortName = makeShortName(baseName);
  if (s.useBitmap())
  {
    if (shortName.find('.')==-1) shortName += ".png";
  }
  QCString outDir = Config_getString(LATEX_OUTPUT);
  PlantumlManager::instance().generatePlantUMLOutput(baseName,outDir,
                              s.useBitmap() ? PlantumlManager::PUML_BITMAP : PlantumlManager::PUML_EPS);
  visitPreStart(m_t, s.hasCaption(), shortName, s.width(), s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void LatexDocVisitor::startPlantUmlFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption,
                                   const QCString &srcFile,
                                   int srcLine
                                  )
{
  QCString outDir = Config_getString(LATEX_OUTPUT);
  std::string inBuf;
  readInputFile(fileName,inBuf);

  bool useBitmap = inBuf.find("@startditaa") != std::string::npos;
  QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                              outDir,QCString(),inBuf.c_str(),
                              useBitmap ? PlantumlManager::PUML_BITMAP : PlantumlManager::PUML_EPS,
                              QCString(),srcFile,srcLine,false);
  baseName=makeBaseName(baseName);
  QCString shortName = makeShortName(baseName);
  if (useBitmap)
  {
    if (shortName.find('.')==-1) shortName += ".png";
  }
  PlantumlManager::instance().generatePlantUMLOutput(baseName,outDir,
                              useBitmap ? PlantumlManager::PUML_BITMAP : PlantumlManager::PUML_EPS);
  visitPreStart(m_t,hasCaption, shortName, width, height);
}

void LatexDocVisitor::endPlantUmlFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}

int LatexDocVisitor::indentLevel() const
{
  return std::min(m_indentLevel,maxIndentLevels-1);
}

void LatexDocVisitor::incIndentLevel()
{
  m_indentLevel++;
  if (m_indentLevel>=maxIndentLevels)
  {
    err("Maximum indent level (%d) exceeded while generating LaTeX output!\n",maxIndentLevels-1);
  }
}

void LatexDocVisitor::decIndentLevel()
{
  if (m_indentLevel>0)
  {
    m_indentLevel--;
  }
}

