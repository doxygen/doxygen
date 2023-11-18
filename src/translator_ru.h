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
 */

#ifndef TRANSLATOR_RU_H
#define TRANSLATOR_RU_H

class TranslatorRussian : public TranslatorAdapter_1_8_15
{
  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage() override
    { return "russian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand() override
    { return "\\usepackage[T2A]{fontenc}\n\\usepackage[russian]{babel}\n"; }

    virtual QCString trISOLang() override
    { return "ru"; }
    virtual QCString getLanguageString() override
    {
      return "0x419 Russian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Относящиеся к классу функции"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(не члены класса)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Подробное описание"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Подробности"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Определения типов"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Перечисления"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Методы"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
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
    virtual QCString trMore() override
    { return "Подробнее..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    virtual QCString trListOfAllMembers() override
    {
      return "Полный список членов класса";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    virtual QCString trMemberList() override
    {
      return "Cписок членов класса";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Полный список членов класса "; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers() override
    { return ", включая наследуемые из базового класса"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Автоматически создано Doxygen";
      if (!s.isEmpty()) result+=QCString(" для ")+s;
      result+=" из исходного текста.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "перечисление"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "элементы перечисления"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "определено в"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Группы"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Иерархия классов"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
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
    virtual QCString trFileList() override
    { return "Файлы"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
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
    virtual QCString trFileMembers() override
    {
      return "Список членов всех файлов";
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return "Описания"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Примеры"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Поиск"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    { return "Иерархия классов."; }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="файлов.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
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
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
        QCString result="Список всех ";
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
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Список всех ";
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
    virtual QCString trExamplesDescription() override
    { return "Полный список примеров."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Полный список дополнительных описаний."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Полный список групп."; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Документация"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Алфавитный указатель групп"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Иерархический список классов"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
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
    virtual QCString trFileIndex() override
    { return "Список файлов"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Группы"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
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
    virtual QCString trFileDocumentation() override
    { return "Файлы"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Оглавление"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Определения типов"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Элементы перечислений"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Типы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Функции"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
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
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Документация ";
      if (!projName.isEmpty()) result+=QCString("по ")+projName;
      result+=QCString(". Последние изменения: ")+date;
      result+=". Создано системой";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("Граф наследования:")+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Предупреждения"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Версия"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Возвращает"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "См. также"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Аргументы"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Исключения"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Создано системой"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Пространства имен"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="пространств имен.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Друзья"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Документация по друзьям класса и функциям, относящимся"
        " к классу"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      QCString result;
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
    virtual QCString trFileReference(const QCString &fileName) override
    {
      return QCString("Файл ")+fileName;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      return QCString("Пространство имен ")+namespaceName;
    }

    virtual QCString trPublicMembers() override
    { return "Открытые члены"; }
    virtual QCString trPublicSlots() override
    { return "Открытые слоты"; }
    virtual QCString trSignals() override
    { return "Сигналы"; }
    virtual QCString trStaticPublicMembers() override
    { return "Открытые статические члены"; }
    virtual QCString trProtectedMembers() override
    { return "Защищенные члены"; }
    virtual QCString trProtectedSlots() override
    { return "Защищенные слоты"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Защищенные статические члены"; }
    virtual QCString trPrivateMembers() override
    { return "Закрытые члены"; }
    virtual QCString trPrivateSlots() override
    { return "Закрытые слоты"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Закрытые статические члены"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries) override
    {
      QCString result;
      int i;
      // the inherits list contain `numEntries' classes
      for (i=0;i<numEntries;i++)
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
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Базовые классы:"+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Производные классы:"+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      QCString result="Переопределяет метод";
      if(numEntries>1)
	result+="ы предков";
      else
	result+=" предка";
      return result+" "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Переопределяется в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Члены пространств имен"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="членов простанств имен.";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Алфавитный указатель пространств имен"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=QCString("Объявления и описания членов ");
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
    virtual QCString trReturnValues() override
    { return "Возвращаемые значения"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Титульная страница"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "См. определение в файле @1 строка @0";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "См. определение в файле @0";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Уст.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Граф связей класса "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Граф включаемых заголовочных файлов для "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Конструктор(ы)";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "См. исходные тексты.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "См. документацию.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Предусловие";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Постусловие";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Инвариант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Инициализатор";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "Исходные тексты";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Иерархия классов. Графический вид.";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "см. графический вид.";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "см. текстовый вид.";
    }
    virtual QCString trPageIndex() override
    {
      return "Алфавитный указатель тематических описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Заметки";
    }
    virtual QCString trPublicTypes() override
    {
      return "Открытые типы";
    }
    virtual QCString trPublicAttribs() override
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
    virtual QCString trStaticPublicAttribs() override
    {
      return "Статические открытые данные";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Защищенные типы";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Защищенные данные";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Статические защищенные данные";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Закрытые типы";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Закрытые данные";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Закрытые статические данные";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo() override
    /*??*/
    {
      return "Необходимо сделать";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    /*??*/
    {
      return "Список задач";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Используется в";
    }
    virtual QCString trRemarks() override
    {
      return "Прим.";
    }
    virtual QCString trAttention() override
    {
      return "Внимание";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Граф файлов, в которые включается этот файл:";
    }
    virtual QCString trSince() override
    /*??*/
    {
      return "Начиная с";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Легенда";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs() override
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
        "Если \\c MAX_DOT_GRAPH_HEIGHT в конфигурационном файле "
        "установлен в 240, получится следующий граф:"
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
    virtual QCString trLegend() override
    {
      return "см. легенду";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Тест";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Список тестов";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Свойства";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Полный список свойств";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
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
    virtual QCString trPackage(const QCString &name) override
    {
      return QCString("Пакет ")+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Полный список документированных пакетов.";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Пакеты";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Макроопределение:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Ошибка";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Ошибки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp() override
    {
      return "1251";
    }
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet() override
    {
      return "204";
    }
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Алфавитный указатель";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        QCString result((first_capital ? "Структуры данных" : "структуры данных"));
        return result;
      }
      else
      {
        QCString result((first_capital ? "Класс" : "класс"));
        if(!singular) result+="ы";
        return result;
      }
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Файл" : "файл"));
      if (!singular)  result+="ы";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Пространств" : "пространств"));
      result+=(singular?"о имен":"а имен");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Групп" : "групп"));
      result+=(singular ? "а" : "ы");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Страниц" : "страниц"));
      result+=(singular ? "а" : "ы");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Член" : "член"));
      if (!singular)  result+="ы";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Глобальны" : "глобальны"));
      result+=(singular ? "й" : "е");
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Автор" : "автор"));
      if (!singular) result+="ы";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Перекрестные ссылки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Замещает "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Замещается в "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Оглавление";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Список устаревших определений и описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "События";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Cобытия";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Типы с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Функции с областью видимости пакета";
    }
    virtual QCString trPackageMembers() override
    {
      return "Члены с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Статические функции с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Переменные с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Статические переменные с областью видимости пакета";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Указатель";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Граф вызовов:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
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
    virtual QCString trSearchResults(int numDocuments) override
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
    virtual QCString trSearchMatches() override
    {
      return "Найдено:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return "Исходный файл " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Дерево директорий"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Директории"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Алфавитный указатель директорий"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=QCString("Содержание директории ")+ dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Директори" : "директори"));
      if (singular) result+="я"; else result+="и";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText() override
    {
       return "Эта функция перегружена и предоставляется исключительно "
              "для удобства использования. Она отличается от вышеупомянутой "
              "только фактическими аргументами.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Граф вызова функции:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Элементы перечислений"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    // Простите переводчика, уже лет 20 не писал на фортране...
    // Любые замечания приму с благодарностью.

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Функции/подпрограммы"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Типы данных"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Поля данных"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Аннотированный список типов данных:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Список всех ";
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
    virtual QCString trCompoundIndexFortran() override
    { return "Типы данных"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Оглавление типов данных"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Типы данных"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Указатель модулей"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Аннотированный список";
      if (!extractAll) result+="документированных ";
      result+="модулей:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
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
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      return QCString("Модуль ") + namespaceName;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Члены модуля"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Список всех ";
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
    virtual QCString trModulesIndex() override
    { return "Указатель модулей"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Модул" : "модул"));
      if (singular)  result+="ь"; else result+="и";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Документация по ";
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
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Тип" : "тип"));
      if (!singular)  result+="ы";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Подпрограмм" : "подпрограмм"));
      if (singular)  result+="а"; else result+="ы";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Согласование типов";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Связь";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Загрузка...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Глобальное пространство имён";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Поиск...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
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
    virtual QCString trFileIn(const QCString &name) override
    {
      return "Файл в "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
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
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Пн","Вт","Ср","Чт","Пт","Сб","Вс" };
      static const char *months[] = { "Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
      static const char *days_full[]    = { "понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "янв", "фев", "мар", "апр", "май", "июн", "июл", "авг", "сен", "окт", "ноя", "дек" };
      static const char *months_full[]  = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period];
    }

///////////////////////////////////////////////////////////////////////
// new since 1.7.5
///////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences() override
    { return "Библиографические ссылки"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Авторство"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return QCString("Директория графа зависимостей ")+name+":"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.0
///////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel() override
    { return "уровень детализации"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters() override
    { return "Параметры шаблона"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number) override
    { return "и "+number+" больше..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Документация для этого перечисления сгенерерирована из файл";
      if (!single) result += "ов";
      result+="а:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Ссылки на перечисление"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" унаследованные от "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers() override
    { return "Дополнительные унаследованные члены"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.2
///////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "включить" : "выключить";
      return "нажмите на "+opt+" для синхронизации панелей";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory() override
    {
      return "По группам @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass() override
    {
      return "Расширяет класс @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods() override
    {
      return "Методы класса";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods() override
    {
      return "Методы экземпляра";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation() override
    {
      return "Документация метода";
    }

///////////////////////////////////////////////////////////////////////
// new since 1.8.4
///////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces() override
    { return "Экспортируемые интерфейсы"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices() override
    { return "Включённые сервисы"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups() override
    { return "Постоянные группы"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Ссылка на постоянную группу";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Ссылка на сервис";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Ссылка на одиночку";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Документация для этого сервиса "
                                "сгенерирована из следующего файл";
      if (single) result+="а:"; else result+="ов:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Документация по этому одиночке "
                                "сгенерирована из следующего файл";
      if (single) result+="а:"; else result+="ов:";
      return result;
    }
};

#endif
