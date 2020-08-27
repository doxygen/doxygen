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

class TranslatorSwedish : public Translator
{
  public:

    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[swedish]{babel}\n";
    }

    virtual QCString trISOLang()
    {
      return "sv";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Besläktade funktioner"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Observera att dessa inte är medlemsfunktioner)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detaljerad beskrivning"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentation av typdefinierade medlemmar"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentation av egenuppräknande medlemmar"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentation av medlemsfunktioner"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
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
    virtual QCString trMore()
    { return "Mer..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista över alla medlemmar"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Medlemslista"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Det här är en fullständig lista över medlemmar för "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return " med alla ärvda medlemmar."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatiskt skapad av Doxygen";
      if (s) result+=(QCString)" för "+s;
      result+=" från källkoden.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum-namn"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum-värde"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definierad i"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduler"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klasshierarki"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
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
    virtual QCString trFileList()
    { return "Fillista"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
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
    virtual QCString trFileMembers()
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
    virtual QCString trRelatedPages()
    { return "Relaterade sidor"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Exempel"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Sök"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Denna lista över arv är grovt, men inte helt, "
             "sorterad i alfabetisk ordning:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filer, med en kort beskrivning:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Här följer datastrukturerna med korta beskrivningar:";
      }
      else
      {
        return "Här följer klasserna, strukterna, unionerna och "
               "gränssnitten med korta beskrivningar:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
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
    virtual QCString trFileMembersDescription(bool extractAll)
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
    virtual QCString trExamplesDescription()
    { return "Här följer en lista med alla exempel:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Här följer en lista över alla relaterade dokumentationssidor:";}

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Här följer en lista över alla moduler:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modulindex"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarkiskt index"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
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
    virtual QCString trFileIndex()
    { return "Filindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Moduldokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentation över datastrukturer";
      }
      else
      {
        return "Klassdokumentation";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Fildokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Exempeldokumentation"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Sid-dokumentation"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referensmanual"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makron"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typdefinitioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Egenuppräknande typer"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funktioner"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variabler"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Egenuppräknade typers värden"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Dokumentation över makrodefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentation över typdefinitioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentation över egenuppräknande typer"; ; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Dokumentation över funktioner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Dokumentation över variabler"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Skapad "+date;
      if (projName) result+=(QCString)" för "+projName;
      result+=(QCString)" av";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klassdiagram för "+clName;
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Endast för internt bruk."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Varning"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Returnerar"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Se även"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametrar"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Undantag"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Skapad av"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Namnrymdslista"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namnrymder med en kort beskrivning:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Vänner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRelatedFunctionDocumentation()
    { return "Dokumenation av vänners och relaterade funktioner"; }

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
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" filreferens";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" namnrymdreferens";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Publika medlemsfunktioner"; }
    virtual QCString trPublicSlots()
    { return "Publika slots"; }
    virtual QCString trSignals()
    { return "Signaler"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiska publika medlemmsfunktioner"; }
    virtual QCString trProtectedMembers()
    { return "Skyddade medlemmsfunktioner"; }
    virtual QCString trProtectedSlots()
    { return "Skyddade slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statiska skyddade medlemmsfunktioner"; }
    virtual QCString trPrivateMembers()
    { return "Privata medlemmsfunktioner"; }
    virtual QCString trPrivateSlots()
    { return "Privata slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statiska privata medlemmsfunktioner"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+=", och ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Ärver "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Ärvd av "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Återskapad från "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Återskapad i "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Namnrymdsmedlemmar"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Namnrymdsindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
      { return "Namnrymd-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namnrymder"; }

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
      QCString result=(QCString)"Dokumentationen för ";
      switch(compType)
      {
        case ClassDef::Class:      result+="denna klass"; break;
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
    virtual QCString trReturnValues()
    { return "Returvärden"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Huvudsida"; }

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
      return "Definition på rad @0 i filen @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definition i filen @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Obsolet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Samarbetsdiagram för "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include-beroendediagram för "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Dokumentation av konstruktorer och destruktorer";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Gå till denna fils källkod.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Gå till denna fils dokumentation.";
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
      return "Begynnelsevärde:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "källkod";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafisk klasshierarki";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Gå till den grafiska klasshierarkin";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Gå till den textbaserade klasshierarkin";
    }
    virtual QCString trPageIndex()
    {
      return "Sidindex";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Notera";
    }
    virtual QCString trPublicTypes()
    {
      return "Publika typer";
    }
    virtual QCString trPublicAttribs()
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
    virtual QCString trStaticPublicAttribs()
    {
      return "Statiska publika attribut";
    }
    virtual QCString trProtectedTypes()
    {
      return "Skyddade typer";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Skyddade attribut";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statiska skyddade attribut";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privata typer";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privata attribut";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statiska privata attribut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Att-göra";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Att göra-lista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Refererad av";
    }
    virtual QCString trRemarks()
    {
      return "Lägg märke till";
    }
    virtual QCString trAttention()
    {
      return "Observera";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Den här grafen visar vilka filer som direkt eller "
             "indirekt inkluderar denna filen:";
    }
    virtual QCString trSince()
    {
      return "Sedan";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Grafförklaring";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
    virtual QCString trLegend()
    {
      return "förklaring";
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
      return "Testlista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Egenskaper";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Egenskapsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
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
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paket "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Paketlista";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Här är en lista över paketen med en kort beskrivning "
             "(om sådan finns):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paket";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Värde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Bugg";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Klass" : "klass"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Fil" : "fil"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Namnrymd" : "namnrymd"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Sid" : "sid"));
      if (singular)
         result+="a";
      else
         result+="or";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Medlem" : "medlem"));
      if (!singular)  result+="mar";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="er";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool /*singular*/)
    {
      QCString result((first_capital ? "Författare" : "författare"));
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referenser";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementerar "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementerad i "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Innehållsförteckning";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista över föråldrade";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Händelser";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Händelsedokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Pakettyper";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Paketfunktioner";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statiska paketfunktioner";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Paketattribut";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statiska paketattribut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Alla";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Här är anropsdiagrammet för den här funktionen:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Träffar:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Källkodsfilen " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Katalogstruktur"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Katalogdokumentation"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Kataloger"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Den här katalogen är grovt sorterad, "
             "men inte helt, i alfabetisk ordning:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Katalogreferens"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Katalog" : "katalog"));
      if (!singular) result+="er";
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
      return "Det här är en överlagrad medlemsfunktion "
             "tillhandahållen för bekvämlighet. Den enda som "
             "skiljer sig från ovanstående funktion är vilka "
             "argument den tar emot.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Här är anropsgrafen för denna funktion:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Uppräknings-dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Dokumentation för memlemsfunktion/subrutin"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Datatyplista"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Datafält"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Här är datatyperna med en kort beskrivning:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Datatypsindex"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Datatypsdokumentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funktioner/subrutiner"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Dokumentation för funktion/subrutin"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datatyper"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modullista"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="moduler med en kort beskrivning:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
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
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Modulreferens";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modulmedlemmar"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Modulindex"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
     QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Dokumentationen för ";
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
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Typ" : "typ"));
      if (!singular)  result+="er";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool /*singular*/)
    {
      QCString result((first_capital ? "Underprogram" : "underprogram"));
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Typbegränsningar";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Relation";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Laddar...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Global namnrymd";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Söker...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
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
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Fil i "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Inkluderar fil i "+name;
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
      static const char *days[]   = { "Mån","Tis","Ons","Tor","Fre","Lör","Sön" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Maj","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };
      QCString sdate;
      sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Bibliografiska Referenser"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Katalogberoendegraf för ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "detaljnivå"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Mall-parametrar"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "och "+number+" flera..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Dokumentationen för denna enum var genererad från föjlande fil";
      if (!single) result += "er";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Enum-referens"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" ärvd ifrån "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Ytterliga ärvda medlemmar"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "aktivera" : "inaktivera";
      return "klicka för att "+opt+" panelsynkronisering";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Tillhandahålls av kategori @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Utökar klass @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Klassmetoder";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Instansmetoder";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Metoddokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces()
    { return "Exporterade interface"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices()
    { return "Inkuderade tjänster"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups()
    { return "Konstantgrupper"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Referens till konstantgrupp";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const char *sName)
    {
      QCString result=(QCString)sName;
      result+=" Tjänstereferens";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const char *sName)
    {
      QCString result=(QCString)sName;
      result+=" Singleton-referens";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Dokumentationen för denna tjänst "
                                "genererades från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Dokumentationen för denna singleton "
                                "genererades från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy()
    { return "Designenhetshirarki"; }
    /** VHDL design unit list */
    virtual QCString trDesignUnitList()
    { return "Designenhetslista"; }
    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers()
    { return "Designenhetsmedlemmar"; }
    /** VHDL design unit list description
     * Orginal: Here is a list of all design unit members with links to
     *          the Entities they belong to:
     */
    virtual QCString trDesignUnitListDescription()
    {
        return "Här är en lista av alla designenhetsmedlemmar med länkar till "
               "entiteterna som de hör till:";
    }
    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex()
    { return "Designenhetsindex"; }
    /** VHDL design units */
    virtual QCString trDesignUnits()
    { return "Designenheter"; }
    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc()
    { return "Funktioner/Procedurer/Processer"; }
    /** VHDL type */
    virtual QCString trVhdlType(uint64 type,bool single)
    {
      switch(type)
      {
        case VhdlDocGen::LIBRARY:
          return "Biblotek";
        case VhdlDocGen::PACKAGE:
          return "Paket";
        case VhdlDocGen::SIGNAL:
          if (single) return "Signal";
          else        return "Signaler";
        case VhdlDocGen::COMPONENT:
          if (single) return "Komponent";
          else        return "Komponenter";
        case VhdlDocGen::CONSTANT:
          if (single) return "Konstant";
          else        return "Konstanter";
        case VhdlDocGen::ENTITY:
          if (single) return "Entitet";
          else        return "Entiteter";
        case VhdlDocGen::TYPE:
          if (single) return "Typ";
          else        return "Typer";
        case VhdlDocGen::SUBTYPE:
          if (single) return "Undertyp";
          else        return "Undertyper";
        case VhdlDocGen::FUNCTION:
          if (single) return "Funktion";
          else        return "Funktioner";
        case VhdlDocGen::RECORD:
          if (single) return "Post";
          else        return "Poster";
        case VhdlDocGen::PROCEDURE:
          if (single) return "Procedur";
          else        return "Procedurer";
        case VhdlDocGen::ARCHITECTURE:
          if (single) return "Arkitektur";
          else        return "Arkitekturer";
        case VhdlDocGen::ATTRIBUTE:
          return "Attribut";
        case VhdlDocGen::PROCESS:
          if (single) return "Process";
          else        return "Processer";
        case VhdlDocGen::PORT:
          if (single) return "Port";
          else        return "Portar";
        case VhdlDocGen::USE:
          if (single) return "use clause";
          else        return "Use Clauses";
        case VhdlDocGen::GENERIC:
          if (single) return "Generisk";
          else        return "Generiska";
        case VhdlDocGen::PACKAGE_BODY:
          return "Paketinehåll";
        case VhdlDocGen::UNITS:
          return "Enheter";
        case VhdlDocGen::SHAREDVARIABLE:
          if (single) return "Delad Variabel";
          else        return "Delade Variabler";
        case VhdlDocGen::VFILE:
          if (single) return "Fil";
          else        return "Filer";
        case VhdlDocGen::GROUP:
          if (single) return "Grupp";
          else        return "Grupper";
        case VhdlDocGen::INSTANTIATION:
          if (single) return "Instantiation";
          else        return "Instantiations";
        case VhdlDocGen::ALIAS:
          return "Alias";
        case VhdlDocGen::CONFIG:
          if (single) return "Konfiguration";
          else        return "Konfigurationer";
        case VhdlDocGen::MISCELLANEOUS:
          return "Diverse";
        case VhdlDocGen::UCF_CONST:
          return "Begränsningar";
        default:
          return "Klass";
      }
    }
    virtual QCString trCustomReference(const char *name)
    { return QCString(name)+"referens"; }

    /* Slice */
    virtual QCString trConstants()
    {
        return "Konstanter";
    }
    virtual QCString trConstantDocumentation()
    {
        return "Konstantdokumentation";
    }
    virtual QCString trSequences()
    {
        return "Sekvenser";
    }
    virtual QCString trSequenceDocumentation()
    {
        return "Sekvensdokumentation";
    }
    virtual QCString trDictionaries()
    {
        return "Uppslagsverk";
    }
    virtual QCString trDictionaryDocumentation()
    {
        return "Uppslagsverksdokumentation";
    }
    virtual QCString trSliceInterfaces()
    {
        return "Gränssnitt";
    }
    virtual QCString trInterfaceIndex()
    {
        return "Gränssnittsindex";
    }
    virtual QCString trInterfaceList()
    {
        return "Gränssnittslist";
    }
    /** Orginal: Here are the interfaces with brief descriptions: */
    virtual QCString trInterfaceListDescription()
    {
        return "Här är gränssnitten med en kort beskrivning";
    }
    virtual QCString trInterfaceHierarchy()
    {
        return "Gränssnittshirarkin";
    }
    /** Orginal: This inheritance list is sorted roughly, but not completely, alphabetically: */
    virtual QCString trInterfaceHierarchyDescription()
    {
        return "Denna arvslista är grovt sorterad, men inte helt, i alfabetisk ordning:";
    }
    virtual QCString trInterfaceDocumentation()
    {
        return "Gränssnittsdokumentation";
    }
    virtual QCString trStructs()
    {
        return "Strukturer";
    }
    virtual QCString trStructIndex()
    {
        return "Strukturindex";
    }
    virtual QCString trStructList()
    {
        return "Strukturlist";
    }
    /** Orginal: Here are the structs with brief descriptions: */
    virtual QCString trStructListDescription()
    {
        return "Här är strukturerna med en kort beskrivning:";
    }
    virtual QCString trStructDocumentation()
    {
        return "Strukturdokumentation";
    }
    virtual QCString trExceptionIndex()
    {
        return "Undantagsindex";
    }
    virtual QCString trExceptionList()
    {
        return "Undantagslista";
    }
    /** Orginal: Here are the exceptions with brief descriptions: */
    virtual QCString trExceptionListDescription()
    {
        return "Här är undantagen med en kort beskrivning:";
    }
    virtual QCString trExceptionHierarchy()
    {
        return "Undantagshirarki";
    }
    /** Orginal:  This inheritance list is sorted roughly, but not completely, alphabetically: */
    virtual QCString trExceptionHierarchyDescription()
    {
        return "Denna arvslista är grovt sorterad, men inte helt, i alfabetisk ordning:";
    }
    virtual QCString trExceptionDocumentation()
    {
        return "Undantagsdokumentation";
    }
    virtual QCString trCompoundReferenceSlice(const char *clName, ClassDef::CompoundType compType, bool isLocal)
    {
      QCString result=(QCString)clName;
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
    virtual QCString trOperations()
    {
        return "Operationer";
    }
    virtual QCString trOperationDocumentation()
    {
        return "Operationsdokumentation";
    }
    virtual QCString trDataMembers()
    {
        return "Datamedlemmar";
    }
    virtual QCString trDataMemberDocumentation()
    {
        return "Datamedlemsdokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation()
    { return "Designenhetsdokumentation"; }

};
#endif
