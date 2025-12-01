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

class TranslatorPolish : public TranslatorAdapter_1_16_0
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
    QCString idLanguage() override
    { return "polish"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    QCString latexLanguageSupportCommand() override
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

    QCString trISOLang() override
    {
      return "pl";
    }
    QCString getLanguageString() override
    {
      return "0x415 Polish";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funkcje powiązane"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Zauważ, że to nie są metody klas.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Opis szczegółowy"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Szczegóły"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentacja składowych definicji typu"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentacja składowych wyliczanych"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
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
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Więcej..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista wszystkich składowych"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Lista składowych"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "To jest kompletna lista składowych dla"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", uwzględniająca wszystkie dziedziczone składowe."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Wygenerowano automatycznie z kodu źródłowego programem Doxygen";
      if (!s.isEmpty()) result+=" dla "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nazwa wyliczenia"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "wartość wyliczenia"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "zdefiniowana w"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduły"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hierarchia klas"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Lista plików"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Powiązane strony"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Przykłady"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Szukaj"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Ta lista dziedziczenia posortowana jest z grubsza, "
             "choć nie całkowicie, alfabetycznie:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="plików wraz z ich krótkimi opisami:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
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
    QCString trExamplesDescription() override
    { return "Tutaj znajduje się lista wszystkich przykładów:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Tutaj znajduje się lista wszystkich powiązanych stron dokumentacji:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Tutaj znajduje się lista wszystkich grup:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentacja"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Indeks grup"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Indeks hierarchiczny"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Indeks plików"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Dokumentacja grup"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Dokumentacja plików"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Podręcznik"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definicje"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definicje typów"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Wyliczenia"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funkcje"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Zmienne"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Wartości wyliczeń"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Dokumentacja definicji"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentacja definicji typów"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentacja typów wyliczanych"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Dokumentacja funkcji"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Dokumentacja zmiennych"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Wygenerowano "+date;
      if (!projName.isEmpty()) result+=" dla "+projName;
      result+=" za pomocą";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagram dziedziczenia dla "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Ostrzeżenie"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Wersja"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Zwraca"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Zobacz również"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametry"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Wyjątki"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Wygenerowano za pomocą"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Lista przestrzeni nazw"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Tutaj znajdują się wszystkie ";
      if (!extractAll) result+="udokumentowane ";
      result+="przestrzenie nazw wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Przyjaciele"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentacja przyjaciół i powiązanych funkcji"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Dokumentacja";
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Dokumentacja pliku ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Dokumentacja przestrzeni nazw ";
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers() override
    { return "Metody publiczne"; }
    QCString trPublicSlots() override
    { return "Sloty publiczne"; }
    QCString trSignals() override
    { return "Sygnały"; }
    QCString trStaticPublicMembers() override
    { return "Statyczne metody publiczne"; }
    QCString trProtectedMembers() override
    { return "Metody chronione"; }
    QCString trProtectedSlots() override
    { return "Sloty chronione"; }
    QCString trStaticProtectedMembers() override
    { return "Statyczne metody chronione"; }
    QCString trPrivateMembers() override
    { return "Metody prywatne"; }
    QCString trPrivateSlots() override
    { return "Sloty prywatne"; }
    QCString trStaticPrivateMembers() override
    { return "Statyczne metody prywatne"; }

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
            result+=" i ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Dziedziczy "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Dziedziczona przez "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementowana z "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementowana w "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Składowe przestrzeni nazw"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
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
    QCString trNamespaceIndex() override
    { return "Indeks przestrzeni nazw"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Dokumentacja przestrzeni nazw"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Przestrzenie nazw"; }

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
      QCString result="Dokumentacja dla te";
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
    QCString trReturnValues() override
    { return "Zwracane wartości"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Strona główna"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "str."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definicja w linii @0 pliku @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definicja w pliku @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Do wycofania";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagram współpracy dla "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Wykres zależności załączania dla "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentacja konstruktora i destruktora";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Idź do kodu źródłowego tego pliku.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Idź do dokumentacji tego pliku.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Warunek wstępny";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Warunek końcowy";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Niezmiennik";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Wartość początkowa:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "kod źródłowy";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Graficzna hierarchia klas";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Idź do graficznej hierarchii klas";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Idź do tekstowej hierarchii klas";
    }
    QCString trPageIndex() override
    {
      return "Indeks stron";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Nota";
    }
    QCString trPublicTypes() override
    {
      return "Typy publiczne";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Statyczne atrybuty publiczne";
    }
    QCString trProtectedTypes() override
    {
      return "Typy chronione";
    }
    QCString trProtectedAttribs() override
    {
      return "Atrybuty chronione";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statyczne atrybuty chronione";
    }
    QCString trPrivateTypes() override
    {
      return "Typy prywatne";
    }
    QCString trPrivateAttribs() override
    {
      return "Atrybuty prywatne";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statyczne atrybuty prywatne";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Do zrobienia";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Lista rzeczy do zrobienia";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Odwołania w";
    }
    QCString trRemarks() override
    {
      return "Spostrzeżenia";
    }
    QCString trAttention() override
    {
      return "Uwaga";
    }
    QCString trInclByDepGraph() override
    {
      return "Ten wykres pokazuje, które pliki bezpośrednio lub "
             "pośrednio załączają ten plik:";
    }
    QCString trSince() override
    {
      return "Od";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Legenda wykresu";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Lista testów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Właściwości";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Dokumentacja właściwości";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Pakiet "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Oto lista pakietów wraz z ich krótkimi opisami (jeśli są dostępne):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pakiety";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Wartość:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Błąd";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Lista błędów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp() override
    {
      return "1250";
    }

    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klas", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "plik", "i");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "przestrze", "nie", "ń") + " nazw";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupa", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stron", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "składow", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "ne", "ny");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "zy");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Odwołuje się do";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////


    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    QCString trImplementedInList(int numEntries) override
    {
      return "Implementowany w "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Spis treści";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista elementów do wycofania";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Zdarzenia";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Dokumentacja zdarzeń";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Typy pakietu";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funkcje pakietu";
    }
    QCString trPackageMembers() override
    {
      return "Składowe pakietu";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statyczne funkcje pakietu";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atrybuty pakietu";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statyczne atrybuty pakietu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Wszystko";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Oto graf wywołań dla tej funkcji:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Pasujące słowa:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Plik źródłowy " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Struktura katalogów"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Dokumentacja katalogów"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Katalogi"; }


    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Dokumentacja katalogu "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "katalog", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
      return "To jest metoda przeciążona, udostępniona dla wygody. "
             "Różni się od powyższej metody tylko zestawem akceptowanych argumentów.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Oto graf wywoływań tej funkcji:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentacja wyliczeń"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentacja składowej funkcji/podprogramu"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lista typów danych"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Pola danych"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Tutaj znajdują się typy danych wraz z ich krótkimi opisami:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
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
    QCString trCompoundIndexFortran() override
    { return "Indeks typów danych"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Dokumentacja typów danych"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funkcje/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Dokumentacja funkcji/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Typy danych"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Lista modułów"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="modułów wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Dokumentacja";
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Dokumentacja modułu ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Składowe modułu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
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
    QCString trModulesIndex() override
    { return "Indeks modułu"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "moduł", "y");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentacja dla te";
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "typ", "y");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "podprogram", "y");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Ograniczenia typów";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relacja "+ QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Wczytywanie...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Globalna przestrzeń nazw";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Szukanie...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Plik w "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Pn","Wt","Śr","Cz","Pt","So","N" };
      static const char *months[] = { "sty","lut","mar","kwi","maj","cze","lip","sie","wrz","paź","lis","gru" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "pon.", "wt.", "śr.", "czw.", "pt.", "sob.", "niedz." };
      static const char *days_full[]    = { "poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "sty", "lut", "mar", "kwi", "maj", "cze", "lip", "sie", "wrz", "paź", "lis", "gru" };
      static const char *months_full[]  = { "styczeń", "luty", "marzec", "kwiecień", "maj", "czerwiec", "lipiec", "sierpień", "wrzesień", "październik", "listopad", "grudzień" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Odwołania do literatury"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Wykres zależności katalogu dla ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "poziom szczegółów"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parametry Szablonu"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "i "+number+" więcej..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumentacja dla tego wyliczenia została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { QCString result="Dokumentacja wyliczenia "; result+=name; return result; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" dziedziczone z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Dodatkowe dziedziczone składowe"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "włączyć" : "wyłączyć";
      return "kliknij żeby "+opt+" pokazywanie otwartego elementu w drzewie zawartości";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Dostarczany przez kategorię @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Rozszerza klasę @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Metody klasy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Metody instancji";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Dokumentacja metod";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Eksportowane interfejsy"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Załączone usługi"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Grupy stałych"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Dokumentacja grup stałych";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Dokumentacja usług";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Dokumentacja wzorców projektowych";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentacja dla tej usługi "
                      "została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentacja dla tego wzorca projektowego "
                      "została wygenerowana z plik";
      if (single) result+="u:"; else result+="ów:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Hierarchia jednostek projektowych"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Lista jednostek projektowych"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Składowe jednostki projektowej"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Tu znajduje się lista wszystkich składowych jednostek projektowych "
        "związanych z Jednostkami Nadrzędnymi do których należą:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Indeks jednostek projektowych"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Jednostki projektowe"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funkcje/Procedury/Procesy"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
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
    QCString trCustomReference(const QCString &name) override
    { return "Dokumentacja "+name; }

    /* Slice */
    QCString trConstants() override
    {
        return "Stałe";
    }
    QCString trConstantDocumentation() override
    {
        return "Dokumentacja stałych";
    }
    QCString trSequences() override
    {
        return "Sekwencje";
    }
    QCString trSequenceDocumentation() override
    {
        return "Dokumentacja sekwencji";
    }
    QCString trDictionaries() override
    {
        return "Słowniki";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Dokumentacja słowników";
    }
    QCString trSliceInterfaces() override
    {
        return "Interfejsy";
    }
    QCString trInterfaceIndex() override
    {
        return "Indeks interfejsów";
    }
    QCString trInterfaceList() override
    {
        return "Lista interfejsów";
    }
    QCString trInterfaceListDescription() override
    {
        return "Tutaj znajduje się lista interfejsów wraz z ich krótkimi opisami:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Hierarchia interfejsów";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Ta lista dziedziczenia posortowana jest z grubsza, "
        "choć nie całkowicie, alfabetycznie:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Dokumentacja interfejsów";
    }
    QCString trStructs() override
    {
        return "Struktury";
    }
    QCString trStructIndex() override
    {
        return "Indeks struktur";
    }
    QCString trStructList() override
    {
        return "Lista struktur";
    }
    QCString trStructListDescription() override
    {
        return "Tutaj znajdują się struktury wraz z ich krótkimi opisami:";
    }
    QCString trStructDocumentation() override
    {
        return "Dokumentacja struktur";
    }
    QCString trExceptionIndex() override
    {
        return "Indeks wyjątków";
    }
    QCString trExceptionList() override
    {
        return "Lista wyjątków";
    }
    QCString trExceptionListDescription() override
    {
        return "Tutaj znajdują się wyjątki wraz z ich krótkimi opisami:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Hierarchia wyjątków";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Ta lista dziedziczenia posortowana jest z grubsza, "
        "choć nie całkowicie, alfabetycznie:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Dokumentacja wyjątków";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result=clName;
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
    QCString trOperations() override
    {
        return "Operacje";
    }
    QCString trOperationDocumentation() override
    {
        return "Dokumentacja operacji";
    }
    QCString trDataMembers() override
    {
        return "Pola klasy";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Dokumentacja pól klasy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Dokumentacja jednostek projektowych"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "koncept", "y");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result="Dokumentacja konceptu " + conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Lista konceptów"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Indeks konceptów"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Dokumentacja konceptów"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Tutaj znajduje się lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="konceptów wraz z ich krótkimi opisami:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Definicja konceptu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Lista pakietów"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
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
    QCString trRelatedSymbols() override
    { return "Powiązane symbole"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(Zauważ, że te symbole nie są składowymi.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Dokumentacja przyjaciół i powiązanych symboli"; }

    /*! the compound type as used for the xrefitems */
    /*
      * TODO Check usecase of this function and verify if noun case is correct
      * At the moment only Czech language is available for cross-reference but its cases are mixed
    */
    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) trType(true,true);
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

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Tutaj jest lista wszystkich ";
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
    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Tutaj jest lista wszystkich ";
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
    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Tutaj jest lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="przestrzeni nazw ";
      QCString singularResult = "";
      QCString pluralResult = "";

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
    QCString trDefinition() override  { return "Definicja";}
    QCString trDeclaration() override { return "Deklaracja";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    /*
      * TODO Verify which one fits best:
      * * Podrozdział - subchapter
      * * Sekcja - section
      * * Temat - topic (doesn't fit well as a standalone word)
    */
    QCString trTopics() override
    { return "Sekcje"; }
    QCString trTopicDocumentation() override
    { return "Dokumentacja sekcji"; }
    QCString trTopicList() override
    { return "Lista sekcji"; }
    QCString trTopicIndex() override
    { return "Indeks sekcji"; }
    QCString trTopicListDescription() override
    { return "Tutaj jest lista wszystkich sekcji wraz z ich krótkimi opisami:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Tutaj jest lista wszystkich ";
      if (!extractAll) result+="udokumentowanych ";
      result+="modułów ";
      QCString singularResult = "";
      QCString pluralResult = "";
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
    QCString trExportedModules() override
    {
      return "Eksportowane moduły";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "Kopiuj do schowka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    QCString trImportant() override
    {
      return "Ważne";
    }
};

#endif
