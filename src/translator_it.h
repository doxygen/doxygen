/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Initial Italian Translation by Ahmed Aldo Faisal
 * Revised and completed by Alessandro Falappa  (June 1999)
 * Updates:
 *		2000/03: translated new items used since version 1.0 and 1.1.0
 *		1999/19: entirely rewritten the translation to correct small variations due
 *				 to feature additions and to conform to the layout of the latest
 *				 commented translator.h for the english language
 *		1999/09: corrected some small typos in the "new since 0.49-990425" section
 *				 added the "new since 0.49-990728" section
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

#ifndef TRANSLATOR_IT_H
#define TRANSLATOR_IT_H

#include "translator.h"

class TranslatorItalian : public Translator
{
  public:
    //--------------------------------------------------------------------
    // NOTICE:
    // the following functions are now obsolete: these are no longer used and
    // will disappear in future versions. You do not have to translate them!
    QCString trInherits()
    { return "Inherits"; }
    QCString trAnd()
    { return "and"; }
    QCString trInheritedBy()
    { return "Inherited By"; }
    QCString trReference()
    { return "Reference"; }
    QCString trReimplementedFrom()
    { return "Reimplemented from"; }
    QCString trReimplementedIn()
    { return "Reimplemented in"; }
    QCString trIncludeFile()
    { return "Include File"; }
    QCString trGeneratedFrom(const char *s,bool single)
    {
      QCString result=(QCString)"The documentation for this"+s+
                     " was generated from the following file";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    // end of obsolete functions
    //--------------------------------------------------------------------

    /*! returns the name of the package that is included by LaTeX */
    QCString latexBabelPackage()
    { return "italian"; }

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "Funzioni collegate"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "(Si noti che queste non sono funzioni membro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription()
    { return "Descrizione Dettagliata"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation()
    { return "Documentazione delle ridefinizioni dei tipi (typedef)"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation()
    { return "Documentazione dei tipi enumerati (enum)"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation()
    { return "Documentazione delle funzioni membro"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation()
    { return "Documentazione dei dati membri"; }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore()
    { return "Continua..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "Lista di tutti i membri."; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList()
    { return "Lista dei membri"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "Questa è la lista completa di tutti i membri di "; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers()
    { return ", inclusi tutti i membri ereditati."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const char *s)
    {
	  QCString result="Generato automaticamente da Doxygen";
      if (s) result+=(QCString)" per "+s;
      result+=" a partire dal codice sorgente.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName()
    { return "nome di tipo enumerato"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue()
    { return "valore di tipo enumerato"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn()
    { return "definito in"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    QCString trVerbatimText(const char *f)
    { return (QCString)"Questo è il contenuto integrale del file include "+f+"."; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \group command).
     */
    QCString trModules()
    { return "Moduli"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy()
    { return "Gerarchia delle classi"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
    { return "Lista dei composti"; }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList()
    { return "Lista dei files"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    QCString trHeaderFiles()
    { return "Lista dei file header"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
    { return "Membri dei composti"; }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers()
    { return "Membri dei files"; }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages()
    { return "Pagine collegate"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples()
    { return "Esempi"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch()
    { return "Cerca"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription()
    {
		return "Questa lista di ereditarietà è ordinata "
		"approssimativamente, ma non completamente, in ordine alfabetico:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Questa è una lista ";
      if (!extractAll) result+="dei files documentati ";
	  else result+="di tutti i files ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
    { return "Queste sono le classi, structs, "
             "unions e interfacce con una loro breve descrizione:";
    }

    /*! This is an introduction to the page with all class members. */
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

    /*! This is an introduction to the page with all file members. */
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

    /*! This is an introduction to the page with the list of all header files. */
    QCString trHeaderFilesDescription()
    { return "Questi sono gli header files che compongono l'API:"; }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription()
    { return "Questa è la lista di tutti gli esempi:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "Questa è la lista di tutte le pagine di documentazione collegate fra loro:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription()
    { return "Questa è la lista di tutti i moduli:"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given.
     */
    QCString trNoDescriptionAvailable()
    { return "Descrizione non disponibile"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation()
    { return "Documentazione"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex()
    { return "Indice dei moduli"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex()
    { return "Indice della gerarchia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex()
    { return "Indice dei composti"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex()
    { return "Indice dei files"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation()
    { return "Documentazione dei moduli"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation()
    { return "Documentazione delle classi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation()
    { return "Documentazione dei files"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    QCString trExampleDocumentation()
    { return "Documentazione degli esempi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    QCString trPageDocumentation()
    { return "Documentazione delle pagine collegate fra loro"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual()
    { return "Manuale di riferimento"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines()
    { return "Definizioni"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    QCString trFuncProtos()
    { return "Prototipi delle funzioni"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs()
    { return "Ridefinizioni di tipo (typedefs)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations()
    { return "Tipi enumerati (enum)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions()
    { return "Funzioni"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables()
    { return "Variabili"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues()
    { return "Valori dei tipi enumerati"; }

    /*! This is used in man pages as the author section. */
    QCString trAuthor()
    { return "Autore"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation()
    { return "Documentazione delle definizioni"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    QCString trFunctionPrototypeDocumentation()
    { return "Documentazione dei prototipi delle funzioni"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation()
    { return "Documentazione delle ridefinizioni di tipo (typedefs)"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "Documentazione dei tipi enumerati"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation()
    { return "Documentazione dei valori dei tipi enumerati"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation()
    { return "Documentazione delle funzioni"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation()
    { return "Documentazione delle variabili"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds()
    { return "Composti"; }

    /*! This is used in the documentation of a group before the list of
     *  links to documented files
     */
    QCString trFiles()
    { return "Files"; }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Generato il "+date;
      if (projName) result+=(QCString)" per "+projName;
      result+=(QCString)" da";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    QCString trWrittenBy()
    {
      return "scritto da";
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagramma delle classi per "+clName;
    }

    /*! this text is generated when the \internal command is used. */
    QCString trForInternalUseOnly()
    { return "Solo per uso interno."; }

    /*! this text is generated when the \reimp command is used. */
    QCString trReimplementedForInternalReasons()
    { return "Reimplementato per motivi interni; l'API non è stata modificata.";}

    /*! this text is generated when the \warning command is used. */
    QCString trWarning()
    { return "Attenzione"; }

    /*! this text is generated when the \bug command is used. */
    QCString trBugsAndLimitations()
    { return "Bugs e limitazioni"; }

    /*! this text is generated when the \version command is used. */
    QCString trVersion()
    { return "Versione"; }

    /*! this text is generated when the \date command is used. */
    QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \author command is used. */
    QCString trAuthors()
    { return "Autore(i)"; }

    /*! this text is generated when the \return command is used. */
    QCString trReturns()
    { return "Restituisce"; }

    /*! this text is generated when the \sa command is used. */
    QCString trSeeAlso()
    { return "Vedi anche"; }

    /*! this text is generated when the \param command is used. */
    QCString trParameters()
    { return "Parametri"; }

    /*! this text is generated when the \exception command is used. */
    QCString trExceptions()
    { return "Eccezioni"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy()
    { return "Generato da"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList()
    { return "Lista dei namespaces"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Questa è la lista ";
      if (!extractAll) result+="dei namespaces documentati, ";
	  else result+="di tutti i namespaces ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends()
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation()
    { return "Documentazione dei friends e delle funzioni collegate"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool /* isTemplate */)
    {
      QCString result="Riferimenti per ";
      switch(compType)
      {
        case ClassDef::Class:  result+="la classe "; break;
        case ClassDef::Struct: result+="la struct "; break;
        case ClassDef::Union:  result+="la union "; break;
        case ClassDef::Interface:  result+="l'interfaccia "; break;
        case ClassDef::Exception:  result+="exception "; break;
      }
      result+=(QCString)clName;
      return result;

    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const char *fileName)
    {
      QCString result="Riferimenti per il file ";
      result+=(QCString)fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Riferimenti per il namespace ";
      result+=(QCString)namespaceName;
      return result;
    }

    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union
     */
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
            result+=", e ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries)
    {
      return "Eredita da "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries)
    {
      return "Base per "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementata in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers()
    { return "Membri dei namespaces"; }

    /*! This is an introduction to the page with all namespace members */
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
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "Indice dei namespaces"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "Documentazione dei namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "Namespaces"; }

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
      QCString result=(QCString)"La documentazione per questa ";
      switch(compType)
      {
        case ClassDef::Class:      result+="classe"; break;
        case ClassDef::Struct:     result+="struct"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interfaccia"; break;
        case ClassDef::Exception:  result+="exception"; break;
      }
      result+=" è stata generata a partire ";
      if (single) result+="dal seguente file:";
	  else result+="dai seguenti files:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
     { return "Lista in ordine alfabetico";}

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Valori di ritorno"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "Pagina Principale"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "pag."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return "Sorgenti";
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return "Definizione alla linea @0 del file @1.";
    }
    QCString trDefinedInSourceFile()
    {
      return "Definizione nel file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Deprecato";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagramma di interrelazione per "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Grafo delle dipendenze di inclusione per "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "Documentazione dei costruttori e dei distruttori"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "Vai al codice sorgente di questo file.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "Vai alla documentazione di questo file.";
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "Precondizione";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "Postcondizione";
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "Invariante";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Valore iniziale:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "codice";
    }
    QCString trGraphicalHierarchy()
    {
      return "Grafico della gerarchia delle classi";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Vai al grafico della gerarchia delle classi";
    }
    QCString trGotoTextualHierarchy()
    {
      return "Vai alla gerarchia delle classi (testuale)";
    }
    QCString trPageIndex()
    {
      return "Indice delle pagine";
    }
    QCString trNote()
    {
      return "Nota";
    }

    QCString trPublicTypes()
    {
      return "Tipi pubblici";
    }
    QCString trPublicAttribs()
    {
      return "Attributi pubblici";
    }
    QCString trStaticPublicAttribs()
    {
      return "Attributi pubblici statici";
    }
    QCString trProtectedTypes()
    {
      return "Tipi protetti";
    }
    QCString trProtectedAttribs()
    {
      return "Attributi protetti";
    }
    QCString trStaticProtectedAttribs()
    {
      return "Attributi protetti statici";
    }
    QCString trPrivateTypes()
    {
      return "Tipi privati";
    }
    QCString trPrivateAttribs()
    {
      return "Attributi privati";
    }
    QCString trStaticPrivateAttribs()
    {
      return "Attributi privati statici";
    }
};

#endif
