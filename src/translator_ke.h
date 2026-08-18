/******************************************************************************
 *
 *
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

//
// Update:
//
// 2004.12.22 (SooYoung Jung: jung5000@gmail.com)
// - LaTex and RTF were not generated correctly.
//   Corrected trRTFansicp and trRTFCharSet.
//   It was wrong.
//
//

#ifndef TRANSLATOR_KE_H
#define TRANSLATOR_KE_H

class TranslatorKoreanEn : public TranslatorEnglish
{
  public:
    DString idLanguage() override
    { return "korean-en"; }
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage{kotex}\n";
    }
    DString trRTFansicp() override
    {
      return "949";
    }
    DString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "129";
    }

    DString trISOLang() override
    {
      return "ko";
    }
    DString getLanguageString() override
    {
      return "0x412 Korean";
    }
};

#endif
