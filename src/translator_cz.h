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
 */

#ifndef TRANSLATOR_CZ_H
#define TRANSLATOR_CZ_H

// Updates:
// --------
// 2012/07/31 - Updates for "new since 1.8.2".
// 2012/04/10 - Updates for "new since 1.8.0".
// 2011/07/28 - Updates for "new since 1.7.5".
// 2010/06/01 - typo
// 2010/04/28 - Updates for "new since 1.6.3".
// 2009/09/02 - Updates for "new since 1.6.0 (mainly for the new search engine)".
// 2008/06/09 - Corrections in trLegendDocs().
// 2007/11/13 - Update for "new since 1.5.4 (mainly for Fortran)".
// 2007/03/20 - removing decode(), conversion of literals to UTF-8.
// 2006/06/13 - translation of the trEnumerationValueDocumentation().
//              and clear in the Czech language.
//              modified trCallGraph() to make the meaning unambiguous
// 2006/05/10 - Update for "new since 1.4.6" -- trCallerGraph(),
// 2005/03/08 - Update for "new since 1.4.1" (trOverloadText())
// 2005/02/11 - The "never used" methods removed.
// 2004/09/14 - The new methods "since 1.3.9" implemented.
// 2004/06/16 - The new method "since 1.3.8" implemented.
// 2004/02/27 - Text inside the trCallGraph() corrected.
// 2004/02/26 - trLegendDocs() updated.
// 2003/08/13 - Four new methods "since 1.3.3" implemented.
// 2003/06/10 - Two new methods "since 1.3.1" implemented.
// 2003/04/28 - Five new methods "since 1.3" implemented.
// 2002/10/15 - The new trEvents() and trEventDocumentation() implemented.
// 2002/07/29 - The new trDeprecatedList() implemented.
// 2002/07/08 - The new trRTFTableOfContents() implemented. (my birthday! ;)
// 2002/03/05 - ... forgot to replace TranslatorAdapter... by Translator.
// 2002/01/23 - Two new methods "since 1.2.13" implemented.
// 2001/11/06 - trReferences() implemented.
// 2001/07/16 - trClassDocumentation() updated as in the English translator.
// 2001/05/25 - Corrections.
// 2001/05/18 - Updates, corrections.
// 2001/05/02 - Decode() inline changed to decode(); cleaning.
//              level as other translators.
//              class Translator.  The English translator is now on the same
//              introducing TranslatorAdapter class and the abstract base
// 2001/04/20 - Update for "new since 1.2.6-20010422". Experimental version
// 2001/04/10 - Updates (1.2.6-20010408), cleaning.
// 2001/03/12 - Minor correction of comments (synchronous with translator.h).
// 2001/02/26 - Update for "new since 1.2.5" version (trBug(), trBugList()).
// 2001/02/15 - trMore() now returns only "..." (ellipsis).
// 2001/01/09 - Update for "new since 1.2.4" version.
// 2000/10/17 - Update for "new since 1.2.2" version.
// 2000/09/11 - Update for "new since 1.2.1" version.
// 2000/09/06 - Reimplementation of trInheritsList().
// 2000/08/31 - ISOToWin() and WinToISO() moved to the base class.
// 2000/08/30 - Macro DECODE replaced by the inline (thanks to Boris Bralo).
// 2000/08/24 - Corrections, updates.
// 2000/08/02 - Updated for 1.2.0
// 2000/07/19 - Updates for "new since 1.1.5"; encoding conversion separated.
// 2000/07/10 - Update to 1.1.5; conditionally decoding to iso-8859-2 for UNIX.
// 2000/06/20 - Prototype: with diacritics; based on ver. 1.1.4 (from scratch).
//
// The first translation from English to Czech was started by
// Vlastimil Havran (1999--2000). The prototype version of Czech strings
// with diacritics was implemented by Petr Prikryl (prikrylp@skil.cz),
// 2000/06/20. Vlastimil agreed that Petr be the new maintainer.

// Todo
// ----
//  - The trReimplementedFromList() should pass the kind of the
//    reimplemented element.  It can be method, typedef or possibly
//    something else.  It is difficult to find the general translation
//    for all kinds in the Czech language.

class TranslatorCzech : public Translator
{
  public:
    // --- Language control methods -------------------

    virtual QCString idLanguage()
    { return "czech"; }

    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[czech]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
        return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Související funkce"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Uvedené funkce nejsou členskými funkcemi.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detailní popis"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentace k členským typům"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentace k členským výčtům"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentace k metodám"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentace k položkám";
      }
      else
      {
        return "Dokumentace k datovým členům";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Seznam všech členů"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Seznam členů třídy"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Zde naleznete úplný seznam členů třídy "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", včetně všech zděděných členů."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result("Generováno automaticky programem Doxygen "
                      "ze zdrojových textů");
      if (s) result += QCString(" projektu ") + s;
      result += ".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "jméno výčtu"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "hodnota výčtu"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definován v"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduly"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Hierarchie tříd"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datové struktury";
      }
      else
      {
        return "Seznam tříd";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Seznam souborů"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datové položky";
      }
      else
      {
        return "Seznam členů tříd";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globální symboly";
      }
      else
      {
        return "Symboly v souborech";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Ostatní stránky"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Příklady"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Hledat"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Zde naleznete seznam, vyjadřující vztah dědičnosti tříd. "
             "Je seřazen přibližně (ale ne úplně) podle abecedy:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="souborů se stručnými popisy:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Následující seznam obsahuje identifikace datových "
                      "struktur a jejich stručné popisy:";
      }
      else
      {
        return "Následující seznam obsahuje především identifikace "
                      "tříd, ale nacházejí se zde i další netriviální prvky, "
                      "jako jsou struktury (struct), unie (union) a rozhraní "
                      "(interface). V seznamu jsou uvedeny jejich stručné "
                      "popisy:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result= "Zde naleznete seznam všech ";
      if (!extractAll)
      {
        result += "dokumentovaných ";
      }

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result += "položek struktur (struct) a unií (union) ";
      }
      else
      {
        result += "členů tříd ";
      }

      result += "s odkazy na ";

      if (extractAll)
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result += "dokumentaci struktur/unií, ke kterým příslušejí:";
        }
        else
        {
          result += "dokumentaci tříd, ke kterým příslušejí:";
        }
      }
      else
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="struktury/unie, ke kterým příslušejí:";
        }
        else
        {
          result+="třídy, ke kterým příslušejí:";
        }
      }

      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="funkcí, proměnných, maker, výčtů a definic typů (typedef) "
                "s odkazy na ";
      }
      else
      {
        result+="symbolů, které jsou definovány na úrovni svých souborů. "
                "Pro každý symbol je uveden odkaz na ";
      }

      if (extractAll)
        result+="soubory, ke kterým příslušejí:";
      else
        result+="dokumentaci:";

      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Zde naleznete seznam všech příkladů:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Následující seznam odkazuje na další stránky projektu:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Zde naleznete seznam všech modulů:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentace"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Rejstřík modulů"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Rejstřík hierarchie tříd"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Rejstřík datových struktur";
      }
      else
      {
        return "Rejstřík tříd";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "Rejstřík souborů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Dokumentace modulů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentace datových struktur";
      }
      else
      {
        return "Dokumentace tříd";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Dokumentace souborů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Dokumentace příkladů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Dokumentace souvisejících stránek"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referenční příručka"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definice maker"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototypy"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definice typů"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Výčty"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funkce"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Proměnné"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Hodnoty výčtu"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Dokumentace k definicím maker"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentace prototypů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentace definic typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentace výčtových typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Dokumentace funkcí"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Dokumentace proměnných"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datové struktry";
      }
      else
      {
        return "Třídy";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result("Generováno ");
      result += date;
      if (projName)
          result += QCString(" pro projekt ") + projName;
      result += " programem";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return " -- autor ";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return QCString("Diagram dědičnosti pro třídu ") + clName;
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Pouze pro vnitřní použití."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Pozor"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Verze"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Návratová hodnota"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Viz také"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametry"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Výjimky"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generováno programem"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Seznam prostorů jmen"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="prostorů jmen se stručným popisem:";
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
    { return "Dokumentace k friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result("Dokumentace ");
      if (isTemplate) result += "šablony ";
      switch(compType)
      {
        case ClassDef::Class:      result += "třídy "; break;
        case ClassDef::Struct:     result += "struktury "; break;
        case ClassDef::Union:      result += "unie "; break;
        case ClassDef::Interface:  result += "rozhraní "; break;
        case ClassDef::Protocol:   result += "protokolu "; break;
        case ClassDef::Category:   result += "kategorie "; break;
        case ClassDef::Exception:  result += "výjimky "; break;
      }
      result += clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result("Dokumentace souboru ");
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result("Dokumentace prostoru jmen ");
      result+=namespaceName;
      return result;
    }

    /*
     *  these are for the member sections of a class, struct or union
     */
    virtual QCString trPublicMembers()
    { return "Veřejné metody"; }
    virtual QCString trPublicSlots()
    { return "Veřejné sloty"; }
    virtual QCString trSignals()
    { return "Signály"; }
    virtual QCString trStaticPublicMembers()
    { return "Statické veřejné metody"; }
    virtual QCString trProtectedMembers()
    { return "Chráněné metody"; }
    virtual QCString trProtectedSlots()
    { return "Chráněné sloty"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statické chráněné metody"; }
    virtual QCString trPrivateMembers()
    { return "Privátní metody"; }
    virtual QCString trPrivateSlots()
    { return "Privátní sloty"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statické privátní metody"; }

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
            result+=" a ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      QCString result("Dědí z ");
      result += (numEntries == 1) ? "bázové třídy " : "bázových tříd ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      QCString result("Zděděna ");
      result += (numEntries == 1) ? "třídou " : "třídami ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      QCString result("Reimplementuje stejnojmenný prvek z ");
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      QCString result("Reimplementováno v ");
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Symboly v prostorech jmen"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="symbolů, které jsou definovány ve svých prostorech jmen. "
              "U každého je uveden odkaz na ";
      if (extractAll)
        result+="dokumentaci příslušného prostoru jmen:";
      else
        result+="příslušný prostor jmen:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Rejstřík prostorů jmen"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Dokumentace prostorů jmen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Prostory jmen"; }

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
      QCString result=(QCString)"Dokumentace pro ";
      switch(compType)
      {
        case ClassDef::Class:      result+="tuto třídu"; break;
        case ClassDef::Struct:     result+="tuto strukturu (struct)"; break;
        case ClassDef::Union:      result+="tuto unii (union)"; break;
        case ClassDef::Interface:  result+="toto rozhraní"; break;
        case ClassDef::Protocol:   result+="tento protokol "; break;
        case ClassDef::Category:   result+="tuto kategorii "; break;
        case ClassDef::Exception:  result+="tuto výjimku"; break;
      }
      result+=" byla generována z ";
      if (single) result+="následujícího souboru:";
      else        result+="následujících souborů:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Rejstřík tříd"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Vracené hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hlavní stránka"; }

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
      return "Definice je uvedena na řádku @0 v souboru @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definice v souboru @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Zastaralé";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagram tříd pro "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graf závislostí na vkládaných souborech "
                    "pro "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Dokumentace konstruktoru a destruktoru";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Zobrazit zdrojový text tohoto souboru.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Zobrazit dokumentaci tohoto souboru.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondition";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondition";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Initializer:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "zdrojový text";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafické zobrazení hierarchie tříd";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Zobrazit grafickou podobu hierarchie tříd";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Zobrazit textovou podobu hierarchie tříd";
    }
    virtual QCString trPageIndex()
    {
      return "Rejstřík stránek";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Poznámka";
    }
    virtual QCString trPublicTypes()
    {
      return "Veřejné typy";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datové položky";
      }
      else
      {
        return "Veřejné atributy";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statické veřejné atributy";
    }
    virtual QCString trProtectedTypes()
    {
      return "Chráněné typy";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Chráněné atributy";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statické chráněné atributy";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privátní typy";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privátní atributy";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statické privátní atributy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Plánované úpravy";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Seznam plánovaných úprav";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Používá se v";
    }
    virtual QCString trRemarks()
    {
      return "Poznámky"; // ??? not checked in a context
    }
    virtual QCString trAttention()
    {
      return "Upozornění"; // ??? not checked in a context
    }
    virtual QCString trInclByDepGraph()
    {
      return "Následující graf ukazuje, které soubory přímo nebo "
             "nepřímo vkládají tento soubor:";
    }
    virtual QCString trSince()
    {
      return "Od"; // ??? not checked in a context
    }

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Vysvětlivky ke grafu";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return
        "Zde naleznete vysvětlení, jak mají být interpretovány grafy, "
        "které byly generovány programem doxygen.<p>\n"
        "Uvažujte následující příklad:\n"
        "\\code\n"
        "/*! Neviditelná třída, která se v grafu nezobrazuje, protože "
        "došlo k ořezání grafu. */\n"
        "class Invisible { };\n\n"
        "/*! Třída, u které došlo k ořezání grafu. Vztah dědičnosti "
        "je skryt. */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Třída, která není dokumentována komentáři programu doxygen. */\n"
        "class Undocumented { };\n\n"
        "/*! Bázová třída děděná veřejně (public inheritance). */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Šablona třídy. */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Bázová třída, použitá pro chráněné dědění "
        "(protected inheritance). */\n"
        "class ProtectedBase { };\n\n"
        "/*! Bázová třída, využitá pro privátní dědění "
        "(private inheritance). */\n"
        "class PrivateBase { };\n\n"
        "/*! Třída, která je využívána třídou Inherited. */\n"
        "class Used { };\n\n"
        "/*! Odvozená třída, která dědí z více tříd. */\n"
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
        "K výše uvedenému bude vygenerován následující graf:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Bloky (tj. uzly) v uvedeném grafu mají následující význam:\n"
        "<ul>\n"
        "<li>Šedě vyplněný obdélník reprezentuje strukturu nebo třídu, "
            "pro kterou byl graf generován.\n"
        "<li>Obdélník s černým obrysem označuje dokumentovanou "
            "strukturu nebo třídu.\n"
        "<li>Obdélník s šedým obrysem označuje nedokumentovanou "
            "strukturu nebo třídu.\n"
        "<li>Obdélník s červeným obrysem označuje dokumentovanou "
            "strukturu nebo třídu, pro kterou\n"
            "nejsou zobrazeny všechny vztahy dědičnosti nebo obsažení. "
            "Graf je ořezán v případě, kdy jej\n"
            "není možné umístit do vymezeného prostoru.\n"
        "</ul>\n"
        "Šipky (tj. hrany grafu) mají následující význam:\n"
        "<ul>\n"
        "<li>Tmavě modrá šipka se používá pro označení vztahu veřejné "
            "dědičnosti (public) mezi dvěma třídami.\n"
        "<li>Tmavě zelená šipka označuje vztah chráněné dědičnosti "
            "(protected).\n"
        "<li>Tmavě červená šipka označuje vztah privátní dědičnosti "
            "(private).\n"
        "<li>Purpurová šipka kreslená čárkovaně se používá v případě, "
            "kdy je třída obsažena v jiné třídě,\n"
            "nebo kdy je používána jinou třídou. Je označena identifikátorem "
            "jedné nebo více proměných, přes které\n"
            "je třída nebo struktura zpřístupněna.\n"
        "<li>Žlutá šipka kreslená čárkovaně vyjadřuje vztah mezi instancí šablony "
            "a šablonou třídy, na základě které byla\n"
            "instance šablony vytvořena. V popisu šipky jsou uvedeny  příslušné"
            " parametry šablony.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "vysvětlivky";
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
      return "Seznam testů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Metody DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Vlastnosti";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Dokumentace k vlastnosti";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return "Třídy";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return QCString("Balík ") + name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Seznam balíků";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Zde naleznete seznam balíků se stručným popisem "
                    "(pokud byl uveden):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Balíky";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Hodnota:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Chyba";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Seznam chyb";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1250";
    }

    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "238";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Rejstřík";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tříd" : "tříd"));
      result += singular ? "a" : "y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Soubor" : "soubor"));
      if (!singular)  result+="y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Prostor" : "prostor"));
      if (!singular)  result+="y";
      result+=" jmen";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Skupin" : "skupin"));
      result += singular ? "a" : "y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Stránk" : "stránk"));
      result += singular ? "a" : "y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Člen" : "člen"));
      if (!singular)
          result += "y";
      return result;
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo,
     * kdo to pouziva.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Auto" : "auto"));
      result += (singular) ? "r" : "ři";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Odkazuje se na";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementováno v "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Obsah";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Seznam zastaralých prvků";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Události";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Dokumentace událostí";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Typy v balíku";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Funkce v balíku";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statické funkce v balíku";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Atributy balíku";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statické atributy balíku";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Vše";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Tato funkce volá...";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the index
     *  of each page before the search field.
     */
    virtual QCString trSearchForIndex()
    {
      return "Vyhledat";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Výsledky vyhledávání";
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
        return "Lituji. Vašemu dotazu neodpovídá žádný dokument.";
      }
      else if (numDocuments==1)
      {
        return "Nalezen jediný dokument, který vyhovuje vašemu dotazu.";
      }
      else
      {
        return "Nalezeno <b>$num</b> dokumentů, které vyhovují vašemu "
                      "dotazu. Nejlépe odpovídající dokumenty jsou zobrazeny "
                      "jako první.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Nalezená slova:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return QCString("Zdrojový soubor ") + filename;
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////


    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Hierarchie adresářů"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dokumentace k adresářům"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Adresáře"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    {
        return "Následující hierarchie adresářů je zhruba, "
                      "ale ne úplně, řazena podle abecedy:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    {
        QCString result = "Reference k adresáři ";
        result += dirName;
        return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
        QCString result((first_capital ? "Adresář" : "adresář"));
        if ( ! singular)
            result += "e";
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
       return "Jde o přetíženou (overloaded) metodu, "
              "která má usnadnit používání. Od výše uvedené metody se liší "
              "pouze jinak zadávanými argumenty.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCallerGraph()
    {
      return "Tuto funkci volají...";
    }


    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    {
      return "Dokumentace výčtových hodnot";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Dokumentace členských funkcí/podprogramů"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Seznam datových typů"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Datová pole"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Datové typy se stručnými popisy:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {

      QCString result="Následuje seznam všech ";
      if (!extractAll)
      {
        result+="dokumentovaných ";
      }
      result+="složek datových typů";
      result+=" s odkazy na ";
      if (!extractAll)
      {
         result+="dokumentaci datové struktury pro každou složku:";
      }
      else
      {
          result+="příslušné datové typy:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Rejstřík datových typů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Dokumentace k datovým typům"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funkce/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Dokumentace funkce/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datové typy"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Seznam modulů"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="modulů se stručnými popisy:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result("Dokumentace ");
      if (isTemplate) result += "šablony ";
      switch(compType)
      {
        case ClassDef::Class:      result += "třídy "; break;
        case ClassDef::Struct:     result += "typu "; break;
        case ClassDef::Union:      result += "unie "; break;
        case ClassDef::Interface:  result += "rozhraní "; break;
        case ClassDef::Protocol:   result += "protokolu "; break;
        case ClassDef::Category:   result += "kategorie "; break;
        case ClassDef::Exception:  result += "výjimky "; break;
      }
      result += clName;
      return result;

    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Dokumentace modulu ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Části modulu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="částí modulů s odkazy ";
      if (extractAll)
      {
        result+="na dokumentaci modulu pro danou část:";
      }
      else
      {
        result+="na moduly, ke kterým část patří:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Rejstřík modulů"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="y";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentace ";
      switch(compType)
      {
        case ClassDef::Class:      result+="k tomuto modulu"; break;
        case ClassDef::Struct:     result+="k tomuto typu"; break;
        case ClassDef::Union:      result+="k této unii"; break;
        case ClassDef::Interface:  result+="k tomuto rozhraní"; break;
        case ClassDef::Protocol:   result+="k tomuto protokolu"; break;
        case ClassDef::Category:   result+="k této kategorii"; break;
        case ClassDef::Exception:  result+="k této výjimce"; break;
      }
      result+=" byla vygenerována z ";
      if (single) result+="následujícího souboru:";
      else result+="následujících souborů:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Typ" : "typ"));
      if (!singular)  result+="y";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Podprogram" : "podprogram"));
      if (!singular)  result+="y";
      return result;
    }

    /*! C# Type Contraint list */
    virtual QCString trTypeConstraints()
    {
      return "Omezení typů (Type Constraints)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Relace " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Načítám...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globální prostor jmen";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Vyhledávám...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nic se nenašlo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Graf závislosti adresářů pro "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Soubor v "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Vkládá (include) soubor z "+name;
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
      static const char *days[]   = { "po","út","st","čt","pá","so","ne" };
      static const char *months[] = { "led","úno","bře","dub","kvě","čer","čec","srp","zář","říj","lis","pro" };
      QCString sdate;
      sdate.sprintf("%s %d. %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d.%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Odkazy na literaturu"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Graf závislosti na adresářích pro  ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "úroveň detailů"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parametry šablon"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "a "+number+" další(ch)..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Dokumentace pro tento výčet byla generována z ";
      if (single)
          result += "následujícího souboru:";
      else
          result += "následujících souborů:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return "Reference k výčtu "+QCString(name); }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" dědí se z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Další zděděné členy"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "povolit" : "zakázat";
      return opt + " synchronizaci panelů";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Deklarováno v kategorii @1.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Rozšiřuje třídu @1.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Metody třídy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Metody instance";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Dokumentace metody";
    }

    /*! Used as the title of the design overview picture created for the
     *  VHDL output.
     */
    virtual QCString trDesignOverview()
    {
      return "Návrhové schéma";
    }

//////////////////////////////////////////////////////////////////////////

};

#endif // TRANSLATOR_CZ_H
