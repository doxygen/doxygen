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

#include <qfileinfo.h>

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

void DocbookDocVisitor::visitCaption(const QList<DocNode> &children)
{
  QListIterator<DocNode> cli(children);
  DocNode *n;
  for (cli.toFirst();(n=cli.current());++cli) n->accept(this);
}

void DocbookDocVisitor::visitPreStart(FTextStream &t,
                   const QList<DocNode> &children,
                   bool hasCaption,
                   const QCString &name,
                   const QCString &width,
                   const QCString &height,
                   bool inlineImage)
{
  if (hasCaption && !inlineImage)
  {
    t << "    <figure>" << endl;
    t << "        <title>" << endl;
    visitCaption(children);
    t << "        </title>" << endl;
  }
  else
  {
    t << "    <informalfigure>" << endl;
  }
  t << "        <mediaobject>" << endl;
  t << "            <imageobject>" << endl;
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
  t << "</imagedata>" << endl;
  t << "            </imageobject>" << endl;
  if (hasCaption && !inlineImage)
  {
    t << "        <!--" << endl; // Needed for general formatting with title for other formats
  }
}

void DocbookDocVisitor::visitPostEnd(FTextStream &t, bool hasCaption, bool inlineImage)
{
  t << endl;
  if (hasCaption && !inlineImage)
  {
    t << "        -->" << endl; // Needed for general formatting with title for other formats
  }
  t << "        </mediaobject>" << endl;
  if (hasCaption && !inlineImage)
  {
    t << "    </figure>" << endl;
  }
  else
  {
    t << "    </informalfigure>" << endl;
  }
}

DocbookDocVisitor::DocbookDocVisitor(FTextStream &t,CodeOutputInterface &ci)
  : DocVisitor(DocVisitor_Docbook), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE)
{
DB_VIS_C
  // m_t << "<section>" << endl;
}
DocbookDocVisitor::~DocbookDocVisitor()
{
DB_VIS_C
  // m_t << "</section>" << endl;
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
  m_t << endl << "<literallayout>&#160;&#xa;</literallayout>" << endl;
  // gives nicer results but gives problems as it is not allowed in <pare> and also problems with dblatex
  // m_t << endl << "<sbr/>" << endl;
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
    case DocStyleChange::Strike:  break;
    case DocStyleChange::Underline:  break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void DocbookDocVisitor::visit(DocVerbatim *s)
{
DB_VIS_C
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(m_langExt);
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "<literallayout><computeroutput>";
      Doxygen::parserManager->getParser(m_langExt)
        ->parseCode(m_ci,s->context(),s->text(),langExt,
            s->isExample(),s->exampleFile());
      m_t << "</computeroutput></literallayout>";
      break;
    case DocVerbatim::Verbatim:
      m_t << "<literallayout><computeroutput>";
      filter(s->text());
      m_t << "</computeroutput></literallayout>";
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
        m_t << "<para>" << endl;
        name.sprintf("%s%d", "dot_inline_dotgraph_", dotindex);
        baseName.sprintf("%s%d",
            (Config_getString(DOCBOOK_OUTPUT)+"/inline_dotgraph_").data(),
            dotindex++
            );
        QFile file(baseName+".dot");
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s.msc for writing\n",baseName.data());
        }
        file.writeBlock( stext, stext.length() );
        file.close();
        writeDotFile(baseName, s);
        m_t << "</para>" << endl;
      }
      break;
    case DocVerbatim::Msc:
      {
        static int mscindex = 1;
        QCString baseName(4096);
        QCString name;
        QCString stext = s->text();
        m_t << "<para>" << endl;
        name.sprintf("%s%d", "msc_inline_mscgraph_", mscindex);
        baseName.sprintf("%s%d",
            (Config_getString(DOCBOOK_OUTPUT)+"/inline_mscgraph_").data(),
            mscindex++
            );
        QFile file(baseName+".msc");
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s.msc for writing\n",baseName.data());
        }
        QCString text = "msc {";
        text+=stext;
        text+="}";
        file.writeBlock( text, text.length() );
        file.close();
        writeMscFile(baseName,s);
        m_t << "</para>" << endl;
      }
      break;
    case DocVerbatim::PlantUML:
      {
        static QCString docbookOutput = Config_getString(DOCBOOK_OUTPUT);
        QCString baseName = PlantumlManager::instance()->writePlantUMLSource(docbookOutput,s->exampleFile(),s->text(),PlantumlManager::PUML_BITMAP);
        QCString shortName = baseName;
        int i;
        if ((i=shortName.findRev('/'))!=-1)
        {
          shortName=shortName.right(shortName.length()-i-1);
        }
        m_t << "<para>" << endl;
        writePlantUMLFile(baseName,s);
        m_t << "</para>" << endl;
      }
      break;
  }
}

void DocbookDocVisitor::visit(DocAnchor *anc)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<anchor xml:id=\"_" <<  stripPath(anc->file()) << "_1" << anc->anchor() << "\"/>";
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
        QFileInfo cfi( inc->file() );
        FileDef *fd = createFileDef( cfi.dirPath().utf8(), cfi.fileName().utf8() );
        Doxygen::parserManager->getParser(inc->extension())
          ->parseCode(m_ci,inc->context(),
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
      Doxygen::parserManager->getParser(inc->extension())
        ->parseCode(m_ci,inc->context(),
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
      break;
    case DocInclude::VerbInclude:
      m_t << "<literallayout>";
      filter(inc->text());
      m_t << "</literallayout>";
      break;
    case DocInclude::Snippet:
      m_t << "<literallayout><computeroutput>";
      Doxygen::parserManager->getParser(inc->extension())
        ->parseCode(m_ci,
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
         QFileInfo cfi( inc->file() );
         FileDef *fd = createFileDef( cfi.dirPath().utf8(), cfi.fileName().utf8() );
         m_t << "<literallayout><computeroutput>";
         Doxygen::parserManager->getParser(inc->extension())
                               ->parseCode(m_ci,
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
      m_t << "<programlisting>";
    }
    pushEnabled();
    m_hide = TRUE;
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

      Doxygen::parserManager->getParser(locLangExt)
        ->parseCode(m_ci,op->context(),
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
    pushEnabled();
    m_hide=TRUE;
  }
  if (op->isLast())
  {
    popEnabled();
    if (!m_hide) m_t << "</programlisting>";
  }
  else
  {
    if (!m_hide) m_t << endl;
  }
}

void DocbookDocVisitor::visit(DocFormula *f)
{
DB_VIS_C
  if (m_hide) return;

  if (f->isInline()) m_t  << "<inlinemediaobject>" << endl;
  else m_t << "        <mediaobject>" << endl;
  m_t << "            <imageobject>" << endl;
  m_t << "                <imagedata ";
  m_t << "align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << f->relPath() << f->name() << ".png\"/>" << endl;
  m_t << "            </imageobject>" << endl;
  if (f->isInline()) m_t  << "</inlinemediaobject>" << endl;
  else m_t << "        </mediaobject>" << endl;
}

void DocbookDocVisitor::visit(DocIndexEntry *ie)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<indexterm><primary>";
  filter(ie->entry());
  m_t << "</primary></indexterm>" << endl;
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
  if (!cite->file().isEmpty()) startLink(cite->file(),cite->anchor());
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
  m_t << endl;
  m_t << "<para>";
}

void DocbookDocVisitor::visitPost(DocPara *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</para>";
  m_t << endl;
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
        m_t << "<formalpara><title>" << theTranslator->trSeeAlso() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSeeAlso()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Return:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trReturns()<< "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trReturns()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Author:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, TRUE) << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, TRUE)) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Authors:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, FALSE) << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trAuthor(TRUE, FALSE)) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Version:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trVersion() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trVersion()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Since:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trSince() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trSince()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Date:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trDate() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trDate()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Note:
      if (m_insidePre) 
      {
        m_t << "<note><title>" << theTranslator->trNote() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<note><title>" << convertToDocBook(theTranslator->trNote()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Warning:
      if (m_insidePre) 
      {
        m_t << "<warning><title>" << theTranslator->trWarning() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<warning><title>" << convertToDocBook(theTranslator->trWarning()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Pre:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trPrecondition() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPrecondition()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Post:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trPostcondition() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trPostcondition()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Copyright:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trCopyright() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trCopyright()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Invar:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trInvariant() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trInvariant()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Remark:
      // <remark> is miising the <title> possibility
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trRemarks() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToDocBook(theTranslator->trRemarks()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::Attention:
      if (m_insidePre) 
      {
        m_t << "<caution><title>" << theTranslator->trAttention() << "</title>" << endl;
      } 
      else 
      {
        m_t << "<caution><title>" << convertToDocBook(theTranslator->trAttention()) << "</title>" << endl;
      }
      break;
    case DocSimpleSect::User:
    case DocSimpleSect::Rcs:
    case DocSimpleSect::Unknown:
      if (s->hasTitle())
        m_t << "<formalpara>" << endl;
      else
        m_t << "<para>" << endl;
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
        m_t << "</formalpara>" << endl;
      else
        m_t << "</para>" << endl;
      break;
    case DocSimpleSect::Note:
      m_t << "</note>" << endl;
      break;
    case DocSimpleSect::Attention:
      m_t << "</caution>" << endl;
      break;
    case DocSimpleSect::Warning:
      m_t << "</warning>" << endl;
      break;
    default:
      m_t << "</formalpara>" << endl;
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
  m_t << "\">" << endl;
  m_t << "<title>";
  filter(s->title());
  m_t << "</title>" << endl;
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
  if (s->type()==DocHtmlList::Ordered)
    m_t << "<orderedlist>\n";
  else
    m_t << "<itemizedlist>\n";
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

void DocbookDocVisitor::visitPre(DocHtmlListItem *)
{
DB_VIS_C
  if (m_hide) return;
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

static int colCnt = 0;
static bool bodySet = FALSE; // it is possible to have tables without a header
void DocbookDocVisitor::visitPre(DocHtmlTable *t)
{
DB_VIS_C
  bodySet = FALSE;
  if (m_hide) return;
  m_t << "<informaltable frame=\"all\">" << endl;
  m_t << "    <tgroup cols=\"" << t->numColumns() << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
  for (uint i = 0; i <t->numColumns(); i++)
  {
    // do something with colwidth based of cell width specification (be aware of possible colspan in the header)?
    m_t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
}

void DocbookDocVisitor::visitPost(DocHtmlTable *)
{
DB_VIS_C
  if (m_hide) return;
  if (bodySet) m_t << "    </tbody>" << endl;
  bodySet = FALSE;
  m_t << "    </tgroup>" << endl;
  m_t << "</informaltable>" << endl;
}

void DocbookDocVisitor::visitPre(DocHtmlRow *tr)
{
DB_VIS_C
  colCnt = 0;
  if (m_hide) return;

  if (tr->isHeading()) m_t << "<thead>\n";
  else if (!bodySet)
  {
    bodySet = TRUE;
    m_t << "<tbody>\n";
  }

  m_t << "      <row ";

  HtmlAttribListIterator li(tr->attribs());
  HtmlAttrib *opt;
  for (li.toFirst();(opt=li.current());++li)
  {
    if (opt->name=="class")
    {
      // just skip it
    }
    else if (opt->name=="style")
    {
      // just skip it
    }
    else if (opt->name=="height")
    {
      // just skip it
    }
    else if (opt->name=="filter")
    {
      // just skip it
    }
    else
    {
      m_t << " " << opt->name << "='" << opt->value << "'";
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
    bodySet = TRUE;
    m_t << "</thead><tbody>\n";
  }
}

void DocbookDocVisitor::visitPre(DocHtmlCell *c)
{
DB_VIS_C
  colCnt++;
  if (m_hide) return;
  m_t << "<entry";

  HtmlAttribListIterator li(c->attribs());
  HtmlAttrib *opt;
  for (li.toFirst();(opt=li.current());++li)
  {
    if (opt->name=="colspan")
    {
      m_t << " namest='c" << colCnt << "'";
      int cols = opt->value.toInt();
      colCnt += (cols - 1);
      m_t << " nameend='c" << colCnt << "'";
    }
    else if (opt->name=="rowspan")
    {
      int extraRows = opt->value.toInt() - 1;
      m_t << " morerows='" << extraRows << "'";
    }
    else if (opt->name=="class")
    {
      if (opt->value == "markdownTableBodyRight")
      {
        m_t << " align='right'";
      }
      else if (opt->value == "markdownTableBodyLeftt")
      {
        m_t << " align='left'";
      }
      else if (opt->value == "markdownTableBodyCenter")
      {
        m_t << " align='center'";
      }
      else if (opt->value == "markdownTableHeadRight")
      {
        m_t << " align='right'";
      }
      else if (opt->value == "markdownTableHeadLeftt")
      {
        m_t << " align='left'";
      }
      else if (opt->value == "markdownTableHeadCenter")
      {
        m_t << " align='center'";
      }
    }
    else if (opt->name=="style")
    {
      // just skip it
    }
    else if (opt->name=="width")
    {
      // just skip it
    }
    else if (opt->name=="height")
    {
      // just skip it
    }
    else
    {
      m_t << " " << opt->name << "='" << opt->value << "'";
    }
  }
  m_t << ">";
}

void DocbookDocVisitor::visitPost(DocHtmlCell *c)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</entry>";
}

void DocbookDocVisitor::visitPre(DocHtmlCaption *c)
{
DB_VIS_C
  if (m_hide) return;
   m_t << "<caption>";
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
  m_t << "<link xlink:href=\"" << convertToDocBook(href->url()) << "\">";
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
    m_t << endl;
    QCString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right(baseName.length()-i-1);
    }
    visitPreStart(m_t, img->children(), img->hasCaption(), img->relPath() + baseName, img->width(), img->height(), img->isInlineImage());
  }
  else
  {
    pushEnabled();
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
      baseName=baseName.right(baseName.length()-i-1);
    }
    QCString m_file;
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::imageNameDict, baseName, ambig);
    if (fd) 
    {
      m_file=fd->absFilePath();
    }
    QFile inImage(m_file);
    QFile outImage(Config_getString(DOCBOOK_OUTPUT)+"/"+baseName.data());
    if (inImage.open(IO_ReadOnly))
    {
      if (outImage.open(IO_WriteOnly))
      {
        char *buffer = new char[inImage.size()];
        inImage.readBlock(buffer,inImage.size());
        outImage.writeBlock(buffer,inImage.size());
        outImage.flush();
        delete[] buffer;
      }
    }
  } 
  else 
  {
    popEnabled();
  }
}

void DocbookDocVisitor::visitPre(DocDotFile *df)
{
DB_VIS_C
  if (m_hide) return;
  startDotFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children());
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
  startMscFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children());
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
  startDiaFile(df->file(),df->width(),df->height(),df->hasCaption(),df->children());
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
    startLink(0,ref->anchor());
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

void DocbookDocVisitor::visitPre(DocSecRefItem *ref)
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
  m_t << "</tocentry>" << endl;
}

void DocbookDocVisitor::visitPre(DocSecRefList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "<toc>" << endl;
}

void DocbookDocVisitor::visitPost(DocSecRefList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</toc>" << endl;
}

void DocbookDocVisitor::visitPre(DocParamSect *s)
{
DB_VIS_C
  if (m_hide) return;
  m_t <<  endl;
  m_t << "                <formalpara>" << endl;
  m_t << "                    <title>" << endl;
  switch(s->type())
  {
    case DocParamSect::Param:         m_t << theTranslator->trParameters();         break;
    case DocParamSect::RetVal:        m_t << theTranslator->trReturnValues();       break;
    case DocParamSect::Exception:     m_t << theTranslator->trExceptions();         break;
    case DocParamSect::TemplateParam: m_t << theTranslator->trTemplateParameters(); break;
    default:
      ASSERT(0);
  }
  m_t << "                    </title>" << endl;
  m_t << "                    <para>" << endl;
  m_t << "                    <table frame=\"all\">" << endl;
  int ncols = 2;
  if (s->type() == DocParamSect::Param)
  {
    bool hasInOutSpecs = s->hasInOutSpecifier();
    bool hasTypeSpecs  = s->hasTypeSpecifier();
    if      (hasInOutSpecs && hasTypeSpecs) ncols += 2;
    else if (hasInOutSpecs || hasTypeSpecs) ncols += 1;
  }
  m_t << "                        <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
  for (int i = 1; i <= ncols; i++)
  {
    if (i == ncols) m_t << "                        <colspec colwidth=\"4*\"/>" << endl;
    else            m_t << "                        <colspec colwidth=\"1*\"/>" << endl;
  }
  m_t << "                        <tbody>" << endl;
}

void DocbookDocVisitor::visitPost(DocParamSect *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "                        </tbody>" << endl;
  m_t << "                        </tgroup>" << endl;
  m_t << "                    </table>" << endl;
  m_t << "                    </para>" << endl;
  m_t << "                </formalpara>" << endl;
  m_t << "                ";
}

void DocbookDocVisitor::visitPre(DocParamList *pl)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "                            <row>" << endl;

  DocParamSect::Type parentType = DocParamSect::Unknown;
  DocParamSect *sect = 0;
  if (pl->parent() && pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    parentType = ((DocParamSect*)pl->parent())->type();
    sect=(DocParamSect*)pl->parent();
  }

  if (sect && sect->hasInOutSpecifier())
  {
    m_t << "                                <entry>";
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
    m_t << "                                </entry>";
  }

  if (sect && sect->hasTypeSpecifier())
  {
    QListIterator<DocNode> li(pl->paramTypes());
    DocNode *type;
    bool first=TRUE;
    m_t << "                                <entry>";
    for (li.toFirst();(type=li.current());++li)
    {
      if (!first) m_t << " | "; else first=FALSE;
      if (type->kind()==DocNode::Kind_Word)
      {
        visit((DocWord*)type);
      }
      else if (type->kind()==DocNode::Kind_LinkedWord)
      {
        visit((DocLinkedWord*)type);
      }
    }
    m_t << "                                </entry>";
  }

  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  if (!li.toFirst())
  {
    m_t << "                                <entry></entry>" << endl;
  }
  else
  {
    m_t << "                                <entry>";
    int cnt = 0;
    for (li.toFirst();(param=li.current());++li)
    {
      if (cnt)
      {
        m_t << ", ";
      }
      if (param->kind()==DocNode::Kind_Word)
      {
        visit((DocWord*)param);
      }
      else if (param->kind()==DocNode::Kind_LinkedWord)
      {
        visit((DocLinkedWord*)param);
      }
      cnt++;
    }
    m_t << "</entry>" << endl;
  }
  m_t << "                                <entry>";
}

void DocbookDocVisitor::visitPost(DocParamList *)
{
DB_VIS_C
  if (m_hide) return;
  m_t << "</entry>" << endl;
  m_t << "                            </row>" << endl;
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


void DocbookDocVisitor::filter(const char *str)
{
DB_VIS_C
  m_t << convertToDocBook(str);
}

void DocbookDocVisitor::startLink(const QCString &file,const QCString &anchor)
{
DB_VIS_C
  m_t << "<link linkend=\"_" << stripPath(file);
  if (!anchor.isEmpty())
  {
    if (file) m_t << "_1";
    m_t << anchor;
  }
  m_t << "\">";
}

void DocbookDocVisitor::endLink()
{
DB_VIS_C
  m_t << "</link>";
}

void DocbookDocVisitor::pushEnabled()
{
DB_VIS_C
  m_enabled.push(new bool(m_hide));
}

void DocbookDocVisitor::popEnabled()
{
DB_VIS_C
  bool *v=m_enabled.pop();
  ASSERT(v!=0);
  m_hide = *v;
  delete v;
}

void DocbookDocVisitor::writeMscFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MSC_BITMAP);
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
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  PlantumlManager::instance()->generatePlantUMLOutput(baseName,outDir,PlantumlManager::PUML_BITMAP);
  visitPreStart(m_t, s->children(), s->hasCaption(), s->relPath() + shortName + ".png", s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startMscFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const QList<DocNode> &children
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("msc_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_BITMAP);
  m_t << "<para>" << endl;
  visitPreStart(m_t, children, hasCaption, baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endMscFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>" << endl;
}

void DocbookDocVisitor::writeDiaFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDiaGraphFromFile(baseName+".dia",outDir,shortName,DIA_BITMAP);
  visitPreStart(m_t, s->children(), s->hasCaption(), shortName, s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startDiaFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const QList<DocNode> &children
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dia_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDiaGraphFromFile(fileName,outDir,baseName,DIA_BITMAP);
  m_t << "<para>" << endl;
  visitPreStart(m_t, children, hasCaption, baseName + ".png",  width,  height);
}

void DocbookDocVisitor::endDiaFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>" << endl;
}

void DocbookDocVisitor::writeDotFile(const QCString &baseName, DocVerbatim *s)
{
DB_VIS_C
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  writeDotGraphFromFile(baseName+".dot",outDir,shortName,GOF_BITMAP);
  visitPreStart(m_t, s->children(), s->hasCaption(), s->relPath() + shortName + "." + getDotImageExtension(), s->width(),s->height());
  visitCaption(s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void DocbookDocVisitor::startDotFile(const QCString &fileName,
    const QCString &width,
    const QCString &height,
    bool hasCaption,
    const QList<DocNode> &children
    )
{
DB_VIS_C
  QCString baseName=fileName;
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dot_");
  QCString outDir = Config_getString(DOCBOOK_OUTPUT);
  QCString imgExt = getDotImageExtension();
  writeDotGraphFromFile(fileName,outDir,baseName,GOF_BITMAP);
  m_t << "<para>" << endl;
  visitPreStart(m_t, children, hasCaption, baseName + "." + imgExt,  width,  height);
}

void DocbookDocVisitor::endDotFile(bool hasCaption)
{
DB_VIS_C
  if (m_hide) return;
  m_t << endl;
  visitPostEnd(m_t, hasCaption);
  m_t << "</para>" << endl;
}

