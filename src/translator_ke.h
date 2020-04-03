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
//	Update:
//
// 2004.12.22 (SooYoung Jung: jung5000@gmail.com)
//	- LaTex and RTF were not generated correctly.
// 	  Corrected trRTFansicp and trRTFCharSet.
// 	  It was wrong.
//
//

#ifndef TRANSLATOR_KE_H
#define TRANSLATOR_KE_H

class TranslatorKoreanEn : public TranslatorEnglish
{
  public:
    virtual QCString idLanguage()
    { return "korean-en"; }
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage{kotex}\n";
    }
    virtual QCString trRTFansicp()
    {
      return "949";
    }
    virtual QCString latexCommandName()
    {
      QCString latex_command = Config_getString(LATEX_CMD_NAME);
      if (latex_command.isEmpty()) latex_command = "latex";
      if (Config_getBool(USE_PDFLATEX))
      {
        if (latex_command == "latex") latex_command = "xelatex";
      }
      if (Config_getBool(LATEX_BATCHMODE))
      {
        latex_command +=  " -interaction=batchmode -halt-on-error";
      }
      return latex_command;
    }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "129";
    }
};

#endif
