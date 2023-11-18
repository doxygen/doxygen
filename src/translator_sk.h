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
 * ----------------------------------------------------------------------------*/

// Updates:
// --------
// 2013/05/14 - Updates for "new since 1.8.4".
// 2012/08/02 - Updates for "new since 1.8.2".
// 2012/04/18 - Updates for "new since 1.8.0".
// 2011/07/28 - Updates for "new since 1.7.5".
// 2010/06/04 - big leap from 1.2.18 to 1.6.3+
//
// Slovak translation started by Stanislav Kudlac (skudlac at pobox dot sk).
// He resigned in March 2008 (thanks for the work).  Until a "native Slovak"
// maintainer is found, the TranslatorSlovak is maintained by Petr Prikryl with
// Slovak speaking Kali and Laco Švec.
// ----------------------------------------------------------------------------

#ifndef TRANSLATOR_SK_H
#define TRANSLATOR_SK_H

class TranslatorSlovak : public TranslatorAdapter_1_8_15
{
  public:
    // --- Language control methods -------------------

    virtual QCString idLanguage() override
    { return "slovak"; }

    virtual QCString latexLanguageSupportCommand() override
    { return "\\usepackage[slovak]{babel}\n"
             "\\usepackage{regexpatch}\n"
             "\\makeatletter\n"
             "% Change the `-` delimiter to an active character\n"
             "\\xpatchparametertext\\@@@cmidrule{-}{\\cA-}{}{}\n"
             "\\xpatchparametertext\\@cline{-}{\\cA-}{}{}\n"
             "\\makeatother\n";
    }
    virtual QCString trISOLang() override
    {
      return "sk";
    }
    virtual QCString getLanguageString() override
    {
      return "0x41B Slovak";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Súvisiace funkcie"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Uvedené funkcie niesú členskými funkciami.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Detailný popis"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Podrobnosti"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Dokumentácia k členským typom"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Dokumentácia k členským enumeráciám"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Dokumentácia k metódam"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentácia k položkám";
      }
      else
      {
        return "Dokumentácia k dátovým členom";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Zoznam všetkých členov"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Zoznam členov triedy"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Tu nájdete úplný zoznam členov triedy "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return ", vrátane všetkých zdedených členov."; }

    /*! this is put at the author sections at the bottom of man pages.
     *	parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result("Generované automaticky programom Doxygen "
                      "zo zdrojových textov");
      if (!s.isEmpty())
          result+=" projektu "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "meno enumerácie"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "hodnota enumerácie"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "definovaný v"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *	compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Moduly"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Hierarchia tried"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dátové štruktúry";
      }
      else
      {
        return "Zoznam tried";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Zoznam súborov"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dátové položky";
      }
      else
      {
        return "Zoznam členov tried";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globálne symboly";
      }
      else
      {
        return "Symboly v súboroch";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Ostatné stránky"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Príklady"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Hľadať"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    { return "Tu nájdete zoznam, vyjadrujúci vzťah dedičnosti tried. "
             "Je zoradený približne (ale nie úplne) podľa abecedy:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
        QCString result("Tu nájdete zoznam všetkých ");
        if (!extractAll) result+="dokumentovaných ";
        result+="súborov so stručnými popismi:";
        return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Nasledujúci zoznam obsahuje identifikáciu dátových "
               "štruktúr a ich stručné popisy:";
      }
      else
      {
        return "Nasledujúci zoznam obsahuje predovšetkým identifikáciu "
               "tried, ale nachádzajú sa tu i ďalšie netriviálne prvky, "
               "ako sú štruktúry (struct), uniony (union) a rozhrania "
               "(interface). V zozname sú uvedené ich stručné "
               "popisy:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result= "Tu nájdete zoznam všetkých ";
      if (!extractAll)
      {
        result += "dokumentovaných ";
      }

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result += "položiek štruktúr (struct) a unionov (union) ";
      }
      else
      {
        result += "členov tried ";
      }

      result += "s odkazmi na ";

      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result += "dokumentáciu štruktúr/unionov, ku ktorým prislúchajú:";
        }
        else
        {
          result += "dokumentáciu tried, ku ktorým prislúchajú:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="štruktúry/uniony, ku ktorým prislúchajú:";
        }
        else
        {
          result+="triedy, ku ktorým prislúchajú:";
        }
      }

      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Tu nájdete zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcií, premenných, makier, enumerácií a definícií typov (typedef) "
                "s odkazmi na ";
      }
      else
      {
        result+="symbolov, ktoré sú definované na úrovni svojich súborov. "
                "Pre každý symbol je uvedený odkaz na ";
      }

      if (extractAll)
        result+="súbory, ku ktorým prislúchajú:";
      else
        result+="dokumentáciu:";

      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() override
    { return "Tu nájdete zoznam všetkých príkladov:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Nasledujúci zoznam odkazuje na ďalšie stránky projektu, "
             "ktoré majú charakter usporiadaných zoznamov informácií, "
             "pozbieraných z rôznych miest v zdrojových súboroch:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Tu nájdete zoznam všetkých modulov:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Dokumentácia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Register modulov"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Register hierarchie tried"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Register dátových štruktúr";
      }
      else
      {
        return "Register tried";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "Register súborov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Dokumentácia modulov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    { return "Dokumentácia tried"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Dokumentácia súborov"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Referenčná príručka"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of defines
     */
    virtual QCString trDefines() override
    { return "Definícia makier"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Definícia typov"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Enumerácie"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Funkcie"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Premenné"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Hodnoty enumerácií"; }

    /*! This is used in the documentation of a file before the list of
     *	documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Dokumentácia k definíciám makier"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Dokumentácia definícií typov"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Dokumentácia enumeračných typov"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Dokumentácia funkcií"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Dokumentácia premenných"; }

    /*! This is used in the documentation of a file/namespace/group before
     *	the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dátové štruktúry";
      }
      else
      {
        return "Triedy";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *	the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generované "+date;
      if (!projName.isEmpty()) result+=" pre projekt "+projName;
      result+=" programom";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagram dedičnosti pre triedu "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Pozor"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Verzia"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Dátum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Návratová hodnota"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Viz tiež"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Parametre"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Výnimky"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Generované programom"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Zoznam priestorov mien"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Tu nájdete zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="priestorov mien so stručným popisom:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *	friends of a class
     */
    virtual QCString trFriends() override
    { return "Priatelia (friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Dokumentácia k priateľom (friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result("Dokumentácia ");
      if (isTemplate) result+="šablóny ";
      switch(compType)
      {
        case ClassDef::Class:	   result+="triedy "; break;
        case ClassDef::Struct:	   result+="štruktúry "; break;
        case ClassDef::Union:	   result+="unionu "; break;
        case ClassDef::Interface:  result+="rozhrania "; break;
        case ClassDef::Protocol:   result+="protokol "; break;
        case ClassDef::Category:   result+="kategória "; break;
        case ClassDef::Exception:  result+="výnimky "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result("Dokumentácia súboru ");
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result("Dokumentácia priestoru mien ");
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    virtual QCString trPublicMembers() override
    { return "Verejné metódy"; }
    virtual QCString trPublicSlots() override
    { return "Verejné sloty"; }
    virtual QCString trSignals() override
    { return "Signály"; }
    virtual QCString trStaticPublicMembers() override
    { return "Statické verejné metódy"; }
    virtual QCString trProtectedMembers() override
    { return "Chránené metódy"; }
    virtual QCString trProtectedSlots() override
    { return "Chránené sloty"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Statické chránené metódy"; }
    virtual QCString trPrivateMembers() override
    { return "Privátne metódy"; }
    virtual QCString trPrivateSlots() override
    { return "Privátne sloty"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Statické privátne metódy"; }

    /*! this function is used to produce a comma-separated list of items.
     *	use generateMarker(i) to indicate where item i should be put.
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
          else				  // the fore last entry
            result+=" a ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *	if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) override
    {
      QCString result("Dedí od ");
      result += (numEntries == 1) ? "bázovej triedy " : "bázových tried ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in class documentation to produce a list of super classes,
     *	if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      QCString result("Zdedená ");
      result += (numEntries == 1) ? "triedou " : "triedami ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *	members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      QCString result("Reimplementuje ");
      result += (numEntries == 1) ? "metódu triedy " : "metódy tried ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *	all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      QCString result("Reimplementované ");
      result += (numEntries == 1) ? "triedou " : "triedami ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Symboly v priestoroch mien"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Tu nájdete zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="symbolov, ktoré sú definované vo svojich priestoroch mien. "
              "U každého je uvedený odkaz na ";
      if (extractAll)
        result+="dokumentáciu príslušného priestoru mien:";
      else
        result+="príslušný priestor mien:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *	index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Register priestorov mien"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Dokumentácia priestorov mien"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *	namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Priestory mien"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *	followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentácia pre ";
      switch(compType)
      {
        case ClassDef::Class:	   result+="túto triedu"; break;
        case ClassDef::Struct:	   result+="túto štruktúru (struct)"; break;
        case ClassDef::Union:	   result+="tento union"; break;
        case ClassDef::Interface:  result+="toto rozhranie"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategória"; break;
        case ClassDef::Exception:  result+="túto výnimku"; break;
        default: break;
      }
      result+=" bola generovaná z ";
      if (single) result+="nasledujúceho súboru:";
      else		  result+="nasledujúcich súborov:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Návratové hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Hlavná stránka"; }

    /*! This is used in references to page that are put in the LaTeX
     *	documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Definícia je uvedená na riadku @0 v súbore @1.";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Definícia v súbore @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Zastaralé";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagram tried pre "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graf závislostí na vkladaných súboroch "
                    "pre "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Dokumentácia konštruktoru a deštruktoru";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Zobraziť zdrojový text tohoto súboru.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Zobraziť dokumentáciu tohoto súboru.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Prepodmienka";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Postpodmienka";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Inicializátor:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "zdrojový text";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Grafické zobrazenie hierarchie tried";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Zobraziť grafickú podobu hierarchie tried";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Zobraziť textovú podobu hierarchie tried";
    }
    virtual QCString trPageIndex() override
    {
      return "Register stránok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Poznámka";
    }
    virtual QCString trPublicTypes() override
    {
      return "Verejné typy";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dátové položky";
      }
      else
      {
        return "Verejné atribúty";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Statické verejné atribúty";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Chránené typy";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Chránené atribúty";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Statické chránené atribúty";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Privátne typy";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Privátne atribúty";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Statické privátne atribúty";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo() override
    {
      return "Plánované úpravy";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Zoznam plánovaných úprav";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Používa sa v";
    }
    virtual QCString trRemarks() override
    {
      return "Poznámky";
    }
    virtual QCString trAttention() override
    {
      return "Upozornenie";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Nasledujúci graf ukazuje, ktoré súbory priamo alebo "
             "nepriamo vkladajú tento súbor:";
    }
    virtual QCString trSince() override
    {
      return "Od";
    }

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Vysvetlivky ku grafu";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs() override
    {
      return
        "Tu nájdete vysvetlenie, ako majú byť interpretované grafy, "
        "ktoré boli generované programom doxygen.<p>\n"
        "Uvažujte nasledujúci príklad:\n"
        "\\code\n"
        "/*! Neviditelná trieda, ktorá sa v grafe nezobrazuje, pretože "
        "došlo k orezaniu grafu. */\n"
        "class Invisible { };\n\n"
        "/*! Trieda, u ktorej došlo k orezaniu grafu. Vzťah dedičnosti "
        "je skrytý. */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Trieda, ktorá nieje dokumentovaná komentármi programu doxygen. */\n"
        "class Undocumented { };\n\n"
        "/*! Odvodená trieda s verejným (public) dedením bázovej triedy. */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Šablóna triedy. */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Odvodená trieda s chráneným (protected) dedením bázovej triedy. */\n"
        "class ProtectedBase { };\n\n"
        "/*! Odvodená trieda s privátnym dedením bázovej triedy. */\n"
        "class PrivateBase { };\n\n"
        "/*! Trieda, ktorá je využívaná triedou Inherited. */\n"
        "class Used { };\n\n"
        "/*! Odvodená trieda, ktorá rôznym spôsobom dedí od viacerých bázových "
        "tried. */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "	 Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "K vyššie uvedenému bude vygenerovaný nasledujúci graf:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Bloky (tj. uzly) v uvedenom grafe majú nasledujúci význam:\n"
        "<ul>\n"
        "<li>Čierne vyplnený obdĺžnik reprezentuje štruktúru alebo triedu, "
            "pre ktorú bol graf generovaný.\n"
        "<li>Obdĺžnik s čiernym obrysom označuje dokumentovanú "
            "štruktúru alebo triedu.\n"
        "<li>Obdĺžnik so šedým obrysom označuje nedokumentovanú "
            "štruktúru alebo triedu.\n"
        "<li>Obdĺžnik s červeným obrysom označuje dokumentovanú "
            "štruktúru alebo triedu, pre ktorú\n"
            "niesú zobrazené všetky vzťahy dedičnosti alebo obsiahnutia. "
            "Graf je orezaný v prípade, kedy ho\n"
            "nieje možné umiestniť do vymedzených hraníc.\n"
        "</ul>\n"
        "Šípky (tj. hrany grafu) majú nasledujúcí význam:\n"
        "<ul>\n"
        "<li>Tmavo modrá šípka sa používa pre označenie vzťahu verejnej "
            "dedičnosti medzi dvoma triedami.\n"
        "<li>Tmavo zelená šípka označuje vzťah chránenej dedičnosti "
            "(protected).\n"
        "<li>Tmavo červená šípka označuje vzťah privátnej dedičnosti.\n"
        "<li>Purpurová šípka kreslená čiarkovane sa používa v prípade, "
            "ak je trieda obsiahnutá v inej triede,\n"
            "alebo ak je používaná inou triedou. Je označená identifikátorom "
            "jednej alebo viacerých premenných (objektov), cez ktoré\n"
            "je trieda alebo štruktúra zprístupnena.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend() override
    {
      return "vysvetlivky";
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
      return "Zoznam testov";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Vlastnosti";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Dokumentácia k vlastnosti";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      return "Triedy";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Balík "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Tu nájdete zoznam balíkov so stručným popisom "
             "(pokiaľ bol uvedený):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Balíky";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Hodnota:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Chyba";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Zoznam chýb";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp() override
    {
      return "1250";
    }

    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Tried" : "tried"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Súbor" : "súbor"));
      if (!singular)  result+="y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Priestor" : "priestor"));
      if (!singular)  result+="y";
      result+=" mien";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Skupin" : "skupin"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Stránk" : "stránk"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Člen" : "člen"));
      if (!singular)  result+="y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Globáln" : "globáln"));
      result+=(singular ? "y" : "e");
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *	for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Auto" : "auto"));
      result += (singular) ? "r" : "ri";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Odkazuje sa na";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *	members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Implementuje " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *	all members that implement this member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Implementované v " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *	of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Obsah";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *	flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Zastarané metódy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Udalosti";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Dokumentácia udalostí";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Typy v balíku";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Funkcie v balíku";
    }
    virtual QCString trPackageMembers() override
    {
      return "Členy v balíku";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Statické funkcie v balíku";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Atribúty balíku";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Statické atribúty balíku";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Všetko";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Táto funkcia volá...";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
    {
      return "Výsledky vyhľadávania";
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
        return "Ľutujem. Vášmu dotazu neodpovedá žiadny dokument.";
      }
      else if (numDocuments==1)
      {
        return "Bol nájdený jediný dokument, ktorý vyhovuje vášmu dotazu.";
      }
      else
      {
        return "Bolo nájdených <b>$num</b> dokumentov, ktoré vyhovujú vášmu "
                      "dotazu. Najviac odpovedajúce dokumenty sú ako prvé.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches() override
    {
      return "Nájdené slová:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return QCString("Zdrojový súbor ") + filename;
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////


    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Hierarchia adresárov"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Dokumentácia k adresárom"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Adresáre"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    {
        QCString result = "Referencia k adresáru ";
        result += dirName;
        return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular) override
    {
        QCString result((first_capital ? "Adresár" : "adresár"));
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
    virtual QCString trOverloadText() override
    {
       return "Ide o preťaženú (overloaded) metódu, "
              "ktorá má uľahčiť používanie. Od vyššie uvedenej metódy sa odlišuje "
              "iba inak zadávanými argumentami.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCallerGraph() override
    { return "Túto funkciu volajú..."; }


    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Dokumentácia enumeračných hodnôt"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentácia členských funkcií/podprogramov"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Zoznam dátových typov"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Dátové polia"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Dátové typy so stručnými popismi:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {

      QCString result="Nasleduje zoznam všetkých ";
      if (!extractAll)
      {
        result+="dokumentovaných ";
      }
      result+="zložiek dátových typov";
      result+=" s odkazmi na ";
      if (!extractAll)
      {
         result+="dokumentáciu dátovej štruktúry pre každú zložku:";
      }
      else
      {
          result+="příslušné dátové typy:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran() override
    { return "Register dátových typov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Dokumentácia k dátovým typom"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Funkcie/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Dokumentácia funkcie/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Dátové typy"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Zoznam modulov"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Nasleduje zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="modulov so stručnými popismi:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result("Dokumentácia ");
      if (isTemplate) result += "šablóny ";
      switch(compType)
      {
        case ClassDef::Class:      result += "triedy "; break;
        case ClassDef::Struct:     result += "typu "; break;
        case ClassDef::Union:      result += "únie "; break;
        case ClassDef::Interface:  result += "rozhrania "; break;
        case ClassDef::Protocol:   result += "protokolu "; break;
        case ClassDef::Category:   result += "kategórie "; break;
        case ClassDef::Exception:  result += "výnimky "; break;
        default: break;
      }
      result += clName;
      return result;

    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Dokumentácia modulu ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Časti modulu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Nasleduje zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="častí modulov s odkazmi ";
      if (extractAll)
      {
        result+="na dokumentáciu modulov pre danú časť:";
      }
      else
      {
        result+="na moduly, ku ktorým časť patrí:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex() override
    { return "Register modulov"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="y";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentácia ";
      switch(compType)
      {
        case ClassDef::Class:      result+="k tomuto modulu"; break;
        case ClassDef::Struct:     result+="k tomuto typu"; break;
        case ClassDef::Union:      result+="k tejto únii"; break;
        case ClassDef::Interface:  result+="k tomuto rozhraniu"; break;
        case ClassDef::Protocol:   result+="k tomuto protokolu"; break;
        case ClassDef::Category:   result+="k tejto kategórii"; break;
        case ClassDef::Exception:  result+="k tejto výnimke"; break;
        default: break;
      }
      result+=" bola vygenerovaná z ";
      if (single) result+="nasledujúceho súboru:";
      else result+="nasledujúcich súborov:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Typ" : "typ"));
      if (!singular)  result+="y";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Podprogram" : "podprogram"));
      if (!singular)  result+="y";
      return result;
    }

    /*! C# Type Contraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Obmedzenie typov (Type Constraints)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return "Relácia " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Načítam...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Globálny priestor mien";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Vyhľadávam...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
    {
      return "Nič sa nenašlo";
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
      return "Súbor v "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
    {
      return "Vkladá (include) súbor z "+name;
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
      static const char *days[]   = { "po","ut","st","št","pi","so","ne" };
      static const char *months[] = { "jan","feb","mar","apr","máj","jún","júl","aug","sep","okt","nov","dec" };
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
    virtual QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "po", "ut", "st", "št", "pi", "so", "ne" };
      static const char *days_full[]    = { "pondelok", "utorok", "streda", "štvrtok", "piatok", "sobota", "nedeľa" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    virtual QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "máj", "jún", "júl", "aug", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "január", "február", "marec", "apríl", "máj", "jún", "júl", "august", "september", "október", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
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
    { return "Odkazy na literatúru"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return QCString("Graf závislosti na priečinkoch pre  ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel() override
    { return "úroveň detailov"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters() override
    { return "Parametry šablón"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number) override
    {
        QCString result("a " + number + " ďaľší");
        if (number.toInt() >= 5)
            result += "ch";
        return result + "...";
    }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumentácia pre tuto enumeráciu bola generovaná z ";
      if (single)
          result += "nasledujúceho súboru:";
      else
          result += "nasledujúcich súborov:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name) override
    { return "Referencia k enumerácii "+QCString(name); }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" dedí sa z "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers() override
    { return "Ďaľšie zdedené členy"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "povoliť" : "zakázať";
      return opt + " synchronizáciu panelov";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory() override
    {
      return "Deklarované v kategórii @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass() override
    {
      return "Rozširuje triedu @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods() override
    {
      return "Metódy triedy";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods() override
    {
      return "Metódy inštancie";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation() override
    {
      return "Dokumentácia metódy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces() override
    { return "Exportované rozhrania"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices() override
    { return "Začlenené služby"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups() override
    { return "Konštantné skupiny"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result="Konštantné skupiny z ";
      result += namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName) override
    {
      QCString result="Popis služby ";
      result += sName;
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName) override
    {
      QCString result="Popis singletonu ";
      result += sName;
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentácia k tejto službe bola vygenerovaná ";
      if (single) result+="z nasledujúceho súboru:";
      else        result+="z nasledujúcich súborov:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentácia k tomuto singletonu bola vygenerovaná ";
      if (single) result+="z nasledujúceho súboru:";
      else        result+="z nasledujúcich súborov:";
      return result;
    }

};

#endif // TRANSLATOR_SK_H
