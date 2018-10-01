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
#ifndef EMOJIENTITY_H
#define EMOJIENTITY_H

#include <qdict.h>
#include <qfile.h>

class FTextStream;

/** @brief Singleton helper class to map emoji entities to other formats */
class EmojiEntityMapper
{
  public:
    static EmojiEntityMapper *instance();
    static void deleteInstance();
    int name2sym(const QCString &symName) const;
    const char *utf8(int symb) const;
    const char *html(int symb) const;
    const char *xml(int symb) const;
    const char *docbook(int symb) const;
    const char *latex(int symb) const;
    const char *man(int symb) const;
    const char *rtf(int symb) const;
    const char *perl(int symb) const;
    const char *code(int symb) const;
    const char *text(int symb) const;
    void writeEmojiFile(QFile &file);
  private:
    EmojiEntityMapper();
   ~EmojiEntityMapper();
    static EmojiEntityMapper *s_instance;
    QDict<int> *m_name2symGh;
    QDict<int> *m_name2symUni;
    QDict<int> *m_name2symStripUni;
};

#endif
