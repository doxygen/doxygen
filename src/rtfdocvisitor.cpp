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

#include <algorithm>

#include "rtfdocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "dot.h"
#include "msc.h"
#include "util.h"
#include "rtfstyle.h"
#include "message.h"
#include "parserintf.h"
#include "msc.h"
#include "dia.h"
#include "filedef.h"
#include "config.h"
#include "htmlentity.h"
#include "emoji.h"
#include "plantuml.h"
#include "fileinfo.h"

//#define DBG_RTF(x) m_t << x
#define DBG_RTF(x) do {} while(0)

static QCString align(DocHtmlCell *cell)
{
  for (const auto &attr : cell->attribs())
  {
    if (attr.name.lower()=="align")
    {
      if (attr.value.lower()=="center")     return "\\qc ";
      else if (attr.value.lower()=="right") return "\\qr ";
      else return "";
    }
  }
  return "";
}

RTFDocVisitor::RTFDocVisitor(TextStream &t,CodeOutputInterface &ci,
                             const QCString &langExt)
  : DocVisitor(DocVisitor_RTF), m_t(t), m_ci(ci), m_langExt(langExt)
{
}

QCString RTFDocVisitor::getStyle(const QCString &name)
{
  QCString n = name + QCString().setNum(indentLevel());
  StyleData &sd = rtf_Style[n.str()];
  return sd.reference();
}

int RTFDocVisitor::indentLevel() const
{
  return std::min(m_indentLevel,maxIndentLevels-1);
}

void RTFDocVisitor::incIndentLevel()
{
  m_indentLevel++;
  if (m_indentLevel>=maxIndentLevels)
  {
    err("Maximum indent level (%d) exceeded while generating RTF output!\n",maxIndentLevels-1);
  }
}

void RTFDocVisitor::decIndentLevel()
{
  if (m_indentLevel>0) m_indentLevel--;
}

  //------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void RTFDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocWord)}\n");
  filter(w->word());
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocLinkedWord)}\n");
  startLink(w->ref(),w->file(),w->anchor());
  filter(w->word());
  endLink(w->ref());
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocWhiteSpace *w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocWhiteSpace)}\n");
  if (m_insidePre)
  {
    m_t << w->chars();
  }
  else
  {
    m_t << " ";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocSymbol)}\n");
  const char *res = HtmlEntityMapper::instance()->rtf(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("RTF: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocEmoji *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocEmoji)}\n");
  const char *res = EmojiEntityMapper::instance()->unicode(s->index());
  if (res)
  {
    const char *p = res;
    int val = 0;
    int val1 = 0;
    while (*p)
    {
      switch(*p)
      {
        case '&': case '#': case 'x':
          break;
        case ';':
	  val1 = val;
	  val = 0xd800 + ( ( val1 - 0x10000 ) & 0xffc00 ) / 0x400 - 0x10000;
          m_t << "\\u" << val << "?";
          val = 0xdC00 + ( ( val1 - 0x10000 ) & 0x3ff ) - 0x10000 ;
          m_t << "\\u" << val << "?";
          val = 0;
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          val = val * 16 + *p - '0';
          break;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          val = val * 16 + *p - 'a' + 10;
          break;
      }
      p++;
    }
  }
  else
  {
    m_t << s->name();
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocURL)}\n");
  if (Config_getBool(RTF_HYPERLINKS))
  {
    m_t << "{\\field "
             "{\\*\\fldinst "
               "{ HYPERLINK \"";
    if (u->isEmail()) m_t << "mailto:";
    m_t << u->url();
    m_t <<  "\" }"
               "{}";
    m_t <<   "}"
             "{\\fldrslt "
               "{\\cs37\\ul\\cf2 ";
    filter(u->url());
    m_t <<     "}"
             "}"
           "}\n";
  }
  else
  {
    m_t << "{\\f2 ";
    filter(u->url());
    m_t << "}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocLineBreak)}\n");
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocHorRuler)}\n");
  m_t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw5\\brsp20 \\adjustright \\par}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  m_lastIsPara=FALSE;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocStyleChange)}\n");
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "{\\b ";      else m_t << "} ";
      break;
    case DocStyleChange::S:
    case DocStyleChange::Strike:
    case DocStyleChange::Del:
      if (s->enable()) m_t << "{\\strike ";      else m_t << "} ";
      break;
    case DocStyleChange::Underline:
    case DocStyleChange::Ins:
      if (s->enable()) m_t << "{\\ul ";      else m_t << "} ";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "{\\i ";     else m_t << "} ";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "{\\f2 ";   else m_t << "} ";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "{\\sub ";    else m_t << "} ";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "{\\super ";    else m_t << "} ";
      break;
    case DocStyleChange::Center:
      if (s->enable()) m_t << "{\\qc "; else m_t << "} ";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "{\\sub ";  else m_t << "} ";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable())
      {
        m_t << "{\n";
        m_t << "\\par\n";
        m_t << rtf_Style_Reset << getStyle("CodeExample");
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "\\par";
        m_t << "}\n";
      }
      m_lastIsPara=TRUE;
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
    case DocStyleChange::Details: /* emulation of the <details> tag */
      if (s->enable())
      {
        m_t << "{\n";
        m_t << "\\par\n";
      }
      else
      {
        m_t << "\\par";
        m_t << "}\n";
      }
      m_lastIsPara=TRUE;
      break;
    case DocStyleChange::Summary: /* emulation of the <summary> tag inside a <details> tag */
      if (s->enable()) m_t << "{\\b ";      else m_t << "} ";
      break;
  }
}

static void visitCaption(RTFDocVisitor *parent, const DocNodeList &children)
{
  for (const auto &n : children) n->accept(parent);
}

void RTFDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocVerbatim)}\n");
  QCString lang = m_langExt;
  if (!s->language().isEmpty()) // explicit language setting
  {
    lang = s->language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch(s->type())
  {
    case DocVerbatim::Code:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      getCodeParser(lang).parseCode(m_ci,s->context(),s->text(),langExt,
                                    s->isExample(),s->exampleFile());
      //m_t << "\\par\n";
      m_t << "}\n";
      break;
    case DocVerbatim::Verbatim:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      filter(s->text(),TRUE);
      //m_t << "\\par\n";
      m_t << "}\n";
      break;
    case DocVerbatim::RtfOnly:
      m_t << s->text();
      break;
    case DocVerbatim::HtmlOnly:
    case DocVerbatim::LatexOnly:
    case DocVerbatim::XmlOnly:
    case DocVerbatim::ManOnly:
    case DocVerbatim::DocbookOnly:
      /* nothing */
      break;
    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString fileName(4096);

        fileName.sprintf("%s%d%s",
            qPrint(Config_getString(RTF_OUTPUT)+"/inline_dotgraph_"),
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
        }

        writeDotFile(fileName, s->hasCaption(), s->srcFile(), s->srcLine());
        visitCaption(this, s->children());
        includePicturePostRTF(true, s->hasCaption());

        if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName.str());
      }
      break;
    case DocVerbatim::Msc:
      {
        static int mscindex = 1;
        QCString baseName(4096);

        baseName.sprintf("%s%d%s",
            qPrint(Config_getString(RTF_OUTPUT)+"/inline_mscgraph_"),
            mscindex++,
            ".msc"
           );
        std::ofstream file(baseName.str(),std::ofstream::out | std::ofstream::binary);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(baseName));
        }
        QCString text = "msc {";
        text+=s->text();
        text+="}";
        file.write( text.data(), text.length() );
        file.close();

        writeMscFile(baseName, s->hasCaption(), s->srcFile(), s->srcLine());
        visitCaption(this, s->children());
        includePicturePostRTF(true, s->hasCaption());

        if (Config_getBool(DOT_CLEANUP)) Dir().remove(baseName.str());
      }
      break;
    case DocVerbatim::PlantUML:
      {
        static QCString rtfOutput = Config_getString(RTF_OUTPUT);
        QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                       rtfOutput,s->exampleFile(),s->text(),PlantumlManager::PUML_BITMAP,
                       s->engine(),s->srcFile(),s->srcLine());

        writePlantUMLFile(baseName, s->hasCaption());
        visitCaption(this, s->children());
        includePicturePostRTF(true, s->hasCaption());
      }
      break;
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocAnchor)}\n");
  QCString anchor;
  if (!anc->file().isEmpty())
  {
    anchor+=stripPath(anc->file());
  }
  if (!anc->file().isEmpty() && !anc->anchor().isEmpty())
  {
    anchor+="_";
  }
  if (!anc->anchor().isEmpty())
  {
    anchor+=anc->anchor();
  }
  m_t << "{\\bkmkstart " << rtfFormatBmkStr(anchor) << "}\n";
  m_t << "{\\bkmkend " << rtfFormatBmkStr(anchor) << "}\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocInclude)}\n");
  switch(inc->type())
  {
     case DocInclude::IncWithLines:
      {
         m_t << "{\n";
         m_t << "\\par\n";
         m_t << rtf_Style_Reset << getStyle("CodeExample");
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
         m_t << "\\par";
         m_t << "}\n";
      }
      break;
    case DocInclude::Include:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      getCodeParser(inc->extension()).parseCode(m_ci,inc->context(),
                                        inc->text(),langExt,inc->isExample(),
                                        inc->exampleFile(),
                                        0,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        TRUE,  // inlineFragment
                                        0,     // memberDef
                                        FALSE  // show line numbers
				       );
      m_t << "\\par";
      m_t << "}\n";
      break;
    case DocInclude::DontInclude:
    case DocInclude::DontIncWithLines:
    case DocInclude::HtmlInclude:
    case DocInclude::LatexInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      break;
    case DocInclude::RtfInclude:
      m_t << inc->text();
      break;
    case DocInclude::VerbInclude:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      filter(inc->text());
      m_t << "\\par";
      m_t << "}\n";
      break;
    case DocInclude::Snippet:
      m_t << "{\n";
      if (!m_lastIsPara) m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      getCodeParser(inc->extension()).parseCode(m_ci,
                                        inc->context(),
                                        extractBlock(inc->text(),inc->blockId()),
                                        langExt,
                                        inc->isExample(),
                                        inc->exampleFile()
                                       );
      m_t << "}";
      break;
    case DocInclude::SnipWithLines:
      {
         FileInfo cfi( inc->file().str() );
         FileDef *fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         m_t << "{\n";
         if (!m_lastIsPara) m_t << "\\par\n";
         m_t << rtf_Style_Reset << getStyle("CodeExample");
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
         m_t << "}";
      }
      break;
    case DocInclude::SnippetDoc:
    case DocInclude::IncludeDoc:
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),qPrint(op->text()));
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocIncOperator)}\n");
  QCString locLangExt = getFileNameExtension(op->includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  if (op->isFirst())
  {
    if (!m_hide)
    {
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
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
      m_t << "\\par";
      m_t << "}\n";
    }
    m_lastIsPara=TRUE;
  }
  else
  {
    if (!m_hide) m_t << "\n";
    m_lastIsPara=FALSE;
  }
}

void RTFDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocFormula)}\n");
  bool bDisplay = !f->isInline();
  if (bDisplay)
  {
    m_t << "\\par";
    m_t << "{";
    m_t << "\\pard\\plain";
    m_t << "\\pard";
    m_t << "\\qc";
  }
  m_t << "{ \\field\\flddirty {\\*\\fldinst  INCLUDEPICTURE \"" << f->relPath() << f->name() << ".png\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt Image}}";
  if (bDisplay)
  {
    m_t << "\\par}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocIndexEntry *i)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocIndexEntry)}\n");
  m_t << "{\\xe \\v " << i->entry() << "}\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visit(DocSimpleSectSep *)
{
}

void RTFDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocCite)}\n");
  if (!cite->file().isEmpty())
  {
    startLink(cite->ref(),cite->file(),cite->anchor());
  }
  else
  {
    m_t << "{\\b ";
  }
  filter(cite->text());
  if (!cite->file().isEmpty())
  {
    endLink(cite->ref());
  }
  else
  {
    m_t << "}";
  }
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void RTFDocVisitor::visitPre(DocAutoList *l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocAutoList)}\n");
  m_t << "{\n";
  int level = indentLevel();
  m_listItemInfo[level].isEnum = l->isEnumList();
  m_listItemInfo[level].type   = '1';
  m_listItemInfo[level].number = 1;
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocAutoList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocAutoList)}\n");
  if (!m_lastIsPara) m_t << "\\par";
  m_t << "}\n";
  m_lastIsPara=TRUE;
  if (indentLevel()==0) m_t << "\\par\n";
}

void RTFDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocAutoListItem)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << rtf_Style_Reset;
  int level = indentLevel();
  if (m_listItemInfo[level].isEnum)
  {
    m_t << getStyle("ListEnum") << "\n";
    m_t << m_listItemInfo[level].number << ".\\tab ";
    m_listItemInfo[level].number++;
  }
  else
  {
    m_t << getStyle("ListBullet") << "\n";
  }
  incIndentLevel();
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocAutoListItem *)
{
  decIndentLevel();
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocAutoListItem)}\n");
}

void RTFDocVisitor::visitPre(DocPara *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocPara)}\n");
}

void RTFDocVisitor::visitPost(DocPara *p)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocPara)}\n");
  if (!m_lastIsPara &&
      !p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameters & sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       )
     )
  {
    m_t << "\\par\n";
    m_lastIsPara=TRUE;
  }
}

void RTFDocVisitor::visitPre(DocRoot *r)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocRoot)}\n");
  if (r->indent()) incIndentLevel();
  m_t << "{" << rtf_Style["BodyText"].reference() << "\n";
}

void RTFDocVisitor::visitPost(DocRoot *r)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocRoot)}\n");
  if (!m_lastIsPara && !r->singleLine()) m_t << "\\par\n";
  m_t << "}";
  m_lastIsPara=TRUE;
  if (r->indent()) decIndentLevel();
}

void RTFDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSimpleSect)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{"; // start desc
  //m_t << "{\\b "; // start bold
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
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
    m_t << "\\par";
    m_t << "}"; // end bold
    incIndentLevel();
    m_t << rtf_Style_Reset << getStyle("DescContinue");
    m_t << "{\\s17 \\sa60 \\sb30\n";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocSimpleSect *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSimpleSect)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  decIndentLevel();
  if (s->type()!=DocSimpleSect::User && s->type()!=DocSimpleSect::Rcs) m_t << "}";
  m_t << "}"; // end desc
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocTitle *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocTitle)}\n");
}

void RTFDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocTitle)}\n");
  m_t << "\\par\n";
  m_t << "}"; // end bold
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSimpleSect)}\n");
  m_t << "{\n";
  m_listItemInfo[indentLevel()].isEnum = FALSE;
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSimpleSect)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSimpleListItem)}\n");
  m_t << "\\par" << rtf_Style_Reset << getStyle("ListBullet") << "\n";
  m_lastIsPara=FALSE;
  incIndentLevel();
}

void RTFDocVisitor::visitPost(DocSimpleListItem *)
{
  decIndentLevel();
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSimpleListItem)}\n");
}

void RTFDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSection)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{\\bkmkstart " << rtfFormatBmkStr(stripPath(s->file())+"_"+s->anchor()) << "}\n";
  m_t << "{\\bkmkend " << rtfFormatBmkStr(stripPath(s->file())+"_"+s->anchor()) << "}\n";
  m_t << "{{" // start section
      << rtf_Style_Reset;
  QCString heading;
  int level = std::min(s->level()+1,4);
  heading.sprintf("Heading%d",level);
  // set style
  m_t << rtf_Style[heading.str()].reference() << "\n";
  // make table of contents entry
  filter(s->title());
  m_t << "\n\\par" << "}\n";
  m_t << "{\\tc\\tcl" << level << " \\v ";
  filter(s->title());
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPost(DocSection *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSection)}\n");
  m_t << "\\par}\n"; // end section
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlList *l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlList)}\n");
  m_t << "{\n";
  int level = indentLevel();
  m_listItemInfo[level].isEnum = l->type()==DocHtmlList::Ordered;
  m_listItemInfo[level].number = 1;
  m_listItemInfo[level].type   = '1';
  for (const auto &opt : l->attribs())
  {
    if (opt.name=="type")
    {
      m_listItemInfo[level].type = opt.value[0];
    }
    if (opt.name=="start")
    {
      bool ok;
      int val = opt.value.toInt(&ok);
      if (ok) m_listItemInfo[level].number = val;
    }
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlList)}\n");
  m_t << "\\par" << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlListItem *l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlListItem)}\n");
  m_t << "\\par\n";
  m_t << rtf_Style_Reset;
  int level = indentLevel();
  if (m_listItemInfo[level].isEnum)
  {
    for (const auto &opt : l->attribs())
    {
      if (opt.name=="value")
      {
        bool ok;
        int val = opt.value.toInt(&ok);
        if (ok) m_listItemInfo[level].number = val;
      }
    }
    m_t << getStyle("ListEnum") << "\n";
    switch (m_listItemInfo[level].type)
    {
      case '1':
        m_t << m_listItemInfo[level].number;
        break;
      case 'a':
        m_t << integerToAlpha(m_listItemInfo[level].number,false);
        break;
      case 'A':
        m_t << integerToAlpha(m_listItemInfo[level].number);
        break;
      case 'i':
        m_t << integerToRoman(m_listItemInfo[level].number,false);
        break;
      case 'I':
        m_t << integerToRoman(m_listItemInfo[level].number);
        break;
      default:
        m_t << m_listItemInfo[level].number;
        break;
    }
    m_t << ".\\tab ";
    m_listItemInfo[level].number++;
  }
  else
  {
    m_t << getStyle("ListBullet") << "\n";
  }
  incIndentLevel();
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlListItem *)
{
  decIndentLevel();
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlListItem)}\n");
}

void RTFDocVisitor::visitPre(DocHtmlDescList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlDescList)}\n");
  //m_t << "{\n";
  //m_t << rtf_Style_Reset << getStyle("ListContinue");
  //m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlDescList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlDescList)}\n");
  //m_t << "}\n";
  //m_t << "\\par\n";
  //m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlDescTitle)}\n");
  //m_t << "\\par\n";
  //m_t << "{\\b ";
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlDescTitle *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlDescTitle)}\n");
  m_t << "\\par\n";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlDescData *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlDescData)}\n");
  incIndentLevel();
  m_t << "{" << rtf_Style_Reset << getStyle("DescContinue");
}

void RTFDocVisitor::visitPost(DocHtmlDescData *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlDescData)}\n");
  m_t << "\\par";
  m_t << "}\n";
  decIndentLevel();
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlTable *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlTable)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPost(DocHtmlTable *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlTable)}\n");
  m_t << "\\pard\\plain\n";
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHtmlCaption *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlCaption)}\n");
  m_t << "\\pard \\qc \\b";
  m_t << "{Table \\field\\flddirty{\\*\\fldinst { SEQ Table \\\\*Arabic }}{\\fldrslt {\\noproof 1}} ";
}

void RTFDocVisitor::visitPost(DocHtmlCaption *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlCaption)}\n");
  m_t << "}\n\\par\n";
}

void RTFDocVisitor::visitPre(DocHtmlRow *r)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlRow)}\n");
  uint i,columnWidth=(uint)r->numCells()>0 ? rtf_pageWidth/(uint)r->numCells() : 10;
  m_t << "\\trowd \\trgaph108\\trleft-108"
         "\\trbrdrt\\brdrs\\brdrw10 "
         "\\trbrdrl\\brdrs\\brdrw10 "
         "\\trbrdrb\\brdrs\\brdrw10 "
         "\\trbrdrr\\brdrs\\brdrw10 "
         "\\trbrdrh\\brdrs\\brdrw10 "
         "\\trbrdrv\\brdrs\\brdrw10 \n";
  for (i=0;i<r->numCells();i++)
  {
    if (r->isHeading())
    {
      m_t << "\\clcbpat16"; // set cell shading to light grey (color 16 in the clut)
    }
    m_t << "\\clvertalt\\clbrdrt\\brdrs\\brdrw10 "
           "\\clbrdrl\\brdrs\\brdrw10 "
           "\\clbrdrb\\brdrs\\brdrw10 "
           "\\clbrdrr \\brdrs\\brdrw10 "
           "\\cltxlrtb "
           "\\cellx" << ((i+1)*columnWidth) << "\n";
  }
  m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlRow *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlRow)}\n");
  m_t << "\n";
  m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
  m_t << "{\\row }\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlCell)}\n");
  m_t << "{" << align(c);
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlCell *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlCell)}\n");
  m_t << "\\cell }";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  //DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocInternal)}\n");
  //m_t << "{"; // start desc
  //m_t << "{\\b "; // start bold
  //m_t << theTranslator->trForInternalUseOnly();
  //m_t << "}"; // end bold
  //m_t << "\\par\n";
  //incIndentLevel();
  //m_t << rtf_Style_Reset << getStyle("DescContinue");
  //m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocInternal *)
{
  if (m_hide) return;
  //DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocInternal)}\n");
  //m_t << "\\par";
  //decIndentLevel();
  //m_t << "}"; // end desc
  //m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHRef)}\n");
  if (Config_getBool(RTF_HYPERLINKS))
  {
    if (href->url().startsWith("#"))
    {
      // when starting with # so a local link
      QCString cite;
      cite = href->file() + "_" + href->url().right(href->url().length()-1);
      m_t << "{\\field "
               "{\\*\\fldinst "
                 "{ HYPERLINK \\\\l \"" << rtfFormatBmkStr(cite) << "\" "
                 "}{}"
               "}"
               "{\\fldrslt "
                 "{\\cs37\\ul\\cf2 ";
    }
    else
    {
      m_t << "{\\field "
                 "{\\*\\fldinst "
                 "{ HYPERLINK \"" << href->url() << "\" "
                 "}{}"
               "}"
               "{\\fldrslt "
                 "{\\cs37\\ul\\cf2 ";
    }
  }
  else
  {
    m_t << "{\\f2 ";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHRef *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHRef)}\n");
  if (Config_getBool(RTF_HYPERLINKS))
  {
    m_t <<     "}"
             "}"
           "}";
  }
  else
  {
    m_t << "}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlHeader)}\n");
  m_t << "{" // start section
      << rtf_Style_Reset;
  QCString heading;
  int level = std::min(header->level(),5);
  heading.sprintf("Heading%d",level);
  // set style
  m_t << rtf_Style[heading.str()].reference();
  // make open table of contents entry that will be closed in visitPost method
  m_t << "{\\tc\\tcl" << level << " ";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocHtmlHeader *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlHeader)}\n");
  // close open table of contents entry
  m_t << "} \\par";
  m_t << "}\n"; // end section
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocImage *img)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocImage)}\n");
  includePicturePreRTF(img->name(), img->type()==DocImage::Rtf, img->hasCaption(), img->isInlineImage());
}
void RTFDocVisitor::includePicturePreRTF(const QCString &name, bool isTypeRTF, bool hasCaption, bool inlineImage)
{
  if (isTypeRTF)
  {
    if (!inlineImage)
    {
      m_t << "\\par\n";
      m_t << "{\n";
      m_t << rtf_Style_Reset << "\n";
      if (hasCaption || m_lastIsPara) m_t << "\\par\n";
      m_t << "\\pard \\qc ";
    }
    m_t << "{ \\field\\flddirty {\\*\\fldinst  INCLUDEPICTURE \"";
    m_t << name;
    m_t << "\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt Image}}\n";
    if (!inlineImage)
    {
      m_t << "\\par\n";
      if (hasCaption)
      {
         m_t << "\\pard \\qc \\b";
         m_t << "{Image \\field\\flddirty{\\*\\fldinst { SEQ Image \\\\*Arabic }}{\\fldrslt {\\noproof 1}} ";
      }
      m_lastIsPara=TRUE;
    }
    else
    {
      if (hasCaption) m_t << "{\\comment "; // to prevent caption to be shown
    }
  }
  else // other format -> skip
  {
    pushHidden(m_hide);
    m_hide=TRUE;
  }
}

void RTFDocVisitor::visitPost(DocImage *img)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocImage)}\n");
  includePicturePostRTF(img->type()==DocImage::Rtf, img->hasCaption(), img->isInlineImage());
}

void RTFDocVisitor::includePicturePostRTF(bool isTypeRTF, bool hasCaption, bool inlineImage)
{
  if (isTypeRTF)
  {
    if (m_hide) return;
    if (inlineImage)
    {
      if (hasCaption) m_t << " }";
    }
    else
    {
      if (hasCaption)
      {
        m_t << "}\n";
        m_t << "\\par}\n";
      }
      else
      {
        m_t << "}\n";
      }
    }
  }
  else
  {
    m_hide = popHidden();
  }
}

void RTFDocVisitor::visitPre(DocDotFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocDotFile)}\n");
  writeDotFile(df);
}

void RTFDocVisitor::visitPost(DocDotFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocDotFile)}\n");
  includePicturePostRTF(true, df->hasCaption());
}
void RTFDocVisitor::visitPre(DocMscFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocMscFile)}\n");
  writeMscFile(df);
}

void RTFDocVisitor::visitPost(DocMscFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocMscFile)}\n");
  includePicturePostRTF(true, df->hasCaption());
}

void RTFDocVisitor::visitPre(DocDiaFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocDiaFile)}\n");
  writeDiaFile(df);
}

void RTFDocVisitor::visitPost(DocDiaFile *df)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocDiaFile)}\n");
  includePicturePostRTF(true, df->hasCaption());
}

void RTFDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocLink)}\n");
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void RTFDocVisitor::visitPost(DocLink *lnk)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocLink)}\n");
  endLink(lnk->ref());
}

void RTFDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocRef)}\n");
  // when ref->isSubPage()==TRUE we use ref->file() for HTML and
  // ref->anchor() for LaTeX/RTF
  if (ref->isSubPage())
  {
    startLink(ref->ref(),QCString(),ref->anchor());
  }
  else
  {
    if (!ref->file().isEmpty()) startLink(ref->ref(),ref->file(),ref->anchor());
  }
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void RTFDocVisitor::visitPost(DocRef *ref)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocRef)}\n");
  if (!ref->file().isEmpty()) endLink(ref->ref());
  //m_t << " ";
}


void RTFDocVisitor::visitPre(DocSecRefItem *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSecRefItem)}\n");
}

void RTFDocVisitor::visitPost(DocSecRefItem *)
{
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSecRefItem)}\n");
}

void RTFDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocSecRefList)}\n");
  m_t << "{\n";
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("LatexTOC") << "\n";
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPost(DocSecRefList *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSecRefList)}\n");
  decIndentLevel();
  m_t << "\\par";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocParamSect)}\n");
  m_t << "{"; // start param list
  if (!m_lastIsPara) m_t << "\\par\n";
  //m_t << "{\\b "; // start bold
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
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
  m_t << "\\par";
  m_t << "}\n";
  bool useTable = s->type()==DocParamSect::Param ||
                  s->type()==DocParamSect::RetVal ||
                  s->type()==DocParamSect::Exception ||
                  s->type()==DocParamSect::TemplateParam;
  if (!useTable)
  {
    incIndentLevel();
  }
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPost(DocParamSect *s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocParamSect)}\n");
  //m_t << "\\par\n";
  bool useTable = s->type()==DocParamSect::Param ||
                  s->type()==DocParamSect::RetVal ||
                  s->type()==DocParamSect::Exception ||
                  s->type()==DocParamSect::TemplateParam;
  if (!useTable)
  {
    decIndentLevel();
  }
  m_t << "}\n";
}

void RTFDocVisitor::visitPre(DocParamList *pl)
{
  static int columnPos[4][5] =
  { { 2, 25, 100, 100, 100 }, // no inout, no type
    { 3, 14,  35, 100, 100 }, // inout, no type
    { 3, 25,  50, 100, 100 }, // no inout, type
    { 4, 14,  35, 55,  100 }, // inout, type
  };
  int config=0;
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocParamList)}\n");

  DocParamSect::Type parentType = DocParamSect::Unknown;
  DocParamSect *sect = 0;
  if (pl->parent() && pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    parentType = ((DocParamSect*)pl->parent())->type();
    sect=(DocParamSect*)pl->parent();
  }
  bool useTable = parentType==DocParamSect::Param ||
                  parentType==DocParamSect::RetVal ||
                  parentType==DocParamSect::Exception ||
                  parentType==DocParamSect::TemplateParam;
  if (sect && sect->hasInOutSpecifier()) config+=1;
  if (sect && sect->hasTypeSpecifier())  config+=2;
  if (useTable)
  {
    int i;
    m_t << "\\trowd \\trgaph108\\trleft426\\tblind426"
         "\\trbrdrt\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrl\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrb\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrr\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrh\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrv\\brdrs\\brdrw10\\brdrcf15 "<< "\n";
    for (i=0;i<columnPos[config][0];i++)
    {
      m_t << "\\clvertalt\\clbrdrt\\brdrs\\brdrw10\\brdrcf15 "
           "\\clbrdrl\\brdrs\\brdrw10\\brdrcf15 "
           "\\clbrdrb\\brdrs\\brdrw10\\brdrcf15 "
           "\\clbrdrr \\brdrs\\brdrw10\\brdrcf15 "
           "\\cltxlrtb "
           "\\cellx" << (rtf_pageWidth*columnPos[config][i+1]/100) << "\n";
    }
    m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
  }

  if (sect && sect->hasInOutSpecifier())
  {
    if (useTable)
    {
      m_t << "{";
    }

    // Put in the direction: in/out/in,out if specified.
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

    if (useTable)
    {
      m_t << "\\cell }";
    }
  }

  if (sect && sect->hasTypeSpecifier())
  {
    if (useTable)
    {
      m_t << "{";
    }
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
    if (useTable)
    {
      m_t << "\\cell }";
    }
  }


  if (useTable)
  {
    m_t << "{";
  }

  m_t << "{\\i ";
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
  m_t << "} ";

  if (useTable)
  {
    m_t << "\\cell }{";
  }
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPost(DocParamList *pl)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocParamList)}\n");

  DocParamSect::Type parentType = DocParamSect::Unknown;
  //DocParamSect *sect = 0;
  if (pl->parent() && pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    parentType = ((DocParamSect*)pl->parent())->type();
    //sect=(DocParamSect*)pl->parent();
  }
  bool useTable = parentType==DocParamSect::Param ||
                  parentType==DocParamSect::RetVal ||
                  parentType==DocParamSect::Exception ||
                  parentType==DocParamSect::TemplateParam;
  if (useTable)
  {
    m_t << "\\cell }\n";
    //m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
    m_t << "{\\row }\n";
  }
  else
  {
    m_t << "\\par\n";
  }

  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  bool anonymousEnum = x->file()=="@";
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocXRefItem)}\n");
  if (!m_lastIsPara)
  {
    m_t << "\\par\n";
    m_lastIsPara=TRUE;
  }
  m_t << "{"; // start param list
  //m_t << "{\\b "; // start bold
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
  if (Config_getBool(RTF_HYPERLINKS) && !anonymousEnum)
  {
    QCString refName;
    if (!x->file().isEmpty())
    {
      refName+=stripPath(x->file());
    }
    if (!x->file().isEmpty() && !x->anchor().isEmpty())
    {
      refName+="_";
    }
    if (!x->anchor().isEmpty())
    {
      refName+=x->anchor();
    }

    m_t << "{\\field "
             "{\\*\\fldinst "
               "{ HYPERLINK  \\\\l \"" << rtfFormatBmkStr(refName) << "\" "
               "}{}"
             "}"
             "{\\fldrslt "
               "{\\cs37\\ul\\cf2 ";
    filter(x->title());
    m_t <<     "}"
             "}"
           "}";
  }
  else
  {
    filter(x->title());
  }
  m_t << ":";
  m_t << "\\par";
  m_t << "}"; // end bold
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::visitPost(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocXRefItem)}\n");
  m_t << "\\par\n";
  decIndentLevel();
  m_t << "}\n"; // end xref item
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocInternalRef)}\n");
  startLink("",ref->file(),ref->anchor());
}

void RTFDocVisitor::visitPost(DocInternalRef *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocInternalRef)}\n");
  endLink("");
  m_t << " ";
}

void RTFDocVisitor::visitPre(DocText *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocText)}\n");
}

void RTFDocVisitor::visitPost(DocText *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocText)}\n");
}

void RTFDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPre(DocHtmlBlockQuote)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{"; // start desc
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("DescContinue");
}

void RTFDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlBlockQuote)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  decIndentLevel();
  m_t << "}"; // end desc
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::visitPre(DocVhdlFlow *)
{
  if (m_hide) return;
}

void RTFDocVisitor::visitPost(DocVhdlFlow *)
{
  if (m_hide) return;
}

void RTFDocVisitor::visitPre(DocParBlock *)
{
  if (m_hide) return;
}

void RTFDocVisitor::visitPost(DocParBlock *)
{
  if (m_hide) return;
}


//static char* getMultiByte(int c)
//{
//    static char s[10];
//    sprintf(s,"\\'%X",c);
//    return s;
//}

void RTFDocVisitor::filter(const QCString &str,bool verbatim)
{
  if (!str.isEmpty())
  {
    const unsigned char *p=(const unsigned char *)str.data();
    unsigned char c;
    //unsigned char pc='\0';
    while (*p)
    {
      //static bool MultiByte = FALSE;
      c=*p++;

      //if ( MultiByte )
      //{
      //  m_t << getMultiByte( c );
      //  MultiByte = FALSE;
      //  continue;
      //}
      //if ( c >= 0x80 )
      //{
      //  MultiByte = TRUE;
      //  m_t << getMultiByte( c );
      //  continue;
      //}

      switch (c)
      {
        case '{':  m_t << "\\{";            break;
        case '}':  m_t << "\\}";            break;
        case '\\': m_t << "\\\\";           break;
        case '\n': if (verbatim)
                   {
                     m_t << "\\par\n";
                   }
                   else
                   {
                     m_t << '\n';
                   }
                   break;
        default:   m_t << (char)c;
      }
      //pc = c;
    }
  }
}

void RTFDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  if (ref.isEmpty() && Config_getBool(RTF_HYPERLINKS))
  {
    QCString refName;
    if (!file.isEmpty())
    {
      refName+=stripPath(file);
    }
    if (!file.isEmpty() && !anchor.isEmpty())
    {
      refName+='_';
    }
    if (!anchor.isEmpty())
    {
      refName+=anchor;
    }

    m_t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    m_t << rtfFormatBmkStr(refName);
    m_t << "\" }{}";
    m_t << "}{\\fldrslt {\\cs37\\ul\\cf2 ";
  }
  else
  {
    m_t << "{\\b ";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::endLink(const QCString &ref)
{
  if (ref.isEmpty() && Config_getBool(RTF_HYPERLINKS))
  {
    m_t << "}}}";
  }
  else
  {
    m_t << "}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::writeDotFile(DocDotFile *df)
{
  writeDotFile(df->file(), df->hasCaption(), df->srcFile(), df->srcLine());
}
void RTFDocVisitor::writeDotFile(const QCString &filename, bool hasCaption,
                                 const QCString &srcFile, int srcLine)
{
  QCString baseName=filename;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeDotGraphFromFile(filename,outDir,baseName,GOF_BITMAP,srcFile,srcLine);
  QCString imgExt = getDotImageExtension();
  includePicturePreRTF(baseName + "." + imgExt, true, hasCaption);
}

void RTFDocVisitor::writeMscFile(DocMscFile *df)
{
  writeMscFile(df->file(), df->hasCaption(), df->srcFile(), df->srcLine());
}
void RTFDocVisitor::writeMscFile(const QCString &fileName, bool hasCaption,
                                 const QCString &srcFile, int srcLine)
{
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_BITMAP,srcFile,srcLine);
  includePicturePreRTF(baseName + ".png", true, hasCaption);
}

void RTFDocVisitor::writeDiaFile(DocDiaFile *df)
{
  QCString baseName=df->file();
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeDiaGraphFromFile(df->file(),outDir,baseName,DIA_BITMAP,df->srcFile(),df->srcLine());
  includePicturePreRTF(baseName + ".png", true, df->hasCaption());
}

void RTFDocVisitor::writePlantUMLFile(const QCString &fileName, bool hasCaption)
{
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  QCString outDir = Config_getString(RTF_OUTPUT);
  PlantumlManager::instance().generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_BITMAP);
  includePicturePreRTF(baseName + ".png", true, hasCaption);
}
