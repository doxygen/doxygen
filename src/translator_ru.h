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

class TranslatorRussian : public Translator
{
  public:
    /*! Used for identification of the language. */
    QCString idLanguage() override
    { return "russian"; }

    /* Used to get the command(s) for the language support. */
    QCString latexLanguageSupportCommand() override
    { return "\\usepackage[T2A]{fontenc}\n\\usepackage[russian]{babel}\n"; }

    QCString trISOLang() override
    { return "ru"; }
    QCString getLanguageString() override
    {
      return "0x419 Russian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Относящиеся к классу функции"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(не члены класса)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Подробное описание"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Подробности"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Определения типов"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Перечисления"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Методы"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Подробнее..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    QCString trListOfAllMembers() override
    {
      return "Полный список членов класса";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    QCString trMemberList() override
    {
      return "Cписок членов класса";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    QCString trThisIsTheListOfAllMembers() override
    { return "Полный список членов класса"; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    QCString trIncludingInheritedMembers() override
    { return ", включая наследуемые из базового класса"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Автоматически создано Doxygen";
      if (!s.isEmpty()) result+=QCString(" для ")+s;
      result+=" из исходного текста.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "перечисление"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "элементы перечисления"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "определено в"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Группы"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Иерархия классов"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Файлы"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
    {
      return "Список членов всех файлов";
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return "Описания"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Примеры"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Поиск"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Иерархия классов."; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="файлов.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
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
    QCString trFileMembersDescription(bool extractAll) override
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
    QCString trExamplesDescription() override
    { return "Полный список примеров."; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Полный список дополнительных описаний."; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Полный список групп."; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Документация"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Алфавитный указатель групп"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Иерархический список классов"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Список файлов"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Группы"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Файлы"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Оглавление"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Определения типов"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Элементы перечислений"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Макросы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Типы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Перечисления"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Функции"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Переменные"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Документация ";
      if (!projName.isEmpty()) result+=QCString("по ")+projName;
      result+=QCString(". Последние изменения: ")+date;
      result+=". Создано системой";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("Граф наследования:")+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Предупреждения"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Версия"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Возвращает"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "См. также"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Аргументы"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Исключения"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Создано системой"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Пространства имен"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="пространств имен.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Друзья"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Документация по друзьям класса и функциям, относящимся"
        " к классу"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
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
    QCString trFileReference(const QCString &fileName) override
    {
      return QCString("Файл ")+fileName;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      return QCString("Пространство имен ")+namespaceName;
    }

    QCString trPublicMembers() override
    { return "Открытые члены"; }
    QCString trPublicSlots() override
    { return "Открытые слоты"; }
    QCString trSignals() override
    { return "Сигналы"; }
    QCString trStaticPublicMembers() override
    { return "Открытые статические члены"; }
    QCString trProtectedMembers() override
    { return "Защищенные члены"; }
    QCString trProtectedSlots() override
    { return "Защищенные слоты"; }
    QCString trStaticProtectedMembers() override
    { return "Защищенные статические члены"; }
    QCString trPrivateMembers() override
    { return "Закрытые члены"; }
    QCString trPrivateSlots() override
    { return "Закрытые слоты"; }
    QCString trStaticPrivateMembers() override
    { return "Закрытые статические члены"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
      QCString result;
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
    QCString trInheritsList(int numEntries) override
    {
      return "Базовые классы:"+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Производные классы:"+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
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
    QCString trReimplementedInList(int numEntries) override
    {
      return "Переопределяется в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Члены пространств имен"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="членов простанств имен.";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Алфавитный указатель пространств имен"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Пространства имен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
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
    QCString trReturnValues() override
    { return "Возвращаемые значения"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Титульная страница"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "См. определение в файле @1 строка @0";
    }
    QCString trDefinedInSourceFile() override
    {
      return "См. определение в файле @0";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Уст.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Граф связей класса "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Граф включаемых заголовочных файлов для "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Конструктор(ы)";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "См. исходные тексты.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "См. документацию.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Предусловие";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Постусловие";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Инвариант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Инициализатор";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "Исходные тексты";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Иерархия классов. Графический вид.";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "см. графический вид.";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "см. текстовый вид.";
    }
    QCString trPageIndex() override
    {
      return "Алфавитный указатель тематических описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Заметки";
    }
    QCString trPublicTypes() override
    {
      return "Открытые типы";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Статические открытые данные";
    }
    QCString trProtectedTypes() override
    {
      return "Защищенные типы";
    }
    QCString trProtectedAttribs() override
    {
      return "Защищенные данные";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Статические защищенные данные";
    }
    QCString trPrivateTypes() override
    {
      return "Закрытые типы";
    }
    QCString trPrivateAttribs() override
    {
      return "Закрытые данные";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Закрытые статические данные";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    /*??*/
    {
      return "Необходимо сделать";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    /*??*/
    {
      return "Список задач";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Используется в";
    }
    QCString trRemarks() override
    {
      return "Прим.";
    }
    QCString trAttention() override
    {
      return "Внимание";
    }
    QCString trInclByDepGraph() override
    {
      return "Граф файлов, в которые включается этот файл:";
    }
    QCString trSince() override
    /*??*/
    {
      return "Начиная с";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Легенда";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "см. легенду";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Тест";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Список тестов";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Свойства";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Полный список свойств";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return QCString("Пакет ")+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Полный список документированных пакетов.";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Пакеты";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Макроопределение:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Ошибка";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Ошибки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp() override
    {
      return "1251";
    }
    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet() override
    {
      return "204";
    }
    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Алфавитный указатель";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        QCString result((first_capital ? "Структуры данных" : "структуры данных"));
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
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "файл", "ы");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "пространств", "а имен", "о имен");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "групп", "ы", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "страниц", "ы", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "член", "ы");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "глобальны", "е", "й");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "автор", "ы");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Перекрестные ссылки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Замещает "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Замещается в "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Оглавление";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Список устаревших определений и описаний";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "События";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Cобытия";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Типы с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Функции с областью видимости пакета";
    }
    QCString trPackageMembers() override
    {
      return "Члены с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Статические функции с областью видимости пакета";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Переменные с областью видимости пакета";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Статические переменные с областью видимости пакета";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Указатель";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Граф вызовов:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Найдено:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Исходный файл " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Дерево директорий"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Директории"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Алфавитный указатель директорий"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=QCString("Содержание директории ")+ dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "директори", "и", "я");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Эта функция перегружена и предоставляется исключительно "
              "для удобства использования. Она отличается от вышеупомянутой "
              "только фактическими аргументами.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Граф вызова функции:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Элементы перечислений"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    // Простите переводчика, уже лет 20 не писал на фортране...
    // Любые замечания приму с благодарностью.

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Функции/подпрограммы"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Типы данных"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Поля данных"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Аннотированный список типов данных:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Типы данных"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Оглавление типов данных"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Функции/подпрограммы"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Типы данных"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Указатель модулей"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Аннотированный список";
      if (!extractAll) result+="документированных ";
      result+="модулей:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      return QCString("Модуль ") + namespaceName;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Члены модуля"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Указатель модулей"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "модул", "и", "ь");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "тип", "ы");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "подпрограмм", "ы", "а");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Согласование типов";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Связь";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Загрузка...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Глобальное пространство имён";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Поиск...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Файл в "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
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
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
      static const char *days_full[]    = { "понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "янв", "фев", "мар", "апр", "май", "июн", "июл", "авг", "сен", "окт", "ноя", "дек" };
      static const char *months_full[]  = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

///////////////////////////////////////////////////////////////////////
// new since 1.7.5
///////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Библиографические ссылки"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Авторство"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Директория графа зависимостей ")+name+":"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.0
///////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "уровень детализации"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Параметры шаблона"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "и "+number+" больше..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Документация для этого перечисления сгенерерирована из файл";
      if (!single) result += "ов";
      result+="а:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Ссылки на перечисление"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" унаследованные от "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Дополнительные унаследованные члены"; }

///////////////////////////////////////////////////////////////////////
// new since 1.8.2
///////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "включить" : "выключить";
      return "нажмите на "+opt+" для синхронизации панелей";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "По группам @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Расширяет класс @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Методы класса";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Методы экземпляра";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Документация метода";
    }

///////////////////////////////////////////////////////////////////////
// new since 1.8.4
///////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Экспортируемые интерфейсы"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Включённые сервисы"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Постоянные группы"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Ссылка на постоянную группу";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Ссылка на сервис";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Ссылка на одиночку";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Документация для этого сервиса "
                                "сгенерирована из следующего файл";
      if (single) result+="а:"; else result+="ов:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Документация по этому одиночке "
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
  QCString trDesignUnitHierarchy() override {
    return "Иерархия проектных единиц";
  }
  /** VHDL design unit list */
  QCString trDesignUnitList() override { return "Список проектных единиц"; }
  /** VHDL design unit members */
  QCString trDesignUnitMembers() override {
    return "Компоненты проектной единицы";
  }
  /** VHDL design unit list description */
  QCString trDesignUnitListDescription() override {
    return "Список всех компонентов проектных единиц со ссылками на "
           "проектные единицы, в которые входят:";
  }
  /** VHDL design unit index */
  QCString trDesignUnitIndex() override { return "Указатель проектных единиц"; }
  /** VHDL design units */
  QCString trDesignUnits() override { return "Проектные единицы"; }
  /** VHDL functions/procedures/processes */
  QCString trFunctionAndProc() override { return "Функции/Процедуры/Процессы"; }
  /** VHDL type */
  QCString trVhdlType(VhdlSpecifier type, bool single) override {
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
        return "Конатанта";
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
  QCString trCustomReference(const QCString &name) override {
    return "Документация " + name;
  }

  /* Slice */
  QCString trConstants() override { return "Константы"; }
  QCString trConstantDocumentation() override {
    return "Документация константы";
  }
  QCString trSequences() override { return "Последовательности"; }
  QCString trSequenceDocumentation() override {
    return "Документация последовательности";
  }
  QCString trDictionaries() override { return "Словари"; }
  QCString trDictionaryDocumentation() override {
    return "Документация словаря";
  }
  QCString trSliceInterfaces() override { return "Интерфейсы"; }
  QCString trInterfaceIndex() override { return "Документация интерфейса"; }
  QCString trInterfaceList() override { return "Список интерфейсов"; }
  QCString trInterfaceListDescription() override {
    return "Здесь представлен список интерфейсов с их кратким описанием:";
  }
  QCString trInterfaceHierarchy() override { return "Иерархия интерфейса"; }
  QCString trInterfaceHierarchyDescription() override {
    return "Данный список наследований по большей части, но не "
           "полностью, отсортирован в алфавитном порядке:";
  }
  QCString trInterfaceDocumentation() override {
    return "Документация интерфейса";
  }
  QCString trStructs() override { return "Структуры"; }
  QCString trStructIndex() override { return "Указатель Структур"; }
  QCString trStructList() override { return "Список Структур"; }
  QCString trStructListDescription() override {
    return "Здесь представлен список структур с их краткими описаниями:";
  }
  QCString trStructDocumentation() override { return "Документация структур"; }
  QCString trExceptionIndex() override { return "Указатель исключений"; }
  QCString trExceptionList() override { return "Список исключений"; }
  QCString trExceptionListDescription() override {
    return "Здесь представлен список исключений с их краткими описаниями:";
  }
  QCString trExceptionHierarchy() override { return "Иерархия Исключения"; }
  QCString trExceptionHierarchyDescription() override {
    return "Данный список наследований частично, но не полностью, отсортирован "
           "в алфавитном порядке:";
  }
  QCString trExceptionDocumentation() override {
    return "Документация исключения";
  }
  QCString trCompoundReferenceSlice(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isLocal) override {
    QCString result = "Ссылка на";
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
  QCString trOperations() override { return "Операции"; }
  QCString trOperationDocumentation() override {
    return "Документация операции";
  }
  QCString trDataMembers() override { return "Поля класса"; }
  QCString trDataMemberDocumentation() override {
    return "Документация для полей класса";
  }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.8.19
  //////////////////////////////////////////////////////////////////////////

  /** VHDL design unit documentation */
  QCString trDesignUnitDocumentation() override { return "Проектная единица"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.2
  //////////////////////////////////////////////////////////////////////////

  /** C++20 concept */
  QCString trConcept(bool first_capital, bool singular) override {
    return createNoun(first_capital, singular, "концепт", "ы");
  }
  /*! used as the title of the HTML page of a C++20 concept page */
  QCString trConceptReference(const QCString &conceptName) override {
    return "Ссылка на концепт " + conceptName;
  }

  /*! used as the title of page containing all the index of all concepts. */
  QCString trConceptList() override { return "Список концептов"; }

  /*! used as the title of chapter containing the index listing all concepts. */
  QCString trConceptIndex() override { return "Указатель концептов"; }

  /*! used as the title of chapter containing all information about concepts. */
  QCString trConceptDocumentation() override {
    return "Документация концептов";
  }

  /*! used as an introduction to the concept list */
  QCString trConceptListDescription(bool extractAll) override {
    QCString result = "Список всех  ";
    if (!extractAll)
      result += "задокументированных ";
    result += "концептов с их краткими описаниями:";
    return result;
  }

  /*! used to introduce the definition of the C++20 concept */
  QCString trConceptDefinition() override { return "Определение концепта"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.4
  //////////////////////////////////////////////////////////////////////////

  QCString trPackageList() override { return "Список пакетов"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.6
  //////////////////////////////////////////////////////////////////////////

  /*! This is used for translation of the word that will be
   *  followed by a single name of the VHDL process flowchart.
   */
  QCString trFlowchart() override { return "Блок-схема:"; }

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
  QCString trRelatedSymbols() override { return "Относящиеся к классу:"; }

  /*! subscript for the related symbols
   *
   *  Supersedes trRelatedSubscript
   */
  QCString trRelatedSymbolsSubscript() override { return "(не члены класса)"; }

  /*! used in the class documentation as a header before the list of all
   * related classes.
   *
   * Supersedes trRelatedFunctionDocumentation
   */
  QCString trRelatedSymbolDocumentation() override {
    return "Друзья класса и относящимся к классу обозначения";
  }

  /*! the compound type as used for the xrefitems */
  QCString trCompoundType(ClassDef::CompoundType compType,
                          SrcLangExt lang) override {
    QCString result;
    switch (compType) {
    case ClassDef::Class:
      if (lang == SrcLangExt::Fortran)
        trType(true, true);
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

  QCString
  trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    QCString result = "Список всех  ";
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
  QCString
  trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    QCString result = "Список всех  ";
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
  QCString trNamespaceMembersDescriptionTotal(
      NamespaceMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    QCString result = "Список всех ";
    if (!extractAll)
      result += "задокументированных ";
    QCString singularResult = "";
    QCString pluralResult = "";
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
  QCString trDefinition() override { return "Определения"; }
  QCString trDeclaration() override { return "Объявления"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.9.8
  //////////////////////////////////////////////////////////////////////////

  QCString trTopics() override { return "Разделы"; }
  QCString trTopicDocumentation() override { return "Документация разделов"; }
  QCString trTopicList() override { return "Список разделов"; }
  QCString trTopicIndex() override { return "Указатель разделов"; }
  QCString trTopicListDescription() override {
    return "Список всех разделов c краткими описаниями:";
  }
  QCString
  trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override {
    bool extractAll = Config_getBool(EXTRACT_ALL);
    QCString result = "Список всех ";
    if (!extractAll)
      result += "задокументированных ";
    QCString singularResult = "";
    QCString pluralResult = "";
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
  QCString trExportedModules() override { return "Экспортируемые модули"; }

  //////////////////////////////////////////////////////////////////////////
  // new since 1.10.0
  //////////////////////////////////////////////////////////////////////////

  QCString trCopyToClipboard() override { return "Скопировать в буфер обмена"; }
  //////////////////////////////////////////////////////////////////////////
  // new since 1.11.0
  //////////////////////////////////////////////////////////////////////////
  QCString trImportant() override { return "Важно!"; }
};

#endif
