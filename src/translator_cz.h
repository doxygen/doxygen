/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
// 2022/08/30 - Updated for "new since 1.9.6" by: <petyovsky@vut.cz>
//              Changed translation: `trMemberFunctionDocumentation()`.
//              Added new translation: `trFlowchart()`.
// 2022/08/25 - Updated for "new since 1.9.4" by: <petyovsky@vut.cz>
//              removed all implicit conversion from DString to const char *,
//              fixed issues: #7434, #8404, #9192,
//              fixed typo and spacing in: `trLegendDocs()`,
//              fixed plural in: `trTemplateParameters()`,
//              better translation considering the context: `trMemberTypedefDocumentation()`,
//                `trMemberEnumerationDocumentation()`, `trMemberFunctionDocumentation()`,
//                `trMemberDataDocumentation()`, `trDefineDocumentation()`, `trDirDocumentation()`,
//                `trTypeDocumentation()`.
//              updated translation in context of non OO languages: `trHierarchicalIndex()`
//                `trGotoGraphicalHierarchy()`, `trGotoTextualHierarchy()`, `trCompoundMembersFortran()`.
//              updated translation in: `trNoMatches()`, trPackageAttribs(), trStaticPackageAttribs().
//              fixed wrong translation in: `trCompoundReferenceFortran()` of `case ClassDef::Class:`.
//              unified translation string in: `trGeneratedFromFilesFortran` according to `trGeneratedFromFiles()`.
//              fixed wrong wording in: `trCompoundReferenceSlice` of `case ClassDef::Interface:` when `isLocal==true`.
//              minor spacing and comments changes for better convergence to the reference file: `translator_en.h`.
//              all unclear or fuzzy czech translations marked with ???.
// 2021/08/31 - Updated for "new since 1.9.3" by: <petyovsky@vut.cz>.
// 2021/01/22 - Updated for "new since 1.9.2" by: <petyovsky@vut.cz>.
// 2013/04/11 - Updates for "new since 1.8.4".
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

/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorCzech : public TranslatorAdapter_1_9_6
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
    { return "czech"; }

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
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[czech]{babel}\n"
             "\\usepackage{regexpatch}\n"
             "\\makeatletter\n"
             "% Change the `-` delimiter to an active character\n"
             "\\xpatchparametertext\\@@@cmidrule{-}{\\cA-}{}{}\n"
             "\\xpatchparametertext\\@cline{-}{\\cA-}{}{}\n"
             "\\makeatother\n";
    }

    DString trISOLang() override
    {
      return "cs";
    }

    DString getLanguageString() override
    {
      return "0x405 Czech";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Související funkce"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Uvedené funkce nejsou členskými funkcemi.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Detailní popis"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Podrobnosti"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Dokumentace členských typů"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Dokumentace členských výčtů"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Dokumentace členských funkcí/procedur/procesů";
      }
      else
      {
        return "Dokumentace členských funkcí";
      }
    }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentace položek";
      }
      else
      {
        return "Dokumentace datových členů";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Seznam všech členů"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Seznam členů třídy"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Zde naleznete úplný seznam členů třídy"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", včetně všech zděděných členů."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Vygenerováno automaticky programem Doxygen "
                      "ze zdrojových textů";
      if (!s.empty()) result+=" projektu "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "jméno výčtu"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "hodnota výčtu"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "definován v"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Moduly"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Hierarchie tříd"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datové struktury";
      }
      else
      {
        return "Seznam tříd";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Seznam souborů"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datové položky";
      }
      else
      {
        return "Seznam členů tříd";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globální symboly";
      }
      else
      {
        return "Symboly v souborech";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Ostatní stránky"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Příklady"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Hledat"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Zde je hierarchický seznam všech entit:";
      }
      else
      {
        return "Zde naleznete seznam, vyjadřující vztah dědičnosti tříd. "
               "Je seřazen přibližně (ale ne úplně) podle abecedy:";
      }
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="souborů se stručnými popisy:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Následující seznam obsahuje identifikace datových "
               "struktur a jejich stručné popisy:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Následující seznam obsahuje identifikace tříd. "
               "V seznamu jsou uvedeny jejich stručné popisy:";
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
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Zde naleznete seznam všech ";
      if (!extractAll)
      {
        result+="dokumentovaných ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="položek struktur (struct) a unií (union)";
      }
      else
      {
        result+="členů tříd";
      }
      result+=" s odkazy na ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktury/unie, ke kterým příslušejí:";
        }
        else
        {
          result+="třídy, ke kterým příslušejí:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="dokumentaci struktur/unií, ke kterým příslušejí:";
        }
        else
        {
          result+="dokumentaci tříd, ke kterým příslušejí:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcí, proměnných, maker, výčtů a definic typů (typedef) "
                "s odkazy";
      }
      else
      {
        result+="symbolů, které jsou definovány na úrovni svých souborů. "
                "Pro každý symbol je uveden odkaz";
      }
      result+=" na ";
      if (extractAll)
        result+="soubory, ke kterým příslušejí:";
      else
        result+="dokumentaci:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Zde naleznete seznam všech příkladů:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Následující seznam odkazuje na další stránky projektu:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Zde naleznete seznam všech modulů:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokumentace"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Rejstřík modulů"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Rejstřík hierarchie"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    DString trFileIndex() override
    { return "Rejstřík souborů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Dokumentace modulů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentace datových struktur";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Dokumentace tříd";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Dokumentace souborů"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Referenční příručka"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Definice maker"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Definice typů"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Výčty"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funkce"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Proměnné"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Hodnoty výčtu"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Dokumentace definic maker"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Dokumentace definic typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Dokumentace výčtových typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Dokumentace funkcí"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Dokumentace proměnných"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datové struktury";
      }
      else
      {
        return "Třídy";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Vygenerováno dne: "+date;
      if (!projName.empty()) result +=", pro projekt: "+projName;
      result+=", programem";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Diagram dědičnosti pro třídu "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Pozor"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Verze"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Návratová hodnota"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Viz také"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametry"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Výjimky"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Vygenerováno programem"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Seznam jmenných prostorů"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="jmenných prostorů se stručným popisem:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Přátelé třídy"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Dokumentace přátel a souvisejících funkcí třídy"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Dokumentace ";
      if (isTemplate) result += "šablony ";
      switch(compType)
      {
        case ClassDef::Class:      result+="třídy "; break;
        case ClassDef::Struct:     result+="struktury "; break;
        case ClassDef::Union:      result+="unie "; break;
        case ClassDef::Interface:  result+="rozhraní "; break;
        case ClassDef::Protocol:   result+="protokolu "; break;
        case ClassDef::Category:   result+="kategorie "; break;
        case ClassDef::Exception:  result+="výjimky "; break;
        default: break;
      }
      result += clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result="Dokumentace souboru ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result="Dokumentace jmenného prostoru ";
      result+=namespaceName;
      return result;
    }

    /*
     *  these are for the member sections of a class, struct or union
     */
    DString trPublicMembers() override
    { return "Veřejné metody"; }
    DString trPublicSlots() override
    { return "Veřejné sloty"; }
    DString trSignals() override
    { return "Signály"; }
    DString trStaticPublicMembers() override
    { return "Statické veřejné metody"; }
    DString trProtectedMembers() override
    { return "Chráněné metody"; }
    DString trProtectedSlots() override
    { return "Chráněné sloty"; }
    DString trStaticProtectedMembers() override
    { return "Statické chráněné metody"; }
    DString trPrivateMembers() override
    { return "Privátní metody"; }
    DString trPrivateSlots() override
    { return "Privátní sloty"; }
    DString trStaticPrivateMembers() override
    { return "Statické privátní metody"; }

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
            result+=" a ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      DString result="Dědí z ";
      result += (numEntries == 1) ? "bázové třídy " : "bázových tříd ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      DString result="Zděděna ";
      result += (numEntries == 1) ? "třídou " : "třídami ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      DString result="Reimplementuje stejnojmenný prvek z ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Reimplementováno v "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Symboly ve jmenném prostoru"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="symbolů, které jsou definovány ve svých jmenných prostorech. "
              "U každého je uveden odkaz na ";
      if (extractAll)
        result+="dokumentaci příslušného jmenného prostoru:";
      else
        result+="příslušný jmenný prostor:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Rejstřík jmenných prostorů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Dokumentace jmenných prostorů"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Jmenné prostory"; }

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
      DString result="Dokumentace pro ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"tuto návrhovou jednotku":"tuto třídu"; break;
        case ClassDef::Struct:     result+="tuto strukturu"; break;
        case ClassDef::Union:      result+="tuto unii"; break;
        case ClassDef::Interface:  result+="toto rozhraní"; break;
        case ClassDef::Protocol:   result+="tento protokol"; break;
        case ClassDef::Category:   result+="tuto kategorii"; break;
        case ClassDef::Exception:  result+="tuto výjimku"; break;
        default: break;
      }
      result+=" byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else        result+="ch souborů:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Vracené hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Hlavní stránka"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Definice je uvedena na řádku @0 v souboru @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Definice v souboru @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Zastaralé";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Diagram pro "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Graf závislostí na vkládaných souborech pro "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Dokumentace konstruktoru a destruktoru";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Zobrazit zdrojový text tohoto souboru.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Zobrazit dokumentaci tohoto souboru.";
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo, kdo to pouziva.*/
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Precondition";    // ???
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Postcondition";    // ???
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariant";    // ???
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Inicializační hodnota:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "zdrojový text";
    }
    DString trGraphicalHierarchy() override
    {
      return "Grafické zobrazení hierarchie tříd";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Zobrazit grafickou podobu hierarchie";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Zobrazit textovou podobu hierarchie";
    }
    DString trPageIndex() override
    {
      return "Rejstřík stránek";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Poznámka";
    }
    DString trPublicTypes() override
    {
      return "Veřejné typy";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datové položky";
      }
      else
      {
        return "Veřejné atributy";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Statické veřejné atributy";
    }
    DString trProtectedTypes() override
    {
      return "Chráněné typy";
    }
    DString trProtectedAttribs() override
    {
      return "Chráněné atributy";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Statické chráněné atributy";
    }
    DString trPrivateTypes() override
    {
      return "Privátní typy";
    }
    DString trPrivateAttribs() override
    {
      return "Privátní atributy";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Statické privátní atributy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "Plánované úpravy";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Seznam plánovaných úprav";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Používá se v";
    }
    DString trRemarks() override
    {
      return "Poznámky";    // ??? not checked in a context
    }
    DString trAttention() override
    {
      return "Upozornění";    // ??? not checked in a context
    }
    DString trInclByDepGraph() override
    {
      return "Následující graf ukazuje, které soubory přímo nebo "
             "nepřímo vkládají tento soubor:";
    }
    DString trSince() override
    {
      return "Od";    // ??? not checked in a context
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Vysvětlivky ke grafu";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
    {
      return
        "Zde naleznete vysvětlení, jak mají být interpretovány grafy, "
        "které byly vygenerovány programem doxygen.<p>\n"
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
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Bloky (tj. uzly) v uvedeném grafu mají následující význam:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Šedě vyplněný obdélník reprezentuje strukturu nebo třídu, "
            "pro kterou byl graf vygenerován.</li>\n"
        "<li>Obdélník s černým obrysem označuje dokumentovanou "
            "strukturu nebo třídu.</li>\n"
        "<li>Obdélník s šedým obrysem označuje nedokumentovanou "
            "strukturu nebo třídu.</li>\n"
        "<li>Obdélník s červeným obrysem označuje dokumentovanou "
            "strukturu nebo třídu, pro kterou\n"
            "nejsou zobrazeny všechny vztahy dědičnosti nebo obsažení. "
            "Graf je ořezán v případě, kdy jej\n"
            "není možné umístit do vymezeného prostoru.</li>\n"
        "</ul>\n"
        "<p>\n"
        "Šipky (tj. hrany grafu) mají následující význam:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Modrá šipka se používá pro označení vztahu veřejné "
            "dědičnosti (public) mezi dvěma třídami.</li>\n"
        "<li>Tmavě zelená šipka označuje vztah chráněné dědičnosti "
            "(protected).</li>\n"
        "<li>Tmavě červená šipka označuje vztah privátní dědičnosti "
            "(private).</li>\n"
        "<li>Purpurová šipka kreslená čárkovaně se používá v případě, "
            "kdy je třída obsažena v jiné třídě,\n"
            "nebo kdy je používána jinou třídou. Šipka je označena "
            "identifikátorem jedné nebo více proměnných, přes které\n"
            "je třída nebo struktura zpřístupněna.</li>\n"
        "<li>Žlutá šipka kreslená čárkovaně vyjadřuje vztah mezi instancí šablony "
            "a šablonou třídy, na základě které byla\n"
            "instance šablony vytvořena. V popisu šipky jsou uvedeny příslušné "
            "parametry šablony.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "vysvětlivky";
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
      return "Seznam testů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Vlastnosti";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Dokumentace vlastností";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datové struktury";
      }
      else
      {
        return "Třídy";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Balík "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Zde naleznete seznam balíků se stručným popisem (pokud byl uveden):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Balíky";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Hodnota:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Chyba";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Seznam chyb";
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
    DString trRTFansicp() override
    {
      return "1250";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Rejstřík";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tříd", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "soubor", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "jmenn", "é", "ý") +
             createNoun(false, singular, " prostor", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "skupin", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stránk", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "člen", "y");
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo, kdo to pouziva.*/
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "auto", "ři", "r");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Odkazuje se na";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Implementováno v "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Obsah";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Seznam zastaralých prvků";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Události";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Dokumentace událostí";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Typy v balíku";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Funkce v balíku";
    }
    DString trPackageMembers() override
    {
      return "Členy v balíku";    // ??? not checked in a context
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statické funkce v balíku";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Atributy v balíku";    // ??? not checked in a context
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statické atributy v balíku";    // ??? not checked in a context
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Vše";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Tato funkce volá...";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
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
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "Lituji. Vašemu dotazu neodpovídá žádný dokument.";
      }
      else if (numDocuments==1)
      {
        return "Nalezen jeden dokument, který vyhovuje vašemu dotazu.";
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
    DString trSearchMatches() override
    {
      return "Nalezená slova:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return "Zdrojový soubor " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Hierarchie adresářů"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Dokumentace adresářů"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Adresáře"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result="Reference k adresáři "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "adresář", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Jde o přetíženou (overloaded) metodu, "
              "která má usnadnit používání. Od výše uvedené metody se liší "
              "pouze jinak zadávanými argumenty.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Tuto funkci volají...";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Dokumentace výčtových hodnot"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Dokumentace členských funkcí/podprogramů"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Seznam datových typů"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Datové položky"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Datové typy se stručnými popisy:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Následuje seznam všech ";
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
    DString trCompoundIndexFortran() override
    { return "Rejstřík datových typů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Dokumentace datových typů"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funkce/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Dokumentace funkce/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Datové typy"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Seznam modulů"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="modulů se stručnými popisy:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Dokumentace ";
      if (isTemplate) result += "šablony ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulu "; break;
        case ClassDef::Struct:     result+="typu "; break;
        case ClassDef::Union:      result+="unie "; break;
        case ClassDef::Interface:  result+="rozhraní "; break;
        case ClassDef::Protocol:   result+="protokolu "; break;
        case ClassDef::Category:   result+="kategorie "; break;
        case ClassDef::Exception:  result+="výjimky "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result="Dokumentace modulu ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Části modulu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Následuje seznam všech ";
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
    DString trModulesIndex() override
    { return "Rejstřík modulů"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "y");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentace pro ";
      switch(compType)
      {
        case ClassDef::Class:      result+="tento modul"; break;
        case ClassDef::Struct:     result+="tento typ"; break;
        case ClassDef::Union:      result+="tuto unii"; break;
        case ClassDef::Interface:  result+="toto rozhraní"; break;
        case ClassDef::Protocol:   result+="tento protokol"; break;
        case ClassDef::Category:   result+="tuto kategorii"; break;
        case ClassDef::Exception:  result+="tuto výjimku"; break;
        default: break;
      }
      result+=" byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else result+="ch souborů:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "typ", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "podprogram", "y");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Omezení typů (Type Constraints)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return "Relace "+name;    // ??? not checked in a context
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Načítám...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Globální jmenný prostor";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Vyhledávám...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Nebylo nic nalezeno";
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
      return "Soubor v "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Vkládá (include) soubor z "+name;
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
      static const char *days[]   = { "po","út","st","čt","pá","so","ne" };
      static const char *months[] = { "led","úno","bře","dub","kvě","čer","čec","srp","zář","říj","lis","pro" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d. %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d.%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "po", "út", "st", "čt", "pá", "so", "ne" };
      static const char *days_full[]    = { "pondělí", "úterý", "středa", "čtvrtek", "pátek", "sobota", "neděle" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "led", "úno", "bře", "dub", "kvě", "čvn", "čvc", "srp", "zář", "říj", "lis", "pro" };
      static const char *months_full[]  = { "leden", "únor", "březen", "duben", "květen", "červen", "červenec", "srpen", "září", "říjen", "listopad", "prosinec" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "dop.", "odp." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Odkazy na literaturu"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return "Graf závislosti na adresářích pro "+name+":"; }    // ??? not checked in a context

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "úroveň detailů"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Parametry šablony"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "a "+number+" další(ch)..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "Dokumentace pro tento výčet byla vygenerována z následující";
      if (single)
          result += "ho souboru:";
      else
          result += "ch souborů:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return "Reference k výčtu "+name; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" dědí se z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Další zděděné členy"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "povolit" : "zakázat";
      return opt + " synchronizaci panelů";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Deklarováno v kategorii @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Rozšiřuje třídu @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Metody třídy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Metody instance";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Dokumentace metody";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Exportovaná rozhraní"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Začleněné služby"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Konstantní skupiny"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result="Konstantní skupiny z ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result="Popis služby ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result="Popis singletonu ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentace k této službě byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else        result+="ch souborů:";
      return result;
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Dokumentace k tomuto singletonu byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else        result+="ch souborů:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "Hierarchie návrhových jednotek"; }
    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "Seznam návrhových jednotek"; }
    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "Seznam členů návrhových jednotek"; }
    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
        return "Zde naleznete seznam všech členů návrhové jednotky s odkazy na "
            "entity, ke kterým příslušejí:";
    }
    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "Rejstřík návrhových jednotek"; }
    /** VHDL design units */
    DString trDesignUnits() override
    { return "Návrhové jednotky"; }
    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "Funkce/Procedury/Procesy"; }
    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Knihovna";
          else        return "Knihovny";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Balík";
          else        return "Balíky";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Signál";
          else        return "Signály";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Komponenta";
          else        return "Komponenty";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Konstanta";
          else        return "Konstanty";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entita";
          else        return "Entity";
        case VhdlSpecifier::TYPE:
          if (single) return "Typ";
          else        return "Typy";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Subtyp";
          else        return "Subtypy";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Funkce";
          else        return "Funkce";
        case VhdlSpecifier::RECORD:
          if (single) return "Záznam";
          else        return "Záznamy";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedura";
          else        return "Procedury";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Architektura";
          else        return "Architektury";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Atribut";
          else        return "Atributy";
        case VhdlSpecifier::PROCESS:
          if (single) return "Proces";
          else        return "Procesy";
        case VhdlSpecifier::PORT:
          if (single) return "Brána";
          else        return "Brány";
        case VhdlSpecifier::USE:
          if (single) return "Klauzule use";
          else        return "Klauzule use";
        case VhdlSpecifier::GENERIC:
          if (single) return "Generický parametr";
          else        return "Generické parametry";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Tělo balíku";
        case VhdlSpecifier::UNITS:
          return "Fyzikální jednotky";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Sdílená proměnná";
          else        return "Sdílené proměnné";
        case VhdlSpecifier::VFILE:
          if (single) return "Soubor";
          else        return "Soubory";
        case VhdlSpecifier::GROUP:
          if (single) return "Skupina";
          else        return "Skupiny";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Vložená instance";
          else        return "Vložené instance";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Aliasy";
        case VhdlSpecifier::CONFIG:
          if (single) return "Konfigurace";
          else        return "Konfigurace";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Ostatní";
        case VhdlSpecifier::UCF_CONST:
          return "Omezení (constraints)";
        default:
          return "Třída";
      }
    }
    DString trCustomReference(const DString &name) override
    { return "Dokumentace pro "+name; }

    /* Slice */
    DString trConstants() override
    {
        return "Konstanty";
    }
    DString trConstantDocumentation() override
    {
        return "Dokumentace konstant";
    }
    DString trSequences() override
    {
        return "Sekvence";
    }
    DString trSequenceDocumentation() override
    {
        return "Dokumentace sekvence";
    }
    DString trDictionaries() override
    {
        return "Slovníky";
    }
    DString trDictionaryDocumentation() override
    {
        return "Dokumentace slovníku";
    }
    DString trSliceInterfaces() override
    {
        return "Rozhraní";
    }
    DString trInterfaceIndex() override
    {
        return "Rejstřík rozhraní";
    }
    DString trInterfaceList() override
    {
        return "Seznam rozhraní";
    }
    DString trInterfaceListDescription() override
    {
        return "Následující seznam obsahuje jména rozhraní a jejich stručné popisy:";
    }
    DString trInterfaceHierarchy() override
    {
        return "Hierarchie rozhraní";
    }
    DString trInterfaceHierarchyDescription() override
    {
        return "Zde naleznete seznam, vyjadřující vztah dědičnosti rozhraní. Je seřazen přibližně (ale ne úplně) podle abecedy:";
    }
    DString trInterfaceDocumentation() override
    {
        return "Dokumentace rozhraní";
    }
    DString trStructs() override
    {
        return "Struktury";
    }
    DString trStructIndex() override
    {
        return "Rejstřík struktur";
    }
    DString trStructList() override
    {
        return "Seznam struktur";
    }
    DString trStructListDescription() override
    {
        return "Následující seznam obsahuje jména struktur a jejich stručné popisy:";
    }
    DString trStructDocumentation() override
    {
        return "Dokumentace struktur";
    }
    DString trExceptionIndex() override
    {
        return "Rejstřík vyjímek";
    }
    DString trExceptionList() override
    {
        return "Seznam vyjímek";
    }
    DString trExceptionListDescription() override
    {
        return "Následující seznam obsahuje jména výjímek a jejich stručné popisy:";
    }
    DString trExceptionHierarchy() override
    {
        return "Hierarchie vyjímek";
    }
    DString trExceptionHierarchyDescription() override
    {
        return "Zde naleznete seznam, vyjadřující vztah dědičnosti vyjímek. Je seřazen přibližně (ale ne úplně) podle abecedy:";
    }
    DString trExceptionDocumentation() override
    {
        return "Dokumentace vyjímek";
    }
    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result="Dokumentace";
      if (isLocal) result+=" lokální";
      switch(compType)
      {
        case ClassDef::Class:      result+=" třídy "; break;
        case ClassDef::Struct:     result+=" struktury "; break;
        case ClassDef::Union:      result+=" unie "; break;
        case ClassDef::Interface:
                                   if (isLocal) result+="ho";
                                   result+=" rozhraní "; break;
        case ClassDef::Protocol:
                                   if (isLocal) result+="ho";
                                   result+=" protokolu "; break;
        case ClassDef::Category:   result+=" kategorie "; break;
        case ClassDef::Exception:  result+=" vyjímky "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    DString trOperations() override
    {
        return "Operace";
    }
    DString trOperationDocumentation() override
    {
        return "Dokumentace operace";
    }
    DString trDataMembers() override
    {
        return "Datové členy";
    }
    DString trDataMemberDocumentation() override
    {
        return "Dokumentace datových členů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    DString trDesignUnitDocumentation() override
    { return "Dokumentace návrhové jednotky"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    DString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "koncept", "y");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    DString trConceptReference(const DString &conceptName) override
    {
      DString result="Dokumentace konceptu ";
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    DString trConceptList() override
    { return "Seznam konceptů"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    DString trConceptIndex() override
    { return "Rejstřík konceptů"; }

    /*! used as the title of chapter containing all information about concepts. */
    DString trConceptDocumentation() override
    { return "Dokumentace konceptů"; }

    /*! used as an introduction to the concept list */
    DString trConceptListDescription(bool extractAll) override
    {
      DString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="konceptů se stručnými popisy:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    DString trConceptDefinition() override
    {
      return "Definice konceptů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    DString trPackageList() override
    { return "Seznam balíků"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
    { return "Vývojový diagram:"; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     *  Done.
     */

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////

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
        case ClassDef::Struct:     result = "Struktury"; break;
        case ClassDef::Union:      result = "Unie"; break;
        case ClassDef::Interface:  result = "Rozhraní"; break;
        case ClassDef::Protocol:   result = "Protokolu"; break;
        case ClassDef::Category:   result = "Kategorie"; break;
        case ClassDef::Exception:  result = "Výjimky"; break;
        case ClassDef::Service:    result = "Služby"; break;
        case ClassDef::Singleton:  result = "Singletonu"; break;
        default: break;
      }
      return result;
    }
};

#endif // TRANSLATOR_CZ_H
