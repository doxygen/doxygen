/******************************************************************************
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
#ifndef HTMLENTITY_H
#define HTMLENTITY_H

#include <unordered_map>
#include <string>

#include "qcstring.h"
#include "construct.h"

class TextStream;

/** @brief Singleton helper class to map html entities to other formats */
class HtmlEntityMapper
{
  public:
    enum SymType { Sym_Unknown = -1,
                   Sym_nbsp, Sym_iexcl, Sym_cent, Sym_pound, Sym_curren,
                   Sym_yen, Sym_brvbar, Sym_sect, Sym_uml, Sym_copy,
                   Sym_ordf, Sym_laquo, Sym_not, Sym_shy, Sym_reg,
                   Sym_macr, Sym_deg, Sym_plusmn, Sym_sup2, Sym_sup3,
                   Sym_acute, Sym_micro, Sym_para, Sym_middot, Sym_cedil,
                   Sym_sup1, Sym_ordm, Sym_raquo, Sym_frac14, Sym_frac12,
                   Sym_frac34, Sym_iquest, Sym_Agrave, Sym_Aacute, Sym_Acirc,
                   Sym_Atilde, Sym_Auml, Sym_Aring, Sym_AElig, Sym_Ccedil,
                   Sym_Egrave, Sym_Eacute, Sym_Ecirc, Sym_Euml, Sym_Igrave,
                   Sym_Iacute, Sym_Icirc, Sym_Iuml, Sym_ETH, Sym_Ntilde,
                   Sym_Ograve, Sym_Oacute, Sym_Ocirc, Sym_Otilde, Sym_Ouml,
                   Sym_times, Sym_Oslash, Sym_Ugrave, Sym_Uacute, Sym_Ucirc,
                   Sym_Uuml, Sym_Yacute, Sym_THORN, Sym_szlig, Sym_agrave,
                   Sym_aacute, Sym_acirc, Sym_atilde, Sym_auml, Sym_aring,
                   Sym_aelig, Sym_ccedil, Sym_egrave, Sym_eacute, Sym_ecirc,
                   Sym_euml, Sym_igrave, Sym_iacute, Sym_icirc, Sym_iuml,
                   Sym_eth, Sym_ntilde, Sym_ograve, Sym_oacute, Sym_ocirc,
                   Sym_otilde, Sym_ouml, Sym_divide, Sym_oslash, Sym_ugrave,
                   Sym_uacute, Sym_ucirc, Sym_uuml, Sym_yacute, Sym_thorn,
                   Sym_yuml, Sym_fnof, Sym_Alpha, Sym_Beta, Sym_Gamma,
                   Sym_Delta, Sym_Epsilon, Sym_Zeta, Sym_Eta, Sym_Theta,
                   Sym_Iota, Sym_Kappa, Sym_Lambda, Sym_Mu, Sym_Nu,
                   Sym_Xi, Sym_Omicron, Sym_Pi, Sym_Rho, Sym_Sigma,
                   Sym_Tau, Sym_Upsilon, Sym_Phi, Sym_Chi, Sym_Psi,
                   Sym_Omega, Sym_alpha, Sym_beta, Sym_gamma, Sym_delta,
                   Sym_epsilon, Sym_zeta, Sym_eta, Sym_theta, Sym_iota,
                   Sym_kappa, Sym_lambda, Sym_mu, Sym_nu, Sym_xi,
                   Sym_omicron, Sym_pi, Sym_rho, Sym_sigmaf, Sym_sigma,
                   Sym_tau, Sym_upsilon, Sym_phi, Sym_chi, Sym_psi,
                   Sym_omega, Sym_thetasym, Sym_upsih, Sym_piv, Sym_bull,
                   Sym_hellip, Sym_prime, Sym_Prime, Sym_oline, Sym_frasl,
                   Sym_weierp, Sym_image, Sym_real, Sym_trade, Sym_alefsym,
                   Sym_larr, Sym_uarr, Sym_rarr, Sym_darr, Sym_harr,
                   Sym_crarr, Sym_lArr, Sym_uArr, Sym_rArr, Sym_dArr,
                   Sym_hArr, Sym_forall, Sym_part, Sym_exist, Sym_empty,
                   Sym_nabla, Sym_isin, Sym_notin, Sym_ni, Sym_prod,
                   Sym_sum, Sym_minus, Sym_lowast, Sym_radic, Sym_prop,
                   Sym_infin, Sym_ang, Sym_and, Sym_or, Sym_cap,
                   Sym_cup, Sym_int, Sym_there4, Sym_sim, Sym_cong,
                   Sym_asymp, Sym_ne, Sym_equiv, Sym_le, Sym_ge,
                   Sym_sub, Sym_sup, Sym_nsub, Sym_sube, Sym_supe,
                   Sym_oplus, Sym_otimes, Sym_perp, Sym_sdot, Sym_lceil,
                   Sym_rceil, Sym_lfloor, Sym_rfloor, Sym_lang, Sym_rang,
                   Sym_loz, Sym_spades, Sym_clubs, Sym_hearts, Sym_diams,
                   Sym_quot, Sym_amp, Sym_lt, Sym_gt, Sym_OElig,
                   Sym_oelig, Sym_Scaron, Sym_scaron, Sym_Yuml, Sym_circ,
                   Sym_tilde, Sym_ensp, Sym_emsp, Sym_thinsp, Sym_zwnj,
                   Sym_zwj, Sym_lrm, Sym_rlm, Sym_ndash, Sym_mdash,
                   Sym_lsquo, Sym_rsquo, Sym_sbquo, Sym_ldquo, Sym_rdquo,
                   Sym_bdquo, Sym_dagger, Sym_Dagger, Sym_permil, Sym_lsaquo,
                   Sym_rsaquo, Sym_euro,

                   /* doxygen extensions */
                   Sym_tm, Sym_apos,

                   /* doxygen commands mapped */
                   Sym_BSlash, Sym_At, Sym_Less, Sym_Greater, Sym_Amp,
                   Sym_Dollar, Sym_Hash, Sym_DoubleColon, Sym_Percent, Sym_Pipe,
                   Sym_Quot, Sym_Minus, Sym_Plus, Sym_Dot, Sym_Colon, Sym_Equal,
                   Sym_Exclam, Sym_Quest
                 };
    enum PerlType { Perl_unknown = 0, Perl_string, Perl_char, Perl_symbol, Perl_umlaut,
                    Perl_acute, Perl_grave, Perl_circ, Perl_slash, Perl_tilde,
                    Perl_cedilla, Perl_ring
                  };
    static HtmlEntityMapper &instance();
    SymType name2sym(const QCString &symName) const;
    const char *utf8(SymType symb,bool useInPrintf=FALSE) const;
    const char *html(SymType symb,bool useInPrintf=FALSE) const;
    const char *xml(SymType symb) const;
    const char *docbook(SymType symb) const;
    const char *latex(SymType symb) const;
    const char *man(SymType symb) const;
    const char *rtf(SymType symb) const;
    struct PerlSymb
    {
      const char     *symb;
      const PerlType  type;
    };
    const PerlSymb *perl(SymType symb) const;
    void  writeXMLSchema(TextStream &t);
  private:
    void  validate();
    HtmlEntityMapper();
   ~HtmlEntityMapper();
    NON_COPYABLE(HtmlEntityMapper)
    static HtmlEntityMapper *s_instance;
    std::unordered_map<std::string,SymType> m_name2sym;
};

#endif
