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

#include "translator.h"

class TranslatorDutch : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "dutch"; }
    QCString trRelatedFunctions()
    { return "Gerelateerde functies"; }
    QCString trRelatedSubscript()
    { return "(Merk op dat dit geen member functies zijn.)"; }
    QCString trDetailedDescription()
    { return "Gedetaileerde Beschrijving"; }
    QCString trMemberTypedefDocumentation()
    { return "Documentatie van type definitie members"; }
    QCString trMemberEnumerationDocumentation()
    { return "Documentatie van enumeratie members"; }
    QCString trEnumerationValueDocumentation()
    { return "Documentatie van enumeratie waarden"; }
    QCString trMemberFunctionDocumentation()
    { return "Documentatie van functie members"; }
    QCString trMemberDataDocumentation()
    { return "Documentatie van data members"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"De documentatie voor deze"+s+
                     " is gegenereerd op grond van de volgende file";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QCString trMore()
    { return "Meer..."; }
    QCString trListOfAllMembers()
    { return "Lijst van alle members."; }
    QCString trMemberList()
    { return "Member Lijst"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Dit is de complete lijst van alle members voor"; }
    QCString trIncludingInheritedMembers()
    { return ", inclusief alle overge&euml;rfde members."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatisch gegenereerd door Doxygen"; 
      if (s) result+=(QCString)" voor "+s;
      result+=" uit de programmacode."; 
      return result;
    }
    QCString trEnumName()
    { return "enum naam"; }
    QCString trEnumValue()
    { return "enum waarde"; }
    QCString trDefinedIn()
    { return "gedefinieerd in"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Dit is de letterlijke tekst van de include file "+f+"."; }
    QCString trModules()
    { return "Modules"; }
    QCString trClassHierarchy()
    { return "Klasse Hi&euml;rarchie"; }
    QCString trCompoundList()
    { return "Compound Lijst"; }
    QCString trFileList()
    { return "File Lijst"; }
    QCString trHeaderFiles()
    { return "Header Lijst"; }
    QCString trCompoundMembers()
    { return "Compound Members"; }
    QCString trFileMembers()
    { return "File members"; }
    QCString trRelatedPages()
    { return "Gerelateerde pagina's"; }
    QCString trExamples()
    { return "Voorbeelden"; }
    QCString trSearch()
    { return "Zoeken"; }
    QCString trClassHierarchyDescription()
    { return "Deze inheritance lijst is min of meer alfabetisch "
             "gesorteerd:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="files, elk met een korte beschrijving:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Hieronder volgen de klassen, structs en "
             "unions met voor elk een korte beschrijving:"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="klasse members met links naar ";
      if (extractAll) result+="de klasse documentatie voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="file members met links naar ";
      if (extractAll) result+="de file documentatie voor elke member:";
      else result+="de files waartoe ze behoren:";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Hieronder volgen de header files die de API vormen:"; }
    QCString trExamplesDescription()
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    QCString trRelatedPagesDescription()
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde documentatie bevatten:"; }
    QCString trModulesDescription()
    { return "Hieronder volgt de lijst met alle modules:"; }
    QCString trNoDescriptionAvailable()
    { return "Geen korte beschrijving beschikbaar"; }

    QCString trDocumentation()
    { return "Documentatie"; }
    QCString trModuleIndex()
    { return "Module Index"; }
    QCString trHierarchicalIndex()
    { return "Hi&euml;rarchische Index"; }
    QCString trCompoundIndex()
    { return "Compound Index"; }
    QCString trFileIndex() 
    { return "File Index"; }
    QCString trModuleDocumentation()
    { return "Module Documentatie"; }
    QCString trClassDocumentation()
    { return "Klasse Documentatie"; }
    QCString trFileDocumentation()
    { return "File Documentatie"; }
    QCString trExampleDocumentation()
    { return "Documentatie van voorbeelden"; }
    QCString trPageDocumentation()
    { return "Documentatie van gerelateerde pagina's"; }
    QCString trReferenceManual()
    { return "Naslagwerk"; }

    QCString trDefines()
    { return "Defines"; }
    QCString trFuncProtos()
    { return "Functie Prototypes"; }
    QCString trTypedefs()
    { return "Typedefs"; }
    QCString trEnumerations()
    { return "Enumeraties"; }
    QCString trFunctions()
    { return "Functies"; }
    QCString trVariables()
    { return "Variabelen"; }
    QCString trEnumerationValues()
    { return "Enumeratie waarden"; }
    QCString trAuthor()
    { return "auteur"; }
    QCString trDefineDocumentation()
    { return "Documentatie van defines"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Documentatie van functie Prototypes"; }
    QCString trTypedefDocumentation()
    { return "Documentatie van typedefs"; }
    QCString trEnumerationTypeDocumentation()
    { return "Documentatie van enumeratie types"; }
    QCString trFunctionDocumentation()
    { return "Documentatie van functies"; }
    QCString trVariableDocumentation()
    { return "Documentatie van variabelen"; }
    QCString trCompounds()
    { return "Compounds"; }
    QCString trFiles()
    { return "Files"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Gegenereerd op "+date;
      if (projName) result+=(QCString)" voor "+projName;
      result+=(QCString)" door";
      return result;
    }
    QCString trWrittenBy()
    {
      return "geschreven door";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klasse diagram voor "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Alleen voor intern gebruik."; }
    QCString trReimplementedForInternalReasons()
    { return "Om interne rederene opnieuwd ge&iuml;mplemented; "
             "de API wordt er niet door be&iuml;nvloed."; 
    }
    QCString trWarning()
    { return "Waarschuwing"; }
    QCString trBugsAndLimitations()
    { return "Fouten en beperkingen"; }
    QCString trVersion()
    { return "Versie"; }
    QCString trDate()
    { return "Datum"; }
    QCString trAuthors()
    { return "Auteur(s)"; }
    QCString trReturns()
    { return "Retourneerd"; }
    QCString trSeeAlso()
    { return "Zie ook"; }
    QCString trParameters()
    { return "Parameters"; }
    QCString trExceptions()
    { return "Excepties"; }
    QCString trGeneratedBy()
    { return "Gegenereerd door"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return "Namespace Lijst"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Hier is een lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespaces met voor elk een korte beschrijving:";
      return result;
    }
    QCString trFriends()
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "Documentatie van friends en gerelateerde functies"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
      }
      result+=" Referentie";
      return result;
    }
    virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" File Referentie"; 
      return result;
    }
    virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Namespace Referentie";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QCString trPublicMembers()
    { return "Public Members"; }
    virtual QCString trPublicSlots()
    { return "Public Slots"; }
    virtual QCString trSignals()
    { return "Signals"; }
    virtual QCString trStaticPublicMembers()
    { return "Static Public Members"; }
    virtual QCString trProtectedMembers()
    { return "Protected Members"; }
    virtual QCString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Static Protected Members"; }
    virtual QCString trPrivateMembers()
    { return "Private Members"; }
    virtual QCString trPrivateSlots()
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Static Private Members"; }
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
            result+=" en ";
        }
      }
      return result; 
    }
    
    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Erft over van "+trWriteList(numEntries)+".";
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Wordt overge&euml;rfd door "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Nieuwe implementatie van "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Opnieuw ge&iuml;mplementeerd in "+trWriteList(numEntries)+".";
    }

    virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace members met links naar ";
      if (extractAll) 
        result+="de namespace documentatie voor iedere member:";
      else 
        result+="de namespaces waartoe ze behoren:";
      return result;
    }
    virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Namespace Index"; }
    virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentatie"; }
};

#endif
