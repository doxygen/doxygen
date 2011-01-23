/*-*- c-basic-offset: 2; tab-width: 8 -*-*/
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
 */
 /*! @file
  * @brief ´Danish translation
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

// class TranslatorDanish : public TranslatorAdapter_1_5_4
class TranslatorDanish : public Translator
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
    virtual QCString latexLanguageSupportCommand()
    {
      return
	"\\usepackage[danish]{babel}\n"
	"\\usepackage[T1]{fontenc}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-1";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Relaterede funktioner"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Bemærk at disse ikke er medlems-funktioner.)"; }

    /*! header that is put before the detailed description of files,
	 * classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detaljeret beskrivelse"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentation af medlems-typedefinitioner"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentation af medlems-enumerationer"; }
	// medlems-enumerationer -> 'indeholdte enumerationer'

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentation af medlemsfunktioner"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Felt-dokumentation";
      } else {
	return "Dokumentation af feltvariable";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Mere..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Liste over alle medlemmer."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Liste over medlemmer"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Dette er den komplette liste over medlemmer i "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", inklusive alle nedarvede medlemmer."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatisk genereret af Doxygen";
      if (s) result+=(QCString)" for "+s;
      result+=" ud fra kildekoden.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enumerationsnavn"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enumerationsværdi"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "defineret i"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduler"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klassehierarki"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Datastrukturer";
      } else {
        return "Oversigt over sammensatte typer";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Filoversigt"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Data-felter";
      } else {
        return "Felter i sammensatte typer";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Globale symboler";
      } else {
	return "Placering i filer"; // Fil-medlemmer"; //TODO
	//"Globale definitioner" ?
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Relaterede sider"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Eksempler"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Søg"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Denne nedarvningsliste er sorteret næsten - "
             "men ikke nødvendigvis helt - alfabetisk:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";
      result+="filer med korte beskrivelser:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Her er datastrukturerne med korte beskrivelser:";
      } else {
        return "Her er klasserne, datastrukturerne, "
               "unionerne og grænsefladerne med korte beskrivelser:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) {
        result+="dokumenterede ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        result+="felter i datastrukturer og unioner";
      } else {
        result+="klassemedlemmer";
      }
      result+=" med links til ";
      if (!extractAll) {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
          result+="datastruktur/unions-dokumentationen for hvert felt:";
        } else {
          result+="klassedokumentationen for hvert medlem:";
        }
      } else {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
          result+="de datastrukturer/unioner, de hører til:";
        } else {
          result+="de klasser, de hører til:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
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
    virtual QCString trExamplesDescription()
    { return "Her er en liste over alle eksempler:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Her er en liste over alle relaterede dokumentationssider:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Her er en liste over alle moduler:"; }

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modul-indeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarkisk indeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Indeks over datastrukturer";
      } else {
        return "Indeks over sammensatte typer";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "Fil-indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modul-dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Datastruktur-documentation";
      } else {
        return "Klasse-dokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Fil-dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Eksempel-dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Side-dokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referencemanual"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "#Defines"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Funktionsprototyper"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typedefinitioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerationer"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funktioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variable"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Enumerationsværdier"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "#Define-dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentation af funktionsprototyper"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentation af typedefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentation af enumerations-typer"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentation af enumerations-værdier"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funktions-dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Variabel-dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Datastrukturer";
      } else {
        return "Sammensatte typer";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Genereret "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" af";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    { return "skrevet af"; }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Stamtræ for "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Kun til intern brug."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Advarsel"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Dato"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Returnerer"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Se også"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametre"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Undtagelser"; }    // "Exceptions"

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Genereret af"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Oversigt over namespaces"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterede ";
      result+="namespaces med korte beskrivelser:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Dokumentation af friends og af relaterede funktioner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse-"; break;
        case ClassDef::Struct:     result+=" Datastruktur-"; break;
        case ClassDef::Union:      result+=" Union-"; break;
        case ClassDef::Interface:  result+=" Grænseflade-"; break;
        case ClassDef::Protocol:   result+=" Protokol-"; break;
        case ClassDef::Category:   result+=" Kategori-"; break; // " Category-"
        case ClassDef::Exception:  result+=" Undtagelse-"; break;   //  " Exception"
      }
      if (isTemplate) result+="skabelon-";  // "template-"
      result+="reference";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" filreference";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" namespace-reference";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Offentlige metoder"; }
    virtual QCString trPublicSlots()
    { return "Offentlige slots"; }
    virtual QCString trSignals()
    { return "Signaler"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiske, offentlige metoder"; }
    virtual QCString trProtectedMembers()
    { return "Beskyttede metoder"; }
    virtual QCString trProtectedSlots()
    { return "Beskyttede slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statiske, beskyttede metoder"; }
    virtual QCString trPrivateMembers()
    { return "Private metoder"; }
    virtual QCString trPrivateSlots()
    { return "Private slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statiske, private metoder"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
    {
      QCString result;
      int i;
      // the inherits list contain `numEntries' classes
      for (i=0;i<numEntries;i++) {
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
    virtual QCString trInheritsList(int numEntries)
    {
      return "Nedarver "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Nedarvet af "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Overskriver metode fra "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementeret i "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Namespace-medlemmer"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Namespace-indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Namespace-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namespaces"; }

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
      QCString result=(QCString)"Dokumentationen for denne ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klasse"; break;
        case ClassDef::Struct:     result+="datastruktur"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="grænseflade"; break;
        case ClassDef::Protocol:   result+="protokol"; break; // "protocol"
        case ClassDef::Category:   result+="kategori"; break; // "category"
        case ClassDef::Exception:  result+="undtagelse"; break;  // "exception"
      }
      result+=" blev genereret ud fra fil";
      if (single) result+="en:"; else result+="erne:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Alfabetisk oversigt"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Returværdier"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hovedside"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Defineret på linje @0 i filen @1.";
    }

    virtual QCString trDefinedInSourceFile()
    {
      return "Defineret i filen @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Frarådes - fortidslevn"; // ?? - What is the context?
	  // "Ugleset" :)
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Samarbejdsdiagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Inklusions-afhængighedsgraf for "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Dokumentation af konstruktører og destruktører";
      // "Constructor & Destructor dokumentation";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    { return "Hop til denne fils kildekode."; }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    { return "Hop til denne fils dokumentation."; }

    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    { return "Forudsætninger (precondition)"; }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    { return "Resultat (postcondition)"; }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    { return "Invariant"; }

    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    { return "Startværdi:"; }

    /*! Text used the source code in the file index */
    virtual QCString trCode()
    { return "kildekode"; }
    virtual QCString trGraphicalHierarchy()
    { return "Grafisk klassehierarki"; }
    virtual QCString trGotoGraphicalHierarchy()
    { return "Hop til det grafiske klassehierarki"; }
    virtual QCString trGotoTextualHierarchy()
    { return "Hop til det tekstuelle klassehierarki"; }
    virtual QCString trPageIndex()
    { return "Sideindeks"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    { return "Note"; }
    virtual QCString trPublicTypes()
    { return "Offentlige typer"; }

    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Datafelter";
      } else {
        return "Offentlige attributter";
      }
    }

    virtual QCString trStaticPublicAttribs()
    { return "Statiske, offentlige attributter"; }
    virtual QCString trProtectedTypes()
    { return "Beskyttede typer"; }
    virtual QCString trProtectedAttribs()
    { return "Beskyttede attributter"; }
    virtual QCString trStaticProtectedAttribs()
    { return "Statiske, beskyttede attributter"; }
    virtual QCString trPrivateTypes()
    { return "Private typer"; }
    virtual QCString trPrivateAttribs()
    { return "Private attributter"; }
    virtual QCString trStaticPrivateAttribs()
    { return "Statiske, private attributter"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Todo-liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Refereret til af";
    }
    virtual QCString trRemarks()
    {
      return "Bemærkninger";
    }
    virtual QCString trAttention()
    {
      return "OBS";
    }
    virtual QCString trInclByDepGraph()
    {
      return
	"Denne graf viser, hvilke filer der direkte eller "
	"indirekte inkluderer denne fil:";
    }
    virtual QCString trSince()
    {
      return "Siden";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Graf-forklaring";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs() //TODO
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "forklaring til graf";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Testliste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP-metoder";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Egenskaber";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Egenskabsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Datastrukturer";
      } else {
        return "Klasser";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Pakke "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Pakkeoversigt";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return
	"Her er en liste over pakkerne, med korte beskrivelser "
	"(hvor en sådan findes):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pakker";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Værdi:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Kendte fejl";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "klasse", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "fil", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "namespace", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "gruppe", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "side", "r");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "medlem", "mer");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "global", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      return createNoun(first_capital, singular, "forfatter", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Indeholder referencer til";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementerer "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementeret i "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Indholdsfortegnelse";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Liste over fortidslevn, hvis brug frarådes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Begivenheder";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Begivenhedsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /* Java: Entities with package scope... */
    virtual QCString trPackageTypes()
    { return "Typer med pakke-scope"; }
    virtual QCString trPackageMembers()
    { return "Metoder med pakke-scope"; }
    virtual QCString trStaticPackageMembers()
    { return "Statiske metoder med pakke-scope"; }
    virtual QCString trPackageAttribs()
    { return "Attributter med pakke-scope"; }
    virtual QCString trStaticPackageAttribs()
    { return "Statiske attributter med pakke-scope"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Alle";
    }

    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Her er kald-grafen for denne funktion:";
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
      return "Søg efter";
    }

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Fundne ord:"; //translation?
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for
     *  file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Kildefilen " + filename;
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Katalogstruktur"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Katalog-dokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Kataloger"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Denne katalogstruktur er sorteret næsten - "
             "men ikke nødvendigvis helt - alfabetisk:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
      { QCString result="Indhold af kataloget "; result+=dirName; return result;}

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      return createNoun(first_capital, singular, "katalog", "er");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
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
    virtual QCString trCallerGraph()
    {
      return "Her er kalder-grafen for denne funktion:";
    }

    // None translated yet PEH 2010-11-27
    // Subroutine

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Medlem Funktion/Subroutine Dokumentation"; }  // "Member Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    /*
    virtual QCString trEnumerationValueDocumentation()
      { return "Enumerator-dokumentation"; } //TODO?
*/


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Data Type Liste"; }       // "Data Types List"

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Data felter"; }   // "Data Fields"

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Her er de datatyper med kort beskrivelse:"; }  // "Here are the data types with brief descriptions:"

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Datatype indeks"; }   // "Data Type Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Datatype dokumentation"; }   // "Data Type Documentation"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funktioner/Subroutiner"; } // "Functions/Subroutines"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Funktion/Subroutine dokumentation"; } // "Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datatyper"; }    // "Data Types"

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modulliste"; }  // "Modules List"

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";     // "Here is a list of all "
      if (!extractAll) result+="dokumenteret ";       // "documented "
      result+="moduler med kort beskrivelse:";   // "modules with brief descriptions:"
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Modul"; break;     // " Module"
        case ClassDef::Struct:     result+=" Type"; break;      // " Type"
        case ClassDef::Union:      result+=" Union"; break;     // " Union"
        case ClassDef::Interface:  result+=" Grænseflade"; break; // " Interface"
        case ClassDef::Protocol:   result+=" Protocol"; break;      // " Protocol"
        case ClassDef::Category:   result+=" Kategori"; break;      // " Category"
        case ClassDef::Exception:  result+=" Undtagelse"; break; // " Exception"
      }
      if (isTemplate) result+=" Template";
      result+=" Reference";
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Modulreference";  // " Module Reference"
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modulmedlemmer"; }    // "Module Members"

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Modulindekser"; }     // "Modules Index"

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modul" : "modul"));   // "Module" : "module"));
      if (!singular)  result+="er";  // "s";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"The documentation for this ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modul"; break;        // "module"
        case ClassDef::Struct:     result+="type"; break;         // "type"
        case ClassDef::Union:      result+="union"; break;        // "union"
        case ClassDef::Interface:  result+="grænseflade"; break;  // "interface"
        case ClassDef::Protocol:   result+="protokol"; break;     // "protocol"
        case ClassDef::Category:   result+="kategori"; break;     // "category"
        case ClassDef::Exception:  result+="undtagelse"; break;   // "exception"
      }
      result+=" var lavet udfra følgende file";                   // " was generated from the following file"
      if (single) result+=":"; else result+="r:";                 // ":" / "s:"
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Type" : "type"));   // "Type" : "type"
      if (!singular)  result+="r";                          // "s"
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Subprogram" : "subprogram"));   // "Subprogram" : "subprogram"
      if (!singular)  result+="er";                                     // "s"
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "typebegrænsninger";        // "Type Constraints"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Relation";        // " Relation"
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Indlæser...";          // "Loading..."
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globalt Namespace";    // "Global Namespace"
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Søger...";    // "Searching..."
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Ingen fund";      // "No Matches"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Afhængighedsgraf for "+name;  // "Directory dependency graph for "
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"File i "+name;     // "File in "
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Inkluderer file i "+name;    // "Includes file in "
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
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    {
      static const char *days[]   = { "Man","Tir","Ons","Tor","Fre","Lør","Søn" }; // { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Maj","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };    //  { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
      QCString sdate;
      sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }


/*---------- For internal use: ----------------------------------------*/
  protected:
	/*! For easy flexible-noun implementation.
	 *  \internal
	 */
    QCString createNoun(bool first_capital, bool singular,
			const char* base, const char* plurSuffix)
    {
      QCString result(base);
      if (first_capital) result.at(0) = toupper(result.at(0));
      if (!singular)  result+=plurSuffix;
      return result;
    }
};

#endif
