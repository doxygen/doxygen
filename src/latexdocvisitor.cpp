/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

#include "latexdocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "code.h"
#include "dot.h"
#include "util.h"
#include "message.h"

static QString escapeLabelName(const char *s)
{
  QString result;
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

QString LatexDocVisitor::escapeMakeIndexChars(const char *s)
{
  QString result;
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


LatexDocVisitor::LatexDocVisitor(QTextStream &t,BaseCodeDocInterface &ci) 
  : m_t(t), m_ci(ci), m_insidePre(FALSE), m_insideItem(FALSE), m_hide(FALSE) 
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
  endLink();
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
    case DocSymbol::Less:    m_t << "$<$"; break;
    case DocSymbol::Greater: m_t << "$>$"; break;
    case DocSymbol::Amp:     m_t << "\\&"; break;
    case DocSymbol::Dollar:  m_t << "\\$"; break;
    case DocSymbol::Hash:    m_t << "\\#"; break;
    case DocSymbol::Percent: m_t << "\\%"; break;
    case DocSymbol::Copy:    m_t << "\\copyright"; break;
    case DocSymbol::Tm:      m_t << "\\texttrademark"; break;
    case DocSymbol::Reg:     m_t << "\\textregistered"; break;
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
    case DocStyleChange::Preformatted:
      if (s->enable()) 
      {
        m_t << "\\small\\begin{alltt}";
        m_insidePre=TRUE;
      }
      else
      {
        m_insidePre=FALSE;
        m_t << "\\end{alltt}\\normalsize " << endl;
      }
      break;
  }
}

void LatexDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      parseCode(m_ci,s->context(),s->text().latin1(),s->isExample(),s->exampleFile());
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
      m_t << s->text(); 
      break;
  }
}

void LatexDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "\\label{" << anc->anchor() << "}" << endl;
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
    case DocInclude::Include: 
      m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
      parseCode(m_ci,inc->context(),inc->text().latin1(),inc->isExample(),inc->exampleFile());
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

void LatexDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    if (!m_hide) m_t << "\n\n\\footnotesize\\begin{verbatim}"; 
    pushEnabled();
    m_hide = TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    if (!m_hide) parseCode(m_ci,op->context(),op->text().latin1(),op->isExample(),op->exampleFile());
  }
  if (op->isLast())  
  {
    popEnabled();
    if (m_hide) m_t << "\\end{verbatim}\\normalsize" << endl; 
  }
  else
  {
    m_t << endl;
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

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void LatexDocVisitor::visitPre(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "\\begin{enumerate}" << endl;
  }
  else
  {
    m_t << "\\begin{itemize}" << endl;
  }
}

void LatexDocVisitor::visitPost(DocAutoList *l)
{
  if (m_hide) return;
  if (l->isEnumList())
  {
    m_t << "\\end{enumerate}" << endl;
  }
  else
  {
    m_t << "\\end{itemize}" << endl;
  }
}

void LatexDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "\\item ";
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
    case DocSimpleSect::Rcs: break;
    case DocSimpleSect::Unknown:  break;
  }

  // special case 1: user defined title
  if (s->type()!=DocSimpleSect::User && s->type()!=DocSimpleSect::Rcs)
  {
    m_t << ":]";
  }
  else
  {
    m_insideItem=TRUE;
  }
}

void LatexDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  m_t << "\\end{Desc}" << endl;
}

void LatexDocVisitor::visitPre(DocTitle *)
{
}

void LatexDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_insideItem=FALSE;
  m_t << "]";
}

void LatexDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "\\begin{itemize}" << endl;
}

void LatexDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "\\end{itemize}" << endl;
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

void LatexDocVisitor::visitPost(DocSection *) 
{
}

void LatexDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\\begin{enumerate}" << endl; 
  else 
    m_t << "\\begin{itemize}" << endl;
}

void LatexDocVisitor::visitPost(DocHtmlList *s) 
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "\\end{enumerate}" << endl; 
  else 
    m_t << "\\end{itemize}" << endl;
}

void LatexDocVisitor::visitPre(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "\\item ";
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
  m_t << "\\begin{description}" << endl;
}

void LatexDocVisitor::visitPost(DocHtmlDescList *) 
{
  if (m_hide) return;
  m_t << "\\end{description}" << endl;
}

void LatexDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "\\item[";
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
    m_t << "\\\\\\hline\n\\end{TabularC}\n";
  }
}

void LatexDocVisitor::visitPre(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "\\\\\\hline\n\\end{TabularC}\n\\centering\n\\caption{";
}

void LatexDocVisitor::visitPost(DocHtmlCaption *) 
{
  if (m_hide) return;
  m_t << "}\n";
}

void LatexDocVisitor::visitPre(DocHtmlRow *)
{
}

void LatexDocVisitor::visitPost(DocHtmlRow *) 
{
  if (m_hide) return;
  m_t << "\\\\\\hline\n";
}

void LatexDocVisitor::visitPre(DocHtmlCell *)
{
}

void LatexDocVisitor::visitPost(DocHtmlCell *c) 
{
  if (m_hide) return;
  if (!c->isLast()) m_t << "&";
}

void LatexDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  m_t << "\\begin{Desc}" << endl 
    << "\\item[" << theTranslator->trForInternalUseOnly() << "]" << endl;
}

void LatexDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  m_t << "\\end{Desc}" << endl;
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
  if (Config_getBool("COMPACT_LATEX"))
  {
    switch(header->level())
    {
      case 1:  m_t << "\\subsection*{";    break;
      case 2:  m_t << "\\subsubsection*{"; break;
      case 3:  m_t << "\\paragraph*{";     break;
      default: m_t << "\\subparagraph*{";  break;
    }
  }
  else
  {
    switch(header->level())
    {
      case 1:  m_t << "\\section*{"; break;
      case 2:  m_t << "\\subsection*{"; break;
      case 3:  m_t << "\\subsubsection*{"; break;
      case 4:  m_t << "\\paragraph*{"; break;
      default: m_t << "\\subparagraph*{"; break;
    }
  }
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
    pushEnabled();
    m_hide=TRUE;
  }
}

void LatexDocVisitor::visitPost(DocImage *img) 
{
  if (img->type()==DocImage::Latex)
  {
    if (m_hide) return;
    m_t << "}" << endl; // end mbox or caption
    if (img->hasCaption())
    {
      m_t << "\\end{center}" << endl;
      m_t << "\\end{figure}" << endl;
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
  if (baseName.right(4)==".dot")
  {
    baseName=baseName.left(baseName.length()-4);
  }
  QString outDir = Config_getString("LATEX_OUTPUT");
  QString name = df->file();
  writeDotGraphFromFile(name,outDir,baseName,EPS);
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

void LatexDocVisitor::visitPost(DocDotFile *df) 
{
  if (m_hide) return;
  m_t << "}" << endl; // end mbox or caption
  if (df->hasCaption())
  {
    m_t << "\\end{center}" << endl;
    m_t << "\\end{figure}" << endl;
  }
}

void LatexDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void LatexDocVisitor::visitPost(DocLink *) 
{
  if (m_hide) return;
  endLink();
}

void LatexDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  startLink(ref->ref(),ref->file(),ref->anchor());
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void LatexDocVisitor::visitPost(DocRef *) 
{
  if (m_hide) return;
  endLink();
  m_t << " ";
}

void LatexDocVisitor::visitPre(DocSecRefItem *)
{
  if (m_hide) return;
  m_t << "\\item \\contentsline{section}{";
}

void LatexDocVisitor::visitPost(DocSecRefItem *ref) 
{
  if (m_hide) return;
  m_t << "}{\\ref{" << ref->anchor() << "}}{}" << endl;
}

void LatexDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "\\footnotesize" << endl;
  m_t << "\\begin{multicols}{2}" << endl;
  m_t << "\\begin{CompactList}" << endl;
}

void LatexDocVisitor::visitPost(DocSecRefList *) 
{
  if (m_hide) return;
  m_t << "\\end{CompactList}" << endl;
  m_t << "\\end{multicols}" << endl;
  m_t << "\\normalsize" << endl;
}

void LatexDocVisitor::visitPre(DocLanguage *l)
{
  QString langId = Config_getEnum("OUTPUT_LANGUAGE");
  if (l->id().lower()!=langId.lower())
  {
    pushEnabled();
    m_hide = TRUE;
  }
}

void LatexDocVisitor::visitPost(DocLanguage *l) 
{
  QString langId = Config_getEnum("OUTPUT_LANGUAGE");
  if (l->id().lower()!=langId.lower())
  {
    popEnabled();
  }
}

void LatexDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
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

void LatexDocVisitor::visitPost(DocParamSect *)
{
  if (m_hide) return;
  m_t << "\\end{description}" << endl;
  m_t << "\\end{Desc}" << endl;
}

void LatexDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  m_t << "\\item[{\\em ";
  QStrListIterator li(pl->parameters());
  const char *s;
  bool first=TRUE;
  for (li.toFirst();(s=li.current());++li)
  {
    if (!first) m_t << ","; else first=FALSE;
    m_insideItem=TRUE;
    filter(s);
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
  m_t << "\\item[";
  if (Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hyperlink{" << x->file() << "_" << x->anchor() << "}{";
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

void LatexDocVisitor::visitPost(DocInternalRef *) 
{
  if (m_hide) return;
  endLink();
  m_t << " ";
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
  filterLatexString(m_t,str,FALSE,m_insidePre,m_insideItem);
}

void LatexDocVisitor::startLink(const QString &ref,const QString &file,const QString &anchor)
{
  if (ref.isEmpty() && Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hyperlink{";
    if (!file.isEmpty()) m_t << file;
    if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
    if (!anchor.isEmpty()) m_t << anchor;
    m_t << "}{";

  }
  else
  {
    m_t << "{\\bf ";
  }
}

void LatexDocVisitor::endLink()
{
  m_t << "}";
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

