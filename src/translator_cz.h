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

#include <qstring.h>

// The translation from English to Czech by Vlastimil Havran.
// In the cases where are more translations possible I hope
// that I have selected the most reasonable text. If not, please,
// mail the comments and text proposals to
//
// havran@fel.cvut.cz, 1999/04/11

class TranslatorCzech : public Translator
{
  public:
    QString latexBabelPackage()
    { return "czech"; }
    QString trInherits()
    { return "Dedi"; }
    QString trAnd()
    { return "a"; }
    QString trInheritedBy()
    { return "Je Potomkem"; }
    QString trRelatedFunctions()
    { return "Pribuzne Metody"; }
    QString trRelatedSubscript()
    { return "(Tyto funkce nejsou metody.)"; }
    QString trDetailedDescription()
    { return "Podrobna Dokumentace"; }
    QString trMemberTypedefDocumentation()
    { return "Dokumentace pro Deklaraci Typu (typedef) ve Tride";}
    QString trMemberEnumerationDocumentation()
    { return "Dokumentace Vyctovych Typu"; }
    QString trEnumerationValueDocumentation()
    { return "Dokumentace Hodnot Vyctovych Typu"; }
    QString trMemberFunctionDocumentation()
    { return "Dokumentace Metod"; }
    QString trMemberDataDocumentation()
    { return "Dokumentace Datovych Slozek Tridy"; }
    QString trGeneratedFrom(const char *s,bool single)
    { 
      QString result=(QString)"Dokumentace pro tento"+s+
                     " byla generovana z nasledujiciho souboru";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QString trMore()
    { return "Detaily"; }
    QString trReference()
    { return "Reference"; }
    QString trListOfAllMembers()
    { return "Seznam vsech datovych slozek a metod tridy."; }
    QString trMemberList()
    { return "Seznam datovych polozek a metod tridy"; }
    QString trThisIsTheListOfAllMembers()
    { return "Toto je uplny seznam datovych slozek a metod tridy pro"; }
    QString trIncludingInheritedMembers()
    { return "zahrnuje vsechny nasledujici zdedene datove slozky a metody."; }
    QString trGeneratedAutomatically(const char *s)
    { QString result="Automaticky vygenerovany pomoci programu Doxygen"; 
      if (s) result+=(QString)" pro "+s;
      result+=" ze zdrojoveho souboru."; 
      return result;
    }
    QString trEnumName()
    { return "pojmenovani vyctoveho typu"; }
    QString trEnumValue()
    { return "hodnota vyctoveho typu"; }
    QString trDefinedIn()
    { return "definovany v"; }
    QString trIncludeFile()
    { return "Vklada soubor"; }
    QString trVerbatimText(const char *f)
    { return (QString)"Toto je presny text z vlozeneho souboru "+f+""; }
    QString trModules()
    { return "Moduly"; }
    QString trClassHierarchy()
    { return "Hierarchie trid"; }
    QString trCompoundList()
    { return "Seznam objektovych typu"; }
    QString trFileList()
    { return "Seznam souboru"; }
    QString trHeaderFiles()
    { return "Hlavickove soubory"; }
    QString trCompoundMembers()
    { return "Datove slozky a metody objektovych typu"; }
    QString trFileMembers()
    { return "Globalni deklarace"; }
    QString trRelatedPages()
    { return "Souvisejici stranky"; }
    QString trExamples()
    { return "Priklady:"; }
    QString trSearch()
    { return "Hledej"; }
    QString trClassHierarchyDescription()
    { return "Tento seznam dedicnych zavislosti je temer setriden"
	     " podle abecedy:";
    }
    QString trFileListDescription(bool extractAll)
    {
      QString result="A toto je seznam vsech ";
      if (!extractAll) result+="dokumentovanych ";
      result+="souboru s kratkymi popisy:";
      return result;
    }
    QString trCompoundListDescription()
    { return "Zde jsou tridy, struktury a "
             "unie s kratkymi popisy:";
    }
    QString trCompoundMembersDescription(bool extractAll)
    {
      QString result="Zde je seznam vsech ";
      if (!extractAll) result+="dokumentovanych ";
      result+="clenu tridy (metod a datovych slozek) s odkazy na ";
      if (extractAll) result+="dokumentaci tridy pro kazdo polozku:";
      else result+="tridy, kam patri:";
      return result;
    }
    QString trFileMembersDescription(bool extractAll)
    {
      QString result="Zde je seznam vsech ";
      if (!extractAll) result+="zdokumentovanych ";
      result+="globalnich deklaraci s odkazy ";
      if (extractAll) result+="na dokumentaci k souboru pro kazdou deklaraci:";
      else result+="na soubory, kde jsou umisteny:";
      return result;
    }
    QString trHeaderFilesDescription()
    {return "Zde jsou hlavickove soubory ktere tvori programove"
            " rozhrani aplikace(API):";
    }
    QString trExamplesDescription()
    { return "Zde je seznam vsech prikladu:"; }
    QString trRelatedPagesDescription()
    { return "Zde je seznam vsech souvisejicich stranek dokumentace:"; }
    QString trModulesDescription()
    { return "Zde je seznam vsech modulu:"; }
    QString trNoDescriptionAvailable()
    { return "Zadny popis neni k dispozici."; }

    QString trDocumentation()
    { return "Dokumentace"; }
    QString trModuleIndex()
    { return "Index modulu"; }
    QString trHierarchicalIndex()
    { return "Index Hierarchie"; }
    QString trCompoundIndex()
    { return "Index objektovych typu"; }
    QString trFileIndex() 
    { return "Index souboru"; }
    QString trModuleDocumentation()
    { return "Dokumentace modulu"; }
    QString trClassDocumentation()
    { return "Dokumentace tridy"; }
    QString trFileDocumentation()
    { return "Dokumentace souboru"; }
    QString trExampleDocumentation()
    { return "Dokumentace prikladu"; }
    QString trPageDocumentation()
    { return "Dokumentace stranek"; }
    QString trReferenceManual()
    { return "Referencni manual"; }

    QString trDefines()
    { return "Definice"; }
    QString trFuncProtos()
    { return "Prototypy funkci"; }
    QString trTypedefs()
    { return "Deklarace datovych typu(typedefs)"; }
    QString trEnumerations()
    { return "Vyctove typy (enumerations)"; }
    QString trFunctions()
    { return "Funkce"; }
    QString trVariables()
    { return "Promenne"; }
    QString trEnumerationValues()
    { return "Hodnoty vyctovych typu"; }
    QString trReimplementedFrom()
    { return "Je znovu implementovan z"; }
    QString trReimplementedIn()
    { return "Je znovu implementovan v"; }
    QString trAuthor()
    { return "Autor"; }
    QString trDefineDocumentation()
    { return "Dokumentace definic pomoci maker"; }
    QString trFunctionPrototypeDocumentation()
    { return "Dokumentace prototypu funkci"; }
    QString trTypedefDocumentation()
    { return "Dokumentace deklaraci datovych typu(typedefs)"; }
    QString trEnumerationTypeDocumentation()
    { return "Dokumentace vyctovych typu"; }
    QString trFunctionDocumentation()
    { return "Dokumentace funkci"; }
    QString trVariableDocumentation()
    { return "Dokumentace promennych"; }
    QString trCompounds()
    { return "Polozky objektovych typu"; }
    QString trFiles()
    { return "Soubory:"; }
    QString trGeneratedAt(const char *date,const char *projName)
    { 
      QString result=(QString)"Gegenerovany v "+date;
      if (projName) result+=(QString)" pro "+projName;
      result+=(QString)" ";
      return result;
    }
    QString trWrittenBy()
    {
      return "napsany  ";
    }
    QString trClassDiagram(const char *clName)
    {
      return (QString)"Diagram trid pro "+clName;
    }
    QString trForInternalUseOnly()
    { return "Jen pro interni pouziti."; }
    QString trReimplementedForInternalReasons()
    {
      return "Znovu implementovany z internich duvodu; programove rozhrani"
             " aplikace(API) neni zmeneno.";
    }
    QString trWarning()
    { return "Upozorneni"; }
    QString trBugsAndLimitations()
    { return "Chyby a omezeni"; }
    QString trVersion()
    { return "Verze"; }
    QString trDate()
    { return "Datum"; }
    QString trAuthors()
    { return "Autor(i)"; }
    QString trReturns()
    { return "ma navratovou hodnotu"; }
    QString trSeeAlso()
    { return "Podivej se take na"; }
    QString trParameters()
    { return "Parametry"; }
    QString trExceptions()
    { return "Vyjimky"; }
    QString trGeneratedBy()
    { return "Gegenerovan podle"; }
    
    // new since 0.49-990307 
    
    QString trNamespaces()
    { return "Prostory jmen"; }
    QString trNamespaceList()
    { return "Seznam prostoru jmen"; }
    QString trNamespaceListDescription(bool extractAll)
    {
      QString result="Zde jsou vsechny ";
      if (!extractAll) result+="dokumentovane ";
      result+="prostory jem s kratkymi popisy:";
      return result;
    }
    QString trFriends()
    { return "Spratelene tridy, funkce a metody"; }
};

#endif // TRANSLATOR_CZ_H


