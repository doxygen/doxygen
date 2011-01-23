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
 * ----------------------------------------------------------------------------*/
 
// Updates:
// --------
// 2010/06/04 - big leap from 1.2.18 to 1.6.3+
//
// Slovak translation started by Stanislav Kudlac (skudlac at pobox dot sk).
// He resigned in March 2008 (thanks for the work).  Until a "native Slovak"
// maintainer is found, the TranslatorSlovak is maintained by Petr Prikryl with
// Slovak speaking Kali and Laco Švec.
// ----------------------------------------------------------------------------

#ifndef TRANSLATOR_SK_H
#define TRANSLATOR_SK_H

class TranslatorSlovak : public Translator
{
  public:
    // --- Language control methods -------------------

    virtual QCString idLanguage()
    { return "slovak"; }

    virtual QCString latexLanguageSupportCommand()
    { return "\\usepackage[slovak]{babel}\n"; }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
        return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Súvisiace funkcie"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Uvedené funkcie niesú členskými funkciami.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detailný popis"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentácia k členským typom"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentácia k členským enumeráciám"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentácia k metódam"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentácia k položkám";
      }
      else
      {
        return "Dokumentácia k dátovým členom";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Zoznam všetkých členov."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Zoznam členov triedy"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Tu nájdete úplný zoznam členov triedy "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", vrátane všetkých zdedených členov."; }

    /*! this is put at the author sections at the bottom of man pages.
     *	parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result("Generované automaticky programom Doxygen "
                      "zo zdrojových textov");
      if (s) 
          result+=(QCString)" projektu "+s;
      result+=".";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "meno enumerácie"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "hodnota enumerácie"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definovaný v"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *	compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduly"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Hierarchia tried"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dátové štruktúry";
      }
      else
      {
        return "Zoznam tried";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Zoznam súborov"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dátové položky";
      }
      else
      {
        return "Zoznam členov tried";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globálne symboly";
      }
      else
      {
        return "Symboly v súboroch";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Ostatné stránky"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Príklady"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Hľadať"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Tu nájdete zoznam, vyjadrujúci vzťah dedičnosti tried. "
             "Je zoradený približne (ale nie úplne) podľa abecedy:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
        QCString result("Tu nájdete zoznam všetkých ");
        if (!extractAll) result+="dokumentovaných ";
        result+="súborov so stručnými popismi:";
        return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result= "Tu nájdete zoznam všetkých ";
      if (!extractAll)
      {
        result += "dokumentovaných ";
      }

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Tu nájdete zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Tu nájdete zoznam všetkých príkladov:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Nasledujúci zoznam odkazuje na ďalšie stránky projektu, "
             "ktoré majú charakter usporiadaných zoznamov informácií, "
             "pozbieraných z rôznych miest v zdrojových súboroch:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Tu nájdete zoznam všetkých modulov:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentácia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Register modulov"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Register hierarchie tried"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex()
    { return "Register súborov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Dokumentácia modulov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { return "Dokumentácia tried"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Dokumentácia súborov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Dokumentácia príkladov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Dokumentácia súvisiacich stránok"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referenčná príručka"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of defines
     */
    virtual QCString trDefines()
    { return "Definícia makier"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototypy"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definícia typov"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerácie"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funkcie"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) variables
     */
    virtual QCString trVariables()
    { return "Premenné"; }

    /*! This is used in the documentation of a file as a header before the
     *	list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Hodnoty enumerácií"; }

    /*! This is used in the documentation of a file before the list of
     *	documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Dokumentácia k definíciám makier"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentácia prototypov"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentácia definícií typov"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentácia enumeračných typov"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Dokumentácia funkcií"; }

    /*! This is used in the documentation of a file/namespace before the list
     *	of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Dokumentácia premenných"; }

    /*! This is used in the documentation of a file/namespace/group before
     *	the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Generované "+date;
      if (projName) result+=(QCString)" pre projekt "+projName;
      result+=(QCString)" programom";
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
      return (QCString)"Diagram dedičnosti pre triedu "+clName;
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Iba pre interné použitie."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Pozor"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Verzia"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Dátum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Návratová hodnota"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Viz tiež"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametre"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Výnimky"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generované programom"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Zoznam priestorov mien"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Tu nájdete zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="priestorov mien so stručným popisom:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *	friends of a class
     */
    virtual QCString trFriends()
    { return "Priatelia (friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Dokumentácia k priateľom (friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
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
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result("Dokumentácia súboru ");
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result("Dokumentácia priestoru mien ");
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    virtual QCString trPublicMembers()
    { return "Verejné metódy"; }
    virtual QCString trPublicSlots()
    { return "Verejné sloty"; }
    virtual QCString trSignals()
    { return "Signály"; }
    virtual QCString trStaticPublicMembers()
    { return "Statické verejné metódy"; }
    virtual QCString trProtectedMembers()
    { return "Chránené metódy"; }
    virtual QCString trProtectedSlots()
    { return "Chránené sloty"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statické chránené metódy"; }
    virtual QCString trPrivateMembers()
    { return "Privátne metódy"; }
    virtual QCString trPrivateSlots()
    { return "Privátne sloty"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statické privátne metódy"; }

    /*! this function is used to produce a comma-separated list of items.
     *	use generateMarker(i) to indicate where item i should be put.
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
          else				  // the fore last entry
            result+=" a ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *	if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      QCString result("Dedí od ");
      result += (numEntries == 1) ? "bázovej triedy " : "bázových tried ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in class documentation to produce a list of super classes,
     *	if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      QCString result("Zdedená ");
      result += (numEntries == 1) ? "triedou " : "triedami ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *	members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      QCString result("Reimplementuje ");
      result += (numEntries == 1) ? "metódu triedy " : "metódy tried ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! used in member documentation blocks to produce a list of
     *	all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      QCString result("Reimplementované ");
      result += (numEntries == 1) ? "triedou " : "triedami ";
      result += trWriteList(numEntries)+".";
      return result;
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Symboly v priestoroch mien"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Register priestorov mien"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *	the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Dokumentácia priestorov mien"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *	namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Priestory mien"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *	followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentácia pre ";
      switch(compType)
      {
        case ClassDef::Class:	   result+="túto triedu"; break;
        case ClassDef::Struct:	   result+="túto štruktúru (struct)"; break;
        case ClassDef::Union:	   result+="tento union"; break;
        case ClassDef::Interface:  result+="toto rozhranie"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategória"; break;
        case ClassDef::Exception:  result+="túto výnimku"; break;
      }
      result+=" bola generovaná z ";
      if (single) result+="nasledujúceho súboru:";
      else		  result+="nasledujúcich súborov:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Register tried"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Návratové hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hlavná stránka"; }

    /*! This is used in references to page that are put in the LaTeX
     *	documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definícia je uvedená na riadku @0 v súbore @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definícia v súbore @0.";
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
      return (QCString)"Diagram tried pre "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graf závislostí na vkladaných súboroch "
                    "pre "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Dokumentácia konštruktoru a deštruktoru";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Zobraziť zdrojový text tohoto súboru.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Zobraziť dokumentáciu tohoto súboru.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Prepodmienka";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postpodmienka";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Inicializátor:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "zdrojový text";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafické zobrazenie hierarchie tried";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Zobraziť grafickú podobu hierarchie tried";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Zobraziť textovú podobu hierarchie tried";
    }
    virtual QCString trPageIndex()
    {
      return "Register stránok";
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
      return "Verejné typy";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dátové položky";
      }
      else
      {
        return "Verejné atribúty";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statické verejné atribúty";
    }
    virtual QCString trProtectedTypes()
    {
      return "Chránené typy";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Chránené atribúty";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statické chránené atribúty";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privátne typy";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privátne atribúty";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statické privátne atribúty";
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
      return "Zoznam plánovaných úprav";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Používa sa v";
    }
    virtual QCString trRemarks()
    {
      return "Poznámky";
    }
    virtual QCString trAttention()
    {
      return "Upozornenie";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Nasledujúci graf ukazuje, ktoré súbory priamo alebo "
             "nepriamo vkladajú tento súbor:";
    }
    virtual QCString trSince()
    {
      return "Od";
    }

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Vysvetlivky ku grafu";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "vysvetlivky";
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
      return "Zoznam testov";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Metódy DCOP";
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
      return "Dokumentácia k vlastnosti";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return "Triedy";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Balík "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Zoznam balíkov";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Tu nájdete zoznam balíkov so stručným popisom "
             "(pokiaľ bol uvedený):";
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
      return "Zoznam chýb";
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
      return "3";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tried" : "tried"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Súbor" : "súbor"));
      if (!singular)  result+="y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
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
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Skupin" : "skupin"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Stránk" : "stránk"));
      result+=(singular ? "a" : "y");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Člen" : "člen"));
      if (!singular)  result+="y";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *	be followed by a single name or by a list of names
     *	of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
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
    virtual QCString trAuthor(bool first_capital, bool singular)
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
    virtual QCString trReferences()
    {
      return "Odkazuje sa na";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *	members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementuje " + trWriteList(numEntries) + ".";
    }

    /*! used in member documentation blocks to produce a list of
     *	all members that implement this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementované v " + trWriteList(numEntries) + ".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *	of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Obsah";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *	flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Zastarané metódy";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Udalosti";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Dokumentácia udalostí";
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
      return "Funkcie v balíku";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Statické funkcie v balíku";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Atribúty balíku";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Statické atribúty balíku";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Všetko";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Táto funkcia volá...";
    }
   
//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the index 
     *  of each page before the search field. 
     */
    virtual QCString trSearchForIndex()
    {
      return "Vyhľadať";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Nájdené slová:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return QCString("Zdrojový súbor ") + filename;
    }
    
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////


    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Hierarchia adresárov"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dokumentácia k adresárom"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Adresáre"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { 
        return "Následujúca hierarchia adresárov je zhruba, "
                      "ale nie úplne, zoradená podľa abecedy:"; 
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { 
        QCString result = "Referencia k adresáru "; 
        result += dirName; 
        return result; 
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
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
    virtual QCString trOverloadText()
    {
       return "Ide o preťaženú (overloaded) metódu, "
              "ktorá má uľahčiť používanie. Od vyššie uvedenej metódy sa odlišuje "
              "iba inak zadávanými argumentami.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCallerGraph()
    { return "Túto funkciu volajú..."; }


    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentácia enumeračných hodnôt"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Dokumentácia členských funkcií/podprogramov"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Zoznam dátových typov"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Dátové polia"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Dátové typy so stručnými popismi:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Register dátových typov"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Dokumentácia k dátovým typom"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funkcie/podprogramy"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Dokumentácia funkcie/podprogramu"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Dátové typy"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Zoznam modulov"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Nasleduje zoznam všetkých ";
      if (!extractAll) result+="dokumentovaných ";
      result+="modulov so stručnými popismi:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
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
      }
      result += clName;
      return result;

    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Dokumentácia modulu ";
      result += namespaceName;        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Časti modulu"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Register modulov"; }
    
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
      QCString result=(QCString)"Dokumentácia ";
      switch(compType)
      {
        case ClassDef::Class:      result+="k tomuto modulu"; break;
        case ClassDef::Struct:     result+="k tomuto typu"; break;
        case ClassDef::Union:      result+="k tejto únii"; break;
        case ClassDef::Interface:  result+="k tomuto rozhraniu"; break;
        case ClassDef::Protocol:   result+="k tomuto protokolu"; break;
        case ClassDef::Category:   result+="k tejto kategórii"; break;
        case ClassDef::Exception:  result+="k tejto výnimke"; break;
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
      return "Obmedzenie typov (Type Constraints)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Relácia " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Načítam...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globálny priestor mien";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Vyhľadávam...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nič sa nenašlo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Graf závislosti adresárov pre "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Súbor v "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Vkladá (include) súbor z "+name;
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
      static const char *days[]   = { "po","ut","st","št","pi","so","ne" };
      static const char *months[] = { "jan","feb","mar","apr","máj","jún","júl","aug","sep","okt","nov","dec" };
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

};

#endif // TRANSLATOR_SK_H
