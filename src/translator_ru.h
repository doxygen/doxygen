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
 */

/*
 *
 * Nickolay Semyonov
 * Andrey V. Stolyarov released Feb 14, 2001
 * Alexandr V. Chelpanov <cav@cryptopro.ru> released Sep 25, 2004
 * Благодарности: Vitaly A. Repin <vitaly@radio.hop.stu.neva.ru>,
 *    Михаил Глушенков <bbman@mail.ru>
 *
 * 01 Jul 2026: updated with the requirements related methods introduced
 *              since release 1.16.0
 */

#ifndef TRANSLATOR_RU_H
#define TRANSLATOR_RU_H

class TranslatorRussian : public Translator
{
  public:
    /*! Used for identification of the language. */
    DString idLanguage() override
    { return "russian"; }

    /* Used to get the command(s) for the language support. */
    DString latexLanguageSupportCommand() override
    { return "\\usepackage[T2A]{fontenc}\n\\usepackage[russian]{babel}\n"; }

    DString trISOLang() override
    { return "ru"; }
    DString getLanguageString() override
    {
      return "0x419 Russian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Относящиеся к классу функции"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(не члены класса)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Подробное описание"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Подробности"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Определения типов"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Перечисления"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "Методы"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Поля";
      }
      else
      {
        return "Данные класса";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Подробнее..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    DString trListOfAllMembers() override
    {
      return "Полный список членов класса";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    DString trMemberList() override
    {
      return "Список членов класса";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    DString trThisIsTheListOfAllMembers() override
    { return "Полный список членов класса"; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    DString trIncludingInheritedMembers() override
    { return ", включая наследуемые из базового класса"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Автоматически создано Doxygen";
      if (!s.empty()) result+=DString(" для ")+s;
      result+=" из исходного текста.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "перечисление"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "элементы перечисления"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "определено в"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Группы"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Иерархия классов"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуры данных";
      }
      else
      {
        return "Классы";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Файлы"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Поля структур";
      }
      else
      {
        return "Члены классов";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    /*??*/
    DString trFileMembers() override
    {
      return "Список членов всех файлов";
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return "Описания"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Примеры"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Поиск"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Иерархия классов."; }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="файлов.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуры данных с их кратким описанием.";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Классы с их кратким описанием.";
      }
      else
      {
        return "Классы с их кратким описанием.";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
        DString result="Список всех ";
        if(!extractAll) result+="документированных ";
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result+="членов структур данных со ссылками на ";
        else
          result+="членов классов со ссылками на ";
        if(!extractAll)
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result+="документацию по структуре для каждого члена.";
          else
            result+="документацию по классу для каждого члена.";
        }
        else
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result += "структуры";
          else
            result += "классы";
          result+=", к которым они принадлежат.";
        }
        return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Список всех ";
      if (!extractAll) result+="документированных ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        result+="файлы, к которым они принадлежат.";
      else
        result+="документацию.";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Полный список примеров."; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Полный список дополнительных описаний."; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Полный список групп."; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Документация"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Алфавитный указатель групп"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Иерархический список классов"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Алфавитный указатель структур данных";
      }
      else
      {
        return "Алфавитный указатель классов";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Список файлов"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Группы"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуры данных";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Классы";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Файлы"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Оглавление"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Определения типов"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Элементы перечислений"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Типы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Функции"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуры данных";
      }
      else
      {
        return "Классы";
      }

    }

    /*! This is used in the documentation of a group before the list of
     *  links to documented files
     */
    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Документация ";
      if (!projName.empty()) result+=DString("по ")+projName;
      result+=DString(". Последние изменения: ")+date;
      result+=". Создано системой";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return DString("Граф наследования:")+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Предупреждения"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Версия"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Возвращает"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "См. также"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Аргументы"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Исключения"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Создано системой"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Пространства имен"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="пространств имен.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Друзья"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Документация по друзьям класса и функциям, относящимся"
        " к классу"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      DString result;
      if (isTemplate)
      {
        result="Шаблон ";
        switch(compType)
        {
          case ClassDef::Class:  result+="класса"; break;
          case ClassDef::Struct: result+="структуры"; break;
          case ClassDef::Union:  result+="объединения"; break;
          case ClassDef::Interface:  result+="интерфейса"; break;
          case ClassDef::Protocol:   result+="протокола"; break;
          case ClassDef::Category:   result+="категории"; break;
          case ClassDef::Exception:  result+="исключения"; break;
          default: break;
        }
      }
      else
      {
        switch(compType)
        {
          case ClassDef::Class:  result+="Класс"; break;
          case ClassDef::Struct: result+="Структура"; break;
          case ClassDef::Union:  result+="Объединение"; break;
          case ClassDef::Interface:  result+="Интерфейс"; break;
          case ClassDef::Protocol:   result+="Протокол"; break;
          case ClassDef::Category:   result+="Категория"; break;
          case ClassDef::Exception:  result+="Исключение"; break;
          default: break;
        }
      }
      result+=" ";
      return result+clName;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      return DString("Файл ")+fileName;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      return DString("Пространство имен ")+namespaceName;
    }

    DString trPublicMembers() override
    { return "Открытые члены"; }
    DString trPublicSlots() override
    { return "Открытые слоты"; }
    DString trSignals() override
    { return "Сигналы"; }
    DString trStaticPublicMembers() override
    { return "Открытые статические члены"; }
    DString trProtectedMembers() override
    { return "Защищенные члены"; }
    DString trProtectedSlots() override
    { return "Защищенные слоты"; }
    DString trStaticProtectedMembers() override
    { return "Защищенные статические члены"; }
    DString trPrivateMembers() override
    { return "Закрытые члены"; }
    DString trPrivateSlots() override
    { return "Закрытые слоты"; }
    DString trStaticPrivateMembers() override
    { return "Закрытые статические члены"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    DString trWriteList(int numEntries) override
    {
      DString result;
      // the inherits list contain `numEntries' classes
      for (int i=0;i<numEntries;i++)
      {
        // use generateMarker to generate placeholders for the class links!
        result+=generateMarker(i); // generate marker for entry i in the list
                                   // (order is left to right)

        if (i!=numEntries-1)  // not the last entry, so we need a separator
        {
          if (i<numEntries-2) // not the fore last entry
            result+=", ";
          else                // the fore last entry
            result+=" и ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Базовые классы:"+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Производные классы:"+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      DString result="Переопределяет метод";
      if(numEntries>1)
        result+="ы предков";
      else
        result+=" предка";
      return result+" "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Переопределяется в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Члены пространств имен"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="членов простанств имен.";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Алфавитный указатель пространств имен"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result=DString("Объявления и описания членов ");
      switch(compType)
      {
        case ClassDef::Class:      result+="класс";
          if (single) result+="а"; else result+="ов";
          break;
        case ClassDef::Struct:     result+="структур";
          if (single) result+="ы";
          break;
        case ClassDef::Union:      result+="объединени";
          if (single) result+="я"; else result+="й";
          break;
        case ClassDef::Interface:  result+="интерфейс";
          if (single) result+="а"; else result+="ов";
          break;
        case ClassDef::Protocol:  result+="протокол";
          if (single) result+="а"; else result+="ов";
          break;
        case ClassDef::Category:  result+="категори";
          if (single) result+="и"; else result+="й";
          break;
        case ClassDef::Exception:  result+="исключени";
          if (single) result+="я"; else result+="й";
          break;
        default:
          break;
      }
      result+=" находятся в файл";
      if (single) result+="е:"; else result+="ах:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Возвращаемые значения"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Титульная страница"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "См. определение в файле @1 строка @0";
    }
    DString trDefinedInSourceFile() override
    {
      return "См. определение в файле @0";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Уст.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Граф связей класса "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Граф включаемых заголовочных файлов для "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Конструктор(ы)";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "См. исходные тексты.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "См. документацию.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Предусловие";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Постусловие";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Инвариант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Инициализатор";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "Исходные тексты";
    }
    DString trGraphicalHierarchy() override
    {
      return "Иерархия классов. Графический вид.";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "см. графический вид.";
    }
    DString trGotoTextualHierarchy() override
    {
      return "см. текстовый вид.";
    }
    DString trPageIndex() override
    {
      return "Алфавитный указатель тематических описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Заметки";
    }
    DString trPublicTypes() override
    {
      return "Открытые типы";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Поля данных";
      }
      else
      {
        return "Открытые атрибуты";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Статические открытые данные";
    }
    DString trProtectedTypes() override
    {
      return "Защищенные типы";
    }
    DString trProtectedAttribs() override
    {
      return "Защищенные данные";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Статические защищенные данные";
    }
    DString trPrivateTypes() override
    {
      return "Закрытые типы";
    }
    DString trPrivateAttribs() override
    {
      return "Закрытые данные";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Закрытые статические данные";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    /*??*/
    {
      return "Необходимо сделать";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    /*??*/
    {
      return "Список задач";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Используется в";
    }
    DString trRemarks() override
    {
      return "Прим.";
    }
    DString trAttention() override
    {
      return "Внимание";
    }
    DString trInclByDepGraph() override
    {
      return "Граф файлов, в которые включается этот файл:";
    }
    DString trSince() override
    /*??*/
    {
      return "Начиная с";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Легенда";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Обозначения, используемые в графах.<p>\n"
        "Рассмотрим следующий пример:\n"
        "\\code\n"
        "/*! Невидимый класс из-за усечения */\n"
        "class Invisible { };\n\n"
        "/*! Усеченный класс, отношение наследования скрыто */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Недокументированный класс */\n"
        "class Undocumented { };\n\n"
        "/*! Открытое наследование */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Шаблон класса */\n"
        "template<class T> class Templ {};\n\n"
        "/*! Защищенное наследование */\n"
        "class ProtectedBase { };\n\n"
        "/*! Закрытое наследование */\n"
        "class PrivateBase { };\n\n"
        "/*! Класс, используемый классом Inherited */\n"
        "class Used { };\n\n"
        "/*! Класс, порожденный от других классов */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Получится следующий граф:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Прямоугольники в этом графе имеют следующее значение:\n"
        "<ul>\n"
        "<li>Заполненный черный прямоугольник представляет структуру или класс, "
        "для которого создан граф.\n"
        "<li>Прямоугольник с черной границей обозначает документированную структуру или класс.\n"
        "<li>Прямоугольник с серой границей обозначает недокументированную структуру или класс.\n"
        "<li>Прямоугольник с красной границей обозначает документированную структуру или класс, для которого\n"
        " не все отношения наследования/содержания показаны. Граф усечен, "
        "если он не поместился в указанных границах.\n"
        "</ul>\n"
        "Стрелки имеют следующее значение:\n"
        "<ul>\n"
        "<li>Темно-синяя стрелка используется для изображения отношения открытого наследования "
        "между двумя классами.\n"
        "<li>Темно-зеленая стрелка используется при защищенном наследовании.\n"
        "<li>Темно-красная стрелка используется при закрытом наследовании.\n"
        "<li>Фиолетовая стрелка используется, если класс содержится в"
        "другом класе или используется другим классом."
        "Со стрелкой указывается переменная, "
        "через которую доступен указываемый класс или структура. \n"
        "<li>Желтая стрелка используется для связи подстановки шаблона и "
        "шаблона, на основе которого эта подстановка выполнена. С шаблоном"
        "указывается параметр подстановки.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "см. легенду";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Тест";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Список тестов";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Свойства";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Полный список свойств";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуры данных";
      }
      else
      {
        return "Классы";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return DString("Пакет ")+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Полный список документированных пакетов.";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Пакеты";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Макроопределение:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Ошибка";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Ошибки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    DString trRTFansicp() override
    {
      return "1251";
    }
    /*! Used as ansicpg for RTF fcharset */
    DString trRTFCharSet() override
    {
      return "204";
    }
    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Алфавитный указатель";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        DString result((first_capital ? "Структуры данных" : "структуры данных"));
        return result;
      }
      else
      {
      return createNoun(first_capital, singular, "класс", "ы");
      }
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "файл", "ы");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "пространств", "а имен", "о имен");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "групп", "ы", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "страниц", "ы", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "член", "ы");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "глобальны", "е", "й");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "автор", "ы");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Перекрестные ссылки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Замещает "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Замещается в "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Оглавление";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Список устаревших определений и описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "События";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Cобытия";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Типы с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Функции с областью видимости пакета";
    }
    DString trPackageMembers() override
    {
      return "Члены с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Статические функции с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Переменные с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Статические переменные с областью видимости пакета";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Указатель";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Граф вызовов:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Результаты поиска";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "К сожалению, по Вашему запросу ничего не найдено.";
      }
      else if( numDocuments == 1 )
      {
        return "Найден 1 документ.";
      }
      else
      {
        return "Найден(о) <b>$num</b> документ(ов). "
          "Документы отсортированы по релевантности.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Найдено:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return "Исходный файл " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Дерево директорий"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Директории"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Алфавитный указатель директорий"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=DString("Содержание директории ")+ dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "директори", "и", "я");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Эта функция перегружена и предоставляется исключительно "
              "для удобства использования. Она отличается от вышеупомянутой "
              "только фактическими аргументами.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Граф вызова функции:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Элементы перечислений"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    // Простите переводчика, уже лет 20 не писал на фортране...
    // Любые замечания приму с благодарностью.

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Функции/подпрограммы"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Типы данных"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Поля данных"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Аннотированный список типов данных:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Список всех ";
      if (!extractAll)
      {
        result+="документированных ";
      }
      result+="членов типа со ссылками ";
      if (!extractAll)
      {
        result+="на документацию для каждого члена:";
      }
      else
      {
         result+="на содержащую структуру:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Типы данных"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Оглавление типов данных"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Типы данных"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Указатель модулей"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Аннотированный список";
      if (!extractAll) result+="документированных ";
      result+="модулей:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
      if (isTemplate)
      {
        switch(compType)
        {
          case ClassDef::Class:      result+=" Модуль"; break;
          case ClassDef::Struct:     result+=" Тип"; break;
          case ClassDef::Union:      result+=" Объединение"; break;
          case ClassDef::Interface:  result+=" Интерфейс"; break;
          case ClassDef::Protocol:   result+=" Протокол"; break;
          case ClassDef::Category:   result+=" Категория"; break;
          case ClassDef::Exception:  result+=" Исключение"; break;
          default: break;
        }
      }
      else
      {
        result+=" Шаблон ";
        switch(compType)
        {
          case ClassDef::Class:      result+="модуля"; break;
          case ClassDef::Struct:     result+="типа"; break;
          case ClassDef::Union:      result+="объединения"; break;
          case ClassDef::Interface:  result+="интерфейса"; break;
          case ClassDef::Protocol:   result+="протокола"; break;
          case ClassDef::Category:   result+="категории"; break;
          case ClassDef::Exception:  result+="исключения"; break;
          default: break;
        }
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      return DString("Модуль ") + namespaceName;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Члены модуля"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Список всех ";
      if (!extractAll) result+="документированных ";
      result+="модулей со ссылками ";
      if (extractAll)
      {
        result+="на документацию для каждого члена:";
      }
      else
      {
        result+="на модули, их содержащие:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Указатель модулей"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "модул", "и", "ь");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="Документация по ";
      switch(compType)
      {
        case ClassDef::Class:      result+="модулю"; break;
        case ClassDef::Struct:     result+="типу"; break;
        case ClassDef::Union:      result+="объединению"; break;
        case ClassDef::Interface:  result+="интерфейсу"; break;
        case ClassDef::Protocol:   result+="протоколу"; break;
        case ClassDef::Category:   result+="категории"; break;
        case ClassDef::Exception:  result+="исключению"; break;
        default: break;
      }
      result+=" сгенерирована на основе следующ";
      if (single) result+="его файла:"; else result+="их файлов:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "тип", "ы");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "подпрограмм", "ы", "а");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Согласование типов";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name+" Связь";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Загрузка...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Глобальное пространство имён";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Поиск...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Не найдено";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    DString trFileIn(const DString &name) override
    {
      return "Файл в "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Включает файл в "+name;
    }

    /** Compiles a date string.
     *  @param year Year in 4 digits
     *  @param month Month of the year: 1=January
     *  @param day Day of the Month: 1..31
     *  @param dayOfWeek Day of the week: 1=Monday..7=Sunday
     *  @param hour Hour of the day: 0..23
     *  @param minutes Minutes in the hour: 0..59
     *  @param seconds Seconds within the minute: 0..59
     *  @param includeTime Include time in the result string?
     */
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Пн","Вт","Ср","Чт","Пт","Сб","Вс" };
      static const char *months[] = { "Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
      static const char *days_full[]    = { "понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    DString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "янв", "фев", "мар", "апр", "май", "июн", "июл", "авг", "сен", "окт", "ноя", "дек" };
      static const char *months_full[]  = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

///////////////////////////////////////////////////////////////////////
// new since 1.7.5
///////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Библиографические ссылки"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Авторство"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Директория графа зависимостей ")+name+":"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.0
///////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "уровень детализации"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Параметры шаблона"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "и "+number+" больше..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "Документация для этого перечисления сгенерерирована из файл";
      if (!single) result += "ов";
      result+="а:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return name+" Ссылки на перечисление"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" унаследованные от "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Дополнительные унаследованные члены"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.2
///////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "включить" : "выключить";
      return "нажмите на "+opt+" для синхронизации панелей";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "По группам @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Расширяет класс @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Методы класса";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Методы экземпляра";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Документация метода";
    }

///////////////////////////////////////////////////////////////////////
// new since 1.8.4
///////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Экспортируемые интерфейсы"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Включённые сервисы"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Постоянные группы"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Ссылка на постоянную группу";
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Ссылка на сервис";
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Ссылка на одиночку";
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Документация для этого сервиса "
                      "сгенерирована из следующего файл";
      if (single) result+="а:"; else result+="ов:";
      return result;
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Документация по этому одиночке "
                      "сгенерирована из следующего файл";
      if (single) result+="а:"; else result+="ов:";
      return result;
    }

/*
 * Russian translations for updates since version 1.8.15
 *
 * Aleksei Leshchenko, 2024
 * Github: FrostMonsterSP
 */

  //////////////////////////////////////////////////////////////////////////
  // new since 1.8.15
  //////////////////////////////////////////////////////////////////////////

  /** VHDL design unit hierarchy */
  DString trDesignUnitHierarchy() override {
    return "Иерархия проектных единиц";
  }
  /** VHDL design unit list */
  DString trDesignUnitList() override { return "Список проектных единиц"; }
  /** VHDL design unit members */
  DString trDesignUnitMembers() override {
    return "Компоненты проектной единицы";
  }
  /** VHDL design unit list description */
  DString trDesignUnitListDescription() override {
    return "Список всех компонентов проектных единиц со ссылками на "
           "проектные единицы, в которые входят:";
  }
  /** VHDL design unit index */
  DString trDesignUnitIndex() override { return "Указатель проектных единиц"; }
  /** VHDL design units */
  DString trDesignUnits() override { return "Проектные единицы"; }
  /** VHDL functions/procedures/processes */
  DString trFunctionAndProc() override { return "Функции/Процедуры/Процессы"; }
  /** VHDL type */
  DString trVhdlType(VhdlSpecifier type, bool single) override {
    switch (type) {
    case VhdlSpecifier::LIBRARY:
      if (single)
        return "Библиотека";
      else
        return "Библиотеки";
    case VhdlSpecifier::PACKAGE:
      if (single)
        return "Пакет";
      else
        return "Пакеты";
    case VhdlSpecifier::SIGNAL:
      if (single)
        return "Сигнал";
      else
        return "Сигналы";
    case VhdlSpecifier::COMPONENT:
      if (single)
        return "Компонент";
      else
        return "Компоненты";
    case VhdlSpecifier::CONSTANT:
      if (single)
        return "Константа";
      else
        return "Константы";
    case VhdlSpecifier::ENTITY:
      if (single)
        return "Единица";
      else
        return "Единицы";
    case VhdlSpecifier::TYPE:
      if (single)
        return "Тип";
      else
        return "Типы";
    case VhdlSpecifier::SUBTYPE:
      if (single)
        return "Подтип";
      else
        return "Подтипы";
    case VhdlSpecifier::FUNCTION:
      if (single)
        return "Функция";
      else
        return "Функции";
    case VhdlSpecifier::RECORD:
      if (single)
        return "Запись";
      else
        return "Записи";
    case VhdlSpecifier::PROCEDURE:
      if (single)
        return "Процедура";
      else
        return "Процедуры";
    case VhdlSpecifier::ARCHITECTURE:
      if (single)
        return "Архитектура";
      else
        return "Архитектуры";
    case VhdlSpecifier::ATTRIBUTE:
      if (single)
        return "Аттрибут";
      else
        return "Аттрибуты";
    case VhdlSpecifier::PROCESS:
      if (single)
        return "Процесс";
      else
        return "Процессы";
    case VhdlSpecifier::PORT:
      if (single)
        return "Порт";
      else
        return "Порты";
    case VhdlSpecifier::USE:
      if (single)
        return "Условие использования";
      else
        return "Условия использования";
    case VhdlSpecifier::GENERIC:
      if (single)
        return "Дженерик";
      else
        return "Дженерики";
    case VhdlSpecifier::PACKAGE_BODY:
      return "Тело пакета";
    case VhdlSpecifier::UNITS:
      return "Единицы";
    case VhdlSpecifier::SHAREDVARIABLE:
      if (single)
        return "Общая переменная";
      else
        return "Общие переменные";
    case VhdlSpecifier::VFILE:
      if (single)
        return "Файл";
      else
        return "Файлы";
    case VhdlSpecifier::GROUP:
      if (single)
        return "Группа";
      else
        return "Группы";
    case VhdlSpecifier::INSTANTIATION:
      if (single)
        return "Созданный экземпляр";
      else
        return "Созданные экземпляры";
    case VhdlSpecifier::ALIAS:
      if (single)
        return "Псевдоним";
      else
        return "Псевдонимы";
    case VhdlSpecifier::CONFIG:
      if (single)
        return "Конфигурация";
      else
        return "Конфигурации";
    case VhdlSpecifier::MISCELLANEOUS:
      return "Разное";
    case VhdlSpecifier::UCF_CONST:
      return "Ограничения";
    default:
      return "Класс";
    }
  }
  DString trCustomReference(const DString &name) override {
    return "Документация " + name;
  }

  /* Slice */
  DString trConstants() override { return "Константы"; }
  DString trConstantDocumentation() override {
    return "Документация константы";
  }
  DString trSequences() override { return "Последовательности"; }
  DString trSequenceDocumentation() override {
    return "Документация последовательности";
  }
  DString trDictionaries() override { return "Словари"; }
  DString trDictionaryDocumentation() override {
    return "Документация словаря";
  }
  DString trSliceInterfaces() override { return "Интерфейсы"; }
  DString trInterfaceIndex() override { return "Документация интерфейса"; }
  DString trInterfaceList() override { return "Список интерфейсов"; }
  DString trInterfaceListDescription() override {
    return "Здесь представлен список интерфейсов с их кратким описанием:";
  }
  DString trInterfaceHierarchy() override { return "Иерархия интерфейса"; }
  DString trInterfaceHierarchyDescription() override {
    return "Данный список наследований по большей части, но не "
           "полностью, отсортирован в алфавитном порядке:";
  }
  DString trInterfaceDocumentation() override {
    return "Документация интерфейса";
  }
  DString trStructs() override { return "Структуры"; }
  DString trStructIndex() override { return "Указатель Структур"; }
  DString trStructList() override { return "Список Структур"; }
  DString trStructListDescription() override {
    return "Здесь представлен список структур с их краткими описаниями:";
  }
  DString trStructDocumentation() override { return "Документация структур"; }
  DString trExceptionIndex() override { return "Указатель исключений"; }
  DString trExceptionList() override { return "Список исключений"; }
  DString trExceptionListDescription() override {
    return "Здесь представлен список исключений с их краткими описаниями:";
  }
  DString trExceptionHierarchy() override { return "Иерархия Исключения"; }
  DString trExceptionHierarchyDescription() override {
    return "Данный список наследований частично, но не полностью, отсортирован "
           "в алфавитном порядке:";
  }
  DString trExceptionDocumentation() override {
    return "Документация исключения";
  }
  DString trCompoundReferenceSlice(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isLocal) override {
    DString result = "Ссылка на";
    switch (compType) {
    case ClassDef::Class:
      if (isLocal)
        result += " локальный";
      result += " класс ";
      break;
    case ClassDef::Struct:
      if (isLocal)
        result += " локальную";
      result += " структуру ";
      break;
    case ClassDef::Union:
      if (isLocal)
        result += " локальное";
      result += " объединение ";
      break;
    case ClassDef::Interface:
      if (isLocal)
        result += " локальный";
      result += " интерфейс ";
      break;
    case ClassDef::Protocol:
      if (isLocal)
        result += " локальный";
      result += " протокол ";
      break;
    case ClassDef::Category:
      if (isLocal)
        result += " локальную";
      result += " категорию ";
      break;
    case ClassDef::Exception:
      if (isLocal)
        result += " локальное";
      result += " исключение ";
      break;
    default:
      break;
    }
    return result + clName;
  }
  DString trOperations() override { return "Операции"; }
  DString trOperationDocumentation() override {
    return "Документация операции";
  }
  DString trDataMembers() override { return "Поля класса"; }
  DString trDataMemberDocumentation() override {
    return "Документация для полей класса";
  }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.8.19
  //////////////////////////////////////////////////////////////////////////

  /** VHDL design unit documentation */
  DString trDesignUnitDocumentation() override { return "Проектная единица"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.2
  //////////////////////////////////////////////////////////////////////////

  /** C++20 concept */
  DString trConcept(bool first_capital, bool singular) override {
    return createNoun(first_capital, singular, "концепт", "ы");
  }
  /*! used as the title of the HTML page of a C++20 concept page */
  DString trConceptReference(const DString &conceptName) override {
    return "Ссылка на концепт " + conceptName;
  }

  /*! used as the title of page containing all the index of all concepts. */
  DString trConceptList() override { return "Список концептов"; }

  /*! used as the title of chapter containing the index listing all concepts. */
  DString trConceptIndex() override { return "Указатель концептов"; }

  /*! used as the title of chapter containing all information about concepts. */
  DString trConceptDocumentation() override {
    return "Документация концептов";
  }

  /*! used as an introduction to the concept list */
  DString trConceptListDescription(bool extractAll) override {
    DString result = "Список всех  ";
    if (!extractAll)
      result += "задокументированных ";
    result += "концептов с их краткими описаниями:";
    return result;
  }

  /*! used to introduce the definition of the C++20 concept */
  DString trConceptDefinition() override { return "Определение концепта"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.4
  //////////////////////////////////////////////////////////////////////////

  DString trPackageList() override { return "Список пакетов"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.6
  //////////////////////////////////////////////////////////////////////////

  /*! This is used for translation of the word that will be
   *  followed by a single name of the VHDL process flowchart.
   */
  DString trFlowchart() override { return "Блок-схема:"; }

  /*! Please translate also updated body of the method
   *  trMemberFunctionDocumentation(), now better adapted for
   *  VHDL sources documentation.
   */

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.7
  //////////////////////////////////////////////////////////////////////////
  /*! used in the compound documentation before a list of related symbols.
   *
   *  Supersedes trRelatedFunctions
   */
  DString trRelatedSymbols() override { return "Относящиеся к классу:"; }

  /*! subscript for the related symbols
   *
   *  Supersedes trRelatedSubscript
   */
  DString trRelatedSymbolsSubscript() override { return "(не члены класса)"; }

  /*! used in the class documentation as a header before the list of all
   * related classes.
   *
   * Supersedes trRelatedFunctionDocumentation
   */
  DString trRelatedSymbolDocumentation() override {
    return "Друзья класса и относящимся к классу обозначения";
  }

  /*! the compound type as used for the xrefitems */
  DString trCompoundType(ClassDef::CompoundType compType,
                          SrcLangExt lang) override {
    DString result;
    switch (compType) {
    case ClassDef::Class:
      if (lang == SrcLangExt::Fortran)
        result = trType(true, true);
      else
        result = trClass(true, true);
      break;
    case ClassDef::Struct:
      result = "Структура";
      break;
    case ClassDef::Union:
      result = "Объединение";
      break;
    case ClassDef::Interface:
      result = "Интерфейс";
      break;
    case ClassDef::Protocol:
      result = "Протокол";
      break;
    case ClassDef::Category:
      result = "Категория";
      break;
    case ClassDef::Exception:
      result = "Исключение";
      break;
    case ClassDef::Service:
      result = "Служба";
      break;
    case ClassDef::Singleton:
      result = "Синглтон";
      break;
    default:
      break;
    }
    return result;
  }

  DString
  trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    DString result = "Список всех  ";
    if (!extractAll)
      result += "задокументированных ";

    switch (hl) {
    case FileMemberHighlight::All:
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result += "функций, переменных, макроопределений, перечислителей и "
                  "определений типов";
      } else {
        result += "прикреплённых файлов";
      }
      break;
    case FileMemberHighlight::Functions:
      result += "функций";
      break;
    case FileMemberHighlight::Variables:
      result += "переменных";
      break;
    case FileMemberHighlight::Typedefs:
      result += "определений типов";
      break;
    case FileMemberHighlight::Sequences:
      result += "последовательностей";
      break;
    case FileMemberHighlight::Dictionaries:
      result += "словарей";
      break;
    case FileMemberHighlight::Enums:
      result += "перечислителей";
      break;
    case FileMemberHighlight::EnumValues:
      result += "значений перечислителей";
      break;
    case FileMemberHighlight::Defines:
      result += "макроопределений";
      break;
    case FileMemberHighlight::Total: // for completeness
      break;
    }
    result += " соссылками на ";
    if (extractAll)
      result += "файлы, к которым они относятся:";
    else
      result += "документацию:";
    return result;
  }
  DString
  trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    DString result = "Список всех  ";
    if (!extractAll) {
      result += "задокументированных ";
    }

    switch (hl) {
    case ClassMemberHighlight::All:
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result += "полей структур и объединений";
      } else {
        result += "членов класса";
      }
      break;
    case ClassMemberHighlight::Functions:
      result += "функций";
      break;
    case ClassMemberHighlight::Variables:
      result += "переменных";
      break;
    case ClassMemberHighlight::Typedefs:
      result += "определений типов";
      break;
    case ClassMemberHighlight::Enums:
      result += "перечислителей";
      break;
    case ClassMemberHighlight::EnumValues:
      result += "значений перечислителей";
      break;
    case ClassMemberHighlight::Properties:
      result += "свойств";
      break;
    case ClassMemberHighlight::Events:
      result += "событий";
      break;
    case ClassMemberHighlight::Related:
      result += "связанных символов";
      break;
    case ClassMemberHighlight::Total: // for completeness
      break;
    }
    result += " со ссылками на ";
    if (!extractAll) {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result += "документацию каждого поля структуры/объединения:";
      } else {
        result += "документацию класса каждого члена:";
      }
    } else {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result += "структуры/объединения, к которым они относятся:";
      } else {
        result += "классы, к которым они относятся:";
      }
    }
    return result;
  }
  DString trNamespaceMembersDescriptionTotal(
      NamespaceMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    DString result = "Список всех ";
    if (!extractAll)
      result += "задокументированных ";
    DString singularResult = "";
    DString pluralResult = "";
    switch (hl) {
    case NamespaceMemberHighlight::All:
      singularResult = "члена";
      pluralResult = "членов";
      break;
    case NamespaceMemberHighlight::Functions:
      singularResult = "функции";
      pluralResult = "функций";
      break;
    case NamespaceMemberHighlight::Variables:
      singularResult = "переменной";
      pluralResult = "переменных";
      break;
    case NamespaceMemberHighlight::Typedefs:
      singularResult = "определения";
      pluralResult = "определений типов";
      break;
    case NamespaceMemberHighlight::Sequences:
      singularResult = "последовательности";
      pluralResult = "последовательностей";
      break;
    case NamespaceMemberHighlight::Dictionaries:
      singularResult = "словаря";
      pluralResult = "словарей";
      break;
    case NamespaceMemberHighlight::Enums:
      singularResult = "перечислителя";
      pluralResult = "перечислителей";
      break;
    case NamespaceMemberHighlight::EnumValues:
      singularResult = "значения";
      pluralResult = "значений перечислителей";
      break;
    case NamespaceMemberHighlight::Total: // for completeness
      break;
    }
    result += pluralResult + " со ссылками на ";
    if (extractAll)
      result +=
          "документацию пространства имён для каждого " + singularResult + ":";
    else
      result += "пространство имён, к которому они принадлежат:";
    return result;
  }
  DString trDefinition() override { return "Определения"; }
  DString trDeclaration() override { return "Объявления"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.8
  //////////////////////////////////////////////////////////////////////////

  DString trTopics() override { return "Разделы"; }
  DString trTopicDocumentation() override { return "Документация разделов"; }
  DString trTopicList() override { return "Список разделов"; }
  DString trTopicIndex() override { return "Указатель разделов"; }
  DString trTopicListDescription() override {
    return "Список всех разделов c краткими описаниями:";
  }
  DString
  trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    DString result = "Список всех ";
    if (!extractAll)
      result += "задокументированных ";
    DString singularResult = "";
    DString pluralResult = "";
    switch (hl) {
    case ModuleMemberHighlight::All:
      singularResult = "члена";
      pluralResult = "членов";
      break;
    case ModuleMemberHighlight::Functions:
      singularResult = "функции";
      pluralResult = "функций";
      break;
    case ModuleMemberHighlight::Variables:
      singularResult = "переменной";
      pluralResult = "переменных";
      break;
    case ModuleMemberHighlight::Typedefs:
      singularResult = "определения";
      pluralResult = "определений типов";
      break;
    case ModuleMemberHighlight::Enums:
      singularResult = "перечислителя";
      pluralResult = "перечислителей";
      break;
    case ModuleMemberHighlight::EnumValues:
      singularResult = "значения";
      pluralResult = "значений перечислителей";
      break;
    case ModuleMemberHighlight::Total: // for completeness
      break;
    };
    result += pluralResult + " со ссылками на ";
    if (extractAll)
      result += "на документацию модуля для каждого " + singularResult + ":";
    else
      result += "на модуль к которому они принадлежат:";
    return result;
  }
  DString trExportedModules() override { return "Экспортируемые модули"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.10.0
  //////////////////////////////////////////////////////////////////////////

  DString trCopyToClipboard() override { return "Скопировать в буфер обмена"; }
  //////////////////////////////////////////////////////////////////////////
  // new since 1.11.0
  //////////////////////////////////////////////////////////////////////////
  DString trImportant() override { return "Важно!"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.16.0
  //////////////////////////////////////////////////////////////////////////

  // the title of the requirements overview page
  DString trRequirements() override
  {
    return "Требования";
  }
  // table header for the column with the requirements IDs
  DString trRequirementID() override
  {
    return "ID";
  }
  // indicates a symbol implements (satisfies) a requirement
  DString trSatisfies(bool singular) override
  {
    return createNoun(true, singular, "Удовлетворяет требовани", "ям", "ю");
  }
  // indicates a requirement is satisfied (implemented) by one or more symbols
  DString trSatisfiedBy(const DString &list) override
  {
    return "Удовлетворяется "+list+".";
  }
  DString trUnsatisfiedRequirements() override
  {
    return "Неудовлетворенные требования";
  }
  DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
  {
    return createNoun(true, singular, "Требовани", "я", "е")+" "+list+" не "+
           (singular ? "имеет" : "имеют")+" отношения 'удовлетворяет'.";
  }
  // indicates a symbol verifies (tests) a requirement
  DString trVerifies(bool singular) override
  {
    return createNoun(true, singular, "Проверяет требовани", "я", "е");
  }
  // indicates a requirement is verified (tested) by one or more symbols
  DString trVerifiedBy(const DString &list) override
  {
    return "Проверяется "+list+".";
  }
  DString trUnverifiedRequirements() override
  {
    return "Непроверенные требования";
  }
  DString trUnverifiedRequirementsText(bool singular,const DString &list) override
  {
    return createNoun(true, singular, "Требовани", "я", "е")+" "+list+" не "+
           (singular ? "имеет" : "имеют")+" отношения 'проверяет'.";
  }
};

#endif
