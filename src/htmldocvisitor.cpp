/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#include <qdir.h>
#include "htmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "code.h"
#include "dot.h"
#include "message.h"
#include "config.h"

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
  : DocVisitor(DocVisitor_Html), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) 
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
  startLink(w->ref(),w->file(),w->relPath(),w->anchor());
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
    case DocSymbol::Tm:      m_t << "&tm;"; break;
    case DocSymbol::Reg:     m_t << "&reg;"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Uml:     m_t << "&" << s->letter() << "uml;"; break;
    case DocSymbol::Acute:   m_t << "&" << s->letter() << "acute;"; break;
    case DocSymbol::Grave:   m_t << "&" << s->letter() << "grave;"; break;
    case DocSymbol::Circ:    m_t << "&" << s->letter() << "circ;"; break;
    case DocSymbol::Slash:   m_t << "&" << s->letter() << "slash;"; break;
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
  m_t << "<a href=\"";
  if (u->isEmail()) m_t << "mailto:";
  m_t << u->url() << "\">";
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
        m_t << "<pre" << htmlAttribsToString(s->attribs()) << ">";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "</pre>";
      }
      break;
    case DocStyleChange::Div:
      if (s->enable()) m_t << "<div" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</div>";
      break;
    case DocStyleChange::Span:
      if (s->enable()) m_t << "<span" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</span>";
      break;

  }
}


void HtmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "<pre><div class=\"fragment\">"; 
      parseCode(m_ci,s->context(),s->text().latin1(),s->isExample(),s->exampleFile());
      m_t << "</div></pre>"; 
      break;
    case DocVerbatim::Verbatim: 
      m_t << "<pre><div class=\"fragment\">";
      filter(s->text());
      m_t << "</div></pre>"; 
      break;
    case DocVerbatim::HtmlOnly: 
      m_t << s->text(); 
      break;
    case DocVerbatim::ManOnly: 
    case DocVerbatim::LatexOnly: 
    case DocVerbatim::XmlOnly: 
      /* nothing */ 
      break;
     
    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString fileName(4096);

        fileName.sprintf("%s%d", 
            (Config_getString("HTML_OUTPUT")+"/inline_dotgraph_").data(), 
            dotindex++
           );
        QFile file(fileName);
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s for writing\n",fileName.data());
        }
        file.writeBlock( s->text(), s->text().length() );
        file.close();

        m_t << "<div align=\"center\">" << endl;
        writeDotFile(fileName);
        m_t << "</div>" << endl;

        file.remove();
      }
      break;
  }
}

void HtmlDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<a class=\"anchor\" name=\"" << anc->anchor() << "\"></a>";
}

void HtmlDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::Include: 
      m_t << "<pre><div class=\"fragment\">";
      parseCode(m_ci,inc->context(),inc->text().latin1(),inc->isExample(),inc->exampleFile());
      m_t << "</div></pre>"; 
      break;
    case DocInclude::IncWithLines:
      { 
         m_t << "<pre><div class=\"fragment\">";
         QFileInfo cfi( inc->file() );
         FileDef fd( cfi.dirPath(), cfi.fileName() );
         parseCode(m_ci,inc->context(),inc->text().latin1(),inc->isExample(),inc->exampleFile(), &fd);
         m_t << "</div></pre>"; 
      }
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      m_t << inc->text(); 
      break;
    case DocInclude::VerbInclude: 
      m_t << "<pre><div class=\"fragment\">";
      filter(inc->text());
      m_t << "</div></pre>"; 
      break;
  }
}

void HtmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    if (!m_hide) m_t << "<pre><div class=\"fragment\">";
    pushEnabled();
    m_hide=TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    popEnabled();
    if (!m_hide) parseCode(m_ci,op->context(),op->text().latin1(),op->isExample(),op->exampleFile());
    pushEnabled();
    m_hide=TRUE;
  }
  if (op->isLast())  
  {
    popEnabled();
    if (!m_hide) m_t << "</div></pre>"; 
  }
  else
  {
    if (!m_hide) m_t << endl;
  }
}

void HtmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  bool bDisplay = f->text().at(0)=='\\';
  if (bDisplay) m_t << "<p class=\"formulaDsp\">" << endl;
  m_t << "<img class=\"formula" << (bDisplay ? "Dsp" : "Inl");
  m_t << "\" alt=\"";
  filterQuotedCdataAttr(f->text());
  m_t << "\"";
  /// @todo cache image dimensions on formula generation and give height/width
  /// for faster preloading and better rendering of the page
  m_t << " src=\"" << f->name() << ".png\">";
  if (bDisplay)
    m_t << endl << "<p>" << endl;
}

void HtmlDocVisitor::visit(DocIndexEntry *)
{
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void HtmlDocVisitor::visitPre(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "<ol>";
  }
  else
  {
    m_t << "<ul>";
  }
  if (!l->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPost(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "</ol>";
  }
  else
  {
    m_t << "</ul>";
  }
  if (!l->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "<li>";
}

void HtmlDocVisitor::visitPost(DocAutoListItem *) 
{
  if (m_hide) return;
  m_t << "</li>";
}

void HtmlDocVisitor::visitPre(DocPara *) 
{
  if (m_hide) return;

  // TODO:
  // Paragraph should be surrounded by <p>..</p>, but
  // 
  // A list item (li), description data (dd), or table data (td) should
  // only have paragraph markers if there are multiple paragraphs (otherwise
  // the output looks ugly).
  //
  // A list or table should be placed outside the paragraph context,
  // so the current paragraph should be ended and restarted. To avoid
  // empty paragraphs, it has to be checked if the list or table is the
  // first or last child within the paragraph.
  
}

void HtmlDocVisitor::visitPost(DocPara *p)
{
  if (m_hide) return;
  if (!p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameter sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       ) 
     ) 
  {
    m_t << "<p>\n";
  }
}

void HtmlDocVisitor::visitPre(DocRoot *)
{
}

void HtmlDocVisitor::visitPost(DocRoot *)
{
}

void HtmlDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
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
    case DocSimpleSect::Rcs: break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s->type()!=DocSimpleSect::User && s->type()!=DocSimpleSect::Rcs)
  {
    m_t << ":</b></dt><dd>";
  }
}

void HtmlDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  m_t << "</dd></dl>\n";
}

void HtmlDocVisitor::visitPre(DocTitle *)
{
}

void HtmlDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "</b></dt><dd>";
}

void HtmlDocVisitor::visitPre(DocSimpleList *sl)
{
  if (m_hide) return;
  m_t << "<ul>";
  if (!sl->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPost(DocSimpleList *sl)
{
  if (m_hide) return;
  m_t << "</ul>";
  if (!sl->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "<li>";
}

void HtmlDocVisitor::visitPost(DocSimpleListItem *li) 
{
  if (m_hide) return;
  m_t << "</li>";
  if (!li->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  m_t << "<h" << s->level()+1 << ">";
  m_t << "<a class=\"anchor\" name=\"" << s->anchor();
  m_t << "\">" << endl;
  filter(s->title());
  m_t << "</a></h" << s->level()+1 << ">\n";
}

void HtmlDocVisitor::visitPost(DocSection *) 
{
}

void HtmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "<ol" << htmlAttribsToString(s->attribs()) << ">\n"; 
  else 
    m_t << "<ul" << htmlAttribsToString(s->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlList *s) 
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "</ol>"; 
  else 
    m_t << "</ul>";
  if (!s->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPre(DocHtmlListItem *i)
{
  if (m_hide) return;
  m_t << "<li" << htmlAttribsToString(i->attribs()) << ">";
  if (!i->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPost(DocHtmlListItem *) 
{
  if (m_hide) return;
  m_t << "</li>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescList *dl)
{
  if (m_hide) return;
  m_t << "<dl" << htmlAttribsToString(dl->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlDescList *) 
{
  if (m_hide) return;
  m_t << "</dl>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescTitle *dt)
{
  if (m_hide) return;
  m_t << "<dt" << htmlAttribsToString(dt->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  if (m_hide) return;
  m_t << "</dt>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescData *dd)
{
  if (m_hide) return;
  m_t << "<dd" << htmlAttribsToString(dd->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescData *) 
{
  if (m_hide) return;
  m_t << "</dd>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;
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
  if (m_hide) return;
  m_t << "</table>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlRow *tr)
{
  if (m_hide) return;
  m_t << "<tr" << htmlAttribsToString(tr->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlRow *) 
{
  if (m_hide) return;
  m_t << "</tr>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;
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
  if (m_hide) return;
  if (c->isHeading()) m_t << "</th>"; else m_t << "</td>";
}

void HtmlDocVisitor::visitPre(DocHtmlCaption *c)
{
  if (m_hide) return;
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
  if (m_hide) return;
  m_t << "</caption>\n";
}

void HtmlDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  m_t << "<p><b>" << theTranslator->trForInternalUseOnly() << "</b></p>" << endl;
  m_t << "<p>" << endl;
}

void HtmlDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  m_t << "</p>" << endl;
}

void HtmlDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  m_t << "<a href=\"" << href->url()  << "\""
      << htmlAttribsToString(href->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHRef *) 
{
  if (m_hide) return;
  m_t << "</a>";
}

void HtmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  m_t << "<h" << header->level() 
      << htmlAttribsToString(header->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlHeader *header) 
{
  if (m_hide) return;
  m_t << "</h" << header->level() << ">\n";
}

void HtmlDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Html)
  {
    if (m_hide) return;
    QString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right(baseName.length()-i-1);
    }
    m_t << "<div align=\"center\">" << endl;
    m_t << "<img src=\"" << img->name() << "\" alt=\"" 
      << baseName << "\"" << htmlAttribsToString(img->attribs()) << ">" << endl;
    if (img->hasCaption())
    {
      m_t << "<p><strong>";
    }
  }
  else // other format -> skip
  {
    pushEnabled();
    m_hide=TRUE;
  }
}

void HtmlDocVisitor::visitPost(DocImage *img) 
{
  if (img->type()==DocImage::Html)
  {
    if (m_hide) return;
    if (img->hasCaption())
    {
      m_t << "</strong></p>";
    }
    m_t << "</div>" << endl;
  }
  else // other format
  {
    popEnabled();
  }
}

void HtmlDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  writeDotFile(df->file());
  m_t << "<div align=\"center\">" << endl;
  if (df->hasCaption())
  { 
    m_t << "<p><strong>";
  }
}

void HtmlDocVisitor::visitPost(DocDotFile *df) 
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</strong></p>" << endl;
  }
  m_t << "</div>" << endl;
}

void HtmlDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->ref(),lnk->file(),lnk->relPath(),lnk->anchor());
}

void HtmlDocVisitor::visitPost(DocLink *) 
{
  if (m_hide) return;
  endLink();
}

void HtmlDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) 
  {
    startLink(ref->ref(),ref->file(),ref->relPath(),ref->anchor());
  }
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void HtmlDocVisitor::visitPost(DocRef *ref) 
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink();
  //m_t << " ";
}

void HtmlDocVisitor::visitPre(DocSecRefItem *ref)
{
  if (m_hide) return;
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
  if (m_hide) return;
  m_t << "</a> ";
}

void HtmlDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "<multicol cols=3>" << endl;
  m_t << "<ul>" << endl;
}

void HtmlDocVisitor::visitPost(DocSecRefList *) 
{
  if (m_hide) return;
  m_t << "</ul>" << endl;
  m_t << "</multicol>" << endl;
}

void HtmlDocVisitor::visitPre(DocLanguage *l)
{
  QString langId = Config_getEnum("OUTPUT_LANGUAGE");
  if (l->id().lower()!=langId.lower())
  {
    pushEnabled();
    m_hide = TRUE;
  }
}

void HtmlDocVisitor::visitPost(DocLanguage *l) 
{
  QString langId = Config_getEnum("OUTPUT_LANGUAGE");
  if (l->id().lower()!=langId.lower())
  {
    popEnabled();
  }
}

void HtmlDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
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
  if (m_hide) return;
  m_t << "  </table>" << endl;
  m_t << "</dl>" << endl;
}

void HtmlDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  m_t << "    <tr><td>";
  if (pl->direction()!=DocParamSect::Unspecified)
  {
    m_t << "<tt>[";
    if (pl->direction()==DocParamSect::In)
    {
      m_t << "in";
    }
    else if (pl->direction()==DocParamSect::Out)
    {
      m_t << "out";
    }
    else if (pl->direction()==DocParamSect::InOut)
    {
      m_t << "in,out";
    }
    m_t << "]</tt>&nbsp;";
  }
  m_t << "</td><td valign=top><em>";
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
  if (m_hide) return;
  m_t << "</td></tr>" << endl;
}

void HtmlDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  m_t << "<dl compact><dt><b><a class=\"el\" href=\"" 
      << x->relPath() << x->file() << Doxygen::htmlFileExtension << "#" << x->anchor() << "\">";
  filter(x->title());
  m_t << ":</a></b></dt><dd>";
}

void HtmlDocVisitor::visitPost(DocXRefItem *)
{
  if (m_hide) return;
  m_t << "</dd></dl>" << endl;
}

void HtmlDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(0,ref->file(),ref->relPath(),ref->anchor());
}

void HtmlDocVisitor::visitPost(DocInternalRef *) 
{
  if (m_hide) return;
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

/// Escape basic entities to produce a valid CDATA attribute value,
/// assume that the outer quoting will be using the double quote &quot;
void HtmlDocVisitor::filterQuotedCdataAttr(const char* str)
{
  if (str==0) return;
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '&':  m_t << "&amp;"; break;
      case '"':  m_t << "&quot;"; break;
       // For SGML compliance, and given the SGML declaration for HTML syntax,
       // it's enough to replace these two, provided that the declaration
       // for the HTML version we generate (and as supported by the browser)
       // specifies that all the other symbols used in rawVal are
       // within the right charachter class (i.e., they're not
       // some multinational weird charachters not in the BASESET).
       // We assume that 1) the browser will support whatever is remaining
       // in the formula and 2) the TeX formulae are generally governed
       // by even stricter charachter restrictions so it should be enough.
       //
       // On some incompliant browsers, additional translation of
       // '>' and '<' into "&gt;" and "&lt;", respectively, might be needed;
       // but I'm unaware of particular modern (last 4 years) versions
       // with such problems, so let's not do it for performance.
       // Also, some brousers will (wrongly) not process the entity references
       // inside the attribute value and show the &...; form instead,  
       // so we won't create entites unless necessary to minimize clutter there.
       // --vassilii 
      default:   m_t << c;
    }
  }
}

void HtmlDocVisitor::startLink(const QString &ref,const QString &file,
                               const QString &relPath,const QString &anchor)
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
    if ((dest=Doxygen::tagDestinationDict[ref])) m_t << relPath << *dest << "/";
  }
  else
  {
    m_t << relPath;
  }
  if (!file.isEmpty()) m_t << file << Doxygen::htmlFileExtension;
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\">";
}

void HtmlDocVisitor::endLink()
{
  m_t << "</a>";
}

void HtmlDocVisitor::pushEnabled()
{
  m_enabled.push(new bool(m_hide));
}

void HtmlDocVisitor::popEnabled()
{
  bool *v=m_enabled.pop();
  ASSERT(v!=0);
  m_hide = *v;
  delete v;
}

void HtmlDocVisitor::writeDotFile(const QString &fileName)
{
  QString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  } 
  QString outDir = Config_getString("HTML_OUTPUT");
  writeDotGraphFromFile(fileName,outDir,baseName,BITMAP);
  QString mapName = baseName+".map";
  QString mapFile = fileName+".map";
  m_t << "<img src=\"" << baseName << "." 
    << Config_getEnum("DOT_IMAGE_FORMAT") << "\" alt=\""
    << baseName << "\" border=\"0\" usemap=\"#" << mapName << "\">" << endl;
  QString imap = getDotImageMapFromFile(fileName,outDir);
  m_t << "<map name=\"" << mapName << "\">" << imap << "</map>" << endl;
}

