/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Initial Italian Translation by Ahmed Aldo Faisal
 * Revised and completed by Alessandro Falappa  (June 1999)
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

#include "translator.h"

class TranslatorItalian : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "italian"; }
    QCString trInherits()
    { return "eredita"; }
    QCString trAnd()
    { return "e"; }
    QCString trInheritedBy()
    //{ return "Ereditato da"; }
    { return "Ereditate da"; }
    QCString trRelatedFunctions()
    //{ return "Funzioni relative"; }
    { return "Funzioni collegate"; }
    QCString trRelatedSubscript()
    //{ return "(Nota che questo no sono funzioni membro)"; }
    { return "(Attenzione, queste non sono funzioni membro)"; }
    QCString trDetailedDescription()
    { return "Descrizione dettagliata"; }
    QCString trMemberTypedefDocumentation()
    //{ return "Documentazione dei membri Typedef"; }
    { return "Documentazione delle ridefinizioni dei tipi (typedef)"; }
    QCString trMemberEnumerationDocumentation()
    //{ return "Documentazione dei membri enumerati"; }
    { return "Documentazione dei tipi enumerati (enum)"; }
    QCString trEnumerationValueDocumentation()
    { return "Documentazione dei valori dei tipi enumerati"; }
    QCString trMemberFunctionDocumentation()
    { return "Documentazione delle funzioni membro"; }
    QCString trMemberDataDocumentation()
    { return "Documentazione dei dati membro"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"La documentazione di questa "+s+
                     " è stata generata a partire";
      if (single) result+=" dal seguente file:";
	  else result+=" dai seguenti files:";
      return result;
    }
    QCString trMore()
    { return "Continua..."; }
	QCString trReference()
    { return "Riferimento"; }
    QCString trListOfAllMembers()
    { return "Lista di tutti i membri."; }
    QCString trMemberList()
    { return "Lista dei membri"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Questa è la lista completa di tutti i membri di "; }
    QCString trIncludingInheritedMembers()
    { return ", includendo tutti i membri ereditati."; }
    QCString trGeneratedAutomatically(const char *s)
    {
	  QCString result="Generato automaticamente da Doxygen"; 
      if (s) result+=(QCString)" per "+s;
      result+=" a partire dal codice sorgente."; 
      return result;
    }
    QCString trEnumName()
    { return "nome di tipo enumerato"; }
    QCString trEnumValue()
    { return "valore di tipo enumerato"; }
    QCString trDefinedIn()
    { return "definito in"; }
    QCString trIncludeFile()
    { return "Include il file"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Questo è il contenuto integrale dell file "+f+"."; }
    QCString trModules()
    { return "Moduli"; }
    QCString trClassHierarchy()
    { return "Gerarchia delle classi"; }
    QCString trCompoundList()
    { return "Lista dei composti"; }
    QCString trFileList()
    { return "Lista dei files"; }
    QCString trHeaderFiles()
    { return "Lista dei file header"; }
    QCString trCompoundMembers()
    { return "Membri dei composti"; }
    QCString trFileMembers()
    { return "Membri dei files"; }
    QCString trRelatedPages()
    { return "Pagine collegate"; }
    QCString trExamples()
    { return "Esempi"; }
    QCString trSearch()
    { return "Cerca"; }
    QCString trClassHierarchyDescription()
    {
	return "Questa lista di ereditarietà è ordinata "
	"approssimativamente, ma non completamente, in ordine alfabetico:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Questa è una lista ";
      if (!extractAll) result+="dei files documentati ";
	  else result+="di tutti i files ";
      result+="con una loro breve descrizione:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Queste sono le classi, structs e "
             "unions con una loro breve descrizione:"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Questa è una lista ";
      if (!extractAll) result+="dei membri documentati, ";
	  else result+="di tutti i membri ";
      result+="con collegamenti ";
      if (extractAll) result+="alla documentazione della classe di ciascun membro:";
      else result+="alla documentazione delle classi a cui appartengono:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Questa è una lista ";
      if (!extractAll) result+="dei membri dei files documentati, ";
	  else result+="di tutti i membri dei files ";
      result+="con collegamenti ";
      if (extractAll) result+="alla documentazione del file di ciascun membro:";
      else result+="alla documentazione dei files a cui appartengono:";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Questi sono gli header files che compongono l'API:"; }
    QCString trExamplesDescription()
    { return "Questa è la lista di tutti gli esempi:"; }
    QCString trRelatedPagesDescription()
    { return "Questa è la lista di tutte le pagine di documentazione collegate fra loro:"; }
    QCString trModulesDescription()
    { return "Questa è la lista di tutti i moduli:"; }
    QCString trNoDescriptionAvailable()
    { return "Descrizione non disponibile"; }

    QCString trDocumentation()
    { return "Documentazione"; }
    QCString trModuleIndex()
    { return "Indice dei moduli"; }
    QCString trHierarchicalIndex()
    { return "Indice della gerarchia"; }
    QCString trCompoundIndex()
    { return "Indice dei composti"; }
    QCString trFileIndex() 
    { return "Indice dei files"; }
    QCString trModuleDocumentation()
    { return "Documentazione dei moduli"; }
    QCString trClassDocumentation()
    { return "Documentazione delle classi"; }
    QCString trFileDocumentation()
    { return "Documentazione dei files"; }
    QCString trExampleDocumentation()
    { return "Documentazione degli esempi"; }
    QCString trPageDocumentation()
    { return "Documentazione delle pagine"; }
    QCString trReferenceManual()
    { return "Manuale di riferimento"; }

    QCString trDefines()
    { return "Definizioni"; }
    QCString trFuncProtos()
    { return "Prototipi delle funzioni"; }
    QCString trTypedefs()
    { return "Typedefs"; }
    QCString trEnumerations()
    { return "Tipi enumerati"; }
    QCString trFunctions()
    { return "Funzioni"; }
    QCString trVariables()
    { return "Variabili"; }
    QCString trEnumerationValues()
    { return "Valori dei tipi enumerati"; }
    QCString trReimplementedFrom()
    { return "Nuova implementazione di"; }
    QCString trReimplementedIn()
    { return "Reimplementato in"; }
    QCString trAuthor()
    { return "Autore"; }
    QCString trDefineDocumentation()
    { return "Documentazione delle definizioni"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Documentazione dei prototipi delle funzioni"; }
    QCString trTypedefDocumentation()
    { return "Documentazione dei typedefs"; }
    QCString trEnumerationTypeDocumentation()
    { return "Documentazione dei tipi enumerati"; }
    QCString trFunctionDocumentation()
    { return "Documentazione delle funzioni"; }
    QCString trVariableDocumentation()
    { return "Documentazione delle variabili"; }
    QCString trCompounds()
    { return "Composti"; }
    QCString trFiles()
    { return "Files"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generato il "+date;
      if (projName) result+=(QCString)" per "+projName;
      result+=(QCString)" da";
      return result;
    }
    QCString trWrittenBy()
    {
      return "scritto da";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagramma delle classi per "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Solo per uso interno."; }
    QCString trReimplementedForInternalReasons()
    { return "Reimplementato per motivi interni; "
             "l'API non è stata modificata."; 
    }
    QCString trWarning()
    { return "Attenzione"; }
    QCString trBugsAndLimitations()
    { return "Bugs e limitazioni"; }
    QCString trVersion()
    { return "Versione"; }
    QCString trDate()
    { return "Data"; }
    QCString trAuthors()
    { return "Autore(i)"; }
    QCString trReturns()
    { return "Restituisce"; }
    QCString trSeeAlso()
    { return "Vedi anche"; }
    QCString trParameters()
    { return "Parametri"; }
    QCString trExceptions()
    { return "Eccezioni"; }
    QCString trGeneratedBy()
    { return "Generato da"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaces()
    { return "Namespaces"; }
    QCString trNamespaceList()
    { return "Lista dei namespaces"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Questa è la lista ";
      if (!extractAll) result+="dei namespaces documentati, ";
	  else result+="di tutti i namespaces ";
      result+="con una loro breve descrizione:";
      return result;
    }
    QCString trFriends()
    { return "Friends"; }
	
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "Documentazione dei friends e delle funzioni collegate"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
    {
      QCString result="Riferimenti per la";
      switch(compType)
      {
        case ClassDef::Class:  result+=" classe"; break;
        case ClassDef::Struct: result+=" struct"; break;
        case ClassDef::Union:  result+=" union"; break;
        case ClassDef::Interface:  result+=" interface"; break;
      }
      result+=" "+(QCString)clName;
      return result;
    }
    QCString trFileReference(const char *fileName)
    {
      QCString result="Riferimenti per il file ";
      result+=(QCString)fileName; 
      return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Riferimenti per il namespace ";
      result+=(QCString)namespaceName;
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return "Membri pubblici"; }
    QCString trPublicSlots()
    { return "Slots pubblici"; }
    QCString trSignals()
    { return "Signals"; }
    QCString trStaticPublicMembers()
    { return "Membri pubblici statici"; }
    QCString trProtectedMembers()
    { return "Membri protetti"; }
    QCString trProtectedSlots()
    { return "Slots protetti"; }
    QCString trStaticProtectedMembers()
    { return "Membri protetti statici"; }
    QCString trPrivateMembers()
    { return "Membri privati"; }
    QCString trPrivateSlots()
    { return "Slots privati"; }
    QCString trStaticPrivateMembers()
    { return "Membri privati statici"; }
  
    
    QCString trInheritsList(int numEntries)
    {
      return "Eredita da "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
    {
      return "Base per "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementa "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementata in "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers()
    { return "Membri dei namespaces"; }
    QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Questa è la lista ";
      if (!extractAll) result+="dei membri dei namespaces documentati, ";
 	  else result+="di tutti i membri dei namespaces ";
      result+="con collegamenti ";
      if (extractAll) 
        result+="alla documentazione del namespace per ciascun membro:";
      else 
        result+="ai namespaces a cui appartengono:";
      return result;
    }
    QCString trNamespaceIndex()
    { return "Indice dei namespaces"; }
    QCString trNamespaceDocumentation()
    { return "Documentazione dei namespaces"; }
};

#endif
