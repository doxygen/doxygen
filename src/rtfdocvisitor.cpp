/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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
#include "rtfgen.h"
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
#include "portable.h"
#include "codefragment.h"

//#define DBG_RTF(x) m_t << x
#define DBG_RTF(x) do {} while(0)

static QCString align(const DocHtmlCell &cell)
{
  for (const auto &attr : cell.attribs())
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

static QCString makeBaseName(const QCString &name)
{
  QCString baseName = name;
  int i = baseName.findRev('/');
  if (i!=-1)
  {
    baseName=baseName.mid(i+1);
  }
  return baseName;
}

RTFDocVisitor::RTFDocVisitor(TextStream &t,OutputCodeList &ci,
                             const QCString &langExt, int hierarchyLevel)
  : m_t(t), m_ci(ci), m_langExt(langExt), m_hierarchyLevel(hierarchyLevel)
{
}

QCString RTFDocVisitor::getStyle(const QCString &name)
{
  QCString n = name + QCString().setNum(indentLevel());
  StyleData &sd = rtf_Style[n.str()];
  return sd.reference();
}

QCString RTFDocVisitor::getListTable(const int id)
{
  for (int i=0 ; rtf_Table_Default[i].definition ; i++ )
  {
    if ((id == rtf_Table_Default[i].id) && (m_indentLevel == rtf_Table_Default[i].lvl))
    {
      return rtf_Table_Default[i].place;
    }
  }
  ASSERT(0);
  return "";
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

void RTFDocVisitor::operator()(const DocWord &w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocWord)}\n");
  filter(w.word());
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocLinkedWord &w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocLinkedWord)}\n");
  startLink(w.ref(),w.file(),w.anchor());
  filter(w.word());
  endLink(w.ref());
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocWhiteSpace &w)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocWhiteSpace)}\n");
  if (m_insidePre)
  {
    m_t << w.chars();
  }
  else
  {
    m_t << " ";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocSymbol &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocSymbol)}\n");
  const char *res = HtmlEntityMapper::instance().rtf(s.symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("RTF: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocEmoji &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocEmoji)}\n");
  const char *res = EmojiEntityMapper::instance().unicode(s.index());
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
    m_t << s.name();
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocURL &u)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocURL)}\n");
  if (Config_getBool(RTF_HYPERLINKS))
  {
    m_t << "{\\field "
             "{\\*\\fldinst "
               "{ HYPERLINK \"";
    if (u.isEmail()) m_t << "mailto:";
    m_t << u.url();
    m_t <<  "\" }"
               "{}";
    m_t <<   "}"
             "{\\fldrslt "
               "{\\cs37\\ul\\cf2 ";
    filter(u.url());
    m_t <<     "}"
             "}"
           "}\n";
  }
  else
  {
    m_t << "{\\f2 ";
    filter(u.url());
    m_t << "}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocLineBreak &)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocLineBreak)}\n");
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHorRuler &)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocHorRuler)}\n");
  m_t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw5\\brsp20 \\adjustright \\par}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocStyleChange &s)
{
  if (m_hide) return;
  m_lastIsPara=FALSE;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocStyleChange)}\n");
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) m_t << "{\\b ";      else m_t << "} ";
      break;
    case DocStyleChange::S:
    case DocStyleChange::Strike:
    case DocStyleChange::Del:
      if (s.enable()) m_t << "{\\strike ";      else m_t << "} ";
      break;
    case DocStyleChange::Underline:
    case DocStyleChange::Ins:
      if (s.enable()) m_t << "{\\ul ";      else m_t << "} ";
      break;
    case DocStyleChange::Italic:
      if (s.enable()) m_t << "{\\i ";     else m_t << "} ";
      break;
    case DocStyleChange::Code:
      if (s.enable()) m_t << "{\\f2 ";   else m_t << "} ";
      break;
    case DocStyleChange::Subscript:
      if (s.enable()) m_t << "{\\sub ";    else m_t << "} ";
      break;
    case DocStyleChange::Superscript:
      if (s.enable()) m_t << "{\\super ";    else m_t << "} ";
      break;
    case DocStyleChange::Center:
      if (s.enable()) m_t << "{\\qc "; else m_t << "} ";
      break;
    case DocStyleChange::Small:
      if (s.enable()) m_t << "{\\sub ";  else m_t << "} ";
      break;
    case DocStyleChange::Cite:
      if (s.enable()) m_t << "{\\i ";     else m_t << "} ";
      break;
    case DocStyleChange::Preformatted:
      if (s.enable())
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
  }
}

void RTFDocVisitor::operator()(const DocVerbatim &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocVerbatim)}\n");
  QCString lang = m_langExt;
  if (!s.language().isEmpty()) // explicit language setting
  {
    lang = s.language();
  }
  SrcLangExt langExt = getLanguageFromCodeLang(lang);
  switch(s.type())
  {
    case DocVerbatim::Code:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      getCodeParser(lang).parseCode(m_ci,s.context(),s.text(),langExt,
                                    Config_getBool(STRIP_CODE_COMMENTS),
                                    s.isExample(),s.exampleFile());
      //m_t << "\\par\n";
      m_t << "}\n";
      break;
    case DocVerbatim::JavaDocLiteral:
      filter(s.text(),TRUE);
      break;
    case DocVerbatim::JavaDocCode:
      m_t << "{\n";
      m_t << "{\\f2 ";
      filter(s.text(),TRUE);
      m_t << "}";
      m_t << "}\n";
      break;
    case DocVerbatim::Verbatim:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      filter(s.text(),TRUE);
      //m_t << "\\par\n";
      m_t << "}\n";
      break;
    case DocVerbatim::RtfOnly:
      m_t << s.text();
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
        QCString fileName(4096, QCString::ExplicitSize);

        fileName.sprintf("%s%d%s",
            qPrint(Config_getString(RTF_OUTPUT)+"/inline_dotgraph_"),
            dotindex++,
            ".dot"
           );
        std::ofstream file = Portable::openOutputStream(fileName);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(fileName));
        }
        else
        {
          QCString stext = s.text();
          file.write( stext.data(), stext.length() );
          file.close();
        }

        writeDotFile(fileName, s.hasCaption(), s.srcFile(), s.srcLine());
        visitChildren(s);
        includePicturePostRTF(true, s.hasCaption());

        if (Config_getBool(DOT_CLEANUP)) Dir().remove(fileName.str());
      }
      break;
    case DocVerbatim::Msc:
      {
        static int mscindex = 1;
        QCString baseName(4096, QCString::ExplicitSize);

        baseName.sprintf("%s%d%s",
            qPrint(Config_getString(RTF_OUTPUT)+"/inline_mscgraph_"),
            mscindex++,
            ".msc"
           );
        std::ofstream file = Portable::openOutputStream(baseName);
        if (!file.is_open())
        {
          err("Could not open file %s for writing\n",qPrint(baseName));
        }
        QCString text = "msc {";
        text+=s.text();
        text+="}";
        file.write( text.data(), text.length() );
        file.close();

        writeMscFile(baseName, s.hasCaption(), s.srcFile(), s.srcLine());
        visitChildren(s);
        includePicturePostRTF(true, s.hasCaption());

        if (Config_getBool(DOT_CLEANUP)) Dir().remove(baseName.str());
      }
      break;
    case DocVerbatim::PlantUML:
      {
        QCString rtfOutput = Config_getString(RTF_OUTPUT);
        QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                       rtfOutput,s.exampleFile(),s.text(),PlantumlManager::PUML_BITMAP,
                       s.engine(),s.srcFile(),s.srcLine(),true);

        writePlantUMLFile(baseName, s.hasCaption());
        visitChildren(s);
        includePicturePostRTF(true, s.hasCaption());
      }
      break;
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocAnchor &anc)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocAnchor)}\n");
  QCString anchor;
  if (!anc.file().isEmpty())
  {
    anchor+=stripPath(anc.file());
  }
  if (!anc.file().isEmpty() && !anc.anchor().isEmpty())
  {
    anchor+="_";
  }
  if (!anc.anchor().isEmpty())
  {
    anchor+=anc.anchor();
  }
  m_t << "{\\bkmkstart " << rtfFormatBmkStr(anchor) << "}\n";
  m_t << "{\\bkmkend " << rtfFormatBmkStr(anchor) << "}\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocInclude &inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc.extension());
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocInclude)}\n");
  switch(inc.type())
  {
     case DocInclude::IncWithLines:
      {
         m_t << "{\n";
         m_t << "\\par\n";
         m_t << rtf_Style_Reset << getStyle("CodeExample");
         FileInfo cfi( inc.file().str() );
         auto fd = createFileDef( cfi.dirPath(), cfi.fileName() );
         getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                           inc.text(),
                                           langExt,
                                           inc.stripCodeComments(),
                                           inc.isExample(),
                                           inc.exampleFile(),
                                           fd.get(),   // fileDef,
                                           -1,    // start line
                                           -1,    // end line
                                           FALSE, // inline fragment
                                           nullptr,     // memberDef
                                           TRUE   // show line numbers
					   );
         m_t << "\\par";
         m_t << "}\n";
      }
      break;
    case DocInclude::Include:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      getCodeParser(inc.extension()).parseCode(m_ci,inc.context(),
                                        inc.text(),langExt,
                                        inc.stripCodeComments(),
                                        inc.isExample(),
                                        inc.exampleFile(),
                                        nullptr,     // fileDef
                                        -1,    // startLine
                                        -1,    // endLine
                                        TRUE,  // inlineFragment
                                        nullptr,     // memberDef
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
      m_t << inc.text();
      break;
    case DocInclude::VerbInclude:
      m_t << "{\n";
      m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      filter(inc.text());
      m_t << "\\par";
      m_t << "}\n";
      break;
    case DocInclude::Snippet:
    case DocInclude::SnippetWithLines:
      m_t << "{\n";
      if (!m_lastIsPara) m_t << "\\par\n";
      m_t << rtf_Style_Reset << getStyle("CodeExample");
      CodeFragmentManager::instance().parseCodeFragment(m_ci,
                                         inc.file(),
                                         inc.blockId(),
                                         inc.context(),
                                         inc.type()==DocInclude::SnippetWithLines,
                                         inc.trimLeft(),
                                         inc.stripCodeComments()
                                        );
      m_t << "}";
      break;
  }
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocIncOperator &op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text='%s'\n",
  //    op.type(),op.isFirst(),op.isLast(),qPrint(op.text()));
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocIncOperator)}\n");
  QCString locLangExt = getFileNameExtension(op.includeFileName());
  if (locLangExt.isEmpty()) locLangExt = m_langExt;
  SrcLangExt langExt = getLanguageFromFileName(locLangExt);
  if (op.isFirst())
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
  if (op.type()!=DocIncOperator::Skip)
  {
    m_hide = popHidden();
    if (!m_hide)
    {
      std::unique_ptr<FileDef> fd = nullptr;
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

void RTFDocVisitor::operator()(const DocFormula &f)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocFormula)}\n");
  bool bDisplay = !f.isInline();
  if (bDisplay)
  {
    m_t << "\\par";
    m_t << "{";
    m_t << "\\pard\\plain";
    m_t << "\\pard";
    m_t << "\\qc";
  }
  m_t << "{ \\field\\flddirty {\\*\\fldinst  INCLUDEPICTURE \"" << f.relPath() << f.name() << ".png\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt Image}}";
  if (bDisplay)
  {
    m_t << "\\par}";
  }
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocIndexEntry &i)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::visit(DocIndexEntry)}\n");
  m_t << "{\\xe \\v " << i.entry() << "}\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocSimpleSectSep &)
{
}

void RTFDocVisitor::operator()(const DocCite &cite)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocCite &)}\n");
  if (!cite.file().isEmpty())
  {
    startLink(cite.ref(),cite.file(),cite.anchor());
  }
  else
  {
    m_t << "{\\b ";
  }
  filter(cite.text());
  if (!cite.file().isEmpty())
  {
    endLink(cite.ref());
  }
  else
  {
    m_t << "}";
  }
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void RTFDocVisitor::operator()(const DocAutoList &l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocAutoList &)}\n");
  m_t << "{\n";
  int level = indentLevel();
  m_listItemInfo[level].isEnum = l.isEnumList();
  m_listItemInfo[level].isCheck = l.isCheckedList();
  m_listItemInfo[level].type   = '1';
  m_listItemInfo[level].number = 1;
  m_lastIsPara=FALSE;
  visitChildren(l);
  if (!m_lastIsPara) m_t << "\\par";
  m_t << "}\n";
  m_lastIsPara=TRUE;
  if (!l.isCheckedList() && indentLevel()==0) m_t << "\\par\n";
}

void RTFDocVisitor::operator()(const DocAutoListItem &li)
{
  static int prevLevel = -1;
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocAutoListItem &)}\n");
  int level = indentLevel();
  if ((level != prevLevel-1) &&
      (!(level==prevLevel && level != 0 && m_listItemInfo[level].isCheck)) &&
      (!m_lastIsPara))
    m_t << "\\par\n";
  prevLevel= level;
  m_t << rtf_Style_Reset;
  if (m_listItemInfo[level].isEnum)
  {
    m_t << getStyle("ListEnum") << "\n";
    m_t << m_listItemInfo[level].number << ".\\tab ";
    m_listItemInfo[level].number++;
  }
  else
  {
    switch (li.itemNumber())
    {
      case DocAutoList::Unchecked: // unchecked
        m_t << getListTable(2) << "\n";
        break;
      case DocAutoList::Checked_x: // checked with x
      case DocAutoList::Checked_X: // checked with X
        m_t << getListTable(3) << "\n";
        break;
      default:
        m_t << getListTable(1) << "\n";
        break;
    }
  }
  incIndentLevel();
  m_lastIsPara=false;
  visitChildren(li);
  decIndentLevel();
}

void RTFDocVisitor::operator()(const DocPara &p)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocPara &)}\n");
  visitChildren(p);
  if (!m_lastIsPara &&
      !p.isLast() &&            // omit <p> for last paragraph
      !(p.parent() &&           // and for parameters & sections
        std::get_if<DocParamSect>(p.parent())
       )
     )
  {
    m_t << "\\par\n";
    m_lastIsPara=TRUE;
  }
}

void RTFDocVisitor::operator()(const DocRoot &r)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocRoot &)}\n");
  if (r.indent()) incIndentLevel();
  m_t << "{" << rtf_Style["BodyText"].reference() << "\n";
  visitChildren(r);
  if (!m_lastIsPara && !r.singleLine()) m_t << "\\par\n";
  m_t << "}";
  m_lastIsPara=TRUE;
  if (r.indent()) decIndentLevel();
}

void RTFDocVisitor::operator()(const DocSimpleSect &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSimpleSect &)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{"; // start desc
  //m_t << "{\\b "; // start bold
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
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

  incIndentLevel();
  if (s.type()!=DocSimpleSect::User && s.type()!=DocSimpleSect::Rcs)
  {
    m_t << "\\par";
    m_t << "}"; // end bold
    m_t << rtf_Style_Reset << getStyle("DescContinue");
    m_t << "{\\s17 \\sa60 \\sb30\n";
  }
  else
  {
    if (s.title())
    {
      std::visit(*this,*s.title());
    }
    m_t << "\\par\n";
    m_t << "}"; // end bold
    m_t << rtf_Style_Reset << getStyle("DescContinue");
  }
  m_lastIsPara=FALSE;
  visitChildren(s);
  if (!m_lastIsPara) m_t << "\\par\n";
  decIndentLevel();
  if (s.type()!=DocSimpleSect::User && s.type()!=DocSimpleSect::Rcs)
  {
    m_t << "}"; // end DescContinue
  }
  m_t << "}"; // end desc
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocTitle &t)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocTitle &)}\n");
  if (m_hide) return;
  visitChildren(t);
}

void RTFDocVisitor::operator()(const DocSimpleList &l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSimpleSect &)}\n");
  m_t << "{\n";
  m_listItemInfo[indentLevel()].isEnum = false;
  m_listItemInfo[indentLevel()].isCheck = false;
  m_lastIsPara=FALSE;
  visitChildren(l);
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocSimpleListItem &li)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSimpleListItem &)}\n");
  m_t << "\\par" << rtf_Style_Reset << getStyle("ListBullet") << "\n";
  m_lastIsPara=FALSE;
  incIndentLevel();
  if (li.paragraph())
  {
    std::visit(*this,*li.paragraph());
  }
  decIndentLevel();
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocSimpleListItem)}\n");
}

void RTFDocVisitor::operator()(const DocSection &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSection &)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{\\bkmkstart " << rtfFormatBmkStr(stripPath(s.file())+"_"+s.anchor()) << "}\n";
  m_t << "{\\bkmkend " << rtfFormatBmkStr(stripPath(s.file())+"_"+s.anchor()) << "}\n";
  m_t << "{{" // start section
      << rtf_Style_Reset;
  QCString heading;
  int level = std::min(s.level()+2+m_hierarchyLevel,4);
  if (level <= 0)
    level = 1;
  heading.sprintf("Heading%d",level);
  // set style
  m_t << rtf_Style[heading.str()].reference() << "\n";
  // make table of contents entry
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  m_t << "\n\\par" << "}\n";
  m_t << "{\\tc\\tcl" << level << " \\v ";
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  m_t << "}\n";
  m_lastIsPara=TRUE;
  visitChildren(s);
  m_t << "\\par}\n"; // end section
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlList &l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlList &)}\n");
  m_t << "{\n";
  int level = indentLevel();
  m_listItemInfo[level].isEnum = l.type()==DocHtmlList::Ordered;
  m_listItemInfo[level].isCheck = false;
  m_listItemInfo[level].number = 1;
  m_listItemInfo[level].type   = '1';
  for (const auto &opt : l.attribs())
  {
    if (opt.name=="type")
    {
      m_listItemInfo[level].type = opt.value[0];
    }
    if (opt.name=="start")
    {
      bool ok = false;
      int val = opt.value.toInt(&ok);
      if (ok) m_listItemInfo[level].number = val;
    }
  }
  m_lastIsPara=FALSE;
  visitChildren(l);
  m_t << "\\par" << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlListItem &l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlListItem &)}\n");
  m_t << "\\par\n";
  m_t << rtf_Style_Reset;
  int level = indentLevel();
  if (m_listItemInfo[level].isEnum)
  {
    for (const auto &opt : l.attribs())
    {
      if (opt.name=="value")
      {
        bool ok = false;
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
  visitChildren(l);
  decIndentLevel();
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocHtmlListItem)}\n");
}

void RTFDocVisitor::operator()(const DocHtmlDescList &dl)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlDescList &)}\n");
  //m_t << "{\n";
  //m_t << rtf_Style_Reset << getStyle("ListContinue");
  //m_lastIsPara=FALSE;
  visitChildren(dl);
  //m_t << "}\n";
  //m_t << "\\par\n";
  //m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlDescTitle &dt)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlDescTitle &)}\n");
  //m_t << "\\par\n";
  //m_t << "{\\b ";
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
  m_lastIsPara=FALSE;
  visitChildren(dt);
  m_t << "\\par\n";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlDescData &dd)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlDescData &)}\n");
  incIndentLevel();
  m_t << "{" << rtf_Style_Reset << getStyle("DescContinue");
  visitChildren(dd);
  m_t << "\\par";
  m_t << "}\n";
  decIndentLevel();
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlTable &t)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlTable &)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_lastIsPara=TRUE;
  if (t.caption())
  {
    const DocHtmlCaption &c = std::get<DocHtmlCaption>(*t.caption());
    m_t << "\\pard \\qc \\b";
    if (!c.file().isEmpty())
    {
      m_t << "{\\bkmkstart " << rtfFormatBmkStr(stripPath(c.file())+"_"+c.anchor()) << "}\n";
      m_t << "{\\bkmkend " << rtfFormatBmkStr(stripPath(c.file())+"_"+c.anchor()) << "}\n";
    }
    m_t << "{Table \\field\\flddirty{\\*\\fldinst { SEQ Table \\\\*Arabic }}{\\fldrslt {\\noproof 1}} ";
    std::visit(*this,*t.caption());
  }
  visitChildren(t);
  m_t << "\\pard\\plain\n";
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlCaption &c)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlCaption &)}\n");
  visitChildren(c);
  m_t << "}\n\\par\n";
}

void RTFDocVisitor::operator()(const DocHtmlRow &r)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlRow &)}\n");
  size_t columnWidth=r.numCells()>0 ? rtf_pageWidth/r.numCells() : 10;
  m_t << "\\trowd \\trgaph108\\trleft-108"
         "\\trbrdrt\\brdrs\\brdrw10 "
         "\\trbrdrl\\brdrs\\brdrw10 "
         "\\trbrdrb\\brdrs\\brdrw10 "
         "\\trbrdrr\\brdrs\\brdrw10 "
         "\\trbrdrh\\brdrs\\brdrw10 "
         "\\trbrdrv\\brdrs\\brdrw10 \n";
  for (size_t i=0;i<r.numCells();i++)
  {
    if (r.isHeading())
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
  visitChildren(r);
  m_t << "\n";
  m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
  m_t << "{\\row }\n";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocHtmlCell &c)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlCell &)}\n");
  m_t << "{" << align(c);
  m_lastIsPara=FALSE;
  visitChildren(c);
  m_t << "\\cell }";
  m_lastIsPara=FALSE;
}

void RTFDocVisitor::operator()(const DocInternal &i)
{
  if (m_hide) return;
  visitChildren(i);
}

void RTFDocVisitor::operator()(const DocHRef &href)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHRef &)}\n");
  if (Config_getBool(RTF_HYPERLINKS))
  {
    if (href.url().startsWith("#"))
    {
      // when starting with # so a local link
      QCString cite;
      cite = href.file() + "_" + href.url().right(href.url().length()-1);
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
                 "{ HYPERLINK \"" << href.url() << "\" "
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
  visitChildren(href);
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

void RTFDocVisitor::operator()(const DocHtmlSummary &s)
{
  if (m_hide) return;
  m_t << "{\\b ";
  visitChildren(s);
  m_t << "}\\par ";
}

void RTFDocVisitor::operator()(const DocHtmlDetails &d)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlDetails &)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  auto summary = d.summary();
  if (summary)
  {
    std::visit(*this,*summary);
    m_t << "{"; // start desc
    incIndentLevel();
    m_t << rtf_Style_Reset << getStyle("DescContinue");
  }
  visitChildren(d);
  if (!m_lastIsPara) m_t << "\\par\n";
  if (summary)
  {
    decIndentLevel();
    m_t << "}"; // end desc
  }
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocHtmlHeader &header)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlHeader &)}\n");
  m_t << "{" // start section
      << rtf_Style_Reset;
  QCString heading;
  int level = std::clamp(header.level()+m_hierarchyLevel,SectionType::MinLevel,SectionType::MaxLevel);
  heading.sprintf("Heading%d",level);
  // set style
  m_t << rtf_Style[heading.str()].reference();
  // make open table of contents entry that will be closed in visitPost method
  m_t << "{\\tc\\tcl" << level << " ";
  m_lastIsPara=FALSE;
  visitChildren(header);
  // close open table of contents entry
  m_t << "} \\par";
  m_t << "}\n"; // end section
  m_lastIsPara=TRUE;
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

void RTFDocVisitor::operator()(const DocImage &img)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocImage &)}\n");
  includePicturePreRTF(img.name(), img.type()==DocImage::Rtf, img.hasCaption(), img.isInlineImage());
  visitChildren(img);
  includePicturePostRTF(img.type()==DocImage::Rtf, img.hasCaption(), img.isInlineImage());
}


void RTFDocVisitor::operator()(const DocDotFile &df)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocDotFile &)}\n");
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(RTF_OUTPUT)+"/"+stripPath(df.file()));
  writeDotFile(df);
  visitChildren(df);
  includePicturePostRTF(true, df.hasCaption());
}
void RTFDocVisitor::operator()(const DocMscFile &df)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocMscFile &)}\n");
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(RTF_OUTPUT)+"/"+stripPath(df.file()));
  writeMscFile(df);
  visitChildren(df);
  includePicturePostRTF(true, df.hasCaption());
}

void RTFDocVisitor::operator()(const DocDiaFile &df)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocDiaFile &)}\n");
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(RTF_OUTPUT)+"/"+stripPath(df.file()));
  writeDiaFile(df);
  visitChildren(df);
  includePicturePostRTF(true, df.hasCaption());
}

void RTFDocVisitor::operator()(const DocPlantUmlFile &df)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocPlantUmlFile &)}\n");
  if (!Config_getBool(DOT_CLEANUP)) copyFile(df.file(),Config_getString(RTF_OUTPUT)+"/"+stripPath(df.file()));
  QCString rtfOutput = Config_getString(RTF_OUTPUT);
  std::string inBuf;
  readInputFile(df.file(),inBuf);
  QCString baseName = PlantumlManager::instance().writePlantUMLSource(
                       rtfOutput,QCString(),inBuf.c_str(),PlantumlManager::PUML_BITMAP,
                       QCString(),df.srcFile(),df.srcLine(),false);
  writePlantUMLFile(baseName, df.hasCaption());
  visitChildren(df);
  includePicturePostRTF(true, df.hasCaption());
}

void RTFDocVisitor::operator()(const DocLink &lnk)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocLink &)}\n");
  startLink(lnk.ref(),lnk.file(),lnk.anchor());
  visitChildren(lnk);
  endLink(lnk.ref());
}

void RTFDocVisitor::operator()(const DocRef &ref)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocRef &)}\n");
  // when ref.isSubPage()==TRUE we use ref.file() for HTML and
  // ref.anchor() for LaTeX/RTF
  if (ref.isSubPage())
  {
    startLink(ref.ref(),QCString(),ref.anchor());
  }
  else
  {
    if (!ref.file().isEmpty()) startLink(ref.ref(),ref.file(),ref.anchor());
  }
  if (!ref.hasLinkText()) filter(ref.targetTitle());
  visitChildren(ref);
  if (!ref.file().isEmpty()) endLink(ref.ref());
  //m_t << " ";
}


void RTFDocVisitor::operator()(const DocSecRefItem &ref)
{
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSecRefItem &)}\n");
  visitChildren(ref);
}

void RTFDocVisitor::operator()(const DocSecRefList &l)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocSecRefList &)}\n");
  m_t << "{\n";
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("LatexTOC") << "\n";
  m_t << "\\par\n";
  m_lastIsPara=TRUE;
  visitChildren(l);
  decIndentLevel();
  m_t << "\\par";
  m_t << "}\n";
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocParamSect &s)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocParamSect &)}\n");
  m_t << "{"; // start param list
  if (!m_lastIsPara) m_t << "\\par\n";
  //m_t << "{\\b "; // start bold
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
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
  m_t << "\\par";
  m_t << "}\n";
  bool useTable = s.type()==DocParamSect::Param ||
                  s.type()==DocParamSect::RetVal ||
                  s.type()==DocParamSect::Exception ||
                  s.type()==DocParamSect::TemplateParam;
  if (!useTable)
  {
    incIndentLevel();
  }
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  m_lastIsPara=TRUE;
  visitChildren(s);
  //m_t << "\\par\n";
  if (!useTable)
  {
    decIndentLevel();
  }
  m_t << "}\n";
}

void RTFDocVisitor::operator()(const DocSeparator &sep)
{
  m_t << " " << sep.chars() << " ";
}

void RTFDocVisitor::operator()(const DocParamList &pl)
{
  static int columnPos[4][5] =
  { { 2, 25, 100, 100, 100 }, // no inout, no type
    { 3, 14,  35, 100, 100 }, // inout, no type
    { 3, 25,  50, 100, 100 }, // no inout, type
    { 4, 14,  35, 55,  100 }, // inout, type
  };
  int config=0;
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocParamList &)}\n");

  DocParamSect::Type parentType = DocParamSect::Unknown;
  const DocParamSect *sect = std::get_if<DocParamSect>(pl.parent());
  if (sect)
  {
    parentType = sect->type();
  }
  bool useTable = parentType==DocParamSect::Param ||
                  parentType==DocParamSect::RetVal ||
                  parentType==DocParamSect::Exception ||
                  parentType==DocParamSect::TemplateParam;
  if (sect && sect->hasInOutSpecifier()) config+=1;
  if (sect && sect->hasTypeSpecifier())  config+=2;
  if (useTable)
  {
    m_t << "\\trowd \\trgaph108\\trleft426\\tblind426"
         "\\trbrdrt\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrl\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrb\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrr\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrh\\brdrs\\brdrw10\\brdrcf15 "
         "\\trbrdrv\\brdrs\\brdrw10\\brdrcf15 "<< "\n";
    for (int i=0;i<columnPos[config][0];i++)
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
    if (pl.direction()!=DocParamSect::Unspecified)
    {
      if (pl.direction()==DocParamSect::In)
      {
        m_t << "in";
      }
      else if (pl.direction()==DocParamSect::Out)
      {
        m_t << "out";
      }
      else if (pl.direction()==DocParamSect::InOut)
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
    for (const auto &type : pl.paramTypes())
    {
      std::visit(*this,type);
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
  for (const auto &param : pl.parameters())
  {
    if (!first) m_t << ","; else first=FALSE;
    std::visit(*this,param);
  }
  m_t << "} ";

  if (useTable)
  {
    m_t << "\\cell }{";
  }
  m_lastIsPara=TRUE;

  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }

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

void RTFDocVisitor::operator()(const DocXRefItem &x)
{
  if (m_hide) return;
  if (x.title().isEmpty()) return;
  bool anonymousEnum = x.file()=="@";
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocXRefItem &)}\n");
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
    if (!x.file().isEmpty())
    {
      refName+=stripPath(x.file());
    }
    if (!x.file().isEmpty() && !x.anchor().isEmpty())
    {
      refName+="_";
    }
    if (!x.anchor().isEmpty())
    {
      refName+=x.anchor();
    }

    m_t << "{\\field "
             "{\\*\\fldinst "
               "{ HYPERLINK  \\\\l \"" << rtfFormatBmkStr(refName) << "\" "
               "}{}"
             "}"
             "{\\fldrslt "
               "{\\cs37\\ul\\cf2 ";
    filter(x.title());
    m_t <<     "}"
             "}"
           "}";
  }
  else
  {
    filter(x.title());
  }
  m_t << ":";
  m_t << "\\par";
  m_t << "}"; // end bold
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  m_lastIsPara=FALSE;
  visitChildren(x);
  if (x.title().isEmpty()) return;
  DBG_RTF("{\\comment RTFDocVisitor::visitPost(DocXRefItem)}\n");
  m_t << "\\par\n";
  decIndentLevel();
  m_t << "}\n"; // end xref item
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocInternalRef &ref)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocInternalRef &)}\n");
  startLink("",ref.file(),ref.anchor());
  visitChildren(ref);
  endLink("");
  m_t << " ";
}

void RTFDocVisitor::operator()(const DocText &t)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocText &)}\n");
  visitChildren(t);
}

void RTFDocVisitor::operator()(const DocHtmlBlockQuote &q)
{
  if (m_hide) return;
  DBG_RTF("{\\comment RTFDocVisitor::operator()(const DocHtmlBlockQuote &)}\n");
  if (!m_lastIsPara) m_t << "\\par\n";
  m_t << "{"; // start desc
  incIndentLevel();
  m_t << rtf_Style_Reset << getStyle("DescContinue");
  visitChildren(q);
  if (!m_lastIsPara) m_t << "\\par\n";
  decIndentLevel();
  m_t << "}"; // end desc
  m_lastIsPara=TRUE;
}

void RTFDocVisitor::operator()(const DocVhdlFlow &)
{
}

void RTFDocVisitor::operator()(const DocParBlock &pb)
{
  if (m_hide) return;
  visitChildren(pb);
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
    const char *p=str.data();
    while (*p)
    {
      char c=*p++;
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
        default:   m_t << c;
      }
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

void RTFDocVisitor::writeDotFile(const DocDotFile &df)
{
  writeDotFile(df.file(), df.hasCaption(), df.srcFile(), df.srcLine());
}
void RTFDocVisitor::writeDotFile(const QCString &filename, bool hasCaption,
                                 const QCString &srcFile, int srcLine)
{
  QCString baseName=makeBaseName(filename);
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeDotGraphFromFile(filename,outDir,baseName,GraphOutputFormat::BITMAP,srcFile,srcLine);
  QCString imgExt = getDotImageExtension();
  includePicturePreRTF(baseName + "." + imgExt, true, hasCaption);
}

void RTFDocVisitor::writeMscFile(const DocMscFile &df)
{
  writeMscFile(df.file(), df.hasCaption(), df.srcFile(), df.srcLine());
}
void RTFDocVisitor::writeMscFile(const QCString &fileName, bool hasCaption,
                                 const QCString &srcFile, int srcLine)
{
  QCString baseName=makeBaseName(fileName);
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MscOutputFormat::BITMAP,srcFile,srcLine);
  includePicturePreRTF(baseName + ".png", true, hasCaption);
}

void RTFDocVisitor::writeDiaFile(const DocDiaFile &df)
{
  QCString baseName=makeBaseName(df.file());
  QCString outDir = Config_getString(RTF_OUTPUT);
  writeDiaGraphFromFile(df.file(),outDir,baseName,DiaOutputFormat::BITMAP,df.srcFile(),df.srcLine());
  includePicturePreRTF(baseName + ".png", true, df.hasCaption());
}

void RTFDocVisitor::writePlantUMLFile(const QCString &fileName, bool hasCaption)
{
  QCString baseName=makeBaseName(fileName);
  QCString outDir = Config_getString(RTF_OUTPUT);
  PlantumlManager::instance().generatePlantUMLOutput(fileName,outDir,PlantumlManager::PUML_BITMAP);
  includePicturePreRTF(baseName + ".png", true, hasCaption);
}
