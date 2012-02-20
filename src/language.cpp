/******************************************************************************
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

#include "message.h"
#include "language.h"
#include "lang_cfg.h"
#include "translator.h"
#include "translatordecoder.h"
#include "translator_en.h"
#if !defined(ENGLISH_ONLY)
#include "translator_adapter.h"
#ifdef LANG_NL
#include "translator_nl.h"
#endif
#ifdef LANG_AM
#include "translator_am.h"
#endif
#ifdef LANG_SV
#include "translator_sv.h"
#endif
#ifdef LANG_CZ
#include "translator_cz.h"
#endif
#ifdef LANG_FR
#include "translator_fr.h"
#endif
#ifdef LANG_ID
#include "translator_id.h"
#endif
#ifdef LANG_IT
#include "translator_it.h"
#endif
#ifdef LANG_DE
#include "translator_de.h"
#endif
#ifdef LANG_JP
#include "translator_jp.h"
#endif
#ifdef LANG_JE
#include "translator_je.h"
#endif
#ifdef LANG_ES
#include "translator_es.h"
#endif
#ifdef LANG_EO
#include "translator_eo.h"
#endif
#ifdef LANG_FI
#include "translator_fi.h"
#endif
#ifdef LANG_RU
#include "translator_ru.h"
#endif
#ifdef LANG_HR
#include "translator_hr.h"
#endif
#ifdef LANG_PL
#include "translator_pl.h"
#endif
#ifdef LANG_PT
#include "translator_pt.h"
#endif
#ifdef LANG_HU
#include "translator_hu.h"
#endif
#ifdef LANG_KE
#include "translator_ke.h"
#endif
#ifdef LANG_KR
#include "translator_kr.h"
#endif
#ifdef LANG_RO
#include "translator_ro.h"
#endif
#ifdef LANG_SI
#include "translator_si.h"
#endif
#ifdef LANG_CN
#include "translator_cn.h"
#endif
#ifdef LANG_TW
#include "translator_tw.h"
#endif
#ifdef LANG_NO
#include "translator_no.h"
#endif
#ifdef LANG_BR
#include "translator_br.h"
#endif
#ifdef LANG_DK
#include "translator_dk.h"
#endif
#ifdef LANG_SK
#include "translator_sk.h"
#endif
#ifdef LANG_UA
#include "translator_ua.h"
#endif
#ifdef LANG_GR
#include "translator_gr.h"
#endif
#ifdef LANG_SR
#include "translator_sr.h"
#endif
#ifdef LANG_CA
#include "translator_ca.h"
#endif
//#ifdef LANG_JS
//#include "translator_js.h"
//#endif
#ifdef LANG_LT
#include "translator_lt.h"
#endif
#ifdef LANG_ZA
#include "translator_za.h"
#endif
#ifdef LANG_AR
#include "translator_ar.h"
#endif
#ifdef LANG_FA
#include "translator_fa.h"
#endif
#ifdef LANG_MK
#include "translator_mk.h"
#endif
#ifdef LANG_SC
#include "translator_sc.h"
#endif
#ifdef LANG_VI
#include "translator_vi.h"
#endif
#ifdef LANG_TR
#include "translator_tr.h"
#endif
#endif // !ENGLISH_ONLY

#define L_EQUAL(a) !stricmp(langName,a)

Translator *theTranslator=0;

static const char obsoleteMsg[] =
        "---------\n"
        "ERROR: The selected language is no longer supported!\n"
        "If you want doxygen to produce output in this language \n"
        "you are kindly requested to help bringing the documentation \n"
        "up to date. Please read the development section of the manual \n"
        "for more information or contact Petr Prikryl (Prikryl@skil.cz).\n"
        "Thanks in advance!\n"
        "---------\n";

bool setTranslator(const char *langName)
{
  if (L_EQUAL("english"))
  {
    theTranslator=new TranslatorEnglish;
  }
#if !defined(ENGLISH_ONLY)
#ifdef LANG_NL
  else if (L_EQUAL("dutch"))
  {
    theTranslator=new TranslatorDutch;
  }
#endif
#ifdef LANG_AM
  else if (L_EQUAL("armenian"))
  {
    theTranslator=new TranslatorArmenian;
  }
#endif
#ifdef LANG_SV
  else if (L_EQUAL("swedish"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorSwedish);
  }
#endif
#ifdef LANG_CZ
  else if (L_EQUAL("czech"))
  {
    theTranslator=new TranslatorCzech;
  }
#endif
#ifdef LANG_FR  
  else if (L_EQUAL("french"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorFrench);
  }
#endif
#ifdef LANG_ID  
  else if (L_EQUAL("indonesian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorIndonesian);
  }
#endif
#ifdef LANG_IT  
  else if (L_EQUAL("italian"))
  {
    theTranslator=new TranslatorItalian;
  }
#endif
#ifdef LANG_DE
  else if (L_EQUAL("german"))
  {
    theTranslator=new TranslatorGerman;
  }
#endif
#ifdef LANG_JP
  else if (L_EQUAL("japanese"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorJapanese);
  }
#endif
#ifdef LANG_JE
  else if (L_EQUAL("japanese-en"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorJapaneseEn);
  }
#endif
#ifdef LANG_ES
  else if (L_EQUAL("spanish"))
  {
    theTranslator=new TranslatorSpanish;
  }
#endif
#ifdef LANG_FI
  else if (L_EQUAL("finnish"))
  {
    theTranslator=new TranslatorFinnish;
  }
#endif
#ifdef LANG_RU
  else if (L_EQUAL("russian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorRussian);
  }
#endif
#ifdef LANG_HR
  else if (L_EQUAL("croatian"))
  {
    theTranslator=new TranslatorCroatian;
  }
#endif
#ifdef LANG_PL
  else if (L_EQUAL("polish"))
  {
    theTranslator=new TranslatorPolish;
  }
#endif
#ifdef LANG_PT
  else if (L_EQUAL("portuguese"))
  {
    theTranslator=new TranslatorPortuguese;
  }
#endif
#ifdef LANG_HU
  else if (L_EQUAL("hungarian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorHungarian);
  }
#endif
#ifdef LANG_KR
  else if (L_EQUAL("korean"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorKorean);
  }
#endif
#ifdef LANG_KE
  else if (L_EQUAL("korean-en"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorKoreanEn);
  }
#endif
#ifdef LANG_RO
  else if (L_EQUAL("romanian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorRomanian);
  }
#endif
#ifdef LANG_SI
  else if (L_EQUAL("slovene"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorSlovene);
  }
#endif
#ifdef LANG_CN
  else if (L_EQUAL("chinese"))
  {
    theTranslator=new TranslatorChinese;
  }
#endif
#ifdef LANG_TW
  else if (L_EQUAL("chinese-traditional"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorChinesetraditional);
  }
#endif
#ifdef LANG_NO
  else if (L_EQUAL("norwegian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorNorwegian);
  }
#endif
#ifdef LANG_BR
  else if (L_EQUAL("brazilian"))
  {
    theTranslator=new TranslatorBrazilian;
  }
#endif
#ifdef LANG_DK
  else if (L_EQUAL("danish"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorDanish);
  }
#endif
#ifdef LANG_SK
  else if (L_EQUAL("slovak"))
  {
    theTranslator=new TranslatorSlovak;
  }
#endif
#ifdef LANG_UA
  else if (L_EQUAL("ukrainian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorUkrainian);
  }
#endif
#ifdef LANG_GR
  else if (L_EQUAL("greek"))
  {
    theTranslator=new TranslatorGreek;
  }
#endif
#ifdef LANG_SR
  else if (L_EQUAL("serbian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorSerbian);
  }
#endif
#ifdef LANG_SC
  else if (L_EQUAL("serbian-cyrilic"))
  {
    theTranslator=new TranslatorSerbian;
  }
#endif
#ifdef LANG_CA
  else if (L_EQUAL("catalan"))
  {
    theTranslator=new TranslatorCatalan;
  }
#endif
#ifdef LANG_LT
  else if (L_EQUAL("lithuanian"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorLithuanian);
  }
#endif
#ifdef LANG_ZA
  else if (L_EQUAL("afrikaans"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorAfrikaans);
  }
#endif
#ifdef LANG_AR
  else if (L_EQUAL("arabic"))
  {
    theTranslator=new TranslatorDecoder(new TranslatorArabic);
  }
#endif
#ifdef LANG_FA
  else if (L_EQUAL("persian") || L_EQUAL("farsi"))
  {
    theTranslator=new TranslatorPersian;
  }
#endif
#ifdef LANG_MK
  else if (L_EQUAL("macedonian"))
  {
    theTranslator=new TranslatorMacedonian;
  }
#endif
#ifdef LANG_VI
  else if (L_EQUAL("vietnamese"))
  {
    theTranslator=new TranslatorVietnamese;
  }
#endif
#ifdef LANG_TR
  else if (L_EQUAL("turkish"))
  {
    theTranslator=new TranslatorTurkish;
  }
#endif
#ifdef LANG_EO
  else if (L_EQUAL("esperanto"))
  {
    theTranslator=new TranslatorEsperanto;
  }
#endif
#endif // ENGLISH_ONLY
  else // use the default language (i.e. english)
  {
    theTranslator=new TranslatorEnglish;
    return FALSE;
  }
  
  QCString msg = theTranslator->updateNeededMessage();
  if (!msg.isEmpty()) err(msg);
  return TRUE;
}
