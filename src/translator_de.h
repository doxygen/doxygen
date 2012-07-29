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
 * The translation into German was provided by
 *   Jens Seidel (jensseidel@users.sf.net)
 * based on work from
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
//    - changed "/*!" to "/*" (documentation is inherited from translator_en.h
//      (INHERIT_DOCS = YES), there's no need to make changes twice)
//    - Update for "new since 1.2.4" version
//
//   2001/04/17 Jens Seidel (jensseidel@users.sourceforge.net)
//    - fixed typos ("Vererbunsbeziehung", "gesch&uumltzter")
//    - use umlauts instead of html code ("&auml;",...)
//      this makes it easier to read and fixes three problems (two in 
//      \code segment)
//
//   2001/04/23 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Update for "new since 1.2.6-20010422" version
//
//   2001/05/06 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Update for "new since 1.2.7" version
//    - Removed implementation of latexBabelPackage(), trVerbatimText(),
//      trFiles(), added latexLanguageSupportCommand().
//
//   2001/05/25 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Update for "new since 1.2.7-20010524" version:
//      removed trAuthors(), trAuthor(), added trAuthor(bool, bool)
//
//   2001/07/24 Jens Seidel (jensseidel@users.sourceforge.net)
//    - trClassDocumentation() updated as in the English translator.
//
//   2001/11/30 Oliver Brandt (o.brandt@tu-bs.de) and
//              Jens Seidel (jensseidel@users.sourceforge.net)
//    - trReferences() implemented.
//    - trCompoundReference(), trLegendDocs() updated
//    - Removed some TODO's
//
//   2001/02/13 Oliver Brandt (o.brandt@tu-bs.de)
//    - Updated for "new since 1.2.13" version
//    - Removed some TODO's
//
//   2002/07/08 Oliver Brandt (o.brandt@tu-bs.de)
//    - Updated for "new since 1.2.16" version
//
//   2002/11/25 Jens Seidel (jensseidel@users.sourceforge.net)
//    - sync with english version 1.3
//    - TranslatorGerman doesn't inherit from TranslatorEnglish anymore,
//      so I changed "/* " back to "/*! " as in the English file
//    - use ngerman instead of german package in LaTeX
//    - changed "Datenelemente" to "Methoden" in
//      tr{,Static}{Public,Protected,Private}Members
//
//   2003/04/28 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.3" version
//    - translated Java package to Paket
//
//   2003/09/11 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.3.1" version
//  
//   2003/09/24 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated a few strings which changed in CVS revision 1.22
//      ("compound" vs. "class")
//    
//   2004/08/01 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.3.8" version
//    
//   2004/09/19 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.3.9" version
//    
//   2004/09/25 Jens Seidel (jensseidel@users.sourceforge.net)
//    - changed "Typendefinitionen" to "Typdefinitionen" in
//      trFileMembersDescription() and trTypedefs()
//    - added a dash after (file, ...) names and in front of description
//    - changed "Eigenschaften" to "Propertys" (yeah, not ..."ies")
//    
//   2005/03/20 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.4.1" version
//    
//   2005/04/09 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Changed Todo list translation as suggested in
//      http://bugzilla.gnome.org/show_bug.cgi?id=172818
//    
//   2005/05/09 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.4.3" version (removed unused methods)
//
//   2006/06/12 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.4.6" version
//
//   2008/02/04 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.5.4" version
//
//   2009/01/09 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Resynced the trLegendDocs() method
//
//   2010/03/17 Jens Seidel (jensseidel@users.sourceforge.net)
//    - Updated for "new since 1.6.0" version
//    - Resynced the trLegendDocs() method (added closing HTML tags)
//    - Changed file encoding to utf-8, adapted idLanguageCharset()
//    - New German orthography (ngerman) is now default in LaTeX
//
//   2012/04/11 Peter Grotrian
//    - Updated for "new since 1.8.0" version
//    - Some small corrections
//
//   Todo:
//    - see FIXME

#ifndef TRANSLATOR_DE_H
#define TRANSLATOR_DE_H

class TranslatorGerman : public TranslatorAdapter_1_8_2
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
    { return "german"; }

    /*! Used to get the LaTeX command(s) for the language support. This method
     *  was designed for languages which do wish to use a babel package.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[ngerman]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Verwandte Funktionen"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Es handelt sich hierbei nicht um Elementfunktionen.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Ausführliche Beschreibung"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentation der benutzerdefinierten Datentypen"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentation der Aufzählungstypen"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentation der Elementfunktionen"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { /* No difference if "OPTIMIZE_OUTPUT_FOR_C" is set! */
      return "Dokumentation der Datenelemente";
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Mehr ..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Aufstellung aller Elemente"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Elementverzeichnis"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Vollständige Aufstellung aller Elemente für "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return " einschließlich aller geerbten Elemente."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatisch erzeugt von Doxygen";
      if (s) result+=(QCString)" für "+s;
      result+=" aus dem Quellcode.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum-Bezeichner"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum-Wert"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "Definiert in"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Module"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klassenhierarchie"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datenstrukturen";
      }
      else
      {
        return "Auflistung der Klassen";
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Auflistung der Dateien"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datenstruktur-Elemente";
      }
      else
      {
        return "Klassen-Elemente";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    /* No difference if "OPTIMIZE_OUTPUT_FOR_C" is set! */
    { return "Datei-Elemente"; }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Zusätzliche Informationen"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Beispiele"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Suchen"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Die Liste der Ableitungen ist -mit Einschränkungen- "
             "alphabetisch sortiert:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateien mit einer Kurzbeschreibung:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
	return "Hier folgt die Aufzählung aller Datenstrukturen "
	       "mit einer Kurzbeschreibung:";
      }
      else
      {
	return "Hier folgt die Aufzählung aller Klassen, Strukturen, "
	       "Varianten und Schnittstellen mit einer Kurzbeschreibung:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll)
      {
        result+="dokumentierten ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="Strukturen und Varianten";
      }
      else
      {
        result+="Klassenelemente";
      }
      result+=" mit Verweisen auf ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="die Dokumentation zu jedem Element:";
        }
        else
        {
          result+="die Klassendokumentation zu jedem Element:";
        }
      }
      else
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="die zugehörigen Elemente:";
        }
        else
        {
          result+="die zugehörigen Klassen:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="Funktionen, Variablen, Makros, Aufzählungen und Typdefinitionen";
      }
      else
      {
        result+="Dateielemente";
      }
      result+=" mit Verweisen auf ";
      if (extractAll) 
        result+="die Dokumentation zu jedem Element:";
      else
        result+="die zugehörigen Dateien:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Hier folgt eine Liste mit allen Beispielen:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Hier folgt eine Liste mit zusammengehörigen Themengebieten:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Hier folgt die Aufzählung aller Module:"; }

    // index titles (the project name is prepended for these) 

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modul-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarchie-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	return "Datenstruktur-Verzeichnis";
      else
        return "Klassen-Verzeichnis";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Datei-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modul-Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datenstruktur-Dokumentation"; 
      }
      else
      {
        return "Klassen-Dokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Datei-Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Dokumentation der Beispiele"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Zusätzliche Informationen"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Nachschlagewerk"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makrodefinitionen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Funktionsprototypen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typdefinitionen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Aufzählungen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funktionen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variablen"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Aufzählungswerte"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Makro-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Funktionsprototypen Dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentation der benutzerdefinierten Typen"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentation der Aufzählungstypen"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Dokumentation der Funktionen"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Variablen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
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

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Erzeugt am "+date;
      if (projName) result+=(QCString)" für "+projName;
      result+=(QCString)" von";
      return result;
    }

    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "geschrieben von";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klassendiagramm für "+clName+":";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Nur für den internen Gebrauch."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Warnung"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Rückgabe"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Siehe auch"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parameter"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Ausnahmebehandlung"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Erzeugt von"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Liste aller Namensbereiche"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Namensbereiche mit Kurzbeschreibung:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Freundbeziehungen"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Freundbeziehungen und Funktionsdokumentation"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      if (isTemplate) result+="Template-";
      switch(compType)
      {
        case ClassDef::Class:  result+="Klassen"; break;
        case ClassDef::Struct: result+="Struktur"; break;
        case ClassDef::Union:  result+="Varianten"; break;
        case ClassDef::Interface:  result+="Schnittstellen"; break;
        case ClassDef::Protocol:  result+="Protokoll"; break;
        case ClassDef::Category:  result+="Kategorie"; break; 
        case ClassDef::Exception:  result+="Ausnahmen"; break;
      }
      result+="referenz";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+="-Dateireferenz";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+="-Namensbereichsreferenz";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Öffentliche Methoden"; }

    virtual QCString trPublicSlots()
    { return "Öffentliche Slots"; }

    virtual QCString trSignals()
    { return "Signale"; }

    virtual QCString trStaticPublicMembers()
    { return "Öffentliche, statische Methoden"; }

    virtual QCString trProtectedMembers()
    { return "Geschützte Methoden"; }

    virtual QCString trProtectedSlots()
    { return "Geschützte Slots"; }

    virtual QCString trStaticProtectedMembers()
    { return "Geschützte, statische Methoden"; }

    virtual QCString trPrivateMembers()
    { return "Private Methoden"; }

    virtual QCString trPrivateSlots()
    { return "Private Slots"; }

    virtual QCString trStaticPrivateMembers()
    { return "Private, statische Methoden"; }

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
            result+=" und ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Abgeleitet von "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Basisklasse für "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Erneute Implementation von "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Erneute Implementation in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Elemente eines Namensbereiches"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Verzeichnis der Namensbereiche"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
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
    { // single is true implies a single file
      QCString result=(QCString)"Die Dokumentation für diese";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Variante"; break;
        case ClassDef::Interface:  result+=" Schnittstelle"; break;
        case ClassDef::Protocol:   result+="s Protokoll"; break; 
        case ClassDef::Category:   result+=" Kategorie"; break;
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
    { return "Rückgabewerte"; }

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
      return (QCString)"Zusammengehörigkeiten von "+clName+":";
    }

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include-Abhängigkeitsdiagramm für "+fName+":";
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

    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Vorbedingung";
    }

    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Nachbedingung";
    }

    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariante";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datenfelder";
      }
      else
      {
        return "Öffentliche Attribute";
      }
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
      return "Statische, geschützte Attribute";
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
      return "Statische, private Attribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Noch zu erledigen";
    }

    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Ausstehende Aufgaben";
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

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Erklärung des Graphen";
    }

    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
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
        "/*! Eine Template-Klasse */\n"
        "template<class T> class Templ { };\n\n"
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
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Dies liefert den folgenden Graphen:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center></p>\n"
        "<p>\n"
        "Die Rechtecke in obigem Graphen bedeuten:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Ein grau gefülltes Rechteck stellt die Struktur oder "
        "Klasse dar, für die der Graph erzeugt wurde.</li>\n"
        "<li>Ein Rechteck mit schwarzem Rahmen kennzeichnet eine dokumentierte "
        "Struktur oder Klasse.</li>\n"
        "<li>Ein Rechteck mit grauem Rahmen kennzeichnet eine undokumentierte "
        "Struktur oder Klasse.</li>\n"
        "<li>Ein Rechteck mit rotem Rahmen kennzeichnet eine dokumentierte "
        "Struktur oder Klasse, für die nicht alle Vererbungs-/"
        "Enthaltenseinsbeziehungen dargestellt werden. Ein Graph wird gekürzt, "
        "wenn er nicht in die angegebenen Schranken passt.</li>\n"
        "</ul>\n"
        "<p>\n"
        "Die Pfeile bedeuten:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Ein dunkelblauer Pfeil stellt eine öffentliche Vererbungsbeziehung "
        "zwischen zwei Klassen dar.</li>\n"
        "<li>Ein dunkelgrüner Pfeil stellt geschützte Vererbung dar.</li>\n"
        "<li>Ein dunkelroter Pfeil stellt private Vererbung dar.</li>\n"
        "<li>Ein gestrichelter violetter Pfeil bedeutet, dass eine Klasse in "
        "einer anderen enthalten ist oder von einer anderen benutzt wird. Am "
        "Pfeil stehen die Variable(n), mit deren Hilfe auf die Struktur oder "
        "Klasse an der Pfeilspitze zugegriffen werden kann.</li>\n"
        "<li>Ein gestrichelter gelber Pfeil kennzeichnet eine Verknüpfung "
        "zwischen einer Template-Instanz und der Template-Klasse von welcher "
        "es abstammt. Neben dem Pfeil sind die Template-Parameter aufgeführt.</li>\n"
        "</ul>\n";
    }

    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "Legende";
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
      return "Test-Liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Methoden";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Propertys";
    }

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Dokumentation der Propertys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        return "Datenstrukturen";
      else
        return "Klassen";
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paket "+name;
    }

    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Paketliste";
    }

    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Hier folgen die Pakete mit einer Kurzbeschreibung (wenn verfügbar):";
    }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pakete";
    }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Wert:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Fehler";
    }

    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Liste der bekannten Fehler";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file. See translator_en.h for details. */
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
      return "Index";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool, bool singular)
    {
      QCString result("Klasse");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool, bool singular)
    {
      QCString result("Datei");
      if (!singular)  result+="en";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool, bool singular)
    {
      QCString result("Namensbereich");
      if (!singular)  result+="e";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool, bool singular)
    {
      QCString result("Gruppe");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool, bool singular)
    {
      QCString result("Seite");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool, bool singular)
    {
      QCString result("Element");
      if (!singular)  result+="e";
      return result;
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Global" : "global")); // FIXME
      if (!singular)  result+="";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7-20010524
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. 
     */
    virtual QCString trAuthor(bool, bool singular)
    {
      QCString result("Autor");
      if (!singular)  result+="en";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Benutzt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementiert " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementiert in " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Inhaltsverzeichnis";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Veraltete Elemente";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Ereignisse";
    }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Ereignisdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Pakettypen";
    }

    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Paketfunktionen";
    }

    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Statische Paketfunktionen";
    }
    
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Paketattribute";
    }
    
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statische Paketattribute";
    }
    
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
      return "Hier ist ein Graph, der zeigt, was diese Funktion aufruft:";
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
      return "Suchen nach";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Suchergebnisse";
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
        return "Es wurden keine Dokumente zu Ihrer Suchanfrage gefunden.";
      }
      else if (numDocuments==1)
      {
        return "Es wurde <b>1</b> Dokument zu Ihrer Suchanfrage gefunden.";
      }
      else 
      {
        return "Es wurden <b>$num</b> Dokumente zu Ihrer Suchanfrage "
	       "gefunden. Die besten Treffer werden zuerst angezeigt.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Treffer:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Quellcode";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Verzeichnishierarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Verzeichnisdokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Verzeichnisse"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Diese Verzeichnishierarchie ist -mit Einschränkungen- "
	     "alphabetisch sortiert:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+="-Verzeichnisreferenz"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool singular)
    { 
      QCString result("Verzeichnis");
      if (!singular) result+="se";
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
       return "Dies ist eine überladene Methode, die der Bequemlichkeit "
              "wegen bereitgestellt wird. Sie unterscheidet sich von der "
              "obigen Funktion nur in den Argumenten die sie unterstützt.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Hier ist ein Graph der zeigt, wo diese Funktion aufgerufen wird:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentation der Aufzählungswerte"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Elementfunktionen/Unterroutinen-Dokumentation"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Datentyp-Liste"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Datenfelder"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
	  { return "Hier folgen die Datentypen mit Kurzbeschreibungen:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Hier folgt eine Liste aller ";
      if (!extractAll)
      {
        result+="dokumentierten ";
      }
      result+="Datentypelemente";
      result+=" mit Links ";
      if (!extractAll) 
      {
         result+="zur Datenstruktur-Dokumentation für jedes Element";
      }
      else 
      {
         result+="zu den Datentypen, zu denen sie gehören:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Datentyp-Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Datentyp-Dokumentation"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funktionen/Unterroutinen"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Funktionen/Unterroutinen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datentypen"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modulliste"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Hier folgt eine Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Module mit ihren Kurzbeschreibungen:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      result += "-";
      switch(compType)
      {
        case ClassDef::Class:      result+="Modul"; break;
        case ClassDef::Struct:     result+="Typ"; break;
        case ClassDef::Union:      result+="Union"; break;
        case ClassDef::Interface:  result+="Interface"; break;
        case ClassDef::Protocol:   result+="Protokoll"; break;
        case ClassDef::Category:   result+="Kategorie"; break;
        case ClassDef::Exception:  result+="Ausnahmen"; break;
      }
      if (isTemplate) result+="-Template";
      result+="-Referenz";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+="-Modul-Referenz";
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modul-Elemente"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Hier folgt eine Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Modulelemente mit Links ";
      if (extractAll) 
      {
        result+="zur Moduldokumentation für jedes Element:";
      }
      else 
      {
        result+="zu den Modulen, zu denen sie gehören:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Modul-Index"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool /*first_capital*/, bool singular)
    {       
      QCString result("Modul");
      if (!singular)  result+="e";
      return result; 
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Die Dokumentation für ";
      switch(compType)
      {
        case ClassDef::Class:      result+="dieses Modul"; break;
        case ClassDef::Struct:     result+="diesen Typ"; break;
        case ClassDef::Union:      result+="diese Union"; break;
        case ClassDef::Interface:  result+="dieses Interface"; break;
        case ClassDef::Protocol:   result+="dieses Protokoll"; break;
        case ClassDef::Category:   result+="diese Kategorie"; break;
        case ClassDef::Exception:  result+="diese Ausnahme"; break;
      }
      if (single)
        result+=" wurde aus der folgenden Datei erzeugt:";
      else
        result+=" wurde aus den folgenden Dateien erzeugt:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool /*first_capital*/, bool singular)
    { 
      QCString result("Typ");
      if (!singular)  result+="en";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool /*first_capital*/, bool singular)
    { 
      QCString result("Unterprogramm");
      if (!singular)  result+="e";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Bezug " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Lade ...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globaler Namensbereich";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Suche ...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Keine Treffer";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////
 
    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Diagramm der Verzeichnisabhängigkeiten für "+name;
    }
 
    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Datei in "+name;
    }
 
    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Include-Dateien in "+name;
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
      static const char *days[]   = { "Mon","Die","Mit","Don","Fre","Sam","Son" };
      static const char *months[] = { "Jan","Feb","Mär","Apr","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez" };
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

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Literaturverzeichnis"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Diagramm der Verzeichnisabhängigkeiten für ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "Detailebene"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Template-Parameter"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "und "+number+" mehr ..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Die Dokumentation für diesen enum wurde aus ";
      if (single)
		result += "der folgenden Datei";
	  else
		result += "den folgenden Dateien";
      result+=" generiert:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Enum-Referenz"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" geerbt von "+what; }

    /*! Header of the sections with inherited members specific for the 
     *  base class(es) 
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Weitere Geerbte Elemente"; }

};

#endif
