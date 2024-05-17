/*-*- c-basic-offset: 2; tab-width: 8 -*-*/
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
 /*! @file
  * @brief Danish translation
  */

/*
 * Danish translation by
 * Erik Søe Sørensen <eriksoe@daimi.au.dk>
 *
 * First version (not complete) for Doxygen 1.2.7
 * Extended and revised for Doxygen 1.3
 * Extended and revised for Doxygen 1.3.4
 * Extended and revised for Doxygen 1.3.8
 */

/*  Translator's notes:

	Oversættelseskonventioner:
	(Konventioner for konventioner:
	  '?'    angiver oversættelser, jeg har været i tvivl om
	  '??'   angiver tvivlsomme oversættelser
	  '..?'  angiver ord, der endnu ikke er fundet en oversættelse til
 	  '(do.)' angiver ord, der med vilje ikke er oversat, idet jeg selv
             overvejende bruger det engelske udtryk
	  '(-> _)' angiver ord, der er fundet en oversættelse til, men som jeg
	        vægrer mig ved at oversætte.
          'KLID:_' angiver ord, hvor jeg med overlæg har rettet mig efter
		KLID.dk's oversættelsesguide (enig eller ej).
	)
	bug -> 'kendt fejl'
	class -> klasse
	compound -> 'sammensat type'
	constructor -> konstruktør ?
	destructor -> destruktør ?
	directory -> KLID:katalog (kunne også være 'bibliotek','mappe','folder')
	event -> begivenhed ?
	exception (-> undtagelse ?)
	friend ..?
	interface -> grænseflade ?
	member -> medlem (TODO)
	namespace -> (do.)
	overloaded -> KLID:overdefineret
	private -> privat
	property -> egenskab?
	protected -> beskyttet ??
	public -> offentlig
	reference(vb) -> "indeholde referencer til" (?)
	slot ..?
	source code -> kildekode
	struct -> datastruktur
	template (-> skabelon ?)
	typedef -> typedefinition (?)
	todo -> (do.)
	union ..?

	Specielle forbindelser:
	'Inheritance diagram' -> Stamtræ  (selvom Nedarvningsdiagram også gik an)


	-----

	(Konstruktivt) input modtages med glæde!
	-- Erik Søe Sørensen <eriksoe@daimi.au.dk>


	links -> (links.)
	-- Poul-Erik Hansen

 */

#ifndef TRANSLATOR_DK_H
#define TRANSLATOR_DK_H

class TranslatorDanish : public TranslatorAdapter_1_8_0
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
    { return "danish"; }

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
	"\\usepackage[danish]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "da";
    }
    QCString getLanguageString() override
    {
      return "0x406 Danish";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Relaterede funktioner"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Bemærk at disse ikke er medlems-funktioner.)"; }

    /*! header that is put before the detailed description of files,
	 * classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Detaljeret beskrivelse"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detaljer"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentation af medlems-typedefinitioner"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentation af medlems-enumerationer"; }
	// medlems-enumerationer -> 'indeholdte enumerationer'

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Dokumentation af medlemsfunktioner"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Felt-dokumentation";
      } else {
	return "Dokumentation af feltvariable";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Mere..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Liste over alle medlemmer"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Liste over medlemmer"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Dette er den komplette liste over medlemmer i"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", inklusive alle nedarvede medlemmer."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automatisk genereret af Doxygen";
      if (!s.isEmpty()) result+=" for "+s;
      result+=" ud fra kildekoden.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "enumerationsnavn"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "enumerationsværdi"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "defineret i"; }

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
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Datastrukturer";
      } else {
        return "Oversigt over sammensatte typer";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Filoversigt"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Data-felter";
      } else {
        return "Felter i sammensatte typer";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Globale symboler";
      } else {
	return "Placering i filer"; // Fil-medlemmer"; //TODO
	//"Globale definitioner" ?
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Relaterede sider"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Eksempler"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Søg"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Denne nedarvningsliste er sorteret næsten - "
             "men ikke nødvendigvis helt - alfabetisk:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";
      result+="filer med korte beskrivelser:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Her er datastrukturerne med korte beskrivelser:";
      } else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE)) {
        return "Her er klasserne med korte beskrivelser:";
      } else {
        return "Her er klasserne, datastrukturerne, "
               "unionerne og grænsefladerne med korte beskrivelser:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) {
        result+="dokumenterede ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result+="felter i datastrukturer og unioner";
      } else {
        result+="klassemedlemmer";
      }
      result+=" med links til ";
      if (!extractAll) {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
          result+="datastruktur/unions-dokumentationen for hvert felt:";
        } else {
          result+="klassedokumentationen for hvert medlem:";
        }
      } else {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
          result+="de datastrukturer/unioner, de hører til:";
        } else {
          result+="de klasser, de hører til:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result+="funktioner, variable, #defines, enumerationer "
	    "og typedefinitioner";
      } else {
        result+="fil-medlemmer";
      }
      result+=", med links til ";
      if (extractAll)
        result+="de filer, de tilhører:";
      else
        result+="deres dokumentation:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Her er en liste over alle eksempler:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Her er en liste over alle relaterede dokumentationssider:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Her er en liste over alle moduler:"; }

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Modul-indeks"; }

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
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Indeks over datastrukturer";
      } else {
        return "Indeks over sammensatte typer";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Fil-indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Modul-dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastruktur-documentation";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klasse-dokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Fil-dokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Referencemanual"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "#Defines"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Typedefinitioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerationer"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funktioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variable"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Enumerationsværdier"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "#Define-dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentation af typedefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentation af enumerations-typer"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentation af enumerations-værdier"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Funktions-dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Variabel-dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Datastrukturer";
      } else {
        return "Sammensatte typer";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Genereret "+date;
      if (!projName.isEmpty()) result+=" for "+projName;
      result+=" af";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Stamtræ for "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Advarsel"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Version"; }

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
    { return "Undtagelser"; }    // "Exceptions"

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Genereret af"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Oversigt over namespaces"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";
      result+="namespaces med korte beskrivelser:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentation af friends og af relaterede funktioner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName+" ";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse-"; break;
        case ClassDef::Struct:     result+=" Datastruktur-"; break;
        case ClassDef::Union:      result+=" Union-"; break;
        case ClassDef::Interface:  result+=" Grænseflade-"; break;
        case ClassDef::Protocol:   result+=" Protokol-"; break;
        case ClassDef::Category:   result+=" Kategori-"; break; // " Category-"
        case ClassDef::Exception:  result+=" Undtagelse-"; break;   //  " Exception"
        default: break;
      }
      if (isTemplate) result+="skabelon-";  // "template-"
      result+="reference";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" filreference";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" namespace-reference";
      return result;
    }

    QCString trPublicMembers() override
    { return "Offentlige metoder"; }
    QCString trPublicSlots() override
    { return "Offentlige slots"; }
    QCString trSignals() override
    { return "Signaler"; }
    QCString trStaticPublicMembers() override
    { return "Statiske, offentlige metoder"; }
    QCString trProtectedMembers() override
    { return "Beskyttede metoder"; }
    QCString trProtectedSlots() override
    { return "Beskyttede slots"; }
    QCString trStaticProtectedMembers() override
    { return "Statiske, beskyttede metoder"; }
    QCString trPrivateMembers() override
    { return "Private metoder"; }
    QCString trPrivateSlots() override
    { return "Private slots"; }
    QCString trStaticPrivateMembers() override
    { return "Statiske, private metoder"; }

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

        if (i!=numEntries-1) { // not the last entry, so we need a separator
          if (i<numEntries-2) // not the fore last entry
            result+=", ";
          else                // the fore last entry
            result+=" og ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Nedarver "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Nedarvet af "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Overskriver metode fra "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementeret i "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Namespace-medlemmer"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";
      result+="namespace-medlemmer med links til ";
      if (extractAll)
        result+="namespace-dokumentationen for hvert medlem:";
      else
        result+="det namespace, de hører til:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Namespace-indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Namespace-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Namespaces"; }

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
      QCString result="Dokumentationen for denne ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klasse"; break;
        case ClassDef::Struct:     result+="datastruktur"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="grænseflade"; break;
        case ClassDef::Protocol:   result+="protokol"; break; // "protocol"
        case ClassDef::Category:   result+="kategori"; break; // "category"
        case ClassDef::Exception:  result+="undtagelse"; break;  // "exception"
        default: break;
      }
      result+=" blev genereret ud fra fil";
      if (single) result+="en:"; else result+="erne:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Returværdier"; }

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
      return "Defineret på linje @0 i filen @1.";
    }

    QCString trDefinedInSourceFile() override
    {
      return "Defineret i filen @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Frarådes - fortidslevn"; // ?? - What is the context?
	  // "Ugleset" :)
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Samarbejdsdiagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Inklusions-afhængighedsgraf for "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentation af konstruktører og destruktører";
      // "Constructor & Destructor dokumentation";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    { return "Hop til denne fils kildekode."; }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    { return "Hop til denne fils dokumentation."; }

    /*! Text for the \\pre command */
    QCString trPrecondition() override
    { return "Forudsætninger (precondition)"; }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    { return "Resultat (postcondition)"; }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    { return "Invariant"; }

    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    { return "Startværdi:"; }

    /*! Text used the source code in the file index */
    QCString trCode() override
    { return "kildekode"; }
    QCString trGraphicalHierarchy() override
    { return "Grafisk klassehierarki"; }
    QCString trGotoGraphicalHierarchy() override
    { return "Hop til det grafiske klassehierarki"; }
    QCString trGotoTextualHierarchy() override
    { return "Hop til det tekstuelle klassehierarki"; }
    QCString trPageIndex() override
    { return "Sideindeks"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    { return "Note"; }
    QCString trPublicTypes() override
    { return "Offentlige typer"; }

    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Datafelter";
      } else {
        return "Offentlige attributter";
      }
    }

    QCString trStaticPublicAttribs() override
    { return "Statiske, offentlige attributter"; }
    QCString trProtectedTypes() override
    { return "Beskyttede typer"; }
    QCString trProtectedAttribs() override
    { return "Beskyttede attributter"; }
    QCString trStaticProtectedAttribs() override
    { return "Statiske, beskyttede attributter"; }
    QCString trPrivateTypes() override
    { return "Private typer"; }
    QCString trPrivateAttribs() override
    { return "Private attributter"; }
    QCString trStaticPrivateAttribs() override
    { return "Statiske, private attributter"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
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
      return "Refereret til af";
    }
    QCString trRemarks() override
    {
      return "Bemærkninger";
    }
    QCString trAttention() override
    {
      return "OBS";
    }
    QCString trInclByDepGraph() override
    {
      return
	"Denne graf viser, hvilke filer der direkte eller "
	"indirekte inkluderer denne fil:";
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
      return "Graf-forklaring";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override //TODO
    {
      return
		"Denne side forklarer, hvordan man skal fortolke de grafer, "
		"der genereres af doxygen.<p>\n"
        "Tag følgende eksempel:\n"
        "\\code\n"
        "/*! Klasse der er usynlig pg.a. beskæring */\n"
        "class Invisible { };\n\n"
        "/*! Beskåret klasse: nedarvningsrelation er skjult */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klasse der ikke er dokumenteret med doxygen-kommentarer */\n"
        "class Undocumented { };\n\n"
        "/*! Klasse der nedarves fra offentligt */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! En template-klasse */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasse der nedarves fra beskyttet */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasse der nedarves fra privat */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasse der bruges af Inherited-klassen */\n"
        "class Used { };\n\n"
        "/*! Klasse der nedarver en masse andre klasser */\n"
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
        "Hvis \\c MAX_DOT_GRAPH_HEIGHT i konfigurationsfilen "
        "er sat til 240, vil dette resultere i følgende graf:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "De forskellige slags kasser i ovenstående graf har følgende "
		"betydninger:\n"
        "<ul>\n"
        "<li>%En udfyldt sort kasse repræsenterer den datastruktur eller "
		"klasse, grafen er genereret for.\n"
        "<li>%En kasse med sort kant betegner en dokumenteret datastruktur "
		" eller klasse.\n"
        "<li>%En kasse med grå kant betegner en udokumenteret datastruktur "
 		" eller klasse.\n"
        "<li>%En kasse med rød kant betegner en dokumenteret datastruktur "
 		" eller klasse, for hvilken ikke alle "
		"nedarvnings- og indeholdelses-relationer er vist. "
        "%Grafer beskæres, hvis de fylder mere end de specificerede dimensioner.\n "
        "</ul>\n"
        "Pilene har følgende betydninger:\n"
        "<ul>\n"
        "<li>%En mørkeblå pil viser en offentlig nedarvningsrelation "
		"mellem to klasser.\n"
        "<li>%En mørkegrøn pil viser en beskyttet nedarvningsrelation.\n"
        "<li>%En mørkerød pil viser en privat nedarvningsrelation.\n"
        "<li>%En lilla, stiplet pil bruges, når en klasse er indeholdt i "
		"eller benyttes af en anden klasse. "
        "Ved pilen står navnet på den eller de variable, gennem hvilke(n) "
		"den klasse, pilen peger på, er tilgængelig.\n"
        "<li>%En gul, stiplet pil viser forholdet mellem en template-instans "
		"og den template-klasse, den er instantieret fra."
        "Ved pilen står template-parametrene brugt ved instantieringen.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "forklaring til graf";
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
      return "Testliste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Egenskaber";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Egenskabsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "Datastrukturer";
      } else {
        return "Klasser";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Pakke "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return
	"Her er en liste over pakkerne, med korte beskrivelser "
	"(hvor en sådan findes):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pakker";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Værdi:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Kendte fejl";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Liste over kendte fejl";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file
	 * (used table extract:)
	 * <pre>
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * ANSI_CHARSET              0 (x00)            1252
	 * </pre>
	 */
    QCString trRTFansicp() override
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset */
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
      return createNoun(first_capital, singular, "namespace", "s");
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
      return "Indeholder referencer til";
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
      return "Implementeret i "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Indholdsfortegnelse";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Liste over fortidslevn, hvis brug frarådes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Begivenheder";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Begivenhedsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /* Java: Entities with package scope... */
    QCString trPackageTypes() override
    { return "Typer med pakke-scope"; }
    QCString trPackageFunctions() override
    { return "Metoder med pakke-scope"; }
    QCString trPackageMembers() override
    { return "Medlemmer med pakke-scope"; }
    QCString trStaticPackageFunctions() override
    { return "Statiske metoder med pakke-scope"; }
    QCString trPackageAttribs() override
    { return "Attributter med pakke-scope"; }
    QCString trStaticPackageAttribs() override
    { return "Statiske attributter med pakke-scope"; }

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
      return "Her er kald-grafen for denne funktion:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Søgeresultater";
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
      if (numDocuments==0) {
        return "Beklager - ingen dokumenter passede til din forespørgsel.";
      } else if (numDocuments==1) {
        return "Fandt <b>1</b> dokument, der passer til din forespørgsel.";
      } else {
        return
	  "Fandt <b>$num</b> dokumenter, der passer til din forespørgsel. "
	  "De, der passer bedst, vises først.";
      }
    }

    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Fundne ord:"; //translation?
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for
     *  file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Kildefilen " + filename;
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Katalogstruktur"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Katalog-dokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Kataloger"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
      { QCString result="Indhold af kataloget "; result+=dirName; return result;}

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
       return "Dette er en overdefineret medlemsfunktion, "
              "defineret af bekvemmelighedshensyn. "
	      "Den adskiller sig kun fra den ovenstående funktion i, "
	      "hvilke argumenter den tager.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Her er kalder-grafen for denne funktion:";
    }

    // None translated yet PEH 2010-11-27
    // Subroutine

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Medlem Funktion/Subroutine Dokumentation"; }  // "Member Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    /*
    QCString trEnumerationValueDocumentation() override
      { return "Enumerator-dokumentation"; } //TODO?
*/


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Data Type Liste"; }       // "Data Types List"

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Data felter"; }   // "Data Fields"

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Her er de datatyper med kort beskrivelse:"; }  // "Here are the data types with brief descriptions:"

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Her er en liste af alle "; // "Here is a list of all "
      if (!extractAll)
      {
        result+="dokumenteret ";  // "documented "
      }
      result+="datatype medlemmer"; //  "data types members"
      result+=" med links til ";    // " with links to "
      if (!extractAll)
      {
         result+="data strukturer dokumenteret for hver medlem";    // "the data structure documentation for each member"
      }
      else
      {
         result+="de datatyper som de tilhører:";  // "the data types they belong to:"
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Datatype indeks"; }   // "Data Type Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Datatype dokumentation"; }   // "Data Type Documentation"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funktioner/Subroutiner"; } // "Functions/Subroutines"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Funktion/Subroutine dokumentation"; } // "Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Datatyper"; }    // "Data Types"

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Modulliste"; }  // "Modules List"

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Her er en liste over alle ";     // "Here is a list of all "
      if (!extractAll) result+="dokumenteret ";       // "documented "
      result+="moduler med kort beskrivelse:";   // "modules with brief descriptions:"
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
        case ClassDef::Class:      result+=" Modul"; break;     // " Module"
        case ClassDef::Struct:     result+=" Type"; break;      // " Type"
        case ClassDef::Union:      result+=" Union"; break;     // " Union"
        case ClassDef::Interface:  result+=" Grænseflade"; break; // " Interface"
        case ClassDef::Protocol:   result+=" Protocol"; break;      // " Protocol"
        case ClassDef::Category:   result+=" Kategori"; break;      // " Category"
        case ClassDef::Exception:  result+=" Undtagelse"; break; // " Exception"
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+=" Reference";
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Modulreference";  // " Module Reference"
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Modulmedlemmer"; }    // "Module Members"

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Her er en list over alle ";     // "Here is a list of all "
      if (!extractAll) result+="Dokumentet ";       // "documented "
      result+="modulmemlemmer med link til ";      // "module members with links to "
      if (extractAll)
      {
        result+="dokumentation for hvert model medlem:";    // "the module documentation for each member:"
      }
      else
      {
        result+="moduler de tilhøre:";  // "the modules they belong to:"
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Modulindekser"; }     // "Modules Index"

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "er");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="The documentation for this ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modul"; break;        // "module"
        case ClassDef::Struct:     result+="type"; break;         // "type"
        case ClassDef::Union:      result+="union"; break;        // "union"
        case ClassDef::Interface:  result+="grænseflade"; break;  // "interface"
        case ClassDef::Protocol:   result+="protokol"; break;     // "protocol"
        case ClassDef::Category:   result+="kategori"; break;     // "category"
        case ClassDef::Exception:  result+="undtagelse"; break;   // "exception"
        default: break;
      }
      result+=" var lavet udfra følgende file";                   // " was generated from the following file"
      if (single) result+=":"; else result+="r:";                 // ":" / "s:"
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "er");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "typebegrænsninger";        // "Type Constraints"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Relation";        // " Relation"
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Indlæser...";          // "Loading..."
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Globalt Namespace";    // "Global Namespace"
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Søger...";    // "Searching..."
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Ingen fund";      // "No Matches"
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
      return "File i "+name;     // "File in "
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Inkluderer file i "+name;    // "Includes file in "
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
      static const char *days[]   = { "Man","Tir","Ons","Tor","Fre","Lør","Søn" }; // { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Maj","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };    //  { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
      static const char *days_short[]   = { "ma", "ti", "on", "to", "fr", "lø", "sø" };
      static const char *days_full[]    = { "mandag", "tirsdag", "onsdag", "torsdag", "fredag", "lørdag", "søndag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "maj", "jun", "jul", "aug", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januar", "februar", "marts", "april", "maj", "juni", "juli", "august", "september", "oktober", "november", "december" };
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

    QCString trCiteReferences() override
    {
      return "Bibliografiske referencer";
    }

    QCString trCopyright() override
    {
      return "Copyright";
    }

    QCString trDirDepGraph(const QCString &name) override
    {
      return QCString("Afhængighedsgraf for katalog ")+name+":";
    }
};

#endif
