/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef _PRINTDOCVISITOR_H
#define _PRINTDOCVISITOR_H

#include "docvisitor.h"

/*! Concrete visitor implementation for pretty printing */
class PrintDocVisitor : public DocVisitor
{
  public:
    PrintDocVisitor() : DocVisitor(DocVisitor_Other), m_indent(0), 
                        m_needsEnter(FALSE), m_insidePre(FALSE) {}
    
    //--------------------------------------
    
    void visit(DocWord *w)
    {
      indent_leaf();
      printf("%s",w->word().data());
    }
    void visit(DocLinkedWord *w)
    {
      indent_leaf();
      printf("%s",w->word().data());
    }
    void visit(DocWhiteSpace *w)
    {
      indent_leaf();
      if (m_insidePre)
      {
        printf("%s",w->chars().data());
      }
      else
      {
        printf(" ");
      }
    }
    void visit(DocSymbol *s)
    {
      indent_leaf();
      switch(s->symbol())
      {
	case DocSymbol::BSlash:  printf("\\"); break;
	case DocSymbol::At:      printf("@"); break;
	case DocSymbol::Less:    printf("<"); break;
	case DocSymbol::Greater: printf(">"); break;
	case DocSymbol::Amp:     printf("&"); break;
	case DocSymbol::Dollar:  printf("$"); break;
	case DocSymbol::Hash:    printf("#"); break;
	case DocSymbol::Percent: printf("%%"); break;
	case DocSymbol::Pipe:    printf("|"); break;
	case DocSymbol::Copy:    printf("&copy;"); break;
	case DocSymbol::Apos:    printf("'"); break;
	case DocSymbol::Quot:    printf("\""); break;
        case DocSymbol::Lsquo:   printf("&lsquo;"); break;
        case DocSymbol::Rsquo:   printf("&rsquo;"); break;
        case DocSymbol::Ldquo:   printf("&ldquo;"); break;
        case DocSymbol::Rdquo:   printf("&rdquo;"); break;
        case DocSymbol::Ndash:   printf("&ndash;"); break;
        case DocSymbol::Mdash:   printf("&mdash;"); break;
	case DocSymbol::Uml:     printf("&%cuml;",s->letter()); break;
	case DocSymbol::Acute:   printf("&%cacute;",s->letter()); break;
	case DocSymbol::Grave:   printf("&%cgrave;",s->letter()); break;
	case DocSymbol::Circ:    printf("&%ccirc;",s->letter()); break;
	case DocSymbol::Tilde:   printf("&%ctilde;",s->letter()); break;
	case DocSymbol::Szlig:   printf("&szlig;"); break;
	case DocSymbol::Cedil:   printf("&%ccedul;",s->letter()); break;
	case DocSymbol::Ring:    printf("&%cring;",s->letter()); break;
	case DocSymbol::Nbsp:    printf("&nbsp;"); break;
	case DocSymbol::Aelig:   printf("&aelig;"); break;
	case DocSymbol::AElig:   printf("&AElig;"); break;
    case DocSymbol::GrkGamma:      printf("&Gamma;"); break;
    case DocSymbol::GrkDelta:      printf("&Delta;"); break;
    case DocSymbol::GrkTheta:      printf("&Theta;"); break;
    case DocSymbol::GrkLambda:     printf("&Lambda;"); break;
    case DocSymbol::GrkXi:         printf("&Xi;"); break;
    case DocSymbol::GrkPi:         printf("&Pi;"); break;
    case DocSymbol::GrkSigma:      printf("&Sigma;"); break;
    case DocSymbol::GrkUpsilon:    printf("&Upsilon;"); break;
    case DocSymbol::GrkPhi:        printf("&Phi;"); break;
    case DocSymbol::GrkPsi:        printf("&Psi;"); break;
    case DocSymbol::GrkOmega:      printf("&Omega;"); break;
    case DocSymbol::Grkalpha:      printf("&alpha;"); break;
    case DocSymbol::Grkbeta:       printf("&beta;"); break;
    case DocSymbol::Grkgamma:      printf("&gamma;"); break;
    case DocSymbol::Grkdelta:      printf("&delta;"); break;
    case DocSymbol::Grkepsilon:    printf("&epsilon;"); break;
    case DocSymbol::Grkzeta:       printf("&zeta;"); break;
    case DocSymbol::Grketa:        printf("&eta;"); break;
    case DocSymbol::Grktheta:      printf("&theta;"); break;
    case DocSymbol::Grkiota:       printf("&iota;"); break;
    case DocSymbol::Grkkappa:      printf("&kappa;"); break;
    case DocSymbol::Grklambda:     printf("&lambda;"); break;
    case DocSymbol::Grkmu:         printf("&mu;"); break;
    case DocSymbol::Grknu:         printf("&nu;"); break;
    case DocSymbol::Grkxi:         printf("&xi;"); break;
    case DocSymbol::Grkpi:         printf("&pi;"); break;
    case DocSymbol::Grkrho:        printf("&rho;"); break;
    case DocSymbol::Grksigma:      printf("&sigma;"); break;
    case DocSymbol::Grktau:        printf("&tau;"); break;
    case DocSymbol::Grkupsilon:    printf("&upsilon;"); break;
    case DocSymbol::Grkphi:        printf("&phi;"); break;
    case DocSymbol::Grkchi:        printf("&chi;"); break;
    case DocSymbol::Grkpsi:        printf("&psi;"); break;
    case DocSymbol::Grkomega:      printf("&omega;"); break;
    case DocSymbol::Grkvarsigma:   printf("&sigmaf;"); break;
    case DocSymbol::Section:       printf("&sect;"); break;
    case DocSymbol::Degree:        printf("&deg;"); break;
    case DocSymbol::Prime:         printf("&prime;"); break;
    case DocSymbol::DoublePrime:   printf("&Prime;"); break;
    case DocSymbol::Infinity:      printf("&infin;"); break;
    case DocSymbol::EmptySet:      printf("&empty;"); break;
    case DocSymbol::PlusMinus:     printf("&plusmn;"); break;
    case DocSymbol::Times:         printf("&times;"); break;
    case DocSymbol::Minus:         printf("&minus;"); break;
    case DocSymbol::CenterDot:     printf("&sdot;"); break;
    case DocSymbol::Partial:       printf("&part;"); break;
    case DocSymbol::Nabla:         printf("&nabla;"); break;
    case DocSymbol::SquareRoot:    printf("&radic;"); break;
    case DocSymbol::Perpendicular: printf("&perp;"); break;
    case DocSymbol::Sum:           printf("&sum;"); break;
    case DocSymbol::Integral:      printf("&int;"); break;
    case DocSymbol::Product:       printf("&prod;"); break;
    case DocSymbol::Similar:       printf("&sim;"); break;
    case DocSymbol::Approx:        printf("&asymp;"); break;
    case DocSymbol::NotEqual:      printf("&ne;"); break;
    case DocSymbol::Equivalent:    printf("&equiv;"); break;
    case DocSymbol::Proportional:  printf("&prop;"); break;
    case DocSymbol::LessEqual:     printf("&le;"); break;
    case DocSymbol::GreaterEqual:  printf("&ge;"); break;
    case DocSymbol::LeftArrow:     printf("&larr;"); break;
    case DocSymbol::RightArrow:    printf("&rarr;"); break;
    case DocSymbol::SetIn:         printf("&isin;"); break;
    case DocSymbol::SetNotIn:      printf("&notin;"); break;
    case DocSymbol::LeftCeil:      printf("&lceil;"); break;
    case DocSymbol::RightCeil:     printf("&rceil;"); break;
    case DocSymbol::LeftFloor:     printf("&lfloor;"); break;
    case DocSymbol::RightFloor:    printf("&rfloor;"); break;
	default:
	  printf("Error: unknown symbol found\n");
      }
    }
    void visit(DocURL *u)
    {
      indent_leaf();
      printf("%s",u->url().data());
    }
    void visit(DocLineBreak *)
    {
      indent_leaf();
      printf("<br/>");
    }
    void visit(DocHorRuler *)
    {
      indent_leaf();
      printf("<hr>");
    }
    void visit(DocStyleChange *s)
    {
      indent_leaf();
      switch (s->style())
      {
        case DocStyleChange::Bold:
         if (s->enable()) printf("<bold>"); else printf("</bold>");
         break;
        case DocStyleChange::Italic:
         if (s->enable()) printf("<italic>"); else printf("</italic>");
         break;
        case DocStyleChange::Code:
         if (s->enable()) printf("<code>"); else printf("</code>");
         break;
        case DocStyleChange::Subscript:
         if (s->enable()) printf("<sub>"); else printf("</sub>");
         break;
        case DocStyleChange::Superscript:
         if (s->enable()) printf("<sup>"); else printf("</sup>");
         break;
        case DocStyleChange::Center:
         if (s->enable()) printf("<center>"); else printf("</center>");
         break;
        case DocStyleChange::Small:
         if (s->enable()) printf("<small>"); else printf("</small>");
         break;
        case DocStyleChange::Preformatted:
         if (s->enable()) printf("<pre>"); else printf("</pre>");
         break;
        case DocStyleChange::Div:
         if (s->enable()) printf("<div>"); else printf("</div>");
         break;
        case DocStyleChange::Span:
         if (s->enable()) printf("<span>"); else printf("</span>");
         break;
      }
    }
    void visit(DocVerbatim *s)
    {
      indent_leaf();
      switch(s->type())
      {
        case DocVerbatim::Code: printf("<code>"); break;
        case DocVerbatim::Verbatim: printf("<verbatim>"); break;
        case DocVerbatim::HtmlOnly: printf("<htmlonly>"); break;
        case DocVerbatim::RtfOnly: printf("<rtfonly>"); break;
        case DocVerbatim::ManOnly: printf("<manonly>"); break;
        case DocVerbatim::LatexOnly: printf("<latexonly>"); break;
        case DocVerbatim::XmlOnly: printf("<xmlonly>"); break;
        case DocVerbatim::DocbookOnly: printf("<docbookonly>"); break;
        case DocVerbatim::Dot: printf("<dot>"); break;
        case DocVerbatim::Msc: printf("<msc>"); break;
      }
      printf("%s",s->text().data());
      switch(s->type())
      {
        case DocVerbatim::Code: printf("</code>"); break;
        case DocVerbatim::Verbatim: printf("</verbatim>"); break;
        case DocVerbatim::HtmlOnly: printf("</htmlonly>"); break;
        case DocVerbatim::RtfOnly: printf("</rtfonly>"); break;
        case DocVerbatim::ManOnly: printf("</manonly>"); break;
        case DocVerbatim::LatexOnly: printf("</latexonly>"); break;
        case DocVerbatim::XmlOnly: printf("</xmlonly>"); break;
        case DocVerbatim::DocbookOnly: printf("</docbookonly>"); break;
        case DocVerbatim::Dot: printf("</dot>"); break;
        case DocVerbatim::Msc: printf("</msc>"); break;
      }
    }
    void visit(DocAnchor *a)
    {
      indent_leaf();
      printf("<anchor name=\"%s\"/>",a->anchor().data());
    }
    void visit(DocInclude *inc)
    {
      indent_leaf();
      printf("<include file=\"%s\" type=\"",inc->file().data());
      switch(inc->type())
      {
        case DocInclude::Include: printf("include"); break;
        case DocInclude::IncWithLines: printf("incwithlines"); break;
        case DocInclude::DontInclude: printf("dontinclude"); break;
        case DocInclude::HtmlInclude: printf("htmlinclude"); break;
        case DocInclude::VerbInclude: printf("verbinclude"); break;
        case DocInclude::Snippet: printf("snippet"); break;
      }
      printf("\"/>");
    }
    void visit(DocIncOperator *op)
    {
      indent_leaf();
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
      indent_leaf();
      printf("<formula name=%s test=%s/>",f->name().data(),f->text().data());
    }
    void visit(DocIndexEntry *i)
    {
      indent_leaf();
      printf("<indexentry>%s</indexentry\n",i->entry().data());
    }
    void visit(DocSimpleSectSep *)
    {
      indent_leaf();
      printf("<simplesectsep/>");
    }
    void visit(DocCite *cite)
    {
      indent_leaf();
      printf("<cite ref=\"%s\" file=\"%s\" "
             "anchor=\"%s\" text=\"%s\""
             "/>\n",
             cite->ref().data(),cite->file().data(),cite->anchor().data(),
             cite->text().data());
    }

    //--------------------------------------
    
    void visitPre(DocAutoList *l)
    {
      indent_pre();
      if (l->isEnumList())
      {
        printf("<ol>\n");
      }
      else
      {
        printf("<ul>\n");
      }
    }
    void visitPost(DocAutoList *l)
    {
      indent_post();
      if (l->isEnumList())
      {
        printf("</ol>\n");
      }
      else
      {
        printf("</ul>\n");
      }
    }
    void visitPre(DocAutoListItem *)
    {
      indent_pre();
      printf("<li>\n");
    }
    void visitPost(DocAutoListItem *) 
    {
      indent_post();
      printf("</li>\n");
    }
    void visitPre(DocPara *) 
    {
      indent_pre();
      printf("<para>\n");
    }
    void visitPost(DocPara *)
    {
      indent_post();
      printf("</para>\n");
    }
    void visitPre(DocRoot *)
    {
      indent_pre();
      printf("<root>\n");
    }
    void visitPost(DocRoot *)
    {
      indent_post();
      printf("</root>\n");
    }
    void visitPre(DocSimpleSect *s)
    {
      indent_pre();
      printf("<simplesect type=");
      switch(s->type())
      {
	case DocSimpleSect::See: printf("see"); break;
	case DocSimpleSect::Return: printf("return"); break;
	case DocSimpleSect::Author: printf("author"); break;
	case DocSimpleSect::Authors: printf("authors"); break;
	case DocSimpleSect::Version: printf("version"); break;
	case DocSimpleSect::Since: printf("since"); break;
	case DocSimpleSect::Date: printf("date"); break;
	case DocSimpleSect::Note: printf("note"); break;
	case DocSimpleSect::Warning: printf("warning"); break;
	case DocSimpleSect::Pre: printf("pre"); break;
	case DocSimpleSect::Post: printf("post"); break;
	case DocSimpleSect::Copyright: printf("copyright"); break;
	case DocSimpleSect::Invar: printf("invar"); break;
	case DocSimpleSect::Remark: printf("remark"); break;
	case DocSimpleSect::Attention: printf("attention"); break;
	case DocSimpleSect::User: printf("user"); break;
	case DocSimpleSect::Rcs: printf("rcs"); break;
	case DocSimpleSect::Unknown: printf("unknown"); break;
      }
      printf(">\n");
    }
    void visitPost(DocSimpleSect *)
    {
      indent_post();
      printf("</simplesect>\n");
    }
    void visitPre(DocTitle *)
    {
      indent_pre();
      printf("<title>\n");
    }
    void visitPost(DocTitle *)
    {
      indent_post();
      printf("</title>\n");
    }
    void visitPre(DocSimpleList *)
    {
      indent_pre();
      printf("<ul>\n");
    }
    void visitPost(DocSimpleList *)
    {
      indent_post();
      printf("</ul>\n");
    }
    void visitPre(DocSimpleListItem *)
    {
      indent_pre();
      printf("<li>\n");
    }
    void visitPost(DocSimpleListItem *) 
    {
      indent_post();
      printf("</li>\n");
    }
    void visitPre(DocSection *s)
    {
      indent_pre();
      printf("<sect%d>\n",s->level());
    }
    void visitPost(DocSection *s) 
    {
      indent_post();
      printf("</sect%d>\n",s->level());
    }
    void visitPre(DocHtmlList *s)
    {
      indent_pre();
      if (s->type()==DocHtmlList::Ordered) printf("<ol>\n"); else printf("<ul>\n");
    }
    void visitPost(DocHtmlList *s) 
    {
      indent_post();
      if (s->type()==DocHtmlList::Ordered) printf("</ol>\n"); else printf("</ul>\n");
    }
    void visitPre(DocHtmlListItem *)
    {
      indent_pre();
      printf("<li>\n");
    }
    void visitPost(DocHtmlListItem *) 
    {
      indent_post();
      printf("</li>\n");
    }
    //void visitPre(DocHtmlPre *)
    //{
    //  indent_pre();
    //  printf("<pre>\n");
    //  m_insidePre=TRUE;
    //}
    //void visitPost(DocHtmlPre *) 
    //{
    //  m_insidePre=FALSE;
    //  indent_post();
    //  printf("</pre>\n");
    //}
    void visitPre(DocHtmlDescList *)
    {
      indent_pre();
      printf("<dl>\n");
    }
    void visitPost(DocHtmlDescList *) 
    {
      indent_post();
      printf("</dl>\n");
    }
    void visitPre(DocHtmlDescTitle *)
    {
      indent_pre();
      printf("<dt>\n");
    }
    void visitPost(DocHtmlDescTitle *) 
    {
      indent_post();
      printf("</dt>\n");
    }
    void visitPre(DocHtmlDescData *)
    {
      indent_pre();
      printf("<dd>\n");
    }
    void visitPost(DocHtmlDescData *) 
    {
      indent_post();
      printf("</dd>\n");
    }
    void visitPre(DocHtmlTable *t)
    {
      indent_pre();
      printf("<table rows=\"%d\" cols=\"%d\">\n",
          t->numRows(),t->numColumns());
    }
    void visitPost(DocHtmlTable *) 
    {
      indent_post();
      printf("</table>\n");
    }
    void visitPre(DocHtmlRow *)
    {
      indent_pre();
      printf("<tr>\n");
    }
    void visitPost(DocHtmlRow *) 
    {
      indent_post();
      printf("</tr>\n");
    }
    void visitPre(DocHtmlCell *c)
    {
      indent_pre();
      printf("<t%c>\n",c->isHeading()?'h':'d');
    }
    void visitPost(DocHtmlCell *c) 
    {
      indent_post();
      printf("</t%c>\n",c->isHeading()?'h':'d');
    }
    void visitPre(DocHtmlCaption *)
    {
      indent_pre();
      printf("<caption>\n");
    }
    void visitPost(DocHtmlCaption *) 
    {
      indent_post();
      printf("</caption>\n");
    }
    void visitPre(DocInternal *)
    {
      indent_pre();
      printf("<internal>\n");
    }
    void visitPost(DocInternal *) 
    {
      indent_post();
      printf("</internal>\n");
    }
    void visitPre(DocHRef *href)
    {
      indent_pre();
      printf("<a url=\"%s\">\n",href->url().data());
    }
    void visitPost(DocHRef *) 
    {
      indent_post();
      printf("</a>\n");
    }
    void visitPre(DocHtmlHeader *header)
    {
      indent_pre();
      printf("<h%d>\n",header->level());
    }
    void visitPost(DocHtmlHeader *header) 
    {
      indent_post();
      printf("</h%d>\n",header->level());
    }
    void visitPre(DocImage *img)
    {
      indent_pre();
      printf("<image src=\"%s\" type=\"",img->name().data());
      switch(img->type())
      {
        case DocImage::Html: printf("html"); break;
        case DocImage::Latex: printf("latex"); break;
        case DocImage::Rtf: printf("rtf"); break;
      }
      printf("\" width=%s height=%s>\n",img->width().data(),img->height().data());
    }
    void visitPost(DocImage *) 
    {
      indent_post();
      printf("</image>\n");
    }
    void visitPre(DocDotFile *df)
    {
      indent_pre();
      printf("<dotfile src=\"%s\">\n",df->name().data());
    }
    void visitPost(DocDotFile *) 
    {
      indent_post();
      printf("</dotfile>\n");
    }
    void visitPre(DocMscFile *df)
    {
      indent_pre();
      printf("<mscfile src=\"%s\">\n",df->name().data());
    }
    void visitPost(DocMscFile *) 
    {
      indent_post();
      printf("</mscfile>\n");
    }
    void visitPre(DocLink *lnk)
    {
      indent_pre();
      printf("<link ref=\"%s\" file=\"%s\" anchor=\"%s\">\n",
          lnk->ref().data(),lnk->file().data(),lnk->anchor().data());
    }
    void visitPost(DocLink *) 
    {
      indent_post();
      printf("</link>\n");
    }
    void visitPre(DocRef *ref)
    {
      indent_pre();
      printf("<ref ref=\"%s\" file=\"%s\" "
             "anchor=\"%s\" targetTitle=\"%s\""
             " hasLinkText=\"%s\" refToAnchor=\"%s\" refToSection=\"%s\">\n",
             ref->ref().data(),ref->file().data(),ref->anchor().data(),
             ref->targetTitle().data(),ref->hasLinkText()?"yes":"no",
             ref->refToAnchor()?"yes":"no", ref->refToSection()?"yes":"no");
    }
    void visitPost(DocRef *) 
    {
      indent_post();
      printf("</ref>\n");
    }
    void visitPre(DocSecRefItem *ref)
    {
      indent_pre();
      printf("<secrefitem target=\"%s\">\n",ref->target().data());
    }
    void visitPost(DocSecRefItem *) 
    {
      indent_post();
      printf("</secrefitem>\n");
    }
    void visitPre(DocSecRefList *)
    {
      indent_pre();
      printf("<secreflist>\n");
    }
    void visitPost(DocSecRefList *) 
    {
      indent_post();
      printf("</secreflist>\n");
    }
    //void visitPre(DocLanguage *l)
    //{
    //  indent_pre();
    //  printf("<language id=%s>\n",l->id().data());
    //}
    //void visitPost(DocLanguage *) 
    //{
    //  indent_post();
    //  printf("</language>\n");
    //}
    void visitPre(DocParamList *pl)
    {
      indent_pre();
      //QStrListIterator sli(pl->parameters());
      QListIterator<DocNode> sli(pl->parameters());
      //const char *s;
      DocNode *param;
      printf("<parameters>");
      for (sli.toFirst();(param=sli.current());++sli)
      {
        printf("<param>");
        if (param->kind()==DocNode::Kind_Word)
        {
          visit((DocWord*)param); 
        }
        else if (param->kind()==DocNode::Kind_LinkedWord)
        {
          visit((DocLinkedWord*)param); 
        }
        printf("</param>");
      }
      printf("\n");
    }
    void visitPost(DocParamList *)
    {
      indent_post();
      printf("</parameters>\n");
    }
    void visitPre(DocParamSect *ps)
    {
      indent_pre();
      printf("<paramsect type=");
      switch (ps->type())
      {
	case DocParamSect::Param: printf("param"); break;
	case DocParamSect::RetVal: printf("retval"); break;
	case DocParamSect::Exception: printf("exception"); break;
	case DocParamSect::TemplateParam: printf("templateparam"); break;
	case DocParamSect::Unknown: printf("unknown"); break;
      }
      printf(">\n");
    }
    void visitPost(DocParamSect *)
    {
      indent_post();
      printf("</paramsect>\n");
    }
    void visitPre(DocXRefItem *x)
    {
      indent_pre();
      printf("<xrefitem file=\"%s\" anchor=\"%s\" title=\"%s\"/>\n",
          x->file().data(),x->anchor().data(),x->title().data());
    }
    void visitPost(DocXRefItem *)
    {
      indent_post();
      printf("<xrefitem/>\n");
    }
    void visitPre(DocInternalRef *r)
    {
      indent_pre();
      printf("<internalref file=%s anchor=%s>\n",r->file().data(),r->anchor().data());
    }
    void visitPost(DocInternalRef *)
    {
      indent_post();
      printf("</internalref>\n");
    }
    void visitPre(DocCopy *c)
    {
      indent_pre();
      printf("<copy link=\"%s\">\n",c->link().data());
    }
    void visitPost(DocCopy *)
    {
      indent_post();
      printf("</copy>\n");
    }
    void visitPre(DocText *)
    {
      indent_pre();
      printf("<text>\n");
    }
    void visitPost(DocText *)
    {
      indent_post();
      printf("</text>\n");
    }
    void visitPre(DocHtmlBlockQuote *)
    {
      indent_pre();
      printf("<blockquote>\n");
    }
    void visitPost(DocHtmlBlockQuote *)
    {
      indent_post();
      printf("</blockquote>\n");
    }
    void visitPre(DocVhdlFlow *)
    {
      indent_pre();
      printf("<vhdlflow>\n");
    }
    void visitPost(DocVhdlFlow *)
    {
      indent_post();
      printf("</vhdlflow>\n");
    }

  private:
    // helper functions
    void indent() 
    { 
      if (m_needsEnter) printf("\n");
      for (int i=0;i<m_indent;i++) printf("."); 
      m_needsEnter=FALSE;
    } 
    void indent_leaf()
    {
      if (!m_needsEnter) indent();
      m_needsEnter=TRUE;
    }
    void indent_pre()
    {
      indent();
      m_indent++;
    }
    void indent_post()
    {
      m_indent--;
      indent();
    }
    
    // member variables
    int m_indent;
    bool m_needsEnter;
    bool m_insidePre;
};

#endif
