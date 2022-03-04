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
#include "fileinfo.h"

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
static const char *hex="0123456789ABCDEF";

static QCString convertIndexWordToAnchor(const QCString &word)
{
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

static bool mustBeOutsideParagraph(const DocNode *n)
{
  if (DocNodeMultiCast< /* <ul> */         DocHtmlList,   DocSimpleList, DocAutoList,
                        /* <dl> */         DocSimpleSect, DocParamSect,  DocHtmlDescList, DocXRefItem,
                        /* <table> */      DocHtmlTable,
                        /* <h?> */         DocSection,    DocHtmlHeader,
                        /* \internal */    DocInternal,
                        /* <div> */        DocInclude,    DocSecRefList,
                        /* <hr> */         DocHorRuler,
                        /* <blockquote> */ DocHtmlBlockQuote,
                        /* \parblock */    DocParBlock,
                                           DocIncOperator >::is_a(n))
  {
    return TRUE;
  }
  const DocVerbatim *dv = dynamic_cast<const DocVerbatim*>(n);
  if (dv)
  {
    DocVerbatim::Type t = dv->type();
    if (t == DocVerbatim::JavaDocCode || t == DocVerbatim::JavaDocLiteral) return FALSE;
    return t!=DocVerbatim::HtmlOnly || dv->isBlock();
  }
  const DocStyleChange *sc = dynamic_cast<const DocStyleChange*>(n);
  if (sc)
  {
    return sc->style()==DocStyleChange::Preformatted ||
           sc->style()==DocStyleChange::Div ||
           sc->style()==DocStyleChange::Center;
  }
  const DocFormula *df = dynamic_cast<const DocFormula*>(n);
  if (df)
  {
    return !df->isInline();
  }
  const DocImage *di = dynamic_cast<const DocImage*>(n);
  if (di)
  {
    return !di->isInlineImage();
  }
  return FALSE;
}

static bool isDocVerbatimVisible(const DocVerbatim *s)
{
  switch (s->type())
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

static bool isDocIncludeVisible(const DocInclude *s)
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

static bool isDocIncOperatorVisible(const DocIncOperator *s)
{
  switch (s->type())
  {
    case DocIncOperator::Skip:
      return FALSE;
    default:
      return TRUE;
  }
}

static bool isInvisibleNode(const DocNode *node)
{
  // skip over white space
  const DocWhiteSpace *ws = dynamic_cast<const DocWhiteSpace *>(node);
  if (ws) return true;
  // skip over image nodes that are not for HTML output
  const DocImage *di = dynamic_cast<const DocImage *>(node);
  if (di) return di->type()!=DocImage::Html;
  // skip over verbatim nodes that are not visible in the HTML output
  const DocVerbatim *dv = dynamic_cast<const DocVerbatim *>(node);
  if (dv) return !isDocVerbatimVisible(dv);
  // skip over include nodes that are not visible in the HTML output
  const DocInclude *dinc = dynamic_cast<const DocInclude *>(node);
  if (dinc) return !isDocIncludeVisible(dinc);
  const DocIncOperator *dio = dynamic_cast<const DocIncOperator *>(node);
  // skip over include operator nodes that are not visible in the HTML output
  if (dio) return !isDocIncOperatorVisible(dio);
  return false;
}

static void mergeHtmlAttributes(const HtmlAttribList &attribs, HtmlAttribList &mergeInto)
{
  for (const auto &att : attribs)
  {
    auto it = std::find_if(mergeInto.begin(),mergeInto.end(),
                           [&att](const auto &opt) { return opt.name==att.name; });
    if (it!=mergeInto.end()) // attribute name already in mergeInto
    {
       it->value = it->value + " " + att.value;
    }
    else // attribute name not yet in mergeInto
    {
      mergeInto.push_back(att);
    }
  }
}

static QCString htmlAttribsToString(const HtmlAttribList &attribs, QCString *pAltValue = 0)
{
  QCString result;
  for (const auto &att : attribs)
  {
    if (!att.value.isEmpty())  // ignore attribute without values as they
                                // are not XHTML compliant, with the exception
				// of the alt attribute with the img tag
    {
      if (att.name=="alt" && pAltValue) // optionally return the value of alt separately
                                         // need to convert <img> to <object> for SVG images,
                                         // which do not support the alt attribute
      {
        *pAltValue = att.value;
      }
      else
      {
        result+=" ";
        result+=att.name;
        result+="=\""+convertToXML(att.value)+"\"";
      }
    }
    else if (att.name=="open")
    {
        // The open attribute is a boolean attribute.
        // Specifies that the details should be visible (open) to the user
        // As it is a boolean attribute the initialisation value is of no interest
        result+=" ";
        result+=att.name;
        result+="=\"true\"";
    }
    else if (att.name=="nowrap") // In XHTML, attribute minimization is forbidden, and the nowrap attribute must be defined as <td nowrap="nowrap">.
    {
        result+=" ";
        result+=att.name;
        result+="=\"nowrap\"";
    }
  }
  return result;
}

//-------------------------------------------------------------------------

HtmlDocVisitor::HtmlDocVisitor(TextStream &t,CodeOutputInterface &ci,
                               const Definition *ctx)
  : DocVisitor(DocVisitor_Html), m_t(t), m_ci(ci), m_ctx(ctx)
{
  if (ctx) m_langExt=ctx->getDefFileExtension();
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void HtmlDocVisitor::visit(DocWord *w)
{
  //printf("word: %s\n",qPrint(w->word()));
  if (m_hide) return;
  filter(w->word());
}

void HtmlDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  //printf("linked word: %s\n",qPrint(w->word()));
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
  if (m_insideTitle &&
      (s->symbol()==DocSymbol::Sym_Quot || s->symbol()==DocSymbol::Sym_quot)) // escape "'s inside title="..."
  {
    m_t << "&quot;";
  }
  else
  {
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
}

void HtmlDocVisitor::visit(DocEmoji *s)
{
  if (m_hide) return;
  const char *res = EmojiEntityMapper::instance()->unicode(s->index());
  if (res)
  {
    m_t << "<span class=\"emoji\">"<<res<<"</span>";
  }
  else
  {
    m_t << s->name();
  }
}

void HtmlDocVisitor::writeObfuscatedMailAddress(const QCString &url)
{
  if (!Config_getBool(OBFUSCATE_EMAILS))
  {
    m_t << "<a href=\"mailto:" << url << "\">";
  }
  else
  {
    m_t << "<a href=\"#\" onclick=\"location.href='mai'+'lto:'";
    if (!url.isEmpty())
    {
      const char *p = url.data();
      uint size=3;
      while (*p)
      {
        m_t << "+'";
        for (uint j=0;j<size && *p;j++)
        {
          p = writeUTF8Char(m_t,p);
        }
        m_t << "'";
        if (size==3) size=2; else size=3;
      }
    }
    m_t << "; return false;\">";
  }
}

void HtmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  if (u->isEmail()) // mail address
  {
    QCString url = u->url();
    // obfuscate the mail address link
    writeObfuscatedMailAddress(url);
    if (!Config_getBool(OBFUSCATE_EMAILS))
    {
      m_t << url;
    }
    else
    {
      const char *p = url.data();
      // also obfuscate the address as shown on the web page
      uint size=5;
      while (*p)
      {
        for (uint j=0;j<size && *p;j++)
        {
          p = writeUTF8Char(m_t,p);
        }
        if (*p) m_t << "<span class=\"obfuscator\">.nosp@m.</span>";
        if (size==5) size=4; else size=5;
      }
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
    case DocStyleChange::Cite:
      if (s->enable()) m_t << "<cite" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</cite>";
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
    case DocStyleChange::Details:
      if (s->enable()) m_t << "<details" << htmlAttribsToString(s->attribs()) << ">\n"; else m_t << "</details>\n";
      break;
    case DocStyleChange::Summary:
      if (s->enable()) m_t << "<summary" << htmlAttribsToString(s->attribs()) << ">";  else m_t << "</summary>";
      break;
  }
}


static void visitPreCaption(TextStream &t, DocVerbatim *s)
{
  if (s->hasCaption())
  {
    t << "<div class=\"caption\">\n";
  }
}


static void visitPostCaption(TextStream &t, DocVerbatim *s)
{
  if (s->hasCaption())
  {
    t << "</div>\n";
  }
}


static void visitCaption(HtmlDocVisitor *parent, DocNodeList &children)
{
  for (const auto &n : children) n->accept(parent);
}

void HtmlDocVisitor::visit(DocVerbatim *s)
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
      forceEndParagraph(s);
      m_ci.startCodeFragment("DoxyCode");
      getCodeParser(lang).parseCode(m_ci,
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
      m_ci.endCodeFragment("DoxyCode");
      forceStartParagraph(s);
      break;
    case DocVerbatim::Verbatim:
      forceEndParagraph(s);
      m_t << "<pre class=\"fragment\">";
      filter(s->text());
      m_t << "</pre>";
      forceStartParagraph(s);
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s->text(), true);
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "<code class=\"JavaDocCode\">";
      filter(s->text(), true);
      m_t << "</code>";
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
            qPrint(Config_getString(HTML_OUTPUT)+"/inline_dotgraph_"),
            dotindex++,
            ".dot"
           );
        std::ofstream file(fileName.str(),std::ofstream::out | std::ofstream::binary);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(fileName));
        }
        else
        {
          QCString stext = s->text();
          file.write( stext.data(), stext.length() );
          file.close();

          m_t << "<div class=\"dotgraph\">\n";
          writeDotFile(fileName,s->relPath(),s->context(),s->srcFile(),s->srcLine());
          visitPreCaption(m_t, s);
          visitCaption(this, s->children());
          visitPostCaption(m_t, s);
          m_t << "</div>\n";

          if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName.str());
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
            qPrint(Config_getString(HTML_OUTPUT)+"/inline_mscgraph_"),
            mscindex++
            );
        std::ofstream file(baseName.str()+".msc",std::ofstream::out | std::ofstream::binary);
        if (!file.is_open())
        {
          err("Could not open file %s.msc for writing\n",qPrint(baseName));
        }
        else
        {
          QCString text = "msc {";
          text+=s->text();
          text+="}";

          file.write( text.data(), text.length() );
          file.close();

          m_t << "<div class=\"mscgraph\">\n";
          writeMscFile(baseName+".msc",s->relPath(),s->context(),s->srcFile(),s->srcLine());
          visitPreCaption(m_t, s);
          visitCaption(this, s->children());
          visitPostCaption(m_t, s);
          m_t << "</div>\n";

          if (Config_getBool(DOT_CLEANUP)) Dir().remove(baseName.str()+".msc");
        }
        forceStartParagraph(s);
      }
      break;
    case DocVerbatim::PlantUML:
      {
        forceEndParagraph(s);
        QCString htmlOutput = Config_getString(HTML_OUTPUT);
        QCString imgExt = getDotImageExtension();
        PlantumlManager::OutputFormat format = PlantumlManager::PUML_BITMAP;	// default : PUML_BITMAP
        if (imgExt=="svg")
        {
          format = PlantumlManager::PUML_SVG;
        }
        QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                                    htmlOutput,s->exampleFile(),
                                    s->text(),format,s->engine(),s->srcFile(),s->srcLine());
        m_t << "<div class=\"plantumlgraph\">\n";
        writePlantUMLFile(baseName,s->relPath(),s->context(),s->srcFile(),s->srcLine());
        visitPreCaption(m_t, s);
        visitCaption(this, s->children());
        visitPostCaption(m_t, s);
        m_t << "</div>\n";
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
      m_ci.startCodeFragment("DoxyCode");
      getCodeParser(inc->extension()).parseCode(m_ci,
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
      m_ci.endCodeFragment("DoxyCode");
      forceStartParagraph(inc);
      break;
    case DocInclude::IncWithLines:
      {
         forceEndParagraph(inc);
         m_ci.startCodeFragment("DoxyCode");
         FileInfo cfi( inc->file().str() );
         FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         getCodeParser(inc->extension()).parseCode(m_ci,
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
         m_ci.endCodeFragment("DoxyCode");
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
      m_t << "<pre class=\"fragment\">";
      filter(inc->text());
      m_t << "</pre>";
      forceStartParagraph(inc);
      break;
    case DocInclude::Snippet:
      {
         forceEndParagraph(inc);
         m_ci.startCodeFragment("DoxyCode");
         getCodeParser(inc->extension()).parseCode(m_ci,
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
         m_ci.endCodeFragment("DoxyCode");
         forceStartParagraph(inc);
      }
      break;
    case DocInclude::SnipWithLines:
      {
         forceEndParagraph(inc);
         m_ci.startCodeFragment("DoxyCode");
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
                                           TRUE,  // show line number
                                           m_ctx  // search context
                                          );
         delete fd;
         m_ci.endCodeFragment("DoxyCode");
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
  //    op->type(),op->isFirst(),op->isLast(),qPrint(op->text()));
  if (op->isFirst())
  {
    forceEndParagraph(op);
    if (!m_hide) m_ci.startCodeFragment("DoxyCode");
    pushHidden(m_hide);
    m_hide=TRUE;
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
      getCodeParser(locLangExt).parseCode(
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
    pushHidden(m_hide);
    m_hide=TRUE;
  }
  if (op->isLast())
  {
    m_hide = popHidden();
    if (!m_hide) m_ci.endCodeFragment("DoxyCode");
    forceStartParagraph(op);
  }
  else
  {
    if (!m_hide) m_t << "\n";
  }
}

void HtmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  bool bDisplay = !f->isInline();
  if (bDisplay)
  {
    forceEndParagraph(f);
    m_t << "<p class=\"formulaDsp\">\n";
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
    else if (!bDisplay && !text.isEmpty())
    {
      closeInline=TRUE;
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
    if (Config_getEnum(HTML_FORMULA_FORMAT)==HTML_FORMULA_FORMAT_t::svg)
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
    m_t << "\n</p>\n";
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
  m_t << "<a id=\"" << anchor << "\" name=\"" << anchor << "\"></a>";
  //printf("*** DocIndexEntry: word='%s' scope='%s' member='%s'\n",
  //       qPrint(e->entry()),
  //       e->scope()  ? qPrint(e->scope()->name())  : "<null>",
  //       e->member() ? qPrint(e->member()->name()) : "<null>"
  //      );
  Doxygen::indexList->addIndexItem(e->scope(),e->member(),anchor,e->entry());
}

void HtmlDocVisitor::visit(DocSimpleSectSep *)
{
  m_t << "</dd>\n";
  m_t << "<dd>\n";
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
    m_t << "<ol type=\"" << types[l->depth() % NUM_HTML_LIST_TYPES] << "\">";
  }
  else
  {
    m_t << "<ul>";
  }
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
   return !parent->children().empty() && parent->children().front().get()==node;
}

template<class T>
bool isLastChildNode(T *parent, DocNode *node)
{
   return !parent->children().empty() && parent->children().back().get()==node;
}

bool isSeparatedParagraph(const DocSimpleSect *parent,DocPara *par)
{
  const DocNodeList &nodes = parent->children();
  auto it = std::find_if(nodes.begin(),nodes.end(),[par](const auto &n) { return n.get()==par; });
  if (it==nodes.end()) return FALSE;
  size_t i = it - nodes.begin();
  size_t count = parent->children().size();
  auto isSeparator = [&nodes](size_t i_) { return dynamic_cast<DocSimpleSectSep*>(nodes.at(i_).get())!=0; };
  if (count>1 && i==0) // first node
  {
    return isSeparator(1);
  }
  else if (count>1 && i==count-1) // last node
  {
    return isSeparator(i-1);
  }
  else if (count>2 && i>0 && i<count-1) // intermediate node
  {
    return isSeparator(i-1) && isSeparator(i+1);
  }
  return false;
}

static int getParagraphContext(DocPara *p,bool &isFirst,bool &isLast)
{
  int t=0;
  isFirst=FALSE;
  isLast=FALSE;
  if (p && p->parent())
  {
    const auto parBlock = dynamic_cast<const DocParBlock*>(p->parent());
    if (parBlock)
    {
      // hierarchy: node N -> para -> parblock -> para
      // adapt return value to kind of N
      DocNode *p3 = 0;
      if ( p->parent()->parent() && p->parent()->parent()->parent() )
      {
        p3 = p->parent()->parent()->parent();
      }
      isFirst=isFirstChildNode(parBlock,p);
      isLast =isLastChildNode (parBlock,p);
      bool isLI = DocNodeMultiCast<DocHtmlListItem,DocSecRefItem>::is_a(p3);
      bool isDD = DocNodeMultiCast<DocHtmlDescData,DocXRefItem,DocSimpleSect>::is_a(p3);
      bool isTD = DocNodeMultiCast<DocHtmlCell,DocParamList>::is_a(p3);
      t=NONE;
      if (isFirst)
      {
        if (isLI) t=STARTLI; else if (isDD) t=STARTDD; else if (isTD) t=STARTTD;
      }
      if (isLast)
      {
        if (isLI) t=ENDLI;   else if (isDD) t=ENDDD;   else if (isTD) t=ENDTD;
      }
      if (!isFirst && !isLast)
      {
        if (isLI) t=INTERLI; else if (isDD) t=INTERDD; else if (isTD) t=INTERTD;
      }
      return t;
    }
    const auto docAutoListItem = dynamic_cast<const DocAutoListItem*>(p->parent());
    if (docAutoListItem)
    {
      isFirst=isFirstChildNode(docAutoListItem,p);
      isLast =isLastChildNode (docAutoListItem,p);
      t=STARTLI; // not used
      return t;
    }
    const auto docSimpleListItem = dynamic_cast<const DocSimpleListItem*>(p->parent());
    if (docSimpleListItem)
    {
      isFirst=TRUE;
      isLast =TRUE;
      t=STARTLI; // not used
      return t;
    }
    const auto docParamList = dynamic_cast<const DocParamList*>(p->parent());
    if (docParamList)
    {
      isFirst=TRUE;
      isLast =TRUE;
      t=STARTLI; // not used
      return t;
    }
    const auto docHtmlListItem = dynamic_cast<const DocHtmlListItem*>(p->parent());
    if (docHtmlListItem)
    {
      isFirst=isFirstChildNode(docHtmlListItem,p);
      isLast =isLastChildNode (docHtmlListItem,p);
      if (isFirst) t=STARTLI;
      if (isLast)  t=ENDLI;
      if (!isFirst && !isLast) t = INTERLI;
      return t;
    }
    const auto docSecRefItem = dynamic_cast<const DocSecRefItem*>(p->parent());
    if (docSecRefItem)
    {
      isFirst=isFirstChildNode(docSecRefItem,p);
      isLast =isLastChildNode (docSecRefItem,p);
      if (isFirst) t=STARTLI;
      if (isLast)  t=ENDLI;
      if (!isFirst && !isLast) t = INTERLI;
      return t;
    }
    const auto docHtmlDescData = dynamic_cast<const DocHtmlDescData*>(p->parent());
    if (docHtmlDescData)
    {
      isFirst=isFirstChildNode(docHtmlDescData,p);
      isLast =isLastChildNode (docHtmlDescData,p);
      if (isFirst) t=STARTDD;
      if (isLast)  t=ENDDD;
      if (!isFirst && !isLast) t = INTERDD;
      return t;
    }
    const auto docXRefItem = dynamic_cast<const DocXRefItem*>(p->parent());
    if (docXRefItem)
    {
      isFirst=isFirstChildNode(docXRefItem,p);
      isLast =isLastChildNode (docXRefItem,p);
      if (isFirst) t=STARTDD;
      if (isLast)  t=ENDDD;
      if (!isFirst && !isLast) t = INTERDD;
      return t;
    }
    const auto docSimpleSect = dynamic_cast<const DocSimpleSect*>(p->parent());
    if (docSimpleSect)
    {
      isFirst=isFirstChildNode(docSimpleSect,p);
      isLast =isLastChildNode (docSimpleSect,p);
      if (isFirst) t=STARTDD;
      if (isLast)  t=ENDDD;
      if (isSeparatedParagraph(docSimpleSect,p))
        // if the paragraph is enclosed with separators it will
        // be included in <dd>..</dd> so avoid addition paragraph
        // markers
      {
        isFirst=isLast=TRUE;
      }
      if (!isFirst && !isLast) t = INTERDD;
      return t;
    }
    const auto docHtmlCell = dynamic_cast<DocHtmlCell*>(p->parent());
    if (docHtmlCell)
    {
      isFirst=isFirstChildNode(docHtmlCell,p);
      isLast =isLastChildNode (docHtmlCell,p);
      if (isFirst) t=STARTTD;
      if (isLast)  t=ENDTD;
      if (!isFirst && !isLast) t = INTERTD;
      return t;
    }
  }
  return t;
}

static bool determineIfNeedsTag(DocPara *p)
{
  bool needsTag = FALSE;
  if (p && p->parent())
  {
    if (DocNodeMultiCast<DocSection,
                         DocInternal,
                         DocHtmlListItem,
                         DocHtmlDescData,
                         DocHtmlCell,
                         DocSimpleListItem,
                         DocAutoListItem,
                         DocSimpleSect,
                         DocXRefItem,
                         DocHtmlBlockQuote,
                         DocParBlock
                         >::is_a(p->parent()))
    {
      needsTag = TRUE;
    }
    else if (dynamic_cast<DocRoot*>(p->parent()))
    {
      needsTag = !dynamic_cast<DocRoot*>(p->parent())->singleLine();
    }
  }
  return needsTag;
}

void HtmlDocVisitor::visitPre(DocPara *p)
{
  if (m_hide) return;

  //printf("DocPara::visitPre: parent of kind %d ",
  //       p->parent() ? p->parent()->kind() : -1);

  bool needsTag = determineIfNeedsTag(p);

  // if the first element of a paragraph is something that should be outside of
  // the paragraph (<ul>,<dl>,<table>,..) then that will already started the
  // paragraph and we don't need to do it here
  size_t nodeIndex = 0;
  if (p && nodeIndex<p->children().size())
  {
    while (nodeIndex<p->children().size() && isInvisibleNode(p->children().at(nodeIndex).get()))
    {
      nodeIndex++;
    }
    if (nodeIndex<p->children().size())
    {
      const DocNode *n = p->children().at(nodeIndex).get();
      if (mustBeOutsideParagraph(n))
      {
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
  {
    if (strlen(contexts[t]))
      m_t << "<p class=\"" << contexts[t] << "\"" << htmlAttribsToString(p->attribs()) << ">";
    else
      m_t << "<p " << htmlAttribsToString(p->attribs()) << ">";
  }
}

void HtmlDocVisitor::visitPost(DocPara *p)
{

  //printf("DocPara::visitPost: parent of kind %d ",
  //       p->parent() ? p->parent()->kind() : -1);

  bool needsTag = determineIfNeedsTag(p);

  // if the last element of a paragraph is something that should be outside of
  // the paragraph (<ul>,<dl>,<table>) then that will already have ended the
  // paragraph and we don't need to do it here
  if (!p->children().empty())
  {
    int nodeIndex = static_cast<int>(p->children().size()-1);
    while (nodeIndex>=0 && isInvisibleNode(p->children().at(nodeIndex).get()))
    {
      nodeIndex--;
    }
    if (nodeIndex>=0)
    {
      const DocNode *n = p->children().at(nodeIndex).get();
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
  m_t << "<h" << s->level() << ">";
  m_t << "<a class=\"anchor\" id=\"" << s->anchor();
  m_t << "\"></a>\n";
  filter(convertCharEntitiesToUTF8(s->title()));
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
  m_t << ">\n";
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
    m_t << "<table class=\"doxtable\">\n";
  }
  else
  {
    m_t << "<table" << htmlAttribsToString(t->attribs()) << ">\n";
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
  m_t << "<h" << header->level() << htmlAttribsToString(header->attribs()) << ">";
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
    if (!inlineImage) m_t << "<div class=\"image\">\n";
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
      extraAttribs.push_back(opt);
    }
    QCString alt;
    mergeHtmlAttributes(img->attribs(),extraAttribs);
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
        m_t << ">" << alt << "</object>\n";
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
        m_insideTitle=true;
      }
      else
      {
        m_t << "<div class=\"caption\">\n";
      }
    }
    else if (inlineImage)
    {
      m_t << "/>";
    }
  }
  else // other format -> skip
  {
    pushHidden(m_hide);
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
        m_insideTitle=false;
      }
      else // end <div class="caption">
      {
        m_t << "</div>";
      }
    }
    if (!inlineImage) // end <div class="image">
    {
      m_t << "</div>\n";
      forceStartParagraph(img);
    }
  }
  else // other format
  {
    m_hide = popHidden();
  }
}

void HtmlDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df->file(),Config_getString(HTML_OUTPUT)+"/"+stripPath(df->file()));
  m_t << "<div class=\"dotgraph\">\n";
  writeDotFile(df->file(),df->relPath(),df->context(),df->srcFile(),df->srcLine());
  if (df->hasCaption())
  {
    m_t << "<div class=\"caption\">\n";
  }
}

void HtmlDocVisitor::visitPost(DocDotFile *df)
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>\n";
  }
  m_t << "</div>\n";
}

void HtmlDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df->file(),Config_getString(HTML_OUTPUT)+"/"+stripPath(df->file()));
  m_t << "<div class=\"mscgraph\">\n";
  writeMscFile(df->file(),df->relPath(),df->context(),df->srcFile(),df->srcLine());
  if (df->hasCaption())
  {
    m_t << "<div class=\"caption\">\n";
  }
}
void HtmlDocVisitor::visitPost(DocMscFile *df)
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>\n";
  }
  m_t << "</div>\n";
}

void HtmlDocVisitor::visitPre(DocDiaFile *df)
{
  if (m_hide) return;
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df->file(),Config_getString(HTML_OUTPUT)+"/"+stripPath(df->file()));
  m_t << "<div class=\"diagraph\">\n";
  writeDiaFile(df->file(),df->relPath(),df->context(),df->srcFile(),df->srcLine());
  if (df->hasCaption())
  {
    m_t << "<div class=\"caption\">\n";
  }
}
void HtmlDocVisitor::visitPost(DocDiaFile *df)
{
  if (m_hide) return;
  if (df->hasCaption())
  {
    m_t << "</div>\n";
  }
  m_t << "</div>\n";
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
  if (!ref->file().isEmpty())
  {
    m_t << "<li>";
    startLink(ref->ref(),ref->file(),ref->relPath(),ref->isSubPage() ? QCString() : ref->anchor());
  }
}

void HtmlDocVisitor::visitPost(DocSecRefItem *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty())
  {
    endLink();
    m_t << "</li>\n";
  }
}

void HtmlDocVisitor::visitPre(DocSecRefList *s)
{
  if (m_hide) return;
  forceEndParagraph(s);
  m_t << "<div>\n";
  m_t << "<ul class=\"multicol\">\n";
}

void HtmlDocVisitor::visitPost(DocSecRefList *s)
{
  if (m_hide) return;
  m_t << "</ul>\n";
  m_t << "</div>\n";
  forceStartParagraph(s);
}

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
  m_t << "</dt><dd>\n";
  m_t << "  <table class=\"" << className << "\">\n";
}

void HtmlDocVisitor::visitPost(DocParamSect *s)
{
  if (m_hide) return;
  m_t << "  </table>\n";
  m_t << "  </dd>\n";
  m_t << "</dl>\n";
  forceStartParagraph(s);
}

void HtmlDocVisitor::visitPre(DocParamList *pl)
{
  //printf("DocParamList::visitPre\n");
  if (m_hide) return;
  m_t << "    <tr>";
  DocParamSect *sect = dynamic_cast<DocParamSect*>(pl->parent());
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
    for (const auto &type : pl->paramTypes())
    {
      DocWord       *word       = dynamic_cast<DocWord*      >(type.get());
      DocLinkedWord *linkedWord = dynamic_cast<DocLinkedWord*>(type.get());
      DocSeparator  *sep        = dynamic_cast<DocSeparator* >(type.get());
      if (word)
      {
        visit(word);
      }
      else if (linkedWord)
      {
        visit(linkedWord);
      }
      else if (sep)
      {
        m_t << "&#160;" << sep->chars() << "&#160;";
      }
    }
    m_t << "</td>";
  }
  m_t << "<td class=\"paramname\">";
  bool first=TRUE;
  for (const auto &param : pl->parameters())
  {
    if (!first) m_t << ","; else first=FALSE;
    DocWord       *word       = dynamic_cast<DocWord*      >(param.get());
    DocLinkedWord *linkedWord = dynamic_cast<DocLinkedWord*>(param.get());
    if (word)
    {
      visit(word);
    }
    else if (linkedWord)
    {
      visit(linkedWord);
    }
  }
  m_t << "</td><td>";
}

void HtmlDocVisitor::visitPost(DocParamList *)
{
  //printf("DocParamList::visitPost\n");
  if (m_hide) return;
  m_t << "</td></tr>\n";
}

void HtmlDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;

  forceEndParagraph(x);
  bool anonymousEnum = x->file()=="@";
  if (!anonymousEnum)
  {
    m_t << "<dl class=\"" << x->key() << "\"><dt><b><a class=\"el\" href=\""
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
  m_t << "</dd></dl>\n";
  forceStartParagraph(x);
}

void HtmlDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(QCString(),ref->file(),ref->relPath(),ref->anchor());
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
  m_t << "<blockquote class=\"doxtable\"" << htmlAttribsToString(b->attribs()) << ">\n";
}

void HtmlDocVisitor::visitPost(DocHtmlBlockQuote *b)
{
  if (m_hide) return;
  m_t << "</blockquote>\n";
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
    m_t << fname;
    m_t << ".svg\">";
    m_t << VhdlDocGen::getFlowMember()->name();
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



void HtmlDocVisitor::filter(const QCString &str, const bool retainNewline)
{
  if (str.isEmpty()) return;
  const char *p=str.data();
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '\n': if(retainNewline) m_t << "<br/>"; m_t << c; break;
      case '<':  m_t << "&lt;"; break;
      case '>':  m_t << "&gt;"; break;
      case '&':  m_t << "&amp;"; break;
      case '\\': if ((*p == '(') || (*p == ')'))
          m_t << "\\&zwj;" << *p++;
        else
          m_t << c;
        break;
      default:
        {
          uchar uc = static_cast<uchar>(c);
          if (uc<32 && !isspace(c)) // non-printable control characters
          {
            m_t << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
          }
          else
          {
            m_t << c;
          }
        }
        break;
    }
  }
}

/// Escape basic entities to produce a valid CDATA attribute value,
/// assume that the outer quoting will be using the double quote &quot;
void HtmlDocVisitor::filterQuotedCdataAttr(const QCString &str)
{
  if (str.isEmpty()) return;
  const char *p=str.data();
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
      default:
        {
          uchar uc = static_cast<uchar>(c);
          if (uc<32 && !isspace(c)) // non-printable control characters
          {
            m_t << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
          }
          else
          {
            m_t << c;
          }
        }
        break;
    }
  }
}

void HtmlDocVisitor::startLink(const QCString &ref,const QCString &file,
                               const QCString &relPath,const QCString &anchor,
                               const QCString &tooltip)
{
  //printf("HtmlDocVisitor: file=%s anchor=%s\n",qPrint(file),qPrint(anchor));
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

void HtmlDocVisitor::writeDotFile(const QCString &fn,const QCString &relPath,
                                  const QCString &context,const QCString &srcFile,int srcLine)
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
  writeDotGraphFromFile(fn,outDir,baseName,GOF_BITMAP,srcFile,srcLine);
  writeDotImageMapFromFile(m_t,fn,outDir,relPath,baseName,context,-1,srcFile,srcLine);
}

void HtmlDocVisitor::writeMscFile(const QCString &fileName,const QCString &relPath,
                                  const QCString &context,const QCString &srcFile,int srcLine)
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
  writeMscGraphFromFile(fileName,outDir,baseName,mscFormat,srcFile,srcLine);
  writeMscImageMapFromFile(m_t,fileName,outDir,relPath,baseName,context,mscFormat,srcFile,srcLine);
}

void HtmlDocVisitor::writeDiaFile(const QCString &fileName, const QCString &relPath,
                                  const QCString &,const QCString &srcFile,int srcLine)
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
  writeDiaGraphFromFile(fileName,outDir,baseName,DIA_BITMAP,srcFile,srcLine);

  m_t << "<img src=\"" << relPath << baseName << ".png" << "\" />\n";
}

void HtmlDocVisitor::writePlantUMLFile(const QCString &fileName, const QCString &relPath,
                                       const QCString &,const QCString &srcFile,int srcLine)
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
  QCString outDir = Config_getString(HTML_OUTPUT);
  QCString imgExt = getDotImageExtension();
  if (imgExt=="svg")
  {
    PlantumlManager::instance().generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_SVG);
    //m_t << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" << relPath << baseName << ".svg" << "\" />\n";
    //m_t << "<p><b>This browser is not able to show SVG: try Firefox, Chrome, Safari, or Opera instead.</b></p>";
    //m_t << "</iframe>\n";
    m_t << "<object type=\"image/svg+xml\" data=\"" << relPath << baseName << ".svg\"></object>\n";
  }
  else
  {
    PlantumlManager::instance().generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_BITMAP);
    m_t << "<img src=\"" << relPath << baseName << ".png" << "\" />\n";
  }
}

/** Returns TRUE if the child nodes in paragraph \a para until \a nodeIndex
    contain a style change node that is still active and that style change is one that
    must be located outside of a paragraph, i.e. it is a center, div, or pre tag.
    See also bug746162.
 */
static bool insideStyleChangeThatIsOutsideParagraph(const DocPara *para,int nodeIndex)
{
  //printf("insideStyleChangeThatIsOutputParagraph(index=%d)\n",nodeIndex);
  int styleMask=0;
  bool styleOutsideParagraph=FALSE;
  while (nodeIndex>=0 && !styleOutsideParagraph)
  {
    const DocNode *n = para->children().at(nodeIndex).get();
    const DocStyleChange *sc = dynamic_cast<const DocStyleChange*>(n);
    if (sc)
    {
      if (!sc->enable()) // remember styles that has been closed already
      {
        styleMask|=static_cast<int>(sc->style());
      }
      bool paraStyle = sc->style()==DocStyleChange::Center ||
                       sc->style()==DocStyleChange::Div    ||
                       sc->style()==DocStyleChange::Preformatted;
      //printf("Found style change %s enabled=%d\n",sc->styleString(),sc->enable());
      if (sc->enable() && (styleMask&static_cast<int>(sc->style()))==0 && // style change that is still active
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
  DocPara *para=0;
  if (n->parent() && (para = dynamic_cast<DocPara*>(n->parent())))
  {
    const DocNodeList &children = para->children();
    auto it = std::find_if(children.begin(),children.end(),[n](const auto &np) { return np.get()==n; });
    if (it==children.end()) return;
    int nodeIndex = static_cast<int>(it - children.begin());
    nodeIndex--;
    if (nodeIndex<0) return; // first node in paragraph
    while (nodeIndex>=0 && isInvisibleNode(children.at(nodeIndex).get()))
    {
      nodeIndex--;
    }
    if (nodeIndex<0) return; // first visible node in paragraph
    n = children.at(nodeIndex).get();
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
  DocPara *para=0;
  if (n->parent() && (para = dynamic_cast<DocPara*>(n->parent()))) // if we are inside a paragraph
  {
    const DocNodeList &children = para->children();
    auto it = std::find_if(children.begin(),children.end(),[n](const auto &np) { return np.get()==n; });
    if (it==children.end()) return;
    int nodeIndex = static_cast<int>(it - children.begin());
    int numNodes  = static_cast<int>(para->children().size());
    bool styleOutsideParagraph=insideStyleChangeThatIsOutsideParagraph(para,nodeIndex);
    if (styleOutsideParagraph) return;
    nodeIndex++;
    if (nodeIndex==numNodes) return; // last node
    while (nodeIndex<numNodes && isInvisibleNode(para->children().at(nodeIndex).get()))
    {
      nodeIndex++;
    }
    if (nodeIndex<numNodes)
    {
      n = para->children().at(nodeIndex).get();
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

    if (needsTag) m_t << "<p>";
  }
}

