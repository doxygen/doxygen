/******************************************************************************
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
 * The translation into Ukrainian was provided by
 *   Olexij Tkatchenko (olexij.tkatchenko@parcs.de)
 *   Petro  Yermolenko (python@i.ua)
 */

#ifndef TRANSLATOR_UA_H
#define TRANSLATOR_UA_H

class TranslatorUkrainian : public TranslatorAdapter_1_8_4
{
  public:
    /*! Used for identification of the language. */
    QCString idLanguage() override
    { return "ukrainian"; }

    /* Used to get the command(s) for the language support. */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[T2A]{fontenc}\n\\usepackage[ukrainian]{babel}\n";
    }
    QCString trISOLang() override
    {
      return "uk";
    }
    QCString getLanguageString() override
    {
      return "0x422 Ukrainian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Споріднені фукції"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(не методи компонент)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Детальний опис"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Подробиці"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Опис типів користувача"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Опис переліків користувача"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Опис методів компонент"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Поля" ;
      }
      else
      {
        return  "Компонентні дані" ;
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Детальніше..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    QCString trListOfAllMembers() override
    {
      return  "Список всіх елементів" ;
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    QCString trMemberList() override
    {
      return  "Cписок елементів" ;
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    QCString trThisIsTheListOfAllMembers() override
    { return "Повний список елементів"; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    QCString trIncludingInheritedMembers() override
    { return ", включаючи всі успадковані елементи"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Автоматично створено за допомогою Doxygen";
      if (!s.isEmpty()) result+=QCString(" для ")+s;
      result+=" з тексту програми.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "Перелік"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "Елемент переліку"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "визначено в "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Програмні модулі"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Ієрархія класів"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Файли"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Поля структур" ;
      }
      else
      {
        return  "Елементи класів" ;
      }
    }

    /*! This is put above each page as a link to all members of files. */
    /*??*/
    QCString trFileMembers() override
    {
      return  "Елементи файлу" ;
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return "Додаткова інформація"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Приклади"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Пошук"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Список успадкувань впорядковано наближено до алфавіту"; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="файлів.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Структури даних з коротким описом." ;
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return  "Класи з коротким описом." ;
      }
      else
      {
        return  "Класи, структури, об'єднання та інтерфейси  з коротким описом." ;
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
        QCString result="Список всіх ";
        if(!extractAll) result+="документоаних ";
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result+="елементів структур даних з посиланням на ";
        else
          result+="елементів класів даних з посиланням на ";
        if(extractAll)
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result+="документацію по структурі/об'єднанню кожного елементу.";
          else
            result+="документацію по класу кожного елементу.";
        }
        else
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result += "відповідні структури";
          else
            result += "відповідні класи";
          result+=", до яких вони належать.";
        }
        return  result ;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Список всіх ";
      if (!extractAll) result+="документованих ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="функцій, змінних, макровизначень, "
                "переліків і визначень типів";
      }
      else
      {
        result+="елементів файлів ";
      }
      result+=" з посиланям на ";
      if (extractAll)
        result+="файли, до яких вони належать.";
      else
        result+="документацію.";
      return  result ;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Повний список прикладів."; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Повний список додаткових описів."; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Повний список модулів."; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Документація"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Алфавітний покажчик модулів"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Ієрархічний покажчик класів"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Алфавітний покажчик структур даних" ;
      }
      else
      {
        return  "Алфавітний покажчик класів" ;
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return  "Покажчик файлв" ; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Програмні Модулі"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Структури даних" ;
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return  "Класи" ;
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Файли"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Довідковий посібник"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Макровизначення"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Визначення типів"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Переліки"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Функції"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Змінні"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Елементи переліків"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Опис макровизначень"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Опис визначень типів"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Опис переліків"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Опис функцій"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Опис змінних"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
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
      QCString result="Документація ";
      if (!projName.isEmpty()) result+=QCString("до ")+projName;
      result+=QCString(" створена ")+date;
      result+=" системою";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("Схема успадкувань для ")+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Застереження"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Версія"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Повертає"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Див. також"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Аргументи"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Обробка виняткових ситуацій"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Створено системою"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Простір імен"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="просторів імен.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Дружні класи"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
      { return "Документация до дружніх класів та відповідних функцій"; }

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
          case ClassDef::Class:  result+="класу"; break;
          case ClassDef::Struct: result+="структури"; break;
          case ClassDef::Union:  result+="об'єднання"; break;
          case ClassDef::Interface:  result+="інтерфейсу"; break;
          case ClassDef::Protocol:   result+="протоколу"; break;
          case ClassDef::Category:   result+="категорії"; break;
          case ClassDef::Exception:  result+="Виняток"; break;
          default: break;
        }
      }
      else
      {
        switch(compType)
        {
          case ClassDef::Class:  result+="Клас"; break;
          case ClassDef::Struct: result+="Структура"; break;
          case ClassDef::Union:  result+="Об'єднання"; break;
          case ClassDef::Interface:  result+="Інтерфейс"; break;
          case ClassDef::Protocol:   result+="Протокол"; break;
          case ClassDef::Category:   result+="Категорія"; break;
          case ClassDef::Exception:  result+="Виняток"; break;
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
      return QCString("Простір імен ")+namespaceName;
    }

    QCString trPublicMembers() override
    { return "Загальнодоступні елементи"; }
    QCString trPublicSlots() override
    { return "Загальнодоступні слоти"; }
    QCString trSignals() override
    { return "Сигнали"; }
    QCString trStaticPublicMembers() override
    { return "Загальнодоступні статичні елементи"; }
    QCString trProtectedMembers() override
    { return "Захищені елементи"; }
    QCString trProtectedSlots() override
    { return "Захищені слоти"; }
    QCString trStaticProtectedMembers() override
    { return "Захищені статичні елементи"; }
    QCString trPrivateMembers() override
    { return "Приватні елементи"; }
    QCString trPrivateSlots() override
    { return "Приватні слоти"; }
    QCString trStaticPrivateMembers() override
    { return "Приватні статичні елементи"; }

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
            result+= " і " ;
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Успадковує клас "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Успадковано класами "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Переозначення з "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Переозначається в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Елементи простору імен"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="елеметів простору імен з посиланням ";
      if (extractAll)
        result+="на документацію для кожного елементу:";
      else
        result+="на простір імен до якого вони належать:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Алфавітний покажчик простору імен"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Опис простору імен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return  "Простори імен" ; }

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
      QCString result="Документація  ";
      switch(compType)
      {
        case ClassDef::Class:
          if (single) result+="цього класу"; else result+="цих класів";
          break;
        case ClassDef::Struct:
          if (single) result+="цієї структури"; else result+="цих структур";
          break;
        case ClassDef::Union:
          if (single) result+="цього об'єднання"; else result+="цих об'єднань";
          break;
        case ClassDef::Interface:
          if (single) result+="цього інтерфейсу"; else result+="цих інтерфейсів";
          break;
        case ClassDef::Protocol:
          if (single) result+="цього протоколу"; else result+="цих протоколів";
          break;
        case ClassDef::Category:
          if (single) result+="цієї категорії"; else result+="цих категорій";
          break;
        case ClassDef::Exception:
          if (single) result+="цього винятку"; else result+="цих винятків";
          break;
        default:
          break;
      }
      result+=" була створена з файл";
      if (single) result+="у:"; else result+="ів:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Значення, що повертаються"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Титульна сторінка"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "стор."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Див. визначення в файлі @1, рядок @0";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Див. визначення в файлі @0";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Не рекомендовано";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Діаграма зв'язків класу "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return QCString("Діаграма включених заголовочних файлів для ")+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Конструктор(и)";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Див. вихідні тексти.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Див. документацію.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Передумови";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Післяумови";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Інваріант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Початкові значення";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "Вихідні тексти";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Графічна ієрархія класів";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "див. графічну ієрархію";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "див. текстову ієрархію";
    }
    QCString trPageIndex() override
    {
      return "Алфавітний покажчик тематичних описів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Нотатки";
    }
    QCString trPublicTypes() override
    {
      return "Загальнодоступні типи";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Поля даних" ;
      }
      else
      {
        return  "Загальнодоступні атрибути" ;
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Статичні загальнодоступні данні";
    }
    QCString trProtectedTypes() override
    {
      return "Захищені типи";
    }
    QCString trProtectedAttribs() override
    {
      return "Захищені дані";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Статичні захищені дані";
    }
    QCString trPrivateTypes() override
    {
      return "Приватні типи";
    }
    QCString trPrivateAttribs() override
    {
      return "Приватні дані";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Приватні статичні дані";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    /*??*/
    {
      return "Необхідно зробити";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    /*??*/
    {
      return "Список завдань";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Вживається в";
    }
    QCString trRemarks() override
    {
      return "Прим.";
    }
    QCString trAttention() override
    {
      return "Увага";
    }
    QCString trInclByDepGraph() override
    {
      return "Граф файлів, які включають цей файл:";
    }
    QCString trSince() override
    /*??*/
    {
      return "Починаючи з";
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
        "Позначення, що використовуються у графах.<p>\n"
        "Розглянемо наступний приклад:\n"
        "\\code\n"
        "/*! Невидимий клас через зрізання */\n"
        "class Invisible { };\n\n"
        "/*! Обмежений клас, відношення успадкування приховане */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Недокументований клас */\n"
        "class Undocumented { };\n\n"
        "/*! Загальнодоступне успадкування */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Захищене успадкування */\n"
        "class ProtectedBase { };\n\n"
        "/*! Приватне успадкування */\n"
        "class PrivateBase { };\n\n"
        "/*! Клас, що використовується класом Inherited */\n"
        "class Used { };\n\n"
        "/*! Клас, що успадковує інші класи */\n"
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
        "Таким чином, отримуємо наступний граф:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Прямокутники в цьому графі мають наступний зміст:\n"
        "<ul>\n"
        "<li>Заповнений чорний прямокутник відображає структуру або клас, "
        "для якого створено граф.\n"
        "<li>Прямокутник з чорною межею відображає документовану структуру або клас.\n"
        "<li>Прямокутник з сірою межею відображає недокументовану структуру або клас.\n"
        "<li>Прямокутник з червоною межею відображає документовану структуру або клас, для яких\n"
        " не всі співвідношення успадкування/змісту показані. Граф зрізаниЙ, "
        "якщо він не вміщається у вказані межі."
        "</ul>\n"
        "Стрілки мають наступний зміст:\n"
        "<ul>\n"
        "<li>Темносиня стрілка відображає відношення загальнодоступного успадкування "
        "між двома класами.\n"
        "<li>Темнозелена стрілка використовується при захищеному успадкуванні.\n"
        "<li>Темночервона стрілка використовується при приватному успадкуванні.\n"
        "<li>Пурпурна стрілка використовується, якщо клас міститься в"
        "іншому класі або ним використовується."
        "Стрілка етикетується змінною, "
        "через яку відбувається доступ до вказаної структури або класу. \n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "див. легенду";
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
      return "Список тестів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Властивості";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Повний список властивостей";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
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
      return "Повний список документованих пакетів.";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Пакети";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Макровизначення:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Дефект";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Список дефектів";
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
      return "Предметний покажчик";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        QCString result((first_capital ? "Структури даних" : "структури даних"));
        return result;
      }
      else
      {
      return createNoun(first_capital, singular, "клас", "и");
      }
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "файл", "и");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "прост", "ори імен", "ір імен");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "груп", "и", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "сторінк", "и", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "елемент", "и");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "глобальн", "і", "ий");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "автор", "и");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Використовує";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Реалізує " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Реалізує в " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Зміст";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Застарілі елементи";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Події";
    }

    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Документація подій";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Типи пакетів";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Функції пакетів";
    }
    QCString trPackageMembers() override
    {
      return "Елементи пакетів";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Статичні функцію пакетів";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Атрибути пакетів";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Статичні атрибути пакетів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Всі";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Граф всіх викликів цієї функції:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Результати пошуку";
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
        return "Не знайдено документів відповідно до вашого запиту.";
      }
      else if (numDocuments==1)
      {
        return "Було знайдено <b>1</b> документ відповідно до вашого запиту.";
      }
      else
      {
        return "Було знайдено <b>$num</b> документів відповідно до вашого запиту. "
	       "Найкращі відповідності показано спочатку.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Відповідність:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Текст програми "+filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Дерево каталогів"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Документація каталогів"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Каталоги"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Довідник каталогу "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "каталог", "и");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Це перезавантажуваний метод, "
              "ввкдкний для зручності. Він відрізняється від "
              "описаної вище функції лише списком аргуметнів.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Граф викликів для цієї функції:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Документація переліку"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Документація функції/підпрограми"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Список типів даних"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Поля даних"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Типи даних з коротким описом:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Список усіх ";
      if (!extractAll)
      {
        result+="задокументованих ";
      }
      result+="елементів типів даних";
      result+=" з посиланнями на ";
      if (!extractAll)
      {
         result+="документацію структури даних кожного елемента";
      }
      else
      {
         result+="типи даних, до яких вони належать:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Список типів даних"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Документація типів даних"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Функції/підпрограми"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Документація функцій/підпрограм"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Типи даних"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Список модулів"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Список усіх ";
      if (!extractAll) result+="задокументованих ";
      result+="модулів з коротким описом:";
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
	  case ClassDef::Union:      result+=" Об'єднання"; break;
	  case ClassDef::Interface:  result+=" Інтерфейс"; break;
	  case ClassDef::Protocol:   result+=" Протокол"; break;
	  case ClassDef::Category:   result+=" Категорія"; break;
	  case ClassDef::Exception:  result+=" Виняток"; break;
          default: break;
	}
      }
      else
      {
	result+=" Шаблон ";
	switch(compType)
	{
	  case ClassDef::Class:      result+="модуля"; break;
	  case ClassDef::Struct:     result+="типу"; break;
	  case ClassDef::Union:      result+="об'єднання"; break;
	  case ClassDef::Interface:  result+="інтерфейсу"; break;
	  case ClassDef::Protocol:   result+="протоколу"; break;
	  case ClassDef::Category:   result+="категорії"; break;
	  case ClassDef::Exception:  result+="винятка"; break;
          default: break;
	}
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Модуль";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Елементи модуля"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Список усіх ";
      if (!extractAll) result+="задокументованих ";
      result+="елементів модулів з посиланнями на ";
      if (extractAll)
      {
        result+="документацію модуля для кожного елемента:";
      }
      else
      {
        result+="модулі, до яких вони належать:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Алфавітний покажчик модулів"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "модул", "і", "ь");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Документацію для ";
      switch(compType)
      {
        case ClassDef::Class:      result+="цього модуля"; break;
        case ClassDef::Struct:     result+="цього типу"; break;
        case ClassDef::Union:      result+="цього об'єднання"; break;
        case ClassDef::Interface:  result+="цього інтерфейсу"; break;
        case ClassDef::Protocol:   result+="цього протоколу"; break;
        case ClassDef::Category:   result+="цієї категорії"; break;
        case ClassDef::Exception:  result+="цього винятку"; break;
        default: break;
      }
      result+=" було згенеровано з ";
      if (single) result+="наступного файлу:"; else result+="наступних файлів:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "тип", "и");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "підпрограм", "и", "а");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Обмеження типу";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Зв'язок з "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Завантаження...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Глобальний простір імен";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Пошук...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Нічого не знайдено";
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
      return "Файл у "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Включає файли в "+name;
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
      static const char *days[]   = { "Понеділок","Вівторок","Середа","Четвер","П'ятниця","Субота","Неділя" };
      static const char *months[] = { "січня","лютого","березня","квітня","травня","червня","липня","серпня","вересня","жотня","листопада","грудня" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
      static const char *days_short[]   = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Нд" };
      static const char *days_full[]    = { "понеділок", "вівторок", "середа", "четвер", "п'ятниця", "субота", "неділя" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Січ", "Лют", "Бер", "Кві", "Тра", "Чер", "Лип", "Сер", "Вер", "Жов", "Лис", "Гру" };
      static const char *months_full[]  = { "січень", "лютий", "березень", "квітень", "травень", "червень", "липень", "серпень", "вересень", "жовтень", "листопад", "грудень" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "дп", "пп" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Бібліографічні посилання"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Копірайт"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Граф залежностей каталогу для ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "рівень елемента"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Параметри шаблона"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "і ще  "+number+"..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Документацію для цього переліку булозгенеровано з ";
    	    result += (single? "наступного файла": "наступних файлів");
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Перелік"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" успадковано з "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Додаткові успадковані елементи"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "дозволити" : "заборонити";
      return "клацніть мишкою, щоб "+opt+" синхронізацію панелі";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Забезпечено категорією @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Розширює клас @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Методи класу";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Методи об'єкта";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Документація метода";
    }
};

#endif

