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

#ifndef TRANSLATOR_IT_H
#define TRANSLATOR_IT_H

#include <qstring.h>

class TranslatorItalian : public Translator
{
  public:
    QString latexBabelPackage()
    { return "italian"; }
    QString trInherits()
    { return "eredita"; }
    QString trAnd()
    { return "e"; }
    QString trInheritedBy()
    { return "Ereditato da"; }
    QString trRelatedFunctions()
    { return "Funzioni relative"; }
    QString trRelatedSubscript()
    { return "(Nota che questo no sono funzioni membro)"; }
    QString trDetailedDescription()
    { return "Descrizione dettagliata"; }
    QString trMemberTypedefDocumentation()
    { return "Documentazione  dei membri Typedef"; }
    QString trMemberEnumerationDocumentation()
    { return "Documentazione dei membri enumerati"; }
    QString trEnumerationValueDocumentation()
    { return "Documentazione dei tipi enumerati"; }
    QString trMemberFunctionDocumentation()
    { return "Documentazione delle funzioni membro"; }
    QString trMemberDataDocumentation()
    { return "Documentazione dei dati membro"; }
    QString trGeneratedFrom(const char *s,bool single)
    { 
      QString result=(QString)"La documentazione di questa "+s+
                     " e stata generata dal seguente file";
      if (single) result+=":";   else result+=":";
      return result;
    }
    QString trMore()
    { return "Continua..."; }
    QString trReference()
    { return "Riferimento"; }
    QString trListOfAllMembers()
    { return "Lista di tuti i memberi."; }
    QString trMemberList()
    { return "Lista membri"; }
    QString trThisIsTheListOfAllMembers()
    { return "Questa e la lista completa di tutti i membri"; }
    QString trIncludingInheritedMembers()
    { return "includendo tutti i membri ereditati."; }
    QString trGeneratedAutomatically(const char *s)
    { QString result="Generato automaticalmenta da Doxygen"; 
      if (s) result+=(QString)" per "+s;
      result+=" dall codice."; 
      return result;
    }
    QString trEnumName()
    { return "nome dell enum"; }
    QString trEnumValue()
    { return "valore dell enum"; }
    QString trDefinedIn()
    { return "definito in"; }
    QString trIncludeFile()
    { return "Include il file"; }
    QString trVerbatimText(const char *f)
    { return (QString)"Questo e il contenuto verbatim dell file "+f+"."; }
    QString trModules()
    { return "Moduli"; }
    QString trClassHierarchy()
    { return "gerarchia delle classi"; }
    QString trCompoundList()
    { return "Lista dei tipi composti"; }
    QString trFileList()
    { return "Lista dei file"; }
    QString trHeaderFiles()
    { return "List dei fei file header"; }
    QString trCompoundMembers()
    { return "Membri composti"; }
    QString trFileMembers()
    { return "Membri dell file"; }
    QString trRelatedPages()
    { return "Relativo a pagina"; }
    QString trExamples()
    { return "Esempio"; }
    QString trSearch()
    { return "Ricerca"; }
    QString trClassHierarchyDescription()
    { return "Qua sono la lista delle classi, structs e unions"
             "gesorte:";
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
      if (extractAll) result+="de klasse Documentazione voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }
    QString trFileMembersDescription(bool extractAll)
    {
      QString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="file members met links naar ";
      if (extractAll) result+="de file Documentazione voor elke member:";
      else result+="de files waartoe ze behoren:";
      return result;
    }
    QString trHeaderFilesDescription()
    { return "Hieronder volgen de header files die de API vormen:"; }
    QString trExamplesDescription()
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    QString trRelatedPagesDescription()
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde Documentazione bevatten:"; }
    QString trModulesDescription()
    { return "Hieronder volgt de lijst met alle modules:"; }
    QString trNoDescriptionAvailable()
    { return "Geen korte beschrijving beschikbaar"; }

    QString trDocumentation()
    { return "Documentazione"; }
    QString trModuleIndex()
    { return "Module Index"; }
    QString trHierarchicalIndex()
    { return "Hi&euml;rarchische Index"; }
    QString trCompoundIndex()
    { return "Compound Index"; }
    QString trFileIndex() 
    { return "File Index"; }
    QString trModuleDocumentation()
    { return "Module Documentazione"; }
    QString trClassDocumentation()
    { return "Klasse Documentazione"; }
    QString trFileDocumentation()
    { return "File Documentazione"; }
    QString trExampleDocumentation()
    { return "Documentazione di voorbeelden"; }
    QString trPageDocumentation()
    { return "Documentazione di gerelateerde pagina's"; }
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
    { return "Nieuwe implementatie di"; }
    QString trReimplementedIn()
    { return "Opnieuw ge&iuml;mplementeerd in"; }
    QString trAuthor()
    { return "auteur"; }
    QString trDefineDocumentation()
    { return "Lista "; }
    QString trFunctionPrototypeDocumentation()
    { return "Documentazione di functie Prototypes"; }
    QString trTypedefDocumentation()
    { return "Documentazione di typedefs"; }
    QString trEnumerationTypeDocumentation()
    { return "Documentazione di enumeratie types"; }
    QString trFunctionDocumentation()
    { return "Documentazione di functies"; }
    QString trVariableDocumentation()
    { return "Lista  variabelen"; }
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
