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

#ifndef TRANSLATOR_NL_H
#define TRANSLATOR_NL_H

#include <qstring.h>

class TranslatorDutch : public Translator
{
  public:
    QString latexBabelPackage()
    { return "dutch"; }
    QString trInherits()
    { return "Erft over van"; }
    QString trAnd()
    { return "en"; }
    QString trInheritedBy()
    { return "Wordt overge&euml;rfd door"; }
    QString trRelatedFunctions()
    { return "Gerelateerde functies"; }
    QString trRelatedSubscript()
    { return "(Merk op dat dit geen member functies zijn.)"; }
    QString trDetailedDescription()
    { return "Gedetaileerde Beschrijving"; }
    QString trMemberTypedefDocumentation()
    { return "Documentatie van type definitie members"; }
    QString trMemberEnumerationDocumentation()
    { return "Documentatie van enumeratie members"; }
    QString trEnumerationValueDocumentation()
    { return "Documentatie van enumeratie waarden"; }
    QString trMemberFunctionDocumentation()
    { return "Documentatie van functie members"; }
    QString trMemberDataDocumentation()
    { return "Documentatie van data members"; }
    QString trGeneratedFrom(const char *s,bool single)
    { 
      QString result=(QString)"De documentatie voor deze"+s+
                     " is gegenereerd op grond van de volgende file";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QString trMore()
    { return "Meer..."; }
    QString trReference()
    { return "Referentie"; }
    QString trListOfAllMembers()
    { return "Lijst van alle members."; }
    QString trMemberList()
    { return "Member Lijst"; }
    QString trThisIsTheListOfAllMembers()
    { return "Dit is de complete lijst van alle members voor"; }
    QString trIncludingInheritedMembers()
    { return "inclusief alle overge&euml;rfde members."; }
    QString trGeneratedAutomatically(const char *s)
    { QString result="Automatisch gegenereerd door Doxygen"; 
      if (s) result+=(QString)" voor "+s;
      result+=" uit de programmacode."; 
      return result;
    }
    QString trEnumName()
    { return "enum naam"; }
    QString trEnumValue()
    { return "enum waarde"; }
    QString trDefinedIn()
    { return "gedefinieerd in"; }
    QString trIncludeFile()
    { return "Include File"; }
    QString trVerbatimText(const char *f)
    { return (QString)"Dit is de letterlijke tekst van de include file "+f+"."; }
    QString trModules()
    { return "Modules"; }
    QString trClassHierarchy()
    { return "Klasse Hi&euml;rarchie"; }
    QString trCompoundList()
    { return "Compound Lijst"; }
    QString trFileList()
    { return "File Lijst"; }
    QString trHeaderFiles()
    { return "Header Lijst"; }
    QString trCompoundMembers()
    { return "Compound Members"; }
    QString trFileMembers()
    { return "File members"; }
    QString trRelatedPages()
    { return "Gerelateerde pagina's"; }
    QString trExamples()
    { return "Voorbeelden"; }
    QString trSearch()
    { return "Zoeken"; }
    QString trClassHierarchyDescription()
    { return "Deze inheritance lijst is min of meer alfabetisch "
             "gesorteerd:";
    }
    QString trFileListDescription(bool extractAll)
    {
      QString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="files, elk met een korte beschrijving:";
      return result;
    }
    QString trCompoundListDescription()
    { return "Hieronder volgen de klassen, structs en "
             "unions met voor elk een korte beschrijving:"; 
    }
    QString trCompoundMembersDescription(bool extractAll)
    {
      QString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="klasse members met links naar ";
      if (extractAll) result+="de klasse documentatie voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }
    QString trFileMembersDescription(bool extractAll)
    {
      QString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="file members met links naar ";
      if (extractAll) result+="de file documentatie voor elke member:";
      else result+="de files waartoe ze behoren:";
      return result;
    }
    QString trHeaderFilesDescription()
    { return "Hieronder volgen de header files die de API vormen:"; }
    QString trExamplesDescription()
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    QString trRelatedPagesDescription()
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde documentatie bevatten:"; }
    QString trModulesDescription()
    { return "Hieronder volgt de lijst met alle modules:"; }
    QString trNoDescriptionAvailable()
    { return "Geen korte beschrijving beschikbaar"; }

    QString trDocumentation()
    { return "Documentatie"; }
    QString trModuleIndex()
    { return "Module Index"; }
    QString trHierarchicalIndex()
    { return "Hi&euml;rarchische Index"; }
    QString trCompoundIndex()
    { return "Compound Index"; }
    QString trFileIndex() 
    { return "File Index"; }
    QString trModuleDocumentation()
    { return "Module Documentatie"; }
    QString trClassDocumentation()
    { return "Klasse Documentatie"; }
    QString trFileDocumentation()
    { return "File Documentatie"; }
    QString trExampleDocumentation()
    { return "Documentatie van voorbeelden"; }
    QString trPageDocumentation()
    { return "Documentatie van gerelateerde pagina's"; }
    QString trReferenceManual()
    { return "Naslagwerk"; }

    QString trDefines()
    { return "Defines"; }
    QString trFuncProtos()
    { return "Functie Prototypes"; }
    QString trTypedefs()
    { return "Typedefs"; }
    QString trEnumerations()
    { return "Enumeraties"; }
    QString trFunctions()
    { return "Functies"; }
    QString trVariables()
    { return "Variabelen"; }
    QString trEnumerationValues()
    { return "Enumeratie waarden"; }
    QString trReimplementedFrom()
    { return "Nieuwe implementatie van"; }
    QString trReimplementedIn()
    { return "Opnieuw ge&iuml;mplementeerd in"; }
    QString trAuthor()
    { return "auteur"; }
    QString trDefineDocumentation()
    { return "Documentatie van defines"; }
    QString trFunctionPrototypeDocumentation()
    { return "Documentatie van functie Prototypes"; }
    QString trTypedefDocumentation()
    { return "Documentatie van typedefs"; }
    QString trEnumerationTypeDocumentation()
    { return "Documentatie van enumeratie types"; }
    QString trFunctionDocumentation()
    { return "Documentatie van functies"; }
    QString trVariableDocumentation()
    { return "Documentatie van variabelen"; }
    QString trCompounds()
    { return "Compounds"; }
    QString trFiles()
    { return "Files"; }
    QString trGeneratedAt(const char *date,const char *projName)
    { 
      QString result=(QString)"Gegenereerd op "+date;
      if (projName) result+=(QString)" voor "+projName;
      result+=(QString)" door";
      return result;
    }
    QString trWrittenBy()
    {
      return "geschreven door";
    }
    QString trClassDiagram(const char *clName)
    {
      return (QString)"Klasse diagram voor "+clName;
    }
    QString trForInternalUseOnly()
    { return "Alleen voor intern gebruik."; }
    QString trReimplementedForInternalReasons()
    { return "Om interne rederene opnieuwd ge&iuml;mplemented; "
             "de API wordt er niet door be&iuml;nvloed."; 
    }
    QString trWarning()
    { return "Waarschuwing"; }
    QString trBugsAndLimitations()
    { return "Fouten en beperkingen"; }
    QString trVersion()
    { return "Versie"; }
    QString trDate()
    { return "Datum"; }
    QString trAuthors()
    { return "Auteur(s)"; }
    QString trReturns()
    { return "Retourneerd"; }
    QString trSeeAlso()
    { return "Zie ook"; }
    QString trParameters()
    { return "Parameters"; }
    QString trExceptions()
    { return "Excepties"; }
    QString trGeneratedBy()
    { return "Gegenereerd door"; }
    
    // new since 0.49-990307 
    
    QString trNamespaces()
    { return "Namespaces"; }
    QString trNamespaceList()
    { return "Namespace Lijst"; }
    QString trNamespaceListDescription(bool extractAll)
    {
      QString result="Hier is een lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespaces met voor elk een korte beschrijving:";
      return result;
    }
    QString trFriends()
    { return "Friends"; }
};

#endif
