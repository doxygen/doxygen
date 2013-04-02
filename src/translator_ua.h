/******************************************************************************
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

class TranslatorUkrainian : public TranslatorAdapter_1_8_2
{
  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage()
    { return "ukrainian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[T2A]{fontenc}\n\\usepackage[ukrainian]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
        return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Споріднені фукції"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(не методи компонент)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Детальний опис"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Опис типів користувача"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Опис переліків користувача"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Опис методів компонент"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Поля" ;
      }
      else
      {
        return  "Компонентні дані" ;
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Детальніше..."; }

    /*! put in the class documentation */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trListOfAllMembers()
    {
      return  "Список всіх елементів" ;
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trMemberList()
    {
      return  "Cписок елементів" ;
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Повний список елементів"; }

    /*! this is the remainder of the sentence after the class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers()
    { return ", включаючи всі успадковані елементи"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Автоматично створено за допомогою Doxygen";
      if (s) result+=QCString(" для ")+s;
      result+=" з тексту програми.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "Перелік"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "Елемент переліку"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "визначено в "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Програмні модулі"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Ієрархія класів"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Файли"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembers()
    {
      return  "Елементи файлу" ;
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return "Додаткова інформація"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Приклади"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Пошук"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Список успадкувань впорядковано наближено до алфавіту"; }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="файлів.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Структури даних з коротким описом." ;
      }
      else
      {
        return  "Класи, структури, об'єднання та інтерфейси  з коротким описом." ;
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
        QCString result="Список всіх ";
        if(!extractAll) result+="документоаних ";
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="елементів структур даних з посиланням на ";
        else
          result+="елементів класів даних з посиланням на ";
        if(extractAll)
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result+="документацію по структурі/об'єднанню кожного елементу.";
          else
            result+="документацію по класу кожного елементу.";
        }
        else
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result += "відповідні структури";
          else
            result += "відповідні класи";
          result+=", до яких вони належать.";
        }
        return  result ;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Список всіх ";
      if (!extractAll) result+="документованих ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Повний список прикладів."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Повний список додаткових описів."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Повний список модулів."; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Документація"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Алфавітний покажчик модулів"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Ієрархічний покажчик класів"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex()
    { return  "Покажчик файлв" ; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Програмні Модулі"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Файли"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Приклади"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Документація по темі"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Довідковий посібник"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Макровизначення"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Прототипи фукцій"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Визначення типів"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Переліки"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Функції"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Змінні"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Елементи переліків"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Опис макровизначень"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Опис прототипів функцій"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Опис визначень типів"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Опис переліків"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Опис функцій"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Опис змінних"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result="Документація ";
      if (projName) result+=QCString("до ")+projName;
      result+=QCString(" створена ")+date;
      result+=" системою";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "Автор:";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return QCString("Схема успадкувань для ")+clName;
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Тільки для внутрішнього користування"; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Застереження"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Версія"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Повертає"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Див. також"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Аргументи"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Обробка виняткових ситуацій"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Створено системою"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Простір імен"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="просторів імен.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Дружні класи"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
      { return "Документация до дружніх класів та відповідних функцій"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
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
    virtual QCString trFileReference(const char *fileName)
    {
      return QCString("Файл ")+fileName;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      return QCString("Простір імен ")+namespaceName;
    }

    virtual QCString trPublicMembers()
    { return "Загальнодоступні елементи"; }
    virtual QCString trPublicSlots()
    { return "Загальнодоступні слоти"; }
    virtual QCString trSignals()
    { return "Сигнали"; }
    virtual QCString trStaticPublicMembers()
    { return "Загальнодоступні статичні елементи"; }
    virtual QCString trProtectedMembers()
    { return "Захищені елементи"; }
    virtual QCString trProtectedSlots()
    { return "Захищені слоти"; }
    virtual QCString trStaticProtectedMembers()
    { return "Захищені статичні елементи"; }
    virtual QCString trPrivateMembers()
    { return "Приватні елементи"; }
    virtual QCString trPrivateSlots()
    { return "Приватні слоти"; }
    virtual QCString trStaticPrivateMembers()
    { return "Приватні статичні елементи"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
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
            result+= " і " ;
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Успадковує клас "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Успадковано класами "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Переозначення з "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Переозначається в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Елементи простору імен"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Алфавітний покажчик простору імен"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Опис простору імен"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return  "Простори імен" ; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Документація  ";
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

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Алфавітний покажчик"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Значення, що повертаються"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Титульна сторінка"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "стор."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Див. визначення в файлі @1, рядок @0";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Див. визначення в файлі @0";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Не рекомендовано";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Діаграма зв'язків класу "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return QCString("Діаграма включених заголовочних файлів для ")+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Конструктор(и)";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Див. вихідні тексти.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Див. документацію.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Передумови";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Післяумови";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Інваріант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Початкові значення";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "Вихідні тексти";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Графічна ієрархія класів";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "див. графічну ієрархію";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "див. текстову ієрархію";
    }
    virtual QCString trPageIndex()
    {
      return "Алфавітний покажчик тематичних описів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Нотатки";
    }
    virtual QCString trPublicTypes()
    {
      return "Загальнодоступні типи";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Поля даних" ;
      }
      else
      {
        return  "Загальнодоступні атрибути" ;
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Статичні загальнодоступні данні";
    }
    virtual QCString trProtectedTypes()
    {
      return "Захищені типи";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Захищені дані";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Статичні захищені дані";
    }
    virtual QCString trPrivateTypes()
    {
      return "Приватні типи";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Приватні дані";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Приватні статичні дані";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    /*??*/
    {
      return "Необхідно зробити";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    /*??*/
    {
      return "Список завдань";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Вживається в";
    }
    virtual QCString trRemarks()
    {
      return "Прим.";
    }
    virtual QCString trAttention()
    {
      return "Увага";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Граф файлів, які включають цей файл:";
    }
    virtual QCString trSince()
    /*??*/
    {
      return "Починаючи з";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Легенда";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "див. легенду";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Тест";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Список тестів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Методи";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Властивості";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Повний список властивостей";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return  "Структури даних" ;
      }
      else
      {
        return  "Класи" ;
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return QCString("Пакет ")+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Повний список пакетів";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Повний список документованих пакетів.";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Пакети";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Макровизначення:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Дефект";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Список дефектів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1251";
    }
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "204";
    }
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Предметний покажчик";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        QCString result((first_capital ? "Структури даних" : "структури даних"));
        return result;
      }
      else
      {
        QCString result((first_capital ? "Клас" : "клас"));
        if(!singular) result+="и";
        return result;
      }
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Файл" : "файл"));
      if (!singular)  result+="и";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Прост" : "прост"));
      result+=(singular?"ір імен":"ори імен");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Груп" : "груп"));
      result+=(singular ? "а" : "и");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Сторінк" : "сторінк"));
      result+=(singular ? "а" : "и");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Елемент" : "елемент"));
      if (!singular)  result+="и";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Глобальн" : "глобальн"));
      result+=(singular ? "ий" : "і");
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Автор" : "автор"));
      if (!singular) result+="и";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Використовує";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Реалізує " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Реалізує в " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Зміст";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Застарілі елементи";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Події";
    }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Документація подій";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Типи пакетів";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Функції пакетів";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Статичні функцію пакетів";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Атрибути пакетів";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Статичні атрибути пакетів";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Всі";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Граф всіх викликів цієї функції:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the header
     *  of each page before the field where one can enter the text to search
     *  for.
     */
    virtual QCString trSearchForIndex()
    {
      return "Шукати";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Відповідність:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Текст програми "+filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Дерево каталогів"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Документація каталогів"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Каталоги"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Дерево каталогів впорядковано наближено "
	     "до алфавіту:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Довідник каталогу "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool singular)
    {
      QCString result("Каталог");
      if (!singular) result+="и";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "Це перезавантажуваний метод, "
              "ввкдкний для зручності. Він відрізняється від "
              "описаної вище функції лише списком аргуметнів.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Граф викликів для цієї функції:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Документація переліку"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Документація функції/підпрограми"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Список типів даних"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Поля даних"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Типи даних з коротким описом:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Список типів даних"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Документація типів даних"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Функції/підпрограми"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Документація функцій/підпрограм"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Типи даних"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Список модулів"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Список усіх ";
      if (!extractAll) result+="задокументованих ";
      result+="модулів з коротким описом:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
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
	if (isTemplate) result+=" Шаблон ";
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
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Модуль";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Елементи модуля"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Алфавітний покажчик модулів"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
    	    QCString result((first_capital ? "Модул" : "модул"));
    	    result+=(singular? "ь": "і");
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Документацію для ";
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
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Тип" : "тип"));
      if (!singular)  result+="и";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Підпрограм" : "підпрограм"));
      result+= (singular? "а": "и");
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Обмеження типу";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Зв'язок з "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Завантаження...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Глобальний простір імен";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Пошук...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Нічого не знайдено";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Граф залежностей каталогу для "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Файл у "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Включає файли в "+name;
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
                                bool includeTime)
    {
      static const char *days[]   = { "Понеділок","Вівторок","Середа","Четвер","П'ятниця","Субота","Неділя" };
      static const char *months[] = { "січня","лютого","березня","квітня","травня","червня","липня","серпня","вересня","жотня","листопада","грудня" };
      QCString sdate;
      sdate.sprintf("%s, %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Бібліографічні посилання"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Копірайт"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Граф залежностей каталогу для ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "рівень елемента"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Параметри шаблона"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "і ще  "+number+"..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Документацію для цього переліку булозгенеровано з ";
    	    result += (single? "наступного файла": "наступних файлів");
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Перелік"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" успадковано з "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Додаткові успадковані елементи"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "дозволити" : "заборонити";
      return "клацніть мишкою, щоб "+opt+" синхронізацію панелі";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Забезпечено категорією @1.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Розширює клас @1.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Методи класу";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Методи об'єкта";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Документація метода";
    }

    /*! Used as the title of the design overview picture created for the
     *  VHDL output.
     */
    virtual QCString trDesignOverview()
    {
      return "Огляд дизайну проекту";
    }

//////////////////////////////////////////////////////////////////////////

};

#endif

