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

#include <qdir.h>
#include "textdocvisitor.h"
#include "message.h"

//-------------------------------------------------------------------------

void TextDocVisitor::visit(DocSymbol *s)
{
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
    case DocSymbol::Pipe:    m_t << "|"; break;
    case DocSymbol::Copy:    m_t << "&copy;"; break;
    case DocSymbol::Tm:      m_t << "&tm;"; break;
    case DocSymbol::Reg:     m_t << "&reg;"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Lsquo:   m_t << "&lsquo;"; break;
    case DocSymbol::Rsquo:   m_t << "&rsquo;"; break;
    case DocSymbol::Ldquo:   m_t << "&ldquo;"; break;
    case DocSymbol::Rdquo:   m_t << "&rdquo;"; break;
    case DocSymbol::Ndash:   m_t << "&ndash;"; break;
    case DocSymbol::Mdash:   m_t << "&mdash;"; break;
    case DocSymbol::Uml:     m_t << "&" << s->letter() << "uml;"; break;
    case DocSymbol::Acute:   m_t << "&" << s->letter() << "acute;"; break;
    case DocSymbol::Grave:   m_t << "&" << s->letter() << "grave;"; break;
    case DocSymbol::Circ:    m_t << "&" << s->letter() << "circ;"; break;
    case DocSymbol::Slash:   m_t << "&" << s->letter() << "slash;"; break;
    case DocSymbol::Tilde:   m_t << "&" << s->letter() << "tilde;"; break;
    case DocSymbol::Szlig:   m_t << "&szlig;"; break;
    case DocSymbol::Cedil:   m_t << "&" << s->letter() << "cedil;"; break;
    case DocSymbol::Ring:    m_t << "&" << s->letter() << "ring;"; break;
    case DocSymbol::Nbsp:    m_t << "&nbsp;"; break;
    case DocSymbol::Aelig:   m_t << "&aelig;"; break;
    case DocSymbol::AElig:   m_t << "&AElig;"; break;
    case DocSymbol::GrkGamma:      m_t << "&Gamma;"; break;
    case DocSymbol::GrkDelta:      m_t << "&Delta;"; break;
    case DocSymbol::GrkTheta:      m_t << "&Theta;"; break;
    case DocSymbol::GrkLambda:     m_t << "&Lambda;"; break;
    case DocSymbol::GrkXi:         m_t << "&Xi;"; break;
    case DocSymbol::GrkPi:         m_t << "&Pi;"; break;
    case DocSymbol::GrkSigma:      m_t << "&Sigma;"; break;
    case DocSymbol::GrkUpsilon:    m_t << "&Upsilon;"; break;
    case DocSymbol::GrkPhi:        m_t << "&Phi;"; break;
    case DocSymbol::GrkPsi:        m_t << "&Psi;"; break;
    case DocSymbol::GrkOmega:      m_t << "&Omega;"; break;
    case DocSymbol::Grkalpha:      m_t << "&alpha;"; break;
    case DocSymbol::Grkbeta:       m_t << "&beta;"; break;
    case DocSymbol::Grkgamma:      m_t << "&gamma;"; break;
    case DocSymbol::Grkdelta:      m_t << "&delta;"; break;
    case DocSymbol::Grkepsilon:    m_t << "&epsilon;"; break;
    case DocSymbol::Grkzeta:       m_t << "&zeta;"; break;
    case DocSymbol::Grketa:        m_t << "&eta;"; break;
    case DocSymbol::Grktheta:      m_t << "&theta;"; break;
    case DocSymbol::Grkiota:       m_t << "&iota;"; break;
    case DocSymbol::Grkkappa:      m_t << "&kappa;"; break;
    case DocSymbol::Grklambda:     m_t << "&lambda;"; break;
    case DocSymbol::Grkmu:         m_t << "&mu;"; break;
    case DocSymbol::Grknu:         m_t << "&nu;"; break;
    case DocSymbol::Grkxi:         m_t << "&xi;"; break;
    case DocSymbol::Grkpi:         m_t << "&pi;"; break;
    case DocSymbol::Grkrho:        m_t << "&rho;"; break;
    case DocSymbol::Grksigma:      m_t << "&sigma;"; break;
    case DocSymbol::Grktau:        m_t << "&tau;"; break;
    case DocSymbol::Grkupsilon:    m_t << "&upsilon;"; break;
    case DocSymbol::Grkphi:        m_t << "&phi;"; break;
    case DocSymbol::Grkchi:        m_t << "&chi;"; break;
    case DocSymbol::Grkpsi:        m_t << "&psi;"; break;
    case DocSymbol::Grkomega:      m_t << "&omega;"; break;
    case DocSymbol::Grkvarsigma:   m_t << "&sigmaf;"; break;
    case DocSymbol::Section:       m_t << "&sect;"; break;
    case DocSymbol::Degree:        m_t << "&deg;"; break;
    case DocSymbol::Prime:         m_t << "&prime;"; break;
    case DocSymbol::DoublePrime:   m_t << "&Prime;"; break;
    case DocSymbol::Infinity:      m_t << "&infin;"; break;
    case DocSymbol::EmptySet:      m_t << "&empty;"; break;
    case DocSymbol::PlusMinus:     m_t << "&plusmn;"; break;
    case DocSymbol::Times:         m_t << "&times;"; break;
    case DocSymbol::Minus:         m_t << "&minus;"; break;
    case DocSymbol::CenterDot:     m_t << "&sdot;"; break;
    case DocSymbol::Partial:       m_t << "&part;"; break;
    case DocSymbol::Nabla:         m_t << "&nabla;"; break;
    case DocSymbol::SquareRoot:    m_t << "&radic;"; break;
    case DocSymbol::Perpendicular: m_t << "&perp;"; break;
    case DocSymbol::Sum:           m_t << "&sum;"; break;
    case DocSymbol::Integral:      m_t << "&int;"; break;
    case DocSymbol::Product:       m_t << "&prod;"; break;
    case DocSymbol::Similar:       m_t << "&sim;"; break;
    case DocSymbol::Approx:        m_t << "&asymp;"; break;
    case DocSymbol::NotEqual:      m_t << "&ne;"; break;
    case DocSymbol::Equivalent:    m_t << "&equiv;"; break;
    case DocSymbol::Proportional:  m_t << "&prop;"; break;
    case DocSymbol::LessEqual:     m_t << "&le;"; break;
    case DocSymbol::GreaterEqual:  m_t << "&ge;"; break;
    case DocSymbol::LeftArrow:     m_t << "&larr;"; break;
    case DocSymbol::RightArrow:    m_t << "&rarr;"; break;
    case DocSymbol::SetIn:         m_t << "&isin;"; break;
    case DocSymbol::SetNotIn:      m_t << "&notin;"; break;
    case DocSymbol::LeftCeil:      m_t << "&lceil;"; break;
    case DocSymbol::RightCeil:     m_t << "&rceil;"; break;
    case DocSymbol::LeftFloor:     m_t << "&lfloor;"; break;
    case DocSymbol::RightFloor:    m_t << "&rfloor;"; break;
    default:
                             err("unknown symbol found\n");
  }
}


void TextDocVisitor::filter(const char *str)
{ 
  if (str==0) return;
  //printf("TextDocVisitor::filter(%s)\n",str);
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '\n':  m_t << " ";      break;
      case '"':   m_t << "&quot;"; break;
      case '\'':  m_t << "&#39;";  break;
      case '<':   m_t << "&lt;";   break;
      case '>':   m_t << "&gt;";   break;
      case '&':   m_t << "&amp;";  break;
      default:    m_t << c;
    }
  }
}

