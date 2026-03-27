/******************************************************************************
 *
 * Copyright (C) 1997-2018 by Dimitri van Heesch.
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

#include <map>
#include <string>

#include "construct.h"

class TextStream;

/** @brief Singleton helper class to map emoji entities to other formats */
class EmojiEntityMapper
{
  public:
    static EmojiEntityMapper &instance();
    const char *name(int index) const;
    const char *unicode(int index) const;
    void writeEmojiFile(TextStream &t);
    int symbol2index(const std::string &symName) const;

  private:
    EmojiEntityMapper();
   ~EmojiEntityMapper();
    NON_COPYABLE(EmojiEntityMapper)
    static EmojiEntityMapper *s_instance;
    std::map<std::string,int> m_name2symGh;
};

#endif
