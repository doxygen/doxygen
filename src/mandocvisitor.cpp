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
#include "codefragment.h"

ManDocVisitor::ManDocVisitor(TextStream &t,OutputCodeList &ci,
                             const QCString &langExt)
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE), m_firstCol(FALSE),
    m_indent(0), m_langExt(langExt)
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void ManDocVisitor::operator()(const DocWord &w)
{
  if (m_hide) return;
  filter(w.word());
  m_firstCol=FALSE;
}

void ManDocVisitor::operator()(const DocLinkedWord &w)
{
  if (m_hide) return;
  m_t << "\\fB";
  filter(w.word());
  m_t << "\\fP";
  m_firstCol=FALSE;
}

void ManDocVisitor::operator()(const DocWhiteSpace &w)
{
  if (m_hide) return;
  if (m_insidePre)
  {
    m_t << w.chars();
    m_firstCol=w.chars().at(w.chars().length()-1)=='\n';
  }
  else
  {
    m_t << " ";
    m_firstCol=FALSE;
  }
}

void ManDocVisitor::operator()(const DocSymbol &s)
{
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance().man(s.symbol());
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

void ManDocVisitor::operator()(const DocEmoji &s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance().name(s.index());
  if (res)
  {
    m_t << res;
  }
  else
  {
    m_t << s.name();
  }
  m_firstCol=FALSE;
}

void ManDocVisitor::operator()(const DocURL &u)
{
  if (m_hide) return;
  m_t << u.url();
  m_firstCol=FALSE;
}

void ManDocVisitor::operator()(const DocLineBreak &)
{
  if (m_hide) return;
  m_t << "\n.br\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocHorRuler &)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocStyleChange &s)
{
  if (m_hide) return;
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) m_t << "\\fB";      else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::S:
    case DocStyleChange::Strike:
    case DocStyleChange::Del:
      /* not supported */
      break;
    case DocStyleChange::Underline: //underline is shown as emphasis
    case DocStyleChange::Ins:
      if (s.enable()) m_t << "\\fI";     else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Italic:
      if (s.enable()) m_t << "\\fI";     else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Code:
      if (s.enable()) m_t << "\\fR";   else m_t << "\\fP";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Subscript:
      if (s.enable()) m_t << "\\*<";    else m_t << "\\*> ";
      m_firstCol=FALSE;
      break;
    case DocStyleChange::Superscript:
      if (s.enable()) m_t << "\\*{";    else m_t << "\\*} ";
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
      if (s.enable())
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

void ManDocVisitor::operator()(const DocVerbatim &s)
{
  if (m_hide) return;
  QCString lang = m_langExt;
  if (!s.language().isEmpty()) // explicit language setting
  {
    lang = s.language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch (s.type())
  {
    case DocVerbatim::Code:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      getCodeParser(lang).parseCode(m_ci,s.context(),s.text(),
                                        langExt,
                                        Config_getBool(STRIP_CODE_COMMENTS),
                                        s.isExample(),s.exampleFile());
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s.text());
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "\\fR\n";
      filter(s.text());
      m_t << "\\fP\n";
      break;
    case DocVerbatim::Verbatim:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      filter(s.text());
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocVerbatim::ManOnly:
      m_t << s.text();
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

void ManDocVisitor::operator()(const DocAnchor &)
{
  /* no support for anchors in man pages */
}

void ManDocVisitor::operator()(const DocInclude &inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc.extension());
  switch(inc.type())
  {
    case DocInclude::IncWithLines:
      {
         if (!m_firstCol) m_t << "\n";
         m_t << ".PP\n";
         m_t << ".nf\n";
         FileInfo cfi( inc.file().str() );
         auto fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                           inc.text(),
                                           langExt,
                                           inc.stripCodeComments(),
                                           inc.isExample(),
                                           inc.exampleFile(),
                                           fd.get(), // fileDef,
                                           -1,    // start line
                                           -1,    // end line
                                           FALSE, // inline fragment
                                           nullptr,     // memberDef
                                           TRUE
					   );
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
      getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                        inc.text(),
                                        langExt,
                                        inc.stripCodeComments(),
                                        inc.isExample(),
                                        inc.exampleFile(),
                                        nullptr,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        TRUE,  // inlineFragment
                                        nullptr,     // memberDef
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
      m_t << inc.text();
      break;
    case DocInclude::VerbInclude:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      m_t << inc.text();
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
    case DocInclude::Snippet:
    case DocInclude::SnippetWithLines:
      if (!m_firstCol) m_t << "\n";
      m_t << ".PP\n";
      m_t << ".nf\n";
      CodeFragmentManager::instance().parseCodeFragment(m_ci,
                                          inc.file(),
                                          inc.blockId(),
                                          inc.context(),
                                          inc.type()==DocInclude::SnippetWithLines,
                                          inc.trimLeft(),
                                          inc.stripCodeComments()
                                         );
      if (!m_firstCol) m_t << "\n";
      m_t << ".fi\n";
      m_t << ".PP\n";
      m_firstCol=TRUE;
      break;
  }
}

void ManDocVisitor::operator()(const DocIncOperator &op)
{
  QCString locLangExt = getFileNameExtension(op.includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op.type(),op.isFirst(),op.isLast(),qPrint(op.text()));
  if (op.isFirst())
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
  if (op.type()!=DocIncOperator::Skip)
  {
    m_hide = popHidden();
    if (!m_hide)
    {
      std::unique_ptr<FileDef> fd;
      if (!op.includeFileName().isEmpty())
      {
        FileInfo cfi( op.includeFileName().str() );
        fd = createFileDef( cfi.dirPath(), cfi.fileName() );
      }

      getCodeParser(locLangExt).parseCode(m_ci,op.context(),op.text(),langExt,
                                        op.stripCodeComments(),
                                        op.isExample(),op.exampleFile(),
                                        fd.get(),     // fileDef
                                        op.line(),    // startLine
                                        -1,    // endLine
                                        FALSE, // inline fragment
                                        nullptr,     // memberDef
                                        op.showLineNo()  // show line numbers
                                       );
    }
    pushHidden(m_hide);
    m_hide=TRUE;
  }
  if (op.isLast())
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

void ManDocVisitor::operator()(const DocFormula &f)
{
  if (m_hide) return;
  m_t << f.text();
}

void ManDocVisitor::operator()(const DocIndexEntry &)
{
}

void ManDocVisitor::operator()(const DocSimpleSectSep &)
{
}

void ManDocVisitor::operator()(const DocCite &cite)
{
  if (m_hide) return;
  m_t << "\\fB";
  if (cite.file().isEmpty()) m_t << "[";
  filter(cite.text());
  if (cite.file().isEmpty()) m_t << "]";
  m_t << "\\fP";
}

void ManDocVisitor::operator()(const DocSeparator &s)
{
  if (m_hide) return;
  m_t << s.chars();
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void ManDocVisitor::operator()(const DocAutoList &l)
{
  if (m_hide) return;
  m_indent++;
  visitChildren(l);
  m_indent--;
  m_t << ".PP\n";
}

void ManDocVisitor::operator()(const DocAutoListItem &li)
{
  if (m_hide) return;
  QCString ws;
  if (m_indent>0) ws.fill(' ',2*(m_indent-1));
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws;
  const DocAutoList *list = std::get_if<DocAutoList>(li.parent());
  if (list && list->isEnumList())
  {
    m_t << li.itemNumber() << ".\" " << (2*(m_indent+1));
  }
  else // bullet list
  {
    switch (li.itemNumber())
    {
      case DocAutoList::Unchecked: // unchecked
        m_t << "[ ]\" " << (2*m_indent) + 2;
        break;
      case DocAutoList::Checked_x: // checked with x
        m_t << "[x]\" " << (2*m_indent) + 2;
        break;
      case DocAutoList::Checked_X: // checked with X
        m_t << "[X]\" " << (2*m_indent) + 2;
        break;
      default:
        m_t << "\\(bu\" " << (2*m_indent);
        break;
    }
  }
  m_t << "\n";
  m_firstCol=TRUE;
  visitChildren(li);
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocPara &p)
{
  if (m_hide) return;
  visitChildren(p);
  if (!p.isLast() &&            // omit <p> for last paragraph
      !(p.parent() &&           // and for parameter sections
        std::get_if<DocParamSect>(p.parent())
       )
     )
  {
    if (!m_firstCol) m_t << "\n";
    m_t << "\n.PP\n";
    m_firstCol=TRUE;
  }
}

void ManDocVisitor::operator()(const DocRoot &r)
{
  visitChildren(r);
}

void ManDocVisitor::operator()(const DocSimpleSect &s)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
  switch(s.type())
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
    case DocSimpleSect::Important:
      m_t << theTranslator->trImportant(); break;
    case DocSimpleSect::User: break;
    case DocSimpleSect::Rcs: break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  m_t << "\\fP\n";
  m_t << ".RS 4\n";
  visitChildren(s);
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocTitle &t)
{
  if (m_hide) return;
  visitChildren(t);
}

void ManDocVisitor::operator()(const DocSimpleList &l)
{
  if (m_hide) return;
  m_indent++;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PD 0\n";
  m_firstCol=true;
  visitChildren(l);
  m_indent--;
  m_t << ".PP\n";
}

void ManDocVisitor::operator()(const DocSimpleListItem &li)
{
  if (m_hide) return;
  QCString ws;
  if (m_indent>0) ws.fill(' ',2*(m_indent-1));
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws << "\\(bu\" " << m_indent << "\n";
  m_firstCol=TRUE;
  if (li.paragraph())
  {
    visit(*this,*li.paragraph());
  }
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocSection &s)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  if (s.level()==1) m_t << ".SH"; else m_t << ".SS";
  m_t << " \"";
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  m_t << "\"\n";
  if (s.level()==1) m_t << ".PP\n";
  m_firstCol=TRUE;
  visitChildren(s);
}

void ManDocVisitor::operator()(const DocHtmlList &l)
{
  if (m_hide) return;
  m_indent++;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PD 0\n";
  m_firstCol=true;
  int indent = std::min(m_indent,maxIndentLevels-1);
  m_listItemInfo[indent].number = 1;
  m_listItemInfo[indent].type   = '1';
  for (const auto &opt : l.attribs())
  {
    if (opt.name=="type")
    {
      m_listItemInfo[indent].type = opt.value[0];
    }
    if (opt.name=="start")
    {
      bool ok = false;
      int val = opt.value.toInt(&ok);
      if (ok) m_listItemInfo[indent].number = val;
    }
  }
  visitChildren(l);
  m_indent--;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
}

void ManDocVisitor::operator()(const DocHtmlListItem &li)
{
  if (m_hide) return;
  QCString ws;
  if (m_indent>0) ws.fill(' ',2*(m_indent-1));
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws;
  const DocHtmlList *list = std::get_if<DocHtmlList>(li.parent());
  if (list && list->type()==DocHtmlList::Ordered)
  {
    int indent = std::min(m_indent,maxIndentLevels-1);
    for (const auto &opt : li.attribs())
    {
      if (opt.name=="value")
      {
        bool ok = false;
        int val = opt.value.toInt(&ok);
        if (ok) m_listItemInfo[indent].number = val;
      }
    }
    switch (m_listItemInfo[indent].type)
    {
      case '1':
        m_t << m_listItemInfo[indent].number;
        break;
      case 'a':
        m_t << integerToAlpha(m_listItemInfo[indent].number,false);
        break;
      case 'A':
        m_t << integerToAlpha(m_listItemInfo[indent].number);
        break;
      case 'i':
        m_t << integerToRoman(m_listItemInfo[indent].number,false);
        break;
      case 'I':
        m_t << integerToRoman(m_listItemInfo[indent].number);
        break;
      default:
        m_t << m_listItemInfo[indent].number;
        break;
    }
    m_t << ".\" " << ((m_indent+1)*2);
    m_listItemInfo[indent].number++;
  }
  else // bullet list
  {
    m_t << "\\(bu\" " << (m_indent*2);
  }
  m_t << "\n";
  m_firstCol=TRUE;
  visitChildren(li);
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocHtmlDescList &dl)
{
  if (m_hide) return;
  m_indent+=2;
  visitChildren(dl);
  m_indent-=2;
  if (!m_firstCol) m_t << "\n";
  m_t << "\n.PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  m_t << "\n.PP";
  m_t << "\n.IP \"\\fB";
  m_firstCol=FALSE;
  visitChildren(dt);
}

void ManDocVisitor::operator()(const DocHtmlDescData &dd)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"\" 1c\n";
  m_firstCol=TRUE;
  visitChildren(dd);
}

void ManDocVisitor::operator()(const DocHtmlTable &t)
{
  visitChildren(t);
}

void ManDocVisitor::operator()(const DocHtmlCaption &c)
{
  visitChildren(c);
}

void ManDocVisitor::operator()(const DocHtmlRow &r)
{
  visitChildren(r);
}

void ManDocVisitor::operator()(const DocHtmlCell &c)
{
  visitChildren(c);
}

void ManDocVisitor::operator()(const DocInternal &i)
{
  visitChildren(i);
}

void ManDocVisitor::operator()(const DocHRef &href)
{
  if (m_hide) return;
  m_t << "\\fR";
  visitChildren(href);
  m_t << "\\fP";
}

void ManDocVisitor::operator()(const DocHtmlSummary &s)
{
  m_t << "\\fB";
  visitChildren(s);
  m_t << "\\fP\n.PP\n";
}

void ManDocVisitor::operator()(const DocHtmlDetails &d)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  auto summary = d.summary();
  if (summary)
  {
    std::visit(*this,*summary);
    m_t << ".PP\n";
    m_t << ".RS 4\n";
  }
  visitChildren(d);
  if (!m_firstCol) m_t << "\n";
  if (summary)
  {
    m_t << ".RE\n";
  }
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocHtmlHeader &header)
{
  if (m_hide) return;
  if (!m_firstCol) m_t << "\n";
  if (header.level()==1) m_t << ".SH"; else m_t << ".SS";
  m_t << " \"";
  visitChildren(header);
  m_t << "\"\n";
  if (header.level()==1) m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocImage &)
{
}

void ManDocVisitor::operator()(const DocDotFile &)
{
}

void ManDocVisitor::operator()(const DocMscFile &)
{
}

void ManDocVisitor::operator()(const DocDiaFile &)
{
}

void ManDocVisitor::operator()(const DocPlantUmlFile &)
{
}

void ManDocVisitor::operator()(const DocLink &dl)
{
  if (m_hide) return;
  m_t << "\\fB";
  visitChildren(dl);
  m_t << "\\fP";
}

void ManDocVisitor::operator()(const DocRef &ref)
{
  if (m_hide) return;
  m_t << "\\fB";
  if (!ref.hasLinkText()) filter(ref.targetTitle());
  visitChildren(ref);
  m_t << "\\fP";
}

void ManDocVisitor::operator()(const DocSecRefItem &ref)
{
  if (m_hide) return;
  QCString ws;
  if (m_indent>0) ws.fill(' ',2*(m_indent-1));
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"" << ws << "\\(bu\" " << (2*m_indent) << "\n";
  m_firstCol=TRUE;
  visitChildren(ref);
  m_t << "\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocSecRefList &l)
{
  if (m_hide) return;
  m_indent++;
  visitChildren(l);
  m_indent--;
  if (!m_firstCol) m_t << "\n";
  m_t << ".PP\n";
}

void ManDocVisitor::operator()(const DocParamSect &s)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
  switch(s.type())
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
  visitChildren(s);
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocParamList &pl)
{
  if (m_hide) return;
  m_t << "\\fI";
  bool first=TRUE;
  for (const auto &param : pl.parameters())
  {
    if (!first) m_t << ","; else first=FALSE;
    std::visit(*this,param);
  }
  m_t << "\\fP ";
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  if (!pl.isLast())
  {
    if (!m_firstCol) m_t << "\n";
    m_t << ".br\n";
  }
}

void ManDocVisitor::operator()(const DocXRefItem &x)
{
  if (m_hide) return;
  if (x.title().isEmpty()) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << "\\fB";
  filter(x.title());
  m_t << "\\fP\n";
  m_t << ".RS 4\n";
  visitChildren(x);
  if (x.title().isEmpty()) return;
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocInternalRef &ref)
{
  if (m_hide) return;
  m_t << "\\fB";
  visitChildren(ref);
  m_t << "\\fP";
}

void ManDocVisitor::operator()(const DocText &t)
{
  visitChildren(t);
}

void ManDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
  if (m_hide) return;
  if (!m_firstCol)
  {
    m_t << "\n";
    m_t << ".PP\n";
  }
  m_t << ".RS 4\n"; // TODO: add support for nested block quotes
  visitChildren(q);
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManDocVisitor::operator()(const DocVhdlFlow &)
{
}

void ManDocVisitor::operator()(const DocParBlock &pb)
{
  visitChildren(pb);
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

