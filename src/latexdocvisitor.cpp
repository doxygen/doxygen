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
#include "htmlattrib.h"
#include <qfileinfo.h> 
#include "latexdocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "dot.h"
#include "util.h"
#include "message.h"
#include "parserintf.h"
#include "msc.h"
#include "dia.h"
#include "cite.h"
#include "filedef.h"
#include "config.h"
#include "htmlentity.h"
#include "plantuml.h"

static QCString escapeLabelName(const char *s)
{
  QCString result;
  const char *p=s;
  char c;
  if (p)
  {
    while ((c=*p++))
    {
      switch (c)
      {
        case '%': result+="\\%"; break;
        case '|': result+="\\texttt{\"|}"; break;
        case '!': result+="\"!"; break;
        case '{': result+="\\lcurly{}"; break;
        case '}': result+="\\rcurly{}"; break;
        case '~': result+="````~"; break; // to get it a bit better in index together with other special characters
        default: result+=c;
      }
    }
  }
  return result;
}

const int maxLevels=5;
static const char *secLabels[maxLevels] = 
   { "section","subsection","subsubsection","paragraph","subparagraph" };

static const char *getSectionName(int level)
{
  static bool compactLatex = Config_getBool(COMPACT_LATEX);
  int l = level;
  if (compactLatex) l++;
  if (Doxygen::insideMainPage) l--;
  return secLabels[QMIN(maxLevels-1,l)];
}

static void visitPreStart(FTextStream &t, const bool hasCaption, QCString name,  QCString width,  QCString height)
{
    if (hasCaption)
    {
      t << "\n\\begin{DoxyImage}\n";
    }
    else
    {
      t << "\n\\begin{DoxyImageNoCaption}\n"
             "  \\mbox{";
    }

    t << "\\includegraphics";
    if (!width.isEmpty() || !height.isEmpty())
    {
      t << "[";
    }
    if (!width.isEmpty())
    {
      t << "width=" << width;
    }
    if (!width.isEmpty() && !height.isEmpty())
    {
      t << ",";
    }
    if (!height.isEmpty())
    {
      t << "height=" << height;
    }
    if (width.isEmpty() && height.isEmpty())
    {
      /* default setting */
      t << "[width=\\textwidth,height=\\textheight/2,keepaspectratio=true]";
    }
    else
    {
      t << "]";
    }

    t << "{" << name << "}";

    if (hasCaption)
    {
      t << "\n\\doxyfigcaption{";
    }
}



static void visitPostEnd(FTextStream &t, const bool hasCaption)
{
    t << "}\n"; // end mbox or caption
    if (hasCaption)
    {
      t << "\\end{DoxyImage}\n";
    }
    else{
      t << "\\end{DoxyImageNoCaption}\n";
    }
}


static void visitCaption(LatexDocVisitor *parent, QList<DocNode> children)
{
  QListIterator<DocNode> cli(children);
  DocNode *n;
  for (cli.toFirst();(n=cli.current());++cli) n->accept(parent);
}

QCString LatexDocVisitor::escapeMakeIndexChars(const char *s)
{
  QCString result;
  const char *p=s;
  char str[2]; str[1]=0;
  char c;
  if (p)
  {
    while ((c=*p++))
    {
      switch (c)
      {
        case '!': m_t << "\"!"; break;
        case '"': m_t << "\"\""; break;
        case '@': m_t << "\"@"; break;
        case '|': m_t << "\\texttt{\"|}"; break;
        case '[': m_t << "["; break;
        case ']': m_t << "]"; break;
        case '{': m_t << "\\lcurly{}"; break;
        case '}': m_t << "\\rcurly{}"; break;
        default:  str[0]=c; filter(str); break;
      }
    }
  }
  return result;
}


LatexDocVisitor::LatexDocVisitor(FTextStream &t,CodeOutputInterface &ci,
                                 const char *langExt,bool insideTabbing) 
  : DocVisitor(DocVisitor_Latex), m_t(t), m_ci(ci), m_insidePre(FALSE), 
    m_insideItem(FALSE), m_hide(FALSE), m_hideCaption(FALSE), m_insideTabbing(insideTabbing),
    m_langExt(langExt)
{
  m_tableStateStack.setAutoDelete(TRUE);
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void LatexDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void LatexDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  startLink(w->ref(),w->file(),w->anchor());
  filter(w->word());
  endLink(w->ref(),w->file(),w->anchor());
}

void LatexDocVisitor::visit(DocWhiteSpace *w)
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

void LatexDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  const char *res = HtmlEntityMapper::instance()->latex(s->symbol());
  if (res)
  {
    if (((s->symbol() == DocSymbol::Sym_lt) || (s->symbol() == DocSymbol::Sym_Less))&& (!m_insidePre))
    {
      m_t << "$<$";
    }
    else if (((s->symbol() == DocSymbol::Sym_gt) || (s->symbol() == DocSymbol::Sym_Greater)) && (!m_insidePre))
    {
      m_t << "$>$";
    }
    else
    {
      m_t << res;
    }
  }
  else
  {
    err("LaTeX: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
}

void LatexDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  if (Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\href{";
    if (u->isEmail()) m_t << "mailto:";
    m_t << u->url() << "}";
  }
  m_t << "{\\tt ";
  filter(u->url());
  m_t << "}";
}

void LatexDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "~\\newline\n";
}

void LatexDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "\n\n";
}

void LatexDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "{\\bfseries ";      else m_t << "}";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "{\\itshape ";     else m_t << "}";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "{\\ttfamily ";   else m_t << "}";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "\\textsubscript{";    else m_t << "}";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "\\textsuperscript{";    else m_t << "}";
      break;
    case DocStyleChange::Center:
      if (s->enable()) m_t << "\\begin{center}"; else m_t << "\\end{center} ";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "\n\\footnotesize ";  else m_t << "\n\\normalsize ";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable()) 
      {
        m_t << "\n\\begin{DoxyPre}";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "\\end{DoxyPre}\n";
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void LatexDocVisitor::visit(DocVerbatim *s)
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
      {
        m_t << "\n\\begin{DoxyCode}\n";
        Doxygen::parserManager->getParser(lang)
                              ->parseCode(m_ci,s->context(),s->text(),langExt,
                                          s->isExample(),s->exampleFile());
        m_t << "\\end{DoxyCode}\n";
      }
      break;
    case DocVerbatim::Verbatim: 
      m_t << "\\begin{DoxyVerb}";
      m_t << s->text();
      m_t << "\\end{DoxyVerb}\n";
      break;
    case DocVerbatim::HtmlOnly: 
    case DocVerbatim::XmlOnly: 
    case DocVerbatim::ManOnly: 
    case DocVerbatim::RtfOnly:
    case DocVerbatim::DocbookOnly:
      /* nothing */ 
      break;
    case DocVerbatim::LatexOnly: 
      m_t << s->text(); 
      break;
    case DocVerbatim::Dot: 
      {
        static int dotindex = 1;
        QCString fileName(4096);

        fileName.sprintf("%s%d%s", 
            (Config_getString(LATEX_OUTPUT)+"/inline_dotgraph_").data(), 
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

          startDotFile(fileName,s->width(),s->height(),s->hasCaption());
          visitCaption(this, s->children());
          endDotFile(s->hasCaption());

          if (Config_getBool(DOT_CLEANUP)) file.remove();
        }
      }
      break;
    case DocVerbatim::Msc: 
      {
        static int mscindex = 1;
        QCString baseName(4096);

        baseName.sprintf("%s%d", 
            (Config_getString(LATEX_OUTPUT)+"/inline_mscgraph_").data(), 
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

          writeMscFile(baseName, s);

          if (Config_getBool(DOT_CLEANUP)) file.remove();
        }
      }
      break;
    case DocVerbatim::PlantUML: 
      {
        QCString latexOutput = Config_getString(LATEX_OUTPUT);
        QCString baseName = writePlantUMLSource(latexOutput,s->exampleFile(),s->text());

        writePlantUMLFile(baseName, s);
      }
      break;
  }
}

void LatexDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "\\label{" << stripPath(anc->file()) << "_" << anc->anchor() << "}%" << endl;
  if (!anc->file().isEmpty() && Config_getBool(PDF_HYPERLINKS)) 
  {
    m_t << "\\hypertarget{" << stripPath(anc->file()) << "_" << anc->anchor() 
      << "}{}%" << endl;
  }    
}

void LatexDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  SrcLangExt langExt = getLanguageFromFileName(inc->extension());
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      { 
         m_t << "\n\\begin{DoxyCodeInclude}\n";
         QFileInfo cfi( inc->file() );
         FileDef fd( cfi.dirPath().utf8(), cfi.fileName().utf8() );
         Doxygen::parserManager->getParser(inc->extension())
                               ->parseCode(m_ci,inc->context(),
                                           inc->text(),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile(), &fd);
         m_t << "\\end{DoxyCodeInclude}" << endl;
      }
      break;    
    case DocInclude::Include: 
      m_t << "\n\\begin{DoxyCodeInclude}\n";
      Doxygen::parserManager->getParser(inc->extension())
                            ->parseCode(m_ci,inc->context(),
                                        inc->text(),langExt,inc->isExample(),
                                        inc->exampleFile());
      m_t << "\\end{DoxyCodeInclude}\n";
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      break;
    case DocInclude::LatexInclude:
      m_t << inc->text();
      break;
    case DocInclude::VerbInclude: 
      m_t << "\n\\begin{DoxyVerbInclude}\n";
      m_t << inc->text();
      m_t << "\\end{DoxyVerbInclude}\n";
      break;
    case DocInclude::Snippet:
      {
         m_t << "\n\\begin{DoxyCodeInclude}\n";
         Doxygen::parserManager->getParser(inc->extension())
                               ->parseCode(m_ci,
                                           inc->context(),
                                           extractBlock(inc->text(),inc->blockId()),
                                           langExt,
                                           inc->isExample(),
                                           inc->exampleFile()
                                          );
         m_t << "\\end{DoxyCodeInclude}" << endl;
      }
      break;
  }
}

void LatexDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    if (!m_hide) m_t << "\n\\begin{DoxyCodeInclude}\n";
    pushEnabled();
    m_hide = TRUE;
  }
  SrcLangExt langExt = getLanguageFromFileName(m_langExt);
  if (op->type()!=DocIncOperator::Skip) 
  {
    popEnabled();
    if (!m_hide) 
    {
      Doxygen::parserManager->getParser(m_langExt)
                            ->parseCode(m_ci,op->context(),op->text(),langExt,
                                        op->isExample(),op->exampleFile());
    }
    pushEnabled();
    m_hide=TRUE;
  }
  if (op->isLast())  
  {
    popEnabled();
    if (!m_hide) m_t << "\n\\end{DoxyCodeInclude}\n";
  }
  else
  {
    if (!m_hide) m_t << endl;
  }
}

void LatexDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << f->text();
}

void LatexDocVisitor::visit(DocIndexEntry *i)
{
  if (m_hide) return;
  m_t << "\\index{" << escapeLabelName(i->entry()) << "@{";
  escapeMakeIndexChars(i->entry());
  m_t << "}}";
}

void LatexDocVisitor::visit(DocSimpleSectSep *)
{
}

void LatexDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  if (!cite->file().isEmpty()) 
  {
    //startLink(cite->ref(),cite->file(),cite->anchor());
    QCString anchor = cite->anchor();
    anchor = anchor.mid(CiteConsts::anchorPrefix.length()); // strip prefix
    m_t << "\\cite{" << anchor << "}";
  }
  else
  {
    m_t << "{\\bfseries [";
    filter(cite->text());
    m_t << "]}";
  }
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void LatexDocVisitor::visitPre(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "\n\\begin{DoxyEnumerate}";
  }
  else
  {
    m_t << "\n\\begin{DoxyItemize}";
  }
}

void LatexDocVisitor::visitPost(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "\n\\end{DoxyEnumerate}";
  }
  else
  {
    m_t << "\n\\end{DoxyItemize}";
  }
}

void LatexDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "\n\\item ";
}

void LatexDocVisitor::visitPost(DocAutoListItem *) 
{
}

void LatexDocVisitor::visitPre(DocPara *) 
{
}

void LatexDocVisitor::visitPost(DocPara *p)
{
  if (m_hide) return;
  if (!p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameter sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       )
     ) m_t << endl << endl;
}

void LatexDocVisitor::visitPre(DocRoot *)
{
}

void LatexDocVisitor::visitPost(DocRoot *)
{
}

void LatexDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocSimpleSect::See:
      m_t << "\\begin{DoxySeeAlso}{";
      filter(theTranslator->trSeeAlso());
      break;
    case DocSimpleSect::Return:
      m_t << "\\begin{DoxyReturn}{";
      filter(theTranslator->trReturns());
      break;
    case DocSimpleSect::Author:
      m_t << "\\begin{DoxyAuthor}{";
      filter(theTranslator->trAuthor(TRUE,TRUE));
      break;
    case DocSimpleSect::Authors:
      m_t << "\\begin{DoxyAuthor}{";
      filter(theTranslator->trAuthor(TRUE,FALSE));
      break;
    case DocSimpleSect::Version:
      m_t << "\\begin{DoxyVersion}{";
      filter(theTranslator->trVersion());
      break;
    case DocSimpleSect::Since:
      m_t << "\\begin{DoxySince}{";
      filter(theTranslator->trSince());
      break;
    case DocSimpleSect::Date:
      m_t << "\\begin{DoxyDate}{";
      filter(theTranslator->trDate());
      break;
    case DocSimpleSect::Note:
      m_t << "\\begin{DoxyNote}{";
      filter(theTranslator->trNote());
      break;
    case DocSimpleSect::Warning:
      m_t << "\\begin{DoxyWarning}{";
      filter(theTranslator->trWarning());
      break;
    case DocSimpleSect::Pre:
      m_t << "\\begin{DoxyPrecond}{";
      filter(theTranslator->trPrecondition());
      break;
    case DocSimpleSect::Post:
      m_t << "\\begin{DoxyPostcond}{";
      filter(theTranslator->trPostcondition());
      break;
    case DocSimpleSect::Copyright:
      m_t << "\\begin{DoxyCopyright}{";
      filter(theTranslator->trCopyright());
      break;
    case DocSimpleSect::Invar:
      m_t << "\\begin{DoxyInvariant}{";
      filter(theTranslator->trInvariant());
      break;
    case DocSimpleSect::Remark:
      m_t << "\\begin{DoxyRemark}{";
      filter(theTranslator->trRemarks());
      break;
    case DocSimpleSect::Attention:
      m_t << "\\begin{DoxyAttention}{";
      filter(theTranslator->trAttention());
      break;
    case DocSimpleSect::User:
      m_t << "\\begin{DoxyParagraph}{";
      break;
    case DocSimpleSect::Rcs: 
      m_t << "\\begin{DoxyParagraph}{";
      break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s->type()!=DocSimpleSect::User && s->type()!=DocSimpleSect::Rcs)
  {
    m_t << "}\n";
  }
  else
  {
    m_insideItem=TRUE;
  }
}

void LatexDocVisitor::visitPost(DocSimpleSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocSimpleSect::See:
      m_t << "\n\\end{DoxySeeAlso}\n";
      break;
    case DocSimpleSect::Return:
      m_t << "\n\\end{DoxyReturn}\n";
      break;
    case DocSimpleSect::Author:
      m_t << "\n\\end{DoxyAuthor}\n";
      break;
    case DocSimpleSect::Authors:
      m_t << "\n\\end{DoxyAuthor}\n";
      break;
    case DocSimpleSect::Version:
      m_t << "\n\\end{DoxyVersion}\n";
      break;
    case DocSimpleSect::Since:
      m_t << "\n\\end{DoxySince}\n";
      break;
    case DocSimpleSect::Date:
      m_t << "\n\\end{DoxyDate}\n";
      break;
    case DocSimpleSect::Note:
      m_t << "\n\\end{DoxyNote}\n";
      break;
    case DocSimpleSect::Warning:
      m_t << "\n\\end{DoxyWarning}\n";
      break;
    case DocSimpleSect::Pre:
      m_t << "\n\\end{DoxyPrecond}\n";
      break;
    case DocSimpleSect::Post:
      m_t << "\n\\end{DoxyPostcond}\n";
      break;
    case DocSimpleSect::Copyright:
      m_t << "\n\\end{DoxyCopyright}\n";
      break;
    case DocSimpleSect::Invar:
      m_t << "\n\\end{DoxyInvariant}\n";
      break;
    case DocSimpleSect::Remark:
      m_t << "\n\\end{DoxyRemark}\n";
      break;
    case DocSimpleSect::Attention:
      m_t << "\n\\end{DoxyAttention}\n";
      break;
    case DocSimpleSect::User:
      m_t << "\n\\end{DoxyParagraph}\n";
      break;
    case DocSimpleSect::Rcs: 
      m_t << "\n\\end{DoxyParagraph}\n";
      break;
    default:
      break;
  }
}

void LatexDocVisitor::visitPre(DocTitle *)
{
}

void LatexDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_insideItem=FALSE;
  m_t << "}\n";
}

void LatexDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "\\begin{DoxyItemize}" << endl;
}

void LatexDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "\\end{DoxyItemize}" << endl;
}

void LatexDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "\\item ";
}

void LatexDocVisitor::visitPost(DocSimpleListItem *) 
{
}

void LatexDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  if (Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\hypertarget{" << stripPath(s->file()) << "_" << s->anchor() << "}{}";
  }
  m_t << "\\" << getSectionName(s->level()) << "{";
  filter(convertCharEntitiesToUTF8(s->title().data()));
  m_t << "}\\label{" << stripPath(s->file()) << "_" << s->anchor() << "}" << endl;
}

void LatexDocVisitor::visitPost(DocSection *) 
{
}

void LatexDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\n\\begin{DoxyEnumerate}";
  else 
    m_t << "\n\\begin{DoxyItemize}";
}

void LatexDocVisitor::visitPost(DocHtmlList *s) 
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\n\\end{DoxyEnumerate}";
  else 
    m_t << "\n\\end{DoxyItemize}";
}

void LatexDocVisitor::visitPre(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "\n\\item ";
}

void LatexDocVisitor::visitPost(DocHtmlListItem *) 
{
}

//void LatexDocVisitor::visitPre(DocHtmlPre *)
//{
//  m_t << "\\small\\begin{alltt}";
//  m_insidePre=TRUE;
//}

//void LatexDocVisitor::visitPost(DocHtmlPre *) 
//{
//  m_insidePre=FALSE;
//  m_t << "\\end{alltt}\\normalsize " << endl;
//}

void LatexDocVisitor::visitPre(DocHtmlDescList *dl)
{
  if (m_hide) return;
  QCString val = dl->attribs().find("class");
  if (val=="reflist")
  {
    m_t << "\n\\begin{DoxyRefList}";
  }
  else
  {
    m_t << "\n\\begin{DoxyDescription}";
  }
}

void LatexDocVisitor::visitPost(DocHtmlDescList *dl) 
{
  if (m_hide) return;
  QCString val = dl->attribs().find("class");
  if (val=="reflist")
  {
    m_t << "\n\\end{DoxyRefList}";
  }
  else
  {
    m_t << "\n\\end{DoxyDescription}";
  }
}

void LatexDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "\n\\item[";
  m_insideItem=TRUE;
}

void LatexDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  if (m_hide) return;
  m_insideItem=FALSE;
  m_t << "]";
}

void LatexDocVisitor::visitPre(DocHtmlDescData *)
{
}

void LatexDocVisitor::visitPost(DocHtmlDescData *) 
{
}

static bool tableIsNested(const DocNode *n)
{
  bool isNested=FALSE;
  while (n && !isNested)
  {
    isNested = n->kind()==DocNode::Kind_HtmlTable || n->kind()==DocNode::Kind_ParamSect;
    n  = n->parent();
  }
  return isNested;
}

static void writeStartTableCommand(FTextStream &t,const DocNode *n,int cols)
{
  if (tableIsNested(n))
  {
    t << "\\begin{tabularx}{\\linewidth}{|*{" << cols << "}{>{\\raggedright\\arraybackslash}X|}}";
  }
  else
  {
    t << "\\tabulinesep=1mm\n\\begin{longtabu} spread 0pt [c]{*" << cols << "{|X[-1]}|}\n";
  }
  //return isNested ? "TabularNC" : "TabularC";
}

static void writeEndTableCommand(FTextStream &t,const DocNode *n)
{
  if (tableIsNested(n))
  {
    t << "\\end{tabularx}\n";
  }
  else
  {
    t << "\\end{longtabu}\n";
  }
  //return isNested ? "TabularNC" : "TabularC";
}

void LatexDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;
  pushTableState();
  if (t->hasCaption())
  {
    DocHtmlCaption *c = t->caption();
    static bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
    if (!c->file().isEmpty() && pdfHyperLinks)
    {
      m_t << "\\hypertarget{" << stripPath(c->file()) << "_" << c->anchor()
        << "}{}";
    }
    m_t << endl;
  }

  writeStartTableCommand(m_t,t->parent(),t->numColumns());

  if (t->hasCaption())
  {
    DocHtmlCaption *c = t->caption();
    m_t << "\\caption{";
    visitCaption(this, c->children());
    m_t << "}";
    m_t << "\\label{" << stripPath(c->file()) << "_" << c->anchor() << "}";
    m_t << "\\\\\n";
  }

  setNumCols(t->numColumns());
  m_t << "\\hline\n";

  // check if first row is a heading and then render the row already here
  // and end it with \endfirsthead (triggered via m_firstRow==TRUE)
  // then repeat the row as normal and end it with \endhead (m_firstRow==FALSE)
  DocHtmlRow *firstRow = t->firstRow();
  if (firstRow && firstRow->isHeading())
  {
    setFirstRow(TRUE);
    firstRow->accept(this);
    setFirstRow(FALSE);
  }
}

void LatexDocVisitor::visitPost(DocHtmlTable *t)
{
  if (m_hide) return;
  writeEndTableCommand(m_t,t->parent());
  popTableState();
}

void LatexDocVisitor::visitPre(DocHtmlCaption *c)
{
  m_hideCaption = m_hide;
  m_hide        = TRUE;
}

void LatexDocVisitor::visitPost(DocHtmlCaption *c)
{
  m_hide        = m_hideCaption;
}

void LatexDocVisitor::visitPre(DocHtmlRow *r)
{
  setCurrentColumn(0);
  if (r->isHeading()) m_t << "\\rowcolor{\\tableheadbgcolor}";
}

void LatexDocVisitor::visitPost(DocHtmlRow *row) 
{
  if (m_hide) return;

  int c=currentColumn();
  while (c<=numCols()) // end of row while inside a row span?
  {
    uint i;
    for (i=0;i<rowSpans().count();i++)
    {
      ActiveRowSpan *span = rowSpans().at(i);
      //printf("  found row span: column=%d rs=%d cs=%d rowIdx=%d cell->rowIdx=%d i=%d c=%d\n",
      //    span->column, span->rowSpan,span->colSpan,row->rowIndex(),span->cell->rowIndex(),i,c);
      if (span->rowSpan>0 && span->column==c &&  // we are at a cell in a row span
          row->rowIndex()>span->cell->rowIndex() // but not the row that started the span
         )
      {
        m_t << "&";
        if (span->colSpan>1) // row span is also part of a column span
        {
          m_t << "\\multicolumn{" << span->colSpan << "}{";
          m_t << "p{(\\linewidth-\\tabcolsep*" 
            << numCols() << "-\\arrayrulewidth*"
            << row->visibleCells() << ")*" 
            << span->colSpan <<"/"<< numCols() << "}|}{}";
        }
        else // solitary row span
        {
          m_t << "\\multicolumn{1}{c|}{}";
        }
      }
    }
    c++;
  }

  m_t << "\\\\";
  
  int col = 1;
  uint i;
  for (i=0;i<rowSpans().count();i++)
  {
    ActiveRowSpan *span = rowSpans().at(i);
    if (span->rowSpan>0) span->rowSpan--;
    if (span->rowSpan<=0)
    {
      // inactive span
    }
    else if (span->column>col)
    {
      m_t << "\\cline{" << col << "-" << (span->column-1) << "}";
      col = span->column+span->colSpan;
    }
    else
    {
      col = span->column+span->colSpan;
    }
  }

  if (col <= numCols())
  {
    m_t << "\\cline{" << col << "-" << numCols() << "}";
  }

  m_t << "\n";

  if (row->isHeading() && row->rowIndex()==1)
  {
    if (firstRow())
    {
      m_t << "\\endfirsthead" << endl;
      m_t << "\\hline" << endl;
      m_t << "\\endfoot" << endl;
      m_t << "\\hline" << endl;
    }
    else
    {
      m_t << "\\endhead" << endl;
    }
  }
}

void LatexDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;

  DocHtmlRow *row = 0;
  if (c->parent() && c->parent()->kind()==DocNode::Kind_HtmlRow)
  {
    row = (DocHtmlRow*)c->parent();
  }

  setCurrentColumn(currentColumn()+1);

  //Skip columns that span from above.
  uint i;
  for (i=0;i<rowSpans().count();i++)
  {
    ActiveRowSpan *span = rowSpans().at(i);
    if (span->rowSpan>0 && span->column==currentColumn())
    {
      if (row && span->colSpan>1)
      {
        m_t << "\\multicolumn{" << span->colSpan << "}{";
        if (currentColumn() /*c->columnIndex()*/==1) // add extra | for first column
        {
          m_t << "|";
        }
        m_t << "p{(\\linewidth-\\tabcolsep*" 
            << numCols() << "-\\arrayrulewidth*"
            << row->visibleCells() << ")*" 
            << span->colSpan <<"/"<< numCols() << "}|}{}";
        setCurrentColumn(currentColumn()+span->colSpan);
      }
      else
      {
        setCurrentColumn(currentColumn()+1);
      }
      m_t << "&";
    }
  }

  int cs = c->colSpan();
  if (cs>1 && row)
  {
    setInColSpan(TRUE);
    m_t << "\\multicolumn{" << cs << "}{";
    if (c->columnIndex()==1) // add extra | for first column
    {
      m_t << "|";
    }
    m_t << "p{(\\linewidth-\\tabcolsep*" 
        << numCols() << "-\\arrayrulewidth*"
        << row->visibleCells() << ")*" 
        << cs <<"/"<< numCols() << "}|}{";
    if (c->isHeading()) m_t << "\\cellcolor{\\tableheadbgcolor}";
  }
  int rs = c->rowSpan();
  if (rs>0)
  {
    setInRowSpan(TRUE);
    //printf("adding row span: cell={r=%d c=%d rs=%d cs=%d} curCol=%d\n",
    //                       c->rowIndex(),c->columnIndex(),c->rowSpan(),c->colSpan(),
    //                       currentColumn());
    addRowSpan(new ActiveRowSpan(c,rs,cs,currentColumn()));
    m_t << "\\multirow{" << rs << "}{\\linewidth}{";
  }
  int a = c->alignment();
  if (a==DocHtmlCell::Center)
  {
    m_t << "\\PBS\\centering ";
  }
  else if (a==DocHtmlCell::Right)
  {
    m_t << "\\PBS\\raggedleft ";
  }
  if (c->isHeading())
  {
    m_t << "{\\bf ";
  }
  if (cs>1)
  {
    setCurrentColumn(currentColumn()+cs-1);
  }
}

void LatexDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (m_hide) return;
  if (c->isHeading())
  {
    m_t << "}";
  }
  if (inRowSpan())
  {
    setInRowSpan(FALSE);
    m_t << "}";
  }
  if (inColSpan())
  {
    setInColSpan(FALSE);
    m_t << "}";
  }
  if (!c->isLast()) m_t << "&";
}

void LatexDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  //m_t << "\\begin{DoxyInternal}{";
  //filter(theTranslator->trForInternalUseOnly());
  //m_t << "}\n";
}

void LatexDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  //m_t << "\\end{DoxyInternal}" << endl;
}

void LatexDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  if (Config_getBool(PDF_HYPERLINKS))
  {
    m_t << "\\href{";
    m_t << href->url();
    m_t << "}";
  }
  m_t << "{\\tt ";
}

void LatexDocVisitor::visitPost(DocHRef *) 
{
  if (m_hide) return;
  m_t << "}";
}

void LatexDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  m_t << "\\" << getSectionName(header->level()) << "*{";
}

void LatexDocVisitor::visitPost(DocHtmlHeader *) 
{
  if (m_hide) return;
  m_t << "}";
}
void LatexDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Latex)
  {
    if (m_hide) return;
    QCString gfxName = img->name();
    if (gfxName.right(4)==".eps" || gfxName.right(4)==".pdf")
    {
      gfxName=gfxName.left(gfxName.length()-4);
    }

    visitPreStart(m_t,img->hasCaption(), gfxName, img->width(),  img->height());
  }
  else // other format -> skip
  {
    pushEnabled();
    m_hide=TRUE;
  }
}

void LatexDocVisitor::visitPost(DocImage *img) 
{
  if (img->type()==DocImage::Latex)
  {
    if (m_hide) return;
    visitPostEnd(m_t,img->hasCaption());
  }
  else // other format
  {
    popEnabled();
  }
}

void LatexDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  startDotFile(df->file(),df->width(),df->height(),df->hasCaption());
}

void LatexDocVisitor::visitPost(DocDotFile *df) 
{
  if (m_hide) return;
  endDotFile(df->hasCaption());
}
void LatexDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  startMscFile(df->file(),df->width(),df->height(),df->hasCaption());
}

void LatexDocVisitor::visitPost(DocMscFile *df) 
{
  if (m_hide) return;
  endMscFile(df->hasCaption());
}

void LatexDocVisitor::visitPre(DocDiaFile *df)
{
  if (m_hide) return;
  startDiaFile(df->file(),df->width(),df->height(),df->hasCaption());
}

void LatexDocVisitor::visitPost(DocDiaFile *df)
{
  if (m_hide) return;
  endDiaFile(df->hasCaption());
}
void LatexDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void LatexDocVisitor::visitPost(DocLink *lnk) 
{
  if (m_hide) return;
  endLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void LatexDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  // when ref->isSubPage()==TRUE we use ref->file() for HTML and
  // ref->anchor() for LaTeX/RTF
  if (ref->isSubPage())
  {
    startLink(ref->ref(),0,ref->anchor());
  }
  else
  {
    if (!ref->file().isEmpty()) startLink(ref->ref(),ref->file(),ref->anchor(),ref->refToTable());
  }
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void LatexDocVisitor::visitPost(DocRef *ref) 
{
  if (m_hide) return;
  if (ref->isSubPage())
  {
    endLink(ref->ref(),0,ref->anchor());
  }
  else
  {
    if (!ref->file().isEmpty()) endLink(ref->ref(),ref->file(),ref->anchor());
  }
}

void LatexDocVisitor::visitPre(DocSecRefItem *ref)
{
  if (m_hide) return;
  m_t << "\\item \\contentsline{section}{";
  static bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (pdfHyperlinks)
  {
    m_t << "\\hyperlink{" << ref->file() << "_" << ref->anchor() << "}{" ;
  }
}

void LatexDocVisitor::visitPost(DocSecRefItem *ref) 
{
  if (m_hide) return;
  static bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  if (pdfHyperlinks)
  {
    m_t << "}";
  }
  m_t << "}{\\ref{" << ref->file() << "_" << ref->anchor() << "}}{}" << endl;
}

void LatexDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "\\footnotesize" << endl;
  m_t << "\\begin{multicols}{2}" << endl;
  m_t << "\\begin{DoxyCompactList}" << endl;
}

void LatexDocVisitor::visitPost(DocSecRefList *) 
{
  if (m_hide) return;
  m_t << "\\end{DoxyCompactList}" << endl;
  m_t << "\\end{multicols}" << endl;
  m_t << "\\normalsize" << endl;
}

void LatexDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  bool hasInOutSpecs = s->hasInOutSpecifier();
  bool hasTypeSpecs  = s->hasTypeSpecifier();
  switch(s->type())
  {
    case DocParamSect::Param:
      m_t << "\n\\begin{DoxyParams}";
      if      (hasInOutSpecs && hasTypeSpecs) m_t << "[2]"; // 2 extra cols
      else if (hasInOutSpecs || hasTypeSpecs) m_t << "[1]"; // 1 extra col
      m_t << "{";
      filter(theTranslator->trParameters());
      break;
    case DocParamSect::RetVal:
      m_t << "\n\\begin{DoxyRetVals}{";
      filter(theTranslator->trReturnValues());
      break;
    case DocParamSect::Exception: 
      m_t << "\n\\begin{DoxyExceptions}{";
      filter(theTranslator->trExceptions());
      break;
    case DocParamSect::TemplateParam: 
      /* TODO: add this 
      filter(theTranslator->trTemplateParam()); break;
      */
      m_t << "\n\\begin{DoxyTemplParams}{";
      filter("Template Parameters");
      break;
    default:
      ASSERT(0);
  }
  m_t << "}\n";
}

void LatexDocVisitor::visitPost(DocParamSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocParamSect::Param:
      m_t << "\\end{DoxyParams}\n";
      break;
    case DocParamSect::RetVal:
      m_t << "\\end{DoxyRetVals}\n";
      break;
    case DocParamSect::Exception: 
      m_t << "\\end{DoxyExceptions}\n";
      break;
    case DocParamSect::TemplateParam: 
      m_t << "\\end{DoxyTemplParams}\n";
      break;
    default:
      ASSERT(0);
  }
}

void LatexDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
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
  if (!useTable)
  {
    m_t << "\\item[";
  }
  if (sect && sect->hasInOutSpecifier())
  {
    if (pl->direction()!=DocParamSect::Unspecified)
    {
      m_t << "\\mbox{\\tt ";
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
      m_t << "} ";
    }
    if (useTable) m_t << " & ";
  }
  if (sect && sect->hasTypeSpecifier())
  {
    QListIterator<DocNode> li(pl->paramTypes());
    DocNode *type;
    bool first=TRUE;
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
    if (useTable) m_t << " & ";
  }
  m_t << "{\\em ";
  //QStrListIterator li(pl->parameters());
  //const char *s;
  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  bool first=TRUE;
  for (li.toFirst();(param=li.current());++li)
  {
    if (!first) m_t << ","; else first=FALSE;
    m_insideItem=TRUE;
    if (param->kind()==DocNode::Kind_Word)
    {
      visit((DocWord*)param); 
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      visit((DocLinkedWord*)param); 
    }
    m_insideItem=FALSE;
  }
  m_t << "}";
  if (useTable)
  {
    m_t << " & ";
  }
  else
  {
    m_t << "]";
  }
}

void LatexDocVisitor::visitPost(DocParamList *pl)
{
  if (m_hide) return;
  DocParamSect::Type parentType = DocParamSect::Unknown;
  if (pl->parent() && pl->parent()->kind()==DocNode::Kind_ParamSect)
  {
    parentType = ((DocParamSect*)pl->parent())->type();
  }
  bool useTable = parentType==DocParamSect::Param ||
                  parentType==DocParamSect::RetVal ||
                  parentType==DocParamSect::Exception ||
                  parentType==DocParamSect::TemplateParam;
  if (useTable)
  {
    m_t << "\\\\" << endl
        << "\\hline" << endl;
  }
}

void LatexDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "\\begin{DoxyRefDesc}{";
  filter(x->title());
  m_t << "}" << endl;
  bool anonymousEnum = x->file()=="@";
  m_t << "\\item[";
  if (Config_getBool(PDF_HYPERLINKS) && !anonymousEnum)
  {
    m_t << "\\hyperlink{" << stripPath(x->file()) << "_" << x->anchor() << "}{";
  }
  else
  {
    m_t << "{\\bf ";
  }
  m_insideItem=TRUE;
  filter(x->title());
  m_insideItem=FALSE;
  m_t << "}]";
}

void LatexDocVisitor::visitPost(DocXRefItem *x)
{
  if (m_hide) return;
  if (x->title().isEmpty()) return;
  m_t << "\\end{DoxyRefDesc}" << endl;
}

void LatexDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(0,ref->file(),ref->anchor());
}

void LatexDocVisitor::visitPost(DocInternalRef *ref) 
{
  if (m_hide) return;
  endLink(0,ref->file(),ref->anchor());
}

void LatexDocVisitor::visitPre(DocCopy *)
{
}

void LatexDocVisitor::visitPost(DocCopy *)
{
}

void LatexDocVisitor::visitPre(DocText *)
{
}

void LatexDocVisitor::visitPost(DocText *)
{
}

void LatexDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "\\begin{quote}" << endl;
}

void LatexDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "\\end{quote}" << endl;
}

void LatexDocVisitor::visitPre(DocVhdlFlow *)
{
  if (m_hide) return;
}

void LatexDocVisitor::visitPost(DocVhdlFlow *)
{
  if (m_hide) return;
}

void LatexDocVisitor::visitPre(DocParBlock *)
{
  if (m_hide) return;
}

void LatexDocVisitor::visitPost(DocParBlock *)
{
  if (m_hide) return;
}

void LatexDocVisitor::filter(const char *str)
{ 
  filterLatexString(m_t,str,m_insideTabbing,m_insidePre,m_insideItem);
}

void LatexDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor,bool refToTable)
{
  static bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
  if (ref.isEmpty() && pdfHyperLinks) // internal PDF link
  {
    if (refToTable)
    {
      m_t << "\\doxytablelink{";
    }
    else
    {
      m_t << "\\hyperlink{";
    }
    if (!file.isEmpty()) m_t << stripPath(file);
    if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
    if (!anchor.isEmpty()) m_t << anchor;
    m_t << "}{";
  }
  else if (ref.isEmpty() && refToTable)
  {
    m_t << "\\doxytableref{";
  }
  else if (ref.isEmpty()) // internal non-PDF link
  {
    m_t << "\\doxyref{";
  }
  else // external link
  {
    m_t << "{\\bf ";
  }
}

void LatexDocVisitor::endLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  m_t << "}";
  static bool pdfHyperLinks = Config_getBool(PDF_HYPERLINKS);
  if (ref.isEmpty() && !pdfHyperLinks)
  {
    m_t << "{";
    filter(theTranslator->trPageAbbreviation());
    m_t << "}{" << file;
    if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
    m_t << anchor << "}";
  }
}

void LatexDocVisitor::pushEnabled()
{
  m_enabled.push(new bool(m_hide));
}

void LatexDocVisitor::popEnabled()
{
  bool *v=m_enabled.pop();
  ASSERT(v!=0);
  m_hide = *v;
  delete v;
}

void LatexDocVisitor::startDotFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption
                                  )
{
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
  QCString outDir = Config_getString(LATEX_OUTPUT);
  QCString name = fileName;
  writeDotGraphFromFile(name,outDir,baseName,GOF_EPS);
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endDotFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}

void LatexDocVisitor::startMscFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption
                                  )
{
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

  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_EPS); 
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endMscFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}


void LatexDocVisitor::writeMscFile(const QCString &baseName, DocVerbatim *s)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  } 
  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MSC_EPS);
  visitPreStart(m_t, s->hasCaption(), shortName, s->width(),s->height());
  visitCaption(this, s->children());
  visitPostEnd(m_t, s->hasCaption());
}


void LatexDocVisitor::startDiaFile(const QCString &fileName,
                                   const QCString &width,
                                   const QCString &height,
                                   bool hasCaption
                                  )
{
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

  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeDiaGraphFromFile(fileName,outDir,baseName,DIA_EPS);
  visitPreStart(m_t,hasCaption, baseName, width, height);
}

void LatexDocVisitor::endDiaFile(bool hasCaption)
{
  if (m_hide) return;
  visitPostEnd(m_t,hasCaption);
}


void LatexDocVisitor::writeDiaFile(const QCString &baseName, DocVerbatim *s)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(LATEX_OUTPUT);
  writeDiaGraphFromFile(baseName+".dia",outDir,shortName,DIA_EPS);
  visitPreStart(m_t, s->hasCaption(), shortName, s->width(), s->height());
  visitCaption(this, s->children());
  visitPostEnd(m_t, s->hasCaption());
}

void LatexDocVisitor::writePlantUMLFile(const QCString &baseName, DocVerbatim *s)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString(LATEX_OUTPUT);
  generatePlantUMLOutput(baseName,outDir,PUML_EPS);
  visitPreStart(m_t, s->hasCaption(), shortName, s->width(), s->height());
  visitCaption(this, s->children());
  visitPostEnd(m_t, s->hasCaption());
}

