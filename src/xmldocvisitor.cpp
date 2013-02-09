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

#include <qfileinfo.h> 

#include "xmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "doxygen.h"
#include "outputgen.h"
#include "xmlgen.h"
#include "dot.h"
#include "message.h"
#include "util.h"
#include "parserintf.h"
#include "filename.h"
#include "config.h"

XmlDocVisitor::XmlDocVisitor(FTextStream &t,CodeOutputInterface &ci) 
  : DocVisitor(DocVisitor_XML), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE) 
{
}

  //--------------------------------------
  // visitor functions for leaf nodes
  //--------------------------------------

void XmlDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void XmlDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  startLink(w->ref(),w->file(),w->anchor());
  filter(w->word());
  endLink();
}

void XmlDocVisitor::visit(DocWhiteSpace *w)
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

void XmlDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  switch(s->symbol())
  {
    case DocSymbol::BSlash:        m_t << "\\"; break;
    case DocSymbol::At:            m_t << "@"; break;
    case DocSymbol::Less:          m_t << "&lt;"; break;
    case DocSymbol::Greater:       m_t << "&gt;"; break;
    case DocSymbol::Amp:           m_t << "&amp;"; break;
    case DocSymbol::Dollar:        m_t << "$"; break;
    case DocSymbol::Hash:          m_t << "#"; break;
    case DocSymbol::DoubleColon:   m_t << "::"; break;
    case DocSymbol::Percent:       m_t << "%"; break;
    case DocSymbol::Pipe:          m_t << "|"; break;
    case DocSymbol::Copy:          m_t << "<copy/>"; break;
    case DocSymbol::Tm:            m_t << "<trademark/>"; break;
    case DocSymbol::Reg:           m_t << "<registered/>"; break;
    case DocSymbol::Apos:          m_t << "'"; break;
    case DocSymbol::Quot:          m_t << "\""; break;
    case DocSymbol::Lsquo:         m_t << "<lsquo/>"; break;
    case DocSymbol::Rsquo:         m_t << "<rsquo/>"; break;
    case DocSymbol::Ldquo:         m_t << "<ldquo/>"; break;
    case DocSymbol::Rdquo:         m_t << "<rdquo/>"; break;
    case DocSymbol::Ndash:         m_t << "<ndash/>"; break;
    case DocSymbol::Mdash:         m_t << "<mdash/>"; break;
    case DocSymbol::Uml:           m_t << "<umlaut char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Acute:         m_t << "<acute char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Grave:         m_t << "<grave char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Circ:          m_t << "<circ char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Tilde:         m_t << "<tilde char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Szlig:         m_t << "<szlig/>"; break;
    case DocSymbol::Cedil:         m_t << "<cedil char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Ring:          m_t << "<ring char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Slash:         m_t << "<slash char=\"" << s->letter() << "\"/>"; break;
    case DocSymbol::Nbsp:          m_t << "<nonbreakablespace/>"; break;
    case DocSymbol::Aelig:         m_t << "<aelig/>"; break;
    case DocSymbol::AElig:         m_t << "<AElig/>"; break;
    case DocSymbol::GrkGamma:      m_t << "<Gamma>"; break;
    case DocSymbol::GrkDelta:      m_t << "<Delta>"; break;
    case DocSymbol::GrkTheta:      m_t << "<Theta>"; break;
    case DocSymbol::GrkLambda:     m_t << "<Lambda>"; break;
    case DocSymbol::GrkXi:         m_t << "<Xi>"; break;
    case DocSymbol::GrkPi:         m_t << "<Pi>"; break;
    case DocSymbol::GrkSigma:      m_t << "<Sigma>"; break;
    case DocSymbol::GrkUpsilon:    m_t << "<Upsilon>"; break;
    case DocSymbol::GrkPhi:        m_t << "<Phi>"; break;
    case DocSymbol::GrkPsi:        m_t << "<Psi>"; break;
    case DocSymbol::GrkOmega:      m_t << "<Omega>"; break;
    case DocSymbol::Grkalpha:      m_t << "<alpha>"; break;
    case DocSymbol::Grkbeta:       m_t << "<beta>"; break;
    case DocSymbol::Grkgamma:      m_t << "<gamma>"; break;
    case DocSymbol::Grkdelta:      m_t << "<delta>"; break;
    case DocSymbol::Grkepsilon:    m_t << "<epsilon>"; break;
    case DocSymbol::Grkzeta:       m_t << "<zeta>"; break;
    case DocSymbol::Grketa:        m_t << "<eta>"; break;
    case DocSymbol::Grktheta:      m_t << "<theta>"; break;
    case DocSymbol::Grkiota:       m_t << "<iota>"; break;
    case DocSymbol::Grkkappa:      m_t << "<kappa>"; break;
    case DocSymbol::Grklambda:     m_t << "<lambda>"; break;
    case DocSymbol::Grkmu:         m_t << "<mu>"; break;
    case DocSymbol::Grknu:         m_t << "<nu>"; break;
    case DocSymbol::Grkxi:         m_t << "<xi>"; break;
    case DocSymbol::Grkpi:         m_t << "<pi>"; break;
    case DocSymbol::Grkrho:        m_t << "<rho>"; break;
    case DocSymbol::Grksigma:      m_t << "<sigma>"; break;
    case DocSymbol::Grktau:        m_t << "<tau>"; break;
    case DocSymbol::Grkupsilon:    m_t << "<upsilon>"; break;
    case DocSymbol::Grkphi:        m_t << "<phi>"; break;
    case DocSymbol::Grkchi:        m_t << "<chi>"; break;
    case DocSymbol::Grkpsi:        m_t << "<psi>"; break;
    case DocSymbol::Grkomega:      m_t << "<omega>"; break;
    case DocSymbol::Grkvarsigma:   m_t << "<sigmaf>"; break;
    case DocSymbol::Section:       m_t << "<sect>"; break;
    case DocSymbol::Degree:        m_t << "<deg>"; break;
    case DocSymbol::Prime:         m_t << "<prime>"; break;
    case DocSymbol::DoublePrime:   m_t << "<Prime>"; break;
    case DocSymbol::Infinity:      m_t << "<infin>"; break;
    case DocSymbol::EmptySet:      m_t << "<empty>"; break;
    case DocSymbol::PlusMinus:     m_t << "<plusmn>"; break;
    case DocSymbol::Times:         m_t << "<times>"; break;
    case DocSymbol::Minus:         m_t << "<minus>"; break;
    case DocSymbol::CenterDot:     m_t << "<sdot>"; break;
    case DocSymbol::Partial:       m_t << "<part>"; break;
    case DocSymbol::Nabla:         m_t << "<nabla>"; break;
    case DocSymbol::SquareRoot:    m_t << "<radic>"; break;
    case DocSymbol::Perpendicular: m_t << "<perp>"; break;
    case DocSymbol::Sum:           m_t << "<sum>"; break;
    case DocSymbol::Integral:      m_t << "<int>"; break;
    case DocSymbol::Product:       m_t << "<prod>"; break;
    case DocSymbol::Similar:       m_t << "<sim>"; break;
    case DocSymbol::Approx:        m_t << "<asymp>"; break;
    case DocSymbol::NotEqual:      m_t << "<ne>"; break;
    case DocSymbol::Equivalent:    m_t << "<equiv>"; break;
    case DocSymbol::Proportional:  m_t << "<prop>"; break;
    case DocSymbol::LessEqual:     m_t << "<le>"; break;
    case DocSymbol::GreaterEqual:  m_t << "<ge>"; break;
    case DocSymbol::LeftArrow:     m_t << "<larr>"; break;
    case DocSymbol::RightArrow:    m_t << "<rarr>"; break;
    case DocSymbol::SetIn:         m_t << "<isin>"; break;
    case DocSymbol::SetNotIn:      m_t << "<notin>"; break;
    case DocSymbol::LeftCeil:      m_t << "<lceil>"; break;
    case DocSymbol::RightCeil:     m_t << "<rceil>"; break;
    case DocSymbol::LeftFloor:     m_t << "<lfloor>"; break;
    case DocSymbol::RightFloor:    m_t << "<rfloor>"; break;
    default:
                             err("error: unknown symbol found\n");
  }
}

void XmlDocVisitor::visit(DocURL *u)
{
  if (m_hide) return;
  m_t << "<ulink url=\""; 
  if (u->isEmail()) m_t << "mailto:";
  filter(u->url());
  m_t << "\">";
  filter(u->url());
  m_t << "</ulink>";
}

void XmlDocVisitor::visit(DocLineBreak *)
{
  if (m_hide) return;
  m_t << "<linebreak/>\n";
}

void XmlDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "<hruler/>\n";
}

void XmlDocVisitor::visit(DocStyleChange *s)
{
  if (m_hide) return;
  switch (s->style())
  {
    case DocStyleChange::Bold:
      if (s->enable()) m_t << "<bold>";      else m_t << "</bold>";
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
      if (s->enable()) m_t << "<center>"; else m_t << "</center>";
      break;
    case DocStyleChange::Small:
      if (s->enable()) m_t << "<small>";  else m_t << "</small>";
      break;
    case DocStyleChange::Preformatted:
      if (s->enable()) 
      {
        m_t << "<preformatted>";  
        m_insidePre=TRUE;
      }
      else 
      {
        m_t << "</preformatted>";
        m_insidePre=FALSE;
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void XmlDocVisitor::visit(DocVerbatim *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocVerbatim::Code: // fall though
      m_t << "<programlisting>"; 
      Doxygen::parserManager->getParser(m_langExt)
                            ->parseCode(m_ci,s->context(),s->text(),
                                        s->isExample(),s->exampleFile());
      m_t << "</programlisting>"; 
      break;
    case DocVerbatim::Verbatim: 
      m_t << "<verbatim>";
      filter(s->text());
      m_t << "</verbatim>"; 
      break;
    case DocVerbatim::HtmlOnly: 
      m_t << "<htmlonly>";
      filter(s->text());
      m_t << "</htmlonly>";
      break;
    case DocVerbatim::RtfOnly: 
      m_t << "<rtfonly>";
      filter(s->text());
      m_t << "</rtfonly>";
      break;
    case DocVerbatim::ManOnly: 
      m_t << "<manonly>";
      filter(s->text());
      m_t << "</manonly>";
      break;
    case DocVerbatim::LatexOnly: 
      m_t << "<latexonly>";
      filter(s->text());
      m_t << "</latexonly>";
      break;
    case DocVerbatim::XmlOnly: 
      m_t << s->text();
      break;
    case DocVerbatim::DocbookOnly:
      m_t << "<docbookonly>";
      filter(s->text());
      m_t << "</docbookonly>";
      break;
    case DocVerbatim::Dot: 
      m_t << "<dot>";
      filter(s->text());
      m_t << "</dot>";
      break;
    case DocVerbatim::Msc: 
      m_t << "<msc>";
      filter(s->text());
      m_t << "</msc>";
      break;
  }
}

void XmlDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<anchor id=\"" << anc->file() << "_1" << anc->anchor() << "\"/>";
}

void XmlDocVisitor::visit(DocInclude *inc)
{
  if (m_hide) return;
  switch(inc->type())
  {
    case DocInclude::IncWithLines:
      { 
         m_t << "<programlisting>";
         QFileInfo cfi( inc->file() );
         FileDef fd( cfi.dirPath().utf8(), cfi.fileName().utf8() );
         Doxygen::parserManager->getParser(inc->extension())
                               ->parseCode(m_ci,inc->context(),
                                           inc->text(),
                                           inc->isExample(),
                                           inc->exampleFile(), &fd);
         m_t << "</programlisting>"; 
      }
      break;    
    case DocInclude::Include: 
      m_t << "<programlisting>";
      Doxygen::parserManager->getParser(inc->extension())
                            ->parseCode(m_ci,inc->context(),
                                        inc->text(),
                                        inc->isExample(),
                                        inc->exampleFile());
      m_t << "</programlisting>"; 
      break;
    case DocInclude::DontInclude: 
      break;
    case DocInclude::HtmlInclude: 
      m_t << "<htmlonly>";
      filter(inc->text());
      m_t << "</htmlonly>";
      break;
    case DocInclude::VerbInclude: 
      m_t << "<verbatim>";
      filter(inc->text());
      m_t << "</verbatim>"; 
      break;
    case DocInclude::Snippet:
      m_t << "<programlisting>";
      Doxygen::parserManager->getParser(inc->extension())
                            ->parseCode(m_ci,
                                        inc->context(),
                                        extractBlock(inc->text(),inc->blockId()),
                                        inc->isExample(),
                                        inc->exampleFile()
                                       );
      m_t << "</programlisting>"; 
      break;
  }
}

void XmlDocVisitor::visit(DocIncOperator *op)
{
  //printf("DocIncOperator: type=%d first=%d, last=%d text=`%s'\n",
  //    op->type(),op->isFirst(),op->isLast(),op->text().data());
  if (op->isFirst()) 
  {
    if (!m_hide)
    {
      m_t << "<programlisting>";
    }
    pushEnabled();
    m_hide = TRUE;
  }
  if (op->type()!=DocIncOperator::Skip) 
  {
    popEnabled();
    if (!m_hide) 
    {
      Doxygen::parserManager->getParser(m_langExt)
                            ->parseCode(m_ci,op->context(),
                                        op->text(),op->isExample(),
                                        op->exampleFile());
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

void XmlDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << "<formula id=\"" << f->id() << "\">";
  filter(f->text());
  m_t << "</formula>";
}

void XmlDocVisitor::visit(DocIndexEntry *ie)
{
  if (m_hide) return;
  m_t << "<indexentry>"
           "<primaryie>";
  filter(ie->entry());
  m_t << "</primaryie>"
           "<secondaryie></secondaryie>"
         "</indexentry>";
}

void XmlDocVisitor::visit(DocSimpleSectSep *)
{
  m_t << "<simplesectsep/>";
}

void XmlDocVisitor::visit(DocCite *cite)
{
  if (m_hide) return;
  if (!cite->file().isEmpty()) startLink(cite->ref(),cite->file(),cite->anchor());
  filter(cite->text());
  if (!cite->file().isEmpty()) endLink();
}

//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void XmlDocVisitor::visitPre(DocAutoList *l)
{
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

void XmlDocVisitor::visitPost(DocAutoList *l)
{
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

void XmlDocVisitor::visitPre(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocAutoListItem *) 
{
  if (m_hide) return;
  m_t << "</listitem>";
}

void XmlDocVisitor::visitPre(DocPara *) 
{
  if (m_hide) return;
  m_t << "<para>";
}

void XmlDocVisitor::visitPost(DocPara *)
{
  if (m_hide) return;
  m_t << "</para>";
}

void XmlDocVisitor::visitPre(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
}

void XmlDocVisitor::visitPost(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
}

void XmlDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
  m_t << "<simplesect kind=\"";
  switch(s->type())
  {
    case DocSimpleSect::See: 
      m_t << "see"; break;
    case DocSimpleSect::Return: 
      m_t << "return"; break;
    case DocSimpleSect::Author: 
      m_t << "author"; break;
    case DocSimpleSect::Authors: 
      m_t << "authors"; break;
    case DocSimpleSect::Version: 
      m_t << "version"; break;
    case DocSimpleSect::Since: 
      m_t << "since"; break;
    case DocSimpleSect::Date: 
      m_t << "date"; break;
    case DocSimpleSect::Note: 
      m_t << "note"; break;
    case DocSimpleSect::Warning:
      m_t << "warning"; break;
    case DocSimpleSect::Pre:
      m_t << "pre"; break;
    case DocSimpleSect::Post:
      m_t << "post"; break;
    case DocSimpleSect::Copyright:
      m_t << "copyright"; break;
    case DocSimpleSect::Invar:
      m_t << "invariant"; break;
    case DocSimpleSect::Remark:
      m_t << "remark"; break;
    case DocSimpleSect::Attention:
      m_t << "attention"; break;
    case DocSimpleSect::User: 
      m_t << "par"; break;
    case DocSimpleSect::Rcs: 
      m_t << "rcs"; break;
    case DocSimpleSect::Unknown:  break;
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  m_t << "</simplesect>\n";
}

void XmlDocVisitor::visitPre(DocTitle *)
{
  if (m_hide) return;
  m_t << "<title>";
}

void XmlDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "</title>";
}

void XmlDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocSimpleListItem *) 
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  m_t << "<sect" << s->level() << " id=\"" << s->file();
  if (!s->anchor().isEmpty()) m_t << "_1" << s->anchor();
  m_t << "\">" << endl;
  m_t << "<title>";
  filter(s->title());
  m_t << "</title>" << endl;
}

void XmlDocVisitor::visitPost(DocSection *s) 
{
  m_t << "</sect" << s->level() << ">\n";
}

void XmlDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "<orderedlist>\n"; 
  else 
    m_t << "<itemizedlist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlList *s) 
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered) 
    m_t << "</orderedlist>\n"; 
  else 
    m_t << "</itemizedlist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>\n";
}

void XmlDocVisitor::visitPost(DocHtmlListItem *) 
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "<variablelist>\n";
}

void XmlDocVisitor::visitPost(DocHtmlDescList *) 
{
  if (m_hide) return;
  m_t << "</variablelist>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "<varlistentry><term>";
}

void XmlDocVisitor::visitPost(DocHtmlDescTitle *) 
{
  if (m_hide) return;
  m_t << "</term></varlistentry>\n";
}

void XmlDocVisitor::visitPre(DocHtmlDescData *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void XmlDocVisitor::visitPost(DocHtmlDescData *) 
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void XmlDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;
  m_t << "<table rows=\"" << t->numRows() 
      << "\" cols=\"" << t->numColumns() << "\">" ;
}

void XmlDocVisitor::visitPost(DocHtmlTable *) 
{
  if (m_hide) return;
  m_t << "</table>\n";
}

void XmlDocVisitor::visitPre(DocHtmlRow *)
{
  if (m_hide) return;
  m_t << "<row>\n";
}

void XmlDocVisitor::visitPost(DocHtmlRow *) 
{
  if (m_hide) return;
  m_t << "</row>\n";
}

void XmlDocVisitor::visitPre(DocHtmlCell *c)
{
  if (m_hide) return;
  if (c->isHeading()) m_t << "<entry thead=\"yes\">"; else m_t << "<entry thead=\"no\">";
}

void XmlDocVisitor::visitPost(DocHtmlCell *) 
{
  if (m_hide) return;
  m_t << "</entry>"; 
}

void XmlDocVisitor::visitPre(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "<caption>";
}

void XmlDocVisitor::visitPost(DocHtmlCaption *) 
{
  if (m_hide) return;
  m_t << "</caption>\n";
}

void XmlDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  m_t << "<internal>";
}

void XmlDocVisitor::visitPost(DocInternal *) 
{
  if (m_hide) return;
  m_t << "</internal>" << endl;
}

void XmlDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  m_t << "<ulink url=\"" << href->url() << "\">";
}

void XmlDocVisitor::visitPost(DocHRef *) 
{
  if (m_hide) return;
  m_t << "</ulink>";
}

void XmlDocVisitor::visitPre(DocHtmlHeader *header)
{
  if (m_hide) return;
  m_t << "<heading level=\"" << header->level() << "\">";
}

void XmlDocVisitor::visitPost(DocHtmlHeader *) 
{
  if (m_hide) return;
  m_t << "</heading>\n";
}

void XmlDocVisitor::visitPre(DocImage *img)
{
  if (m_hide) return;
  m_t << "<image type=\"";
  switch(img->type())
  {
    case DocImage::Html:  m_t << "html"; break;
    case DocImage::Latex: m_t << "latex"; break;
    case DocImage::Rtf:   m_t << "rtf"; break;
  }
  m_t << "\"";

  QCString baseName=img->name();
  int i;
  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1);
  }
  m_t << " name=\"" << baseName << "\"";
  if (!img->width().isEmpty())
  {
    m_t << " width=\"";
    filter(img->width());
    m_t << "\"";
  }
  else if (!img->height().isEmpty())
  {
    m_t << " height=\"";
    filter(img->height());
    m_t << "\"";
  }
  m_t << ">";

  // copy the image to the output dir
  QFile inImage(img->name());
  QFile outImage(Config_getString("XML_OUTPUT")+"/"+baseName.data());
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

void XmlDocVisitor::visitPost(DocImage *) 
{
  if (m_hide) return;
  m_t << "</image>" << endl;
}

void XmlDocVisitor::visitPre(DocDotFile *df)
{
  if (m_hide) return;
  m_t << "<dotfile name=\"" << df->file() << "\">";
}

void XmlDocVisitor::visitPost(DocDotFile *) 
{
  if (m_hide) return;
  m_t << "</dotfile>" << endl;
}

void XmlDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  m_t << "<mscfile name=\"" << df->file() << "\">";
}

void XmlDocVisitor::visitPost(DocMscFile *) 
{
  if (m_hide) return;
  m_t << "</mscfile>" << endl;
}
void XmlDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->ref(),lnk->file(),lnk->anchor());
}

void XmlDocVisitor::visitPost(DocLink *) 
{
  if (m_hide) return;
  endLink();
}

void XmlDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) 
  {
    startLink(ref->ref(),ref->file(),ref->isSubPage() ? QCString() : ref->anchor());
  }
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void XmlDocVisitor::visitPost(DocRef *ref) 
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink();
  //m_t << " ";
}

void XmlDocVisitor::visitPre(DocSecRefItem *ref)
{
  if (m_hide) return;
  m_t << "<tocitem id=\"" << ref->file() << "_1" << ref->anchor() << "\">";
}

void XmlDocVisitor::visitPost(DocSecRefItem *) 
{
  if (m_hide) return;
  m_t << "</tocitem>" << endl;
}

void XmlDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "<toclist>" << endl;
}

void XmlDocVisitor::visitPost(DocSecRefList *) 
{
  if (m_hide) return;
  m_t << "</toclist>" << endl;
}

//void XmlDocVisitor::visitPre(DocLanguage *l)
//{
//  if (m_hide) return;
//  m_t << "<language langid=\"" << l->id() << "\">";
//}
//
//void XmlDocVisitor::visitPost(DocLanguage *) 
//{
//  if (m_hide) return;
//  m_t << "</language>" << endl;
//}

void XmlDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  m_t << "<parameterlist kind=\"";
  switch(s->type())
  {
    case DocParamSect::Param: 
      m_t << "param"; break;
    case DocParamSect::RetVal: 
      m_t << "retval"; break;
    case DocParamSect::Exception: 
      m_t << "exception"; break;
    case DocParamSect::TemplateParam: 
      m_t << "templateparam"; break;
    default:
      ASSERT(0);
  }
  m_t << "\">";
}

void XmlDocVisitor::visitPost(DocParamSect *)
{
  if (m_hide) return;
  m_t << "</parameterlist>" << endl;
}

void XmlDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  m_t << "<parameteritem>" << endl;
  m_t << "<parameternamelist>" << endl;
  //QStrListIterator li(pl->parameters());
  //const char *s;
  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  for (li.toFirst();(param=li.current());++li)
  {
    if (pl->paramTypes().count()>0)
    {
      QListIterator<DocNode> li(pl->paramTypes());
      DocNode *type;
      for (li.toFirst();(type=li.current());++li)
      {
        m_t << "<parametertype>";
        if (type->kind()==DocNode::Kind_Word)
        {
          visit((DocWord*)type); 
        }
        else if (type->kind()==DocNode::Kind_LinkedWord)
        {
          visit((DocLinkedWord*)type); 
        }
        m_t << "</parametertype>" << endl;
      }
    }
    m_t << "<parametername";
    if (pl->direction()!=DocParamSect::Unspecified)
    {
      m_t << " direction=\"";
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
        m_t << "inout";
      }
      m_t << "\"";
    }
    m_t << ">";
    if (param->kind()==DocNode::Kind_Word)
    {
      visit((DocWord*)param); 
    }
    else if (param->kind()==DocNode::Kind_LinkedWord)
    {
      visit((DocLinkedWord*)param); 
    }
    m_t << "</parametername>" << endl;
  }
  m_t << "</parameternamelist>" << endl;
  m_t << "<parameterdescription>" << endl;
}

void XmlDocVisitor::visitPost(DocParamList *)
{
  if (m_hide) return;
  m_t << "</parameterdescription>" << endl;
  m_t << "</parameteritem>" << endl;
}

void XmlDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  m_t << "<xrefsect id=\"";
  m_t << x->file() << "_1" << x->anchor();
  m_t << "\">";
  m_t << "<xreftitle>";
  filter(x->title());
  m_t << "</xreftitle>";
  m_t << "<xrefdescription>";
}

void XmlDocVisitor::visitPost(DocXRefItem *)
{
  if (m_hide) return;
  m_t << "</xrefdescription>";
  m_t << "</xrefsect>";
}

void XmlDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(0,ref->file(),ref->anchor());
}

void XmlDocVisitor::visitPost(DocInternalRef *) 
{
  if (m_hide) return;
  endLink();
  m_t << " ";
}

void XmlDocVisitor::visitPre(DocCopy *c)
{
  if (m_hide) return;
  m_t << "<copydoc link=\"" << convertToXML(c->link()) << "\">";
}

void XmlDocVisitor::visitPost(DocCopy *)
{
  if (m_hide) return;
  m_t << "</copydoc>" << endl;
}

void XmlDocVisitor::visitPre(DocText *)
{
}

void XmlDocVisitor::visitPost(DocText *)
{
}

void XmlDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "<blockquote>";
}

void XmlDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "</blockquote>";
}

void XmlDocVisitor::visitPre(DocVhdlFlow *)
{
}

void XmlDocVisitor::visitPost(DocVhdlFlow *)
{
}

void XmlDocVisitor::filter(const char *str)
{ 
  m_t << convertToXML(str);
}

void XmlDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  //printf("XmlDocVisitor: file=%s anchor=%s\n",file.data(),anchor.data());
  m_t << "<ref refid=\"" << file;
  if (!anchor.isEmpty()) m_t << "_1" << anchor;
  m_t << "\" kindref=\"";
  if (!anchor.isEmpty()) m_t << "member"; else m_t << "compound";
  m_t << "\"";
  if (!ref.isEmpty()) m_t << " external=\"" << ref << "\"";
  m_t << ">";
}

void XmlDocVisitor::endLink()
{
  m_t << "</ref>";
}

void XmlDocVisitor::pushEnabled()
{
  m_enabled.push(new bool(m_hide));
}

void XmlDocVisitor::popEnabled()
{
  bool *v=m_enabled.pop();
  ASSERT(v!=0);
  m_hide = *v;
  delete v;
}

