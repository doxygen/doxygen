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

/*
 *  Russian translation by Andrey V. Stolyarov <crocodil<AT>croco.net>
 *  released Feb 14, 2001
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
	return "Связанные функции";
    }
    QCString trRelatedSubscript()
    {
	return "(Обратите внимание, что эти функции - не члены класса)";
    }
    QCString trDetailedDescription()
    {
	return "Детальное описание";
    }
    QCString trMemberTypedefDocumentation()
    {
	// Need additional translation
	return "Описание typedef-членов";
    }
    QCString trMemberEnumerationDocumentation()
    {
	return "Описание членов перечислимого типа";
    }
    QCString trMemberFunctionDocumentation()
    {
	return "Описание функций-членов";
    }
    QCString trMemberDataDocumentation()
    {
      if (Config::optimizeForCFlag)
      {
        return "Описание полей";
      }
      else
      {
        return "Описание переменных-членов";
      }
    }
    QCString trMore()
    {
	return "Дальше...";
    }
    QCString trListOfAllMembers()
    {
	return "Cписок всех членов класса.";
    }
    QCString trMemberList()
    {
	return "Список членов";
    }
    QCString trThisIsTheListOfAllMembers()
    {
	return "Это полный список членов класса/структуры ";
    }
    QCString trIncludeIncludingInheritedMembers()
    {
	return ", включая все унаследованные члены.";
    }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Автоматически создано системой Doxygen";
      if (s) result+=(QCString)" для "+s;
      result+=" по файлам исходного текста."; 
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
	return (QCString)"Это прямая вставка текста из включаемого файла"+f;
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
      if (Config::optimizeForCFlag)
      {
        return "Структуры данных";
      }
      else
      {
        // doubtful translation; originally - "Compound List"
        return "Список составных объектов";
      }
    }
    QCString trFileList()
    {
	return "Список файлов";
    }
    QCString trHeaderFiles()
    {
	return "Заголовочные файлы";
    }
    QCString trCompoundMembers()
    {
      if (Config::optimizeForCFlag)
      {
        return "Поля структур";
      }
      else
      {
        return "Члены-данные";
      }
    }
    QCString trFileMembers()
    {
      if (Config::optimizeForCFlag)
      {
        return "Глобальные имена";
      }
      else
      {
         // doubtful translation 
         // (originally - "File members", but not good for Russian)
        return "Члены файла";
      }
    }
    QCString trRelatedPages()
    {
         // non-verbatim translation: originally "related pages"
	return "См. также: ";
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
	return "Это дерево наследования отсортировано приблизительно "
               "(но не совсем) по алфавиту";
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
      if (Config::optimizeForCFlag)
      {
        return "Структуры данных и их краткое описание: ";
      }
      else
      {
	return "Классы, структуры, объединения и интерфейсы " 
	       "и их краткое описание: ";
      }
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
	// Need additional translation
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="членов классов со ссылками на ";
	if(extractAll)
	    result+="документацию по классу для каждого члена:";
	else
	    result+="классы, к которым они принадлежат:";
	return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Список всех ";
      if (!extractAll) result+="документированных ";

      if (Config::optimizeForCFlag)
      {
        result+="функций, переменных, макроопределений, "
                "перечислений и определений типов";
      }
      else
      {
        result+="членов файлов ";
      }
      result+=" со ссылками на ";
      if (extractAll)
        result+="файлы, к которым они принадлежат:";
      else
        result+="документацию:";
      return result;
    }
    QCString trHeaderFilesDescription()
    {
	return "Список заголовочных файлов, составляющих API:";
    }
    QCString trExamplesDescription()
    {
	return "Список всех примеров:";
    }
    QCString trRelatedPagesDescription()
    {
	return "Список всех родственных страниц документации:";
    }
    QCString trModulesDescription()
    {
	return "Список всех модулей:";
    }
    QCString trNoDescriptionAvailable()
    {
	return "Описание отсутствует";
    }
    QCString trDocumentation()
    {
	return "Документация";
    }
    QCString trModuleIndex()
    {
	return "Указатель модулей";
    }
    QCString trHierarchicalIndex()
    {
	return "Иерархический список";
    }
    QCString trCompoundIndex()
    {
      if (Config::optimizeForCFlag)
      {
        return "Список структур данных";
      }
      else
      {
          // originally "compound index"
        return "Список классов и структур";
      }
    }
    QCString trFileIndex()
    {
	return "Список файлов";
    }
    QCString trModuleDocumentation()
    {
	return "Документация по модулям";
    }
    QCString trClassDocumentation()
    {
	return "Документация по классам";
    }
    QCString trFileDocumentation()
    {
	return "Документация по файлам";
    }
    QCString trExampleDocumentation()
    {
	return "Описания примеров";
    }
    QCString trPageDocumentation()
    {
	return "Документация по страницам";
    }
    QCString trReferenceManual()
    {
           // originally reference manual
	return "Краткий справочник";
    }
    QCString trDefines()
    {
	return "Определения";
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
	return "Перечисления";
    }
    QCString trFunctions()
    {
	return "Функции";
    }
    QCString trVariables()
    {
	return "Переменные";
    }
    QCString trEnumerationValues()
    {
	return "Значения перечислимого типа";
    }
    QCString trAuthor()
    {
	return "Автор";
    }
    QCString trDefineDocumentation()
    {	
	return "Документация по определениям";
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
      if (Config::optimizeForCFlag)
      {
        return "Структуры данных";
      }
      else
      { 
          // originally "compounds"
        return "Составные типы";
      }

    }
    QCString trFiles()
    {
	return "Файлы";
    }
    QCString trGeneratedAt(const char* date,const char* projName)
    {
	QCString result=(QCString)"Создано "+date;
	if(projName) result+=(QCString)" для "+projName;
	result+=(QCString)" с помощью ";
	return result;
    }
    QCString trWrittenBy()
    {
	return "написанного";
    }
    QCString trClassDiagram(const char* clName)
    {
	return (QCString)"Диаграмма наследования для "+clName+":";
    }
    QCString trForInternalUseOnly()
    {
	return "Только для внутреннего использования.";
    }
    QCString trReimplementedForInternalREasons()
    {
	return "Реализация изменена по внутренним причинам;"
	       " Интерфейсная часть не изменилась.";
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
	return "См. также";
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
	return "Создано при помощи";
    }
    QCString trNamespaceList()
    {
	return "Список пространств имен (namespaces)";
    }
    QCString trNamespaceListDescription(bool extractAll)
    {
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
	return "Документация по друзьям класса и связанным функциям";
    }
    QCString trCompoundReference(const char* clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    {
	QCString result=(QCString)"Описание ";
        if (isTemplate){ 
	  	switch(compType)
	  	{
	  	    case ClassDef::Struct: result+="шаблонной "; break;
	  	    case ClassDef::Class: 
	  	    case ClassDef::Union: 
	  	    case ClassDef::Interface: 
	  	    case ClassDef::Exception: 
					result+="шаблонного "; break;
	  	}
	}   
	switch(compType)
	{
	    case ClassDef::Class: result+="класса "; break;
	    case ClassDef::Struct: result+="структуры "; break;
	    case ClassDef::Union: result+="объединения "; break;
	    case ClassDef::Interface: result+="интерфейса "; break;
	    case ClassDef::Exception: result+="исключения "; break;
	}
	result+=clName;
	return result;
    }
    QCString trFileReference(const char* fileName)
    {
	QCString result;
	result+=(QCString)"Описание файла "+fileName;
	return result;
    }
    QCString trNamespaceReference(const char* namespaceName)
    {
	QCString result=(QCString)"Описание пространства имен "+namespaceName;
	return result;
    }
    QCString trPublicMembers()
    {
	return "Открытые члены";
    }
    QCString trPublicSlots()
    {
	return "Открытые слоты";
    }
    QCString trSignals()
    {
	return "Сигналы";
    }
    QCString trStaticPublicMembers()
    {
	return "Статические открытые члены";
    }
    QCString trProtectedMembers()
    {
	return "Защищенные члены";
    }
    QCString trProtectedSlots()
    {
	return "Защищенные слоты";
    }
    QCString trStaticProtectedSlots()
    {
	return "Статические защищенные слоты";
    }
    QCString trPrivateMembers()
    {
	return "Закрытые члены";
    }
    QCString trPrivateSlots()
    {
	return "Закрытые слоты";
    }
    QCString trStaticPrivateMembers()
    {
	return "Закрытые статические члены";
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
		    result+=" и ";
	    }
	}
	return result;
    }
    QCString trInheritsList(int numEntries)
    {
	return "Унаследован от "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
    {
	return "Наследуется "+trWriteList(numEntries)+".";
    }

    QCString trReimplementedFromList(int numEntries)
    {
        // Originally: "reimplemented from"
        // This translation assumes numEntries is always 1
        //    which is true as of 1.2.5 and it's hard for me to 
        //    imagine a situation when it could be wrong. 
      return "Переопределяет метод предка "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
	return "Переопределяется в "+trWriteList(numEntries)+".";
    }
    QCString trNamespaceMembers()
    {
	return "Члены пространств имен";
    }
    QCString trNamespaceMemberDescription(bool extractAll)
    {
	QCString result="Список всех ";
	if(!extractAll) result+="документированных ";
	result+="членов пространств имен со ссылками на ";
	if(extractAll)
	    result+="документацию по каждому члену:";
	else
	    result+="пространства имен, к которым они принадлежат:";
	return result;
    }
    QCString trNamespaceIndex()
    {
	return "Индекс пространств имен";
    }
    QCString trNamespaceDocumentation()
    {
	return "Документация пространств имен";
    }
    QCString trNamespaces()
    {
	return "Пространства имен";
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
	return "Алфавитный список";
    }
    QCString trReturnValues()
    {
	return "Возвращаемые значения";
    }
    QCString trMainPage()
    {
	return "Главная страница";
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
	return "Определяется в строке @0 файла @1.";
    }
    QCString trDefinedInSourceFile()
    {
	return "Определяется в файле @0.";
    }
    QCString trDeprecated()
    {
	return "Устарело";
    }
    QCString trCollaborationDiagram(const char* clName)
    {
           // originally "collaboration diagram"
	return (QCString)"Диаграмма использования для "+clName+":";
    }
    QCString trInclDepGraph(const char* fName)
    {
	return (QCString)"Граф зависимостей по включениям "
                         "файлов для "+fName+":";
    }
    QCString trConstructorDocumentation()
    {
	return "Документация по конструктору и деструктору";
    }
    QCString trGotoSourceCode()
    {
	return "Перейти к исходному тексту этого файла.";
    }
    QCString trGotoDocumentation()
    {
	return "Перейти к документации на этот файл.";
    }
    QCString trPrecondition()
    {
	return "Предусловие";
    }
    QCString trPostcondition()
    {
	return "Постусловие";
    }
    QCString trInvariant()
    {
	return "Инвариант";
    }
    QCString trInitialValue()
    {
	return "Начальное значение:";
    }
    QCString trCode()
    {
	return "исходный текст";
    }
    QCString trGraphicalHierarchy()
    {
	return "Графическая иерархия класса(ов)";
    }
    QCString trGotoGraphicalHierarchy()
    {
	return "Перейти к графической иерархии";
    }
    QCString trGotoTextualHierarchy()
    {
	return "Перейти к текстовой иерархии";
    }
    QCString trPageIndex()
    {
	return "Индекс страниц";
    }
    QCString trNote()
    {
	return "Примечание";
    }
    QCString trPublicTypes()
    {
	return "Открытые типы";
    }
    QCString trPublicAttribs()
    {
      if (Config::optimizeForCFlag)
      {
        return "Поля данных";
      }
      else
      {
	return "Открытые атрибуты";
      }
    }
    QCString trStaticPublicAttribs()
    {
	return "Открытые статические атрибуты";
    }
    QCString trProtectedTypes()
    {
	return "Защищенные типы";
    }
    QCString trProtectedAttribs()
    {
	return "Защищенные атрибуты";
    }
    QCString trStaticProtectedAttribs()
    {
	return "Статические защищенные  атрибуты";
    }
    QCString trPrivateTypes()
    {
	return "Закрытые типы";
    }
    QCString trPrivateAttribs()
    {
	return "Закрытые атрибуты";
    }
    QCString trStaticPrivateAttribs()
    {
	return "Закрытые статические атрибуты";
    }
    QCString trTodo()
    {
      return "Необходимо сделать:";
    }
    QCString trTodoList()
    {
      return "Список дел";
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
             "косвенно включают данный файл:";
    }
    QCString trSince()
    {
      return "Начиная с";
    }
    QCString trLegendTitle()
    {
      return "Условные обозначения";
    }
    QCString trLegendDocs()
    {
      return 
        //"Условные обозначения на графах, генерируемых программой "
        //"doxygen.<p>\n"
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
        "/*! Класс, используемый в классе Inherited */\n"
        "class Used { };\n\n"
        "/*! Суперкласс, унаследованный от нескольких классов */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Если тэг \\c MAX_DOT_GRAPH_HEIGHT в конфигурационном файле "
        "имеет значение 200, получим следующий граф:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Прямоугольники в этом графе имеют следующие значения:\n"
        "<ul>\n"
        "<li>Заполненный черный прямоугольник представляет структуру или класс, "
	"для которого создан данный граф.\n"
        "<li>Прямоугольник с черной границей обозначает документированную структуру или класс.\n"
        "<li>Прямоугольник с серой границей обозначает недокументированную структуру или класс.\n"
        "<li>Прямоугольник с красной границей обозначает документированную структуру или класс, для которого\n"
        " не все отношения наследования/содержания показаны. Граф усекается, "
        "если он не поместился в указанные границы."
        "</ul>\n"
        "Стрелки имеют следующие значения:\n"
        "<ul>\n"
        "<li>Темносиняя стрелка используется для изображения отношения открытого наследования "
        "между двумя классами.\n"
        "<li>Темнозеленая стрелка используется при защищенном наследовании.\n"
        "<li>Темнокрасная стрелка используется при закрытом наследовании.\n"
        "<li>Фиолетовая пунктирная стрелка используется, если класс содержится "
        "в другом классе или используется другим классом. "
        "Стрелка маркируется именем переменной, "
        "через которую доступен указываемый класс или структура. \n"
        "</ul>\n";
    }
    QCString trLegend()
    {
      return "обозначения";
    }
    QCString trTest()
    {
      return "Тест";
    }
    QCString trTestList()
    {
      return "Список тестов";
    }

    virtual QCString trDCOPMethods()
    {
      return "Методы DCOP";
    }


    virtual QCString trProperties()
    {
      return "Свойства";
    }
    virtual QCString trPropertyDocumentation()
    {
      return "Документация свойств";
    }


    virtual QCString trInterfaces()
    {
      return "Интерфейсы";
    }
    virtual QCString trClasses()
    {
      return "Классы";
    }
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Пакет "+name;
    }
    virtual QCString trPackageList()
    {
      return "Список пакетов";
    }
    virtual QCString trPackageListDescription()
    {
      return "Список пакетов с кратким описанием (если есть):";
    }
    virtual QCString trPackages()
    {
      return "Пакеты";
    }
    virtual QCString trPackageDocumentation()
    {
      return "Документация пакетов";
    }
    virtual QCString trDefineValue()
    {
      return "Значение:";
    }
};

#endif
