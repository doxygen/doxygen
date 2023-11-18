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

#ifndef TRANSLATOR_BG_H
#define TRANSLATOR_BG_H

/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/

/*
 * Kiril Kirilov released released Feb 28, 2022
 *
 */
class TranslatorBulgarian : public TranslatorAdapter_1_9_4
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage() override
    { return "bulgarian"; }

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
    virtual QCString latexLanguageSupportCommand() override
    {
    { return "\\usepackage[T2A]{fontenc}\n\\usepackage[bulgarian]{babel}\n"; }
    }

    virtual QCString trISOLang() override
    {
      return "bg";
    }
    virtual QCString getLanguageString() override
    {
      return "0x402 bulgarian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Функции, свързани с класа"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Имайте предвид, че тези функции не са членове на класа.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Подробно описание"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Подробности"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Членове Дефинирани типове Документация"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Членове Изброявания Документация"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Членове Функции(методи) Документация"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Полета Документация";
      }
      else
      {
        return "Член данни Документация";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Допълнително..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Списък на всички членове"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Членове Списък"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Това е пълен списък с членове за "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return ", включително всички наследени членове."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Генерирано автоматично от Doxygen";
      if (!s.isEmpty()) result+=(QCString)" за "+s;
      result+=" от изходния код.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "изброяване"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "елементи на изброяване"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "дефинирани в"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Групи"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Класове Йерархия"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни";
      }
      else
      {
        return "Класове Списък";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Файлове Списък"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Член Полета с данни";
      }
      else
      {
        return "Членове на класа";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Глобални";
      }
      else
      {
        return "Файлове Членове";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Свързани страници"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Примери"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Търсене"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Йерархичен списък на всички обекти:";
      }
      else
      {
        return "Този списък с наследявания е сортиран, "
               "но не изцяло по азбучен ред:";
      }
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Пълен списък с ";
      if (!extractAll) result+="документирани ";
      result+="файлове с кратко описание:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни с кратко описание:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Класове с кратко описание:";
      }
      else
      {
        return "Класове, структури, "
               "обединения и интерфейси с кратко описание:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll)
      {
        result+="документирани ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="членове поле, структура и обединение";
      }
      else
      {
        result+="членове на класа";
      }
      result+=" с връзки към ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="документацията за структура/обединение за всяко поле:";
        }
        else
        {
          result+="документацията на класа за всеки член:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="структурите/обединенията, към които принадлежат:";
        }
        else
        {
          result+="класовете, към които принадлежат:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll) result+="документирани ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="функции, променливи, макроопределения, изброявания и дефиниции на типове";
      }
      else
      {
        result+="членове на файлове";
      }
      result+=" с връзки към ";
      if (extractAll)
        result+="файловете,към които принадлежат:";
      else
        result+="документацията:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() override
    { return "Списък на всички примери:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Списък на всички свързани страници с документация:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Списък на всички групи:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Документация"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Групи Указател"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Класове Йерархичен указател"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни Указател";
      }
      else
      {
        return "Класове Указател";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "Файлове Списък"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Групи Документация"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни Документация";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Класове Документация";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Файлове Документация"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Помощно ръководство"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Макроси"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Дефиниции на типове"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Изброявания"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Променливи"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Елементи на изброяване"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Макро дефиниции Документация"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Дефинирани типове Документация"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Изброени типове Документация"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Функции Документация"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Променливи Документация"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни";
      }
      else
      {
        return "Класове";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result=(QCString)"Генерирано на "+date;
      if (!projName.isEmpty()) result+=(QCString)" за "+projName;
      result+=(QCString)" от";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return (QCString)"Диаграма на наследяване за "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Предупреждение"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Версия"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Връща"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Виж също"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Аргументи"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Изключения"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Генериран от"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Именни пространства Списък "; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="пълен списък с ";
      if (!extractAll) result+="документирани ";
      result+="именни пространства с кратко описание:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Приятели"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Приятели и Свързани функции Документация"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Клас"; break;
        case ClassDef::Struct:     result+=" Структура"; break;
        case ClassDef::Union:      result+=" Обединение"; break;
        case ClassDef::Interface:  result+=" Интерфейс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категория"; break;
        case ClassDef::Exception:  result+=" Изключение"; break;
        default: break;
      }
      if (isTemplate) result+=" Шаблон";
      result+=" Препратка";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" Файл Справка";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Именно пространството Справка";
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Общодостъпни членове функции"; }
    virtual QCString trPublicSlots() override
    { return "Общодостъпни слотове"; }
    virtual QCString trSignals() override
    { return "Сигнали"; }
    virtual QCString trStaticPublicMembers() override
    { return "Статични общодостъпни членове функции"; }
    virtual QCString trProtectedMembers() override
    { return "Защитени членове функции"; }
    virtual QCString trProtectedSlots() override
    { return "Защитени слотове"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Статични защитени членове функции"; }
    virtual QCString trPrivateMembers() override
    { return "Частни членове функции"; }
    virtual QCString trPrivateSlots() override
    { return "Частни слотове"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Статични частни членове функции"; }

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
            result+=", и ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Базови класове "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Производни класове "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return "Заменя наследеният метод "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Заменя в "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Членове Именни пространства";
	}

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Пълен списък с ";
      if (!extractAll) result+="документирани ";
      result+="членове именни пространства с връзки към ";
      if (extractAll)
        result+="документация за именно пространство за всеки член:";
      else
        result+="именните пространства към които принадлежат:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Именни пространства Указател"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Именни пространства Документация"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Именни пространства"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result=(QCString)"Документация за ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"проектна единица":"клас"; break;
        case ClassDef::Struct:     result+="структура"; break;
        case ClassDef::Union:      result+="обединение"; break;
        case ClassDef::Interface:  result+="интерфейс"; break;
        case ClassDef::Protocol:   result+="протокол"; break;
        case ClassDef::Category:   result+="категория"; break;
        case ClassDef::Exception:  result+="изключение"; break;
        default: break;
      }
      result+=" генериран от ";
      if (single) result+="следният файл:"; else result+="следните файлове:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Връщана стойност"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Главна страница"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Виж дефиницията във файла @1 ред @0.";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Виж дефиницията във файла @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Остарело";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return (QCString)"Граф на зависимости за класа "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return (QCString)"Граф на включените заглавни файлове за "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Конструктор & Деструктор Документация";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Вижте изходният код на този файл.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Вижте документацията за този файл.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Предпоставка";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Следусловие";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Инвариант";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Първоначална стойност:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "изходен код";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Йерархия на класовете Графичен вид";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Виж графичен вид на йерархията на класовете";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Виж текстови вид на йерархията на класовете";
    }
    virtual QCString trPageIndex() override
    {
      return "Тематични описания Указател";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Пояснение";
    }
    virtual QCString trPublicTypes() override
    {
      return "Общодостъпни типове";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Полета данни";
      }
      else
      {
        return "Общодостъпни атрибути";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Статични общодостъпни атрибути";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Защитени типове";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Защитени атрибути";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Статични защитени атрибути";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Частни типове";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Частни атрибути";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Статични частни атрибути";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() override
    {
      return "Елементи на списъка със задачи";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Списък със задачи за обсъждане свързани с подобренията";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Използва се";
    }
    virtual QCString trRemarks() override
    {
      return "Забележки";
    }
    virtual QCString trAttention() override
    {
      return "Внимание";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Граф на файлове показващ, кой файлове включват "
             "явно или косвено този файл:";
    }
    virtual QCString trSince() override
    {
      return "От";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Графична легенда";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
    {
      return
        "Тази страница обяснява как да интерпретирате графиките, генерирани "
        "от doxygen.<p>\n"
        "Разгледайте следният пример:\n"
        "\\code\n"
        "/*! Невидим клас поради съкращаване */\n"
        "class Invisible { };\n\n"
        "/*! Съкратен клас, наследствена връзка е скрита */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Класът не е документиран с doxygen коментари */\n"
        "class Undocumented { };\n\n"
        "/*! Клас, който се наследява чрез публично наследяване */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Шаблонен клас */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Клас, който се наследява чрез защитено наследяване */\n"
        "class ProtectedBase { };\n\n"
        "/*! Клас, който се наследява чрез частно наследяване */\n"
        "class PrivateBase { };\n\n"
        "/*! Клас, използващ наследеният клас */\n"
        "class Used { };\n\n"
        "/*! Супер клас, който наследява редица други класове */\n"
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
        "Правоъгълниците в този граф имат следното значение:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Правоъгълниците в този граф имат следното значение:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%Запълненият сиво правоъгълник представлява структурата или клас, "
        "за който е създаден графа.</li>\n"
        "<li>%Правоъгълника с черна рамка обозначава документирана структура или клас.</li>\n"
        "<li>%Правоъгълника със сива рамка обозначава недокументирана структура или клас.</li>\n"
        "<li>%Правоъгълника с червена рамка обозначава документирана структура или клас за"
        "който не са показани всички отношения наследяване/съдържание. %A Графa e"
        "съкратен, ако не се вписва в определените граници.</li>\n"
        "</ul>\n"
        "<p>\n"
        "Стрелките имат следното значение:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%Тъмносиня стрелка се използва за визуализиране на публично наследство "
        "между два класа.</li>\n"
        "<li>%Тъмнозелена стрелка се използва за защитено наследяване.</li>\n"
        "<li>%Тъмночервена стрелка се използва за частно наследяване.</li>\n"
        "<li>%Лилава пунктирана стрелка се използва, ако клас се съдържа или използва "
        "от друг клас. Стрелката указва променлива(и) "
        "чрез който е достъпен посоченият клас или структура.</li>\n"
        "<li>%Жълта пунктирана стрелка обозначава връзка между екземпляр на шаблон и "
        "класът шаблон, от който е създаден. Стрелката указва "
        "параметрите на шаблона на екземпляра.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend() override
    {
      return "легенда";
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
      return "Тестове Списък";
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
      return "Свойство Документация";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структури от данни";
      }
      else
      {
        return "Класове";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return (QCString)"Пакет "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Списък на пакетите с кратки описания(ако има такива):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Пакети";
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
      return "Бъг";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Бъгове Списък";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file
     *
     * The following table shows the correlation of Charset name, Charset Value and
     * Codepage number:
     * <pre>
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
    virtual QCString trRTFansicp() override
    {
      return "1251";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet() override
    {
      return "204";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Азбучен указател";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Клас" : "клас"));
      if (!singular)  result+="ове";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Файл" : "файл"));
      if (!singular)  result+="ове";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Именн" : "именн"));
	  result+=(singular ? "о пространство" : "и пространства");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Груп" : "груп"));
	  result+=(singular ? "а" : "и");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Страниц" : "странц"));
	  result+=(singular ? "а" : "и");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Член" : "член"));
      if (!singular)  result+="ове";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Глобалн" : "глобалн"));
	  result+=(singular ? "а" : "и");
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
      if (!singular)  result+="и";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Кръстосани препратки";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Заменя "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Заменя в "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Съдържание";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Остарели дефиниции и декларации Списък";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Събития";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Събития Документация";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Типове с област на видимост пакет";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Функции с област на видимост пакет";
    }
    virtual QCString trPackageMembers() override
    {
      return "Членове с област на видимост пакет";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Статични функции с област на видимост пакет";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Променливи с област на видимост пакет";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Статични променливи с област на видимост пакет";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Указател към не филтриран списък на всички членове";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Граф с извикванията за тази функция:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
    {
      return "Резултати от търсенето";
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
        return "За съжаление няма документи, отговарящи на вашата заявка.";
      }
      else if (numDocuments==1)
      {
        return "Намерен е <b>1</b> документ, съответстващ на вашата заявка.";
      }
      else
      {
        return "Намерени са <b>$num</b> документи, съответстващи на вашата заявка. "
               "Първо се показват най-добрите съвпадения.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches() override
    {
      return "Намерено:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return filename + " Изходен файл";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Директории Йерархия"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Директории Документация"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Директории"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Директория Съдържание"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
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
       return "Това е преопределена член функция, "
              "предоставена за удобство. Различава се от спомената по горе "
              "функция само по броя на фактическите аргументи.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Граф на извикванията за тази функция:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Изброяване Документация"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Функции/подпрограми Документация"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Типове данни Списък"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Полета данни"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Анотиран списък от типовете данни с кратки описания:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll)
      {
        result+="документирани ";
      }
      result+="членове типове данни";
      result+=" с препратки към ";
      if (!extractAll)
      {
         result+="документацията за структура от данни за всеки член";
      }
      else
      {
         result+="типовете данни, към които принадлежат:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran() override
    { return "Типове данни Указател"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Типове данни Документация"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Функции/Подпрограми"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Функции/Подпрограми Документация"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Типове данни"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Модули Списък"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll) result+="документирани ";
      result+="модули с кратко описание:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Модул"; break;
        case ClassDef::Struct:     result+=" Тип"; break;
        case ClassDef::Union:      result+=" Обединение"; break;
        case ClassDef::Interface:  result+=" Интерфейс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категория"; break;
        case ClassDef::Exception:  result+=" Изключение"; break;
        default: break;
      }
      if (isTemplate) result+=" Шаблон";
      result+=" Отнася се";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Модул Справка";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Модул Членове"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll) result+="документирани ";
      result+="членове на модула с връзки към ";
      if (extractAll)
      {
        result+="документацията на модула за всеки член:";
      }
      else
      {
        result+="модулите, към които принадлежат:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex() override
    { return "Модули Указател"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Модул" : "модул"));
      if (!singular)  result+="и";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result=(QCString)"Документацията за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="модул"; break;
        case ClassDef::Struct:     result+="тип"; break;
        case ClassDef::Union:      result+="обединение"; break;
        case ClassDef::Interface:  result+="интерфейс"; break;
        case ClassDef::Protocol:   result+="протокол"; break;
        case ClassDef::Category:   result+="категория"; break;
        case ClassDef::Exception:  result+="изключение"; break;
        default: break;
      }
      result+=" беше генериран ";
      if (single) result+="от следният файл:"; else result+="от следните файлове:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Тип" : "тип"));
      if (!singular)  result+="ове";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Подпрограм" : "подпрограм"));
	  if (singular) result+="а:"; else result+="и:";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Съвместими типове";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Връзка";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Зареждане...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Глобални именни пространства";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Търсене...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
    {
      return "Не намерено";
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
      return (QCString)"Файл в "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
    {
      return (QCString)"Включва файла в "+name;
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
      static const char *days[]   = { "Пн","Вт","Ср","Чт","Пт","Сб","Нд" };
      static const char *months[] = { "Яну","Фев","Мар","Апр","Май","Юни","Юли","Авг","Сеп","Окт","Ное","Дек" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
      static const char *days_short[]   = { "пон", "вт", "ср", "четв", "пет", "съб", "нед" };
      static const char *days_full[]    = { "понеделник", "вторник", "сряда", "четвъртък", "петък", "събота", "неделя" };
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "яну", "фев", "мар", "апр", "май", "юни", "юли", "авг", "сеп", "окт", "ное", "дек" };
      static const char *months_full[]  = { "януари", "февруари", "март", "април", "май", "юни", "юли", "август", "септември", "октомври", "ноември", "декември" };
      return full? months_full[month-1] : months_short[month-1];
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "пр.об.", "сл.об." };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences() override
    { return "Библиография"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Авторско право"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return QCString("Граф на зависимостта на директория за ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel() override
    { return "ниво на детайлност"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters() override
    { return "Шаблона Параметри"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number) override
    { return "и "+number+" повече..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Документацията за това изброяване е генерирана от ";
	  if (single) result+="следният файл:"; else result+="следните файлове:";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Изброяване Справка"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" наследен от "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers() override
    { return "Допълнителни наследени членове"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "включване" : "изключване";
      return "натиснете на "+opt+" за синхронизация на панела";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory() override
    {
      return "По групи @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass() override
    {
      return "Разширява класа @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods() override
    {
      return "Клас Методи";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods() override
    {
      return "Общодостъпни Методи";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation() override
    {
      return "Метод Документация";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces() override
    { return "Експортирани интерфейси"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices() override
    { return "Включени услуги"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups() override
    { return "Постоянни групи"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Постоянни групи Справка";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName) override
    {
      QCString result=(QCString)sName;
      result+=" Сървис Справка";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=(QCString)sName;
      result+=" Конкретика Справка";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result=(QCString)"Документацията за този сървис "
                                "беше генерирано от ";
      if (single) result+="следният файл:"; else result+="следните файлове:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result=(QCString)"Документацията за тази конкретика "
                                "беше генерирано от ";
      if (single) result+="следният файл:"; else result+="следните файлове:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy() override
    { return "Проектни единици Йерархия"; }
    /** VHDL design unit list */
    virtual QCString trDesignUnitList() override
    { return "Проектни единици Списък"; }
    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers() override
    { return "Проектна единица Членове"; }
    /** VHDL design unit list description */
    virtual QCString trDesignUnitListDescription() override
    {
        return "Here is a list of all design unit members with links to "
            "the Entities they belong to:";
    }
    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex() override
    { return "Проектна единица Указател"; }
    /** VHDL design units */
    virtual QCString trDesignUnits() override
    { return "Проектни единици"; }
    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc() override
    { return "Функции/Процедури/Процеси"; }
    /** VHDL type */
    virtual QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Библиотека";
          else        return "Библиотеки";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Пакет";
          else        return "Пакети";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Сигнал";
          else        return "Сигнали";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Компонент";
          else        return "Компоненти";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Константа";
          else        return "Константи";
        case VhdlSpecifier::ENTITY:
          if (single) return "Същност";
          else        return "Същности";
        case VhdlSpecifier::TYPE:
          if (single) return "Тип";
          else        return "Типове";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Подтип";
          else        return "Подтипове";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Функция";
          else        return "Функции";
        case VhdlSpecifier::RECORD:
          if (single) return "Запис";
          else        return "Записи";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Процедура";
          else        return "Процедури";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Архитектура";
          else        return "Архитектури";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Атрибут";
          else        return "Атрибути";
        case VhdlSpecifier::PROCESS:
          if (single) return "Процес";
          else        return "Процеси";
        case VhdlSpecifier::PORT:
          if (single) return "Порт";
          else        return "Портове";
        case VhdlSpecifier::USE:
          if (single) return "use клауза";
          else        return "Use клаузи";
        case VhdlSpecifier::GENERIC:
          if (single) return "Роден";
          else        return "Родни";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Тяло на пакета";
        case VhdlSpecifier::UNITS:
          return "Единици";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Споделена променлива";
          else        return "Споделени променливи";
        case VhdlSpecifier::VFILE:
          if (single) return "Файл";
          else        return "Файлове";
        case VhdlSpecifier::GROUP:
          if (single) return "Група";
          else        return "Групи";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Инстанция";
          else        return "Инстанции";
        case VhdlSpecifier::ALIAS:
          if (single) return "Псевдоним";
          else        return "Псевдоними";
        case VhdlSpecifier::CONFIG:
          if (single) return "Конфигурация";
          else        return "Конфигурации";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Други";
        case VhdlSpecifier::UCF_CONST:
          return "Ограничения";
        default:
          return "Клас";
      }
    }
    virtual QCString trCustomReference(const QCString &name) override
    { return QCString(name)+" Отнася се"; }

    /* Slice */
    virtual QCString trConstants() override
    {
        return "Константи";
    }
    virtual QCString trConstantDocumentation() override
    {
        return "Константи Документация";
    }
    virtual QCString trSequences() override
    {
        return "Последователности";
    }
    virtual QCString trSequenceDocumentation() override
    {
        return "Последователности Документация";
    }
    virtual QCString trDictionaries() override
    {
        return "Речници";
    }
    virtual QCString trDictionaryDocumentation() override
    {
        return "Речници Документация";
    }
    virtual QCString trSliceInterfaces() override
    {
        return "Интерфейси";
    }
    virtual QCString trInterfaceIndex() override
    {
        return "Интерфейси Указател";
    }
    virtual QCString trInterfaceList() override
    {
        return "Интерфейси списък";
    }
    virtual QCString trInterfaceListDescription() override
    {
        return "Интерфейсите с кратки описания:";
    }
    virtual QCString trInterfaceHierarchy() override
    {
        return "Интерфейси Йерархия";
    }
    virtual QCString trInterfaceHierarchyDescription() override
    {
        return "Този списък на наследяване е сортиран, но не напълно, по азбучен ред:";
    }
    virtual QCString trInterfaceDocumentation() override
    {
        return "Интерфейси Документация";
    }
    virtual QCString trStructs() override
    {
        return "Структури";
    }
    virtual QCString trStructIndex() override
    {
        return "Структури Указател";
    }
    virtual QCString trStructList() override
    {
        return "Структури Списък";
    }
    virtual QCString trStructListDescription() override
    {
        return "Структури с кратки описания:";
    }
    virtual QCString trStructDocumentation() override
    {
        return "Структури Документация";
    }
    virtual QCString trExceptionIndex() override
    {
        return "Изключения Указател";
    }
    virtual QCString trExceptionList() override
    {
        return "Изключения Списък";
    }
    virtual QCString trExceptionListDescription() override
    {
        return "Изключения с кратки описания:";
    }
    virtual QCString trExceptionHierarchy() override
    {
        return "Изключения Йерархия";
    }
    virtual QCString trExceptionHierarchyDescription() override
    {
        return "Този списък на наследяване е сортиран, но не напълно, по азбучен ред:";
    }
    virtual QCString trExceptionDocumentation() override
    {
        return "Изключения Документация";
    }
    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result=(QCString)clName;
      if (isLocal) result+=" Локален";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Клас"; break;
        case ClassDef::Struct:     result+=" Структура"; break;
        case ClassDef::Union:      result+=" Обединение"; break;
        case ClassDef::Interface:  result+=" Интерфейс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категория"; break;
        case ClassDef::Exception:  result+=" Изключение"; break;
        default: break;
      }
      result+=" Отнася се";
      return result;
    }
    virtual QCString trOperations() override
    {
        return "Операции";
    }
    virtual QCString trOperationDocumentation() override
    {
        return "Операции Документация";
    }
    virtual QCString trDataMembers() override
    {
        return "Членове Данни";
    }
    virtual QCString trDataMemberDocumentation() override
    {
        return "Членове Данни Документация";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation() override
    { return "Проектни единици Документация"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    virtual QCString trConcept(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Концепци" : "концепци"));
	  if (singular)  result+="я"; else result+="и";
      return result;
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    virtual QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result=conceptName;
      result+=" Концепции Справка";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    virtual QCString trConceptList() override
    { return "Концепции Списък"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    virtual QCString trConceptIndex() override
    { return "Концепции Указател"; }

    /*! used as the title of chapter containing all information about concepts. */
    virtual QCString trConceptDocumentation() override
    { return "Концепции Документация"; }

    /*! used as an introduction to the concept list */
    virtual QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Списък на всички ";
      if (!extractAll) result+="документирани ";
      result+="концепции с кратко описание:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    virtual QCString trConceptDefinition() override
    {
      return "Дефиниция на концепция";
    }
};

#endif
