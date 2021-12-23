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

#include "mandocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "code.h"
#include "dot.h"
#include "util.h"
#include "message.h"
#include "parserintf.h"
#include "filedef.h"
#include "htmlentity.h"
#include "emoji.h"
#include "fileinfo.h"

ManListItemInfo man_listItemInfo[man_maxIndentLevels];

ManDocVisitor::ManDocVisitor(TextStream &t,CodeOutputInterface &ci,
                             const QCString &langExt)
  : DocVisitor(DocVisitor_Man), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE), m_firstCol(FALSE),
    m_indent(0), m_langExt(langExt)
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void ManDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
  m_firstCol=FALSE;
}

void ManDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  m_t << "\\fB";
  filter(w->word());
  m_t << "\\fP";
  m_firstCol=FALSE;
}

void ManDocVisitor::visit(DocWhiteSpace *w)
{
  if (m_hide) return;
  if (m_insidePre)
  {
    m_t << w->chars();
    m_firstCol=w->chars().at(w->chars().length()-1)=='\n';
  }
  else
  {
    m_t << " ";
    m_firstCol=FALSE;
  }
}

void ManDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance()->man(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    // no error or warning to be supplied
    // err("man: non supported HTML-entity found: &%s;\n",get_symbol_item(s->symbol()));
  }
  m_firstCol=FALSE;
}

void ManDocVisitor::visit(DocEmoji *s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance()->name(s->index());
  if (res)
  {
    m_t << res;
  }
  else
  {
    m_t << s->name();
  }
  m_firstCol=FALSE;
}

void ManDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  m_t << u->url();
  m_firstCol=FALSE;
}

void ManDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "\n.br\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "\\fB";      else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::S:
    case DocStyleChange::Strike:
    case DocStyleChange::Del:
      /* not supported */
      break;
    case DocStyleChange::Underline: //underline is shown as emphasis
    case DocStyleChange::Ins:
      if (s->enable()) m_t << "\\fI";     else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "\\fI";     else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "\\fC";   else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "\\*<";    else m_t << "\\*> ";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "\\*{";    else m_t << "\\*} ";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Center:
      /* not supported */
      break;
    case DocStyleChange::Small:
      /* not supported */
      break;
    case DocStyleChange::Cite:
      /* not supported */
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        if (!m_firstCol) m_t << "\n";
        m_t << ".PP\n";
        m_t << ".nf\n";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        if (!m_firstCol) m_t << "\n";
        m_t << ".fi\n";
        m_t << ".PP\n";
        m_firstCol=TRUE;
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void ManDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  QCString lang = m_langExt;
  if (!s->language().isEmpty()) // explicit language setting
  {
    lang = s->language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch (s->type())
  {
    case DocVerbatim::Code:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      getCodeParser(lang).parseCode(m_ci,s->context(),s->text(),
                                        langExt,
                                        s->isExample(),s->exampleFile());
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s->text());
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "\\fC\n";
      filter(s->text());
      m_t << "\\fP\n";
      break;
    case DocVerbatim::Verbatim:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      filter(s->text());
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocVerbatim::ManOnly:
      m_t << s->text();
      break;
    case DocVerbatim::HtmlOnly:
    case DocVerbatim::XmlOnly:
    case DocVerbatim::LatexOnly:
    case DocVerbatim::RtfOnly:
    case DocVerbatim::DocbookOnly:
    case DocVerbatim::Dot:
    case DocVerbatim::Msc:
    case DocVerbatim::PlantUML:
      /* nothing */
      break;
  }
}

void ManDocVisitor::visit(DocAnchor *)
{
  /* no support for anchors in man pages */
}

void ManDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      {
         if (!m_firstCol) m_t << "\n";
         m_t << ".PP\n";
         m_t << ".nf\n";
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
                                           TRUE
					   );
         delete fd;
         if (!m_firstCol) m_t << "\n";
         m_t << ".fi\n";
         m_t << ".PP\n";
         m_firstCol=TRUE;
      }
      break;
    case DocInclude::Include:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
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
                                        FALSE
				       );
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
    case DocInclude::HtmlInclude:
    case DocInclude::LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      break;
    case DocInclude::ManInclude:
      m_t << inc->text();
      break;
    case DocInclude::VerbInclude:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      m_t << inc->text();
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocInclude::Snippet:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      getCodeParser(inc->extension()).parseCode(m_ci,
                                        inc->context(),
                                        extractBlock(inc->text(),inc->blockId()),
                                        langExt,
                                        inc->isExample(),
                                        inc->exampleFile()
                                       );
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocInclude::SnipWithLines:
      {
         if (!m_firstCol) m_t << "\n";
         m_t << ".PP\n";
         m_t << ".nf\n";
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
         if (!m_firstCol) m_t << "\n";
         m_t << ".fi\n";
         m_t << ".PP\n";
         m_firstCol=TRUE;
      }
      break;
    case DocInclude::SnippetDoc:
    case DocInclude::IncludeDoc:
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
}

void ManDocVisitor::visit(DocIncOperator *op)
{
  QCString locLangExt = getFileNameExtension(op->includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),qPrint(op->text()));
  if (op->isFirst())
  {
    if (!m_hide)
    {
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
    }
    pushHidden(m_hide);
    m_hide = TRUE;
  }
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

      getCodeParser(locLangExt).parseCode(m_ci,op->context(),op->text(),langExt,
                                        op->isExample(),op->exampleFile(),
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
    if (!m_hide)
    {
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
    }
  }
  else
  {
    if (!m_hide) m_t << "\n";
  }
}

void ManDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << f->text();
}

void ManDocVisitor::visit(DocIndexEntry *)
{
}

void ManDocVisitor::visit(DocSimpleSectSep *)
{
}

void ManDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  m_t << "\\fB";
  if (cite->file().isEmpty()) m_t << "[";
  filter(cite->text());
  if (cite->file().isEmpty()) m_t << "]";
  m_t << "\\fP";
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void ManDocVisitor::visitPre(DocAutoList *)
{
  if (m_hide) return;
  m_indent+=2;
}

void ManDocVisitor::visitPost(DocAutoList *)
{
  if (m_hide) return;
  m_indent-=2;
  m_t << ".PP\n";
}

void ManDocVisitor::visitPre(DocAutoListItem *li)
{
  if (m_hide) return;
  QCString ws;
  ws.fill(' ',m_indent-2);
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws;
  if (((DocAutoList *)li->parent())->isEnumList())
  {
    m_t << li->itemNumber() << ".\" " << m_indent+2;
  }
  else // bullet list
  {
    m_t << "\\(bu\" " << m_indent;
  }
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPost(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocPara *)
{
}

void ManDocVisitor::visitPost(DocPara *p)
{
  if (m_hide) return;
  if (!p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameter sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       )
     )
  {
    if (!m_firstCol) m_t << "\n";
    m_t << ".PP\n";
    m_firstCol=TRUE;
  }
}

void ManDocVisitor::visitPre(DocRoot *)
{
}

void ManDocVisitor::visitPost(DocRoot *)
{
}

void ManDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
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
    case DocSimpleSect::Copyright:
      m_t << theTranslator->trCopyright(); break;
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
    m_t << "\\fP\n";
    m_t << ".RS 4\n";
  }
}

void ManDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocTitle *)
{
}

void ManDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "\\fP\n";
  m_t << ".RS 4\n";
}

void ManDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_indent+=2;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PD 0\n";
}

void ManDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_indent-=2;
  m_t << ".PP\n";
}

void ManDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  QCString ws;
  ws.fill(' ',m_indent-2);
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws << "\\(bu\" " << m_indent << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPost(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  if (s->level()==1) m_t << ".SH"; else m_t << ".SS";
  m_t << " \"";
  filter(s->title());
  m_t << "\"\n";
  if (s->level()==1) m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPost(DocSection *)
{
}

void ManDocVisitor::visitPre(DocHtmlList *l)
{
  if (m_hide) return;
  m_indent+=2;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PD 0\n";
  man_listItemInfo[m_indent].number = 1;
  man_listItemInfo[m_indent].type   = '1';
  for (const auto &opt : l->attribs())
  {
    if (opt.name=="type")
    {
      man_listItemInfo[m_indent].type = opt.value[0];
    }
    if (opt.name=="start")
    {
      bool ok;
      int val = opt.value.toInt(&ok);
      if (ok) man_listItemInfo[m_indent].number = val;
    }
  }
}

void ManDocVisitor::visitPost(DocHtmlList *)
{
  if (m_hide) return;
  m_indent-=2;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
}

void ManDocVisitor::visitPre(DocHtmlListItem *li)
{
  if (m_hide) return;
  QCString ws;
  ws.fill(' ',m_indent-2);
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws;
  if (((DocHtmlList *)li->parent())->type()==DocHtmlList::Ordered)
  {
    for (const auto &opt : li->attribs())
    {
      if (opt.name=="value")
      {
        bool ok;
        int val = opt.value.toInt(&ok);
        if (ok) man_listItemInfo[m_indent].number = val;
      }
    }
    switch (man_listItemInfo[m_indent].type)
    {
      case '1':
        m_t << man_listItemInfo[m_indent].number;
        break;
      case 'a':
        m_t << integerToAlpha(man_listItemInfo[m_indent].number,false);
        break;
      case 'A':
        m_t << integerToAlpha(man_listItemInfo[m_indent].number);
        break;
      case 'i':
        m_t << integerToRoman(man_listItemInfo[m_indent].number,false);
        break;
      case 'I':
        m_t << integerToRoman(man_listItemInfo[m_indent].number);
        break;
      default:
        m_t << man_listItemInfo[m_indent].number;
        break;
    }
    m_t << ".\" " << m_indent+2;
    man_listItemInfo[m_indent].number++;
  }
  else // bullet list
  {
    m_t << "\\(bu\" " << m_indent;
  }
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPost(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "\n";
  m_firstCol=TRUE;
}

//void ManDocVisitor::visitPre(DocHtmlPre *)
//{
//  if (!m_firstCol) m_t << "\n";
//  m_t << ".PP\n";
//  m_t << ".nf\n";
//  m_insidePre=TRUE;
//}
//
//void ManDocVisitor::visitPost(DocHtmlPre *)
//{
//  m_insidePre=FALSE;
//  if (!m_firstCol) m_t << "\n";
//  m_t << ".fi\n";
//  m_t << ".PP\n";
//  m_firstCol=TRUE;
//}

void ManDocVisitor::visitPre(DocHtmlDescList *)
{
}

void ManDocVisitor::visitPost(DocHtmlDescList *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"\\fB";
  m_firstCol=FALSE;
}

void ManDocVisitor::visitPost(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "\\fP\" 1c\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocHtmlDescData *)
{
}

void ManDocVisitor::visitPost(DocHtmlDescData *)
{
}

void ManDocVisitor::visitPre(DocHtmlTable *)
{
}

void ManDocVisitor::visitPost(DocHtmlTable *)
{
}

void ManDocVisitor::visitPre(DocHtmlCaption *)
{
}

void ManDocVisitor::visitPost(DocHtmlCaption *)
{
}

void ManDocVisitor::visitPre(DocHtmlRow *)
{
}

void ManDocVisitor::visitPost(DocHtmlRow *)
{
}

void ManDocVisitor::visitPre(DocHtmlCell *)
{
}

void ManDocVisitor::visitPost(DocHtmlCell *)
{
}

void ManDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  //if (!m_firstCol) m_t << "\n";
  //m_t << ".PP\n";
  //m_t << "\\fB" << theTranslator->trForInternalUseOnly() << "\\fP\n";
  //m_t << ".RS 4\n";
}

void ManDocVisitor::visitPost(DocInternal *)
{
  if (m_hide) return;
  //if (!m_firstCol) m_t << "\n";
  //m_t << ".RE\n";
  //m_t << ".PP\n";
  //m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocHRef *)
{
  if (m_hide) return;
  m_t << "\\fC";
}

void ManDocVisitor::visitPost(DocHRef *)
{
  if (m_hide) return;
  m_t << "\\fP";
}

void ManDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  if (header->level()==1) m_t << ".SH"; else m_t << ".SS";
  m_t << " \"";
}

void ManDocVisitor::visitPost(DocHtmlHeader *header)
{
  if (m_hide) return;
  m_t << "\"\n";
  if (header->level()==1) m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocImage *)
{
}

void ManDocVisitor::visitPost(DocImage *)
{
}

void ManDocVisitor::visitPre(DocDotFile *)
{
}

void ManDocVisitor::visitPost(DocDotFile *)
{
}
void ManDocVisitor::visitPre(DocMscFile *)
{
}

void ManDocVisitor::visitPost(DocMscFile *)
{
}

void ManDocVisitor::visitPre(DocDiaFile *)
{
}

void ManDocVisitor::visitPost(DocDiaFile *)
{
}

void ManDocVisitor::visitPre(DocLink *)
{
  if (m_hide) return;
  m_t << "\\fB";
}

void ManDocVisitor::visitPost(DocLink *)
{
  if (m_hide) return;
  m_t << "\\fP";
}

void ManDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  m_t << "\\fB";
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void ManDocVisitor::visitPost(DocRef *)
{
  if (m_hide) return;
  m_t << "\\fP";
}

void ManDocVisitor::visitPre(DocSecRefItem *)
{
  if (m_hide) return;
  QCString ws;
  ws.fill(' ',m_indent-2);
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws << "\\(bu\" " << m_indent << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPost(DocSecRefItem *)
{
  if (m_hide) return;
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_indent+=2;
}

void ManDocVisitor::visitPost(DocSecRefList *)
{
  if (m_hide) return;
  m_indent-=2;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
}

void ManDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
  switch(s->type())
  {
    case DocParamSect::Param:
      m_t << theTranslator->trParameters(); break;
    case DocParamSect::RetVal:
      m_t << theTranslator->trReturnValues(); break;
    case DocParamSect::Exception:
      m_t << theTranslator->trExceptions(); break;
    case DocParamSect::TemplateParam:
      m_t << theTranslator->trTemplateParameters(); break;
    default:
      ASSERT(0);
  }
  m_t << "\\fP\n";
  m_t << ".RS 4\n";
}

void ManDocVisitor::visitPost(DocParamSect *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  m_t << "\\fI";
  bool first=TRUE;
  for (const auto &param : pl->parameters())
  {
    if (!first) m_t << ","; else first=FALSE;
    if (param->kind()==DocNode::Kind_Word)
    {
      visit((DocWord*)param.get());
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      visit((DocLinkedWord*)param.get());
    }
  }
  m_t << "\\fP ";
}

void ManDocVisitor::visitPost(DocParamList *pl)
{
  if (m_hide) return;
  if (!pl->isLast())
  {
    if (!m_firstCol) m_t << "\n";
    m_t << ".br\n";
  }
}

void ManDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
  filter(x->title());
  m_t << "\\fP\n";
  m_t << ".RS 4\n";
}

void ManDocVisitor::visitPost(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocInternalRef *)
{
  if (m_hide) return;
  m_t << "\\fB";
}

void ManDocVisitor::visitPost(DocInternalRef *)
{
  if (m_hide) return;
  m_t << "\\fP";
}

void ManDocVisitor::visitPre(DocText *)
{
}

void ManDocVisitor::visitPost(DocText *)
{
}

void ManDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << ".RS 4\n"; // TODO: add support for nested block quotes
}

void ManDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::visitPre(DocVhdlFlow *)
{
}

void ManDocVisitor::visitPost(DocVhdlFlow *)
{
}

void ManDocVisitor::visitPre(DocParBlock *)
{
}

void ManDocVisitor::visitPost(DocParBlock *)
{
}


void ManDocVisitor::filter(const QCString &str)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
    char c=0;
    while ((c=*p++))
    {
      switch(c)
      {
        case '.':  m_t << "\\&."; break; // see  bug652277
        case '\\': m_t << "\\\\"; break;
        case '"':  c = '\''; // fall through
        default: m_t << c; break;
      }
    }
  }
}

