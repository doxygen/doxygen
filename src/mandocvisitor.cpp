/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

ManDocVisitor::ManDocVisitor(QTextStream &t,BaseCodeDocInterface &ci) 
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) 
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void ManDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void ManDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  m_t << "\\fB";
  filter(w->word());
  m_t << "\\fP";
}

void ManDocVisitor::visit(DocWhiteSpace *w)
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

void ManDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  switch(s->symbol())
  {
    case DocSymbol::BSlash:  m_t << "$\\backslash$"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    m_t << "$<$"; break;
    case DocSymbol::Greater: m_t << "$>$"; break;
    case DocSymbol::Amp:     m_t << "\\&"; break;
    case DocSymbol::Dollar:  m_t << "\\$"; break;
    case DocSymbol::Hash:    m_t << "\\#"; break;
    case DocSymbol::Percent: m_t << "\\%"; break;
    case DocSymbol::Copy:    m_t << "\\copyright"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "''"; break;
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
    case DocSymbol::Tilde:   m_t << "\\~{"  << s->letter() << "}"; break;
    case DocSymbol::Szlig:   m_t << "\"s"; break;
    case DocSymbol::Cedil:   m_t << "\\c{" << s->letter() << "}"; break;
    case DocSymbol::Ring:    m_t << "\\" << s->letter() << s->letter(); break;
    case DocSymbol::Nbsp:    m_t << "\\ "; break;
    default:
                             err("Error: unknown symbol found\n");
  }
}

void ManDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\href{" << u->url() << "}";
  }
  m_t << "{\\tt " << u->url() << "}";
}

void ManDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "\\par\n";
}

void ManDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "\n\n";
}

void ManDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "{\\bf ";      else m_t << "} ";
      break;
    case DocStyleChange::Italic:
      if (s->enable()) m_t << "{\\em ";     else m_t << "} ";
      break;
    case DocStyleChange::Code:
      if (s->enable()) m_t << "{\\tt ";   else m_t << "} ";
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
      if (s->enable()) m_t << "\\footnotesize ";  else m_t << "\\normalsize ";
      break;
  }
}

void ManDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      parseCode(m_ci,s->context(),s->text().latin1(),FALSE,0);
      m_t << "\\end{verbatim}\\normalsize" << endl; 
      break;
    case DocVerbatim::Verbatim: 
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      m_t << s->text();
      m_t << "\\end{verbatim}\\normalsize" << endl; 
      break;
    case DocVerbatim::HtmlOnly: 
      /* nothing */ 
      break;
    case DocVerbatim::LatexOnly: 
      /* nothing */ 
      break;
  }
}

void ManDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "\\label{" << anc->anchor() << "}" << endl;
  if (!anc->file().isEmpty() && Config_getBool("PDF_HYPERLINKS")) 
  {
    m_t << "\\hypertarget{" << anc->file() << "_" << anc->anchor() 
      << "}{}" << endl;
  }    
}

void ManDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::Include: 
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      parseCode(m_ci,inc->context(),inc->text().latin1(),FALSE,0);
      m_t << "\\end{verbatim}\\normalsize" << endl; 
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      break;
    case DocInclude::VerbInclude: 
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      m_t << inc->text();
      m_t << "\\end{verbatim}\\normalsize" << endl; 
      break;
  }
}

void ManDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
    m_hide = TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    parseCode(m_ci,op->context(),op->text().latin1(),FALSE,0);
  }
  if (op->isLast())  
  {
    m_hide = FALSE;
    m_t << "\\end{verbatim}\\normalsize" << endl; 
  }
  else
  {
    m_t << endl;
  }
}

void ManDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << f->text();
}

void ManDocVisitor::visit(DocIndexEntry *i)
{
  m_t << "\\index{" << i->entry() << "@{";
  m_t << "}}";
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void ManDocVisitor::visitPre(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "\\begin{enumerate}" << endl;
  }
  else
  {
    m_t << "\\begin{itemize}" << endl;
  }
}

void ManDocVisitor::visitPost(DocAutoList *l)
{
  if (l->isEnumList())
  {
    m_t << "\\end{enumerate}" << endl;
  }
  else
  {
    m_t << "\\end{itemize}" << endl;
  }
}

void ManDocVisitor::visitPre(DocAutoListItem *)
{
  m_t << "\\item ";
}

void ManDocVisitor::visitPost(DocAutoListItem *) 
{
}

void ManDocVisitor::visitPre(DocPara *) 
{
}

void ManDocVisitor::visitPost(DocPara *p)
{
  if (!p->isLast() &&            // omit <p> for last paragraph
      !(p->parent() &&           // and for parameter sections
        p->parent()->kind()==DocNode::Kind_ParamSect
       )
     ) m_t << endl << endl;
}

void ManDocVisitor::visitPre(DocRoot *)
{
}

void ManDocVisitor::visitPost(DocRoot *)
{
}

void ManDocVisitor::visitPre(DocSimpleSect *s)
{
  m_t << "\\begin{Desc}\n\\item[";
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
    case DocSimpleSect::Invar:
      m_t << theTranslator->trInvariant(); break;
    case DocSimpleSect::Remark:
      m_t << theTranslator->trRemarks(); break;
    case DocSimpleSect::Attention:
      m_t << theTranslator->trAttention(); break;
    case DocSimpleSect::User: break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s->type()!=DocSimpleSect::User)
  {
    m_t << ":]";
  }
}

void ManDocVisitor::visitPost(DocSimpleSect *)
{
  m_t << "\\end{Desc}" << endl;
}

void ManDocVisitor::visitPre(DocTitle *)
{
}

void ManDocVisitor::visitPost(DocTitle *)
{
  m_t << "]";
}

void ManDocVisitor::visitPre(DocSimpleList *)
{
  m_t << "\\begin{itemize}" << endl;
}

void ManDocVisitor::visitPost(DocSimpleList *)
{
  m_t << "\\end{itemize}" << endl;
}

void ManDocVisitor::visitPre(DocSimpleListItem *)
{
  m_t << "\\item ";
}

void ManDocVisitor::visitPost(DocSimpleListItem *) 
{
}

void ManDocVisitor::visitPre(DocSection *s)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hypertarget{" << s->file() << "_" << s->anchor() << "}{}";
  }
  if (Config_getBool("COMPACT_LATEX"))
  {
    switch(s->level())
    {
      case 1: m_t << "\\subsubsection{"; break;
      case 2: m_t << "\\paragraph{";     break;
      case 3: m_t << "\\subparagraph{";  break;
      case 4: m_t << "\\subparagraph{";  break;
    }
  }
  else
  {
    switch(s->level())
    {
      case 1: m_t << "\\subsection{";    break;
      case 2: m_t << "\\subsubsection{"; break;
      case 3: m_t << "\\paragraph{";     break;
      case 4: m_t << "\\subparagraph{";  break;
    }
  }
  filter(s->title());
  m_t << "}\\label{" << s->anchor() << "}" << endl;
}

void ManDocVisitor::visitPost(DocSection *) 
{
}

void ManDocVisitor::visitPre(DocHtmlList *s)
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\\begin{enumerate}" << endl; 
  else 
    m_t << "\\begin{itemize}" << endl;
}

void ManDocVisitor::visitPost(DocHtmlList *s) 
{
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\\end{enumerate}" << endl; 
  else 
    m_t << "\\end{itemize}" << endl;
}

void ManDocVisitor::visitPre(DocHtmlListItem *)
{
  m_t << "\\item ";
}

void ManDocVisitor::visitPost(DocHtmlListItem *) 
{
}

void ManDocVisitor::visitPre(DocHtmlPre *)
{
  m_t << "\\small\\begin{alltt}";
  m_insidePre=TRUE;
}

void ManDocVisitor::visitPost(DocHtmlPre *) 
{
  m_insidePre=FALSE;
  m_t << "\\end{alltt}\\normalsize " << endl;
}

void ManDocVisitor::visitPre(DocHtmlDescList *)
{
  m_t << "\\begin{description}" << endl;
}

void ManDocVisitor::visitPost(DocHtmlDescList *) 
{
  m_t << "\\end{description}" << endl;
}

void ManDocVisitor::visitPre(DocHtmlDescTitle *)
{
  m_t << "\\item[";
}

void ManDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  m_t << "]";
}

void ManDocVisitor::visitPre(DocHtmlDescData *)
{
}

void ManDocVisitor::visitPost(DocHtmlDescData *) 
{
}

void ManDocVisitor::visitPre(DocHtmlTable *t)
{
  if (t->hasCaption()) 
  {
    m_t << "\\begin{table}[h]";
  }
  m_t << "\\begin{TabularC}{" << t->numCols() << "}\n\\hline\n";
}

void ManDocVisitor::visitPost(DocHtmlTable *t) 
{
  if (t->hasCaption())
  {
    m_t << "\\end{table}\n";
  }
  else
  {
    m_t << "\\\\\\hline\n\\end{TabularC}\n";
  }
}

void ManDocVisitor::visitPre(DocHtmlCaption *)
{
  m_t << "\\\\\\hline\n\\end{TabularC}\n\\centering\n\\caption{";
}

void ManDocVisitor::visitPost(DocHtmlCaption *) 
{
  m_t << "}\n";
}

void ManDocVisitor::visitPre(DocHtmlRow *)
{
}

void ManDocVisitor::visitPost(DocHtmlRow *) 
{
  m_t << "\\\\\\hline\n";
}

void ManDocVisitor::visitPre(DocHtmlCell *)
{
}

void ManDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (!c->isLast()) m_t << "&";
}

void ManDocVisitor::visitPre(DocInternal *)
{
  m_t << "\\begin{Desc}" << endl 
    << "\\item[" << theTranslator->trForInternalUseOnly() << "]" << endl;
}

void ManDocVisitor::visitPost(DocInternal *) 
{
  m_t << "\\end{Desc}" << endl;
}

void ManDocVisitor::visitPre(DocHRef *href)
{
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\href{";
    m_t << href->url();
    m_t << "}";
  }
  m_t << "{\\tt ";
}

void ManDocVisitor::visitPost(DocHRef *) 
{
  m_t << "}";
}

void ManDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (Config_getBool("COMPACT_LATEX"))
  {
    switch(header->level())
    {
      case 1: m_t << "\\subsection*{"; break;
      case 2: m_t << "\\subsubsection*{"; break;
      case 3: m_t << "\\paragraph*{"; break;
    }
  }
  else
  {
    switch(header->level())
    {
      case 1: m_t << "\\section*{"; break;
      case 2: m_t << "\\subsection*{"; break;
      case 3: m_t << "\\subsubsection*{"; break;
    }
  }
}

void ManDocVisitor::visitPost(DocHtmlHeader *) 
{
  m_t << "}";
}

void ManDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Latex)
  {
    if (img->hasCaption())
    {
      m_t << "\\begin{figure}[H]" << endl;
      m_t << "\\begin{center}" << endl;
    }
    else
    {
      m_t << "\\mbox{";
    }
    QString gfxName = img->name();
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
      m_t << "\\caption{";
    }
  }
  else // other format -> skip
  {
    m_hide=TRUE;
  }
}

void ManDocVisitor::visitPost(DocImage *img) 
{
  if (img->type()==DocImage::Latex)
  {
    m_t << "}" << endl; // end mbox or caption
    if (img->hasCaption())
    {
      m_t << "\\end{center}" << endl;
      m_t << "\\end{figure}" << endl;
    }
  }
  else // other format
  {
    m_hide=FALSE;
  }
}

void ManDocVisitor::visitPre(DocDotFile *df)
{
  QString baseName=df->file();
  int i;
  if ((i=baseName.findRev('/'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  } 
  if (baseName.right(4)==".eps" || baseName.right(4)==".pdf")
  {
    baseName=baseName.left(baseName.length()-4);
  }
  QString outDir = Config_getString("LATEX_OUTPUT");
  writeDotGraphFromFile(df->file(),outDir,baseName,EPS);
  if (df->hasCaption())
  {
    m_t << "\\begin{figure}[H]" << endl;
    m_t << "\\begin{center}" << endl;
  }
  else
  {
    m_t << "\\mbox{";
  }
  m_t << "\\includegraphics";
  if (!df->width().isEmpty())
  {
    m_t << "[width=" << df->width() << "]";
  }
  else if (!df->height().isEmpty())
  {
    m_t << "[height=" << df->height() << "]";
  }
  m_t << "{" << baseName << "}";

  if (df->hasCaption())
  {
    m_t << "\\caption{";
  }
}

void ManDocVisitor::visitPost(DocDotFile *df) 
{
  m_t << "}" << endl; // end mbox or caption
  if (df->hasCaption())
  {
    m_t << "\\end{center}" << endl;
    m_t << "\\end{figure}" << endl;
  }
}

void ManDocVisitor::visitPre(DocLink *)
{
  m_t << "\\fB ";
}

void ManDocVisitor::visitPost(DocLink *) 
{
  m_t << "\\fP ";
}

void ManDocVisitor::visitPre(DocRef *ref)
{
  m_t << "\\fB ";
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void ManDocVisitor::visitPost(DocRef *) 
{
  m_t << "\\fP ";
}

void ManDocVisitor::visitPre(DocSecRefItem *)
{
  m_t << "\\item \\contentsline{section}{";
}

void ManDocVisitor::visitPost(DocSecRefItem *ref) 
{
  m_t << "}{\\ref{" << ref->anchor() << "}}{}" << endl;
}

void ManDocVisitor::visitPre(DocSecRefList *)
{
  m_t << "\\footnotesize" << endl;
  m_t << "\\begin{multicols}{2}" << endl;
  m_t << "\\begin{CompactList}" << endl;
}

void ManDocVisitor::visitPost(DocSecRefList *) 
{
  m_t << "\\end{CompactList}" << endl;
  m_t << "\\end{multicols}" << endl;
  m_t << "\\normalsize" << endl;
}

void ManDocVisitor::visitPre(DocLanguage *)
{
}

void ManDocVisitor::visitPost(DocLanguage *) 
{
}

void ManDocVisitor::visitPre(DocParamSect *s)
{
  m_t << "\\begin{Desc}" << endl;
  m_t << "\\item[";
  switch(s->type())
  {
    case DocParamSect::Param: 
      m_t << theTranslator->trParameters(); break;
    case DocParamSect::RetVal: 
      m_t << theTranslator->trReturnValues(); break;
    case DocParamSect::Exception: 
      m_t << theTranslator->trExceptions(); break;
    default:
      ASSERT(0);
  }
  m_t << ":]" << endl;
  m_t << "\\begin{description}" << endl;
}

void ManDocVisitor::visitPost(DocParamSect *)
{
  m_t << "\\end{description}" << endl;
  m_t << "\\end{Desc}" << endl;
}

void ManDocVisitor::visitPre(DocParamList *pl)
{
  m_t << "\\item[{\\em ";
  QStrListIterator li(pl->parameters());
  const char *s;
  bool first=TRUE;
  for (li.toFirst();(s=li.current());++li)
  {
    if (!first) m_t << ","; else first=FALSE;
    m_t << s;
  }
  m_t << "}]";
}

void ManDocVisitor::visitPost(DocParamList *)
{
}

void ManDocVisitor::visitPre(DocXRefItem *x)
{
  m_t << "\\begin{Desc}" << endl;
  m_t << "\\item[";
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hyperlink{" << x->file() << "_" << x->anchor() << "}{";
  }
  else
  {
    m_t << "{\\bf ";
  }
  filter(x->title());
  m_t << "}]";
}

void ManDocVisitor::visitPost(DocXRefItem *)
{
  m_t << "\\end{Desc}" << endl;
}

void ManDocVisitor::visitPre(DocInternalRef *)
{
  m_t << "\\fB ";
}

void ManDocVisitor::visitPost(DocInternalRef *) 
{
  m_t << "\\fP ";
}

void ManDocVisitor::visitPre(DocCopy *)
{
}

void ManDocVisitor::visitPost(DocCopy *)
{
}

void ManDocVisitor::filter(const char *str)
{ 
  if (str)
  {
    const char *p=str;
    char c=0;
    while ((c=*p++)) 
    {
      switch(c)
      {
        case '\\': m_t << "\\\\"; break;
        case '"':  c = '\''; // fall through
        default: m_t << c; break;
      }
    }
  }
}

