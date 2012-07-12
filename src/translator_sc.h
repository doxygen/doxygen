/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_SC_H
#define TRANSLATOR_SC_H

/*! 
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionaly to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorSerbianCyrilic : public TranslatorAdapter_1_6_0
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
    { return "serbiancyr"; }
    
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
      return "";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Повезане функције"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Напомињемо да ово нису функције чланице.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Опширније"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Документација дефиниције типа"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Документација члана набрајања"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Документација функције чланице"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Документација поља"; 
      }
      else
      {
        return "Документација атрибута"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Још..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Списак свих чланова"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Списак чланова"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Ово је списак свих чланова од "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", укључујући све наслеђене чланове."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Аутоматски направљено помоћу Doxygen-а";
      if (s) result+=(QCString)" за "+s;
      result+=" из изворног кода."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "назив набрајања"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "вредност набрајања"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "дефинисано у"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Модули"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Хијерархија класа"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Структуре";
      }
      else
      {
        return "Списак класа"; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Списак датотека"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Поља"; 
      }
      else
      {
        return "Чланови класе"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Глобално"; 
      }
      else
      {
        return "Чланови датотеке"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Повезане странице"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Примери"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Тражи"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Овај списак наслеђивања је уређен "
             "скоро по абецеди:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="датотека са кратким описима:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Овде су структуре са кратким описима:"; 
      }
      else
      {
        return "Овде су класе, структуре, "
               "уније и интерфејси са кратким описима:"; 
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll)
      {
        result+="документованих ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="поља структура и унија";
      }
      else
      {
        result+="чланова класа";
      }
      result+=" са везама ка ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="документацији структуре/уније за свако поље:";
        }
        else
        {
          result+="документацији класе за сваки члан:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="структури/унији којој припадају:";
        }
        else
        {
          result+="класи којој припадају:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="фрункција, променљивих, макро замена, набрајања, и дефиниција типова";
      }
      else
      {
        result+="чланова датотеке";
      }
      result+=" са везама ка ";
      if (extractAll) 
        result+="датотекама којима припадају:";
      else 
        result+="документацији:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Овде је списак свих примера:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Овде је списак свих повезаних страница документације:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Овде је списак свих модула:"; }

    // index titles (the project name is prepended for these) 

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Документација"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Индекс модула"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Хијерархијски индекс"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Индекс структура";
      }
      else
      {
        return "Индекс класа"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Индекс датотека"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Документација модула"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Документација структуре"; 
      }
      else
      {
        return "Документација класе"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Документација датотеке"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Документација примера"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Документација странице"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Приручник"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Дефиниције"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Декларације функција"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Дефиниције типова"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Набрајања"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Функције"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Променљиве"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Вредности набрајања"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Документација дефиниције"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Документација декларације функције"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Документација дефиниције типа"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Документација набрајања"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Документација функције"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Документација променљиве"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Структуре"; 
      }
      else
      {
        return "Класе"; 
      }
    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Направљено "+date;
      if (projName) result+=(QCString)" за "+projName;
      result+=(QCString)" помоћу";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "написао";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Дијаграм наслеђивања за "+clName+":";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Само за унутрашњу употребу."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Упозорење"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Верзија"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Датум"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Враћа"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Види"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Параметри"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Изизеци"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Направљено помоћу"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Списак простора имена"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="простора имена са кратким описима:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Пријатељи"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Документација за пријатеље и повезане функције"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      if (isTemplate) {
	result+=" Шаблон";
	switch(compType)
	{
		case ClassDef::Class:      result+="ска класа"; break;
		case ClassDef::Struct:     result+="ска структура"; break;
		case ClassDef::Union:      result+="ска унија"; break;
		case ClassDef::Interface:  result+="ски интерфејс"; break;
		case ClassDef::Protocol:   result+="ски протокол"; break;
		case ClassDef::Category:   result+="ска категорија"; break;
		case ClassDef::Exception:  result+="ски изузетак"; break;
	}
      } else {
	result+=" Референца";
	switch(compType)
	{
		case ClassDef::Class:      result+=" класе"; break;
		case ClassDef::Struct:     result+=" структуре"; break;
		case ClassDef::Union:      result+=" уније"; break;
		case ClassDef::Interface:  result+=" интерфејса"; break;
		case ClassDef::Protocol:   result+=" протокола"; break;
		case ClassDef::Category:   result+=" категорије"; break;
		case ClassDef::Exception:  result+=" изузетка"; break;
	}
      }
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Референца датотеке"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Референца простора имена";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Јавне функције чланице"; }
    virtual QCString trPublicSlots()
    { return "Јавни слотови"; }
    virtual QCString trSignals()
    { return "Сигнали"; }
    virtual QCString trStaticPublicMembers()
    { return "Статичке јавне функције чланице"; }
    virtual QCString trProtectedMembers()
    { return "Заштићене функције чланице"; }
    virtual QCString trProtectedSlots()
    { return "Заштићени слотови"; }
    virtual QCString trStaticProtectedMembers()
    { return "Статичке заштићене функције чланице"; }
    virtual QCString trPrivateMembers()
    { return "Приватне функције чланице"; }
    virtual QCString trPrivateSlots()
    { return "Приватни слотови"; }
    virtual QCString trStaticPrivateMembers()
    { return "Статичке приватне функције чланице"; }
    
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
      return "Наслеђује "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Наслеђују "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Поново имплементирано од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Поново имплементирано у "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Чланови простора имена"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="чланова простора имена са везама ка ";
      if (extractAll) 
        result+="документацији простора имена за сваки члан:";
      else 
        result+="просторима имена којима припадају:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Индекс простора имена"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Документација простора имена"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Простори имена"; }

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
      QCString result=(QCString)"Документација за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="ову класу"; break;
        case ClassDef::Struct:     result+="ову структуру"; break;
        case ClassDef::Union:      result+="ову унију"; break;
        case ClassDef::Interface:  result+="овај интерфејс"; break;
        case ClassDef::Protocol:   result+="овај протокол"; break;
        case ClassDef::Category:   result+="ову категорију"; break;
        case ClassDef::Exception:  result+="овај изузетак"; break;
      }
      result+=" је произведена из";
      if (single) result+="следеће датотеке:"; else result+="следећих датотека:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Абецедни списак"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Враћене вредности"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Главна страница"; }

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
      return "Дефиниција у линији @0 датотеке @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Дефиниција у датотеци @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Застарело";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Дијаграм сарадње за "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Дијаграм зависности укључивања за "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Документација конструктора и деструктора"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Иди на изворни код овог фајла.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Иди на документацију овог фајла.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Услов пре";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Услов после";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Инваријанта";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Почетна вредност:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "код";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Графичка хијерархија класа";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Иди на графичку хијерархију класа";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Иди на текстуалну хијерархију класа";
    }
    virtual QCString trPageIndex()
    {
      return "Индекс страна";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Напомена";
    }
    virtual QCString trPublicTypes()
    {
      return "Јавни типови";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Поља";
      }
      else
      {
        return "Јавни атрибути";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Статички јавни атрибути";
    }
    virtual QCString trProtectedTypes()
    {
      return "Заштићени типови";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Заштићени атрибути";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Статички заштићени атрибути";
    }
    virtual QCString trPrivateTypes()
    {
      return "Приватни типови";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Приватни атрибути";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Статички приватни атрибути";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Урадити";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Подсетник шта још урадити";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Референцирано од";
    }
    virtual QCString trRemarks()
    {
      return "Напомене";
    }
    virtual QCString trAttention()
    {
      return "Пажња";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Овај граф показује које датотеке директно или "
             "или индиректно укључују овај фајл:";
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
      return "Легенда графова";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "Ова страница објашњава како тумачити графове који су направљени "
        "doxygen-ом.<p>\n"
        "Размотримо следећи пример:\n"
        "\\code\n"
        "/*! Невидљива класа због одсецања */\n"
        "class Invisible { };\n\n"
        "/*! Одсечена класа, веза наслеђивања је скривена */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Класа која није документована doxygen коментарима */\n"
        "class Undocumented { };\n\n"
        "/*! Класа која је наслеђена јавним наслеђивањем */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Шаблонска класа */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Класа која је наслеђена заштићеним наслеђивањем */\n"
        "class ProtectedBase { };\n\n"
        "/*! Класа која је наслеђена јавним наслеђивањем */\n"
        "class PrivateBase { };\n\n"
        "/*! Класа коју користи наслеђена класа */\n"
        "class Used { };\n\n"
        "/*! Надкласа која наслеђује неки број других класа */\n"
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
        "Ако је \\c MAX_DOT_GRAPH_HEIGHT таг у конфигурационој датотеци "
        "подешен на 240, то ће резултовати на следећи начин:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Правоугаоници имају следеће значење:\n"
        "<ul>\n"
        "<li>%Пуни сиви правоугаоник представља структуру или класу за коју је "
        "граф направљен.\n"
        "<li>%Правоугаоник са црним оквиром означава документовану структуру или класу.\n"
        "<li>%Правоугаоник са сивим оквиром означава недокументовану структуру или класу.\n"
        "<li>%Правоугаоник са црвеним оквиром означава документовану структуру или класу за"
        "за коју нису све релације наслеђивања/садржавања приказане. %Граф је "
        "одсечен ако излази из специфицираних оквира.\n"
        "</ul>\n"
        "Стрелице имају следећа значења:\n"
        "<ul>\n"
        "<li>%Тамноплава стрелица се користи да прикаже релацију јавног извођења "
        "између двеју класа.\n"
        "<li>%Тамнозелена стрелица се користи за заштићено наслеђивање.\n"
        "<li>%Тамноцрвена стрелица се користи за приватно наслеђивање.\n"
        "<li>%Љубичаста испрекидана стрелица се користи ако класа садржи или користи "
        "друга класа. Стрелица је означена променљивом/променљивама "
        "кроз које је показивана класа или структура доступна.\n"
        "<li>%Жута испрекидана стрелица означава везу између примерка шаблона и "
        "и шаблонске класе из које је инстанцирана. Стрелица је означена "
        "параметрима примерка шаблона.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "легенда";
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
      return "Списак тестова";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP функције чланице";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Своства";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Документација свосјтва";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Структуре";
      }
      else
      {
        return "Класе";
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
      return "Списак пакета";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Овде су пакети са кратким описима (ако су доступни):";
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
      return "Грешка";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Списак грешака";
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
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Индекс";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Клас" : "клас"));
      if (!singular)  result+="e"; else result+="a";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Датотек" : "датотек"));
      if (!singular)  result+="e"; else result+="a";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Простор" : "простор"));
      if (!singular)  result+="и имена"; else result+=" имена";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Груп" : "груп"));
      if (!singular)  result+="е"; else result+="a";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Страниц" : "страниц"));
      if (!singular)  result+="е"; else result += "a";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Члан" : "члан"));
      if (!singular)  result+="ови";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Глобалн" : "глобалн"));
      if (!singular)  result+="а"; else result+="о";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Аутор" : "аутор"));
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
      return "Референце";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Имплементира "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Имплементирано у "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Садржај";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Списак застарелог";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Догађаји";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Документација догажаја";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Типови пакета";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Функције пакета";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Статичке функције пакета";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Атрибути пакета";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Статички атрибути пакета";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Све";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Овде је граф позивања за ову функцију:";
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
      return "Тражим";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Резултати претраге";
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
        return "Жао ми је, али нема докумената који одговарају упиту.";
      }
      else if (numDocuments==1)
      {
        return "Пронађен <b>1</b> документ који одговара упиту.";
      }
      else if (numDocuments==2)
      {
        return "Пронађена <b>а</b> документа која одговарају упиту.";
      }
      else if (numDocuments==3)
      {
        return "Пронађена <b>3</b> документа која одговарају упиту.";
      }
      else if (numDocuments==4)
      {
        return "Пронађена <b>4</b> документа која одговарају упиту.";
      }
      else 
      {
        return "Пронађено <b>$num</b> докумената који одговарају упиту. "
               "Приказују се прво најбољи поготци.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Поготци:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Изворна датотека";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Хијерархија директоријума"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Документација директоријума"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Директоријуми"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Ова хијерархија директоријума је уређена "
             "приближно по абецеди:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Референца директоријума"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Директоријум" : "директоријум"));
      if (singular) result+=""; else result+="и";
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
       return "Ово је преоптерећена функција чланица. "
              "Разликује се од наведене само по врсти аргумената кое прихвата";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Ово је граф функција које позивају ову функцију:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Документација вредности набрајања"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Документацијаr функције чланице, односно потпрограма члана"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Списак типова података"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Поља"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Овде су типови података са кратким описима:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll)
      {
        result+="документованих ";
      }
      result+="чланова типова података";
      result+=" са везама ка ";
      if (!extractAll) 
      {
         result+="документацији структуре података за сваки члан";
      }
      else 
      {
         result+="типовима података којима припадају:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Индекс типова података"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Документација типова података"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Функције/потпрограми"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Документација функције/потпрограма"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Типови података"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Списак модула"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="модула са кратким описима:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      if (isTemplate) result+=" Шаблон";
      result+=" Референца";
      switch(compType)
      {
        case ClassDef::Class:      result+=" модула"; break;
        case ClassDef::Struct:     result+=" типа"; break;
        case ClassDef::Union:      result+=" уније"; break;
        case ClassDef::Interface:  result+=" интерфејса"; break;
        case ClassDef::Protocol:   result+=" протокола"; break;
        case ClassDef::Category:   result+=" категорије"; break;
        case ClassDef::Exception:  result+=" изузетка"; break;
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Референца модула";        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Чланови модула"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="чланова модула са везама ка ";
      if (extractAll) 
      {
        result+="документацији модула за сваки члан:";
      }
      else 
      {
        result+="модулима којима припадају:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Индекс модула"; }
    
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
      QCString result=(QCString)"Документација за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="овај модул"; break;
        case ClassDef::Struct:     result+="овај тип"; break;
        case ClassDef::Union:      result+="ову унију"; break;
        case ClassDef::Interface:  result+="овај интерфејс"; break;
        case ClassDef::Protocol:   result+="овај протокол"; break;
        case ClassDef::Category:   result+="ову категорију"; break;
        case ClassDef::Exception:  result+="овај изузетак"; break;
      }
      result+=" је направљен из следећ";
      if (single) result+="е датотеке:"; else result+="их датотека:";
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
      if (!singular)  result+="и";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Ограничења типова";
    }

};

#endif
