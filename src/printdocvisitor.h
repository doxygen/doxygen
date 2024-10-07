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

#include "htmlentity.h"
#include "emoji.h"
#include "message.h"

/*! Visitor implementation for pretty printing */
class PrintDocVisitor
{
  public:
    PrintDocVisitor() : m_indent(0), m_needsEnter(FALSE), m_insidePre(FALSE) {}

    //--------------------------------------

    void operator()(const DocWord &w)
    {
      indent_leaf();
      printf("%s",qPrint(w.word()));
    }
    void operator()(const DocLinkedWord &w)
    {
      indent_leaf();
      printf("%s",qPrint(w.word()));
    }
    void operator()(const DocWhiteSpace &w)
    {
      indent_leaf();
      if (m_insidePre)
      {
        printf("%s",qPrint(w.chars()));
      }
      else
      {
        printf(" ");
      }
    }
    void operator()(const DocSymbol &s)
    {
      indent_leaf();
      const char *res = HtmlEntityMapper::instance().utf8(s.symbol(),TRUE);
      if (res)
      {
        printf("%s",res);
      }
      else
      {
        printf("print: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
      }
    }
    void operator()(const DocEmoji &s)
    {
      indent_leaf();
      const char *res = EmojiEntityMapper::instance().name(s.index());
      if (res)
      {
        printf("%s",res);
      }
      else
      {
        printf("print: non supported emoji found: %s\n",qPrint(s.name()));
      }
    }
    void operator()(const DocURL &u)
    {
      indent_leaf();
      printf("%s",qPrint(u.url()));
    }
    void operator()(const DocLineBreak &)
    {
      indent_leaf();
      printf("<br/>");
    }
    void operator()(const DocHorRuler &)
    {
      indent_leaf();
      printf("<hr>");
    }
    void operator()(const DocStyleChange &s)
    {
      indent_leaf();
      switch (s.style())
      {
        case DocStyleChange::Bold:
          if (s.enable()) printf("<bold>"); else printf("</bold>");
          break;
        case DocStyleChange::S:
          if (s.enable()) printf("<s>"); else printf("</s>");
          break;
        case DocStyleChange::Strike:
          if (s.enable()) printf("<strike>"); else printf("</strike>");
          break;
        case DocStyleChange::Del:
          if (s.enable()) printf("<del>"); else printf("</del>");
          break;
        case DocStyleChange::Underline:
          if (s.enable()) printf("<underline>"); else printf("</underline>");
          break;
        case DocStyleChange::Ins:
          if (s.enable()) printf("<ins>"); else printf("</ins>");
          break;
        case DocStyleChange::Italic:
          if (s.enable()) printf("<italic>"); else printf("</italic>");
          break;
        case DocStyleChange::Code:
          if (s.enable()) printf("<code>"); else printf("</code>");
          break;
        case DocStyleChange::Subscript:
          if (s.enable()) printf("<sub>"); else printf("</sub>");
          break;
        case DocStyleChange::Superscript:
          if (s.enable()) printf("<sup>"); else printf("</sup>");
          break;
        case DocStyleChange::Center:
          if (s.enable()) printf("<center>"); else printf("</center>");
          break;
        case DocStyleChange::Small:
          if (s.enable()) printf("<small>"); else printf("</small>");
          break;
        case DocStyleChange::Cite:
          if (s.enable()) printf("<cite>"); else printf("</cite>");
          break;
        case DocStyleChange::Preformatted:
          if (s.enable()) printf("<pre>"); else printf("</pre>");
          break;
        case DocStyleChange::Div:
          if (s.enable()) printf("<div>"); else printf("</div>");
          break;
        case DocStyleChange::Span:
          if (s.enable()) printf("<span>"); else printf("</span>");
          break;
      }
    }
    void operator()(const DocVerbatim &s)
    {
      indent_leaf();
      switch(s.type())
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
      printf("%s",qPrint(s.text()));
      switch(s.type())
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
    void operator()(const DocAnchor &a)
    {
      indent_leaf();
      printf("<anchor name=\"%s\"/>",qPrint(a.anchor()));
    }
    void operator()(const DocInclude &inc)
    {
      indent_leaf();
      printf("<include file=\"%s\" type=\"",qPrint(inc.file()));
      switch(inc.type())
      {
        case DocInclude::Include: printf("include"); break;
        case DocInclude::IncWithLines: printf("incwithlines"); break;
        case DocInclude::DontInclude: printf("dontinclude"); break;
        case DocInclude::DontIncWithLines: printf("dontinwithlines"); break;
        case DocInclude::HtmlInclude:
               printf("htmlinclude");
               if (inc.isBlock()) printf(" block=\"yes\"");
               break;
        case DocInclude::LatexInclude: printf("latexinclude"); break;
        case DocInclude::RtfInclude: printf("rtfinclude"); break;
        case DocInclude::DocbookInclude: printf("docbookinclude"); break;
        case DocInclude::ManInclude: printf("maninclude"); break;
        case DocInclude::XmlInclude: printf("xmlinclude"); break;
        case DocInclude::VerbInclude: printf("verbinclude"); break;
        case DocInclude::Snippet: printf("snippet"); break;
        case DocInclude::SnippetWithLines: printf("snipwithlines"); break;
      }
      printf("\"/>");
    }
    void operator()(const DocIncOperator &op)
    {
      indent_leaf();
      printf("<incoperator pattern=\"%s\" type=\"",qPrint(op.pattern()));
      switch(op.type())
      {
        case DocIncOperator::Line:     printf("line");     break;
        case DocIncOperator::Skip:     printf("skip");     break;
        case DocIncOperator::SkipLine: printf("skipline"); break;
        case DocIncOperator::Until:    printf("until");    break;
      }
      printf("\"/>");
    }
    void operator()(const DocFormula &f)
    {
      indent_leaf();
      printf("<formula name=%s text=%s/>",qPrint(f.name()),qPrint(f.text()));
    }
    void operator()(const DocIndexEntry &i)
    {
      indent_leaf();
      printf("<indexentry>%s</indexentry\n",qPrint(i.entry()));
    }
    void operator()(const DocSimpleSectSep &)
    {
      indent_leaf();
      printf("<simplesectsep/>");
    }
    void operator()(const DocCite &cite)
    {
      indent_leaf();
      printf("<cite ref=\"%s\" file=\"%s\" "
             "anchor=\"%s\" text=\"%s\""
             "/>\n",
             qPrint(cite.ref()),qPrint(cite.file()),qPrint(cite.anchor()),
             qPrint(cite.text()));
    }
    void operator()(const DocSeparator &)
    {
      indent_leaf();
      printf("<sep/>");
    }

    //--------------------------------------
    template<class T>
    void visitChildren(const T &t)
    {
      for (const auto &child : t.children())
      {
        std::visit(*this, child);
      }
    }

    void operator()(const DocAutoList &l)
    {
      indent_pre();
      if (l.isEnumList())
      {
        printf("<ol>\n");
      }
      else
      {
        printf("<ul>\n");
      }
      visitChildren(l);
      indent_post();
      if (l.isEnumList())
      {
        printf("</ol>\n");
      }
      else
      {
        printf("</ul>\n");
      }
    }
    void operator()(const DocAutoListItem &li)
    {
      indent_pre();
      switch (li.itemNumber())
      {
        case DocAutoList::Unchecked: // unchecked
          printf("<li class=\"unchecked\">\n");
          break;
        case DocAutoList::Checked_x: // checked with x
        case DocAutoList::Checked_X: // checked with X
          printf("<li class=\"checked\">\n");
          break;
        default:
          printf("<li>\n");
          break;
      }
      visitChildren(li);
      indent_post();
      printf("</li>\n");
    }
    void operator()(const DocPara &p)
    {
      indent_pre();
      printf("<para>\n");
      visitChildren(p);
      indent_post();
      printf("</para>\n");
    }
    void operator()(const DocRoot &r)
    {
      indent_pre();
      printf("<root>\n");
      visitChildren(r);
      indent_post();
      printf("</root>\n");
    }
    void operator()(const DocSimpleSect &s)
    {
      indent_pre();
      printf("<simplesect type=");
      switch(s.type())
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
	case DocSimpleSect::Important: printf("important"); break;
	case DocSimpleSect::User: printf("user"); break;
	case DocSimpleSect::Rcs: printf("rcs"); break;
	case DocSimpleSect::Unknown: printf("unknown"); break;
      }
      printf(">\n");
      if (s.title())
      {
        std::visit(*this, *s.title());
      }
      visitChildren(s);
      indent_post();
      printf("</simplesect>\n");
    }
    void operator()(const DocTitle &t)
    {
      indent_pre();
      printf("<title>\n");
      visitChildren(t);
      indent_post();
      printf("</title>\n");
    }
    void operator()(const DocSimpleList &l)
    {
      indent_pre();
      printf("<ul>\n");
      visitChildren(l);
      indent_post();
      printf("</ul>\n");
    }
    void operator()(const DocSimpleListItem &li)
    {
      indent_pre();
      printf("<li>\n");
      if (li.paragraph())
      {
        visit(*this,*li.paragraph());
      }
      indent_post();
      printf("</li>\n");
    }
    void operator()(const DocSection &s)
    {
      indent_pre();
      printf("<sect%d>\n",s.level());
      if (s.title())
      {
        std::visit(*this, *s.title());
      }
      visitChildren(s);
      indent_post();
      printf("</sect%d>\n",s.level());
    }
    void operator()(const DocHtmlList &s)
    {
      indent_pre();
      if (s.type()==DocHtmlList::Ordered)
      {
        printf("<ol");
        for (const auto &opt : s.attribs())
        {
          printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
        }
        printf(">\n");
      }
      else
      {
        printf("<ul>\n");
      }
      visitChildren(s);
      indent_post();
      if (s.type()==DocHtmlList::Ordered)
      {
        printf("</ol>\n");
      }
      else
      {
        printf("</ul>\n");
      }
    }
    void operator()(const DocHtmlListItem &li)
    {
      indent_pre();
      printf("<li");
      for (const auto &opt : li.attribs())
      {
        printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
      }
      printf(">\n");
      visitChildren(li);
      indent_post();
      printf("</li>\n");
    }
    void operator()(const DocHtmlDescList &l)
    {
      indent_pre();
      printf("<dl>\n");
      visitChildren(l);
      indent_post();
      printf("</dl>\n");
    }
    void operator()(const DocHtmlDescTitle &dt)
    {
      indent_pre();
      printf("<dt>\n");
      visitChildren(dt);
      indent_post();
      printf("</dt>\n");
    }
    void operator()(const DocHtmlDescData &dd)
    {
      indent_pre();
      printf("<dd>\n");
      visitChildren(dd);
      indent_post();
      printf("</dd>\n");
    }
    void operator()(const DocHtmlTable &t)
    {
      indent_pre();
      printf("<table rows=\"%zu\" cols=\"%zu\">\n",
          t.numRows(),t.numColumns());
      visitChildren(t);
      if (t.caption())
      {
        std::visit(*this, *t.caption());
      }
      indent_post();
      printf("</table>\n");
    }
    void operator()(const DocHtmlRow &tr)
    {
      indent_pre();
      printf("<tr>\n");
      visitChildren(tr);
      indent_post();
      printf("</tr>\n");
    }
    void operator()(const DocHtmlCell &c)
    {
      indent_pre();
      printf("<t%c>\n",c.isHeading()?'h':'d');
      visitChildren(c);
      indent_post();
      printf("</t%c>\n",c.isHeading()?'h':'d');
    }
    void operator()(const DocHtmlCaption &c)
    {
      indent_pre();
      printf("<caption>\n");
      visitChildren(c);
      indent_post();
      printf("</caption>\n");
    }
    void operator()(const DocInternal &i)
    {
      indent_pre();
      printf("<internal>\n");
      visitChildren(i);
      indent_post();
      printf("</internal>\n");
    }
    void operator()(const DocHRef &href)
    {
      indent_pre();
      printf("<a url=\"%s\">\n",qPrint(href.url()));
      visitChildren(href);
      indent_post();
      printf("</a>\n");
    }
    void operator()(const DocHtmlSummary &summary)
    {
      indent_pre();
      printf("<summary");
      for (const auto &opt : summary.attribs())
      {
        printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
      }
      printf(">\n");
      visitChildren(summary);
      indent_post();
      printf("</summary>\n");
    }
    void operator()(const DocHtmlDetails &details)
    {
      indent_pre();
      printf("<details");
      for (const auto &opt : details.attribs())
      {
        printf(" %s=\"%s\"",qPrint(opt.name),qPrint(opt.value));
      }
      printf(">\n");
      auto summary = details.summary();
      if (summary)
      {
        std::visit(*this,*summary);
      }
      visitChildren(details);
      indent_post();
      printf("</details>\n");
    }
    void operator()(const DocHtmlHeader &header)
    {
      indent_pre();
      printf("<h%d>\n",header.level());
      visitChildren(header);
      indent_post();
      printf("</h%d>\n",header.level());
    }
    void operator()(const DocImage &img)
    {
      indent_pre();
      printf("<image src=\"%s\" type=\"",qPrint(img.name()));
      switch(img.type())
      {
        case DocImage::Html:    printf("html"); break;
        case DocImage::Latex:   printf("latex"); break;
        case DocImage::Rtf:     printf("rtf"); break;
        case DocImage::DocBook: printf("docbook"); break;
        case DocImage::Xml:     printf("xml"); break;
      }
      printf("\" %s %s inline=\"%s\">\n",qPrint(img.width()),qPrint(img.height()),img.isInlineImage() ? "yes" : "no");
      visitChildren(img);
      indent_post();
      printf("</image>\n");
    }
    void operator()(const DocDotFile &df)
    {
      indent_pre();
      printf("<dotfile src=\"%s\">\n",qPrint(df.name()));
      visitChildren(df);
      indent_post();
      printf("</dotfile>\n");
    }
    void operator()(const DocMscFile &df)
    {
      indent_pre();
      printf("<mscfile src=\"%s\">\n",qPrint(df.name()));
      visitChildren(df);
      indent_post();
      printf("</mscfile>\n");
    }
    void operator()(const DocDiaFile &df)
    {
      indent_pre();
      printf("<diafile src=\"%s\">\n",qPrint(df.name()));
      visitChildren(df);
      indent_post();
      printf("</diafile>\n");
    }
    void operator()(const DocPlantUmlFile &df)
    {
      indent_pre();
      printf("<plantumlfile src=\"%s\">\n",qPrint(df.name()));
      visitChildren(df);
      indent_post();
      printf("</plantumlfile>\n");
    }
    void operator()(const DocLink &lnk)
    {
      indent_pre();
      printf("<link ref=\"%s\" file=\"%s\" anchor=\"%s\">\n",
          qPrint(lnk.ref()),qPrint(lnk.file()),qPrint(lnk.anchor()));
      visitChildren(lnk);
      indent_post();
      printf("</link>\n");
    }
    void operator()(const DocRef &ref)
    {
      indent_pre();
      printf("<ref ref=\"%s\" file=\"%s\" "
             "anchor=\"%s\" targetTitle=\"%s\""
             " hasLinkText=\"%s\" refToAnchor=\"%s\" refToSection=\"%s\" refToTable=\"%s\">\n",
             qPrint(ref.ref()),qPrint(ref.file()),qPrint(ref.anchor()),
             qPrint(ref.targetTitle()),ref.hasLinkText()?"yes":"no",
             ref.refToAnchor()?"yes":"no", ref.refToSection()?"yes":"no",
             ref.refToTable()?"yes":"no");
      visitChildren(ref);
      indent_post();
      printf("</ref>\n");
    }
    void operator()(const DocSecRefItem &ref)
    {
      indent_pre();
      printf("<secrefitem target=\"%s\">\n",qPrint(ref.target()));
      visitChildren(ref);
      indent_post();
      printf("</secrefitem>\n");
    }
    void operator()(const DocSecRefList &rl)
    {
      indent_pre();
      printf("<secreflist>\n");
      visitChildren(rl);
      indent_post();
      printf("</secreflist>\n");
    }
    void operator()(const DocParamList &pl)
    {
      indent_pre();
      printf("<parameters>");
      if (!pl.parameters().empty())
      {
        printf("<param>");
        for (const auto &param : pl.parameters())
        {
          std::visit(*this,param);
        }
        printf("</param>");
      }
      printf("\n");
      indent_post();
      printf("</parameters>\n");
    }
    void operator()(const DocParamSect &ps)
    {
      indent_pre();
      printf("<paramsect type=");
      switch (ps.type())
      {
	case DocParamSect::Param: printf("param"); break;
	case DocParamSect::RetVal: printf("retval"); break;
	case DocParamSect::Exception: printf("exception"); break;
	case DocParamSect::TemplateParam: printf("templateparam"); break;
	case DocParamSect::Unknown: printf("unknown"); break;
      }
      printf(">\n");
      visitChildren(ps);
      indent_post();
      printf("</paramsect>\n");
    }
    void operator()(const DocXRefItem &x)
    {
      indent_pre();
      printf("<xrefitem file=\"%s\" anchor=\"%s\" title=\"%s\">\n",
          qPrint(x.file()),qPrint(x.anchor()),qPrint(x.title()));
      visitChildren(x);
      indent_post();
      printf("</xrefitem>\n");
    }
    void operator()(const DocInternalRef &r)
    {
      indent_pre();
      printf("<internalref file=%s anchor=%s>\n",qPrint(r.file()),qPrint(r.anchor()));
      visitChildren(r);
      indent_post();
      printf("</internalref>\n");
    }
    void operator()(const DocText &t)
    {
      indent_pre();
      printf("<text>\n");
      visitChildren(t);
      indent_post();
      printf("</text>\n");
    }
    void operator()(const DocHtmlBlockQuote &q)
    {
      indent_pre();
      printf("<blockquote>\n");
      visitChildren(q);
      indent_post();
      printf("</blockquote>\n");
    }
    void operator()(const DocVhdlFlow &vf)
    {
      indent_pre();
      printf("<vhdlflow>\n");
      visitChildren(vf);
      indent_post();
      printf("</vhdlflow>\n");
    }
    void operator()(const DocParBlock &pb)
    {
      indent_pre();
      printf("<parblock>\n");
      visitChildren(pb);
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
