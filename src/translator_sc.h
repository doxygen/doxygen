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

#ifndef TRANSLATOR_SC_H
#define TRANSLATOR_SC_H

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
class TranslatorSerbianCyrillic : public TranslatorAdapter_1_6_0
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
    { return "serbian-cyrillic"; }

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
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[serbianc]{babel}\n";
    }
    QCString latexFontenc() override
    {
      return "";
    }
    QCString trISOLang() override
    {
      return "sr-Cyrl";
    }
    QCString getLanguageString() override
    {
      return "0xC1A Serbian (Serbia, Cyrillic)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Повезане функције"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Напомињемо да ово нису функције чланице.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Опширније"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Детаљи"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Документација дефиниције типа"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Документација члана набрајања"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Документација функције чланице"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Документација поља";
      }
      else
      {
        return "Документација атрибута";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Још..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Списак свих чланова"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Списак чланова"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Ово је списак свих чланова од"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", укључујући све наслеђене чланове."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Аутоматски направљено помоћу Doxygen-а";
      if (!s.isEmpty()) result+=" за "+s;
      result+=" из изворног кода.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "назив набрајања"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "вредност набрајања"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "дефинисано у"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Модули"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Хијерархија класа"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуре";
      }
      else
      {
        return "Списак класа";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Списак датотека"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Поља";
      }
      else
      {
        return "Чланови класе";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Глобално";
      }
      else
      {
        return "Чланови датотеке";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Повезане странице"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Примери"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Тражи"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Овај списак наслеђивања је уређен "
             "скоро по абецеди:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="датотека са кратким описима:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Овде су структуре са кратким описима:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Овде су класе са кратким описима:";
      }
      else
      {
        return "Овде су класе, структуре, "
               "уније и интерфејси са кратким описима:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Овде је списак свих ";
      if (!extractAll)
      {
        result+="документованих ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trExamplesDescription() override
    { return "Овде је списак свих примера:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Овде је списак свих повезаних страница документације:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Овде је списак свих модула:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Документација"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Индекс модула"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Хијерархијски индекс"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Индекс датотека"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Документација модула"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Документација структуре";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Документација класе";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Документација датотеке"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Приручник"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Дефиниције"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Дефиниције типова"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Набрајања"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Функције"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Променљиве"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Вредности набрајања"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Документација дефиниције"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Документација дефиниције типа"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Документација набрајања"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Документација функције"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Документација променљиве"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Направљено "+date;
      if (!projName.isEmpty()) result+=" за "+projName;
      result+=" помоћу";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Дијаграм наслеђивања за "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Упозорење"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Верзија"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Датум"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Враћа"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Види"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Параметри"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Изизеци"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Направљено помоћу"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Списак простора имена"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="простора имена са кратким описима:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Пријатељи"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Документација за пријатеље и повезане функције"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      if (isTemplate)
      {
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
          default: break;
        }
      }
      else
      {
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
          default: break;
        }
      }
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" Референца датотеке";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Референца простора имена";
      return result;
    }

    QCString trPublicMembers() override
    { return "Јавне функције чланице"; }
    QCString trPublicSlots() override
    { return "Јавни слотови"; }
    QCString trSignals() override
    { return "Сигнали"; }
    QCString trStaticPublicMembers() override
    { return "Статичке јавне функције чланице"; }
    QCString trProtectedMembers() override
    { return "Заштићене функције чланице"; }
    QCString trProtectedSlots() override
    { return "Заштићени слотови"; }
    QCString trStaticProtectedMembers() override
    { return "Статичке заштићене функције чланице"; }
    QCString trPrivateMembers() override
    { return "Приватне функције чланице"; }
    QCString trPrivateSlots() override
    { return "Приватни слотови"; }
    QCString trStaticPrivateMembers() override
    { return "Статичке приватне функције чланице"; }

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
      return "Наслеђује "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Наслеђују "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Поново имплементирано од "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Поново имплементирано у "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Чланови простора имена"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Индекс простора имена"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Документација простора имена"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Простори имена"; }

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
      QCString result="Документација за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="ову класу"; break;
        case ClassDef::Struct:     result+="ову структуру"; break;
        case ClassDef::Union:      result+="ову унију"; break;
        case ClassDef::Interface:  result+="овај интерфејс"; break;
        case ClassDef::Protocol:   result+="овај протокол"; break;
        case ClassDef::Category:   result+="ову категорију"; break;
        case ClassDef::Exception:  result+="овај изузетак"; break;
        default: break;
      }
      result+=" је произведена из";
      if (single) result+="следеће датотеке:"; else result+="следећих датотека:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Враћене вредности"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Главна страница"; }

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
      return "Дефиниција у линији @0 датотеке @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Дефиниција у датотеци @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Застарело";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Дијаграм сарадње за "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Дијаграм зависности укључивања за "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Документација конструктора и деструктора";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Иди на изворни код овог фајла.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Иди на документацију овог фајла.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Услов пре";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Услов после";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Инваријанта";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Почетна вредност:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "код";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Графичка хијерархија класа";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Иди на графичку хијерархију класа";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Иди на текстуалну хијерархију класа";
    }
    QCString trPageIndex() override
    {
      return "Индекс страна";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Напомена";
    }
    QCString trPublicTypes() override
    {
      return "Јавни типови";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Поља";
      }
      else
      {
        return "Јавни атрибути";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Статички јавни атрибути";
    }
    QCString trProtectedTypes() override
    {
      return "Заштићени типови";
    }
    QCString trProtectedAttribs() override
    {
      return "Заштићени атрибути";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Статички заштићени атрибути";
    }
    QCString trPrivateTypes() override
    {
      return "Приватни типови";
    }
    QCString trPrivateAttribs() override
    {
      return "Приватни атрибути";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Статички приватни атрибути";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Урадити";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Подсетник шта још урадити";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Референцирано од";
    }
    QCString trRemarks() override
    {
      return "Напомене";
    }
    QCString trAttention() override
    {
      return "Пажња";
    }
    QCString trInclByDepGraph() override
    {
      return "Овај граф показује које датотеке директно или "
             "или индиректно укључују овај фајл:";
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
      return "Легенда графова";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
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
    QCString trLegend() override
    {
      return "легенда";
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
      return "Списак тестова";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Своства";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Документација свосјтва";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Структуре";
      }
      else
      {
        return "Класе";
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
      return "Овде су пакети са кратким описима (ако су доступни):";
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
      return "Грешка";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
      return "Индекс";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "клас", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "датотек", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "простор", "и имена", " имена");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "груп", "е", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "страниц", "е", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "члан", "ови");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "глобалн", "а", "о");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "аутор", "и");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Референце";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Имплементира "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Имплементирано у "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Садржај";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Списак застарелог";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Догађаји";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Документација догажаја";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Типови пакета";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Функције пакета";
    }
    QCString trPackageMembers() override
    {
      return "Чланови пакета";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Статичке функције пакета";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Атрибути пакета";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Статички атрибути пакета";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Све";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Овде је граф позивања за ову функцију:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Поготци:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " Изворна датотека";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Хијерархија директоријума"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Документација директоријума"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Директоријуми"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Референца директоријума"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "директоријум", "и");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Ово је преоптерећена функција чланица. "
              "Разликује се од наведене само по врсти аргумената кое прихвата";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Ово је граф функција које позивају ову функцију:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Документација вредности набрајања"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Документацијаr функције чланице, односно потпрограма члана"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Списак типова података"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Поља"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Овде су типови података са кратким описима:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Индекс типова података"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Документација типова података"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Функције/потпрограми"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Документација функције/потпрограма"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Типови података"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Списак модула"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Овде је списак свих ";
      if (!extractAll) result+="документованих ";
      result+="модула са кратким описима:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
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
        default: break;
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Референца модула";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Чланови модула"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Индекс модула"; }

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
      QCString result="Документација за ";
      switch(compType)
      {
        case ClassDef::Class:      result+="овај модул"; break;
        case ClassDef::Struct:     result+="овај тип"; break;
        case ClassDef::Union:      result+="ову унију"; break;
        case ClassDef::Interface:  result+="овај интерфејс"; break;
        case ClassDef::Protocol:   result+="овај протокол"; break;
        case ClassDef::Category:   result+="ову категорију"; break;
        case ClassDef::Exception:  result+="овај изузетак"; break;
        default: break;
      }
      result+=" је направљен из следећ";
      if (single) result+="е датотеке:"; else result+="их датотека:";
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
      return createNoun(first_capital, singular, "потпрограм", "и");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Ограничења типова";
    }

    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[] = { "пон", "уто", "сри", "чет", "пет", "суб", "нед" };
      static const char *days_full[] = { "Понедељак", "Уторак", "Среда", "Четвртак", "Петак", "Субота", "Недеља" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Јан", "Феб", "Мар", "Апр", "Мај", "Јун", "Јул", "Авг", "Сеп", "Окт", " новембар", "дец" };
      static const char *months_full[] = { "јануар", "фебруар", "март", "април", "мај", "јун", "јул", "август", "септембар", "октобар", " Новембар Децембар" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "пре подне", "по подне" };
      return dayPeriod[period?1:0];
    }
};

#endif
