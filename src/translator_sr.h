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

#ifndef TRANSLATOR_SR_H
#define TRANSLATOR_SR_H

// translation by Dejan D. M. Milosavljevic <dmilos@email.com>;<dmilosx@ptt.yu>;<office@ddmrm.com>
// // 10x 2 Ivana Miletic for grammatical consultation.

// UTF-8 patch by Nenad Bulatovic <buletina@gmail.com>
// translation update by Andrija M. Bosnjakovic <andrija@etf.bg.ac.yu>

class TranslatorSerbian : public TranslatorAdapter_1_6_0
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
    { return "serbian"; }

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
      QCString result="\\usepackage[serbian]{babel}\n";
      return result;
    }
    QCString trISOLang() override
    {
      return "sr-Latn";
    }
    QCString getLanguageString() override
    {
      return "0x81A Serbian (Serbia, Latin)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    //! Čini se da je ovako manje loše nego "Povezane funkcije",
    //! što uopšte ne izgleda dobro jer ta kartica sadrži prijatelje i globalne funkcije
    { return "Relevantne funkcije"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(To nisu funkcije članice.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Opširniji opis"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Podrobnosće"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentacija unutrašnjih definicija tipa"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    //! Ovo je u skladu sa "unutrašnja klasa" što se može videti u knjizi.
    { return "Dokumentacija unutrašnjih nabrajanja"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Dokumentacija funkcija članica"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentacija polja";
      }
      else
      {
        return "Dokumentacija atributa";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Još..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Spisak svih članova"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Spisak članova"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Ovo je spisak svih članova"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", uključujući nasleđene članove."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Napravljeno automatski korišćenjem alata Doxygen";
      if (!s.isEmpty()) result+=" za projekat " + s;
      result+=" od izvornog koda.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "ime nabrajanja "; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "vrednost nabrojane konstante"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definicija u"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduli"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hijerarhija klasa"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Spisak struktura";
      }
      else
      {
        return "Spisak klasa";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Spisak datoteka"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Sva polja struktura";
      }
      else
      {
        return "Svi članovi klasa";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      //if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      //{
      // return "Članovi datoteke"; // TODO: translate me
      //}
      //else
      //{
      return "Članovi datoteke";
      //}
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Stranice koje imaju veze sa ovom stranicom"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Primeri"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Traži"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Hijerahija klasa uređena približno po abecedi:"; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";
      result+="datoteka, sa kratkim opisima:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Spisak struktura sa kratkim opisima:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Spisak klasa sa kratkim opisima:";
      }
      else
      {
        return "Spisak klasa, struktura, unija i interfejsa sa kratkim opisima:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll)
      {
        result+="dokumentovanih ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="članova struktura/unija";
      }
      else
      {
        result+="članova klasa";
      }
      result+=" sa vezama ka ";
      if (extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="dokumentaciji svakog polja strukture/unije:";
        }
        else
        {
          result+="dokumentaciji svakog člana klase:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="strukturama/unijama kojima pripadaju:";
        }
        else
        {
          result+="klasama kojima pripadaju:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcija, promenljivih, makro zamena, nabrajanja i definicija tipa";
      }
      else
      {
        result+="članova";
      }
      result+=" sa vezama ka ";
      if (extractAll)
        result+="datotekama u kojima se nalaze:";
      else
        result+="dokumentaciji:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Spisak svih primera:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Spisak stranica koje imaju veze sa ovom stranicom:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Spisak svih modula:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Indeks modula"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hijerarhijski sadržaj"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Spisak struktura/unija";
      }
      else
      {
        return "Spisak klasa";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Indeks datoteka"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Dokumentacija modula"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentacija stuktura/unija";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Dokumentacija klasa";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Dokumentacija datoteke"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Priručnik"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Makro zamene"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definicije tipa"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Nabrajanja"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funkcije"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Promenljive"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Vrednosti nabrojanih konstanti"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Dokumentacija makro zamene"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentacija definicije tipa"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentacija nabrajanja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Dokumentacija funkcije"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Dokumentacija promenljive"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Strukture i unije";
      }
      else
      {
        return "Klase, strukture i unije";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="[" + date + "] Napravljeno automatski ";
      if (!projName.isEmpty()) result+=" za projekat " + projName;
	  result+=" upotrebom ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("Dijagram nasleđivanja za klasu ") + clName + ":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Upozorenje"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Verzija"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Vrednost funkcije"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Takođe pogledati"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Izuzeci"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Napravio"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Spisak prostora imena"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";
      result+="prostora imena sa kratkim opisom:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Prijatelji"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentacija prijatelja i relevantnih funkcija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result( "Dokumentacija " );
      switch(compType)
      {
        case ClassDef::Class:      result+="klase "; break;
        case ClassDef::Struct:     result+="strukture "; break;
        case ClassDef::Union:      result+="unije "; break;
        case ClassDef::Interface:  result+="interfejsa "; break;
        case ClassDef::Protocol:   result+="protokola "; break;
        case ClassDef::Category:   result+="kategorije "; break;
        case ClassDef::Exception:  result+="izuzetka "; break;
        default: break;
      }
      if (isTemplate) result += "šablona ";
      result += clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result = "Opis datoteke ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Opis prostora imena ";
      result += namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Javni članovi"; }
    QCString trPublicSlots() override
    { return "Javni slotovi"; }
    QCString trSignals() override
    { return "Signali"; }
    QCString trStaticPublicMembers() override
    { return "Zajednički javni članovi"; }
    QCString trProtectedMembers() override
    { return "Zaštićeni članovi";  }
    QCString trProtectedSlots() override
    { return "Zaštićeni slotovi"; }
    QCString trStaticProtectedMembers() override
    { return "Zajednički zaštićeni članovi"; }
    QCString trPrivateMembers() override
    { return "Privatni članovi"; }
    QCString trPrivateSlots() override
    { return "Privatni slotovi"; }
    QCString trStaticPrivateMembers() override
    { return "Zajednički privatni članovi"; }

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
    QCString trInheritsList(int /*numEntries*/) override
    {
      return "Spisak osnovnih klasa: ";
    }

    /*! used in class documentation to produce a list of derived classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int /*numEntries*/) override
    {
      return "Spisak izvedenih klasa: ";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Menja definiciju iz "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    { //! Ako već ne možemo jednu reč (redefinicija), da uskladimo sa prethodnim i izbacimo upotrebu roda
      return "Definicija je izmenjena u "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Članovi prostora imena"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";
      result+="članova prostora imena sa vezama prema ";
      if (extractAll)
        result+="dokumentaciji svakog člana prostora imena: ";
      else
        result+="prostorima imena kojima pripadaju: ";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Indeks prostora imena"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Dokumentacija prostora imena"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Prostori imena"; }

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
      QCString result="Dokumentacija ";
      switch(compType)
      {
        case ClassDef::Class:      result+="ove klase"; break;
        case ClassDef::Struct:     result+="ove strukture"; break;
        case ClassDef::Union:      result+="ove unije"; break;
        case ClassDef::Interface:  result+="ovog interfejsa"; break;
        case ClassDef::Protocol:   result+="ovog protokola"; break;
        case ClassDef::Category:   result+="ove kategorije"; break;
        case ClassDef::Exception:  result+="ovog izuzetka"; break;
        default: break;
      }
      result+=" je napravljena na osnovu ";
      if (single) result+="datoteke "; else result+="sledećih datoteka:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Karakteristične vrednosti funkcije"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Glavna strana"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "str."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    { //! Izbacujemo rod
      return "Definicija je u redu @0 datoteke @1.";
    }
    QCString trDefinedInSourceFile() override
    { //! Izbacujemo rod
      return "Definicija je u datoteci @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Zastarelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Klasni dijagram za "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graf zavisnosti datoteka za "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentacija konstruktora i destruktora";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Izvorni kod.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Dokumentacija.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Preduslovi";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Stanje po izvršenju";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invarijanta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Početna vrednost:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "programski kod";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Hijerarhija klasa u obliku grafa";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Prikaz hijerarhije klasa u obliku grafa";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Prikaz hijerarhije klasa u obliku nazubljenog teksta";
    }
    QCString trPageIndex() override
    {
      return "Indeks stranice";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Beleška";
    }
    QCString trPublicTypes() override
    {
      return "Javni tipovi";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Polja";
      }
      else
      {
        return "Javni članovi";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Zajednički javni članovi";
    }
    QCString trProtectedTypes() override
    {
      return "Zaštićeni tipovi";
    }
    QCString trProtectedAttribs() override
    {
      return "Zaštićeni članovi";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Zajednički zaštićeni članovi";
    }
    QCString trPrivateTypes() override
    {
      return "Privatni tipovi";
    }
    QCString trPrivateAttribs() override
    {
      return "Privatni članovi";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Zajednički privatni članovi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Uraditi";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Spisak stvari koje treba uraditi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    { //! Izbegavanje roda. Uskladjivanje sa trReferences
      return "Korisnici: ";
    }
    QCString trRemarks() override
    {
      return "Napomene";
    }
    QCString trAttention() override
    {
      return "Pažnja";
    }
    QCString trInclByDepGraph() override
    {
      return "Ovaj graf pokazuje koje datoteke direktno "
             "ili indirektno uključuju ovu datoteku: ";
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
      return "Objašnjenje korišćenih simbola";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Ova stranica objašnjava kako tumačiti grafikone koje je napravio "
        "doxygen.<p>\n"
        "Na primer:\n"
        "\\code\n"
        "/*! Klasa nevidljiva zbog trenutnih ograničenja */\n"
        "class Invisible { };\n\n"
        "/*! Klasa kojoj se ne vidi način izvođenja */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klasa bez doxygen komentara */\n"
        "class Undocumented { };\n\n"
        "/*! Klasa izvedena iz osnovne klase javnim izvođenjem */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Šablonska klasa */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasa izvedena iz osnovne klase zaštićenim izvođenjem */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasa izvedena iz osnovne klase privatnim izvođenjem */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasa korišćena u nekoj/nekim od drugih klasa */\n"
        "class Used { };\n\n"
        "/*! Klasa izvedena iz više osnovnih klasa */\n"
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
        "Ako je \\c MAX_DOT_GRAPH_HEIGHT tag u konfiguracionoj datoteci "
        "postavljen na \\c 200 graf izvođenja će izgledati ovako:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "Graf će biti odsečen ako ne stane unutar zadatih granica.\n"
        "<p>\n"
        "Pravougaonici imaju sledeća značenja:\n"
        "<ul>\n"
         "<li>Puni sivi predstavlja strukturu ili klasu za koju je graf napravljen.\n"
         "<li>Sa crnom ivicom predstavlja dokumentovanu strukturu ili klasu.\n"
         "<li>Sa sivom ivicom predstavlja strukturu ili klasu bez doxygen komentara.\n"
         "<li>Sa crvenom ivicom predstavlja dokumentovanu strukturu ili klasu\n"
         "za koju nisu prikazani svi relevantni grafovi.\n"
         "</ul>"
        "Strelice imaju sledeća značenja:\n"
        "<ul>\n"
         "<li>Tamnoplava strelica označava javno izvođenje.\n"
         "<li>Tamnozelena strelica označava zaštićeno izvođenje.\n"
         "<li>Tamnocrvena strelica označava privatno izvođenje.\n"
         "<li>Ljubičasta isprekidana strelica označava da je klasa sadržana "
          "ili korišćena u drugoj klasi. Strelica je označena imenom atributa "
          "preko koga se pristupa klasi/strukturi na koju pokazuje.\n"
         "<li>Žuta isprekidana strelica označava vezu između primerka šablona i"
          " šablona klase od kojeg je primerak napravljen. "
          "Strelica je označena stvarnim argumentima šablona.\n"
        "</ul>\n"
        ;
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "Objašnjenje korišćenih simbola";
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
      return "Spisak testova";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Osobine";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Dokumentacija osobina";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Strukture i unije";
      }
      else
      {
        return "Klase";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paket "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Paketi s kratkim opisom (ukoliko postoji):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paketi";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Vrednost:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Greška";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Spisak grešaka";
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
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Sadržaj";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klas", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "datotek", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "prostor", "i") + " imena";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grup", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stran", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "član", "ovi");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globalni poda", "ci", "tak");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Koristi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int /*numEntries*/) override
    { //! "Definiše" je previše kratko, ispada sa de definišu same apstraktne klase
      return "Definiše apstraktnu funkciju deklarisanu u ";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int /*numEntries*/) override
    { //! Izbegavanje roda
      return "Definicija u ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
    *  of Contents.
         */
    QCString trRTFTableOfContents() override
     {
      return "Sadržaj";
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Spisak zastarelih stvari";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Događaji";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Dokumentacija događaja";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipovi u paketu";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funkcije u paketu";
    }
    QCString trPackageMembers() override
    {
      return "Članovi u paketu";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statičke funkcije u paketu";  // Zajednicke funkcije u paketu
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributi u paketu"; // Clanovi u paketu
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statički atributi u paketu";  // Zajednicki clanovi u paketu
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Sve";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Graf poziva iz ove funkcije:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Rezultati pretraživanja";
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
        return "Nema dokumenata koji odgovaraju Vašem upitu.";
      }
      else if (numDocuments==1)
      { return "Nađen je <b>1</b> dokument koji odgovara vašem upitu."; }
      else if (numDocuments<5)
      { return "Nađena su <b>$num</b> dokumenta koji odgovaraju vašem upitu."
               " Najbolji su prikazani prvi."; }
      else
      { return "Nađeno je <b>$num</b> dokumenata koji odgovaraju vašem upitu."
               " Najbolji su prikazani prvi.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Pronađeno:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Izvorni kod datoteke " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hijerarhija direktorijuma"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Dokumentacija direktorijuma"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Direktorijumi"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+="Opis direktorijuma"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "direktorijum", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Ovo je funkcija prekopljenog imena, razlikuje se "
              "od gore navedene samo po argumentima koje prihvata.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    { //! Možda je bolje "Graf pozivalaca ove funkcije"
      return "Graf funkcija koje pozivaju ovu funkciju:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentacija enum vrednosti"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentacija funkcija i procedura"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    /* TODO: Koji je prevod za Compound u Fortran kontekstu */
    QCString trCompoundListFortran() override
    { return "Složeni tipovi podataka"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Polja u složenim tipovima podataka"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Spisak složenih tipova podataka sa kratkim opisima:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result=" Spisak svih ";
      if (!extractAll)
      {
        result+="dokumentovanih ";
      }
      result+="polja složenih tipova podataka";
      result+=" sa vezama ka ";
      if (!extractAll)
      {
         result+="dokumentaciji strukture podataka za svakog člana";
      }
      else
      {
         result+="složenim tipovima podataka kojima pripadaju:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Sadržaj složenog tipa podataka"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Dokumentacija tipova podataka"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funkcije i procedure"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Dokumentacija funkcija i procedura"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Složeni tipovi podataka"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Spisak modula"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";
      result+="modula sa kratkim opisima:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Modul"; break;
        case ClassDef::Struct:     result+=" Tip"; break;
        case ClassDef::Union:      result+=" Unija"; break;
        case ClassDef::Interface:  result+=" Interfejs"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategorija"; break;
        case ClassDef::Exception:  result+=" Izuzetak"; break;
        default: break;
      }
      result+=" - sažet pregled";
      if (isTemplate) result+=" šablona";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" - sažet pregled modula";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Članovi modula"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Spisak svih ";
      if (!extractAll) result+="dokumentovanih ";
      result+="članova modula sa vezama ka ";
      if (extractAll)
      {
        result+="dokumentaciji za svakog člana modula:";
      }
      else
      {
        result+="modulima kojima pripadaju:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Spisak modula"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "i");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentacija za ovaj ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modul"; break;
        case ClassDef::Struct:     result+="tip"; break;
        case ClassDef::Union:      result+="uniju"; break;
        case ClassDef::Interface:  result+="interfejs"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategoriju"; break;
        case ClassDef::Exception:  result+="izuzetak"; break;
        default: break;
      }
      result+=" napravljena je automatski od sledeć";
      if (single) result+="e datoteke:"; else result+="ih datoteka:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "ovi");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "procedur", "e", "a");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Ograničenja tipova";
    }

    /*! Used for Java interfaces in the summary section of Java packages */
    QCString trInterfaces() override
    {
      return "Interfejsi";  //!< Radna okruzenja. Ali to je dve reci.
    }

    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "pon", "uto", "sre", "čet", "pet", "sub", "ned" };
      static const char *days_full[]    = { "ponedeljak", "utorak", "sreda", "četvrtak", "petak", "subota", "nedelja" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "maj", "jun", "jul", "avg", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januar", "februar", "mart", "april", "maj", "jun", "jul", "avgust", "septembar", "oktobar", "novembar", "decembar" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "pre podne", "po podne" };
      return dayPeriod[period?1:0];
    }
};

#endif
