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

#include <vector>

#include "htmlentity.h"
#include "message.h"
#include "textstream.h"

//! @brief Structure defining all HTML4 entities, doxygen extensions and doxygen commands representing special symbols.
//! @details In case an entity does not exist a nullptr is given for the entity. The first column contains the symbolic code
//!          for the entity, see also doxparser.h The second column contains the name of the entity (without the starting \& and
//!          ending ;)
struct HtmlEntityInfo
{
  HtmlEntityMapper::SymType symb;
  const char *item;
  const char *UTF8;
  const char *html;
  const char *xml;
  const char *docbook;
  const char *latex;
  const char *man;
  const char *rtf;
  HtmlEntityMapper::PerlSymb perl;
};

static const std::vector<HtmlEntityInfo> g_htmlEntities
{
#undef SYM
// helper macro to force consistent entries for the symbol and item columns
#define SYM(s) HtmlEntityMapper::Sym_##s,"&"#s";"
  // HTML4 entities
  // symb+item     UTF-8           html          xml                     docbook          latex                     man       rtf            perl
  { SYM(nbsp),     "\xc2\xa0",     "&#160;",     "<nonbreakablespace/>", "&#160;",        "~",                      " ",      "\\~",         { " ",          HtmlEntityMapper::Perl_char    }},
  { SYM(iexcl),    "\xc2\xa1",     "&iexcl;",    "<iexcl/>",             "&#161;",        "!`",                     nullptr,  "\\'A1",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(cent),     "\xc2\xa2",     "&cent;",     "<cent/>",              "&#162;",        "\\textcent{}",           nullptr,  "\\'A2",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(pound),    "\xc2\xa3",     "&pound;",    "<pound/>",             "&#163;",        "{$\\pounds$}",           nullptr,  "\\'A3",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(curren),   "\xc2\xa4",     "&curren;",   "<curren/>",            "&#164;",        "\\textcurrency{}",       nullptr,  "\\'A4",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(yen),      "\xc2\xa5",     "&yen;",      "<yen/>",               "&#165;",        "{$\\yen$}",              nullptr,  "\\'A5",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(brvbar),   "\xc2\xa6",     "&brvbar;",   "<brvbar/>",            "&#166;",        "\\textbrokenbar{}",      nullptr,  "\\'A6",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(sect),     "\xc2\xa7",     "&sect;",     "<sect/>",              "<simplesect/>", "{$\\S$}",                nullptr,  "\\'A7",       { "sect",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(uml),      "\xc2\xa8",     "&uml;",      "<umlaut/>",            "&#168;",        "\\\"{ }",                " \\*(4", "\\'A8",       { " ",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(copy),     "\xc2\xa9",     "&copy;",     "<copy/>",              "&#169;",        "\\copyright{}",          "(C)",    "\\'A9",       { "copyright",  HtmlEntityMapper::Perl_symbol  }},
  { SYM(ordf),     "\xc2\xaa",     "&ordf;",     "<ordf/>",              "&#170;",        "\\textordfeminine{}",    nullptr,  "\\'AA",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(laquo),    "\xc2\xab",     "&laquo;",    "<laquo/>",             "&#171;",        "\\guillemotleft{}",      nullptr,  "\\'AB",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(not),      "\xc2\xac",     "&not;",      "<not/>",               "&#172;",        "\\textlnot",             nullptr,  "\\'AC",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(shy),      "\xc2\xad",     "&shy;",      "<shy/>",               "&#173;",        "{$\\-$}",                nullptr,  "\\-",         { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(reg),      "\xc2\xae",     "&reg;",      "<registered/>",        "&#174;",        "\\textregistered{}",     "(R)",    "\\'AE",       { "registered", HtmlEntityMapper::Perl_symbol  }},
  { SYM(macr),     "\xc2\xaf",     "&macr;",     "<macr/>",              "&#175;",        "\\={}",                  nullptr,  "\\'AF",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(deg),      "\xc2\xb0",     "&deg;",      "<deg/>",               "&#176;",        "\\textdegree{}",         nullptr,  "\\'B0",       { "deg",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(plusmn),   "\xc2\xb1",     "&plusmn;",   "<plusmn/>",            "&#177;",        "{$\\pm$}",               nullptr,  "\\'B1",       { "+/-",        HtmlEntityMapper::Perl_string  }},
  { SYM(sup2),     "\xc2\xb2",     "&sup2;",     "<sup2/>",              "&#178;",        "\\texttwosuperior{}",    nullptr,  "\\'B2",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(sup3),     "\xc2\xb3",     "&sup3;",     "<sup3/>",              "&#179;",        "\\textthreesuperior{}",  nullptr,  "\\'B3",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(acute),    "\xc2\xb4",     "&acute;",    "<acute/>",             "&#180;",        "\\'{}",                  nullptr,  "\\'B4",       { " ",          HtmlEntityMapper::Perl_acute   }},
  { SYM(micro),    "\xc2\xb5",     "&micro;",    "<micro/>",             "&#181;",        "{$\\mu$}",               nullptr,  "\\'B5",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(para),     "\xc2\xb6",     "&para;",     "<para/>",              "&#182;",        "{$\\P$}",                nullptr,  "\\'B6",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(middot),   "\xc2\xb7",     "&middot;",   "<middot/>",            "&#183;",        "\\textperiodcentered{}", nullptr,  "\\'B7",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(cedil),    "\xc2\xb8",     "&cedil;",    "<cedil/>",             "&#184;",        "\\c{}",                  " \\*,",  "\\'B8",       { " ",          HtmlEntityMapper::Perl_cedilla }},
  { SYM(sup1),     "\xc2\xb9",     "&sup1;",     "<sup1/>",              "&#185;",        "\\textonesuperior{}",    nullptr,  "\\'B9",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(ordm),     "\xc2\xba",     "&ordm;",     "<ordm/>",              "&#186;",        "\\textordmasculine{}",   nullptr,  "\\'BA",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(raquo),    "\xc2\xbb",     "&raquo;",    "<raquo/>",             "&#187;",        "\\guillemotright{}",     nullptr,  "\\'BB",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(frac14),   "\xc2\xbc",     "&frac14;",   "<frac14/>",            "&#188;",        "{$\\frac14$}",           "1/4",    "\\'BC",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(frac12),   "\xc2\xbd",     "&frac12;",   "<frac12/>",            "&#189;",        "{$\\frac12$}",           "1/2",    "\\'BD",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(frac34),   "\xc2\xbe",     "&frac34;",   "<frac34/>",            "&#190;",        "{$\\frac34$}",           "3/4",    "\\'BE",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(iquest),   "\xc2\xbf",     "&iquest;",   "<iquest/>",            "&#191;",        "?`",                     nullptr,  "\\'BF",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Agrave),   "\xc3\x80",     "&Agrave;",   "<Agrave/>",            "&#192;",        "\\`{A}",                 "A\\*:",  "\\'C0",       { "A",          HtmlEntityMapper::Perl_grave   }},
  { SYM(Aacute),   "\xc3\x81",     "&Aacute;",   "<Aacute/>",            "&#193;",        "\\'{A}",                 "A\\*(`", "\\'C1",       { "A",          HtmlEntityMapper::Perl_acute   }},
  { SYM(Acirc),    "\xc3\x82",     "&Acirc;",    "<Acirc/>",             "&#194;",        "\\^{A}",                 "A\\*^",  "\\'C2",       { "A",          HtmlEntityMapper::Perl_circ    }},
  { SYM(Atilde),   "\xc3\x83",     "&Atilde;",   "<Atilde/>",            "&#195;",        "\\~{A}",                 "A\\*~",  "\\'C3",       { "A",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(Auml),     "\xc3\x84",     "&Auml;",     "<Aumlaut/>",           "&#196;",        "\\\"{A}",                "A\\*(4", "\\'C4",       { "A",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(Aring),    "\xc3\x85",     "&Aring;",    "<Aring/>",             "&#197;",        "\\AA",                   "A\\*o",  "\\'C5",       { "A",          HtmlEntityMapper::Perl_ring    }},
  { SYM(AElig),    "\xc3\x86",     "&AElig;",    "<AElig/>",             "&#198;",        "{\\AE}",                 nullptr,  "\\'C6",       { "AElig",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(Ccedil),   "\xc3\x87",     "&Ccedil;",   "<Ccedil/>",            "&#199;",        "\\c{C}",                 "C\\*,",  "\\'C7",       { "C",          HtmlEntityMapper::Perl_cedilla }},
  { SYM(Egrave),   "\xc3\x88",     "&Egrave;",   "<Egrave/>",            "&#200;",        "\\`{E}",                 "E\\*:",  "\\'C8",       { "E",          HtmlEntityMapper::Perl_grave   }},
  { SYM(Eacute),   "\xc3\x89",     "&Eacute;",   "<Eacute/>",            "&#201;",        "\\'{E}",                 "E\\*(`", "\\'C9",       { "E",          HtmlEntityMapper::Perl_acute   }},
  { SYM(Ecirc),    "\xc3\x8a",     "&Ecirc;",    "<Ecirc/>",             "&#202;",        "\\^{E}",                 "E\\*^",  "\\'CA",       { "E",          HtmlEntityMapper::Perl_circ    }},
  { SYM(Euml),     "\xc3\x8b",     "&Euml;",     "<Eumlaut/>",           "&#203;",        "\\\"{E}",                "E\\*(4", "\\'CB",       { "E",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(Igrave),   "\xc3\x8c",     "&Igrave;",   "<Igrave/>",            "&#204;",        "\\`{I}",                 "I\\*:",  "\\'CC",       { "I",          HtmlEntityMapper::Perl_grave   }},
  { SYM(Iacute),   "\xc3\x8d",     "&Iacute;",   "<Iacute/>",            "&#205;",        "\\'{I}",                 "I\\*(`", "\\'CD",       { "I",          HtmlEntityMapper::Perl_acute   }},
  { SYM(Icirc),    "\xc3\x8e",     "&Icirc;",    "<Icirc/>",             "&#206;",        "\\^{I}",                 "I\\*^",  "\\'CE",       { "I",          HtmlEntityMapper::Perl_circ    }},
  { SYM(Iuml),     "\xc3\x8f",     "&Iuml;",     "<Iumlaut/>",           "&#207;",        "\\\"{I}",                "I\\*(4", "\\'CF",       { "I",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(ETH),      "\xc3\x90",     "&ETH;",      "<ETH/>",               "&#208;",        "\\DH",                   nullptr,  "\\'D0",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Ntilde),   "\xc3\x91",     "&Ntilde;",   "<Ntilde/>",            "&#209;",        "\\~{N}",                 "N\\*~",  "\\'D1",       { "N",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(Ograve),   "\xc3\x92",     "&Ograve;",   "<Ograve/>",            "&#210;",        "\\`{O}",                 "O\\*:",  "\\'D2",       { "O",          HtmlEntityMapper::Perl_grave   }},
  { SYM(Oacute),   "\xc3\x93",     "&Oacute;",   "<Oacute/>",            "&#211;",        "\\'{O}",                 "O\\*(`", "\\'D3",       { "O",          HtmlEntityMapper::Perl_acute   }},
  { SYM(Ocirc),    "\xc3\x94",     "&Ocirc;",    "<Ocirc/>",             "&#212;",        "\\^{O}",                 "O\\*^",  "\\'D4",       { "O",          HtmlEntityMapper::Perl_circ    }},
  { SYM(Otilde),   "\xc3\x95",     "&Otilde;",   "<Otilde/>",            "&#213;",        "\\~{O}",                 "O\\*~",  "\\'D5",       { "O",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(Ouml),     "\xc3\x96",     "&Ouml;",     "<Oumlaut/>",           "&#214;",        "\\\"{O}",                "O\\*(4", "\\'D6",       { "O",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(times),    "\xc3\x97",     "&times;",    "<times/>",             "&#215;",        "{$\\times$}",            nullptr,  "\\'D7",       { "*",          HtmlEntityMapper::Perl_char    }},
  { SYM(Oslash),   "\xc3\x98",     "&Oslash;",   "<Oslash/>",            "&#216;",        "{\\O}",                  "O\x08/", "\\'D8",       { "O",          HtmlEntityMapper::Perl_slash   }},
  { SYM(Ugrave),   "\xc3\x99",     "&Ugrave;",   "<Ugrave/>",            "&#217;",        "\\`{U}",                 "U\\*:",  "\\'D9",       { "U",          HtmlEntityMapper::Perl_grave   }},
  { SYM(Uacute),   "\xc3\x9a",     "&Uacute;",   "<Uacute/>",            "&#218;",        "\\'{U}",                 "U\\*(`", "\\'DA",       { "U",          HtmlEntityMapper::Perl_acute   }},
  { SYM(Ucirc),    "\xc3\x9b",     "&Ucirc;",    "<Ucirc/>",             "&#219;",        "\\^{U}",                 "U\\*^",  "\\'DB",       { "U",          HtmlEntityMapper::Perl_circ    }},
  { SYM(Uuml),     "\xc3\x9c",     "&Uuml;",     "<Uumlaut/>",           "&#220;",        "\\\"{U}",                "U\\*(4", "\\'DC",       { "U",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(Yacute),   "\xc3\x9d",     "&Yacute;",   "<Yacute/>",            "&#221;",        "\\'{Y}",                 "Y\\*(`", "\\'DD",       { "Y",          HtmlEntityMapper::Perl_acute   }},
  { SYM(THORN),    "\xc3\x9e",     "&THORN;",    "<THORN/>",             "&#222;",        "\\TH",                   nullptr,  "\\'DE",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(szlig),    "\xc3\x9f",     "&szlig;",    "<szlig/>",             "&#223;",        "{\\ss}",                 "s\\*:",  "\\'DF",       { "szlig",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(agrave),   "\xc3\xa0",     "&agrave;",   "<agrave/>",            "&#224;",        "\\`{a}",                 "a\\*:",  "\\'E0",       { "a",          HtmlEntityMapper::Perl_grave   }},
  { SYM(aacute),   "\xc3\xa1",     "&aacute;",   "<aacute/>",            "&#225;",        "\\'{a}",                 "a\\*(`", "\\'E1",       { "a",          HtmlEntityMapper::Perl_acute   }},
  { SYM(acirc),    "\xc3\xa2",     "&acirc;",    "<acirc/>",             "&#226;",        "\\^{a}",                 "a\\*^",  "\\'E2",       { "a",          HtmlEntityMapper::Perl_circ    }},
  { SYM(atilde),   "\xc3\xa3",     "&atilde;",   "<atilde/>",            "&#227;",        "\\~{a}",                 "a\\*~",  "\\'E3",       { "a",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(auml),     "\xc3\xa4",     "&auml;",     "<aumlaut/>",           "&#228;",        "\\\"{a}",                "a\\*(4", "\\'E4",       { "a",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(aring),    "\xc3\xa5",     "&aring;",    "<aring/>",             "&#229;",        "\\aa",                   "a\\*o",  "\\'E5",       { "a",          HtmlEntityMapper::Perl_ring    }},
  { SYM(aelig),    "\xc3\xa6",     "&aelig;",    "<aelig/>",             "&#230;",        "{\\ae}",                 nullptr,  "\\'E6",       { "aelig",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(ccedil),   "\xc3\xa7",     "&ccedil;",   "<ccedil/>",            "&#231;",        "\\c{c}",                 "c\\*,",  "\\'E7",       { "c",          HtmlEntityMapper::Perl_cedilla }},
  { SYM(egrave),   "\xc3\xa8",     "&egrave;",   "<egrave/>",            "&#232;",        "\\`{e}",                 "e\\*:",  "\\'E8",       { "e",          HtmlEntityMapper::Perl_grave   }},
  { SYM(eacute),   "\xc3\xa9",     "&eacute;",   "<eacute/>",            "&#233;",        "\\'{e}",                 "e\\*(`", "\\'E9",       { "e",          HtmlEntityMapper::Perl_acute   }},
  { SYM(ecirc),    "\xc3\xaa",     "&ecirc;",    "<ecirc/>",             "&#234;",        "\\^{e}",                 "e\\*^",  "\\'EA",       { "e",          HtmlEntityMapper::Perl_circ    }},
  { SYM(euml),     "\xc3\xab",     "&euml;",     "<eumlaut/>",           "&#235;",        "\\\"{e}",                "e\\*(4", "\\'EB",       { "e",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(igrave),   "\xc3\xac",     "&igrave;",   "<igrave/>",            "&#236;",        "\\`{\\i}",               "i\\*:",  "\\'EC",       { "i",          HtmlEntityMapper::Perl_grave   }},
  { SYM(iacute),   "\xc3\xad",     "&iacute;",   "<iacute/>",            "&#237;",        "\\'{\\i}",               "i\\*(`", "\\'ED",       { "i",          HtmlEntityMapper::Perl_acute   }},
  { SYM(icirc),    "\xc3\xae",     "&icirc;",    "<icirc/>",             "&#238;",        "\\^{\\i}",               "i\\*^",  "\\'EE",       { "i",          HtmlEntityMapper::Perl_circ    }},
  { SYM(iuml),     "\xc3\xaf",     "&iuml;",     "<iumlaut/>",           "&#239;",        "\\\"{\\i}",              "i\\*(4", "\\'EF",       { "i",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(eth),      "\xc3\xb0",     "&eth;",      "<eth/>",               "&#240;",        "\\dh",                   nullptr,  "\\'F0",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(ntilde),   "\xc3\xb1",     "&ntilde;",   "<ntilde/>",            "&#241;",        "\\~{n}",                 "n\\*~",  "\\'F1",       { "n",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(ograve),   "\xc3\xb2",     "&ograve;",   "<ograve/>",            "&#242;",        "\\`{o}",                 "o\\*:",  "\\'F2",       { "o",          HtmlEntityMapper::Perl_grave   }},
  { SYM(oacute),   "\xc3\xb3",     "&oacute;",   "<oacute/>",            "&#243;",        "\\'{o}",                 "o\\*(`", "\\'F3",       { "o",          HtmlEntityMapper::Perl_acute   }},
  { SYM(ocirc),    "\xc3\xb4",     "&ocirc;",    "<ocirc/>",             "&#244;",        "\\^{o}",                 "o\\*^",  "\\'F4",       { "o",          HtmlEntityMapper::Perl_circ    }},
  { SYM(otilde),   "\xc3\xb5",     "&otilde;",   "<otilde/>",            "&#245;",        "\\~{o}",                 "o\\*~",  "\\'F5",       { "o",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(ouml),     "\xc3\xb6",     "&ouml;",     "<oumlaut/>",           "&#246;",        "\\\"{o}",                "o\\*(4", "\\'F6",       { "o",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(divide),   "\xc3\xb7",     "&divide;",   "<divide/>",            "&#247;",        "{$\\div$}",              nullptr,  "\\'F7",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(oslash),   "\xc3\xb8",     "&oslash;",   "<oslash/>",            "&#248;",        "{\\o}",                  "o\x08/", "\\'F8",       { "o",          HtmlEntityMapper::Perl_slash   }},
  { SYM(ugrave),   "\xc3\xb9",     "&ugrave;",   "<ugrave/>",            "&#249;",        "\\`{u}",                 "u\\*:",  "\\'F9",       { "u",          HtmlEntityMapper::Perl_grave   }},
  { SYM(uacute),   "\xc3\xba",     "&uacute;",   "<uacute/>",            "&#250;",        "\\'{u}",                 "u\\*(`", "\\'FA",       { "u",          HtmlEntityMapper::Perl_acute   }},
  { SYM(ucirc),    "\xc3\xbb",     "&ucirc;",    "<ucirc/>",             "&#251;",        "\\^{u}",                 "u\\*^",  "\\'FB",       { "u",          HtmlEntityMapper::Perl_circ    }},
  { SYM(uuml),     "\xc3\xbc",     "&uuml;",     "<uumlaut/>",           "&#252;",        "\\\"{u}",                "u\\*(4", "\\'FC",       { "u",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(yacute),   "\xc3\xbd",     "&yacute;",   "<yacute/>",            "&#253;",        "\\'{y}",                 "y\\*(`", "\\'FD",       { "y",          HtmlEntityMapper::Perl_acute   }},
  { SYM(thorn),    "\xc3\xbe",     "&thorn;",    "<thorn/>",             "&#254;",        "\\th",                   nullptr,  "\\'FE",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(yuml),     "\xc3\xbf",     "&yuml;",     "<yumlaut/>",           "&#255;",        "\\\"{y}",                "y\\*(4", "\\'FF",       { "y",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(fnof),     "\xc6\x92",     "&fnof;",     "<fnof/>",              "&#402;",        "\\textflorin",           nullptr,  "\\'83",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Alpha),    "\xce\x91",     "&Alpha;",    "<Alpha/>",             "&#913;",        "A",                      nullptr,  "\\u0913?",    { "A",          HtmlEntityMapper::Perl_char    }},
  { SYM(Beta),     "\xce\x92",     "&Beta;",     "<Beta/>",              "&#914;",        "B",                      nullptr,  "\\u0914?",    { "B",          HtmlEntityMapper::Perl_char    }},
  { SYM(Gamma),    "\xce\x93",     "&Gamma;",    "<Gamma/>",             "&#915;",        "{$\\Gamma$}",            nullptr,  "\\u0915?",    { "Gamma",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(Delta),    "\xce\x94",     "&Delta;",    "<Delta/>",             "&#916;",        "{$\\Delta$}",            nullptr,  "\\u0916?",    { "Delta",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(Epsilon),  "\xce\x95",     "&Epsilon;",  "<Epsilon/>",           "&#917;",        "E",                      nullptr,  "\\u0917?",    { "E",          HtmlEntityMapper::Perl_char    }},
  { SYM(Zeta),     "\xce\x96",     "&Zeta;",     "<Zeta/>",              "&#918;",        "Z",                      nullptr,  "\\u0918?",    { "Z",          HtmlEntityMapper::Perl_char    }},
  { SYM(Eta),      "\xce\x97",     "&Eta;",      "<Eta/>",               "&#919;",        "H",                      nullptr,  "\\u0919?",    { "H",          HtmlEntityMapper::Perl_char    }},
  { SYM(Theta),    "\xce\x98",     "&Theta;",    "<Theta/>",             "&#920;",        "{$\\Theta$}",            nullptr,  "\\u0920?",    { "Theta",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(Iota),     "\xce\x99",     "&Iota;",     "<Iota/>",              "&#921;",        "I",                      nullptr,  "\\u0921?",    { "I",          HtmlEntityMapper::Perl_char    }},
  { SYM(Kappa),    "\xce\x9a",     "&Kappa;",    "<Kappa/>",             "&#922;",        "K",                      nullptr,  "\\u0922?",    { "K",          HtmlEntityMapper::Perl_char    }},
  { SYM(Lambda),   "\xce\x9b",     "&Lambda;",   "<Lambda/>",            "&#923;",        "{$\\Lambda$}",           nullptr,  "\\u0923?",    { "Lambda",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(Mu),       "\xce\x9c",     "&Mu;",       "<Mu/>",                "&#924;",        "M",                      nullptr,  "\\u0924?",    { "M",          HtmlEntityMapper::Perl_char    }},
  { SYM(Nu),       "\xce\x9d",     "&Nu;",       "<Nu/>",                "&#925;",        "N",                      nullptr,  "\\u0925?",    { "N",          HtmlEntityMapper::Perl_char    }},
  { SYM(Xi),       "\xce\x9e",     "&Xi;",       "<Xi/>",                "&#926;",        "{$\\Xi$}",               nullptr,  "\\u0926?",    { "Xi",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(Omicron),  "\xce\x9f",     "&Omicron;",  "<Omicron/>",           "&#927;",        "O",                      nullptr,  "\\u0927?",    { "O",          HtmlEntityMapper::Perl_char    }},
  { SYM(Pi),       "\xce\xa0",     "&Pi;",       "<Pi/>",                "&#928;",        "{$\\Pi$}",               nullptr,  "\\u0928?",    { "Pi",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(Rho),      "\xce\xa1",     "&Rho;",      "<Rho/>",               "&#929;",        "P",                      nullptr,  "\\u0929?",    { "P",          HtmlEntityMapper::Perl_char    }},
  { SYM(Sigma),    "\xce\xa3",     "&Sigma;",    "<Sigma/>",             "&#931;",        "{$\\Sigma$}",            nullptr,  "\\u0931?",    { "Sigma",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(Tau),      "\xce\xa4",     "&Tau;",      "<Tau/>",               "&#932;",        "T",                      nullptr,  "\\u0932?",    { "T",          HtmlEntityMapper::Perl_char    }},
  { SYM(Upsilon),  "\xce\xa5",     "&Upsilon;",  "<Upsilon/>",           "&#933;",        "{$\\Upsilon$}",          nullptr,  "\\u0933?",    { "Upsilon",    HtmlEntityMapper::Perl_symbol  }},
  { SYM(Phi),      "\xce\xa6",     "&Phi;",      "<Phi/>",               "&#934;",        "{$\\Phi$}",              nullptr,  "\\u0934?",    { "Phi",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(Chi),      "\xce\xa7",     "&Chi;",      "<Chi/>",               "&#935;",        "X",                      nullptr,  "\\u0935?",    { "X",          HtmlEntityMapper::Perl_char    }},
  { SYM(Psi),      "\xce\xa8",     "&Psi;",      "<Psi/>",               "&#936;",        "{$\\Psi$}",              nullptr,  "\\u0936?",    { "Psi",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(Omega),    "\xce\xa9",     "&Omega;",    "<Omega/>",             "&#937;",        "{$\\Omega$}",            nullptr,  "\\u0937?",    { "Omega",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(alpha),    "\xce\xb1",     "&alpha;",    "<alpha/>",             "&#945;",        "{$\\alpha$}",            nullptr,  "\\u0945?",    { "alpha",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(beta),     "\xce\xb2",     "&beta;",     "<beta/>",              "&#946;",        "{$\\beta$}",             nullptr,  "\\u0946?",    { "beta",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(gamma),    "\xce\xb3",     "&gamma;",    "<gamma/>",             "&#947;",        "{$\\gamma$}",            nullptr,  "\\u0947?",    { "gamma",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(delta),    "\xce\xb4",     "&delta;",    "<delta/>",             "&#948;",        "{$\\delta$}",            nullptr,  "\\u0948?",    { "delta",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(epsilon),  "\xce\xb5",     "&epsilon;",  "<epsilon/>",           "&#949;",        "{$\\varepsilon$}",       nullptr,  "\\u0949?",    { "epsilon",    HtmlEntityMapper::Perl_symbol  }},
  { SYM(zeta),     "\xce\xb6",     "&zeta;",     "<zeta/>",              "&#950;",        "{$\\zeta$}",             nullptr,  "\\u0950?",    { "zeta",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(eta),      "\xce\xb7",     "&eta;",      "<eta/>",               "&#951;",        "{$\\eta$}",              nullptr,  "\\u0951?",    { "eta",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(theta),    "\xce\xb8",     "&theta;",    "<theta/>",             "&#952;",        "{$\\theta$}",            nullptr,  "\\u0952?",    { "theta",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(iota),     "\xce\xb9",     "&iota;",     "<iota/>",              "&#953;",        "{$\\iota$}",             nullptr,  "\\u0953?",    { "iota",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(kappa),    "\xce\xba",     "&kappa;",    "<kappa/>",             "&#954;",        "{$\\kappa$}",            nullptr,  "\\u0954?",    { "kappa",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(lambda),   "\xce\xbb",     "&lambda;",   "<lambda/>",            "&#955;",        "{$\\lambda$}",           nullptr,  "\\u0955?",    { "lambda",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(mu),       "\xce\xbc",     "&mu;",       "<mu/>",                "&#956;",        "{$\\mu$}",               nullptr,  "\\u0956?",    { "mu",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(nu),       "\xce\xbd",     "&nu;",       "<nu/>",                "&#957;",        "{$\\nu$}",               nullptr,  "\\u0957?",    { "nu",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(xi),       "\xce\xbe",     "&xi;",       "<xi/>",                "&#958;",        "{$\\xi$}",               nullptr,  "\\u0958?",    { "xi",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(omicron),  "\xce\xbf",     "&omicron;",  "<omicron/>",           "&#959;",        "{$\\omicron$}",          nullptr,  "\\u0959?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(pi),       "\xcf\x80",     "&pi;",       "<pi/>",                "&#960;",        "{$\\pi$}",               nullptr,  "\\u0960?",    { "pi",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(rho),      "\xcf\x81",     "&rho;",      "<rho/>",               "&#961;",        "{$\\rho$}",              nullptr,  "\\u0961?",    { "rho",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(sigmaf),   "\xcf\x82",     "&sigmaf;",   "<sigmaf/>",            "&#962;",        "{$\\varsigma$}",         nullptr,  "\\u0962?",    { "sigma",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(sigma),    "\xcf\x83",     "&sigma;",    "<sigma/>",             "&#963;",        "{$\\sigma$}",            nullptr,  "\\u0963?",    { "sigma",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(tau),      "\xcf\x84",     "&tau;",      "<tau/>",               "&#964;",        "{$\\tau$}",              nullptr,  "\\u0964?",    { "tau",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(upsilon),  "\xcf\x85",     "&upsilon;",  "<upsilon/>",           "&#965;",        "{$\\upsilon$}",          nullptr,  "\\u0965?",    { "upsilon",    HtmlEntityMapper::Perl_symbol  }},
  { SYM(phi),      "\xcf\x86",     "&phi;",      "<phi/>",               "&#966;",        "{$\\varphi$}",           nullptr,  "\\u0966?",    { "phi",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(chi),      "\xcf\x87",     "&chi;",      "<chi/>",               "&#967;",        "{$\\chi$}",              nullptr,  "\\u0967?",    { "chi",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(psi),      "\xcf\x88",     "&psi;",      "<psi/>",               "&#968;",        "{$\\psi$}",              nullptr,  "\\u0968?",    { "psi",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(omega),    "\xcf\x89",     "&omega;",    "<omega/>",             "&#969;",        "{$\\omega$}",            nullptr,  "\\u0969?",    { "omega",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(thetasym), "\xcf\x91",     "&thetasym;", "<thetasym/>",          "&#977;",        "{$\\vartheta$}",         nullptr,  "\\u977?",     { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(upsih),    "\xcf\x92",     "&upsih;",    "<upsih/>",             "&#978;",        "{$\\Upsilon$}",          nullptr,  "\\u978?",     { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(piv),      "\xcf\x96",     "&piv;",      "<piv/>",               "&#982;",        "{$\\varpi$}",            nullptr,  "\\u982?",     { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(bull),     "\xe2\x80\xa2", "&bull;",     "<bull/>",              "&#8226;",       "\\textbullet{}",         nullptr,  "\\'95",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(hellip),   "\xe2\x80\xa6", "&hellip;",   "<hellip/>",            "&#8230;",       "{$\\dots$}",             nullptr,  "\\'85",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(prime),    "\xe2\x80\xb2", "&prime;",    "<prime/>",             "&#8242;",       "'",                      nullptr,  "\\u8242?",    { "\\\'",       HtmlEntityMapper::Perl_string  }},
  { SYM(Prime),    "\xe2\x80\xb3", "&Prime;",    "<Prime/>",             "&#8243;",       "''",                     nullptr,  "\\u8243?",    { "\"",         HtmlEntityMapper::Perl_char    }},
  { SYM(oline),    "\xe2\x80\xbe", "&oline;",    "<oline/>",             "&#8254;",       "{$\\overline{\\,}$}",    nullptr,  "\\u8254?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(frasl),    "\xe2\x81\x84", "&frasl;",    "<frasl/>",             "&#8260;",       "/",                      nullptr,  "\\u8260?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(weierp),   "\xe2\x84\x98", "&weierp;",   "<weierp/>",            "&#8472;",       "{$\\wp$}",               nullptr,  "\\u8472?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(image),    "\xe2\x84\x91", "&image;",    "<imaginary/>",         "&#8465;",       "{$\\Im$}",               nullptr,  "\\u8465?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(real),     "\xe2\x84\x9c", "&real;",     "<real/>",              "&#8476;",       "{$\\Re$}",               nullptr,  "\\u8476?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(trade),    "\xe2\x84\xa2", "&trade;",    "<trademark/>",         "&#8482;",       "\\texttrademark{}",      "(TM)",   "\\'99",       { "trademark",  HtmlEntityMapper::Perl_symbol  }},
  { SYM(alefsym),  "\xe2\x85\xb5", "&alefsym;",  "<alefsym/>",           "&#8501;",       "{$\\aleph$}",            nullptr,  "\\u8501?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(larr),     "\xe2\x86\x90", "&larr;",     "<larr/>",              "&#8592;",       "{$\\leftarrow$}",        nullptr,  "\\u8592?",    { "<-",         HtmlEntityMapper::Perl_string  }},
  { SYM(uarr),     "\xe2\x86\x91", "&uarr;",     "<uarr/>",              "&#8593;",       "{$\\uparrow$}",          nullptr,  "\\u8593?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(rarr),     "\xe2\x86\x92", "&rarr;",     "<rarr/>",              "&#8594;",       "{$\\rightarrow$}",       nullptr,  "\\u8594?",    { "->",         HtmlEntityMapper::Perl_string  }},
  { SYM(darr),     "\xe2\x86\x93", "&darr;",     "<darr/>",              "&#8595;",       "{$\\downarrow$}",        nullptr,  "\\u8595?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(harr),     "\xe2\x86\x94", "&harr;",     "<harr/>",              "&#8596;",       "{$\\leftrightarrow$}",   nullptr,  "\\u8596?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(crarr),    "\xe2\x86\xb5", "&crarr;",    "<crarr/>",             "&#8629;",       "{$\\hookleftarrow$}",    nullptr,  "\\u8629?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(lArr),     "\xe2\x87\x90", "&lArr;",     "<lArr/>",              "&#8656;",       "{$\\Leftarrow$}",        nullptr,  "\\u8656?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(uArr),     "\xe2\x87\x91", "&uArr;",     "<uArr/>",              "&#8657;",       "{$\\Uparrow$}",          nullptr,  "\\u8657?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(rArr),     "\xe2\x87\x92", "&rArr;",     "<rArr/>",              "&#8658;",       "{$\\Rightarrow$}",       nullptr,  "\\u8658?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(dArr),     "\xe2\x87\x93", "&dArr;",     "<dArr/>",              "&#8659;",       "{$\\Downarrow$}",        nullptr,  "\\u8659?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(hArr),     "\xe2\x87\x94", "&hArr;",     "<hArr/>",              "&#8660;",       "{$\\Leftrightarrow$}",   nullptr,  "\\u8660?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(forall),   "\xe2\x88\x80", "&forall;",   "<forall/>",            "&#8704;",       "{$\\forall$}",           nullptr,  "\\u8704?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(part),     "\xe2\x88\x82", "&part;",     "<part/>",              "&#8706;",       "{$\\partial$}",          nullptr,  "\\u8706?",    { "partial",    HtmlEntityMapper::Perl_symbol  }},
  { SYM(exist),    "\xe2\x88\x83", "&exist;",    "<exist/>",             "&#8707;",       "{$\\exists$}",           nullptr,  "\\u8707?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(empty),    "\xe2\x88\x85", "&empty;",    "<empty/>",             "&#8709;",       "{$\\emptyset$}",         nullptr,  "\\u8709?",    { "empty",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(nabla),    "\xe2\x88\x87", "&nabla;",    "<nabla/>",             "&#8711;",       "{$\\nabla$}",            nullptr,  "\\u8711?",    { "nabla",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(isin),     "\xe2\x88\x88", "&isin;",     "<isin/>",              "&#8712;",       "{$\\in$}",               nullptr,  "\\u8712?",    { "in",         HtmlEntityMapper::Perl_symbol  }},
  { SYM(notin),    "\xe2\x88\x89", "&notin;",    "<notin/>",             "&#8713;",       "{$\\notin$}",            nullptr,  "\\u8713?",    { "notin",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(ni),       "\xe2\x88\x8b", "&ni;",       "<ni/>",                "&#8715;",       "{$\\ni$}",               nullptr,  "\\u8715?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(prod),     "\xe2\x88\x8f", "&prod;",     "<prod/>",              "&#8719;",       "{$\\prod$}",             nullptr,  "\\u8719?",    { "prod",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(sum),      "\xe2\x88\x91", "&sum;",      "<sum/>",               "&#8721;",       "{$\\sum$}",              nullptr,  "\\u8721?",    { "sum",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(minus),    "\xe2\x88\x92", "&minus;",    "<minus/>",             "&#8722;",       "-",                      nullptr,  "\\u8722?",    { "-",          HtmlEntityMapper::Perl_char    }},
  { SYM(lowast),   "\xe2\x88\x97", "&lowast;",   "<lowast/>",            "&#8727;",       "{$\\ast$}",              nullptr,  "\\u8727?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(radic),    "\xe2\x88\x9a", "&radic;",    "<radic/>",             "&#8730;",       "{$\\surd$}",             nullptr,  "\\u8730?",    { "sqrt",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(prop),     "\xe2\x88\x9d", "&prop;",     "<prop/>",              "&#8733;",       "{$\\propto$}",           nullptr,  "\\u8733?",    { "propto",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(infin),    "\xe2\x88\x9e", "&infin;",    "<infin/>",             "&#8734;",       "{$\\infty$}",            nullptr,  "\\u8734?",    { "inf",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(ang),      "\xe2\x88\xa0", "&ang;",      "<ang/>",               "&#8736;",       "{$\\angle$}",            nullptr,  "\\u8736?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(and),      "\xe2\x88\xa7", "&and;",      "<and/>",               "&#8743;",       "{$\\wedge$}",            nullptr,  "\\u8743?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(or),       "\xe2\x88\xa8", "&or;",       "<or/>",                "&#8744;",       "{$\\vee$}",              nullptr,  "\\u8744?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(cap),      "\xe2\x88\xa9", "&cap;",      "<cap/>",               "&#8745;",       "{$\\cap$}",              nullptr,  "\\u8745?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(cup),      "\xe2\x88\xaa", "&cup;",      "<cup/>",               "&#8746;",       "{$\\cup$}",              nullptr,  "\\u8746?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(int),      "\xe2\x88\xab", "&int;",      "<int/>",               "&#8747;",       "{$\\int$}",              nullptr,  "\\u8747?",    { "int",        HtmlEntityMapper::Perl_symbol  }},
  { SYM(there4),   "\xe2\x88\xb4", "&there4;",   "<there4/>",            "&#8756;",       "{$\\therefore$}",        nullptr,  "\\u8756?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(sim),      "\xe2\x88\xbc", "&sim;",      "<sim/>",               "&#8764;",       "{$\\sim$}",              nullptr,  "\\u8764?",    { "~",          HtmlEntityMapper::Perl_char    }},
  { SYM(cong),     "\xe2\x89\x85", "&cong;",     "<cong/>",              "&#8773;",       "{$\\cong$}",             nullptr,  "\\u8773?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(asymp),    "\xe2\x89\x88", "&asymp;",    "<asymp/>",             "&#8776;",       "{$\\approx$}",           nullptr,  "\\u8776?",    { "approx",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(ne),       "\xe2\x89\xa0", "&ne;",       "<ne/>",                "&#8800;",       "{$\\ne$}",               nullptr,  "\\u8800?",    { "!=",         HtmlEntityMapper::Perl_string  }},
  { SYM(equiv),    "\xe2\x89\xa1", "&equiv;",    "<equiv/>",             "&#8801;",       "{$\\equiv$}",            nullptr,  "\\u8801?",    { "equiv",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(le),       "\xe2\x89\xa4", "&le;",       "<le/>",                "&#8804;",       "{$\\le$}",               nullptr,  "\\u8804?",    { "<=",         HtmlEntityMapper::Perl_string  }},
  { SYM(ge),       "\xe2\x89\xa5", "&ge;",       "<ge/>",                "&#8805;",       "{$\\ge$}",               nullptr,  "\\u8805?",    { ">=",         HtmlEntityMapper::Perl_string  }},
  { SYM(sub),      "\xe2\x8a\x82", "&sub;",      "<sub/>",               "&#8834;",       "{$\\subset$}",           nullptr,  "\\u8834?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(sup),      "\xe2\x8a\x83", "&sup;",      "<sup/>",               "&#8835;",       "{$\\supset$}",           nullptr,  "\\u8835?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(nsub),     "\xe2\x8a\x84", "&nsub;",     "<nsub/>",              "&#8836;",       "{$\\not\\subset$}",      nullptr,  "\\u8836?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(sube),     "\xe2\x8a\x86", "&sube;",     "<sube/>",              "&#8838;",       "{$\\subseteq$}",         nullptr,  "\\u8838?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(supe),     "\xe2\x8a\x87", "&supe;",     "<supe/>",              "&#8839;",       "{$\\supseteq$}",         nullptr,  "\\u8839?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(oplus),    "\xe2\x8a\x95", "&oplus;",    "<oplus/>",             "&#8853;",       "{$\\oplus$}",            nullptr,  "\\u8853?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(otimes),   "\xe2\x8a\x97", "&otimes;",   "<otimes/>",            "&#8855;",       "{$\\otimes$}",           nullptr,  "\\u8855?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(perp),     "\xe2\x8a\xa5", "&perp;",     "<perp/>",              "&#8869;",       "{$\\perp$}",             nullptr,  "\\u8869?",    { "perp",       HtmlEntityMapper::Perl_symbol  }},
  { SYM(sdot),     "\xe2\x8b\x85", "&sdot;",     "<sdot/>",              "&#8901;",       "{$\\cdot$}",             nullptr,  "\\u8901?",    { ".",          HtmlEntityMapper::Perl_char    }},
  { SYM(lceil),    "\xe2\x8c\x88", "&lceil;",    "<lceil/>",             "&#8968;",       "{$\\lceil$}",            nullptr,  "\\u8968?",    { "lceil",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(rceil),    "\xe2\x8c\x89", "&rceil;",    "<rceil/>",             "&#8969;",       "{$\\rceil$}",            nullptr,  "\\u8969?",    { "rceil",      HtmlEntityMapper::Perl_symbol  }},
  { SYM(lfloor),   "\xe2\x8c\x8a", "&lfloor;",   "<lfloor/>",            "&#8970;",       "{$\\lfloor$}",           nullptr,  "\\u8970?",    { "lfloor",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(rfloor),   "\xe2\x8c\x8b", "&rfloor;",   "<rfloor/>",            "&#8971;",       "{$\\rfloor$}",           nullptr,  "\\u8971?",    { "rfloor",     HtmlEntityMapper::Perl_symbol  }},
  { SYM(lang),     "\xe2\x8c\xa9", "&lang;",     "<lang/>",              "&#9001;",       "{$\\langle$}",           nullptr,  "\\u9001?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(rang),     "\xe2\x8c\xaa", "&rang;",     "<rang/>",              "&#9002;",       "{$\\rangle$}",           nullptr,  "\\u9002?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(loz),      "\xe2\x97\x8a", "&loz;",      "<loz/>",               "&#9674;",       "{$\\lozenge$}",          nullptr,  "\\u9674?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(spades),   "\xe2\x99\xa0", "&spades;",   "<spades/>",            "&#9824;",       "{$\\spadesuit$}",        nullptr,  "\\u9824?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(clubs),    "\xe2\x99\xa3", "&clubs;",    "<clubs/>",             "&#9827;",       "{$\\clubsuit$}",         nullptr,  "\\u9827?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(hearts),   "\xe2\x99\xa5", "&hearts;",   "<hearts/>",            "&#9829;",       "{$\\heartsuit$}",        nullptr,  "\\u9829?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(diams),    "\xe2\x99\xa6", "&diams;",    "<diams/>",             "&#9830;",       "{$\\diamondsuit$}",      nullptr,  "\\u9830?",    { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(quot),     "\"",           "&quot;",     "\"",                   "&quot;",        "\"{}",                   "\"",     "\"",          { "\"",         HtmlEntityMapper::Perl_char    }},
  { SYM(amp),      "&",            "&amp;",      "&amp;",                "&amp;",         "\\&",                    "&",      "&",           { "&",          HtmlEntityMapper::Perl_char    }},
  { SYM(lt),       "<",            "&lt;",       "&lt;",                 "&lt;",          "<",                      "<",      "<",           { "<",          HtmlEntityMapper::Perl_char    }},
  { SYM(gt),       ">",            "&gt;",       "&gt;",                 "&gt;",          ">",                      ">",      ">",           { ">",          HtmlEntityMapper::Perl_char    }},
  { SYM(OElig),    "\xc5\x92",     "&OElig;",    "<OElig/>",             "&#338;",        "\\OE",                   nullptr,  "\\'8C",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(oelig),    "\xc5\x93",     "&oelig;",    "<oelig/>",             "&#339;",        "\\oe",                   nullptr,  "\\'9C",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Scaron),   "\xc5\xa0",     "&Scaron;",   "<Scaron/>",            "&#352;",        "\\v{S}",                 nullptr,  "\\'8A",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(scaron),   "\xc5\xa1",     "&scaron;",   "<scaron/>",            "&#353;",        "\\v{s}",                 nullptr,  "\\'9A",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Yuml),     "\xc5\xb8",     "&Yuml;",     "<Yumlaut/>",           "&#376;",        "\\\"{Y}",                "Y\\*(4", "\\'9F",       { "Y",          HtmlEntityMapper::Perl_umlaut  }},
  { SYM(circ),     "\xcb\x86",     "&circ;",     "<circ/>",              "&#710;",        "{$\\circ$}",             nullptr,  "\\'88",       { " ",          HtmlEntityMapper::Perl_circ    }},
  { SYM(tilde),    "\xcb\x9c",     "&tilde;",    "<tilde/>",             "&#732;",        "\\~{}",                  "~",      "\\'98",       { " ",          HtmlEntityMapper::Perl_tilde   }},
  { SYM(ensp),     "\xe2\x80\x82", "&ensp;",     "<ensp/>",              "&#8194;",       "\\enskip{}",             nullptr,  "{\\enspace}", { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(emsp),     "\xe2\x80\x83", "&emsp;",     "<emsp/>",              "&#8195;",       "\\quad{}",               nullptr,  "{\\emspace}", { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(thinsp),   "\xe2\x80\x89", "&thinsp;",   "<thinsp/>",            "&#8201;",       "\\,",                    nullptr,  "{\\qmspace}", { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(zwnj),     "\xe2\x80\x8c", "&zwnj;",     "<zwnj/>",              "&#8204;",       "{}",                     nullptr,  "\\zwnj",      { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(zwj),      "\xe2\x80\x8d", "&zwj;",      "<zwj/>",               "&#8205;",       "",                       nullptr,  "\\zwj",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(lrm),      "\xe2\x80\x8e", "&lrm;",      "<lrm/>",               "&#8206;",       "",                       nullptr,  "\\ltrmark",   { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(rlm),      "\xe2\x80\x8f", "&rlm;",      "<rlm/>",               "&#8207;",       "",                       nullptr,  "\\rtlmark",   { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(ndash),    "\xe2\x80\x93", "&ndash;",    "<ndash/>",             "&#8211;",       "--",                     "--",     "\\'96",       { "-",          HtmlEntityMapper::Perl_char    }},
  { SYM(mdash),    "\xe2\x80\x94", "&mdash;",    "<mdash/>",             "&#8212;",       "---",                    "---",    "\\'97",       { "--",         HtmlEntityMapper::Perl_string  }},
  { SYM(lsquo),    "\xe2\x80\x98", "&lsquo;",    "<lsquo/>",             "&#8216;",       "`",                      "`",      "\\'91",       { "\\\'",       HtmlEntityMapper::Perl_string  }},
  { SYM(rsquo),    "\xe2\x80\x99", "&rsquo;",    "<rsquo/>",             "&#8217;",       "'",                      "'",      "\\'92",       { "\\\'",       HtmlEntityMapper::Perl_string  }},
  { SYM(sbquo),    "\xe2\x80\x9a", "&sbquo;",    "<sbquo/>",             "&#8218;",       "\\quotesinglbase{}",     nullptr,  "\\'82",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(ldquo),    "\xe2\x80\x9c", "&ldquo;",    "<ldquo/>",             "&#8220;",       "``",                     "``",     "\\'93",       { "\"",         HtmlEntityMapper::Perl_char    }},
  { SYM(rdquo),    "\xe2\x80\x9d", "&rdquo;",    "<rdquo/>",             "&#8221;",       "''",                     "''",     "\\'94",       { "\"",         HtmlEntityMapper::Perl_char    }},
  { SYM(bdquo),    "\xe2\x80\x9e", "&bdquo;",    "<bdquo/>",             "&#8222;",       "\\quotedblbase{}",       nullptr,  "\\'84",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(dagger),   "\xe2\x80\xa0", "&dagger;",   "<dagger/>",            "&#8224;",       "{$\\dagger$}",           nullptr,  "\\'86",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(Dagger),   "\xe2\x80\xa1", "&Dagger;",   "<Dagger/>",            "&#8225;",       "{$\\ddagger$}",          nullptr,  "\\'87",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(permil),   "\xe2\x80\xb0", "&permil;",   "<permil/>",            "&#8240;",       "{$\\permil{}$}",         nullptr,  "\\'89",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(lsaquo),   "\xe2\x80\xb9", "&lsaquo;",   "<lsaquo/>",            "&#8249;",       "\\guilsinglleft{}",      nullptr,  "\\'8B",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(rsaquo),   "\xe2\x80\xba", "&rsaquo;",   "<rsaquo/>",            "&#8250;",       "\\guilsinglright{}",     nullptr,  "\\'9B",       { nullptr,      HtmlEntityMapper::Perl_unknown }},
  { SYM(euro),     "\xe2\x82\xac", "&euro;",     "<euro/>",              "&#8364;",       "\\texteuro{}",           nullptr,  "\\'80",       { nullptr,      HtmlEntityMapper::Perl_unknown }},

  // doxygen extension to the HTML4 table of HTML entities
  { SYM(tm),       "\xe2\x84\xa2", "&trade;",    "<tm/>",                "&#8482;",       "\\texttrademark{}",      "(TM)",   "\\'99",       { "trademark",  HtmlEntityMapper::Perl_symbol  }},
  { SYM(apos),     "'",            "'",          "'",                    "&apos;",        "\\textquotesingle{}",    "'",      "'",           { "\\\'",       HtmlEntityMapper::Perl_string  }},

  // doxygen commands represented as HTML entities
/* 1  */  { SYM(BSlash),   "\\",           "\\",         "\\",                   "\\",            "\\textbackslash{}",      "\\\\",   "\\\\",        { "\\\\",       HtmlEntityMapper::Perl_string  }},
/* 2  */  { SYM(At),       "@",            "@",          "@",                    "@",             "@",                      "@",      "@",           { "@",          HtmlEntityMapper::Perl_char    }},
/* 3  */  { SYM(Less),     "<",            "&lt;",       "&lt;",                 "&lt;",          "<",                      "<",      "<",           { "<",          HtmlEntityMapper::Perl_char    }},
/* 4  */  { SYM(Greater),  ">",            "&gt;",       "&gt;",                 "&gt;",          ">",                      ">",      ">",           { ">",          HtmlEntityMapper::Perl_char    }},
/* 5  */  { SYM(Amp),      "&",            "&amp;",      "&amp;",                "&amp;",         "\\&",                    "&",      "&",           { "&",          HtmlEntityMapper::Perl_char    }},
/* 6  */  { SYM(Dollar),   "$",            "$",          "$",                    "$",             "\\$",                    "$",      "$",           { "$",          HtmlEntityMapper::Perl_char    }},
/* 7  */  { SYM(Hash),     "#;",           "#",          "#",                    "#",             "\\#",                    "#",      "#",           { "#",          HtmlEntityMapper::Perl_char    }},
/* 8  */  { SYM(DoubleColon), "::",        "::",         "::",                   "::",            "::",                     "::",     "::",          { "::",         HtmlEntityMapper::Perl_string  }},
/* 9  */  { SYM(Percent),  "%",            "%",          "%",                    "%",             "\\%",                    "%",      "%",           { "%",          HtmlEntityMapper::Perl_char    }},
/* 10 */  { SYM(Pipe),     "|",            "|",          "|",                    "|",             "$|$",                    "|",      "|",           { "|",          HtmlEntityMapper::Perl_char    }},
/* 11 */  { SYM(Quot),     "\"",           "\"",         "\"",                   "&quot;",        "\"{}",                   "\"",     "\"",          { "\"",         HtmlEntityMapper::Perl_char    }},
/* 12 */  { SYM(Minus),    "-",            "-",          "-",                    "-",             "-\\/",                   "-",      "-",           { "-",          HtmlEntityMapper::Perl_char    }},
/* 13 */  { SYM(Plus),     "+",            "+",          "+",                    "+",             "+",                      "+",      "+",           { "+",          HtmlEntityMapper::Perl_char    }},
/* 14 */  { SYM(Dot),      ".",            ".",          ".",                    ".",             ".",                      ".",      ".",           { ".",          HtmlEntityMapper::Perl_char    }},
/* 15 */  { SYM(Colon),    ":",            ":",          ":",                    ":",             ":",                      ":",      ":",           { ":",          HtmlEntityMapper::Perl_char    }},
/* 16 */  { SYM(Equal),    "=",            "=",          "=",                    "=",             "=",                      "=",      "=",           { "=",          HtmlEntityMapper::Perl_char    }},
/* 17 */  { SYM(Exclam),   "!",            "!",          "!",                    "!",             "!",                      "!",      "!",           { "!",          HtmlEntityMapper::Perl_char    }},
/* 18 */  { SYM(Quest),    "?",            "&quest;",    "?",                    "?",             "?",                      "?",      "?",           { "?",          HtmlEntityMapper::Perl_char    }}
};

HtmlEntityMapper::HtmlEntityMapper()
{
  for (const auto &entity : g_htmlEntities)
  {
    m_name2sym.emplace(entity.item,entity.symb);
  }
  validate();
}

HtmlEntityMapper::~HtmlEntityMapper()
{
}

/** Returns the one and only instance of the HTML entity mapper */
HtmlEntityMapper &HtmlEntityMapper::instance()
{
  static HtmlEntityMapper inst;
  return inst;
}

/*! @brief Access routine to the UTF8 code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @param useInPrintf If TRUE the result will be escaped such that it can be
 *                    used in a printf string pattern
 * @return the UTF8 code of the HTML entity,
 *         in case the UTF code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::utf8(HtmlEntityMapper::SymType symb,bool useInPrintf) const
{
  if (useInPrintf && symb==HtmlEntityMapper::Sym_Percent)
  {
    return "%%"; // escape for printf
  }
  else
  {
    return g_htmlEntities[symb].UTF8;
  }
}

/*! @brief Access routine to the html code of the HTML entity
 *
 * @param symb        Code of the requested HTML entity
 * @param useInPrintf If TRUE the result will be escaped such that it can be
 *                    used in a printf string pattern
 * @return the html representation of the HTML entity,
 *         in case the html code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::html(HtmlEntityMapper::SymType symb,bool useInPrintf) const
{
  if (useInPrintf && symb==HtmlEntityMapper::Sym_Percent)
  {
    return "%%"; // escape for printf
  }
  else
  {
    return g_htmlEntities[symb].html;
  }
}

/*! @brief Access routine to the XML code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the XML code of the HTML entity,
 *         in case the XML code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::xml(HtmlEntityMapper::SymType symb) const
{
  return g_htmlEntities[symb].xml;
}

/*! @brief Access routine to the docbook code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the docbook code of the HTML entity,
 *         in case the docbook code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::docbook(HtmlEntityMapper::SymType symb) const
{
  return g_htmlEntities[symb].docbook;
}

/*! @brief Access routine to the LaTeX code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the LaTeX code of the HTML entity,
 *         in case the LaTeX code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::latex(HtmlEntityMapper::SymType symb) const
{
  return g_htmlEntities[symb].latex;
}

/*! @brief Access routine to the man code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the man of the HTML entity,
 *         in case the man code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::man(HtmlEntityMapper::SymType symb) const
{
  return g_htmlEntities[symb].man;
}

/*! @brief Access routine to the RTF code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the RTF of the HTML entity,
 *         in case the RTF code is unknown \c nullptr is returned.
 */
const char *HtmlEntityMapper::rtf(HtmlEntityMapper::SymType symb) const
{
  return g_htmlEntities[symb].rtf;
}

/*! @brief Access routine to the perl struct with the perl code of the HTML entity
 *
 * @param symb Code of the requested HTML entity
 * @return the pointer to perl struct with the perl code of the HTML entity,
 *         in case the perl code does not exists the nullptr pointer is entered in the
 *         \c symb field and in the `HtmlEntityMapper::Perl_unknown` in the \c type field.
 */
const HtmlEntityMapper::PerlSymb *HtmlEntityMapper::perl(HtmlEntityMapper::SymType symb) const
{
  return &g_htmlEntities[symb].perl;
}

/*!
 * @brief Give code of the requested HTML entity name
 * @param symName HTML entity name without \c & and \c ;
 * @return the code for the requested HTML entity name,
 *         in case the requested HTML item does not exist `HtmlEntityMapper::Sym_unknown` is returned.
 */
HtmlEntityMapper::SymType HtmlEntityMapper::name2sym(const QCString &symName) const
{
  auto it = m_name2sym.find(symName.str());
  return it!=m_name2sym.end() ? it->second : HtmlEntityMapper::Sym_Unknown;
}

void HtmlEntityMapper::writeXMLSchema(TextStream &t)
{
  for (size_t i=0;i<g_htmlEntities.size();i++)
  {
    QCString bareName = g_htmlEntities[i].xml;
    if (!bareName.isEmpty() && bareName.at(0)=='<' && bareName.endsWith("/>"))
    {
      bareName = bareName.mid(1,bareName.length()-3); // strip < and />
      t << "      <xsd:element name=\"" << bareName << "\" type=\"docEmptyType\" />\n";
    }
  }
}

/*! @brief Routine to check if the entries of the html_entities are numbered correctly
 *  @details in case of a mismatch a warning message is given.
 */
void HtmlEntityMapper::validate()
{
  int i=0;
  for (const auto &entity : g_htmlEntities)
  {
    if (i != entity.symb)
    {
      warn_uncond("Internal inconsistency, htmlentries code {} (item={})\n",i,entity.item);
    }
    i++;
  }
}
