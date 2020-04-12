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

#include <qdir.h>
#include "htmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "dot.h"
#include "message.h"
#include "config.h"
#include "htmlgen.h"
#include "parserintf.h"
#include "msc.h"
#include "dia.h"
#include "util.h"
#include "vhdldocgen.h"
#include "filedef.h"
#include "memberdef.h"
#include "htmlentity.h"
#include "emoji.h"
#include "plantuml.h"
#include "formula.h"

static const int NUM_HTML_LIST_TYPES = 4;
static const char types[][NUM_HTML_LIST_TYPES] = {"1", "a", "i", "A"};
enum contexts_t
{
    NONE,      // 0
    STARTLI,   // 1
    STARTDD,   // 2
    ENDLI,     // 3
    ENDDD,     // 4
    STARTTD,   // 5
    ENDTD,     // 6
    INTERLI,   // 7
    INTERDD,   // 8
    INTERTD    // 9
};
static const char *contexts[10] = 
{ "",          // 0
  "startli",   // 1
  "startdd",   // 2
  "endli",     // 3
  "enddd",     // 4
  "starttd",   // 5
  "endtd",     // 6
  "interli",   // 7
  "interdd",   // 8
  "intertd"    // 9
};

static QCString convertIndexWordToAnchor(const QCString &word)
{
  static char hex[] = "0123456789abcdef";
  static int cnt = 0;
  QCString result="a";
  QCString cntStr;
  result += cntStr.setNum(cnt);
  result += "_";
  cnt++;
  const char *str = word.data();
  unsigned char c;
  if (str)
  {
    while ((c = *str++))
    {
      if ((c >= 'a' && c <= 'z') || // ALPHA
          (c >= 'A' && c <= 'Z') || // ALPHA
          (c >= '0' && c <= '9') || // DIGIT
          c == '-' ||
          c == '.' ||
          c == '_'
         )
      {
        result += c;
      }
      else
      {
        char enc[4];
        enc[0] = ':';
        enc[1] = hex[(c & 0xf0) >> 4];
        enc[2] = hex[c & 0xf];
        enc[3] = 0;
        result += enc;
      }
    }
  }
  return result;
}

static bool mustBeOutsideParagraph(DocNode *n)
{
  switch (n->kind())
  {
          /* <ul> */
        case DocNode::Kind_HtmlList:
        case DocNode::Kind_SimpleList:
        case DocNode::Kind_AutoList:
          /* <dl> */
        case DocNode::Kind_SimpleSect:
        case DocNode::Kind_ParamSect:
        case DocNode::Kind_HtmlDescList:
        case DocNode::Kind_XRefItem:
          /* <table> */
        case DocNode::Kind_HtmlTable:
          /* <h?> */
        case DocNode::Kind_Section:
        case DocNode::Kind_HtmlHeader:
          /* \internal */
        case DocNode::Kind_Internal:
          /* <div> */
        case DocNode::Kind_Include:
        case DocNode::Kind_SecRefList:
          /* <hr> */
        case DocNode::Kind_HorRuler:
          /* CopyDoc gets paragraph markers from the wrapping DocPara node,
           * but needs to insert them for all documentation being copied to
           * preserve formatting.
           */
        case DocNode::Kind_Copy:
          /* <blockquote> */
        case DocNode::Kind_HtmlBlockQuote:
          /* \parblock */
        case DocNode::Kind_ParBlock:
        case DocNode::Kind_IncOperator:
          return TRUE;
        case DocNode::Kind_Verbatim:
          {
            DocVerbatim *dv = (DocVerbatim*)n;
            return dv->type()!=DocVerbatim::HtmlOnly || dv->isBlock();
          }
        case DocNode::Kind_StyleChange:
          return ((DocStyleChange*)n)->style()==DocStyleChange::Preformatted ||
                 ((DocStyleChange*)n)->style()==DocStyleChange::Div ||
                 ((DocStyleChange*)n)->style()==DocStyleChange::Center;
        case DocNode::Kind_Formula:
          return !((DocFormula*)n)->isInline();
        case DocNode::Kind_Image:
          return !((DocImage*)n)->isInlineImage();
        default:
          break;
  }
  return FALSE;
}

static bool isDocVerbatimVisible(DocVerbatim *s)
{
  switch(s->type())
  {
    case DocVerbatim::ManOnly:
    case DocVerbatim::LatexOnly:
    case DocVerbatim::XmlOnly:
    case DocVerbatim::RtfOnly:
    case DocVerbatim::DocbookOnly:
      return FALSE;
    default:
      return TRUE;
  }
}

static bool isDocIncludeVisible(DocInclude *s)
{
  switch (s->type())
  {
    case DocInclude::DontInclude:
    case DocInclude::LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      return FALSE;
    default:
      return TRUE;
  }
}

static bool isDocIncOperatorVisible(DocIncOperator *s)
{
  switch (s->type())
  {
    case DocIncOperator::Skip:
      return FALSE;
    default:
      return TRUE;
  }
}

static bool isInvisibleNode(DocNode *node)
{
  return (node->kind()==DocNode::Kind_WhiteSpace)
      || // skip over image nodes that are not for HTML output
         (node->kind()==DocNode::Kind_Image && ((DocImage*)node)->type()!=DocImage::Html)
      || // skip over verbatim nodes that are not visible in the HTML output
         (node->kind()==DocNode::Kind_Verbatim && !isDocVerbatimVisible((DocVerbatim*)node))
      || // skip over include nodes that are not visible in the HTML output
         (node->kind()==DocNode::Kind_Include && !isDocIncludeVisible((DocInclude*)node))
      || // skip over include operator nodes that are not visible in the HTML output
         (node->kind()==DocNode::Kind_IncOperator && !isDocIncOperatorVisible((DocIncOperator*)node))
      ;
}

static void mergeHtmlAttributes(const HtmlAttribList &attribs, HtmlAttribList *mergeInto)
{
  HtmlAttribListIterator li(attribs);
  HtmlAttrib *att;
  for (li.toFirst();(att=li.current());++li)
  {
    HtmlAttribListIterator ml(*mergeInto);
    HtmlAttrib *opt;
    bool found = false;
    for (ml.toFirst();(opt=ml.current());++ml)
    {
      if (opt->name == att -> name)
      {
        found = true;
        break;
      }
    }
    if (found)
    {
       opt->value = opt->value + " " + att->value;
    }
    else
    {
      mergeInto->append(att);
    }
  }
}

static QCString htmlAttribsToString(const HtmlAttribList &attribs, QCString *pAltValue = 0)
{
  QCString result;
  HtmlAttribListIterator li(attribs);
  HtmlAttrib *att;
  for (li.toFirst();(att=li.current());++li)
  {
    if (!att->value.isEmpty())  // ignore attribute without values as they
                                // are not XHTML compliant, with the exception
				// of the alt attribute with the img tag
    {
      if (att->name=="alt" && pAltValue) // optionally return the value of alt separately
                                         // need to convert <img> to <object> for SVG images,
                                         // which do not support the alt attribute
      {
        *pAltValue = att->value;
      }
      else
      {
        result+=" ";
        result+=att->name;
        result+="=\""+convertToXML(att->value)+"\"";
      }
    }
    else if (att->name=="nowrap") // In XHTML, attribute minimization is forbidden, and the nowrap attribute must be defined as <td nowrap="nowrap">.
    {
        result+=" ";
        result+=att->name;
        result+="=\"nowrap\"";
    }
  }
  return result;
}

//-------------------------------------------------------------------------

HtmlDocVisitor::HtmlDocVisitor(FTextStream &t,CodeOutputInterface &ci,
                               const Definition *ctx) 
  : DocVisitor(DocVisitor_Html), m_t(t), m_ci(ci), m_insidePre(FALSE), 
                                 m_hide(FALSE), m_ctx(ctx)
{
  if (ctx) m_langExt=ctx->getDefFileExtension();
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void HtmlDocVisitor::visit(DocWord *w)
{
  //printf("word: %s\n",w->word().data());
  if (m_hide) return;
  filter(w->word());
}

void HtmlDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  //printf("linked word: %s\n",w->word().data());
  startLink(w->ref(),w->file(),w->relPath(),w->anchor(),w->tooltip());
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
  const char *res = HtmlEntityMapper::instance()->html(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("HTML: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
}

void HtmlDocVisitor::visit(DocEmoji *s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance()->unicode(s->index());
  if (res)
  {
    m_t << res;
  }
  else
  {
    m_t << s->name();
  }
}

void HtmlDocVisitor::writeObfuscatedMailAddress(const QCString &url)
{
  m_t << "<a href=\"#\" onclick=\"location.href='mai'+'lto:'";
  uint i;
  int size=3;
  for (i=0;i<url.length();)
  {
    m_t << "+'" << url.mid(i,size) << "'";
    i+=size;
    if (size==3) size=2; else size=3;
  }
  m_t << "; return false;\">";
}

void HtmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  if (u->isEmail()) // mail address
  {
    QCString url = u->url();
    writeObfuscatedMailAddress(url);
    uint size=5,i;
    for (i=0;i<url.length();)
    {
      filter(url.mid(i,size));
      if (i<url.length()-size) m_t << "<span style=\"display: none;\">.nosp@m.</span>";
      i+=size;
      if (size==5) size=4; else size=5;
    }
    m_t << "</a>";
  }
  else // web address
  {
    m_t << "<a href=\"";
    m_t << u->url() << "\">";
    filter(u->url());
    m_t << "</a>";
  }
}

void HtmlDocVisitor::visit(DocLineBreak *br)
{
  if (m_hide) return;
  m_t << "<br "<< htmlAttribsToString(br->attribs()) << " />\n";
}

void HtmlDocVisitor::visit(DocHorRuler *hr)
{
  if (m_hide) return;
  forceEndParagraph(hr);
  m_t << "<hr "<< htmlAttribsToString(hr->attribs()) << " />\n";
  forceStartParagraph(hr);
}

void HtmlDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "<b" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</b>";
      break;
    case DocStyleChange::S:
      if (s->enable()) m_t << "<s" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</s>";
      break;
    case DocStyleChange::Strike:
      if (s->enable()) m_t << "<strike" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</strike>";
      break;
    case DocStyleChange::Del:
      if (s->enable()) m_t << "<del" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</del>";
      break;
    case DocStyleChange::Underline:
      if (s->enable()) m_t << "<u" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</u>";
      break;
    case DocStyleChange::Ins:
      if (s->enable()) m_t << "<ins" << htmlAttribsToString(s->attribs()) << ">";      else m_t << "</ins>";
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
      if (s->enable()) 
      {
        forceEndParagraph(s);
        m_t << "<center" << htmlAttribsToString(s->attribs()) << ">"; 
      }
      else 
      {
        m_t << "</center>";
        forceStartParagraph(s);
      }
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "<small" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</small>";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        forceEndParagraph(s);
        m_t << "<pre" << htmlAttribsToString(s->attribs()) << ">";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "</pre>";
        forceStartParagraph(s);
      }
      break;
    case DocStyleChange::Div:
      if (s->enable()) 
      {
        forceEndParagraph(s);
        m_t << "<div" << htmlAttribsToString(s->attribs()) << ">";  
      }
      else 
      {
        m_t << "</div>";
        forceStartParagraph(s);
      }
      break;
    case DocStyleChange::Span:
      if (s->enable()) m_t << "<span" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</span>";
      break;

  }
}


static void visitPreCaption(FTextStream &t, DocVerbatim *s)
{
  if (s->hasCaption())
  { 
    t << "<div class=\"caption\">" << endl;
  }
}


static void visitPostCaption(FTextStream &t, DocVerbatim *s)
{
  if (s->hasCaption())
  {
    t << "</div>" << endl;
  }
}


static void visitCaption(HtmlDocVisitor *parent, QList<DocNode> children)
{
  QListIterator<DocNode> cli(children);
  DocNode *n;
  for (cli.toFirst();(n=cli.current());++cli) n->accept(parent);
}

void HtmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  QCString lang = m_langExt;
  if (!s->language().isEmpty()) // explicit language setting
  {
    lang = s->language();
  }
  SrcLangExt langExt = getLanguageFromFileName(lang);
  switch(s->type())
  {
    case DocVerbatim::Code: 
      forceEndParagraph(s);
      m_t << PREFRAG_START;
      Doxygen::parserManager->getCodeParser(lang)
                             .parseCode(m_ci,
                                        s->context(),
                                        s->text(),
                                        langExt,
                                        s->isExample(),
                                        s->exampleFile(),
                                        0,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        FALSE, // inlineFragment
                                        0,     // memberDef
                                        TRUE,  // show line numbers
                                        m_ctx  // search context
                                       );
      m_t << PREFRAG_END;
      forceStartParagraph(s);
      break;
    case DocVerbatim::Verbatim: 
      forceEndParagraph(s);
      m_t << /*PREFRAG_START <<*/ "<pre class=\"fragment\">";
      filter(s->text());
      m_t << "</pre>" /*<< PREFRAG_END*/;
      forceStartParagraph(s);
      break;
    case DocVerbatim::HtmlOnly:
      {
        if (s->isBlock()) forceEndParagraph(s);
        m_t << s->text();
        if (s->isBlock()) forceStartParagraph(s);
      }
      break;
    case DocVerbatim::ManOnly: 
    case DocVerbatim::LatexOnly: 
    case DocVerbatim::XmlOnly: 
    case DocVerbatim::RtfOnly:
    case DocVerbatim::DocbookOnly:
      /* nothing */ 
      break;

    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString fileName(4096);

        forceEndParagraph(s);
        fileName.sprintf("%s%d%s", 
            (Config_getString(HTML_OUTPUT)+"/inline_dotgraph_").data(), 
            dotindex++,
            ".dot"
           );
        QFile file(fileName);
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s for writing\n",fileName.data());
        }
        else
        {
          file.writeBlock( s->text(), s->text().length() );
          file.close();

          m_t << "<div class=\"dotgraph\">" << endl;
          writeDotFile(fileName,s->relPath(),s->context());
          visitPreCaption(m_t, s);
          visitCaption(this, s->children());
          visitPostCaption(m_t, s);
          m_t << "</div>" << endl;

          if (Config_getBool(DOT_CLEANUP)) file.remove();
        }
        forceStartParagraph(s);
      }
      break;
    case DocVerbatim::Msc:
      {
        forceEndParagraph(s);

        static int mscindex = 1;
        QCString baseName(4096);

        baseName.sprintf("%s%d", 
            (Config_getString(HTML_OUTPUT)+"/inline_mscgraph_").data(), 
            mscindex++
            );
        QFile file(baseName+".msc");
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s.msc for writing\n",baseName.data());
        }
        else
        {
          QCString text = "msc {";
          text+=s->text();
          text+="}";

          file.writeBlock( text, text.length() );
          file.close();

          m_t << "<div class=\"mscgraph\">" << endl;
          writeMscFile(baseName+".msc",s->relPath(),s->context());
          visitPreCaption(m_t, s);
          visitCaption(this, s->children());
          visitPostCaption(m_t, s);
          m_t << "</div>" << endl;

          if (Config_getBool(DOT_CLEANUP)) file.remove();
        }
        forceStartParagraph(s);
      }
      break;
    case DocVerbatim::PlantUML:
      {
        forceEndParagraph(s);
        static QCString htmlOutput = Config_getString(HTML_OUTPUT);
        QCString imgExt = getDotImageExtension();
        PlantumlManager::OutputFormat format = PlantumlManager::PUML_BITMAP;	// default : PUML_BITMAP
        if (imgExt=="svg")
        {
          format = PlantumlManager::PUML_SVG;
        }
        QCString baseName = PlantumlManager::instance()->writePlantUMLSource(htmlOutput,s->exampleFile(),s->text(),format);
        m_t << "<div class=\"plantumlgraph\">" << endl;
        writePlantUMLFile(baseName,s->relPath(),s->context());
        visitPreCaption(m_t, s);
        visitCaption(this, s->children());
        visitPostCaption(m_t, s);
        m_t << "</div>" << endl;
        forceStartParagraph(s);
      }
      break;
  }
}

void HtmlDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<a class=\"anchor\" id=\"" << anc->anchor() << "\"" << htmlAttribsToString(anc->attribs()) << "></a>";
}

void HtmlDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  switch(inc->type())
  {
    case DocInclude::Include: 
      forceEndParagraph(inc);
      m_t << PREFRAG_START;
      Doxygen::parserManager->getCodeParser(inc->extension())
                             .parseCode(m_ci,                 
                                        inc->context(),
                                        inc->text(),
                                        langExt,
                                        inc->isExample(),
                                        inc->exampleFile(),
                                        0,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        TRUE,  // inlineFragment
                                        0,     // memberDef
                                        FALSE, // show line numbers
                                        m_ctx  // search context 
                                       );
      m_t << PREFRAG_END;
      forceStartParagraph(inc);
      break;
    case DocInclude::IncWithLines:
      { 
         forceEndParagraph(inc);
         m_t << PREFRAG_START;
         QFileInfo cfi( inc->file() );
         FileDef *fd = createFileDef( cfi.dirPath().utf8(), cfi.fileName().utf8() );
         Doxygen::parserManager->getCodeParser(inc->extension())
                                .parseCode(m_ci,
                                           inc->context(),
                                           inc->text(),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile(), 
                                           fd,   // fileDef,
                                           -1,    // start line
                                           -1,    // end line
                                           FALSE, // inline fragment
                                           0,     // memberDef
                                           TRUE,  // show line numbers
                                           m_ctx  // search context
                                           );
         delete fd;
         m_t << PREFRAG_END;
         forceStartParagraph(inc);
      }
      break;
    case DocInclude::DontInclude:
    case DocInclude::LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
    case DocInclude::DontIncWithLines:
      break;
    case DocInclude::HtmlInclude:
      {
        if (inc->isBlock()) forceEndParagraph(inc);
        m_t << inc->text();
        if (inc->isBlock()) forceStartParagraph(inc);
      }
      break;
    case DocInclude::VerbInclude: 
      forceEndParagraph(inc);
      m_t << /*PREFRAG_START <<*/ "<pre class=\"fragment\">";
      filter(inc->text());
      m_t << "</pre>" /*<< PREFRAG_END*/;
      forceStartParagraph(inc);
      break;
    case DocInclude::Snippet:
      {
         forceEndParagraph(inc);
         m_t << PREFRAG_START;
         Doxygen::parserManager->getCodeParser(inc->extension())
                                .parseCode(m_ci,
                                           inc->context(),
                                           extractBlock(inc->text(),inc->blockId()),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile(), 
                                           0,
                                           -1,    // startLine
                                           -1,    // endLine
                                           TRUE,  // inlineFragment
                                           0,     // memberDef
                                           FALSE, // show line number
                                           m_ctx  // search context
                                          );
         m_t << PREFRAG_END;
         forceStartParagraph(inc);
      }
      break;
    case DocInclude::SnipWithLines:
      {
         forceEndParagraph(inc);
         m_t << PREFRAG_START;
         QFileInfo cfi( inc->file() );
         FileDef *fd = createFileDef( cfi.dirPath().utf8(), cfi.fileName().utf8() );
         Doxygen::parserManager->getCodeParser(inc->extension())
                                .parseCode(m_ci,
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
                                           TRUE,  // show line number
                                           m_ctx  // search context
                                          );
         delete fd;
         m_t << PREFRAG_END;
         forceStartParagraph(inc);
      }
      break;
    case DocInclude::SnippetDoc: 
    case DocInclude::IncludeDoc: 
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
}

void HtmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    forceEndParagraph(op);
    if (!m_hide) m_t << PREFRAG_START;
    pushEnabled();
    m_hide=TRUE;
  }
  QCString locLangExt = getFileNameExtension(op->includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  if (op->type()!=DocIncOperator::Skip) 
  {
    popEnabled();
    if (!m_hide) 
    {
      FileDef *fd = 0;
      if (!op->includeFileName().isEmpty())
      {
        QFileInfo cfi( op->includeFileName() );
        fd = createFileDef( cfi.dirPath().utf8(), cfi.fileName().utf8() );
      }
      Doxygen::parserManager->getCodeParser(locLangExt)
                             .parseCode(
                                m_ci,
                                op->context(),
                                op->text(),
                                langExt,
                                op->isExample(),
                                op->exampleFile(),
                                fd,     // fileDef
                                op->line(),    // startLine
                                -1,    // endLine
                                FALSE, // inline fragment
                                0,     // memberDef
                                op->showLineNo(),  // show line numbers
                                m_ctx  // search context
                               );
      if (fd) delete fd;
    }
    pushEnabled();
    m_hide=TRUE;
  }
  if (op->isLast())  
  {
    popEnabled();
    if (!m_hide) m_t << PREFRAG_END;
    forceStartParagraph(op);
  }
  else
  {
    if (!m_hide) m_t << endl;
  }
}

void HtmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  bool bDisplay = !f->isInline();
  if (bDisplay) 
  {
    forceEndParagraph(f);
    m_t << "<p class=\"formulaDsp\">" << endl;
  }

  if (Config_getBool(USE_MATHJAX))
  {
    QCString text = f->text();
    bool closeInline = FALSE;
    if (!bDisplay && !text.isEmpty() && text.at(0)=='$' && 
                      text.at(text.length()-1)=='$')
    {
      closeInline=TRUE;
      text = text.mid(1,text.length()-2);
      m_t << "\\(";
    }
    m_t << convertToHtml(text);
    if (closeInline)
    {
      m_t << "\\)";
    }
  }
  else
  {
    m_t << "<img class=\"formula" 
      << (bDisplay ? "Dsp" : "Inl");
    m_t << "\" alt=\"";
    filterQuotedCdataAttr(f->text());
    m_t << "\"";
    m_t << " src=\"" << f->relPath() << f->name();
    if (Config_getEnum(HTML_FORMULA_FORMAT)=="svg")
    {
      m_t << ".svg";
    }
    else
    {
      m_t << ".png";
    }
    FormulaManager::DisplaySize size = FormulaManager::instance().displaySize(f->id());
    if (size.width!=-1)
    {
      m_t << "\" width=\"" << size.width;
    }
    if (size.height!=-1)
    {
      m_t << "\" height=\"" << size.height;
    }
    m_t << "\"/>";
  }
  if (bDisplay)
  {
    m_t << endl << "</p>" << endl;
    forceStartParagraph(f);
  }
}

void HtmlDocVisitor::visit(DocIndexEntry *e)
{
  QCString anchor = convertIndexWordToAnchor(e->entry());
  if (e->member()) 
  {
    anchor.prepend(e->member()->anchor()+"_");
  }
  m_t << "<a name=\"" << anchor << "\"></a>";
  //printf("*** DocIndexEntry: word='%s' scope='%s' member='%s'\n",
  //       e->entry().data(),
  //       e->scope()  ? e->scope()->name().data()  : "<null>",
  //       e->member() ? e->member()->name().data() : "<null>"
  //      );
  Doxygen::indexList->addIndexItem(e->scope(),e->member(),anchor,e->entry());
}

void HtmlDocVisitor::visit(DocSimpleSectSep *)
{
  m_t << "</dd>" << endl;
  m_t << "<dd>" << endl;
}

void HtmlDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  if (!cite->file().isEmpty()) 
  {
    startLink(cite->ref(),cite->file(),cite->relPath(),cite->anchor());
  }
  else
  {
    m_t << "<b>[";
  }
  filter(cite->text());
  if (!cite->file().isEmpty()) 
  {
    endLink();
  }
  else
  {
    m_t << "]</b>";
  }
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------


void HtmlDocVisitor::visitPre(DocAutoList *l)
{
  //printf("DocAutoList::visitPre\n");
  if (m_hide) return;
  forceEndParagraph(l);
  if (l->isEnumList())
  {
    //
    // Do list type based on depth:
    // 1.
    //   a.
    //     i.
    //       A. 
    //         1. (repeat)...
    //
    m_t << "<ol type=\"" << types[l->depth() % NUM_HTML_LIST_TYPES] << "\"";
  }
  else
  {
    m_t << "<ul";
  }
  m_t << getDirHtmlClassOfNode(getTextDirByConfig(l)) << ">";
  if (!l->isPreformatted()) m_t << "\n";
}

void HtmlDocVisitor::visitPost(DocAutoList *l)
{
  //printf("DocAutoList::visitPost\n");
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
  forceStartParagraph(l);
}

void HtmlDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "<li>";
}

void HtmlDocVisitor::visitPost(DocAutoListItem *li) 
{
  if (m_hide) return;
  m_t << "</li>";
  if (!li->isPreformatted()) m_t << "\n";
}

template<class T> 
bool isFirstChildNode(T *parent, DocNode *node)
{
   return parent->children().getFirst()==node;
}

template<class T> 
bool isLastChildNode(T *parent, DocNode *node)
{
   return parent->children().getLast()==node;
}

bool isSeparatedParagraph(DocSimpleSect *parent,DocPara *par)
{
  QList<DocNode> nodes = parent->children();
  int i = nodes.findRef(par);
  if (i==-1) return FALSE;
  int count = parent->children().count();
  if (count>1 && i==0) // first node
  {
    if (nodes.at(i+1)->kind()==DocNode::Kind_SimpleSectSep)
    {
      return TRUE;
    }
  }
  else if (count>1 && i==count-1) // last node
  {
    if (nodes.at(i-1)->kind()==DocNode::Kind_SimpleSectSep)
    {
      return TRUE;
    }
  }
  else if (count>2 && i>0 && i<count-1) // intermediate node
  {
    if (nodes.at(i-1)->kind()==DocNode::Kind_SimpleSectSep &&
        nodes.at(i+1)->kind()==DocNode::Kind_SimpleSectSep)
    {
      return TRUE;
    }
  }
  return FALSE;
}

static int getParagraphContext(DocPara *p,bool &isFirst,bool &isLast)
{
  int t=0;
  isFirst=FALSE;
  isLast=FALSE;
  if (p && p->parent())
  {
    switch (p->parent()->kind()) 
    {
      case DocNode::Kind_ParBlock:
        { // hierarchy: node N -> para -> parblock -> para
          // adapt return value to kind of N
          DocNode::Kind kind = DocNode::Kind_Para;
          if ( p->parent()->parent() && p->parent()->parent()->parent() )
          {
            kind = p->parent()->parent()->parent()->kind();
          }
          isFirst=isFirstChildNode((DocParBlock*)p->parent(),p);
          isLast =isLastChildNode ((DocParBlock*)p->parent(),p);
          t=NONE;
          if (isFirst)
          {
            if (kind==DocNode::Kind_HtmlListItem ||
                kind==DocNode::Kind_SecRefItem)
            {
              t=STARTLI;
            }
            else if (kind==DocNode::Kind_HtmlDescData ||
                     kind==DocNode::Kind_XRefItem ||
                     kind==DocNode::Kind_SimpleSect)
            {
              t=STARTDD;
            }
            else if (kind==DocNode::Kind_HtmlCell ||
                     kind==DocNode::Kind_ParamList)
            {
              t=STARTTD;
            }
          }
          if (isLast)
          {
            if (kind==DocNode::Kind_HtmlListItem ||
                kind==DocNode::Kind_SecRefItem)
            {
              t=ENDLI;
            }
            else if (kind==DocNode::Kind_HtmlDescData ||
                     kind==DocNode::Kind_XRefItem ||
                     kind==DocNode::Kind_SimpleSect)
            {
              t=ENDDD;
            }
            else if (kind==DocNode::Kind_HtmlCell ||
                     kind==DocNode::Kind_ParamList)
            {
              t=ENDTD;
            }
          }
          if (!isFirst && !isLast)
          {
            if (kind==DocNode::Kind_HtmlListItem ||
                kind==DocNode::Kind_SecRefItem)
            {
              t=INTERLI;
            }
            else if (kind==DocNode::Kind_HtmlDescData ||
                     kind==DocNode::Kind_XRefItem ||
                     kind==DocNode::Kind_SimpleSect)
            {
              t=INTERDD;
            }
            else if (kind==DocNode::Kind_HtmlCell ||
                     kind==DocNode::Kind_ParamList)
            {
              t=INTERTD;
            }
          }
          break;
        }
      case DocNode::Kind_AutoListItem:
        isFirst=isFirstChildNode((DocAutoListItem*)p->parent(),p);
        isLast =isLastChildNode ((DocAutoListItem*)p->parent(),p);
        t=STARTLI; // not used
        break;
      case DocNode::Kind_SimpleListItem:
        isFirst=TRUE;
        isLast =TRUE;
        t=STARTLI; // not used
        break;
      case DocNode::Kind_ParamList:
        isFirst=TRUE;
        isLast =TRUE;
        t=STARTLI; // not used
        break;
      case DocNode::Kind_HtmlListItem:
        isFirst=isFirstChildNode((DocHtmlListItem*)p->parent(),p);
        isLast =isLastChildNode ((DocHtmlListItem*)p->parent(),p);
        if (isFirst) t=STARTLI;
        if (isLast)  t=ENDLI;
        if (!isFirst && !isLast) t = INTERLI;
        break;
      case DocNode::Kind_SecRefItem:
        isFirst=isFirstChildNode((DocSecRefItem*)p->parent(),p);
        isLast =isLastChildNode ((DocSecRefItem*)p->parent(),p);
        if (isFirst) t=STARTLI;
        if (isLast)  t=ENDLI;
        if (!isFirst && !isLast) t = INTERLI;
        break;
      case DocNode::Kind_HtmlDescData:
        isFirst=isFirstChildNode((DocHtmlDescData*)p->parent(),p);
        isLast =isLastChildNode ((DocHtmlDescData*)p->parent(),p);
        if (isFirst) t=STARTDD;
        if (isLast)  t=ENDDD;
        if (!isFirst && !isLast) t = INTERDD;
        break;
      case DocNode::Kind_XRefItem:
        isFirst=isFirstChildNode((DocXRefItem*)p->parent(),p);
        isLast =isLastChildNode ((DocXRefItem*)p->parent(),p);
        if (isFirst) t=STARTDD;
        if (isLast)  t=ENDDD;
        if (!isFirst && !isLast) t = INTERDD;
        break;
      case DocNode::Kind_SimpleSect:
        isFirst=isFirstChildNode((DocSimpleSect*)p->parent(),p);
        isLast =isLastChildNode ((DocSimpleSect*)p->parent(),p);
        if (isFirst) t=STARTDD;
        if (isLast)  t=ENDDD;
        if (isSeparatedParagraph((DocSimpleSect*)p->parent(),p))
          // if the paragraph is enclosed with separators it will
          // be included in <dd>..</dd> so avoid addition paragraph
          // markers
        {
          isFirst=isLast=TRUE;
        }
        if (!isFirst && !isLast) t = INTERDD;
        break;
      case DocNode::Kind_HtmlCell:
        isFirst=isFirstChildNode((DocHtmlCell*)p->parent(),p);
        isLast =isLastChildNode ((DocHtmlCell*)p->parent(),p);
        if (isFirst) t=STARTTD;
        if (isLast)  t=ENDTD;
        if (!isFirst && !isLast) t = INTERTD;
        break;
      default:
        break;
    }
    //printf("para=%p parent()->kind=%d isFirst=%d isLast=%d t=%d\n",
    //    p,p->parent()->kind(),isFirst,isLast,t);
  }
  return t;
}

void HtmlDocVisitor::visitPre(DocPara *p) 
{
  if (m_hide) return;

  //printf("DocPara::visitPre: parent of kind %d ",
  //       p->parent() ? p->parent()->kind() : -1);

  bool needsTag = FALSE;
  if (p && p->parent()) 
  {
    switch (p->parent()->kind()) 
    {
      case DocNode::Kind_Section:
      case DocNode::Kind_Internal:
      case DocNode::Kind_HtmlListItem:
      case DocNode::Kind_HtmlDescData:
      case DocNode::Kind_HtmlCell:
      case DocNode::Kind_SimpleListItem:
      case DocNode::Kind_AutoListItem:
      case DocNode::Kind_SimpleSect:
      case DocNode::Kind_XRefItem:
      case DocNode::Kind_Copy:
      case DocNode::Kind_HtmlBlockQuote:
      case DocNode::Kind_ParBlock:
        needsTag = TRUE;
        break;
      case DocNode::Kind_Root:
        needsTag = !((DocRoot*)p->parent())->singleLine();
        break;
      default:
        needsTag = FALSE;
    }
  }

  // if the first element of a paragraph is something that should be outside of
  // the paragraph (<ul>,<dl>,<table>,..) then that will already started the 
  // paragraph and we don't need to do it here
  bool paragraphAlreadyStarted = false;
  uint nodeIndex = 0;
  if (p && nodeIndex<p->children().count())
  {
    while (nodeIndex<p->children().count() && isInvisibleNode(p->children().at(nodeIndex)))
    {
      nodeIndex++;
    }
    if (nodeIndex<p->children().count())
    {
      DocNode *n = p->children().at(nodeIndex);
      if (mustBeOutsideParagraph(n))
      {
        paragraphAlreadyStarted = true;
        needsTag = FALSE;
      }
    }
  }

  // check if this paragraph is the first or last or intermediate child of a <li> or <dd>.
  // this allows us to mark the tag with a special class so we can
  // fix the otherwise ugly spacing.
  int t;
  bool isFirst;
  bool isLast;
  t = getParagraphContext(p,isFirst,isLast);
  //printf("startPara first=%d last=%d\n",isFirst,isLast);
  if (isFirst && isLast) needsTag=FALSE;

  //printf("  needsTag=%d\n",needsTag);
  // write the paragraph tag (if needed)
  if (needsTag)
    m_t << "<p" << getDirHtmlClassOfNode(getTextDirByConfig(p), contexts[t]) << htmlAttribsToString(p->attribs()) << ">";
  else if(!paragraphAlreadyStarted)
    m_t << getHtmlDirEmbeddingChar(getTextDirByConfig(p)) << htmlAttribsToString(p->attribs());
}

void HtmlDocVisitor::visitPost(DocPara *p)
{

  //printf("DocPara::visitPost: parent of kind %d ",
  //       p->parent() ? p->parent()->kind() : -1);

  bool needsTag = FALSE;
  if (p->parent()) 
  {
    switch (p->parent()->kind()) 
    {
      case DocNode::Kind_Section:
      case DocNode::Kind_Internal:
      case DocNode::Kind_HtmlListItem:
      case DocNode::Kind_HtmlDescData:
      case DocNode::Kind_HtmlCell:
      case DocNode::Kind_SimpleListItem:
      case DocNode::Kind_AutoListItem:
      case DocNode::Kind_SimpleSect:
      case DocNode::Kind_XRefItem:
      case DocNode::Kind_Copy:
      case DocNode::Kind_HtmlBlockQuote:
      case DocNode::Kind_ParBlock:
        needsTag = TRUE;
        break;
      case DocNode::Kind_Root:
        needsTag = !((DocRoot*)p->parent())->singleLine();
        break;
      default:
        needsTag = FALSE;
    }
  }

  // if the last element of a paragraph is something that should be outside of
  // the paragraph (<ul>,<dl>,<table>) then that will already have ended the 
  // paragraph and we don't need to do it here
  int nodeIndex = p->children().count()-1;
  if (nodeIndex>=0)
  {
    while (nodeIndex>=0 && isInvisibleNode(p->children().at(nodeIndex)))
    {
      nodeIndex--;
    }
    if (nodeIndex>=0)
    {
      DocNode *n = p->children().at(nodeIndex);
      if (mustBeOutsideParagraph(n))
      {
        needsTag = FALSE;
      }
    }
  }

  bool isFirst;
  bool isLast;
  getParagraphContext(p,isFirst,isLast);
  //printf("endPara first=%d last=%d\n",isFirst,isLast);
  if (isFirst && isLast) needsTag=FALSE;

  //printf("DocPara::visitPost needsTag=%d\n",needsTag);

  if (needsTag) m_t << "</p>\n";

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
  forceEndParagraph(s);
  if (s->type() != DocSimpleSect::Return)
    m_t << "<dl" << getDirHtmlClassOfNode(getTextDirByConfig(s), "section " + s->typeString()) << "><dt>";
  else
    m_t << "<dl class=\"section " << s->typeString() << "\"><dt>";
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
    m_t << "</dt><dd>";
  }
}

void HtmlDocVisitor::visitPost(DocSimpleSect *s)
{
  if (m_hide) return;
  m_t << "</dd></dl>\n";
  forceStartParagraph(s);
}

void HtmlDocVisitor::visitPre(DocTitle *)
{
}

void HtmlDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "</dt><dd>";
}

void HtmlDocVisitor::visitPre(DocSimpleList *sl)
{
  if (m_hide) return;
  forceEndParagraph(sl);
  m_t << "<ul>";
  if (!sl->isPreformatted()) m_t << "\n";

}

void HtmlDocVisitor::visitPost(DocSimpleList *sl)
{
  if (m_hide) return;
  m_t << "</ul>";
  if (!sl->isPreformatted()) m_t << "\n";
  forceStartParagraph(sl);
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
  forceEndParagraph(s);
  m_t << "<h" << s->level() << getDirHtmlClassOfNode(getTextDirByConfig(s->title())) << ">";
  m_t << "<a class=\"anchor\" id=\"" << s->anchor();
  m_t << "\"></a>" << endl;
  filter(convertCharEntitiesToUTF8(s->title().data()));
  m_t << "</h" << s->level() << ">\n";
}

void HtmlDocVisitor::visitPost(DocSection *s) 
{
  forceStartParagraph(s);
}

void HtmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  forceEndParagraph(s);
  if (s->type()==DocHtmlList::Ordered) 
  {
    m_t << "<ol" << htmlAttribsToString(s->attribs()); 
  }
  else 
  {
    m_t << "<ul" << htmlAttribsToString(s->attribs());
  }
  m_t << getDirHtmlClassOfNode(getTextDirByConfig(s)) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlList *s) 
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
  {
    m_t << "</ol>"; 
  }
  else
  { 
    m_t << "</ul>";
  }
  if (!s->isPreformatted()) m_t << "\n";
  forceStartParagraph(s);
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
  forceEndParagraph(dl);
  m_t << "<dl" << htmlAttribsToString(dl->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlDescList *dl) 
{
  if (m_hide) return;
  m_t << "</dl>\n";
  forceStartParagraph(dl);
}

void HtmlDocVisitor::visitPre(DocHtmlDescTitle *dt)
{
  if (m_hide) return;
  m_t << "<dt" << htmlAttribsToString(dt->attribs())
    << getDirHtmlClassOfNode(getTextDirByConfig(dt))
    << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  if (m_hide) return;
  m_t << "</dt>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlDescData *dd)
{
  if (m_hide) return;
  m_t << "<dd" << htmlAttribsToString(dd->attribs()) 
    << getDirHtmlClassOfNode(getTextDirByConfig(dd))
    << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlDescData *) 
{
  if (m_hide) return;
  m_t << "</dd>\n";
}

void HtmlDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;

  forceEndParagraph(t);

  if (t->hasCaption())
  {
    QCString anc =  t->caption()->anchor();
    if (!anc.isEmpty())
    {
      m_t << "<a class=\"anchor\" id=\"" << anc << "\"></a>\n";
    }
  }

  QCString attrs = htmlAttribsToString(t->attribs());
  if (attrs.isEmpty())
  {
    m_t << "<table";
    if(t->hasCaption())
      m_t << getDirHtmlClassOfNode(getTextDirByConfig(t->caption()), "doxtable");
    else
      m_t << getDirHtmlClassOfNode(getTextDirByConfig(t), "doxtable");
    m_t << ">\n";
  }
  else
  {
    m_t << "<table";
    if (t->hasCaption())
      m_t << getDirHtmlClassOfNode(getTextDirByConfig(t->caption()));
    else
      m_t << getDirHtmlClassOfNode(getTextDirByConfig(t));
    m_t << htmlAttribsToString(t->attribs()) << ">\n";
  }
}

void HtmlDocVisitor::visitPost(DocHtmlTable *t) 
{
  if (m_hide) return;
  m_t << "</table>\n";
  forceStartParagraph(t);
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
  m_t << "<caption" << htmlAttribsToString(c->attribs()) << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlCaption *) 
{
  if (m_hide) return;
  m_t << "</caption>\n";
}

void HtmlDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  //forceEndParagraph(i);
  //m_t << "<p><b>" << theTranslator->trForInternalUseOnly() << "</b></p>" << endl;
}

void HtmlDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  //forceStartParagraph(i);
}

void HtmlDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  if (href->url().left(7)=="mailto:")
  {
    writeObfuscatedMailAddress(href->url().mid(7));
  }
  else
  {
    QCString url = correctURL(href->url(),href->relPath());
    m_t << "<a href=\"" << convertToHtml(url)  << "\""
        << htmlAttribsToString(href->attribs()) << ">";
  }
}

void HtmlDocVisitor::visitPost(DocHRef *) 
{
  if (m_hide) return;
  m_t << "</a>";
}

void HtmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  forceEndParagraph(header);
  m_t << "<h" << header->level() 
      << htmlAttribsToString(header->attribs()) 
      << getDirHtmlClassOfNode(getTextDirByConfig(header))
      << ">";
}

void HtmlDocVisitor::visitPost(DocHtmlHeader *header) 
{
  if (m_hide) return;
  m_t << "</h" << header->level() << ">\n";
  forceStartParagraph(header);
}

void HtmlDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Html)
  {
    bool inlineImage = img->isInlineImage();
    bool typeSVG = img->isSVG();
    QCString url = img->url();

    if (!inlineImage)
    {
      forceEndParagraph(img);
    }
    if (m_hide) return;
    QCString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right(baseName.length()-i-1);
    }
    if (!inlineImage) m_t << "<div class=\"image\">" << endl;
    QCString sizeAttribs;
    if (!img->width().isEmpty())
    {
      sizeAttribs+=" width=\""+img->width()+"\"";
    }
    if (!img->height().isEmpty()) // link to local file
    {
      sizeAttribs+=" height=\""+img->height()+"\"";
    }
    // 16 cases: url.isEmpty() | typeSVG | inlineImage | img->hasCaption()

    HtmlAttribList extraAttribs;
    if (typeSVG)
    {
      HtmlAttrib opt;
      opt.name  = "style";
      opt.value = "pointer-events: none;";
      extraAttribs.append(&opt);
    }
    QCString alt;
    mergeHtmlAttributes(img->attribs(),&extraAttribs);
    QCString attrs = htmlAttribsToString(extraAttribs,&alt);
    QCString src;
    if (url.isEmpty())
    {
      src = img->relPath()+img->name();
    }
    else
    {
      src = correctURL(url,img->relPath());
    }
    if (typeSVG && !inlineImage)
    {
      m_t << "<object type=\"image/svg+xml\" data=\"" << convertToHtml(src)
        << "\"" << sizeAttribs << attrs;
      if (inlineImage)
      {
        // skip closing tag
      }
      else
      {
        m_t << ">" << alt << "</object>" << endl;
      }
    }
    else
    {
      m_t << "<img src=\"" << convertToHtml(src) << "\" alt=\"" << alt << "\"" << sizeAttribs << attrs;
      if (inlineImage)
      {
        m_t << " class=\"inline\"";
      }
      else
      {
        m_t << "/>\n";
      }
    }
    if (img->hasCaption())
    {
      if (inlineImage)
      {
        m_t << " title=\"";
      }
      else
      {
        m_t << "<div class=\"caption\">" << endl;
        m_t << getHtmlDirEmbeddingChar(getTextDirByConfig(img));
      }
    }
    else if (inlineImage)
    {
      m_t << "/>";
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
    bool inlineImage = img->isInlineImage();
    if (img->hasCaption())
    {
      if (inlineImage)
      {
        m_t << "\"/>";
      }
      else // end <div class="caption">
      {
        m_t << "</div>";
      }
    }
    if (!inlineImage) // end <div class="image">
    {
      m_t << "</div>" << endl;
      forceStartParagraph(img);
    }
  }
  else // other format
  {
    popEnabled();
  }
}

void HtmlDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  m_t << "<div class=\"dotgraph\">" << endl;
  writeDotFile(df->file(),df->relPath(),df->context());
  if (df->hasCaption())
  { 
    m_t << "<div class=\"caption\">" << endl;
  }
}

void HtmlDocVisitor::visitPost(DocDotFile *df) 
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>" << endl;
  }
  m_t << "</div>" << endl;
}

void HtmlDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  m_t << "<div class=\"mscgraph\">" << endl;
  writeMscFile(df->file(),df->relPath(),df->context());
  if (df->hasCaption())
  { 
    m_t << "<div class=\"caption\">" << endl;
  }
}
void HtmlDocVisitor::visitPost(DocMscFile *df) 
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>" << endl;
  }
  m_t << "</div>" << endl;
}

void HtmlDocVisitor::visitPre(DocDiaFile *df)
{
  if (m_hide) return;
  m_t << "<div class=\"diagraph\">" << endl;
  writeDiaFile(df->file(),df->relPath(),df->context());
  if (df->hasCaption())
  {
    m_t << "<div class=\"caption\">" << endl;
  }
}
void HtmlDocVisitor::visitPost(DocDiaFile *df)
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>" << endl;
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
    // when ref->isSubPage()==TRUE we use ref->file() for HTML and
    // ref->anchor() for LaTeX/RTF
    startLink(ref->ref(),ref->file(),ref->relPath(),ref->isSubPage() ? QCString() : ref->anchor());
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
  QCString refName=addHtmlExtensionIfMissing(ref->file());
  m_t << "<li><a href=\"" << refName << "#" << ref->anchor() << "\">";

}

void HtmlDocVisitor::visitPost(DocSecRefItem *) 
{
  if (m_hide) return;
  m_t << "</a></li>\n";
}

void HtmlDocVisitor::visitPre(DocSecRefList *s)
{
  if (m_hide) return;
  forceEndParagraph(s);
  m_t << "<div>" << endl;
  m_t << "<ul class=\"multicol\">" << endl;
}

void HtmlDocVisitor::visitPost(DocSecRefList *s) 
{
  if (m_hide) return;
  m_t << "</ul>" << endl;
  m_t << "</div>" << endl;
  forceStartParagraph(s);
}

//void HtmlDocVisitor::visitPre(DocLanguage *l)
//{
//  QCString langId = Config_getEnum(OUTPUT_LANGUAGE);
//  if (l->id().lower()!=langId.lower())
//  {
//    pushEnabled();
//    m_hide = TRUE;
//  }
//}
//
//void HtmlDocVisitor::visitPost(DocLanguage *l) 
//{
//  QCString langId = Config_getEnum(OUTPUT_LANGUAGE);
//  if (l->id().lower()!=langId.lower())
//  {
//    popEnabled();
//  }
//}

void HtmlDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  forceEndParagraph(s);
  QCString className;
  QCString heading;
  switch(s->type())
  {
    case DocParamSect::Param: 
      heading=theTranslator->trParameters(); 
      className="params";
      break;
    case DocParamSect::RetVal: 
      heading=theTranslator->trReturnValues(); 
      className="retval";
      break;
    case DocParamSect::Exception: 
      heading=theTranslator->trExceptions(); 
      className="exception";
      break;
    case DocParamSect::TemplateParam: 
      heading=theTranslator->trTemplateParameters();
      className="tparams";
      break;
    default:
      ASSERT(0);
  }
  m_t << "<dl class=\"" << className << "\"><dt>";
  m_t << heading;
  m_t << "</dt><dd>" << endl;
  m_t << "  <table class=\"" << className << "\">" << endl;
}

void HtmlDocVisitor::visitPost(DocParamSect *s)
{
  if (m_hide) return;
  m_t << "  </table>" << endl;
  m_t << "  </dd>" << endl;
  m_t << "</dl>" << endl;
  forceStartParagraph(s);
}

void HtmlDocVisitor::visitPre(DocParamList *pl)
{
  //printf("DocParamList::visitPre\n");
  if (m_hide) return;
  m_t << "    <tr>";
  DocParamSect *sect = 0;
  if (pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    sect=(DocParamSect*)pl->parent();
  }
  if (sect && sect->hasInOutSpecifier())
  {
    m_t << "<td class=\"paramdir\">";
    if (pl->direction()!=DocParamSect::Unspecified)
    {
      m_t << "[";
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
      m_t << "]";
    }
    m_t << "</td>";
  }
  if (sect && sect->hasTypeSpecifier())
  {
    m_t << "<td class=\"paramtype\">";
    QListIterator<DocNode> li(pl->paramTypes());
    DocNode *type;
    for (li.toFirst();(type=li.current());++li)
    {
      if (type->kind()==DocNode::Kind_Word)
      {
        visit((DocWord*)type); 
      }
      else if (type->kind()==DocNode::Kind_LinkedWord)
      {
        visit((DocLinkedWord*)type); 
      }
      else if (type->kind()==DocNode::Kind_Sep)
      {
        m_t << "&#160;" << ((DocSeparator *)type)->chars() << "&#160;";
      }
    }
    m_t << "</td>";
  }
  m_t << "<td class=\"paramname\">";
  //QStrListIterator li(pl->parameters());
  //const char *s;
  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  bool first=TRUE;
  for (li.toFirst();(param=li.current());++li)
  {
    if (!first) m_t << ","; else first=FALSE;
    if (param->kind()==DocNode::Kind_Word)
    {
      visit((DocWord*)param); 
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      visit((DocLinkedWord*)param); 
    }
  }
  m_t << "</td><td>";
}

void HtmlDocVisitor::visitPost(DocParamList *)
{
  //printf("DocParamList::visitPost\n");
  if (m_hide) return;
  m_t << "</td></tr>" << endl;
}

void HtmlDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;

  forceEndParagraph(x);
  bool anonymousEnum = x->file()=="@";
  if (!anonymousEnum)
  {
    m_t << "<dl" << getDirHtmlClassOfNode(getTextDirByConfig(x), x->key())  
        << "><dt><b><a class=\"el\" href=\""
        << x->relPath() << addHtmlExtensionIfMissing(x->file())
        << "#" << x->anchor() << "\">";
  }
  else 
  {
    m_t << "<dl class=\"" << x->key() << "\"><dt><b>";
  }
  filter(x->title());
  m_t << ":";
  if (!anonymousEnum) m_t << "</a>";
  m_t << "</b></dt><dd>";
}

void HtmlDocVisitor::visitPost(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "</dd></dl>" << endl;
  forceStartParagraph(x);
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

void HtmlDocVisitor::visitPre(DocText *)
{
}

void HtmlDocVisitor::visitPost(DocText *)
{
}

void HtmlDocVisitor::visitPre(DocHtmlBlockQuote *b)
{
  if (m_hide) return;
  forceEndParagraph(b);
  QCString attrs = htmlAttribsToString(b->attribs());
  if (attrs.isEmpty())
  {
    m_t << "<blockquote" << getDirHtmlClassOfNode(getTextDirByConfig(b), "doxtable")
      << ">\n";
  }
  else
  {
    m_t << "<blockquote" << getDirHtmlClassOfNode(getTextDirByConfig(b))
      << htmlAttribsToString(b->attribs()) << ">\n";
  }
}

void HtmlDocVisitor::visitPost(DocHtmlBlockQuote *b)
{
  if (m_hide) return;
  m_t << "</blockquote>" << endl;
  forceStartParagraph(b);
}

void HtmlDocVisitor::visitPre(DocVhdlFlow *vf)
{
  if (m_hide) return;
  if (VhdlDocGen::getFlowMember()) // use VHDL flow chart creator
  {
    forceEndParagraph(vf);
    QCString fname=FlowChart::convertNameToFileName();
    m_t << "<p>";
    m_t << "flowchart: " ; // TODO: translate me
    m_t << "<a href=\"";
    m_t << fname.data(); 
    m_t << ".svg\">";
    m_t << VhdlDocGen::getFlowMember()->name().data();
    m_t << "</a>";
    if (vf->hasCaption())
    {
      m_t << "<br />";
    }
  }
}

void HtmlDocVisitor::visitPost(DocVhdlFlow *vf)
{
  if (m_hide) return;
  if (VhdlDocGen::getFlowMember()) // use VHDL flow chart creator
  {
    m_t << "</p>";
    forceStartParagraph(vf);
  }
}

void HtmlDocVisitor::visitPre(DocParBlock *)
{
  if (m_hide) return;
}

void HtmlDocVisitor::visitPost(DocParBlock *)
{
  if (m_hide) return;
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
      case '\\': if ((*p == '(') || (*p == ')'))
                   m_t << "\\&zwj;" << *p++;
                 else
                   m_t << c;
                 break;
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
      case '<':  m_t << "&lt;"; break;
      case '>':  m_t << "&gt;"; break;
      case '\\': if ((*p == '(') || (*p == ')'))
                   m_t << "\\&zwj;" << *p++;
                 else
                   m_t << c;
                 break;
      default:   m_t << c;
    }
  }
}

void HtmlDocVisitor::startLink(const QCString &ref,const QCString &file,
                               const QCString &relPath,const QCString &anchor,
                               const QCString &tooltip)
{
  //printf("HtmlDocVisitor: file=%s anchor=%s\n",file.data(),anchor.data());
  if (!ref.isEmpty()) // link to entity imported via tag file
  {
    m_t << "<a class=\"elRef\" ";
    m_t << externalLinkTarget();
  }
  else // local link
  {
    m_t << "<a class=\"el\" ";
  }
  m_t << "href=\"";
  m_t << externalRef(relPath,ref,TRUE);
  if (!file.isEmpty())
  {
    m_t << addHtmlExtensionIfMissing(file);
  }
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\"";
  if (!tooltip.isEmpty()) m_t << " title=\"" << convertToHtml(tooltip) << "\"";
  m_t << ">";
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

void HtmlDocVisitor::writeDotFile(const QCString &fn,const QCString &relPath,
                                  const QCString &context)
{
  QCString baseName=fn;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1) // strip extension
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dot_");
  QCString outDir = Config_getString(HTML_OUTPUT);
  writeDotGraphFromFile(fn,outDir,baseName,GOF_BITMAP);
  writeDotImageMapFromFile(m_t,fn,outDir,relPath,baseName,context);
}

void HtmlDocVisitor::writeMscFile(const QCString &fileName,
                                  const QCString &relPath,
                                  const QCString &context)
{
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1) // strip path
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1) // strip extension
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("msc_");
  QCString outDir = Config_getString(HTML_OUTPUT);
  QCString imgExt = getDotImageExtension();
  MscOutputFormat mscFormat = MSC_BITMAP;
  if ("svg" == imgExt)
    mscFormat = MSC_SVG;
  writeMscGraphFromFile(fileName,outDir,baseName,mscFormat);
  writeMscImageMapFromFile(m_t,fileName,outDir,relPath,baseName,context,mscFormat);
}

void HtmlDocVisitor::writeDiaFile(const QCString &fileName,
                                  const QCString &relPath,
                                  const QCString &)
{
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1) // strip path
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1) // strip extension
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dia_");
  QCString outDir = Config_getString(HTML_OUTPUT);
  writeDiaGraphFromFile(fileName,outDir,baseName,DIA_BITMAP);

  m_t << "<img src=\"" << relPath << baseName << ".png" << "\" />" << endl;
}

void HtmlDocVisitor::writePlantUMLFile(const QCString &fileName,
                                       const QCString &relPath,
                                       const QCString &)
{
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1) // strip path
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.findRev('.'))!=-1) // strip extension
  {
    baseName=baseName.left(i);
  }
  static QCString outDir = Config_getString(HTML_OUTPUT);
  QCString imgExt = getDotImageExtension();
  if (imgExt=="svg")
  {
    PlantumlManager::instance()->generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_SVG);
    //m_t << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" << relPath << baseName << ".svg" << "\" />" << endl;
    //m_t << "<p><b>This browser is not able to show SVG: try Firefox, Chrome, Safari, or Opera instead.</b></p>";
    //m_t << "</iframe>" << endl;
    m_t << "<object type=\"image/svg+xml\" data=\"" << relPath << baseName << ".svg\"></object>" << endl;
  }
  else
  {
    PlantumlManager::instance()->generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_BITMAP);
    m_t << "<img src=\"" << relPath << baseName << ".png" << "\" />" << endl;
  }
}

/** Returns TRUE if the child nodes in paragraph \a para until \a nodeIndex
    contain a style change node that is still active and that style change is one that
    must be located outside of a paragraph, i.e. it is a center, div, or pre tag.
    See also bug746162.
 */
static bool insideStyleChangeThatIsOutsideParagraph(DocPara *para,int nodeIndex)
{
  //printf("insideStyleChangeThatIsOutputParagraph(index=%d)\n",nodeIndex);
  int styleMask=0;
  bool styleOutsideParagraph=FALSE;
  while (nodeIndex>=0 && !styleOutsideParagraph)
  {
    DocNode *n = para->children().at(nodeIndex);
    if (n->kind()==DocNode::Kind_StyleChange)
    {
      DocStyleChange *sc = (DocStyleChange*)n;
      if (!sc->enable()) // remember styles that has been closed already
      {
        styleMask|=(int)sc->style();
      }
      bool paraStyle = sc->style()==DocStyleChange::Center ||
                       sc->style()==DocStyleChange::Div    ||
                       sc->style()==DocStyleChange::Preformatted;
      //printf("Found style change %s enabled=%d\n",sc->styleString(),sc->enable());
      if (sc->enable() && (styleMask&(int)sc->style())==0 && // style change that is still active
          paraStyle
         )
      {
        styleOutsideParagraph=TRUE;
      }
    }
    nodeIndex--;
  }
  return styleOutsideParagraph;
}

/** Used for items found inside a paragraph, which due to XHTML restrictions
 *  have to be outside of the paragraph. This method will forcefully end
 *  the current paragraph and forceStartParagraph() will restart it.
 */
void HtmlDocVisitor::forceEndParagraph(DocNode *n)
{
  //printf("forceEndParagraph(%p) %d\n",n,n->kind());
  if (n->parent() && n->parent()->kind()==DocNode::Kind_Para)
  {
    DocPara *para = (DocPara*)n->parent();
    int nodeIndex = para->children().findRef(n);
    nodeIndex--;
    if (nodeIndex<0) return; // first node in paragraph
    while (nodeIndex>=0 && isInvisibleNode(para->children().at(nodeIndex)))
    {
        nodeIndex--;
    }
    if (nodeIndex<0) return; // first visible node in paragraph
    n = para->children().at(nodeIndex);
    if (mustBeOutsideParagraph(n)) return; // previous node already outside paragraph context
    nodeIndex--;
    bool styleOutsideParagraph=insideStyleChangeThatIsOutsideParagraph(para,nodeIndex);
    bool isFirst;
    bool isLast;
    getParagraphContext(para,isFirst,isLast);
    //printf("forceEnd first=%d last=%d styleOutsideParagraph=%d\n",isFirst,isLast,styleOutsideParagraph);
    if (isFirst && isLast) return;
    if (styleOutsideParagraph) return;

    m_t << "</p>";
  }
}

/** Used for items found inside a paragraph, which due to XHTML restrictions
 *  have to be outside of the paragraph. This method will forcefully start
 *  the paragraph, that was previously ended by forceEndParagraph().
 */
void HtmlDocVisitor::forceStartParagraph(DocNode *n)
{
  //printf("forceStartParagraph(%p) %d\n",n,n->kind());
  if (n->parent() && n->parent()->kind()==DocNode::Kind_Para) // if we are inside a paragraph
  {
    DocPara *para = (DocPara*)n->parent();
    int nodeIndex = para->children().findRef(n);
    int numNodes  = para->children().count();
    bool styleOutsideParagraph=insideStyleChangeThatIsOutsideParagraph(para,nodeIndex);
    if (styleOutsideParagraph) return;
    nodeIndex++;
    if (nodeIndex==numNodes) return; // last node
    while (nodeIndex<numNodes && isInvisibleNode(para->children().at(nodeIndex)))
    {
      nodeIndex++;
    }
    if (nodeIndex<numNodes)
    {
      n = para->children().at(nodeIndex);
      if (mustBeOutsideParagraph(n)) return; // next element also outside paragraph
    }
    else
    {
      return; // only whitespace at the end!
    }

    bool needsTag = TRUE;
    bool isFirst;
    bool isLast;
    getParagraphContext(para,isFirst,isLast);
    if (isFirst && isLast) needsTag = FALSE;
    //printf("forceStart first=%d last=%d needsTag=%d\n",isFirst,isLast,needsTag);

    if (needsTag)
      m_t << "<p" << getDirHtmlClassOfNode(getTextDirByConfig(para, nodeIndex)) << ">";
    else
      m_t << getHtmlDirEmbeddingChar(getTextDirByConfig(para, nodeIndex));
  }
}

