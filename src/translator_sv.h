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

/*
==================================================================================
Svensk översättning av:
Samuel Hägglund      <sahag96@ite.mh.se>
Xet Erixon           <xet@xeqt.com>
Mikael Hallin        <mikaelhallin@yahoo.se>           2003-07-28
Björn Palmqvist      <bjorn@aidium.se>                 2014-02-01
Magnus Österlund     <magnus.osterlund@capgemini.com>  2016-09-12
Björn Palmqvist      <bjorn@aidium.se>                 2020-01-08
==================================================================================
Uppdateringar.
1999/04/29
*  Omskrivningar av en hel del ordval, t.ex.
   ENG                  INNAN          EFTER
   compound             inhängnad      sammansatt
   structs              structs        strukter
   unions               unions         unioner

   osv...

*  Alla översättnings-strängar returnerar i alla fall en något så när vettig
   förklaring...

1999/05/27
*  Det verkade som vi glömt en del mellanslag i vissa strängar vilket resulterade
   i att en del ord blev ihopskrivna.

*  Bytt en del ordval igen...
   ENG                       INNAN          EFTER
   reference manual          Uppslagsbok    referensmanual

*  Skrivit ihop en del ord som innan hade bindestreck
*  En del nya översättningar är tillagda.
*  Gamla översättningar borttagna

===================================================================================
Problem!
   Slot: nån hygglig svensk översättning???

   Skicka gärna synpunkter.
===================================================================================
1999/09/01
*  Det verkar som om "namnrymd" är en hyggligt vedertagen svensk översättning
   av "namespace", så jag kör med det från och med nu.
*  "interface" heter numera "gränssnitt"

2003/07/28
*  Jag har updaterat översättningen efter ett par års träda..
Problem!
   Deprecated: nån hygglig svensk översättning???

   Skicka gärna synpunkter.

2015/01/09
* Uppdaterat den till senaste versionen 1.8.9.1

2015/09/12
* Fixat lite särksirvningar och inkonsekvenser

2020/01/08
* Uppdaterat den till senaste språkversionen 1.8.15

Bytte ut Deprecated från Föråldrad till Obsolet

VHDL översättningarna är kanske inte perfekta, då jag endast använt de en gång tidigare.
Jag lämnade use clause orörd, då jag inte hittade en lämplig översättning för den.

English:
* Updated the language translation to 1.8.15

Changed Deprecated from Föråldrad to Obsolet

The VHDL translations may not perfect, as I only used it once before.
I left use clause untouched as I didn't find a suitable translation for it.

2020/08/14
* Uppdaterat översättningarna till 1.8.19
English:
* Updated the language translation to 1.8.19

2020/08/19
* Uppdaterat översättningarna till 1.9.2
English:
* Updated the language translation to 1.9.2

2022/12/28
* Uppdaterat översättningarna till 1.9.6
English:
* Updated the language translation to 1.9.6

===================================================================================
  Ordlista
===================================================================================
  ENGELSKA          SVENSKA
* Attribute         Attribut
* Category          Kategori
* Class             Klass
* Compound          Sammansatt
* Deprecated        Obsolet
* Directory         Katalog
* Dictionary        Uppslagsverk       // Frågan om de är de som menas i de fallet
* Entity            Entitet
* Enum              Enum
* Enumeration       Egenuppräknande
* Event             Händelse
* Exception         Undantag
* Field             Fält
* Friend            Vän
* Function          Funktion
* Inherited         Ärvd
* Interface         Gränssnitt
* Library           Biblotek
* Macro             Makro
* Member            Medlem
* Member Data       Medlemsdata
* Member function   Medlemsfunktion
* Module            Module
* Namespace         Namnrymd
* Package           Paket
* Postcondition     Postcondition
* Precondition      Precondition
* Private           Privat
* Propertie         Egenskap
* Protected         Skyddad
* Protocol          Protokoll
* Public            Publik
* Record            Post            // Ge gärna exempel på bättre översättning
* Service           Tjänst
* Signal            Signal
* Slot              Slot            // Ge gärna exempel på bättre översättning
* Static            Statisk
* Struct            Struktur
* Subprogram        Underprogram
* Subroutine        Subrutin
* Subtype           Undertyp
* Template          Mall
* Typedef           Typdefinition
* Union             Union
* Unit              Enhet           // Lämplig översättning i VHDL kontextet?
* Variable          Variabel
===================================================================================
*/

#ifndef TRANSLATOR_SE_H
#define TRANSLATOR_SE_H

class TranslatorSwedish : public TranslatorAdapter_1_9_6
{
  public:

    QCString idLanguage() override
    { return "swedish"; }

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
      return "\\usepackage[swedish]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "sv";
    }
    QCString getLanguageString() override
    {
      return "0x41D Swedish";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Besläktade funktioner"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Observera att dessa inte är medlemsfunktioner)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Detaljerad beskrivning"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detaljer"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentation av typdefinierade medlemmar"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentation av egenuppräknande medlemmar"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Dokumentation av medlemsfunktioner/-procedurer/-processer";
      }
      else
      {
        return "Dokumentation av medlemsfunktioner";
      }
    }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentation av fält";
      }
      else
      {
        return "Dokumentation av datamedlemmar";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Mer..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista över alla medlemmar"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Medlemslista"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Det här är en fullständig lista över medlemmar för"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return " med alla ärvda medlemmar."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automatiskt skapad av Doxygen";
      if (!s.isEmpty()) result+=" för "+s;
      result+=" från källkoden.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "enum-namn"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "enum-värde"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definierad i"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduler"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Klasshierarki"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasslista";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Fillista"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datafält";
      }
      else
      {
        return "Klassmedlemmar";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globala symboler";
      }
      else
      {
        return "Filmedlemmar";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Relaterade sidor"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Exempel"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Sök"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Denna lista över arv är grovt, men inte helt, "
             "sorterad i alfabetisk ordning:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filer, med en kort beskrivning:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Här följer datastrukturerna med korta beskrivningar:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Här följer klasserna med korta beskrivningar:";
      }
      else
      {
        return "Här följer klasserna, strukterna, unionerna och "
               "gränssnitten med korta beskrivningar:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll)
      {
        result+="dokumenterade ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struktur- och unions-fält";
      }
      else
      {
        result+="klassmedlemmar";
      }
      result+=" med länkar till ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktur/union-dokumentationen för varje fält:";
        }
        else
        {
          result+="klassdokumentationen för varje medlem:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+=" strukturerna/unionerna de tillhör:";
        }
        else
        {
          result+="klasserna de tillhör:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funktioner, variabler, definitioner, enumerationer "
                "och typdefinitioner";
      }
      else
      {
        result+= "filmedlemmar";
      }
      result+= " med länkar till ";
      if (extractAll)
        result+= "filerna de tillhör:";
      else
        result+= "dokumentationen:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Här följer en lista med alla exempel:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Här följer en lista över alla relaterade dokumentationssidor:";}

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Här följer en lista över alla moduler:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Modulindex"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hierarkiskt index"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Index över datastrukturer";
      }
      else
      {
        return "Klassindex";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Filindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Moduldokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentation över datastrukturer";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klassdokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Fildokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Referensmanual"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Makron"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Typdefinitioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Egenuppräknande typer"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funktioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variabler"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Egenuppräknade typers värden"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Dokumentation över makrodefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentation över typdefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentation över egenuppräknande typer"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Dokumentation över funktioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Dokumentation över variabler"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasser";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Skapad "+date;
      if (!projName.isEmpty()) result+=" för "+projName;
      result+=" av";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Klassdiagram för "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Varning"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Returnerar"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Se även"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametrar"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Undantag"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Skapad av"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Namnrymdslista"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namnrymder med en kort beskrivning:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Vänner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation() override
    { return "Dokumenation av vänners och relaterade funktioner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:  result+=" klass"; break;
        case ClassDef::Struct: result+=" strukt"; break;
        case ClassDef::Union:  result+=" union"; break;
        case ClassDef::Interface:  result+=" gränssnitt"; break;
        case ClassDef::Protocol:   result+=" protokoll"; break;
        case ClassDef::Category:   result+=" kategori"; break;
        case ClassDef::Exception:  result+=" undantag"; break;
        default: break;
      }
      if (isTemplate) result+="-mall";
      result+=" referens";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" filreferens";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" namnrymdreferens";
      return result;
    }

    QCString trPublicMembers() override
    { return "Publika medlemsfunktioner"; }
    QCString trPublicSlots() override
    { return "Publika slots"; }
    QCString trSignals() override
    { return "Signaler"; }
    QCString trStaticPublicMembers() override
    { return "Statiska publika medlemmsfunktioner"; }
    QCString trProtectedMembers() override
    { return "Skyddade medlemmsfunktioner"; }
    QCString trProtectedSlots() override
    { return "Skyddade slots"; }
    QCString trStaticProtectedMembers() override
    { return "Statiska skyddade medlemmsfunktioner"; }
    QCString trPrivateMembers() override
    { return "Privata medlemmsfunktioner"; }
    QCString trPrivateSlots() override
    { return "Privata slots"; }
    QCString trStaticPrivateMembers() override
    { return "Statiska privata medlemmsfunktioner"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+=", och ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Ärver "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Ärvd av "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Återskapad från "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Återskapad i "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Namnrymdsmedlemmar"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namnrymdsmedlemmar med länkar till ";
      if (extractAll)
        result+=" namnrymd-dokumentationen för varje medlem:";
      else
        result+="de namnrymder de tillhör:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Namnrymdsindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
      { return "Namnrymd-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Namnrymder"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="Dokumentationen för ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt? "denna designenhets":"denna klass"; break;
        case ClassDef::Struct:     result+="denna strukt"; break;
        case ClassDef::Union:      result+="denna union"; break;
        case ClassDef::Interface:  result+="detta gränssnitt"; break;
        case ClassDef::Protocol:   result+="detta protokoll"; break;
        case ClassDef::Category:   result+="denna kategori"; break;
        case ClassDef::Exception:  result+="detta undantag"; break;
        default: break;
      }
      result+=" var genererad från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Returvärden"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Huvudsida"; }

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
      return "Definition på rad @0 i filen @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definition i filen @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Obsolet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Samarbetsdiagram för "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Include-beroendediagram för "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentation av konstruktorer och destruktorer";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Gå till denna fils källkod.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Gå till denna fils dokumentation.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondition";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondition";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Begynnelsevärde:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "källkod";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafisk klasshierarki";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Gå till den grafiska klasshierarkin";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Gå till den textbaserade klasshierarkin";
    }
    QCString trPageIndex() override
    {
      return "Sidindex";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Notera";
    }
    QCString trPublicTypes() override
    {
      return "Publika typer";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datafält";
      }
      else
      {
        return "Publika attribut";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Statiska publika attribut";
    }
    QCString trProtectedTypes() override
    {
      return "Skyddade typer";
    }
    QCString trProtectedAttribs() override
    {
      return "Skyddade attribut";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statiska skyddade attribut";
    }
    QCString trPrivateTypes() override
    {
      return "Privata typer";
    }
    QCString trPrivateAttribs() override
    {
      return "Privata attribut";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statiska privata attribut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Att-göra";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Att göra-lista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Refererad av";
    }
    QCString trRemarks() override
    {
      return "Lägg märke till";
    }
    QCString trAttention() override
    {
      return "Observera";
    }
    QCString trInclByDepGraph() override
    {
      return "Den här grafen visar vilka filer som direkt eller "
             "indirekt inkluderar denna filen:";
    }
    QCString trSince() override
    {
      return "Sedan";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Grafförklaring";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Den här sidan förklarar hur man ska tolka de grafer som doxygen "
        "genererar.<p>\n"
        "Överväg följande exempel:\n"
        "\\code\n"
        "/*! Osynlig klass på grund av trunkering */\n"
        "class Invisible { };\n\n"
        "/*! Trunkerad klass, ärvningsrelationen är dold */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klass utan doxygen-kommentarer */\n"
        "class Undocumented { };\n\n"
        "/*! Klass som ärvs med publikt arv */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! En template-klass */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klass som ärvs med skyddat arv */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klass som ärvs med privat arv */\n"
        "class PrivateBase { };\n\n"
        "/*! Klass som används av den ärvda klassen */\n"
        "class Used { };\n\n"
        "/*! Super-klass som ärver ett antal andra klasser */\n"
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
        "Detta resulterar i att följande graf genereras:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Rektanglarna i den ovanstående grafen har följande betydelser:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%En fylld grå rektangel representerar den strukt eller klass "
        "som har genererat grafen.</li>\n"
        "<li>%En rektangel med svart kant symboliserar en dokumenterad strukt eller klass.</li>\n"
        "<li>%En rektangel med grå kant symboliserar en odokumenterad strukt eller klass.</li>\n"
        "<li>%En klass med röd kant symboliserar en strukt eller klass där "
        "alla dess relationer inte visas. %En graf trunkeras om den inte får "
        "plats inom de givna gränserna.</li>\n"
        "</ul>\n"
        "<p>\n"
        "Pilarna har följande betydelser:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%En mörkblå pil används för att visualisera en publik arvrelation "
        "mellan två klasser.</li>\n"
        "<li>%En mörkgrön pil används för en skyddad arvsrelation.</li>\n"
        "<li>%En mörkröd pil används för en privat arvsrelation.\n</li>"
        "<li>%En sträckad lila pil används om en klass är innesluten eller "
        "använd av en annan klass. Vid pilen står namnet på den eller de "
        "variabler som klassen pilen pekar på kommer åt.</li>\n"
        "<li>%En sträckad gul pil symboliserar förhållandet mellan en "
        "template-instans och template-klassen, som den instantierades från.\n"
        "Vid pilen står instansens template-parametrar.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "förklaring";
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
      return "Testlista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Egenskaper";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Egenskapsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Klasser";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paket "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Här är en lista över paketen med en kort beskrivning "
             "(om sådan finns):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paket";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Värde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bugg";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Bugglista";
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
     * ANSI_CHARSET              0 (x00)            1252
     * </pre>
     */
    QCString trRTFansicp() override
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klass", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fil", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namnrymd", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupp", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sid", "or", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "medlem", "mar");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "er");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool /*singular*/) override
    {
      return createNoun(first_capital, false, "författare", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referenser";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementerar "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementerad i "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Innehållsförteckning";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista över föråldrade";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Händelser";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Händelsedokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Pakettyper";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Paketfunktioner";
    }
    QCString trPackageMembers() override
    {
      return "Paketmedlemmar";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statiska paketfunktioner";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Paketattribut";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statiska paketattribut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Alla";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Här är anropsdiagrammet för den här funktionen:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Sökresultat";
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
        return "Tyvärr finns det inga dokument som matchar din sökning.";
      }
      else if (numDocuments==1)
      {
        return "Hittade <b>1</b> dokument som matchar din sökning.";
      }
      else
      {
        return "Hittade <b>$num</b> dokument som matchar din sökning. "
               "Visar de bästa träffarna först.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Träffar:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Källkodsfilen " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Katalogstruktur"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Katalogdokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Kataloger"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Katalogreferens"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "katalog", "er");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
      return "Det här är en överlagrad medlemsfunktion "
             "tillhandahållen för bekvämlighet. Den enda som "
             "skiljer sig från ovanstående funktion är vilka "
             "argument den tar emot.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Här är anropsgrafen för denna funktion:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Uppräknings-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentation för memlemsfunktion/subrutin"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Datatyplista"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Datafält"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Här är datatyperna med en kort beskrivning:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll)
      {
        result+="dokumenterade ";
      }
      result+="datatypsmedlemmar";
      result+=" med länkar till ";
      if (!extractAll)
      {
         result+="dokumentation för varje medlem";
      }
      else
      {
         result+="klasserna de hör till:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Datatypsindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Datatypsdokumentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funktioner/subrutiner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Dokumentation för funktion/subrutin"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Datatyper"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Modullista"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="moduler med en kort beskrivning:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Modul"; break;
        case ClassDef::Struct:     result+=" Typ"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Gränssnitt"; break;
        case ClassDef::Protocol:   result+=" Protokoll"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" Undantag"; break;
        default: break;
      }
      if (isTemplate) result+="-mall";
      result+=" referens";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Modulreferens";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Modulmedlemmar"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="documented ";
      result+="modulmedlemmar med länkar till ";
      if (extractAll)
      {
        result+="moduldokumentationen för varje medlem:";
      }
      else
      {
        result+="modulerna de hör till:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Modulindex"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "er");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentationen för ";
      switch(compType)
      {
        case ClassDef::Class:      result+="denna modul"; break;
        case ClassDef::Struct:     result+="denna typ"; break;
        case ClassDef::Union:      result+="denna union"; break;
        case ClassDef::Interface:  result+="detta gränssnitt"; break;
        case ClassDef::Protocol:   result+="detta protokoll"; break;
        case ClassDef::Category:   result+="denna kategori"; break;
        case ClassDef::Exception:  result+="detta undantag"; break;
        default: break;
      }
      result+=" genererades från följade fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "typ", "er");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool /*singular*/) override
    {
      return createNoun(first_capital, false, "underprogram", "");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Typbegränsningar";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Relation";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Laddar...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Global namnrymd";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Söker...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Inga träffar";
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
      return "Fil i "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Inkluderar fil i "+name;
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
      static const char *days[]   = { "Mån","Tis","Ons","Tor","Fre","Lör","Sön" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Maj","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "mån", "tis", "ons", "tor", "fre", "lör", "sön" };
      static const char *days_full[]    = { "måndag", "tisdag", "onsdag", "torsdag", "fredag", "lördag", "söndag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "maj", "jun", "jul", "aug", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januari", "februari", "mars", "april", "maj", "juni", "juli", "augusti", "september", "oktober", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "fm", "em" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Bibliografiska Referenser"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Katalogberoendegraf för ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "detaljnivå"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Mall-parametrar"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "och "+number+" flera..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumentationen för denna enum var genererad från föjlande fil";
      if (!single) result += "er";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Enum-referens"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" ärvd ifrån "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Ytterliga ärvda medlemmar"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "aktivera" : "inaktivera";
      return "klicka för att "+opt+" panelsynkronisering";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Tillhandahålls av kategori @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Utökar klass @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Klassmetoder";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Instansmetoder";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Metoddokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Exporterade interface"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Inkuderade tjänster"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Konstantgrupper"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Referens till konstantgrupp";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Tjänstereferens";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Singleton-referens";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentationen för denna tjänst "
                                "genererades från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentationen för denna singleton "
                                "genererades från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Designenhetshirarki"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Designenhetslista"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Designenhetsmedlemmar"; }
    /** VHDL design unit list description
     * Orginal: Here is a list of all design unit members with links to
     *          the Entities they belong to:
     */
    QCString trDesignUnitListDescription() override
    {
        return "Här är en lista av alla designenhetsmedlemmar med länkar till "
               "entiteterna som de hör till:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Designenhetsindex"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Designenheter"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funktioner/Procedurer/Processer"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          return "Biblotek";
        case VhdlSpecifier::PACKAGE:
          return "Paket";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Signal";
          else        return "Signaler";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Komponent";
          else        return "Komponenter";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Konstant";
          else        return "Konstanter";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entitet";
          else        return "Entiteter";
        case VhdlSpecifier::TYPE:
          if (single) return "Typ";
          else        return "Typer";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Undertyp";
          else        return "Undertyper";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Funktion";
          else        return "Funktioner";
        case VhdlSpecifier::RECORD:
          if (single) return "Post";
          else        return "Poster";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedur";
          else        return "Procedurer";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Arkitektur";
          else        return "Arkitekturer";
        case VhdlSpecifier::ATTRIBUTE:
          return "Attribut";
        case VhdlSpecifier::PROCESS:
          if (single) return "Process";
          else        return "Processer";
        case VhdlSpecifier::PORT:
          if (single) return "Port";
          else        return "Portar";
        case VhdlSpecifier::USE:
          if (single) return "use clause";
          else        return "Use Clauses";
        case VhdlSpecifier::GENERIC:
          if (single) return "Generisk";
          else        return "Generiska";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Paketinehåll";
        case VhdlSpecifier::UNITS:
          return "Enheter";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Delad Variabel";
          else        return "Delade Variabler";
        case VhdlSpecifier::VFILE:
          if (single) return "Fil";
          else        return "Filer";
        case VhdlSpecifier::GROUP:
          if (single) return "Grupp";
          else        return "Grupper";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Instantiation";
          else        return "Instantiations";
        case VhdlSpecifier::ALIAS:
          return "Alias";
        case VhdlSpecifier::CONFIG:
          if (single) return "Konfiguration";
          else        return "Konfigurationer";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Diverse";
        case VhdlSpecifier::UCF_CONST:
          return "Begränsningar";
        default:
          return "Klass";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return QCString(name)+"referens"; }

    /* Slice */
    QCString trConstants() override
    {
        return "Konstanter";
    }
    QCString trConstantDocumentation() override
    {
        return "Konstantdokumentation";
    }
    QCString trSequences() override
    {
        return "Sekvenser";
    }
    QCString trSequenceDocumentation() override
    {
        return "Sekvensdokumentation";
    }
    QCString trDictionaries() override
    {
        return "Uppslagsverk";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Uppslagsverksdokumentation";
    }
    QCString trSliceInterfaces() override
    {
        return "Gränssnitt";
    }
    QCString trInterfaceIndex() override
    {
        return "Gränssnittsindex";
    }
    QCString trInterfaceList() override
    {
        return "Gränssnittslist";
    }
    /** Orginal: Here are the interfaces with brief descriptions: */
    QCString trInterfaceListDescription() override
    {
        return "Här är gränssnitten med en kort beskrivning";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Gränssnittshirarkin";
    }
    /** Orginal: This inheritance list is sorted roughly, but not completely, alphabetically: */
    QCString trInterfaceHierarchyDescription() override
    {
        return "Denna arvslista är grovt sorterad, men inte helt, i alfabetisk ordning:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Gränssnittsdokumentation";
    }
    QCString trStructs() override
    {
        return "Strukturer";
    }
    QCString trStructIndex() override
    {
        return "Strukturindex";
    }
    QCString trStructList() override
    {
        return "Strukturlist";
    }
    /** Orginal: Here are the structs with brief descriptions: */
    QCString trStructListDescription() override
    {
        return "Här är strukturerna med en kort beskrivning:";
    }
    QCString trStructDocumentation() override
    {
        return "Strukturdokumentation";
    }
    QCString trExceptionIndex() override
    {
        return "Undantagsindex";
    }
    QCString trExceptionList() override
    {
        return "Undantagslista";
    }
    /** Orginal: Here are the exceptions with brief descriptions: */
    QCString trExceptionListDescription() override
    {
        return "Här är undantagen med en kort beskrivning:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Undantagshirarki";
    }
    /** Orginal:  This inheritance list is sorted roughly, but not completely, alphabetically: */
    QCString trExceptionHierarchyDescription() override
    {
        return "Denna arvslista är grovt sorterad, men inte helt, i alfabetisk ordning:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Undantagsdokumentation";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result=clName;
      if (isLocal) result+=" Lokal";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Klass"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Unions"; break;
        case ClassDef::Interface:  result+=" Gränssnitts"; break;
        case ClassDef::Protocol:   result+=" Protokoll"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" Undantags"; break;
        default: break;
      }
      result+="referens";
      return result;
    }
    QCString trOperations() override
    {
        return "Operationer";
    }
    QCString trOperationDocumentation() override
    {
        return "Operationsdokumentation";
    }
    QCString trDataMembers() override
    {
        return "Datamedlemmar";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Datamedlemsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Designenhetsdokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool /* singular */) override
    {
      return createNoun(first_capital, false, "koncept", "");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result=conceptName;
      result+=" Konceptreferens";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Konceptlista"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Konceptindex"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Konceptdokumentation"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Här är listan över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="koncept med en kort beskrivning:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Konceptdefinition";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Paketlista"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    QCString trFlowchart() override
    { return "Flödesdiagram:"; }

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
        case ClassDef::Struct:     result="Strukt"; break;
        case ClassDef::Union:      result="Union"; break;
        case ClassDef::Interface:  result="Gränssnitt"; break;
        case ClassDef::Protocol:   result="Protokoll"; break;
        case ClassDef::Category:   result="Kategori"; break;
        case ClassDef::Exception:  result="Undantag"; break;
        case ClassDef::Service:    result="Tjänstere"; break;
        case ClassDef::Singleton:  result="Singleton"; break;
        default: break;
      }
      return result;
    }
};
#endif
