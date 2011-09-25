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
 * The translation into Ukrainian was provided by
 *   Olexij Tkatchenko (olexij.tkatchenko@parcs.de)
 */

#ifndef TRANSLATOR_UA_H
#define TRANSLATOR_UA_H

class TranslatorUkrainian : public TranslatorAdapter_1_4_1
{
  private:
    /*! The Decode() inline assumes the source written in the 
        Koi8-U encoding (maintainer dependent). 
     */
    inline QCString decode(const QCString & sInput)
    { 
//#ifdef _WIN32
//      return Koi8RToWindows1251(sInput);
//#else
      return sInput;
//#endif
    }

  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage()
    { return "ukrainian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand()
    {
//#ifdef _WIN32
//      return "\\usepackage[cp1251]{inputenc}\n\\usepackage[ukrainian]{babel}\n"; 
//#else
      return "\\usepackage[T2A]{fontenc}\n\\usepackage[ukrainian]{babel}\n"; 
//#endif
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
//#ifdef _WIN32
//      return "Windows-1251"; 
//#else
      return "koi8-u";
//#endif
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return decode("Спор╕днен╕ фукц╕╖"); }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return decode("(не методи компонент)"); }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return decode("Детальний опис"); }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return decode("Опис тип╕в користувача"); }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return decode("Опис перел╕к╕в користувача"); }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return decode("Опис метод╕в компонент"); }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля" );
      }
      else
      {
        return decode( "Компонентн╕ дан╕" );
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return decode("Детальн╕ше..."); }

    /*! put in the class documentation */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trListOfAllMembers()
    {
      return decode( "Список вс╕х елемент╕в" );
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trMemberList()
    {
      return decode( "Cписок елемент╕в" );
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers()
    { return decode("Повний список елемент╕в"); }

    /*! this is the remainder of the sentence after the class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers()
    { return decode(", включаючи вс╕ успадкован╕ елементи"); }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result=decode("Автоматично створено за допомогою Doxygen");
      if (s) result+=decode(" для ")+s;
      result+=decode(" з тексту програми."); 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return decode("Перел╕к"); }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return decode("Елемент перел╕ку"); }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return decode("визначено в "); }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return decode("Програмн╕ модул╕"); }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return decode("╤╓рарх╕я клас╕в"); }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структури даних" );
      }
      else
      {
        return decode( "Класи" );
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return decode("Файли"); }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return decode("Заголовочн╕ файли"); }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля структур" );
      }
      else
      {
        return decode( "Елементи клас╕в" );
      }
    }

    /*! This is put above each page as a link to all members of files. */
    /*??*/
    virtual QCString trFileMembers()
    {
      return decode( "Елементи файлу" );
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return decode("Додаткова ╕нформац╕я"); }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return decode("Приклади"); }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return decode("Пошук"); }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return decode("Список успадкувань впорядковано наближено до алфав╕ту"); }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Повний список ";
      if (!extractAll) result+="документованих ";
      result+="файл╕в.";
      return decode(result);
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структури даних з коротким описом." );
      }
      else
      {
        return decode( "Класи, структури, об'╓днання та ╕нтерфейси  з коротким описом." );
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
        QCString result="Список вс╕х ";
        if(!extractAll) result+="документоаних ";
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="елемент╕в структур даних з посиланням на ";
        else
          result+="елемент╕в клас╕в даних з посиланням на ";
        if(extractAll)
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result+="документац╕ю по структур╕/об'╓днанню кожного елементу.";
          else
            result+="документац╕ю по класу кожного елементу.";
        }
        else
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result += "в╕дпов╕дн╕ структури";
          else
            result += "в╕дпов╕дн╕ класи";
          result+=", до яких вони належать.";
        }
        return decode( result );
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Список вс╕х ";
      if (!extractAll) result+="документованих ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="функц╕й, зм╕нних, макроозначень, "
                "перел╕к╕в ╕ означень тип╕в";
      }
      else
      {
        result+="елемент╕в файл╕в ";
      }
      result+=" з посиланям на ";
      if (extractAll)
        result+="файли, до яких вони належать.";
      else
        result+="документац╕ю.";
      return decode( result );
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return decode("Повний список заголовочних файл╕в."); }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return decode("Повний список приклад╕в."); }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return decode("Повний список додаткових опис╕в."); }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return decode("Повний список модул╕в."); }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return decode("Опис в╕дсутн╕й"); }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return decode("Документац╕я"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return decode("Алфав╕тний покажчик модул╕в"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return decode("╤╓рарх╕чний покажчик клас╕в"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Алфав╕тний покажчик структур даних" );
      }
      else
      {
        return decode( "Алфав╕тний покажчик клас╕в" );
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return decode( "Покажчик файлв" ); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return decode("Програмн╕ Модул╕"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структури даних" );
      }
      else
      {
        return decode( "Класи" );
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return decode("Файли"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return decode("Приклади"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return decode("Документац╕я по тем╕"); }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return decode("Дов╕дковий пос╕бник"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return decode("Макроозначення"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return decode("Прототипи фукц╕й"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return decode("Означення тип╕в"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return decode("Перел╕ки"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return decode("Функц╕╖"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return decode("Зм╕нн╕"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return decode("Елементи перел╕к╕в"); }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return decode("Опис макроозначень"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return decode("Опис прототип╕в функц╕й"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return decode("Опис означень тип╕в"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return decode("Опис перел╕к╕в"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return decode("Опис елемент╕в перел╕ку"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return decode("Опис функц╕й"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return decode("Опис зм╕нних"); }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структури даних" );
      }
      else
      { 
        return decode( "Класи" );
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
      QCString result=decode("Документац╕я ");
      if (projName) result+=decode("до ")+projName;
      result+=decode(" створена ")+date;
      result+=decode(" системою");
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return decode("Автор:");
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return decode("Схема успадкувань для ")+clName;
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return decode("Т╕льки для внутр╕шнього користування"); }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    /*??*/
    { return decode("Зм╕нено з внутр╕шн╕х причин. Зм╕ни не торкаються API."); 
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return decode("Застереження"); }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return decode("Помилки та обмеження застосування"); }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return decode("Верс╕я"); }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return decode("Дата"); }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return decode("Поверта╓"); }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return decode("Див. також"); }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return decode("Аргументи"); }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return decode("Обробка виняткових ситуац╕й"); }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return decode("Створено системою"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return decode("Прост╕р ╕мен"); }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Повний перел╕к ";
      if (!extractAll) result+="документованих ";
      result+="простор╕в ╕мен.";
      return decode(result);
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return decode("Дружн╕ класи"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
      { return decode("Документация до дружн╕х клас╕в та в╕дпов╕дних функц╕й"); }
    
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
          case ClassDef::Union:  result+="об'╓днання"; break;
          case ClassDef::Interface:  result+="╕нтерфейсу"; break;
          case ClassDef::Protocol:   result+="протоколу"; break;
          case ClassDef::Category:   result+="категор╕╖"; break;
          case ClassDef::Exception:  result+="Виняток"; break;
        }
      }
      else
      {
        switch(compType)
        {
          case ClassDef::Class:  result+="Клас"; break;
          case ClassDef::Struct: result+="Структура"; break;
          case ClassDef::Union:  result+="Об'╓днання"; break;
          case ClassDef::Interface:  result+="╤нтерфейс"; break;
          case ClassDef::Protocol:   result+="Протокол"; break;
          case ClassDef::Category:   result+="Категор╕я"; break;
          case ClassDef::Exception:  result+="Виняток"; break;
        }
      }
      result+=" ";
      return decode(result)+clName;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      return decode("Файл ")+fileName;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      return decode("Прост╕р ╕мен ")+namespaceName;
    }
    
    virtual QCString trPublicMembers()
    { return decode("Загальнодоступн╕ елементи"); }
    virtual QCString trPublicSlots()
    { return decode("Загальнодоступн╕ слоти"); }
    virtual QCString trSignals()
    { return decode("Сигнали"); }
    virtual QCString trStaticPublicMembers()
    { return decode("Загальнодоступн╕ статичн╕ елементи"); }
    virtual QCString trProtectedMembers()
    { return decode("Захищен╕ елементи"); }
    virtual QCString trProtectedSlots()
    { return decode("Захищен╕ слоти"); }
    virtual QCString trStaticProtectedMembers()
    { return decode("Захищен╕ статичн╕ елементи"); }
    virtual QCString trPrivateMembers()
    { return decode("Приватн╕ елементи"); }
    virtual QCString trPrivateSlots()
    { return decode("Приватн╕ слоти"); }
    virtual QCString trStaticPrivateMembers()
    { return decode("Приватн╕ статичн╕ елементи"); }
    
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
            result+=decode( " ╕ " );
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return decode("Успадкову╓ клас ")+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return decode("Успадковано класами ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return decode("Переозначення з ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return decode("Переознача╓ться в ")+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return decode("Елементи простору ╕мен"); }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Повний перел╕к ";
      if (!extractAll) result+="документованих ";
      result+="елемет╕в простору ╕мен з посиланням ";
      if (extractAll) 
        result+="на документац╕ю для кожного елементу:";
      else 
        result+="на прост╕р ╕мен до якого вони належать:";
      return decode(result);
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return decode("Алфав╕тний покажчик простору ╕мен"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return decode("Опис простору ╕мен"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return decode( "Простори ╕мен" ); }

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
      QCString result=(QCString)"Документац╕я  ";
      switch(compType)
      {
        case ClassDef::Class:
          if (single) result+="цього класу"; else result+="цих клас╕в";
          break;
        case ClassDef::Struct:
          if (single) result+="ц╕╓╖ структури"; else result+="цих структур";
          break;
        case ClassDef::Union:
          if (single) result+="цього об'╓днання"; else result+="цих об'╓днань";
          break;
        case ClassDef::Interface:
          if (single) result+="цього ╕нтерфейсу"; else result+="цих ╕нтерфейс╕в";
          break;
        case ClassDef::Protocol:  
          if (single) result+="цього протоколу"; else result+="цих протокол╕в";
          break; 
        case ClassDef::Category:
          if (single) result+="ц╕╓╖ категор╕╖"; else result+="цих категор╕й";
          break; 
        case ClassDef::Exception:
          if (single) result+="цього винятку"; else result+="цих винятк╕в";
          break;
      }
      result+=" була створена з файл";
      if (single) result+="у:"; else result+="╕в:";
      return decode(result);
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return decode("Алфав╕тний покажчик"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return decode("Значення, що повертаються"); }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return decode("Титульна стор╕нка"); }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return decode("стор."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return decode("Вих╕дн╕ тексти.");
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return decode("Див. означення в файл╕ @1, рядок @0");
    }
    virtual QCString trDefinedInSourceFile()
    {
      return decode("Див. означення в файл╕ @0");
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return decode("Не рекомендовано");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)decode("Д╕аграма зв'язк╕в класу ")+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return decode("Д╕аграма включених заголовочних файл╕в для ")+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return decode("Конструктор(и)"); 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return decode("Див. вих╕дн╕ тексти.");
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return decode("Див. документац╕ю.");
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return decode("Передумови");
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return decode("П╕сляумови");
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return decode("╤нвар╕ант");
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return decode("Початков╕ значення");
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return decode("Вих╕дн╕ тексти");
    }
    virtual QCString trGraphicalHierarchy()
    {
      return decode("Граф╕чна ╕╓рарх╕я клас╕в");
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return decode("див. граф╕чну ╕╓рарх╕ю");
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return decode("див. текстову ╕╓рарх╕ю");
    }
    virtual QCString trPageIndex()
    {
      return decode("Алфав╕тний покажчик тематичних опис╕в");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return decode("Нотатки");
    }
    virtual QCString trPublicTypes()
    {
      return decode("Загальнодоступн╕ типи");
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля даних" );
      }
      else
      {
        return decode( "Загальнодоступн╕ атрибути" );
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return decode("Статичн╕ загальнодоступн╕ данн╕");
    }
    virtual QCString trProtectedTypes()
    {
      return decode("Захищен╕ типи");
    }
    virtual QCString trProtectedAttribs()
    {
      return decode("Захищен╕ дан╕");
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return decode("Статичн╕ захищен╕ дан╕");
    }
    virtual QCString trPrivateTypes()
    {
      return decode("Приватн╕ типи");
    }
    virtual QCString trPrivateAttribs()
    {
      return decode("Приватн╕ дан╕");
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return decode("Приватн╕ статичн╕ дан╕");
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    /*??*/
    {
      return decode("Необх╕дно зробити");
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    /*??*/
    {
      return decode("Перел╕к завдань");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return decode("Вжива╓ться в");
    }
    virtual QCString trRemarks()
    {
      return decode("Прим.");
    }
    virtual QCString trAttention()
    {
      return decode("Увага");
    }
    virtual QCString trInclByDepGraph()
    {
      return decode("Граф файл╕в, як╕ включають цей файл:");
    }
    virtual QCString trSince()
    /*??*/
    {
      return decode("Починаючи з");
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return decode("Легенда");
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return decode( 
        "Позначення, що використовуються у графах.<p>\n"
        "Розглянемо наступний приклад:\n"
        "\\code\n"
        "/*! Невидимий клас через зр╕зання */\n"
        "class Invisible { };\n\n"
        "/*! Обмежений клас, в╕дношення успадкування приховане */\n"
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
        "/*! Клас, що використову╓ться класом Inherited */\n"
        "class Used { };\n\n"
        "/*! Клас, що успадкову╓ ╕нш╕ класи */\n"
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
        "Якщо \\c MAX_DOT_GRAPH_HEIGHT в конф╕гурац╕йному файл╕ "
        "установлене на 200, отрима╓мо наступний граф:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Прямокутники в цьому граф╕ мають наступний зм╕ст:\n"
        "<ul>\n"
        "<li>Заповнений чорний прямокутник в╕добража╓ структуру або клас, "
        "для якого створено граф.\n"
        "<li>Прямокутник з чорною межею в╕добража╓ документовану структуру або клас.\n"
        "<li>Прямокутник з с╕рою межею в╕добража╓ недокументовану структуру або клас.\n"
        "<li>Прямокутник з червоною межею в╕добража╓ документовану структуру або клас, для яких\n"
        " не вс╕ сп╕вв╕дношення успадкування/зм╕сту показан╕. Граф зр╕заниЙ, "
        "якщо в╕н не вм╕ща╓ться у вказан╕ меж╕."
        "</ul>\n"
        "Стр╕лки мають наступний зм╕ст:\n"
        "<ul>\n"
        "<li>Темносиня стр╕лка в╕добража╓ в╕дношення загальнодоступного успадкування "
        "м╕ж двома класами.\n"
        "<li>Темнозелена стр╕лка використову╓ться при захищеному успадкуванн╕.\n"
        "<li>Темночервона стр╕лка використову╓ться при приватному успадкуванн╕.\n"
        "<li>Пурпурна стр╕лка використову╓ться, якщо клас м╕ститься в"
        "╕ншому клас╕ або ним використову╓ться."
        "Стр╕лка етикету╓ться зм╕нною, "
        "через яку в╕дбува╓ться доступ до вказано╖ структури або класу. \n"
        "</ul>\n");
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return decode("див. легенду");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return decode("Тест");
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return decode("Перел╕к тест╕в");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return decode("DCOP Методи");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return decode("Властивост╕");
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return decode("Повний перел╕к властивостей");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return decode("╤нтерфейси");
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структури даних" );
      }
      else
      {
        return decode( "Класи" );
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return decode("Пакет ")+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return decode("Повний перел╕к пакет╕в");
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return decode("Повний перел╕к документованих пакет╕в.");
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return decode("Пакети");
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return decode("Опис пакет╕в");
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return decode("Макроозначення:");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return decode("Дефект");
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return decode("Перел╕к дефект╕в");
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
      return decode("Предметний покажчик");
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
        return decode(result); 
      }
      else
      {
        QCString result((first_capital ? "Клас" : "клас"));
        if(!singular) result+="и";
        return decode(result); 
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
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Прост" : "прост"));
      result+=(singular?"╕р ╕мен":"ори ╕мен");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Груп" : "груп"));
      result+=(singular ? "а" : "и");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Стор╕нк" : "стор╕нк"));
      result+=(singular ? "а" : "и");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Елемент" : "елемент"));
      if (!singular)  result+="и";
      return decode(result); 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Пол" : "пол"));
      result+=(singular ? "е" : "я");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Глобальн" : "глобальн"));
      result+=(singular ? "ий" : "╕");
      return decode(result); 
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
      return decode(result); 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Використову╓";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Реал╕зу╓ " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Реал╕зу╓ в " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Зм╕ст";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Застар╕л╕ елементи";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Под╕╖";
    }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Документац╕я под╕й";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Типи пакет╕в";
    }

    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Функц╕╖ пакет╕в";
    }

    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Статичн╕ функц╕ю пакет╕в";
    }
    
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Атрибути пакет╕в";
    }
    
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Статичн╕ атрибути пакет╕в";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Вс╕";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Граф вс╕х виклик╕в ц╕╓╖ функц╕╖:";
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
        return "Не знайдено документ╕в в╕дпов╕дно до вашого запиту.";
      }
      else if (numDocuments==1)
      {
        return "Було знайдено <b>1</b> документ в╕дпов╕дно до вашого запиту.";
      }
      else 
      {
        return "Було знайдено <b>$num</b> документ╕в в╕дпов╕дно до вашого запиту. "
	       "Найкращ╕ в╕дпов╕дност╕ показано спочатку.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "В╕дпов╕дн╕сть:";
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
    { return "Дерево каталог╕в"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Документац╕я каталог╕в"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Каталоги"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Дерево каталог╕в впорядковано наближено "
	     "до алфав╕ту:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Дов╕дник каталогу "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool singular)
    { 
      QCString result("Каталог");
      if (!singular) result+="и";
      return result; 
    }

};

#endif

