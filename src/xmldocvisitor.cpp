/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include "code.h"
#include "dot.h"
#include "message.h"

XmlDocVisitor::XmlDocVisitor(QTextStream &t,BaseCodeDocInterface &ci) 
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) 
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void XmlDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void XmlDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  startLink(w->ref(),w->file(),w->anchor());
  filter(w->word());
  endLink();
}

void XmlDocVisitor::visit(DocWhiteSpace *w)
{
  if (m_hide) return;
  if (m_insidePre)
  {
    m_t << w->chars();
  }
  else
  {
    m_t << " ";
  }
}

void XmlDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  switch(s->symbol())
  {
    case DocSymbol::BSlash:  m_t << "\\"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    m_t << "&lt;"; break;
    case DocSymbol::Greater: m_t << "&gt;"; break;
    case DocSymbol::Amp:     m_t << "&amp;"; break;
    case DocSymbol::Dollar:  m_t << "$"; break;
    case DocSymbol::Hash:    m_t << "#"; break;
    case DocSymbol::Percent: m_t << "%"; break;
    case DocSymbol::Copy:    m_t << "<copy/>"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Uml:     m_t << "<umlaut char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Acute:   m_t << "<acute char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Grave:   m_t << "<grave char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Circ:    m_t << "<circ char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Tilde:   m_t << "<tilde char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Szlig:   m_t << "<szlig/>"; break;
    case DocSymbol::Cedil:   m_t << "<cedil char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Ring:    m_t << "<ring char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Nbsp:    m_t << "<nonbreakablespace/>"; break;
    default:
                             err("Error: unknown symbol found\n");
  }
}

void XmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  m_t << "<ulink url=\"" << u->url() << "\">";
  filter(u->url());
  m_t << "</ulink>";
}

void XmlDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "<linebreak/>\n";
}

void XmlDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "<hruler/>\n";
}

void XmlDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "<bold>";      else m_t << "</bold> ";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "<emphasis>";     else m_t << "</emphasis> ";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "<computeroutput>";   else m_t << "</computeroutput> ";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "<subscript>";    else m_t << "</subscript> ";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "<superscript>";    else m_t << "</superscript> ";
      break;
    case DocStyleChange::Center:
      if (s->enable()) m_t << "<center>"; else m_t << "</center> ";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "<small>";  else m_t << "</small> ";
      break;
  }
}

void XmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "<programlisting>"; 
      parseCode(m_ci,s->context(),s->text(),FALSE,0);
      m_t << "</programlisting>"; 
      break;
    case DocVerbatim::Verbatim: 
      m_t << "<preformatted>";
      filter(s->text());
      m_t << "</preformatted>"; 
      break;
    case DocVerbatim::HtmlOnly: 
      m_t << "<htmlonly>";
      filter(s->text());
      m_t << "</htmlonly>";
      break;
    case DocVerbatim::LatexOnly: 
      m_t << "<latexonly>";
      filter(s->text());
      m_t << "</latexonly>";
      break;
  }
}

void XmlDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<anchor id=\"" << anc->file() << "_1" << anc->anchor() << "\"/>";
}

void XmlDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::Include: 
      m_t << "<programlisting>";
      parseCode(m_ci,inc->context(),inc->text(),FALSE,0);
      m_t << "</programlisting>"; 
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      m_t << "<htmlonly>";
      filter(inc->text());
      m_t << "</htmlonly>";
      break;
    case DocInclude::VerbInclude: 
      m_t << "<preformatted>";
      filter(inc->text());
      m_t << "</preformatted>"; 
      break;
  }
}

void XmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    m_t << "<programlisting>";
    m_hide = TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    parseCode(m_ci,op->context(),op->text(),FALSE,0);
  }
  if (op->isLast())  
  {
    m_hide = FALSE;
    m_t << "</programlisting>"; 
  }
  else
  {
    m_t << endl;
  }
}

void XmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << "<formula id=\"" << f->id() << "\">";
  filter(f->text());
  m_t << "</formula>";
}

void XmlDocVisitor::visit(DocIndexEntry *ie)
{
  m_t << "<indexentry>"
           "<primaryie>";
  filter(ie->entry());
  m_t << "</primaryie>"
           "<secondaryie></secondaryie>"
         "</indexentry>";
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void XmlDocVisitor::visitPre(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "<orderedlist>\n";
  }
  else
  {
    m_t << "<itemizedlist>\n";
  }
}

void XmlDocVisitor::visitPost(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "</orderedlist>\n";
  }
  else
  {
    m_t << "</itemizedlist>\n";
  }
}

void XmlDocVisitor::visitPre(DocAutoListItem *)
{
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocAutoListItem *) 
{
  m_t << "</listitem>";
}

void XmlDocVisitor::visitPre(DocPara *) 
{
  m_t << "<para>";
}

void XmlDocVisitor::visitPost(DocPara *)
{
  m_t << "</para>";
}

void XmlDocVisitor::visitPre(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
}

void XmlDocVisitor::visitPost(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
}

void XmlDocVisitor::visitPre(DocSimpleSect *s)
{
  m_t << "<simplesect kind=\">";
  switch(s->type())
  {
    case DocSimpleSect::See: 
      m_t << "see"; break;
    case DocSimpleSect::Return: 
      m_t << "return"; break;
    case DocSimpleSect::Author: 
      m_t << "author"; break;
    case DocSimpleSect::Authors: 
      m_t << "authors"; break;
    case DocSimpleSect::Version: 
      m_t << "version"; break;
    case DocSimpleSect::Since: 
      m_t << "since"; break;
    case DocSimpleSect::Date: 
      m_t << "date"; break;
    case DocSimpleSect::Note: 
      m_t << "bug"; break;
    case DocSimpleSect::Warning:
      m_t << "warning"; break;
    case DocSimpleSect::Pre:
      m_t << "pre"; break;
    case DocSimpleSect::Post:
      m_t << "post"; break;
    case DocSimpleSect::Invar:
      m_t << "invariant"; break;
    case DocSimpleSect::Remark:
      m_t << "remark"; break;
    case DocSimpleSect::Attention:
      m_t << "attention"; break;
    case DocSimpleSect::User: 
      m_t << "par"; break;
    case DocSimpleSect::Unknown:  break;
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocSimpleSect *)
{
  m_t << "</simplesect>\n";
}

void XmlDocVisitor::visitPre(DocTitle *)
{
  m_t << "<title>";
}

void XmlDocVisitor::visitPost(DocTitle *)
{
  m_t << "</title>";
}

void XmlDocVisitor::visitPre(DocSimpleList *)
{
  m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocSimpleList *)
{
  m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocSimpleListItem *)
{
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocSimpleListItem *) 
{
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocSection *s)
{
  m_t << "<sect" << s->level()+1 << ">";
  filter(s->title());
  m_t << "</sect" << s->level()+1 << ">\n";
}

void XmlDocVisitor::visitPost(DocSection *) 
{
}

void XmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "<orderedlist>\n"; 
  else 
    m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlList *s) 
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "</orderedlist>\n"; 
  else 
    m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlListItem *)
{
  m_t << "<listitem>\n";
}

void XmlDocVisitor::visitPost(DocHtmlListItem *) 
{
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlPre *)
{
  m_t << "<preformatted>\n";
  m_insidePre=TRUE;
}

void XmlDocVisitor::visitPost(DocHtmlPre *) 
{
  m_insidePre=FALSE;
  m_t << "</preformatted>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescList *)
{
  m_t << "<variablelist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlDescList *) 
{
  m_t << "</variablelist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescTitle *)
{
  m_t << "<varlistentry><term>";
}

void XmlDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  m_t << "</term></varlistentry>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescData *)
{
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocHtmlDescData *) 
{
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlTable *t)
{
  m_t << "<table rows=\"" << t->numRows() 
      << "\" cols=\"" << t->numCols() << "\">" ;
}

void XmlDocVisitor::visitPost(DocHtmlTable *) 
{
  m_t << "</table>\n";
}

void XmlDocVisitor::visitPre(DocHtmlRow *)
{
  m_t << "<row>\n";
}

void XmlDocVisitor::visitPost(DocHtmlRow *) 
{
  m_t << "</row>\n";
}

void XmlDocVisitor::visitPre(DocHtmlCell *c)
{
  if (c->isHeading()) m_t << "<entry thead=\"yes\">"; else m_t << "<entry thead=\"no\">";
}

void XmlDocVisitor::visitPost(DocHtmlCell *) 
{
  m_t << "</entry>"; 
}

void XmlDocVisitor::visitPre(DocHtmlCaption *)
{
  m_t << "<caption>";
}

void XmlDocVisitor::visitPost(DocHtmlCaption *) 
{
  m_t << "</caption>\n";
}

void XmlDocVisitor::visitPre(DocInternal *)
{
  m_t << "<internal>";
}

void XmlDocVisitor::visitPost(DocInternal *) 
{
  m_t << "</internal>" << endl;
}

void XmlDocVisitor::visitPre(DocHRef *href)
{
  m_t << "<ulink url=\"" << href->url() << "\">";
}

void XmlDocVisitor::visitPost(DocHRef *) 
{
  m_t << "</ulink>";
}

void XmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  m_t << "<sect" << header->level() << ">";
}

void XmlDocVisitor::visitPost(DocHtmlHeader *header) 
{
  m_t << "</sect" << header->level() << ">\n";
}

void XmlDocVisitor::visitPre(DocImage *img)
{
  m_t << "<image type=\"";
  switch(img->type())
  {
    case DocImage::Html:  m_t << "html"; break;
    case DocImage::Latex: m_t << "latex"; break;
    case DocImage::Rtf:   m_t << "rtf"; break;
  }
  m_t << "\"";

  QCString baseName=img->name();
  int i;
  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  m_t << " name=\"" << baseName << "\"";
  if (!img->width().isEmpty())
  {
    m_t << " width=\"";
    filter(img->width());
    m_t << "\"";
  }
  else if (!img->height().isEmpty())
  {
    m_t << " height=\"";
    filter(img->height());
    m_t << "\"";
  }
  m_t << ">";
}

void XmlDocVisitor::visitPost(DocImage *) 
{
  m_t << "</image>" << endl;
}

void XmlDocVisitor::visitPre(DocDotFile *df)
{
  m_t << "<dotfile name=\"" << df->file() << "\">";
}

void XmlDocVisitor::visitPost(DocDotFile *) 
{
  m_t << "</dotfile>" << endl;
}

void XmlDocVisitor::visitPre(DocLink *lnk)
{
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void XmlDocVisitor::visitPost(DocLink *) 
{
  endLink();
}

void XmlDocVisitor::visitPre(DocRef *ref)
{
  startLink(ref->ref(),ref->file(),ref->anchor());
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void XmlDocVisitor::visitPost(DocRef *) 
{
  endLink();
  m_t << " ";
}

void XmlDocVisitor::visitPre(DocSecRefItem *ref)
{
  m_t << "<tocitem id=\"" << ref->file() << "_1" << ref->anchor() << "\">";
}

void XmlDocVisitor::visitPost(DocSecRefItem *) 
{
  m_t << "</tocitem>" << endl;
}

void XmlDocVisitor::visitPre(DocSecRefList *)
{
  m_t << "<toclist>" << endl;
}

void XmlDocVisitor::visitPost(DocSecRefList *) 
{
  m_t << "</toclist>" << endl;
}

void XmlDocVisitor::visitPre(DocLanguage *l)
{
  m_t << "<language id=\"" << l->id() << "\">";
}

void XmlDocVisitor::visitPost(DocLanguage *) 
{
  m_t << "</language>" << endl;
}

void XmlDocVisitor::visitPre(DocParamSect *s)
{
  m_t << "<parameterlist kind=\"";
  switch(s->type())
  {
    case DocParamSect::Param: 
      m_t << "param"; break;
    case DocParamSect::RetVal: 
      m_t << "retval"; break;
    case DocParamSect::Exception: 
      m_t << "exception"; break;
    default:
      ASSERT(0);
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocParamSect *)
{
  m_t << "</parameterlist>" << endl;
}

void XmlDocVisitor::visitPre(DocParamList *pl)
{
  QStrListIterator li(pl->parameters());
  const char *s;
  for (li.toFirst();(s=li.current());++li)
  {
    m_t << "<parametername>";
    filter(s);
    m_t << "</parametername>" << endl;
  }
  m_t << "<parameterdescription>" << endl;
}

void XmlDocVisitor::visitPost(DocParamList *)
{
  m_t << "</parameterdescription>";
}

void XmlDocVisitor::visitPre(DocXRefItem *x)
{
  m_t << "<xrefsect id=\"";
  m_t << x->file() << "_1" << x->anchor();
  m_t << "\">";
  m_t << "<xreftitle>";
  filter(x->title());
  m_t << "</xreftitle>";
  m_t << "<xrefdescription>";
}

void XmlDocVisitor::visitPost(DocXRefItem *)
{
  m_t << "</xrefdescription>";
  m_t << "</xrefsect>";
}

void XmlDocVisitor::visitPre(DocInternalRef *ref)
{
  startLink(0,ref->file(),ref->anchor());
}

void XmlDocVisitor::visitPost(DocInternalRef *) 
{
  endLink();
  m_t << " ";
}

void XmlDocVisitor::visitPre(DocCopy *)
{
}

void XmlDocVisitor::visitPost(DocCopy *)
{
}

void XmlDocVisitor::filter(const char *str)
{ 
  if (str==0) return;
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '<':  m_t << "&lt;"; break;
      case '>':  m_t << "&gt;"; break;
      case '&':  m_t << "&amp;"; break;
      default:   m_t << c;
    }
  }
}

void XmlDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
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

