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

#include "htmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "code.h"
#include "dot.h"
#include "message.h"

static QString htmlAttribsToString(const HtmlAttribList &attribs)
{
  QString result;
  HtmlAttribListIterator li(attribs);
  HtmlAttrib *att;
  for (li.toFirst();(att=li.current());++li)
  {
    result+=" ";
    result+=att->name;
    if (!att->value.isEmpty()) result+="=\""+att->value+"\"";
  }
  return result;
}

//-------------------------------------------------------------------------

HtmlDocVisitor::HtmlDocVisitor(QTextStream &t,BaseCodeDocInterface &ci) 
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) 
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void HtmlDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void HtmlDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  startLink(w->ref(),w->file(),w->anchor());
  filter(w->word());
  endLink();
}

void HtmlDocVisitor::visit(DocWhiteSpace *w)
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

void HtmlDocVisitor::visit(DocSymbol *s)
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
    case DocSymbol::Copy:    m_t << "&copy;"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Uml:     m_t << "&" << s->letter() << "uml;"; break;
    case DocSymbol::Acute:   m_t << "&" << s->letter() << "acute;"; break;
    case DocSymbol::Grave:   m_t << "&" << s->letter() << "grave;"; break;
    case DocSymbol::Circ:    m_t << "&" << s->letter() << "circ;"; break;
    case DocSymbol::Tilde:   m_t << "&" << s->letter() << "tilde;"; break;
    case DocSymbol::Szlig:   m_t << "&szlig;"; break;
    case DocSymbol::Cedil:   m_t << "&" << s->letter() << "cedil;"; break;
    case DocSymbol::Ring:    m_t << "&" << s->letter() << "ring;"; break;
    case DocSymbol::Nbsp:    m_t << "&nbsp;"; break;
    default:
                             err("Error: unknown symbol found\n");
  }
}

void HtmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  m_t << "<a href=\"" << u->url() << "\">";
  filter(u->url());
  m_t << "</a>";
}

void HtmlDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "<br>\n";
}

void HtmlDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "<hr>\n";
}

void HtmlDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "<b" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</b>";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "<em" << htmlAttribsToString(s->attribs()) << ">";     else m_t << "</em>";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "<code" << htmlAttribsToString(s->attribs()) << ">";   else m_t << "</code>";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "<sub" << htmlAttribsToString(s->attribs()) << ">";    else m_t << "</sub>";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "<sup" << htmlAttribsToString(s->attribs()) << ">";    else m_t << "</sup>";
      break;
    case DocStyleChange::Center:
      if (s->enable()) m_t << "<center" << htmlAttribsToString(s->attribs()) << ">"; else m_t << "</center>";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "<small" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</small>";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        m_t << "<pre" << htmlAttribsToString(s->attribs()) << ">\n";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "</pre>\n";
      }
  }
}

void HtmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "<div class=\"fragment\"><pre>"; 
      parseCode(m_ci,s->context(),s->text().latin1(),s->isExample(),s->exampleFile());
      m_t << "</pre></div>"; 
      break;
    case DocVerbatim::Verbatim: 
      m_t << "<div class=\"fragment\"><pre>";
      filter(s->text());
      m_t << "</pre></div>"; 
      break;
    case DocVerbatim::HtmlOnly: 
      m_t << s->text(); 
      break;
    case DocVerbatim::LatexOnly: 
      /* nothing */ 
      break;
  }
}

void HtmlDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<a name=\"" /*<< anc->file() << "#"*/ << anc->anchor() << "\"/></a>";
}

void HtmlDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::Include: 
      m_t << "<div class=\"fragment\"><pre>";
      parseCode(m_ci,inc->context(),inc->text().latin1(),inc->isExample(),inc->exampleFile());
      m_t << "</pre></div>"; 
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      m_t << inc->text(); 
      break;
    case DocInclude::VerbInclude: 
      m_t << "<div class=\"fragment\"><pre>";
      filter(inc->text());
      m_t << "</pre></div>"; 
      break;
  }
}

void HtmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    m_t << "<div class=\"fragment\"><pre>";
    m_hide = TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    parseCode(m_ci,op->context(),op->text().latin1(),op->isExample(),op->exampleFile());
  }
  if (op->isLast())  
  {
    m_hide = FALSE;
    m_t << "</pre></div>"; 
  }
  else
  {
    m_t << endl;
  }
}

void HtmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  if (f->text().at(0)=='\\') m_t << "<p><center>" << endl;
  m_t << "<img align=";
#if !defined(_WIN32)
  m_t << "\"top\"";     // assume Unix users use Netscape 4.x which does
  // not seem to support align == "middle" :-((
#else
  m_t << "\"middle\"";  // assume Windows users use IE or HtmlHelp which on
  // displays formulas nicely with align == "middle" 
#endif
  m_t << " src=\"" << f->name() << ".png\">";
  if (f->text().at(0)=='\\') 
    m_t << endl << "</center><p>" << endl;
  else
    m_t << " ";
}

void HtmlDocVisitor::visit(DocIndexEntry *)
{
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void HtmlDocVisitor::visitPre(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "<ol>\n";
  }
  else
  {
    m_t << "<ul>\n";
  }
}

void HtmlDocVisitor::visitPost(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "</ol>\n";
  }
  else
  {
    m_t << "</ul>\n";
  }
}

void HtmlDocVisitor::visitPre(DocAutoListItem *)
{
  m_t << "<li>";
}

void HtmlDocVisitor::visitPost(DocAutoListItem *) 
{
  m_t << "</li>";
}

void HtmlDocVisitor::visitPre(DocPara *) 
{
}

void HtmlDocVisitor::visitPost(DocPara *p)
{
  if (!p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameter sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       )
     ) m_t << "\n<p>\n";
}

void HtmlDocVisitor::visitPre(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
}

void HtmlDocVisitor::visitPost(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
}

void HtmlDocVisitor::visitPre(DocSimpleSect *s)
{
  m_t << "<dl compact><dt><b>";
  switch(s->type())
  {
    case DocSimpleSect::See: 
      m_t << theTranslator->trSeeAlso(); break;
    case DocSimpleSect::Return: 
      m_t << theTranslator->trReturns(); break;
    case DocSimpleSect::Author: 
      m_t << theTranslator->trAuthor(TRUE,TRUE); break;
    case DocSimpleSect::Authors: 
      m_t << theTranslator->trAuthor(TRUE,FALSE); break;
    case DocSimpleSect::Version: 
      m_t << theTranslator->trVersion(); break;
    case DocSimpleSect::Since: 
      m_t << theTranslator->trSince(); break;
    case DocSimpleSect::Date: 
      m_t << theTranslator->trDate(); break;
    case DocSimpleSect::Note: 
      m_t << theTranslator->trNote(); break;
    case DocSimpleSect::Warning:
      m_t << theTranslator->trWarning(); break;
    case DocSimpleSect::Pre:
      m_t << theTranslator->trPrecondition(); break;
    case DocSimpleSect::Post:
      m_t << theTranslator->trPostcondition(); break;
    case DocSimpleSect::Invar:
      m_t << theTranslator->trInvariant(); break;
    case DocSimpleSect::Remark:
      m_t << theTranslator->trRemarks(); break;
    case DocSimpleSect::Attention:
      m_t << theTranslator->trAttention(); break;
    case DocSimpleSect::User: break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s->type()!=DocSimpleSect::User)
  {
    m_t << ":</b></dt><dd>";
  }
}

void HtmlDocVisitor::visitPost(DocSimpleSect *)
{
  m_t << "</dd></dl>\n";
}

void HtmlDocVisitor::visitPre(DocTitle *)
{
}

void HtmlDocVisitor::visitPost(DocTitle *)
{
  m_t << "</b></dt><dd>";
}

void HtmlDocVisitor::visitPre(DocSimpleList *)
{
  m_t << "<ul>\n";
}

void HtmlDocVisitor::visitPost(DocSimpleList *)
{
  m_t << "</ul>\n";
}

void HtmlDocVisitor::visitPre(DocSimpleListItem *)
{
  m_t << "<li>";
}

void HtmlDocVisitor::visitPost(DocSimpleListItem *) 
{
  m_t << "</li>\n";
}

void HtmlDocVisitor::visitPre(DocSection *s)
{
  m_t << "<h" << s->level()+1 << ">";
  m_t << "<a name=\"" << s->anchor();
  m_t << "\"></a>" << endl;
  filter(s->title());
  m_t << "</h" << s->level()+1 << ">\n";
}

void HtmlDocVisitor::visitPost(DocSection *) 
{
}

void HtmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "<ol" << htmlAttribsToString(s->attribs()) << ">\n"; 
  else 
    m_t << "<ul" << htmlAttribsToString(s->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlList *s) 
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "</ol>\n"; 
  else 
    m_t << "</ul>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlListItem *i)
{
  m_t << "<li" << htmlAttribsToString(i->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlListItem *) 
{
  m_t << "</li>\n";
}

//void HtmlDocVisitor::visitPre(DocHtmlPre *p)
//{
//  m_t << "<pre" << htmlAttribsToString(p->attribs()) << ">\n";
//  m_insidePre=TRUE;
//}

//void HtmlDocVisitor::visitPost(DocHtmlPre *) 
//{
//  m_insidePre=FALSE;
//  m_t << "</pre>\n";
//}

void HtmlDocVisitor::visitPre(DocHtmlDescList *dl)
{
  m_t << "<dl" << htmlAttribsToString(dl->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlDescList *) 
{
  m_t << "</dl>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescTitle *dt)
{
  m_t << "<dt" << htmlAttribsToString(dt->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  m_t << "</dt>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescData *dd)
{
  m_t << "<dd" << htmlAttribsToString(dd->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescData *) 
{
  m_t << "</dd>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlTable *t)
{
  bool hasBorder      = FALSE;
  bool hasCellSpacing = FALSE;
  bool hasCellPadding = FALSE;

  HtmlAttribListIterator li(t->attribs());
  HtmlAttrib *att;
  for (li.toFirst();(att=li.current());++li)
  {
    if      (att->name=="border")      hasBorder=TRUE;
    else if (att->name=="cellspacing") hasCellSpacing=TRUE;
    else if (att->name=="cellpadding") hasCellPadding=TRUE;
  }
  m_t << "<table" << htmlAttribsToString(t->attribs());
  if (!hasBorder)      m_t << " border=\"1\"";
  if (!hasCellSpacing) m_t << " cellspacing=\"3\"";
  if (!hasCellPadding) m_t << " cellpadding=\"3\"";
  m_t << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlTable *) 
{
  m_t << "</table>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlRow *tr)
{
  m_t << "<tr" << htmlAttribsToString(tr->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlRow *) 
{
  m_t << "</tr>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlCell *c)
{
  if (c->isHeading()) 
  {
    m_t << "<th" << htmlAttribsToString(c->attribs()) << ">"; 
  }
  else 
  {
    m_t << "<td" << htmlAttribsToString(c->attribs()) << ">";
  }
}

void HtmlDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (c->isHeading()) m_t << "</th>"; else m_t << "</td>";
}

void HtmlDocVisitor::visitPre(DocHtmlCaption *c)
{
  bool hasAlign      = FALSE;
  HtmlAttribListIterator li(c->attribs());
  HtmlAttrib *att;
  for (li.toFirst();(att=li.current());++li)
  {
    if (att->name=="align") hasAlign=TRUE;
  }
  m_t << "<caption" << htmlAttribsToString(c->attribs());
  if (!hasAlign) m_t << " align=\"bottom\"";
  m_t << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlCaption *) 
{
  m_t << "</caption>\n";
}

void HtmlDocVisitor::visitPre(DocInternal *)
{
  m_t << "<p><b>" << theTranslator->trForInternalUseOnly() << "</b></p>" << endl;
  m_t << "<p>" << endl;
}

void HtmlDocVisitor::visitPost(DocInternal *) 
{
  m_t << "</p>" << endl;
}

void HtmlDocVisitor::visitPre(DocHRef *href)
{
  m_t << "<a href=\"" << href->url() << "\">";
}

void HtmlDocVisitor::visitPost(DocHRef *) 
{
  m_t << "</a>";
}

void HtmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  m_t << "<h" << header->level() 
      << htmlAttribsToString(header->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlHeader *header) 
{
  m_t << "</h" << header->level() << ">\n";
}

void HtmlDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Html)
  {
    QString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right(baseName.length()-i-1);
    }
    m_t << "<div align=\"center\">" << endl;
    m_t << "<img src=\"" << img->name() << "\" alt=\"" 
      << baseName << "\">" << endl;
    if (img->hasCaption())
    {
      m_t << "<p><strong>";
    }
  }
  else // other format -> skip
  {
    m_hide=TRUE;
  }
}

void HtmlDocVisitor::visitPost(DocImage *img) 
{
  if (img->type()==DocImage::Html)
  {
    if (img->hasCaption())
    {
      m_t << "</strong></p>";
    }
    m_t << "</div>" << endl;
  }
  else // other format
  {
    m_hide=FALSE;
  }
}

void HtmlDocVisitor::visitPre(DocDotFile *df)
{
  QString baseName=df->file();
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  } 
  QString outDir = Config_getString("HTML_OUTPUT");
  writeDotGraphFromFile(df->file(),outDir,baseName,BITMAP);
  m_t << "<div align=\"center\">" << endl;
  m_t << "<img src=\"" << baseName << "." 
    << Config_getEnum("DOT_IMAGE_FORMAT") << "\" alt=\""
    << baseName << "\">" << endl;
  if (df->hasCaption())
  { 
    m_t << "<p><strong>";
  }
}

void HtmlDocVisitor::visitPost(DocDotFile *df) 
{
  if (df->hasCaption())
  {
    m_t << "</strong></p>" << endl;
  }
  m_t << "</div>" << endl;
}

void HtmlDocVisitor::visitPre(DocLink *lnk)
{
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void HtmlDocVisitor::visitPost(DocLink *) 
{
  endLink();
}

void HtmlDocVisitor::visitPre(DocRef *ref)
{
  startLink(ref->ref(),ref->file(),ref->anchor());
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void HtmlDocVisitor::visitPost(DocRef *) 
{
  endLink();
  m_t << " ";
}

void HtmlDocVisitor::visitPre(DocSecRefItem *ref)
{
  QString refName=ref->file();
  if (refName.right(Doxygen::htmlFileExtension.length())!=
      QString(Doxygen::htmlFileExtension))
  {
    refName+=Doxygen::htmlFileExtension;
  }
  m_t << "<li><a href=\"" << refName << "#" << ref->anchor() << "\">";

}

void HtmlDocVisitor::visitPost(DocSecRefItem *) 
{
  m_t << "</a> ";
}

void HtmlDocVisitor::visitPre(DocSecRefList *)
{
  m_t << "<multicol cols=3>" << endl;
  m_t << "<ul>" << endl;
}

void HtmlDocVisitor::visitPost(DocSecRefList *) 
{
  m_t << "</ul>" << endl;
  m_t << "</multicol>" << endl;
}

void HtmlDocVisitor::visitPre(DocLanguage *)
{
}

void HtmlDocVisitor::visitPost(DocLanguage *) 
{
}

void HtmlDocVisitor::visitPre(DocParamSect *s)
{
  m_t << "<dl compact><dt><b>";
  switch(s->type())
  {
    case DocParamSect::Param: 
      m_t << theTranslator->trParameters(); break;
    case DocParamSect::RetVal: 
      m_t << theTranslator->trReturnValues(); break;
    case DocParamSect::Exception: 
      m_t << theTranslator->trExceptions(); break;
    default:
      ASSERT(0);
  }
  m_t << ":";
  m_t << "</b></dt><dd>" << endl;
  m_t << "  <table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">" << endl;
}

void HtmlDocVisitor::visitPost(DocParamSect *)
{
  m_t << "  </table>" << endl;
  m_t << "</dl>" << endl;
}

void HtmlDocVisitor::visitPre(DocParamList *pl)
{
  m_t << "    <tr><td valign=top><em>";
  QStrListIterator li(pl->parameters());
  const char *s;
  bool first=TRUE;
  for (li.toFirst();(s=li.current());++li)
  {
    if (!first) m_t << ","; else first=FALSE;
    filter(s);
  }
  m_t << "</em>&nbsp;</td><td>";
}

void HtmlDocVisitor::visitPost(DocParamList *)
{
  m_t << "</td></tr>" << endl;
}

void HtmlDocVisitor::visitPre(DocXRefItem *x)
{
  m_t << "<dl compact><dt><b><a class=\"el\" href=\"" 
    << x->file() << Doxygen::htmlFileExtension << "#" << x->anchor() << "\">";
  filter(x->title());
  m_t << ":</a></b></dt><dd>";
}

void HtmlDocVisitor::visitPost(DocXRefItem *)
{
  m_t << "</dd></dl>" << endl;
}

void HtmlDocVisitor::visitPre(DocInternalRef *ref)
{
  startLink(0,ref->file(),ref->anchor());
}

void HtmlDocVisitor::visitPost(DocInternalRef *) 
{
  endLink();
  m_t << " ";
}

void HtmlDocVisitor::visitPre(DocCopy *)
{
}

void HtmlDocVisitor::visitPost(DocCopy *)
{
}

void HtmlDocVisitor::visitPre(DocText *)
{
}

void HtmlDocVisitor::visitPost(DocText *)
{
}

void HtmlDocVisitor::filter(const char *str)
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

void HtmlDocVisitor::startLink(const QString &ref,const QString &file,const QString &anchor)
{
  QCString *dest;
  if (!ref.isEmpty()) // link to entity imported via tag file
  {
    m_t << "<a class=\"elRef\" ";
    m_t << "doxygen=\"" << ref << ":";
    if ((dest=Doxygen::tagDestinationDict[ref])) m_t << *dest << "/";
    m_t << "\" ";
  }
  else // local link
  {
    m_t << "<a class=\"el\" ";
  }
  m_t << "href=\"";
  if (!ref.isEmpty())
  {
    if ((dest=Doxygen::tagDestinationDict[ref])) m_t << *dest << "/";
  }
  if (!file.isEmpty()) m_t << file << Doxygen::htmlFileExtension;
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\">";
}

void HtmlDocVisitor::endLink()
{
  m_t << "</a>";
}

