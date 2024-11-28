/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
 *  2015/07: updated translation of new items used since version 1.8.2 and 1.8.4
 *           corrected a typo
 *           updated translator mail address
 *  2012/04: updated translation of new items used since version 1.7.5 and 1.8.0
 *  2010/08: updated translation of new items used since version 1.6.0 and 1.6.3
 *           completely reviewed the translation in the trLegendDocs() function
 *           corrected some typos all around
 *           reviewed some translations all around
 *  2007/11: updated translation of new items used since version 1.5.4
 *  2007/10: Included corrections provided by Arialdo Martini <arialdomartini@bebox.it>
 *           updated some strings marked with 'translate me' comment
 *  2006/10: made class to derive directly from Translator class (reported in Petr Prikryl October 9 translator report)
 *  2006/06: updated translation of new items used since version 1.4.6
 *  2006/05: translated new items used since version 1.4.6
 *           corrected typo in trPackageFunction method
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
 *           corrections regarding the plurals of some english terms maintained in the translation,
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
 * eventuali sviste potete scrivermi all'indirizzo: alex.falappa@gmail.com
 */

#ifndef TRANSLATOR_IT_H
#define TRANSLATOR_IT_H

class TranslatorItalian : public TranslatorAdapter_1_8_15
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
    { return "italian"; }

    /*! Used to get the LaTeX command(s) for the language support. This method
     *  was designed for languages which do wish to use a babel package.
     */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[italian]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "it";
    }
    QCString getLanguageString() override
    {
      return "0x410 Italian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funzioni collegate"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Si noti che queste non sono funzioni membro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Descrizione dettagliata"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Dettagli"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentazione delle ridefinizioni dei tipi (typedef)"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentazione dei tipi enumerati (enum)"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Documentazione delle funzioni membro"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentazione dei campi";
      }
      else
      {
        return "Documentazione dei membri dato";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Continua..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Elenco di tutti i membri"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Elenco dei membri"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Questo è l'elenco completo di tutti i membri di"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", inclusi tutti i membri ereditati."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    {
      QCString result="Generato automaticamente da Doxygen";
      if (!s.isEmpty()) result+=" per "+s;
      result+=" a partire dal codice sorgente.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nome di tipo enumerato"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "valore di tipo enumerato"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definito in"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduli"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Gerarchia delle classi"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Strutture dati";
      }
      else
      {
        return "Elenco dei tipi composti";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Elenco dei file"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
     {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campi dei dati";
      }
      else
      {
        return "Membri dei composti";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Elementi globali";
      }
      else
      {
        return "Elementi dei file";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Pagine collegate"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Esempi"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Cerca"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
        return "Questo elenco di ereditarietà è ordinato "
        "approssimativamente, ma non completamente, in ordine alfabetico:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Questo è un elenco ";
      if (!extractAll) result+="dei file documentati ";
      else result+="di tutti i file ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Queste sono le strutture dati con una loro breve descrizione:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Queste sono le classi con una loro breve descrizione:";
      }
      else
      {
        return "Queste sono le classi, le struct, le union e le interfacce con una loro breve descrizione:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Questo è un elenco ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Questo è un elenco ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trExamplesDescription() override
    { return "Questo è l'elenco di tutti gli esempi:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Questo è l'elenco di tutte le pagine di documentazione collegate:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Questo è l'elenco di tutti i moduli:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Documentazione"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Indice dei moduli"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Indice della gerarchia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Indice dei file"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Documentazione dei moduli"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    { return "Documentazione delle classi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Documentazione dei file"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Manuale di riferimento"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definizioni"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Ridefinizioni di tipo (typedef)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Tipi enumerati (enum)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funzioni"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variabili"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Valori del tipo enumerato"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Documentazione delle definizioni"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Documentazione delle ridefinizioni di tipo (typedef)"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Documentazione dei tipi enumerati"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Documentazione delle funzioni"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Documentazione delle variabili"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generato "+date;
      if (!projName.isEmpty()) result+=" per "+projName;
      result+=" da";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagramma delle classi per "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Avvertimento"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versione"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Restituisce"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Si veda anche"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Eccezioni"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generato da"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Lista dei namespace"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
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
    QCString trFriends() override
    { return "Friend"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Documentazione dei friend e delle funzioni collegate"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
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
        default: break;
      }
      result+=clName;
      return result;

    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Riferimenti per il file ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Riferimenti per il namespace ";
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers() override
    { return "Membri pubblici"; }
    QCString trPublicSlots() override
    { return "Slot pubblici"; }
    QCString trSignals() override
    { return "Signal"; }
    QCString trStaticPublicMembers() override
    { return "Membri pubblici statici"; }
    QCString trProtectedMembers() override
    { return "Membri protetti"; }
    QCString trProtectedSlots() override
    { return "Slot protetti"; }
    QCString trStaticProtectedMembers() override
    { return "Membri protetti statici"; }
    QCString trPrivateMembers() override
    { return "Membri privati"; }
    QCString trPrivateSlots() override
    { return "Slot privati"; }
    QCString trStaticPrivateMembers() override
    { return "Membri privati statici"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
      QCString result;
      // the inherits list contain `numEntries' classes
      for (int i=0;i<numEntries;i++)
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
    QCString trInheritsList(int numEntries) override
    {
      return "Eredita da "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Base per "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementata in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Membri dei namespace"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Indice dei namespace"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Documentazione dei namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="La documentazione per quest";
      switch(compType)
      {
        case ClassDef::Class:      result+="a classe"; break;
        case ClassDef::Struct:     result+="a struct"; break;
        case ClassDef::Union:      result+="a union"; break;
        case ClassDef::Interface:  result+="a interfaccia"; break;
        case ClassDef::Protocol:   result+="o protocollo"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a eccezione"; break;
        default: break;
      }
      result+=" è stata generata a partire ";
      if (single) result+="dal seguente file:";
      else result+="dai seguenti file:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Valori di ritorno"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Pagina Principale"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "pag."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definizione alla linea @0 del file @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definizione nel file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Deprecato";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagramma di collaborazione per "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Grafo delle dipendenze di inclusione per "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Documentazione dei costruttori e dei distruttori";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Vai al codice sorgente di questo file.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Vai alla documentazione di questo file.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondizione";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondizione";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariante";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Valore iniziale:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "codice";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafico della gerarchia delle classi";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Vai al grafico della gerarchia delle classi";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Vai alla gerarchia delle classi (testuale)";
    }
    QCString trPageIndex() override
    {
      return "Indice delle pagine";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Nota";
    }
    QCString trPublicTypes() override
    {
      return "Tipi pubblici";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campi";
      }
      else
      {
      return "Attributi pubblici";
      }
    }

    QCString trStaticPublicAttribs() override
    {
      return "Attributi pubblici statici";
    }
    QCString trProtectedTypes() override
    {
      return "Tipi protetti";
    }
    QCString trProtectedAttribs() override
    {
      return "Attributi protetti";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Attributi protetti statici";
    }
    QCString trPrivateTypes() override
    {
      return "Tipi privati";
    }
    QCString trPrivateAttribs() override
    {
      return "Attributi privati";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Attributi privati statici";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Da fare";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Elenco delle cose da fare";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referenziato da";
    }
    QCString trRemarks() override
    {
      return "Osservazioni";
    }
    QCString trAttention() override
    {
      return "Attenzione";
    }
    QCString trInclByDepGraph() override
    {
      return "Questo grafo mostra quali altri file includono direttamente o indirettamente questo file:";
    }
    QCString trSince() override
    {
      return "A partire da";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Legenda del grafo";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
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
    QCString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Lista dei test";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Proprietà";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Documentazione delle proprietà";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Strutture dati";
      }
      else
      {
          return "Classi";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Questi sono i package e una loro breve descrizione (se disponibile):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Package";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Valore:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Lista dei bug";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp() override
    {
      return "1252";
    }

    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Indice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "class", "i", "e");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "file", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "namespace", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupp", "i", "o");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pagin", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membr", "i", "o");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "i", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "i", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referenzia";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementato in "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Sommario";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista degli elementi deprecati";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Eventi";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Documentazione degli eventi";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipi con visibilità di package";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funzioni con visibilità di package";
    }
    QCString trPackageMembers() override
    {
      return "Membri con visibilità di package";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Funzioni statiche con visibilità di package";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Attributi con visibilità di package";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Attributi statici con visibilità di package";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Tutto";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Questo è il grafo delle chiamate per questa funzione:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Corrispondenze:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return " File sorgente " + filename ;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Gerarchia delle directory"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Documentazione delle directory"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Directory"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Riferimenti per la directory "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "directory", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Questa è una funzione membro sovraccaricata (overloaded), "
              "fornita per comodità. Differisce dalla funzione di cui sopra "
              "unicamente per gli argomenti passati.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Questo è il grafo dei chiamanti di questa funzione:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentazione dei tipi enumerati"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentazione delle funzioni membro/subroutine"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Elenco dei tipi di dato"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Membri dei tipi di dato"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Questi sono i tipi dato con una loro breve descrizione:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Indice dei tipi dati"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentazione dei tipi dato"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funzioni/Subroutine"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentazione funzioni/subroutine"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipi di dato"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Elenco dei moduli"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Questa è una lista di tutti i moduli ";
      if (!extractAll) result+="documentati ";
      result+="con una loro breve descrizione:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
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
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Riferimenti per il modulo ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Membri del modulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Indice dei moduli"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "i", "o");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="La documentazione per quest";
      switch(compType)
      {
        case ClassDef::Class:      result+="o modulo"; break;
        case ClassDef::Struct:     result+="o tipo"; break;
        case ClassDef::Union:      result+="a union"; break;
        case ClassDef::Interface:  result+="a interfaccia"; break;
        case ClassDef::Protocol:   result+="o protocollo"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a eccezione"; break;
        default: break;
      }
      result+=" è stata generata a partire ";
      if (single) result+="dal seguente file:"; else result+="dai seguenti file:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "i", "o");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sottoprogramm", "i", "a");
    }

    /*! C# Type Contraint list */
    QCString trTypeConstraints() override
    {
      return "Vincoli dei tipi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relazione per "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Caricamento in corso...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Namespace globale";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Ricerca in corso...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Nessun risultato";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    QCString trFileIn(const QCString &name) override
    {
      return "File in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Include il file in "+name;
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Lun","Mar","Mer","Gio","Ven","Sab","Dom" };
      static const char *months[] = { "Gen","Feb","Mar","Apr","Mag","Giu","Lug","Ago","Set","Ott","Nov","Dic" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "lun", "mar", "mer", "gio", "ven", "sab", "dom" };
      static const char *days_full[]    = { "lunedì", "martedì", "mercoledì", "giovedì", "venerdì", "sabato", "domenica" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "gen", "feb", "mar", "apr", "mag", "giu", "lug", "ago", "set", "ott", "nov", "dic" };
      static const char *months_full[]  = { "gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Riferimenti bibliografici"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Grafo di dipendenza delle directory per ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "livello di dettaglio"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parametri dei template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "e altri "+number+" ..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "La documentazione per questo tipo enumerato è stata generata a partire";
      if (!single) result += " dai seguenti";
      else result += " dal seguente";
      result+=" file:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString("Riferimenti per il tipo enumerato ") + QCString(name); }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" ereditati da "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Altri membri ereditati"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "abilitare" : "disabilitare";
      return "cliccare per "+opt+" la sincronizzazione del pannello";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Fornito dalla categoria @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Estende la classe @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Metodi della classe";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Metodi di instanza";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Documentazione dei metodi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Interfacce esportate"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Servizi inclusi"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Gruppi di costanti"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result="Riferimenti per il gruppo di costanti ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result="Riferimenti per il servizio ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result="Riferimenti per il singleton ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="La documentazione per questo servizio "
                                "è stata generata a partire ";
      if (single) result+="dal seguente file:"; else result+="dai seguenti file:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="La documentazione per questo singleton "
                                "è stata generata a partire ";
      if (single) result+="dal seguente file:"; else result+="dai seguenti file:";
      return result;
    }

};

#endif


