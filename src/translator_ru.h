/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
 * Alexandr V. Chelpanov <cav@cryptopro.ru> released Jun 16, 2004
 *
 * При переводе заголовков слова list и documentation убраны.
 *
 * Modules переведено как группы, так как модули чаще всего в русском 
 *  переводе встречается в контексте модуля как единицы трансляции, файла
 *  и др., что в данном случае не совсем подходит, тем более что 
 *  соответствующая команда в doxygen именуется \group.
 * 
 * Просьба не оптимизировать decode обрамляя результат, это может привести
 *  к перекодировке входа в том числе русских имен файлов.
 *
 * Отдано предпочтение при переводе related functions - "относящиеся к классу
 *  функции", а не "связанные функции", что верно до тех пор пока есть только
 *  related to class, и нет related to template, package и т.д. Последнее
 *  замечание касается большинства member.
 *
 * По возможности при переводе members указывается членом чего этот member
 *  является. 
 *
 * Compounds переведено как классы, см Строустрап.
 *
 * Определения помеченные ?? являются спорными.
 *
 * Благодарности: Vitaly A. Repin <vitaly@radio.hop.stu.neva.ru>,
 *    Михаил Глушенков <bbman@mail.ru>
 */

#ifndef TRANSLATOR_RU_H
#define TRANSLATOR_RU_H

class TranslatorRussian : public Translator
{
  private:
    /*! The Decode() inline assumes the source written in the 
        Koi8-R encoding (maintainer dependent). 
     */
    inline QCString decode(const QCString & sInput)
    { 
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      {
        return Koi8RToWindows1251(sInput);
      }
      else
      {
        return sInput;
      }
    }

  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage()
    { return "russian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand()
    {
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      { 
        return "\\usepackage[cp1251]{inputenc}\n\\usepackage[russian]{babel}\n"; 
      }
      else
      { 
        return "\\usepackage[koi8-r]{inputenc}\n\\usepackage[russian]{babel}\n"; 
      }
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      { 
        return "Windows-1251"; 
      }
      else
      { 
        return "koi8-r"; 
      }
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return decode("Относящиеся к классу функции"); }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return decode("(не члены класса)"); }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return decode("Подробное описание"); }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return decode("Определения типов"); }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return decode("Перечисления"); }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return decode("Методы"); }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля" );
      }
      else
      {
        return decode( "Данные класса" );
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return decode("Подробнее..."); }

    /*! put in the class documentation */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trListOfAllMembers()
    {
      return decode( "Полный список членов класса" );
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trMemberList()
    {
      return decode( "Cписок членов класса" );
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers()
    { return decode("Полный список членов класса "); }

    /*! this is the remainder of the sentence after the class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers()
    { return decode(", включая наследуемые из базового класса"); }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result=decode("Автоматически создано Doxygen");
      if (s) result+=decode(" для ")+s;
      result+=decode(" из исходного текста."); 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return decode("перечисление"); }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return decode("элементы перечисления"); }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return decode("определено в"); }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return decode("Группы"); }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return decode("Иерархия классов"); }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структуры данных" );
      }
      else
      {
        return decode( "Классы" );
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return decode("Файлы"); }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return decode("Заголовочные файлы"); }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля структур" );
      }
      else
      {
        return decode( "Члены классов" );
      }
    }

    /*! This is put above each page as a link to all members of files. */
    /*??*/
    virtual QCString trFileMembers()
    {
      return decode( "Список членов всех файлов" );
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    /* ?? Вариант перевода "См. также: " более удачный, но не в заголовке,
     как в данном случае. */
    { return decode("Описания"); }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return decode("Примеры"); }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return decode("Поиск"); }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return decode("Иерархия классов."); }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="файлов.";
      return decode(result);
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структуры данных с их кратким описанием." );
      }
      else
      {
        return decode( "Классы с их кратким описанием." );
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
        QCString result="Список всех ";
        if(!extractAll) result+="документированных ";
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="членов структур данных со ссылками на ";
        else
          result+="членов классов со ссылками на ";
        if(!extractAll)
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result+="документацию по структуре для каждого члена.";
          else
            result+="документацию по классу для каждого члена.";
        }
        else
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result += "структуры";
          else
            result += "классы";
          result+=", к которым они принадлежат.";
        }
        return decode( result );
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Список всех ";
      if (!extractAll) result+="документированных ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      return decode( result );
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return decode("Полный список заголовочных файлов."); }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return decode("Полный список примеров."); }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return decode("Полный список дополнительных описаний."); }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return decode("Полный список групп."); }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return decode("Описание отсутсвует"); }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return decode("Документация"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return decode("Алфавитный указатель групп"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return decode("Иерархический список классов"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Алфавитный указатель структур данных" );
      }
      else
      {
        return decode( "Алфавитный указатель классов" );
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return decode( "Список файлов" ); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return decode("Группы"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структуры данных" );
      }
      else
      {
        return decode( "Классы" );
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return decode("Файлы"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return decode("Примеры"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return decode("Тематические описания"); }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return decode("Оглавление"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return decode("Макросы"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return decode("Прототипы функций"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return decode("Определения типов"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return decode("Перечисления"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return decode("Функции"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return decode("Переменные"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return decode("Элементы перечислений"); }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return decode("Макросы"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return decode("Прототипы функций"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return decode("Типы"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return decode("Перечисления"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return decode("Элементы перечислений"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return decode("Функции"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return decode("Переменные"); }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структуры данных" );
      }
      else
      { 
        return decode( "Классы" );
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
      QCString result=decode("Документация ");
      if (projName) result+=decode("по ")+projName;
      result+=decode(". Последние изменения: ")+date;
      result+=decode(". Создано системой");
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
      return decode("Граф наследования:")+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return decode("Только для внутреннего использования"); }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    /*??*/
    { return decode("Изменено по внутренним причинам"); 
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return decode("Предупреждения"); }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return decode("Ошибки и ограничения"); }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return decode("Версия"); }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return decode("Дата"); }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return decode("Возвращает"); }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return decode("См. также"); }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return decode("Аргументы"); }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return decode("Исключения"); }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return decode("Создано системой"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return decode("Пространства имен"); }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="пространств имен.";
      return decode(result);
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return decode("Друзья"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return decode("Документация по друзьям класса и функциям, отноносящимся"
        " к классу"); }
    
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
          case ClassDef::Class:  result+="класса"; break;
          case ClassDef::Struct: result+="структуры"; break;
          case ClassDef::Union:  result+="объединения"; break;
          case ClassDef::Interface:  result+="интерфейса"; break;
          case ClassDef::Protocol:   result+="protocol"; break; // translate me!
          case ClassDef::Category:   result+="category"; break; // translate me!
          case ClassDef::Exception:  result+="исключения"; break;
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
          case ClassDef::Protocol:   result+="protocol"; break; // translate me!
          case ClassDef::Category:   result+="category"; break; // translate me!
          case ClassDef::Exception:  result+="Исключение"; break;
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
      return decode("Пространство имен ")+namespaceName;
    }
    
    virtual QCString trPublicMembers()
    { return decode("Открытые члены"); }
    virtual QCString trPublicSlots()
    { return decode("Открытые слоты"); }
    virtual QCString trSignals()
    { return decode("Сигналы"); }
    virtual QCString trStaticPublicMembers()
    { return decode("Открытые статические члены"); }
    virtual QCString trProtectedMembers()
    { return decode("Защищенные члены"); }
    virtual QCString trProtectedSlots()
    { return decode("Защищенные слоты"); }
    virtual QCString trStaticProtectedMembers()
    { return decode("Защищенные статические члены"); }
    virtual QCString trPrivateMembers()
    { return decode("Закрытые члены"); }
    virtual QCString trPrivateSlots()
    { return decode("Закрытые слоты"); }
    virtual QCString trStaticPrivateMembers()
    { return decode("Закрытые статические члены"); }
    
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
            result+=decode( " и " );
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return decode("Базовые классы:")+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return decode("Производные классы:")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      QCString result="Переопределяет метод";
      if(numEntries>1)
	result+="ы предков";
      else
	result+=" предка";
      return decode(result+" ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return decode("Переопределяется в ")+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return decode("Члены пространств имен"); }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="членов простанств имен.";
      return decode(result);
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return decode("Алфавитный указатель пространств имен"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return decode("Пространства имен"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return decode( "Пространства имен" ); }

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
      QCString result=(QCString)"Объявления и описания членов ";
      switch(compType)
      {
        case ClassDef::Class:      result+="класс"; 
          if (single) result+='а'; else result+="ов";
          break;
        case ClassDef::Struct:     result+="структур"; 
          if (single) result+='ы';
          break;
        case ClassDef::Union:      result+="объединени";
          if (single) result+='я'; else result+='й';
          break;
        case ClassDef::Interface:  result+="интерфейс";
          if (single) result+='а'; else result+="ов";
          break;
        case ClassDef::Protocol:  result+="Protocol"; // translate me!
          break;
        case ClassDef::Category:  result+="Category"; // translate me!
          break;
        case ClassDef::Exception:  result+="исключени";
          if (single) result+='я'; else result+='й';
          break;
      }
      result+=" находятся в файл";
      if (single) result+="е:"; else result+="ах:";
      return decode(result);
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return decode("Алфавитный указатель"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return decode("Возвращаемые значения"); }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return decode("Титульная страница"); }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return decode("стр."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return decode("Исходные тексты.");
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return decode("См. определение в файле @1 строка @0");
    }
    virtual QCString trDefinedInSourceFile()
    {
      return decode("См. определение в файле @0");
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return decode("Уст.");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)decode("Граф связей класса ")+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return decode("Граф включаемых заголовочных файлов для ")+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return decode("Конструктор(ы)"); 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return decode("См. исходные тексты.");
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return decode("См. документацию.");
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return decode("Предусловие");
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return decode("Постусловие");
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return decode("Инвариант");
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return decode("Инициализатор");
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return decode("Исходные тексты");
    }
    virtual QCString trGraphicalHierarchy()
    {
      return decode("Иерархия классов. Графический вид.");
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return decode("см. графический вид.");
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return decode("см. текстовый вид.");
    }
    virtual QCString trPageIndex()
    {
      return decode("Алфавитный указатель тематических описаний");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return decode("Заметки");
    }
    virtual QCString trPublicTypes()
    {
      return decode("Открытые типы");
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Поля данных" );
      }
      else
      {
        return decode( "Открытые атрибуты" );
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return decode("Статические открытые данные");
    }
    virtual QCString trProtectedTypes()
    {
      return decode("Защищенные типы");
    }
    virtual QCString trProtectedAttribs()
    {
      return decode("Защищенные данные");
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return decode("Статические защищенные данные");
    }
    virtual QCString trPrivateTypes()
    {
      return decode("Закрытые типы");
    }
    virtual QCString trPrivateAttribs()
    {
      return decode("Закрытые данные");
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return decode("Закрытые статические данные");
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    /*??*/
    {
      return decode("Необходимо сделать");
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    /*??*/
    {
      return decode("Список задач");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return decode("Используется в");
    }
    virtual QCString trRemarks()
    {
      return decode("Прим.");
    }
    virtual QCString trAttention()
    {
      return decode("Внимание");
    }
    virtual QCString trInclByDepGraph()
    {
      return decode("Граф файлов, в которые включается этот файл:");
    }
    virtual QCString trSince()
    /*??*/
    {
      return decode("Начиная с");
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
        "</ul>\n");
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return decode("см. легенду");
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
      return decode("Список тестов");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return decode("DCOP Методы");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return decode("Свойства");
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return decode("Полный список свойств");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return decode("Интерфейсы");
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode( "Структуры данных" );
      }
      else
      {
        return decode( "Классы" );
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
      return decode("Полный список пакетов ");
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return decode("Полный список документированных пакетов.");
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return decode("Пакеты");
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return decode("Пакеты");
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return decode("Макроопределение:");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return decode("Ошибка");
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return decode("Ошибки");
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
      return decode("Алфавитный указатель");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        QCString result((first_capital ? "Структуры данных" : "структуры данных"));
        return decode(result); 
      }
      else
      {
        QCString result((first_capital ? "Класс" : "класс"));
        if(!singular) result+="ы";
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
      if (!singular)  result+="ы";
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Пространств" : "пространств"));
      result+=(singular?"о имен":"а имен");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Групп" : "групп"));
      result+=(singular ? "а" : "ы");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Страниц" : "страниц"));
      result+=(singular ? "а" : "ы");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Член" : "член"));
      if (!singular)  result+="ы";
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
      QCString result((first_capital ? "Глобальны" : "глобальны"));
      result+=(singular ? "й" : "е");
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
      if (!singular) result+="ы";
      return decode(result); 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return decode("Перекрестные ссылки");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return decode("Замещает ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return decode("Замещается в ")+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return decode("Оглавление");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return decode( "Список устаревших определений и описаний" );
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return decode( "События" );
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return decode( "Cобытия" );
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return decode( "Типы с областью видимости пакета" );
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return decode( "Функции с областью видимости пакета" );
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return decode( "Статические функции с областью видимости пакета" );
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return decode( "Переменные с областью видимости пакета" );
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return decode( "Статические переменные с областью видимости пакета" );
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return decode( "Указатель" );
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return decode( "Граф вызовов:" );
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
      return decode( "Поиск" );
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return decode( "Результаты поиска" );
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
        return decode( "К сожалению, по Вашему запросу ничего не найдено." );
      }
      else if( numDocuments == 1 )
      {
        return decode( "Найден 1 документ." );
      }
      else 
      {
        return decode( "Найден(о) <b>$num</b> документ(ов). "
          "Документы отсортированы по релевантности." );
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return decode( "Найдено:" );
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Исходный файл " + filename;
    }
};

#endif
