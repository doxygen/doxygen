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

#ifndef _HTMLDOCVISITOR_H
#define _HTMLDOCVISITOR_H

#include "docvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "code.h"

/*! Concrete visitor implementation for pretty printing */
class HtmlDocVisitor : public DocVisitor
{
  public:
    HtmlDocVisitor(QTextStream &t,BaseCodeDocInterface &ci) 
      : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) {}
    
    //--------------------------------------
    
    void visit(DocWord *w)
    {
      if (m_hide) return;
      filter(w->word());
    }
    void visit(DocLinkedWord *w)
    {
      if (m_hide) return;
      startLink(w->ref(),w->file(),w->anchor());
      filter(w->word());
      endLink();
    }
    void visit(DocWhiteSpace *w)
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
    void visit(DocSymbol *s)
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
	case DocSymbol::Cedil:   m_t << "&" << s->letter() << "cedul;"; break;
	case DocSymbol::Ring:    m_t << "&" << s->letter() << "ring;"; break;
	case DocSymbol::Nbsp:    m_t << "&nbsp;"; break;
	default:
	  printf("Error: unknown symbol found\n");
      }
    }
    void visit(DocURL *u)
    {
      if (m_hide) return;
      m_t << "<a href=\"" << u->url() << "\">" << u->url() << "</a> ";
    }
    void visit(DocLineBreak *)
    {
      if (m_hide) return;
      m_t << "<br>\n";
    }
    void visit(DocHorRuler *)
    {
      if (m_hide) return;
      m_t << "<hr>\n";
    }
    void visit(DocStyleChange *s)
    {
      if (m_hide) return;
      switch (s->style())
      {
        case DocStyleChange::Bold:
         if (s->enable()) m_t << "<b>";      else m_t << "</b> ";
         break;
        case DocStyleChange::Italic:
         if (s->enable()) m_t << "<em>";     else m_t << "</em> ";
         break;
        case DocStyleChange::Code:
         if (s->enable()) m_t << "<code>";   else m_t << "</code> ";
         break;
        case DocStyleChange::Subscript:
         if (s->enable()) m_t << "<sub>";    else m_t << "</sub> ";
         break;
        case DocStyleChange::Superscript:
         if (s->enable()) m_t << "<sup>";    else m_t << "</sup> ";
         break;
        case DocStyleChange::Center:
         if (s->enable()) m_t << "<center>"; else m_t << "</center> ";
         break;
        case DocStyleChange::Small:
         if (s->enable()) m_t << "<small>";  else m_t << "</small> ";
         break;
      }
    }
    void visit(DocVerbatim *s)
    {
      if (m_hide) return;
      switch(s->type())
      {
        case DocVerbatim::Code: // fall though
           m_t << "<div class=\"fragment\"><pre>"; 
           parseCode(m_ci,s->context(),s->text(),FALSE,0);
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
    void visit(DocAnchor *)
    {
      if (m_hide) return;
      m_t << "<a name=\"%s\"/></a>";
    }
    void visit(DocCopy *c)
    {
      if (m_hide) return;
      // TODO
      printf("<copy link=\"%s\"/>",c->link().data());
    }
    void visit(DocInclude *inc)
    {
      if (m_hide) return;
      // TODO
      printf("<include file=\"%s\" type=\"",inc->file().data());
      switch(inc->type())
      {
        case DocInclude::Include: printf("include"); break;
        case DocInclude::DontInclude: printf("dontinclude"); break;
        case DocInclude::HtmlInclude: printf("htmlinclude"); break;
        case DocInclude::VerbInclude: printf("verbinclude"); break;
      }
      printf("\"/>");
    }
    void visit(DocIncOperator *op)
    {
      if (m_hide) return;
      // TODO
      printf("<incoperator pattern=\"%s\" type=\"",op->pattern().data());
      switch(op->type())
      {
        case DocIncOperator::Line:     printf("line");     break;
        case DocIncOperator::Skip:     printf("skip");     break;
        case DocIncOperator::SkipLine: printf("skipline"); break;
        case DocIncOperator::Until:    printf("until");    break;
      }
      printf("\"/>");
    }
    void visit(DocFormula *f)
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
      m_t << " src=\"" << f->name() << ".png\">" << endl;
      if (f->text().at(0)=='\\') m_t << "</center><p>" << endl;
    }
    //--------------------------------------
    
    void visitPre(DocAutoList *l)
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
    void visitPost(DocAutoList *l)
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
    void visitPre(DocAutoListItem *)
    {
      m_t << "<li>";
    }
    void visitPost(DocAutoListItem *) 
    {
      m_t << "</li>";
    }
    void visitPre(DocPara *) 
    {
    }
    void visitPost(DocPara *p)
    {
      if (!p->isLast() &&            // omit <p> for last paragraph
          !(p->parent() &&           // and for parameter sections
            p->parent()->kind()==DocNode::Kind_ParamSect
           )
         ) m_t << "<p>\n";
    }
    void visitPre(DocRoot *)
    {
      m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
    }
    void visitPost(DocRoot *)
    {
      m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
    }
    void visitPre(DocSimpleSect *s)
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
      m_t << ":";

      // special case 1: user defined title
      if (s->type()!=DocSimpleSect::User)
      {
        m_t << "</b></dt><dd>";
      }
    }
    void visitPost(DocSimpleSect *)
    {
      m_t << "</dd></dl>\n";
    }
    void visitPre(DocTitle *)
    {
    }
    void visitPost(DocTitle *)
    {
      m_t << "</b></dt><dd>";
    }
    void visitPre(DocSimpleList *)
    {
      m_t << "<ul>\n";
    }
    void visitPost(DocSimpleList *)
    {
      m_t << "</ul>\n";
    }
    void visitPre(DocSimpleListItem *)
    {
      m_t << "<li>";
    }
    void visitPost(DocSimpleListItem *) 
    {
      m_t << "</li>\n";
    }
    void visitPre(DocSection *s)
    {
      m_t << "<h%d>",s->level();
    }
    void visitPost(DocSection *s) 
    {
      m_t << "</h%d>\n",s->level();
    }
    void visitPre(DocHtmlList *s)
    {
      if (s->type()==DocHtmlList::Ordered) 
        m_t << "<ol>\n"; 
      else 
        m_t << "<ul>\n";
    }
    void visitPost(DocHtmlList *s) 
    {
      if (s->type()==DocHtmlList::Ordered) 
        m_t << "</ol>\n"; 
      else 
        m_t << "</ul>\n";
    }
    void visitPre(DocHtmlListItem *)
    {
      m_t << "<li>\n";
    }
    void visitPost(DocHtmlListItem *) 
    {
      m_t << "</li>\n";
    }
    void visitPre(DocHtmlPre *)
    {
      m_t << "<pre>\n";
      m_insidePre=TRUE;
    }
    void visitPost(DocHtmlPre *) 
    {
      m_insidePre=FALSE;
      m_t << "</pre>\n";
    }
    void visitPre(DocHtmlDescList *)
    {
      m_t << "<dl>\n";
    }
    void visitPost(DocHtmlDescList *) 
    {
      m_t << "</dl>\n";
    }
    void visitPre(DocHtmlDescTitle *)
    {
      m_t << "<dt>";
    }
    void visitPost(DocHtmlDescTitle *) 
    {
      m_t << "</dt>\n";
    }
    void visitPre(DocHtmlDescData *)
    {
      m_t << "<dd>";
    }
    void visitPost(DocHtmlDescData *) 
    {
      m_t << "</dd>\n";
    }
    void visitPre(DocHtmlTable *)
    {
      m_t << "<table border=\"1\" cellspacing=\"3\" cellpadding=\"3\">\n";
    }
    void visitPost(DocHtmlTable *) 
    {
      m_t << "</table>\n";
    }
    void visitPre(DocHtmlRow *)
    {
      m_t << "<tr>\n";
    }
    void visitPost(DocHtmlRow *) 
    {
      m_t << "</tr>\n";
    }
    void visitPre(DocHtmlCell *c)
    {
      if (c->isHeading()) m_t << "<th>"; else m_t << "<td>";
    }
    void visitPost(DocHtmlCell *c) 
    {
      if (c->isHeading()) m_t << "</th>"; else m_t << "</td>";
    }
    void visitPre(DocHtmlCaption *)
    {
      m_t << "<caption align=\"bottom\">";
    }
    void visitPost(DocHtmlCaption *) 
    {
      m_t << "</caption>\n";
    }
    void visitPre(DocIndexEntry *)
    {
      m_hide = TRUE;
    }
    void visitPost(DocIndexEntry *) 
    {
      m_hide = FALSE;
    }
    void visitPre(DocInternal *)
    {
      m_t << "<p><b>" << theTranslator->trForInternalUseOnly() << "</b></p>" << endl;
      m_t << "<p>" << endl;
    }
    void visitPost(DocInternal *) 
    {
      m_t << "</p>" << endl;
    }
    void visitPre(DocHRef *href)
    {
      m_t << "<a href=\"" << href->url() << "\">";
    }
    void visitPost(DocHRef *) 
    {
      m_t << "</a>\n";
    }
    void visitPre(DocHtmlHeader *header)
    {
      m_t << "<h" << header->level() << ">";
    }
    void visitPost(DocHtmlHeader *header) 
    {
      m_t << "</h" << header->level() << ">\n";
    }
    void visitPre(DocImage *img)
    {
      m_t << "<img src=\"" << img->name() << "\">\n";
    }
    void visitPost(DocImage *) 
    {
    }
    void visitPre(DocDotFile *)
    {
      // TODO
    }
    void visitPost(DocDotFile *) 
    {
      // TODO
    }
    void visitPre(DocLink *lnk)
    {
      startLink(lnk->ref(),lnk->file(),lnk->anchor());
    }
    void visitPost(DocLink *) 
    {
      endLink();
    }
    void visitPre(DocRef *ref)
    {
      startLink(ref->ref(),ref->file(),ref->anchor());
      if (!ref->hasLinkText()) filter(ref->targetTitle());
    }
    void visitPost(DocRef *) 
    {
      endLink();
    }
    void visitPre(DocSecRefItem *)
    {
      // TODO
    }
    void visitPost(DocSecRefItem *) 
    {
      // TODO
    }
    void visitPre(DocSecRefList *)
    {
      // TODO
    }
    void visitPost(DocSecRefList *) 
    {
      // TODO
    }
    void visitPre(DocLanguage *)
    {
      // TODO
    }
    void visitPost(DocLanguage *) 
    {
      // TODO
    }
    void visitPre(DocParamSect *s)
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
      }
      m_t << ":";
      m_t << "</b></dt><dd>" << endl;
      m_t << "  <table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">" << endl;
    }
    void visitPost(DocParamSect *)
    {
      m_t << "  </table>" << endl;
      m_t << "</dl>" << endl;
    }
    void visitPre(DocParamList *pl)
    {
      m_t << "    <tr><td valign=top><em>";
      QStrListIterator li(pl->parameters());
      const char *s;
      bool first=TRUE;
      for (li.toFirst();(s=li.current());++li)
      {
        if (!first) m_t << ","; else first=FALSE;
        m_t << s;
      }
      m_t << "</em>&nbsp;</td><td>";
    }
    void visitPost(DocParamList *)
    {
      m_t << "</td></tr>" << endl;
    }
    void visitPre(DocXRefItem *x)
    {
      m_t << "<dl compact><dt><b><a class=\"el\" href=\"" 
          << x->file() << Doxygen::htmlFileExtension << "#" << x->anchor() << "\">"
          << x->title() << ":</a></b></dt><dd>";
    }
    void visitPost(DocXRefItem *)
    {
      m_t << "</dd></dl>" << endl;
    }
    void visitPre(DocInternalRef *ref)
    {
      startLink(0,ref->file(),ref->anchor());
    }
    void visitPost(DocInternalRef *) 
    {
      endLink();
    }

  private:
    void filter(const char *str)
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

    void startLink(const QCString &ref,const QCString &file,const QCString &anchor)
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
    void endLink()
    {
      m_t << "</a> ";
    }
    QTextStream &m_t;
    BaseCodeDocInterface &m_ci;
    bool m_insidePre;
    bool m_hide;
};

#endif
