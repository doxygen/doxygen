/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
 * Alexandr V. Chelpanov <cav@cryptopro.ru> released Apr 21, 2001
 *
 * Доступны так же перевод примеров на русский язык по email (см. выше)
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
 */

#ifndef TRANSLATOR_RU_H
#define TRANSLATOR_RU_H

#include "translator.h"

class TranslatorRussian : public Translator
{
  protected:
    /*! Returns the string converted from koi8-r to windows-1251. */
    /* The method was designed initially for translator_cz.h. 
       It is used for on-line encoding conversion related to conditional
       compilation in Unix/MS Windows environments (both use different
       encoding). 
       Encoding table got from QT:qtextcodec.cpp
     */
    QCString Koi8RToWindows1251( const QCString sInput )
    {
      static Q_UINT16 koi8_r[128] = 
      { 0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
        0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
        0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219/**/, 0x221A, 0x2248,
        0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
        0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
        0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255D, 0x255E,
        0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
        0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0x00A9,
        0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
        0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
        0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
        0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
        0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
        0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
        0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
        0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A 
      };

      QString result;
      int len = sInput.length();

      result.setUnicode(0, len);
      QChar* uc = (QChar*)result.unicode(); // const_cast
      const unsigned char * c = (const unsigned char *)(const char*)sInput;
      for( int i=0; i<len; i++ ) {
        if ( c[i] > 127 )
	  uc[i] = koi8_r[c[i]-128];
        else
	  uc[i] = c[i];
      }
      return result.local8Bit();
    }
    /*! returns the string converted from Windows-1251 to koi8-r */
    /* See the comments of the Koi8RToWindows1251() method for details.
       Encoding table got from QT:qtextcodec.cpp */
    QCString Windows1251ToKoi8R( const QCString sInput )
    {
      static Q_UINT16 windows_1251[128] = 
      { 0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
        0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
        0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0xFFFD, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
        0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
        0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
        0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
        0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
        0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
        0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
        0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
        0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
        0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
        0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
        0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
        0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
      };

      QString result;
      int len = sInput.length();

      result.setUnicode(0, len);
      QChar* uc = (QChar*)result.unicode(); // const_cast
      const unsigned char * c = (const unsigned char *)(const char*)sInput;
      for( int i=0; i<len; i++ ) {
        if ( c[i] > 127 )
	  uc[i] = windows_1251[c[i]-128];
        else
	  uc[i] = c[i];
      }
      return result.local8Bit();
    }

  private:
    /*! The Decode() inline assumes the source written in the 
        Koi8-R encoding (maintainer dependent). 
     */
    inline QCString decode(const QCString & sInput)
    { 
#ifdef _WIN32
      return Koi8RToWindows1251(sInput);
#else
      return sInput;
#endif
    }

  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage()
    { return "russian"; }

    /* Used to get the command(s) for the language support. */
    // virtual QCString latexLanguageSupportCommand()

    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    virtual QCString latexBabelPackage()
    { return "russianb"; }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
#ifdef _WIN32
    { return "Windows-1215"; }
#else
    { return "koi8-r"; }
#endif

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
    { return decode("Полный список членов класса"); }

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

    // TODO: trVerbatimText is no longer used => remove!
    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    virtual QCString trVerbatimText(const char *f)
    { return decode( (QCString)"Это прямая вставка текста из включаемого файла")+f; }

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
      return decode( "Члены файла" );
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
	if(extractAll)
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

    /*! This is used in man pages as the author section. */
    virtual QCString trAuthor()
    { return decode("Автор"); }

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
    virtual QCString trFiles()
    { return decode("Файлы"); }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=decode("Документация ");
      if (projName) result+=decode("по ")+projName;
      result+=decode(". Последние изменения: ")+date;
      result+=decode(". Создано системой ");
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

    /*! this text is generated when the \\author command is used. */
    virtual QCString trAuthors()
    { return decode("Автор(ы)"); }

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
    { return decode("Документация по друзьям класса и функциям отноносящихся"
	"к классу"); }
    
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
    { return decode("Публичные члены"); }
    virtual QCString trPublicSlots()
    { return decode("Публичные слоты"); }
    virtual QCString trSignals()
    { return decode("Сигналы"); }
    virtual QCString trStaticPublicMembers()
    { return decode("Публичные статические члены"); }
    virtual QCString trProtectedMembers()
    { return decode("Защищенные члены"); }
    virtual QCString trProtectedSlots()
    { return decode("Защищенные слоты"); }
    virtual QCString trStaticProtectedMembers()
    { return decode("Защищенные статические члены"); }
    virtual QCString trPrivateMembers()
    { return decode("Приватные члены"); }
    virtual QCString trPrivateSlots()
    { return decode("Приватные слоты"); }
    virtual QCString trStaticPrivateMembers()
    { return decode("Приватные статические члены"); }
    
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
      return decode("Переопределяет метод предка ")+trWriteList(numEntries)+".";
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
    { return decode("Члены пространства имен"); }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Полный список ";
      if (!extractAll) result+="документированных ";
      result+="членов простанства имен.";
      return decode(result);
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return decode("Алфавитный указатель пространства имен"); }

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
    /*! Text for the \pre command */
    virtual QCString trPrecondition()
    {
      return decode("Предусловие");
    }
    /*! Text for the \post command */
    virtual QCString trPostcondition()
    {
      return decode("Постусловие");
    }
    /*! Text for the \invariant command */
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
      return decode("Публичные типы");
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
      return decode("Статические публичные данные");
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
      return decode("Приватные типы");
    }
    virtual QCString trPrivateAttribs()
    {
      return decode("Приватные данные");
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return decode("Приватные статические данные");
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
      return decode("Граф файлов, в которые включеется этот файл:");
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
        "/*! Публичное наследование */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Защищенное наследование */\n"
        "class ProtectedBase { };\n\n"
        "/*! Приватное наследование */\n"
        "class PrivateBase { };\n\n"
        "/*! Класс, используемый классом Inherited */\n"
        "class Used { };\n\n"
        "/*! Класс, порожденный от других классов */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Если \\c MAX_DOT_GRAPH_HEIGHT в конфигурационном файле "
        "установлен в 200, получится следующий граф:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Прямоугольники в этом графе имеют следующее значение:\n"
        "<ul>\n"
        "<li>Заполненный чернный прямоугольник представляет структуру или класс, "
	"для которого создан граф.\n"
        "<li>Прямоугольник с черной границей обозначает документированную структуру или класс.\n"
        "<li>Прямоугольник с серой границей обозначает недокументированную структуру или класс.\n"
        "<li>Прямоугольник с красной границей обозначает документированную структуру или класс, для которого\n"
        " не все отношения наследования/содержания показаны. Граф усечен, "
        "если он не поместился в указанных границах."
        "</ul>\n"
        "Стрелки имеют следующее значение:\n"
        "<ul>\n"
        "<li>Темносиняя стрелка используется для изображения отношения публичного наследования "
        "между двумя классами.\n"
        "<li>Темнозеленая стрелка используется при защищенном наследовании.\n"
        "<li>Темнокрасная стрелка используется при приватном наследовании.\n"
        "<li>Фиолетовая стрелка используется, если класс содержится в"
	"другом класе или используется другим классом."
        "Со стрелкой указывается переменная, "
        "через которую доступен указываемый класс или структура. \n"
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
      return decode("Классы");
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
// new since 1.2.?
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
};

#endif
