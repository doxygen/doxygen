/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

/*
==================================================================================
Svensk översättning av:
Samuel Hägglund      <sahag96@ite.mh.se>
Xet Erixon           <xet@xeqt.com>
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
   Namespace och slot: har de nån hygglig svensk översättning???

   Skicka gärna synpunkter.
===================================================================================
*/

#ifndef TRANSLATOR_SE_H
#define TRANSLATOR_SE_H

#include "translator.h"

class TranslatorSwedish : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "swedish"; }

    QCString trRelatedFunctions()
    { return "Besläktade funktioner"; }

    QCString trRelatedSubscript()
    { return "(Observera att dessa inte är medlemsfunktioner)"; }

    QCString trDetailedDescription()
    { return "Detaljerad beskrivning"; }

    QCString trMemberTypedefDocumentation()
    { return "Dokumentation över typdefinierade medlemmar"; }

    QCString trMemberEnumerationDocumentation()
    { return "Dokumentation över egenuppräknande medlemmar"; }

    QCString trMemberFunctionDocumentation()
    { return "Dokumentation över medlemsfunktioner"; }

    QCString trMemberDataDocumentation()
    { return "Dokumentation över datamedlemmar"; }

    QCString trGeneratedFrom(const char *s,bool single)
    { 

    QCString result=(QCString)"Dokumentationen för denna"+s+
                     " är skapad utifrån följande fil";
      if (single) result+=":";   else result+="er:";
      return result;
    }
    QCString trMore()
    { return "Mer..."; }

    QCString trListOfAllMembers()
    { return "Lista över alla medlemmar."; }

    QCString trMemberList()
    { return "Medlemslista"; }

    QCString trThisIsTheListOfAllMembers()
    { return "Det här är en fullständig lista över medlemmar för "; }

    QCString trIncludingInheritedMembers()
    { return " med alla ärvda medlemmar."; }

    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatiskt skapad av Doxygen";
      if (s) result+=(QCString)" för "+s;
      result+=" från källkoden."; 
      return result;
    }

    QCString trEnumName()
    { return "enum namn"; }

    QCString trEnumValue()
    { return "enum värde"; }

    QCString trDefinedIn()
    { return "definierad i"; }


    QCString trVerbatimText(const char *f)
    { return (QCString)"Detta är den ordagranna texten från inkluderingsfilen "+f; }

    QCString trModules()
    { return "Moduler"; }

    QCString trClassHierarchy()
    { return "Klasshierarki"; }

    QCString trCompoundList()
    { return "Sammansatt klasslista"; }

    QCString trFileList()
    { return "Fillista"; }

    QCString trHeaderFiles()
    { return "Headerfiler"; }

    QCString trCompoundMembers()
    { return "Sammansatta klassmedlemmar"; }

    QCString trFileMembers()
    { return "Filmedlemmar"; }

    QCString trRelatedPages()
    { return "Besläktade sidor"; }

    QCString trExamples()
    { return "Exempel"; }

    QCString trSearch()
    { return "Sök"; }

    QCString trClassHierarchyDescription()
    { return "Denna lista över arv är grovt, men inte helt, "
             "sorterad i alfabetisk ordning:";
    }

    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filer, med en kort beskrivning:";
      return result;
    }

    QCString trCompoundListDescription()
     { return "Här följer klasserna, strukterna och "
             "unionerna med en kort beskrivning:"; 
    }

    QCString trCompoundMembersDescription(bool extractAll)
    {

    QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="klassmedlemmar med länkar till ";
      if (extractAll) result+="klassdokumentationen för varje medlem:";
      else result+="klasserna som de tillhör:";
      return result;
    }

    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filmedlemmar med länkar till ";
      if (extractAll) result+="dokumentationsfilen för varje medlem:";
      else result+="filerna som de tillhör:";
      return result;
    }

    QCString trHeaderFilesDescription()
    { return "Här följer headerfilerna som API:n består av:"; }

    QCString trExamplesDescription()
    { return "Här följer en lista med alla exempel:"; }

    QCString trRelatedPagesDescription()
    { return "Här följer en lista över alla besläktade dokumentationssidor:";}

    QCString trModulesDescription()
    { return "Här följer en lista över alla moduler:"; }

    QCString trNoDescriptionAvailable()
    { return "Beskrivning saknas"; }

    QCString trDocumentation()
    { return "Dokumentation"; }

    QCString trModuleIndex()
    { return "Modulindex"; }

    QCString trHierarchicalIndex()
    { return "Hierarkiskt Index"; }

    QCString trCompoundIndex()
    { return "Sammansatt Index"; }

    QCString trFileIndex()
    { return "Filindex"; }

    QCString trModuleDocumentation()
    { return "Dokumentation över moduler"; }

    QCString trClassDocumentation()
    { return "Dokumentation över klasser"; }

    QCString trFileDocumentation()
    { return "Dokumentation över filer"; }

    QCString trExampleDocumentation()
    { return "Dokumentation över exempel"; }

    QCString trPageDocumentation()
    { return "Dokumentation av sidor"; }

    QCString trReferenceManual()
    { return "Referensmanual"; }

    QCString trDefines()
    { return "Definitioner"; }
    QCString trFuncProtos()
    { return "Funktionsprototyper"; }
    QCString trTypedefs()
    { return "Typdefinitioner"; }
    QCString trEnumerations()
    { return "Egenuppräknande typer"; }
    QCString trFunctions()
    { return "Funktioner"; }

    QCString trVariables()
    { return "Variabler"; }

    QCString trEnumerationValues()
    { return "Egenuppräknade typers värden"; }

    QCString trAuthor()
    { return "Författare"; }

    QCString trDefineDocumentation()
    { return "Dokumentation över definitioner"; }

    QCString trFunctionPrototypeDocumentation()
    { return "Dokumentation över funktionsprototyper"; }

    QCString trTypedefDocumentation()
    { return "Dokumentation över typdefinitioner"; }

    QCString trEnumerationTypeDocumentation()
    { return "Dokumentation över egenuppräknande typer"; }

    QCString trEnumerationValueDocumentation()
    { return "Dokumentation över egenuppräknande typers värden"; }

    QCString trFunctionDocumentation()
    { return "Dokumentation över funktioner"; }

    QCString trVariableDocumentation()
    { return "Dokumentation över variabler"; }

    QCString trCompounds()
    { return "Sammansättning"; }

    QCString trFiles()
    { return "Filer"; }

    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Skapad "+date;
      if (projName) result+=(QCString)" för "+projName;
      result+=(QCString)" av";
      return result;
    }

    QCString trWrittenBy()
    {
      return "skriven av";
    }

    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klassdiagram för "+clName;
    }

    QCString trForInternalUseOnly()
    { return "Endast för internt bruk."; }

    QCString trReimplementedForInternalReasons()
    { return "Omskriven av intern anledning ; API:n påverkas inte.";}

    QCString trWarning()
    { return "Varning"; }

    QCString trBugsAndLimitations()
    { return "Fel och begränsningar"; }

    QCString trVersion()
    { return "Version"; }

    QCString trDate()
    { return "Datum"; }

    QCString trAuthors()
    { return "Författare"; }

    QCString trReturns()
    { return "Returnerar"; }

    QCString trSeeAlso()
    { return "Se även"; }

    QCString trParameters()
    { return "Parametrar"; }

    QCString trExceptions()
    { return "Undantag"; }

    QCString trGeneratedBy()
    { return "Skapad av"; }

// new since 0.49-990307

    virtual QCString trNamespaceList()
    { return "Namespacelista"; }

    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namespaces med en kort förklaring:";
      return result;
    }

    virtual QCString trFriends()
    { return "Vänner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRelatedFunctionDocumentation()
    { return "Vänners och besläktade funktioners dokumentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Klass"; break;
        case ClassDef::Struct: result+=" Strukt"; break;
        case ClassDef::Union:  result+=" Union"; break;
      }
      result+="referens";
      return result;
    }

    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" filreferens";
      return result;
    }

    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" namespacereferens";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Publika medlemmar"; }
    virtual QCString trPublicSlots()
    { return "Publika slots"; }
    virtual QCString trSignals()
    { return "Signaler"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiska  publika medlemmar"; }
    virtual QCString trProtectedMembers()
    { return "Skyddade medlemmar"; }
    virtual QCString trProtectedSlots()
    { return "Skyddade slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statiska skyddade medlemmar"; }
    virtual QCString trPrivateMembers()
    { return "Privata medlemmar"; }
    virtual QCString trPrivateSlots()
    { return "Privata slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statiska privata medlemmar"; }
    // end of member sections

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

    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Ärver "+trWriteList(numEntries)+".";
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Ärvd av "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Återskapad från "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Återskapad i "+trWriteList(numEntries)+".";
    }

    virtual QCString trNamespaceMembers()
    { return "Namespacemedlemmar"; }
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namespacemedlemmar med länkar till ";
      if (extractAll)
        result+=" namespace-dokumentationen för varje medlem:";
      else
        result+="de namespaces de tillhör:";
      return result;
    }

    virtual QCString trNamespaceIndex()
    { return "Namespaceindex"; }

    virtual QCString trNamespaceDocumentation()

{ return "Namespace-dokumentation"; }
};

#endif
