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
 * All output generated with Doxygen is not covered by this license.
 *
 * The translation into German was provided by
 *   Jens Breitenstein (Jens.Breitenstein@tlc.de)
 */

#ifndef TRANSLATOR_DE_H
#define TRANSLATOR_DE_H

#include "translator.h"

class TranslatorGerman : public Translator
{
  public:
    //--------------------------------------------------------------------
    // NOTICE:
    // the following functions are now obsolete: these are no longer used and
    // will disappear in future versions. You do not have to translate them!
    QCString trInherits()
    { return "Abgeleitet von "; }
    QCString trAnd()
    { return "und"; }
    QCString trInheritedBy()
    { return "Basisklasse f&uuml;r"; }
    QCString trReference()
    { return "Verweis"; }
    QCString trReimplementedFrom()
    { return "Implementiert von"; }
    QCString trReimplementedIn()
    { return "erneute Implementation in"; }
    QCString trIncludeFile()
    { return "Include-Datei"; }
    // end of obsolete functions
    //--------------------------------------------------------------------

    QCString latexBabelPackage()
      // returns the name of the package that is included by LaTeX
    { return "deutsch"; }

    QCString trRelatedFunctions()
      // used in the compound documentation before a list of related functions.
    { return "Verwandte Funktionen"; }

    QCString trRelatedSubscript()
      // subscript for the related functions.
    { return "(Es handelt sich hierbei nicht um Elementfunktionen.)"; }

    QCString trDetailedDescription()
      // header that is put before the detailed description of files, classes and namespaces.
    { return "Ausf&uuml;hrliche Beschreibung"; }

    QCString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "Dokumentation der benutzerdefinierten Datentypen"; }
    
    QCString trMemberEnumerationDocumentation()
      // header that is put before the list of enumerations.
    { return "Dokumentation der Aufz&auml;hlungstypen"; }

    QCString trMemberFunctionDocumentation()
      // header that is put before the list of member functions.
    { return "Dokumentation der Elementfunktionen"; }

    QCString trMemberDataDocumentation()
      // header that is put before the list of member attributes.
    { return "Dokumentation der Datenelemente"; }

    QCString trGeneratedFrom(const char *s,bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Die Dokumentation f&uuml;r diese"+s+
                     " wurde aus ";
      if (single)
         result+="folgender Datei erzeugt:";
      else
         result+="folgenden Dateien erzeugt:";

      return result;
    }

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
    { return "Vollst&auml;ndige Aufstellung aller Elemente f&uuml;r "; }

    QCString trIncludingInheritedMembers()
      // this is the remainder of the sentence after the class name
    { return " einschlie&szlig;lich aller geerbten Elemente."; }

    QCString trGeneratedAutomatically(const char *s)
      // this is put at the author sections at the bottom of man pages.
      // parameter s is name of the project name.
    { QCString result="Automatisch erzeugt von Doxygen";
      if (s) result+=(QCString)" f&uuml;r "+s;
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
    { return (QCString)"Dieses ist der unver&auml;nderte Text aus der "
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
    { return "&Uuml;bersicht"; }
    QCString trFileList()
      // This is put above each page as a link to the list of documented files
    { return "Auflistung der Dateien"; }
    QCString trHeaderFiles()
      // This is put above each page as a link to the list of all verbatim headers
    { return "Auflistung der Header-Dateien"; }
    QCString trCompoundMembers()
      // This is put above each page as a link to all members of compounds.
    { return "Element&uuml;bersicht"; }
    QCString trFileMembers()
      // This is put above each page as a link to all members of files.
    { return "Datei-Elemente"; }
    QCString trRelatedPages()
      // This is put above each page as a link to all related pages.
    { return "Zus&auml;tzliche Informationen"; }
    QCString trExamples()
      // This is put above each page as a link to all examples.
    { return "Beispiele"; }
    QCString trSearch()
      // This is put above each page as a link to the search engine.
    { return "Suchen"; }

    QCString trClassHierarchyDescription()
      // This is an introduction to the class hierarchy.
    { return "Die Liste der Ableitungen ist -mit Einschr&auml;nkungen- "
             "alphabetisch sortiert:";
    }
    QCString trFileListDescription(bool extractAll)
      // This is an introduction to the list with all files.
    {
      QCString result="Hier folgt die Aufz&auml;hlung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateien mit einer Kurzbeschreibung:";
      return result;
    }
    QCString trCompoundListDescription()
      // This is an introduction to the annotated compound list
    { return "Hier folgt die Aufz&auml;hlung aller Klassen, Strukturen "
             "und Varianten mit einer Kurzbeschreibung:";
    }
    QCString trCompoundMembersDescription(bool extractAll)
      // This is an introduction to the page with all class members
    {
      QCString result="Hier folgt die Aufz&auml;hlung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Klassenelemente mit Verweisen auf ";
      if (extractAll) result+="die Klassendokumentation zu jedem Element:";
      else result+="die zugeh&ouml;rigen Klassen:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
      // This is an introduction to the page with all file members
    {
      QCString result="Hier folgt die Aufz&auml;hlung aller ";
      if (!extractAll) result+="dokumentierter ";
      result+="Dateielemente mit Verweisen auf ";
      if (extractAll) result+="die Dateidokumentation zu jedem Element:";
      else result+="die zugeh&ouml;rigen Dateien:";
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
    { return "Hier folgt eine Liste mit zusammengeh&ouml;rigen Themengebieten:"; }
    QCString trModulesDescription()
      // This is an introduction to the page with the list of class/file groups
    { return "Hier folgt die Aufz&auml;hlung aller Module:"; }
    QCString trNoDescriptionAvailable()
      // This sentences is used in the annotated class/file lists if no brief
      // description is given.
    { return "Keine Beschreibung verf&uuml;gbar"; }

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
    { return "Compound-Verzeichnis"; }
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
    { return "Seitenbeschreibung"; }
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
    { return "Aufz&auml;hlungen"; }
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
    { return "Aufz&auml;hlungswerte"; }

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
    { return "Dokumentation der Aufz&auml;hlungstypen"; }
    QCString trEnumerationValueDocumentation()
      // This is used in the documentation of a file/namespace before the list
      // of documentation blocks for enumeration values
    { return "Dokumentation des Wertebereiches der Aufz&auml;hlungstypen"; }
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
    { return "&Uuml;bersicht"; }
    QCString trFiles()
      // This is used in the documentation of a group before the list of
      // links to documented files
    { return "Dateien"; }

    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Erzeugt am "+date;
      if (projName) result+=(QCString)" f&uuml;r "+projName;
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
      return (QCString)"Klassendiagramm f&uuml;r "+clName;
    }

    QCString trForInternalUseOnly()
      // this text is generated when the \internal command is used.
    { return "Nur f&uuml;r den internen Gebrauch."; }
    QCString trReimplementedForInternalReasons()
      // this text is generated when the \reimp command is used.
    { return "Aufgrund interner Gr&uuml;nde neu implementiert; "
             "Das API wird davon nicht ber&uuml;hrt.";
    }
    QCString trWarning()
      // this text is generated when the \warning command is used.
    { return "Warnung"; }
    QCString trBugsAndLimitations()
      // this text is generated when the \bug command is used.
    { return "Fehler und Einschr&auml;nkungen"; }
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
    { return "R&uuml;ckgabe"; }
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

    // new since 0.49-990307

    QCString trNamespaceList()
      // used as the title of page containing all the index of all namespaces.
    { return ""; }
    QCString trNamespaceListDescription(bool extractAll)
      // used as an introduction to the namespace list
    {
      QCString result="Lister aller ";
      if (!extractAll) result+="dokumentierter ";
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
                                    ClassDef::CompoundType compType)
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
    { return "&Ouml;ffentliche Datenelemente"; }
    QCString trPublicSlots()
    { return "&Ouml;ffentliche Slots"; }
    QCString trSignals()
    { return "Signale"; }
    QCString trStaticPublicMembers()
    { return "&Ouml;ffentliche, statische Datenelemente"; }
    QCString trProtectedMembers()
    { return "Gesch&uuml;tzte Datenelemente"; }
    QCString trProtectedSlots()
    { return "Gesch&uuml;tzte Slots"; }
    QCString trStaticProtectedMembers()
    { return "Gesch&uuml;tzte, statische Datenelemente"; }
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
            result+=", und ";
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
      return "Basisklasse f&uuml;r "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Implementiert von "+trWriteList(numEntries)+".";
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
      QCString result="Hier folgt die Aufz&auml;hlung aller ";
      if (!extractAll) result+="dokumentierter ";
      result+="Namensbereichselemente mit Verweisen auf ";
      if (extractAll)
        result+="die Namensbereichsdokumentation f&uuml;r jedes Element:";
      else
        result+="die zugeh&ouml;rigen Dateien:";
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

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namensbereiche"; }

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

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Alphabetische Liste"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "R&uuml;ckgabewerte"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hauptseite"; }

    /*! This is used in references to page that are put in the LaTeX 
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

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Zusammengeh&ouml;rigkeiten von "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include-Abhängikeitsdiagramm f&uuml;r "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Beschreibung der Konstruktoren und Destruktoren"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "gehe zum Quellcode dieser Datei";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "gehe zur Dokumentation dieser Datei";
    }
    /*! Text for the \pre command */
    virtual QCString trPrecondition()
    {
      return "Vorbedingung";
    }
    /*! Text for the \post command */
    virtual QCString trPostcondition()
    {
      return "Nachbedingung";
    }
    /*! Text for the \invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Initialisierung:";
    }
    /*! Text used the source code in the file index */
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

    virtual QCString trNote()
    {
      return "Bemerkung";
    }

    virtual QCString trPublicTypes()
    {
      return "&Ouml;ffentliche Typen";
    }
    virtual QCString trPublicAttribs()
    {
      return "&Ouml;ffentliche Attribute";
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statische &ouml;ffentliche Attribute";
    }
    virtual QCString trProtectedTypes()
    {
      return "Gesch&uuml;tzte Typen";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Gesch&uuml;tzte Attribute";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statische gesch&uuml;tzte Attribute";
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
    // new since 1.1.1
    //////////////////////////////////////////////////////////////////////////

};


#endif

