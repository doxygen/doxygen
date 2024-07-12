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

#ifndef TRANSLATOR_CZ_H
#define TRANSLATOR_CZ_H

// Updates:
// --------
// 2022/08/30 - Updated for "new since 1.9.6" by: <petyovsky@vut.cz>
//              Changed translation: `trMemberFunctionDocumentation()`.
//              Added new translation: `trFlowchart()`.
// 2022/08/25 - Updated for "new since 1.9.4" by: <petyovsky@vut.cz>
//              removed all implicit conversion from QCString to const char *,
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
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
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

    QCString trISOLang() override
    {
      return "cs";
    }

    QCString getLanguageString() override
    {
      return "0x405 Czech";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Související funkce"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Uvedené funkce nejsou členskými funkcemi.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Detailní popis"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Podrobnosti"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentace členských typů"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentace členských výčtů"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
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
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Seznam všech členů"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Seznam členů třídy"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Zde naleznete úplný seznam členů třídy"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", včetně všech zděděných členů."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Vygenerováno automaticky programem Doxygen "
                      "ze zdrojových textů";
      if (!s.isEmpty()) result+=" projektu "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "jméno výčtu"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "hodnota výčtu"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definován v"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduly"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hierarchie tříd"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Seznam souborů"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Ostatní stránky"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Příklady"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Hledat"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
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
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="souborů se stručnými popisy:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Zde naleznete seznam všech ";
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Zde naleznete seznam všech ";
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
    QCString trExamplesDescription() override
    { return "Zde naleznete seznam všech příkladů:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Následující seznam odkazuje na další stránky projektu:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Zde naleznete seznam všech modulů:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentace"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Rejstřík modulů"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Rejstřík hierarchie"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Rejstřík souborů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Dokumentace modulů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Dokumentace souborů"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Referenční příručka"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definice maker"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definice typů"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Výčty"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funkce"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Proměnné"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Hodnoty výčtu"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Dokumentace definic maker"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentace definic typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentace výčtových typů"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Dokumentace funkcí"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Dokumentace proměnných"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Vygenerováno dne: "+date;
      if (!projName.isEmpty()) result +=", pro projekt: "+projName;
      result+=", programem";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagram dědičnosti pro třídu "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Pozor"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Verze"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Návratová hodnota"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Viz také"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametry"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Výjimky"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Vygenerováno programem"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Seznam jmenných prostorů"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="jmenných prostorů se stručným popisem:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Přátelé třídy"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentace přátel a souvisejících funkcí třídy"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Dokumentace ";
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Dokumentace souboru ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Dokumentace jmenného prostoru ";
      result+=namespaceName;
      return result;
    }

    /*
     *  these are for the member sections of a class, struct or union
     */
    QCString trPublicMembers() override
    { return "Veřejné metody"; }
    QCString trPublicSlots() override
    { return "Veřejné sloty"; }
    QCString trSignals() override
    { return "Signály"; }
    QCString trStaticPublicMembers() override
    { return "Statické veřejné metody"; }
    QCString trProtectedMembers() override
    { return "Chráněné metody"; }
    QCString trProtectedSlots() override
    { return "Chráněné sloty"; }
    QCString trStaticProtectedMembers() override
    { return "Statické chráněné metody"; }
    QCString trPrivateMembers() override
    { return "Privátní metody"; }
    QCString trPrivateSlots() override
    { return "Privátní sloty"; }
    QCString trStaticPrivateMembers() override
    { return "Statické privátní metody"; }

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
            result+=" a ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      QCString result="Dědí z ";
      result += (numEntries == 1) ? "bázové třídy " : "bázových tříd ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      QCString result="Zděděna ";
      result += (numEntries == 1) ? "třídou " : "třídami ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      QCString result="Reimplementuje stejnojmenný prvek z ";
      result += trWriteList(numEntries) + ".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementováno v "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Symboly ve jmenném prostoru"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Zde naleznete seznam všech ";
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
    QCString trNamespaceIndex() override
    { return "Rejstřík jmenných prostorů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Dokumentace jmenných prostorů"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Jmenné prostory"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="Dokumentace pro ";
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
    QCString trReturnValues() override
    { return "Vracené hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Hlavní stránka"; }

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
      return "Definice je uvedena na řádku @0 v souboru @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definice v souboru @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Zastaralé";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagram pro "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graf závislostí na vkládaných souborech pro "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentace konstruktoru a destruktoru";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Zobrazit zdrojový text tohoto souboru.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Zobrazit dokumentaci tohoto souboru.";
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo, kdo to pouziva.*/
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondition";    // ???
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondition";    // ???
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";    // ???
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Inicializační hodnota:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "zdrojový text";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafické zobrazení hierarchie tříd";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Zobrazit grafickou podobu hierarchie";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Zobrazit textovou podobu hierarchie";
    }
    QCString trPageIndex() override
    {
      return "Rejstřík stránek";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Poznámka";
    }
    QCString trPublicTypes() override
    {
      return "Veřejné typy";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Statické veřejné atributy";
    }
    QCString trProtectedTypes() override
    {
      return "Chráněné typy";
    }
    QCString trProtectedAttribs() override
    {
      return "Chráněné atributy";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statické chráněné atributy";
    }
    QCString trPrivateTypes() override
    {
      return "Privátní typy";
    }
    QCString trPrivateAttribs() override
    {
      return "Privátní atributy";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statické privátní atributy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Plánované úpravy";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Seznam plánovaných úprav";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Používá se v";
    }
    QCString trRemarks() override
    {
      return "Poznámky";    // ??? not checked in a context
    }
    QCString trAttention() override
    {
      return "Upozornění";    // ??? not checked in a context
    }
    QCString trInclByDepGraph() override
    {
      return "Následující graf ukazuje, které soubory přímo nebo "
             "nepřímo vkládají tento soubor:";
    }
    QCString trSince() override
    {
      return "Od";    // ??? not checked in a context
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Vysvětlivky ke grafu";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "vysvětlivky";
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
      return "Seznam testů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Vlastnosti";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Dokumentace vlastností";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Balík "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Zde naleznete seznam balíků se stručným popisem (pokud byl uveden):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Balíky";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Hodnota:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Chyba";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1250";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Rejstřík";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tříd", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "soubor", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "jmenn", "é", "ý") +
             createNoun(false, singular, " prostor", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "skupin", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stránk", "y", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "člen", "y");
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo, kdo to pouziva.*/
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "auto", "ři", "r");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Odkazuje se na";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementováno v "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Obsah";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Seznam zastaralých prvků";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Události";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Dokumentace událostí";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Typy v balíku";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funkce v balíku";
    }
    QCString trPackageMembers() override
    {
      return "Členy v balíku";    // ??? not checked in a context
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statické funkce v balíku";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributy v balíku";    // ??? not checked in a context
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statické atributy v balíku";    // ??? not checked in a context
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Vše";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Tato funkce volá...";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Nalezená slova:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Zdrojový soubor " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hierarchie adresářů"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Dokumentace adresářů"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Adresáře"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Reference k adresáři "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "adresář", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Jde o přetíženou (overloaded) metodu, "
              "která má usnadnit používání. Od výše uvedené metody se liší "
              "pouze jinak zadávanými argumenty.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Tuto funkci volají...";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentace výčtových hodnot"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentace členských funkcí/podprogramů"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Seznam datových typů"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Datové položky"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Datové typy se stručnými popisy:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Rejstřík datových typů"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Dokumentace datových typů"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funkce/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Dokumentace funkce/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Datové typy"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Seznam modulů"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="modulů se stručnými popisy:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Dokumentace ";
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Dokumentace modulu ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Části modulu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Rejstřík modulů"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "y");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentace pro ";
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "typ", "y");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "podprogram", "y");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Omezení typů (Type Constraints)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relace "+name;    // ??? not checked in a context
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Načítám...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Globální jmenný prostor";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Vyhledávám...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Soubor v "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "po","út","st","čt","pá","so","ne" };
      static const char *months[] = { "led","úno","bře","dub","kvě","čer","čec","srp","zář","říj","lis","pro" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d. %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d.%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "po", "út", "st", "čt", "pá", "so", "ne" };
      static const char *days_full[]    = { "pondělí", "úterý", "středa", "čtvrtek", "pátek", "sobota", "neděle" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "led", "úno", "bře", "dub", "kvě", "čvn", "čvc", "srp", "zář", "říj", "lis", "pro" };
      static const char *months_full[]  = { "leden", "únor", "březen", "duben", "květen", "červen", "červenec", "srpen", "září", "říjen", "listopad", "prosinec" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "dop.", "odp." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Odkazy na literaturu"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return "Graf závislosti na adresářích pro "+name+":"; }    // ??? not checked in a context

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "úroveň detailů"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parametry šablony"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "a "+number+" další(ch)..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumentace pro tento výčet byla vygenerována z následující";
      if (single)
          result += "ho souboru:";
      else
          result += "ch souborů:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return "Reference k výčtu "+name; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return members+" dědí se z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Další zděděné členy"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "povolit" : "zakázat";
      return opt + " synchronizaci panelů";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Deklarováno v kategorii @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Rozšiřuje třídu @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Metody třídy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Metody instance";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Dokumentace metody";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Exportovaná rozhraní"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Začleněné služby"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Konstantní skupiny"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result="Konstantní skupiny z ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result="Popis služby ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result="Popis singletonu ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentace k této službě byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else        result+="ch souborů:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentace k tomuto singletonu byla vygenerována z následující";
      if (single) result+="ho souboru:";
      else        result+="ch souborů:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Hierarchie návrhových jednotek"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Seznam návrhových jednotek"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Seznam členů návrhových jednotek"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Zde naleznete seznam všech členů návrhové jednotky s odkazy na "
            "entity, ke kterým příslušejí:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Rejstřík návrhových jednotek"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Návrhové jednotky"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funkce/Procedury/Procesy"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
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
    QCString trCustomReference(const QCString &name) override
    { return "Dokumentace pro "+name; }

    /* Slice */
    QCString trConstants() override
    {
        return "Konstanty";
    }
    QCString trConstantDocumentation() override
    {
        return "Dokumentace konstant";
    }
    QCString trSequences() override
    {
        return "Sekvence";
    }
    QCString trSequenceDocumentation() override
    {
        return "Dokumentace sekvence";
    }
    QCString trDictionaries() override
    {
        return "Slovníky";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Dokumentace slovníku";
    }
    QCString trSliceInterfaces() override
    {
        return "Rozhraní";
    }
    QCString trInterfaceIndex() override
    {
        return "Rejstřík rozhraní";
    }
    QCString trInterfaceList() override
    {
        return "Seznam rozhraní";
    }
    QCString trInterfaceListDescription() override
    {
        return "Následující seznam obsahuje jména rozhraní a jejich stručné popisy:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Hierarchie rozhraní";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Zde naleznete seznam, vyjadřující vztah dědičnosti rozhraní. Je seřazen přibližně (ale ne úplně) podle abecedy:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Dokumentace rozhraní";
    }
    QCString trStructs() override
    {
        return "Struktury";
    }
    QCString trStructIndex() override
    {
        return "Rejstřík struktur";
    }
    QCString trStructList() override
    {
        return "Seznam struktur";
    }
    QCString trStructListDescription() override
    {
        return "Následující seznam obsahuje jména struktur a jejich stručné popisy:";
    }
    QCString trStructDocumentation() override
    {
        return "Dokumentace struktur";
    }
    QCString trExceptionIndex() override
    {
        return "Rejstřík vyjímek";
    }
    QCString trExceptionList() override
    {
        return "Seznam vyjímek";
    }
    QCString trExceptionListDescription() override
    {
        return "Následující seznam obsahuje jména výjímek a jejich stručné popisy:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Hierarchie vyjímek";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Zde naleznete seznam, vyjadřující vztah dědičnosti vyjímek. Je seřazen přibližně (ale ne úplně) podle abecedy:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Dokumentace vyjímek";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result="Dokumentace";
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
    QCString trOperations() override
    {
        return "Operace";
    }
    QCString trOperationDocumentation() override
    {
        return "Dokumentace operace";
    }
    QCString trDataMembers() override
    {
        return "Datové členy";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Dokumentace datových členů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Dokumentace návrhové jednotky"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "koncept", "y");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result="Dokumentace konceptu ";
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Seznam konceptů"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Rejstřík konceptů"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Dokumentace konceptů"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Následuje seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="konceptů se stručnými popisy:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Definice konceptů";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Seznam balíků"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
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
    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) trType(true,true);
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
