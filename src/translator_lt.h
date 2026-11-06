/******************************************************************************
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

/* Translation from English to Lithanian by Tomas Simonaitis, Mindaugas Radzius and Aidas Berukstis
 *  (haden@homelan.lt/mindaugasradzius@takas.lt/aidasber@takas.lt)
 *  We tried to keep meaning close to original,
 *  if you have any suggestions drop us an email
 *  -------------------------------------------
 *  Project start                  :13.May.2k4
 *  Last Doxygen version covered   : 1.4.3
 *
 *  Revisions:
 *  Updated to 1.3.9 25.Oct.2k4
 *
*/

#ifndef TRANSLATOR_LT_H
#define TRANSLATOR_LT_H


class TranslatorLithuanian : public TranslatorAdapter_1_4_6
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
    { return "lithuanian"; }

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
             "\\usepackage[lithuanian]{babel}\n";
    }

    DString trISOLang() override
    {
      return "lt";
    }
    DString getLanguageString() override
    {
      return "0x427 Lithuanian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Susiję Funkcijos"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "Atkreipkite dėmesį, čią ne metodai"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Smulkus aprašymas"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Išsamiau"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Tipo Aprašymo Dokumentacija"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Išvardinimo Dokumentacija"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "Metodų Dokumentacija"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Lauko aprašymas";
      }
      else
      {
        return "Atributų Dokumentacija";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Daugiau..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Visų narių sąrašas"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Narių sąrašas"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Tai galutinis narių sąrašas"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", įtraukiant visus paveldėtus narius."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Automatiškai sugeneruota Doxygen įrankiu";
      if (!s.empty()) result+=" "+s;
      result+=" iš programos kodo.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "išvardinimo vardas"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "išvardinimo reikšmė"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "apibrėžta"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Moduliai"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Klasių hierarchija"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Struktūros";
      }
      else
      {
        return "Klasės";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Failai"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Laukai";
      }
      else
      {
        return "Klasių Nariai";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globalūs Nariai";
      }
      else
      {
        return "Failų Nariai";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Susiję Puslapiai"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Pavyzdžiai"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Paieška"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Šis paveldėjimo sąrašas yra beveik surikiuotas "
             "abėcėlės tvarka:";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Visų ";
      if (!extractAll) result+="dokumentuotų ";
      result+="failų sąrašas su trumpais aprašymais:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų struktūros su trumpais aprašymais:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Klasės su trumpais aprašymais:";
      }
      else
      {
        return "Klasės, struktūros, "
               "sąjungos ir sąsajos su trumpais aprašymais:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Visų ";
      if (!extractAll)
      {
        result+="dokumentuotų ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struktūrų ir sąjungų laukų sąrašas";
      }
      else
      {
        result+="klasių nariai";
      }
      result+=" su ryšiais į ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūrų/sąjungų kiekvieno lauko dokumentaciją:";
        }
        else
        {
          result+="klasės dokumentacija kiekvienam nariui:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūros/sąjungos, kurios priklauso:";
        }
        else
        {
          result+="klasės, kurios priklauso:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Sąrašas visų ";
      if (!extractAll) result+="dokumentuotų ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcijų, kintamųjų, apibrėžimų, išvardinimų, ir tipų apibrėžimų";
      }
      else
      {
        result+="globalių narių";
      }
      result+=" su ryšiais į ";
      if (extractAll)
        result+="failus, kuriems jie priklauso:";
      else
        result+="dokumentacija:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Visų pavyzdžių sąrašas:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Visų susijusių dokumentacijos puslapių sąrašas:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Visų modulių sąrašas:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Modulio Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Hierarchijos Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Struktūros Indeksas";
      }
      else
      {
        return "Klasės Indeksas";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Failo Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Modulio Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Struktūros Dokumentacija";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klasės Dokumentacija";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Failo Dokumentacija"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Informacinis Vadovas"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Apibrėžimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Tipų apibrėžimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Išvardinimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funkcijos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Kintamieji"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Išvardinimų reikšmės"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Apibrėžimų Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Tipų apibrėžimų Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Išvardinimo Tipo Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Funkcijos Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Kintamojo Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų struktųros";
      }
      else
      {
        return "Klasės";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Sugeneruota "+date;/*FIXME*/
      if (!projName.empty()) result+=" "+projName;/*FIXME*/
      result+=" ";/*FIXME*/
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Paveldimumo diagrama "+clName+":"; /*FIXME*/
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Įspėjimas"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Versija"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Gražina"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Taip pat žiūrėti"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametrai"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Išimtys"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Sugeneruota"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Varų Srities Sąrašas"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Sąrašas visų ";
      if (!extractAll) result+="dokumentuotų ";
      result+="vardų sričių su trumpais aprašymais:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Draugai"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Draugiškų Ir Susijusių Funkcijų Dokumentacija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klasė"; break;
        case ClassDef::Struct:     result+=" Struktūra"; break;
        case ClassDef::Union:      result+=" Sąjunga"; break;
        case ClassDef::Interface:  result+=" Sąsaja"; break;
        case ClassDef::Protocol:   result+=" Protokolas"; break;
        case ClassDef::Category:   result+=" Kategorija"; break;
        case ClassDef::Exception:  result+=" Išimtis"; break;
        default: break;
      }
      if (isTemplate) result+=" Šablonas";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName;
      result+=" Failo Nuoroda";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Vardų Srities Nuoroda";
      return result;
    }

    DString trPublicMembers() override
    { return "Vieši Metodai"; }
    DString trPublicSlots() override
    { return "Vieši Slotai"; } /*FIXME*/
    DString trSignals() override
    { return "Signalai"; }
    DString trStaticPublicMembers() override
    { return "Statiniai Vieši Metodai"; }
    DString trProtectedMembers() override
    { return "Apsaugoti Metodai"; }
    DString trProtectedSlots() override
    { return "Apsaugoti Slotai"; }/*FIXME*/
    DString trStaticProtectedMembers() override
    { return "Statiniai Apsaugoti Metodai"; }
    DString trPrivateMembers() override
    { return "Privatatūs Metodai"; }
    DString trPrivateSlots() override
    { return "Privatūs Slotai"; }/*FIXME*/
    DString trStaticPrivateMembers() override
    { return "Statiniai Privatūs Metodai"; }

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
            result+=", ir ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Paveldi "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Paveldėta "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Perkrauna metodą iš "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Metodas perkraunamas "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Vardų Srities Nariai"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Sąrašas visų ";
      if (!extractAll) result+="dokumentuotų ";
      result+="vardų srities narių su nuorodomis į ";
      if (extractAll)
        result+="vardų srities dokumentaciją kiekvienam nariui:";
      else
        result+="vardų sritis, kurioms jos priklauso:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Vardų Srities Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Vardų Srities Dokumentacija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Vardų Sritys"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="Dokumentacija ";
      switch(compType)
      {
        case ClassDef::Class:      result+="šiai klasei"; break;
        case ClassDef::Struct:     result+="šiai struktūrai"; break;
        case ClassDef::Union:      result+="šiai sąjungai"; break;
        case ClassDef::Interface:  result+="šiai sąsajai"; break;
        case ClassDef::Protocol:   result+="šiam protocolui"; break;
        case ClassDef::Category:   result+="šiai kategorijai"; break;
        case ClassDef::Exception:  result+="šiai išimčiai"; break;
        default: break;
      }
      result+=" sugeneruota iš ";
      if (single) result+="šio failo:"; else result+="šių failų:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Gražinamos reikšmės"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Pagrindinis Puslapis"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "psl."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Apibrėžimas failo @1 eilutėje @0.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Apibrėžimas faile @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Smerktina";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Bendradarbiavimo diagrama "+clName+":";/*FIXME*/
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Įtraukimo priklausomybių diagrama "+fName+":";/*FIXME*/
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Konstruktoriaus ir Destruktoriaus Dokumentacija";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Eiti į šio failo programos kodą.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Eiti į šio failo dokumentaciją.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Išakstinė sąlyga";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Postcondition";/*FIXME*/
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Pastovus";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Pradinė reikšmė:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "tekstas";/*FIXME*/
    }
    DString trGraphicalHierarchy() override
    {
      return "Grafinė Klasės Hierarchija";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Eiti į grafinę klasės hierarchiją";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Eiti į tekstinę klasės hierarchiją";
    }
    DString trPageIndex() override
    {
      return "Puslapio Indeksas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Pastaba";
    }
    DString trPublicTypes() override
    {
      return "Vieši Tipai";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Laukai";
      }
      else
      {
        return "Vieši Atributai";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Statiniai Vieši Atributai";
    }
    DString trProtectedTypes() override
    {
      return "Apsaugoti Tipai";
    }
    DString trProtectedAttribs() override
    {
      return "Apsaugoti Atributai";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Statiniai Apsaugoti Atributai";
    }
    DString trPrivateTypes() override
    {
      return "Privatūs Tipai";
    }
    DString trPrivateAttribs() override
    {
      return "Privatūs Atributai";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Statiniai Privatūs Atributai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "Daryti";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Tolimesni Darbai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Naudojamas";
    }
    DString trRemarks() override
    {
      return "Pastabos";
    }
    DString trAttention() override
    {
      return "Attention";
    }
    DString trInclByDepGraph() override
    {
      return "Šis grafas rodo, kuris failas tiesiogiai ar "
             "netiesiogiai įtraukia šį failą:";
    }
    DString trSince() override
    {
      return "Nuo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Grafo Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
    {
      return
        "Šis puslapis paaiškina, kaip interpretuoti sugeneruotus grafus "
        "su Doxygen įrankiu.<p>\n"
        "Panagrinėkite pavyzdį:\n"
        "\\kodo pradžia\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class that is inherited using protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class that is inherited using private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Class that is used by the Inherited class */\n"
        "class Used { };\n\n"
        "/*! Super class that inherits a number of other classes */\n"
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
        "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "is set to 240 this will result in the following graph:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a gray border denotes an undocumented struct or class.\n"
        "<li>%A box with a red border denotes a documented struct or class for"
        "which not all inheritance/containment relations are shown. %A graph is "
        "truncated if it does not fit within the specified boundaries.\n"
        "</ul>\n"
        "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>%A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.\n"
        "<li>%A dark green arrow is used for protected inheritance.\n"
        "<li>%A dark red arrow is used for private inheritance.\n"
        "<li>%A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible.\n"
        "<li>%A yellow dashed arrow denotes a relation between a template instance and "
        "the template class it was instantiated from. The arrow is labeled with "
        "the template parameters of the instance.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Testas";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Testo Sąrašas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Savybės";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Savybės Dokumentacija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Duomenų Struktūros";
      }
      else
      {
        return "Klasės";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Paketas "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Paketai su trumpu aprašymu:";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Paketai";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Reikšmė:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Klaida";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Klaidų Sąrašas";
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
      return "1257";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "186";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Indeksas";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klasė", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "faila", "i", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "vardų srit", "ys", "is");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupė", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "puslapi", "ai", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "nar", "iai", "ys");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "ūs", "us");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autori", "ai", "us");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Ryšiai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Realizuoja "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Realizuota "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Turinys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Smerktinumų Sąrašas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Įvykiai";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Įvykio Dokumentacija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Paketo Tipai";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Paketo Funkcijos";
    }
    DString trPackageMembers() override
    {
      return "Paketo Nariai";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statinės Paketo Funkcijos";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Paketo Atributai";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statiniai Paketo Atributai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Visi";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Funkcijos kvietimo grafas:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Paieškos Rezultatai";
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
        return "Atsiprašome, jokių dokumentų nerasta pagal Jūsų užklausą.";
      }
      else if (numDocuments==1)
      {
        return "Surasta <b>1</b> dokumentas, atitinkantis Jūsų užklausą.";
      }
      else
      {
        return "Surasta <b>$num</b> dokumentų, atitinkančių Jūsų užklausą. "
               "Pirmiausiai rodomi labiausiai tenkinantys užklausą.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Atitikmenys:";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " išeities tekstas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Direktorijų hierarchija"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Direktorijų dokumentacija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Direktorijos"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" Directorijos aprašas"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "direktorij", "os", "a");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Perkraunamas metodas sukurtas patogumui. "
              "Jis skiriasi nuo aukščiau minėto tik argumetais.";
    }


    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "pr", "an", "tr", "kt", "pn", "št", "sk" };
      static const char *days_full[]    = { "pirmadienis", "antradienis", "trečiadienis", "ketvirtadienis", "penktadienis", "šeštadienis", "sekmadienis" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "saus.", "vas.", "kov.", "bal.", "geg.", "birž.", "liep.", "rugp.", "rugs.", "spal.", "lapkr.", "gruod." };
      static const char *months_full[]  = { "sausis", "vasaris", "kovas", "balandis", "gegužė", "birželis", "liepa", "rugpjūtis", "rugsėjis", "spalis", "lapkritis", "gruodis" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "priešpiet", "popiet" };
      return dayPeriod[period?1:0];
    }
};

#endif
