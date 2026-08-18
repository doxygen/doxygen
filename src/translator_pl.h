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
 * Polish translation was updated to version 1.3.9 by
 * Piotr Kaminski (Piotr.Kaminski@ctm.gdynia.pl)
 * Polish translation was updated since version 1.4.6 by
 * Krzysztof Kral (krzysztof.kral@gmail.com)
 * Polish translation was updated to version 1.10.0 by
 * Marek Ledworowski (mledworo@gmail.com)
 */

#ifndef TRANSLATOR_PL_H
#define TRANSLATOR_PL_H

class TranslatorPolish : public Translator
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. May resemble
     * the string returned by latexBabelPackage(), but it is not used
     * for the same purpose. The identification should not be translated.
     * It should be replaced by the name of the language in English
     * (e.g. Czech, Japanese, Russian, etc.). It should be equal to
     * the identification in language.h.
     */
    DString idLanguage() override
    { return "polish"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage[polish]{babel}\n"
             "\\let\\lll\\undefined\n"; /* to overcome problems with the <<<
                                           symbol as defined in the amssymb
                                           package, and the Polish symbol
                                           "Latin Small Letter L With Stroke"
                                           &lstrok; or &#322; or &#x0142;
                                           We take the amssymb symbol as leading.
                                         */
    }

    DString trISOLang() override
    {
      return "pl";
    }
    DString getLanguageString() override
    {
      return "0x415 Polish";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Funkcje powiązane"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Zauważ, że to nie są metody klas.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Opis szczegółowy"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Szczegóły"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Dokumentacja składowych definicji typu"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Dokumentacja składowych wyliczanych"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Dokumentacja funkcji/procedur/procesów składowych";
      }
      else
      {
        return "Dokumentacja funkcji składowych";
      }
    }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentacja pól";
      }
      else
      {
        return "Dokumentacja atrybutów składowych";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Więcej..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Lista wszystkich składowych"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Lista składowych"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "To jest kompletna lista składowych dla"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", uwzględniająca wszystkie dziedziczone składowe."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Wygenerowano automatycznie z kodu źródłowego programem Doxygen";
      if (!s.empty()) result+=" dla "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "nazwa wyliczenia"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "wartość wyliczenia"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "zdefiniowana w"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Moduły"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Hierarchia klas"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Struktury danych";
      }
      else
      {
        return "Lista klas";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Lista plików"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Pola danych";
      }
      else
      {
        return "Składowe klas";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globalne";
      }
      else
      {
        return "Składowe plików";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Powiązane strony"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Przykłady"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Szukaj"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Ta lista dziedziczenia posortowana jest z grubsza, "
             "choć nie całkowicie, alfabetycznie:";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="plików wraz z ich krótkimi opisami:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tutaj znajdują się struktury danych wraz z ich krótkimi opisami:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Tutaj znajdują się klasy wraz z ich krótkimi opisami:";
      }
      else
      {
        return "Tutaj znajdują się klasy, struktury, "
             "unie i interfejsy wraz z ich krótkimi opisami:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll)
      {
        result+="udokumentowanych ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="pól struktur i unii";
      }
      else
      {
        result+="składowych";
      }
      result+=" wraz z odnośnikami do ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="dokumentacji struktur/unii dla każdego pola:";
        }
        else
        {
          result+="dokumentacji klas dla każdej składowej:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktur/unii, do których dane pole należy:";
        }
        else
        {
          result+="klas, do których dana składowa należy:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcji, zmiennych, definicji, wyliczeń i definicji typów";
      }
      else
      {
        result+="składowych plików";
      }
      result+=" wraz z odnośnikami do ";
      if (extractAll)
        result+="plików, do których one należą:";
      else
        result+="dokumentacji:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Tutaj znajduje się lista wszystkich przykładów:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Tutaj znajduje się lista wszystkich powiązanych stron dokumentacji:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Tutaj znajduje się lista wszystkich grup:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokumentacja"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Indeks grup"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Indeks hierarchiczny"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
     if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Indeks struktur danych";
      }
      else
      {
        return "Indeks klas";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Indeks plików"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Dokumentacja grup"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentacja struktur danych";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Dokumentacja klas";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Dokumentacja plików"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Podręcznik"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Definicje"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Definicje typów"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Wyliczenia"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funkcje"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Zmienne"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Wartości wyliczeń"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Dokumentacja definicji"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Dokumentacja definicji typów"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Dokumentacja typów wyliczanych"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Dokumentacja funkcji"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Dokumentacja zmiennych"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Struktury danych";
      }
      else
      {
        return "Komponenty";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Wygenerowano "+date;
      if (!projName.empty()) result+=" dla "+projName;
      result+=" za pomocą";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Diagram dziedziczenia dla "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Ostrzeżenie"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Wersja"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Zwraca"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Zobacz również"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametry"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Wyjątki"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Wygenerowano za pomocą"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Lista przestrzeni nazw"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Tutaj znajdują się wszystkie ";
      if (!extractAll) result+="udokumentowane ";
      result+="przestrzenie nazw wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Przyjaciele"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Dokumentacja przyjaciół i powiązanych funkcji"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Dokumentacja";
      if (isTemplate) result+=" szablonu";
      switch(compType)
      {
        case ClassDef::Class:      result+=" klasy "; break;
        case ClassDef::Struct:     result+=" struktury "; break;
        case ClassDef::Union:      result+=" unii "; break;
        case ClassDef::Interface:  result+=" interfejsu "; break;
        case ClassDef::Protocol:   result+=" protokołu "; break;
        case ClassDef::Category:   result+=" kategorii "; break;
        case ClassDef::Exception:  result+=" wyjątku "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result="Dokumentacja pliku ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result="Dokumentacja przestrzeni nazw ";
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    DString trPublicMembers() override
    { return "Metody publiczne"; }
    DString trPublicSlots() override
    { return "Sloty publiczne"; }
    DString trSignals() override
    { return "Sygnały"; }
    DString trStaticPublicMembers() override
    { return "Statyczne metody publiczne"; }
    DString trProtectedMembers() override
    { return "Metody chronione"; }
    DString trProtectedSlots() override
    { return "Sloty chronione"; }
    DString trStaticProtectedMembers() override
    { return "Statyczne metody chronione"; }
    DString trPrivateMembers() override
    { return "Metody prywatne"; }
    DString trPrivateSlots() override
    { return "Sloty prywatne"; }
    DString trStaticPrivateMembers() override
    { return "Statyczne metody prywatne"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    DString trWriteList(int numEntries) override
    {
      DString result;
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
            result+=" i ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Dziedziczy "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Dziedziczona przez "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementowana z "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Reimplementowana w "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Składowe przestrzeni nazw"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="składowych wraz z odnośnikami do ";
      if (extractAll)
        result+="dokumentacji przestrzeni nazw dla każdej składowej:";
      else
        result+="przestrzeni nazw do których składowe te należą:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Indeks przestrzeni nazw"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Dokumentacja przestrzeni nazw"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Przestrzenie nazw"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="Dokumentacja dla te";
      switch(compType)
      {
        case ClassDef::Class:      result+="j klasy"; break;
        case ClassDef::Struct:     result+="j struktury"; break;
        case ClassDef::Union:      result+="j unii"; break;
        case ClassDef::Interface:  result+="go interfejsu"; break;
        case ClassDef::Protocol:   result+="go protokołu"; break;
        case ClassDef::Category:   result+="j kategorii"; break;
        case ClassDef::Exception:  result+="go wyjątku"; break;
        default: break;
      }
      result+=" została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Zwracane wartości"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Strona główna"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "str."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Definicja w linii @0 pliku @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Definicja w pliku @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Do wycofania";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Diagram współpracy dla "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Wykres zależności załączania dla "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Dokumentacja konstruktora i destruktora";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Idź do kodu źródłowego tego pliku.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Idź do dokumentacji tego pliku.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Warunek wstępny";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Warunek końcowy";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Niezmiennik";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Wartość początkowa:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "kod źródłowy";
    }
    DString trGraphicalHierarchy() override
    {
      return "Graficzna hierarchia klas";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Idź do graficznej hierarchii klas";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Idź do tekstowej hierarchii klas";
    }
    DString trPageIndex() override
    {
      return "Indeks stron";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Nota";
    }
    DString trPublicTypes() override
    {
      return "Typy publiczne";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Pola danych";
      }
      else
      {
        return "Atrybuty publiczne";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Statyczne atrybuty publiczne";
    }
    DString trProtectedTypes() override
    {
      return "Typy chronione";
    }
    DString trProtectedAttribs() override
    {
      return "Atrybuty chronione";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Statyczne atrybuty chronione";
    }
    DString trPrivateTypes() override
    {
      return "Typy prywatne";
    }
    DString trPrivateAttribs() override
    {
      return "Atrybuty prywatne";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Statyczne atrybuty prywatne";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    {
      return "Do zrobienia";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Lista rzeczy do zrobienia";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Odwołania w";
    }
    DString trRemarks() override
    {
      return "Spostrzeżenia";
    }
    DString trAttention() override
    {
      return "Uwaga";
    }
    DString trInclByDepGraph() override
    {
      return "Ten wykres pokazuje, które pliki bezpośrednio lub "
             "pośrednio załączają ten plik:";
    }
    DString trSince() override
    {
      return "Od";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Legenda wykresu";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Ta strona wyjaśnia jak interpretować wykresy, które są wygenerowane "
        "przez doxygen.<p>\n"
        "Rozważ następujący przykład:\n"
        "\\code\n"
        "/*! Klasa Niewidzialna z powodu okrojenia */\n"
        "class Niewidzialna { };\n\n"
        "/*! Klasa Okrojona, relacja dziedziczenia jest ukryta */\n"
        "class Okrojona : public Niewidzialna { };\n\n"
        "/* Klasa nie udokumentowana komentarzami doxygen */\n"
        "class Nieudokumentowana { };\n\n"
        "/*! Klasa, która jest dziedziczona publicznie */\n"
        "class PublicznaBaza : public Okrojona { };\n\n"
        "/*! Szablon klasy */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasa, która jest dziedziczona przy użyciu dziedziczenia chronionego */\n"
        "class ChronionaBaza { };\n\n"
        "/*! Klasa, która jest dziedziczona prywatnie */\n"
        "class PrywatnaBaza { };\n\n"
        "/*! Klasa, która jest użyta przez klasę Dziedziczona */\n"
        "class Uzyta { };\n\n"
        "/*! Superklasa, która dziedziczy kilka innych klas */\n"
        "class Dziedziczona : public PublicznaBaza,\n"
        "                     protected ChronionaBaza,\n"
        "                     private PrywatnaBaza,\n"
        "                     public Nieudokumentowana,\n"
        "                     public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Uzyta *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Rezultat na następującym wykresie:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Prostokąty w powyższym wykresie mają następujące znaczenie:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Wypełniony czarny prostokąt reprezentuje strukturę lub klasę dla "
        "której został wygenerowany wykres.</li>\n"
        "<li>Prostokąt z czarną obwolutą oznacza udokumentowaną strukturę lub klasę.</li>\n"
        "<li>Prostokąt z szarą obwolutą oznacza nieudokumentowaną strukturę lub klasę.</li>\n"
        "<li>Prostokąt z czerwoną obwolutą oznacza udokumentowaną strukturę lub klasę dla\n"
        "której nie są pokazane wszystkie relacje dziedziczenia/zawierania. Wykres jest "
        "okrojony, jeśli nie mieści się w określonych brzegach.</li>\n"
        "</ul>\n"
        "<p>\n"
        "Strzałki mają następujące znaczenie:\n"
        "<p>\n"
        "<ul>\n"
        "<li>Ciemno niebieska strzałka jest używana do wizualizacji relacji "
        "dziedziczenia publicznego pomiędzy dwiema klasami.</li>\n"
        "<li>Ciemno zielona strzałka jest używana dla dziedziczenia chronionego.</li>\n"
        "<li>Ciemno czerwona strzałka jest używana dla dziedziczenia prywatnego.</li>\n"
        "<li>Fioletowa przerywana strzałka jest używana jeśli klasa jest zawarta "
        "lub użyta przez inną klasę. Strzałka jest podpisana zmienną(ymi) "
        "przez które wskazywana klasa lub struktura jest dostępna. </li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Lista testów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Właściwości";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Dokumentacja właściwości";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Struktury Danych";
      }
      else
      {
        return "Klasy";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Pakiet "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Oto lista pakietów wraz z ich krótkimi opisami (jeśli są dostępne):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Pakiety";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Wartość:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Błąd";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Lista błędów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    DString trRTFansicp() override
    {
      return "1250";
    }

    /*! Used as ansicpg for RTF fcharset */
    DString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klas", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "plik", "i");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "przestrze", "nie", "ń") + " nazw";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupa", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stron", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "składow", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "ne", "ny");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "zy");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Odwołuje się do";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////


    DString trImplementedFromList(int numEntries) override
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    DString trImplementedInList(int numEntries) override
    {
      return "Implementowany w "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Spis treści";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Lista elementów do wycofania";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Zdarzenia";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Dokumentacja zdarzeń";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Typy pakietu";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Funkcje pakietu";
    }
    DString trPackageMembers() override
    {
      return "Składowe pakietu";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statyczne funkcje pakietu";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Atrybuty pakietu";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statyczne atrybuty pakietu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Wszystko";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Oto graf wywołań dla tej funkcji:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Wyniki szukania";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "Niestety żaden dokument nie pasuje do twojego zapytania.";
      }
      else if (numDocuments==1)
      {
        return "Znaleziono <b>1</b> dokument pasujący do twojego zapytania.";
      }
      int count = numDocuments % 10;
      if ((count>=2) && (count<=4))
      {
        return "Znaleziono <b>$num</b> dokumenty pasujące do twojego zapytania. "
               "Najlepiej pasujące dokumenty wyświetlane są na początku listy.";
      }
      else
      {
        return "Znaleziono <b>$num</b> dokumentów pasujących do twojego zapytania. "
               "Najlepiej pasujące dokumenty wyświetlane są na początku listy.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Pasujące słowa:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return "Plik źródłowy " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Struktura katalogów"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Dokumentacja katalogów"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Katalogi"; }


    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result="Dokumentacja katalogu "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "katalog", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
      return "To jest metoda przeciążona, udostępniona dla wygody. "
             "Różni się od powyższej metody tylko zestawem akceptowanych argumentów.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Oto graf wywoływań tej funkcji:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Dokumentacja wyliczeń"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Dokumentacja składowej funkcji/podprogramu"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Lista typów danych"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Pola danych"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Tutaj znajdują się typy danych wraz z ich krótkimi opisami:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll)
      {
        result+="udokumentowanych ";
      }
      result+="składowych typów danych";
      result+=" wraz z odnośnikami do ";
      if (!extractAll)
      {
         result+="dokumentacji struktury danych dla każdej składowej";
      }
      else
      {
         result+="typów danych, do których dana składowa należy:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Indeks typów danych"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Dokumentacja typów danych"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funkcje/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Dokumentacja funkcji/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Typy danych"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Lista modułów"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="modułów wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Dokumentacja";
      if (isTemplate) result+=" szablonu";
      switch(compType)
      {
        case ClassDef::Class:      result+=" modułu "; break;
        case ClassDef::Struct:     result+=" typu "; break;
        case ClassDef::Union:      result+=" unii "; break;
        case ClassDef::Interface:  result+=" interfejsu "; break;
        case ClassDef::Protocol:   result+=" protokołu "; break;
        case ClassDef::Category:   result+=" kategorii "; break;
        case ClassDef::Exception:  result+=" wyjątku "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result="Dokumentacja modułu ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Składowe modułu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="składowych modułów wraz z odnośnikami do ";
      if (extractAll)
      {
        result+="dokumentacji modułu dla każdej składowej:";
      }
      else
      {
        result+="modułów, do których składowe te należą:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Indeks modułu"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "moduł", "y");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentacja dla te";
      switch(compType)
      {
        case ClassDef::Class:      result+="go modułu"; break;
        case ClassDef::Struct:     result+="go typu"; break;
        case ClassDef::Union:      result+="j unii"; break;
        case ClassDef::Interface:  result+="go interfejsu"; break;
        case ClassDef::Protocol:   result+="go protokołu"; break;
        case ClassDef::Category:   result+="j kategorii"; break;
        case ClassDef::Exception:  result+="go wyjątku"; break;
        default: break;
      }
      result+=" została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "typ", "y");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "podprogram", "y");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Ograniczenia typów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return "Relacja "+ name;
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Wczytywanie...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Globalna przestrzeń nazw";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Szukanie...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Brak dopasowań";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    DString trFileIn(const DString &name) override
    {
      return "Plik w "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Zawiera plik w "+name;
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
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Pn","Wt","Śr","Cz","Pt","So","N" };
      static const char *months[] = { "sty","lut","mar","kwi","maj","cze","lip","sie","wrz","paź","lis","gru" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "pon.", "wt.", "śr.", "czw.", "pt.", "sob.", "niedz." };
      static const char *days_full[]    = { "poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "sty", "lut", "mar", "kwi", "maj", "cze", "lip", "sie", "wrz", "paź", "lis", "gru" };
      static const char *months_full[]  = { "styczeń", "luty", "marzec", "kwiecień", "maj", "czerwiec", "lipiec", "sierpień", "wrzesień", "październik", "listopad", "grudzień" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Odwołania do literatury"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Wykres zależności katalogu dla ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "poziom szczegółów"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Parametry Szablonu"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "i "+number+" więcej..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "Dokumentacja dla tego wyliczenia została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { DString result="Dokumentacja wyliczenia "; result+=name; return result; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" dziedziczone z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Dodatkowe dziedziczone składowe"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "włączyć" : "wyłączyć";
      return "kliknij żeby "+opt+" pokazywanie otwartego elementu w drzewie zawartości";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Dostarczany przez kategorię @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Rozszerza klasę @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Metody klasy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Metody instancji";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Dokumentacja metod";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Eksportowane interfejsy"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Załączone usługi"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Grupy stałych"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Dokumentacja grup stałych";
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Dokumentacja usług";
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Dokumentacja wzorców projektowych";
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentacja dla tej usługi "
                      "została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentacja dla tego wzorca projektowego "
                      "została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "Hierarchia jednostek projektowych"; }
    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "Lista jednostek projektowych"; }
    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "Składowe jednostki projektowej"; }
    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
        return "Tu znajduje się lista wszystkich składowych jednostek projektowych "
        "związanych z Jednostkami Nadrzędnymi do których należą:";
    }
    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "Indeks jednostek projektowych"; }
    /** VHDL design units */
    DString trDesignUnits() override
    { return "Jednostki projektowe"; }
    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "Funkcje/Procedury/Procesy"; }
    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Biblioteka";
          else        return "Biblioteki";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Pakiet";
          else        return "Pakiety";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Sygnał";
          else        return "Sygnały";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Komponent";
          else        return "Komponenty";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Stała";
          else        return "Stałe";
        case VhdlSpecifier::ENTITY:
          if (single) return "Jednostka";
          else        return "Jednostki";
        case VhdlSpecifier::TYPE:
          if (single) return "Typ";
          else        return "Typy";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Podtyp";
          else        return "Podtypy";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Funkcja";
          else        return "Funkcje";
        case VhdlSpecifier::RECORD:
          if (single) return "Rekord";
          else        return "Rekordy";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedura";
          else        return "Procedury";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Architektura";
          else        return "Architektury";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Atrybut";
          else        return "Atrybuty";
        case VhdlSpecifier::PROCESS:
          if (single) return "Proces";
          else        return "Procesy";
        case VhdlSpecifier::PORT:
          if (single) return "Port";
          else        return "Porty";
        case VhdlSpecifier::USE:
          if (single) return "Klauzula użycia";
          else        return "Klauzule użycia";
        case VhdlSpecifier::GENERIC:
          if (single) return "Generyczny";
          else        return "Generyczne";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Ciało pakietu";
        case VhdlSpecifier::UNITS:
          return "Jednostki";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Współdzielona zmienna";
          else        return "Współdzielone zmienne";
        case VhdlSpecifier::VFILE:
          if (single) return "Plik";
          else        return "Pliki";
        case VhdlSpecifier::GROUP:
          if (single) return "Grupa";
          else        return "Grupy";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Utworzona instancja";
          else        return "Utworzone instancje";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Aliasy";
        case VhdlSpecifier::CONFIG:
          if (single) return "Konfiguracja";
          else        return "Konfiguracje";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Różne";
        case VhdlSpecifier::UCF_CONST:
          return "Ograniczenia";
        default:
          return "Klasa";
      }
    }
    DString trCustomReference(const DString &name) override
    { return "Dokumentacja "+name; }

    /* Slice */
    DString trConstants() override
    {
        return "Stałe";
    }
    DString trConstantDocumentation() override
    {
        return "Dokumentacja stałych";
    }
    DString trSequences() override
    {
        return "Sekwencje";
    }
    DString trSequenceDocumentation() override
    {
        return "Dokumentacja sekwencji";
    }
    DString trDictionaries() override
    {
        return "Słowniki";
    }
    DString trDictionaryDocumentation() override
    {
        return "Dokumentacja słowników";
    }
    DString trSliceInterfaces() override
    {
        return "Interfejsy";
    }
    DString trInterfaceIndex() override
    {
        return "Indeks interfejsów";
    }
    DString trInterfaceList() override
    {
        return "Lista interfejsów";
    }
    DString trInterfaceListDescription() override
    {
        return "Tutaj znajduje się lista interfejsów wraz z ich krótkimi opisami:";
    }
    DString trInterfaceHierarchy() override
    {
        return "Hierarchia interfejsów";
    }
    DString trInterfaceHierarchyDescription() override
    {
        return "Ta lista dziedziczenia posortowana jest z grubsza, "
        "choć nie całkowicie, alfabetycznie:";
    }
    DString trInterfaceDocumentation() override
    {
        return "Dokumentacja interfejsów";
    }
    DString trStructs() override
    {
        return "Struktury";
    }
    DString trStructIndex() override
    {
        return "Indeks struktur";
    }
    DString trStructList() override
    {
        return "Lista struktur";
    }
    DString trStructListDescription() override
    {
        return "Tutaj znajdują się struktury wraz z ich krótkimi opisami:";
    }
    DString trStructDocumentation() override
    {
        return "Dokumentacja struktur";
    }
    DString trExceptionIndex() override
    {
        return "Indeks wyjątków";
    }
    DString trExceptionList() override
    {
        return "Lista wyjątków";
    }
    DString trExceptionListDescription() override
    {
        return "Tutaj znajdują się wyjątki wraz z ich krótkimi opisami:";
    }
    DString trExceptionHierarchy() override
    {
        return "Hierarchia wyjątków";
    }
    DString trExceptionHierarchyDescription() override
    {
        return "Ta lista dziedziczenia posortowana jest z grubsza, "
        "choć nie całkowicie, alfabetycznie:";
    }
    DString trExceptionDocumentation() override
    {
        return "Dokumentacja wyjątków";
    }
    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result=clName;
      result+= "Dokumentacja";
      if (isLocal) result+=" Lokal";
      switch(compType)
      {
        case ClassDef::Class:      if (isLocal) result+="nej"; result+=" Klasy"; break;
        case ClassDef::Struct:     if (isLocal) result+="nej"; result+=" Struktury"; break;
        case ClassDef::Union:      if (isLocal) result+="nej"; result+=" Unii"; break;
        case ClassDef::Interface:  if (isLocal) result+="nego"; result+=" Interfejsu"; break;
        case ClassDef::Protocol:   if (isLocal) result+="nego"; result+=" Protokołu"; break;
        case ClassDef::Category:   if (isLocal) result+="nej"; result+=" Kategorii"; break;
        case ClassDef::Exception:  if (isLocal) result+="nego"; result+=" Wyjątku"; break;
        default: break;
      }
      result+=" " + clName;
      return result;
    }
    DString trOperations() override
    {
        return "Operacje";
    }
    DString trOperationDocumentation() override
    {
        return "Dokumentacja operacji";
    }
    DString trDataMembers() override
    {
        return "Pola klasy";
    }
    DString trDataMemberDocumentation() override
    {
        return "Dokumentacja pól klasy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    DString trDesignUnitDocumentation() override
    { return "Dokumentacja jednostek projektowych"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    DString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "koncept", "y");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    DString trConceptReference(const DString &conceptName) override
    {
      DString result="Dokumentacja konceptu " + conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    DString trConceptList() override
    { return "Lista konceptów"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    DString trConceptIndex() override
    { return "Indeks konceptów"; }

    /*! used as the title of chapter containing all information about concepts. */
    DString trConceptDocumentation() override
    { return "Dokumentacja konceptów"; }

    /*! used as an introduction to the concept list */
    DString trConceptListDescription(bool extractAll) override
    {
      DString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="konceptów wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    DString trConceptDefinition() override
    {
      return "Definicja konceptu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    DString trPackageList() override
    { return "Lista pakietów"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
    { return "Schemat blokowy:"; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     */

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    DString trRelatedSymbols() override
    { return "Powiązane symbole"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    DString trRelatedSymbolsSubscript() override
    { return "(Zauważ, że te symbole nie są składowymi.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    DString trRelatedSymbolDocumentation() override
    { return "Dokumentacja przyjaciół i powiązanych symboli"; }

    /*! the compound type as used for the xrefitems */
    /*
      * TODO Check usecase of this function and verify if noun case is correct
      * At the moment only Czech language is available for cross-reference but its cases are mixed
    */
    DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      DString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) result=trType(true,true);
          else result=trClass(true,true);
          break;
        case ClassDef::Struct:     result="Struktura"; break;
        case ClassDef::Union:      result="Unia"; break;
        case ClassDef::Interface:  result="Interfejs"; break;
        case ClassDef::Protocol:   result="Protokół"; break;
        case ClassDef::Category:   result="Kategoria"; break;
        case ClassDef::Exception:  result="Wyjątek"; break;
        case ClassDef::Service:    result="Usługa"; break;
        case ClassDef::Singleton:  result="Wzorzec Projektowy"; break;
        default: break;
      }
      return result;
    }

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Tutaj jest lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";

      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="funkcji, zmiennych, definicji, wyliczeń, i definicji typów";
          }
          else
          {
            result+="dołączonych plików";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="funkcji";
          break;
        case FileMemberHighlight::Variables:
          result+="zmiennych";
          break;
        case FileMemberHighlight::Typedefs:
          result+="definicji typów";
          break;
        case FileMemberHighlight::Sequences:
          result+="sekwencji";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="słowników";
          break;
        case FileMemberHighlight::Enums:
          result+="wyliczeń";
          break;
        case FileMemberHighlight::EnumValues:
          result+="wartości wyliczeń";
          break;
        case FileMemberHighlight::Defines:
          result+="definicji";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result+=" związanych z ";
      if (extractAll)
        result+="plikami do których należą:";
      else
        result+="dokumentacją:";
      return result;
    }
    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Tutaj jest lista wszystkich ";
      if (!extractAll)
      {
        result+="udokumentowanych ";
      }

      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="struktur i pól unii";
          }
          else
          {
            result+="składowych klasy";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="funkcji";
          break;
        case ClassMemberHighlight::Variables:
          result+="zmiennych";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="definicji typów";
          break;
        case ClassMemberHighlight::Enums:
          result+="wyliczeń";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="wartości wyliczeń";
          break;
        case ClassMemberHighlight::Properties:
          result+="właściwości";
          break;
        case ClassMemberHighlight::Events:
          result+="zdarzeń";
          break;
        case ClassMemberHighlight::Related:
          result+="powiązanych symboli";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result+=" związanych z";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+=" dokumentacją każdego pola struktury/unii:";
        }
        else
        {
          result+=" dokumentacją każdej składowej klasy:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="e strukturą/unią do której należą:";
        }
        else
        {
          result+=" klasą do której należą:";
        }
      }
      return result;
    }
    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Tutaj jest lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="przestrzeni nazw ";
      DString singularResult = "";
      DString pluralResult = "";

      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="każdej składowej";
          pluralResult="wszystkich składowych";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult="każdej funkcji";
          pluralResult="wszystkich funkcji";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult="każdej zmiennej";
          pluralResult="wszystkich zmiennych";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult="każdej definicji typu";
          pluralResult="wszystkich definicji typów";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult="każdej sekwencji";
          pluralResult="wszystkich sekwencji";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult="każdego słownika";
          pluralResult="wszystkich słowników";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult="każdego wyliczenia";
          pluralResult="wszystkich wyliczeń";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult="każdej wartości wyliczenia";
          pluralResult="wszystkich wartości wyliczeń";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+=pluralResult;
      result+=" związanych z ";
      if (extractAll)
        result+="przestrzenią nazw dla " + singularResult + ":";
      else
        result+="przestrzenią nazw do której należą:";
      return result;
    }
    DString trDefinition() override  { return "Definicja";}
    DString trDeclaration() override { return "Deklaracja";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    /*
      * TODO Verify which one fits best:
      * * Podrozdział - subchapter
      * * Sekcja - section
      * * Temat - topic (doesn't fit well as a standalone word)
    */
    DString trTopics() override
    { return "Sekcje"; }
    DString trTopicDocumentation() override
    { return "Dokumentacja sekcji"; }
    DString trTopicList() override
    { return "Lista sekcji"; }
    DString trTopicIndex() override
    { return "Indeks sekcji"; }
    DString trTopicListDescription() override
    { return "Tutaj jest lista wszystkich sekcji wraz z ich krótkimi opisami:"; }
    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Tutaj jest lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="modułów ";
      DString singularResult = "";
      DString pluralResult = "";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          singularResult="każdej składowej";
          pluralResult = "wszystkich składowych";
          break;
        case ModuleMemberHighlight::Functions:
          singularResult="każdej funkcji";
          pluralResult = "wszystkich funkcji";
          break;
        case ModuleMemberHighlight::Variables:
          singularResult="każdej zmiennej";
          pluralResult = "wszystkich zmiennych";
          break;
        case ModuleMemberHighlight::Typedefs:
          singularResult="każdej definicji typu";
          pluralResult = "wszystkich definicji typów";
          break;
        case ModuleMemberHighlight::Enums:
          singularResult="każdego wyliczenia";
          pluralResult = "wszystkich wyliczeń";
          break;
        case ModuleMemberHighlight::EnumValues:
          singularResult="każdej wartości wyliczeń";
          pluralResult = "wszystkich wartości wyliczeń";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result+=pluralResult;
      result+=" związanych z ";
      if (extractAll)
        result+="dokumentacją modułu dla " + singularResult + ":";
      else
        result+="modułem do którego należą:";
      return result;
    }
    DString trExportedModules() override
    {
      return "Eksportowane moduły";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    DString trCopyToClipboard() override
    {
      return "Kopiuj do schowka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    DString trImportant() override
    {
      return "Ważne";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    DString trRequirements() override
    {
      return "Wymagania";
    }
    // table header for the column with the requirements IDs
    DString trRequirementID() override
    {
      return "ID";
    }
    // indicates a symbol implements (satisfies) a requirement
    DString trSatisfies(bool singular) override
    {
      return createNoun(true, singular, "Spełnia wym", "ogi", "óg");
    }
    // indicates a requirement is satisfied (implemented) by one or more symbols
    DString trSatisfiedBy(const DString &list) override
    {
      return "Spełniony przez "+list+".";
    }
    DString trUnsatisfiedRequirements() override
    {
      return "Niespełnione Wymagania";
    }
    DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "Wymóg "+list+" nie ma relacji 'spełnia'." :
        "Wymogi "+list+" nie ma relacji 'spełnia'.";
    }
    // indicates a symbol verifies (tests) a requirement
    DString trVerifies(bool singular) override
    {
      return createNoun(true, singular, "Weryfikuje wym", "ogi", "óg");
    }
    // indicates a requirement is verified (tested) by one or more symbols
    DString trVerifiedBy(const DString &list) override
    {
      return "Zweryfikowany przez "+list+".";
    }
    DString trUnverifiedRequirements() override
    {
      return "Niezweryfikowane Wymagania";
    }
    DString trUnverifiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "Wymóg "+list+" nie posiada relacji 'weryfikuje'." :
        "Wymogi "+list+" nie posiadają relacji 'weryfikuje'.";
    }
};

#endif
