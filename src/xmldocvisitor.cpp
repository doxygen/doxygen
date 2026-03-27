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

#include "xmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "xmlgen.h"
#include "dot.h"
#include "message.h"
#include "util.h"
#include "parserintf.h"
#include "filename.h"
#include "config.h"
#include "htmlentity.h"
#include "emoji.h"
#include "filedef.h"
#include "fileinfo.h"
#include "codefragment.h"
#include "cite.h"

static void startSimpleSect(TextStream &t,const DocSimpleSect &s)
{
  t << "<simplesect kind=\"";
  switch(s.type())
  {
    case DocSimpleSect::See:
      t << "see"; break;
    case DocSimpleSect::Return:
      t << "return"; break;
    case DocSimpleSect::Author:
      t << "author"; break;
    case DocSimpleSect::Authors:
      t << "authors"; break;
    case DocSimpleSect::Version:
      t << "version"; break;
    case DocSimpleSect::Since:
      t << "since"; break;
    case DocSimpleSect::Date:
      t << "date"; break;
    case DocSimpleSect::Note:
      t << "note"; break;
    case DocSimpleSect::Warning:
      t << "warning"; break;
    case DocSimpleSect::Pre:
      t << "pre"; break;
    case DocSimpleSect::Post:
      t << "post"; break;
    case DocSimpleSect::Copyright:
      t << "copyright"; break;
    case DocSimpleSect::Invar:
      t << "invariant"; break;
    case DocSimpleSect::Remark:
      t << "remark"; break;
    case DocSimpleSect::Attention:
      t << "attention"; break;
    case DocSimpleSect::Important:
      t << "important"; break;
    case DocSimpleSect::User:
      t << "par"; break;
    case DocSimpleSect::Rcs:
      t << "rcs"; break;
    case DocSimpleSect::Unknown:  break;
  }
  t << "\">";
}

static void endSimpleSect(TextStream &t,const DocSimpleSect &)
{
  t << "</simplesect>\n";
}

static void visitCaption(XmlDocVisitor &visitor, const DocNodeList &children)
{
  for (const auto &n : children)
  {
    std::visit(visitor,n);
  }
}

static void visitPreStart(TextStream &t, const char *cmd, bool doCaption,
                          XmlDocVisitor &visitor, const DocNodeList &children,
                          const QCString &name, bool writeType, DocImage::Type type, const QCString &width,
                          const QCString &height, const QCString engine = QCString(), const QCString &alt = QCString(), bool inlineImage = FALSE)
{
  t << "<" << cmd;
  if (writeType)
  {
    t << " type=\"";
    switch(type)
    {
      case DocImage::Html:    t << "html"; break;
      case DocImage::Latex:   t << "latex"; break;
      case DocImage::Rtf:     t << "rtf"; break;
      case DocImage::DocBook: t << "docbook"; break;
      case DocImage::Xml:     t << "xml"; break;
    }
    t << "\"";
  }
  if (!name.isEmpty())
  {
    t << " name=\"" << convertToXML(name, TRUE) << "\"";
  }
  if (!width.isEmpty())
  {
    t << " width=\"" << convertToXML(width) << "\"";
  }
  if (!height.isEmpty())
  {
    t << " height=\"" << convertToXML(height) << "\"";
  }
  if (!engine.isEmpty())
  {
    t << " engine=\"" << convertToXML(engine) << "\"";
  }
  if (!alt.isEmpty())
  {
    t << " alt=\"" << convertToXML(alt) << "\"";
  }
  if (inlineImage)
  {
    t << " inline=\"yes\"";
  }
  if (doCaption)
  {
    t << " caption=\"";
    visitCaption(visitor, children);
    t << "\"";
  }
  t << ">";
}

static void visitPostEnd(TextStream &t, const char *cmd)
{
  t << "</" << cmd << ">\n";
}

XmlDocVisitor::XmlDocVisitor(TextStream &t,OutputCodeList &ci,const QCString &langExt)
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE),
    m_langExt(langExt), m_sectionLevel(0)
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void XmlDocVisitor::operator()(const DocWord &w)
{
  if (m_hide) return;
  filter(w.word());
}

void XmlDocVisitor::operator()(const DocLinkedWord &w)
{
  if (m_hide) return;
  startLink(w.ref(),w.file(),w.anchor());
  filter(w.word());
  endLink();
}

void XmlDocVisitor::operator()(const DocWhiteSpace &w)
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

void XmlDocVisitor::operator()(const DocSymbol &s)
{
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance().xml(s.symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("XML: non supported HTML-entity found: {}\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
  }
}

void XmlDocVisitor::operator()(const DocEmoji &s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance().name(s.index());
  if (res)
  {
    QCString name=res;
    name = name.mid(1,name.length()-2);
    m_t << "<emoji name=\"" << name << "\" unicode=\"";
    filter(EmojiEntityMapper::instance().unicode(s.index()));
    m_t << "\"/>";
  }
  else
  {
    m_t << s.name();
  }
}

void XmlDocVisitor::operator()(const DocURL &u)
{
  if (m_hide) return;
  m_t << "<ulink url=\"";
  if (u.isEmail()) m_t << "mailto:";
  filter(u.url());
  m_t << "\">";
  filter(u.url());
  m_t << "</ulink>";
}

void XmlDocVisitor::operator()(const DocLineBreak &)
{
  if (m_hide) return;
  m_t << "<linebreak/>\n";
}

void XmlDocVisitor::operator()(const DocHorRuler &)
{
  if (m_hide) return;
  m_t << "<hruler/>\n";
}

void XmlDocVisitor::operator()(const DocStyleChange &s)
{
  if (m_hide) return;
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) m_t << "<bold>";      else m_t << "</bold>";
      break;
    case DocStyleChange::S:
      if (s.enable()) m_t << "<s>";      else m_t << "</s>";
      break;
    case DocStyleChange::Strike:
      if (s.enable()) m_t << "<strike>";      else m_t << "</strike>";
      break;
    case DocStyleChange::Del:
      if (s.enable()) m_t << "<del>";      else m_t << "</del>";
      break;
    case DocStyleChange::Underline:
      if (s.enable()) m_t << "<underline>";      else m_t << "</underline>";
      break;
    case DocStyleChange::Ins:
      if (s.enable()) m_t << "<ins>";      else m_t << "</ins>";
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
      if (s.enable()) m_t << "<center>"; else m_t << "</center>";
      break;
    case DocStyleChange::Small:
      if (s.enable()) m_t << "<small>";  else m_t << "</small>";
      break;
    case DocStyleChange::Cite:
      if (s.enable()) m_t << "<cite>";  else m_t << "</cite>";
      break;
    case DocStyleChange::Preformatted:
      if (s.enable())
      {
        m_t << "<preformatted>";
        m_insidePre=TRUE;
      }
      else
      {
        m_t << "</preformatted>";
        m_insidePre=FALSE;
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void XmlDocVisitor::operator()(const DocVerbatim &s)
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
      m_t << "<programlisting";
      if (!s.language().isEmpty())
          m_t << " filename=\"" << lang << "\">";
      else
          m_t << ">";
      getCodeParser(lang).parseCode(m_ci,s.context(),s.text(),langExt,
                                    Config_getBool(STRIP_CODE_COMMENTS),
                                    CodeParserOptions().setExample(s.isExample(),s.exampleFile()));
      m_t << "</programlisting>";
      break;
    case DocVerbatim::JavaDocLiteral:
      m_t << "<javadocliteral>";
      filter(s.text());
      m_t << "</javadocliteral>";
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "<javadoccode>";
      filter(s.text());
      m_t << "</javadoccode>";
      break;
    case DocVerbatim::Verbatim:
      m_t << "<verbatim>";
      filter(s.text());
      m_t << "</verbatim>";
      break;
    case DocVerbatim::HtmlOnly:
      if (s.isBlock())
      {
        m_t << "<htmlonly block=\"yes\">";
      }
      else
      {
        m_t << "<htmlonly>";
      }
      filter(s.text());
      m_t << "</htmlonly>";
      break;
    case DocVerbatim::RtfOnly:
      m_t << "<rtfonly>";
      filter(s.text());
      m_t << "</rtfonly>";
      break;
    case DocVerbatim::ManOnly:
      m_t << "<manonly>";
      filter(s.text());
      m_t << "</manonly>";
      break;
    case DocVerbatim::LatexOnly:
      m_t << "<latexonly>";
      filter(s.text());
      m_t << "</latexonly>";
      break;
    case DocVerbatim::DocbookOnly:
      m_t << "<docbookonly>";
      filter(s.text());
      m_t << "</docbookonly>";
      break;
    case DocVerbatim::XmlOnly:
      m_t << s.text();
      break;
    case DocVerbatim::Dot:
      visitPreStart(m_t, "dot", s.hasCaption(), *this, s.children(), QCString(""), FALSE, DocImage::Html, s.width(), s.height());
      filter(s.text());
      visitPostEnd(m_t, "dot");
      break;
    case DocVerbatim::Msc:
      visitPreStart(m_t, "msc", s.hasCaption(), *this, s.children(),  QCString(""), FALSE, DocImage::Html, s.width(), s.height());
      filter(s.text());
      visitPostEnd(m_t, "msc");
      break;
    case DocVerbatim::PlantUML:
      visitPreStart(m_t, "plantuml", s.hasCaption(), *this, s.children(),  QCString(""), FALSE, DocImage::Html, s.width(), s.height(), s.engine());
      filter(s.text());
      visitPostEnd(m_t, "plantuml");
      break;
  }
}

void XmlDocVisitor::operator()(const DocAnchor &anc)
{
  if (m_hide) return;
  m_t << "<anchor id=\"" << anc.file() << "_1" << anc.anchor() << "\"/>";
}

void XmlDocVisitor::operator()(const DocInclude &inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc.extension());
  //printf("XMLDocVisitor: DocInclude type=%d trimleft=%d\n",inc.type(),inc.trimLeft());
  switch(inc.type())
  {
    case DocInclude::IncWithLines:
      {
         m_t << "<programlisting filename=\"" << inc.file() << "\">";
         FileInfo cfi( inc.file().str() );
         auto fd = createFileDef( cfi.dirPath(), cfi.fileName());
         getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                           inc.text(),
                                           langExt,
                                           inc.stripCodeComments(),
                                           CodeParserOptions()
                                           .setExample(inc.isExample(),inc.exampleFile())
                                           .setFileDef(fd.get())
                                           .setInlineFragment(true)
					   );
         m_t << "</programlisting>";
      }
      break;
    case DocInclude::Include:
      m_t << "<programlisting filename=\"" << inc.file() << "\">";
      getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                        inc.text(),
                                        langExt,
                                        inc.stripCodeComments(),
                                        CodeParserOptions()
                                        .setExample(inc.isExample(),inc.exampleFile())
                                        .setInlineFragment(true)
                                        .setShowLineNumbers(false)
				       );
      m_t << "</programlisting>";
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
      break;
    case DocInclude::HtmlInclude:
      if (inc.isBlock())
      {
        m_t << "<htmlonly block=\"yes\">";
      }
      else
      {
        m_t << "<htmlonly>";
      }
      filter(inc.text());
      m_t << "</htmlonly>";
      break;
    case DocInclude::LatexInclude:
      m_t << "<latexonly>";
      filter(inc.text());
      m_t << "</latexonly>";
      break;
    case DocInclude::RtfInclude:
      m_t << "<rtfonly>";
      filter(inc.text());
      m_t << "</rtfonly>";
      break;
    case DocInclude::ManInclude:
      m_t << "<manonly>";
      filter(inc.text());
      m_t << "</manonly>";
      break;
    case DocInclude::XmlInclude:
      filter(inc.text());
      break;
    case DocInclude::DocbookInclude:
      m_t << "<docbookonly>";
      filter(inc.text());
      m_t << "</docbookonly>";
      break;
    case DocInclude::VerbInclude:
      m_t << "<verbatim>";
      filter(inc.text());
      m_t << "</verbatim>";
      break;
    case DocInclude::Snippet:
    case DocInclude::SnippetWithLines:
      m_t << "<programlisting filename=\"" << inc.file() << "\">";
      CodeFragmentManager::instance().parseCodeFragment(m_ci,
                                       inc.file(),
                                       inc.blockId(),
                                       inc.context(),
                                       inc.type()==DocInclude::SnippetWithLines,
                                       inc.trimLeft(),
                                       inc.stripCodeComments()
                                      );
      m_t << "</programlisting>";
      break;
  }
}

void XmlDocVisitor::operator()(const DocIncOperator &op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op.type(),op.isFirst(),op.isLast(),qPrint(op.text()));
  if (op.isFirst())
  {
    if (!m_hide)
    {
      m_t << "<programlisting filename=\"" << op.includeFileName() << "\">";
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

void XmlDocVisitor::operator()(const DocFormula &f)
{
  if (m_hide) return;
  m_t << "<formula id=\"" << f.id() << "\">";
  filter(f.text());
  m_t << "</formula>";
}

void XmlDocVisitor::operator()(const DocIndexEntry &ie)
{
  if (m_hide) return;
  m_t << "<indexentry>"
           "<primaryie>";
  filter(ie.entry());
  m_t << "</primaryie>"
           "<secondaryie></secondaryie>"
         "</indexentry>";
}

void XmlDocVisitor::operator()(const DocSimpleSectSep &sep)
{
  const DocSimpleSect *sect = std::get_if<DocSimpleSect>(sep.parent());
  if (sect)
  {
    endSimpleSect(m_t,*sect);
    startSimpleSect(m_t,*sect);
  }
}

void XmlDocVisitor::operator()(const DocCite &cite)
{
  if (m_hide) return;
  auto opt = cite.option();
  if (!cite.file().isEmpty())
  {
    if (!opt.noCite()) startLink(cite.ref(),cite.file(),cite.anchor());

    filter(cite.getText());

    if (!opt.noCite()) endLink();
  }
  else
  {
    m_t << "<b>";
    if (!opt.noPar()) filter("[");
    filter(cite.target());
    if (!opt.noPar()) filter("]");
    m_t << "</b>";
  }
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void XmlDocVisitor::operator()(const DocAutoList &l)
{
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

void XmlDocVisitor::operator()(const DocAutoListItem &li)
{
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

void XmlDocVisitor::operator()(const DocPara &p)
{
  if (m_hide) return;
  m_t << "<para>";
  visitChildren(p);
  m_t << "</para>\n";
}

void XmlDocVisitor::operator()(const DocRoot &r)
{
  visitChildren(r);
}

void XmlDocVisitor::operator()(const DocSimpleSect &s)
{
  if (m_hide) return;
  startSimpleSect(m_t,s);
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);
  endSimpleSect(m_t,s);
}

void XmlDocVisitor::operator()(const DocTitle &t)
{
  if (m_hide) return;
  m_t << "<title>";
  visitChildren(t);
  m_t << "</title>";
}

void XmlDocVisitor::operator()(const DocSimpleList &l)
{
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
  visitChildren(l);
  m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::operator()(const DocSimpleListItem &li)
{
  if (m_hide) return;
  m_t << "<listitem>";
  if (li.paragraph())
  {
    std::visit(*this,*li.paragraph());
  }
  m_t << "</listitem>\n";
}

void XmlDocVisitor::operator()(const DocSection &s)
{
  if (m_hide) return;
  int orgSectionLevel = m_sectionLevel;
  QCString sectId = s.file();
  if (!s.anchor().isEmpty()) sectId += "_1"+s.anchor();
  while (m_sectionLevel+1<s.level()) // fix missing intermediate levels
  {
    m_sectionLevel++;
    m_t << "<sect" << m_sectionLevel << " id=\"" << sectId << "_1s" << m_sectionLevel << "\">";
  }
  m_sectionLevel++;
  m_t << "<sect" << s.level() << " id=\"" << sectId << "\">\n";
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);
  m_t << "</sect" << s.level() << ">";
  m_sectionLevel--;
  while (orgSectionLevel<m_sectionLevel) // fix missing intermediate levels
  {
    m_t << "</sect" << m_sectionLevel << ">";
    m_sectionLevel--;
  }
  m_t << "\n";
}

void XmlDocVisitor::operator()(const DocHtmlList &s)
{
  if (m_hide) return;
  if (s.type()==DocHtmlList::Ordered)
  {
    m_t << "<orderedlist";
    for (const auto &opt : s.attribs())
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
    m_t << ">\n";
  }
  else
  {
    m_t << "<itemizedlist>\n";
  }
  visitChildren(s);
  if (s.type()==DocHtmlList::Ordered)
  {
    m_t << "</orderedlist>\n";
  }
  else
  {
    m_t << "</itemizedlist>\n";
  }
}

void XmlDocVisitor::operator()(const DocHtmlListItem &l)
{
  if (m_hide) return;
  m_t << "<listitem";
  for (const auto &opt : l.attribs())
  {
    if (opt.name=="value")
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
  }
  m_t << ">\n";
  visitChildren(l);
  m_t << "</listitem>\n";
}

void XmlDocVisitor::operator()(const DocHtmlDescList &dl)
{
  if (m_hide) return;
  m_t << "<variablelist>\n";
  visitChildren(dl);
  m_t << "</variablelist>\n";
}

void XmlDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
  if (m_hide) return;
  m_t << "<varlistentry><term>";
  visitChildren(dt);
  m_t << "</term></varlistentry>\n";
}

void XmlDocVisitor::operator()(const DocHtmlDescData &dd)
{
  if (m_hide) return;
  m_t << "<listitem>";
  visitChildren(dd);
  m_t << "</listitem>\n";
}

void XmlDocVisitor::operator()(const DocHtmlTable &t)
{
  if (m_hide) return;
  m_t << "<table rows=\"" << t.numRows()
      << "\" cols=\"" << t.numColumns() << "\"" ;
  for (const auto &opt : t.attribs())
  {
    if (opt.name=="width")
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
  }
  m_t << ">";
  if (t.caption())
  {
    std::visit(*this,*t.caption());
  }
  visitChildren(t);
  m_t << "</table>\n";
}

void XmlDocVisitor::operator()(const DocHtmlRow &r)
{
  if (m_hide) return;
  m_t << "<row>\n";
  visitChildren(r);
  m_t << "</row>\n";
}

void XmlDocVisitor::operator()(const DocHtmlCell &c)
{
  if (m_hide) return;
  if (c.isHeading()) m_t << "<entry thead=\"yes\""; else m_t << "<entry thead=\"no\"";
  for (const auto &opt : c.attribs())
  {
    if (opt.name=="colspan" || opt.name=="rowspan")
    {
      m_t << " " << opt.name << "=\"" << opt.value.toInt() << "\"";
    }
    else if (opt.name=="align" &&
             (opt.value=="right" || opt.value=="left" || opt.value=="center"))
    {
      m_t << " align=\"" << opt.value << "\"";
    }
    else if (opt.name=="valign" &&
      (opt.value == "bottom" || opt.value == "top" || opt.value == "middle"))
    {
      m_t << " valign=\"" << opt.value << "\"";
    }
    else if (opt.name=="width")
    {
      m_t << " width=\"" << opt.value << "\"";
    }
    else if (opt.name=="class") // handle markdown generated attributes
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
      else if (!opt.value.isEmpty())
      {
        m_t << " class=\"" << convertToXML(opt.value) << "\"";
      }
    }
  }
  m_t << ">";
  visitChildren(c);
  m_t << "</entry>";
}

void XmlDocVisitor::operator()(const DocHtmlCaption &c)
{
  if (m_hide) return;
  m_t << "<caption";
  if (!c.file().isEmpty())
  {
    m_t << " id=\""  << stripPath(c.file()) << "_1" << c.anchor() << "\"";
  }
  m_t << ">";
  visitChildren(c);
  m_t << "</caption>\n";
}

void XmlDocVisitor::operator()(const DocInternal &i)
{
  if (m_hide) return;
  m_t << "<internal>";
  visitChildren(i);
  m_t << "</internal>\n";
}

void XmlDocVisitor::operator()(const DocHRef &href)
{
  if (m_hide) return;
  m_t << "<ulink url=\"" << convertToXML(href.url(), TRUE) << "\">";
  visitChildren(href);
  m_t << "</ulink>";
}

void XmlDocVisitor::operator()(const DocHtmlSummary &s)
{
  if (m_hide) return;
  m_t << "<summary>";
  visitChildren(s);
  m_t << "</summary>";
}

void XmlDocVisitor::operator()(const DocHtmlDetails &d)
{
  if (m_hide) return;
  m_t << "<details>";
  auto summary = d.summary();
  if (summary)
  {
    std::visit(*this,*summary);
  }
  visitChildren(d);
  m_t << "</details>";
}

void XmlDocVisitor::operator()(const DocHtmlHeader &header)
{
  if (m_hide) return;
  m_t << "<heading level=\"" << header.level() << "\">";
  visitChildren(header);
  m_t << "</heading>\n";
}

void XmlDocVisitor::operator()(const DocImage &img)
{
  if (m_hide) return;

  QCString url = img.url();
  QCString baseName;
  if (url.isEmpty())
  {
    baseName = img.relPath()+img.name();
  }
  else
  {
    baseName = correctURL(url,img.relPath());
  }
  HtmlAttribList attribs = img.attribs();
  auto it = std::find_if(attribs.begin(),attribs.end(),
                         [](const auto &att) { return att.name=="alt"; });
  QCString altValue = it!=attribs.end() ? it->value : "";
  visitPreStart(m_t, "image", FALSE, *this, img.children(), baseName, TRUE,
                img.type(), img.width(), img.height(), QCString(),
                altValue, img.isInlineImage());

  // copy the image to the output dir
  FileDef *fd = nullptr;
  bool ambig;
  if (url.isEmpty() && (fd=findFileDef(Doxygen::imageNameLinkedMap,img.name(),ambig)))
  {
    copyFile(fd->absFilePath(),Config_getString(XML_OUTPUT)+"/"+baseName);
  }
  visitChildren(img);
  visitPostEnd(m_t, "image");
}

void XmlDocVisitor::operator()(const DocDotFile &df)
{
  if (m_hide) return;
  copyFile(df.file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df.file()));
  visitPreStart(m_t, "dotfile", FALSE, *this, df.children(), stripPath(df.file()), FALSE, DocImage::Html, df.width(), df.height());
  visitChildren(df);
  visitPostEnd(m_t, "dotfile");
}

void XmlDocVisitor::operator()(const DocMscFile &df)
{
  if (m_hide) return;
  copyFile(df.file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df.file()));
  visitPreStart(m_t, "mscfile", FALSE, *this, df.children(), stripPath(df.file()), FALSE, DocImage::Html, df.width(), df.height());
  visitChildren(df);
  visitPostEnd(m_t, "mscfile");
}

void XmlDocVisitor::operator()(const DocDiaFile &df)
{
  if (m_hide) return;
  copyFile(df.file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df.file()));
  visitPreStart(m_t, "diafile", FALSE, *this, df.children(), stripPath(df.file()), FALSE, DocImage::Html, df.width(), df.height());
  visitChildren(df);
  visitPostEnd(m_t, "diafile");
}

void XmlDocVisitor::operator()(const DocPlantUmlFile &df)
{
  if (m_hide) return;
  copyFile(df.file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df.file()));
  visitPreStart(m_t, "plantumlfile", FALSE, *this, df.children(), stripPath(df.file()), FALSE, DocImage::Html, df.width(), df.height());
  visitChildren(df);
  visitPostEnd(m_t, "plantumlfile");
}

void XmlDocVisitor::operator()(const DocLink &lnk)
{
  if (m_hide) return;
  startLink(lnk.ref(),lnk.file(),lnk.anchor());
  visitChildren(lnk);
  endLink();
}

void XmlDocVisitor::operator()(const DocRef &ref)
{
  if (m_hide) return;
  if (!ref.file().isEmpty())
  {
    startLink(ref.ref(),ref.file(),ref.isSubPage() ? QCString() : ref.anchor());
  }
  if (!ref.hasLinkText()) filter(ref.targetTitle());
  visitChildren(ref);
  if (!ref.file().isEmpty()) endLink();
}

void XmlDocVisitor::operator()(const DocSecRefItem &ref)
{
  if (m_hide) return;
  m_t << "<tocitem id=\"" << ref.file();
  if (!ref.anchor().isEmpty()) m_t << "_1" << ref.anchor();
  m_t << "\"";
  m_t << ">";
  visitChildren(ref);
  m_t << "</tocitem>\n";
}

void XmlDocVisitor::operator()(const DocSecRefList &l)
{
  if (m_hide) return;
  m_t << "<toclist>\n";
  visitChildren(l);
  m_t << "</toclist>\n";
}

void XmlDocVisitor::operator()(const DocParamSect &s)
{
  if (m_hide) return;
  m_t << "<parameterlist kind=\"";
  switch(s.type())
  {
    case DocParamSect::Param:
      m_t << "param"; break;
    case DocParamSect::RetVal:
      m_t << "retval"; break;
    case DocParamSect::Exception:
      m_t << "exception"; break;
    case DocParamSect::TemplateParam:
      m_t << "templateparam"; break;
    default:
      ASSERT(0);
  }
  m_t << "\">";
  visitChildren(s);
  m_t << "</parameterlist>\n";
}

void XmlDocVisitor::operator()(const DocSeparator &)
{
  m_t << "</parametertype>\n";
  m_t << "<parametertype>";
}

void XmlDocVisitor::operator()(const DocParamList &pl)
{
  if (m_hide) return;
  m_t << "<parameteritem>\n";
  m_t << "<parameternamelist>\n";
  for (const auto &param : pl.parameters())
  {
    if (!pl.paramTypes().empty())
    {
      m_t << "<parametertype>";
      for (const auto &type : pl.paramTypes())
      {
        std::visit(*this,type);
      }
      m_t << "</parametertype>\n";
    }
    m_t << "<parametername";
    if (pl.direction()!=DocParamSect::Unspecified)
    {
      m_t << " direction=\"";
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
        m_t << "inout";
      }
      m_t << "\"";
    }
    m_t << ">";
    std::visit(*this,param);
    m_t << "</parametername>\n";
  }
  m_t << "</parameternamelist>\n";
  m_t << "<parameterdescription>\n";
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  m_t << "</parameterdescription>\n";
  m_t << "</parameteritem>\n";
}

void XmlDocVisitor::operator()(const DocXRefItem &x)
{
  if (m_hide) return;
  if (x.title().isEmpty()) return;
  m_t << "<xrefsect id=\"";
  m_t << x.file() << "_1" << x.anchor();
  m_t << "\">";
  m_t << "<xreftitle>";
  filter(x.title());
  m_t << "</xreftitle>";
  m_t << "<xrefdescription>";
  visitChildren(x);
  if (x.title().isEmpty()) return;
  m_t << "</xrefdescription>";
  m_t << "</xrefsect>";
}

void XmlDocVisitor::operator()(const DocInternalRef &ref)
{
  if (m_hide) return;
  startLink(QCString(),ref.file(),ref.anchor());
  visitChildren(ref);
  endLink();
  m_t << " ";
}

void XmlDocVisitor::operator()(const DocText &t)
{
  visitChildren(t);
}

void XmlDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
  if (m_hide) return;
  m_t << "<blockquote>";
  visitChildren(q);
  m_t << "</blockquote>";
}

void XmlDocVisitor::operator()(const DocVhdlFlow &)
{
}

void XmlDocVisitor::operator()(const DocParBlock &pb)
{
  if (m_hide) return;
  m_t << "<parblock>";
  visitChildren(pb);
  m_t << "</parblock>";
}


void XmlDocVisitor::filter(const QCString &str)
{
  m_t << convertToXML(str);
}

void XmlDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  //printf("XmlDocVisitor: file=%s anchor=%s\n",qPrint(file),qPrint(anchor));
  m_t << "<ref refid=\"" << file;
  if (!anchor.isEmpty()) m_t << "_1" << anchor;
  m_t << "\" kindref=\"";
  if (!anchor.isEmpty()) m_t << "member"; else m_t << "compound";
  m_t << "\"";
  if (!ref.isEmpty()) m_t << " external=\"" << ref << "\"";
  m_t << ">";
}

void XmlDocVisitor::endLink()
{
  m_t << "</ref>";
}

