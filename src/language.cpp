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

#include "types.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "language.h"
#include "translator.h"
#include "translator_en.h"
#include "translator_adapter.h"
#include "translator_nl.h"
#include "translator_am.h"
#include "translator_sv.h"
#include "translator_cz.h"
#include "translator_fr.h"
#include "translator_id.h"
#include "translator_it.h"
#include "translator_de.h"
#include "translator_jp.h"
#include "translator_je.h"
#include "translator_es.h"
#include "translator_eo.h"
#include "translator_fi.h"
#include "translator_ru.h"
#include "translator_hr.h"
#include "translator_pl.h"
#include "translator_pt.h"
#include "translator_hi.h"
#include "translator_hu.h"
#include "translator_ke.h"
#include "translator_kr.h"
#include "translator_ro.h"
#include "translator_si.h"
#include "translator_cn.h"
#include "translator_tw.h"
#include "translator_no.h"
#include "translator_br.h"
#include "translator_dk.h"
#include "translator_sk.h"
#include "translator_ua.h"
#include "translator_gr.h"
#include "translator_sr.h"
#include "translator_ca.h"
#include "translator_lt.h"
#include "translator_lv.h"
#include "translator_za.h"
#include "translator_ar.h"
#include "translator_fa.h"
#include "translator_mk.h"
#include "translator_sc.h"
#include "translator_vi.h"
#include "translator_tr.h"
#include "translator_bg.h"
#include "configimpl.h"

Translator *theTranslator=nullptr;

void setTranslator(OUTPUT_LANGUAGE_t langName)
{
  switch (langName)
  {
    case OUTPUT_LANGUAGE_t::English:             theTranslator = new TranslatorEnglish; break;
    case OUTPUT_LANGUAGE_t::Afrikaans:           theTranslator = new TranslatorAfrikaans; break;
    case OUTPUT_LANGUAGE_t::Arabic:              theTranslator = new TranslatorArabic; break;
    case OUTPUT_LANGUAGE_t::Armenian:            theTranslator = new TranslatorArmenian; break;
    case OUTPUT_LANGUAGE_t::Brazilian:           theTranslator = new TranslatorBrazilian; break;
    case OUTPUT_LANGUAGE_t::Bulgarian:           theTranslator = new TranslatorBulgarian; break;
    case OUTPUT_LANGUAGE_t::Catalan:             theTranslator = new TranslatorCatalan; break;
    case OUTPUT_LANGUAGE_t::Chinese:             theTranslator = new TranslatorChinese; break;
    case OUTPUT_LANGUAGE_t::Chinese_Traditional: theTranslator = new TranslatorChinesetraditional; break;
    case OUTPUT_LANGUAGE_t::Croatian:            theTranslator = new TranslatorCroatian; break;
    case OUTPUT_LANGUAGE_t::Czech:               theTranslator = new TranslatorCzech; break;
    case OUTPUT_LANGUAGE_t::Danish:              theTranslator = new TranslatorDanish; break;
    case OUTPUT_LANGUAGE_t::Dutch:               theTranslator = new TranslatorDutch; break;
    case OUTPUT_LANGUAGE_t::Esperanto:           theTranslator = new TranslatorEsperanto; break;
    case OUTPUT_LANGUAGE_t::Farsi:               theTranslator = new TranslatorPersian; break;
    case OUTPUT_LANGUAGE_t::Finnish:             theTranslator = new TranslatorFinnish; break;
    case OUTPUT_LANGUAGE_t::French:              theTranslator = new TranslatorFrench; break;
    case OUTPUT_LANGUAGE_t::German:              theTranslator = new TranslatorGerman; break;
    case OUTPUT_LANGUAGE_t::Greek:               theTranslator = new TranslatorGreek; break;
    case OUTPUT_LANGUAGE_t::Hindi:               theTranslator = new TranslatorHindi; break;
    case OUTPUT_LANGUAGE_t::Hungarian:           theTranslator = new TranslatorHungarian; break;
    case OUTPUT_LANGUAGE_t::Indonesian:          theTranslator = new TranslatorIndonesian; break;
    case OUTPUT_LANGUAGE_t::Italian:             theTranslator = new TranslatorItalian; break;
    case OUTPUT_LANGUAGE_t::Japanese:            theTranslator = new TranslatorJapanese; break;
    case OUTPUT_LANGUAGE_t::Japanese_en:         theTranslator = new TranslatorJapaneseEn; break;
    case OUTPUT_LANGUAGE_t::Korean:              theTranslator = new TranslatorKorean; break;
    case OUTPUT_LANGUAGE_t::Korean_en:           theTranslator = new TranslatorKoreanEn; break;
    case OUTPUT_LANGUAGE_t::Latvian:             theTranslator = new TranslatorLatvian; break;
    case OUTPUT_LANGUAGE_t::Lithuanian:          theTranslator = new TranslatorLithuanian; break;
    case OUTPUT_LANGUAGE_t::Macedonian:          theTranslator = new TranslatorMacedonian; break;
    case OUTPUT_LANGUAGE_t::Norwegian:           theTranslator = new TranslatorNorwegian; break;
    case OUTPUT_LANGUAGE_t::Persian:             theTranslator = new TranslatorPersian; break;
    case OUTPUT_LANGUAGE_t::Polish:              theTranslator = new TranslatorPolish; break;
    case OUTPUT_LANGUAGE_t::Portuguese:          theTranslator = new TranslatorPortuguese; break;
    case OUTPUT_LANGUAGE_t::Romanian:            theTranslator = new TranslatorRomanian; break;
    case OUTPUT_LANGUAGE_t::Russian:             theTranslator = new TranslatorRussian; break;
    case OUTPUT_LANGUAGE_t::Serbian:             theTranslator = new TranslatorSerbian; break;
    case OUTPUT_LANGUAGE_t::Serbian_Cyrillic:    theTranslator = new TranslatorSerbianCyrillic; break;
    case OUTPUT_LANGUAGE_t::Slovak:              theTranslator = new TranslatorSlovak; break;
    case OUTPUT_LANGUAGE_t::Slovene:             theTranslator = new TranslatorSlovene; break;
    case OUTPUT_LANGUAGE_t::Spanish:             theTranslator = new TranslatorSpanish; break;
    case OUTPUT_LANGUAGE_t::Swedish:             theTranslator = new TranslatorSwedish; break;
    case OUTPUT_LANGUAGE_t::Turkish:             theTranslator = new TranslatorTurkish; break;
    case OUTPUT_LANGUAGE_t::Ukrainian:           theTranslator = new TranslatorUkrainian; break;
    case OUTPUT_LANGUAGE_t::Vietnamese:          theTranslator = new TranslatorVietnamese; break;
  }

  QCString msg = theTranslator->updateNeededMessage();
  if (!msg.isEmpty()) ConfigImpl::config_warn("{}", qPrint(msg));
}
