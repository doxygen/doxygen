/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
*/

#ifndef TRANSLATOR_SE_H
#define TRANSLATOR_SE_H

class TranslatorSwedish : public TranslatorAdapter_1_3_3
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
      return "";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-1";
    }

    virtual QCString trRelatedFunctions()
    { return "Besläktade funktioner"; }

    virtual QCString trRelatedSubscript()
    { return "(Observera att dessa inte är medlemsfunktioner)"; }

    virtual QCString trDetailedDescription()
    { return "Detaljerad beskrivning"; }

    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentation av typdefinierade medlemmar"; }

    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentation av egenuppräknande medlemmar"; }

    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentation av medlemsfunktioner"; }

    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Fält dokumentation"; 
      }
      else
      {
        return "Dokumentation av datamedlemmar";
      }
    }

    virtual QCString trMore()
    { return "Mer..."; }

    virtual QCString trListOfAllMembers()
    { return "Lista över alla medlemmar."; }

    virtual QCString trMemberList()
    { return "Medlemslista"; }

    virtual QCString trThisIsTheListOfAllMembers()
    { return "Det här är en fullständig lista över medlemmar för "; }

    virtual QCString trIncludingInheritedMembers()
    { return " med alla ärvda medlemmar."; }

    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatiskt skapad av Doxygen";
      if (s) result+=(QCString)" för "+s;
      result+=" från källkoden."; 
      return result;
    }

    virtual QCString trEnumName()
    { return "enum namn"; }

    virtual QCString trEnumValue()
    { return "enum värde"; }

    virtual QCString trDefinedIn()
    { return "definierad i"; }

/*
    QCString trVerbatimText(const char *f)
    { return (QCString)"Detta är den ordagranna texten från inkluderingsfilen "+f; }
*/
    virtual QCString trModules()
    { return "Moduler"; }

    virtual QCString trClassHierarchy()
    { return "Klasshierarki"; }

    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Sammansatt klasslista";
      }
    }

    virtual QCString trFileList()
    { return "Fillista"; }

    virtual QCString trHeaderFiles()
    { return "Headerfiler"; }

    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Data fält"; 
      }
      else
      {
        return "Sammansatta klassmedlemmar";
      }
    }

    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globala symboler"; 
      }
      else
      {
        return "Filmedlemmar";
      }
    }

    virtual QCString trRelatedPages()
    { return "Besläktade sidor"; }

    virtual QCString trExamples()
    { return "Exempel"; }

    virtual QCString trSearch()
    { return "Sök"; }

    virtual QCString trClassHierarchyDescription()
    { return "Denna lista över arv är grovt, men inte helt, "
             "sorterad i alfabetisk ordning:";
    }

    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filer, med en kort beskrivning:";
      return result;
    }

    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="struktur/unions-dokumentationen för varje fält:";
        }
        else
        {
          result+="klassdokumentationen för varje medlem:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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

    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        result+= "filerna som de tillhör:";
      else
        result+= "dokumentationen:";
      return result;
    }

    virtual QCString trHeaderFilesDescription()
    { return "Här följer headerfilerna som API:n består av:"; }

    virtual QCString trExamplesDescription()
    { return "Här följer en lista med alla exempel:"; }

    virtual QCString trRelatedPagesDescription()
    { return "Här följer en lista över alla besläktade dokumentationssidor:";}

    virtual QCString trModulesDescription()
    { return "Här följer en lista över alla moduler:"; }

    virtual QCString trNoDescriptionAvailable()
    { return "Beskrivning saknas"; }

    virtual QCString trDocumentation()
    { return "Dokumentation"; }

    virtual QCString trModuleIndex()
    { return "Modulindex"; }

    virtual QCString trHierarchicalIndex()
    { return "Hierarkiskt Index"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
        return "Index över datastrukturer";
      } else {
        return "Index över sammensatta typer";
      }
    }

    virtual QCString trFileIndex()
    { return "Filindex"; }

    virtual QCString trModuleDocumentation()
    { return "Dokumentation över moduler"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentation över datastrukturer";
      }
      else
      {
        return "Documentation över klasser";
      }
    }

    virtual QCString trFileDocumentation()
    { return "Dokumentation över filer"; }

    virtual QCString trExampleDocumentation()
    { return "Dokumentation över exempel"; }

    virtual QCString trPageDocumentation()
    { return "Dokumentation av sidor"; }

    virtual QCString trReferenceManual()
    { return "Referensmanual"; }

    virtual QCString trDefines()
    { return "Definitioner"; }
    virtual QCString trFuncProtos()
    { return "Funktionsprototyper"; }
    virtual QCString trTypedefs()
    { return "Typdefinitioner"; }
    virtual QCString trEnumerations()
    { return "Egenuppräknande typer"; }
    virtual QCString trFunctions()
    { return "Funktioner"; }
    virtual QCString trVariables()
    { return "Variabler"; }

    virtual QCString trEnumerationValues()
    { return "Egenuppräknade typers värden"; }

    virtual QCString trDefineDocumentation()
    { return "Dokumentation över definitioner"; }

    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentation över funktionsprototyper"; }

    virtual QCString trTypedefDocumentation()
    { return "Dokumentation över typdefinitioner"; }

    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentation över egenuppräknande typer"; }

    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentation över egenuppräknande typers värden"; }

    virtual QCString trFunctionDocumentation()
    { return "Dokumentation över funktioner"; }

    virtual QCString trVariableDocumentation()
    { return "Dokumentation över variabler"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datastrukturer";
      }
      else
      {
        return "Sammansättning";
      }
    }

    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Skapad "+date;
      if (projName) result+=(QCString)" för "+projName;
      result+=(QCString)" av";
      return result;
    }

    virtual QCString trWrittenBy()
    {
      return "skriven av";
    }

    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klassdiagram för "+clName;
    }

    virtual QCString trForInternalUseOnly()
    { return "Endast för internt bruk."; }

    virtual QCString trReimplementedForInternalReasons()
    { return "Omskriven av intern anledning; API:n påverkas inte.";}

    virtual QCString trWarning()
    { return "Varning"; }

    virtual QCString trBugsAndLimitations()
    { return "Fel och begränsningar"; }

    virtual QCString trVersion()
    { return "Version"; }

    virtual QCString trDate()
    { return "Datum"; }

    virtual QCString trReturns()
    { return "Returnerar"; }

    virtual QCString trSeeAlso()
    { return "Se även"; }

    virtual QCString trParameters()
    { return "Parametrar"; }

    virtual QCString trExceptions()
    { return "Undantag"; }

    virtual QCString trGeneratedBy()
    { return "Skapad av"; }

// new since 0.49-990307

    virtual QCString trNamespaceList()
    { return "Namnrymdslista"; }

    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Här är en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="namnrymder med en kort beskrivning:";
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
                                    bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" klass"; break;
        case ClassDef::Struct: result+=" strukt"; break;
        case ClassDef::Union:  result+=" union"; break;
        case ClassDef::Interface:  result+=" gränssnitt"; break;
        case ClassDef::Protocol:   result+=" protocol"; break; // translate me!
        case ClassDef::Category:   result+=" category"; break; // translate me!
        case ClassDef::Exception:  result+=" undantag"; break;
      }
      if (isTemplate) result+="template";
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
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
        case ClassDef::Exception:  result+="detta undantag "; break;
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

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Föråldrad";
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
      return "Förhandsvillkor";
    }

    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Resultat"; //"Postcondition";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      return "Att-göra lista";
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
	      "indirekt inkluderar denna filen.";
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
        "Tag följande exempel:\n"
        "\\code\n"
	"/*! Osynlig klass på grund av stympning */\n"
        "class Invisible { };\n\n"
	"/*! Stympad klass, ärvningsrelationen är dold */\n"
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
	"/*! Klass som används av Inherited klassen */\n"
        "class Used { };\n\n"
	"/*! Super klassen som ärver ett antal andra klasser */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
	"Om \\c MAX_DOT_GRAPH_HEIGHT är satt till 240 i konfigurationsfilen, "
	"kommer följande graf att generas:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
	"Rektanglarna i den ovanstående grafen har följande betydelser:\n"
        "<ul>\n"
	"<li>%En fylld svart rektangel representerar den strukt eller klass "
	"som har genererat grafen.\n"
	"<li>%En rektangel med svart kant symboliserar en dokumenterad "
	"strukt eller klass.\n"
	"<li>%En rektangel med grå kant symboliserar en odokumenterad strukt "
	"eller klass.\n"
	"<li>%En klass med röd kant symboliserar en strukt eller klass där "
	"alla dess relationer inte visas. %En graf stympas om den inte får "
	"plats inom de givna gränserna.\n"
        "</ul>\n"
	"Pilarna har följande betydelser:\n"
        "<ul>\n"
	"<li>%En mörkblå pil används för att visualisera en publik arvrelation "
	"mellan två klasser.\n"
	"<li>%En mörkgrön pil används för en skyddad arvsrelation.\n"
	"<li>%En mörkröd pil används för en privat arvsrelation.\n"
	"<li>%En sträckad lila pil används om en klass är innesluten eller "
	"använd av en annan klass. Vid pilen står namnet på den eller de "
	"variabler som klassen pilen pekar på kommer åt.\n"
	"<li>%En sträckad gul pil symboliserar förhållandet mellan en "
	"template-instans och template-klassen, som den instantierades från.\n"
	"Vid pilen står instansens template-parametrar.\n"
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
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP metoder";
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

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Gränssnitt";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Paketdokumentation";
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
	 * (used table extract:)
	 * <pre>
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * ANSI_CHARSET              0 (x00)            1252
	 * </pre>
	 */
    virtual QCString trRTFansicp()
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset */
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
      QCString result((first_capital ? "Namnrymd" : "namnrynd"));
      if (!singular)  result+="er";
      return result; 
    }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Grupp" : "grupp"));
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
    virtual QCString trField(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Fält" : "fält"));
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
      return "Händelse Dokumentation";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Paket typer";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Paket funktioner";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Statiska paket funktioner";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Paket attribut";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Statiska paket attribut";
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
      return "Här är anropnings diagrammet för den här funktionen:";
    }



};

#endif
