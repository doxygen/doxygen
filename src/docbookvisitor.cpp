/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include "docbookvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "docbookgen.h"
#include "dot.h"
#include "message.h"
#include "util.h"
#include "parserintf.h"
#include "filename.h"
#include "config.h"
#include "filedef.h"
#include "msc.h"
#include "dia.h"
#include "htmlentity.h"
#include "emoji.h"
#include "plantuml.h"
#include "fileinfo.h"
#include "portable.h"
#include "codefragment.h"
#include "cite.h"
#include "md5.h"

#if 0
#define DB_VIS_C DB_VIS_C1(m_t)
#define DB_VIS_C1(x) x << "<!-- DB_VIS_C " << __LINE__ << " -->\n";
#define DB_VIS_C2(y) DB_VIS_C2a(m_t,y)
#define DB_VIS_C2a(x,y) x << "<!-- DB_VIS_C " << __LINE__ << " " << y << " -->\n";
#else
#define DB_VIS_C
#define DB_VIS_C1(x)
#define DB_VIS_C2(y)
#define DB_VIS_C2a(x,y)
#endif

static QCString filterId(const QCString &s)
{
  if (s.isEmpty()) return s;
  QCString result;
  result.reserve(s.length()+8);
  const char *p=s.data();
  char c=0;
  while ((c=*p++))
  {
    switch (c)
    {
      case ':':  result+="_1"; break;
      default:   result+=c;    break;
    }
  }
  return result;
}

static bool supportedHtmlAttribute(const QCString &name)
{
  return (name=="align" ||
          name=="bgcolor" ||
          name=="border" ||
          name=="cellpadding" ||
          name=="cellspacing" ||
          name=="class" ||
          name=="frame" ||
          name=="label" ||
          name=="style" ||
          name=="width" ||
          name=="tabstyle" ||
          name=="title");
}

void DocbookDocVisitor::visitCaption(const DocNodeList &children)
{
  for (const auto &n : children)
  {
    std::visit(*this,n);
  }
}

void DocbookDocVisitor::visitPreStart(TextStream &t,
                   const DocNodeList &children,
                   bool hasCaption,
                   const QCString &name,
                   const QCString &width,
                   const QCString &height,
                   bool inlineImage)
{
  if (hasCaption && !inlineImage)
  {
    t << "    <figure>\n";
    t << "        <title>\n";
    visitCaption(children);
    t << "        </title>\n";
  }
  else
  {
    t << "    <informalfigure>\n";
  }
  t << "        <mediaobject>\n";
  t << "            <imageobject>\n";
  t << "                <imagedata";
  if (!width.isEmpty())
  {
    t << " width=\"" << convertToDocBook(width) << "\"";
  }
  else
  {
    if (!height.isEmpty() && !inlineImage) t << " width=\"50%\"";
  }
  if (!height.isEmpty())
  {
    t << " depth=\"" << convertToDocBook(height) << "\"";
  }
  t << " align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << name << "\">";
  t << "</imagedata>\n";
  t << "            </imageobject>\n";
  if (hasCaption && !inlineImage)
  {
    t << "        <!--\n"; // Needed for general formatting with title for other formats
  }
}

void DocbookDocVisitor::visitPostEnd(TextStream &t, bool hasCaption, bool inlineImage)
{
  t << "\n";
  if (hasCaption && !inlineImage)
  {
    t << "        -->\n"; // Needed for general formatting with title for other formats
  }
  t << "        </mediaobject>\n";
  if (hasCaption && !inlineImage)
  {
    t << "    </figure>\n";
  }
  else
  {
    t << "    </informalfigure>\n";
  }
}

DocbookDocVisitor::DocbookDocVisitor(TextStream &t,OutputCodeList &ci,const QCString &langExt)
  : m_t(t), m_ci(ci),m_langExt(langExt)
{
DB_VIS_C
  // m_t << "<section>\n";
}

//--------------------------------------
// visitor functions for leaf nodes
//--------------------------------------

void DocbookDocVisitor::operator()(const DocWord &w)
{
DB_VIS_C
  if (m_hide) return;
  filter(w.word());
}

void DocbookDocVisitor::operator()(const DocLinkedWord &w)
{
DB_VIS_C
  if (m_hide) return;
  startLink(w.file(),w.anchor());
  filter(w.word());
  endLink();
}

void DocbookDocVisitor::operator()(const DocWhiteSpace &w)
{
DB_VIS_C
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

void DocbookDocVisitor::operator()(const DocSymbol &s)
{
DB_VIS_C
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance().docbook(s.symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("DocBook: non supported HTML-entity found: {}\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
  }
}

void DocbookDocVisitor::operator()(const DocEmoji &s)
{
DB_VIS_C
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance().unicode(s.index());
  if (res)
  {
    m_t << res;
  }
  else
  {
    m_t << s.name();
  }
}

void DocbookDocVisitor::operator()(const DocURL &u)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<link xlink:href=\"";
  if (u.isEmail()) m_t << "mailto:";
  filter(u.url());
  m_t << "\">";
  filter(u.url());
  m_t << "</link>";
}

void DocbookDocVisitor::operator()(const DocLineBreak &)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<?linebreak?>";
  // gives nicer results but gives problems as it is not allowed in <pare> and also problems with dblatex
  // m_t << "\n" << "<sbr/>\n";
}

void DocbookDocVisitor::operator()(const DocHorRuler &)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<informaltable frame='bottom'><tgroup cols='1'><colspec align='center'/><tbody><row><entry align='center'>\n";
  m_t << "</entry></row></tbody></tgroup></informaltable>\n";
}

void DocbookDocVisitor::operator()(const DocStyleChange &s)
{
DB_VIS_C
  if (m_hide) return;
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) m_t << "<emphasis role=\"bold\">";      else m_t << "</emphasis>";
      break;
    case DocStyleChange::Italic:
      if (s.enable()) m_t << "<emphasis>";     else m_t << "</emphasis>";
      break;
    case DocStyleChange::Kbd:
    case DocStyleChange::Typewriter:
    case DocStyleChange::Code:
      if (s.enable()) m_t << "<computeroutput>";   else m_t << "</computeroutput>";
      break;
    case DocStyleChange::Subscript:
      if (s.enable()) m_t << "<subscript>";    else m_t << "</subscript>";
      break;
    case DocStyleChange::Superscript:
      if (s.enable()) m_t << "<superscript>";    else m_t << "</superscript>";
      break;
    case DocStyleChange::Center:
      if (s.enable()) m_t << "<informaltable frame='none'><tgroup cols='1'><colspec align='center'/><tbody><row><entry align='center'>";
      else m_t << "</entry></row></tbody></tgroup></informaltable>";
      break;
    case DocStyleChange::Preformatted:
      if (s.enable())
      {
        m_t << "<literallayout>";
        m_insidePre=TRUE;
      }
      else
      {
        m_t << "</literallayout>";
        m_insidePre=FALSE;
      }
      break;
      /* There is no equivalent Docbook tag for rendering Small text */
    case DocStyleChange::Small: /* XSLT Stylesheets can be used */ break;
                                                                   /* HTML only */
    case DocStyleChange::Cite:  break;
    case DocStyleChange::S:  break;
    case DocStyleChange::Strike:  break;
    case DocStyleChange::Del:        break;
    case DocStyleChange::Underline:  break;
    case DocStyleChange::Ins:        break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void DocbookDocVisitor::operator()(const DocVerbatim &s)
{
DB_VIS_C
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
      m_t << "<literallayout><computeroutput>";
      getCodeParser(m_langExt).parseCode(m_ci,s.context(),
                                         s.text(),
                                         langExt,
                                         Config_getBool(STRIP_CODE_COMMENTS),
                                         CodeParserOptions()
                                         .setExample(s.isExample(),s.exampleFile())
                                        );
      m_t << "</computeroutput></literallayout>";
      break;
    case DocVerbatim::Verbatim:
      m_t << "<literallayout><computeroutput>";
      filter(s.text());
      m_t << "</computeroutput></literallayout>";
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s.text(), true);
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "<computeroutput>";
      filter(s.text(), true);
      m_t << "</computeroutput>";
      break;
    case DocVerbatim::HtmlOnly:
      break;
    case DocVerbatim::RtfOnly:
      break;
    case DocVerbatim::ManOnly:
      break;
    case DocVerbatim::LatexOnly:
      break;
    case DocVerbatim::XmlOnly:
      break;
    case DocVerbatim::DocbookOnly:
      m_t << s.text();
      break;
    case DocVerbatim::Dot:
      {
        m_t << "<para>\n";
        bool exists = false;
        auto fileName = writeFileContents(Config_getString(DOCBOOK_OUTPUT)+"/inline_dotgraph_", // baseName
                                          ".dot",                                               // extension
                                          s.text(),                                             // contents
                                          exists);
        if (!fileName.isEmpty())
        {
          writeDotFile(fileName, s, !exists);
        }
        m_t << "</para>\n";
      }
      break;
    case DocVerbatim::Msc:
      {
        m_t << "<para>\n";
        bool exists = false;
        auto fileName = writeFileContents(Config_getString(DOCBOOK_OUTPUT)+"/inline_mscgraph_", // baseName
                                          ".msc",                                               // extension
                                          "msc {"+s.text()+"}",                                 // contents
                                          exists);
        if (!fileName.isEmpty())
        {
          writeMscFile(fileName,s,!exists);
        }
        m_t << "</para>\n";
      }
      break;
    case DocVerbatim::PlantUML:
      {
        QCString docbookOutput = Config_getString(DOCBOOK_OUTPUT);
        auto baseNameVector = PlantumlManager::instance().writePlantUMLSource(docbookOutput,
            s.exampleFile(),s.text(),PlantumlManager::PUML_BITMAP,
            s.engine(),s.srcFile(),s.srcLine(),true);
        for (const auto &baseName: baseNameVector)
        {
          m_t << "<para>\n";
          writePlantUMLFile(baseName,s);
          m_t << "</para>\n";
        }
      }
      break;
  }
}

void DocbookDocVisitor::operator()(const DocAnchor &anc)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<anchor xml:id=\"_" <<  stripPath(anc.file()) << "_1" << filterId(anc.anchor()) << "\"/>";
}

void DocbookDocVisitor::operator()(const DocInclude &inc)
{
DB_VIS_C
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc.extension());
  switch(inc.type())
  {
    case DocInclude::IncWithLines:
      {
        m_t << "<literallayout><computeroutput>";
        FileInfo cfi( inc.file().str() );
        auto fd = createFileDef( cfi.dirPath(), cfi.fileName() );
        getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                                  inc.text(),
                                                  langExt,
                                                  inc.stripCodeComments(),
                                                  CodeParserOptions()
                                                  .setExample(inc.isExample(),inc.exampleFile())
                                                  .setFileDef(fd.get())
                                                 );
        m_t << "</computeroutput></literallayout>";
      }
      break;
    case DocInclude::Include:
      m_t << "<literallayout><computeroutput>";
      getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                                inc.text(),
                                                langExt,
                                                inc.stripCodeComments(),
                                                CodeParserOptions()
                                                .setExample(inc.isExample(),inc.exampleFile())
                                               );
      m_t << "</computeroutput></literallayout>";
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
    case DocInclude::HtmlInclude:
    case DocInclude::LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
      break;
    case DocInclude::DocbookInclude:
      m_t << inc.text();
      break;
    case DocInclude::VerbInclude:
      m_t << "<literallayout>";
      filter(inc.text());
      m_t << "</literallayout>";
      break;
    case DocInclude::Snippet:
    case DocInclude::SnippetWithLines:
      m_t << "<literallayout><computeroutput>";
      CodeFragmentManager::instance().parseCodeFragment(m_ci,
                                          inc.file(),
                                          inc.blockId(),
                                          inc.context(),
                                          inc.type()==DocInclude::SnippetWithLines,
                                          inc.trimLeft(),
                                          inc.stripCodeComments()
                                         );
      m_t << "</computeroutput></literallayout>";
      break;
  }
}

void DocbookDocVisitor::operator()(const DocIncOperator &op)
{
DB_VIS_C
  if (op.isFirst())
  {
    if (!m_hide)
    {
      m_t << "<programlisting linenumbering=\"unnumbered\">";
    }
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

      getCodeParser(locLangExt).parseCode(m_ci,op.context(),
                                        op.text(),langExt,
                                        op.stripCodeComments(),
                                        CodeParserOptions()
                                        .setExample(op.isExample(), op.exampleFile())
                                        .setFileDef(fd.get())
                                        .setStartLine(op.line())
                                        .setShowLineNumbers(op.showLineNo())
                                       );
    }
    pushHidden(m_hide);
    m_hide=TRUE;
  }
  if (op.isLast())
  {
    m_hide = popHidden();
    if (!m_hide) m_t << "</programlisting>";
  }
  else
  {
    if (!m_hide) m_t << "\n";
  }
}

void DocbookDocVisitor::operator()(const DocFormula &f)
{
DB_VIS_C
  if (m_hide) return;

  if (f.isInline()) m_t  << "<inlinemediaobject>\n";
  else m_t << "        <mediaobject>\n";
  m_t << "            <imageobject>\n";
  m_t << "                <imagedata ";
  m_t << "align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << f.relPath() << f.name() << ".png\"/>\n";
  m_t << "            </imageobject>\n";
  if (f.isInline()) m_t  << "</inlinemediaobject>\n";
  else m_t << "        </mediaobject>\n";
}

void DocbookDocVisitor::operator()(const DocIndexEntry &ie)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<indexterm><primary>";
  filter(ie.entry());
  m_t << "</primary></indexterm>\n";
}

void DocbookDocVisitor::operator()(const DocSimpleSectSep &)
{
DB_VIS_C
  // m_t << "<simplesect/>";
}

void DocbookDocVisitor::operator()(const DocCite &cite)
{
DB_VIS_C
  if (m_hide) return;
  auto opt = cite.option();
  if (!cite.file().isEmpty())
  {
    if (!opt.noCite()) startLink(cite.file(),filterId(cite.anchor()));

    filter(cite.getText());

    if (!opt.noCite()) endLink();
  }
  else
  {
    if (!opt.noPar()) filter("[");
    filter(cite.target());
    if (!opt.noPar()) filter("]");

  }

}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void DocbookDocVisitor::operator()(const DocAutoList &l)
{
DB_VIS_C
  if (m_hide) return;
  if (l.isEnumList())
  {
    m_t << "<orderedlist>\n";
  }
  else
  {
    m_t << "<itemizedlist>\n";
  }
  visitChildren(l);
  if (l.isEnumList())
  {
    m_t << "</orderedlist>\n";
  }
  else
  {
    m_t << "</itemizedlist>\n";
  }
}

void DocbookDocVisitor::operator()(const DocAutoListItem &li)
{
DB_VIS_C
  if (m_hide) return;
  switch (li.itemNumber())
  {
    case DocAutoList::Unchecked: // unchecked
      m_t << "<listitem override=\"unchecked\">";
      break;
    case DocAutoList::Checked_x: // checked with x
    case DocAutoList::Checked_X: // checked with X
      m_t << "<listitem override=\"checked\">";
      break;
    default:
      m_t << "<listitem>";
      break;
  }
  visitChildren(li);
  m_t << "</listitem>";
}

void DocbookDocVisitor::operator()(const DocPara &p)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  m_t << "<para>";
  visitChildren(p);
  m_t << "</para>";
  m_t << "\n";
}

void DocbookDocVisitor::operator()(const DocRoot &r)
{
DB_VIS_C
  visitChildren(r);
}

void DocbookDocVisitor::operator()(const DocSimpleSect &s)
{
DB_VIS_C
  if (m_hide) return;
  switch(s.type())
  {
    case DocSimpleSect::See:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trSeeAlso() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSeeAlso()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Return:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trReturns()<< "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trReturns()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Author:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, TRUE) << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, TRUE)) << "</title>\n";
      }
      break;
    case DocSimpleSect::Authors:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, FALSE) << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, FALSE)) << "</title>\n";
      }
      break;
    case DocSimpleSect::Version:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trVersion() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trVersion()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Since:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trSince() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSince()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Date:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trDate() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trDate()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Note:
      if (m_insidePre)
      {
        m_t << "<note><title>" << theTranslator->trNote() << "</title>\n";
      }
      else
      {
        m_t << "<note><title>" << convertToDocBook(theTranslator->trNote()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Warning:
      if (m_insidePre)
      {
        m_t << "<warning><title>" << theTranslator->trWarning() << "</title>\n";
      }
      else
      {
        m_t << "<warning><title>" << convertToDocBook(theTranslator->trWarning()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Pre:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trPrecondition() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPrecondition()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Post:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trPostcondition() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPostcondition()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Copyright:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trCopyright() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trCopyright()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Invar:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trInvariant() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trInvariant()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Remark:
      // <remark> is miising the <title> possibility
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trRemarks() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trRemarks()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Attention:
      if (m_insidePre)
      {
        m_t << "<caution><title>" << theTranslator->trAttention() << "</title>\n";
      }
      else
      {
        m_t << "<caution><title>" << convertToDocBook(theTranslator->trAttention()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Important:
      if (m_insidePre)
      {
        m_t << "<important><title>" << theTranslator->trImportant() << "</title>\n";
      }
      else
      {
        m_t << "<important><title>" << convertToDocBook(theTranslator->trImportant()) << "</title>\n";
      }
      break;
    case DocSimpleSect::User:
    case DocSimpleSect::Rcs:
    case DocSimpleSect::Unknown:
      if (s.hasTitle())
        m_t << "<formalpara>\n";
      else
        m_t << "<para>\n";
      break;
  }

  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);

  switch(s.type())
  {
    case DocSimpleSect::User:
    case DocSimpleSect::Rcs:
    case DocSimpleSect::Unknown:
      if (s.hasTitle())
        m_t << "</formalpara>\n";
      else
        m_t << "</para>\n";
      break;
    case DocSimpleSect::Note:
      m_t << "</note>\n";
      break;
    case DocSimpleSect::Attention:
      m_t << "</caution>\n";
      break;
    case DocSimpleSect::Important:
      m_t << "</important>\n";
      break;
    case DocSimpleSect::Warning:
      m_t << "</warning>\n";
      break;
    default:
      m_t << "</formalpara>\n";
      break;
  }
}

void DocbookDocVisitor::operator()(const DocTitle &t)
{
DB_VIS_C
  if (m_hide) return;
  if (t.hasTitle()) m_t << "<title>";
  visitChildren(t);
  if (t.hasTitle()) m_t << "</title>";
}

void DocbookDocVisitor::operator()(const DocSimpleList &l)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
  visitChildren(l);
  m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::operator()(const DocSimpleListItem &li)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<listitem>";
  if (li.paragraph())
  {
    visit(*this,*li.paragraph());
  }
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::operator()(const DocSection &s)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<section xml:id=\"_" <<  stripPath(s.file());
  if (!s.anchor().isEmpty()) m_t << "_1" << s.anchor();
  m_t << "\">\n";
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);
  m_t << "</section>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlList &s)
{
DB_VIS_C
  if (m_hide) return;
  if (s.children().empty()) return;
  // opening tag for ordered list will be handled in DocHtmlListItem
  // due to (re-)numbering possibilities
  if (s.type()!=DocHtmlList::Ordered)
    m_t << "<itemizedlist>\n";
  visitChildren(s);
  if (s.type()==DocHtmlList::Ordered)
    m_t << "</orderedlist>\n";
  else
    m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlListItem &s)
{
DB_VIS_C
  if (m_hide) return;
  const DocHtmlList *l = std::get_if<DocHtmlList>(s.parent());
  if (l->type()==DocHtmlList::Ordered)
  {
    bool isFirst = &std::get<DocHtmlListItem>(l->children().front())==&s;
    int value = 0;
    QCString type;
    for (const auto &opt : s.attribs())
    {
      if (opt.name=="value")
      {
        bool ok = false;
        int val = opt.value.toInt(&ok);
        if (ok) value = val;
      }
    }

    if (value>0 || isFirst)
    {
      for (const auto &opt : l->attribs())
      {
        if (opt.name=="type")
        {
          if (opt.value=="1")
            type = " numeration=\"arabic\"";
          else if (opt.value=="a")
            type = " numeration=\"loweralpha\"";
            else if (opt.value=="A")
            type =  " numeration=\"upperalpha\"";
          else if (opt.value=="i")
            type =  " numeration=\"lowerroman\"";
          else if (opt.value=="I")
            type =  " numeration=\"upperroman\"";
        }
        else if (value==0 && opt.name=="start")
        {
          bool ok = false;
          int val = opt.value.toInt(&ok);
          if (ok) value = val;
        }
      }
    }

    if (value>0 && !isFirst)
    {
      m_t << "</orderedlist>\n";
    }
    if (value>0 || isFirst)
    {
      m_t << "<orderedlist";
      if (!type.isEmpty()) m_t << type.data();
      if (value>0)         m_t << " startingnumber=\"" << value << "\"";
      m_t << ">\n";
    }
  }
  m_t << "<listitem>\n";
  visitChildren(s);
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlDescList &l)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<variablelist>\n";
  visitChildren(l);
  m_t << "</variablelist>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<varlistentry><term>";
  visitChildren(dt);
  m_t << "</term></varlistentry>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlDescData &dd)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<listitem>";
  visitChildren(dd);
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlTable &t)
{
DB_VIS_C
  m_bodySet.push(false);
  if (m_hide) return;
  m_t << "<informaltable frame=\"all\">\n";
  m_t << "    <tgroup cols=\"" << t.numColumns() << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (uint32_t i = 0; i <t.numColumns(); i++)
  {
    // do something with colwidth based of cell width specification (be aware of possible colspan in the header)?
    m_t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
  if (t.caption())
  {
    std::visit(*this,*t.caption());
  }
  visitChildren(t);
  if (m_bodySet.top()) m_t << "    </tbody>\n";
  m_bodySet.pop();
  m_t << "    </tgroup>\n";
  m_t << "</informaltable>\n";
}

void DocbookDocVisitor::operator()(const DocHtmlRow &tr)
{
DB_VIS_C
  m_colCnt = 0;
  if (m_hide) return;

  if (tr.isHeading())
  {
    if (m_bodySet.top()) m_t << "</tbody>\n";
    m_bodySet.top() = false;
    m_t << "<thead>\n";
  }
  else if (!m_bodySet.top())
  {
    m_bodySet.top() = true;
    m_t << "<tbody>\n";
  }

  m_t << "      <row ";

  for (const auto &opt : tr.attribs())
  {
    if (supportedHtmlAttribute(opt.name))
    {
      // process supported attributes only
      m_t << " " << opt.name << "='" << convertToDocBook(opt.value) << "'";
    }
  }
  m_t << ">\n";
  visitChildren(tr);
  m_t << "</row>\n";
  if (tr.isHeading())
  {
    m_t << "</thead><tbody>\n";
    m_bodySet.top() = true;
  }
}

void DocbookDocVisitor::operator()(const DocHtmlCell &c)
{
DB_VIS_C
  m_colCnt++;
  if (m_hide) return;
  m_t << "<entry";

  for (const auto &opt : c.attribs())
  {
    if (opt.name=="colspan")
    {
      m_t << " namest='c" << m_colCnt << "'";
      int cols = opt.value.toInt();
      m_colCnt += (cols - 1);
      m_t << " nameend='c" << m_colCnt << "'";
    }
    else if (opt.name=="rowspan")
    {
      int extraRows = opt.value.toInt() - 1;
      m_t << " morerows='" << extraRows << "'";
    }
    else if (opt.name=="class")
    {
      if (opt.value.startsWith("markdownTable")) // handle markdown generated attributes
      {
        if (opt.value.endsWith("Right"))
        {
          m_t << " align='right'";
        }
        else if (opt.value.endsWith("Left"))
        {
          m_t << " align='left'";
        }
        else if (opt.value.endsWith("Center"))
        {
          m_t << " align='center'";
        }
        // skip 'markdownTable*' value ending with "None"
      }
      else
      {
        m_t << " class='" << convertToDocBook(opt.value) << "'";
      }
    }
    else if (supportedHtmlAttribute(opt.name))
    {
      // process supported attributes only
      m_t << " " << opt.name << "='" << convertToDocBook(opt.value) << "'";
    }
  }
  m_t << ">";
  visitChildren(c);
  m_t << "</entry>";
}

void DocbookDocVisitor::operator()(const DocHtmlCaption &c)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<caption>";
  if (!c.file().isEmpty())
  {
    m_t << "<anchor xml:id=\"_" <<  stripPath(c.file()) << "_1" << filterId(c.anchor()) << "\"/>";
  }
  visitChildren(c);
  m_t << "</caption>\n";
}

void DocbookDocVisitor::operator()(const DocInternal &i)
{
DB_VIS_C
  if (m_hide) return;
  visitChildren(i);
}

void DocbookDocVisitor::operator()(const DocHRef &href)
{
DB_VIS_C
  if (m_hide) return;
  if (href.url().at(0) != '#')
  {
    m_t << "<link xlink:href=\"" << convertToDocBook(href.url()) << "\">";
  }
  else
  {
    startLink(href.file(),filterId(href.url().mid(1)));
  }
  visitChildren(href);
  m_t << "</link>";
}

void DocbookDocVisitor::operator()(const DocHtmlSummary &s)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<para><emphasis role=\"bold\">";
  visitChildren(s);
  m_t << "</emphasis></para>";
}

void DocbookDocVisitor::operator()(const DocHtmlDetails &d)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  auto summary = d.summary();
  if (summary)
  {
    std::visit(*this,*summary);
  }
  m_t << "<para>";
  visitChildren(d);
  m_t << "</para>";
  m_t << "\n";
}

void DocbookDocVisitor::operator()(const DocHtmlHeader &h)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<formalpara><title>";
  visitChildren(h);
  m_t << "</title></formalpara>\n";
}

void DocbookDocVisitor::operator()(const DocImage &img)
{
DB_VIS_C
  if (img.type()==DocImage::DocBook)
  {
    if (m_hide) return;
    m_t << "\n";
    QCString baseName=stripPath(img.name());
    visitPreStart(m_t, img.children(), img.hasCaption(), img.relPath() + baseName, img.width(), img.height(), img.isInlineImage());
    visitChildren(img);
    visitPostEnd(m_t, img.hasCaption(),img.isInlineImage());
    QCString file;
    bool ambig = false;
    FileDef *fd=findFileDef(Doxygen::imageNameLinkedMap, baseName, ambig);
    if (fd)
    {
      file=fd->absFilePath();
    }
    copyFile(file,Config_getString(DOCBOOK_OUTPUT)+"/"+baseName);
  }
  else // skip other formats
  {
  }
}

void DocbookDocVisitor::operator()(const DocDotFile &df)
{
DB_VIS_C
  if (m_hide) return;
  bool exists = false;
  std::string inBuf;
  if (readInputFile(df.file(),inBuf))
  {
    auto fileName = writeFileContents(Config_getString(DOCBOOK_OUTPUT)+"/"+stripPath(df.file())+"_", // baseName
                                      ".dot",                                                        // extension
                                      inBuf,                                                         // contents
                                      exists);
    if (!fileName.isEmpty())
    {
      startDotFile(fileName,df.relPath(),df.width(),df.height(),df.hasCaption(),df.children(),df.srcFile(),df.srcLine(),!exists);
      visitChildren(df);
      endDotFile(df.hasCaption());
    }
  }
}

void DocbookDocVisitor::operator()(const DocMscFile &df)
{
DB_VIS_C
  if (m_hide) return;
  bool exists = false;
  std::string inBuf;
  if (readInputFile(df.file(),inBuf))
  {
    auto fileName = writeFileContents(Config_getString(DOCBOOK_OUTPUT)+"/"+stripPath(df.file())+"_", // baseName
                                      ".msc",                                                        // extension
                                      inBuf,                                                         // contents
                                      exists);
    if (!fileName.isEmpty())
    {
      startMscFile(fileName,df.relPath(),df.width(),df.height(),df.hasCaption(),df.children(),df.srcFile(),df.srcLine(),!exists);
      visitChildren(df);
      endMscFile(df.hasCaption());
    }
  }
}

void DocbookDocVisitor::operator()(const DocDiaFile &df)
{
DB_VIS_C
  if (m_hide) return;
  bool exists = false;
  std::string inBuf;
  if (readInputFile(df.file(),inBuf))
  {
    auto fileName = writeFileContents(Config_getString(DOCBOOK_OUTPUT)+"/"+stripPath(df.file())+"_", // baseName
                                      ".dia",                                                        // extension
                                      inBuf,                                                         // contents
                                      exists);
    if (!fileName.isEmpty())
    {
      startDiaFile(fileName,df.relPath(),df.width(),df.height(),df.hasCaption(),df.children(),df.srcFile(),df.srcLine(),!exists);
      visitChildren(df);
      endDiaFile(df.hasCaption());
    }
  }
}

void DocbookDocVisitor::operator()(const DocPlantUmlFile &df)
{
DB_VIS_C
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(DOCBOOK_OUTPUT)+"/"+stripPath(df.file()));
  startPlantUmlFile(df.file(),df.relPath(),df.width(),df.height(),df.hasCaption(),df.children(),df.srcFile(),df.srcLine());
  visitChildren(df);
  endPlantUmlFile(df.hasCaption());
}

void DocbookDocVisitor::operator()(const DocLink &lnk)
{
DB_VIS_C
  if (m_hide) return;
  startLink(lnk.file(),lnk.anchor());
  visitChildren(lnk);
  endLink();
}

void DocbookDocVisitor::operator()(const DocRef &ref)
{
DB_VIS_C
  if (m_hide) return;
  if (ref.isSubPage())
  {
    startLink(QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty()) startLink(ref.file(),ref.anchor());
  }

  if (!ref.hasLinkText()) filter(ref.targetTitle());
  visitChildren(ref);
  if (!ref.file().isEmpty()) endLink();
}

void DocbookDocVisitor::operator()(const DocSecRefItem &ref)
{
DB_VIS_C
  if (m_hide) return;
  //m_t << "<tocentry xml:idref=\"_" <<  stripPath(ref->file()) << "_1" << ref->anchor() << "\">";
  m_t << "<tocentry>";
  visitChildren(ref);
  m_t << "</tocentry>\n";
}

void DocbookDocVisitor::operator()(const DocSecRefList &l)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<toc>\n";
  visitChildren(l);
  m_t << "</toc>\n";
}

void DocbookDocVisitor::operator()(const DocParamSect &s)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  m_t << "                <formalpara>\n";
  m_t << "                    <title>\n";
  switch(s.type())
  {
    case DocParamSect::Param:         m_t << theTranslator->trParameters();         break;
    case DocParamSect::RetVal:        m_t << theTranslator->trReturnValues();       break;
    case DocParamSect::Exception:     m_t << theTranslator->trExceptions();         break;
    case DocParamSect::TemplateParam: m_t << theTranslator->trTemplateParameters(); break;
    default:
      ASSERT(0);
  }
  m_t << "</title>\n";
  m_t << "                    <para>\n";
  m_t << "                    <table frame=\"all\">\n";
  int ncols = 2;
  if (s.type() == DocParamSect::Param)
  {
    bool hasInOutSpecs = s.hasInOutSpecifier();
    bool hasTypeSpecs  = s.hasTypeSpecifier();
    if      (hasInOutSpecs && hasTypeSpecs) ncols += 2;
    else if (hasInOutSpecs || hasTypeSpecs) ncols += 1;
  }
  m_t << "                        <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (int i = 1; i <= ncols; i++)
  {
    if (i == ncols) m_t << "                        <colspec colwidth=\"4*\"/>\n";
    else            m_t << "                        <colspec colwidth=\"1*\"/>\n";
  }
  m_t << "                        <tbody>\n";
  visitChildren(s);
  m_t << "                        </tbody>\n";
  m_t << "                        </tgroup>\n";
  m_t << "                    </table>\n";
  m_t << "                    </para>\n";
  m_t << "                </formalpara>\n";
  m_t << "                ";
}

void DocbookDocVisitor::operator()(const DocSeparator &sep)
{
DB_VIS_C
  if (m_hide) return;
  m_t << " " << sep.chars() << " ";
}

void DocbookDocVisitor::operator()(const DocParamList &pl)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "                            <row>\n";

  const DocParamSect *sect = std::get_if<DocParamSect>(pl.parent());
  if (sect && sect->hasInOutSpecifier())
  {
    m_t << "<entry>";
    if (pl.direction()!=DocParamSect::Unspecified)
    {
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
    }
    m_t << "</entry>";
  }

  if (sect && sect->hasTypeSpecifier())
  {
    m_t << "<entry>";
    for (const auto &type : pl.paramTypes())
    {
      std::visit(*this,type);
    }
    m_t << "</entry>";
  }

  if (pl.parameters().empty())
  {
    m_t << "<entry></entry>\n";
  }
  else
  {
    m_t << "<entry>";
    int cnt = 0;
    for (const auto &param : pl.parameters())
    {
      if (cnt)
      {
        m_t << ", ";
      }
      std::visit(*this,param);
      cnt++;
    }
    m_t << "</entry>";
  }
  m_t << "<entry>";
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  m_t << "</entry>\n";
  m_t << "                            </row>\n";
}

void DocbookDocVisitor::operator()(const DocXRefItem &x)
{
DB_VIS_C
  if (m_hide) return;
  if (x.title().isEmpty()) return;
  m_t << "<para><link linkend=\"_";
  m_t << stripPath(x.file()) << "_1" << x.anchor();
  m_t << "\">";
  filter(x.title());
  m_t << "</link>";
  m_t << " ";
  visitChildren(x);
  if (x.title().isEmpty()) return;
  m_t << "</para>";
}

void DocbookDocVisitor::operator()(const DocInternalRef &ref)
{
DB_VIS_C
  if (m_hide) return;
  startLink(ref.file(),ref.anchor());
  visitChildren(ref);
  endLink();
  m_t << " ";
}

void DocbookDocVisitor::operator()(const DocText &t)
{
DB_VIS_C
  visitChildren(t);
}


void DocbookDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<blockquote>";
  visitChildren(q);
  m_t << "</blockquote>";
}

void DocbookDocVisitor::operator()(const DocVhdlFlow &)
{
DB_VIS_C
  // TODO: to be implemented
}

void DocbookDocVisitor::operator()(const DocParBlock &b)
{
DB_VIS_C
  if (m_hide) return;
  visitChildren(b);
}


void DocbookDocVisitor::filter(const QCString &str, const bool retainNewLine)
{
DB_VIS_C
  m_t << convertToDocBook(str, retainNewLine);
}

void DocbookDocVisitor::startLink(const QCString &file,const QCString &anchor)
{
DB_VIS_C
  m_t << "<link linkend=\"_" << stripPath(file);
  if (!anchor.isEmpty())
  {
    if (!file.isEmpty()) m_t << "_1";
    m_t << anchor;
  }
  m_t << "\">";
}

void DocbookDocVisitor::endLink()
{
DB_VIS_C
  m_t << "</link>";
}

void DocbookDocVisitor::writeMscFile(const QCString &fileName, const DocVerbatim &s, bool newFile)
{
DB_VIS_C
  QCString shortName = makeBaseName(fileName,".msc");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  if (newFile) writeMscGraphFromFile(fileName,outDir,shortName,MscOutputFormat::BITMAP,s.srcFile(),s.srcLine());
  visitPreStart(m_t, s.children(), s.hasCaption(), s.relPath() + shortName + ".png", s.width(), s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void DocbookDocVisitor::writePlantUMLFile(const QCString &baseName, const DocVerbatim &s)
{
DB_VIS_C
  QCString shortName = stripPath(baseName);
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  PlantumlManager::instance().generatePlantUMLOutput(baseName,outDir,PlantumlManager::PUML_BITMAP);
  visitPreStart(m_t, s.children(), s.hasCaption(), s.relPath() + shortName + ".png", s.width(),s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void DocbookDocVisitor::startPlantUmlFile(const QCString &fileName,
    const QCString &relPath,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine
    )
{
DB_VIS_C
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  std::string inBuf;
  readInputFile(fileName,inBuf);
  auto baseNameVector = PlantumlManager::instance().writePlantUMLSource(outDir,
                           QCString(),inBuf,PlantumlManager::PUML_BITMAP,QCString(),srcFile,srcLine,false);
  bool first = true;
  for (const auto &bName: baseNameVector)
  {
    QCString baseName=makeBaseName(bName,".pu");
    PlantumlManager::instance().generatePlantUMLOutput(baseName,outDir,PlantumlManager::PUML_BITMAP);
    if (!first) endPlantUmlFile(hasCaption);
    first = false;
    m_t << "<para>\n";
    visitPreStart(m_t, children, hasCaption, relPath + baseName + ".png",  width,  height);
  }
}

void DocbookDocVisitor::endPlantUmlFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

void DocbookDocVisitor::startMscFile(const QCString &fileName,
    const QCString &relPath,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine, bool newFile
    )
{
DB_VIS_C
  QCString baseName=makeBaseName(fileName,".msc");
  baseName.prepend("msc_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  if (newFile) writeMscGraphFromFile(fileName,outDir,baseName,MscOutputFormat::BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, relPath + baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endMscFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

void DocbookDocVisitor::writeDiaFile(const QCString &baseName, const DocVerbatim &s)
{
DB_VIS_C
  QCString shortName = stripPath(baseName);
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDiaGraphFromFile(baseName+".dia",outDir,shortName,DiaOutputFormat::BITMAP,s.srcFile(),s.srcLine());
  visitPreStart(m_t, s.children(), s.hasCaption(), shortName, s.width(),s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void DocbookDocVisitor::startDiaFile(const QCString &fileName,
    const QCString &relPath,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine, bool newFile
    )
{
DB_VIS_C
  QCString baseName=makeBaseName(fileName,".dia");
  baseName.prepend("dia_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  if (newFile) writeDiaGraphFromFile(fileName,outDir,baseName,DiaOutputFormat::BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, relPath + baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endDiaFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

void DocbookDocVisitor::writeDotFile(const QCString &fileName, const DocVerbatim &s, bool newFile)
{
DB_VIS_C
  QCString shortName = makeBaseName(fileName,".dot");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  if (newFile) writeDotGraphFromFile(fileName,outDir,shortName,GraphOutputFormat::BITMAP,s.srcFile(),s.srcLine());
  visitPreStart(m_t, s.children(), s.hasCaption(), s.relPath() + shortName + "." + getDotImageExtension(), s.width(),s.height());
  visitCaption(s.children());
  visitPostEnd(m_t, s.hasCaption());
}

void DocbookDocVisitor::startDotFile(const QCString &fileName,
    const QCString &relPath,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine, bool newFile
    )
{
DB_VIS_C
  QCString baseName=makeBaseName(fileName,".dot");
  baseName.prepend("dot_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  QCString imgExt = getDotImageExtension();
  if (newFile) writeDotGraphFromFile(fileName,outDir,baseName,GraphOutputFormat::BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, relPath + baseName + "." + imgExt,  width,  height);
}

void DocbookDocVisitor::endDotFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

