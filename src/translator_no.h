/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

==================================================================================
*/

#ifndef TRANSLATOR_NO_H
#define TRANSLATOR_NO_H

#include "translator_adapter.h"

class TranslatorNorwegian : public TranslatorAdapter_1_2_2
{
  public:
    QCString idLanguage()
    { return "norwegian"; }
    QCString latexBabelPackage()
    { return "norwegian"; }

    QCString trRelatedFunctions()
    { return "Beslektede funksjoner"; }

    QCString trRelatedSubscript()
    { return "(Observer at disse ikke er medlemsfunksjoner)"; }

    QCString trDetailedDescription()
    { return "Detaljert beskrivelse"; }

    QCString trMemberTypedefDocumentation()
    { return "Dokumentasjon over typedefinerte medlemmer"; }

    QCString trMemberEnumerationDocumentation()
    { return "Dokumentasjon over enumererte medlemmer"; }

    QCString trMemberFunctionDocumentation()
    { return "Dokumentasjon over medlemsfunksjoner"; }

    QCString trMemberDataDocumentation()
    { return "Dokumentasjon over datamedlemmer"; }

    QCString trMore()
    { return "Mer..."; }

    QCString trListOfAllMembers()
    { return "Liste over alle medlemmer."; }

    QCString trMemberList()
    { return "Medlemsliste"; }

    QCString trThisIsTheListOfAllMembers()
    { return "Det her er en fullstendig liste over medlemmer for "; }

    QCString trIncludingInheritedMembers()
    { return " med alle nedarvede medlemmer."; }

    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatisk generert av Doxygen";
      if (s) result+=(QCString)" for "+s;
      result+=" fra kildekoden."; 
      return result;
    }

    QCString trEnumName()
    { return "enum navn"; }

    QCString trEnumValue()
    { return "enum verdi"; }

    QCString trDefinedIn()
    { return "definert i"; }


    QCString trVerbatimText(const char *f)
    { return (QCString)"Dette er den ordrette teksten fra inkluderingsfilen "+f; }

    QCString trModules()
    { return "Moduler"; }

    QCString trClassHierarchy()
    { return "Klassehierarki"; }

    QCString trCompoundList()
    { return "Sammensatt klasseliste"; }

    QCString trFileList()
    { return "Filliste"; }

    QCString trHeaderFiles()
    { return "Headerfiler"; }

    QCString trCompoundMembers()
    { return "Sammensatte klassemedlemmer"; }

    QCString trFileMembers()
    { return "Filmedlemmer"; }

    QCString trRelatedPages()
    { return "Beslektede sider"; }

    QCString trExamples()
    { return "Eksempel"; }

    QCString trSearch()
    { return "Søk"; }

    QCString trClassHierarchyDescription()
    { return "Denne listen over arv er grovt, men ikke helt, "
             "sortert i alfabetisk rekkefølge:";
    }

    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Her følger en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="filer, med en kort beskrivelse:";
      return result;
    }

    QCString trCompoundListDescription()
     { return "Her følger klassene, struktene og "
             "unionene med en kort beskrivelse:"; 
    }

    QCString trCompoundMembersDescription(bool extractAll)
    {

    QCString result="Her følger en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="klassemedlemmer med lenker til ";
      if (extractAll) result+="klassedokumentasjonen for hvert medlem:";
      else result+="klassene som de tilhører:";
      return result;
    }

    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Her følger en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="filmedlemmer med lenker til ";
      if (extractAll) result+="dokumentasjonsfilen for hvert medlem:";
      else result+="filene som de tilhører:";
      return result;
    }

    QCString trHeaderFilesDescription()
    { return "Her følger headerfilene som API består av:"; }

    QCString trExamplesDescription()
    { return "Her følger en liste med alle eksempler:"; }

    QCString trRelatedPagesDescription()
    { return "Her følger en liste over alle beslektede dokumentasjonssider:";}

    QCString trModulesDescription()
    { return "Her følger en liste over alle moduler:"; }

    QCString trNoDescriptionAvailable()
    { return "Beskrivelse mangler"; }

    QCString trDocumentation()
    { return "Dokumentasjon"; }

    QCString trModuleIndex()
    { return "Modulindex"; }

    QCString trHierarchicalIndex()
    { return "Hierarkisk Indeks"; }

    QCString trCompoundIndex()
    { return "Sammensatt Indeks"; }

    QCString trFileIndex()
    { return "Filindeks"; }

    QCString trModuleDocumentation()
    { return "Dokumentasjon av moduler"; }

    QCString trClassDocumentation()
    { return "Dokumentasjon av klasser"; }

    QCString trFileDocumentation()
    { return "Dokumentasjon av filer"; }

    QCString trExampleDocumentation()
    { return "Dokumentasjon av eksempler"; }

    QCString trPageDocumentation()
    { return "Dokumentasjon av sider"; }

    QCString trReferenceManual()
    { return "Referansemanual"; }

    QCString trDefines()
    { return "Definisjoner"; }
    QCString trFuncProtos()
    { return "Funksjonsprototyper"; }
    QCString trTypedefs()
    { return "Typedefinisjoner"; }
    QCString trEnumerations()
    { return "Enumererte typer"; }
    QCString trFunctions()
    { return "Funksjoner"; }

    QCString trVariables()
    { return "Variable"; }

    QCString trEnumerationValues()
    { return "Enumererte typers verdier"; }

    QCString trAuthor()
    { return "Forfatter"; }

    QCString trDefineDocumentation()
    { return "Dokumentasjon over definisjoner"; }

    QCString trFunctionPrototypeDocumentation()
    { return "Dokumentasjon over funksjonsprototyper"; }

    QCString trTypedefDocumentation()
    { return "Dokumentasjon over typedefinisjoner"; }

    QCString trEnumerationTypeDocumentation()
    { return "Dokumentasjon over enumererte typer"; }

    QCString trEnumerationValueDocumentation()
    { return "Dokumentasjon over enumererte typers verdier"; }

    QCString trFunctionDocumentation()
    { return "Dokumentasjon over funksjoner"; }

    QCString trVariableDocumentation()
    { return "Dokumentasjon over variable"; }

    QCString trCompounds()
    { return "Sammensetning"; }

    QCString trFiles()
    { return "Filer"; }

    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Generert "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" av";
      return result;
    }

    QCString trWrittenBy()
    {
      return "skrevet av";
    }

    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klassediagram for "+clName;
    }

    QCString trForInternalUseOnly()
    { return "Kun for internt bruk."; }

    QCString trReimplementedForInternalReasons()
    { return "Omskrevet av interne grunner ; API påvirkes ikke.";}

    QCString trWarning()
    { return "Advarsel"; }

    QCString trBugsAndLimitations()
    { return "Feil och begrensninger"; }

    QCString trVersion()
    { return "Versjon"; }

    QCString trDate()
    { return "Dato"; }

    QCString trAuthors()
    { return "Forfatter(e)"; }

    QCString trReturns()
    { return "Returnerer"; }

    QCString trSeeAlso()
    { return "Se også"; }

    QCString trParameters()
    { return "Parametre"; }

    QCString trExceptions()
    { return "Unntak"; }

    QCString trGeneratedBy()
    { return "Generert av"; }

// new since 0.49-990307

    virtual QCString trNamespaceList()
    { return "Navneområdeliste"; }

    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterade ";
      result+="navneområder med en kort forklaring:";
      return result;
    }

    virtual QCString trFriends()
    { return "Venner"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRelatedFunctionDocumentation()
    { return "Venners och beslektede funksjoners dokumentasjon"; }

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
        case ClassDef::Class:  result+=" klasse"; break;
        case ClassDef::Struct: result+=" struktur"; break;
        case ClassDef::Union:  result+=" union"; break;
        case ClassDef::Interface:  result+=" grensesnitt"; break;
        case ClassDef::Exception:  result+=" unntak"; break;
      }
      result+="referanse";
      return result;
    }

    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" filreferanse";
      return result;
    }

    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" navneområdereferanse";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Public medlemmer"; }
    virtual QCString trPublicSlots()
    { return "Public slots"; }
    virtual QCString trSignals()
    { return "Signaler"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiske public medlemmer"; }
    virtual QCString trProtectedMembers()
    { return "Beskyttede medlemmer"; }
    virtual QCString trProtectedSlots()
    { return "Beskyttede slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statiska beskyttede medlemmer"; }
    virtual QCString trPrivateMembers()
    { return "Private medlemmer"; }
    virtual QCString trPrivateSlots()
    { return "Private slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statiske private medlemmer"; }
    // end of member secsjons

    virtual QCString trWriteList(int numEntries)
    {
      // this funcsjon is used to produce a comma-separated list of items.
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
            result+=", og ";
        }
      }
      return result;
    }

    virtual QCString trInheritsList(int numEntries)
      // used in class documentasjon to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Arver "+trWriteList(numEntries)+".";
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentasjon to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Arvet fra "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentasjon blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Reimplementert fra "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentasjon blocks to produce a list of
      // all member that overwrite the implementasjon of this member.
      return "Reimplementert i "+trWriteList(numEntries)+".";
    }

    virtual QCString trNamespaceMembers()
    { return "Navneområdemedlemmer"; }
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Her er en liste over alle ";
      if (!extractAll) result+="dokumenterte ";
      result+="navneområdemedlemmer med lenker til ";
      if (extractAll)
        result+=" navneområde-dokumentasjonen for hvert medlem:";
      else
        result+="de navneområder de tilhører:";
      return result;
    }

    virtual QCString trNamespaceIndex()
    { return "Navneområdeindeks"; }

    virtual QCString trNamespaceDocumentation()
      { return "Navneområde-dokumentasjon"; }
    //////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentasjon before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Navneområder"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentasjon page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentasjonen for ";
      switch(compType)
      {
        case ClassDef::Class:      result+="denne klasse "; break;
        case ClassDef::Struct:     result+="denne strukt "; break;
        case ClassDef::Union:      result+="denne union "; break;
        case ClassDef::Interface:  result+="dette grensesnittet "; break;
        case ClassDef::Exception:  result+="unntak "; break;
      }
      result+="var generert fra følgende fil";
      if (single) result+=":"; else result+="er:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    {
      return "Alfabetisk liste";
    }
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Returverdier"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Hovedside"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentasjon. It should be an abbreviasjon of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "s."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "Kilder";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definisjon på rad @0 i filen @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definisjon i filen @0.";
    }
//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Foreldet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Samarbeidsdiagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include-avhengighetsgraf for "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Konstruktør- og Destruktørdokumentasjon"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Gå til kildekoden for denne filen.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Gå til dokumentasjonen for denne filen.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Før-kondisjon";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Etter-kondisjon";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Initializer:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "kode";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Graphical Class Hierarchy";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Go to the graphical class hierarchy";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Go to the textual class hierarchy";
    }
    virtual QCString trPageIndex()
    {
      return "Sideindeks";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Notat";
    }
    virtual QCString trPublicTypes()
    {
      return "Public Typer";
    }
    virtual QCString trPublicAttribs()
    {
      return "Public Attributter";
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statiske Public Attributter";
    }
    virtual QCString trProtectedTypes()
    {
      return "Beskyttede Typer";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Beskyttede Attributter";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statiske Beskyttede Attributter";
    }
    virtual QCString trPrivateTypes()
    {
      return "Private Typer";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Private Attributter";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statiske Private Attributter";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Todo-liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referert at";
    }
    virtual QCString trRemarks()
    {
      return "Merknader";
    }
    virtual QCString trAttention()
    {
      return "Attention";
    }
    virtual QCString trInclByDepGraph()
    {
      return "This graph shows which files directly or "
             "indirectly include this file:";
    }
    virtual QCString trSince()
    {
      return "Siden";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Graph Legend";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return 
        "This page explains how to interpret the graphs that are generated "
        "by doxygen.<p>\n"
        "Consider the following example:\n"
        "\\code\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
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
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "is set to 200 this will result in the following graph:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>A box with a black border denotes a documented struct or class.\n"
        "<li>A box with a grey border denotes an undocumented struct or class.\n"
        "<li>A box with a red border denotes a documented struct or class for\n"
        "which not all inheritance/containment relations are shown. A graph is "
        "truncated if it does not fit within the specified boundaries."
        "</ul>\n"
        "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.\n"
        "<li>A dark green arrow is used for protected inheritance.\n"
        "<li>A dark red arrow is used for private inheritance.\n"
        "<li>A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible. \n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "legend";
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
      return "Test-liste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Methods";
    }
};

#endif









