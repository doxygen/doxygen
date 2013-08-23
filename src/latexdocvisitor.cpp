/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "cite.h"
#include "filedef.h"
#include "config.h"

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
  static bool compactLatex = Config_getBool("COMPACT_LATEX");
  int l = level;
  if (compactLatex) l++;
  if (Doxygen::insideMainPage) l--;
  return secLabels[QMIN(maxLevels-1,l)];
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
    m_insideTable(FALSE), m_langExt(langExt), m_currentColumn(0), 
    m_inRowspan(FALSE), m_inColspan(FALSE)
{
  m_rowSpans.setAutoDelete(TRUE);
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
    case DocSymbol::BSlash:  m_t << "\\textbackslash{}"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    if (m_insidePre) m_t << "<"; else m_t << "$<$"; 
                             break;
    case DocSymbol::Greater: if (m_insidePre) m_t << ">"; else m_t << "$>$"; break;
    case DocSymbol::Amp:     m_t << "\\&"; break;
    case DocSymbol::Dollar:  m_t << "\\$"; break;
    case DocSymbol::Hash:    m_t << "\\#"; break;
    case DocSymbol::DoubleColon:  m_t << "::"; break;
    case DocSymbol::Percent: m_t << "\\%"; break;
    case DocSymbol::Pipe:    m_t << "$|$"; break;
    case DocSymbol::Copy:    m_t << "\\copyright{}"; break;
    case DocSymbol::Tm:      m_t << "\\texttrademark{}"; break;
    case DocSymbol::Reg:     m_t << "\\textregistered{}"; break;
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
    case DocSymbol::GrkGamma:      m_t << "{$\\Gamma$}"; break;
    case DocSymbol::GrkDelta:      m_t << "{$\\Delta$}"; break;
    case DocSymbol::GrkTheta:      m_t << "{$\\Theta$}"; break;
    case DocSymbol::GrkLambda:     m_t << "{$\\Lambda$}"; break;
    case DocSymbol::GrkXi:         m_t << "{$\\Xi$}"; break;
    case DocSymbol::GrkPi:         m_t << "{$\\Pi$}"; break;
    case DocSymbol::GrkSigma:      m_t << "{$\\Sigma$}"; break;
    case DocSymbol::GrkUpsilon:    m_t << "{$\\Upsilon$}"; break;
    case DocSymbol::GrkPhi:        m_t << "{$\\Phi$}"; break;
    case DocSymbol::GrkPsi:        m_t << "{$\\Psi$}"; break;
    case DocSymbol::GrkOmega:      m_t << "{$\\Omega$}"; break;
    case DocSymbol::Grkalpha:      m_t << "{$\\alpha$}"; break;
    case DocSymbol::Grkbeta:       m_t << "{$\\beta$}"; break;
    case DocSymbol::Grkgamma:      m_t << "{$\\gamma$}"; break;
    case DocSymbol::Grkdelta:      m_t << "{$\\delta$}"; break;
    case DocSymbol::Grkepsilon:    m_t << "{$\\varepsilon$}"; break;
    case DocSymbol::Grkzeta:       m_t << "{$\\zeta$}"; break;
    case DocSymbol::Grketa:        m_t << "{$\\eta$}"; break;
    case DocSymbol::Grktheta:      m_t << "{$\\theta$}"; break;
    case DocSymbol::Grkiota:       m_t << "{$\\iota$}"; break;
    case DocSymbol::Grkkappa:      m_t << "{$\\kappa$}"; break;
    case DocSymbol::Grklambda:     m_t << "{$\\lambda$}"; break;
    case DocSymbol::Grkmu:         m_t << "{$\\mu$}"; break;
    case DocSymbol::Grknu:         m_t << "{$\\nu$}"; break;
    case DocSymbol::Grkxi:         m_t << "{$\\xi$}"; break;
    case DocSymbol::Grkpi:         m_t << "{$\\pi$}"; break;
    case DocSymbol::Grkrho:        m_t << "{$\\rho$}"; break;
    case DocSymbol::Grksigma:      m_t << "{$\\sigma$}"; break;
    case DocSymbol::Grktau:        m_t << "{$\\tau$}"; break;
    case DocSymbol::Grkupsilon:    m_t << "{$\\upsilon$}"; break;
    case DocSymbol::Grkphi:        m_t << "{$\\varphi$}"; break;
    case DocSymbol::Grkchi:        m_t << "{$\\chi$}"; break;
    case DocSymbol::Grkpsi:        m_t << "{$\\psi$}"; break;
    case DocSymbol::Grkomega:      m_t << "{$\\omega$}"; break;
    case DocSymbol::Grkvarsigma:   m_t << "{$\\varsigma$}"; break;
    case DocSymbol::Section:       m_t << "{$\\S$}"; break;
    case DocSymbol::Degree:        m_t << "\\textdegree"; break;
    case DocSymbol::Prime:         m_t << "'"; break;
    case DocSymbol::DoublePrime:   m_t << "''"; break;
    case DocSymbol::Infinity:      m_t << "{$\\infty$}"; break;
    case DocSymbol::EmptySet:      m_t << "{$\\emptyset$}"; break;
    case DocSymbol::PlusMinus:     m_t << "{$\\pm$}"; break;
    case DocSymbol::Times:         m_t << "{$\\times$}"; break;
    case DocSymbol::Minus:         m_t << "-"; break;
    case DocSymbol::CenterDot:     m_t << "{$\\cdot$}"; break;
    case DocSymbol::Partial:       m_t << "{$\\partial$}"; break;
    case DocSymbol::Nabla:         m_t << "{$\\nabla$}"; break;
    case DocSymbol::SquareRoot:    m_t << "{$\\surd$}"; break;
    case DocSymbol::Perpendicular: m_t << "{$\\perp$}"; break;
    case DocSymbol::Sum:           m_t << "{$\\sum$}"; break;
    case DocSymbol::Integral:      m_t << "{$\\int$}"; break;
    case DocSymbol::Product:       m_t << "{$\\prod$}"; break;
    case DocSymbol::Similar:       m_t << "{$\\sim$}"; break;
    case DocSymbol::Approx:        m_t << "{$\\approx$}"; break;
    case DocSymbol::NotEqual:      m_t << "{$\\ne$}"; break;
    case DocSymbol::Equivalent:    m_t << "{$\\equiv$}"; break;
    case DocSymbol::Proportional:  m_t << "{$\\propto$}"; break;
    case DocSymbol::LessEqual:     m_t << "{$\\le$}"; break;
    case DocSymbol::GreaterEqual:  m_t << "{$\\ge$}"; break;
    case DocSymbol::LeftArrow:     m_t << "{$\\leftarrow$}"; break;
    case DocSymbol::RightArrow:    m_t << "{$\\rightarrow$}"; break;
    case DocSymbol::SetIn:         m_t << "{$\\in$}"; break;
    case DocSymbol::SetNotIn:      m_t << "{$\\notin$}"; break;
    case DocSymbol::LeftCeil:      m_t << "{$\\lceil$}"; break;
    case DocSymbol::RightCeil:     m_t << "{$\\rceil$}"; break;
    case DocSymbol::LeftFloor:     m_t << "{$\\lfloor$}"; break;
    case DocSymbol::RightFloor:    m_t << "{$\\rfloor$}"; break;
    default:
                             err("unknown symbol found\n");
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
  if (m_insideTable) m_t << "\\newline\n";
  else m_t << "\\par\n";
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
  m_t << "\\label{" << stripPath(anc->file()) << "_" << anc->anchor() << "}%" << endl;
  if (!anc->file().isEmpty() && Config_getBool("PDF_HYPERLINKS")) 
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
  if (Config_getBool("PDF_HYPERLINKS"))
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

void LatexDocVisitor::visitPre(DocHtmlTable *t)
{
  m_rowSpans.clear();
  m_insideTable=TRUE;
  if (m_hide) return;
  if (t->hasCaption()) 
  {
    m_t << "\\begin{table}[h]";
  }
  m_t << "\\begin{TabularC}{" << t->numColumns() << "}\n";
  m_numCols = t->numColumns();
  m_t << "\\hline\n";
}

void LatexDocVisitor::visitPost(DocHtmlTable *t) 
{
  m_insideTable=FALSE;
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

void LatexDocVisitor::visitPre(DocHtmlRow *r)
{
  m_currentColumn = 0;
  if (r->isHeading()) m_t << "\\rowcolor{lightgray}";
}

void LatexDocVisitor::visitPost(DocHtmlRow *row) 
{
  if (m_hide) return;

  int c=m_currentColumn;
  while (c<=m_numCols) // end of row while inside a row span?
  {
    uint i;
    for (i=0;i<m_rowSpans.count();i++)
    {
      ActiveRowSpan *span = m_rowSpans.at(i);
      //printf("  founc row span: column=%d rs=%d cs=%d rowIdx=%d cell->rowIdx=%d\n",
      //    span->column, span->rowSpan,span->colSpan,row->rowIndex(),span->cell->rowIndex());
      if (span->rowSpan>0 && span->column==c &&  // we are at a cell in a row span
          row->rowIndex()>span->cell->rowIndex() // but not the row that started the span
         )
      {
        m_t << "&";
        if (span->colSpan>1) // row span is also part of a column span
        {
          m_t << "\\multicolumn{" << span->colSpan << "}{";
          m_t << "p{(\\linewidth-\\tabcolsep*" 
            << m_numCols << "-\\arrayrulewidth*"
            << row->visibleCells() << ")*" 
            << span->colSpan <<"/"<< m_numCols << "}|}{}";
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
  for (i=0;i<m_rowSpans.count();i++)
  {
    ActiveRowSpan *span = m_rowSpans.at(i);
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

  if (col <= m_numCols)
  {
    m_t << "\\cline{" << col << "-" << m_numCols << "}";
  }

  m_t << "\n";
}

void LatexDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;

  DocHtmlRow *row = 0;
  if (c->parent() && c->parent()->kind()==DocNode::Kind_HtmlRow)
  {
    row = (DocHtmlRow*)c->parent();
  }
  
  m_currentColumn++;

  //Skip columns that span from above.
  uint i;
  for (i=0;i<m_rowSpans.count();i++)
  {
    ActiveRowSpan *span = m_rowSpans.at(i);
    if (span->rowSpan>0 && span->column==m_currentColumn)
    {
      if (row && span->colSpan>1)
      {
        m_t << "\\multicolumn{" << span->colSpan << "}{";
        if (m_currentColumn /*c->columnIndex()*/==1) // add extra | for first column
        {
          m_t << "|";
        }
        m_t << "p{(\\linewidth-\\tabcolsep*" 
            << m_numCols << "-\\arrayrulewidth*"
            << row->visibleCells() << ")*" 
            << span->colSpan <<"/"<< m_numCols << "}|}{}";
        m_currentColumn+=span->colSpan;
      }
      else
      {
        m_currentColumn++;
      }
      m_t << "&";
    }
  }

#if 0
  QMap<int, int>::Iterator it = m_rowspanIndices.find(m_currentColumn);
  if (it!=m_rowspanIndices.end() && it.data()>0)
  {
    m_t << "&";
    m_currentColumn++;
    it++;
  }
#endif

  int cs = c->colSpan();
  if (cs>1 && row)
  {
    m_inColspan = TRUE;
    m_t << "\\multicolumn{" << cs << "}{";
    if (c->columnIndex()==1) // add extra | for first column
    {
      m_t << "|";
    }
    m_t << "p{(\\linewidth-\\tabcolsep*" 
        << m_numCols << "-\\arrayrulewidth*"
        << row->visibleCells() << ")*" 
        << cs <<"/"<< m_numCols << "}|}{";
    if (c->isHeading()) m_t << "\\cellcolor{lightgray}";
  }
  int rs = c->rowSpan();
  if (rs>0)
  {
    m_inRowspan = TRUE;
    //m_rowspanIndices[m_currentColumn] = rs;
    m_rowSpans.append(new ActiveRowSpan(c,rs,cs,m_currentColumn));
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
    m_currentColumn+=cs-1;
  }
}

void LatexDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (m_hide) return;
  if (c->isHeading())
  {
    m_t << "}";
  }
  if (m_inRowspan)
  {
    m_inRowspan = FALSE;
    m_t << "}";
  }
  if (m_inColspan)
  {
    m_inColspan = FALSE;
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
    if (!ref->file().isEmpty()) startLink(ref->ref(),ref->file(),ref->anchor());
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
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  if (pdfHyperlinks)
  {
    m_t << "\\hyperlink{" << ref->file() << "_" << ref->anchor() << "}{" ;
  }
}

void LatexDocVisitor::visitPost(DocSecRefItem *ref) 
{
  if (m_hide) return;
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
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
  m_t << "\\begin{DoxyRefDesc}{";
  filter(x->title());
  m_t << "}" << endl;
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
  m_t << "\\begin{quotation}" << endl;
}

void LatexDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "\\end{quotation}" << endl;
}

void LatexDocVisitor::visitPre(DocVhdlFlow *)
{
  if (m_hide) return;
}

void LatexDocVisitor::visitPost(DocVhdlFlow *)
{
  if (m_hide) return;
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
  if ((i=baseName.find('.'))!=-1)
  {
    baseName=baseName.left(i);
  }
  baseName.prepend("dot_");
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
  else
  {
    m_t << "[width=\\textwidth,height=\\textheight/2,keepaspectratio=true]";
  }
  m_t << "{" << baseName;
  //if (Config_getBool("USE_PDFLATEX"))
  //{
  //  m_t << ".pdf";
  //}
  //else
  //{
  //  m_t << ".eps";
  //}
  m_t << "}";

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

  QCString outDir = Config_getString("LATEX_OUTPUT");
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_EPS); 
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
  else
  {
    m_t << "[width=\\textwidth,height=\\textheight/2,keepaspectratio=true]";
  }
  m_t << "{" << baseName;
  //if (Config_getBool("USE_PDFLATEX"))
  //{
  //  m_t << ".pdf";
  //}
  //else
  //{
  //  m_t << ".eps";
  //}
  m_t << "}";

  if (hasCaption)
  {
    m_t << "\n\\caption{";
  }
}

void LatexDocVisitor::endMscFile(bool hasCaption)
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
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MSC_EPS);
  m_t << "\n\\begin{DoxyImageNoCaption}"
         "  \\mbox{\\includegraphics";
  m_t << "{" << shortName << "}";
  m_t << "}\n"; // end mbox
  m_t << "\\end{DoxyImageNoCaption}\n";
}

