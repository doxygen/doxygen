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

#ifndef TRANSLATOR_CZ_H
#define TRANSLATOR_CZ_H

#include "translator.h"

// The translation from English to Czech by Vlastimil Havran.
// In the cases where are more translations possible I hope
// that I have selected the most reasonable text. If not, please,
// mail the comments and text proposals to
//
// havran@fel.cvut.cz, 1999/04/11

class TranslatorCzech : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "czech"; }
    QCString trInherits()
    { return "Dedi"; }
    QCString trAnd()
    { return "a"; }
    QCString trInheritedBy()
    { return "Je potomkem"; }
    QCString trRelatedFunctions()
    { return "Pribuzne metody"; }
    QCString trRelatedSubscript()
    { return "(Tyto funkce nejsou metody.)"; }
    QCString trDetailedDescription()
    { return "Podrobna dokumentace"; }
    QCString trMemberTypedefDocumentation()
    { return "Dokumentace pro deklaraci typu (typedef) ve tride";}
    QCString trMemberEnumerationDocumentation()
    { return "Dokumentace vyctovych typu"; }
    QCString trEnumerationValueDocumentation()
    { return "Dokumentace hodnot vyctovych typu"; }
    QCString trMemberFunctionDocumentation()
    { return "Dokumentace metod"; }
    QCString trMemberDataDocumentation()
    { return "Dokumentace datovych slozek tridy"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"Dokumentace pro tento"+s+
                     " byla generovana z nasledujiciho souboru";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QCString trMore()
    { return "Detaily"; }
    QCString trReference()
    { return "Reference"; }
    QCString trListOfAllMembers()
    { return "Seznam vsech datovych polozek a metod tridy."; }
    QCString trMemberList()
    { return "Seznam datovych polozek a metod tridy"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Toto je uplny seznam datovych polozek a metod tridy pro"; }
    QCString trIncludingInheritedMembers()
    { return "zahrnuje vsechny nasledujici zdedene datove polozky a metody."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automaticky vygenerovany pomoci programu doxygen"; 
      if (s) result+=(QCString)" pro "+s;
      result+=" ze zdrojoveho souboru."; 
      return result;
    }
    QCString trEnumName()
    { return "pojmenovani vyctoveho typu"; }
    QCString trEnumValue()
    { return "hodnota vyctoveho typu"; }
    QCString trDefinedIn()
    { return "definovany v"; }
    QCString trIncludeFile()
    { return "Vklada soubor"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Toto je presny text z vlozeneho souboru "+f+""; }
    QCString trModules()
    { return "Moduly"; }
    QCString trClassHierarchy()
    { return "Hierarchie trid"; }
    QCString trCompoundList()
    { return "Seznam objektovych typu"; }
    QCString trFileList()
    { return "Seznam souboru"; }
    QCString trHeaderFiles()
    { return "Hlavickove soubory"; }
    QCString trCompoundMembers()
    { return "Datove slozky a metody objektovych typu"; }
    QCString trFileMembers()
    { return "Globalni deklarace"; }
    QCString trRelatedPages()
    { return "Souvisejici stranky"; }
    QCString trExamples()
    { return "Priklady:"; }
    QCString trSearch()
    { return "Hledej"; }
    QCString trClassHierarchyDescription()
    { return "Tento seznam dedicnych zavislosti je temer setriden"
	     " podle abecedy:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="A toto je seznam vsech ";
      if (!extractAll) result+="zdokumentovanych ";
      result+="souboru s kratkymi popisy:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Zde jsou tridy, struktury a "
             "unie s kratkymi popisy:";
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Zde je seznam vsech ";
      if (!extractAll) result+="zdokumentovanych ";
      result+="clenu tridy (metod a datovych slozek) s odkazy na ";
      if (extractAll) result+="dokumentaci tridy pro kazdo polozku:";
      else result+="tridy, kam patri:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Zde je seznam vsech ";
      if (!extractAll) result+="zdokumentovanych ";
      result+="globalnich deklaraci s odkazy ";
      if (extractAll) result+="na dokumentaci k souboru pro kazdou deklaraci:";
      else result+="na soubory, kde jsou umisteny:";
      return result;
    }
    QCString trHeaderFilesDescription()
    {return "Zde jsou hlavickove soubory ktere tvori programove"
            " rozhrani aplikace(API):";
    }
    QCString trExamplesDescription()
    { return "Zde je seznam vsech prikladu:"; }
    QCString trRelatedPagesDescription()
    { return "Zde je seznam vsech souvisejicich stranek dokumentace:"; }
    QCString trModulesDescription()
    { return "Zde je seznam vsech modulu:"; }
    QCString trNoDescriptionAvailable()
    { return "Zadny popis neni k dispozici."; }

    QCString trDocumentation()
    { return "Dokumentace"; }
    QCString trModuleIndex()
    { return "Index modulu"; }
    QCString trHierarchicalIndex()
    { return "Index hierarchie"; }
    QCString trCompoundIndex()
    { return "Index objektovych typu"; }
    QCString trFileIndex() 
    { return "Index souboru"; }
    QCString trModuleDocumentation()
    { return "Dokumentace modulu"; }
    QCString trClassDocumentation()
    { return "Dokumentace tridy"; }
    QCString trFileDocumentation()
    { return "Dokumentace souboru"; }
    QCString trExampleDocumentation()
    { return "Dokumentace prikladu"; }
    QCString trPageDocumentation()
    { return "Dokumentace stranek"; }
    QCString trReferenceManual()
    { return "Referencni manual"; }

    QCString trDefines()
    { return "Definice"; }
    QCString trFuncProtos()
    { return "Prototypy funkci"; }
    QCString trTypedefs()
    { return "Deklarace datovych typu(typedefs)"; }
    QCString trEnumerations()
    { return "Vyctove typy (enumerations)"; }
    QCString trFunctions()
    { return "Funkce"; }
    QCString trVariables()
    { return "Promenne"; }
    QCString trEnumerationValues()
    { return "Hodnoty vyctovych typu"; }
    QCString trReimplementedFrom()
    { return "Je znovu implementovan podle"; }
    QCString trReimplementedIn()
    { return "Je znovu implementovan v"; }
    QCString trAuthor()
    { return "Autor"; }
    QCString trDefineDocumentation()
    { return "Dokumentace definic pomoci maker"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Dokumentace prototypu funkci"; }
    QCString trTypedefDocumentation()
    { return "Dokumentace deklaraci datovych typu(typedefs)"; }
    QCString trEnumerationTypeDocumentation()
    { return "Dokumentace vyctovych typu"; }
    QCString trFunctionDocumentation()
    { return "Dokumentace funkci"; }
    QCString trVariableDocumentation()
    { return "Dokumentace promennych"; }
    QCString trCompounds()
    { return "Polozky objektovych typu"; }
    QCString trFiles()
    { return "Soubory:"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generovany dne "+date;
      if (projName) result+=(QCString)" pro "+projName;
      result+=(QCString)" ";
      return result;
    }
    QCString trWrittenBy()
    {
      return "napsany podle";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagram trid pro "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Jen pro interni pouziti."; }
    QCString trReimplementedForInternalReasons()
    {
      return "Znovu implementovany z internich duvodu; programove rozhrani"
             " aplikace(API) neni zmeneno.";
    }
    QCString trWarning()
    { return "Upozorneni"; }
    QCString trBugsAndLimitations()
    { return "Chyby a omezeni"; }
    QCString trVersion()
    { return "Verze"; }
    QCString trDate()
    { return "Datum"; }
    QCString trAuthors()
    { return "Autor(i)"; }
    QCString trReturns()
    { return "ma navratovou hodnotu"; }
    QCString trSeeAlso()
    { return "Podrobnosti take tady"; }
    QCString trParameters()
    { return "Parametry"; }
    QCString trExceptions()
    { return "Vyjimky"; }
    QCString trGeneratedBy()
    { return "Generovan podle"; }
    
    // new since 0.49-990307 
    
    QCString trNamespaceList()
    { return "Seznam prostoru jmen"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Zde je seznam vsech ";
      if (!extractAll) result+="dokumentovanych ";
      result+="prostoru jmen s kratkymi popisy:";
      return result;
    }
    QCString trFriends()
    { return "Spratelene tridy, funkce a metody"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    QCString trRelatedFunctionDocumentation()
    { return "Dokumentace pro spratelene funkce a ostatni"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
    {
      QCString result=(QCString)clName+" ";
      result+=" Reference";
      switch(compType)
      {
        case ClassDef::Class:      result+=" tridy"; break;
        case ClassDef::Struct:     result+=" struktury"; break;
        case ClassDef::Union:      result+=" unie"; break;
        case ClassDef::Interface:  result+=" rozhrani"; break;
      }
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Reference souboru"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Reference prostoru jmen";
      return result;
    }
    
    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union 
     */
    QCString trPublicMembers()
    { return "Verejne metody a datove polozky"; }
    QCString trPublicSlots()
    { return "Verejne sloty"; }
    QCString trSignals()
    { return "Signaly"; }
    QCString trStaticPublicMembers()
    { return "Staticke verejne metody a datove polozky"; }
    QCString trProtectedMembers()
    { return "Chranene metody a datove polozky"; }
    QCString trProtectedSlots()
    { return "Chranene sloty"; }
    QCString trStaticProtectedMembers()
    { return "Staticke chranene metody a datove polozky"; }
    QCString trPrivateMembers()
    { return "Privatni metody a datove polozky"; }
    QCString trPrivateSlots()
    { return "Privatni sloty"; }
    QCString trStaticPrivateMembers()
    { return "Staticke privatni metody a datove polozky"; }
    /*! \endmgroup */ 
    
    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries)
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
            result+=" a ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries)
    {
      return "dedi "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries)
    {
      return "je zdeden z "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries)
    {
      return "Znovu implementuje "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries)
    {
      return "znovu implementovan v "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers()
    { return "Polozky prostoru jmen"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Tady je seznam vsech ";
      if (!extractAll) result+="zdokumentovanych ";
      result+="clenu prostoru jmen s odkazy na ";
      if (extractAll) 
        result+="dukumentaci prostoru jmen pro kazdou polozku:";
      else 
        result+="prostor jmen, kteremu patri:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "Index prostoru jmen"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "Dokumentace prostoru jmen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "Prostory jmen"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentace pro ";
      switch(compType)
      {
        case ClassDef::Class:      result+="tuto tridu"; break;
        case ClassDef::Struct:     result+="tuto struktury"; break;
        case ClassDef::Union:      result+="tuto unii"; break;
        case ClassDef::Interface:  result+="toto rozhrani"; break;
      }
      result+=" byla vygenerovana z nasledujiciho souboru";
      if (single) result+=":"; else result+="s:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "Abecedni seznam"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Vraci hodnoty"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "Hlavni stranka"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "p."; }
};

#endif // TRANSLATOR_CZ_H


