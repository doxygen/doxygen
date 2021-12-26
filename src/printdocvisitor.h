/******************************************************************************
 *
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#ifndef PRINTDOCVISITOR_H
#define PRINTDOCVISITOR_H

#include "docvisitor.h"
#include "htmlentity.h"
#include "emoji.h"
#include "message.h"

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
      printf("%s",qPrint(w->word()));
    }
    void visit(DocLinkedWord *w)
    {
      indent_leaf();
      printf("%s",qPrint(w->word()));
    }
    void visit(DocWhiteSpace *w)
    {
      indent_leaf();
      if (m_insidePre)
      {
        printf("%s",qPrint(w->chars()));
      }
      else
      {
        printf(" ");
      }
    }
    void visit(DocSymbol *s)
    {
      indent_leaf();
      const char *res = HtmlEntityMapper::instance()->utf8(s->symbol(),TRUE);
      if (res)
      {
        printf("%s",res);
      }
      else
      {
        printf("print: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
      }
    }
    void visit(DocEmoji *s)
    {
      indent_leaf();
      const char *res = EmojiEntityMapper::instance()->name(s->index());
      if (res)
      {
        printf("%s",res);
      }
      else
      {
        printf("print: non supported emoji found: %s\n",qPrint(s->name()));
      }
    }
    void visit(DocURL *u)
    {
      indent_leaf();
      printf("%s",qPrint(u->url()));
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
        case DocStyleChange::S:
          if (s->enable()) printf("<s>"); else printf("</s>");
          break;
        case DocStyleChange::Strike:
          if (s->enable()) printf("<strike>"); else printf("</strike>");
          break;
        case DocStyleChange::Del:
          if (s->enable()) printf("<del>"); else printf("</del>");
          break;
        case DocStyleChange::Underline:
          if (s->enable()) printf("<underline>"); else printf("</underline>");
          break;
        case DocStyleChange::Ins:
          if (s->enable()) printf("<ins>"); else printf("</ins>");
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
        case DocStyleChange::Cite:
          if (s->enable()) printf("<cite>"); else printf("</cite>");
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
        case DocStyleChange::Details:
          if (s->enable())
          {
            indent_pre();
            printf("<details>\n"); 
          }
          else 
          {
            indent_post();
            printf("</details>\n");
          }
          break;
        case DocStyleChange::Summary:
          if (s->enable())
          {
            indent_pre();
            printf("<summary>\n"); 
          }
          else 
          {
            indent_post();
            printf("</summary>\n");
          }
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
        case DocVerbatim::JavaDocLiteral: printf("<javadocliteral>"); break;
        case DocVerbatim::JavaDocCode: printf("<javadoccode>"); break;
        case DocVerbatim::HtmlOnly: printf("<htmlonly>"); break;
        case DocVerbatim::RtfOnly: printf("<rtfonly>"); break;
        case DocVerbatim::ManOnly: printf("<manonly>"); break;
        case DocVerbatim::LatexOnly: printf("<latexonly>"); break;
        case DocVerbatim::XmlOnly: printf("<xmlonly>"); break;
        case DocVerbatim::DocbookOnly: printf("<docbookonly>"); break;
        case DocVerbatim::Dot: printf("<dot>"); break;
        case DocVerbatim::Msc: printf("<msc>"); break;
        case DocVerbatim::PlantUML: printf("<plantuml>"); break;
      }
      printf("%s",qPrint(s->text()));
      switch(s->type())
      {
        case DocVerbatim::Code: printf("</code>"); break;
        case DocVerbatim::Verbatim: printf("</verbatim>"); break;
        case DocVerbatim::JavaDocLiteral: printf("</javadocliteral>"); break;
        case DocVerbatim::JavaDocCode: printf("</javadoccode>"); break;
        case DocVerbatim::HtmlOnly: printf("</htmlonly>"); break;
        case DocVerbatim::RtfOnly: printf("</rtfonly>"); break;
        case DocVerbatim::ManOnly: printf("</manonly>"); break;
        case DocVerbatim::LatexOnly: printf("</latexonly>"); break;
        case DocVerbatim::XmlOnly: printf("</xmlonly>"); break;
        case DocVerbatim::DocbookOnly: printf("</docbookonly>"); break;
        case DocVerbatim::Dot: printf("</dot>"); break;
        case DocVerbatim::Msc: printf("</msc>"); break;
        case DocVerbatim::PlantUML: printf("</plantuml>"); break;
      }
    }
    void visit(DocAnchor *a)
    {
      indent_leaf();
      printf("<anchor name=\"%s\"/>",qPrint(a->anchor()));
    }
    void visit(DocInclude *inc)
    {
      indent_leaf();
      printf("<include file=\"%s\" type=\"",qPrint(inc->file()));
      switch(inc->type())
      {
        case DocInclude::Include: printf("include"); break;
        case DocInclude::IncWithLines: printf("incwithlines"); break;
        case DocInclude::DontInclude: printf("dontinclude"); break;
        case DocInclude::DontIncWithLines: printf("dontinwithlines"); break;
        case DocInclude::HtmlInclude:
               printf("htmlinclude");
               if (inc->isBlock()) printf(" block=\"yes\"");
               break;
        case DocInclude::LatexInclude: printf("latexinclude"); break;
        case DocInclude::RtfInclude: printf("rtfinclude"); break;
        case DocInclude::DocbookInclude: printf("docbookinclude"); break;
        case DocInclude::ManInclude: printf("maninclude"); break;
        case DocInclude::XmlInclude: printf("xmlinclude"); break;
        case DocInclude::VerbInclude: printf("verbinclude"); break;
        case DocInclude::Snippet: printf("snippet"); break;
        case DocInclude::SnipWithLines: printf("snipwithlines"); break;
        case DocInclude::SnippetDoc:
        case DocInclude::IncludeDoc:
          err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
              "Please create a bug report\n",__FILE__);
          break;
      }
      printf("\"/>");
    }
    void visit(DocIncOperator *op)
    {
      indent_leaf();
      printf("<incoperator pattern=\"%s\" type=\"",qPrint(op->pattern()));
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
      printf("<formula name=%s text=%s/>",qPrint(f->name()),qPrint(f->text()));
    }
    void visit(DocIndexEntry *i)
    {
      indent_leaf();
      printf("<indexentry>%s</indexentry\n",qPrint(i->entry()));
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
             qPrint(cite->ref()),qPrint(cite->file()),qPrint(cite->anchor()),
             qPrint(cite->text()));
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
      if (s->type()==DocHtmlList::Ordered)
      {
        printf("<ol");
        for (const auto &opt : s->attribs())
        {
          printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
        }
        printf(">\n");
      }
      else printf("<ul>\n");
 
    }
    void visitPost(DocHtmlList *s)
    {
      indent_post();
      if (s->type()==DocHtmlList::Ordered) printf("</ol>\n"); else printf("</ul>\n");
    }
    void visitPre(DocHtmlListItem *s)
    {
      indent_pre();
      printf("<li");
      for (const auto &opt : s->attribs())
      {
        printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
      }
      printf(">\n");
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
      printf("<table rows=\"%zu\" cols=\"%zu\">\n",
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
      printf("<a url=\"%s\">\n",qPrint(href->url()));
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
      printf("<image src=\"%s\" type=\"",qPrint(img->name()));
      switch(img->type())
      {
        case DocImage::Html:    printf("html"); break;
        case DocImage::Latex:   printf("latex"); break;
        case DocImage::Rtf:     printf("rtf"); break;
        case DocImage::DocBook: printf("docbook"); break;
        case DocImage::Xml:     printf("xml"); break;
      }
      printf("\" %s %s inline=\"%s\">\n",qPrint(img->width()),qPrint(img->height()),img->isInlineImage() ? "yes" : "no");
    }
    void visitPost(DocImage *)
    {
      indent_post();
      printf("</image>\n");
    }
    void visitPre(DocDotFile *df)
    {
      indent_pre();
      printf("<dotfile src=\"%s\">\n",qPrint(df->name()));
    }
    void visitPost(DocDotFile *)
    {
      indent_post();
      printf("</dotfile>\n");
    }
    void visitPre(DocMscFile *df)
    {
      indent_pre();
      printf("<mscfile src=\"%s\">\n",qPrint(df->name()));
    }
    void visitPost(DocMscFile *)
    {
      indent_post();
      printf("</mscfile>\n");
    }
    void visitPre(DocDiaFile *df)
    {
      indent_pre();
      printf("<diafile src=\"%s\">\n",qPrint(df->name()));
    }
    void visitPost(DocDiaFile *)
    {
      indent_post();
      printf("</diafile>\n");
    }
    void visitPre(DocLink *lnk)
    {
      indent_pre();
      printf("<link ref=\"%s\" file=\"%s\" anchor=\"%s\">\n",
          qPrint(lnk->ref()),qPrint(lnk->file()),qPrint(lnk->anchor()));
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
             " hasLinkText=\"%s\" refToAnchor=\"%s\" refToSection=\"%s\" refToTable=\"%s\">\n",
             qPrint(ref->ref()),qPrint(ref->file()),qPrint(ref->anchor()),
             qPrint(ref->targetTitle()),ref->hasLinkText()?"yes":"no",
             ref->refToAnchor()?"yes":"no", ref->refToSection()?"yes":"no",
             ref->refToTable()?"yes":"no");
    }
    void visitPost(DocRef *)
    {
      indent_post();
      printf("</ref>\n");
    }
    void visitPre(DocSecRefItem *ref)
    {
      indent_pre();
      printf("<secrefitem target=\"%s\">\n",qPrint(ref->target()));
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
    //  printf("<language id=%s>\n",qPrint(l->id()));
    //}
    //void visitPost(DocLanguage *)
    //{
    //  indent_post();
    //  printf("</language>\n");
    //}
    void visitPre(DocParamList *pl)
    {
      indent_pre();
      printf("<parameters>");
      if (!pl->parameters().empty())
      {
        printf("<param>");
        for (const auto &param : pl->parameters())
        {
          if (param->kind()==DocNode::Kind_Word)
          {
            visit((DocWord*)param.get());
          }
          else if (param->kind()==DocNode::Kind_LinkedWord)
          {
            visit((DocLinkedWord*)param.get());
          }
          else if (param->kind()==DocNode::Kind_Sep)
          {
            printf("</param>");
            printf("<param>");
          }
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
      printf("<xrefitem file=\"%s\" anchor=\"%s\" title=\"%s\">\n",
          qPrint(x->file()),qPrint(x->anchor()),qPrint(x->title()));
    }
    void visitPost(DocXRefItem *)
    {
      indent_post();
      printf("</xrefitem>\n");
    }
    void visitPre(DocInternalRef *r)
    {
      indent_pre();
      printf("<internalref file=%s anchor=%s>\n",qPrint(r->file()),qPrint(r->anchor()));
    }
    void visitPost(DocInternalRef *)
    {
      indent_post();
      printf("</internalref>\n");
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
    void visitPre(DocParBlock *)
    {
      indent_pre();
      printf("<parblock>\n");
    }
    void visitPost(DocParBlock *)
    {
      indent_post();
      printf("</parblock>\n");
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
