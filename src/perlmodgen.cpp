/******************************************************************************
 *
 *
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include "page.h"

#include <qdir.h>
#include <qstack.h>
#include <qdict.h>
#include <qfile.h>
#include <qtextstream.h>

#define PERLOUTPUT_MAX_INDENTATION 40

class PerlModOutputStream
{
public:

  QString m_s;
  QTextStream *m_t;

  PerlModOutputStream(QTextStream *t = 0) : m_t(t) { }

  void add(char c);
  void add(const char *s);
  void add(QCString &s);
  void add(QString &s);
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

void PerlModOutputStream::add(QString &s)
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

  PerlModOutput(PerlModOutputStream *os) : m_stream(os), m_indentation(0) { m_spaces[0] = 0; }
  virtual ~PerlModOutput() { }

  inline PerlModOutput &openSave() { iopenSave(); return *this; }
  inline PerlModOutput &closeSave(QString &s) { icloseSave(s); return *this; }
  
  inline PerlModOutput &add(char c) { m_stream->add(c); return *this; }
  inline PerlModOutput &add(const char *s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(QCString &s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(QString &s) { m_stream->add(s); return *this; }
  inline PerlModOutput &add(int n) { m_stream->add(n); return *this; }
  inline PerlModOutput &add(unsigned int n) { m_stream->add(n); return *this; }

  PerlModOutput &addQuoted(const char *s) { iaddQuoted(s); return *this; }
  inline PerlModOutput &indent() { m_stream->add(m_spaces); return *this; }

  inline PerlModOutput &open(char c, const char *s = 0) { iopen(c, s); return *this; }
  inline PerlModOutput &close(char c = 0) { iclose(c); return *this; }

  inline PerlModOutput &addField(const char *s) { iaddField(s); return *this; }
  inline PerlModOutput &addFieldQuoted(const char *field, const char *content)
  {
    iaddFieldQuoted(field, content); return *this;
  }
  inline PerlModOutput &openList(const char *s = 0) { open('[', s); return *this; }
  inline PerlModOutput &closeList() { close(']'); return *this; }
  inline PerlModOutput &openHash(const char *s = 0 ) { open('{', s); return *this; }
  inline PerlModOutput &closeHash() { close('}'); return *this; }

protected:
  
  void iopenSave();
  void icloseSave(QString &);
  
  void incIndent();
  void decIndent();

  void iaddQuoted(const char *);
  void iaddFieldQuoted(const char *, const char *);
  void iaddField(const char *);

  void iopen(char, const char *);
  void iclose(char);

private:
  
  PerlModOutputStream *m_stream;
  int m_indentation;

  QStack<PerlModOutputStream> m_saved;
  char m_spaces[PERLOUTPUT_MAX_INDENTATION * 2 + 2];
};

void PerlModOutput::iopenSave()
{
  m_saved.push(m_stream);
  m_stream = new PerlModOutputStream();
}

void PerlModOutput::icloseSave(QString &s)
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
    if (c == '\'')
      m_stream->add('\\');
    m_stream->add(c);
  }
}
  
inline void PerlModOutput::iaddField(const char *s)
{
  indent();
  m_stream->add(s);
  m_stream->add(" => ");
}

void PerlModOutput::iaddFieldQuoted(const char *field, const char *content)
{
  iaddField(field);
  m_stream->add('\'');
  iaddQuoted(content);
  m_stream->add("',\n");
}

void PerlModOutput::iopen(char c, const char *s)
{
  if (s != 0)
    iaddField(s);
  else
    indent();
  m_stream->add(c);
  m_stream->add('\n');
  incIndent();
}

void PerlModOutput::iclose(char c)
{
  decIndent(); 
  indent();
  if (c != 0)
    m_stream->add(c); 
  m_stream->add(",\n"); 
}

/*! @brief Concrete visitor implementation for Perl output. */
class PerlModDocVisitor : public DocVisitor
{
public:
  PerlModDocVisitor(PerlModOutput &);
  virtual ~PerlModDocVisitor() {}

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
  void visitPre(DocHtmlPre *);
  void visitPost(DocHtmlPre *);
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
  void visitPre(DocLink *);
  void visitPost(DocLink *);
  void visitPre(DocRef *);
  void visitPost(DocRef *);
  void visitPre(DocSecRefItem *);
  void visitPost(DocSecRefItem *);
  void visitPre(DocSecRefList *);
  void visitPost(DocSecRefList *);
  void visitPre(DocLanguage *);
  void visitPost(DocLanguage *);
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

private:

  //--------------------------------------
  // helper functions
  //--------------------------------------

  void addLink(const QString &ref,const QString &file,
	       const QString &anchor);
   
  void enterText();
  void leaveText();

  void openItem(const char *);
  void closeItem();
  void singleItem(const char *);
  void openSubBlock(const char * = 0);
  void closeSubBlock();

  //--------------------------------------
  // state variables
  //--------------------------------------

  PerlModOutput &m_output;
  bool m_textmode;
  QString m_other;
};

PerlModDocVisitor::PerlModDocVisitor(PerlModOutput &output)
  : m_output(output), m_textmode(false)
{
  m_output.openList("doc");
}

void PerlModDocVisitor::finish()
{
  leaveText();
  m_output.closeList()
    .add(m_other);
}

void PerlModDocVisitor::addLink(const QString &,const QString &file,const QString &anchor)
{
  QString link = file;
  if (!anchor.isEmpty())
    (link += "_1") += anchor;
  m_output.addFieldQuoted("link", link);
}

void PerlModDocVisitor::openItem(const char *name)
{
  leaveText();
  m_output.openHash().addFieldQuoted("type", name);
}

void PerlModDocVisitor::closeItem()
{
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
  m_output.add("\'\n");
  closeItem();
  m_textmode = false;
}

void PerlModDocVisitor::singleItem(const char *name)
{
  openItem(name);
  closeItem();
}

void PerlModDocVisitor::openSubBlock(const char *s)
{
  m_output.openList(s);
}

void PerlModDocVisitor::closeSubBlock()
{
  leaveText();
  m_output.closeList();
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
  m_output.addFieldQuoted("content", w->word());
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
  switch(sy->symbol())
  {
  case DocSymbol::At:      c = '@'; break;
  case DocSymbol::Less:    c = '<'; break;
  case DocSymbol::Greater: c = '>'; break;
  case DocSymbol::Amp:     c = '&'; break;
  case DocSymbol::Dollar:  c = '$'; break;
  case DocSymbol::Hash:    c = '#'; break;
  case DocSymbol::Percent: c = '%'; break;
  case DocSymbol::Quot:    c = '"'; break;
  case DocSymbol::Szlig:   c = 'ß'; break;
  case DocSymbol::Nbsp:    c = ' '; break;
  case DocSymbol::Uml:     c = sy->letter(); break;
  case DocSymbol::Acute:   c = sy->letter(); break;
  case DocSymbol::Grave:   c = sy->letter(); break;
  case DocSymbol::Circ:    c = sy->letter(); break;
  case DocSymbol::Tilde:   c = sy->letter(); break;
  case DocSymbol::Cedil:   c = sy->letter(); break;
  case DocSymbol::Ring:    c = sy->letter(); break;
  case DocSymbol::BSlash:  s = "\\\\"; break;
  case DocSymbol::Copy:    s = "(c)"; break;
  case DocSymbol::Apos:    s = "\\\'"; break;
  default:
    err("Error: unknown symbol found\n");
  }
  enterText();
  if (c != 0)
    m_output.add(c);
  else
    m_output.add(s);
}

void PerlModDocVisitor::visit(DocURL *u)
{
  openItem("url");
  m_output.addQuoted(u->url());
  closeItem();
}

void PerlModDocVisitor::visit(DocLineBreak *) { singleItem("linebreak"); }
void PerlModDocVisitor::visit(DocHorRuler *) { singleItem("hruler"); }
void PerlModDocVisitor::visit(DocStyleChange *s)
{
  switch (s->style())
  {
  case DocStyleChange::Bold: 		break; // bold
  case DocStyleChange::Italic: 		break; // italic
  case DocStyleChange::Code: 		break; // code
  case DocStyleChange::Subscript: 	break; // subscript
  case DocStyleChange::Superscript: 	break; // superscript
  case DocStyleChange::Center: 		break; // center
  case DocStyleChange::Small: 		break; // small
  default:
    err("Error: unknown style found.\n");
  }
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
  case DocVerbatim::LatexOnly:	type = "latexonly"; break;
  }
  openItem(type);
  m_output.addFieldQuoted("content", s->text());
  closeItem();
}

void PerlModDocVisitor::visit(DocAnchor *anc)
{
  QString anchor = anc->file() + "_1" + anc->anchor();
  openItem("anchor");
  m_output.addFieldQuoted("id", anchor);
  closeItem();
}

void PerlModDocVisitor::visit(DocInclude *inc)
{
  const char *type = 0;
  switch(inc->type())
  {
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
  }
  openItem(type);
  m_output.addFieldQuoted("content", inc->text());
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
  QString id;
  id += f->id();
  m_output.addFieldQuoted("id", id).addFieldQuoted("content", f->text());
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

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void PerlModDocVisitor::visitPre(DocAutoList *l)
{
  openItem(l->isEnumList() ? "ordered_list" : "itemized_list");
  openSubBlock("content");
}
void PerlModDocVisitor::visitPost(DocAutoList *) { closeSubBlock(); }

void PerlModDocVisitor::visitPre(DocAutoListItem *) { openSubBlock(); }
void PerlModDocVisitor::visitPost(DocAutoListItem *) { closeSubBlock(); }
void PerlModDocVisitor::visitPre(DocPara *)
{
  openItem("para");
  openSubBlock("content");
}
void PerlModDocVisitor::visitPost(DocPara *)
{
  closeSubBlock();
  closeItem();
}

void PerlModDocVisitor::visitPre(DocRoot *)
{
}

void PerlModDocVisitor::visitPost(DocRoot *)
{
}

void PerlModDocVisitor::visitPre(DocSimpleSect *s)
{
  leaveText();
  const char *type;
  switch (s->type())
  {
  case DocSimpleSect::See:		type = "see"; break;
  case DocSimpleSect::Return:		type = "return"; break;
  case DocSimpleSect::Author:		type = "author"; break;
  case DocSimpleSect::Authors:		type = "authors"; break;
  case DocSimpleSect::Version:		type = "version"; break;
  case DocSimpleSect::Since:		type = "since"; break;
  case DocSimpleSect::Date:		type = "date"; break;
  case DocSimpleSect::Note:		type = "bug"; break;
  case DocSimpleSect::Warning:		type = "warning"; break;
  case DocSimpleSect::Pre:		type = "pre"; break;
  case DocSimpleSect::Post:		type = "post"; break;
  case DocSimpleSect::Invar:		type = "invariant"; break;
  case DocSimpleSect::Remark:		type = "remark"; break;
  case DocSimpleSect::Attention:	type = "attention"; break;
  case DocSimpleSect::User:		type = "par"; break;
  default:	    			type = "illegal"; break;
  }
  m_output.openSave().openList(type);
}

void PerlModDocVisitor::visitPost(DocSimpleSect *)
{
  QString content;
  leaveText();
  m_output.closeList().closeSave(content);
  m_other += content;
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
  openItem("itemized_list");
  openSubBlock("content");
}
void PerlModDocVisitor::visitPost(DocSimpleList *) { closeSubBlock(); }
void PerlModDocVisitor::visitPre(DocSimpleListItem *) { openSubBlock(); }
void PerlModDocVisitor::visitPost(DocSimpleListItem *) { closeSubBlock(); }

void PerlModDocVisitor::visitPre(DocSection *s)
{
  QString sect = "sect" + (s->level() + 1);
  openItem(sect);
  openSubBlock("content");
}
void PerlModDocVisitor::visitPost(DocSection *) { closeSubBlock(); }

void PerlModDocVisitor::visitPre(DocHtmlList *l)
{
  openItem((l->type() == DocHtmlList::Ordered) ? "ordered_list" : "itemized_list");
  openSubBlock("content");
}
void PerlModDocVisitor::visitPost(DocHtmlList *) { closeSubBlock(); }
void PerlModDocVisitor::visitPre(DocHtmlListItem *) { openSubBlock(); }
void PerlModDocVisitor::visitPost(DocHtmlListItem *) { closeSubBlock(); }

void PerlModDocVisitor::visitPre(DocHtmlPre *)
{
  openItem("preformatted");
  openSubBlock("content");
  //m_insidePre=TRUE;
}

void PerlModDocVisitor::visitPost(DocHtmlPre *)
{
  //m_insidePre=FALSE;
  closeSubBlock();
  closeItem();
}

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
    m_output.addFieldQuoted("text", ref->targetTitle());
}

void PerlModDocVisitor::visitPost(DocRef *)
{
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

void PerlModDocVisitor::visitPre(DocLanguage *l)
{
  openItem("language");
  m_output.addFieldQuoted("id", l->id());
}

void PerlModDocVisitor::visitPost(DocLanguage *)
{
  closeItem();
}

void PerlModDocVisitor::visitPre(DocParamSect *s)
{
  leaveText();
  const char *type = 0;
  switch(s->type())
  {
  case DocParamSect::Param:     type = "params"; break;
  case DocParamSect::RetVal:    type = "retvals"; break;
  case DocParamSect::Exception: type = "exceptions"; break;
  default: ASSERT(0);
  }
  m_output.openSave().openList(type);
}

void PerlModDocVisitor::visitPost(DocParamSect *)
{
  leaveText();
  QString content;
  m_output.closeList().closeSave(content);
  m_other += content;
}

void PerlModDocVisitor::visitPre(DocParamList *pl)
{
  leaveText();
  m_output.openHash()
    .openList("parameters");
  QStrListIterator li(pl->parameters());
  const char *s;
  for (li.toFirst();(s=li.current());++li)
  {
    m_output.openHash()
      .addFieldQuoted("name", s)
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
}

void PerlModDocVisitor::visitPost(DocXRefItem *)
{
#if 0
  m_output.add("</xrefdescription>");
  m_output.add("</xrefsect>");
#endif
}

void PerlModDocVisitor::visitPre(DocInternalRef *ref)
{
  openItem("ref");
  addLink(0,ref->file(),ref->anchor());
}

void PerlModDocVisitor::visitPost(DocInternalRef *)
{
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

static void addTemplateArgumentList(ArgumentList *al,
            PerlModOutput &output,const char * /*name*/)
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
      output.addFieldQuoted("type", a->type);
    if (!a->name.isEmpty())
      output.addFieldQuoted("declaration_name", a->name)
	.addFieldQuoted("definition_name", a->name);
    if (!a->defval.isEmpty())
      output.addFieldQuoted("default", a->defval);
    output.closeHash();
  }
  output.closeList();
}

/*
static void addMemberTemplateLists(MemberDef *md,PerlModOutput &output)
{
  ClassDef *cd = md->getClassDef();
  const char *cname = cd ? cd->name().data() : 0;
  if (md->templateArguments()) // function template prefix
    addTemplateArgumentList(md->templateArguments(),output,cname);
}
*/

static void addTemplateList(ClassDef *cd,PerlModOutput &output)
{
  addTemplateArgumentList(cd->templateArguments(),output,cd->name());
}

static void addPerlModDocBlock(PerlModOutput &output,
			    const char *name,
			    const QCString &fileName,
			    int lineNr,
			    const QCString &scope,
			    MemberDef *md,
			    const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty())
  {
    output.addField(name).add("{},\n");
  }
  else 
  {
    DocNode *root = validatingParseDoc(fileName,lineNr,scope,md,stext,FALSE);
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

static void generatePerlForMember(MemberDef *md,PerlModOutput &output,Definition *)
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
  case MemberDef::EnumValue:   ASSERT(0);           break;
  case MemberDef::Property:    memType="property";  break;
  case MemberDef::Variable:    memType="variable";  break;
  case MemberDef::Typedef:     memType="typedef";   break;
  case MemberDef::Enumeration: memType="enum";      break;
  case MemberDef::Function:    memType="function";  isFunc=TRUE; break;
  case MemberDef::Signal:      memType="signal";    isFunc=TRUE; break;
  case MemberDef::Prototype:   memType="prototype"; isFunc=TRUE; break;
  case MemberDef::Friend:      memType="friend";    isFunc=TRUE; break;
  case MemberDef::DCOP:        memType="dcop";      isFunc=TRUE; break;
  case MemberDef::Slot:        memType="slot";      isFunc=TRUE; break;
  }

  QCString scopeName;
  if (md->getClassDef())
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef())
    scopeName=md->getNamespaceDef()->name();

  output.openHash()
    .addFieldQuoted("kind", memType)
    .addFieldQuoted("name", md->name())
    .addFieldQuoted("virtualness", getVirtualnessName(md->virtualness()))
    .addFieldQuoted("protection", getProtectionName(md->protection()));

  addPerlModDocBlock(output,"brief",md->getDefFileName(),md->getDefLine(),scopeName,md,md->briefDescription());
  addPerlModDocBlock(output,"detailed",md->getDefFileName(),md->getDefLine(),scopeName,md,md->documentation());
  if (md->memberType()!=MemberDef::Define &&
      md->memberType()!=MemberDef::Enumeration)
    output.addFieldQuoted("type", md->typeString());
  
  if (isFunc) //function
  {
    ArgumentList *al = md->argumentList();
    output.addFieldQuoted("const", (al && al->constSpecifier) ? "yes" : "no")
      .addFieldQuoted("volatile", (al && al->volatileSpecifier) ? "yes" : "no");
    
    output.openList("parameters");
    ArgumentList *declAl = md->declArgumentList();
    ArgumentList *defAl = md->argumentList();
    if (declAl && declAl->count()>0)
    {
      ArgumentListIterator declAli(*declAl);
      ArgumentListIterator defAli(*defAl);
      Argument *a;
      for (declAli.toFirst();(a=declAli.current());++declAli)
      {
	Argument *defArg = defAli.current();
	output.openHash();

	if (!a->name.isEmpty())
	  output.addFieldQuoted("declaration_name", a->name);

	if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
	  output.addFieldQuoted("definition_name", defArg->name);

	if (!a->type.isEmpty())
	  output.addFieldQuoted("type", a->type);

	if (!a->array.isEmpty())
	  output.addFieldQuoted("array", a->array);

	if (!a->defval.isEmpty())
	  output.addFieldQuoted("default_value", a->defval);

	if (!a->attrib.isEmpty())
	  output.addFieldQuoted("attributes", a->attrib);
	
	output.closeHash();
	if (defArg) ++defAli;
      }
    }
    output.closeList();
  }
  else if (md->memberType()==MemberDef::Define &&
	   md->argsString()!=0) // define
  {
    output.openList("parameters");
    ArgumentListIterator ali(*md->argumentList());
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      output.openHash()
	.addFieldQuoted("name", a->type)
	.closeHash();
    }
    output.closeList();
  }
  if (!md->initializer().isEmpty())
    output.addFieldQuoted("initializer", md->initializer());
  
  if (md->excpString())
    output.addFieldQuoted("exceptions", md->excpString());
  
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    if (md->enumFieldList())
    {
      output.openList("values");
      MemberListIterator emli(*md->enumFieldList());
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
	output.openHash()
	  .addFieldQuoted("name", emd->name());
	 
	if (!emd->initializer().isEmpty())
	  output.addFieldQuoted("initializer", emd->initializer());

	addPerlModDocBlock(output,"brief",emd->getDefFileName(),emd->getDefLine(),scopeName,emd,emd->briefDescription());

	addPerlModDocBlock(output,"detailed",emd->getDefFileName(),emd->getDefLine(),scopeName,emd,emd->documentation());

	output.closeHash();
      }
      output.closeList();
    }
  }

  MemberDef *rmd = md->reimplements();
  if (rmd)
    output.openHash("reimplements")
      .addFieldQuoted("name", rmd->name())
      .closeHash();

  MemberList *rbml = md->reimplementedBy();
  if (rbml)
  {
    MemberListIterator mli(*rbml);
    output.openList("reimplemented_by");
    for (mli.toFirst();(rmd=mli.current());++mli)
      output.openHash()
	.addFieldQuoted("name", rmd->name())
	.closeHash();
    output.closeList();
  }
  
  output.closeHash();
}

static void generatePerlSection(Definition *d,PerlModOutput &output,
				MemberList *ml,const char *name,const char *header=0)
{
  if (ml->count()==0) return; // empty list

  output.openHash(name);

  if (header)
    output.addFieldQuoted("header", header);
  
  output.openList("members");
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    generatePerlForMember(md,output,d);
  }
  output.closeList()
    .closeHash();
}

static void addListOfAllMembers(ClassDef *cd, PerlModOutput &output)
{
  output.openList("all_members");
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

      output.openHash()
	.addFieldQuoted("name", md->name())
	.addFieldQuoted("virtualness", getVirtualnessName(md->virtualness()))
	.addFieldQuoted("protection", getProtectionName(mi->prot));

      if (!mi->ambiguityResolutionScope.isEmpty())
	output.addFieldQuoted("ambiguity_scope", mi->ambiguityResolutionScope);

      output.addFieldQuoted("scope", cd->name())
	.closeHash();
    }
  }
  output.closeList();
}

static void generatePerlForClass(ClassDef *cd, PerlModOutput &output)
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

  output.openHash()
    .addFieldQuoted("name", cd->name());
  
  if (cd->baseClasses()->count()>0)
  {
    output.openList("base");
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
      output.openHash()
	.addFieldQuoted("name", bcd->classDef->displayName())
	.addFieldQuoted("virtualness", getVirtualnessName(bcd->virt))
	.addFieldQuoted("protection", getProtectionName(bcd->prot))
	.closeHash();
    output.closeList();
  }

  if (cd->subClasses()->count()>0)
  {
    output.openList("derived");
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
      output.openHash()
	.addFieldQuoted("name", bcd->classDef->displayName())
	.addFieldQuoted("virtualness", getVirtualnessName(bcd->virt))
	.addFieldQuoted("protection", getProtectionName(bcd->prot))
	.closeHash();
    output.closeList();
  }

  ClassSDict *cl = cd->getInnerClasses();
  if (cl)
  {
    output.openList("inner");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      output.openHash()
	.addFieldQuoted("name", cd->name())
	.closeHash();
    output.closeList();
  }

  IncludeInfo *ii=cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      output.openHash("includes");
#if 0
      if (ii->fileDef && !ii->fileDef->isReference()) // TODO: support external references
        t << " id=\"" << ii->fileDef->getOutputFileBase() << "\"";
#endif
      output.addFieldQuoted("local", ii->local ? "yes" : "no")
	.addFieldQuoted("name", nm)
	.closeHash();
    }
  }

  addTemplateList(cd,output);
  addListOfAllMembers(cd,output);
  MemberGroupSDict::Iterator mgli(*cd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
    generatePerlSection(cd,output,mg->members(),"user_defined",mg->header());

  generatePerlSection(cd,output,&cd->pubTypes,"public_typedefs");
  generatePerlSection(cd,output,&cd->pubMethods,"public_methods");
  generatePerlSection(cd,output,&cd->pubAttribs,"public_members");
  generatePerlSection(cd,output,&cd->pubSlots,"public_slots");
  generatePerlSection(cd,output,&cd->signals,"signals");
  generatePerlSection(cd,output,&cd->dcopMethods,"dcop_methods");
  generatePerlSection(cd,output,&cd->properties,"properties");
  generatePerlSection(cd,output,&cd->pubStaticMethods,"public_static_methods");
  generatePerlSection(cd,output,&cd->pubStaticAttribs,"public_static_members");
  generatePerlSection(cd,output,&cd->proTypes,"protected_typedefs");
  generatePerlSection(cd,output,&cd->proMethods,"protected_methods");
  generatePerlSection(cd,output,&cd->proAttribs,"protected_members");
  generatePerlSection(cd,output,&cd->proSlots,"protected_slots");
  generatePerlSection(cd,output,&cd->proStaticMethods,"protected_static_methods");
  generatePerlSection(cd,output,&cd->proStaticAttribs,"protected_static_members");
  generatePerlSection(cd,output,&cd->priTypes,"private_typedefs");
  generatePerlSection(cd,output,&cd->priMethods,"private_methods");
  generatePerlSection(cd,output,&cd->priAttribs,"private_members");
  generatePerlSection(cd,output,&cd->priSlots,"private_slots");
  generatePerlSection(cd,output,&cd->priStaticMethods,"private_static_methods");
  generatePerlSection(cd,output,&cd->priStaticAttribs,"private_static_members");
  generatePerlSection(cd,output,&cd->friends,"friend_methods");
  generatePerlSection(cd,output,&cd->related,"related_methods");

  addPerlModDocBlock(output,"brief",cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->briefDescription());
  addPerlModDocBlock(output,"detailed",cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->documentation());

#if 0
  DotClassGraph inheritanceGraph(cd,DotClassGraph::Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "    <inheritancegraph>" << endl;
    inheritanceGraph.writePerl(t);
    t << "    </inheritancegraph>" << endl;
  }
  DotClassGraph collaborationGraph(cd,DotClassGraph::Implementation);
  if (!collaborationGraph.isTrivial())
  {
    t << "    <collaborationgraph>" << endl;
    collaborationGraph.writePerl(t);
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

  output.closeHash();
}

static void generatePerlForNamespace(NamespaceDef *nd, PerlModOutput &output)
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

  output.openHash()
    .addFieldQuoted("name", nd->name());
  
  ClassSDict *cl = nd->classSDict;
  if (cl)
  {
    output.openList("classes");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      output.openHash()
	.addFieldQuoted("name", cd->name())
	.closeHash();
    output.closeList();
  }

  NamespaceSDict *nl = nd->namespaceSDict;
  if (nl)
  {
    output.openList("namespaces");
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
      output.openHash()
	.addFieldQuoted("name", nd->name())
	.closeHash();
    output.closeList();
  }

  MemberGroupSDict::Iterator mgli(*nd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
    generatePerlSection(nd,output,mg->members(),"user-defined",mg->header());

  generatePerlSection(nd,output,&nd->decDefineMembers,"defines");
  generatePerlSection(nd,output,&nd->decProtoMembers,"prototypes");
  generatePerlSection(nd,output,&nd->decTypedefMembers,"typedefs");
  generatePerlSection(nd,output,&nd->decEnumMembers,"enums");
  generatePerlSection(nd,output,&nd->decFuncMembers,"functions");
  generatePerlSection(nd,output,&nd->decVarMembers,"variables");

  addPerlModDocBlock(output,"brief",nd->getDefFileName(),nd->getDefLine(),0,0,nd->briefDescription());
  addPerlModDocBlock(output,"detailed",nd->getDefFileName(),nd->getDefLine(),0,0,nd->documentation());

  output.closeHash();
}

static void generatePerlForFile(FileDef *fd, PerlModOutput &output)
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

  output.openHash()
    .addFieldQuoted("name", fd->name());
  
  QListIterator<IncludeInfo> ili1(*fd->includeFileList());
  IncludeInfo *inc;
  output.openList("includes");
  for (ili1.toFirst();(inc=ili1.current());++ili1)
  {
    output.openHash()
      .addFieldQuoted("name", inc->includeName);
    if (inc->fileDef && !inc->fileDef->isReference())
    {
      output.addFieldQuoted("ref", inc->fileDef->getOutputFileBase());
    }
    output.closeHash();
  }
  output.closeList();
  
  QListIterator<IncludeInfo> ili2(*fd->includedByFileList());
  output.openList("included_by");
  for (ili2.toFirst();(inc=ili2.current());++ili2)
  {
    output.openHash()
      .addFieldQuoted("name => ", inc->includeName);
    if (inc->fileDef && !inc->fileDef->isReference())
    {
      output.addFieldQuoted("ref", inc->fileDef->getOutputFileBase());
    }
    output.closeHash();
  }
  output.closeList();
  
  generatePerlSection(fd,output,&fd->decDefineMembers,"defines");
  generatePerlSection(fd,output,&fd->decProtoMembers,"prototypes");
  generatePerlSection(fd,output,&fd->decTypedefMembers,"typedefs");
  generatePerlSection(fd,output,&fd->decEnumMembers,"enums");
  generatePerlSection(fd,output,&fd->decFuncMembers,"functions");
  generatePerlSection(fd,output,&fd->decVarMembers,"variables");

  addPerlModDocBlock(output,"brief",fd->getDefFileName(),fd->getDefLine(),0,0,fd->briefDescription());
  addPerlModDocBlock(output,"detailed",fd->getDefFileName(),fd->getDefLine(),0,0,fd->documentation());

  output.closeHash();
}

static void generatePerlForGroup(GroupDef *gd, PerlModOutput &output)
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

  output.openHash()
    .addFieldQuoted("name", gd->name())
    .addFieldQuoted("title", gd->groupTitle());

  FileList *fl = gd->getFiles();
  if (fl)
  {
    output.openList("files");
    QListIterator<FileDef> fli(*fl);
    FileDef *fd = fl->first();
    for (fli.toFirst();(fd=fli.current());++fli)
      output.openHash()
	.addFieldQuoted("name", fd->name())
	.closeHash();
    output.closeList();
  }

  ClassSDict *cl = gd->getClasses();
  if (cl)
  {
    output.openList("classes");
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
      output.openHash()
	.addFieldQuoted("name", cd->name())
	.closeHash();
    output.closeList();
  }

  NamespaceList *nl = gd->getNamespaces();
  if (nl)
  {
    output.openList("namespaces");
    NamespaceListIterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
      output.openHash()
	.addFieldQuoted("name", nd->name())
	.closeHash();
    output.closeList();
  }

  PageSDict *pl = gd->getPages();
  if (pl)
  {
    output.openList("pages");
    PageSDict::Iterator pli(*pl);
    PageInfo *pi;
    for (pli.toFirst();(pi=pli.current());++pli)
      output.openHash()
	.addFieldQuoted("title", pi->title)
	.closeHash();
    output.closeList();
  }

  GroupList *gl = gd->getSubGroups();
  if (gl)
  {
    output.openList("groups");
    GroupListIterator gli(*gl);
    GroupDef *sgd;
    for (gli.toFirst();(sgd=gli.current());++gli)
      output.openHash()
	.addFieldQuoted("title", gd->groupTitle())
	.closeHash();
    output.closeList();
  }

  MemberGroupSDict::Iterator mgli(*gd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
    generatePerlSection(gd,output,mg->members(),"user-defined",mg->header());

  generatePerlSection(gd,output,&gd->decDefineMembers,"defines");
  generatePerlSection(gd,output,&gd->decProtoMembers,"prototypes");
  generatePerlSection(gd,output,&gd->decTypedefMembers,"typedefs");
  generatePerlSection(gd,output,&gd->decEnumMembers,"enums");
  generatePerlSection(gd,output,&gd->decFuncMembers,"functions");
  generatePerlSection(gd,output,&gd->decVarMembers,"variables");

  addPerlModDocBlock(output,"brief",gd->getDefFileName(),gd->getDefLine(),0,0,gd->briefDescription());
  addPerlModDocBlock(output,"detailed",gd->getDefFileName(),gd->getDefLine(),0,0,gd->documentation());

  output.closeHash();
}

static void generatePerlForPage(PageInfo *pi, PerlModOutput &output)
{
  // + name
  // + title
  // + documentation

  if (pi->isReference()) return;

  output.openHash()
    .addFieldQuoted("name", pi->name);
    
  SectionInfo *si = Doxygen::sectionDict.find(pi->name);
  if (si)
    output.addFieldQuoted("title", si->title);

  addPerlModDocBlock(output,"detailed",pi->defFileName,pi->defLine,0,0,pi->doc);
}

void generatePerlMod()
{
  // + classes
  // + namespaces
  // + files
  // - packages
  // + groups
  // + related pages
  // - examples

  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty())
  {
    outputDirectory=QDir::currentDirPath();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
	err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",outputDirectory.data());
	exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
	err("Notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath();
  }

  QDir dir(outputDirectory);
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(outputDirectory))
    {
      err("Cannot create directory %s\n",outputDirectory.data());
      return;
    }
  }
 
  QCString fileName=outputDirectory+"/DoxyDocs.pm";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  
  QTextStream t(&f);
  PerlModOutputStream os(&t);
  PerlModOutput output(&os);
  output.add("$doxydocs =\n").openHash();
  
  output.openList("classes");
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
    generatePerlForClass(cd,output);
  output.closeList();

  output.openList("namespaces");
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
    generatePerlForNamespace(nd,output);
  output.closeList();

  output.openList("files");
  FileNameListIterator fnli(Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
      generatePerlForFile(fd,output);
  }
  output.closeList();

  output.openList("groups");
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
    generatePerlForGroup(gd,output);
  output.closeList();

  output.openList("pages");
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
    generatePerlForPage(pi,output);
  if (Doxygen::mainPage)
    generatePerlForPage(Doxygen::mainPage,output);
  output.closeList();

  output.closeHash().add(";\n1;\n");
}

// Local Variables:
// c-basic-offset: 2
// End:
