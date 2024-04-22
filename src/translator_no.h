/******************************************************************************
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

/******************************************************************************
 * Norwegian translation by Lars Erik Jordet <lejordet@gmail.com>, parts by Frode Nilsen
 *
 * This is a new translation made from scratch, not based on my older Norwegian translation (for 1.2.2)
 *
 * Translation notes (in Norwegian)
 *
 * Jeg har stort sett brukt ord som ligger ganske nær de engelske ekvivalentene,
 * for eksempel "enumeration" -> "enumerasjon", og i enkelte tilfeller det engelske
 * ordet direkte, der jeg finner det mer naturlig enn å prøve å stable en setning
 * på beina på norsk, eller jeg selv foretrekker det engelske ordet (eks: "Header-fil").
 * Om noen ikke skulle like disse valgene, kontakt meg på mailadressen over.
 *
 * Doxygen har mange strings som består av sammensatte ord ("Member function description", for eksempel),
 * som ikke alltid ser like ryddig ut på norsk. Jeg har brukt bindestrek for å få
 * det til å se presentabelt ut, men om noen har en bedre idé, send til mailadressen over.
 *
 * 2006-03-06:
 * Jeg bruker ikke doxygen selv lenger, så det går nok litt i lengste laget mellom oppdateringer...
 *
 * Changelog
 *
 * 2003-12-18: Initial translation
 * 2004-07-19: Fixup to prepare for 1.3.8 (I had forgotten some functions)
 * 2006-03-06: Added a .diff from Frode Nilsen, now compatible with 1.4.6.
 */

#ifndef TRANSLATOR_NO_H
#define TRANSLATOR_NO_H

class TranslatorNorwegian : public TranslatorAdapter_1_4_6
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
    { return "norwegian"; }

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
     */
    QCString latexLanguageSupportCommand() override
    {
      return
        "\\usepackage[norsk]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "nn";
    }
    QCString getLanguageString() override
    {
      return "0x814 Norwegian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Relaterte funksjoner"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Merk at disse ikke er medlemsfunksjoner.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Detaljert beskrivelse"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detaljar"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Medlemstypedef-dokumentasjon"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Medlemsenumerasjon-dokumentasjon"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Medlemsfunksjon-dokumentasjon"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Feltdokumentasjon";
      }
      else
      {
        return "Medlemsdata-dokumentasjon";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Mer..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Liste over alle medlemmer"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Medlemsliste"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Dette er den fullstendige listen over medlemmer for"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", alle arvede medlemmer inkludert."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Generert automatisk av Doxygen";
      if (!s.isEmpty()) result+=" for "+s;
      result+=" fra kildekoden.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "enum-navn"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "enum-verdi"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definert i"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduler"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Klassehierarki"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasseliste";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Fil-liste"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datafelt";
      }
      else
      {
        return "Klassemedlemmer";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globale";
      }
      else
      {
        return "Filmedlemmer";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Relaterte sider"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Eksempler"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Søk"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Denne arvelisten er grovsortert alfabetisk "
             "(ikke nødvendigvis korrekt):";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="filer med korte beskrivelser:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
	return "Her er datastrukturene med korte beskrivelser:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
	return "Her er klasser med korte beskrivelser:";
      }
      else
      {
	return "Her er klasser, struct'er, "
	       "unioner og interface'er med korte beskrivelser:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll)
      {
        result+="dokumenterte ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struct- og unionfelter";
      }
      else
      {
        result+="klassemedlemmer";
      }
      result+=" med koblinger til ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struct/union dokumentasjon for hvert felt:";
        }
        else
        {
	  result+="klassedokumentasjonen for hvert medlem:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
	  result+="struct'ene/unionene de hører til:";
        }
        else
        {
	  result+="klassene de hører til:";
        }
      }
      return result;
    }
    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterte ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funksjoner, variabler, definisjoner, enum'er, og typedef'er";
      }
      else
      {
        result+="filmedlemmer";
      }
      result+=" med koblinger til ";
      if (extractAll)
        result+="filene de hører til:";
      else
        result+="dokumentasjonen:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Her er en liste over alle eksemplene:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Her er en liste over alle relaterte dokumentasjonssider:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Her er en liste over alle moduler:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentasjon"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Modulindeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hierarkisk indeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturindeks";
      }
      else
      {
        return "Klasseindeks";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Filindeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Moduldokumentasjon"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturdokumentasjon";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klassedokumentasjon";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Fildokumentasjon"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Referansemanual"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definisjoner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Typedef'er"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerasjoner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funksjoner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variabler"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Enumerasjonsverdier"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Define-dokumentasjon"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Typedef-dokumentasjon"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Enumerasjontype dokumentasjon"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Enumerasjonsverdi dokumentasjon"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Funksjonsdokumentasjon"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Variabeldokumentasjon"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasser";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generert "+date;
      if (!projName.isEmpty()) result+=" for "+projName;
      result+=" av";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Arvediagram for "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Advarsel"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versjon"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Dato"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Returnerer"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Se også"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametre"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Unntak"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generert av"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Navneromsliste"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="navnerom med korte beskrivelser:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Venner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Venner og relatert funksjonsdokumentasjon"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse"; break;
        case ClassDef::Struct:     result+=" Struct"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Grensesnitt"; break;
        case ClassDef::Exception:  result+=" Unntak"; break;
        case ClassDef::Protocol:   result+=" Protocol"; break;
        case ClassDef::Category:   result+=" Category"; break;
        default: break;
      }
      if (isTemplate) result+=" Mal";
      result+=" Referanse";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" filreferanse";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" navneromsreferanse";
      return result;
    }

    QCString trPublicMembers() override
    { return "Public medlemsfunksjoner"; }
    QCString trPublicSlots() override
    { return "Public slots"; }
    QCString trSignals() override
    { return "Signaler"; }
    QCString trStaticPublicMembers() override
    { return "Statiske public medlemsfunksjoner"; }
    QCString trProtectedMembers() override
    { return "Protected memdlemsfunksjoner"; }
    QCString trProtectedSlots() override
    { return "Protected slots"; }
    QCString trStaticProtectedMembers() override
    { return "Statiske protected medlemsfunksjoner"; }
    QCString trPrivateMembers() override
    { return "Private medlemsfunksjoner"; }
    QCString trPrivateSlots() override
    { return "Private slots"; }
    QCString trStaticPrivateMembers() override
    { return "Statiske private medlemsfunksjoner"; }

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
            result+=", og ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Arver "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Arvet av "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementert fra "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementert i "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Navneromsmedlemmer"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="navneromsmedlemmer med koblinger til ";
      if (extractAll)
        result+="navneromsdokumentasjonen for hvert medlem:";
      else
        result+="navnerommet de hører til:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Navneromsindeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Navneromsdokumentasjon"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Navnerom"; }

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
      QCString result="Dokumentasjonen for ";
      switch(compType)
      {
        case ClassDef::Class:      result+="denne klasse"; break;
        case ClassDef::Struct:     result+="denne struct"; break;
        case ClassDef::Union:      result+="denne union"; break;
        case ClassDef::Interface:  result+="dette interface"; break;
        case ClassDef::Exception:  result+="dette unntak"; break;
        case ClassDef::Protocol:   result+="denne protocol"; break;
        case ClassDef::Category:   result+="denne category"; break;
        default: break;
      }
      result+=" ble generert fra følgende fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Returverdier"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Hovedside"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definisjon på linje @0 i filen @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definisjon i filen @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Foreldet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Samarbeidsdiagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Avhengighetsgraf for "+fName+":";
    }

    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Konstruktør- & destruktør-dokumentasjon";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Gå til kildekoden til denne filen.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Gå til dokumentasjonen til denne filen.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Forhåndsbetingelse";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Etterbetingelse";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Startverdi:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "kode";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafisk klassehierarki";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Gå til det grafiske klasse hierarkiet";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Gå til tekst-klassehierarki";
    }
    QCString trPageIndex() override
    {
      return "Innhold";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Notat";
    }
    QCString trPublicTypes() override
    {
      return "Public typer";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datafelt";
      }
      else
      {
        return "Public attributter";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Statiske public attributter";
    }
    QCString trProtectedTypes() override
    {
      return "Protected typer";
    }
    QCString trProtectedAttribs() override
    {
      return "Protected attributter";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statiske protected attributter";
    }
    QCString trPrivateTypes() override
    {
      return "Private typer";
    }
    QCString trPrivateAttribs() override
    {
      return "Private attributter";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statiske private attributter";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Todo-liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referert av";
    }
    QCString trRemarks() override
    {
      return "Merknader";
    }
    QCString trAttention() override
    {
      return "Viktig";
    }
    QCString trInclByDepGraph() override
    {
      return "Denne grafen viser hvilke filer som direkte eller "
             "indirekte inkluderer denne filen:";
    }
    QCString trSince() override
    {
      return "Siden";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Symbolforklaring";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
    {
      return
	"Denne siden forklarer hvordan man tolker grafene doxygen genererer.<p>\n"
	"Vi baserer oss på følgende eksempel:\n"
        "\\code\n"
        "/*! Usynlig klasse pga. trunkasjon */\n"
        "class Invisible { };\n\n"
        "/*! Trunkert klasse, arve-relasjon er skjult */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klasse som ikke er dokumentert med doxygen-kommentarer */"
        "class Undocumented { };\n\n"
        "/*! Klasse med public-arv */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasse med protected-arv */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasse med private-arv */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasse som blir brukt av klassen Inherited */\n"
        "class Used { };\n\n"
        "/*! Super-klasse som arver flere andre klasser */\n"
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
        "Hvis \\c MAX_DOT_GRAPH_HEIGHT er satt til 200 i "
        "konfigurasjonsfila vil dette resultere i følgende graf:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Boksene i grafen over betyr følgende:\n"
        "<ul>\n"
        "<li>En fylt svart boks representerer klassen grafen "
        "er generert for.\n"
	"<li>En boks med svart ramme angir en dokumentert struct eller klasse.\n"
	"<li>En boks med grå ramme angir en udokumentert struct eller klasse.\n"
	"<li>En boks med rød ramme angir en dokumentert struct eller klasse "
	"der ikke alle relasjoner er vist. En graf blir trunkert om den ikke "
	"passer innenfor de spesifiserte rammene.\n"
	"</ul>\n"
	"Pilene i grafen har følgende betydning:\n"
	"</ul>\n"
	"<li>En mørk blå pil brukes til å visualisere public-arv mellom to klasser.\n"
	"<li>En mørk grønn pil brukes for protected-arv.\n"
	"<li>En mørk rød pil angir private-arv.\n"
	"<li>En stiplet lilla pil angir at en klasse er inkludert eller brukt "
	"i en annen klasse. Pilen er merket med variablen(e) klassen "
	"er tilgjengelig gjennom.\n"
	"</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "symbolforklaring";
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
      return "Test-liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Egenskaper";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Egenskaps-dokumentasjon";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    QCString trInterfaces() override
    {
      return "Grensesnitt";
    }
    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasser";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Her er pakkene med korte beskrivelser (om tilgjengelig):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pakker";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Verdi:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Feil";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Feil-liste";
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
      return "0";
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
      return createNoun(first_capital, singular, "klasse", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fil", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, false, "navnerom", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "gruppe", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "side", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "medlem", "mer");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "forfatter", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referanser";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementerer "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementert i "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Innholdsfortegnelse";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Liste over foreldede enheter";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Hendelser";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Hendelsesdokumentasjon";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Pakketyper";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Pakkefunksjoner";
    }
    QCString trPackageMembers() override
    {
      return "Pakkemedlemmer";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statiske Pakkefunksjoner";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Pakkeattributter";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statiske Pakkeattributter";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Her er kall-grafen for denne funksjonen:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Søkeresultater";
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
        return "Beklager, men ingen dokumenter ble funnet.";
      }
      else if (numDocuments==1)
      {
        return "Fant <b>ett</b> dokument som passet ditt søk.";
      }
      else
      {
        return "Fant <b>$num</b> dokumenter som passet ditt søk. "
               "Viser beste treff først.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Treff:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
         */
    QCString trSourceFile(const QCString& filename) override
    {
       	return "Kildefil " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Katalog hierarki"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Katalogdokumentasjon"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Kataloger"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Katalog referanse"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "katalog", "er");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Dette er en overloaded medlemsfunksjon, "
              "generert for deg. Den skiller seg ut fra "
              "funksjonen ovenfor i argument(ene) den aksepterer.";
    }

    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "må.", "ty.", "on.", "to.", "fr.", "la.", "sø." };
      static const char *days_full[]    = { "måndag", "tysdag", "onsdag", "torsdag", "fredag", "laurdag", "søndag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "mai", "jun", "jul", "aug", "sep", "okt", "nov", "des" };
      static const char *months_full[]  = { "januar", "februar", "mars", "april", "mai", "juni", "juli", "august", "september", "oktober", "november", "desember" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "f.m.", "e.m." };
      return dayPeriod[period?1:0];
    }
};

#endif
