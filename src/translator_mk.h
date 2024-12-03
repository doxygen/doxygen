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
// Тranslated by Slave Jovanovski <slavejovanovski@yahoo.com>
//
// The cyrillic strings were entered using Macedonian language support in
// Windows. The editor used was Eclipse 3.2. The file was saved in UTF-8.
//
// Updates:
// --------
// 2007/12/09
//  - Initial translation to Macedonian.
//
// 2008/05/22
//	- Translation for 1.5.4.
//
//

#ifndef TRANSLATOR_MK_H
#define TRANSLATOR_MK_H

class TranslatorMacedonian : public TranslatorAdapter_1_6_0
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
    { return "macedonian"; }

    /*! Used to get the LaTeX command(s) for the language support.
     *  This method should return string with commands that switch
     *  LaTeX to the desired language.  For example
     *  <pre>"\\usepackage[german]{babel}\n"
     *  </pre>
     *  or
     *  <pre>"\\usepackage{polski}\n"
     *  "\\usepackage[latin2]{inputenc}\n"
     *  "\\usepackage[T1]{fontenc}\n"
     *  </pre>
     *
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[macedonian]{babel}\n";
    }
    QCString trISOLang() override
    {
      return "mk";
    }
    QCString getLanguageString() override
    {
      return "0x042f Macedonian (Former Yugoslav Republic of Macedonia)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Поврзани Функции"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Овие функции не се членови.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Детален опис"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Детали"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Документација на членови дефиниции на тип"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Документација на набројани членови"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Документација на функции членови"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      //if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Документација на членови";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Повеќе..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Список на сите членови"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Список на членови"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Ова е целосниот список на членови на"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", вклучувајќи ги сите наследени членови."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Автоматски создадено од Doxygen";
      if (!s.isEmpty()) result+=" за "+s;
      result+=" изворниот код.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "име на набројан член"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "вредност на набројан член"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "дефиниран во"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Модули"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Стебло на класи"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Список на Структури";
      }
      else
      {
        return "Список на Класи";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Список на Датотеки"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Членови";
      }
      else
      {
        return "Членови на Класата";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Глобални Членови";
      }
      else
      {
        return "Членови на Датотеката";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Страници Поврзани со Оваа"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Примери"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Пребарај"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Овој список на наследување е приближно азбучно подреден:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="членови со кратки описи:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Список на структури со кратки описи:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Список на класи со кратки описи:";
      }
      else
      {
        return "Список на класи, структури, унии и интерфејси "
               "со кратки описи:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll)
      {
        result+="документирани ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="структури и унии";
      }
      else
      {
        result+="членови на класата";
      }
      result+=" со врски до ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="документацијата за секој член на структурата/унијата:";
        }
        else
        {
          result+="документацијата на секој член на класата:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="структурите/униите на кои што припаѓаат:";
        }
        else
        {
          result+="класите на кои што припаѓаат:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="функции, променливи, дефиниции, набројувања и дефиниции на тип";
      }
      else
      {
        result+="членови на датотеки";
      }
      result+=" со врски до ";
      if (extractAll)
        result+="датотеките на кои што припаѓаат:";
      else
        result+="документацијата:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Список на сите примери:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Список на сите страници поврзани со оваа и нивна документација:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Список на сите модули:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Документација"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Попис на Модули"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Попис на Стебло"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Попис на Структури";
      }
      else
      {
        return "Попис на Класи";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Попис на Датотеки"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Попис на Документација"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Документација на Структури";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Документација на Класи";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Документија на Датотеки"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Прирачник"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Дефинирања"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Дефиниции на Тип"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Набројувања"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Променливи"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Вредности на Набројувањата"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Документција на Дефиниции"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Документација на Дефиниции на Тип"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Документација на Набројувања"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Документација на Функции"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Документација на Променливи"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури";
      }
      else
      {
        return "Класи";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Создадено на "+date;
      if (!projName.isEmpty()) result+=" за "+projName;
      result+=" од";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Диаграм на наследување за "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Предупредување"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Верзија"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Враќа"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Истотака погледнете"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Параметри"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Исклучоци"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Создадено од"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Список на Имиња на Простор"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="имиња на простор со кратки описи:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Пријатели"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Документација на Пријатели и Поврзани Функции"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Класа"; break;
        case ClassDef::Struct:     result+=" Структура"; break;
        case ClassDef::Union:      result+=" Унија"; break;
        case ClassDef::Interface:  result+=" Интерфејс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категорија"; break;
        case ClassDef::Exception:  result+=" Исклучок"; break;
        default: break;
      }
      if (isTemplate) result+=" Образец";
      result+=" Повикување";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result = "Опис на Датотекaта ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result = "Опис на Имeто на Простор ";
      result += namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Јавни Функции Членови"; }
    QCString trPublicSlots() override
    { return "Јавни Слотови"; }
    QCString trSignals() override
    { return "Сигнали"; }
    QCString trStaticPublicMembers() override
    { return "Статични Јавни Функции Членови"; }
    QCString trProtectedMembers() override
    { return "Заштитени Функции Членови"; }
    QCString trProtectedSlots() override
    { return "Заштитени Слотови"; }
    QCString trStaticProtectedMembers() override
    { return "Статични Заштитени Функции Членови"; }
    QCString trPrivateMembers() override
    { return "Приватни Функции Членови"; }
    QCString trPrivateSlots() override
    { return "Приватни Слотови"; }
    QCString trStaticPrivateMembers() override
    { return "Статични Приватни Функции Членови"; }

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
      return "Наследува "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Наследено од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Преприменето од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Преприменето во "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Членови на Име на Простор"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="членови на името на простор со врски до ";
      if (extractAll)
        result+="документацијата на секој член:";
      else
        result+="името на простор на кое што му припаѓаат:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Попис на Имиња на Простор"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Документација на Имиња на Простор"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Имиња на Простор"; }

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
      QCString result="Документацијата за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="оваа класа"; break;
        case ClassDef::Struct:     result+="оваа структура"; break;
        case ClassDef::Union:      result+="оваа унија"; break;
        case ClassDef::Interface:  result+="овој интерфејс"; break;
        case ClassDef::Protocol:   result+="овој протокол"; break;
        case ClassDef::Category:   result+="оваа категорија"; break;
        case ClassDef::Exception:  result+="овој исклучок"; break;
        default: break;
      }
      result+=" беше создадена од ";
      if (single) result+="следнава датотека:"; else result+="следниве датотеки:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Вратена Вредност"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Главна Страна"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Дефиницијата е на линија @0 во датотека @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Дефинијата е во датотека @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Застарено";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Диаграм на соработка за "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Вклучен дијаграм на зависност за "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Документација на Конструктор и Деструктор";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Оди до изворниот код на оваа датотека.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Оди до документацијата на оваа датотека.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Предуслов";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Постуслов";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Инваријанта";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Основна вредност:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "код";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Графичко Стебло на Класи";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Оди до графичкото стебло на класи";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Оди до текстуалното стебло на класи";
    }
    QCString trPageIndex() override
    {
      return "Попис на Страници";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Забелешка";
    }
    QCString trPublicTypes() override
    {
      return "Јавни Типови";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Членови";
      }
      else
      {
        return "Јавни Членови";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Статични Јавни Членови";
    }
    QCString trProtectedTypes() override
    {
      return "Заштитени Типови";
    }
    QCString trProtectedAttribs() override
    {
      return "Заштитени Членови";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Статични Заштитени Типови";
    }
    QCString trPrivateTypes() override
    {
      return "Приватни Типови";
    }
    QCString trPrivateAttribs() override
    {
      return "Приватни Членови";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Статични Приватни Членови";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Незавршено";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Список на Незавршени";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Повикано од";
    }
    QCString trRemarks() override
    {
      return "Забелешки";
    }
    QCString trAttention() override
    {
      return "Внимание";
    }
    QCString trInclByDepGraph() override
    {
      return "Овој дијаграм покажува кои датотеки се "
             "директно или индеректно вклучени во оваа датотека:";
    }
    QCString trSince() override
    {
      return "Од";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Легенда на Дијаграмот";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Оваа страна објаснува како да ги толкувате дијаграмите создадени"
        " од doxygen.<p>\n"
        "На пример:\n"
        "\\code\n"
        "/*! Невидлива класа заради ограничувања на дијаграмот */\n"
        "class Invisible { };\n\n"
        "/*! Одсечена класа, врската на наследување е скриена */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Класата не е документирана со doxygen коментари */\n"
        "class Undocumented { };\n\n"
        "/*! Класа која е наследена со јавно наследување */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Класа Образец */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Класа која е наследена со заштитено наследување */\n"
        "class ProtectedBase { };\n\n"
        "/*! Класа која е наследена со приватно наследување */\n"
        "class PrivateBase { };\n\n"
        "/*! Класа која е користена од страна на наследената класа */\n"
        "class Used { };\n\n"
        "/*! Класа која наследува од повеќе други класи */\n"
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
        "Ако вредноста на \\c MAX_DOT_GRAPH_HEIGHT во конфигурациската датотека "
        "е 240 тогаш примерот ќе го создаде следниов дијаграм:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Правоаголниците во дијаграмот погоре го имаат следново значење:\n"
        "<ul>\n"
        "<li>Полн сив правоаголник ја означува структурата или класата за "
        "којадијаграмот е создаден.\n"
        "<li>Правоаголник со црна граница означува документирана структура или класа.\n"
        "<li>Правоаголник со сива граница означува недокументирана структура или класа.\n"
        "<li>Правоаголник со црвена граница означува документирана структура или класа "
        "за која не сите врски на наследување се прикажани. Дијаграмот е одсечен доколку "
        "не го собира во зададените граници..\n"
        "</ul>\n"
        "Стрелките го имаат следново значење:\n"
        "<ul>\n"
        "<li>Сина стрелка означува врска на јавно наследување помеѓу две класи.\n"
        "<li>Зелена стрелка означува заштитено наследување.\n"
        "<li>Црвена стрелка означува приватно наследување.\n"
        "<li>Виолетова испрекината линија означува класа која е користена или вклучена "
        "во друга класа. Стрелаката е означена со името на променливата преку "
        "која класата е достапна.\n"
        "<li>Жолта испрекината линија означува врска меѓу примерок од образец класа "
        "и класата образец од која е создадена. Стрелката е означена со имињата на "
        "параметрите на класата.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "Легенда";
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
      return "Листа на Тестови";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Особини";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Документација на Особини";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури";
      }
      else
      {
        return "Класи";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Пакет "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Пакети со краток опис (доколку достапен):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Пакети";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Вредност:";
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
      return "Листа на Дефекти";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file
     *
     * The following table shows the correlation of Charset name, Charset Value and
     * <pre>
     * Codepage number:
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * DEFAULT_CHARSET           1 (x01)
     * SYMBOL_CHARSET            2 (x02)
     * OEM_CHARSET             255 (xFF)
     * ANSI_CHARSET              0 (x00)            1252
     * RUSSIAN_CHARSET         204 (xCC)            1251
     * EE_CHARSET              238 (xEE)            1250
     * GREEK_CHARSET           161 (xA1)            1253
     * TURKISH_CHARSET         162 (xA2)            1254
     * BALTIC_CHARSET          186 (xBA)            1257
     * HEBREW_CHARSET          177 (xB1)            1255
     * ARABIC _CHARSET         178 (xB2)            1256
     * SHIFTJIS_CHARSET        128 (x80)             932
     * HANGEUL_CHARSET         129 (x81)             949
     * GB2313_CHARSET          134 (x86)             936
     * CHINESEBIG5_CHARSET     136 (x88)             950
     * </pre>
     *
     */
    QCString trRTFansicp() override
    {
      return "1251";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "204";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Попис";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "клас", "и", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "датотек", "и", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "им", "иња на простори", "е на простор");
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
      return createNoun(first_capital, singular, "стран", "и", "а");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "член", "ови");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "глобал", "ни", "ен");
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
      return "Повикувања";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Применува "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Применето во "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Содржина";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Список на Застарени";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Настани";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Документација на Настани";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Типови во Пакетот";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Функции во Пакетот";
    }
    QCString trPackageMembers() override
    {
      return "Членови во Пакетот";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Статични Функции во Пакетот";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Членови во Пакетот";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Статични Членови во Пакетот";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Сите";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Дијаграм на повикувања за оваа функција:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Резултати од Пребарувањето";
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
        return "Нема документи кои се поклопуваат со вашето барање.";
      }
      else if (numDocuments==1)
      {
        return "Пронајден <b>1</b> документ кој се поклопува со вашето барање.";
      }
      else
      {
        return "Пронајдени <b>$num</b> документи кои се поклопуваат со вашето барање."
               "Најдобро поклопените документи се прикажани први.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Пронајдени:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Изворен код на датотеката " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Стебло на Именикот"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Документација на Именикот"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Именици"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result= "Опис на Именикот "; result += dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "имени", "ци", "к");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Оваа е преоптоварена фунција. Се разликува по "
              "типот и бројот на аргументи што ги прифаќа.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Ова е графот на повикување за оваа фунција:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Документија на Набројувања"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Документација на функции/процедури членови"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Список на Типови"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Членови"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Список на типови со кратки описи:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll)
      {
        result+="документирани ";
      }
      result+="членови на типови";
      result+=" со врски до ";
      if (!extractAll)
      {
         result+="документацијата за секој член:";
      }
      else
      {
         result+="типовите на кои што припаѓаат:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Попис на Типови"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Документација на Типови"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Функции/Процедури"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Документација на Функции/Процедури"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Типови"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Попис на Модули"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="модули со кратки описи:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName + " - Повикување на";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Класа"; break;
        case ClassDef::Struct:     result+=" Тип"; break;
        case ClassDef::Union:      result+=" Унија"; break;
        case ClassDef::Interface:  result+=" Интерфејс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категорија"; break;
        case ClassDef::Exception:  result+=" Исклучок"; break;
        default: break;
      }
      if (isTemplate) result+=" Образец";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" - Содржина на Модул";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Членови на Модул"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="членови на модулот со врски до ";
      if (extractAll)
      {
        result+="документацијата за секој член:";
      }
      else
      {
        result+="модулите на кои што припаѓаат:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Попис на Модули"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "модул", "и");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Документацијата за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="оваа класа"; break;
        case ClassDef::Struct:     result+="овој тип"; break;
        case ClassDef::Union:      result+="оваа унија"; break;
        case ClassDef::Interface:  result+="овој интерфејс"; break;
        case ClassDef::Protocol:   result+="овој протокол"; break;
        case ClassDef::Category:   result+="оваа категорија"; break;
        case ClassDef::Exception:  result+="овој исклучок"; break;
        default: break;
      }
      result+=" беше создадена од ";
      if (single) result+="следнава датотека:"; else result+="следниве датотеки:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "тип", "ови");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "потпрограм", "и", "а");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Ограничувања на Тип";
    }

    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "пон.", "вт.", "сре.", "чет.", "пет.", "саб.", "нед." };
      static const char *days_full[]    = { "понеделник", "вторник", "среда", "четврток", "петок", "сабота", "недела" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "јан.", "фев.", "мар.", "апр.", "мај", "јун.", "јул.", "авг.", "септ.", "окт.", "ноем.", "дек." };
      static const char *months_full[]  = { "јануари", "февруари", "март", "април", "мај", "јуни", "јули", "август", "септември", "октомври", "ноември", "декември" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "претпл.", "попл." };
      return dayPeriod[period?1:0];
    }
};

#endif
