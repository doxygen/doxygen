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

#include <qdict.h>
#include "docparser.h"

class FTextStream;

/** @brief Singleton helper class to map html entities to other formats */
class HtmlEntityMapper
{
  public:
    static HtmlEntityMapper *instance();
    static void deleteInstance();
    DocSymbol::SymType name2sym(const QCString &symName) const;
    const char *utf8(DocSymbol::SymType symb,bool useInPrintf=FALSE) const;
    const char *html(DocSymbol::SymType symb,bool useInPrintf=FALSE) const;
    const char *xml(DocSymbol::SymType symb) const;
    const char *docbook(DocSymbol::SymType symb) const;
    const char *latex(DocSymbol::SymType symb) const;
    const char *man(DocSymbol::SymType symb) const;
    const char *rtf(DocSymbol::SymType symb) const;
    const DocSymbol::PerlSymb *perl(DocSymbol::SymType symb) const;
    void  writeXMLSchema(FTextStream &t);
  private:
    void  validate();
    HtmlEntityMapper();
   ~HtmlEntityMapper();
    static HtmlEntityMapper *s_instance;
    QDict<int> *m_name2sym;
};

#endif
