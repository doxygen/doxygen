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

#include <fstream>

#include "docbookvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "docbookgen.h"
#include "dot.h"
#include "message.h"
#include "util.h"
#include "parserintf.h"
#include "filename.h"
#include "config.h"
#include "filedef.h"
#include "msc.h"
#include "dia.h"
#include "htmlentity.h"
#include "emoji.h"
#include "plantuml.h"
#include "growbuf.h"
#include "fileinfo.h"

#if 0
#define DB_VIS_C DB_VIS_C1(m_t)
#define DB_VIS_C1(x) x << "<!-- DB_VIS_C " << __LINE__ << " -->\n";
#define DB_VIS_C2(y) DB_VIS_C2a(m_t,y)
#define DB_VIS_C2a(x,y) x << "<!-- DB_VIS_C " << __LINE__ << " " << y << " -->\n";
#else
#define DB_VIS_C
#define DB_VIS_C1(x)
#define DB_VIS_C2(y)
#define DB_VIS_C2a(x,y)
#endif

static QCString filterId(const QCString &s)
{
  if (s.isEmpty()) return s;
  static GrowBuf growBuf;
  growBuf.clear();
  const char *p=s.data();
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case ':':  growBuf.addStr("_1");   break;
      default:   growBuf.addChar(c);       break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

static bool supportedHtmlAttribute(const QCString &name)
{
  return (name=="align" ||
          name=="bgcolor" ||
          name=="border" ||
          name=="cellpadding" ||
          name=="cellspacing" ||
          name=="class" ||
          name=="frame" ||
          name=="label" ||
          name=="style" ||
          name=="width" ||
          name=="tabstyle" ||
          name=="title");
}


void DocbookDocVisitor::visitCaption(const DocNodeList &children)
{
  for (const auto &n : children) n->accept(this);
}

void DocbookDocVisitor::visitPreStart(TextStream &t,
                   const DocNodeList &children,
                   bool hasCaption,
                   const QCString &name,
                   const QCString &width,
                   const QCString &height,
                   bool inlineImage)
{
  if (hasCaption && !inlineImage)
  {
    t << "    <figure>\n";
    t << "        <title>\n";
    visitCaption(children);
    t << "        </title>\n";
  }
  else
  {
    t << "    <informalfigure>\n";
  }
  t << "        <mediaobject>\n";
  t << "            <imageobject>\n";
  t << "                <imagedata";
  if (!width.isEmpty())
  {
    t << " width=\"" << convertToDocBook(width) << "\"";
  }
  else
  {
    if (!height.isEmpty() && !inlineImage) t << " width=\"50%\"";
  }
  if (!height.isEmpty())
  {
    t << " depth=\"" << convertToDocBook(height) << "\"";
  }
  t << " align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << name << "\">";
  t << "</imagedata>\n";
  t << "            </imageobject>\n";
  if (hasCaption && !inlineImage)
  {
    t << "        <!--\n"; // Needed for general formatting with title for other formats
  }
}

void DocbookDocVisitor::visitPostEnd(TextStream &t, bool hasCaption, bool inlineImage)
{
  t << "\n";
  if (hasCaption && !inlineImage)
  {
    t << "        -->\n"; // Needed for general formatting with title for other formats
  }
  t << "        </mediaobject>\n";
  if (hasCaption && !inlineImage)
  {
    t << "    </figure>\n";
  }
  else
  {
    t << "    </informalfigure>\n";
  }
}

DocbookDocVisitor::DocbookDocVisitor(TextStream &t,CodeOutputInterface &ci,const QCString &langExt)
  : DocVisitor(DocVisitor_Docbook), m_t(t), m_ci(ci),m_langExt(langExt)
{
DB_VIS_C
  // m_t << "<section>\n";
}
DocbookDocVisitor::~DocbookDocVisitor()
{
DB_VIS_C
  // m_t << "</section>\n";
}

//--------------------------------------
// visitor functions for leaf nodes
//--------------------------------------

void DocbookDocVisitor::visit(DocWord *w)
{
DB_VIS_C
  if (m_hide) return;
  filter(w->word());
}

void DocbookDocVisitor::visit(DocLinkedWord *w)
{
DB_VIS_C
  if (m_hide) return;
  startLink(w->file(),w->anchor());
  filter(w->word());
  endLink();
}

void DocbookDocVisitor::visit(DocWhiteSpace *w)
{
DB_VIS_C
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

void DocbookDocVisitor::visit(DocSymbol *s)
{
DB_VIS_C
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance()->docbook(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("DocBook: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
}

void DocbookDocVisitor::visit(DocEmoji *s)
{
DB_VIS_C
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

void DocbookDocVisitor::visit(DocURL *u)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<link xlink:href=\"";
  if (u->isEmail()) m_t << "mailto:";
  filter(u->url());
  m_t << "\">";
  filter(u->url());
  m_t << "</link>";
}

void DocbookDocVisitor::visit(DocLineBreak *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n<literallayout>&#160;&#xa;</literallayout>\n";
  // gives nicer results but gives problems as it is not allowed in <pare> and also problems with dblatex
  // m_t << "\n" << "<sbr/>\n";
}

void DocbookDocVisitor::visit(DocHorRuler *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<informaltable frame='bottom'><tgroup cols='1'><colspec align='center'/><tbody><row><entry align='center'>\n";
  m_t << "</entry></row></tbody></tgroup></informaltable>\n";
}

void DocbookDocVisitor::visit(DocStyleChange *s)
{
DB_VIS_C
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "<emphasis role=\"bold\">";      else m_t << "</emphasis>";
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
      if (s->enable()) m_t << "<informaltable frame='none'><tgroup cols='1'><colspec align='center'/><tbody><row><entry align='center'>";
      else m_t << "</entry></row></tbody></tgroup></informaltable>";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        m_t << "<literallayout>";
        m_insidePre=TRUE;
      }
      else
      {
        m_t << "</literallayout>";
        m_insidePre=FALSE;
      }
      break;
      /* There is no equivalent Docbook tag for rendering Small text */
    case DocStyleChange::Small: /* XSLT Stylesheets can be used */ break;
                                                                   /* HTML only */
    case DocStyleChange::Cite:  break;
    case DocStyleChange::S:  break;
    case DocStyleChange::Strike:  break;
    case DocStyleChange::Del:        break;
    case DocStyleChange::Underline:  break;
    case DocStyleChange::Ins:        break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void DocbookDocVisitor::visit(DocVerbatim *s)
{
DB_VIS_C
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
      m_t << "<literallayout><computeroutput>";
      getCodeParser(m_langExt).parseCode(m_ci,s->context(),
                                         s->text(),
                                         langExt,
                                         s->isExample(),
                                         s->exampleFile());
      m_t << "</computeroutput></literallayout>";
      break;
    case DocVerbatim::Verbatim:
      m_t << "<literallayout><computeroutput>";
      filter(s->text());
      m_t << "</computeroutput></literallayout>";
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s->text(), true);
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "<computeroutput>";
      filter(s->text(), true);
      m_t << "</computeroutput>";
      break;
    case DocVerbatim::HtmlOnly:
      break;
    case DocVerbatim::RtfOnly:
      break;
    case DocVerbatim::ManOnly:
      break;
    case DocVerbatim::LatexOnly:
      break;
    case DocVerbatim::XmlOnly:
      break;
    case DocVerbatim::DocbookOnly:
      m_t << s->text();
      break;
    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString baseName(4096);
        QCString name;
        QCString stext = s->text();
        m_t << "<para>\n";
        name.sprintf("%s%d", "dot_inline_dotgraph_", dotindex);
        baseName.sprintf("%s%d",
            qPrint(Config_getString(DOCBOOK_OUTPUT)+"/inline_dotgraph_"),
            dotindex++
            );
        std::string fileName = baseName.str()+".dot";
        std::ofstream file(fileName,std::ofstream::out | std::ofstream::binary);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",fileName.c_str());
        }
        file.write( stext.data(), stext.length() );
        file.close();
        writeDotFile(baseName, s);
        m_t << "</para>\n";
        if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName);
      }
      break;
    case DocVerbatim::Msc:
      {
        static int mscindex = 1;
        QCString baseName(4096);
        QCString name;
        QCString stext = s->text();
        m_t << "<para>\n";
        name.sprintf("%s%d", "msc_inline_mscgraph_", mscindex);
        baseName.sprintf("%s%d",
            (Config_getString(DOCBOOK_OUTPUT)+"/inline_mscgraph_").data(),
            mscindex++
            );
        std::string fileName = baseName.str()+".msc";
        std::ofstream file(fileName,std::ofstream::out | std::ofstream::binary);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",fileName.c_str());
        }
        QCString text = "msc {";
        text+=stext;
        text+="}";
        file.write( text.data(), text.length() );
        file.close();
        writeMscFile(baseName,s);
        m_t << "</para>\n";
        if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName);
      }
      break;
    case DocVerbatim::PlantUML:
      {
        static QCString docbookOutput = Config_getString(DOCBOOK_OUTPUT);
        QCString baseName = PlantumlManager::instance().writePlantUMLSource(docbookOutput,s->exampleFile(),s->text(),PlantumlManager::PUML_BITMAP,s->engine(),s->srcFile(),s->srcLine());
        QCString shortName = baseName;
        int i;
        if ((i=shortName.findRev('/'))!=-1)
        {
          shortName=shortName.right((int)shortName.length()-i-1);
        }
        m_t << "<para>\n";
        writePlantUMLFile(baseName,s);
        m_t << "</para>\n";
      }
      break;
  }
}

void DocbookDocVisitor::visit(DocAnchor *anc)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<anchor xml:id=\"_" <<  stripPath(anc->file()) << "_1" << filterId(anc->anchor()) << "\"/>";
}

void DocbookDocVisitor::visit(DocInclude *inc)
{
DB_VIS_C
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      {
        m_t << "<literallayout><computeroutput>";
        FileInfo cfi( inc->file().str() );
        FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
        getCodeParser(inc->extension()).parseCode(m_ci,inc->context(),
                                                  inc->text(),
                                                  langExt,
                                                  inc->isExample(),
                                                  inc->exampleFile(), fd);
        delete fd;
        m_t << "</computeroutput></literallayout>";
      }
      break;
    case DocInclude::Include:
      m_t << "<literallayout><computeroutput>";
      getCodeParser(inc->extension()).parseCode(m_ci,inc->context(),
                                                inc->text(),
                                                langExt,
                                                inc->isExample(),
                                                inc->exampleFile());
      m_t << "</computeroutput></literallayout>";
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
    case DocInclude::HtmlInclude:
    case DocInclude::LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
      break;
    case DocInclude::DocbookInclude:
      m_t << inc->text();
      break;
    case DocInclude::VerbInclude:
      m_t << "<literallayout>";
      filter(inc->text());
      m_t << "</literallayout>";
      break;
    case DocInclude::Snippet:
      m_t << "<literallayout><computeroutput>";
      getCodeParser(inc->extension()).parseCode(m_ci,
                                                inc->context(),
                                                extractBlock(inc->text(),inc->blockId()),
                                                langExt,
                                                inc->isExample(),
                                                inc->exampleFile()
                                               );
      m_t << "</computeroutput></literallayout>";
      break;
    case DocInclude::SnipWithLines:
      {
         FileInfo cfi( inc->file().str() );
         FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         m_t << "<literallayout><computeroutput>";
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
         m_t << "</computeroutput></literallayout>";
      }
      break;
    case DocInclude::SnippetDoc:
    case DocInclude::IncludeDoc:
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
}

void DocbookDocVisitor::visit(DocIncOperator *op)
{
DB_VIS_C
  if (op->isFirst())
  {
    if (!m_hide)
    {
      m_t << "<programlisting linenumbering=\"unnumbered\">";
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

void DocbookDocVisitor::visit(DocFormula *f)
{
DB_VIS_C
  if (m_hide) return;

  if (f->isInline()) m_t  << "<inlinemediaobject>\n";
  else m_t << "        <mediaobject>\n";
  m_t << "            <imageobject>\n";
  m_t << "                <imagedata ";
  m_t << "align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << f->relPath() << f->name() << ".png\"/>\n";
  m_t << "            </imageobject>\n";
  if (f->isInline()) m_t  << "</inlinemediaobject>\n";
  else m_t << "        </mediaobject>\n";
}

void DocbookDocVisitor::visit(DocIndexEntry *ie)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<indexterm><primary>";
  filter(ie->entry());
  m_t << "</primary></indexterm>\n";
}

void DocbookDocVisitor::visit(DocSimpleSectSep *)
{
DB_VIS_C
  // m_t << "<simplesect/>";
}

void DocbookDocVisitor::visit(DocCite *cite)
{
DB_VIS_C
  if (m_hide) return;
  if (!cite->file().isEmpty()) startLink(cite->file(),filterId(cite->anchor()));
  filter(cite->text());
  if (!cite->file().isEmpty()) endLink();
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void DocbookDocVisitor::visitPre(DocAutoList *l)
{
DB_VIS_C
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

void DocbookDocVisitor::visitPost(DocAutoList *l)
{
DB_VIS_C
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

void DocbookDocVisitor::visitPre(DocAutoListItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocAutoListItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</listitem>";
}

void DocbookDocVisitor::visitPre(DocPara *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  m_t << "<para>";
}

void DocbookDocVisitor::visitPost(DocPara *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</para>";
  m_t << "\n";
}

void DocbookDocVisitor::visitPre(DocRoot *)
{
DB_VIS_C
  //m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
}

void DocbookDocVisitor::visitPost(DocRoot *)
{
DB_VIS_C
  //m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
}

void DocbookDocVisitor::visitPre(DocSimpleSect *s)
{
DB_VIS_C
  if (m_hide) return;
  switch(s->type())
  {
    case DocSimpleSect::See:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trSeeAlso() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSeeAlso()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Return:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trReturns()<< "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trReturns()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Author:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, TRUE) << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, TRUE)) << "</title>\n";
      }
      break;
    case DocSimpleSect::Authors:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, FALSE) << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, FALSE)) << "</title>\n";
      }
      break;
    case DocSimpleSect::Version:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trVersion() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trVersion()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Since:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trSince() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSince()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Date:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trDate() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trDate()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Note:
      if (m_insidePre)
      {
        m_t << "<note><title>" << theTranslator->trNote() << "</title>\n";
      }
      else
      {
        m_t << "<note><title>" << convertToDocBook(theTranslator->trNote()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Warning:
      if (m_insidePre)
      {
        m_t << "<warning><title>" << theTranslator->trWarning() << "</title>\n";
      }
      else
      {
        m_t << "<warning><title>" << convertToDocBook(theTranslator->trWarning()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Pre:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trPrecondition() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPrecondition()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Post:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trPostcondition() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPostcondition()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Copyright:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trCopyright() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trCopyright()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Invar:
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trInvariant() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trInvariant()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Remark:
      // <remark> is miising the <title> possibility
      if (m_insidePre)
      {
        m_t << "<formalpara><title>" << theTranslator->trRemarks() << "</title>\n";
      }
      else
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trRemarks()) << "</title>\n";
      }
      break;
    case DocSimpleSect::Attention:
      if (m_insidePre)
      {
        m_t << "<caution><title>" << theTranslator->trAttention() << "</title>\n";
      }
      else
      {
        m_t << "<caution><title>" << convertToDocBook(theTranslator->trAttention()) << "</title>\n";
      }
      break;
    case DocSimpleSect::User:
    case DocSimpleSect::Rcs:
    case DocSimpleSect::Unknown:
      if (s->hasTitle())
        m_t << "<formalpara>\n";
      else
        m_t << "<para>\n";
      break;
  }
}

void DocbookDocVisitor::visitPost(DocSimpleSect *s)
{
DB_VIS_C
  if (m_hide) return;
  switch(s->type())
  {
    case DocSimpleSect::User:
    case DocSimpleSect::Rcs:
    case DocSimpleSect::Unknown:
      if (s->hasTitle())
        m_t << "</formalpara>\n";
      else
        m_t << "</para>\n";
      break;
    case DocSimpleSect::Note:
      m_t << "</note>\n";
      break;
    case DocSimpleSect::Attention:
      m_t << "</caution>\n";
      break;
    case DocSimpleSect::Warning:
      m_t << "</warning>\n";
      break;
    default:
      m_t << "</formalpara>\n";
      break;
  }
}

void DocbookDocVisitor::visitPre(DocTitle *t)
{
DB_VIS_C
  if (m_hide) return;
  if (t->hasTitle()) m_t << "<title>";
}

void DocbookDocVisitor::visitPost(DocTitle *t)
{
DB_VIS_C
  if (m_hide) return;
  if (t->hasTitle()) m_t << "</title>";
}

void DocbookDocVisitor::visitPre(DocSimpleList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
}

void DocbookDocVisitor::visitPost(DocSimpleList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::visitPre(DocSimpleListItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocSimpleListItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::visitPre(DocSection *s)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<section xml:id=\"_" <<  stripPath(s->file());
  if (!s->anchor().isEmpty()) m_t << "_1" << s->anchor();
  m_t << "\">\n";
  m_t << "<title>";
  filter(s->title());
  m_t << "</title>\n";
}

void DocbookDocVisitor::visitPost(DocSection *)
{
DB_VIS_C
  m_t << "</section>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlList *s)
{
DB_VIS_C
  if (m_hide) return;
  // This will be handled in DocHtmlListItem
}

void DocbookDocVisitor::visitPost(DocHtmlList *s)
{
DB_VIS_C
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered)
    m_t << "</orderedlist>\n";
  else
    m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlListItem *s)
{
DB_VIS_C
  if (m_hide) return;
  DocHtmlList *l = (DocHtmlList *)s->parent();
  if (l->type()==DocHtmlList::Ordered)
  {
    bool isFirst = l->children().front().get()==s;
    int value = 0;
    QCString type;
    for (const auto &opt : s->attribs())
    {
      if (opt.name=="value")
      {
        bool ok;
        int val = opt.value.toInt(&ok);
        if (ok) value = val;
      }
    }

    if (value>0 || isFirst)
    {
      for (const auto &opt : l->attribs())
      {
        if (opt.name=="type")
        {
          if (opt.value=="1")
            type = " numeration=\"arabic\"";
          else if (opt.value=="a")
            type = " numeration=\"loweralpha\"";
            else if (opt.value=="A")
            type =  " numeration=\"upperalpha\"";
          else if (opt.value=="i")
            type =  " numeration=\"lowerroman\"";
          else if (opt.value=="I")
            type =  " numeration=\"upperroman\"";
        }
        else if (value==0 && opt.name=="start")
        {
          bool ok;
          int val = opt.value.toInt(&ok);
          if (ok) value = val;
        }
      }
    }

    if (value>0 && !isFirst)
    {
      m_t << "</orderedlist>\n";
    }
    if (value>0 || isFirst)
    {
      m_t << "<orderedlist";
      if (!type.isEmpty()) m_t << type.data();
      if (value>0)         m_t << " startingnumber=\"" << value << "\"";
      m_t << ">\n";
    }
  }
  else
  {
    m_t << "<itemizedlist>\n";
  }
  m_t << "<listitem>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlListItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<variablelist>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlDescList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</variablelist>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescTitle *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<varlistentry><term>";
}

void DocbookDocVisitor::visitPost(DocHtmlDescTitle *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</term>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescData *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocHtmlDescData *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</listitem></varlistentry>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlTable *t)
{
DB_VIS_C
  m_bodySet.push(false);
  if (m_hide) return;
  m_t << "<informaltable frame=\"all\">\n";
  m_t << "    <tgroup cols=\"" << (unsigned int)t->numColumns() << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (uint i = 0; i <t->numColumns(); i++)
  {
    // do something with colwidth based of cell width specification (be aware of possible colspan in the header)?
    m_t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
  if (t->hasCaption())
  {
    DocHtmlCaption *c = t->caption();
    m_t << "<caption>";
    if (!c->file().isEmpty())
    {
      m_t << "<anchor xml:id=\"_" <<  stripPath(c->file()) << "_1" << filterId(c->anchor()) << "\"/>";
    }
  }
}

void DocbookDocVisitor::visitPost(DocHtmlTable *)
{
DB_VIS_C
  if (m_hide) return;
  if (m_bodySet.top()) m_t << "    </tbody>\n";
  m_bodySet.pop();
  m_t << "    </tgroup>\n";
  m_t << "</informaltable>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlRow *tr)
{
DB_VIS_C
  m_colCnt = 0;
  if (m_hide) return;

  if (tr->isHeading())
  {
    if (m_bodySet.top()) m_t << "</tbody>\n";
    m_bodySet.top() = false;
    m_t << "<thead>\n";
  }
  else if (!m_bodySet.top())
  {
    m_bodySet.top() = true;
    m_t << "<tbody>\n";
  }

  m_t << "      <row ";

  for (const auto &opt : tr->attribs())
  {
    if (supportedHtmlAttribute(opt.name))
    {
      // process supported attributes only
      m_t << " " << opt.name << "='" << convertToDocBook(opt.value) << "'";
    }
  }
  m_t << ">\n";
}

void DocbookDocVisitor::visitPost(DocHtmlRow *tr)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</row>\n";
  if (tr->isHeading())
  {
    m_t << "</thead><tbody>\n";
    m_bodySet.top() = true;
  }
}

void DocbookDocVisitor::visitPre(DocHtmlCell *c)
{
DB_VIS_C
  m_colCnt++;
  if (m_hide) return;
  m_t << "<entry";

  for (const auto &opt : c->attribs())
  {
    if (opt.name=="colspan")
    {
      m_t << " namest='c" << m_colCnt << "'";
      int cols = opt.value.toInt();
      m_colCnt += (cols - 1);
      m_t << " nameend='c" << m_colCnt << "'";
    }
    else if (opt.name=="rowspan")
    {
      int extraRows = opt.value.toInt() - 1;
      m_t << " morerows='" << extraRows << "'";
    }
    else if (opt.name=="class")
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
      else
      {
        m_t << " class='" << convertToDocBook(opt.value) << "'";
      }
    }
    else if (supportedHtmlAttribute(opt.name))
    {
      // process supported attributes only
      m_t << " " << opt.name << "='" << convertToDocBook(opt.value) << "'";
    }
  }
  m_t << ">";
}

void DocbookDocVisitor::visitPost(DocHtmlCell *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</entry>";
}

void DocbookDocVisitor::visitPre(DocHtmlCaption *)
{
DB_VIS_C
  if (m_hide) return;
  // start of caption is handled in the DocbookDocVisitor::visitPre(DocHtmlTable *t)
}

void DocbookDocVisitor::visitPost(DocHtmlCaption *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</caption>\n";
}

void DocbookDocVisitor::visitPre(DocInternal *)
{
DB_VIS_C
  if (m_hide) return;
  // TODO: to be implemented
}

void DocbookDocVisitor::visitPost(DocInternal *)
{
DB_VIS_C
  if (m_hide) return;
  // TODO: to be implemented
}

void DocbookDocVisitor::visitPre(DocHRef *href)
{
DB_VIS_C
  if (m_hide) return;
  if (href->url().at(0) != '#')
  {
    m_t << "<link xlink:href=\"" << convertToDocBook(href->url()) << "\">";
  }
  else
  {
    startLink(href->file(),filterId(href->url().mid(1)));
  }
}

void DocbookDocVisitor::visitPost(DocHRef *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</link>";
}

void DocbookDocVisitor::visitPre(DocHtmlHeader *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<formalpara><title>";
}

void DocbookDocVisitor::visitPost(DocHtmlHeader *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</title></formalpara>\n";
}

void DocbookDocVisitor::visitPre(DocImage *img)
{
DB_VIS_C
  if (img->type()==DocImage::DocBook)
  {
    if (m_hide) return;
    m_t << "\n";
    QCString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right((int)baseName.length()-i-1);
    }
    visitPreStart(m_t, img->children(), img->hasCaption(), img->relPath() + baseName, img->width(), img->height(), img->isInlineImage());
  }
  else
  {
    pushHidden(m_hide);
    m_hide=TRUE;
  }
}

void DocbookDocVisitor::visitPost(DocImage *img)
{
DB_VIS_C
  if (img->type()==DocImage::DocBook)
  {
    if (m_hide) return;
    visitPostEnd(m_t, img -> hasCaption(),img -> isInlineImage());
    // copy the image to the output dir
    QCString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right((int)baseName.length()-i-1);
    }
    QCString m_file;
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::imageNameLinkedMap, baseName, ambig);
    if (fd)
    {
      m_file=fd->absFilePath();
    }
    copyFile(m_file,Config_getString(DOCBOOK_OUTPUT)+"/"+baseName);
  }
  else
  {
    m_hide = popHidden();
  }
}

void DocbookDocVisitor::visitPre(DocDotFile *df)
{
DB_VIS_C
  if (m_hide) return;
  startDotFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children(),df->srcFile(),df->srcLine());
}

void DocbookDocVisitor::visitPost(DocDotFile *df)
{
DB_VIS_C
  if (m_hide) return;
  endDotFile(df->hasCaption());
}

void DocbookDocVisitor::visitPre(DocMscFile *df)
{
DB_VIS_C
  if (m_hide) return;
  startMscFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children(),df->srcFile(),df->srcLine());
}

void DocbookDocVisitor::visitPost(DocMscFile *df)
{
DB_VIS_C
  if (m_hide) return;
  endMscFile(df->hasCaption());
}
void DocbookDocVisitor::visitPre(DocDiaFile *df)
{
DB_VIS_C
  if (m_hide) return;
  startDiaFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children(),df->srcFile(),df->srcLine());
}

void DocbookDocVisitor::visitPost(DocDiaFile *df)
{
DB_VIS_C
  if (m_hide) return;
  endDiaFile(df->hasCaption());
}

void DocbookDocVisitor::visitPre(DocLink *lnk)
{
DB_VIS_C
  if (m_hide) return;
  startLink(lnk->file(),lnk->anchor());
}

void DocbookDocVisitor::visitPost(DocLink *)
{
DB_VIS_C
  if (m_hide) return;
  endLink();
}

void DocbookDocVisitor::visitPre(DocRef *ref)
{
DB_VIS_C
  if (m_hide) return;
  if (ref->isSubPage())
  {
    startLink(QCString(),ref->anchor());
  }
  else
  {
    if (!ref->file().isEmpty()) startLink(ref->file(),ref->anchor());
  }

  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void DocbookDocVisitor::visitPost(DocRef *ref)
{
DB_VIS_C
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink();
}

void DocbookDocVisitor::visitPre(DocSecRefItem *)
{
DB_VIS_C
  if (m_hide) return;
  //m_t << "<tocentry xml:idref=\"_" <<  stripPath(ref->file()) << "_1" << ref->anchor() << "\">";
  m_t << "<tocentry>";
}

void DocbookDocVisitor::visitPost(DocSecRefItem *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</tocentry>\n";
}

void DocbookDocVisitor::visitPre(DocSecRefList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<toc>\n";
}

void DocbookDocVisitor::visitPost(DocSecRefList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</toc>\n";
}

void DocbookDocVisitor::visitPre(DocParamSect *s)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  m_t << "                <formalpara>\n";
  m_t << "                    <title>\n";
  switch(s->type())
  {
    case DocParamSect::Param:         m_t << theTranslator->trParameters();         break;
    case DocParamSect::RetVal:        m_t << theTranslator->trReturnValues();       break;
    case DocParamSect::Exception:     m_t << theTranslator->trExceptions();         break;
    case DocParamSect::TemplateParam: m_t << theTranslator->trTemplateParameters(); break;
    default:
      ASSERT(0);
  }
  m_t << "</title>\n";
  m_t << "                    <para>\n";
  m_t << "                    <table frame=\"all\">\n";
  int ncols = 2;
  if (s->type() == DocParamSect::Param)
  {
    bool hasInOutSpecs = s->hasInOutSpecifier();
    bool hasTypeSpecs  = s->hasTypeSpecifier();
    if      (hasInOutSpecs && hasTypeSpecs) ncols += 2;
    else if (hasInOutSpecs || hasTypeSpecs) ncols += 1;
  }
  m_t << "                        <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (int i = 1; i <= ncols; i++)
  {
    if (i == ncols) m_t << "                        <colspec colwidth=\"4*\"/>\n";
    else            m_t << "                        <colspec colwidth=\"1*\"/>\n";
  }
  m_t << "                        <tbody>\n";
}

void DocbookDocVisitor::visitPost(DocParamSect *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "                        </tbody>\n";
  m_t << "                        </tgroup>\n";
  m_t << "                    </table>\n";
  m_t << "                    </para>\n";
  m_t << "                </formalpara>\n";
  m_t << "                ";
}

void DocbookDocVisitor::visitPre(DocParamList *pl)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "                            <row>\n";

  DocParamSect *sect = 0;
  if (pl->parent() && pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    sect=(DocParamSect*)pl->parent();
  }

  if (sect && sect->hasInOutSpecifier())
  {
    m_t << "<entry>";
    if (pl->direction()!=DocParamSect::Unspecified)
    {
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
    }
    m_t << "</entry>";
  }

  if (sect && sect->hasTypeSpecifier())
  {
    m_t << "<entry>";
    for (const auto &type : pl->paramTypes())
    {
      if (type->kind()==DocNode::Kind_Word)
      {
        visit((DocWord*)type.get());
      }
      else if (type->kind()==DocNode::Kind_LinkedWord)
      {
        visit((DocLinkedWord*)type.get());
      }
      else if (type->kind()==DocNode::Kind_Sep)
      {
        m_t << " " << ((DocSeparator *)type.get())->chars() << " ";
      }

    }
    m_t << "</entry>";
  }

  if (pl->parameters().empty())
  {
    m_t << "<entry></entry>\n";
  }
  else
  {
    m_t << "<entry>";
    int cnt = 0;
    for (const auto &param : pl->parameters())
    {
      if (cnt)
      {
        m_t << ", ";
      }
      if (param->kind()==DocNode::Kind_Word)
      {
        visit((DocWord*)param.get());
      }
      else if (param->kind()==DocNode::Kind_LinkedWord)
      {
        visit((DocLinkedWord*)param.get());
      }
      cnt++;
    }
    m_t << "</entry>";
  }
  m_t << "<entry>";
}

void DocbookDocVisitor::visitPost(DocParamList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</entry>\n";
  m_t << "                            </row>\n";
}

void DocbookDocVisitor::visitPre(DocXRefItem *x)
{
DB_VIS_C
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "<para><link linkend=\"_";
  m_t << stripPath(x->file()) << "_1" << x->anchor();
  m_t << "\">";
  filter(x->title());
  m_t << "</link>";
  m_t << " ";
}

void DocbookDocVisitor::visitPost(DocXRefItem *x)
{
DB_VIS_C
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "</para>";
}

void DocbookDocVisitor::visitPre(DocInternalRef *ref)
{
DB_VIS_C
  if (m_hide) return;
  startLink(ref->file(),ref->anchor());
}

void DocbookDocVisitor::visitPost(DocInternalRef *)
{
DB_VIS_C
  if (m_hide) return;
  endLink();
  m_t << " ";
}

void DocbookDocVisitor::visitPre(DocText *)
{
DB_VIS_C
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPost(DocText *)
{
DB_VIS_C
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPre(DocHtmlBlockQuote *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<blockquote>";
}

void DocbookDocVisitor::visitPost(DocHtmlBlockQuote *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</blockquote>";
}

void DocbookDocVisitor::visitPre(DocVhdlFlow *)
{
DB_VIS_C
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPost(DocVhdlFlow *)
{
DB_VIS_C
  // TODO: to be implemented
}

void DocbookDocVisitor::visitPre(DocParBlock *)
{
DB_VIS_C
}

void DocbookDocVisitor::visitPost(DocParBlock *)
{
DB_VIS_C
}


void DocbookDocVisitor::filter(const QCString &str, const bool retainNewLine)
{
DB_VIS_C
  m_t << convertToDocBook(str, retainNewLine);
}

void DocbookDocVisitor::startLink(const QCString &file,const QCString &anchor)
{
DB_VIS_C
  m_t << "<link linkend=\"_" << stripPath(file);
  if (!anchor.isEmpty())
  {
    if (!file.isEmpty()) m_t << "_1";
    m_t << anchor;
  }
  m_t << "\">";
}

void DocbookDocVisitor::endLink()
{
DB_VIS_C
  m_t << "</link>";
}

void DocbookDocVisitor::writeMscFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right((int)shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MSC_BITMAP,s->srcFile(),s->srcLine());
  visitPreStart(m_t, s->children(), s->hasCaption(), s->relPath() + shortName + ".png", s->width(), s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::writePlantUMLFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right((int)shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  PlantumlManager::instance().generatePlantUMLOutput(baseName,outDir,PlantumlManager::PUML_BITMAP);
  visitPreStart(m_t, s->children(), s->hasCaption(), s->relPath() + shortName + ".png", s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startMscFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right((int)baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("msc_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endMscFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

void DocbookDocVisitor::writeDiaFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right((int)shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDiaGraphFromFile(baseName+".dia",outDir,shortName,DIA_BITMAP,s->srcFile(),s->srcLine());
  visitPreStart(m_t, s->children(), s->hasCaption(), shortName, s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startDiaFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right((int)baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dia_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDiaGraphFromFile(fileName,outDir,baseName,DIA_BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endDiaFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

void DocbookDocVisitor::writeDotFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right((int)shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDotGraphFromFile(baseName+".dot",outDir,shortName,GOF_BITMAP,s->srcFile(),s->srcLine());
  visitPreStart(m_t, s->children(), s->hasCaption(), s->relPath() + shortName + "." + getDotImageExtension(), s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startDotFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const DocNodeList &children,
    const QCString &srcFile,
    int srcLine
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right((int)baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dot_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  QCString imgExt = getDotImageExtension();
  writeDotGraphFromFile(fileName,outDir,baseName,GOF_BITMAP,srcFile,srcLine);
  m_t << "<para>\n";
  visitPreStart(m_t, children, hasCaption, baseName + "." + imgExt,  width,  height);
}

void DocbookDocVisitor::endDotFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "\n";
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>\n";
}

