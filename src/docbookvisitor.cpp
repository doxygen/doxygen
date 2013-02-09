/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

DocbookDocVisitor::DocbookDocVisitor(FTextStream &t,CodeOutputInterface &ci)
  : DocVisitor(DocVisitor_Docbook), m_t(t), m_ci(ci), m_insidePre(FALSE), m_hide(FALSE)
{
}

//--------------------------------------
// visitor functions for leaf nodes
//--------------------------------------

void DocbookDocVisitor::visit(DocWord *w)
{
  if (m_hide) return;
  filter(w->word());
}

void DocbookDocVisitor::visit(DocLinkedWord *w)
{
  if (m_hide) return;
  startLink(w->file(),w->anchor());
  filter(w->word());
  endLink();
}

void DocbookDocVisitor::visit(DocWhiteSpace *w)
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

void DocbookDocVisitor::visit(DocSymbol *s)
{
  if (m_hide) return;
  switch(s->symbol())
  {
    case DocSymbol::BSlash:  m_t << "\\"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    m_t << "&lt;"; break;
    case DocSymbol::Greater: m_t << "&gt;"; break;
    case DocSymbol::Amp:     m_t << "&amp;"; break;
    case DocSymbol::Dollar:  m_t << "$"; break;
    case DocSymbol::Hash:    m_t << "#"; break;
    case DocSymbol::DoubleColon: m_t << "::"; break;
    case DocSymbol::Percent: m_t << "%"; break;
    case DocSymbol::Copy:    m_t << "&#x00A9;"; break;
    case DocSymbol::Tm:      m_t << "&#8482;"; break;
    case DocSymbol::Reg:     m_t << "&#174;"; break;
    case DocSymbol::Apos:    m_t << "&apos;"; break;
    case DocSymbol::Quot:    m_t << "&quot;"; break;
    case DocSymbol::Lsquo:   m_t << "&#8216;"; break;
    case DocSymbol::Rsquo:   m_t << "&#8217;"; break;
    case DocSymbol::Ldquo:   m_t << "&#8220;"; break;
    case DocSymbol::Rdquo:   m_t << "&#8221;"; break;
    case DocSymbol::Ndash:   m_t << "&#8211;"; break;
    case DocSymbol::Mdash:   m_t << "&#8212;"; break;
    case DocSymbol::Uml:     m_t << "&#168;"; break;
    case DocSymbol::Acute:   m_t << "&#180;"; break;
    case DocSymbol::Grave:   m_t << "&#192;"; break;
    case DocSymbol::Circ:    m_t << "&#710;"; break;
    case DocSymbol::Tilde:   m_t << "&#732;"; break;
    case DocSymbol::Szlig:   m_t << "&#223;"; break;
    case DocSymbol::Cedil:   m_t << "&#184;"; break;
    case DocSymbol::Ring:    m_t << "&#197;"; break;
    case DocSymbol::Slash:   m_t << "&#216;"; break;
    case DocSymbol::Nbsp:    m_t << "&#160;"; break;
    case DocSymbol::Aelig:   m_t << "&#230;"; break;
    case DocSymbol::AElig:   m_t << "&#198;"; break;
    case DocSymbol::GrkGamma:      m_t << "&#915;"; break;
    case DocSymbol::GrkDelta:      m_t << "&#916;"; break;
    case DocSymbol::GrkTheta:      m_t << "&#920;"; break;
    case DocSymbol::GrkLambda:     m_t << "&#923;"; break;
    case DocSymbol::GrkXi:         m_t << "&#926;"; break;
    case DocSymbol::GrkPi:         m_t << "&#928;"; break;
    case DocSymbol::GrkSigma:      m_t << "&#931;"; break;
    case DocSymbol::GrkUpsilon:    m_t << "&#933;"; break;
    case DocSymbol::GrkPhi:        m_t << "&#934;"; break;
    case DocSymbol::GrkPsi:        m_t << "&#936;"; break;
    case DocSymbol::GrkOmega:      m_t << "&#937;"; break;
    case DocSymbol::Grkalpha:      m_t << "&#945;"; break;
    case DocSymbol::Grkbeta:       m_t << "&#946;"; break;
    case DocSymbol::Grkgamma:      m_t << "&#947;"; break;
    case DocSymbol::Grkdelta:      m_t << "&#948;"; break;
    case DocSymbol::Grkepsilon:    m_t << "&#949;"; break;
    case DocSymbol::Grkzeta:       m_t << "&#950;"; break;
    case DocSymbol::Grketa:        m_t << "&#951;"; break;
    case DocSymbol::Grktheta:      m_t << "&#952;"; break;
    case DocSymbol::Grkiota:       m_t << "&#953;"; break;
    case DocSymbol::Grkkappa:      m_t << "&#954;"; break;
    case DocSymbol::Grklambda:     m_t << "&#955;"; break;
    case DocSymbol::Grkmu:         m_t << "&#956;"; break;
    case DocSymbol::Grknu:         m_t << "&#957;"; break;
    case DocSymbol::Grkxi:         m_t << "&#958;"; break;
    case DocSymbol::Grkpi:         m_t << "&#960;"; break;
    case DocSymbol::Grkrho:        m_t << "&#961;"; break;
    case DocSymbol::Grksigma:      m_t << "&#963;"; break;
    case DocSymbol::Grktau:        m_t << "&#964;"; break;
    case DocSymbol::Grkupsilon:    m_t << "&#965;"; break;
    case DocSymbol::Grkphi:        m_t << "&#966;"; break;
    case DocSymbol::Grkchi:        m_t << "&#967;"; break;
    case DocSymbol::Grkpsi:        m_t << "&#968;"; break;
    case DocSymbol::Grkomega:      m_t << "&#969;"; break;
    case DocSymbol::Grkvarsigma:   m_t << "&#962;"; break;
    case DocSymbol::Section:       m_t << "<simplesect/>"; break;
    case DocSymbol::Degree:        m_t << "&#176;"; break;
    case DocSymbol::Prime:         m_t << "&#8242;"; break;
    case DocSymbol::DoublePrime:   m_t << "&#8243;"; break;
    case DocSymbol::Infinity:      m_t << "&#8734;"; break;
    case DocSymbol::EmptySet:      m_t << "&#8709;"; break;
    case DocSymbol::PlusMinus:     m_t << "&#177;"; break;
    case DocSymbol::Times:         m_t << "&#215;"; break;
    case DocSymbol::Minus:         m_t << "&#8722;"; break;
    case DocSymbol::CenterDot:     m_t << "&#8901;"; break;
    case DocSymbol::Partial:       m_t << "&#8706;"; break;
    case DocSymbol::Nabla:         m_t << "&#8711;"; break;
    case DocSymbol::SquareRoot:    m_t << "&#8730;"; break;
    case DocSymbol::Perpendicular: m_t << "&#8869;"; break;
    case DocSymbol::Sum:           m_t << "&#8721;"; break;
    case DocSymbol::Integral:      m_t << "&#8747;"; break;
    case DocSymbol::Product:       m_t << "&#8719;"; break;
    case DocSymbol::Similar:       m_t << "&#8764;"; break;
    case DocSymbol::Approx:        m_t << "&#8776;"; break;
    case DocSymbol::NotEqual:      m_t << "&#8800;"; break;
    case DocSymbol::Equivalent:    m_t << "&#8801;"; break;
    case DocSymbol::Proportional:  m_t << "&#8733;"; break;
    case DocSymbol::LessEqual:     m_t << "&#8804;"; break;
    case DocSymbol::GreaterEqual:  m_t << "&#8805;"; break;
    case DocSymbol::LeftArrow:     m_t << "&#8592;"; break;
    case DocSymbol::RightArrow:    m_t << "&#8594;"; break;
    case DocSymbol::SetIn:         m_t << "&#8712;"; break;
    case DocSymbol::SetNotIn:      m_t << "&#8713;"; break;
    case DocSymbol::LeftCeil:      m_t << "&#8968;"; break;
    case DocSymbol::RightCeil:     m_t << "&#8969;"; break;
    case DocSymbol::LeftFloor:     m_t << "&#8970;"; break;
    case DocSymbol::RightFloor:    m_t << "&#8971;"; break;
    default:
                                   err("error: unknown symbol found\n");
  }
}

void DocbookDocVisitor::visit(DocURL *u)
{
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
  if (m_hide) return;
  m_t << endl << "<literallayout>\n</literallayout>" << endl;
}

void DocbookDocVisitor::visit(DocHorRuler *)
{
  if (m_hide) return;
  m_t << "<informaltable frame='bottom'><tgroup cols='1'><colspec align='center'/><tbody><row><entry align='center'>\n";
  m_t << "</entry></row></tbody></tgroup></informaltable>\n";
}

void DocbookDocVisitor::visit(DocStyleChange *s)
{
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
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void DocbookDocVisitor::visit(DocVerbatim *s)
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
      m_t << "<programlisting><literallayout>";
      filter(s->text());
      m_t << "</literallayout></programlisting>";
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
      break;
      m_t << s->text();
      break;
    case DocVerbatim::Dot:
      {
        static int dotindex = 1;
        QCString baseName(4096);
        QCString stext = s->text();
        m_t << "<para>" << endl;
        baseName.sprintf("%s%d",
            (Config_getString("DOCBOOK_OUTPUT")+"/inline_dotgraph_").data(),
            dotindex++
            );
        QFile file(baseName+".dot");
        if (!file.open(IO_WriteOnly))
        {
          err("Could not open file %s.msc for writing\n",baseName.data());
        }
        file.writeBlock( stext, stext.length() );
        file.close();
        m_t << "    <figure>" << endl;
        m_t << "        <title></title>" << endl;
        m_t << "        <mediaobject>" << endl;
        m_t << "            <imageobject>" << endl;
        writeDotFile(baseName);
        m_t << "            </imageobject>" << endl;
        m_t << "       </mediaobject>" << endl;
        m_t << "    </figure>" << endl;
        m_t << "</para>" << endl;
      }
      break;
    case DocVerbatim::Msc:
      static int mscindex = 1;
      QCString baseName(4096);
      QCString stext = s->text();
      m_t << "<para>" << endl;
      baseName.sprintf("%s%d",
          (Config_getString("DOCBOOK_OUTPUT")+"/inline_mscgraph_").data(),
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
      m_t << "    <figure>" << endl;
      m_t << "        <title></title>" << endl;
      m_t << "        <mediaobject>" << endl;
      m_t << "            <imageobject>" << endl;
      writeMscFile(baseName);
      m_t << "            </imageobject>" << endl;
      m_t << "       </mediaobject>" << endl;
      m_t << "    </figure>" << endl;
      m_t << "</para>" << endl;
      break;
  }
}

void DocbookDocVisitor::visit(DocAnchor *anc)
{
  if (m_hide) return;
  m_t << "<anchor id=\"" << anc->file() << "_1" << anc->anchor() << "\"/>";
}

void DocbookDocVisitor::visit(DocInclude *inc)
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

void DocbookDocVisitor::visit(DocIncOperator *op)
{
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

void DocbookDocVisitor::visit(DocFormula *f)
{
  if (m_hide) return;
  m_t << "<equation><title>" << f->name() << "</title>";
  filter(f->text());
  m_t << "</equation>";
}

void DocbookDocVisitor::visit(DocIndexEntry *ie)
{
  if (m_hide) return;
  m_t << "<indexentry><primaryie>" << endl;
  filter(ie->entry());
  m_t << "</primaryie><secondaryie></secondaryie></indexentry>" << endl;
}

void DocbookDocVisitor::visit(DocSimpleSectSep *)
{
  m_t << "<simplesect/>";
}

void DocbookDocVisitor::visit(DocCite *cite)
{
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
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocAutoListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>";
}

void DocbookDocVisitor::visitPre(DocPara *)
{
  if (m_hide) return;
  m_t << endl;
  m_t << "<para>";
}

void DocbookDocVisitor::visitPost(DocPara *)
{
  if (m_hide) return;
  m_t << "</para>";
  m_t << endl;
}

void DocbookDocVisitor::visitPre(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">New parser:</font></h4>\n";
}

void DocbookDocVisitor::visitPost(DocRoot *)
{
  //m_t << "<hr><h4><font color=\"red\">Old parser:</font></h4>\n";
}

void DocbookDocVisitor::visitPre(DocSimpleSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocSimpleSect::See:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trSeeAlso() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trSeeAlso()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Return:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trReturns()<< ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trReturns()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Author:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, TRUE) << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trAuthor(TRUE, TRUE)) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Authors:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trAuthor(TRUE, FALSE) << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trAuthor(TRUE, FALSE)) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Version:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trVersion() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trVersion()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Since:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trSince() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trSince()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Date:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trDate() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trDate()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Note:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trNote() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trNote()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Warning:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trWarning() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trWarning()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Pre:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trPrecondition() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trPrecondition()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Post:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trPostcondition() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trPostcondition()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Copyright:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trCopyright() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trCopyright()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Invar:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trInvariant() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trInvariant()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Remark:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trRemarks() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trRemarks()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::Attention:
      if (m_insidePre) 
      {
        m_t << "<formalpara><title>" << theTranslator->trAttention() << ": </title>" << endl;
      } 
      else 
      {
        m_t << "<formalpara><title>" << convertToXML(theTranslator->trAttention()) << ": </title>" << endl;
      }
      break;
    case DocSimpleSect::User:
      m_t << "<formalpara><title></title>" << endl;
      break;
    case DocSimpleSect::Rcs:
      m_t << "<formalpara><title></title>" << endl;
      break;
    case DocSimpleSect::Unknown: m_t << "<formalpara><title></title>" << endl; break;
  }
}

void DocbookDocVisitor::visitPost(DocSimpleSect *)
{
  if (m_hide) return;
  m_t << "</formalpara>" << endl;
}

void DocbookDocVisitor::visitPre(DocTitle *)
{
  if (m_hide) return;
  m_t << "<title>";
}

void DocbookDocVisitor::visitPost(DocTitle *)
{
  if (m_hide) return;
  m_t << "</title>";
}

void DocbookDocVisitor::visitPre(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "<itemizedlist>\n";
}

void DocbookDocVisitor::visitPost(DocSimpleList *)
{
  if (m_hide) return;
  m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::visitPre(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocSimpleListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::visitPre(DocSection *s)
{
  if (m_hide) return;
  m_t << "<section xml:id=\"" << s->file();
  if (!s->anchor().isEmpty()) m_t << "_1" << s->anchor();
  m_t << "\">" << endl;
  m_t << "<title>";
  filter(s->title());
  m_t << "</title>" << endl;
}

void DocbookDocVisitor::visitPost(DocSection *)
{
  m_t << "</section>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered)
    m_t << "<orderedlist>\n";
  else
    m_t << "<itemizedlist>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlList *s)
{
  if (m_hide) return;
  if (s->type()==DocHtmlList::Ordered)
    m_t << "</orderedlist>\n";
  else
    m_t << "</itemizedlist>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "<listitem>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlListItem *)
{
  if (m_hide) return;
  m_t << "</listitem>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "<variablelist>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlDescList *)
{
  if (m_hide) return;
  m_t << "</variablelist>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "<varlistentry><term>";
}

void DocbookDocVisitor::visitPost(DocHtmlDescTitle *)
{
  if (m_hide) return;
  m_t << "</term>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlDescData *)
{
  if (m_hide) return;
  m_t << "<listitem>";
}

void DocbookDocVisitor::visitPost(DocHtmlDescData *)
{
  if (m_hide) return;
  m_t << "</listitem></varlistentry>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlTable *t)
{
  if (m_hide) return;
  m_t << "<table frame=\"all\">" << endl;
  m_t << "    <title></title>" << endl;
  m_t << "    <tgroup cols=\"" << t->numColumns() << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
  m_t << "    <tbody>" << endl;
}

void DocbookDocVisitor::visitPost(DocHtmlTable *)
{
  if (m_hide) return;
  m_t << "    </tbody>" << endl;
  m_t << "    </tgroup>" << endl;
  m_t << "</table>" << endl;
}

void DocbookDocVisitor::visitPre(DocHtmlRow *)
{
  if (m_hide) return;
  m_t << "<row>\n";
}

void DocbookDocVisitor::visitPost(DocHtmlRow *)
{
  if (m_hide) return;
  m_t << "</row>\n";
}

void DocbookDocVisitor::visitPre(DocHtmlCell *)
{
  if (m_hide) return;
  m_t << "<entry>";
}

void DocbookDocVisitor::visitPost(DocHtmlCell *)
{
  if (m_hide) return;
  m_t << "</entry>";
}

void DocbookDocVisitor::visitPre(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "<caption>";
}

void DocbookDocVisitor::visitPost(DocHtmlCaption *)
{
  if (m_hide) return;
  m_t << "</caption>\n";
}

void DocbookDocVisitor::visitPre(DocInternal *)
{
  if (m_hide) return;
  // TODO: to be implemented
}

void DocbookDocVisitor::visitPost(DocInternal *)
{
  if (m_hide) return;
  // TODO: to be implemented
}

void DocbookDocVisitor::visitPre(DocHRef *href)
{
  if (m_hide) return;
  m_t << "<link xlink:href=\"" << href->url() << "\">";
}

void DocbookDocVisitor::visitPost(DocHRef *)
{
  if (m_hide) return;
  m_t << "</link>";
}

void DocbookDocVisitor::visitPre(DocHtmlHeader *)
{
  if (m_hide) return;
  m_t << "<formalpara><title>";
}

void DocbookDocVisitor::visitPost(DocHtmlHeader *)
{
  if (m_hide) return;
  m_t << "</title></formalpara>\n";
}

void DocbookDocVisitor::visitPre(DocImage *img)
{
  if (img->type()==DocImage::Latex)
  {
    if (m_hide) return;
    m_t << endl;
    m_t << "    <figure>" << endl;
    m_t << "        <title>";
  } 
  else 
  {
    pushEnabled();
    m_hide=TRUE;
  }
}

void DocbookDocVisitor::visitPost(DocImage *img)
{
  if (img->type()==DocImage::Latex)
  {
    if (m_hide) return;
    QCString typevar;
    m_t << "</title>" << endl;
    m_t << "    <mediaobject>" << endl;
    m_t << "        <imageobject>" << endl;
    QCString baseName=img->name();
    int i;
    if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
    {
      baseName=baseName.right(baseName.length()-i-1);
    }
    m_t << "            <imagedata";
    if (!img->width().isEmpty())
    {
      m_t << " width=\"";
      filter(img->width());
      m_t << "\"";
    }
    else if (!img->height().isEmpty())
    {
      m_t << " depth=\"";
      filter(img->height());
      m_t << "\"";
    }
    m_t << " align=\"center\" fileref=\"" << baseName << "\">";
    m_t << "</imagedata>" << endl;
    m_t << "        </imageobject>" << endl;
    m_t << "    </mediaobject>" << endl;
    m_t << "    </figure>" << endl;
    // copy the image to the output dir
    QCString m_file;
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::imageNameDict, baseName, ambig);
    if (fd) 
    {
      m_file=fd->absFilePath();
    }
    QFile inImage(m_file);
    QFile outImage(Config_getString("DOCBOOK_OUTPUT")+"/"+baseName.data());
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
  if (m_hide) return;
  startDotFile(df->file(),df->width(),df->height(),df->hasCaption());
}

void DocbookDocVisitor::visitPost(DocDotFile *df)
{
  if (m_hide) return;
  endDotFile(df->hasCaption());
}

void DocbookDocVisitor::visitPre(DocMscFile *df)
{
  if (m_hide) return;
  startMscFile(df->file(),df->width(),df->height(),df->hasCaption());
}

void DocbookDocVisitor::visitPost(DocMscFile *df)
{
  if (m_hide) return;
  endMscFile(df->hasCaption());
}
void DocbookDocVisitor::visitPre(DocLink *lnk)
{
  if (m_hide) return;
  startLink(lnk->file(),lnk->anchor());
}

void DocbookDocVisitor::visitPost(DocLink *)
{
  if (m_hide) return;
  endLink();
}

void DocbookDocVisitor::visitPre(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) startLink(ref->file(),ref->anchor());
  if (!ref->hasLinkText()) filter(ref->targetTitle());
}

void DocbookDocVisitor::visitPost(DocRef *ref)
{
  if (m_hide) return;
  if (!ref->file().isEmpty()) endLink();
}

void DocbookDocVisitor::visitPre(DocSecRefItem *ref)
{
  if (m_hide) return;
  m_t << "<tocitem id=\"" << ref->file() << "_1" << ref->anchor() << "\">";
}

void DocbookDocVisitor::visitPost(DocSecRefItem *)
{
  if (m_hide) return;
  m_t << "</tocitem>" << endl;
}

void DocbookDocVisitor::visitPre(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "<toclist>" << endl;
}

void DocbookDocVisitor::visitPost(DocSecRefList *)
{
  if (m_hide) return;
  m_t << "</toclist>" << endl;
}

void DocbookDocVisitor::visitPre(DocParamSect *s)
{
  if (m_hide) return;
  switch(s->type())
  {
    case DocParamSect::Param:
      {
        m_t <<  endl;
        m_t << "                <formalpara>" << endl;
        m_t << "                    <title/>" << endl;
        m_t << "                    <table frame=\"all\">" << endl;
        m_t << "                        <title>Parameters</title>" << endl;
        m_t << "                        <tgroup cols=\"2\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
        m_t << "                        <tbody>" << endl;
        break;
      }
    case DocParamSect::RetVal:
      {
        m_t <<  endl;
        m_t << "                <formalpara>" << endl;
        m_t << "                    <title/>" << endl;
        m_t << "                    <table frame=\"all\">" << endl;
        m_t << "                        <title>Parameters</title>" << endl;
        m_t << "                        <tgroup cols=\"2\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
        m_t << "                        <tbody>" << endl;
        break;
      }
    case DocParamSect::Exception:
      m_t << "exception"; break;
    case DocParamSect::TemplateParam:
      m_t << "templateparam"; break;
    default:
      ASSERT(0);
  }
}

void DocbookDocVisitor::visitPost(DocParamSect *)
{
  if (m_hide) return;
  m_t << "                        </tbody>" << endl;
  m_t << "                        </tgroup>" << endl;
  m_t << "                    </table>" << endl;
  m_t << "                </formalpara>" << endl;
  m_t << "                ";
}

void DocbookDocVisitor::visitPre(DocParamList *pl)
{
  if (m_hide) return;
  QListIterator<DocNode> li(pl->parameters());
  DocNode *param;
  m_t << "                            <row>" << endl;
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
  if (m_hide) return;
  m_t << "</entry>" << endl;
  m_t << "                            </row>" << endl;
}

void DocbookDocVisitor::visitPre(DocXRefItem *x)
{
  if (m_hide) return;
  m_t << "<para><link linkend=\"";
  m_t << x->file() << "_1" << x->anchor();
  m_t << "\">";
  filter(x->title());
  m_t << "</link>";
  m_t << " ";
}

void DocbookDocVisitor::visitPost(DocXRefItem *)
{
  if (m_hide) return;
  m_t << "</para>";
}

void DocbookDocVisitor::visitPre(DocInternalRef *ref)
{
  if (m_hide) return;
  startLink(ref->file(),ref->anchor());
}

void DocbookDocVisitor::visitPost(DocInternalRef *)
{
  if (m_hide) return;
  endLink();
  m_t << " ";
}

void DocbookDocVisitor::visitPre(DocCopy *)
{
  if (m_hide) return;
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPost(DocCopy *)
{
  if (m_hide) return;
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPre(DocText *)
{
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPost(DocText *)
{
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPre(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "<blockquote>";
}

void DocbookDocVisitor::visitPost(DocHtmlBlockQuote *)
{
  if (m_hide) return;
  m_t << "</blockquote>";
}

void DocbookDocVisitor::visitPre(DocVhdlFlow *)
{
  // TODO: to be implemented
}


void DocbookDocVisitor::visitPost(DocVhdlFlow *)
{
  // TODO: to be implemented
}

void DocbookDocVisitor::filter(const char *str)
{
  m_t << convertToXML(str);
}

void DocbookDocVisitor::startLink(const QCString &file,const QCString &anchor)
{
  m_t << "<link linkend=\"" << file;
  if (!anchor.isEmpty()) m_t << "_1" << anchor;
  m_t << "\">";
}

void DocbookDocVisitor::endLink()
{
  m_t << "</link>";
}

void DocbookDocVisitor::pushEnabled()
{
  m_enabled.push(new bool(m_hide));
}

void DocbookDocVisitor::popEnabled()
{
  bool *v=m_enabled.pop();
  ASSERT(v!=0);
  m_hide = *v;
  delete v;
}

void DocbookDocVisitor::writeMscFile(const QCString &baseName)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString("DOCBOOK_OUTPUT");
  writeMscGraphFromFile(baseName+".msc",outDir,shortName,MSC_BITMAP);
  m_t << "                <imagedata";
  m_t << " align=\"center\" fileref=\"" << shortName << ".png" << "\">";
  m_t << "</imagedata>" << endl;
}

void DocbookDocVisitor::startMscFile(const QCString &fileName,
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
  QCString outDir = Config_getString("DOCBOOK_OUTPUT");
  writeMscGraphFromFile(fileName,outDir,baseName,MSC_BITMAP);
  m_t << "<para>" << endl;
  m_t << "    <figure>" << endl;
  m_t << "        <title></title>" << endl;
  m_t << "        <mediaobject>" << endl;
  m_t << "            <imageobject>" << endl;
  m_t << "                <imagedata";
  if (!width.isEmpty())
  {
    m_t << " width=\"";
    m_t << width;
    m_t << "\"";
  }
  else if (!height.isEmpty())
  {
    m_t << " depth=\"";
    m_t << height;
    m_t << "\"";
  }
  m_t << " align=\"center\" fileref=\"" << baseName << ".png" << "\">";
  m_t << "</imagedata>" << endl;
  m_t << "            </imageobject>" << endl;
  if (hasCaption)
  {
    m_t << "        <caption>" << endl;
  }
}

void DocbookDocVisitor::endMscFile(bool hasCaption)
{
  if (m_hide) return;
  m_t << "endl";
  if (hasCaption)
  {
    m_t << "        </caption>" << endl;
  }
  m_t << "        </mediaobject>" << endl;
  m_t << "    </figure>" << endl;
  m_t << "</para>" << endl;
}

void DocbookDocVisitor::writeDotFile(const QCString &baseName)
{
  QCString shortName = baseName;
  int i;
  if ((i=shortName.findRev('/'))!=-1)
  {
    shortName=shortName.right(shortName.length()-i-1);
  }
  QCString outDir = Config_getString("DOCBOOK_OUTPUT");
  writeDotGraphFromFile(baseName+".dot",outDir,shortName,BITMAP);
  m_t << "                <imagedata";
  m_t << " align=\"center\" fileref=\"" << shortName << ".png" << "\">";
  m_t << "</imagedata>" << endl;
}

void DocbookDocVisitor::startDotFile(const QCString &fileName,
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
  QCString outDir = Config_getString("DOCBOOK_OUTPUT");
  writeDotGraphFromFile(fileName,outDir,baseName,BITMAP);
  m_t << "<para>" << endl;
  m_t << "    <figure>" << endl;
  m_t << "        <title></title>" << endl;
  m_t << "        <mediaobject>" << endl;
  m_t << "            <imageobject>" << endl;
  m_t << "                <imagedata";
  if (!width.isEmpty())
  {
    m_t << " width=\"";
    m_t << width;
    m_t << "\"";
  }
  else if (!height.isEmpty())
  {
    m_t << " depth=\"";
    m_t << height;
    m_t << "\"";
  }
  m_t << " align=\"center\" fileref=\"" << baseName << ".png" << "\">";
  m_t << "</imagedata>" << endl;
  m_t << "            </imageobject>" << endl;
  if (hasCaption)
  {
    m_t << "        <caption>" << endl;
  }
}

void DocbookDocVisitor::endDotFile(bool hasCaption)
{
  if (m_hide) return;
  m_t << "endl";
  if (hasCaption)
  {
    m_t << "        </caption>" << endl;
  }
  m_t << "        </mediaobject>" << endl;
  m_t << "    </figure>" << endl;
  m_t << "</para>" << endl;
}

