/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
#include "dot.h"
#include "message.h"
#include "util.h"
#include "parserintf.h"
#include "filename.h"
#include "config.h"
#include "htmlentity.h"
#include "emoji.h"
#include "filedef.h"
#include "fileinfo.h"

static void visitCaption(XmlDocVisitor *parent, const DocNodeList &children)
{
  for (const auto &n : children) n->accept(parent);
}

static void visitPreStart(TextStream &t, const char *cmd, bool doCaption,
                          XmlDocVisitor *parent, const DocNodeList &children,
                          const QCString &name, bool writeType, DocImage::Type type, const QCString &width,
                          const QCString &height, const QCString engine = QCString(), const QCString &alt = QCString(), bool inlineImage = FALSE)
{
  t << "<" << cmd;
  if (writeType)
  {
    t << " type=\"";
    switch(type)
    {
      case DocImage::Html:    t << "html"; break;
      case DocImage::Latex:   t << "latex"; break;
      case DocImage::Rtf:     t << "rtf"; break;
      case DocImage::DocBook: t << "docbook"; break;
      case DocImage::Xml:     t << "xml"; break;
    }
    t << "\"";
  }
  if (!name.isEmpty())
  {
    t << " name=\"" << convertToXML(name, TRUE) << "\"";
  }
  if (!width.isEmpty())
  {
    t << " width=\"" << convertToXML(width) << "\"";
  }
  if (!height.isEmpty())
  {
    t << " height=\"" << convertToXML(height) << "\"";
  }
  if (!engine.isEmpty())
  {
    t << " engine=\"" << convertToXML(engine) << "\"";
  }
  if (!alt.isEmpty())
  {
    t << " alt=\"" << convertToXML(alt) << "\"";
  }
  if (inlineImage)
  {
    t << " inline=\"yes\"";
  }
  if (doCaption)
  {
    t << " caption=\"";
    visitCaption(parent, children);
    t << "\"";
  }
  t << ">";
}

static void visitPostEnd(TextStream &t, const char *cmd)
{
  t << "</" << cmd << ">\n";
}

XmlDocVisitor::XmlDocVisitor(TextStream &t,CodeOutputInterface &ci,const QCString &langExt)
  : DocVisitor(DocVisitor_XML), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE),
    m_langExt(langExt)
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
  const char *res = HtmlEntityMapper::instance()->xml(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("XML: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
}

void XmlDocVisitor::visit(DocEmoji *s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance()->name(s->index());
  if (res)
  {
    QCString name=res;
    name = name.mid(1,name.length()-2);
    m_t << "<emoji name=\"" << name << "\" unicode=\"";
    filter(EmojiEntityMapper::instance()->unicode(s->index()));
    m_t << "\"/>";
  }
  else
  {
    m_t << s->name();
  }
}

void XmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  m_t << "<ulink url=\"";
  if (u->isEmail()) m_t << "mailto:";
  filter(u->url());
  m_t << "\">";
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
      if (s->enable()) m_t << "<bold>";      else m_t << "</bold>";
      break;
    case DocStyleChange::S:
      if (s->enable()) m_t << "<s>";      else m_t << "</s>";
      break;
    case DocStyleChange::Strike:
      if (s->enable()) m_t << "<strike>";      else m_t << "</strike>";
      break;
    case DocStyleChange::Del:
      if (s->enable()) m_t << "<del>";      else m_t << "</del>";
      break;
    case DocStyleChange::Underline:
      if (s->enable()) m_t << "<underline>";      else m_t << "</underline>";
      break;
    case DocStyleChange::Ins:
      if (s->enable()) m_t << "<ins>";      else m_t << "</ins>";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "<emphasis>";     else m_t << "</emphasis>";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "<computeroutput>";   else m_t << "</computeroutput>";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "<subscript>";    else m_t << "</subscript>";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "<superscript>";    else m_t << "</superscript>";
      break;
    case DocStyleChange::Center:
      if (s->enable()) m_t << "<center>"; else m_t << "</center>";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "<small>";  else m_t << "</small>";
      break;
    case DocStyleChange::Cite:
      if (s->enable()) m_t << "<cite>";  else m_t << "</cite>";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        m_t << "<preformatted>";
        m_insidePre=TRUE;
      }
      else
      {
        m_t << "</preformatted>";
        m_insidePre=FALSE;
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
    case DocStyleChange::Details:
      if (s->enable()) m_t << "<details>";  else m_t << "</details>";
      break;
    case DocStyleChange::Summary:
      if (s->enable()) m_t << "<summary>";  else m_t << "</summary>";
      break;
  }
}

void XmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  QCString lang = m_langExt;
  if (!s->language().isEmpty()) // explicit language setting
  {
    lang = s->language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch(s->type())
  {
    case DocVerbatim::Code:
      m_t << "<programlisting";
      if (!s->language().isEmpty())
          m_t << " filename=\"" << lang << "\">";
      else
          m_t << ">";
      getCodeParser(lang).parseCode(m_ci,s->context(),s->text(),langExt,
                                    s->isExample(),s->exampleFile());
      m_t << "</programlisting>";
      break;
    case DocVerbatim::JavaDocLiteral:
      m_t << "<javadocliteral>";
      filter(s->text());
      m_t << "</javadocliteral>";
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "<javadoccode>";
      filter(s->text());
      m_t << "</javadoccode>";
      break;
    case DocVerbatim::Verbatim:
      m_t << "<verbatim>";
      filter(s->text());
      m_t << "</verbatim>";
      break;
    case DocVerbatim::HtmlOnly:
      if (s->isBlock())
      {
        m_t << "<htmlonly block=\"yes\">";
      }
      else
      {
        m_t << "<htmlonly>";
      }
      filter(s->text());
      m_t << "</htmlonly>";
      break;
    case DocVerbatim::RtfOnly:
      m_t << "<rtfonly>";
      filter(s->text());
      m_t << "</rtfonly>";
      break;
    case DocVerbatim::ManOnly:
      m_t << "<manonly>";
      filter(s->text());
      m_t << "</manonly>";
      break;
    case DocVerbatim::LatexOnly:
      m_t << "<latexonly>";
      filter(s->text());
      m_t << "</latexonly>";
      break;
    case DocVerbatim::DocbookOnly:
      m_t << "<docbookonly>";
      filter(s->text());
      m_t << "</docbookonly>";
      break;
    case DocVerbatim::XmlOnly:
      m_t << s->text();
      break;
    case DocVerbatim::Dot:
      visitPreStart(m_t, "dot", s->hasCaption(), this, s->children(), QCString(""), FALSE, DocImage::Html, s->width(), s->height());
      filter(s->text());
      visitPostEnd(m_t, "dot");
      break;
    case DocVerbatim::Msc:
      visitPreStart(m_t, "msc", s->hasCaption(), this, s->children(),  QCString(""), FALSE, DocImage::Html, s->width(), s->height());
      filter(s->text());
      visitPostEnd(m_t, "msc");
      break;
    case DocVerbatim::PlantUML:
      visitPreStart(m_t, "plantuml", s->hasCaption(), this, s->children(),  QCString(""), FALSE, DocImage::Html, s->width(), s->height(), s->engine());
      filter(s->text());
      visitPostEnd(m_t, "plantuml");
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
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      {
         m_t << "<programlisting filename=\"" << inc->file() << "\">";
         FileInfo cfi( inc->file().str() );
         FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         getCodeParser(inc->extension()).parseCode(m_ci,inc->context(),
                                           inc->text(),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile(),
                                           fd,   // fileDef,
                                           -1,    // start line
                                           -1,    // end line
                                           FALSE, // inline fragment
                                           0,     // memberDef
                                           TRUE   // show line numbers
					   );
         delete fd;
         m_t << "</programlisting>";
      }
      break;
    case DocInclude::Include:
      m_t << "<programlisting filename=\"" << inc->file() << "\">";
      getCodeParser(inc->extension()).parseCode(m_ci,inc->context(),
                                        inc->text(),
                                        langExt,
                                        inc->isExample(),
                                        inc->exampleFile(),
                                        0,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        TRUE,  // inlineFragment
                                        0,     // memberDef
                                        FALSE  // show line numbers
				       );
      m_t << "</programlisting>";
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
      break;
    case DocInclude::HtmlInclude:
      if (inc->isBlock())
      {
        m_t << "<htmlonly block=\"yes\">";
      }
      else
      {
        m_t << "<htmlonly>";
      }
      filter(inc->text());
      m_t << "</htmlonly>";
      break;
    case DocInclude::LatexInclude:
      m_t << "<latexonly>";
      filter(inc->text());
      m_t << "</latexonly>";
      break;
    case DocInclude::RtfInclude:
      m_t << "<rtfonly>";
      filter(inc->text());
      m_t << "</rtfonly>";
      break;
    case DocInclude::ManInclude:
      m_t << "<manonly>";
      filter(inc->text());
      m_t << "</manonly>";
      break;
    case DocInclude::XmlInclude:
      filter(inc->text());
      break;
    case DocInclude::DocbookInclude:
      m_t << "<docbookonly>";
      filter(inc->text());
      m_t << "</docbookonly>";
      break;
    case DocInclude::VerbInclude:
      m_t << "<verbatim>";
      filter(inc->text());
      m_t << "</verbatim>";
      break;
    case DocInclude::Snippet:
      m_t << "<programlisting filename=\"" << inc->file() << "\">";
      getCodeParser(inc->extension()).parseCode(m_ci,
                                        inc->context(),
                                        extractBlock(inc->text(),inc->blockId()),
                                        langExt,
                                        inc->isExample(),
                                        inc->exampleFile()
                                       );
      m_t << "</programlisting>";
      break;
    case DocInclude::SnipWithLines:
      {
         m_t << "<programlisting filename=\"" << inc->file() << "\">";
         FileInfo cfi( inc->file().str() );
         FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         getCodeParser(inc->extension()).parseCode(m_ci,
                                           inc->context(),
                                           extractBlock(inc->text(),inc->blockId()),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile(),
                                           fd,
                                           lineBlock(inc->text(),inc->blockId()),
                                           -1,    // endLine
                                           FALSE, // inlineFragment
                                           0,     // memberDef
                                           TRUE   // show line number
                                          );
         delete fd;
         m_t << "</programlisting>";
      }
      break;
    case DocInclude::SnippetDoc:
    case DocInclude::IncludeDoc:
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
}

void XmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),qPrint(op->text()));
  if (op->isFirst())
  {
    if (!m_hide)
    {
      m_t << "<programlisting filename=\"" << op->includeFileName() << "\">";
    }
    pushHidden(m_hide);
    m_hide = TRUE;
  }
  QCString locLangExt = getFileNameExtension(op->includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  if (op->type()!=DocIncOperator::Skip)
  {
    m_hide = popHidden();
    if (!m_hide)
    {
      FileDef *fd = 0;
      if (!op->includeFileName().isEmpty())
      {
        FileInfo cfi( op->includeFileName().str() );
        fd = createFileDef( cfi.dirPath(), cfi.fileName() );
      }

      getCodeParser(locLangExt).parseCode(m_ci,op->context(),
                                          op->text(),langExt,op->isExample(),
                                          op->exampleFile(),
                                          fd,     // fileDef
                                          op->line(),    // startLine
                                          -1,    // endLine
                                          FALSE, // inline fragment
                                          0,     // memberDef
                                          op->showLineNo()  // show line numbers
                                         );
      if (fd) delete fd;
    }
    pushHidden(m_hide);
    m_hide=TRUE;
  }
  if (op->isLast())
  {
    m_hide = popHidden();
    if (!m_hide) m_t << "</programlisting>";
  }
  else
  {
    if (!m_hide) m_t << "\n";
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
  if (m_hide) return;
  m_t << "<indexentry>"
           "<primaryie>";
  filter(ie->entry());
  m_t << "</primaryie>"
           "<secondaryie></secondaryie>"
         "</indexentry>";
}

void XmlDocVisitor::visit(DocSimpleSectSep *sep)
{
  if (sep->parent() && sep->parent()->kind()==DocNode::Kind_SimpleSect)
  {
    visitPost(dynamic_cast<DocSimpleSect*>(sep->parent())); // end current section
    visitPre(dynamic_cast<DocSimpleSect*>(sep->parent()));  // start new section
  }
}

void XmlDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  if (!cite->file().isEmpty()) startLink(cite->ref(),cite->file(),cite->anchor());
  filter(cite->text());
  if (!cite->file().isEmpty()) endLink();
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void XmlDocVisitor::visitPre(DocAutoList *l)
{
  if (m_hide) return;
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
  if (m_hide) return;
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
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>";
}

void XmlDocVisitor::visitPre(DocPara *)
{
  if (m_hide) return;
  m_t << "<para>";
}

void XmlDocVisitor::visitPost(DocPara *)
{
  if (m_hide) return;
  m_t << "</para>\n";
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
  if (m_hide) return;
  m_t << "<simplesect kind=\"";
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
      m_t << "note"; break;
    case DocSimpleSect::Warning:
      m_t << "warning"; break;
    case DocSimpleSect::Pre:
      m_t << "pre"; break;
    case DocSimpleSect::Post:
      m_t << "post"; break;
    case DocSimpleSect::Copyright:
      m_t << "copyright"; break;
    case DocSimpleSect::Invar:
      m_t << "invariant"; break;
    case DocSimpleSect::Remark:
      m_t << "remark"; break;
    case DocSimpleSect::Attention:
      m_t << "attention"; break;
    case DocSimpleSect::User:
      m_t << "par"; break;
    case DocSimpleSect::Rcs:
      m_t << "rcs"; break;
    case DocSimpleSect::Unknown:  break;
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  m_t << "</simplesect>\n";
}

void XmlDocVisitor::visitPre(DocTitle *)
{
  if (m_hide) return;
  m_t << "<title>";
}

void XmlDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "</title>";
}

void XmlDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  m_t << "<sect" << s->level() << " id=\"" << s->file();
  if (!s->anchor().isEmpty()) m_t << "_1" << s->anchor();
  m_t << "\">\n";
  m_t << "<title>";
  filter(convertCharEntitiesToUTF8(s->title()));
  m_t << "</title>\n";
}

void XmlDocVisitor::visitPost(DocSection *s)
{
  m_t << "</sect" << s->level() << ">\n";
}

void XmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered)
  {
    m_t << "<orderedlist";
    for (const auto &opt : s->attribs())
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
    m_t << ">\n";
  }
  else
    m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered)
    m_t << "</orderedlist>\n";
  else
    m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlListItem *l)
{
  if (m_hide) return;
  m_t << "<listitem";
  for (const auto &opt : l->attribs())
  {
    if (opt.name=="value")
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
  }
  m_t << ">\n";
}

void XmlDocVisitor::visitPost(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "<variablelist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "</variablelist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "<varlistentry><term>";
}

void XmlDocVisitor::visitPost(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "</term></varlistentry>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescData *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocHtmlDescData *)
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;
  m_t << "<table rows=\"" << t->numRows()
      << "\" cols=\"" << t->numColumns() << "\"" ;
  for (const auto &opt : t->attribs())
  {
    if (opt.name=="width")
    {
      m_t << " " << opt.name << "=\"" << opt.value << "\"";
    }
  }
  m_t << ">";
  if (t->hasCaption())
  {
    DocHtmlCaption *c = t->caption();
    m_t << "<caption";
    if (!c->file().isEmpty())
    {
      m_t << " id=\""  << stripPath(c->file()) << "_1" << c->anchor() << "\"";
    }
    m_t << ">";
  }
}

void XmlDocVisitor::visitPost(DocHtmlTable *)
{
  if (m_hide) return;
  m_t << "</table>\n";
}

void XmlDocVisitor::visitPre(DocHtmlRow *)
{
  if (m_hide) return;
  m_t << "<row>\n";
}

void XmlDocVisitor::visitPost(DocHtmlRow *)
{
  if (m_hide) return;
  m_t << "</row>\n";
}

void XmlDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;
  if (c->isHeading()) m_t << "<entry thead=\"yes\""; else m_t << "<entry thead=\"no\"";
  for (const auto &opt : c->attribs())
  {
    if (opt.name=="colspan" || opt.name=="rowspan")
    {
      m_t << " " << opt.name << "=\"" << opt.value.toInt() << "\"";
    }
    else if (opt.name=="align" &&
             (opt.value=="right" || opt.value=="left" || opt.value=="center"))
    {
      m_t << " align=\"" << opt.value << "\"";
    }
    else if (opt.name=="valign" &&
      (opt.value == "bottom" || opt.value == "top" || opt.value == "middle"))
    {
      m_t << " valign=\"" << opt.value << "\"";
    }
    else if (opt.name=="width")
    {
      m_t << " width=\"" << opt.value << "\"";
    }
    else if (opt.name=="class") // handle markdown generated attributes
    {
      if (opt.value.left(13)=="markdownTable") // handle markdown generated attributes
      {
        if (opt.value.right(5)=="Right")
        {
          m_t << " align='right'";
        }
        else if (opt.value.right(4)=="Left")
        {
          m_t << " align='left'";
        }
        else if (opt.value.right(6)=="Center")
        {
          m_t << " align='center'";
        }
        // skip 'markdownTable*' value ending with "None"
      }
      else if (!opt.value.isEmpty())
      {
        m_t << " class=\"" << convertToXML(opt.value) << "\"";
      }
    }
  }
  m_t << ">";
}

void XmlDocVisitor::visitPost(DocHtmlCell *)
{
  if (m_hide) return;
  m_t << "</entry>";
}

void XmlDocVisitor::visitPre(DocHtmlCaption *)
{
  if (m_hide) return;
  // start of caption is handled in the XmlDocVisitor::visitPre(DocHtmlTable *t)
}

void XmlDocVisitor::visitPost(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "</caption>\n";
}

void XmlDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  m_t << "<internal>";
}

void XmlDocVisitor::visitPost(DocInternal *)
{
  if (m_hide) return;
  m_t << "</internal>\n";
}

void XmlDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  m_t << "<ulink url=\"" << convertToXML(href->url(), TRUE) << "\">";
}

void XmlDocVisitor::visitPost(DocHRef *)
{
  if (m_hide) return;
  m_t << "</ulink>";
}

void XmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  m_t << "<heading level=\"" << header->level() << "\">";
}

void XmlDocVisitor::visitPost(DocHtmlHeader *)
{
  if (m_hide) return;
  m_t << "</heading>\n";
}

void XmlDocVisitor::visitPre(DocImage *img)
{
  if (m_hide) return;

  QCString url = img->url();
  QCString baseName;
  if (url.isEmpty())
  {
    baseName = img->relPath()+img->name();
  }
  else
  {
    baseName = correctURL(url,img->relPath());
  }
  HtmlAttribList attribs = img->attribs();
  auto it = std::find_if(attribs.begin(),attribs.end(),
                         [](const auto &att) { return att.name=="alt"; });
  QCString altValue = it!=attribs.end() ? it->value : "";
  visitPreStart(m_t, "image", FALSE, this, img->children(), baseName, TRUE,
                img->type(), img->width(), img->height(), QCString(),
                altValue, img->isInlineImage());

  // copy the image to the output dir
  FileDef *fd;
  bool ambig;
  if (url.isEmpty() && (fd=findFileDef(Doxygen::imageNameLinkedMap,img->name(),ambig)))
  {
    copyFile(fd->absFilePath(),Config_getString(XML_OUTPUT)+"/"+img->name());
  }
}

void XmlDocVisitor::visitPost(DocImage *)
{
  if (m_hide) return;
  visitPostEnd(m_t, "image");
}

void XmlDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  copyFile(df->file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df->file()));
  visitPreStart(m_t, "dotfile", FALSE, this, df->children(), stripPath(df->file()), FALSE, DocImage::Html, df->width(), df->height());
}

void XmlDocVisitor::visitPost(DocDotFile *)
{
  if (m_hide) return;
  visitPostEnd(m_t, "dotfile");
}

void XmlDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  copyFile(df->file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df->file()));
  visitPreStart(m_t, "mscfile", FALSE, this, df->children(), stripPath(df->file()), FALSE, DocImage::Html, df->width(), df->height());
}

void XmlDocVisitor::visitPost(DocMscFile *)
{
  if (m_hide) return;
  visitPostEnd(m_t, "mscfile");
}

void XmlDocVisitor::visitPre(DocDiaFile *df)
{
  if (m_hide) return;
  copyFile(df->file(),Config_getString(XML_OUTPUT)+"/"+stripPath(df->file()));
  visitPreStart(m_t, "diafile", FALSE, this, df->children(), stripPath(df->file()), FALSE, DocImage::Html, df->width(), df->height());
}

void XmlDocVisitor::visitPost(DocDiaFile *)
{
  if (m_hide) return;
  visitPostEnd(m_t, "diafile");
}

void XmlDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void XmlDocVisitor::visitPost(DocLink *)
{
  if (m_hide) return;
  endLink();
}

void XmlDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty())
  {
    startLink(ref->ref(),ref->file(),ref->isSubPage() ? QCString() : ref->anchor());
  }
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void XmlDocVisitor::visitPost(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink();
  //m_t << " ";
}

void XmlDocVisitor::visitPre(DocSecRefItem *ref)
{
  if (m_hide) return;
  m_t << "<tocitem id=\"" << ref->file();
  if (!ref->anchor().isEmpty()) m_t << "_1" << ref->anchor();
  m_t << "\"";
  m_t << ">";
}

void XmlDocVisitor::visitPost(DocSecRefItem *)
{
  if (m_hide) return;
  m_t << "</tocitem>\n";
}

void XmlDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "<toclist>\n";
}

void XmlDocVisitor::visitPost(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "</toclist>\n";
}

//void XmlDocVisitor::visitPre(DocLanguage *l)
//{
//  if (m_hide) return;
//  m_t << "<language langid=\"" << l->id() << "\">";
//}
//
//void XmlDocVisitor::visitPost(DocLanguage *)
//{
//  if (m_hide) return;
//  m_t << "</language>\n";
//}

void XmlDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  m_t << "<parameterlist kind=\"";
  switch(s->type())
  {
    case DocParamSect::Param:
      m_t << "param"; break;
    case DocParamSect::RetVal:
      m_t << "retval"; break;
    case DocParamSect::Exception:
      m_t << "exception"; break;
    case DocParamSect::TemplateParam:
      m_t << "templateparam"; break;
    default:
      ASSERT(0);
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocParamSect *)
{
  if (m_hide) return;
  m_t << "</parameterlist>\n";
}

void XmlDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  m_t << "<parameteritem>\n";
  m_t << "<parameternamelist>\n";
  for (const auto &param : pl->parameters())
  {
    if (!pl->paramTypes().empty())
    {
      m_t << "<parametertype>";
      for (const auto &type : pl->paramTypes())
      {
        if (type->kind()==DocNode::Kind_Word)
        {
          visit(dynamic_cast<DocWord*>(type.get()));
        }
        else if (type->kind()==DocNode::Kind_LinkedWord)
        {
          visit(dynamic_cast<DocLinkedWord*>(type.get()));
        }
        else if (type->kind()==DocNode::Kind_Sep)
        {
          m_t << "</parametertype>\n";
          m_t << "<parametertype>";
        }
      }
      m_t << "</parametertype>\n";
    }
    m_t << "<parametername";
    if (pl->direction()!=DocParamSect::Unspecified)
    {
      m_t << " direction=\"";
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
        m_t << "inout";
      }
      m_t << "\"";
    }
    m_t << ">";
    if (param->kind()==DocNode::Kind_Word)
    {
      visit(dynamic_cast<DocWord*>(param.get()));
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      visit(dynamic_cast<DocLinkedWord*>(param.get()));
    }
    m_t << "</parametername>\n";
  }
  m_t << "</parameternamelist>\n";
  m_t << "<parameterdescription>\n";
}

void XmlDocVisitor::visitPost(DocParamList *)
{
  if (m_hide) return;
  m_t << "</parameterdescription>\n";
  m_t << "</parameteritem>\n";
}

void XmlDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "<xrefsect id=\"";
  m_t << x->file() << "_1" << x->anchor();
  m_t << "\">";
  m_t << "<xreftitle>";
  filter(x->title());
  m_t << "</xreftitle>";
  m_t << "<xrefdescription>";
}

void XmlDocVisitor::visitPost(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "</xrefdescription>";
  m_t << "</xrefsect>";
}

void XmlDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(QCString(),ref->file(),ref->anchor());
}

void XmlDocVisitor::visitPost(DocInternalRef *)
{
  if (m_hide) return;
  endLink();
  m_t << " ";
}

void XmlDocVisitor::visitPre(DocText *)
{
}

void XmlDocVisitor::visitPost(DocText *)
{
}

void XmlDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "<blockquote>";
}

void XmlDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "</blockquote>";
}

void XmlDocVisitor::visitPre(DocVhdlFlow *)
{
}

void XmlDocVisitor::visitPost(DocVhdlFlow *)
{
}

void XmlDocVisitor::visitPre(DocParBlock *)
{
  if (m_hide) return;
  m_t << "<parblock>";
}

void XmlDocVisitor::visitPost(DocParBlock *)
{
  if (m_hide) return;
  m_t << "</parblock>";
}


void XmlDocVisitor::filter(const QCString &str)
{
  m_t << convertToXML(str);
}

void XmlDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  //printf("XmlDocVisitor: file=%s anchor=%s\n",qPrint(file),qPrint(anchor));
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

