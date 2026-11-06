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
 * Reviewed, and extended by
 *   Jens Kallup (kallup-dev@web.de)
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
//   2024/05/03 Jens Kallup (kallup-dev@web.de)
//    - Updated for 1.10.0
//
//   Todo:
//    - see FIXME

#ifndef TRANSLATOR_DE_H
#define TRANSLATOR_DE_H

class TranslatorGerman : public Translator
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    DString idLanguage() override
    { return "german"; }

    /*! Used to get the LaTeX command(s) for the language support. This method
     *  was designed for languages which do wish to use a babel package.
     */
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage[ngerman]{babel}\n";
    }

    DString trISOLang() override
    {
      return "de";
    }
    DString getLanguageString() override
    {
      return "0x407 German";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Verwandte Funktionen"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Es handelt sich hierbei nicht um Elementfunktionen.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Ausführliche Beschreibung"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Details"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Dokumentation der benutzerdefinierten Datentypen"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Dokumentation der Aufzählungstypen"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "Dokumentation der Elementfunktionen"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
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
    DString trMore() override
    { return "Mehr ..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Aufstellung aller Elemente"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Elementverzeichnis"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Vollständige Aufstellung aller Elemente für"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return " einschließlich aller geerbten Elemente."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Automatisch erzeugt von Doxygen";
      if (!s.empty()) result+=" für "+s;
      result+=" aus dem Quellcode.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "enum-Bezeichner"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "enum-Wert"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "Definiert in"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Module"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Klassenhierarchie"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
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
    DString trFileList() override
    { return "Auflistung der Dateien"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
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
    DString trFileMembers() override
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
    DString trRelatedPages() override
    { return "Zusätzliche Informationen"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Beispiele"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Suchen"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Hier folgt eine hierarchische Auflistung der "
               "Entwurfseinheiten:";
      }
      else
      {
        return "Die Liste der Ableitungen ist, mit Einschränkungen, "
               "alphabetisch sortiert:";
      }
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Hier folgt die Aufzählung aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Dateien mit einer Kurzbeschreibung:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
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
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Hier folgt die Aufzählung aller ";
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
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Hier folgt die Aufzählung aller ";
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
    DString trExamplesDescription() override
    { return "Hier folgt eine Liste mit allen Beispielen:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Hier folgt eine Liste mit zusammengehörigen Themengebieten:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Hier folgt die Aufzählung aller Module:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Modul-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Hierarchie-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
    return "Datenstruktur-Verzeichnis";
      else
        return "Klassen-Verzeichnis";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Datei-Verzeichnis"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Modul-Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
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
    DString trFileDocumentation() override
    { return "Datei-Dokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Nachschlagewerk"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Makrodefinitionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Typdefinitionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Aufzählungen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funktionen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Variablen"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Aufzählungswerte"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Makro-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Dokumentation der benutzerdefinierten Typen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Dokumentation der Aufzählungstypen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Dokumentation der Funktionen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Variablen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
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
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Erzeugt am "+date;
      if (!projName.empty()) result+=" für "+projName;
      result+=" von";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Klassendiagramm für "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Warnung"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Rückgabe"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Siehe auch"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parameter"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Ausnahmebehandlung"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Erzeugt von"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Liste aller Namensbereiche"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Namensbereiche mit Kurzbeschreibung:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Freundbeziehungen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Freundbeziehungen und Funktionsdokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      DString result=clName+" ";
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
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName;
      result+="-Dateireferenz";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+="-Namensbereichsreferenz";
      return result;
    }

    DString trPublicMembers() override
    { return "Öffentliche Methoden"; }

    DString trPublicSlots() override
    { return "Öffentliche Slots"; }

    DString trSignals() override
    { return "Signale"; }

    DString trStaticPublicMembers() override
    { return "Öffentliche, statische Methoden"; }

    DString trProtectedMembers() override
    { return "Geschützte Methoden"; }

    DString trProtectedSlots() override
    { return "Geschützte Slots"; }

    DString trStaticProtectedMembers() override
    { return "Geschützte, statische Methoden"; }

    DString trPrivateMembers() override
    { return "Private Methoden"; }

    DString trPrivateSlots() override
    { return "Private Slots"; }

    DString trStaticPrivateMembers() override
    { return "Private, statische Methoden"; }

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
            result+=" und ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Abgeleitet von "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Basisklasse für "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Erneute Implementation von "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Erneute Implementation in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Elemente eines Namensbereiches"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Hier folgt die Aufzählung aller ";
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
    DString trNamespaceIndex() override
    { return "Verzeichnis der Namensbereiche"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Dokumentation der Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Namensbereiche"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      DString result="Die Dokumentation für diese";
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
    DString trReturnValues() override
    { return "Rückgabewerte"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Hauptseite"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "S."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Definiert in Zeile @0 der Datei @1.";
    }

    DString trDefinedInSourceFile() override
    {
      return "Definiert in Datei @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Veraltet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Zusammengehörigkeiten von "+clName+":";
    }

    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Include-Abhängigkeitsdiagramm für "+fName+":";
    }

    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Beschreibung der Konstruktoren und Destruktoren";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "gehe zum Quellcode dieser Datei";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "gehe zur Dokumentation dieser Datei";
    }

    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Vorbedingung";
    }

    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Nachbedingung";
    }

    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariante";
    }

    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Initialisierung:";
    }

    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "code";
    }

    DString trGraphicalHierarchy() override
    {
      return "Grafische Darstellung der Klassenhierarchie";
    }

    DString trGotoGraphicalHierarchy() override
    {
      return "gehe zur grafischen Darstellung der Klassenhierarchie";
    }

    DString trGotoTextualHierarchy() override
    {
      return "gehe zur textbasierten Darstellung der Klassenhierarchie";
    }

    DString trPageIndex() override
    {
      return "Seitenindex";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    //RK: had to change here because of the new command \remark
    DString trNote() override
    {
      return "Zu beachten";
    }

    DString trPublicTypes() override
    {
      return "Öffentliche Typen";
    }

    DString trPublicAttribs() override
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

    DString trStaticPublicAttribs() override
    {
      return "Statische öffentliche Attribute";
    }

    DString trProtectedTypes() override
    {
      return "Geschützte Typen";
    }

    DString trProtectedAttribs() override
    {
      return "Geschützte Attribute";
    }

    DString trStaticProtectedAttribs() override
    {
      return "Statische, geschützte Attribute";
    }

    DString trPrivateTypes() override
    {
      return "Private Typen";
    }

    DString trPrivateAttribs() override
    {
      return "Private Attribute";
    }

    DString trStaticPrivateAttribs() override
    {
      return "Statische, private Attribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "Noch zu erledigen";
    }

    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Ausstehende Aufgaben";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Wird benutzt von";
    }

    DString trRemarks() override
    {
      return "Bemerkungen";
    }

    DString trAttention() override
    {
      return "Achtung";
    }

    DString trInclByDepGraph() override
    {
      return "Dieser Graph zeigt, welche Datei direkt oder "
             "indirekt diese Datei enthält:";
    }

    DString trSince() override
    {
      return "Seit";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Erklärung des Graphen";
    }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
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
    DString trLegend() override
    {
      return "Legende";
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
      return "Test-Liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Propertys";
    }

    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Dokumentation der Propertys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
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
    DString trPackage(const DString &name) override
    {
      return "Paket "+name;
    }


    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Hier folgen die Pakete mit einer Kurzbeschreibung (wenn verfügbar):";
    }

    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Pakete";
    }

    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Wert:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Fehler";
    }

    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Liste der bekannten Fehler";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file. See translator_en.h for details. */
    DString trRTFansicp() override
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
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
    DString trClass(bool, bool singular) override
    {
      return createNoun(true, singular, "Klasse", "n");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool, bool singular) override
    {
      return createNoun(true, singular, "Datei", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool, bool singular) override
    {
      return createNoun(true, singular, "Namensbereich", "e");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool, bool singular) override
    {
      return createNoun(true, singular, "Gruppe", "n");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool, bool singular) override
    {
      return createNoun(true, singular, "Seite", "n");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool, bool singular) override
    {
      return createNoun(true, singular, "Element", "e");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool, bool singular) override
    {
      return createNoun(true, singular, "Global", ""); // FIXME
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7-20010524
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages.
     */
    DString trAuthor(bool, bool singular) override
    {
      return createNoun(true, singular, "Autor", "en");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Benutzt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Implementiert " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Implementiert in " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Inhaltsverzeichnis";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Veraltete Elemente";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Ereignisse";
    }

    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Ereignisdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Pakettypen";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Paketfunktionen";
    }
    DString trPackageMembers() override
    {
      return "Paketelemente";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statische Paketfunktionen";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Paketattribute";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statische Paketattribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Hier ist ein Graph, der zeigt, was diese Funktion aufruft:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
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
    DString trSearchResults(int numDocuments) override
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
    DString trSearchMatches() override
    {
      return "Treffer:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " Quellcode";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Verzeichnishierarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Verzeichnisdokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Verzeichnisse"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+="-Verzeichnisreferenz"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool, bool singular) override
    {
      return createNoun(true, singular, "Verzeichnis", "se");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Dies ist eine überladene Methode, die der Bequemlichkeit "
              "wegen bereitgestellt wird. Sie unterscheidet sich von der "
              "obigen Funktion nur in den Argumenten die sie unterstützt.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Hier ist ein Graph der zeigt, wo diese Funktion aufgerufen wird:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Dokumentation der Aufzählungswerte"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Elementfunktionen/Unterroutinen-Dokumentation"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Datentyp-Liste"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Datenfelder"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
      { return "Hier folgen die Datentypen mit Kurzbeschreibungen:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Hier folgt eine Liste aller ";
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
    DString trCompoundIndexFortran() override
    { return "Datentyp-Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Datentyp-Dokumentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funktionen/Unterroutinen"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Funktionen/Unterroutinen-Dokumentation"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Datentypen"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Modulliste"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Hier folgt eine Liste aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Module mit ihren Kurzbeschreibungen:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
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
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+="-Modul-Referenz";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Modul-Elemente"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Hier folgt eine Liste aller ";
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
    DString trModulesIndex() override
    { return "Modul-Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool /*first_capital*/, bool singular) override
    {
      return createNoun(true, singular, "Modul", "e");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      DString result="Die Dokumentation für ";
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
    DString trType(bool /*first_capital*/, bool singular) override
    {
      return createNoun(true, singular, "Typ", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool /*first_capital*/, bool singular) override
    {
      return createNoun(true, singular, "Unterprogramm", "e");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return "Bezug " + name;
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Lade ...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Globaler Namensbereich";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Suche ...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
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
    DString trFileIn(const DString &name) override
    {
      return "Datei in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
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
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Mon","Die","Mit","Don","Fre","Sam","Son" };
      static const char *months[] = { "Jan","Feb","Mär","Apr","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
    DString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };
      static const char *days_full[]    = { "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    DString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez" };
      static const char *months_full[]  = { "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      return text;
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
    { return "Literaturverzeichnis"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Diagramm der Verzeichnisabhängigkeiten für ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "Detailebene"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Template-Parameter"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "und "+number+" mehr ..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "Die Dokumentation für diesen enum wurde aus ";
      if (single)
        result += "der folgenden Datei";
      else
        result += "den folgenden Dateien";
      result+=" generiert:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return name+" Enum-Referenz"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" geerbt von "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Weitere Geerbte Elemente"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button thatappears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "einzuschalten" : "auszuschalten";
      return "Klicken um Panelsynchronisation "+opt;
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Bestandteil der Kategorie @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Erweitert Klasse @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Klassenmethoden";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Instanzmethoden";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Methodendokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Exportierte Interfaces"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Eingebundene Dienste"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Konstante Gruppen"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Konstantengruppen-Referenz";
      return result;
    }

    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Dienstreferenz";
      return result;
    }

    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Singleton-Referenz";
      return result;
    }

    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      DString result="Die Dokumentation für diesen Dienst "
                                "wurde generiert aus ";
      if (single) result+="folgender Datei: "; else result+="folgenden Dateien: ";
      return result;
    }

    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      DString result="Die Dokumentation für diesen Singleton wurde generiert aus ";
      if (single) result+="folgender Datei:"; else result+="folgenden Dateien:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "Entwurfseinheiten-Hierarchie"; }
    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "Auflistung der Entwurfseinheiten"; }
    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "Entwurfseinheiten-Elemente"; }
    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
        return "Hier folgt die Aufzählung aller Entwurfseinheiten und deren "
               "Elemente mit einer Kurzbeschreibung:";
    }
    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "Entwurfseinheiten-Verzeichnis"; }
    /** VHDL design units */
    DString trDesignUnits() override
    { return "Entwurfseinheiten"; }
    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "Funktionen/Prozeduren/Prozesse"; }
    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type,bool single) override
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
    DString trCustomReference(const DString &name) override
    { return name+"-Referenz"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    DString trDesignUnitDocumentation() override
    { return "Entwurfseinheiten-Dokumentation"; }

    DString trConstants() override
    { return "Konstanten"; }

    DString trConstantDocumentation() override
    { return "Konstanten Dokumentation"; }

    DString trSequences() override
    { return "Sequenzen"; }

    DString trSequenceDocumentation() override
    { return "Sequenzen Dokumentation"; }

    DString trDictionaries() override
    { return "Wörterbücher"; }

    DString trDictionaryDocumentation() override
    { return "Wörterbuch Dokumentation"; }

    DString trSliceInterfaces() override
    { return "Schnittstellen"; }

    DString trInterfaceIndex() override
    { return "Schnittstellen-Verzeichnis"; }

    DString trInterfaceList() override
    { return "Liste aller Schnittstellen"; }

    DString trInterfaceListDescription() override
    { return "Hier folgen die Schnittstellen mit Kurzbeschreibung:"; }

    DString trInterfaceHierarchy() override
    { return "Schnittstellenhierarchie"; }

    DString trInterfaceHierarchyDescription() override
    { return "Die Liste aller Schnittstellen ist, mit Einschränkungen, alphabetisch sortiert:"; }

    DString trInterfaceDocumentation() override
    { return "Schnittstellen Dokumentation"; }

    DString trStructs() override
    { return "Strukturen"; }

    DString trStructIndex() override
    { return "Strukturen-Verzeichnis"; }

    DString trStructList() override
    { return "Liste aller Strukturen"; }

    DString trStructListDescription() override
    { return "Hier sind alle Strukturen mit Kurzbeschreibung:"; }

    DString trStructDocumentation() override
    { return "Strukturen Dokumentation"; }

    DString trExceptionIndex() override
    { return "Ausnahmen-Verzeichnis"; }

    DString trExceptionList() override
    { return "Liste aller Ausnahmen"; }

    DString trExceptionListDescription() override
    { return "Hier folgen alle Ausnahmen mit Kurzbeschreibung:"; }

    DString trExceptionHierarchy() override
    { return "Ausnahmen Hierarchie"; }

    DString trExceptionHierarchyDescription() override
    { return "Die Liste der Ausnahmen ist, mit Einschränkungen, alphabetisch sortiert:"; }

    DString trExceptionDocumentation() override
    { return "Ausnahmen Dokumentation"; }

    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result=clName;
      if (isLocal) result+=" Lokal";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasse"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Schnittstelle"; break;
        case ClassDef::Protocol:   result+=" Protokoll"; break;
        case ClassDef::Category:   result+=" Kategorie"; break;
        case ClassDef::Exception:  result+=" Ausnahme"; break;
        default: break;
      }
      result+=" Referenz";
      return result;
    }
    DString trOperations() override
    { return "Operationen"; }

    DString trOperationDocumentation() override
    { return "Operationen Dokumentation"; }

    DString trDataMembers() override
    { return "Data Members"; }

    DString trDataMemberDocumentation() override
    { return "Data Member Dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    DString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "Konzept", "Konzepte");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    DString trConceptReference(const DString &conceptName) override
    {
      DString result=conceptName;
      result+=" Konzept Referenz";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    DString trConceptList() override
    { return "Konzept Liste"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    DString trConceptIndex() override
    { return "Konzept Index"; }

    /*! used as the title of chapter containing all information about concepts. */
    DString trConceptDocumentation() override
    { return "Konzept Dokumentation"; }

    /*! used as an introduction to the concept list */
    DString trConceptListDescription(bool extractAll) override
    {
      DString result="Hier folgt eine Lister aller ";
      if (!extractAll) result+="dokumentierten ";
      result+="Konzepte mit Kurzbeschreibungen:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    DString trConceptDefinition() override
    {
      return "Konzept-Definition";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    DString trPackageList() override
    { return "Paket-Liste"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
    { return "Flußdiagram:"; }

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
    { return "Zugehörige Symbole"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    DString trRelatedSymbolsSubscript() override
    { return "(Hinweis: diese Symbole sind keine Funktionen.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    DString trRelatedSymbolDocumentation() override
    { return "Freunde und bezugnehmende Symbol-Dokumentation"; }

    /*! the compound type as used for the xrefitems */
    DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      DString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) result=trType(true,true);
          else result=trClass(true,true);
          break;
        case ClassDef::Struct:     result="Struct"; break;
        case ClassDef::Union:      result="Union"; break;
        case ClassDef::Interface:  result="Interface"; break;
        case ClassDef::Protocol:   result="Protocol"; break;
        case ClassDef::Category:   result="Category"; break;
        case ClassDef::Exception:  result="Exception"; break;
        case ClassDef::Service:    result="Service"; break;
        case ClassDef::Singleton:  result="Singleton"; break;
        default: break;
      }
      return result;
    }

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hier ist eine Lister aller ";
      if (!extractAll) result+="documented ";

      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="functions, variables, defines, enums, and typedefs";
          }
          else
          {
            result+="zugehöriger Dateien";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="Funktionen";
          break;
        case FileMemberHighlight::Variables:
          result+="Variablen";
          break;
        case FileMemberHighlight::Typedefs:
          result+="Typen-Definitionen";
          break;
        case FileMemberHighlight::Sequences:
          result+="Abfolgen";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="Wörterbücher";
          break;
        case FileMemberHighlight::Enums:
          result+="Aufzählungen";
          break;
        case FileMemberHighlight::EnumValues:
          result+="Aufzählungs-Werte";
          break;
        case FileMemberHighlight::Defines:
          result+="Makros";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result+=" mit verweisen auf ";
      if (extractAll)
        result+="bezugnehmenden Dateien:";
      else
        result+="die Dokumentation";
      return result;
    }
    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hier ist eine Liste aller ";
      if (!extractAll)
      {
        result+="dokumentierter ";
      }

      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="Struktur- und Einheits-Felder";
          }
          else
          {
            result+="Klassen-Funktionen";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="Funktionen";
          break;
        case ClassMemberHighlight::Variables:
          result+="Variablen";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="Typen-Definitionen";
          break;
        case ClassMemberHighlight::Enums:
          result+="Aufzählungen";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="Aufzählungs-Werte";
          break;
        case ClassMemberHighlight::Properties:
          result+="Eigenschaften";
          break;
        case ClassMemberHighlight::Events:
          result+="Ereignisse";
          break;
        case ClassMemberHighlight::Related:
          result+="zugehörige Symbole";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result+=" mit Verweisen auf ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="die struct/union Dokumentation für jedes Feld:";
        }
        else
        {
          result+="die Klassen-Dokumentation für jede Funktion:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="die Strukturen/Einheiten auf denen sie sich beziehen:";
        }
        else
        {
          result+="die Klassen auf denen sie sich beziehen:";
        }
      }
      return result;
    }
    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);

      DString result           = "Hier folgt eine Liste mit Verweisen auf die Namensraum-Dokumentation für ";
      DString result_singular1 = result + "jede dokumentierte";
      DString result_singular2 = result + "jedes dokumentiertes";
      DString result_singular3 = result + "jeder dokumentierten";
      DString result_plural    = result + "alle dokumentierten";

      DString singularResult = "";
      DString pluralResult   = "";

      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult = result_singular2 + "Mitglied";
          pluralResult   = result_plural    + "Mitglieder";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult = result_singular1 + "Funktion";
          pluralResult   = result_plural    + "Funktionen";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult = result_singular1 + "Variable";
          pluralResult   = result_plural    + "Variablen";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult = result_singular1 + "Typ-Definition";
          pluralResult   = result_plural    + "Typen-Defintionen";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult = result_singular1 + "Abfolge";
          pluralResult   = result_plural    + "Abfolgen";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult = result_singular2 + "Wörterbuch";
          pluralResult   = result_plural    + "Wörterbücher";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult = result_singular3 + "Aufzählung";
          pluralResult   = result_plural    + "Aufzählungen";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult = result_singular3 + "Aufzählung";
          pluralResult   = result_plural    + "Aufzählungen";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }

      // TODO: plural form !!!
      if (!extractAll)
        result += singularResult + " mit Verweise auf die Namensraum-Dokumentation:"; else
        result += singularResult + " mit Verweise auf dem sich beziehenden Namensraum:";

      return result;
    }
    DString trDefinition() override  { return "Definition";}
    DString trDeclaration() override { return "Deklaration";}

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0 added by paule32
//////////////////////////////////////////////////////////////////////////

    DString trTopics() override
    { return "Themen"; }
    DString trTopicDocumentation() override
    { return "Themen Dokumentation"; }
    DString trTopicList() override
    { return "Themen-Liste"; }
    DString trTopicIndex() override
    { return "Themen Index"; }
    DString trTopicListDescription() override
    { return "Es folgt eine Liste der Themen mit einer Kurzbeschreibung"; }
    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result = "Zusammengefaßt ";

      DString singularResult = result + "bezieht sich das Modul auf: ";
      DString pluralResult   = result + "beziehen sich die Module auf: ";

      switch (hl)
      {
        case ModuleMemberHighlight::All:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jedes Mitglied:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen aller Mitglieder:";
          break;
        case ModuleMemberHighlight::Functions:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jede Funktion:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen aller Funktionen:";
          break;
        case ModuleMemberHighlight::Variables:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jede Variable:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen für alle Variablen:";
          break;
        case ModuleMemberHighlight::Typedefs:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jede Typ-Definition:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen für jeden Definitions-Typen:";
          break;
        case ModuleMemberHighlight::Enums:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jede Aufzählung:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen aller Aufzählungen:";
          break;
        case ModuleMemberHighlight::EnumValues:
          if (!extractAll)
              singularResult += "die verlinkte Modul-Beschreibung für jedem Aufzählungswert:"; else
              pluralResult   += "die verlinkten Modul-Beschreibungen aller Aufzählungswerte:";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      if (!extractAll)
        result = singularResult; else
        result = pluralResult;

      return result;
    }
    DString trExportedModules() override
    {
      return "Exportierte Module";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    DString trCopyToClipboard() override
    {
      return "In die Zwischenablage kopieren";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    DString trImportant() override
    {
      return "Wichtig";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    DString trRequirements() override
    {
      return "Anforderungen";
    }
    // table header for the column with the requirements IDs
    DString trRequirementID() override
    {
      return "ID";
    }
    // indicates a symbol implements (satisfies) a requirement
    DString trSatisfies(bool singular) override
    {
      return createNoun(true,singular,"Erfüllt anforderung","en");
    }
    // indicates a requirement is satisfied (implemented) by one or more symbols
    DString trSatisfiedBy(const DString &list) override
    {
      return "Wird erfüllt durch "+list+".";
    }
    DString trUnsatisfiedRequirements() override
    {
      return "Nicht erfüllte Anforderungen";
    }
    DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "Die Anforderung "+list+" hat keine 'erfüllt'-Beziehung." :
        "Die Anforderungen "+list+" haben keine 'erfüllt'-Beziehung.";
    }
    // indicates a symbol verifies (tests) a requirement
    DString trVerifies(bool singular) override
    {
      return createNoun(true,singular,"Verifiziert anforderung","en");
    }
    // indicates a requirement is verified (tested) by one or more symbols
    DString trVerifiedBy(const DString &list) override
    {
      return "Wird verifiziert durch "+list+".";
    }
    DString trUnverifiedRequirements() override
    {
      return "Nicht verifizierte Anforderungen";
    }
    DString trUnverifiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "Die Anforderung "+list+" hat keine 'verifiziert'-Beziehung." :
        "Die Anforderungen "+list+" haben keine 'verifiziert'-Beziehung.";
    }

};
#endif
