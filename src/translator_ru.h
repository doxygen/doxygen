/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef TRANSLATOR_RU_H
#define TRANSLATOR_RU_H

#include "translator.h"

class TranslatorRussian : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "russian"; }
    QCString trRelatedFunctions()
    {
	return "Связанные Функции";
    }
    QCString trRelatedSubscript()
    {
	return "(Обратите внимание, что эти функции не члены класса)";
    }
    QCString trDetailedDescription()
    {
	return "Детальное Описание";
    }
    QCString trMemberTypedefDocumentation()
    {
	return "Описание Членов typedef";
    }
    QCString trMemberEnumerationDocumentation()
    {
	return "Описание Членов Перечислимого Типа";
    }
    QCString trMemberFunctionDocumentation()
    {
	return "Описание Функции Члена";
    }
    QCString trMemberDataDocumentation()
    {
	return "Описание Данных Члена";
    }
    QCString trMore()
    {
	return "Еще...";
    }
    QCString trListOfAllMembers()
    {
	return "Cписок всех членов класса";
    }
    QCString trMemberList()
    {
	return "Список Членов";
    }
    QCString trThisIsTheListOfAllMembers()
    {
	return "Это полный список членов ";
    }
    QCString trIncludeIncludingInheritedMembers()
    {
	return ", включая все унаследованные методы.";
    }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Автоматически создано системой Doxygen";
      if (s) result+=(QCString)" для "+s;
      result+=" из исходного текста."; 
      return result;
    }
    QCString trEnumName()
    {
	return "имя перечисления";
    }
    QCString trEnumValue()
    {
	return "значение перечисления";
    }
    QCString trDefinedIn()
    {
	return "определено в";
    }
    QCString trVerbatimText(const char* f)
    {
	return (QCString)"Это прямая вставка текста из "+f+"включаемого файла";
    }
    QCString trModules()
    {
	return "Модули";
    }
    QCString trClassHierarchy()
    {
	return "Иерархия классов";
    }
    QCString trCompoundList()
    {
	return "Составной список (compound list)";
    }
    QCString trFileList()
    {
	return "Список Файлов";
    }
    QCString trHeaderFiles()
    {
	return "Список Заголовочных Файлов";
    }
    QCString trCompoundMembers()
    {
	return "Составные члены (compound members)";
    }
    QCString trFileMembers()
    {
	return "Члены Файла";
    }
    QCString trRelatedPages()
    {
	return "Связанные Страницы";
    }
    QCString trExamples()
    {
	return "Примеры";
    }
    QCString trSearch()
    {
	return "Поиск";
    }
    QCString trClassHierarchyDescription()
    {
	return "Этот список (inheritance) тупо отсортирован, почти по алфафиту";
    }
    
};

#endif
