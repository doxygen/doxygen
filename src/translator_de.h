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
//   2016/03/15 Carsten Schumann (carsten at familie-schumann dot info)
//    - Updated for "new since 1.8.4" version
//
//   2017/10/12 Arnd Weber (arnd dot weber at bafg dot de)
//              Beatrix Konz
//    - Updated for 1.8.13
//    - Resynced trMemberDataDocumentation() and trFileMembers() to include the
//      boolean switch OPTIMIZE_OUTPUT_FOR_C
//    - Replaced "\t" by "    "
//
//   Todo:
//    - see FIXME

#ifndef TRANSLATOR_DE_H
#define TRANSLATOR_DE_H

class TranslatorGerman : public TranslatorAdapter_1_8_15
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage() override
    { return "german"; }

    /*! Used to get the LaTeX command(s) for the language support. This method
     *  was designed for languages which do wish to use a babel package.
     */
    virtual QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[ngerman]{babel}\n";
    }

    virtual QCString trISOLang() override
    {
      return "de";
    }
    virtual QCString getLanguageString() override
    {
      return "0x407 German";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Verwandte Funktionen"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Es handelt sich hierbei nicht um Elementfunktionen.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Ausführliche Beschreibung"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Details"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Dokumentation der benutzerdefinierten Datentypen"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Dokumentation der Aufzählungstypen"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Dokumentation der Elementfunktionen"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentation der Felder";
      }
      else
      {
        return "Dokumentation der Datenelemente";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Mehr ..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Aufstellung aller Elemente"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Elementverzeichnis"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Vollständige Aufstellung aller Elemente für "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return " einschließlich aller geerbten Elemente."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automatisch erzeugt von Doxygen";
      if (!s.isEmpty()) result+=" für "+s;
      result+=" aus dem Quellcode.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "enum-Bezeichner"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "enum-Wert"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "Definiert in"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Module"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Klassenhierarchie"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datenstrukturen";
      }
      else
      {
        return "Auflistung der Klassen";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Auflistung der Dateien"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datenstruktur-Elemente";
      }
      else
      {
        return "Klassen-Elemente";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globale Elemente";
      }
      else
      {
        return "Datei-Elemente";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Zusätzliche Informationen"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Beispiele"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Suchen"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Hier folgt eine hierarchische Auflistung der "
               "Entwurfseinheiten:";
      }
      else
      {
        return "Die Liste der Ableitungen ist -mit Einschränkungen- "
               "alphabetisch sortiert:";
      }
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateien mit einer Kurzbeschreibung:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Hier folgt die Aufzählung aller Datenstrukturen "
               "mit einer Kurzbeschreibung:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Hier folgt die Aufzählung aller Klassen mit einer Kurzbeschreibung:";
      }
      else
      {
        return "Hier folgt die Aufzählung aller Klassen, Strukturen, "
               "Varianten und Schnittstellen mit einer Kurzbeschreibung:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll)
      {
        result+="dokumentierten ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trExamplesDescription() override
    { return "Hier folgt eine Liste mit allen Beispielen:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Hier folgt eine Liste mit zusammengehörigen Themengebieten:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Hier folgt die Aufzählung aller Module:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Modul-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Hierarchie-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
    return "Datenstruktur-Verzeichnis";
      else
        return "Klassen-Verzeichnis";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "Datei-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Modul-Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datenstruktur-Dokumentation";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klassen-Dokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Datei-Dokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Nachschlagewerk"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Makrodefinitionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Typdefinitionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Aufzählungen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Funktionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Variablen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Aufzählungswerte"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Makro-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Dokumentation der benutzerdefinierten Typen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Dokumentation der Aufzählungstypen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Dokumentation der Funktionen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Variablen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Erzeugt am "+date;
      if (!projName.isEmpty()) result+=" für "+projName;
      result+=" von";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return "Klassendiagramm für "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Warnung"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Rückgabe"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Siehe auch"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Parameter"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Ausnahmebehandlung"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Erzeugt von"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Liste aller Namensbereiche"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Namensbereiche mit Kurzbeschreibung:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Freundbeziehungen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Freundbeziehungen und Funktionsdokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      QCString result=clName+" ";
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
        default: break;
      }
      result+="referenz";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+="-Dateireferenz";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+="-Namensbereichsreferenz";
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Öffentliche Methoden"; }

    virtual QCString trPublicSlots() override
    { return "Öffentliche Slots"; }

    virtual QCString trSignals() override
    { return "Signale"; }

    virtual QCString trStaticPublicMembers() override
    { return "Öffentliche, statische Methoden"; }

    virtual QCString trProtectedMembers() override
    { return "Geschützte Methoden"; }

    virtual QCString trProtectedSlots() override
    { return "Geschützte Slots"; }

    virtual QCString trStaticProtectedMembers() override
    { return "Geschützte, statische Methoden"; }

    virtual QCString trPrivateMembers() override
    { return "Private Methoden"; }

    virtual QCString trPrivateSlots() override
    { return "Private Slots"; }

    virtual QCString trStaticPrivateMembers() override
    { return "Private, statische Methoden"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries) override
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
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Abgeleitet von "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Basisklasse für "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return "Erneute Implementation von "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Erneute Implementation in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Elemente eines Namensbereiches"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
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
    virtual QCString trNamespaceIndex() override
    { return "Verzeichnis der Namensbereiche"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Dokumentation der Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="Die Dokumentation für diese";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?" Entwurfseinheiten":" Klasse"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Variante"; break;
        case ClassDef::Interface:  result+=" Schnittstelle"; break;
        case ClassDef::Protocol:   result+="s Protokoll"; break;
        case ClassDef::Category:   result+=" Kategorie"; break;
        case ClassDef::Exception:  result+=" Ausnahme"; break;
        default: break;
      }
      result+=" wurde erzeugt aufgrund der Datei";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Rückgabewerte"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Hauptseite"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "S."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Definiert in Zeile @0 der Datei @1.";
    }

    virtual QCString trDefinedInSourceFile() override
    {
      return "Definiert in Datei @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Veraltet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Zusammengehörigkeiten von "+clName+":";
    }

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Include-Abhängigkeitsdiagramm für "+fName+":";
    }

    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Beschreibung der Konstruktoren und Destruktoren";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "gehe zum Quellcode dieser Datei";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "gehe zur Dokumentation dieser Datei";
    }

    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Vorbedingung";
    }

    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Nachbedingung";
    }

    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Invariante";
    }

    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Initialisierung:";
    }

    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "code";
    }

    virtual QCString trGraphicalHierarchy() override
    {
      return "Grafische Darstellung der Klassenhierarchie";
    }

    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "gehe zur grafischen Darstellung der Klassenhierarchie";
    }

    virtual QCString trGotoTextualHierarchy() override
    {
      return "gehe zur textbasierten Darstellung der Klassenhierarchie";
    }

    virtual QCString trPageIndex() override
    {
      return "Seitenindex";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    //RK: had to change here because of the new command \remark
    virtual QCString trNote() override
    {
      return "Zu beachten";
    }

    virtual QCString trPublicTypes() override
    {
      return "Öffentliche Typen";
    }

    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datenfelder";
      }
      else
      {
        return "Öffentliche Attribute";
      }
    }

    virtual QCString trStaticPublicAttribs() override
    {
      return "Statische öffentliche Attribute";
    }

    virtual QCString trProtectedTypes() override
    {
      return "Geschützte Typen";
    }

    virtual QCString trProtectedAttribs() override
    {
      return "Geschützte Attribute";
    }

    virtual QCString trStaticProtectedAttribs() override
    {
      return "Statische, geschützte Attribute";
    }

    virtual QCString trPrivateTypes() override
    {
      return "Private Typen";
    }

    virtual QCString trPrivateAttribs() override
    {
      return "Private Attribute";
    }

    virtual QCString trStaticPrivateAttribs() override
    {
      return "Statische, private Attribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() override
    {
      return "Noch zu erledigen";
    }

    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Ausstehende Aufgaben";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Wird benutzt von";
    }

    virtual QCString trRemarks() override
    {
      return "Bemerkungen";
    }

    virtual QCString trAttention() override
    {
      return "Achtung";
    }

    virtual QCString trInclByDepGraph() override
    {
      return "Dieser Graph zeigt, welche Datei direkt oder "
             "indirekt diese Datei enthält:";
    }

    virtual QCString trSince() override
    {
      return "Seit";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Erklärung des Graphen";
    }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
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
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
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
        "<li>Ein blauer Pfeil stellt eine öffentliche Vererbungsbeziehung "
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
    virtual QCString trLegend() override
    {
      return "Legende";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Test";
    }

    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Test-Liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Propertys";
    }

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Dokumentation der Propertys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datenstrukturen";
      }
      else
      {
        return "Klassen";
      }
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Paket "+name;
    }


    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Hier folgen die Pakete mit einer Kurzbeschreibung (wenn verfügbar):";
    }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Pakete";
    }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Wert:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Fehler";
    }

    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Liste der bekannten Fehler";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file. See translator_en.h for details. */
    virtual QCString trRTFansicp() override
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
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
    virtual QCString trClass(bool, bool singular) override
    {
      QCString result("Klasse");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool, bool singular) override
    {
      QCString result("Datei");
      if (!singular)  result+="en";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool, bool singular) override
    {
      QCString result("Namensbereich");
      if (!singular)  result+="e";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool, bool singular) override
    {
      QCString result("Gruppe");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool, bool singular) override
    {
      QCString result("Seite");
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool, bool singular) override
    {
      QCString result("Element");
      if (!singular)  result+="e";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
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
    virtual QCString trAuthor(bool, bool singular) override
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
    virtual QCString trReferences() override
    {
      return "Benutzt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Implementiert " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Implementiert in " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Inhaltsverzeichnis";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Veraltete Elemente";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Ereignisse";
    }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Ereignisdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Pakettypen";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Paketfunktionen";
    }
    virtual QCString trPackageMembers() override
    {
      return "Paketelemente";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Statische Paketfunktionen";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Paketattribute";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Statische Paketattribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Hier ist ein Graph, der zeigt, was diese Funktion aufruft:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
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
    virtual QCString trSearchResults(int numDocuments) override
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
    virtual QCString trSearchMatches() override
    {
      return "Treffer:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return filename + " Quellcode";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Verzeichnishierarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Verzeichnisdokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Verzeichnisse"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+="-Verzeichnisreferenz"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool singular) override
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
    virtual QCString trOverloadText() override
    {
       return "Dies ist eine überladene Methode, die der Bequemlichkeit "
              "wegen bereitgestellt wird. Sie unterscheidet sich von der "
              "obigen Funktion nur in den Argumenten die sie unterstützt.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Hier ist ein Graph der zeigt, wo diese Funktion aufgerufen wird:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Dokumentation der Aufzählungswerte"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Elementfunktionen/Unterroutinen-Dokumentation"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Datentyp-Liste"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Datenfelder"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
      { return "Hier folgen die Datentypen mit Kurzbeschreibungen:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    virtual QCString trCompoundIndexFortran() override
    { return "Datentyp-Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Datentyp-Dokumentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Funktionen/Unterroutinen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Funktionen/Unterroutinen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Datentypen"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Modulliste"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Hier folgt eine Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Module mit ihren Kurzbeschreibungen:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
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
        default: break;
      }
      if (isTemplate) result+="-Template";
      result+="-Referenz";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+="-Modul-Referenz";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Modul-Elemente"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
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
    virtual QCString trModulesIndex() override
    { return "Modul-Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool /*first_capital*/, bool singular) override
    {
      QCString result("Modul");
      if (!singular)  result+="e";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Die Dokumentation für ";
      switch(compType)
      {
        case ClassDef::Class:      result+="dieses Modul"; break;
        case ClassDef::Struct:     result+="diesen Typ"; break;
        case ClassDef::Union:      result+="diese Union"; break;
        case ClassDef::Interface:  result+="dieses Interface"; break;
        case ClassDef::Protocol:   result+="dieses Protokoll"; break;
        case ClassDef::Category:   result+="diese Kategorie"; break;
        case ClassDef::Exception:  result+="diese Ausnahme"; break;
        default: break;
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
    virtual QCString trType(bool /*first_capital*/, bool singular) override
    {
      QCString result("Typ");
      if (!singular)  result+="en";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool /*first_capital*/, bool singular) override
    {
      QCString result("Unterprogramm");
      if (!singular)  result+="e";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return "Bezug " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Lade ...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Globaler Namensbereich";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Suche ...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
    {
      return "Keine Treffer";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const QCString &name) override
    {
      return "Datei in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
    {
      return "Include-Dateien in "+name;
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
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Mon","Die","Mit","Don","Fre","Sam","Son" };
      static const char *months[] = { "Jan","Feb","Mär","Apr","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez" };
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
    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };
      static const char *days_full[]    = { "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez" };
      static const char *months_full[]  = { "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences() override
    { return "Literaturverzeichnis"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return QCString("Diagramm der Verzeichnisabhängigkeiten für ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel() override
    { return "Detailebene"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters() override
    { return "Template-Parameter"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number) override
    { return "und "+number+" mehr ..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Die Dokumentation für diesen enum wurde aus ";
      if (single)
        result += "der folgenden Datei";
      else
        result += "den folgenden Dateien";
      result+=" generiert:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Enum-Referenz"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" geerbt von "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers() override
    { return "Weitere Geerbte Elemente"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button thatappears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "einzuschalten" : "auszuschalten";
      return "Klicken um Panelsynchronisation "+opt;
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory() override
    {
      return "Bestandteil der Kategorie @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass() override
    {
      return "Erweitert Klasse @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods() override
    {
      return "Klassenmethoden";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods() override
    {
      return "Instanzmethoden";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation() override
    {
      return "Methodendokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces() override
    { return "Exportierte Interfaces"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices() override
    { return "Eingebundene Dienste"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups() override
    { return "Konstante Gruppen"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Konstantengruppen-Referenz";
      return result;
    }

    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Dienstreferenz";
      return result;
    }

    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Singleton-Referenz";
      return result;
    }

    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single) override
    {
      QCString result="Die Dokumentation für diesen Dienst "
                                "wurde generiert aus ";
      if (single) result+="folgender Datei: "; else result+="folgenden Dateien: ";
      return result;
    }

    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single) override
    {
      QCString result="Die Dokumentation für diesen Singleton wurde generiert aus ";
      if (single) result+="folgender Datei:"; else result+="folgenden Dateien:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy() override
    { return "Entwurfseinheiten-Hierarchie"; }
    /** VHDL design unit list */
    virtual QCString trDesignUnitList() override
    { return "Auflistung der Entwurfseinheiten"; }
    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers() override
    { return "Entwurfseinheiten-Elemente"; }
    /** VHDL design unit list description */
    virtual QCString trDesignUnitListDescription() override
    {
        return "Hier folgt die Aufzählung aller Entwurfseinheiten und deren "
               "Elemente mit einer Kurzbeschreibung:";
    }
    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex() override
    { return "Entwurfseinheiten-Verzeichnis"; }
    /** VHDL design units */
    virtual QCString trDesignUnits() override
    { return "Entwurfseinheiten"; }
    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc() override
    { return "Funktionen/Prozeduren/Prozesse"; }
    /** VHDL type */
    virtual QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Bibliothek";
          else        return "Bibliotheken";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Paket";
          else        return "Pakete";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Signal";
          else        return "Signale";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Komponente";
          else        return "Komponenten";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Konstante";
          else        return "Konstanten";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entwurfseinheit";
          else        return "Entwurfseinheiten";
        case VhdlSpecifier::TYPE:
          if (single) return "Typ";
          else        return "Typen";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Subtyp";
          else        return "Subtypen";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Funktion";
          else        return "Funktionen";
        case VhdlSpecifier::RECORD:
          if (single) return "Datenstruktur";
          else        return "Datenstrukturen";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Prozedur";
          else        return "Prozeduren";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Architektur";
          else        return "Architekturen";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Attribut";
          else        return "Attribute";
        case VhdlSpecifier::PROCESS:
          if (single) return "Prozess";
          else        return "Prozesse";
        case VhdlSpecifier::PORT:
          if (single) return "Schnittstelle";
          else        return "Schnittstellen";
        case VhdlSpecifier::USE:
          if (single) return "Use Klausel";
          else        return "Use Klauseln";
        case VhdlSpecifier::GENERIC:
          if (single) return "Parameter";
          else        return "Parameter";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Paketkörper";
        case VhdlSpecifier::UNITS:
          return "Einheiten";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Geteilte Variable";
          else        return "Geteilte Variablen";
        case VhdlSpecifier::VFILE:
          if (single) return "Datei";
          else        return "Dateien";
        case VhdlSpecifier::GROUP:
          if (single) return "Gruppe";
          else        return "Gruppen";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Instanziierung";
          else        return "Instanziierungen";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Aliase";
        case VhdlSpecifier::CONFIG:
          if (single) return "Konfiguration";
          else        return "Konfigurationen";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Verschiedenes";
        case VhdlSpecifier::UCF_CONST:
          return "Constraints";
        default:
          return "Klasse";
      }
    }
    virtual QCString trCustomReference(const QCString &name) override
    { return QCString(name)+"-Referenz"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation() override
    { return "Entwurfseinheiten-Dokumentation"; }

};

#endif
