/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include "message.h"
#include "language.h"
#include "translator.h"
#include "translator_en.h"
#if !defined(ENGLISH_ONLY)
#include "translator_adapter.h"
#include "translator_nl.h"
#include "translator_se.h"
#include "translator_cz.h"
#include "translator_fr.h"
#include "translator_it.h"
#include "translator_de.h"
#include "translator_jp.h"
#include "translator_es.h"
#include "translator_fi.h"
#include "translator_ru.h"
#include "translator_hr.h"
#include "translator_pl.h"
#include "translator_pt.h"
#include "translator_hu.h"
#include "translator_kr.h"
#include "translator_ro.h"
#include "translator_si.h"
#include "translator_cn.h"
#include "translator_no.h"
#include "translator_br.h"
#include "translator_dk.h"
#endif

#define L_EQUAL(a) !stricmp(langName,a)

Translator *theTranslator=0;

bool setTranslator(const char *langName)
{
  if (L_EQUAL("english"))
  {
    theTranslator=new TranslatorEnglish;
  }
#if !defined(ENGLISH_ONLY)
  else if (L_EQUAL("dutch"))
  {
    theTranslator=new TranslatorDutch;
  }
  else if (L_EQUAL("swedish"))
  {
    theTranslator=new TranslatorSwedish;
  }
  else if (L_EQUAL("czech"))
  {
    theTranslator=new TranslatorCzech;
  }
  else if (L_EQUAL("french"))
  {
    theTranslator=new TranslatorFrench;
  }
  else if (L_EQUAL("italian"))
  {
    theTranslator=new TranslatorItalian;
  }
  else if (L_EQUAL("german"))
  {
    theTranslator=new TranslatorGerman;
  }
  else if (L_EQUAL("japanese"))
  {
    theTranslator=new TranslatorJapanese;
  }
  else if (L_EQUAL("spanish"))
  {
    theTranslator=new TranslatorSpanish;
  }
  else if (L_EQUAL("finnish"))
  {
    theTranslator=new TranslatorFinnish;
  }
  else if (L_EQUAL("russian"))
  {
    theTranslator=new TranslatorRussian;
  }
  else if (L_EQUAL("croatian"))
  {
    theTranslator=new TranslatorCroatian;
  }
  else if (L_EQUAL("polish"))
  {
    theTranslator=new TranslatorPolish;
  }
  else if (L_EQUAL("portuguese"))
  {
    theTranslator=new TranslatorPortuguese;
  }
  else if (L_EQUAL("hungarian"))
  {
    theTranslator=new TranslatorHungarian;
  }
  else if (L_EQUAL("korean"))
  {
    theTranslator=new TranslatorKorean;
  }
  else if (L_EQUAL("romanian"))
  {
    theTranslator=new TranslatorRomanian;
  }
  else if (L_EQUAL("slovene"))
  {
    theTranslator=new TranslatorSlovene;
  }
  else if (L_EQUAL("chinese"))
  {
    theTranslator=new TranslatorChinese;
  }
  else if (L_EQUAL("norwegian"))
  {
    theTranslator=new TranslatorNorwegian;
  }
  else if (L_EQUAL("brazilian"))
  {
    theTranslator=new TranslatorBrazilian;
  }
  else if (L_EQUAL("danish"))
  {
    theTranslator=new TranslatorDanish;
  }
#endif
  else // use the default language (i.e. english)
  {
    theTranslator=new TranslatorEnglish;
    return FALSE;
  }
  
  QCString msg = theTranslator->updateNeededMessage();
  if (!msg.isEmpty()) warn_cont(msg);
  return TRUE;
}
