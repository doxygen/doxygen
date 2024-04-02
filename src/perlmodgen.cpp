/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
 * Authors: Dimitri van Heesch, Miguel Lobo.
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
#include <stack>

#include "perlmodgen.h"
#include "docparser.h"
#include "docnode.h"
#include "message.h"
#include "doxygen.h"
#include "pagedef.h"
#include "memberlist.h"
#include "arguments.h"
#include "config.h"
#include "groupdef.h"
#include "classdef.h"
#include "classlist.h"
#include "filename.h"
#include "membername.h"
#include "namespacedef.h"
#include "membergroup.h"
#include "section.h"
#include "util.h"
#include "htmlentity.h"
#include "emoji.h"
#include "dir.h"
#include "portable.h"
#include "moduledef.h"

#define PERLOUTPUT_MAX_INDENTATION 40

class PerlModOutputStream
{
  public:
    //QCString m_s;
    std::ostream *m_t;

    PerlModOutputStream(std::ostream &t) : m_t(&t) { }
   ~PerlModOutputStream() { m_t=nullptr; }

    void add(char c);
    void add(const QCString &s);
    void add(int n);
    void add(unsigned int n);
};

void PerlModOutputStream::add(char c)
{
  *m_t << c;
}

void PerlModOutputStream::add(const QCString &s)
{
  *m_t << s;
}

void PerlModOutputStream::add(int n)
{
  *m_t << n;
}

void PerlModOutputStream::add(unsigned int n)
{
  *m_t << n;
}

class PerlModOutput
{
public:

  bool m_pretty;

  inline PerlModOutput(bool pretty)
    : m_pretty(pretty), m_stream(nullptr), m_indentation(false), m_blockstart(true)
  {
    m_spaces[0] = 0;
  }

  virtual ~PerlModOutput() { reset(); }

  void reset() { m_stream=nullptr; }

  inline void setPerlModOutputStream(PerlModOutputStream *os) { m_stream = os; }

  //inline PerlModOutput &openSave() { iopenSave(); return *this; }
  //inline PerlModOutput &closeSave(QCString &s) { icloseSave(s); return *this; }

  inline PerlModOutput &continueBlock()
  {
    if (m_blockstart)
      m_blockstart = false;
    else
      m_stream->add(',');
    indent();
    return *this;
  }

  inline PerlModOutput &add(char c) { m_stream->add(c); return *this; }
  inline PerlModOutput &add(const QCString &s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(QCString &s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(int n) { m_stream->add(n); return *this; }
  inline PerlModOutput &add(unsigned int n) { m_stream->add(n); return *this; }

  PerlModOutput &addQuoted(const QCString &s) { iaddQuoted(s); return *this; }

  inline PerlModOutput &indent()
  {
    if (m_pretty) {
      m_stream->add('\n');
      m_stream->add(m_spaces);
    }
    return *this;
  }

  inline PerlModOutput &open(char c, const QCString &s = QCString()) { iopen(c, s); return *this; }
  inline PerlModOutput &close(char c = 0) { iclose(c); return *this; }

  inline PerlModOutput &addField(const QCString &s) { iaddField(s); return *this; }
  inline PerlModOutput &addFieldQuotedChar(const QCString &field, char content)
  {
    iaddFieldQuotedChar(field, content); return *this;
  }
  inline PerlModOutput &addFieldQuotedString(const QCString &field, const QCString &content)
  {
    iaddFieldQuotedString(field, content); return *this;
  }
  inline PerlModOutput &addFieldBoolean(const QCString &field, bool content)
  {
    return addFieldQuotedString(field, content ? "yes" : "no");
  }
  inline PerlModOutput &openList(const QCString &s = QCString()) { open('[', s); return *this; }
  inline PerlModOutput &closeList() { close(']'); return *this; }
  inline PerlModOutput &openHash(const QCString &s = QCString() ) { open('{', s); return *this; }
  inline PerlModOutput &closeHash() { close('}'); return *this; }

protected:

  //void iopenSave();
  //void icloseSave(QCString &);

  void incIndent();
  void decIndent();

  void iaddQuoted(const QCString &);
  void iaddFieldQuotedChar(const QCString &, char);
  void iaddFieldQuotedString(const QCString &, const QCString &);
  void iaddField(const QCString &);

  void iopen(char, const QCString &);
  void iclose(char);

private:

  PerlModOutputStream *m_stream;
  int m_indentation;
  bool m_blockstart;

  //std::stack<PerlModOutputStream*> m_saved;
  char m_spaces[PERLOUTPUT_MAX_INDENTATION * 2 + 2];
};

//void PerlModOutput::iopenSave()
//{
//  m_saved.push(m_stream);
//  m_stream = new PerlModOutputStream();
//}

//void PerlModOutput::icloseSave(QCString &s)
//{
//  s = m_stream->m_s;
//  delete m_stream;
//  m_stream = m_saved.top();
//  m_saved.pop();
//}

void PerlModOutput::incIndent()
{
  if (m_indentation < PERLOUTPUT_MAX_INDENTATION)
  {
    char *s = &m_spaces[m_indentation * 2];
    *s++ = ' '; *s++ = ' '; *s = 0;
  }
  m_indentation++;
}

void PerlModOutput::decIndent()
{
  m_indentation--;
  if (m_indentation < PERLOUTPUT_MAX_INDENTATION)
    m_spaces[m_indentation * 2] = 0;
}

void PerlModOutput::iaddQuoted(const QCString &str)
{
  if (str.isEmpty()) return;
  const char *s = str.data();
  char c;
  while ((c = *s++) != 0)
  {
    if ((c == '\'') || (c == '\\'))
    {
      m_stream->add('\\');
    }
    m_stream->add(c);
  }
}

void PerlModOutput::iaddField(const QCString &s)
{
  continueBlock();
  m_stream->add(s);
  m_stream->add(m_pretty ? " => " : "=>");
}

void PerlModOutput::iaddFieldQuotedChar(const QCString &field, char content)
{
  iaddField(field);
  m_stream->add('\'');
  if ((content == '\'') || (content == '\\'))
    m_stream->add('\\');
  m_stream->add(content);
  m_stream->add('\'');
}

void PerlModOutput::iaddFieldQuotedString(const QCString &field, const QCString &content)
{
  if (content == nullptr)
    return;
  iaddField(field);
  m_stream->add('\'');
  iaddQuoted(content);
  m_stream->add('\'');
}

void PerlModOutput::iopen(char c, const QCString &s)
{
  if (s != nullptr)
    iaddField(s);
  else
    continueBlock();
  m_stream->add(c);
  incIndent();
  m_blockstart = true;
}

void PerlModOutput::iclose(char c)
{
  decIndent();
  indent();
  if (c != 0)
    m_stream->add(c);
  m_blockstart = false;
}

/*! @brief Concrete visitor implementation for PerlMod output. */
class PerlModDocVisitor : public DocVisitor
{
  public:
    PerlModDocVisitor(PerlModOutput &);
    virtual ~PerlModDocVisitor() { }

    void finish();

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void operator()(const DocWord &);
    void operator()(const DocLinkedWord &);
    void operator()(const DocWhiteSpace &);
    void operator()(const DocSymbol &);
    void operator()(const DocEmoji &);
    void operator()(const DocURL &);
    void operator()(const DocLineBreak &);
    void operator()(const DocHorRuler &);
    void operator()(const DocStyleChange &);
    void operator()(const DocVerbatim &);
    void operator()(const DocAnchor &);
    void operator()(const DocInclude &);
    void operator()(const DocIncOperator &);
    void operator()(const DocFormula &);
    void operator()(const DocIndexEntry &);
    void operator()(const DocSimpleSectSep &);
    void operator()(const DocCite &);
    void operator()(const DocSeparator &);

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    void operator()(const DocAutoList &);
    void operator()(const DocAutoListItem &);
    void operator()(const DocPara &) ;
    void operator()(const DocRoot &);
    void operator()(const DocSimpleSect &);
    void operator()(const DocTitle &);
    void operator()(const DocSimpleList &);
    void operator()(const DocSimpleListItem &);
    void operator()(const DocSection &);
    void operator()(const DocHtmlList &);
    void operator()(const DocHtmlListItem &);
    void operator()(const DocHtmlDescList &);
    void operator()(const DocHtmlDescTitle &);
    void operator()(const DocHtmlDescData &);
    void operator()(const DocHtmlTable &);
    void operator()(const DocHtmlRow &);
    void operator()(const DocHtmlCell &);
    void operator()(const DocHtmlCaption &);
    void operator()(const DocInternal &);
    void operator()(const DocHRef &);
    void operator()(const DocHtmlSummary &);
    void operator()(const DocHtmlDetails &);
    void operator()(const DocHtmlHeader &);
    void operator()(const DocImage &);
    void operator()(const DocDotFile &);
    void operator()(const DocMscFile &);
    void operator()(const DocDiaFile &);
    void operator()(const DocLink &);
    void operator()(const DocRef &);
    void operator()(const DocSecRefItem &);
    void operator()(const DocSecRefList &);
    void operator()(const DocParamSect &);
    void operator()(const DocParamList &);
    void operator()(const DocXRefItem &);
    void operator()(const DocInternalRef &);
    void operator()(const DocText &);
    void operator()(const DocHtmlBlockQuote &);
    void operator()(const DocVhdlFlow &);
    void operator()(const DocParBlock &);

  private:
    template<class T>
    void visitChildren(const T &t)
    {
      for (const auto &child : t.children())
      {
         std::visit(*this, child);
      }
    }

    //--------------------------------------
    // helper functions
    //--------------------------------------

    void addLink(const QCString &ref, const QCString &file,
        const QCString &anchor);

    void enterText();
    void leaveText();

    void openItem(const QCString &);
    void closeItem();
    void singleItem(const QCString &);
    void openSubBlock(const QCString & = QCString());
    void closeSubBlock();

    //--------------------------------------
    // state variables
    //--------------------------------------

    PerlModOutput &m_output;
    bool m_textmode;
    bool m_textblockstart;
    QCString m_other;
};

PerlModDocVisitor::PerlModDocVisitor(PerlModOutput &output)
  : m_output(output), m_textmode(false), m_textblockstart(FALSE)
{
  m_output.openList("doc");
}

void PerlModDocVisitor::finish()
{
  leaveText();
  m_output.closeList()
    .add(m_other);
}

void PerlModDocVisitor::addLink(const QCString &,const QCString &file,const QCString &anchor)
{
  QCString link = file;
  if (!anchor.isEmpty())
    (link += "_1") += anchor;
  m_output.addFieldQuotedString("link", link);
}

void PerlModDocVisitor::openItem(const QCString &name)
{
  leaveText();
  m_output.openHash().addFieldQuotedString("type", name);
}

void PerlModDocVisitor::closeItem()
{
  leaveText();
  m_output.closeHash();
}

void PerlModDocVisitor::enterText()
{
  if (m_textmode)
    return;
  openItem("text");
  m_output.addField("content").add('\'');
  m_textmode = true;
}

void PerlModDocVisitor::leaveText()
{
  if (!m_textmode)
    return;
  m_textmode = false;
  m_output
    .add('\'')
    .closeHash();
}

void PerlModDocVisitor::singleItem(const QCString &name)
{
  openItem(name);
  closeItem();
}

void PerlModDocVisitor::openSubBlock(const QCString &s)
{
  leaveText();
  m_output.openList(s);
  m_textblockstart = true;
}

void PerlModDocVisitor::closeSubBlock()
{
  leaveText();
  m_output.closeList();
}

//void PerlModDocVisitor::openOther()
//{
  // Using a secondary text stream will corrupt the perl file. Instead of
  // printing doc => [ data => [] ], it will print doc => [] data => [].
  /*
  leaveText();
  m_output.openSave();
  */
//}

//void PerlModDocVisitor::closeOther()
//{
  // Using a secondary text stream will corrupt the perl file. Instead of
  // printing doc => [ data => [] ], it will print doc => [] data => [].
  /*
  QCString other;
  leaveText();
  m_output.closeSave(other);
  m_other += other;
  */
//}

void PerlModDocVisitor::operator()(const DocWord &w)
{
  enterText();
  m_output.addQuoted(w.word());
}

void PerlModDocVisitor::operator()(const DocLinkedWord &w)
{
  openItem("url");
  addLink(w.ref(), w.file(), w.anchor());
  m_output.addFieldQuotedString("content", w.word());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocWhiteSpace &)
{
  enterText();
  m_output.add(' ');
}

void PerlModDocVisitor::operator()(const DocSymbol &sy)
{
  const HtmlEntityMapper::PerlSymb *res = HtmlEntityMapper::instance().perl(sy.symbol());
  const char *accent=nullptr;
  if (res->symb)
  {
    switch (res->type)
    {
      case HtmlEntityMapper::Perl_string:
        enterText();
        m_output.add(res->symb);
        break;
      case HtmlEntityMapper::Perl_char:
        enterText();
        m_output.add(res->symb[0]);
        break;
      case HtmlEntityMapper::Perl_symbol:
        leaveText();
        openItem("symbol");
        m_output.addFieldQuotedString("symbol", res->symb);
        closeItem();
        break;
      default:
        switch(res->type)
        {
          case HtmlEntityMapper::Perl_umlaut:
            accent = "umlaut";
            break;
          case HtmlEntityMapper::Perl_acute:
            accent = "acute";
            break;
          case HtmlEntityMapper::Perl_grave:
            accent = "grave";
            break;
          case HtmlEntityMapper::Perl_circ:
            accent = "circ";
            break;
          case HtmlEntityMapper::Perl_slash:
            accent = "slash";
            break;
          case HtmlEntityMapper::Perl_tilde:
            accent = "tilde";
            break;
          case HtmlEntityMapper::Perl_cedilla:
            accent = "cedilla";
            break;
          case HtmlEntityMapper::Perl_ring:
            accent = "ring";
            break;
          default:
            break;
        }
        leaveText();
        if (accent)
        {
          openItem("accent");
          m_output
            .addFieldQuotedString("accent", accent)
            .addFieldQuotedChar("letter", res->symb[0]);
          closeItem();
        }
        break;
    }
  }
  else
  {
    err("perl: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance().html(sy.symbol(),TRUE));
  }
}

void PerlModDocVisitor::operator()(const DocEmoji &sy)
{
  enterText();
  const char *name = EmojiEntityMapper::instance().name(sy.index());
  if (name)
  {
    m_output.add(name);
  }
  else
  {
    m_output.add(sy.name());
  }
}

void PerlModDocVisitor::operator()(const DocURL &u)
{
  openItem("url");
  m_output.addFieldQuotedString("content", u.url());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocLineBreak &)
{
  singleItem("linebreak");
}

void PerlModDocVisitor::operator()(const DocHorRuler &)
{
  singleItem("hruler");
}

void PerlModDocVisitor::operator()(const DocStyleChange &s)
{
  const char *style = nullptr;
  switch (s.style())
  {
    case DocStyleChange::Bold:          style = "bold"; break;
    case DocStyleChange::S:             style = "s"; break;
    case DocStyleChange::Strike:        style = "strike"; break;
    case DocStyleChange::Del:           style = "del"; break;
    case DocStyleChange::Underline:     style = "underline"; break;
    case DocStyleChange::Ins:           style = "ins"; break;
    case DocStyleChange::Italic:        style = "italic"; break;
    case DocStyleChange::Code:          style = "code"; break;
    case DocStyleChange::Subscript:     style = "subscript"; break;
    case DocStyleChange::Superscript:   style = "superscript"; break;
    case DocStyleChange::Center:        style = "center"; break;
    case DocStyleChange::Small:         style = "small"; break;
    case DocStyleChange::Cite:          style = "cite"; break;
    case DocStyleChange::Preformatted:  style = "preformatted"; break;
    case DocStyleChange::Div:           style = "div"; break;
    case DocStyleChange::Span:          style = "span"; break;
  }
  openItem("style");
  m_output.addFieldQuotedString("style", style)
    .addFieldBoolean("enable", s.enable());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocVerbatim &s)
{
  const char *type = nullptr;
  switch (s.type())
  {
    case DocVerbatim::Code:
#if 0
      m_output.add("<programlisting>");
      parseCode(m_ci,s->context(),s->text(),FALSE,0);
      m_output.add("</programlisting>");
      return;
#endif
    case DocVerbatim::JavaDocCode:
    case DocVerbatim::JavaDocLiteral:
    case DocVerbatim::Verbatim:  type = "preformatted"; break;
    case DocVerbatim::HtmlOnly:  type = "htmlonly";     break;
    case DocVerbatim::RtfOnly:   type = "rtfonly";      break;
    case DocVerbatim::ManOnly:   type = "manonly";      break;
    case DocVerbatim::LatexOnly: type = "latexonly";    break;
    case DocVerbatim::XmlOnly:   type = "xmlonly";      break;
    case DocVerbatim::DocbookOnly: type = "docbookonly"; break;
    case DocVerbatim::Dot:       type = "dot";          break;
    case DocVerbatim::Msc:       type = "msc";          break;
    case DocVerbatim::PlantUML:  type = "plantuml";     break;
  }
  openItem(type);
  if (s.hasCaption())
  {
     openSubBlock("caption");
     visitChildren(s);
     closeSubBlock();
  }
  m_output.addFieldQuotedString("content", s.text());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocAnchor &anc)
{
  QCString anchor = anc.file() + "_1" + anc.anchor();
  openItem("anchor");
  m_output.addFieldQuotedString("id", anchor);
  closeItem();
}

void PerlModDocVisitor::operator()(const DocInclude &inc)
{
  const char *type = nullptr;
  switch (inc.type())
  {
    case DocInclude::IncWithLines:
      return;
    case DocInclude::Include:
      return;
    case DocInclude::DontInclude:	return;
    case DocInclude::DontIncWithLines: return;
    case DocInclude::HtmlInclude:	type = "htmlonly"; break;
    case DocInclude::LatexInclude: type = "latexonly"; break;
    case DocInclude::RtfInclude: type = "rtfonly"; break;
    case DocInclude::ManInclude: type = "manonly"; break;
    case DocInclude::XmlInclude: type = "xmlonly"; break;
    case DocInclude::DocbookInclude: type = "docbookonly"; break;
    case DocInclude::VerbInclude:	type = "preformatted"; break;
    case DocInclude::Snippet: return;
    case DocInclude::SnippetTrimLeft: return;
    case DocInclude::SnippetWithLines: return;
  }
  openItem(type);
  m_output.addFieldQuotedString("content", inc.text());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocIncOperator &)
{
#if 0
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op.type(),op.isFirst(),op.isLast(),op.text().data());
  if (op.isFirst())
  {
    m_output.add("<programlisting>");
  }
  if (op.type()!=DocIncOperator::Skip)
  {
    parseCode(m_ci,op.context(),op.text(),FALSE,0);
  }
  if (op.isLast())
  {
    m_output.add("</programlisting>");
  }
  else
  {
    m_output.add('\n');
  }
#endif
}

void PerlModDocVisitor::operator()(const DocFormula &f)
{
  openItem("formula");
  QCString id;
  id += QCString().setNum(f.id());
  m_output.addFieldQuotedString("id", id).addFieldQuotedString("content", f.text());
  closeItem();
}

void PerlModDocVisitor::operator()(const DocIndexEntry &)
{
#if 0
  m_output.add("<indexentry>"
	       "<primaryie>");
  m_output.addQuoted(ie->entry());
  m_output.add("</primaryie>"
	       "<secondaryie></secondaryie>"
	       "</indexentry>");
#endif
}

void PerlModDocVisitor::operator()(const DocSimpleSectSep &)
{
}

void PerlModDocVisitor::operator()(const DocCite &cite)
{
  openItem("cite");
  m_output.addFieldQuotedString("text", cite.text());
  closeItem();
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void PerlModDocVisitor::operator()(const DocAutoList &l)
{
  openItem("list");
  m_output.addFieldQuotedString("style", l.isEnumList() ? "ordered" : (l.isCheckedList() ? "check" :"itemized"));
  openSubBlock("content");
  visitChildren(l);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocAutoListItem &li)
{
  openSubBlock();
  switch (li.itemNumber())
  {
    case DocAutoList::Unchecked: // unchecked
      m_output.addFieldQuotedString("style", "Unchecked");
      break;
    case DocAutoList::Checked_x: // checked with x
    case DocAutoList::Checked_X: // checked with X
      m_output.addFieldQuotedString("style", "Checked");
      break;
    default:
      break;
  }
  visitChildren(li);
  closeSubBlock();
}

void PerlModDocVisitor::operator()(const DocPara &p)
{
  if (m_textblockstart)
    m_textblockstart = false;
  else
    singleItem("parbreak");
  /*
  openItem("para");
  openSubBlock("content");
  */
  visitChildren(p);
  /*
  closeSubBlock();
  closeItem();
  */
}

void PerlModDocVisitor::operator()(const DocRoot &r)
{
  visitChildren(r);
}

void PerlModDocVisitor::operator()(const DocSimpleSect &s)
{
  const char *type = nullptr;
  switch (s.type())
  {
  case DocSimpleSect::See:		type = "see"; break;
  case DocSimpleSect::Return:		type = "return"; break;
  case DocSimpleSect::Author:		type = "author"; break;
  case DocSimpleSect::Authors:		type = "authors"; break;
  case DocSimpleSect::Version:		type = "version"; break;
  case DocSimpleSect::Since:		type = "since"; break;
  case DocSimpleSect::Date:		type = "date"; break;
  case DocSimpleSect::Note:		type = "note"; break;
  case DocSimpleSect::Warning:		type = "warning"; break;
  case DocSimpleSect::Pre:		type = "pre"; break;
  case DocSimpleSect::Post:		type = "post"; break;
  case DocSimpleSect::Copyright:	type = "copyright"; break;
  case DocSimpleSect::Invar:		type = "invariant"; break;
  case DocSimpleSect::Remark:		type = "remark"; break;
  case DocSimpleSect::Attention:	type = "attention"; break;
  case DocSimpleSect::Important:	type = "important"; break;
  case DocSimpleSect::User:		type = "par"; break;
  case DocSimpleSect::Rcs:		type = "rcs"; break;
  case DocSimpleSect::Unknown:
    err("unknown simple section found\n");
    break;
  }
  leaveText();
  m_output.openHash();
  //openOther();
  openSubBlock(type);
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);
  closeSubBlock();
  //closeOther();
  m_output.closeHash();
}

void PerlModDocVisitor::operator()(const DocTitle &t)
{
  openItem("title");
  openSubBlock("content");
  visitChildren(t);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocSimpleList &l)
{
  openItem("list");
  m_output.addFieldQuotedString("style", "itemized");
  openSubBlock("content");
  visitChildren(l);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocSimpleListItem &li)
{
  openSubBlock();
  if (li.paragraph())
  {
    std::visit(*this,*li.paragraph());
  }
  closeSubBlock();
}

void PerlModDocVisitor::operator()(const DocSection &s)
{
  QCString sect = QCString().sprintf("sect%d",s.level());
  openItem(sect);
  //m_output.addFieldQuotedString("title", s.title());
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  openSubBlock("content");
  visitChildren(s);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocHtmlList &l)
{
  openItem("list");
  m_output.addFieldQuotedString("style", (l.type() == DocHtmlList::Ordered) ? "ordered" : "itemized");
  for (const auto &opt : l.attribs())
  {
    if (opt.name=="type")
    {
      m_output.addFieldQuotedString("list_type", qPrint(opt.value));
    }
    if (opt.name=="start")
    {
      m_output.addFieldQuotedString("start", qPrint(opt.value));
    }
  }
  openSubBlock("content");
  visitChildren(l);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocHtmlListItem &l)
{
  for (const auto &opt : l.attribs())
  {
    if (opt.name=="value")
    {
      m_output.addFieldQuotedString("item_value", qPrint(opt.value));
    }
  }
  openSubBlock();
  visitChildren(l);
  closeSubBlock();
}

void PerlModDocVisitor::operator()(const DocHtmlDescList &dl)
{
#if 0
  m_output.add("<variablelist>\n");
#endif
  visitChildren(dl);
#if 0
  m_output.add("</variablelist>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
#if 0
  m_output.add("<varlistentry><term>");
#endif
  visitChildren(dt);
#if 0
  m_output.add("</term></varlistentry>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlDescData &dd)
{
#if 0
  m_output.add("<listitem>");
#endif
  visitChildren(dd);
#if 0
  m_output.add("</listitem>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlTable &t)
{
#if 0
  m_output.add("<table rows=\""); m_output.add(t.numRows());
  m_output.add("\" cols=\""); m_output.add(t.numCols()); m_output.add("\">");
#endif
  if (t.caption())
  {
    std::visit(*this,*t.caption());
  }
  visitChildren(t);
#if 0
  m_output.add("</table>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlRow &r)
{
#if 0
  m_output.add("<row>\n");
#endif
  visitChildren(r);
#if 0
  m_output.add("</row>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlCell &c)
{
#if 0
  if (c.isHeading()) m_output.add("<entry thead=\"yes\">"); else m_output.add("<entry thead=\"no\">");
#endif
  visitChildren(c);
#if 0
  m_output.add("</entry>");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlCaption &c)
{
#if 0
  m_output.add("<caption>");
#endif
  visitChildren(c);
#if 0
  m_output.add("</caption>\n");
#endif
}

void PerlModDocVisitor::operator()(const DocInternal &i)
{
#if 0
  m_output.add("<internal>");
#endif
  visitChildren(i);
#if 0
  m_output.add("</internal>");
#endif
}

void PerlModDocVisitor::operator()(const DocHRef &href)
{
#if 0
  m_output.add("<ulink url=\""); m_output.add(href.url()); m_output.add("\">");
#endif
  visitChildren(href);
#if 0
  m_output.add("</ulink>");
#endif
}

void PerlModDocVisitor::operator()(const DocHtmlSummary &summary)
{
  openItem("summary");
  openSubBlock("content");
  visitChildren(summary);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocHtmlDetails &details)
{
  openItem("details");
  auto summary = details.summary();
  if (summary)
  {
    std::visit(*this,*summary);
  }
  openSubBlock("content");
  visitChildren(details);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocHtmlHeader &header)
{
#if 0
  m_output.add("<sect"); m_output.add(header.level()); m_output.add(">");
#endif
  visitChildren(header);
#if 0
  m_output.add("</sect"); m_output.add(header.level()); m_output.add(">\n");
#endif
}

void PerlModDocVisitor::operator()(const DocImage &img)
{
#if 0
  m_output.add("<image type=\"");
  switch(img.type())
  {
  case DocImage::Html:  m_output.add("html"); break;
  case DocImage::Latex: m_output.add("latex"); break;
  case DocImage::Rtf:   m_output.add("rtf"); break;
  }
  m_output.add("\"");

  QCString baseName=img.name();
  int i;
  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  m_output.add(" name=\""); m_output.add(baseName); m_output.add("\"");
  if (!img.width().isEmpty())
  {
    m_output.add(" width=\"");
    m_output.addQuoted(img.width());
    m_output.add("\"");
  }
  else if (!img.height().isEmpty())
  {
    m_output.add(" height=\"");
    m_output.addQuoted(img.height());
    m_output.add("\"");
  }
  m_output.add(">");
#endif
  visitChildren(img);
#if 0
  m_output.add("</image>");
#endif
}

void PerlModDocVisitor::operator()(const DocDotFile &df)
{
#if 0
  m_output.add("<dotfile name=\""); m_output.add(df->file()); m_output.add("\">");
#endif
  visitChildren(df);
#if 0
  m_output.add("</dotfile>");
#endif
}
void PerlModDocVisitor::operator()(const DocMscFile &df)
{
#if 0
  m_output.add("<mscfile name=\""); m_output.add(df->file()); m_output.add("\">");
#endif
  visitChildren(df);
#if 0
  m_output.add("<mscfile>");
#endif
}

void PerlModDocVisitor::operator()(const DocDiaFile &df)
{
#if 0
  m_output.add("<diafile name=\""); m_output.add(df->file()); m_output.add("\">");
#endif
  visitChildren(df);
#if 0
  m_output.add("</diafile>");
#endif
}


void PerlModDocVisitor::operator()(const DocLink &lnk)
{
  openItem("link");
  addLink(lnk.ref(), lnk.file(), lnk.anchor());
  visitChildren(lnk);
  closeItem();
}

void PerlModDocVisitor::operator()(const DocRef &ref)
{
  openItem("ref");
  if (!ref.hasLinkText())
    m_output.addFieldQuotedString("text", ref.targetTitle());
  openSubBlock("content");
  visitChildren(ref);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocSecRefItem &ref)
{
#if 0
  m_output.add("<tocitem id=\""); m_output.add(ref->file()); m_output.add("_1"); m_output.add(ref->anchor()); m_output.add("\">");
#endif
  visitChildren(ref);
#if 0
  m_output.add("</tocitem>");
#endif
}

void PerlModDocVisitor::operator()(const DocSecRefList &l)
{
#if 0
  m_output.add("<toclist>");
#endif
  visitChildren(l);
#if 0
  m_output.add("</toclist>");
#endif
}

void PerlModDocVisitor::operator()(const DocParamSect &s)
{
  leaveText();
  const char *type = nullptr;
  switch(s.type())
  {
    case DocParamSect::Param:     type = "params"; break;
    case DocParamSect::RetVal:    type = "retvals"; break;
    case DocParamSect::Exception: type = "exceptions"; break;
    case DocParamSect::TemplateParam: type = "templateparam"; break;
    case DocParamSect::Unknown:
      err("unknown parameter section found\n");
      break;
  }
  m_output.openHash();
  //openOther();
  openSubBlock(type);
  visitChildren(s);
  closeSubBlock();
  //closeOther();
  m_output.closeHash();
}

void PerlModDocVisitor::operator()(const DocSeparator &)
{
}

void PerlModDocVisitor::operator()(const DocParamList &pl)
{
  leaveText();
  m_output.openHash().openList("parameters");
  for (const auto &param : pl.parameters())
  {
    QCString name;
    const DocWord *word = std::get_if<DocWord>(&param);
    const DocLinkedWord *linkedWord = std::get_if<DocLinkedWord>(&param);
    if (word)
    {
      name = word->word();
    }
    else if (linkedWord)
    {
      name = linkedWord->word();
    }

    QCString dir = "";
    const DocParamSect *sect = std::get_if<DocParamSect>(pl.parent());
    if (sect && sect->hasInOutSpecifier())
    {
      if (pl.direction()!=DocParamSect::Unspecified)
      {
        if (pl.direction()==DocParamSect::In)
        {
          dir = "in";
        }
        else if (pl.direction()==DocParamSect::Out)
        {
          dir = "out";
        }
        else if (pl.direction()==DocParamSect::InOut)
        {
          dir = "in,out";
        }
      }
    }

    m_output.openHash()
      .addFieldQuotedString("name", name).addFieldQuotedString("dir", dir)
      .closeHash();
  }
  m_output.closeList()
    .openList("doc");
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  leaveText();
  m_output.closeList()
    .closeHash();
}

void PerlModDocVisitor::operator()(const DocXRefItem &x)
{
#if 0
  m_output.add("<xrefsect id=\"");
  m_output.add(x->file()); m_output.add("_1"); m_output.add(x->anchor());
  m_output.add("\">");
  m_output.add("<xreftitle>");
  m_output.addQuoted(x->title());
  m_output.add("</xreftitle>");
  m_output.add("<xrefdescription>");
#endif
  if (x.title().isEmpty()) return;
  openItem("xrefitem");
  openSubBlock("content");
  visitChildren(x);
  if (x.title().isEmpty()) return;
  closeSubBlock();
  closeItem();
#if 0
  m_output.add("</xrefdescription>");
  m_output.add("</xrefsect>");
#endif
}

void PerlModDocVisitor::operator()(const DocInternalRef &ref)
{
  openItem("ref");
  addLink(QCString(),ref.file(),ref.anchor());
  openSubBlock("content");
  visitChildren(ref);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocText &t)
{
  visitChildren(t);
}

void PerlModDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
  openItem("blockquote");
  openSubBlock("content");
  visitChildren(q);
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::operator()(const DocVhdlFlow &)
{
}

void PerlModDocVisitor::operator()(const DocParBlock &pb)
{
  visitChildren(pb);
}


static void addTemplateArgumentList(const ArgumentList &al,PerlModOutput &output,const QCString &)
{
  if (!al.hasParameters()) return;
  output.openList("template_parameters");
  for (const Argument &a : al)
  {
    output.openHash();
    if (!a.type.isEmpty())
      output.addFieldQuotedString("type", a.type);
    if (!a.name.isEmpty())
      output.addFieldQuotedString("declaration_name", a.name)
	.addFieldQuotedString("definition_name", a.name);
    if (!a.defval.isEmpty())
      output.addFieldQuotedString("default", a.defval);
    output.closeHash();
  }
  output.closeList();
}

static void addTemplateList(const ClassDef *cd,PerlModOutput &output)
{
  addTemplateArgumentList(cd->templateArguments(),output,cd->name());
}

static void addTemplateList(const ConceptDef *cd,PerlModOutput &output)
{
  addTemplateArgumentList(cd->getTemplateParameterList(),output,cd->name());
}

static void addPerlModDocBlock(PerlModOutput &output,
			    const QCString &name,
			    const QCString &fileName,
			    int lineNr,
			    const Definition *scope,
			    const MemberDef *md,
			    const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty())
  {
    output.addField(name).add("{}");
  }
  else
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     fileName,lineNr,scope,md,stext,FALSE,FALSE,
                                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    output.openHash(name);
    auto astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
    if (astImpl)
    {
      PerlModDocVisitor visitor(output);
      std::visit(visitor,astImpl->root);
      visitor.finish();
    }
    output.closeHash();
  }
}

static const char *getProtectionName(Protection prot)
{
  switch (prot)
  {
    case Protection::Public:    return "public";
    case Protection::Protected: return "protected";
    case Protection::Private:   return "private";
    case Protection::Package:   return "package";
  }
  return nullptr;
}

static const char *getVirtualnessName(Specifier virt)
{
  switch(virt)
  {
    case Specifier::Normal:  return "non_virtual";
    case Specifier::Virtual: return "virtual";
    case Specifier::Pure:    return "pure_virtual";
  }
  return nullptr;
}

static QCString pathDoxyfile;
static QCString pathDoxyExec;

void setPerlModDoxyfile(const QCString &qs)
{
  pathDoxyfile = qs;
  pathDoxyExec = Dir::currentDirPath();
}

class PerlModGenerator
{
public:

  PerlModOutput m_output;

  QCString pathDoxyStructurePM;
  QCString pathDoxyDocsTex;
  QCString pathDoxyFormatTex;
  QCString pathDoxyLatexTex;
  QCString pathDoxyLatexDVI;
  QCString pathDoxyLatexPDF;
  QCString pathDoxyStructureTex;
  QCString pathDoxyDocsPM;
  QCString pathDoxyLatexPL;
  QCString pathDoxyLatexStructurePL;
  QCString pathDoxyRules;
  // perlmodPython PerlModGenerator path define <start>
  QCString pathPythonPut2cpp_py;
  QCString pathPythonREADME_md;
  QCString pathPythonSample_py;
  QCString pathPythonDoxy2py_pl;
  // perlmodPython PerlModGenerator path define <end>
  QCString pathMakefile;

  inline PerlModGenerator(bool pretty) : m_output(pretty) { }

  void generatePerlModForMember(const MemberDef *md, const Definition *);
  void generatePerlUserDefinedSection(const Definition *d, const MemberGroupList &mgl);
  void generatePerlModSection(const Definition *d, MemberList *ml,
			      const QCString &name, const QCString &header=QCString());
  void addListOfAllMembers(const ClassDef *cd);
  void addIncludeInfo(const IncludeInfo *ii);
  void generatePerlModForClass(const ClassDef *cd);
  void generatePerlModForConcept(const ConceptDef *cd);
  void generatePerlModForModule(const ModuleDef *mod);
  void generatePerlModForNamespace(const NamespaceDef *nd);
  void generatePerlModForFile(const FileDef *fd);
  void generatePerlModForGroup(const GroupDef *gd);
  void generatePerlModForPage(PageDef *pi);

  bool createOutputFile(std::ofstream &f, const QCString &s);
  bool createOutputDir(Dir &perlModDir);
  bool generateDoxyLatexTex();
  bool generateDoxyFormatTex();
  bool generateDoxyStructurePM();
  bool generateDoxyLatexPL();
  bool generateDoxyLatexStructurePL();
  bool generateDoxyRules();
  // perlmodPython PerlModGenerator declare function <start>
  bool generatePythonPut2cpp_py();
  bool generatePythonREADME_md();
  bool generatePythonSample_py();
  bool generatePythonDoxy2py_pl();
  // perlmodPython PerlModGenerator declare function <end>
  bool generateMakefile();
  bool generatePerlModOutput();

  void generate();
};

void PerlModGenerator::generatePerlModForMember(const MemberDef *md,const Definition *)
{
  // + declaration/definition arg lists
  // + reimplements
  // + reimplementedBy
  // + exceptions
  // + const/volatile specifiers
  // - examples
  // - source definition
  // - source references
  // - source referenced by
  // - body code
  // - template arguments
  //     (templateArguments(), definitionTemplateParameterLists())

  QCString memType;
  QCString name;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberType_Define:      memType="define";     break;
    case MemberType_EnumValue:   memType="enumvalue";  break;
    case MemberType_Property:    memType="property";   break;
    case MemberType_Variable:    memType="variable";   break;
    case MemberType_Typedef:     memType="typedef";    break;
    case MemberType_Enumeration: memType="enum";       break;
    case MemberType_Function:    memType="function";   isFunc=TRUE; break;
    case MemberType_Signal:      memType="signal";     isFunc=TRUE; break;
    case MemberType_Friend:      memType="friend";     isFunc=TRUE; break;
    case MemberType_DCOP:        memType="dcop";       isFunc=TRUE; break;
    case MemberType_Slot:        memType="slot";       isFunc=TRUE; break;
    case MemberType_Event:       memType="event";      break;
    case MemberType_Interface:   memType="interface";  break;
    case MemberType_Service:     memType="service";    break;
    case MemberType_Sequence:    memType="sequence";   break;
    case MemberType_Dictionary:  memType="dictionary"; break;
  }

  bool isFortran = md->getLanguage()==SrcLangExt::Fortran;
  name = md->name();
  if (md->isAnonymous()) name = "__unnamed" + name.right(name.length() - 1)+"__";

  m_output.openHash()
    .addFieldQuotedString("kind", memType)
    .addFieldQuotedString("name", name)
    .addFieldQuotedString("virtualness", getVirtualnessName(md->virtualness()))
    .addFieldQuotedString("protection", getProtectionName(md->protection()))
    .addFieldBoolean("static", md->isStatic());

  addPerlModDocBlock(m_output,"brief",md->getDefFileName(),md->getDefLine(),md->getOuterScope(),md,md->briefDescription());
  addPerlModDocBlock(m_output,"detailed",md->getDefFileName(),md->getDefLine(),md->getOuterScope(),md,md->documentation());
  if (md->memberType()!=MemberType_Define &&
      md->memberType()!=MemberType_Enumeration)
    m_output.addFieldQuotedString("type", md->typeString());

  const ArgumentList &al = md->argumentList();
  if (isFunc) //function
  {
    m_output.addFieldBoolean("const",    al.constSpecifier())
            .addFieldBoolean("volatile", al.volatileSpecifier());

    m_output.openList("parameters");
    const ArgumentList &declAl = md->declArgumentList();
    if (!declAl.empty())
    {
      auto defIt = al.begin();
      for (const Argument &a : declAl)
      {
	const Argument *defArg = nullptr;
        if (defIt!=al.end())
        {
          defArg = &(*defIt);
          ++defIt;
        }
	m_output.openHash();

	if (!a.name.isEmpty())
	  m_output.addFieldQuotedString("declaration_name", a.name);

	if (defArg && !defArg->name.isEmpty() && defArg->name!=a.name)
	  m_output.addFieldQuotedString("definition_name", defArg->name);

        if (isFortran && defArg && !defArg->type.isEmpty())
	  m_output.addFieldQuotedString("type", defArg->type);
	else if (!a.type.isEmpty())
	  m_output.addFieldQuotedString("type", a.type);

	if (!a.array.isEmpty())
	  m_output.addFieldQuotedString("array", a.array);

	if (!a.defval.isEmpty())
	  m_output.addFieldQuotedString("default_value", a.defval);

	if (!a.attrib.isEmpty())
	  m_output.addFieldQuotedString("attributes", a.attrib);

	m_output.closeHash();
      }
    }
    m_output.closeList();
  }
  else if (md->memberType()==MemberType_Define &&
	   md->argsString()!=nullptr) // define
  {
    m_output.openList("parameters");
    for (const Argument &a : al)
    {
      m_output.openHash()
	.addFieldQuotedString("name", a.type)
	.closeHash();
    }
    m_output.closeList();
  }
  else if (md->argsString()!=nullptr)
  {
    m_output.addFieldQuotedString("arguments", md->argsString());
  }

  if (!md->initializer().isEmpty())
    m_output.addFieldQuotedString("initializer", md->initializer());

  if (!md->excpString().isEmpty())
    m_output.addFieldQuotedString("exceptions", md->excpString());

  if (md->memberType()==MemberType_Enumeration) // enum
  {
    const MemberVector &enumFields = md->enumFieldList();
    m_output.addFieldQuotedString("type", md->enumBaseType());
    if (!enumFields.empty())
    {
      m_output.openList("values");
      for (const auto &emd : enumFields)
      {
	m_output.openHash()
	  .addFieldQuotedString("name", emd->name());

	if (!emd->initializer().isEmpty())
	  m_output.addFieldQuotedString("initializer", emd->initializer());

	addPerlModDocBlock(m_output,"brief",emd->getDefFileName(),emd->getDefLine(),emd->getOuterScope(),emd,emd->briefDescription());

	addPerlModDocBlock(m_output,"detailed",emd->getDefFileName(),emd->getDefLine(),emd->getOuterScope(),emd,emd->documentation());

	m_output.closeHash();
      }
      m_output.closeList();
    }
  }

  if (md->memberType() == MemberType_Variable && !md->bitfieldString().isEmpty())
  {
    QCString bitfield = md->bitfieldString();
    if (bitfield.at(0) == ':') bitfield = bitfield.mid(1);
    m_output.addFieldQuotedString("bitfield", bitfield);
  }

  const MemberDef *rmd = md->reimplements();
  if (rmd)
    m_output.openHash("reimplements")
      .addFieldQuotedString("name", rmd->name())
      .closeHash();

  const MemberVector &rbml = md->reimplementedBy();
  if (!rbml.empty())
  {
    m_output.openList("reimplemented_by");
    for (const auto &rbmd : rbml)
      m_output.openHash()
	.addFieldQuotedString("name", rbmd->name())
	.closeHash();
    m_output.closeList();
  }

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModSection(const Definition *d,
					      MemberList *ml,const QCString &name,const QCString &header)
{
  if (ml==nullptr) return; // empty list

  m_output.openHash(name);

  if (!header.isEmpty())
    m_output.addFieldQuotedString("header", header);

  m_output.openList("members");
  for (const auto &md : *ml)
  {
    generatePerlModForMember(md,d);
  }
  m_output.closeList()
    .closeHash();
}

void PerlModGenerator::addListOfAllMembers(const ClassDef *cd)
{
  m_output.openList("all_members");
  for (auto &mni : cd->memberNameInfoLinkedMap())
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      const ClassDef  *mcd=md->getClassDef();

      m_output.openHash()
        .addFieldQuotedString("name", md->name())
        .addFieldQuotedString("virtualness", getVirtualnessName(md->virtualness()))
        .addFieldQuotedString("protection", getProtectionName(mi->prot()));

      if (!mi->ambiguityResolutionScope().isEmpty())
        m_output.addFieldQuotedString("ambiguity_scope", mi->ambiguityResolutionScope());

      m_output.addFieldQuotedString("scope", mcd->name())
        .closeHash();
    }
  }
  m_output.closeList();
}

void PerlModGenerator::generatePerlUserDefinedSection(const Definition *d, const MemberGroupList &mgl)
{
  if (!mgl.empty())
  {
    m_output.openList("user_defined");
    for (const auto &mg : mgl)
    {
      m_output.openHash();
      if (!mg->header().isEmpty())
      {
        m_output.addFieldQuotedString("header", mg->header());
      }

      if (!mg->members().empty())
      {
        m_output.openList("members");
        for (const auto &md : mg->members())
        {
          generatePerlModForMember(md, d);
        }
        m_output.closeList();
      }
      m_output.closeHash();
    }
    m_output.closeList();
  }
}

void PerlModGenerator::addIncludeInfo(const IncludeInfo *ii)
{
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      m_output.openHash("includes");
      m_output.addFieldBoolean("local", ii->kind==IncludeKind::IncludeLocal || ii->kind==IncludeKind::ImportLocal)
	.addFieldQuotedString("name", nm)
	.closeHash();
    }
  }
}

void PerlModGenerator::generatePerlModForClass(const ClassDef *cd)
{
  // + brief description
  // + detailed description
  // + template argument list(s)
  // - include file
  // + member groups
  // + inheritance diagram
  // + list of direct super classes
  // + list of direct sub classes
  // + list of inner classes
  // + collaboration diagram
  // + list of all members
  // + user defined member sections
  // + standard member sections
  // + detailed member documentation
  // - examples using the class

  if (cd->isReference())        return; // skip external references.
  if (cd->isAnonymous())        return; // skip anonymous compounds.
  if (cd->templateMaster()!=nullptr)  return; // skip generated template instances.

  m_output.openHash()
    .addFieldQuotedString("name", cd->name());
  /* DGA: fix # #7547 Perlmod does not generate "kind" information to discriminate struct/union */
  m_output.addFieldQuotedString("kind", cd->compoundTypeString());

  if (!cd->baseClasses().empty())
  {
    m_output.openList("base");
    for (const auto &bcd : cd->baseClasses())
    {
      m_output.openHash()
	.addFieldQuotedString("name", bcd.classDef->displayName())
	.addFieldQuotedString("virtualness", getVirtualnessName(bcd.virt))
	.addFieldQuotedString("protection", getProtectionName(bcd.prot))
	.closeHash();
    }
    m_output.closeList();
  }

  if (!cd->subClasses().empty())
  {
    m_output.openList("derived");
    for (const auto &bcd : cd->subClasses())
    {
      m_output.openHash()
	.addFieldQuotedString("name", bcd.classDef->displayName())
	.addFieldQuotedString("virtualness", getVirtualnessName(bcd.virt))
	.addFieldQuotedString("protection", getProtectionName(bcd.prot))
	.closeHash();
    }
    m_output.closeList();
  }

  {
    m_output.openList("inner");
    for (const auto &icd : cd->getClasses())
      m_output.openHash()
	.addFieldQuotedString("name", icd->name())
	.closeHash();
    m_output.closeList();
  }

  addIncludeInfo(cd->includeInfo());

  addTemplateList(cd,m_output);
  addListOfAllMembers(cd);
  generatePerlUserDefinedSection(cd, cd->getMemberGroups());

  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubTypes),"public_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubMethods),"public_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubAttribs),"public_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubSlots),"public_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_signals),"signals");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_dcopMethods),"dcop_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_properties),"properties");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubStaticMethods),"public_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_pubStaticAttribs),"public_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proTypes),"protected_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proMethods),"protected_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proAttribs),"protected_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proSlots),"protected_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proStaticMethods),"protected_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_proStaticAttribs),"protected_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priTypes),"private_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priMethods),"private_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priAttribs),"private_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priSlots),"private_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priStaticMethods),"private_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_priStaticAttribs),"private_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_friends),"friend_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberListType_related),"related_methods");

  addPerlModDocBlock(m_output,"brief",cd->getDefFileName(),cd->getDefLine(),cd,nullptr,cd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",cd->getDefFileName(),cd->getDefLine(),cd,nullptr,cd->documentation());

#if 0
  DotClassGraph inheritanceGraph(cd,DotClassGraph::Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "    <inheritancegraph>" << endl;
    inheritanceGraph.writePerlMod(t);
    t << "    </inheritancegraph>" << endl;
  }
  DotClassGraph collaborationGraph(cd,DotClassGraph::Implementation);
  if (!collaborationGraph.isTrivial())
  {
    t << "    <collaborationgraph>" << endl;
    collaborationGraph.writePerlMod(t);
    t << "    </collaborationgraph>" << endl;
  }
  t << "    <location file=\""
    << cd->getDefFileName() << "\" line=\""
    << cd->getDefLine() << "\"";
    if (cd->getStartBodyLine()!=-1)
    {
      t << " bodystart=\"" << cd->getStartBodyLine() << "\" bodyend=\""
        << cd->getEndBodyLine() << "\"";
    }
  t << "/>" << endl;
#endif

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForConcept(const ConceptDef *cd)
{
  if (cd->isReference()) return; // skip external references

  m_output.openHash()
    .addFieldQuotedString("name", cd->name());

  addIncludeInfo(cd->includeInfo());
  addTemplateList(cd,m_output);
  m_output.addFieldQuotedString("initializer", cd->initializer());
  addPerlModDocBlock(m_output,"brief",cd->getDefFileName(),cd->getDefLine(),nullptr,nullptr,cd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",cd->getDefFileName(),cd->getDefLine(),nullptr,nullptr,cd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForModule(const ModuleDef *mod)
{
  // + contained class definitions
  // + contained concept definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location (file_id, line, column)
  // - exports
  // + used files

  if (mod->isReference()) return; // skip external references

  m_output.openHash()
    .addFieldQuotedString("name", mod->name());

  generatePerlUserDefinedSection(mod, mod->getMemberGroups());

  if (!mod->getClasses().empty())
  {
    m_output.openList("classes");
    for (const auto &cd : mod->getClasses())
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!mod->getConcepts().empty())
  {
    m_output.openList("concepts");
    for (const auto &cd : mod->getConcepts())
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  generatePerlModSection(mod,mod->getMemberList(MemberListType_decTypedefMembers),"typedefs");
  generatePerlModSection(mod,mod->getMemberList(MemberListType_decEnumMembers),"enums");
  generatePerlModSection(mod,mod->getMemberList(MemberListType_decFuncMembers),"functions");
  generatePerlModSection(mod,mod->getMemberList(MemberListType_decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",mod->getDefFileName(),mod->getDefLine(),nullptr,nullptr,mod->briefDescription());
  addPerlModDocBlock(m_output,"detailed",mod->getDefFileName(),mod->getDefLine(),nullptr,nullptr,mod->documentation());

  if (!mod->getUsedFiles().empty())
  {
    m_output.openList("files");
    for (const auto &fd : mod->getUsedFiles())
      m_output.openHash()
	.addFieldQuotedString("name", fd->name())
	.closeHash();
    m_output.closeList();
  }

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForNamespace(const NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location
  // - files containing (parts of) the namespace definition

  if (nd->isReference()) return; // skip external references

  m_output.openHash()
    .addFieldQuotedString("name", nd->name());

  if (!nd->getClasses().empty())
  {
    m_output.openList("classes");
    for (const auto &cd : nd->getClasses())
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!nd->getNamespaces().empty())
  {
    m_output.openList("namespaces");
    for (const auto &ind : nd->getNamespaces())
      m_output.openHash()
	.addFieldQuotedString("name", ind->name())
	.closeHash();
    m_output.closeList();
  }

  generatePerlUserDefinedSection(nd, nd->getMemberGroups());

  generatePerlModSection(nd,nd->getMemberList(MemberListType_decDefineMembers),"defines");
  generatePerlModSection(nd,nd->getMemberList(MemberListType_decProtoMembers),"prototypes");
  generatePerlModSection(nd,nd->getMemberList(MemberListType_decTypedefMembers),"typedefs");
  generatePerlModSection(nd,nd->getMemberList(MemberListType_decEnumMembers),"enums");
  generatePerlModSection(nd,nd->getMemberList(MemberListType_decFuncMembers),"functions");
  generatePerlModSection(nd,nd->getMemberList(MemberListType_decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",nd->getDefFileName(),nd->getDefLine(),nullptr,nullptr,nd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",nd->getDefFileName(),nd->getDefLine(),nullptr,nullptr,nd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForFile(const FileDef *fd)
{
  // + includes files
  // + includedby files
  // - include graph
  // - included by graph
  // - contained class definitions
  // - contained namespace definitions
  // - member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // - source code
  // - location
  // - number of lines

  if (fd->isReference()) return;

  m_output.openHash()
    .addFieldQuotedString("name", fd->name());

  m_output.openList("includes");
  for (const auto &inc: fd->includeFileList())
  {
    m_output.openHash()
      .addFieldQuotedString("name", inc.includeName);
    if (inc.fileDef && !inc.fileDef->isReference())
    {
      m_output.addFieldQuotedString("ref", inc.fileDef->getOutputFileBase());
    }
    m_output.closeHash();
  }
  m_output.closeList();

  m_output.openList("included_by");
  for (const auto &inc : fd->includedByFileList())
  {
    m_output.openHash()
      .addFieldQuotedString("name", inc.includeName);
    if (inc.fileDef && !inc.fileDef->isReference())
    {
      m_output.addFieldQuotedString("ref", inc.fileDef->getOutputFileBase());
    }
    m_output.closeHash();
  }
  m_output.closeList();

  generatePerlUserDefinedSection(fd, fd->getMemberGroups());

  generatePerlModSection(fd,fd->getMemberList(MemberListType_decDefineMembers),"defines");
  generatePerlModSection(fd,fd->getMemberList(MemberListType_decProtoMembers),"prototypes");
  generatePerlModSection(fd,fd->getMemberList(MemberListType_decTypedefMembers),"typedefs");
  generatePerlModSection(fd,fd->getMemberList(MemberListType_decEnumMembers),"enums");
  generatePerlModSection(fd,fd->getMemberList(MemberListType_decFuncMembers),"functions");
  generatePerlModSection(fd,fd->getMemberList(MemberListType_decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",fd->getDefFileName(),fd->getDefLine(),nullptr,nullptr,fd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",fd->getDefFileName(),fd->getDefLine(),nullptr,nullptr,fd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForGroup(const GroupDef *gd)
{
  // + members
  // + member groups
  // + files
  // + classes
  // + namespaces
  // - packages
  // + pages
  // + child groups
  // - examples
  // + brief description
  // + detailed description

  if (gd->isReference()) return; // skip external references

  m_output.openHash()
    .addFieldQuotedString("name", gd->name())
    .addFieldQuotedString("title", gd->groupTitle());

  if (!gd->getFiles().empty())
  {
    m_output.openList("files");
    for (const auto &fd : gd->getFiles())
      m_output.openHash()
	.addFieldQuotedString("name", fd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getClasses().empty())
  {
    m_output.openList("classes");
    for (const auto &cd : gd->getClasses())
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getConcepts().empty())
  {
    m_output.openList("concepts");
    for (const auto &cd : gd->getConcepts())
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getModules().empty())
  {
    m_output.openList("modules");
    for (const auto &mod : gd->getModules())
      m_output.openHash()
	.addFieldQuotedString("name", mod->name())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getNamespaces().empty())
  {
    m_output.openList("namespaces");
    for (const auto &nd : gd->getNamespaces())
      m_output.openHash()
	.addFieldQuotedString("name", nd->name())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getPages().empty())
  {
    m_output.openList("pages");
    for (const auto &pd : gd->getPages())
      m_output.openHash()
	.addFieldQuotedString("title", pd->title())
	.closeHash();
    m_output.closeList();
  }

  if (!gd->getSubGroups().empty())
  {
    m_output.openList("groups");
    for (const auto &sgd : gd->getSubGroups())
      m_output.openHash()
	.addFieldQuotedString("title", sgd->groupTitle())
	.closeHash();
    m_output.closeList();
  }

  generatePerlUserDefinedSection(gd, gd->getMemberGroups());

  generatePerlModSection(gd,gd->getMemberList(MemberListType_decDefineMembers),"defines");
  generatePerlModSection(gd,gd->getMemberList(MemberListType_decProtoMembers),"prototypes");
  generatePerlModSection(gd,gd->getMemberList(MemberListType_decTypedefMembers),"typedefs");
  generatePerlModSection(gd,gd->getMemberList(MemberListType_decEnumMembers),"enums");
  generatePerlModSection(gd,gd->getMemberList(MemberListType_decFuncMembers),"functions");
  generatePerlModSection(gd,gd->getMemberList(MemberListType_decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",gd->getDefFileName(),gd->getDefLine(),nullptr,nullptr,gd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",gd->getDefFileName(),gd->getDefLine(),nullptr,nullptr,gd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForPage(PageDef *pd)
{
  // + name
  // + title
  // + documentation

  if (pd->isReference()) return;

  m_output.openHash()
    .addFieldQuotedString("name", pd->name());

  const SectionInfo *si = SectionManager::instance().find(pd->name());
  if (si)
    m_output.addFieldQuotedString("title4", filterTitle(si->title()));

  addPerlModDocBlock(m_output,"detailed",pd->docFile(),pd->docLine(),nullptr,nullptr,pd->documentation());
  m_output.closeHash();
}

bool PerlModGenerator::generatePerlModOutput()
{
  std::ofstream outputFileStream;
  if (!createOutputFile(outputFileStream, pathDoxyDocsPM))
    return false;

  PerlModOutputStream outputStream(outputFileStream);
  m_output.setPerlModOutputStream(&outputStream);
  m_output.add("$doxydocs=").openHash();

  m_output.openList("classes");
  for (const auto &cd : *Doxygen::classLinkedMap)
    generatePerlModForClass(cd.get());
  m_output.closeList();

  m_output.openList("concepts");
  for (const auto &cd : *Doxygen::conceptLinkedMap)
    generatePerlModForConcept(cd.get());
  m_output.closeList();

  m_output.openList("modules");
  for (const auto &mod : ModuleManager::instance().modules())
    generatePerlModForModule(mod.get());
  m_output.closeList();

  m_output.openList("namespaces");
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
    generatePerlModForNamespace(nd.get());
  m_output.closeList();

  m_output.openList("files");
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      generatePerlModForFile(fd.get());
    }
  }
  m_output.closeList();

  m_output.openList("groups");
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    generatePerlModForGroup(gd.get());
  }
  m_output.closeList();

  m_output.openList("pages");
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    generatePerlModForPage(pd.get());
  }
  if (Doxygen::mainPage)
  {
    generatePerlModForPage(Doxygen::mainPage.get());
  }
  m_output.closeList();

  m_output.closeHash().add(";\n1;\n");
  m_output.reset();
  return true;
}

bool PerlModGenerator::createOutputFile(std::ofstream &f, const QCString &s)
{
  f = Portable::openOutputStream(s);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n", qPrint(s));
    return false;
  }
  return true;
}

bool PerlModGenerator::createOutputDir(Dir &perlModDir)
{
  std::string outputDirectory = Config_getString(OUTPUT_DIRECTORY).str();
  perlModDir.setPath(outputDirectory+"/perlmod");
  if (!perlModDir.exists() && !perlModDir.mkdir(outputDirectory+"/perlmod"))
  {
    err("Could not create perlmod directory in %s\n",outputDirectory.data());
    return false;
  }
  return true;
}

bool PerlModGenerator::generateDoxyStructurePM()
{
  std::ofstream doxyModelPMStream;
  if (!createOutputFile(doxyModelPMStream, pathDoxyStructurePM))
    return false;

  doxyModelPMStream <<
    "sub memberlist($) {\n"
    "    my $prefix = $_[0];\n"
    "    return\n"
    "\t[ \"hash\", $prefix . \"s\",\n"
    "\t  {\n"
    "\t    members =>\n"
    "\t      [ \"list\", $prefix . \"List\",\n"
    "\t\t[ \"hash\", $prefix,\n"
    "\t\t  {\n"
    "\t\t    kind => [ \"string\", $prefix . \"Kind\" ],\n"
    "\t\t    name => [ \"string\", $prefix . \"Name\" ],\n"
    "\t\t    static => [ \"string\", $prefix . \"Static\" ],\n"
    "\t\t    virtualness => [ \"string\", $prefix . \"Virtualness\" ],\n"
    "\t\t    protection => [ \"string\", $prefix . \"Protection\" ],\n"
    "\t\t    type => [ \"string\", $prefix . \"Type\" ],\n"
    "\t\t    parameters =>\n"
    "\t\t      [ \"list\", $prefix . \"Params\",\n"
    "\t\t\t[ \"hash\", $prefix . \"Param\",\n"
    "\t\t\t  {\n"
    "\t\t\t    declaration_name => [ \"string\", $prefix . \"ParamName\" ],\n"
    "\t\t\t    type => [ \"string\", $prefix . \"ParamType\" ],\n"
    "\t\t\t  },\n"
    "\t\t\t],\n"
    "\t\t      ],\n"
    "\t\t    detailed =>\n"
    "\t\t      [ \"hash\", $prefix . \"Detailed\",\n"
    "\t\t\t{\n"
    "\t\t\t  doc => [ \"doc\", $prefix . \"DetailedDoc\" ],\n"
    "\t\t\t  return => [ \"doc\", $prefix . \"Return\" ],\n"
    "\t\t\t  see => [ \"doc\", $prefix . \"See\" ],\n"
    "\t\t\t  params =>\n"
    "\t\t\t    [ \"list\", $prefix . \"PDBlocks\",\n"
    "\t\t\t      [ \"hash\", $prefix . \"PDBlock\",\n"
    "\t\t\t\t{\n"
    "\t\t\t\t  parameters =>\n"
    "\t\t\t\t    [ \"list\", $prefix . \"PDParams\",\n"
    "\t\t\t\t      [ \"hash\", $prefix . \"PDParam\",\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t  name => [ \"string\", $prefix . \"PDParamName\" ],\n"
    "\t\t\t\t\t},\n"
    "\t\t\t\t      ],\n"
    "\t\t\t\t    ],\n"
    "\t\t\t\t  doc => [ \"doc\", $prefix . \"PDDoc\" ],\n"
    "\t\t\t\t},\n"
    "\t\t\t      ],\n"
    "\t\t\t    ],\n"
    "\t\t\t},\n"
    "\t\t      ],\n"
    "\t\t  },\n"
    "\t\t],\n"
    "\t      ],\n"
    "\t  },\n"
    "\t];\n"
    "}\n"
    "\n"
    "$doxystructure =\n"
    "    [ \"hash\", \"Root\",\n"
    "      {\n"
    "\tfiles =>\n"
    "\t  [ \"list\", \"Files\",\n"
    "\t    [ \"hash\", \"File\",\n"
    "\t      {\n"
    "\t\tname => [ \"string\", \"FileName\" ],\n"
    "\t\ttypedefs => memberlist(\"FileTypedef\"),\n"
    "\t\tvariables => memberlist(\"FileVariable\"),\n"
    "\t\tfunctions => memberlist(\"FileFunction\"),\n"
    "\t\tdetailed =>\n"
    "\t\t  [ \"hash\", \"FileDetailed\",\n"
    "\t\t    {\n"
    "\t\t      doc => [ \"doc\", \"FileDetailedDoc\" ],\n"
    "\t\t    },\n"
    "\t\t  ],\n"
    "\t      },\n"
    "\t    ],\n"
    "\t  ],\n"
    "\tpages =>\n"
    "\t  [ \"list\", \"Pages\",\n"
    "\t    [ \"hash\", \"Page\",\n"
    "\t      {\n"
    "\t\tname => [ \"string\", \"PageName\" ],\n"
    "\t\tdetailed =>\n"
    "\t\t  [ \"hash\", \"PageDetailed\",\n"
    "\t\t    {\n"
    "\t\t      doc => [ \"doc\", \"PageDetailedDoc\" ],\n"
    "\t\t    },\n"
    "\t\t  ],\n"
    "\t      },\n"
    "\t    ],\n"
    "\t  ],\n"
    "\tclasses =>\n"
    "\t  [ \"list\", \"Classes\",\n"
    "\t    [ \"hash\", \"Class\",\n"
    "\t      {\n"
    "\t\tname => [ \"string\", \"ClassName\" ],\n"
    "\t\tpublic_typedefs => memberlist(\"ClassPublicTypedef\"),\n"
    "\t\tpublic_methods => memberlist(\"ClassPublicMethod\"),\n"
    "\t\tpublic_members => memberlist(\"ClassPublicMember\"),\n"
    "\t\tprotected_typedefs => memberlist(\"ClassProtectedTypedef\"),\n"
    "\t\tprotected_methods => memberlist(\"ClassProtectedMethod\"),\n"
    "\t\tprotected_members => memberlist(\"ClassProtectedMember\"),\n"
    "\t\tprivate_typedefs => memberlist(\"ClassPrivateTypedef\"),\n"
    "\t\tprivate_methods => memberlist(\"ClassPrivateMethod\"),\n"
    "\t\tprivate_members => memberlist(\"ClassPrivateMember\"),\n"
    "\t\tdetailed =>\n"
    "\t\t  [ \"hash\", \"ClassDetailed\",\n"
    "\t\t    {\n"
    "\t\t      doc => [ \"doc\", \"ClassDetailedDoc\" ],\n"
    "\t\t    },\n"
    "\t\t  ],\n"
    "\t      },\n"
    "\t    ],\n"
    "\t  ],\n"
    "\tgroups =>\n"
    "\t  [ \"list\", \"Groups\",\n"
    "\t    [ \"hash\", \"Group\",\n"
    "\t      {\n"
    "\t\tname => [ \"string\", \"GroupName\" ],\n"
    "\t\ttitle => [ \"string\", \"GroupTitle\" ],\n"
    "\t\tfiles =>\n"
    "\t\t  [ \"list\", \"Files\",\n"
    "\t\t    [ \"hash\", \"File\",\n"
    "\t\t      {\n"
    "\t\t        name => [ \"string\", \"Filename\" ]\n"
    "\t\t      }\n"
    "\t\t    ],\n"
    "\t\t  ],\n"
    "\t\tclasses  =>\n"
    "\t\t  [ \"list\", \"Classes\",\n"
    "\t\t    [ \"hash\", \"Class\",\n"
    "\t\t      {\n"
    "\t\t        name => [ \"string\", \"Classname\" ]\n"
    "\t\t      }\n"
    "\t\t    ],\n"
    "\t\t  ],\n"
    "\t\tnamespaces =>\n"
    "\t\t  [ \"list\", \"Namespaces\",\n"
    "\t\t    [ \"hash\", \"Namespace\",\n"
    "\t\t      {\n"
    "\t\t        name => [ \"string\", \"NamespaceName\" ]\n"
    "\t\t      }\n"
    "\t\t    ],\n"
    "\t\t  ],\n"
    "\t\tpages =>\n"
    "\t\t  [ \"list\", \"Pages\",\n"
    "\t\t    [ \"hash\", \"Page\","
    "\t\t      {\n"
    "\t\t        title => [ \"string\", \"PageName\" ]\n"
    "\t\t      }\n"
    "\t\t    ],\n"
    "\t\t  ],\n"
    "\t\tgroups =>\n"
    "\t\t  [ \"list\", \"Groups\",\n"
    "\t\t    [ \"hash\", \"Group\",\n"
    "\t\t      {\n"
    "\t\t        title => [ \"string\", \"GroupName\" ]\n"
    "\t\t      }\n"
    "\t\t    ],\n"
    "\t\t  ],\n"
    "\t\tfunctions => memberlist(\"GroupFunction\"),\n"
    "\t\tdetailed =>\n"
    "\t\t  [ \"hash\", \"GroupDetailed\",\n"
    "\t\t    {\n"
    "\t\t      doc => [ \"doc\", \"GroupDetailedDoc\" ],\n"
    "\t\t    },\n"
    "\t\t  ],\n"
    "\t      }\n"
    "\t    ],\n"
    "\t  ],\n"
    "      },\n"
    "    ];\n"
    "\n"
    "1;\n";

  return true;
}

bool PerlModGenerator::generateDoxyRules()
{
  std::ofstream doxyRulesStream;
  if (!createOutputFile(doxyRulesStream, pathDoxyRules))
    return false;

  bool perlmodLatex = Config_getBool(PERLMOD_LATEX);
  QCString prefix = Config_getString(PERLMOD_MAKEVAR_PREFIX);

  doxyRulesStream <<
    prefix << "DOXY_EXEC_PATH = " << pathDoxyExec << "\n" <<
    prefix << "DOXYFILE = " << pathDoxyfile << "\n" <<
    prefix << "DOXYDOCS_PM = " << pathDoxyDocsPM << "\n" <<
    prefix << "DOXYSTRUCTURE_PM = " << pathDoxyStructurePM << "\n" <<
    prefix << "DOXYRULES = " << pathDoxyRules << "\n";
  if (perlmodLatex)
    doxyRulesStream <<
      prefix << "DOXYLATEX_PL = " << pathDoxyLatexPL << "\n" <<
      prefix << "DOXYLATEXSTRUCTURE_PL = " << pathDoxyLatexStructurePL << "\n" <<
      prefix << "DOXYSTRUCTURE_TEX = " << pathDoxyStructureTex << "\n" <<
      prefix << "DOXYDOCS_TEX = " << pathDoxyDocsTex << "\n" <<
      prefix << "DOXYFORMAT_TEX = " << pathDoxyFormatTex << "\n" <<
      prefix << "DOXYLATEX_TEX = " << pathDoxyLatexTex << "\n" <<
      prefix << "DOXYLATEX_DVI = " << pathDoxyLatexDVI << "\n" <<
      prefix << "DOXYLATEX_PDF = " << pathDoxyLatexPDF << "\n";

  doxyRulesStream <<
    "\n"
    ".PHONY: clean-perlmod\n"
    "clean-perlmod::\n"
    "\trm -f $(" << prefix << "DOXYSTRUCTURE_PM) \\\n"
    "\t$(" << prefix << "DOXYDOCS_PM)";
  if (perlmodLatex)
    doxyRulesStream <<
      " \\\n"
      "\t$(" << prefix << "DOXYLATEX_PL) \\\n"
      "\t$(" << prefix << "DOXYLATEXSTRUCTURE_PL) \\\n"
      "\t$(" << prefix << "DOXYDOCS_TEX) \\\n"
      "\t$(" << prefix << "DOXYSTRUCTURE_TEX) \\\n"
      "\t$(" << prefix << "DOXYFORMAT_TEX) \\\n"
      "\t$(" << prefix << "DOXYLATEX_TEX) \\\n"
      "\t$(" << prefix << "DOXYLATEX_PDF) \\\n"
      "\t$(" << prefix << "DOXYLATEX_DVI) \\\n"
      "\t$(addprefix $(" << prefix << "DOXYLATEX_TEX:tex=),out aux log)";
  doxyRulesStream << "\n\n";

  doxyRulesStream <<
    "$(" << prefix << "DOXYRULES) \\\n"
    "$(" << prefix << "DOXYMAKEFILE) \\\n"
    "$(" << prefix << "DOXYSTRUCTURE_PM) \\\n"
    "$(" << prefix << "DOXYDOCS_PM)";
  if (perlmodLatex) {
    doxyRulesStream <<
      " \\\n"
      "$(" << prefix << "DOXYLATEX_PL) \\\n"
      "$(" << prefix << "DOXYLATEXSTRUCTURE_PL) \\\n"
      "$(" << prefix << "DOXYFORMAT_TEX) \\\n"
      "$(" << prefix << "DOXYLATEX_TEX)";
  }
  doxyRulesStream <<
    ": \\\n"
    "\t$(" << prefix << "DOXYFILE)\n"
    "\tcd $(" << prefix << "DOXY_EXEC_PATH) ; doxygen \"$<\"\n";

  if (perlmodLatex) {
    doxyRulesStream <<
      "\n"
      "$(" << prefix << "DOXYDOCS_TEX): \\\n"
      "$(" << prefix << "DOXYLATEX_PL) \\\n"
      "$(" << prefix << "DOXYDOCS_PM)\n"
      "\tperl -I\"$(<D)\" \"$<\" >\"$@\"\n"
      "\n"
      "$(" << prefix << "DOXYSTRUCTURE_TEX): \\\n"
      "$(" << prefix << "DOXYLATEXSTRUCTURE_PL) \\\n"
      "$(" << prefix << "DOXYSTRUCTURE_PM)\n"
      "\tperl -I\"$(<D)\" \"$<\" >\"$@\"\n"
      "\n"
      "$(" << prefix << "DOXYLATEX_PDF) \\\n"
      "$(" << prefix << "DOXYLATEX_DVI): \\\n"
      "$(" << prefix << "DOXYLATEX_TEX) \\\n"
      "$(" << prefix << "DOXYFORMAT_TEX) \\\n"
      "$(" << prefix << "DOXYSTRUCTURE_TEX) \\\n"
      "$(" << prefix << "DOXYDOCS_TEX)\n"
      "\n"
      "$(" << prefix << "DOXYLATEX_PDF): \\\n"
      "$(" << prefix << "DOXYLATEX_TEX)\n"
      "\tpdflatex -interaction=nonstopmode \"$<\"\n"
      "\n"
      "$(" << prefix << "DOXYLATEX_DVI): \\\n"
      "$(" << prefix << "DOXYLATEX_TEX)\n"
      "\tlatex -interaction=nonstopmode \"$<\"\n";
  }

  return true;
}

// perlmodPython PerlModGenerator file creation code <start>
bool PerlModGenerator::generatePythonPut2cpp_py()
{
  std::ofstream pythonStream;
  if (!createOutputFile(pythonStream, pathPythonPut2cpp_py))
    return false;

  pythonStream <<
    "import sys\n"
    "import argparse\n"
    "import re\n"
    "import os\n"
    "\n"
    "class Solution:\n"
    "    \'\'\'\n"
    "    we replace several points.\n"
    "    - path define\n"
    "    - declare function\n"
    "    - file creation code\n"
    "    - code in generate function\n"
    "    - Makefile\n"
    "    \'\'\'\n"
    "    def __init__(self,inputdir,outlog,perlmodgen,debug):\n"
    "        \'\'\'\n"
    "            This is  init funciton \"\"\" test \"\"\"\n"
    "        \'\'\'\n"
    "        self.inputdir = inputdir\n"
    "        self.outlog = outlog\n"
    "        self.perlmodgen = perlmodgen\n"
    "        self.debug = debug\n"
    "        if self.perlmodgen and not os.path.exists(self.perlmodgen):\n"
    "            print(\'error:  not exist file :\', self.perlmodgen)\n"
    "            print(\' !! set --perlmodgen option with the location of perlmodgen.cpp\')\n"
    "            quit(4)\n"
    "        files = os.listdir(self.inputdir)\n"
    "        print(\'files:\',files)\n"
    "        self.p = []\n"
    "        self.o = {}\n"
    "        self.o[\'path define\'] = []\n"
    "        self.o[\'declare function\'] = []\n"
    "        self.o[\'file creation code\'] = []\n"
    "        self.o[\'code in generate function\'] = []\n"
    "        self.o[\'Makefile\'] = []\n"
    "        self.p.append(\'\'\'class PerlModGenerator\'\'\')\n"
    "        self.p.append(\'\'\'{\'\'\')\n"
    "        self.p.append(\'\'\'  QCString pathDoxyRules;\'\'\')\n"
    "        self.p.append(\'\'\'  // perlmodPython code\'\'\')\n"
    "        for file in files:\n"
    "            self.o[\'path define\'].append(\'\'\'  QCString path{f};\'\'\'.format(f=self.createPythonName(file)))\n"
    "        self.p += self.o[\'path define\']\n"
    "        self.p.append(\'\'\'  QCString pathMakefile;\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'\'\'  bool generateDoxyRules();\'\'\')\n"
    "        self.p.append(\'\'\'  // perlmodPython code\'\'\')\n"
    "        for file in files:\n"
    "            self.o[\'declare function\'].append(\'\'\'  bool generate{f}();\'\'\'.format(f=self.createPythonName(file)))\n"
    "        self.p += self.o[\'declare function\']\n"
    "        self.p.append(\'\'\'  bool generateMakefile();\'\'\')\n"
    "        self.p.append(\'\'\'}\'\'\')\n"
    "        self.p.append(\'\')\n"
    "\n"
    "        self.p.append(\'\'\'bool PerlModGenerator::generateDoxyRules()\'\'\')\n"
    "        self.p.append(\'\'\'{\'\'\')\n"
    "        self.p.append(\'\'\'}\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'\'\'// perlmodPython code\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        for file in files:\n"
    "            filepath = self.inputdir+\'/\'+file\n"
    "            ff = self.createPythonName(file)\n"
    "            cpp = []\n"
    "            with open(filepath,\'r\', encoding=\'utf-8\', errors=\'ignore\') as f:\n"
    "                lines = f.readlines()\n"
    "                 #bs = \' \\ \'\n"
    "                 #bs = bs.replace(\' \',\'\')\n"
    "                for line in lines:\n"
    "                    cpp.append(self.line2cpp(4,line))\n"
    "            self.o[\'file creation code\'].append(\'\'\'bool PerlModGenerator::generate{ff}()\'\'\'.format(ff=ff))\n"
    "            self.o[\'file creation code\'].append(\'\'\'{\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\'\'  std::ofstream pythonStream;\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\'\'  if (!createOutputFile(pythonStream, path{ff}))\'\'\'.format(ff=ff))\n"
    "            self.o[\'file creation code\'].append(\'\'\'    return false;\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\')\n"
    "            self.o[\'file creation code\'].append(\'\'\'  pythonStream <<\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\\n\'.join(cpp) + \';\')\n"
    "            self.o[\'file creation code\'].append(\'\')\n"
    "            self.o[\'file creation code\'].append(\'\'\'  return true;\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\'\'}\'\'\')\n"
    "            self.o[\'file creation code\'].append(\'\')\n"
    "        self.p += self.o[\'file creation code\']\n"
    "        self.p.append(\'\'\'bool PerlModGenerator::generateMakefile()\'\'\')\n"
    "        self.p.append(\'\'\'{\'\'\')\n"
    "        self.p.append(\'\'\'}\'\'\')\n"
    "        self.p.append(\'\')\n"
    "\n"
    "        self.p.append(\'\'\'void PerlModGenerator::generate()\'\'\')\n"
    "        self.p.append(\'\'\'{\'\'\')\n"
    "        self.p.append(\'\'\'  bool perlmodLatex = Config_getBool(PERLMOD_LATEX);\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'\'\'  if (perlmodLatex) {\'\'\')\n"
    "        self.p.append(\'\'\'  }\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'  // perlmodPython code\')\n"
    "        self.p.append(\'\')\n"
    "        generateList = []\n"
    "        for file in files:\n"
    "            ff = self.createPythonName(file)\n"
    "            self.o[\'code in generate function\'].append(\'\'\'    path{ff} = perlModAbsPath + \"/{f}\";\'\'\'.format(f=file,ff=ff))\n"
    "            generateList.append(\'generate\' + ff + \'()\')\n"
    "        print(\'generateList\',generateList)\n"
    "        self.o[\'code in generate function\'].append(\'\'\'    if (!(\'\'\' + \'\'\'\\n      && \'\'\'.join(generateList) + \'\'\'))\'\'\')\n"
    "        self.o[\'code in generate function\'].append(\'\'\'      return;\'\'\')\n"
    "        self.p += self.o[\'code in generate function\']\n"
    "        self.p.append(\'\'\'}\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'\'\'void generatePerlMod()\'\'\')\n"
    "        self.p.append(\'\'\'{\'\'\')\n"
    "        self.p.append(\'\'\'}\'\'\')\n"
    "        self.p.append(\'\')\n"
    "        self.p.append(\'\'\'  bool perlmodPython = Config_getBool(PERLMOD_PYTHON);\'\'\')\n"
    "        self.p.append(\'\'\'  if (perlmodPython) {\'\'\')\n"
    "        self.o[\'Makefile\'].append(\'\'\'    makefileStream <<\'\'\')\n"
    "        mk = []\n"
    "        mk.append(\'\')\n"
    "        mk.append(\'\'\'# code2cpp\'\'\')\n"
    "        mk.append(\'\'\'code2cpp:\'\'\')\n"
    "        mk.append(\'\'\'\\trm -rf input_perlmodgen_files\'\'\')\n"
    "        mk.append(\'\'\'\\tmkdir -p input_perlmodgen_files\'\'\')\n"
    "        for file in files:\n"
    "            mk.append(\'\'\'\\tcd input_perlmodgen_files; ln -s ../{f} {f}\'\'\'.format(f=file))\n"
    "        mk.append(\'\'\'\\t# if you want to add python file into perlmodgen.cpp , you can use it.\'\'\')\n"
    "        mk.append(\'\'\'\\t# first of all, you add your py in input_perlmodgen_files directory\'\'\')\n"
    "        mk.append(\'\'\'\\t# please change ../../../../src/perlmodgen.cpp to location of src/perlmodgen.cpp\'\'\')\n"
    "        mk.append(\'\'\'\\t# output filename : perlmodgen.cpp.mod\'\'\')\n"
    "        mk.append(\'\'\'\\tpython3 put2cpp.py --inputdir=input_perlmodgen_files --perlmodgen=../../../../src/perlmodgen.cpp --outlog=out-perlmodgen.log\'\'\')\n"
    "        mk.append(\'\')\n"
    "        mk.append(\'\'\'# run python code with DoxyDocs\'\'\')\n"
    "        mk.append(\'\'\'python_doc:\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \"if you want to get plantuml without running java ,\"\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \" 1. set PLANTUML_JAR_PATH = ./plantuml.jar in Doxygile\"\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \" 2. do \'touch plantuml.jar\'\"\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \" 3. run \'doxygen\' then your DoxyDocs.pm will include plantuml.\"\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \"\"\'\'\')\n"
    "        mk.append(\'\'\'\\tperl doxy2py.pl\'\'\')\n"
    "        mk.append(\'\'\'\\tpython3 sample.py --outfile=output.md\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \"\"\'\'\')\n"
    "        mk.append(\'\'\'\\t@echo \"refer to https://github.com/cheoljoo/doxygen_perlmod_python for getting more example and usage\"\'\'\')\n"
    "        mk.append(\'\')\n"
    "        self.o[\'Makefile\'].append(\'\\n\'.join([self.line2cpp(6,item) for item in mk]) + \';\')\n"
    "        self.p += self.o[\'Makefile\']\n"
    "        self.p.append(\'\'\'  }\'\'\')\n"
    "        with open(self.outlog ,\"w\") as f:\n"
    "            print(\'write:\',self.outlog)\n"
    "            f.write(\'\\n\'.join(self.p))\n"
    "        startRe = re.compile(\'^\\s*//\\s+perlmodPython\\s+PerlModGenerator\\s+(?P<key>[^<>]+)<start>\\s*\')\n"
    "        endRe = re.compile(\'^\\s*//\\s+perlmodPython\\s+PerlModGenerator\\s+(?P<key>[^<>]+)<end>\\s*\')\n"
    "        if self.perlmodgen:\n"
    "            pmg = []\n"
    "            lines = []\n"
    "            with open(self.perlmodgen,\'r\', encoding=\'utf-8\', errors=\'ignore\') as f:\n"
    "                lines = f.readlines()\n"
    "            flag = 0\n"
    "            startKey = \'\'\n"
    "            endKey = \'-\'\n"
    "            for line in lines:\n"
    "                line = line.rstrip()\n"
    "                grpStart = startRe.search(line)\n"
    "                grpEnd = endRe.search(line)\n"
    "                if grpStart:\n"
    "                    if flag == 1:\n"
    "                        print(\'error: already you were in  perlmodpython pair\')\n"
    "                        print(\'  !! please chek <start> ~ <end> pair\')\n"
    "                        quit(4)\n"
    "                    print(\'start [\',grpStart.group(\'key\').strip(),\']\',line)\n"
    "                    startKey = grpStart.group(\'key\').strip()\n"
    "                    flag = 1\n"
    "                    pmg.append(line)\n"
    "                elif grpEnd:\n"
    "                    if flag == 0:\n"
    "                        print(\'error: already you were out of perlmodpython pair\')\n"
    "                        print(\'  !! please chek <start> ~ <end> pair\')\n"
    "                        quit(4)\n"
    "                    print(\'end [\',grpEnd.group(\'key\').strip(),\']\',line)\n"
    "                    endKey = grpEnd.group(\'key\').strip()\n"
    "                    flag = 0\n"
    "                    pmg += self.o[endKey]\n"
    "                    pmg.append(line)\n"
    "                else:\n"
    "                    if flag == 0:\n"
    "                        pmg.append(line)\n"
    "            with open(self.perlmodgen + \'.mod\',\'w\') as f:\n"
    "                print(\'! output perlmodgen file:\',self.perlmodgen + \'.mod\')\n"
    "                s = \"\\n\".join(pmg)\n"
    "                f.write(s)\n"
    "\n"
    "    def createPythonName(self,file):\n"
    "        \'\'\'\n"
    "        create new path name\n"
    "        \'\'\'\n"
    "        if not file:\n"
    "            return file\n"
    "        file2 = file.replace(\'.\',\'_\')\n"
    "        fileList = list(file2)\n"
    "        ff = \'Python\' + fileList[0].upper() + \'\'.join(fileList[1:])\n"
    "        print(ff)\n"
    "        return ff\n"
    "\n"
    "    def line2cpp(self,indent,line):\n"
    "        \'\'\'\n"
    "        change characters for C++\n"
    "        \'\'\'\n"
    "        line = line.rstrip()\n"
    "        line = line.replace(\'\\\\\',\'\\\\\\\\\')\n"
    "        line = line.replace(\"\'\", \"\\\\\'\")\n"
    "        line = line.replace(\'\"\', \'\\\\\"\')\n"
    "        return \' \'*indent + \'\"\' + line + str(\'\\\\n\') + \'\"\'\n"
    "\n"
    "if (__name__ == \"__main__\"):\n"
    "    parser = argparse.ArgumentParser(\n"
    "        prog=sys.argv[0],\n"
    "        description=\n"
    "        \'put python code into cpp code and edit perlmodgen.cpp to add python code\'\n"
    "    )\n"
    "    parser.add_argument( \'--debug\', \'-d\' , action=\'store_const\' , const=1 , help=\'debug on\')\n"
    "    parser.add_argument( \'--inputdir\',\n"
    "        metavar=\"<str>\",\n"
    "        type=str,\n"
    "        default=\'input_perlmodgen_files\',\n"
    "        help=\'input directory with python code. files of this directory will be inserted in cpp\')\n"
    "    parser.add_argument( \'--perlmodgen\',\n"
    "        metavar=\"<str>\",\n"
    "        type=str,\n"
    "        default=\'./perlmodgen.cpp\',\n"
    "        help=\'location of perlmodgen.cpp.  python codes in inputdir will be updated.\')\n"
    "    parser.add_argument( \'--outlog\',\n"
    "        metavar=\"<str>\",\n"
    "        type=str,\n"
    "        default=\'out-perlmodgen.log\',\n"
    "        help=\'log file\')\n"
    "\n"
    "    args = parser.parse_args()\n"
    "    debug = args.debug\n"
    "\n"
    "    print(\'inputdir:\',args.inputdir)\n"
    "    print(\'input location of perlmodegen.cpp:\',args.perlmodgen)\n"
    "    S = Solution(args.inputdir,args.outlog,args.perlmodgen,args.debug)\n"
    "\n";

  return true;
}

bool PerlModGenerator::generatePythonREADME_md()
{
  std::ofstream pythonStream;
  if (!createOutputFile(pythonStream, pathPythonREADME_md))
    return false;

  pythonStream <<
    "# PERLMOD_PYTHON\n"
    "- developers want to use parsing data from doxygen.\n"
    "- they want to do something with python.\n"
    "\n"
    "- set **GENERATE_PERLMOD = YES**   and **PERLMOD_PYTHON = YES**\n"
    "\n"
    "## make python_doc\n"
    "- DoxyDocs.pm converts to DoxyDocs.py.\n"
    "  - DoxyDocs.py is loaded in python program.\n"
    "- sample.py makes markdown (output.md) file as final output.\n"
    "  - output.md includes table for classes and member functions.\n"
    "  - if doxygen comments include plantuml , output.md includes plantuml   if you set PLANTUML_JAR_PATH = ./plantuml.jar and this file exists.\n"
    "- output\n"
    "	- output.md : show markdown including plantuml with vscode (useful extension : markdown preview enhanced)\n"
    "\n"
    "## make code2cpp\n"
    "- add new file in input_perlmodgen_files directory\n"
    "    - add new line for new file in Makefile : `cd input_perlmodgen_files; ln -s ../[wanted_file] [wanted_file]`\n"
    "- all codes with related to PERLMOD_PYTHON are created by perlmodgen.cpp.\n"
    "- put2cpp.py insert converted c++ code into perlmodgen.cpp from files in input_perlmodgen_files directory.\n"
    "- `--perlmodgen` option : location of perlmodgen.cpp. python codes in inputdir will be updated.\n"
    "- generated code will be inserted between `// perlmodPython PerlModGenerator [key] <start>` and `// perlmodPython PerlModGenerator [key] <end>`\n"
    "- output    ( `--perlmodgen=../../../../src/perlmodgen.cpp` )\n"
    "	- ../../../../src/perlmodgen.cpp.mod : you can replace perlmodgen.cpp with it.\n";

  return true;
}

bool PerlModGenerator::generatePythonSample_py()
{
  std::ofstream pythonStream;
  if (!createOutputFile(pythonStream, pathPythonSample_py))
    return false;

  pythonStream <<
    "import DoxyDocs\n"
    "import sys\n"
    "import argparse\n"
    "\n"
    "class DoxyDocsClass :\n"
    "    \'\'\'\n"
    "    analyze classes as example. it will print table for class with markdown format\n"
    "    \'\'\'\n"
    "    def __init__(self,doxydocs={},outfile=\'\',debug=False):\n"
    "        self.plantumlCnt = 0\n"
    "        self.ret = \'<br>\'   # or  \'\\\\n\'\n"
    "\n"
    "        self.D = doxydocs\n"
    "        self.outfile = outfile\n"
    "        self.debug = debug\n"
    "        self.of = None\n"
    "        if self.outfile.strip():\n"
    "            print(\'write:\',self.outfile)\n"
    "            self.of = open(self.outfile.strip(),\'w\')\n"
    "    def __fini__(self):\n"
    "        if self.outfile.strip():\n"
    "            close(self.of)\n"
    "\n"
    "    def run(self):\n"
    "        self.print(0,\'# classes\')\n"
    "        self.print(0,\'| Class | Derived | Includes | Brief | Detailed Description|\')\n"
    "        self.print(0,\'|-------|-------------|---------|---------|----|\')\n"
    "\n"
    "        self.cls = {}\n"
    "        for k,v in self.D[\'classes\'].items():\n"
    "            self.cls[k] = {}\n"
    "            name = v[\'name\']\n"
    "            includes = v.get(\'includes\',{}).get(\'name\',\'\')\n"
    "            self.cls[k][\'name\'] = name\n"
    "            self.cls[k][\'brief\'] = self.getDescription(doc=v.get(\'brief\',{}).get(\'doc\',{}))\n"
    "            self.cls[k][\'detailed\'] = self.getDescription(doc=v.get(\'detailed\',{}).get(\'doc\',{}))\n"
    "            self.cls[k][\'derived\'] = []\n"
    "            for k1,v2 in v.get(\'derived\',{}).items():\n"
    "                self.cls[k][\'derived\'].append(v2.get(\'name\',\'\'))\n"
    "            self.cls[k][\'includes\'] = includes\n"
    "            t = self.cls[k]\n"
    "            self.print(0,\'\'\'|{name}|{drived}|{includes}|{brief}|{detail}|\'\'\'.format(name=t[\'name\'],brief=t[\'brief\'],drived=\' \'.join(t[\'derived\']),includes=t[\'includes\'],detail=t[\'detailed\']))\n"
    "\n"
    "        for k,v in self.D[\'classes\'].items():\n"
    "            self.print(0,\'## Function Lists of {c} class\'.format(c=v[\'name\']))\n"
    "            # plantuml\n"
    "            plantuml = self.getPlantuml(doc=v.get(\'detailed\',{}).get(\'doc\',{}))\n"
    "            for p in plantuml:\n"
    "                self.plantumlCnt += 1\n"
    "                self.print(0,\"\"\"- ```puml plantmul-{c}.png\"\"\".format(c=self.plantumlCnt))\n"
    "                self.print(2,\'@startuml\')\n"
    "                self.print(2,p)\n"
    "                self.print(2,\'@enduml\')\n"
    "                self.print(2,\'```\')\n"
    "                self.print(0,\'\')\n"
    "            # Table\n"
    "            self.print(2,\'| Accessibility | Function | Description | Parameters | param input | param output | Returns | return Description |\')\n"
    "            self.print(2,\'|-------|-------|----------|-------------|-------|-----|----|-------|\')\n"
    "            for accessibility,v2 in v.items():\n"
    "                if accessibility == \'public_methods\':\n"
    "                    for k3,v3 in v2.get(\'members\',{}).items():\n"
    "                        t = v2[\'members\'][k3]\n"
    "                        func = t[\'name\']\n"
    "                        brief = self.getDescription(doc=t.get(\'brief\',{}).get(\'doc\',{}))\n"
    "                        detailed = self.getDescription(doc=t.get(\'detailed\',{}).get(\'doc\',{}))\n"
    "                        returnType = t.get(\'type\',\'\')\n"
    "                        # returnDoc = []\n"
    "                        returnDoc = self.ret.join(self.getReturnDescription(doc=t.get(\'detailed\',{}).get(\'doc\',{})))\n"
    "                        # params = { \'parameters\': [] , \'in\':[] , \'out\':[] }\n"
    "                        params = self.getParameters(parameters=t.get(\'parameters\',{}),detailed_doc=t.get(\'detailed\',{}).get(\'doc\',{}))\n"
    "                         #print(\'a\',accessibility,func,detailed)\n"
    "                         #print(\'r\',returnType,returnDoc)\n"
    "                         #print((self.ret).join(params[\'parameters\']))\n"
    "                         #print((self.ret).join([x for x,y in params[\'in\']]))\n"
    "                        self.print(2,\'\'\'|{acc}|{func}|{desc}|{param}|{i}|{o}|{r}|{rdesc}|\'\'\'.format(acc=accessibility,func=func,desc=detailed,param=self.ret.join(params[\'parameters\']),i=self.ret.join([ x for x,y in params[\'in\']]),o=self.ret.join([x for x,y in params[\'out\']]),r=returnType,rdesc=returnDoc))\n"
    "            self.print(0,\'\')\n"
    "\n"
    "\n"
    "    def print(self,spaceCnt=0,text=\'\'):\n"
    "        spaces = \' \'* (spaceCnt)\n"
    "        for txt in text.split(\'\\n\'):\n"
    "            if self.of:\n"
    "                print(spaces,txt,sep=\'\',file=self.of)\n"
    "            else:\n"
    "                print(spaces,txt,sep=\'\')\n"
    "\n"
    "    def getParameters(self,parameters,detailed_doc):\n"
    "        \"\"\" parameters\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'1\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'1\'] [\'declaration_name\'] =  \'\'\'a\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'1\'] [\'type\'] =  \'\'\'int &\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'2\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'2\'] [\'type\'] =  \'\'\'int &\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'parameters\'] [\'2\'] [\'declaration_name\'] =  \'\'\'b\'\'\'\n"
    "        # when it declare without name (for example, (const QCString &) ) , it does not have \'declaration_name\' value\n"
    "        D [\'classes\'] [\'4\'] [\'private_methods\'] [\'members\'] [\'5\'] [\'parameters\'] [\'1\'] [\'type\'] =  \'\'\'const QCString &\'\'\'\n"
    "        D [\'classes\'] [\'4\'] [\'private_methods\'] [\'members\'] [\'5\'] [\'parameters\'] [\'1\'] [\'definition_name\'] =  \'\'\'name\'\'\'\n"
    "        \"\"\"\n"
    "        retParameters = []\n"
    "        for k,v in parameters.items():\n"
    "            if v.get(\'declaration_name\',\'\'):\n"
    "                retParameters.append(\'{t} {d}\'.format(t=v.get(\'type\',\'\'),d=v.get(\'declaration_name\',\'\')))\n"
    "            else:\n"
    "                retParameters.append(\'{t} {d}\'.format(t=v.get(\'type\',\'\'),d=v.get(\'definition_name\',\'\')))\n"
    "\n"
    "        \"\"\" detailed_doc\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'doc\'] [\'1\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'doc\'] [\'1\'] [\'type\'] =  \'\'\'text\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'doc\'] [\'1\'] [\'content\'] =  \'\'\'example of out \'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'parameters\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'parameters\'] [\'1\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'parameters\'] [\'1\'] [\'name\'] =  \'\'\'b\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'4\'] [\'params\'] [\'1\'] [\'parameters\'] [\'1\'] [\'dir\'] =  \'\'\'out\'\'\'\n"
    "        \"\"\"\n"
    "        retIn = []\n"
    "        retOut = []\n"
    "        for k,v in detailed_doc.items():        # k = \'4\'\n"
    "            for k2,v2 in v.items():             # k2 = \'params\'\n"
    "                if k2 == \'params\':\n"
    "                    for k3,v3 in v2.items():    # k3 = \'1\'\n"
    "                        ldesc = self.getDescription(doc=v3.get(\'doc\'))\n"
    "                        ldir = v3.get(\'parameters\',{}).get(\'1\',{}).get(\'dir\',\'\')\n"
    "                        lname = v3.get(\'parameters\',{}).get(\'1\',{}).get(\'name\',\'\')\n"
    "                        # lname is connected to \'declaration_name\' of \'parameters\' when it has no warning.\n"
    "                        if ldir.find(\'in\') >= 0:\n"
    "                            retIn.append([lname,ldesc])\n"
    "                        if ldir.find(\'out\') >= 0:\n"
    "                            retOut.append([lname,ldesc])\n"
    "\n"
    "\n"
    "\n"
    "        # params = { \'parameters\': [] , \'in\':[] , \'out\':[] }\n"
    "        return { \'parameters\': retParameters , \'in\':retIn , \'out\':retOut }\n"
    "\n"
    "    def getReturnDescription(self,doc):\n"
    "        \"\"\"\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'1\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'1\'] [\'type\'] =  \'\'\'text\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'1\'] [\'content\'] =  \'\'\'The area of the \'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'2\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'2\'] [\'content\'] =  \'\'\'Triangle\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'2\'] [\'link\'] =  \'\'\'classTriangle\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'2\'] [\'type\'] =  \'\'\'url\'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'3\']={}\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'3\'] [\'content\'] =  \'\'\' object. \'\'\'\n"
    "        D [\'classes\'] [\'9\'] [\'public_methods\'] [\'members\'] [\'16\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'return\'] [\'3\'] [\'type\'] =  \'\'\'text\'\'\'\n"
    "        \"\"\"\n"
    "        ret = []\n"
    "        for k,v in doc.items():  # k : \'3\' , v [return]...\n"
    "            s = \'\'\n"
    "            for k2,v2 in v.items():\n"
    "                if k2 == \'return\':\n"
    "                    ret.append(self.getDescription(doc=doc[k][k2]))\n"
    "        return ret\n"
    "\n"
    "    def getPlantuml(self,doc):\n"
    "        \"\"\"\n"
    "        D [\'classes\'] [\'2\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'type\'] =  \'\'\'plantuml\'\'\'\n"
    "        D [\'classes\'] [\'2\'] [\'detailed\'] [\'doc\'] [\'3\'] [\'content\'] =  \'\'\'package \"customer domain\" #DDDDDD { ... \'\'\'\n"
    "        \"\"\"\n"
    "        l = []\n"
    "        for k,v in doc.items():     # k = \'3\'\n"
    "            type = v.get(\'type\',\'\')\n"
    "            if type == \'plantuml\':\n"
    "                l.append(v[\'content\'])\n"
    "        return l\n"
    "\n"
    "    def getDescription(self,doc):\n"
    "        \'\'\'\n"
    "        link =  D [\'classes\'] [\'2\'] [\'public_methods\'] [\'members\'] [\'1\'] [\'brief\'] [\'doc\']\n"
    "        link =  D [\'classes\'] [\'2\'] [\'public_methods\'] [\'members\'] [\'2\'] [\'detailed\'] [\'doc\']\n"
    "        \'\'\'\n"
    "        s = \'\'\n"
    "        for k,v in doc.items():\n"
    "            type = v.get(\'type\',\'\')\n"
    "            if type in [\'url\',\'text\']:\n"
    "                s += v.get(\'content\',\'\')\n"
    "            elif type in [\'linebreak\']:  # \'parbreak\'\n"
    "                s += self.ret\n"
    "        return s\n"
    "\n"
    "if (__name__ == \"__main__\"):\n"
    "    parser = argparse.ArgumentParser(\n"
    "        prog=sys.argv[0],\n"
    "        description=\n"
    "        \'python sample for analyzing class and their member functions\'\n"
    "    )\n"
    "    parser.add_argument( \'--debug\', default=False , action=\"store_true\" , help=\"debug mode on  default : false\")\n"
    "    parser.add_argument( \'--outfile\',\n"
    "        metavar=\"<str>\",\n"
    "        type=str,\n"
    "        default=\'\',\n"
    "        help=\'if not exist , it prints stdout\')\n"
    "    args = parser.parse_args()\n"
    "\n"
    "    ddc = DoxyDocsClass(doxydocs=DoxyDocs.D,outfile=args.outfile,debug=args.debug)\n"
    "    ddc.run()\n";

  return true;
}

bool PerlModGenerator::generatePythonDoxy2py_pl()
{
  std::ofstream pythonStream;
  if (!createOutputFile(pythonStream, pathPythonDoxy2py_pl))
    return false;

  pythonStream <<
    "# BEGIN {push @INC, \'..\'}\n"
    "use lib \'.\';\n"
    "use DoxyDocs;\n"
    "\n"
    "our %init4py;\n"
    "\n"
    "sub change_special_code {\n"
    "	my ($s) = @_;\n"
    "	$s =~ s/\\\'/\\\\\\\'/g;\n"
    "	return $s;\n"
    "}\n"
    "\n"
    "sub generate_cc($$) {\n"
    "	my $str = $_[0];\n"
    "	my $doc = $_[1];\n"
    "	my $cnt = 1;\n"
    "\n"
    "	my $ps = $str;\n"
    "	$ps =~ s/\\{\\s*/\\[\\\'/g;\n"
    "	$ps =~ s/\\s*\\}/\\\'\\]/g;\n"
    "	$ps =~ s/^\\$//;\n"
    "\n"
    "	#print \"generate_cc $doc\\n\";\n"
    "	if($doc =~ /^HASH\\(/){\n"
    "		#print PH \"=== $ps\\n\";\n"
    "		if($init4py{$ps} eq \"\"){\n"
    "			print PH \"$ps={}\\n\";\n"
    "			$init4py{$ps} = \"init\";\n"
    "		}\n"
    "		#print \"HASH \" . %{$doc} . \"\\n\";\n"
    "		foreach $key (keys %{$doc}) {\n"
    "			if( not ( (${$doc}{$key} =~ /^ARRAY\\(/) || (${$doc}{$key} =~ /^HASH\\(/) ) ){\n"
    "				#print \"hash $str { $key } = value( ${$doc}{$key} )\\n\";\n"
    "				print PH \"$ps [\'$key\'] =  \\\'\\\'\\\'\" . change_special_code(${$doc}{$key}) . \"\\\'\\\'\\\'\\n\";\n"
    "				 #print PH \"$ps [\'$key\'] =  \\\"\" . change_special_code(${$doc}{$key}) . \"\\\"\\n\";\n"
    "			} else {\n"
    "				generate_cc(\"$str { $key }\",  ${$doc}{$key});\n"
    "			}\n"
    "		}\n"
    "	} elsif($doc =~ /^ARRAY\\(/){\n"
    "		#print \"ARRAY @$doc\\n\";\n"
    "		#print PH \"=== $ps\\n\";\n"
    "		if($init4py{$ps} eq \"\"){\n"
    "			print PH \"$ps={}\\n\";\n"
    "			$init4py{$ps} = \"init\";\n"
    "		}\n"
    "		foreach $key (@{$doc}) {\n"
    "			#print \"array $cnt key( $key )\\n\";\n"
    "			#print \"array $doc->{$key}\\n\";\n"
    "			if( not ( ($key =~ /^ARRAY\\(/) || ($key =~ /^HASH\\(/) ) ){\n"
    "				#print \"hash $str { $key } = value( ${$doc}{$key} )\\n\";\n"
    "				print  \"ARRAY $str [ $key ] =  \\\'\\\'\\\'\" . change_special_code(${$doc}[$key]) . \"\\\'\\\'\\\'\\n\";\n"
    "			} else {\n"
    "				generate_cc(\"$str { $cnt }\",$key);\n"
    "				$cnt ++;\n"
    "			}\n"
    "		}\n"
    "	} else {\n"
    "		print PH \">>> $ps\\n\";\n"
    "		#print \"DOC $str $doc\\n\";\n"
    "		#print \"DOC $str @{$doc}\\n\";\n"
    "		#print \"DOC $str %{$doc}\\n\";\n"
    "	}\n"
    "}\n"
    "\n"
    "\n"
    "print STDERR \"input DoxyDocs.pm  output filename = DoxyDocs.py\\n\";\n"
    "open(PH, \">\",\"DoxyDocs.py\") or die \"Can\'t open < DoxyDocs.py : $!\";\n"
    "#generate($doxydocs, $doxystructure);\n"
    "generate_cc(\"\\$D\",$doxydocs);\n"
    "close PH;\n"
    "print STDERR \"DoxyDocs.py has variable of D dictionary including all DoxyDocs.pm\\n\";\n"
    "\n";

  return true;
}

// perlmodPython PerlModGenerator file creation code <end>

bool PerlModGenerator::generateMakefile()
{
  std::ofstream makefileStream;
  if (!createOutputFile(makefileStream, pathMakefile))
    return false;

  bool perlmodLatex = Config_getBool(PERLMOD_LATEX);
  QCString prefix = Config_getString(PERLMOD_MAKEVAR_PREFIX);

  makefileStream <<
    ".PHONY: default clean" << (perlmodLatex ? " pdf" : "") << "\n"
    "default: " << (perlmodLatex ? "pdf" : "clean") << "\n"
    "\n"
    "include " << pathDoxyRules << "\n"
    "\n"
    "clean: clean-perlmod\n";

  if (perlmodLatex) {
    makefileStream <<
      "pdf: $(" << prefix << "DOXYLATEX_PDF)\n"
      "dvi: $(" << prefix << "DOXYLATEX_DVI)\n";
  }

  bool perlmodPython = Config_getBool(PERLMOD_PYTHON);
  if (perlmodPython) {
    // perlmodPython PerlModGenerator Makefile <start>
    makefileStream <<
      "\n"
      "# code2cpp\n"
      "code2cpp:\n"
      "	rm -rf input_perlmodgen_files\n"
      "	mkdir -p input_perlmodgen_files\n"
      "	cd input_perlmodgen_files; ln -s ../put2cpp.py put2cpp.py\n"
      "	cd input_perlmodgen_files; ln -s ../README.md README.md\n"
      "	cd input_perlmodgen_files; ln -s ../sample.py sample.py\n"
      "	cd input_perlmodgen_files; ln -s ../doxy2py.pl doxy2py.pl\n"
      "	# if you want to add python file into perlmodgen.cpp , you can use it.\n"
      "	# first of all, you add your py in input_perlmodgen_files directory\n"
      "	# please change ../../../../src/perlmodgen.cpp to location of src/perlmodgen.cpp\n"
      "	# output filename : perlmodgen.cpp.mod\n"
      "	python3 put2cpp.py --inputdir=input_perlmodgen_files --perlmodgen=../../../../src/perlmodgen.cpp --outlog=out-perlmodgen.log\n"
      "\n"
      "# run python code with DoxyDocs\n"
      "python_doc:\n"
      "	@echo \"if you want to get plantuml without running java ,\"\n"
      "	@echo \" 1. set PLANTUML_JAR_PATH = ./plantuml.jar in Doxygile\"\n"
      "	@echo \" 2. do \'touch plantuml.jar\'\"\n"
      "	@echo \" 3. run \'doxygen\' then your DoxyDocs.pm will include plantuml.\"\n"
      "	@echo \"\"\n"
      "	perl doxy2py.pl\n"
      "	python3 sample.py --outfile=output.md\n"
      "	@echo \"\"\n"
      "	@echo \"refer to https://github.com/cheoljoo/doxygen_perlmod_python for getting more example and usage\"\n"
      "\n";
    // perlmodPython PerlModGenerator Makefile <end>
  }

  return true;
}

bool PerlModGenerator::generateDoxyLatexStructurePL()
{
  std::ofstream doxyLatexStructurePLStream;
  if (!createOutputFile(doxyLatexStructurePLStream, pathDoxyLatexStructurePL))
    return false;

  doxyLatexStructurePLStream <<
    "use DoxyStructure;\n"
    "\n"
    "sub process($) {\n"
    "\tmy $node = $_[0];\n"
    "\tmy ($type, $name) = @$node[0, 1];\n"
    "\tmy $command;\n"
    "\tif ($type eq \"string\") { $command = \"String\" }\n"
    "\telsif ($type eq \"doc\") { $command = \"Doc\" }\n"
    "\telsif ($type eq \"hash\") {\n"
    "\t\t$command = \"Hash\";\n"
    "\t\tfor my $subnode (values %{$$node[2]}) {\n"
    "\t\t\tprocess($subnode);\n"
    "\t\t}\n"
    "\t}\n"
    "\telsif ($type eq \"list\") {\n"
    "\t\t$command = \"List\";\n"
    "\t\tprocess($$node[2]);\n"
    "\t}\n"
    "\tprint \"\\\\\" . $command . \"Node{\" . $name . \"}%\\n\";\n"
    "}\n"
    "\n"
    "process($doxystructure);\n";

  return true;
}

bool PerlModGenerator::generateDoxyLatexPL()
{
  std::ofstream doxyLatexPLStream;
  if (!createOutputFile(doxyLatexPLStream, pathDoxyLatexPL))
    return false;

  doxyLatexPLStream <<
    "use DoxyStructure;\n"
    "use DoxyDocs;\n"
    "\n"
    "sub latex_quote($) {\n"
    "\tmy $text = $_[0];\n"
    "\t$text =~ s/\\\\/\\\\textbackslash /g;\n"
    "\t$text =~ s/\\|/\\\\textbar /g;\n"
    "\t$text =~ s/</\\\\textless /g;\n"
    "\t$text =~ s/>/\\\\textgreater /g;\n"
    "\t$text =~ s/~/\\\\textasciitilde /g;\n"
    "\t$text =~ s/\\^/\\\\textasciicircum /g;\n"
    "\t$text =~ s/[\\$&%#_{}]/\\\\$&/g;\n"
    "\tprint $text;\n"
    "}\n"
    "\n"
    "sub generate_doc($) {\n"
    "\tmy $doc = $_[0];\n"
    "\tfor my $item (@$doc) {\n"
    "\t\tmy $type = $$item{type};\n"
    "\t\tif ($type eq \"text\") {\n"
    "\t\t\tlatex_quote($$item{content});\n"
    "\t\t} elsif ($type eq \"parbreak\") {\n"
    "\t\t\tprint \"\\n\\n\";\n"
    "\t\t} elsif ($type eq \"style\") {\n"
    "\t\t\tmy $style = $$item{style};\n"
    "\t\t\tif ($$item{enable} eq \"yes\") {\n"
    "\t\t\t\tif ($style eq \"bold\") { print '\\bfseries'; }\n"
    "\t\t\t\tif ($style eq \"italic\") { print '\\itshape'; }\n"
    "\t\t\t\tif ($style eq \"code\") { print '\\ttfamily'; }\n"
    "\t\t\t} else {\n"
    "\t\t\t\tif ($style eq \"bold\") { print '\\mdseries'; }\n"
    "\t\t\t\tif ($style eq \"italic\") { print '\\upshape'; }\n"
    "\t\t\t\tif ($style eq \"code\") { print '\\rmfamily'; }\n"
    "\t\t\t}\n"
    "\t\t\tprint '{}';\n"
    "\t\t} elsif ($type eq \"symbol\") {\n"
    "\t\t\tmy $symbol = $$item{symbol};\n"
    "\t\t\tif ($symbol eq \"copyright\") { print '\\copyright'; }\n"
    "\t\t\telsif ($symbol eq \"szlig\") { print '\\ss'; }\n"
    "\t\t\tprint '{}';\n"
    "\t\t} elsif ($type eq \"accent\") {\n"
    "\t\t\tmy ($accent) = $$item{accent};\n"
    "\t\t\tif ($accent eq \"umlaut\") { print '\\\"'; }\n"
    "\t\t\telsif ($accent eq \"acute\") { print '\\\\\\''; }\n"
    "\t\t\telsif ($accent eq \"grave\") { print '\\`'; }\n"
    "\t\t\telsif ($accent eq \"circ\") { print '\\^'; }\n"
    "\t\t\telsif ($accent eq \"tilde\") { print '\\~'; }\n"
    "\t\t\telsif ($accent eq \"cedilla\") { print '\\c'; }\n"
    "\t\t\telsif ($accent eq \"ring\") { print '\\r'; }\n"
    "\t\t\tprint \"{\" . $$item{letter} . \"}\"; \n"
    "\t\t} elsif ($type eq \"list\") {\n"
    "\t\t\tmy $env = ($$item{style} eq \"ordered\") ? \"enumerate\" : \"itemize\";\n"
    "\t\t\tprint \"\\n\\\\begin{\" . $env .\"}\";\n"
    "\t\t  \tfor my $subitem (@{$$item{content}}) {\n"
    "\t\t\t\tprint \"\\n\\\\item \";\n"
    "\t\t\t\tgenerate_doc($subitem);\n"
    "\t\t  \t}\n"
    "\t\t\tprint \"\\n\\\\end{\" . $env .\"}\";\n"
    "\t\t} elsif ($type eq \"url\") {\n"
    "\t\t\tlatex_quote($$item{content});\n"
    "\t\t}\n"
    "\t}\n"
    "}\n"
    "\n"
    "sub generate($$) {\n"
    "\tmy ($item, $node) = @_;\n"
    "\tmy ($type, $name) = @$node[0, 1];\n"
    "\tif ($type eq \"string\") {\n"
    "\t\tprint \"\\\\\" . $name . \"{\";\n"
    "\t\tlatex_quote($item);\n"
    "\t\tprint \"}\";\n"
    "\t} elsif ($type eq \"doc\") {\n"
    "\t\tif (@$item) {\n"
    "\t\t\tprint \"\\\\\" . $name . \"{\";\n"
    "\t\t\tgenerate_doc($item);\n"
    "\t\t\tprint \"}%\\n\";\n"
    "\t\t} else {\n"
    "#\t\t\tprint \"\\\\\" . $name . \"Empty%\\n\";\n"
    "\t\t}\n"
    "\t} elsif ($type eq \"hash\") {\n"
    "\t\tmy ($key, $value);\n"
    "\t\twhile (($key, $subnode) = each %{$$node[2]}) {\n"
    "\t\t\tmy $subname = $$subnode[1];\n"
    "\t\t\tprint \"\\\\Defcs{field\" . $subname . \"}{\";\n"
    "\t\t\tif ($$item{$key}) {\n"
    "\t\t\t\tgenerate($$item{$key}, $subnode);\n"
    "\t\t\t} else {\n"
    "#\t\t\t\t\tprint \"\\\\\" . $subname . \"Empty%\\n\";\n"
    "\t\t\t}\n"
    "\t\t\tprint \"}%\\n\";\n"
    "\t\t}\n"
    "\t\tprint \"\\\\\" . $name . \"%\\n\";\n"
    "\t} elsif ($type eq \"list\") {\n"
    "\t\tmy $index = 0;\n"
    "\t\tif (@$item) {\n"
    "\t\t\tprint \"\\\\\" . $name . \"{%\\n\";\n"
    "\t\t\tfor my $subitem (@$item) {\n"
    "\t\t\t\tif ($index) {\n"
    "\t\t\t\t\tprint \"\\\\\" . $name . \"Sep%\\n\";\n"
    "\t\t\t\t}\n"
    "\t\t\t\tgenerate($subitem, $$node[2]);\n"
    "\t\t\t\t$index++;\n"
    "\t\t\t}\n"
    "\t\t\tprint \"}%\\n\";\n"
    "\t\t} else {\n"
    "#\t\t\tprint \"\\\\\" . $name . \"Empty%\\n\";\n"
    "\t\t}\n"
    "\t}\n"
    "}\n"
    "\n"
    "generate($doxydocs, $doxystructure);\n";

  return true;
}

bool PerlModGenerator::generateDoxyFormatTex()
{
  std::ofstream doxyFormatTexStream;
  if (!createOutputFile(doxyFormatTexStream, pathDoxyFormatTex))
    return false;

  doxyFormatTexStream <<
    "\\def\\Defcs#1{\\long\\expandafter\\def\\csname#1\\endcsname}\n"
    "\\Defcs{Empty}{}\n"
    "\\def\\IfEmpty#1{\\expandafter\\ifx\\csname#1\\endcsname\\Empty}\n"
    "\n"
    "\\def\\StringNode#1{\\Defcs{#1}##1{##1}}\n"
    "\\def\\DocNode#1{\\Defcs{#1}##1{##1}}\n"
    "\\def\\ListNode#1{\\Defcs{#1}##1{##1}\\Defcs{#1Sep}{}}\n"
    "\\def\\HashNode#1{\\Defcs{#1}{}}\n"
    "\n"
    "\\input{" << pathDoxyStructureTex << "}\n"
    "\n"
    "\\newbox\\BoxA\n"
    "\\dimendef\\DimenA=151\\relax\n"
    "\\dimendef\\DimenB=152\\relax\n"
    "\\countdef\\ZoneDepth=151\\relax\n"
    "\n"
    "\\def\\Cs#1{\\csname#1\\endcsname}\n"
    "\\def\\Letcs#1{\\expandafter\\let\\csname#1\\endcsname}\n"
    "\\def\\Heading#1{\\vskip 4mm\\relax\\textbf{#1}}\n"
    "\\def\\See#1{\\begin{flushleft}\\Heading{See also: }#1\\end{flushleft}}\n"
    "\n"
    "\\def\\Frame#1{\\vskip 3mm\\relax\\fbox{ \\vbox{\\hsize0.95\\hsize\\vskip 1mm\\relax\n"
    "\\raggedright#1\\vskip 0.5mm\\relax} }}\n"
    "\n"
    "\\def\\Zone#1#2#3{%\n"
    "\\Defcs{Test#1}{#2}%\n"
    "\\Defcs{Emit#1}{#3}%\n"
    "\\Defcs{#1}{%\n"
    "\\advance\\ZoneDepth1\\relax\n"
    "\\Letcs{Mode\\number\\ZoneDepth}0\\relax\n"
    "\\Letcs{Present\\number\\ZoneDepth}0\\relax\n"
    "\\Cs{Test#1}\n"
    "\\expandafter\\if\\Cs{Present\\number\\ZoneDepth}1%\n"
    "\\advance\\ZoneDepth-1\\relax\n"
    "\\Letcs{Present\\number\\ZoneDepth}1\\relax\n"
    "\\expandafter\\if\\Cs{Mode\\number\\ZoneDepth}1%\n"
    "\\advance\\ZoneDepth1\\relax\n"
    "\\Letcs{Mode\\number\\ZoneDepth}1\\relax\n"
    "\\Cs{Emit#1}\n"
    "\\advance\\ZoneDepth-1\\relax\\fi\n"
    "\\advance\\ZoneDepth1\\relax\\fi\n"
    "\\advance\\ZoneDepth-1\\relax}}\n"
    "\n"
    "\\def\\Member#1#2{%\n"
    "\\Defcs{Test#1}{\\Cs{field#1Detailed}\n"
    "\\IfEmpty{field#1DetailedDoc}\\else\\Letcs{Present#1}1\\fi}\n"
    "\\Defcs{#1}{\\Letcs{Present#1}0\\relax\n"
    "\\Cs{Test#1}\\if1\\Cs{Present#1}\\Letcs{Present\\number\\ZoneDepth}1\\relax\n"
    "\\if1\\Cs{Mode\\number\\ZoneDepth}#2\\fi\\fi}}\n"
    "\n"
    "\\def\\TypedefMemberList#1#2{%\n"
    "\\Defcs{#1DetailedDoc}##1{\\vskip 5.5mm\\relax##1}%\n"
    "\\Defcs{#1Name}##1{\\textbf{##1}}%\n"
    "\\Defcs{#1See}##1{\\See{##1}}%\n"
    "%\n"
    "\\Zone{#1s}{\\Cs{field#1List}}{\\subsubsection{#2}\\Cs{field#1List}}%\n"
    "\\Member{#1}{\\Frame{typedef \\Cs{field#1Type} \\Cs{field#1Name}}%\n"
    "\\Cs{field#1DetailedDoc}\\Cs{field#1See}\\vskip 5mm\\relax}}%\n"
    "\n"
    "\\def\\VariableMemberList#1#2{%\n"
    "\\Defcs{#1DetailedDoc}##1{\\vskip 5.5mm\\relax##1}%\n"
    "\\Defcs{#1Name}##1{\\textbf{##1}}%\n"
    "\\Defcs{#1See}##1{\\See{##1}}%\n"
    "%\n"
    "\\Zone{#1s}{\\Cs{field#1List}}{\\subsubsection{#2}\\Cs{field#1List}}%\n"
    "\\Member{#1}{\\Frame{\\Cs{field#1Type}{} \\Cs{field#1Name}}%\n"
    "\\Cs{field#1DetailedDoc}\\Cs{field#1See}\\vskip 5mm\\relax}}%\n"
    "\n"
    "\\def\\FunctionMemberList#1#2{%\n"
    "\\Defcs{#1PDParamName}##1{\\textit{##1}}%\n"
    "\\Defcs{#1PDParam}{\\Cs{field#1PDParamName}}%\n"
    "\\Defcs{#1PDParamsSep}{, }%\n"
    "\\Defcs{#1PDBlocksSep}{\\vskip 2mm\\relax}%\n"
    "%\n"
    "\\Defcs{#1PDBlocks}##1{%\n"
    "\\Heading{Parameters:}\\vskip 1.5mm\\relax\n"
    "\\DimenA0pt\\relax\n"
    "\\Defcs{#1PDBlock}{\\setbox\\BoxA\\hbox{\\Cs{field#1PDParams}}%\n"
    "\\ifdim\\DimenA<\\wd\\BoxA\\DimenA\\wd\\BoxA\\fi}%\n"
    "##1%\n"
    "\\advance\\DimenA3mm\\relax\n"
    "\\DimenB\\hsize\\advance\\DimenB-\\DimenA\\relax\n"
    "\\Defcs{#1PDBlock}{\\hbox to\\hsize{\\vtop{\\hsize\\DimenA\\relax\n"
    "\\Cs{field#1PDParams}}\\hfill\n"
    "\\vtop{\\hsize\\DimenB\\relax\\Cs{field#1PDDoc}}}}%\n"
    "##1}\n"
    "\n"
    "\\Defcs{#1ParamName}##1{\\textit{##1}}\n"
    "\\Defcs{#1Param}{\\Cs{field#1ParamType}{} \\Cs{field#1ParamName}}\n"
    "\\Defcs{#1ParamsSep}{, }\n"
    "\n"
    "\\Defcs{#1Name}##1{\\textbf{##1}}\n"
    "\\Defcs{#1See}##1{\\See{##1}}\n"
    "\\Defcs{#1Return}##1{\\Heading{Returns: }##1}\n"
    "\\Defcs{field#1Title}{\\Frame{\\Cs{field#1Type}{} \\Cs{field#1Name}(\\Cs{field#1Params})}}%\n"
    "%\n"
    "\\Zone{#1s}{\\Cs{field#1List}}{\\subsubsection{#2}\\Cs{field#1List}}%\n"
    "\\Member{#1}{%\n"
    "\\Cs{field#1Title}\\vskip 6mm\\relax\\Cs{field#1DetailedDoc}\n"
    "\\Cs{field#1Return}\\Cs{field#1PDBlocks}\\Cs{field#1See}\\vskip 5mm\\relax}}\n"
    "\n"
    "\\def\\FileDetailed{\\fieldFileDetailedDoc\\par}\n"
    "\\def\\ClassDetailed{\\fieldClassDetailedDoc\\par}\n"
    "\n"
    "\\def\\FileSubzones{\\fieldFileTypedefs\\fieldFileVariables\\fieldFileFunctions}\n"
    "\n"
    "\\def\\ClassSubzones{%\n"
    "\\fieldClassPublicTypedefs\\fieldClassPublicMembers\\fieldClassPublicMethods\n"
    "\\fieldClassProtectedTypedefs\\fieldClassProtectedMembers\\fieldClassProtectedMethods\n"
    "\\fieldClassPrivateTypedefs\\fieldClassPrivateMembers\\fieldClassPrivateMethods}\n"
    "\n"
    "\\Member{Page}{\\subsection{\\fieldPageName}\\fieldPageDetailedDoc}\n"
    "\n"
    "\\TypedefMemberList{FileTypedef}{Typedefs}\n"
    "\\VariableMemberList{FileVariable}{Variables}\n"
    "\\FunctionMemberList{FileFunction}{Functions}\n"
    "\\Zone{File}{\\FileSubzones}{\\subsection{\\fieldFileName}\\fieldFileDetailed\\FileSubzones}\n"
    "\n"
    "\\TypedefMemberList{ClassPublicTypedef}{Public Typedefs}\n"
    "\\TypedefMemberList{ClassProtectedTypedef}{Protected Typedefs}\n"
    "\\TypedefMemberList{ClassPrivateTypedef}{Private Typedefs}\n"
    "\\VariableMemberList{ClassPublicMember}{Public Members}\n"
    "\\VariableMemberList{ClassProtectedMember}{Protected Members}\n"
    "\\VariableMemberList{ClassPrivateMember}{Private Members}\n"
    "\\FunctionMemberList{ClassPublicMethod}{Public Methods}\n"
    "\\FunctionMemberList{ClassProtectedMethod}{Protected Methods}\n"
    "\\FunctionMemberList{ClassPrivateMethod}{Private Methods}\n"
    "\\Zone{Class}{\\ClassSubzones}{\\subsection{\\fieldClassName}\\fieldClassDetailed\\ClassSubzones}\n"
    "\n"
    "\\Zone{AllPages}{\\fieldPages}{\\section{Pages}\\fieldPages}\n"
    "\\Zone{AllFiles}{\\fieldFiles}{\\section{Files}\\fieldFiles}\n"
    "\\Zone{AllClasses}{\\fieldClasses}{\\section{Classes}\\fieldClasses}\n"
    "\n"
    "\\newlength{\\oldparskip}\n"
    "\\newlength{\\oldparindent}\n"
    "\\newlength{\\oldfboxrule}\n"
    "\n"
    "\\ZoneDepth0\\relax\n"
    "\\Letcs{Mode0}1\\relax\n"
    "\n"
    "\\def\\EmitDoxyDocs{%\n"
    "\\setlength{\\oldparskip}{\\parskip}\n"
    "\\setlength{\\oldparindent}{\\parindent}\n"
    "\\setlength{\\oldfboxrule}{\\fboxrule}\n"
    "\\setlength{\\parskip}{0cm}\n"
    "\\setlength{\\parindent}{0cm}\n"
    "\\setlength{\\fboxrule}{1pt}\n"
    "\\AllPages\\AllFiles\\AllClasses\n"
    "\\setlength{\\parskip}{\\oldparskip}\n"
    "\\setlength{\\parindent}{\\oldparindent}\n"
    "\\setlength{\\fboxrule}{\\oldfboxrule}}\n";

  return true;
}

bool PerlModGenerator::generateDoxyLatexTex()
{
  std::ofstream doxyLatexTexStream;
  if (!createOutputFile(doxyLatexTexStream, pathDoxyLatexTex))
    return false;

  doxyLatexTexStream <<
    "\\documentclass[a4paper,12pt]{article}\n"
    "\\usepackage[latin1]{inputenc}\n"
    "\\usepackage[none]{hyphenat}\n"
    "\\usepackage[T1]{fontenc}\n"
    "\\usepackage{hyperref}\n"
    "\\usepackage{times}\n"
    "\n"
    "\\input{doxyformat}\n"
    "\n"
    "\\begin{document}\n"
    "\\input{" << pathDoxyDocsTex << "}\n"
    "\\sloppy\n"
    "\\EmitDoxyDocs\n"
    "\\end{document}\n";

  return true;
}

void PerlModGenerator::generate()
{
  // + classes
  // + namespaces
  // + files
  // - packages
  // + groups
  // + related pages
  // - examples

  Dir perlModDir;
  if (!createOutputDir(perlModDir))
    return;

  bool perlmodLatex = Config_getBool(PERLMOD_LATEX);

  QCString perlModAbsPath = perlModDir.absPath();
  pathDoxyDocsPM = perlModAbsPath + "/DoxyDocs.pm";
  pathDoxyStructurePM = perlModAbsPath + "/DoxyStructure.pm";
  pathMakefile = perlModAbsPath + "/Makefile";
  pathDoxyRules = perlModAbsPath + "/doxyrules.make";

  if (perlmodLatex) {
    pathDoxyStructureTex = perlModAbsPath + "/doxystructure.tex";
    pathDoxyFormatTex = perlModAbsPath + "/doxyformat.tex";
    pathDoxyLatexTex = perlModAbsPath + "/doxylatex.tex";
    pathDoxyLatexDVI = perlModAbsPath + "/doxylatex.dvi";
    pathDoxyLatexPDF = perlModAbsPath + "/doxylatex.pdf";
    pathDoxyDocsTex = perlModAbsPath + "/doxydocs.tex";
    pathDoxyLatexPL = perlModAbsPath + "/doxylatex.pl";
    pathDoxyLatexStructurePL = perlModAbsPath + "/doxylatex-structure.pl";
  }

  if (!(generatePerlModOutput()
	&& generateDoxyStructurePM()
	&& generateMakefile()
	&& generateDoxyRules()))
    return;

  if (perlmodLatex) {
    if (!(generateDoxyLatexStructurePL()
	  && generateDoxyLatexPL()
	  && generateDoxyLatexTex()
	  && generateDoxyFormatTex()))
      return;
  }

  bool perlmodPython = Config_getBool(PERLMOD_PYTHON);
  if (perlmodPython) {
    // perlmodPython PerlModGenerator code in generate function <start>
    pathPythonPut2cpp_py = perlModAbsPath + "/put2cpp.py";
    pathPythonREADME_md = perlModAbsPath + "/README.md";
    pathPythonSample_py = perlModAbsPath + "/sample.py";
    pathPythonDoxy2py_pl = perlModAbsPath + "/doxy2py.pl";
    if (!(generatePythonPut2cpp_py()
      && generatePythonREADME_md()
      && generatePythonSample_py()
      && generatePythonDoxy2py_pl()))
      return;
    // perlmodPython PerlModGenerator code in generate function <end>
  }

}

void generatePerlMod()
{
  PerlModGenerator pmg(Config_getBool(PERLMOD_PRETTY));
  pmg.generate();
}

// Local Variables:
// c-basic-offset: 2
// End:

/* This elisp function for XEmacs makes Control-Z transform
   the text in the region into a valid C string.

  (global-set-key '(control z) (lambda () (interactive)
   (save-excursion
    (if (< (mark) (point)) (exchange-point-and-mark))
    (let ((start (point)) (replacers
     '(("\\\\" "\\\\\\\\")
       ("\"" "\\\\\"")
       ("\t" "\\\\t")
       ("^.*$" "\"\\&\\\\n\""))))
     (while replacers
      (while (re-search-forward (caar replacers) (mark) t)
       (replace-match (cadar replacers) t))
      (goto-char start)
      (setq replacers (cdr replacers)))))))
*/