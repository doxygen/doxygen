/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

/******************************************************************************
 *
 * Revision history
 *
 *  2012/04: updated translation of new items used since version 1.7.5 and 1.8.0
 *  2010/08: updated translation of new items used since version 1.6.0 and 1.6.3
 *           completely reviewed the translation in the trLegendDocs() function
 *           corrected some typos all around
 *           reviewed some translations all around
 *  2007/11: updated translation of new items used since version 1.5.4
 *  2007/10: Included corrections provided by Arialdo Martini <arialdomartini@bebox.it>, updated some strings marked with 'translate me' comment
 *  2006/10: made class to derive directly from Translator class (reported in Petr Prikryl October 9 translator report)
 *  2006/06: updated translation of new items used since version 1.4.6
 *  2006/05: translated new items used since version 1.4.6
 *           corrected typo in trPackageMembers method
 *  2005/03: translated new items used since version 1.4.1
 *           removed unused methods listed in Petr Prikryl February 28 translator report
 *  2004/09: translated new items used since version 1.3.9
 *  2004/06: translated new items used since version 1.3.8
 *  2003/11: translated new items used since version 1.3.3
 *  2003/06: translated new items used since version 1.3.1
 *  2003/04: translated new items used since version 1.3
 *  2002/11: translated new items used since version 1.2.18
 *  2002/08: translated new items used since version 1.2.17
 *  2002/07: translated new items used since version 1.2.16
 *  2002/06: modified trRelatedPagesDescription() method
 *           corrected typo in trInclByDepGraph() method
 *  2002/01: translated new items used since version 1.2.13
 *           updated e-mail address
 *  2001/11: corrected the translation fixing the issues reported by the translator.pl script
 *           translated new items used since version 1.2.11
 *  2001/08: corrected the translation fixing the issues reported by the translator.pl script
 *           translated new items used since version 1.2.7
 *  2001/05: adopted new translation mechanism (trough adapters),
 *           translated new items used since version 1.2.5 and 1.2.6,
 *           revised those function returning strings in OPTIMIZE_OTPUT_FOR_C case,
 *           corrections regarding the plurals of some english terms mantained in the translation,
 *           changed some terms to better suit the sense
 *  2001/02: translated new items used since version 1.2.4
 *  2000/11: modified slightly the translation in trLegendDocs() function,
 *           translated new items used since version 1.2.1 and 1.2.2
 *  2000/08: translated new items used since version 1.1.3, 1.1.4, 1.1.5 and 1.2.0
 *  2000/03: translated new items used since version 1.0 and 1.1.0
 *  1999/19: entirely rewritten the translation to correct small variations due
 *           to feature additions and to conform to the layout of the latest
 *           commented translator.h for the english language
 *  1999/09: corrected some small typos in the "new since 0.49-990425" section
 *           added the "new since 0.49-990728" section
 *  1999/06: revised and completed by Alessandro Falappa (current mantainer)
 *  1999/??: initial italian translation by Ahmed Aldo Faisal
 */

/******************************************************************************
 *
 * Note sui criteri adottati per la traduzione
 *
 * Nella traduzione non si sono tradotti alcuni termini inglesi ormai entrati
 * a far parte del "gergo" informatico (per es. file o namespace)
 *
 * Il plurale dei termini inglesi non tradotti è stato reso con il singolare
 * della parola inglese secondo una convenzione spesso ritrovata nella documentazione
 * tecnica (ad es. "lista dei file" e non "lista dei files")
 *
 * Se avete suggerimenti sulla traduzione di alcuni termini o volete segnalare
 * eventuali sviste potete scrivermi all'indirizzo: alessandro@falappa.net
 */

#ifndef TRANSLATOR_IT_H
#define TRANSLATOR_IT_H

class TranslatorItalian : public TranslatorAdapter_1_8_2
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "italian"; }

    /*! Used to get the LaTeX command(s) for the language support. This method
     *  was designed for languages which do wish to use a babel package.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[italian]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "UTF-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "Funzioni collegate"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "(Si noti che queste non sono funzioni membro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription()
    { return "Descrizione dettagliata"; }

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
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentazione dei campi";
      }
      else
      {
        return "Documentazione dei membri dato";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore()
    { return "Continua..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "Elenco di tutti i membri"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList()
    { return "Elenco dei membri"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "Questo è l'elenco completo di tutti i membri di "; }

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

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules()
    { return "Moduli"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy()
    { return "Gerarchia delle classi"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Strutture dati";
      }
      else
      {
        return "Elenco dei tipi composti";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList()
    { return "Elenco dei file"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
     {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Campi dei dati";
      }
      else
      {
        return "Membri dei composti";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Elementi globali";
      }
      else
      {
        return "Elementi dei file";
      }
    }

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
        return "Questo elenco di ereditarietà è ordinato "
        "approssimativamente, ma non completamente, in ordine alfabetico:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Questo è un elenco ";
      if (!extractAll) result+="dei file documentati ";
      else result+="di tutti i file ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Queste sono le strutture dati con una loro breve descrizione:";
      }
      else
      {
        return "Queste sono le classi, le struct, le union e le interfacce con una loro breve descrizione:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Questo è un elenco ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        if (!extractAll) result+="delle struct e delle union documentate ";
        else result+="di tutte le struct e le union ";
      }
      else
      {
        if (!extractAll) result+="dei membri documentati ";
        else result+="di tutti i membri ";
      }
      result+="con collegamenti alla documentazione ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          if (extractAll) result+="della struct/union per ciascun campo:";
          else result+="delle struct/union a cui appartengono:";
      }
      else
      {
          if (extractAll) result+="della classe a cui appartengono:";
          else result+="delle classi a cui appartengono:";
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Questo è un elenco ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          if (!extractAll) result+="delle funzioni, delle variabili, delle define, dei tipi enumerati e delle ridefinizioni dei tipi (typedef) documentate ";
          else result+="di tutte le funzioni, variabili, define, tipi enumerati, e ridefinizioni dei tipi (typedef) ";
      }
      else
      {
          if (!extractAll) result+="degli elementi documentati dei file ";
          else result+="di tutti gli elementi dei file ";
      }
      result+="con collegamenti alla documentazione";
      if (extractAll) result+=" del file a cui appartengono:";
      else result+=":";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription()
    { return "Questo è l'elenco di tutti gli esempi:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "Questo è l'elenco di tutte le pagine di documentazione collegate:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription()
    { return "Questo è l'elenco di tutti i moduli:"; }

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
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Indice delle strutture dati";
      }
      else
      {
        return "Indice dei tipi composti";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex()
    { return "Indice dei file"; }

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
    { return "Documentazione dei file"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    QCString trExampleDocumentation()
    { return "Documentazione degli esempi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    QCString trPageDocumentation()
    { return "Documentazione delle pagine tra loro collegate "; }

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
    { return "Ridefinizioni di tipo (typedef)"; }

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
    { return "Valori del tipo enumerato"; }

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
    { return "Documentazione delle ridefinizioni di tipo (typedef)"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "Documentazione dei tipi enumerati"; }

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
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Strutture dati";
      }
      else
      {
        return "Composti";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Generato "+date;
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

    /*! this text is generated when the \\internal command is used. */
    QCString trForInternalUseOnly()
    { return "Solo per uso interno."; }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning()
    { return "Avvertimento"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion()
    { return "Versione"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns()
    { return "Restituisce"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso()
    { return "Si veda anche"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters()
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
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
    { return "Lista dei namespace"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Questa è l'elenco ";
      if (!extractAll) result+="dei namespace documentati, ";
      else result+="di tutti i namespace ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends()
    { return "Friend"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation()
    { return "Documentazione dei friend e delle funzioni collegate"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    {
      QCString result="Riferimenti per ";
      if (isTemplate) result="Template per ";
      switch(compType)
      {
        case ClassDef::Class:  result+="la classe "; break;
        case ClassDef::Struct: result+="la struct "; break;
        case ClassDef::Union:  result+="la union "; break;
        case ClassDef::Interface:  result+="l'interfaccia "; break;
        case ClassDef::Protocol:   result+="il protocollo "; break;
        case ClassDef::Category:   result+="la categoria "; break;
        case ClassDef::Exception:  result+="l'eccezione "; break;
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

    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers()
    { return "Membri pubblici"; }
    QCString trPublicSlots()
    { return "Slot pubblici"; }
    QCString trSignals()
    { return "Signal"; }
    QCString trStaticPublicMembers()
    { return "Membri pubblici statici"; }
    QCString trProtectedMembers()
    { return "Membri protetti"; }
    QCString trProtectedSlots()
    { return "Slot protetti"; }
    QCString trStaticProtectedMembers()
    { return "Membri protetti statici"; }
    QCString trPrivateMembers()
    { return "Membri privati"; }
    QCString trPrivateSlots()
    { return "Slot privati"; }
    QCString trStaticPrivateMembers()
    { return "Membri privati statici"; }

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
    { return "Membri dei namespace"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Questa è la lista ";
      if (!extractAll) result+="dei membri documentati del namespace, ";
       else result+="di tutti i membri del namespace ";
      result+="con collegamenti ";
      if (extractAll)
        result+="alla documentazione del namespace per ciascun membro:";
      else
        result+="ai namespace a cui appartengono:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "Indice dei namespace"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "Documentazione dei namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "Namespace"; }

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
      QCString result=(QCString)"La documentazione per quest";
      switch(compType)
      {
        case ClassDef::Class:      result+="a classe"; break;
        case ClassDef::Struct:     result+="a struct"; break;
        case ClassDef::Union:      result+="a union"; break;
        case ClassDef::Interface:  result+="a interfaccia"; break;
        case ClassDef::Protocol:   result+="o protocollo"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a eccezione"; break;
      }
      result+=" è stata generata a partire ";
      if (single) result+="dal seguente file:";
      else result+="dai seguenti file:";
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

    QCString trDefinedAtLineInSourceFile()
    {
      return "Definizione alla linea @0 del file @1.";
    }
    QCString trDefinedInSourceFile()
    {
      return "Definizione nel file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Deprecato";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagramma di collaborazione per "+clName+":";
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
    /*! Text for the \\pre command */
    QCString trPrecondition()
    {
      return "Precondizione";
    }
    /*! Text for the \\post command */
    QCString trPostcondition()
    {
      return "Postcondizione";
    }
    /*! Text for the \\invariant command */
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

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Campi";
      }
      else
      {
      return "Attributi pubblici";
      }
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

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Da fare";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Elenco delle cose da fare";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referenziato da";
    }
    virtual QCString trRemarks()
    {
      return "Osservazioni";
    }
    virtual QCString trAttention()
    {
      return "Attenzione";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Questo grafo mostra quali altri file includono direttamente o indirettamente questo file:";
    }
    virtual QCString trSince()
    {
      return "A partire da";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda del grafo";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return
        "Questa pagina spiega come interpretare i grafi generati da doxygen.<p>\n"
        "Si consideri l'esempio seguente:\n"
        "\\code\n"
        "/*! Classe invisibile per troncamento */\n"
        "class Invisible { };\n\n"
        "/*! Classe tronca, la relazione di ereditarietà è nascosta */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Classe non documentata con i commenti speciali di doxygen*/\n"
        "class Undocumented { };\n\n"
        "/*! Classe estesa mediante ereditarietà pubblica */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Classe templatizzata */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Classe estesa mediante ereditarietà protetta*/\n"
        "class ProtectedBase { };\n\n"
        "/*! Classe estesa mediante ereditarietà privata*/\n"
        "class PrivateBase { };\n\n"
        "/*! Classe utilizzata dalla classe Inherited */\n"
        "class Used { };\n\n"
        "/*! Classe che eredita da varie classi*/\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Verrà prodotto il grafo seguente:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center></p>\n"
        "<p>\n"
        "I riquadri nel grafo qui sopra hanno il seguente significato:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Il riquadro grigio pieno rappresenta la struct o la classe per la quale il grafo è stato generato.</li>\n"
        "<li>Un riquadro con un bordo nero denota una struct o una classe documentata.</li>\n"
        "<li>Un riquadro con un bordo grigio indica una struct o una classe non documentata.</li>\n"
        "<li>Un riquadro con un bordo rosso indica una struct o una classe per la quale non sono mostrate tutte le relazioni di ereditarietà/contenimento (un grafo viene troncato se non rientra nei limiti prestabiliti).</li>\n"
        "</ul>\n"
        "<p>\n"
        "Le frecce hanno il seguente significato:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Una freccia blu scuro indica una relazione di ereditarietà pubblica tra due classi.</li>\n"
        "<li>Una freccia verde indica un'ereditarietà protetta.</li>\n"
        "<li>Una freccia rossa indica un'ereditarietà privata.</li>\n"
        "<li>Una freccia viola tratteggiata indica che una classe è contenuta o usata da un'altra classe."
        " La freccia viene etichettata con la o le variabili attraverso cui la struct o la classe puntata dalla freccia è accessibile.</li>\n"
        "<li>Una freccia gialla tratteggiata indica la relazione tra una istanza di un template e la classe templatizzata da cui è stata istanziata."
        " La freccia viene etichettata con i parametri di template dell'istanza.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "legenda";
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
      return "Lista dei test";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Metodi DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Proprietà";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentazione delle proprietà";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Strutture dati";
      }
      else
      {
          return "Classi";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Package "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Lista dei package";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Questi sono i package e una loro breve descrizione (se disponibile):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Package";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Valore:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Lista dei bug";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Indice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Class" : "class"));
      result+=(singular ? "e" : "i");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool)
    {
      QCString result((first_capital ? "File" : "file"));
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool)
    {
      QCString result((first_capital ? "Namespace" : "namespace"));
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Grupp" : "grupp"));
      result+=(singular ? "o" : "i");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Pagin" : "pagin"));
      result+=(singular ? "a" : "e");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Membr" : "membr"));
      result+=(singular ? "o" : "i");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Global" : "global"));
      result+=(singular ? "e" : "i");
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Autor" : "autor"));
      result+=(singular ? "e" : "i");
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referenzia";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementato in "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Sommario";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista degli elementi deprecati";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Eventi";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Documentazione degli eventi";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Tipi con visibilità di package";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Funzioni con visibilità di package";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Funzioni statiche con visibilità di package";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Attributi con visibilità di package";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Attributi statici con visibilità di package";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Tutto";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Questo è il grafo delle chiamate per questa funzione:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the header
     *  of each page before the field where one can enter the text to search
     *  for.
     */
    virtual QCString trSearchForIndex()
    {
      return "Cerca";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Risultati della ricerca";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    virtual QCString trSearchResults(int numDocuments)
    {
      if (numDocuments==0)
      {
        return "Nessun documento soddisfa la tua richiesta.";
      }
      else if (numDocuments==1)
      {
        return "Trovato <b>1</b> documento che soddisfa la tua richiesta.";
      }
      else
      {
        return "Trovati <b>$num</b> documenti che soddisfano la tua richiesta. "
               "Le corrispondenze migliori sono in testa.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Corrispondenze:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return " File sorgente " + filename ;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Gerarchia delle directory"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Documentazione delle directory"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Directory"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Questa gerarchia di directory è approssimativamente, "
        "ma non completamente, ordinata in ordine alfabetico:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Riferimenti per la directory "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool)
    {
      QCString result((first_capital ? "Directory" : "directory"));
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "Questa è una funzione membro sovraccaricata (overloaded), "
              "fornita per comodità. Differisce dalla funzione di cui sopra "
              "unicamente per gli argomenti passati.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Questo è il grafo dei chiamanti di questa funzione:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentazione dei tipi enumerati"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentazione delle funzioni membro/subroutine"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Elenco dei tipi di dato"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Membri dei tipi di dato"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Questi sono i tipi dato con una loro breve descrizione:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Questa è una lista di tutti i membri ";
      if (!extractAll)
      {
        result+="documentati ";
      }
      result+="dei tipi di dato con collegamenti ";
      if (!extractAll) 
      {
         result+="alla documentazione della struttura dati per ciascun membro:";
      }
      else 
      {
         result+="ai tipi dato a cui appartengono:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Indice dei tipi dati"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentazione dei tipi dato"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funzioni/Subroutine"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentazione funzioni/subroutine"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tipi di dato"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Elenco dei moduli"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Questa è una lista di tutti i moduli ";
      if (!extractAll) result+="documentati ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Riferimenti per ";
      if (isTemplate) result="Template per ";
      switch(compType)
      {
        case ClassDef::Class:      result+=" il modulo"; break;
        case ClassDef::Struct:     result+=" il tipo dato"; break;
        case ClassDef::Union:      result+=" l'union"; break;
        case ClassDef::Interface:  result+=" l'nterfaccia"; break;
        case ClassDef::Protocol:   result+=" il protocollo"; break;
        case ClassDef::Category:   result+=" la categoria"; break;
        case ClassDef::Exception:  result+=" l'eccezione"; break;
      }
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Riferimenti per il modulo ";
      result+=namespaceName;        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Membri del modulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Questo è un elenco di tutti i membri dei moduli ";
      if (!extractAll) result+="documentati ";
      result+="con collegamenti ";
      if (extractAll) 
      {
        result+="alla documentazione del modulo per ciascun membro:";
      }
      else 
      {
        result+="al modulo a cui appartengono:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Indice dei moduli"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Modul" : "modul"));
      if (singular) result+="o";
      else result+="i";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"La documentazione per quest";
      switch(compType)
      {
        case ClassDef::Class:      result+="o modulo"; break;
        case ClassDef::Struct:     result+="o tipo"; break;
        case ClassDef::Union:      result+="a union"; break;
        case ClassDef::Interface:  result+="a interfaccia"; break;
        case ClassDef::Protocol:   result+="o protocollo"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a eccezione"; break;
      }
      result+=" è stata generata a partire ";
      if (single) result+="dal seguente file:"; else result+="dai seguenti file:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tip" : "tip"));
      if (singular) result+="o";
      else result+="i";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Sottoprogramm" : "sottoprogramm"));
      if (singular) result+="a";
      else result+="i";
      return result; 
    }

    /*! C# Type Contraint list */
    virtual QCString trTypeConstraints()
    {
      return "Vincoli dei tipi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Relazione per "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Caricamento in corso...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Namespace globale";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Ricerca in corso...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nessun risultato";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Grafo di dipendenza delle directory per "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"File in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Include il file in "+name;
    }

    /** Compiles a date string.
     *  @param year Year in 4 digits
     *  @param month Month of the year: 1=January
     *  @param day Day of the Month: 1..31
     *  @param dayOfWeek Day of the week: 1=Monday..7=Sunday
     *  @param hour Hour of the day: 0..23
     *  @param minutes Minutes in the hour: 0..59
     *  @param seconds Seconds within the minute: 0..59
     *  @param includeTime Include time in the result string?
     */
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    {
      static const char *days[]   = { "Lun","Mar","Mer","Gio","Ven","Sab","Dom" };
      static const char *months[] = { "Gen","Feb","Mar","Apr","Mag","Giu","Lug","Ago","Set","Ott","Nov","Dic" };
      QCString sdate;
      sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Riferimenti bibliografici"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Grafo di dipendenza delle directory per ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "livello di dettaglio"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parametri dei template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "e altri "+number+" ..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "La documentazione per questo tipo enumerato è stata generata";
      if (!single) result += " dai seguenti";
      else result += " dal seguente";
      result+=" file:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString("Riferimenti per il tipo enumerato ") + QCString(name); }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" ereditati da "+what; }

    /*! Header of the sections with inherited members specific for the 
     *  base class(es) 
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Altri membri ereditati"; }

};

#endif


