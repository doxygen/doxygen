/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
 * The translation into German was provided by
 *   Jens Breitenstein (Jens.Breitenstein@tlc.de)
 */

// Updates:
// --------
//   RK (who are you?)
//    - Update for "new since 1.1.3" version
//
//   2001/03/23 Jens Seidel (jensseidel@users.sourceforge.net)
//    - fixed typos
//    - changed trPageDocumentation() "Seitenbeschreibung" to
//      "Zusätzliche Informationen"
//    - removed old trGeneratedFrom()
//    - changed "/*!" to "/*" (documentation is inherited from translator.h
//      (INHERIT_DOCS = YES), there's no need to make changes twice)
//    - Update for "new since 1.2.4" version
//
//   2001/04/17 Jens Seidel (jensseidel@users.sourceforge.net)
//    - fixed typos ("Vererbunsbeziehung", "gesch&uumltzter")
//    - use umlauts instead of html code ("&auml;",...)
//      this makes it easier to read and fixes three problems (two in 
//      \code segment)
//
// Todo: 
//   - translation of all Config_getBool("OPTIMIZE_OUTPUT_FOR_C")
//     strings (see translator.h)
//   - translation of "compound"
//   - see FIXME
//   - was ist richtig: "Liste aller dokumentierter Elemente" oder
//                      "Liste aller dokumentierten Elemente" (aktuell)
//     (nach "aller" suchen)
//     "Mithilfe" oder "Mit Hilfe"

#ifndef TRANSLATOR_DE_H
#define TRANSLATOR_DE_H

#include "translator_adapter.h"

class TranslatorGerman : public TranslatorAdapter_1_2_6
{
  public:
    QCString idLanguage()
    { return "german"; }

    QCString latexBabelPackage()
      // returns the name of the package that is included by LaTeX
    { return "german"; }

    QCString trRelatedFunctions()
      // used in the compound documentation before a list of related functions.
    { return "Verwandte Funktionen"; }

    QCString trRelatedSubscript()
      // subscript for the related functions.
    { return "(Es handelt sich hierbei nicht um Elementfunktionen.)"; }

    QCString trDetailedDescription()
      // header that is put before the detailed description of files, classes and namespaces.
    { return "Ausführliche Beschreibung"; }

    QCString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "Dokumentation der benutzerdefinierten Datentypen"; }
    
    QCString trMemberEnumerationDocumentation()
      // header that is put before the list of enumerations.
    { return "Dokumentation der Aufzählungstypen"; }

    QCString trMemberFunctionDocumentation()
      // header that is put before the list of member functions.
    { return "Dokumentation der Elementfunktionen"; }

    QCString trMemberDataDocumentation()
      // header that is put before the list of member attributes.
    { return "Dokumentation der Datenelemente"; }

    QCString trMore()
      // this is the text of a link put after brief descriptions.
    { return "Mehr..."; }

    QCString trListOfAllMembers()
      // put in the class documentation
    { return "Aufstellung aller Elemente"; }

    QCString trMemberList()
      // used as the title of the "list of all members" page of a class
    { return "Elementverzeichnis"; }

    QCString trThisIsTheListOfAllMembers()
      // this is the first part of a sentence that is followed by a class name
    { return "Vollständige Aufstellung aller Elemente für "; }

    QCString trIncludingInheritedMembers()
      // this is the remainder of the sentence after the class name
    { return " einschließlich aller geerbten Elemente."; }

    QCString trGeneratedAutomatically(const char *s)
      // this is put at the author sections at the bottom of man pages.
      // parameter s is name of the project name.
    { QCString result="Automatisch erzeugt von Doxygen";
      if (s) result+=(QCString)" für "+s;
      result+=" aus dem Quellcode.";
      return result;
    }

    QCString trEnumName()
      // put after an enum name in the list of all members
    { return "enum Bezeichner"; }

    QCString trEnumValue()
      // put after an enum value in the list of all members
    { return "enum Wert"; }

    QCString trDefinedIn()
      // put after an undocumented member in the list of all members
    { return "Definiert in"; }

    QCString trVerbatimText(const char *f)
      // put as in introduction in the verbatim header file of a class.
      // parameter f is the name of the include file.
    { return (QCString)"Dieses ist der unveränderte Text aus der "
             "Include-Datei "+f+"."; }

    // quick reference sections

    QCString trModules()
      // This is put above each page as a link to the list of all groups of
      // compounds or files (see the \group command).
    { return "Module"; }

    QCString trClassHierarchy()
      // This is put above each page as a link to the class hierarchy
    { return "Klassenhierarchie"; }

    QCString trCompoundList()
      // This is put above each page as a link to the list of annotated classes
    { return "Übersicht"; }

    QCString trFileList()
      // This is put above each page as a link to the list of documented files
    { return "Auflistung der Dateien"; }

    QCString trHeaderFiles()
      // This is put above each page as a link to the list of all verbatim headers
    { return "Auflistung der Header-Dateien"; }

    QCString trCompoundMembers()
      // This is put above each page as a link to all members of compounds.
    { return "Elementübersicht"; }

    QCString trFileMembers()
      // This is put above each page as a link to all members of files.
    { return "Datei-Elemente"; }

    QCString trRelatedPages()
      // This is put above each page as a link to all related pages.
    { return "Zusätzliche Informationen"; }

    QCString trExamples()
      // This is put above each page as a link to all examples.
    { return "Beispiele"; }

    QCString trSearch()
      // This is put above each page as a link to the search engine.
    { return "Suchen"; }

    QCString trClassHierarchyDescription()
      // This is an introduction to the class hierarchy.
    { return "Die Liste der Ableitungen ist -mit Einschränkungen- "
             "alphabetisch sortiert:";
    }

    QCString trFileListDescription(bool extractAll)
      // This is an introduction to the list with all files.
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateien mit einer Kurzbeschreibung:";
      return result;
    }

    QCString trCompoundListDescription()
      // This is an introduction to the annotated compound list
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	return "Hier folgt die Aufzählung aller Datenstrukturen "
	       "mit einer Kurzbeschreibung:";
      else
	return "Hier folgt die Aufzählung aller Klassen, Strukturen "
	       "und Varianten mit einer Kurzbeschreibung:"; // FIXME: "interfaces" = ??
    }

    QCString trCompoundMembersDescription(bool extractAll)
      // This is an introduction to the page with all class members
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        result+="Strukturen und Varianten mit Verweisen auf ";
      else
        result+="Klassenelemente mit Verweisen auf ";
      if (extractAll) {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="die struct/union Dokumentation zu jedem Element:";
        else 
          result+="die Klassendokumentation zu jedem Element:";
      } else { 
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="die zugehörigen Elemente:";
        else
          result+="die zugehörigen Klassen:";
      }
      return result;
    }

    QCString trFileMembersDescription(bool extractAll)
      // This is an introduction to the page with all file members
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateielemente mit Verweisen auf ";
      if (extractAll) result+="die Dateidokumentation zu jedem Element:";
      else result+="die zugehörigen Dateien:";
      return result;
    }

    QCString trHeaderFilesDescription()
      // This is an introduction to the page with the list of all header files
    { return "Hier folgen die Headerdateien, welche die API definieren:"; }

    QCString trExamplesDescription()
      // This is an introduction to the page with the list of all examples
    { return "Hier folgt eine Liste mit allen Beispielen:"; }

    QCString trRelatedPagesDescription()
      // This is an introduction to the page with the list of related pages
    { return "Hier folgt eine Liste mit zusammengehörigen Themengebieten:"; }

    QCString trModulesDescription()
      // This is an introduction to the page with the list of class/file groups
    { return "Hier folgt die Aufzählung aller Module:"; }

    QCString trNoDescriptionAvailable()
      // This sentences is used in the annotated class/file lists if no brief
      // description is given.
    { return "Keine Beschreibung verfügbar"; }

    // index titles (the project name is prepended for these)
   
    QCString trDocumentation()
      // This is used in HTML as the title of index.html.
    { return "Dokumentation"; }

    QCString trModuleIndex()
      // This is used in LaTeX as the title of the chapter with the
      // index of all groups.
    { return "Modul-Verzeichnis"; }

    QCString trHierarchicalIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // class hierarchy.
    { return "Hierarchie-Verzeichnis"; }

    QCString trCompoundIndex()
      // This is used in LaTeX as the title of the chapter with the
      // annotated compound index
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	return "Datenstruktur-Verzeichnis"; 
      else
	return "Datenstruktur-Verzeichnis"; // FIXME: war compound
    }

    QCString trFileIndex()
      // This is used in LaTeX as the title of the chapter with the
      // list of all files.
    { return "Datei-Verzeichnis"; }

    QCString trModuleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all groups.
    { return "Modul-Dokumentation"; }

    QCString trClassDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all classes, structs and unions.
    { return "Klassen-Dokumentation"; }

    QCString trFileDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all files.
    { return "Datei-Dokumentation"; }

    QCString trExampleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all examples.
    { return "Dokumentation der Beispiele"; }

    QCString trPageDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all related pages.
    { return "Zusätzliche Informationen"; }

    QCString trReferenceManual()
      // This is used in LaTeX as the title of the document
    { return "Nachschlagewerk"; }

    QCString trDefines()
      // This is used in the documentation of a file as a header before the
      // list of defines
    { return "Makrodefinitionen"; }

    QCString trFuncProtos()
      // This is used in the documentation of a file as a header before the
      // list of function prototypes
    { return "Funktionsprototypen"; }

    QCString trTypedefs()
      // This is used in the documentation of a file as a header before the
      // list of typedefs
    { return "Typendefinitionen"; }

    QCString trEnumerations()
      // This is used in the documentation of a file as a header before the
      // list of enumerations
    { return "Aufzählungen"; }

    QCString trFunctions()
      // This is used in the documentation of a file as a header before the
      // list of (global) functions
    { return "Funktionen"; }

    QCString trVariables()
      // This is used in the documentation of a file as a header before the
      // list of (global) variables
    { return "Variablen"; }

    QCString trEnumerationValues()
      // This is used in the documentation of a file as a header before the
      // list of (global) variables
    { return "Aufzählungswerte"; }

    QCString trAuthor()
      // This is used in man pages as the author section.
    { return "Autor"; }

    QCString trDefineDocumentation()
      // This is used in the documentation of a file before the list of
      // documentation blocks for defines
    { return "Makro-Dokumentation"; }

    QCString trFunctionPrototypeDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for function prototypes
    { return "Funktionsprototypen Dokumentation"; }

    QCString trTypedefDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for typedefs
    { return "Dokumentation der benutzerdefinerten Typen"; }

    QCString trEnumerationTypeDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for enumeration types
    { return "Dokumentation der Aufzählungstypen"; }

    QCString trEnumerationValueDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for enumeration values
    { return "Dokumentation des Wertebereiches der Aufzählungstypen"; }

    QCString trFunctionDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for functions
    { return "Dokumentation der Funktionen"; }

    QCString trVariableDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for variables
    { return "Variablen-Dokumentation"; }

    QCString trCompounds()
      // This is used in the documentation of a file/namespace/group before
      // the list of links to documented compounds
    { return "Übersicht"; }

    QCString trFiles()
      // This is used in the documentation of a group before the list of
      // links to documented files
    { return "Dateien"; }

    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Erzeugt am "+date;
      if (projName) result+=(QCString)" für "+projName;
      result+=(QCString)" von";
      return result;
    }

    QCString trWrittenBy()
    {
      return "geschrieben von";
    }

    QCString trClassDiagram(const char *clName)
      // this text is put before a class diagram
    {
      return (QCString)"Klassendiagramm für "+clName;
    }

    QCString trForInternalUseOnly()
      // this text is generated when the \internal command is used.
    { return "Nur für den internen Gebrauch."; }

    QCString trReimplementedForInternalReasons()
      // this text is generated when the \reimp command is used.
    { return "Aus internen Gründen neu implementiert. "
             "Das API wird davon nicht berührt."; }

    QCString trWarning()
      // this text is generated when the \warning command is used.
    { return "Warnung"; }

    QCString trBugsAndLimitations()
      // this text is generated when the \bug command is used.
    { return "Fehler und Einschränkungen"; }

    QCString trVersion()
      // this text is generated when the \version command is used.
    { return "Version"; }

    QCString trDate()
      // this text is generated when the \date command is used.
    { return "Datum"; }

    QCString trAuthors()
      // this text is generated when the \author command is used.
    { return "Autor(en)"; }

    QCString trReturns()
      // this text is generated when the \return command is used.
    { return "Rückgabe"; }

    QCString trSeeAlso()
      // this text is generated when the \sa command is used.
    { return "Siehe auch"; }

    QCString trParameters()
      // this text is generated when the \param command is used.
    { return "Parameter"; }

    QCString trExceptions()
      // this text is generated when the \exception command is used.
    { return "Ausnahmebehandlung"; }

    QCString trGeneratedBy()
      // this text is used in the title page of a LaTeX document.
    { return "Erzeugt von"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaceList()
      // used as the title of page containing all the index of all namespaces.
    { return "Liste aller Namensbereiche"; }

    QCString trNamespaceListDescription(bool extractAll)
      // used as an introduction to the namespace list
    {
      QCString result="Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Namensbereiche mit Kurzbeschreibung:";
      return result;
    }

    QCString trFriends()
      // used in the class documentation as a header before the list of all
      // friends of a class
    { return "Freundbeziehungen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation()
      // used in the class documentation as a header before the list of all
      // related classes
    { return "Freundbeziehungen und Funktionsdokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool /*isTemplate*/)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Klassen"; break;
        case ClassDef::Struct: result+=" Strukturen"; break;
        case ClassDef::Union:  result+=" Varianten"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
      }
      result+="referenz";
      return result;
    }

    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" Dateireferenz";
      return result;
    }

    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Namensbereichsreferenz";
      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers()
    { return "Öffentliche Datenelemente"; }

    QCString trPublicSlots()
    { return "Öffentliche Slots"; }

    QCString trSignals()
    { return "Signale"; }

    QCString trStaticPublicMembers()
    { return "Öffentliche, statische Datenelemente"; }

    QCString trProtectedMembers()
    { return "Geschützte Datenelemente"; }

    QCString trProtectedSlots()
    { return "Geschützte Slots"; }

    QCString trStaticProtectedMembers()
    { return "Geschützte, statische Datenelemente"; }

    QCString trPrivateMembers()
    { return "Private Datenelemente"; }

    QCString trPrivateSlots()
    { return "Private Slots"; }

    QCString trStaticPrivateMembers()
    { return "Private, statische Datenelemente"; }

    // end of member sections

    QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+=" und ";
        }
      }
      return result;
    }

    QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Abgeleitet von "+trWriteList(numEntries)+".";
    }

    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Basisklasse für "+trWriteList(numEntries)+".";
    }

    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Erneute Implementation von "+trWriteList(numEntries)+".";
    }

    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Erneute Implementation in "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Elemente eines Namensbereiches"; }

    QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Namensbereichselemente mit Verweisen auf ";
      if (extractAll)
        result+="die Namensbereichsdokumentation für jedes Element:";
      else
        result+="die zugehörigen Dateien:";
      return result;
    }

    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Verzeichnis der Namensbereiche"; }

    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Dokumentation der Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*  This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*  This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Die Dokumentation für diese";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Variante"; break;
        case ClassDef::Interface:  result+="s Interface"; break;
        case ClassDef::Exception:  result+=" Ausnahme"; break;
      }
      result+=" wurde erzeugt aufgrund der Datei";
      if (single) result+=":"; else result+="en:";
      return result;
    }

    /* This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Alphabetische Liste"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /* This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Rückgabewerte"; }

    /* This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hauptseite"; }

    /*  This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "S."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "Quellen";
    }

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definiert in Zeile @0 der Datei @1.";
    }

    virtual QCString trDefinedInSourceFile()
    {
      return "Definiert in Datei @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Veraltet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /* this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Zusammengehörigkeiten von "+clName+":";
    }

    /* this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include-Abhängigkeitsdiagramm für "+fName+":";
    }

    /* header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Beschreibung der Konstruktoren und Destruktoren"; 
    }

    /* Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "gehe zum Quellcode dieser Datei";
    }

    /* Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "gehe zur Dokumentation dieser Datei";
    }

    /* Text for the \pre command */
    virtual QCString trPrecondition()
    {
      return "Vorbedingung";
    }

    /* Text for the \post command */
    virtual QCString trPostcondition()
    {
      return "Nachbedingung";
    }

    /* Text for the \invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }

    /* Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Initialisierung:";
    }

    /* Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "code";
    }

    virtual QCString trGraphicalHierarchy()
    {
      return "Grafische Darstellung der Klassenhierarchie";
    }

    virtual QCString trGotoGraphicalHierarchy()
    {
      return "gehe zur grafischen Darstellung der Klassenhierarchie";
    }

    virtual QCString trGotoTextualHierarchy()
    {
      return "gehe zur textbasierten Darstellung der Klassenhierarchie";
    }

    virtual QCString trPageIndex()
    {
      return "Seitenindex";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    //RK: had to change here because of the new command \remark
    virtual QCString trNote()
    {
      return "Zu beachten";
    }

    virtual QCString trPublicTypes()
    {
      return "Öffentliche Typen";
    }

    virtual QCString trPublicAttribs()
    {
      return "Öffentliche Attribute";
    }

    virtual QCString trStaticPublicAttribs()
    {
      return "Statische öffentliche Attribute";
    }

    virtual QCString trProtectedTypes()
    {
      return "Geschützte Typen";
    }

    virtual QCString trProtectedAttribs()
    {
      return "Geschützte Attribute";
    }

    virtual QCString trStaticProtectedAttribs()
    {
      return "Statische geschützte Attribute";
    }

    virtual QCString trPrivateTypes()
    {
      return "Private Typen";
    }

    virtual QCString trPrivateAttribs()
    {
      return "Private Attribute";
    }

    virtual QCString trStaticPrivateAttribs()
    {
      return "Statische private Attribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /* Used as a marker that is put before a \todo item */
    virtual QCString trTodo()
    {
      return "Noch zu erledigen";
    }

    /* Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Liste der zu erledigenden Dinge";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Wird benutzt von";
    }

    virtual QCString trRemarks()
    {
      return "Bemerkungen";
    }

    virtual QCString trAttention()
    {
      return "Achtung";
    }

    virtual QCString trInclByDepGraph()
    {
      return "Dieser Graph zeigt, welche Datei direkt oder "
             "indirekt diese Datei enthält:";
    }

    virtual QCString trSince()
    {
      return "Seit";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /* title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Erklärung des Graphen";
    }

    /* page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return 
        "Diese Seite erklärt die Interpretation der von doxygen "
        "erzeugten Graphen.<p>\n"
        "Beispiel:\n"
        "\\code\n"
        "/*! Wegen Verkürzung unsichtbare Klasse */\n"
        "class Invisible { };\n\n"
        "/*! Klasse verkürzt dargestellt, Vererbungsbeziehung ist versteckt */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Nicht mit doxygen-Kommentaren dokumentierte Klasse */\n"
        "class Undocumented { };\n\n"
        "/*! Mithilfe öffentlicher Vererbung vererbte Klasse */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Mithilfe geschützter Vererbung vererbte Klasse */\n"
        "class ProtectedBase { };\n\n"
        "/*! Mithilfe privater Vererbung vererbte Klasse */\n"
        "class PrivateBase { };\n\n"
        "/*! Von der Klasse Inherited benutzte Klasse */\n"
        "class Used { };\n\n"
        "/*! Superklasse, die von mehreren anderen Klassen erbt */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Setzen des Tags \\c MAX_DOT_GRAPH_HEIGHT in der Konfigurationsdatei "
        "auf 200 liefert den folgenden Graphen:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Die Rechtecke in obigem Graphen bedeuten:\n"
        "<ul>\n"
        "<li>Ein schwarz gefülltes Rechteck stellt die Struktur oder "
        "Klasse dar, für die der Graph erzeugt wurde.\n"
        "<li>Ein Rechteck mit schwarzem Rahmen kennzeichnet eine dokumentierte "
        " Struktur oder Klasse.\n"
        "<li>Ein Rechteck mit grauem Rahmen kennzeichnet eine undokumentierte "
        " Struktur oder Klasse.\n"
        "<li>Ein Rechteck mit rotem Rahmen kennzeichnet eine dokumentierte "
        " Struktur oder Klasse, für die nicht alle Vererbungs-/"
        "Enthaltenseinsbeziehungen dargestellt werden. Ein Graph wird gekürzt, "
        "wenn er nicht in die angegebenen Schranken passt."  
        "</ul>\n"
        "Die Pfeile bedeuten:\n"
        "<ul>\n"
        "<li>Ein dunkelblauer Pfeil stellt eine öffentliche Vererbungsbeziehung "
        "zwischen zwei Klassen dar.\n"
        "<li>Ein dunkelgrüner Pfeil stellt geschützte Vererbung dar.\n"
        "<li>Ein dunkelroter Pfeil stellt private Vererbung dar.\n"
        "<li>Ein gestrichelter violetter Pfeil bedeutet, dass eine Klasse in einer "
        "anderen enthalten ist oder von einer anderen benutzt wird. Am Pfeil "
        "stehen die Variable(n), mit deren Hilfe auf die Struktur oder Klasse "
        "an der Pfeilspitze zugegriffen werden kann. \n"
        "</ul>\n";
    }

    /* text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "Legende";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /* Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Test";
    }

    /* Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Test-Liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /* Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Methoden";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /* Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Eigenschaften";
    }

    /* Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Dokumentation der Eigenschaften";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /* Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Schnittstellen";
    }

    /* Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datenstrukturen";
      }
      else
      {
        return "Klassen";
      }
    }

    /* Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Package "+name;
    }

    /* Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Package Liste";
    }

    /* The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Hier folgen die packages mit einer Kurzbeschreibung (wenn verfügbar):";
    }

    /* The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Packages";
    }

    /* Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Package Dokumentation";
    }

    /* Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Wert:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /* Used as a marker that is put before a \bug item */
    virtual QCString trBug()
    {
      return "Bug";
    }

    /* Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Bug Liste";
    }
};

#endif

