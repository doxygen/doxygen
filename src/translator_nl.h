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
    QString trListOfAllMembers()
    { return "Lijst van alle members."; }
    QString trMemberList()
    { return "Member Lijst"; }
    QString trThisIsTheListOfAllMembers()
    { return "Dit is de complete lijst van alle members voor"; }
    QString trIncludingInheritedMembers()
    { return ", inclusief alle overge&euml;rfde members."; }
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
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
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

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QString trRelatedFunctionDocumentation()
    { return "Documentatie van friends en gerelateerde functies"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QString result=(QString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
      }
      result+=" Referentie";
      return result;
    }
    virtual QString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QString result=fileName;
      result+=" File Referentie"; 
      return result;
    }
    virtual QString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QString result=namespaceName;
      result+=" Namespace Referentie";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QString trPublicMembers()
    { return "Public Members"; }
    virtual QString trPublicSlots()
    { return "Public Slots"; }
    virtual QString trSignals()
    { return "Signals"; }
    virtual QString trStaticPublicMembers()
    { return "Static Public Members"; }
    virtual QString trProtectedMembers()
    { return "Protected Members"; }
    virtual QString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QString trStaticProtectedMembers()
    { return "Static Protected Members"; }
    virtual QString trPrivateMembers()
    { return "Private Members"; }
    virtual QString trPrivateSlots()
    { return "Private Slots"; }
    virtual QString trStaticPrivateMembers()
    { return "Static Private Members"; }
    // end of member sections 
    
    virtual QString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
      QString result;
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
    
    virtual QString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Erft over van "+trWriteList(numEntries)+".";
    }
    virtual QString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Wordt overge&euml;rfd door "+trWriteList(numEntries)+".";
    }
    virtual QString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Nieuwe implementatie van "+trWriteList(numEntries)+".";
    }
    virtual QString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Opnieuw ge&iuml;mplementeerd in "+trWriteList(numEntries)+".";
    }

    virtual QString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    virtual QString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace members met links naar ";
      if (extractAll) 
        result+="de namespace documentatie voor iedere member:";
      else 
        result+="de namespaces waartoe ze behoren:";
      return result;
    }
    virtual QString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Namespace Index"; }
    virtual QString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentatie"; }
};

#endif
