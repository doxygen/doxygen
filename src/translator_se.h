/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

*/

#ifndef TRANSLATOR_SE_H
#define TRANSLATOR_SE_H

class TranslatorSwedish : public TranslatorEnglish
{
  public:
    /*! This method is used to generate a warning message to signal
     *  the user that the translation of his/her language of choice
     *  needs updating.
     */
    virtual QCString updateNeededMessage()
    {
      return "Warning: The Swedish translator is really obsolete.\n"
             "It was not updated since version 1.0.0.  As a result,\n"
             "some sentences may appear in English.\n\n";
    }

    QCString idLanguage()
    { return "swedish"; }
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
    { return "Namnrymdlista"; }

    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namnrymder med en kort förklaring:";
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
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/)
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" klass"; break;
        case ClassDef::Struct: result+=" strukt"; break;
        case ClassDef::Union:  result+=" union"; break;
        case ClassDef::Interface:  result+=" gränssnitt"; break;
        case ClassDef::Exception:  result+=" exception"; break;
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
      result+=" namnrymdreferens";
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
    { return "Namnrymdsmedlemmar"; }
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

    virtual QCString trNamespaceIndex()
    { return "Namnrymdsindex"; }

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
        case ClassDef::Class:      result+="denna klass "; break;
        case ClassDef::Struct:     result+="denna strukt "; break;
        case ClassDef::Union:      result+="denna union "; break;
        case ClassDef::Interface:  result+="detta gränssnitt "; break;
        case ClassDef::Exception:  result+="exception "; break;
      }
      result+="var genererad från följande fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    {
      return "Alfabetisk lista";
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

    virtual QCString trSources()
    {
      return "Källor";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definition på rad @0 i filen @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definition i filen @0.";
    }
};

#endif
