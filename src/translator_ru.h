/******************************************************************************
 *
 * 
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
    QCString idLanguage()
    { return "russian"; }
    QCString latexBabelPackage()
    { return "russian"; }
    QCString idLanguageCharset()
    {
      return "koi8-r";
    }
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
	// Need additional translation
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
	return ", включая все унаследованные члены.";
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
	// Need additional translation
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
	// Need additional translation
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
	// Need additional translation
	return "Этот список (inheritance) тупо отсортирован, почти по алфафиту";
    }
    QCString trFileListDescription(bool extractAll)
    {
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="файлов с кратким описанием:";
	return result;
    }
    QCString trCompoundListDescription()
    {
	return "Краткое описание классов, структур, объединений и "
	       "интерфейсов:";
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
	// Need additional translation
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="членов класса которые связаны с ";
	if(extractAll)
	    result+="the class documentation for each member:";
	else
	    result+="the classes the belong to:";
	return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
	// Need additional translation
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="членов файла, которые (links) указывают на";
	if(extractAll)
	    result+="the class documentation for each member:";
	else
	    result+="the classes the belong to:";
	return result;
    }
    QCString trHeaderFilesDescription()
    {
	return "Список заголовочных файлов, которые представляют API:";
    }
    QCString trExamplesDescription()
    {
	return "Список всех примеров:";
    }
    QCString trRelatedPagesDescription()
    {
	// Need additional translation
	return "Список всех страниц документации связанных с этой темой:";
    }
    QCString trModulesDescription()
    {
	return "Список всех модулей:";
    }
    QCString trNoDescriptionAvailable()
    {
	return "Нет описания";
    }
    QCString trDocumentation()
    {
	return "Документация";
    }
    QCString trModuleIndex()
    {
	return "Индекс модулей";
    }
    QCString trHierarchicalIndex()
    {
	return "Иерархический список";
    }
    QCString trCompoundIndex()
    {
	// Need additional translation
	return "Составной список";
    }
    QCString trFileIndex()
    {
	return "Индекс Файлов";
    }
    QCString trModuleDocumentation()
    {
	return "Документация по Модулям";
    }
    QCString trClassDocumentation()
    {
	return "Документация по Классам";
    }
    QCString trFileDocumentation()
    {
	return "Документация по Файлам";
    }
    QCString trExampleDocumentation()
    {
	return "Примеры к Документации";
    }
    QCString trPageDocumentation()
    {
	return "Документация по Страницам";
    }
    QCString trReferenceManual()
    {
	return "Справочное Описание";
    }
    QCString trDefines()
    {
	// Need Translation
	return "Определения (Defines)";
    }
    QCString trFunctionPrototypes()
    {
	return "Прототипы функций";
    }
    QCString trTypedefs()
    {
	return "Определения типов";
    }
    QCString trEnumerations()
    {
	// Need Translation
	return "Перечисления (Enumerations)";
    }
    QCString trFunctions()
    {
	return "Функции:";
    }
    QCString trVariables()
    {
	return "Переменные:";
    }
    QCString trEnumerationValues()
    {
	return "Значение перечислимого типа";
    }
    QCString trAuthor()
    {
	return "Автор";
    }
    QCString trDefineDocumentation()
    {	
	return "Документация по определениям (defines)";
    }
    QCString trFunctionPrototypeDocumentation()
    {
	return "Документация по прототипам функций";
    }
    QCString trTypedefDocumentation()
    {
	return "Документация по определениям типов"; 
    }
    QCString trEnumerationTypeDocumentation()
    {
	return "Документация по перечислимым типам";
    }
    QCString trEnumerationValueDocumentation()
    {
	return "Документация по значениям перечислимых типов";
    }
    QCString trFunctionDocumentation()
    {
	return "Документация по функциям";
    }
    QCString trVariableDocumentation()
    {
	return "Документация по переменным";
    }
    QCString trCompounds()
    {
	// need translation
	return "Compounds";
    }
    QCString trFiles()
    {
	return "Файлы";
    }
    QCString trGeneratedAt(const char* date,const char* projName)
    {
	QCString result=(QCString)"Создано "+date;
	if(projName) result+=(QCString)" для "+projName;
	result+=(QCString)" с использованием";
	return result;
    }
    QCString trWrittenBy()
    {
	return "Создано с использованием";
    }
    QCString trClassDiagram(const char* clName)
    {
	return (QCString)"Диаграмма наследования для "+clName;
    }
    QCString trForInternalUseOnly()
    {
	return "Только для внутреннего использования.";
    }
    QCString trReimplementedForInternalREasons()
    {
	// need translation
	return "Переписано по необходимости о которой вам знать не нужно;"
	       " програмный интерфейс не изменен.";
    }
    QCString trWarning()
    {
	return "Предупреждение";
    }
    QCString trBugsAndLimitations()
    {
	return "Ошибки и ограничения";
    }
    QCString trVersion()
    {
	return "Версия";
    }
    QCString trDate()
    {
	return "Дата";
    }
    QCString trAuthors()
    {
	return "Автор(ы)";
    }
    QCString trReturns()
    {
	return "Возвращает";
    }
    QCString trSeeAlso()
    {
	return "Смотри";
    }
    QCString trParameters()
    {
	return "Параметры";
    }
    QCString trExceptions()
    {
	return "Исключения";
    }
    QCString trGeneratedBy()
    {
	return "Создано с использованием";
    }
    QCString trNamespaceList()
    {
	// Need translation
	return "Список Пространств Имен";
    }
    QCString trNamespaceListDescription(bool extractAll)
    {
	// Need translation
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="пространств имен с кратким описанием:";
	return result;
    }
    QCString trFriends()
    {
	return "Друзья";
    }
    QCString trRelatedFunctionDocumentation()
    {
	// need translation
	return "Документация по друзьям класса и связанным с ними функциям";
    }
    QCString trCompoundReference(const char* clName,
                                 ClassDef::CompoundType compType,
                                 bool /*isTemplate*/)
    {
	QCString result=(QCString)clName+" ";
	switch(compType)
	{
	    case ClassDef::Class: result+=" Класс"; break;
	    case ClassDef::Struct: result+=" Структура"; break;
	    case ClassDef::Union: result+=" Объединение"; break;
	    case ClassDef::Interface: result+=" Интерфейс"; break;
	    case ClassDef::Exception: result+=" Исключение"; break;
	}
	result+=" Описание";
	return result;
    }
    QCString trFileReference(const char* fileName)
    {
	QCString result;
	result+=(QCString)"Описание Файла "+fileName;
	return result;
    }
    QCString trNamespaceReference(const char* namespaceName)
    {
	// need translation
	QCString result=(QCString)"Описание пространства имен "+namespaceName;
	return result;
    }
    QCString trPublicMembers()
    {
	return "Открытые Методы";
    }
    QCString trPublicSlots()
    {
	// need translation
	return "Открытые Слоты (Slots)";
    }
    QCString trSignals()
    {
	return "Сигналы";
    }
    QCString trStaticPublicMembers()
    {
	return "Статические Открытые Методы";
    }
    QCString trProtectedMembers()
    {
	return "Защищенные Методы";
    }
    QCString trProtectedSlots()
    {
	// need translation
	return "Защищенные Слоты (Slots)";
    }
    QCString trStaticProtectedSlots()
    {
	// need translation
	return "Статические Защищенные Слоты (Slots)";
    }
    QCString trPrivateMembers()
    {
	return "Закрытые Методы";
    }
    QCString trPrivateSlots()
    {
	// need translation
	return "Закрытые Слоты (Slots)";
    }
    QCString trStaticPrivateMembers()
    {
	return "Статические Закрытые Методы";
    }
    QCString trWriteList(int numEntries)
    {
	QCString result;
	int i;
	for(i=0;i<numEntries;i++)
	{
	    result+=generateMarker(i);
	    if(i!=numEntries-1)
	    {
		if(i<numEntries-2)
		    result+=", ";
		else
		    result+=", и ";
	    }
	}
	return result;
    }
    QCString trInheritsList(int numEntries)
    {
	// need translation
	return "Дети (Inherits) "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
    {
	// need translation
	return "Предки (Inherited By) "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
	return "Переопределяется в "+trWriteList(numEntries)+".";
    }
    QCString trNamespaceMembers()
    {
	// Need translation
	return "Члены Пространства Имен";
    }
    QCString trNamespaceMemberDescription(bool extractAll)
    {
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="членов пространства имен со ссылками на ";
	if(extractAll)
	    result+="документацию по каждому члену";
	else
	    result+="документацию по пространству имен к которому они принадлежат";
	return result;
    }
    QCString trNamespaceIndex()
    {
	return "Индекс Пространства Имен";
    }
    QCString trNamespaceDocumentation()
    {
	return "Документация Пространства Имен";
    }
    QCString trNamespaces()
    {
	return "Пространства Имен";
    }
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType, bool single)
    {
	QCString result=(QCString)"Документация для это";
	switch(compType)
	{
	    case ClassDef::Class: result+="го класса"; break;
	    case ClassDef::Struct: result+="й структуры"; break;
	    case ClassDef::Union: result+="го объединения"; break;
	    case ClassDef::Interface: result+="го интерфейса"; break;
	    case ClassDef::Exception: result+="го исключения"; break;
	}
	result+=" была создана из файл";
	if(single) 
	    result+="а:"; 
	else
	    result+="ов:";
	return result;
    }
    QCString trAlphabeticalList()
    {
	return "Алфавитный Список";
    }
    QCString trReturnValues()
    {
	return "Возвращаемые значения";
    }
    QCString trMainPage()
    {
	// need translation
	return "Начальная Страница (Main Page)";
    }
    QCString trPageAbbreviation()
    {
	return "стр.";
    }
    QCString trSources()
    {
	return "Исходные тексты";
    }
    QCString trDefinedAtLineInSourceFile()
    {
	return "Определено в строке @0 файла @1.";
    }
    QCString trDefinedInSourceFile()
    {
	return "Определено в файле @0.";
    }
    QCString trDeprecated()
    {
	return "Устарело";
    }
    QCString trCollaborationDiagram(const char* clName)
    {
	// need translation
	return (QCString)"Диаграмма связей для "+clName+":";
    }
    QCString trInclDepGraph(const char* fName)
    {
	// need translation
	return (QCString)"Граф зависимостей для "+fName+":";
    }
    QCString trConstructorDocumentation()
    {
	return "Документация по Конструктору и Деструктору";
    }
    QCString trGotoSourceCode()
    {
	// need translation
	return "Смотри исходные тексты этого файла.";
    }
    QCString trGotoDocumentation()
    {
	return "Смотри документацию по этому файлу.";
    }
    QCString trPrecondition()
    {
	// need translation
	return "Начальное условие (Precondition)";
    }
    QCString trPostcondition()
    {
	// need translation
	return "Конечное условие (Postcondition)";
    }
    QCString trInvariant()
    {
	// need translation
	return "Инвариант";
    }
    QCString trInitialValue()
    {
	return "Начальное значение:";
    }
    QCString trCode()
    {
	return "исходные тексты";
    }
    QCString trGraphicalHierarchy()
    {
	// need translation
	return "Графическая Иерархия Класса (ов?)";
    }
    QCString trGotoGraphicalHierarchy()
    {
	// need translation
	return "Смотри графическую иерархию класса";
    }
    QCString trGotoTextualHierarchy()
    {
	// need translation
	return "Смотри текстовую иерархию класса";
    }
    QCString trPageIndex()
    {
	return "Индекс Страниц";
    }
    QCString trNote()
    {
	return "Примечание";
    }
    QCString trPublicTypes()
    {
	return "Открытые Типы";
    }
    QCString trPublicAttribs()
    {
	return "Открытые Аттрибуты";
    }
    QCString trStaticPublicAttribs()
    {
	return "Статические Открытые Аттрибуты";
    }
    QCString trProtectedTypes()
    {
	return "Защищенные Типы";
    }
    QCString trProtectedAttribs()
    {
	return "Защищенные Аттрибуты";
    }
    QCString trStaticProtectedAttribs()
    {
	return "Статические Защищенные Аттрибуты";
    }
    QCString trPrivateTypes()
    {
	return "Закрытые Типы";
    }
    QCString trPrivateAttribs()
    {
	return "Закрытые Аттрибуты";
    }
    QCString trStaticPrivateAttribs()
    {
	return "Статические Закрытые Аттрибуты";
    }
    QCString trTodo()
    {
      return "Todo";
    }
    QCString trTodoList()
    {
      return "Список Оставшихся Дел";
    }
    QCString trReferencedBy()
    {
      return "Упоминается в";
    }
    QCString trRemarks()
    {
      return "Примечания";
    }
    QCString trAttention()
    {
      return "Внимание";
    }
    QCString trInclByDepGraph()
    {
      return "Этот граф показывает, какие файлы прямо или "
             "косвенно включают этот файл:";
    }
    QCString trSince()
    {
      return "После";
    }
    QCString trLegendTitle()
    {
      return "Толкование графа";
    }
    QCString trLegendDocs()
    {
      return 
        "Эта страница обьясняет, как понимать графы, которые генерирует "
        "doxygen.<p>\n"
        "Рассмотрим следующий пример:\n"
        "\\code\n"
        "/*! Невидимый класс из-за усечения */\n"
        "class Invisible { };\n\n"
        "/*! Усеченный класс, отношение наследования скрыто */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Класс, не документированный комментариями doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Класс, использующий открытое наследование */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Класс, использующий защищенное наследование */\n"
        "class ProtectedBase { };\n\n"
        "/*! Класс, использующий закрытое наследование */\n"
        "class PrivateBase { };\n\n"
        "/*! Класс, используемый классом Inherited */\n"
        "class Used { };\n\n"
        "/*! Класс, порожденный от других классов */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Если \\c MAX_DOT_GRAPH_HEIGHT тэг в конфигурационном файле "
        "установлен в 200, получится следующий граф:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Прямоугольники в этом графе имеют следующее значение:\n"
        "<ul>\n"
        "<li>Заполненный чернный прямоугольник представляет структуру или класс, "
	"для которого создан граф.\n"
        "<li>Прямоугольник с черной границей обозначает документированную структуру или класс.\n"
        "<li>Прямоугольник с серой границей обозначает документированную структуру или класс.\n"
        "<li>Прямоугольник с красной границей обозначает документированную структуру или класс, для которого\n"
        " не все отношения наследования/содержания показаны. Граф усечен, "
        "если он не поместился в указанных границах."
        "</ul>\n"
        "Стрелки имеют следующее значение:\n"
        "<ul>\n"
        "<li>Темносиняя стрелка используется для изображения отношения открытого наследования "
        "между двумя классами.\n"
        "<li>Темнозеленая стрелка используется при защищенном наследовании.\n"
        "<li>Темнозеленая стрелка используется при закрытом наследовании.\n"
        "<li>Фиолетовая стрелка используется, если класс содержится в или используется другим классом"
        ". Стрелка маркированна переменной, "
        "через которую доступен указываемый класс или структура. \n"
        "</ul>\n";
    }
    QCString trLegend()
    {
      return "толкование";
    }
    QCString trTest()
    {
      return "Тест";
    }
    QCString trTestList()
    {
      return "Список Тестов";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Методы DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Свойства";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Документация Свойств";
    }


};

#endif
