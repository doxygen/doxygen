/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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
#include "htmlattrib.h"

static QCString escapeLabelName(const char *s)
{
  QCString result;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '%': result+="\\%"; break;
      case '|': result+="\\texttt{\"|}"; break;
      case '!': result+="\"!"; break;
      default: result+=c;
    }
  }
  return result;
}

const int maxLevels=5;
static const char *secLabels[maxLevels] = 
   { "section","subsection","subsubsection","paragraph","subparagraph" };

static const char *getSectionName(int level)
{
  int l = level;
  if (Config_getBool("COMPACT_LATEX")) l++;
  if (Doxygen::insideMainPage) l--;
  return secLabels[QMIN(maxLevels-1,l)];
}

static int rowspan(DocHtmlCell *cell)
{
  int retval = 0;
  HtmlAttribList attrs = cell->attribs ();
  for (unsigned int i = 0; i < attrs.count(); ++i) 
  {
    if ("rowspan" == attrs.at(i)->name.lower())
    {
      retval = attrs.at(i)->value.toInt();
      break;
    }
  }
  return retval;
}

QCString LatexDocVisitor::escapeMakeIndexChars(const char *s)
{
  QCString result;
  const char *p=s;
  char str[2]; str[1]=0;
  char c;
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
      default:  str[0]=c; filter(str); break;
    }
  }
  return result;
}


LatexDocVisitor::LatexDocVisitor(FTextStream &t,CodeOutputInterface &ci,
                                 const char *langExt,bool insideTabbing) 
  : DocVisitor(DocVisitor_Latex), m_t(t), m_ci(ci), m_insidePre(FALSE), 
    m_insideItem(FALSE), m_hide(FALSE), m_insideTabbing(insideTabbing),
    m_langExt(langExt), m_currentColumn(0), m_inRowspan(FALSE)
{
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
  switch(s->symbol())
  {
    case DocSymbol::BSlash:  m_t << "$\\backslash$"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    if (m_insidePre) m_t << "<"; else m_t << "$<$"; 
                             break;
    case DocSymbol::Greater: if (m_insidePre) m_t << ">"; else m_t << "$>$"; break;
    case DocSymbol::Amp:     m_t << "\\&"; break;
    case DocSymbol::Dollar:  m_t << "\\$"; break;
    case DocSymbol::Hash:    m_t << "\\#"; break;
    case DocSymbol::Percent: m_t << "\\%"; break;
    case DocSymbol::Copy:    m_t << "\\copyright"; break;
    case DocSymbol::Tm:      m_t << "\\texttrademark"; break;
    case DocSymbol::Reg:     m_t << "\\textregistered"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Lsquo:   m_t << "`"; break;
    case DocSymbol::Rsquo:   m_t << "'"; break;
    case DocSymbol::Ldquo:   m_t << "``"; break;
    case DocSymbol::Rdquo:   m_t << "''"; break;
    case DocSymbol::Ndash:   m_t << "--"; break;
    case DocSymbol::Mdash:   m_t << "---"; break;
    case DocSymbol::Uml:     
                             if (s->letter()=='i') 
                               m_t << "\\\"{\\i}"; 
                             else                  
                               m_t << "\\\"{" << s->letter() << "}"; 
                             break;
    case DocSymbol::Acute:   
                             if (s->letter()=='i') 
                               m_t << "\\'{\\i}"; 
                             else                  
                               m_t << "\\'{" << s->letter() << "}"; 
                             break;
    case DocSymbol::Grave:   
                             if (s->letter()=='i') 
                               m_t << "\\`{\\i}"; 
                             else                  
                               m_t << "\\`{" << s->letter() << "}"; 
                             break;
    case DocSymbol::Circ:    
                             if (s->letter()=='i') 
                               m_t << "\\^{\\i}"; 
                             else                  
                               m_t << "\\^{" << s->letter() << "}"; 
                             break;
    case DocSymbol::Slash:   if (tolower(s->letter())=='o')
                               m_t << "{\\" << s->letter() << "}";
                             else
                               m_t << s->letter();
                             break;
    case DocSymbol::Tilde:   m_t << "\\~{"  << s->letter() << "}"; break;
    case DocSymbol::Szlig:   m_t << "{\\ss}"; break;
    case DocSymbol::Cedil:   m_t << "\\c{" << s->letter() << "}"; break;
    case DocSymbol::Ring:    m_t << "\\" << s->letter() << s->letter(); break;
    case DocSymbol::Nbsp:    m_t << "~"; break;
    case DocSymbol::AElig:   m_t << "{\\AE}"; break;
    case DocSymbol::Aelig:   m_t << "{\\ae}"; break;
    default:
                             err("error: unknown symbol found\n");
  }
}

void LatexDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  if (Config_getBool("PDF_HYPERLINKS"))
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
  m_t << "\\par\n";
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
      if (s->enable()) m_t << "{\\itshape ";     else m_t << "\\/}";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "{\\ttfamily ";   else m_t << "}";
      break;
    case DocStyleChange::Subscript:
      if (s->enable()) m_t << "$_{\\mbox{";    else m_t << "}}$ ";
      break;
    case DocStyleChange::Superscript:
      if (s->enable()) m_t << "$^{\\mbox{";    else m_t << "}}$ ";
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
  //static bool latexSourceCode = Config_getBool("LATEX_SOURCE_CODE");
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: 
      //if (latexSourceCode)
      //{
      //  m_t << "\n\n\\begin{footnotesize}\\begin{alltt}" << endl; 
      //}
      //else
      {
        m_t << "\n\\begin{DoxyCode}\n";
      }
      Doxygen::parserManager->getParser(m_langExt)
                            ->parseCode(m_ci,s->context(),s->text(),
                                        s->isExample(),s->exampleFile());
      //if (latexSourceCode)
      //{
      //  m_t << "\\end{alltt}\\end{footnotesize}" << endl; 
      //}
      //else
      {
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
            (Config_getString("LATEX_OUTPUT")+"/inline_dotgraph_").data(), 
            dotindex++,
            ".dot"
           );
        QFile file(fileName);
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s for writing\n",fileName.data());
        }
        file.writeBlock( s->text(), s->text().length() );
        file.close();

        m_t << "\\begin{center}\n";
        startDotFile(fileName,"","",FALSE);
        endDotFile(FALSE);
        m_t << "\\end{center}\n";

        if (Config_getBool("DOT_CLEANUP")) file.remove();
      }
      break;
    case DocVerbatim::Msc: 
      {
        static int mscindex = 1;
        QCString baseName(4096);

        baseName.sprintf("%s%d", 
            (Config_getString("LATEX_OUTPUT")+"/inline_mscgraph_").data(), 
            mscindex++
           );
        QFile file(baseName+".msc");
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s.msc for writing\n",baseName.data());
        }
        QCString text = "msc {";
        text+=s->text();
        text+="}";
        file.writeBlock( text, text.length() );
        file.close();

        m_t << "\\begin{center}\n";
        writeMscFile(baseName);
        m_t << "\\end{center}\n";

        if (Config_getBool("DOT_CLEANUP")) file.remove();
      }
      break;
  }
}

void LatexDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "\\label{" << anc->file() << "_" << anc->anchor() << "}" << endl;
  if (!anc->file().isEmpty() && Config_getBool("PDF_HYPERLINKS")) 
  {
    m_t << "\\hypertarget{" << anc->file() << "_" << anc->anchor() 
      << "}{}" << endl;
  }    
}

void LatexDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      { 
         m_t << "\n\\begin{DoxyCodeInclude}\n";
         QFileInfo cfi( inc->file() );
         FileDef fd( cfi.dirPath(), cfi.fileName() );
         Doxygen::parserManager->getParser(inc->extension())
                               ->parseCode(m_ci,inc->context(),
                                           inc->text(),
                                           inc->isExample(),
                                           inc->exampleFile(), &fd);
         m_t << "\\end{DoxyCodeInclude}" << endl;
      }
      break;    
    case DocInclude::Include: 
      m_t << "\n\\begin{DoxyCodeInclude}\n";
      Doxygen::parserManager->getParser(inc->extension())
                            ->parseCode(m_ci,inc->context(),
                                        inc->text(),inc->isExample(),
                                        inc->exampleFile());
      m_t << "\\end{DoxyCodeInclude}\n";
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      break;
    case DocInclude::VerbInclude: 
      m_t << "\n\\begin{DoxyVerbInclude}\n";
      m_t << inc->text();
      m_t << "\\end{DoxyVerbInclude}\n";
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
  if (op->type()!=DocIncOperator::Skip) 
  {
    popEnabled();
    if (!m_hide) 
    {
      Doxygen::parserManager->getParser(m_langExt)
                            ->parseCode(m_ci,op->context(),op->text(),
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
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hypertarget{" << s->file() << "_" << s->anchor() << "}{}";
  }
  m_t << "\\" << getSectionName(s->level()) << "{";
  filter(convertCharEntitiesToUTF8(s->title().data()));
  m_t << "}\\label{" << s->file() << "_" << s->anchor() << "}" << endl;
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

void LatexDocVisitor::visitPre(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "\n\\begin{DoxyDescription}";
}

void LatexDocVisitor::visitPost(DocHtmlDescList *) 
{
  if (m_hide) return;
  m_t << "\n\\end{DoxyDescription}";
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

void LatexDocVisitor::visitPre(DocHtmlTable *t)
{
  m_rowspanIndices.clear();
  if (m_hide) return;
  if (t->hasCaption()) 
  {
    m_t << "\\begin{table}[h]";
  }
  m_t << "\\begin{TabularC}{" << t->numCols() << "}\n\\hline\n";
}

void LatexDocVisitor::visitPost(DocHtmlTable *t) 
{
  if (m_hide) return;
  if (t->hasCaption())
  {
    m_t << "\\end{table}\n";
  }
  else
  {
    m_t << "\\end{TabularC}\n";
  }
}

void LatexDocVisitor::visitPre(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "\\end{TabularC}\n\\centering\n\\caption{";
}

void LatexDocVisitor::visitPost(DocHtmlCaption *) 
{
  if (m_hide) return;
  m_t << "}\n";
}

void LatexDocVisitor::visitPre(DocHtmlRow *)
{
  m_currentColumn = 0;
}

void LatexDocVisitor::visitPost(DocHtmlRow *) 
{
  if (m_hide) return;

  m_t << "\\\\";
  
  QMap<int, int>::Iterator it;
  int col = 1;
    for (it = m_rowspanIndices.begin(); it != m_rowspanIndices.end(); ++it) 
  {
    it.data()--;
    if (it.data () <= 0)
      m_rowspanIndices.remove (it);
    else if (0 < it.data() - col)
      m_t << "\\cline{" << col << "-" << it.data() - col << "}";
      
    col = 1 + it.data ();
    }

  if (col <= m_currentColumn)
    m_t << "\\cline{" << col << "-" << m_currentColumn << "}";

  m_t << "\n";
}

void LatexDocVisitor::visitPre(DocHtmlCell *cell)
{
  if (m_hide) return;
  
  m_currentColumn++;
  //Skip columns that span from above.
  QMap<int, int>::Iterator it = m_rowspanIndices.find(m_currentColumn);
  while (0 < it.data() && it != m_rowspanIndices.end())
  {
    m_t << "&";
    m_currentColumn++;
    it++;
  }

  int rs = rowspan(cell);
  if (0 < rs)
  {
    m_inRowspan = TRUE;
    m_rowspanIndices[m_currentColumn] = rs;
    m_t << "\\multirow{" << rs << "}{\\linewidth}{";
  }
}

void LatexDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (m_hide) return;
  if (m_inRowspan)
  {
    m_inRowspan = FALSE;
    m_t << "}";
  }
  if (!c->isLast()) m_t << "&";
}

void LatexDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  m_t << "\\begin{DoxyInternal}{";
  filter(theTranslator->trForInternalUseOnly());
  m_t << "}\n";
}

void LatexDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  m_t << "\\end{DoxyInternal}" << endl;
}

void LatexDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  if (Config_getBool("PDF_HYPERLINKS"))
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
    if (img->hasCaption())
    {
      m_t << "\n\\begin{DoxyImage}\n";
    }
    else
    {
      m_t << "\n\\begin{DoxyImageNoCaption}\n"
             "  \\mbox{";
    }
    QCString gfxName = img->name();
    if (gfxName.right(4)==".eps" || gfxName.right(4)==".pdf")
    {
      gfxName=gfxName.left(gfxName.length()-4);
    }
    m_t << "\\includegraphics";
    if (!img->width().isEmpty())
    {
      m_t << "[width=" << img->width() << "]";
    }
    else if (!img->height().isEmpty())
    {
      m_t << "[height=" << img->height() << "]";
    }
    m_t << "{" << gfxName << "}";
    if (img->hasCaption())
    {
      m_t << "\n\\caption{";
    }
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
    m_t << "}\n"; // end mbox or caption
    if (img->hasCaption())
    {
      m_t << "\\end{DoxyImage}\n";
    }
    else{
      m_t << "\\end{DoxyImageNoCaption}\n";
    }
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
  if (!ref->file().isEmpty()) startLink(ref->ref(),ref->file(),ref->anchor());
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void LatexDocVisitor::visitPost(DocRef *ref) 
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink(ref->ref(),ref->file(),ref->anchor());
}

void LatexDocVisitor::visitPre(DocSecRefItem *)
{
  if (m_hide) return;
  m_t << "\\item \\contentsline{section}{";
}

void LatexDocVisitor::visitPost(DocSecRefItem *ref) 
{
  if (m_hide) return;
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

//void LatexDocVisitor::visitPre(DocLanguage *l)
//{
//  QCString langId = Config_getEnum("OUTPUT_LANGUAGE");
//  if (l->id().lower()!=langId.lower())
//  {
//    pushEnabled();
//    m_hide = TRUE;
//  }
//}
//
//void LatexDocVisitor::visitPost(DocLanguage *l) 
//{
//  QCString langId = Config_getEnum("OUTPUT_LANGUAGE");
//  if (l->id().lower()!=langId.lower())
//  {
//    popEnabled();
//  }
//}

void LatexDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocParamSect::Param:
      m_t << "\n\\begin{DoxyParams}{";
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
  m_t << "\\item[";
  if (pl->direction()!=DocParamSect::Unspecified)
  {
    m_t << "\\mbox{\\tt[";
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
    m_t << "]} ";
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
  m_t << "}]";
}

void LatexDocVisitor::visitPost(DocParamList *)
{
}

void LatexDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  m_t << "\\begin{Desc}" << endl;
  bool anonymousEnum = x->file()=="@";
  m_t << "\\item[";
  if (Config_getBool("PDF_HYPERLINKS") && !anonymousEnum)
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

void LatexDocVisitor::visitPost(DocXRefItem *)
{
  if (m_hide) return;
  m_t << "\\end{Desc}" << endl;
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

void LatexDocVisitor::filter(const char *str)
{ 
  filterLatexString(m_t,str,m_insideTabbing,m_insidePre,m_insideItem);
}

void LatexDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  if (ref.isEmpty() && Config_getBool("PDF_HYPERLINKS")) // internal PDF link 
  {
    if (ref.isEmpty()) {
      m_t << "\\hyperlink{";
      if (!file.isEmpty()) m_t << stripPath(file);
      if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
      if (!anchor.isEmpty()) m_t << anchor;
      m_t << "}{";
    }
    else
    {
      QCString *dest;
      m_t << "\\href{";
      if ((dest=Doxygen::tagDestinationDict[ref])) m_t << *dest << "/";
      if (!file.isEmpty()) m_t << file << Doxygen::htmlFileExtension;
      if (!anchor.isEmpty()) m_t << "#" << anchor;
      m_t << "}{";
    }
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
  if (ref.isEmpty() && !Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "{"; 
    filter(theTranslator->trPageAbbreviation());
    m_t << "}{" << file;
    if (!anchor.isEmpty()) m_t << "_" << anchor;
    m_t << "}";
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
  if (baseName.right(4)==".eps" || baseName.right(4)==".pdf")
  {
    baseName=baseName.left(baseName.length()-4);
  }
  if (baseName.right(4)==".dot")
  {
    baseName=baseName.left(baseName.length()-4);
  }
  QCString outDir = Config_getString("LATEX_OUTPUT");
  QCString name = fileName;
  writeDotGraphFromFile(name,outDir,baseName,EPS);
  if (hasCaption)
  {
    m_t << "\n\\begin{DoxyImage}\n";
  }
  else
  {
    m_t << "\n\\begin{DoxyImageNoCaption}\n"
           "  \\mbox{";
  }
  m_t << "\\includegraphics";
  if (!width.isEmpty())
  {
    m_t << "[width=" << width << "]";
  }
  else if (!height.isEmpty())
  {
    m_t << "[height=" << height << "]";
  }
  m_t << "{" << baseName << "}";

  if (hasCaption)
  {
    m_t << "\n\\caption{";
  }
}

void LatexDocVisitor::endDotFile(bool hasCaption)
{
  if (m_hide) return;
  m_t << "}\n"; // end caption or mbox
  if (hasCaption)
  {
    m_t << "\\end{DoxyImage}\n";
  }
  else
  {
    m_t << "\\end{DoxyImageNoCaption}\n";
  }
}

void LatexDocVisitor::writeMscFile(const QCString &baseName)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  } 
  QCString outDir = Config_getString("LATEX_OUTPUT");
  writeMscGraphFromFile(baseName,outDir,baseName,MSC_EPS);
  m_t << "\n\\begin{DoxyImageNoCaption}"
         "  \\mbox{\\includegraphics";
  m_t << "{" << shortName << "}";
  m_t << "}\n"; // end mbox
  m_t << "\\end{DoxyImageNoCaption}\n";
}

