/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
// The cyrilic strings were entered using Macedonian language support in
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
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[macedonian]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "UTF-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Поврзани Функции"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Овие функции не се членови.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Детален опис"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Документација на членови дефиниции на тип"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Документација на набројани членови"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Документација на функции членови"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      //if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Документација на членови"; 
      }      
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Повеќе..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Список на сите членови."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Список на членови"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Ова е целосниот список на членови на "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", вклучувајќи ги сите наследени членови."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Автоматски создадено од Doxygen";
      if (s) result+=(QCString)" за "+s;
      result+=" изворниот код."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "име на набројан член"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "вредност на набројан член"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "дефиниран во"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Модули"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Стебло на класи"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Список на Структури";
      }
      else
      {
        return "Список на Класи";
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Список на Датотеки"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Членови"; 
      }
      else
      {
        return "Членови на Класата"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Глобални Членови"; 
      }
      else
      {
        return "Членови на Датотеката"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Страници Поврзани со Оваа"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Примери"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Пребарај"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Овој список на наследување е приближно азбучно подреден:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="членови со кратки описи:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Список на структури со кратки описи:"; 
      }
      else
      {
        return "Список на класи, структури, унии и интерфејси "
               "со кратки описи:"; 
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Список на сите ";
      if (!extractAll)
      {
        result+="документирани ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Список на сите примери:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Список на сите страници поврзани со оваа и нивна документација:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Список на сите модули:"; }

    // index titles (the project name is prepended for these) 

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Документација"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Попис на Модули"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Попис на Стебло"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex() 
    { return "Попис на Датотеки"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Попис на Документација"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Документација на Структури"; 
      }
      else
      {
        return "Документација на Класи"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Документија на Датотеки"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Документаија на Примери"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Документација на Страници"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Прирачник"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Дефинирања"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Прототипи на Функции"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Дефиниции на Тип"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Набројувања"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Функции"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Променливи"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Вредности на Набројувањата"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Документција на Дефиниции"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Документација на Прототип на Функции"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Документација на Дефиниции на Тип"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Документација на Набројувања"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Документација на Функции"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Документација на Променливи"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Создадено на "+date;
      if (projName) result+=(QCString)" за "+projName;
      result+=(QCString)" од";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "напишано од";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Диаграм на наследување за "+clName+":";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Само за интерна употреба."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Предупредување"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Верзија"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Дата"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Враќа"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Истотака погледнете"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Параметри"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Исклучоци"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Создадено од"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Список на Имиња на Простор"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="имиња на простор со кратки описи:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Пријатели"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Документација на Пријатели и Поврзани Функции"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Класа"; break;
        case ClassDef::Struct:     result+=" Структура"; break;
        case ClassDef::Union:      result+=" Унија"; break;
        case ClassDef::Interface:  result+=" Интерфејс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категорија"; break;
        case ClassDef::Exception:  result+=" Исклучок"; break;
      }
      if (isTemplate) result+=" Образец";
      result+=" Повикување";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result = "Опис на Датотекaта ";
      result += fileName; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result = "Опис на Имeто на Простор ";
      result += namespaceName;
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Јавни Функции Членови"; }
    virtual QCString trPublicSlots()
    { return "Јавни Слотови"; }
    virtual QCString trSignals()
    { return "Сигнали"; }
    virtual QCString trStaticPublicMembers()
    { return "Статични Јавни Функции Членови"; }
    virtual QCString trProtectedMembers()
    { return "Заштитени Функции Членови"; }
    virtual QCString trProtectedSlots()
    { return "Заштитени Слотови"; }
    virtual QCString trStaticProtectedMembers()
    { return "Статични Заштитени Функции Членови"; }
    virtual QCString trPrivateMembers()
    { return "Приватни Функции Членови"; }
    virtual QCString trPrivateSlots()
    { return "Приватни Слотови"; }
    virtual QCString trStaticPrivateMembers()
    { return "Статични Приватни Функции Членови"; }
    
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
            result+=" и ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Наследува "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Наследено од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Преприменето од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Преприменето во "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Членови на Име на Простор"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Попис на Имиња на Простор"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Документација на Имиња на Простор"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Имиња на Простор"; }

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
      QCString result=(QCString)"Документацијата за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="оваа класа"; break;
        case ClassDef::Struct:     result+="оваа структура"; break;
        case ClassDef::Union:      result+="оваа унија"; break;
        case ClassDef::Interface:  result+="овој интерфејс"; break;
        case ClassDef::Protocol:   result+="овој протокол"; break;
        case ClassDef::Category:   result+="оваа категорија"; break;
        case ClassDef::Exception:  result+="овој исклучок"; break;
      }
      result+=" беше создадена од ";
      if (single) result+="следнава датотека:"; else result+="следниве датотеки:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Азбучен Список"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Вратена Вредност"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Главна Страна"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "стр."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Дефиницијата е на линија @0 во датотека @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Дефинијата е во датотека @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Застарено";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Диаграм на соработка за "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Вклучен дијаграм на зависност за "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Документација на Конструктор и Деструктор"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Оди до изворниот код на оваа датотека.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Оди до документацијата на оваа датотека.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Предуслов";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Постуслов";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Инваријанта";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Основна вредност:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "код";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Графичко Стебло на Класи";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Оди до графичкото стебло на класи";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Оди до текстуалното стебло на класи";
    }
    virtual QCString trPageIndex()
    {
      return "Попис на Страници";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Забелешка";
    }
    virtual QCString trPublicTypes()
    {
      return "Јавни Типови";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Членови";
      }
      else
      {
        return "Јавни Членови";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Статични Јавни Членови";
    }
    virtual QCString trProtectedTypes()
    {
      return "Заштитени Типови";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Заштитени Членови";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Статични Заштитени Типови";
    }
    virtual QCString trPrivateTypes()
    {
      return "Приватни Типови";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Приватни Членови";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Статични Приватни Членови";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Незавршено";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Список на Незавршени";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Повикано од";
    }
    virtual QCString trRemarks()
    {
      return "Забелешки";
    }
    virtual QCString trAttention()
    {
      return "Внимание";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Овој дијаграм покажува кои датотеки се "
             "директно или индеректно вклучени во оваа датотека:";
    }
    virtual QCString trSince()
    {
      return "Од";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Легенда на Дијаграмот";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "Легенда";
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
      return "Листа на Тестови";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Функции Членови";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Особини";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Документација на Особини";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Структури";
      }
      else
      {
        return "Класи";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Пакет "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Список на Пакети";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Пакети со краток опис (доколку достапен):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Пакети";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Вредност:";
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
    virtual QCString trRTFansicp()
    {
      return "1252";
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "238";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Попис";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Клас" : "клас"));
      result += (singular ? "а" : "и");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Датотек" : "датотек"));
      result += (singular ? "а" : "и");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Им" : "им"));      
      result += (singular ? "е на простор" : "иња на простори");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Груп" : "груп"));
      result += (singular ? "а" : "и");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Стран" : "стран"));
      result += (singular ? "а" : "и");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Член" : "член"));
      if (!singular)  result+="ови";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Глобал" : "глобал"));
      result += (singular ? "ен" : "ни");
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
      if (!singular)  result+="и";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Повикувања";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Применува "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Применето во "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Содржина";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Список на Застарени";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Настани";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Документација на Настани";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Типови во Пакетот";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Функции во Пакетот";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Статични Функции во Пакетот";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Членови во Пакетот";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Статични Членови во Пакетот";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Сите";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Дијаграм на повикувања за оваа функција:";
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
      return "Пребарај";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Пронајдени:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Изворен код на датотеката " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Стебло на Именикот"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Документација на Именикот"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Именици"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Ова стебло на именици е приближно азбучно подреден:";}

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result= "Опис на Именикот "; result += dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Имени" : "имени"));
      if (singular) result+="к"; else result+="ци";
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
       return "Оваа е преоптоварена фунција. Се разликува по "
              "типот и бројот на аргументи што ги прифаќа.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Ова е графот на повикување за оваа фунција:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Документија на Набројувања"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Документација на функции/процедури членови"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Список на Типови"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Членови"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Список на типови со кратки описи:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Попис на Типови"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Документација на Типови"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Функции/Процедури"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Документација на Функции/Процедури"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Типови"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Попис на Модули"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Список на сите ";
      if (!extractAll) result+="документирани ";
      result+="модули со кратки описи:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName + " - Повикување на";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Класа"; break;
        case ClassDef::Struct:     result+=" Тип"; break;
        case ClassDef::Union:      result+=" Унија"; break;
        case ClassDef::Interface:  result+=" Интерфејс"; break;
        case ClassDef::Protocol:   result+=" Протокол"; break;
        case ClassDef::Category:   result+=" Категорија"; break;
        case ClassDef::Exception:  result+=" Исклучок"; break;
      }
      if (isTemplate) result+=" Образец";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" - Содржина на Модул";        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Членови на Модул"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Попис на Модули"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Модул" : "модул"));
      if (!singular)  result+="и";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Документацијата за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="оваа класа"; break;
        case ClassDef::Struct:     result+="овој тип"; break;
        case ClassDef::Union:      result+="оваа унија"; break;
        case ClassDef::Interface:  result+="овој интерфејс"; break;
        case ClassDef::Protocol:   result+="овој протокол"; break;
        case ClassDef::Category:   result+="оваа категорија"; break;
        case ClassDef::Exception:  result+="овој исклучок"; break;
      }
      result+=" беше создадена од ";
      if (single) result+="следнава датотека:"; else result+="следниве датотеки:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Тип" : "тип"));
      if (!singular)  result+="ови";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Потпрограм" : "потпрограм"));
      if (singular){
		result+="а";
      }else{
      	result+="и";
      }
      
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Ограничувања на Тип";
    }
};

#endif
