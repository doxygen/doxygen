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
    { return "Je Potomkem"; }
    QCString trRelatedFunctions()
    { return "Pribuzne Metody"; }
    QCString trRelatedSubscript()
    { return "(Tyto funkce nejsou metody.)"; }
    QCString trDetailedDescription()
    { return "Podrobna Dokumentace"; }
    QCString trMemberTypedefDocumentation()
    { return "Dokumentace pro Deklaraci Typu (typedef) ve Tride";}
    QCString trMemberEnumerationDocumentation()
    { return "Dokumentace Vyctovych Typu"; }
    QCString trEnumerationValueDocumentation()
    { return "Dokumentace Hodnot Vyctovych Typu"; }
    QCString trMemberFunctionDocumentation()
    { return "Dokumentace Metod"; }
    QCString trMemberDataDocumentation()
    { return "Dokumentace Datovych Slozek Tridy"; }
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
    { return "Seznam vsech datovych slozek a metod tridy."; }
    QCString trMemberList()
    { return "Seznam datovych polozek a metod tridy"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Toto je uplny seznam datovych slozek a metod tridy pro"; }
    QCString trIncludingInheritedMembers()
    { return "zahrnuje vsechny nasledujici zdedene datove slozky a metody."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automaticky vygenerovany pomoci programu Doxygen"; 
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
      if (!extractAll) result+="dokumentovanych ";
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
      if (!extractAll) result+="dokumentovanych ";
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
    { return "Index Hierarchie"; }
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
    { return "Je znovu implementovan z"; }
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
      QCString result=(QCString)"Gegenerovany v "+date;
      if (projName) result+=(QCString)" pro "+projName;
      result+=(QCString)" ";
      return result;
    }
    QCString trWrittenBy()
    {
      return "napsany  ";
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
    { return "Podivej se take na"; }
    QCString trParameters()
    { return "Parametry"; }
    QCString trExceptions()
    { return "Vyjimky"; }
    QCString trGeneratedBy()
    { return "Gegenerovan podle"; }
    
    // new since 0.49-990307 
    
    QCString trNamespaces()
    { return "Prostory jmen"; }
    QCString trNamespaceList()
    { return "Seznam prostoru jmen"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Zde jsou vsechny ";
      if (!extractAll) result+="dokumentovane ";
      result+="prostory jem s kratkymi popisy:";
      return result;
    }
    QCString trFriends()
    { return "Spratelene tridy, funkce a metody"; }
};

#endif // TRANSLATOR_CZ_H


