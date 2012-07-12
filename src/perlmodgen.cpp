/******************************************************************************
 *
 *
 *
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include "perlmodgen.h"
#include "docparser.h"
#include "message.h"
#include "doxygen.h"
#include "pagedef.h"

#include <qdir.h>
#include <qstack.h>
#include <qdict.h>
#include <qfile.h>
#include "ftextstream.h"
#include "arguments.h"

#define PERLOUTPUT_MAX_INDENTATION 40

class PerlModOutputStream
{
public:

  QCString m_s;
  FTextStream *m_t;

  PerlModOutputStream(FTextStream *t = 0) : m_t(t) { }

  void add(char c);
  void add(const char *s);
  void add(QCString &s);
  void add(int n);
  void add(unsigned int n);
};

void PerlModOutputStream::add(char c)
{
  if (m_t != 0)
    (*m_t) << c;
  else
    m_s += c;
}

void PerlModOutputStream::add(const char *s)
{
  if (m_t != 0)
    (*m_t) << s;
  else
    m_s += s;
}

void PerlModOutputStream::add(QCString &s)
{
  if (m_t != 0)
    (*m_t) << s;
  else
    m_s += s;
}

void PerlModOutputStream::add(int n)
{
  if (m_t != 0)
    (*m_t) << n;
  else
    m_s += n;
}

void PerlModOutputStream::add(unsigned int n)
{
  if (m_t != 0)
    (*m_t) << n;
  else
    m_s += n;
}

class PerlModOutput
{
public:

  bool m_pretty;

  inline PerlModOutput(bool pretty)
    : m_pretty(pretty), m_stream(0), m_indentation(false), m_blockstart(true)
  {
    m_spaces[0] = 0;
  }

  virtual ~PerlModOutput() { }

  inline void setPerlModOutputStream(PerlModOutputStream *os) { m_stream = os; }

  inline PerlModOutput &openSave() { iopenSave(); return *this; }
  inline PerlModOutput &closeSave(QCString &s) { icloseSave(s); return *this; }

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
  inline PerlModOutput &add(const char *s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(QCString &s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(int n) { m_stream->add(n); return *this; }
  inline PerlModOutput &add(unsigned int n) { m_stream->add(n); return *this; }

  PerlModOutput &addQuoted(const char *s) { iaddQuoted(s); return *this; }

  inline PerlModOutput &indent()
  {
    if (m_pretty) {
      m_stream->add('\n');
      m_stream->add(m_spaces);
    }
    return *this;
  }

  inline PerlModOutput &open(char c, const char *s = 0) { iopen(c, s); return *this; }
  inline PerlModOutput &close(char c = 0) { iclose(c); return *this; }

  inline PerlModOutput &addField(const char *s) { iaddField(s); return *this; }
  inline PerlModOutput &addFieldQuotedChar(const char *field, char content)
  {
    iaddFieldQuotedChar(field, content); return *this;
  }
  inline PerlModOutput &addFieldQuotedString(const char *field, const char *content)
  {
    iaddFieldQuotedString(field, content); return *this;
  }
  inline PerlModOutput &addFieldBoolean(const char *field, bool content)
  {
    return addFieldQuotedString(field, content ? "yes" : "no");
  }
  inline PerlModOutput &openList(const char *s = 0) { open('[', s); return *this; }
  inline PerlModOutput &closeList() { close(']'); return *this; }
  inline PerlModOutput &openHash(const char *s = 0 ) { open('{', s); return *this; }
  inline PerlModOutput &closeHash() { close('}'); return *this; }

protected:
  
  void iopenSave();
  void icloseSave(QCString &);
  
  void incIndent();
  void decIndent();

  void iaddQuoted(const char *);
  void iaddFieldQuotedChar(const char *, char);
  void iaddFieldQuotedString(const char *, const char *);
  void iaddField(const char *);

  void iopen(char, const char *);
  void iclose(char);

private:
  
  PerlModOutputStream *m_stream;
  int m_indentation;
  bool m_blockstart;

  QStack<PerlModOutputStream> m_saved;
  char m_spaces[PERLOUTPUT_MAX_INDENTATION * 2 + 2];
};

void PerlModOutput::iopenSave()
{
  m_saved.push(m_stream);
  m_stream = new PerlModOutputStream();
}

void PerlModOutput::icloseSave(QCString &s)
{
  s = m_stream->m_s;
  delete m_stream;
  m_stream = m_saved.pop();
}

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

void PerlModOutput::iaddQuoted(const char *s) 
{
  char c;
  while ((c = *s++) != 0) {
    if ((c == '\'') || (c == '\\'))
      m_stream->add('\\');
    m_stream->add(c);
  }
}
  
void PerlModOutput::iaddField(const char *s)
{
  continueBlock();
  m_stream->add(s);
  m_stream->add(m_pretty ? " => " : "=>");
}

void PerlModOutput::iaddFieldQuotedChar(const char *field, char content)
{
  iaddField(field);
  m_stream->add('\'');
  if ((content == '\'') || (content == '\\'))
    m_stream->add('\\');
  m_stream->add(content);
  m_stream->add('\'');
}

void PerlModOutput::iaddFieldQuotedString(const char *field, const char *content)
{
  if (content == 0)
    return;
  iaddField(field);
  m_stream->add('\'');
  iaddQuoted(content);
  m_stream->add('\'');
}

void PerlModOutput::iopen(char c, const char *s)
{
  if (s != 0)
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
   
  void visit(DocWord *);
  void visit(DocLinkedWord *);
  void visit(DocWhiteSpace *);
  void visit(DocSymbol *);
  void visit(DocURL *);
  void visit(DocLineBreak *);
  void visit(DocHorRuler *);
  void visit(DocStyleChange *);
  void visit(DocVerbatim *);
  void visit(DocAnchor *);
  void visit(DocInclude *);
  void visit(DocIncOperator *);
  void visit(DocFormula *);
  void visit(DocIndexEntry *);
  void visit(DocSimpleSectSep *);
  void visit(DocCite *);

  //--------------------------------------
  // visitor functions for compound nodes
  //--------------------------------------
   
  void visitPre(DocAutoList *);
  void visitPost(DocAutoList *);
  void visitPre(DocAutoListItem *);
  void visitPost(DocAutoListItem *);
  void visitPre(DocPara *) ;
  void visitPost(DocPara *);
  void visitPre(DocRoot *);
  void visitPost(DocRoot *);
  void visitPre(DocSimpleSect *);
  void visitPost(DocSimpleSect *);
  void visitPre(DocTitle *);
  void visitPost(DocTitle *);
  void visitPre(DocSimpleList *);
  void visitPost(DocSimpleList *);
  void visitPre(DocSimpleListItem *);
  void visitPost(DocSimpleListItem *);
  void visitPre(DocSection *);
  void visitPost(DocSection *);
  void visitPre(DocHtmlList *);
  void visitPost(DocHtmlList *) ;
  void visitPre(DocHtmlListItem *);
  void visitPost(DocHtmlListItem *);
  //void visitPre(DocHtmlPre *);
  //void visitPost(DocHtmlPre *);
  void visitPre(DocHtmlDescList *);
  void visitPost(DocHtmlDescList *);
  void visitPre(DocHtmlDescTitle *);
  void visitPost(DocHtmlDescTitle *);
  void visitPre(DocHtmlDescData *);
  void visitPost(DocHtmlDescData *);
  void visitPre(DocHtmlTable *);
  void visitPost(DocHtmlTable *);
  void visitPre(DocHtmlRow *);
  void visitPost(DocHtmlRow *) ;
  void visitPre(DocHtmlCell *);
  void visitPost(DocHtmlCell *);
  void visitPre(DocHtmlCaption *);
  void visitPost(DocHtmlCaption *);
  void visitPre(DocInternal *);
  void visitPost(DocInternal *);
  void visitPre(DocHRef *);
  void visitPost(DocHRef *);
  void visitPre(DocHtmlHeader *);
  void visitPost(DocHtmlHeader *);
  void visitPre(DocImage *);
  void visitPost(DocImage *);
  void visitPre(DocDotFile *);
  void visitPost(DocDotFile *);
  void visitPre(DocMscFile *);
  void visitPost(DocMscFile *);
  void visitPre(DocLink *);
  void visitPost(DocLink *);
  void visitPre(DocRef *);
  void visitPost(DocRef *);
  void visitPre(DocSecRefItem *);
  void visitPost(DocSecRefItem *);
  void visitPre(DocSecRefList *);
  void visitPost(DocSecRefList *);
  //void visitPre(DocLanguage *);
  //void visitPost(DocLanguage *);
  void visitPre(DocParamSect *);
  void visitPost(DocParamSect *);
  void visitPre(DocParamList *);
  void visitPost(DocParamList *);
  void visitPre(DocXRefItem *);
  void visitPost(DocXRefItem *);
  void visitPre(DocInternalRef *);
  void visitPost(DocInternalRef *);
  void visitPre(DocCopy *);
  void visitPost(DocCopy *);
  void visitPre(DocText *);
  void visitPost(DocText *);
  void visitPre(DocHtmlBlockQuote *);
  void visitPost(DocHtmlBlockQuote *);

private:

  //--------------------------------------
  // helper functions
  //--------------------------------------

  void addLink(const QCString &ref, const QCString &file,
	       const QCString &anchor);
   
  void enterText();
  void leaveText();

  void openItem(const char *);
  void closeItem();
  void singleItem(const char *);
  void openSubBlock(const char * = 0);
  void closeSubBlock();
  void openOther();
  void closeOther();

  //--------------------------------------
  // state variables
  //--------------------------------------

  PerlModOutput &m_output;
  bool m_textmode;
  bool m_textblockstart;
  QCString m_other;
};

PerlModDocVisitor::PerlModDocVisitor(PerlModOutput &output)
  : DocVisitor(DocVisitor_Other), m_output(output), m_textmode(false)
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

void PerlModDocVisitor::openItem(const char *name)
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

void PerlModDocVisitor::singleItem(const char *name)
{
  openItem(name);
  closeItem();
}

void PerlModDocVisitor::openSubBlock(const char *s)
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

void PerlModDocVisitor::openOther()
{
  // Using a secondary text stream will corrupt the perl file. Instead of
  // printing doc => [ data => [] ], it will print doc => [] data => [].
  /*
  leaveText();
  m_output.openSave();
  */
}

void PerlModDocVisitor::closeOther()
{
  // Using a secondary text stream will corrupt the perl file. Instead of
  // printing doc => [ data => [] ], it will print doc => [] data => [].
  /*
  QCString other;
  leaveText();
  m_output.closeSave(other);
  m_other += other;
  */
}

void PerlModDocVisitor::visit(DocWord *w)
{
  enterText();
  m_output.addQuoted(w->word());
}

void PerlModDocVisitor::visit(DocLinkedWord *w)
{
  openItem("url");
  addLink(w->ref(), w->file(), w->anchor());
  m_output.addFieldQuotedString("content", w->word());
  closeItem();
}

void PerlModDocVisitor::visit(DocWhiteSpace *)
{
  enterText();
  m_output.add(' ');
}

void PerlModDocVisitor::visit(DocSymbol *sy)
{
  char c = 0;
  const char *s = 0;
  const char *accent = 0;
  const char *symbol = 0;
  switch(sy->symbol())
  {
    case DocSymbol::At:      c = '@'; break;
    case DocSymbol::Less:    c = '<'; break;
    case DocSymbol::Greater: c = '>'; break;
    case DocSymbol::Amp:     c = '&'; break;
    case DocSymbol::Dollar:  c = '$'; break;
    case DocSymbol::Hash:    c = '#'; break;
    case DocSymbol::DoubleColon: s = "::"; break;
    case DocSymbol::Percent: c = '%'; break;
    case DocSymbol::Pipe:    c = '|'; break;
    case DocSymbol::Quot:    c = '"'; break;
    case DocSymbol::Lsquo:   s = "\\\'"; break;
    case DocSymbol::Rsquo:   s = "\\\'"; break;
    case DocSymbol::Ldquo:   c = '"'; break;
    case DocSymbol::Rdquo:   c = '"'; break;
    case DocSymbol::Ndash:   c = '-'; break;
    case DocSymbol::Mdash:   s = "--"; break;
    case DocSymbol::Nbsp:    c = ' '; break;
    case DocSymbol::Uml:     accent = "umlaut"; break;
    case DocSymbol::Acute:   accent = "acute"; break;
    case DocSymbol::Grave:   accent = "grave"; break;
    case DocSymbol::Circ:    accent = "circ"; break;
    case DocSymbol::Slash:   accent = "slash"; break;
    case DocSymbol::Tilde:   accent = "tilde"; break;
    case DocSymbol::Cedil:   accent = "cedilla"; break;
    case DocSymbol::Ring:    accent = "ring"; break;
    case DocSymbol::BSlash:  s = "\\\\"; break;
    case DocSymbol::Copy:    symbol = "copyright"; break;
    case DocSymbol::Tm:      symbol = "trademark"; break;
    case DocSymbol::Reg:     symbol = "registered"; break;
    case DocSymbol::Szlig:   symbol = "szlig"; break;
    case DocSymbol::Apos:    s = "\\\'"; break;
    case DocSymbol::Aelig:   symbol = "aelig"; break;
    case DocSymbol::AElig:   symbol = "AElig"; break;
    case DocSymbol::GrkGamma:      symbol = "Gamma"; break;
    case DocSymbol::GrkDelta:      symbol = "Delta"; break;
    case DocSymbol::GrkTheta:      symbol = "Theta"; break;
    case DocSymbol::GrkLambda:     symbol = "Lambda"; break;
    case DocSymbol::GrkXi:         symbol = "Xi"; break;
    case DocSymbol::GrkPi:         symbol = "Pi"; break;
    case DocSymbol::GrkSigma:      symbol = "Sigma"; break;
    case DocSymbol::GrkUpsilon:    symbol = "Upsilon"; break;
    case DocSymbol::GrkPhi:        symbol = "Phi"; break;
    case DocSymbol::GrkPsi:        symbol = "Psi"; break;
    case DocSymbol::GrkOmega:      symbol = "Omega"; break;
    case DocSymbol::Grkalpha:      symbol = "alpha"; break;
    case DocSymbol::Grkbeta:       symbol = "beta"; break;
    case DocSymbol::Grkgamma:      symbol = "gamma"; break;
    case DocSymbol::Grkdelta:      symbol = "delta"; break;
    case DocSymbol::Grkepsilon:    symbol = "epsilon"; break;
    case DocSymbol::Grkzeta:       symbol = "zeta"; break;
    case DocSymbol::Grketa:        symbol = "eta"; break;
    case DocSymbol::Grktheta:      symbol = "theta"; break;
    case DocSymbol::Grkiota:       symbol = "iota"; break;
    case DocSymbol::Grkkappa:      symbol = "kappa"; break;
    case DocSymbol::Grklambda:     symbol = "lambda"; break;
    case DocSymbol::Grkmu:         symbol = "mu"; break;
    case DocSymbol::Grknu:         symbol = "nu"; break;
    case DocSymbol::Grkxi:         symbol = "xi"; break;
    case DocSymbol::Grkpi:         symbol = "pi"; break;
    case DocSymbol::Grkrho:        symbol = "rho"; break;
    case DocSymbol::Grksigma:      symbol = "sigma"; break;
    case DocSymbol::Grktau:        symbol = "tau"; break;
    case DocSymbol::Grkupsilon:    symbol = "upsilon"; break;
    case DocSymbol::Grkphi:        symbol = "phi"; break;
    case DocSymbol::Grkchi:        symbol = "chi"; break;
    case DocSymbol::Grkpsi:        symbol = "psi"; break;
    case DocSymbol::Grkomega:      symbol = "omega"; break;
    case DocSymbol::Grkvarsigma:   symbol = "sigma"; break;
    case DocSymbol::Section:       symbol = "sect"; break;
    case DocSymbol::Degree:        symbol = "deg"; break;
    case DocSymbol::Prime:         s = "\\\'"; break;
    case DocSymbol::DoublePrime:   c = '"'; break;
    case DocSymbol::Infinity:      symbol = "inf"; break;
    case DocSymbol::EmptySet:      symbol = "empty"; break;
    case DocSymbol::PlusMinus:     s = "+/-"; break;
    case DocSymbol::Times:         c = '*'; break;
    case DocSymbol::Minus:         c = '-'; break;
    case DocSymbol::CenterDot:     c = '.'; break;
    case DocSymbol::Partial:       symbol = "partial"; break;
    case DocSymbol::Nabla:         symbol = "nabla"; break;
    case DocSymbol::SquareRoot:    symbol = "sqrt"; break;
    case DocSymbol::Perpendicular: symbol = "perp"; break;
    case DocSymbol::Sum:           symbol = "sum"; break;
    case DocSymbol::Integral:      symbol = "int"; break;
    case DocSymbol::Product:       symbol = "prod"; break;
    case DocSymbol::Similar:       c = '~'; break;
    case DocSymbol::Approx:        symbol = "approx"; break;
    case DocSymbol::NotEqual:      s = "!="; break;
    case DocSymbol::Equivalent:    symbol = "equiv"; break;
    case DocSymbol::Proportional:  symbol = "propto"; break;
    case DocSymbol::LessEqual:     s = "<="; break;
    case DocSymbol::GreaterEqual:  s = ">="; break;
    case DocSymbol::LeftArrow:     s = "<-"; break;
    case DocSymbol::RightArrow:    s = "->"; break;
    case DocSymbol::SetIn:         symbol = "in"; break;
    case DocSymbol::SetNotIn:      symbol = "notin"; break;
    case DocSymbol::LeftCeil:      symbol = "lceil"; break;
    case DocSymbol::RightCeil:     symbol = "rceil"; break;
    case DocSymbol::LeftFloor:     symbol = "lfloor"; break;
    case DocSymbol::RightFloor:    symbol = "rfloor"; break;
    case DocSymbol::Unknown: err("error: unknown symbol found\n");
                             break;
  }
  if (c != 0) 
  {
    enterText();
    m_output.add(c);
  } 
  else if (s != 0) 
  {
    enterText();
    m_output.add(s);
  } 
  else if (symbol != 0) 
  {
    leaveText();
    openItem("symbol");
    m_output.addFieldQuotedString("symbol", symbol);
    closeItem();
  } 
  else if (accent != 0) 
  {
    leaveText();
    openItem("accent");
    m_output
      .addFieldQuotedString("accent", accent)
      .addFieldQuotedChar("letter", sy->letter());
    closeItem();
  }
}

void PerlModDocVisitor::visit(DocURL *u)
{
  openItem("url");
  m_output.addFieldQuotedString("content", u->url());
  closeItem();
}

void PerlModDocVisitor::visit(DocLineBreak *) { singleItem("linebreak"); }
void PerlModDocVisitor::visit(DocHorRuler *) { singleItem("hruler"); }

void PerlModDocVisitor::visit(DocStyleChange *s)
{
  const char *style = 0;
  switch (s->style())
  {
    case DocStyleChange::Bold:          style = "bold"; break;
    case DocStyleChange::Italic:        style = "italic"; break;
    case DocStyleChange::Code:          style = "code"; break;
    case DocStyleChange::Subscript:     style = "subscript"; break;
    case DocStyleChange::Superscript:   style = "superscript"; break;
    case DocStyleChange::Center:        style = "center"; break;
    case DocStyleChange::Small:         style = "small"; break;
    case DocStyleChange::Preformatted:  style = "preformatted"; break;
    case DocStyleChange::Div:           style = "div"; break;
    case DocStyleChange::Span:          style = "span"; break;
                                        
  }
  openItem("style");
  m_output.addFieldQuotedString("style", style)
    .addFieldBoolean("enable", s->enable());
  closeItem();
}

void PerlModDocVisitor::visit(DocVerbatim *s)
{
  const char *type = 0;
  switch(s->type())
  {
  case DocVerbatim::Code:
#if 0
    m_output.add("<programlisting>");
    parseCode(m_ci,s->context(),s->text(),FALSE,0);
    m_output.add("</programlisting>");
#endif
    return;
  case DocVerbatim::Verbatim:	type = "preformatted"; break;
  case DocVerbatim::HtmlOnly:	type = "htmlonly"; break;
  case DocVerbatim::RtfOnly:	type = "rtfonly"; break;
  case DocVerbatim::ManOnly:	type = "manonly"; break;
  case DocVerbatim::LatexOnly:	type = "latexonly"; break;
  case DocVerbatim::XmlOnly:	type = "xmlonly"; break;
  case DocVerbatim::Dot:	type = "dot"; break;
  case DocVerbatim::Msc:	type = "msc"; break;
  }
  openItem(type);
  m_output.addFieldQuotedString("content", s->text());
  closeItem();
}

void PerlModDocVisitor::visit(DocAnchor *anc)
{
  QCString anchor = anc->file() + "_1" + anc->anchor();
  openItem("anchor");
  m_output.addFieldQuotedString("id", anchor);
  closeItem();
}

void PerlModDocVisitor::visit(DocInclude *inc)
{
  const char *type = 0;
  switch(inc->type())
  {
  case DocInclude::IncWithLines:
  #if 0
      { 
         m_t << "<div class=\"fragment\"><pre>";
         QFileInfo cfi( inc->file() );
         FileDef fd( cfi.dirPath(), cfi.fileName() );
         parseCode(m_ci,inc->context(),inc->text().latin1(),inc->isExample(),inc->exampleFile(), &fd);
         m_t << "</pre></div>"; 
      }
      break;
  #endif
    return;
  case DocInclude::Include:
#if 0
    m_output.add("<programlisting>");
    parseCode(m_ci,inc->context(),inc->text(),FALSE,0);
    m_output.add("</programlisting>");
#endif
    return;
  case DocInclude::DontInclude:	return;
  case DocInclude::HtmlInclude:	type = "htmlonly"; break;
  case DocInclude::VerbInclude:	type = "preformatted"; break;
  case DocInclude::Snippet: return;
  }
  openItem(type);
  m_output.addFieldQuotedString("content", inc->text());
  closeItem();
}

void PerlModDocVisitor::visit(DocIncOperator *)
{
#if 0
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst())
  {
    m_output.add("<programlisting>");
  }
  if (op->type()!=DocIncOperator::Skip)
  {
    parseCode(m_ci,op->context(),op->text(),FALSE,0);
  }
  if (op->isLast()) 
  {
    m_output.add("</programlisting>");
  }
  else
  {
    m_output.add('\n');
  }
#endif
}

void PerlModDocVisitor::visit(DocFormula *f)
{
  openItem("formula");
  QCString id;
  id += f->id();
  m_output.addFieldQuotedString("id", id).addFieldQuotedString("content", f->text());
  closeItem();
}

void PerlModDocVisitor::visit(DocIndexEntry *)
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

void PerlModDocVisitor::visit(DocSimpleSectSep *)
{
}

void PerlModDocVisitor::visit(DocCite *cite)
{
  openItem("cite");
  m_output.addFieldQuotedString("text", cite->text());
  closeItem();
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void PerlModDocVisitor::visitPre(DocAutoList *l)
{
  openItem("list");
  m_output.addFieldQuotedString("style", l->isEnumList() ? "ordered" : "itemized");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocAutoList *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocAutoListItem *)
{
  openSubBlock();
}

void PerlModDocVisitor::visitPost(DocAutoListItem *)
{
  closeSubBlock();
}

void PerlModDocVisitor::visitPre(DocPara *)
{
  if (m_textblockstart)
    m_textblockstart = false;
  else
    singleItem("parbreak");
  /*
  openItem("para");
  openSubBlock("content");
  */
}

void PerlModDocVisitor::visitPost(DocPara *)
{
  /*
  closeSubBlock();
  closeItem();
  */
}

void PerlModDocVisitor::visitPre(DocRoot *)
{
}

void PerlModDocVisitor::visitPost(DocRoot *)
{
}

void PerlModDocVisitor::visitPre(DocSimpleSect *s)
{
  const char *type = 0;
  switch (s->type())
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
  case DocSimpleSect::User:		type = "par"; break;
  case DocSimpleSect::Rcs:		type = "rcs"; break;
  case DocSimpleSect::Unknown:
    err("error: unknown simple section found\n");
    break;
  }
  leaveText();
  m_output.openHash();
  openOther();
  openSubBlock(type);
}

void PerlModDocVisitor::visitPost(DocSimpleSect *)
{
  closeSubBlock();
  closeOther();
  m_output.closeHash();
}

void PerlModDocVisitor::visitPre(DocTitle *)
{
  openItem("title");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocTitle *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocSimpleList *) 
{
  openItem("list");
  m_output.addFieldQuotedString("style", "itemized");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocSimpleList *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocSimpleListItem *) { openSubBlock(); }
void PerlModDocVisitor::visitPost(DocSimpleListItem *) { closeSubBlock(); }

void PerlModDocVisitor::visitPre(DocSection *s)
{
  QCString sect = QCString().sprintf("sect%d",s->level());
  openItem(sect);
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocSection *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocHtmlList *l)
{
  openItem("list");
  m_output.addFieldQuotedString("style", (l->type() == DocHtmlList::Ordered) ? "ordered" : "itemized");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocHtmlList *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocHtmlListItem *) { openSubBlock(); }
void PerlModDocVisitor::visitPost(DocHtmlListItem *) { closeSubBlock(); }

//void PerlModDocVisitor::visitPre(DocHtmlPre *)
//{
//  openItem("preformatted");
//  openSubBlock("content");
//  //m_insidePre=TRUE;
//}

//void PerlModDocVisitor::visitPost(DocHtmlPre *)
//{
//  //m_insidePre=FALSE;
//  closeSubBlock();
//  closeItem();
//}

void PerlModDocVisitor::visitPre(DocHtmlDescList *)
{
#if 0
  m_output.add("<variablelist>\n");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlDescList *)
{
#if 0
  m_output.add("</variablelist>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlDescTitle *)
{
#if 0
  m_output.add("<varlistentry><term>");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlDescTitle *)
{
#if 0
  m_output.add("</term></varlistentry>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlDescData *)
{
#if 0
  m_output.add("<listitem>");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlDescData *)
{
#if 0
  m_output.add("</listitem>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlTable *)
{
#if 0
  m_output.add("<table rows=\""); m_output.add(t->numRows());
  m_output.add("\" cols=\""); m_output.add(t->numCols()); m_output.add("\">");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlTable *)
{
#if 0
  m_output.add("</table>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlRow *)
{
#if 0
  m_output.add("<row>\n");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlRow *)
{
#if 0
  m_output.add("</row>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlCell *)
{
#if 0
  if (c->isHeading()) m_output.add("<entry thead=\"yes\">"); else m_output.add("<entry thead=\"no\">");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlCell *)
{
#if 0
  m_output.add("</entry>");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlCaption *)
{
#if 0
  m_output.add("<caption>");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlCaption *)
{
#if 0
  m_output.add("</caption>\n");
#endif
}

void PerlModDocVisitor::visitPre(DocInternal *)
{
#if 0
  m_output.add("<internal>");
#endif
}

void PerlModDocVisitor::visitPost(DocInternal *)
{
#if 0
  m_output.add("</internal>");
#endif
}

void PerlModDocVisitor::visitPre(DocHRef *)
{
#if 0
  m_output.add("<ulink url=\""); m_output.add(href->url()); m_output.add("\">");
#endif
}

void PerlModDocVisitor::visitPost(DocHRef *)
{
#if 0
  m_output.add("</ulink>");
#endif
}

void PerlModDocVisitor::visitPre(DocHtmlHeader *)
{
#if 0
  m_output.add("<sect"); m_output.add(header->level()); m_output.add(">");
#endif
}

void PerlModDocVisitor::visitPost(DocHtmlHeader *)
{
#if 0
  m_output.add("</sect"); m_output.add(header->level()); m_output.add(">\n");
#endif
}

void PerlModDocVisitor::visitPre(DocImage *)
{
#if 0
  m_output.add("<image type=\"");
  switch(img->type())
  {
  case DocImage::Html:  m_output.add("html"); break;
  case DocImage::Latex: m_output.add("latex"); break;
  case DocImage::Rtf:   m_output.add("rtf"); break;
  }
  m_output.add("\"");
  
  QCString baseName=img->name();
  int i;
  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  m_output.add(" name=\""); m_output.add(baseName); m_output.add("\"");
  if (!img->width().isEmpty())
  {
    m_output.add(" width=\"");
    m_output.addQuoted(img->width());
    m_output.add("\"");
  }
  else if (!img->height().isEmpty())
  {
    m_output.add(" height=\"");
    m_output.addQuoted(img->height());
    m_output.add("\"");
  }
  m_output.add(">");
#endif
}

void PerlModDocVisitor::visitPost(DocImage *)
{
#if 0
  m_output.add("</image>");
#endif
}

void PerlModDocVisitor::visitPre(DocDotFile *)
{
#if 0
  m_output.add("<dotfile name=\""); m_output.add(df->file()); m_output.add("\">");
#endif
}

void PerlModDocVisitor::visitPost(DocDotFile *)
{
#if 0
  m_output.add("</dotfile>");
#endif
}
void PerlModDocVisitor::visitPre(DocMscFile *)
{
#if 0
  m_output.add("<mscfile name=\""); m_output.add(df->file()); m_output.add("\">");
#endif
}

void PerlModDocVisitor::visitPost(DocMscFile *)
{
#if 0
  m_output.add("<mscfile>");
#endif
}


void PerlModDocVisitor::visitPre(DocLink *lnk)
{
  openItem("link");
  addLink(lnk->ref(), lnk->file(), lnk->anchor());
}

void PerlModDocVisitor::visitPost(DocLink *)
{
  closeItem();
}

void PerlModDocVisitor::visitPre(DocRef *ref)
{
  openItem("ref");
  if (!ref->hasLinkText())
    m_output.addFieldQuotedString("text", ref->targetTitle());
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocRef *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocSecRefItem *)
{
#if 0
  m_output.add("<tocitem id=\""); m_output.add(ref->file()); m_output.add("_1"); m_output.add(ref->anchor()); m_output.add("\">");
#endif
}

void PerlModDocVisitor::visitPost(DocSecRefItem *)
{
#if 0
  m_output.add("</tocitem>");
#endif
}

void PerlModDocVisitor::visitPre(DocSecRefList *)
{
#if 0
  m_output.add("<toclist>");
#endif
}

void PerlModDocVisitor::visitPost(DocSecRefList *)
{
#if 0
  m_output.add("</toclist>");
#endif
}

//void PerlModDocVisitor::visitPre(DocLanguage *l)
//{
//  openItem("language");
//  m_output.addFieldQuotedString("id", l->id());
//}
//
//void PerlModDocVisitor::visitPost(DocLanguage *)
//{
//  closeItem();
//}

void PerlModDocVisitor::visitPre(DocParamSect *s)
{
  leaveText();
  const char *type = 0;
  switch(s->type())
  {
  case DocParamSect::Param:     type = "params"; break;
  case DocParamSect::RetVal:    type = "retvals"; break;
  case DocParamSect::Exception: type = "exceptions"; break;
  case DocParamSect::TemplateParam: type = "templateparam"; break;
  case DocParamSect::Unknown:
    err("error: unknown parameter section found\n");
    break;
  }
  openOther();
  openSubBlock(type);
}

void PerlModDocVisitor::visitPost(DocParamSect *)
{
  closeSubBlock();
  closeOther();
}

void PerlModDocVisitor::visitPre(DocParamList *pl)
{
  leaveText();
  m_output.openHash()
    .openList("parameters");
  //QStrListIterator li(pl->parameters());
  //const char *s;
  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  for (li.toFirst();(param=li.current());++li)
  {
    QCString s;
    if (param->kind()==DocNode::Kind_Word)
    {
      s = ((DocWord*)param)->word(); 
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      s = ((DocLinkedWord*)param)->word(); 
    }
    m_output.openHash()
      .addFieldQuotedString("name", s)
      .closeHash();
  }
  m_output.closeList()
    .openList("doc");
}

void PerlModDocVisitor::visitPost(DocParamList *)
{
  leaveText();
  m_output.closeList()
    .closeHash();
}

void PerlModDocVisitor::visitPre(DocXRefItem *)
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
  openItem("xrefitem");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocXRefItem *)
{
  closeSubBlock();
  closeItem();
#if 0
  m_output.add("</xrefdescription>");
  m_output.add("</xrefsect>");
#endif
}

void PerlModDocVisitor::visitPre(DocInternalRef *ref)
{
  openItem("ref");
  addLink(0,ref->file(),ref->anchor());
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocInternalRef *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocCopy *)
{
}

void PerlModDocVisitor::visitPost(DocCopy *)
{
}

void PerlModDocVisitor::visitPre(DocText *)
{
}

void PerlModDocVisitor::visitPost(DocText *)
{
}

void PerlModDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  openItem("blockquote");
  openSubBlock("content");
}

void PerlModDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  closeSubBlock();
  closeItem();
}

static void addTemplateArgumentList(ArgumentList *al,PerlModOutput &output,const char *)
{
  QCString indentStr;
  if (!al)
    return;
  output.openList("template_parameters");
  ArgumentListIterator ali(*al);
  Argument *a;
  for (ali.toFirst();(a=ali.current());++ali)
  {
    output.openHash();
    if (!a->type.isEmpty())
      output.addFieldQuotedString("type", a->type);
    if (!a->name.isEmpty())
      output.addFieldQuotedString("declaration_name", a->name)
	.addFieldQuotedString("definition_name", a->name);
    if (!a->defval.isEmpty())
      output.addFieldQuotedString("default", a->defval);
    output.closeHash();
  }
  output.closeList();
}

#if 0
static void addMemberTemplateLists(MemberDef *md,PerlModOutput &output)
{
  ClassDef *cd = md->getClassDef();
  const char *cname = cd ? cd->name().data() : 0;
  if (md->templateArguments()) // function template prefix
    addTemplateArgumentList(md->templateArguments(),output,cname);
}
#endif

static void addTemplateList(ClassDef *cd,PerlModOutput &output)
{
  addTemplateArgumentList(cd->templateArguments(),output,cd->name());
}

static void addPerlModDocBlock(PerlModOutput &output,
			    const char *name,
			    const QCString &fileName,
			    int lineNr,
			    Definition *scope,
			    MemberDef *md,
			    const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty())
    output.addField(name).add("{}");
  else {
    DocNode *root = validatingParseDoc(fileName,lineNr,scope,md,stext,FALSE,0);
    output.openHash(name);
    PerlModDocVisitor *visitor = new PerlModDocVisitor(output);
    root->accept(visitor);
    visitor->finish();
    output.closeHash();
    delete visitor;
    delete root;
  }
}

static const char *getProtectionName(Protection prot) 
{
  switch (prot)
  {
  case Public:    return "public";
  case Protected: return "protected";
  case Private:   return "private";
  case Package:   return "package";
  }
  return 0;
}

static const char *getVirtualnessName(Specifier virt)
{
  switch(virt)
  {
  case Normal:  return "non_virtual";
  case Virtual: return "virtual";
  case Pure:    return "pure_virtual";
  }
  return 0;
}

static QCString pathDoxyfile;
static QCString pathDoxyExec;

void setPerlModDoxyfile(const QCString &qs)
{
  pathDoxyfile = qs;
  pathDoxyExec = QDir::currentDirPath().utf8();
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
  QCString pathMakefile;

  inline PerlModGenerator(bool pretty) : m_output(pretty) { }

  void generatePerlModForMember(MemberDef *md, Definition *);
  void generatePerlModSection(Definition *d, MemberList *ml,
			      const char *name, const char *header=0);
  void addListOfAllMembers(ClassDef *cd);
  void generatePerlModForClass(ClassDef *cd);
  void generatePerlModForNamespace(NamespaceDef *nd);
  void generatePerlModForFile(FileDef *fd);
  void generatePerlModForGroup(GroupDef *gd);
  void generatePerlModForPage(PageDef *pi);
  
  bool createOutputFile(QFile &f, const char *s);
  bool createOutputDir(QDir &perlModDir);
  bool generateDoxyLatexTex();
  bool generateDoxyFormatTex();
  bool generateDoxyStructurePM();
  bool generateDoxyLatexPL();
  bool generateDoxyLatexStructurePL();
  bool generateDoxyRules();
  bool generateMakefile();
  bool generatePerlModOutput();

  void generate();
};

void PerlModGenerator::generatePerlModForMember(MemberDef *md,Definition *)
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
  bool isFunc=FALSE;
  switch (md->memberType())
  {
  case MemberDef::Define:      memType="define";    break;
  case MemberDef::EnumValue:   memType="enumvalue"; break;
  case MemberDef::Property:    memType="property";  break;
  case MemberDef::Variable:    memType="variable";  break;
  case MemberDef::Typedef:     memType="typedef";   break;
  case MemberDef::Enumeration: memType="enum";      break;
  case MemberDef::Function:    memType="function";  isFunc=TRUE; break;
  case MemberDef::Signal:      memType="signal";    isFunc=TRUE; break;
  //case MemberDef::Prototype:   memType="prototype"; isFunc=TRUE; break;
  case MemberDef::Friend:      memType="friend";    isFunc=TRUE; break;
  case MemberDef::DCOP:        memType="dcop";      isFunc=TRUE; break;
  case MemberDef::Slot:        memType="slot";      isFunc=TRUE; break;
  case MemberDef::Event:       memType="event";     break;
  }

  m_output.openHash()
    .addFieldQuotedString("kind", memType)
    .addFieldQuotedString("name", md->name())
    .addFieldQuotedString("virtualness", getVirtualnessName(md->virtualness()))
    .addFieldQuotedString("protection", getProtectionName(md->protection()))
    .addFieldBoolean("static", md->isStatic());
  
  addPerlModDocBlock(m_output,"brief",md->getDefFileName(),md->getDefLine(),md->getOuterScope(),md,md->briefDescription());
  addPerlModDocBlock(m_output,"detailed",md->getDefFileName(),md->getDefLine(),md->getOuterScope(),md,md->documentation());
  if (md->memberType()!=MemberDef::Define &&
      md->memberType()!=MemberDef::Enumeration)
    m_output.addFieldQuotedString("type", md->typeString());
  
  LockingPtr<ArgumentList> al = md->argumentList();
  if (isFunc) //function
  {
    m_output.addFieldBoolean("const", al!=0 && al->constSpecifier)
      .addFieldBoolean("volatile", al!=0 && al->volatileSpecifier);

    m_output.openList("parameters");
    LockingPtr<ArgumentList> declAl = md->declArgumentList();
    LockingPtr<ArgumentList> defAl  = md->argumentList();
    if (declAl!=0 && declAl->count()>0)
    {
      ArgumentListIterator declAli(*declAl);
      ArgumentListIterator defAli(*defAl);
      Argument *a;
      for (declAli.toFirst();(a=declAli.current());++declAli)
      {
	Argument *defArg = defAli.current();
	m_output.openHash();

	if (!a->name.isEmpty())
	  m_output.addFieldQuotedString("declaration_name", a->name);

	if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
	  m_output.addFieldQuotedString("definition_name", defArg->name);

	if (!a->type.isEmpty())
	  m_output.addFieldQuotedString("type", a->type);

	if (!a->array.isEmpty())
	  m_output.addFieldQuotedString("array", a->array);

	if (!a->defval.isEmpty())
	  m_output.addFieldQuotedString("default_value", a->defval);

	if (!a->attrib.isEmpty())
	  m_output.addFieldQuotedString("attributes", a->attrib);
	
	m_output.closeHash();
	if (defArg) ++defAli;
      }
    }
    m_output.closeList();
  }
  else if (md->memberType()==MemberDef::Define &&
	   md->argsString()!=0) // define
  {
    m_output.openList("parameters");
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      m_output.openHash()
	.addFieldQuotedString("name", a->type)
	.closeHash();
    }
    m_output.closeList();
  }
  else if (md->argsString()!=0) 
  {
    m_output.addFieldQuotedString("arguments", md->argsString());
  }

  if (!md->initializer().isEmpty())
    m_output.addFieldQuotedString("initializer", md->initializer());
  
  if (md->excpString())
    m_output.addFieldQuotedString("exceptions", md->excpString());
  
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    LockingPtr<MemberList> enumFields = md->enumFieldList();
    if (enumFields!=0)
    {
      m_output.openList("values");
      MemberListIterator emli(*enumFields);
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
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

  MemberDef *rmd = md->reimplements();
  if (rmd)
    m_output.openHash("reimplements")
      .addFieldQuotedString("name", rmd->name())
      .closeHash();

  LockingPtr<MemberList> rbml = md->reimplementedBy();
  if (rbml!=0)
  {
    MemberListIterator mli(*rbml);
    m_output.openList("reimplemented_by");
    for (mli.toFirst();(rmd=mli.current());++mli)
      m_output.openHash()
	.addFieldQuotedString("name", rmd->name())
	.closeHash();
    m_output.closeList();
  }
  
  m_output.closeHash();
}

void PerlModGenerator::generatePerlModSection(Definition *d,
					      MemberList *ml,const char *name,const char *header)
{
  if (ml==0) return; // empty list

  m_output.openHash(name);

  if (header)
    m_output.addFieldQuotedString("header", header);
  
  m_output.openList("members");
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    generatePerlModForMember(md,d);
  }
  m_output.closeList()
    .closeHash();
}

void PerlModGenerator::addListOfAllMembers(ClassDef *cd)
{
  m_output.openList("all_members");
  if (cd->memberNameInfoSDict())
  {
    MemberNameInfoSDict::Iterator mnii(*cd->memberNameInfoSDict());
    MemberNameInfo *mni;
    for (mnii.toFirst();(mni=mnii.current());++mnii)
    {
      MemberNameInfoIterator mii(*mni);
      MemberInfo *mi;
      for (mii.toFirst();(mi=mii.current());++mii)
      {
        MemberDef *md=mi->memberDef;
        ClassDef  *cd=md->getClassDef();
        Definition *d=md->getGroupDef();
        if (d==0) d = cd;

        m_output.openHash()
          .addFieldQuotedString("name", md->name())
          .addFieldQuotedString("virtualness", getVirtualnessName(md->virtualness()))
          .addFieldQuotedString("protection", getProtectionName(mi->prot));

        if (!mi->ambiguityResolutionScope.isEmpty())
          m_output.addFieldQuotedString("ambiguity_scope", mi->ambiguityResolutionScope);

        m_output.addFieldQuotedString("scope", cd->name())
          .closeHash();
      }
    }
  }
  m_output.closeList();
}

void PerlModGenerator::generatePerlModForClass(ClassDef *cd)
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
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.

  m_output.openHash()
    .addFieldQuotedString("name", cd->name());
  
  if (cd->baseClasses())
  {
    m_output.openList("base");
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
      m_output.openHash()
	.addFieldQuotedString("name", bcd->classDef->displayName())
	.addFieldQuotedString("virtualness", getVirtualnessName(bcd->virt))
	.addFieldQuotedString("protection", getProtectionName(bcd->prot))
	.closeHash();
    m_output.closeList();
  }

  if (cd->subClasses())
  {
    m_output.openList("derived");
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
      m_output.openHash()
	.addFieldQuotedString("name", bcd->classDef->displayName())
	.addFieldQuotedString("virtualness", getVirtualnessName(bcd->virt))
	.addFieldQuotedString("protection", getProtectionName(bcd->prot))
	.closeHash();
    m_output.closeList();
  }

  ClassSDict *cl = cd->getClassSDict();
  if (cl)
  {
    m_output.openList("inner");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  IncludeInfo *ii=cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      m_output.openHash("includes");
#if 0
      if (ii->fileDef && !ii->fileDef->isReference()) // TODO: support external references
        t << " id=\"" << ii->fileDef->getOutputFileBase() << "\"";
#endif
      m_output.addFieldBoolean("local", ii->local)
	.addFieldQuotedString("name", nm)
	.closeHash();
    }
  }

  addTemplateList(cd,m_output);
  addListOfAllMembers(cd);
  if (cd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*cd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
      generatePerlModSection(cd,mg->members(),"user_defined",mg->header());
  }

  generatePerlModSection(cd,cd->getMemberList(MemberList::pubTypes),"public_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberList::pubMethods),"public_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::pubAttribs),"public_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::pubSlots),"public_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberList::signals),"signals");
  generatePerlModSection(cd,cd->getMemberList(MemberList::dcopMethods),"dcop_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::properties),"properties");
  generatePerlModSection(cd,cd->getMemberList(MemberList::pubStaticMethods),"public_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::pubStaticAttribs),"public_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proTypes),"protected_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proMethods),"protected_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proAttribs),"protected_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proSlots),"protected_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proStaticMethods),"protected_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::proStaticAttribs),"protected_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priTypes),"private_typedefs");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priMethods),"private_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priAttribs),"private_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priSlots),"private_slots");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priStaticMethods),"private_static_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::priStaticAttribs),"private_static_members");
  generatePerlModSection(cd,cd->getMemberList(MemberList::friends),"friend_methods");
  generatePerlModSection(cd,cd->getMemberList(MemberList::related),"related_methods");

  addPerlModDocBlock(m_output,"brief",cd->getDefFileName(),cd->getDefLine(),cd,0,cd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",cd->getDefFileName(),cd->getDefLine(),cd,0,cd->documentation());

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

void PerlModGenerator::generatePerlModForNamespace(NamespaceDef *nd)
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
  
  ClassSDict *cl = nd->getClassSDict();
  if (cl)
  {
    m_output.openList("classes");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  NamespaceSDict *nl = nd->getNamespaceSDict();
  if (nl)
  {
    m_output.openList("namespaces");
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
      m_output.openHash()
	.addFieldQuotedString("name", nd->name())
	.closeHash();
    m_output.closeList();
  }

  if (nd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*nd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
      generatePerlModSection(nd,mg->members(),"user-defined",mg->header());
  }

  generatePerlModSection(nd,nd->getMemberList(MemberList::decDefineMembers),"defines");
  generatePerlModSection(nd,nd->getMemberList(MemberList::decProtoMembers),"prototypes");
  generatePerlModSection(nd,nd->getMemberList(MemberList::decTypedefMembers),"typedefs");
  generatePerlModSection(nd,nd->getMemberList(MemberList::decEnumMembers),"enums");
  generatePerlModSection(nd,nd->getMemberList(MemberList::decFuncMembers),"functions");
  generatePerlModSection(nd,nd->getMemberList(MemberList::decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",nd->getDefFileName(),nd->getDefLine(),0,0,nd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",nd->getDefFileName(),nd->getDefLine(),0,0,nd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForFile(FileDef *fd)
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
  
  IncludeInfo *inc;
  m_output.openList("includes");
  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> ili1(*fd->includeFileList());
    for (ili1.toFirst();(inc=ili1.current());++ili1)
    {
      m_output.openHash()
        .addFieldQuotedString("name", inc->includeName);
      if (inc->fileDef && !inc->fileDef->isReference())
      {
        m_output.addFieldQuotedString("ref", inc->fileDef->getOutputFileBase());
      }
      m_output.closeHash();
    }
  }
  m_output.closeList();
  
  m_output.openList("included_by");
  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili2(*fd->includedByFileList());
    for (ili2.toFirst();(inc=ili2.current());++ili2)
    {
      m_output.openHash()
        .addFieldQuotedString("name", inc->includeName);
      if (inc->fileDef && !inc->fileDef->isReference())
      {
        m_output.addFieldQuotedString("ref", inc->fileDef->getOutputFileBase());
      }
      m_output.closeHash();
    }
  }
  m_output.closeList();
  
  generatePerlModSection(fd,fd->getMemberList(MemberList::decDefineMembers),"defines");
  generatePerlModSection(fd,fd->getMemberList(MemberList::decProtoMembers),"prototypes");
  generatePerlModSection(fd,fd->getMemberList(MemberList::decTypedefMembers),"typedefs");
  generatePerlModSection(fd,fd->getMemberList(MemberList::decEnumMembers),"enums");
  generatePerlModSection(fd,fd->getMemberList(MemberList::decFuncMembers),"functions");
  generatePerlModSection(fd,fd->getMemberList(MemberList::decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",fd->getDefFileName(),fd->getDefLine(),0,0,fd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",fd->getDefFileName(),fd->getDefLine(),0,0,fd->documentation());

  m_output.closeHash();
}

void PerlModGenerator::generatePerlModForGroup(GroupDef *gd)
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

  FileList *fl = gd->getFiles();
  if (fl)
  {
    m_output.openList("files");
    QListIterator<FileDef> fli(*fl);
    FileDef *fd = fl->first();
    for (fli.toFirst();(fd=fli.current());++fli)
      m_output.openHash()
	.addFieldQuotedString("name", fd->name())
	.closeHash();
    m_output.closeList();
  }

  ClassSDict *cl = gd->getClasses();
  if (cl)
  {
    m_output.openList("classes");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      m_output.openHash()
	.addFieldQuotedString("name", cd->name())
	.closeHash();
    m_output.closeList();
  }

  NamespaceSDict *nl = gd->getNamespaces();
  if (nl)
  {
    m_output.openList("namespaces");
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
      m_output.openHash()
	.addFieldQuotedString("name", nd->name())
	.closeHash();
    m_output.closeList();
  }

  PageSDict *pl = gd->getPages();
  if (pl)
  {
    m_output.openList("pages");
    PageSDict::Iterator pli(*pl);
    PageDef *pd;
    for (pli.toFirst();(pd=pli.current());++pli)
      m_output.openHash()
	.addFieldQuotedString("title", pd->title())
	.closeHash();
    m_output.closeList();
  }

  GroupList *gl = gd->getSubGroups();
  if (gl)
  {
    m_output.openList("groups");
    GroupListIterator gli(*gl);
    GroupDef *sgd;
    for (gli.toFirst();(sgd=gli.current());++gli)
      m_output.openHash()
	.addFieldQuotedString("title", sgd->groupTitle())
	.closeHash();
    m_output.closeList();
  }

  if (gd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*gd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
      generatePerlModSection(gd,mg->members(),"user-defined",mg->header());
  }

  generatePerlModSection(gd,gd->getMemberList(MemberList::decDefineMembers),"defines");
  generatePerlModSection(gd,gd->getMemberList(MemberList::decProtoMembers),"prototypes");
  generatePerlModSection(gd,gd->getMemberList(MemberList::decTypedefMembers),"typedefs");
  generatePerlModSection(gd,gd->getMemberList(MemberList::decEnumMembers),"enums");
  generatePerlModSection(gd,gd->getMemberList(MemberList::decFuncMembers),"functions");
  generatePerlModSection(gd,gd->getMemberList(MemberList::decVarMembers),"variables");

  addPerlModDocBlock(m_output,"brief",gd->getDefFileName(),gd->getDefLine(),0,0,gd->briefDescription());
  addPerlModDocBlock(m_output,"detailed",gd->getDefFileName(),gd->getDefLine(),0,0,gd->documentation());

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
    
  SectionInfo *si = Doxygen::sectionDict.find(pd->name());
  if (si)
    m_output.addFieldQuotedString("title", si->title);

  addPerlModDocBlock(m_output,"detailed",pd->docFile(),pd->docLine(),0,0,pd->documentation());
  m_output.closeHash();
}

bool PerlModGenerator::generatePerlModOutput()
{
  QFile outputFile;
  if (!createOutputFile(outputFile, pathDoxyDocsPM))
    return false;
  
  FTextStream outputTextStream(&outputFile);
  PerlModOutputStream outputStream(&outputTextStream);
  m_output.setPerlModOutputStream(&outputStream);
  m_output.add("$doxydocs=").openHash();
  
  m_output.openList("classes");
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
    generatePerlModForClass(cd);
  m_output.closeList();

  m_output.openList("namespaces");
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
    generatePerlModForNamespace(nd);
  m_output.closeList();

  m_output.openList("files");
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
      generatePerlModForFile(fd);
  }
  m_output.closeList();

  m_output.openList("groups");
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    generatePerlModForGroup(gd);
  }
  m_output.closeList();

  m_output.openList("pages");
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    generatePerlModForPage(pd);
  }
  if (Doxygen::mainPage)
  {
    generatePerlModForPage(Doxygen::mainPage);
  }
  m_output.closeList();

  m_output.closeHash().add(";\n1;\n");
  return true;
}

bool PerlModGenerator::createOutputFile(QFile &f, const char *s)
{
  f.setName(s);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n", s);
    return false;
  }
  return true;
}

bool PerlModGenerator::createOutputDir(QDir &perlModDir)
{
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty())
  {
    outputDirectory=QDir::currentDirPath().utf8();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
	err("error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",outputDirectory.data());
	exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
	err("notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath().utf8();
  }

  QDir dir(outputDirectory);
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(outputDirectory))
    {
      err("Cannot create directory %s\n",outputDirectory.data());
      return false;
    }
  }
 
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
  QFile doxyModelPM;
  if (!createOutputFile(doxyModelPM, pathDoxyStructurePM))
    return false;

  FTextStream doxyModelPMStream(&doxyModelPM);
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
  QFile doxyRules;
  if (!createOutputFile(doxyRules, pathDoxyRules))
    return false;

  bool perlmodLatex = Config_getBool("PERLMOD_LATEX");
  QCString prefix = Config_getString("PERLMOD_MAKEVAR_PREFIX");

  FTextStream doxyRulesStream(&doxyRules);
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

bool PerlModGenerator::generateMakefile()
{
  QFile makefile;
  if (!createOutputFile(makefile, pathMakefile))
    return false;

  bool perlmodLatex = Config_getBool("PERLMOD_LATEX");
  QCString prefix = Config_getString("PERLMOD_MAKEVAR_PREFIX");

  FTextStream makefileStream(&makefile);
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

  return true;
}

bool PerlModGenerator::generateDoxyLatexStructurePL()
{
  QFile doxyLatexStructurePL;
  if (!createOutputFile(doxyLatexStructurePL, pathDoxyLatexStructurePL))
    return false;

  FTextStream doxyLatexStructurePLStream(&doxyLatexStructurePL);
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
  QFile doxyLatexPL;
  if (!createOutputFile(doxyLatexPL, pathDoxyLatexPL))
    return false;

  FTextStream doxyLatexPLStream(&doxyLatexPL);
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
  QFile doxyFormatTex;
  if (!createOutputFile(doxyFormatTex, pathDoxyFormatTex))
    return false;

  FTextStream doxyFormatTexStream(&doxyFormatTex);
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
  QFile doxyLatexTex;
  if (!createOutputFile(doxyLatexTex, pathDoxyLatexTex))
    return false;

  FTextStream doxyLatexTexStream(&doxyLatexTex);
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

  QDir perlModDir;
  if (!createOutputDir(perlModDir))
    return;

  bool perlmodLatex = Config_getBool("PERLMOD_LATEX");

  QCString perlModAbsPath = perlModDir.absPath().utf8();
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
}

void generatePerlMod()
{
  PerlModGenerator pmg(Config_getBool("PERLMOD_PRETTY"));
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
