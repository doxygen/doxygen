/******************************************************************************
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[lithuanian]{babel}\n";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Susiję Funkcijos"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "Atkreipkite dėmesį, čią ne metodai"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Smulkus aprašymas"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Tipo Aprašymo Dokumentacija"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Išvardinimo Dokumentacija"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Metodų Dokumentacija"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Lauko aprašymas";
      }
      else
      {
        return "Atributų Dokumentacija";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Daugiau..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Visų narių sąrašas"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Narių sąrašas"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Tai galutinis narių sąrašas "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", įtraukiant visus paveldėtus narius."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatiškai sugeneruota Doxygen įrankiu";
      if (s) result+=(QCString)" "+s;
      result+=" iš programos kodo.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "išvardinimo vardas"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "išvardinimo reikšmė"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "apibrėžta"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduliai"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klasių hierarchija"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų Struktūros";
      }
      else
      {
        return "Klasės";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Failai"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų Laukai";
      }
      else
      {
        return "Klasių Nariai";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globalūs Nariai";
      }
      else
      {
        return "Globalūs Nariai";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Susiję Puslapiai"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Pavyzdžiai"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Paieška"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Šis paveldėjimo sąrašas yra beveik surikiuotas "
             "abėcėlės tvarka:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Visų ";
      if (!extractAll) result+="dokumentuotų ";
      result+="failų sąrašas su trumpais aprašymais:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų struktūros su trumpais aprašymais:";
      }
      else
      {
        return "Klasės, struktūros, "
               "sąjungos ir sąsajos su trumpais aprašymais:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Visų ";
      if (!extractAll)
      {
        result+="dokumentuotų ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Sąrašas visų ";
      if (!extractAll) result+="dokumentuotų ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Visų pavyzdžių sąrašas:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Visų susijusių dokumentacijos puslapių sąrašas:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Visų modulių sąrašas:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modulio Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarchijos Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex()
    { return "Failo Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modulio Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų Struktūros Dokumentacija";
      }
      else
      {
        return "Klasės Dokumentacija";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Failo Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Pavyzdžio Dokumentacija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Puslapio Dokumentacija"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Informacinis Vadovas"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Apibrėžimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Tipų apibrėžimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Išvardinimai"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funkcijos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Kintamieji"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Išvardinimų reikšmės"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Apibrėžimų Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Tipų apibrėžimų Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Išvardinimo Tipo Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funkcijos Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Kintamojo Dokumentacija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Sugeneruota "+date;/*FIXME*/
      if (projName) result+=(QCString)" "+projName;/*FIXME*/
      result+=(QCString)" ";/*FIXME*/
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Paveldimumo diagrama "+clName+":"; /*FIXME*/
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Tiktai vidiniam naudojimui."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Įspėjimas"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versija"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Gražina"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Taip pat žiūrėti"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametrai"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Išimtys"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Sugeneruota"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Varų Srities Sąrašas"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Sąrašas visų ";
      if (!extractAll) result+="dokumentuotų ";
      result+="vardų sričių su trumpais aprašymais:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Draugai"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Draugiškų Ir Susijusių Funkcijų Dokumentacija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
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
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Failo Nuoroda";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Vardų Srities Nuoroda";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Vieši Metodai"; }
    virtual QCString trPublicSlots()
    { return "Vieši Slotai"; } /*FIXME*/
    virtual QCString trSignals()
    { return "Signalai"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiniai Vieši Metodai"; }
    virtual QCString trProtectedMembers()
    { return "Apsaugoti Metodai"; }
    virtual QCString trProtectedSlots()
    { return "Apsaugoti Slotai"; }/*FIXME*/
    virtual QCString trStaticProtectedMembers()
    { return "Statiniai Apsaugoti Metodai"; }
    virtual QCString trPrivateMembers()
    { return "Privatatūs Metodai"; }
    virtual QCString trPrivateSlots()
    { return "Privatūs Slotai"; }/*FIXME*/
    virtual QCString trStaticPrivateMembers()
    { return "Statiniai Privatūs Metodai"; }

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
            result+=", ir ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Paveldi "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Paveldėta "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Perkrauna metodą iš "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Metodas perkraunamas "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Vardų Srities Nariai"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Sąrašas visų ";
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
    virtual QCString trNamespaceIndex()
    { return "Vardų Srities Indeksas"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Vardų Srities Dokumentacija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Vardų Sritys"; }

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
      QCString result=(QCString)"Dokumentacija ";
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
    virtual QCString trReturnValues()
    { return "Gražinamos reikšmės"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Pagrindinis Puslapis"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "psl."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Apibrėžimas failo @1 eilutėje @0.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Apibrėžimas faile @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Smerktina";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Bendradarbiavimo diagrama "+clName+":";/*FIXME*/
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Įtraukimo priklausomybių diagrama "+fName+":";/*FIXME*/
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Konstruktoriaus ir Destruktoriaus Dokumentacija";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Eiti į šio failo programos kodą.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Eiti į šio failo dokumentaciją.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Išakstinė sąlyga";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondition";/*FIXME*/
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Pastovus";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Pradinė reikšmė:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "tekstas";/*FIXME*/
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafinė Klasės Hierarchija";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Eiti į grafinę klasės hierarchiją";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Eiti į tekstinę klasės hierarchiją";
    }
    virtual QCString trPageIndex()
    {
      return "Puslapio Indeksas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Pastaba";
    }
    virtual QCString trPublicTypes()
    {
      return "Vieši Tipai";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų Laukai";
      }
      else
      {
        return "Vieši Atributai";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statiniai Vieši Atributai";
    }
    virtual QCString trProtectedTypes()
    {
      return "Apsaugoti Tipai";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Apsaugoti Atributai";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statiniai Apsaugoti Atributai";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privatūs Tipai";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privatūs Atributai";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statiniai Privatūs Atributai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Daryti";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Tolimesni Darbai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Naudojamas";
    }
    virtual QCString trRemarks()
    {
      return "Pastabos";
    }
    virtual QCString trAttention()
    {
      return "Attention";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Šis grafas rodo, kuris failas tiesiogiai ar "
             "netiesiogiai įtraukia šį failą:";
    }
    virtual QCString trSince()
    {
      return "Nuo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Grafo Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a grey border denotes an undocumented struct or class.\n"
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
    virtual QCString trLegend()
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Testas";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Testo Sąrašas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Savybės";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Savybės Dokumentacija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Duomenų Struktūros";
      }
      else
      {
        return "Klasės";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paketas "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Paketo Sąrašas";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Paketai su trumpu aprašymu:";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paketai";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Reikšmė:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Klaida";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1257";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "186";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Indeksas";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Klasė" : "klasė"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Faila" : "faila"));
      if (!singular)  result+="i";
      else  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Vardų srit" : "vardų srit"));
      if (!singular)  result+="ys";
      else  result+="is";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Grupė" : "grupė"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Puslapi" : "puslapi"));
      if (!singular)  result+="ai";
      else  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Nar" : "nar"));
      if (!singular)  result+="iai";
      else  result+="ys";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="ūs";
      else  result+="us";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Autori" : "autori"));
      if (!singular)  result+="ai";
      else  result+="us";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Ryšiai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Realizuoja "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Realizuota "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Turinys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Smerktinumų Sąrašas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Įvykiai";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Įvykio Dokumentacija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Paketo Tipai";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Paketo Funkcijos";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statinės Paketo Funkcijos";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Paketo Atributai";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statiniai Paketo Atributai";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Visi";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Funkcijos kvietimo grafas:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Atitikmenys:";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " išeities tekstas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Direktorijų hierarchija"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Direktorijų dokumentacija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Direktorijos"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Ši direktorjų strūktūra grubiai surikiuota abėcėlės tvarka:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Directorijos aprašas"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Direktorij" : "direktorij"));
      if (singular) result+="a"; else result+="os";
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
       return "Perkraunamas metodas sukurtas patogumui. "
              "Jis skiriasi nuo aukščiau minėto tik argumetais.";
    }


};

#endif
