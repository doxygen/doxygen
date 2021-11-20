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

#include "message.h"
#include "config.h"
#include "util.h"
#include "language.h"
#include "lang_cfg.h"
#include "vhdldocgen.h"
#include "translator.h"
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
#ifdef LANG_LV
#include "translator_lv.h"
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

Translator *theTranslator=0;

void setTranslator(OUTPUT_LANGUAGE_t langName)
{
  switch (langName)
  {
    case OUTPUT_LANGUAGE_t::English:             theTranslator = new TranslatorEnglish; break;
#if !defined(ENGLISH_ONLY)
#ifdef LANG_ZA
    case OUTPUT_LANGUAGE_t::Afrikaans:           theTranslator = new TranslatorAfrikaans; break;
#endif
#ifdef LANG_AR
    case OUTPUT_LANGUAGE_t::Arabic:              theTranslator = new TranslatorArabic; break;
#endif
#ifdef LANG_AM
    case OUTPUT_LANGUAGE_t::Armenian:            theTranslator = new TranslatorArmenian; break;
#endif
#ifdef LANG_BR
    case OUTPUT_LANGUAGE_t::Brazilian:           theTranslator = new TranslatorBrazilian; break;
#endif
#ifdef LANG_CA
    case OUTPUT_LANGUAGE_t::Catalan:             theTranslator = new TranslatorCatalan; break;
#endif
#ifdef LANG_CN
    case OUTPUT_LANGUAGE_t::Chinese:             theTranslator = new TranslatorChinese; break;
#endif
#ifdef LANG_TW
    case OUTPUT_LANGUAGE_t::Chinese_Traditional: theTranslator = new TranslatorChinesetraditional; break;
#endif
#ifdef LANG_HR
    case OUTPUT_LANGUAGE_t::Croatian:            theTranslator = new TranslatorCroatian; break;
#endif
#ifdef LANG_CZ
    case OUTPUT_LANGUAGE_t::Czech:               theTranslator = new TranslatorCzech; break;
#endif
#ifdef LANG_DK
    case OUTPUT_LANGUAGE_t::Danish:              theTranslator = new TranslatorDanish; break;
#endif
#ifdef LANG_NL
    case OUTPUT_LANGUAGE_t::Dutch:               theTranslator = new TranslatorDutch; break;
#endif
#ifdef LANG_EO
    case OUTPUT_LANGUAGE_t::Esperanto:           theTranslator = new TranslatorEsperanto; break;
#endif
#ifdef LANG_FA
    case OUTPUT_LANGUAGE_t::Farsi:               theTranslator = new TranslatorPersian; break;
#endif
#ifdef LANG_FI
    case OUTPUT_LANGUAGE_t::Finnish:             theTranslator = new TranslatorFinnish; break;
#endif
#ifdef LANG_FR
    case OUTPUT_LANGUAGE_t::French:              theTranslator = new TranslatorFrench; break;
#endif
#ifdef LANG_DE
    case OUTPUT_LANGUAGE_t::German:              theTranslator = new TranslatorGerman; break;
#endif
#ifdef LANG_GR
    case OUTPUT_LANGUAGE_t::Greek:               theTranslator = new TranslatorGreek; break;
#endif
#ifdef LANG_HU
    case OUTPUT_LANGUAGE_t::Hungarian:           theTranslator = new TranslatorHungarian; break;
#endif
#ifdef LANG_ID
    case OUTPUT_LANGUAGE_t::Indonesian:          theTranslator = new TranslatorIndonesian; break;
#endif
#ifdef LANG_IT
    case OUTPUT_LANGUAGE_t::Italian:             theTranslator = new TranslatorItalian; break;
#endif
#ifdef LANG_JP
    case OUTPUT_LANGUAGE_t::Japanese:            theTranslator = new TranslatorJapanese; break;
#endif
#ifdef LANG_JE
    case OUTPUT_LANGUAGE_t::Japanese_en:         theTranslator = new TranslatorJapaneseEn; break;
#endif
#ifdef LANG_KR
    case OUTPUT_LANGUAGE_t::Korean:              theTranslator = new TranslatorKorean; break;
#endif
#ifdef LANG_KE
    case OUTPUT_LANGUAGE_t::Korean_en:           theTranslator = new TranslatorKoreanEn; break;
#endif
#ifdef LANG_LV
    case OUTPUT_LANGUAGE_t::Latvian:             theTranslator = new TranslatorLatvian; break;
#endif
#ifdef LANG_LT
    case OUTPUT_LANGUAGE_t::Lithuanian:          theTranslator = new TranslatorLithuanian; break;
#endif
#ifdef LANG_MK
    case OUTPUT_LANGUAGE_t::Macedonian:          theTranslator = new TranslatorMacedonian; break;
#endif
#ifdef LANG_NO
    case OUTPUT_LANGUAGE_t::Norwegian:           theTranslator = new TranslatorNorwegian; break;
#endif
#ifdef LANG_FA
    case OUTPUT_LANGUAGE_t::Persian:             theTranslator = new TranslatorPersian; break;
#endif
#ifdef LANG_PL
    case OUTPUT_LANGUAGE_t::Polish:              theTranslator = new TranslatorPolish; break;
#endif
#ifdef LANG_PT
    case OUTPUT_LANGUAGE_t::Portuguese:          theTranslator = new TranslatorPortuguese; break;
#endif
#ifdef LANG_RO
    case OUTPUT_LANGUAGE_t::Romanian:            theTranslator = new TranslatorRomanian; break;
#endif
#ifdef LANG_RU
    case OUTPUT_LANGUAGE_t::Russian:             theTranslator = new TranslatorRussian; break;
#endif
#ifdef LANG_SR
    case OUTPUT_LANGUAGE_t::Serbian:             theTranslator = new TranslatorSerbian; break;
#endif
#ifdef LANG_SC
    case OUTPUT_LANGUAGE_t::Serbian_Cyrillic:    theTranslator = new TranslatorSerbianCyrillic; break;
#endif
#ifdef LANG_SK
    case OUTPUT_LANGUAGE_t::Slovak:              theTranslator = new TranslatorSlovak; break;
#endif
#ifdef LANG_SI
    case OUTPUT_LANGUAGE_t::Slovene:             theTranslator = new TranslatorSlovene; break;
#endif
#ifdef LANG_ES
    case OUTPUT_LANGUAGE_t::Spanish:             theTranslator = new TranslatorSpanish; break;
#endif
#ifdef LANG_SV
    case OUTPUT_LANGUAGE_t::Swedish:             theTranslator = new TranslatorSwedish; break;
#endif
#ifdef LANG_TR
    case OUTPUT_LANGUAGE_t::Turkish:             theTranslator = new TranslatorTurkish; break;
#endif
#ifdef LANG_UA
    case OUTPUT_LANGUAGE_t::Ukrainian:           theTranslator = new TranslatorUkrainian; break;
#endif
#ifdef LANG_VI
    case OUTPUT_LANGUAGE_t::Vietnamese:          theTranslator = new TranslatorVietnamese; break;
#endif
#endif
  }
}
