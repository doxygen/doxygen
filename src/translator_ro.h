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

/* Original translation from English to Romanian by Alexandru Iosup [aiosup@yahoo.com].
 * Updated by Ionuţ Dumitraşcu [reddumy@yahoo.com]
 *
 * -------------------------------------------
 * Project start                : 20.09.2000
 * Last Doxygen version covered : 1.8.4
 * Last revision                : 17.05.2013
 * -------------------------------------------
 *
 * Revision history
 * ----------------
 * 17.05.2013 - Updated translation to cover Doxygen 1.8.4 (Ionuț Dumitrașcu)
 * 15.01.2009 - Updated Romanian translation to Doxygen 1.5.8 and modified strings to UTF-8, as well as some other changes (Ionuţ Dumitraşcu)
 * 28.07.2008 - Updated version - covering Doxygen 1.5.6 - and some minor changes  (Ionuţ Dumitraşcu)
 *
 * 01.Mar.2k5  Third revision, covering Doxygen 1.4.1
 *
 * 07.Mar.2k2  Second revision, covering Doxygen 1.2.14
 *             - fixed some bugs
 *
 * 20.Sep.2k   First version, covering Doxygen 1.2.1
 *
 */
#ifndef TRANSLATOR_RO_H
#define TRANSLATOR_RO_H


class TranslatorRomanian : public TranslatorAdapter_1_8_15
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    DString idLanguage() override
    { return "romanian"; }
    /*! Used to get the LaTeX command(s) for the language support.
     *  This method should return string with commands that switch
     *  LaTeX to the desired language.  For example
     *  <pre>"\\usepackage[german]{babel}\n"
     *  </pre>
     *  or
     *  <pre>"\\usepackage{polski}\n"
     *  "\\usepackage[latin2]{inputenc}\n"
     *  "\\usepackage[T1]{fontenc}\n"
     *  </pre>
     *
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage[romanian]{babel}\n";
    }
    DString trISOLang() override
    {
      return "ro";
    }
    DString getLanguageString() override
    {
      return "0x418 Romanian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Funcţii înrudite"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Atenţie: acestea nu sunt funcţii membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Descriere Detaliată"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Detalii"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Documentaţia Definiţiilor de Tipuri (typedef) Membre"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Documentaţia Enumerărilor Membre"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "Documentaţia Funcţiilor Membre"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentaţia Câmpurilor";
      }
      else
      {
        return "Documentaţia Datelor Membre";
      }

    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Mai mult..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Lista tuturor membrilor"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Lista Membrilor"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Lista completă a membrilor din"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", inclusiv a tuturor membrilor moşteniţi."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Generat automat de Doxygen";
      if (!s.empty()) result+=" pentru "+s;
      result+=" din codul sursă.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "nume enumerare"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "valoare enumerare"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "definit în"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Module"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Ierarhia Claselor"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Structuri de Date";
      }
      else
      {
        return "Lista Claselor";
      }

    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Lista fişierelor"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Câmpurile de Date";
      }
      else
      {
        return "Membrii Componenţi"; //cu articol hotarat
      }

    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globale";
      }
      else
      {
        return "Membrii din Fişier"; //cu articol hotarat
      }

    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Pagini înrudite"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Exemple"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Caută"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Această listă de moşteniri este sortată în general, "
             "dar nu complet, în ordine alfabetică:";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Lista tuturor ";
      result+="fişierelor";
	  if (!extractAll) result+=" documentate";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Lista structurilor de date, cu scurte descrieri:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Lista claselor, cu scurte descrieri:";
      }
      else
      {
        return "Lista claselor, structurilor, uniunilor şi interfeţelor"
		", cu scurte descrieri:";
      }

    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Lista tuturor ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="câmpurilor ";
        if (!extractAll) result+=" documentate ";
        result+="din structuri si uniuni ";
      }
      else
      {
        result+="membrilor ";
	  if (!extractAll) result+="documentaţi ";
        result+="din toate clasele ";
      }
      result+=", cu legături către ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="documentaţia structurii/uniunii pentru fiecare câmp în parte:";
        }
        else
        {
          result+="documentaţia clasei pentru fiecare membru în parte:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="structurile/uniunile de care aparţin:";
        }
        else
        {
          result+="clasele de care aparţin:";
        }
      }

      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Lista tuturor ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funcţiilor, variabilelor, definiţiilor, enumerărilor şi definiţiilor de tip";
	  if (!extractAll) result+=" documentate";
      }
      else
      {
        result+="membrilor ";
	  if (!extractAll) result+="documentaţi ";
	  result+="din toate fişierele";
      }
      result+=", cu legături către ";
      if (extractAll)
        result+="fişierele de care aparţin:";
      else
        result+="documentaţia aferentă:";

      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Lista tuturor exemplelor:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Lista tuturor documentaţiilor înrudite:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Lista tuturor modulelor:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Documentaţie"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Indexul Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Index Ierarhic"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Indexul Structurilor de Date";
      }
      else
      {
        return "Indexul Claselor";
      }

    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Indexul Fişierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Documentaţia Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentaţia Structurilor de Date";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Documentaţia Claselor";
      }

    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Documentaţia Fişierelor"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Manual de utilizare"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Definiţii"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Definiţii de tipuri"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Enumerări"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funcţii"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Variabile"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Valori de enumerări"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Documentaţia definiţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Documentaţia definiţiilor de tipuri"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Documentaţia enumerărilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Documentaţia funcţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Documentaţia variabilelor"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Structuri de Date";
      }
      else
      {
        return "Membri";
      }

    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Generat "+date;
      if (!projName.empty()) result+=" pentru "+projName;
      result+=" de către";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Diagrama de relaţii pentru "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Atenţie"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Versiunea"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Întoarce"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Vezi şi"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Excepţii"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Generat de"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Lista de Namespace-uri"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Lista tuturor ";
      result+="namespace-urilor ";
	  if (!extractAll) result+="documentate ";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Prieteni"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Documentaţia funcţiilor prietene sau înrudite"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
	  DString result="Referinţă la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasa "; break;
        case ClassDef::Struct:     result+="structura "; break;
        case ClassDef::Union:      result+="uniunea "; break;
        case ClassDef::Interface:  result+="interfaţa "; break;
        case ClassDef::Protocol:   result+="protocolul "; break;
        case ClassDef::Category:   result+="categoria "; break;
        case ClassDef::Exception:  result+="excepţia "; break;
        default: break;
      }
      if (isTemplate) result+=" (Template) ";
      result+=clName;

      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result="Referinţă la fişierul ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result="Referinţă la Namespace-ul ";
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    DString trPublicMembers() override
    { return "Metode Publice"; }
    DString trPublicSlots() override
    { return "Conectori (slots) Publici"; }
    DString trSignals() override
    { return "Semnale"; }
    DString trStaticPublicMembers() override
    { return "Metode Statice Publice"; }
    DString trProtectedMembers() override
    { return "Metode Protejate"; }
    DString trProtectedSlots() override
    { return "Conectori (slots) Protejaţi"; }
    DString trStaticProtectedMembers() override
    { return "Metode Statice Protejate"; }
    DString trPrivateMembers() override
    { return "Metode Private"; }
    DString trPrivateSlots() override
    { return "Conectori (slots) Privaţi"; }
    DString trStaticPrivateMembers() override
    { return "Metode Statice Private"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    DString trWriteList(int numEntries) override
    {
      DString result;
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
            result+=" şi ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Moşteneşte "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Moştenit de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementat din "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Reimplementat în "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Membrii Namespace-ului"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Lista tuturor membrilor ";
      if (!extractAll) result+="documentaţi ";
	  result+="din toate namespace-urile, cu legături către ";

      if (extractAll)
        result+="documentaţia namespace-ului pentru fiecare membru în parte:";
      else
        result+="namespace-urile de care aparţin:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Indexul Namespace-ului"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Documentaţia Namespace-ului"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Namespace-uri"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="Documentaţia pentru această ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasă"; break;
        case ClassDef::Struct:     result+="structură"; break;
        case ClassDef::Union:      result+="uniune"; break;
        case ClassDef::Interface:  result+="interfaţă"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="categorie"; break;
        case ClassDef::Exception:  result+="excepţie"; break;
        default: break;
      }
      result+=" a fost generată din fişier";
      if (single) result+="ul:"; else result+="ele:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Valori returnate"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Pagina principală"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "pg."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Definiţia în linia @0 a fişierului @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Definiţia în fişierul @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Învechită(Deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Diagrama de relaţii pentru "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Graful dependenţelor prin incluziune pentru "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Documentaţia pentru Constructori şi Destructori";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Vezi sursele.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Vezi documentaţia.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Precondiţie";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Postcondiţie";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Valoarea iniţială:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "cod";
    }
    DString trGraphicalHierarchy() override
    {
      return "Ierarhia Claselor în mod grafic";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Vezi ierarhia claselor în mod grafic";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Vezi ierarhia claselor în mod text";
    }
    DString trPageIndex() override
    {
      return "Indexul Paginilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Notă";
    }
    DString trPublicTypes() override
    {
      return "Tipuri Publice";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Câmpuri de Date";
      }
      else
      {
        return "Atribute Publice";
      }

    }
    DString trStaticPublicAttribs() override
    {
      return "Atribute Statice Publice";
    }
    DString trProtectedTypes() override
    {
      return "Tipuri Protejate";
    }
    DString trProtectedAttribs() override
    {
      return "Atribute Protejate";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Atribute Statice Protejate";
    }
    DString trPrivateTypes() override
    {
      return "Tipuri Private";
    }
    DString trPrivateAttribs() override
    {
      return "Atribute Private";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Atribute Statice Private";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    {
      return "De făcut";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Lista lucrurilor de făcut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Semnalat de";
    }
    DString trRemarks() override
    {
      return "Observaţii";
    }
    DString trAttention() override
    {
      return "Atenţie";
    }
    DString trInclByDepGraph() override
    {
      return "Acest graf arată care fişiere includ, "
		  "direct sau indirect, acest fişier:";
    }
    DString trSince() override
    {
      return "Din";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Legenda grafului";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Această pagină arată modul în care trebuie să interpretaţi "
		"grafurile generate de doxygen.<p>\n"
        "Consideraţi următorul exemplu:\n"
        "\\code\n"
        "/*! Clasă invizibilă, tăiată din cauza depăşirii spaţiului */\n"
        "class Invisible { };\n\n"
        "/*! Altă clasă tăiată, relaţia de moştenire este ascunsă */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Clasă necomentată în stil doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Clasă care este moştenită în mod public */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Clasă template */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Clasă care este moştenită în mod protejat */\n"
        "class ProtectedBase { };\n\n"
        "/*! Clasă care este moştenită în mod privat */\n"
        "class PrivateBase { };\n\n"
        "/*! Clasă care este folosită de clasa Inherited */\n"
        "class Used { };\n\n"
        "/*! Superclasă care moşteneşte un număr de alte clase */\n"
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
        "Dacă tagul \\c MAX_DOT_GRAPH_HEIGHT din fişierul de configurare "
        "este setat la 200, acesta este graful rezultat:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Căsuţele din partea de sus au următoarea semnificaţie:\n"
        "<ul>\n"
        "<li>O căsuţă neagră reprezintă structura sau clasa pentru care "
        "graful este generat.\n"
        "<li>O căsuţă cu marginea neagră reprezintă o structură sau o clasă documentate.\n"
        "<li>O căsuţă cu marginea gri reprezintă o structură sau o clasă nedocumentate.\n"
        "<li>O căsuţă cu marginea roşie reprezintă o structură sau o clasă documentate, pentru\n"
        "care nu toate relaţiile de moştenire/incluziune sunt arătate. Un graf este "
        "tăiat dacă nu încape în marginile specificate."
        "</ul>\n"
        "Săgeţile au următoarea semnificaţie:\n"
        "<ul>\n"
        "<li>O săgeată de un albastru închis este folosită când avem o relaţie de "
        "moştenire publică între două clase.\n"
        "<li>O săgeată de un verde închis este folosită când avem o moştenire protejată.\n"
        "<li>O săgeată de un roşu închis este folosită când avem o moştenire privată.\n"
        "<li>O săgeată violetă punctată este folosită pentru o clasă conţinută sau folosită "
        "de o altă clasă. Săgeata este marcată cu variabila(e) "
        "prin care este accesibilă clasa sau structura spre care este îndreptată. \n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Listă de teste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Proprietăţi";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Documentaţia Proprietăţilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Structuri de Date";
      }
      else
      {
        return "Clase";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Pachet "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Lista pachetelor, însoţită de scurte explicaţii, acolo unde acestea există:";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Pachete";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Valoare:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Problema (Bug)";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Lista de Probleme (Bugs)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file
     *
     * The following table shows the correlation of Charset name, Charset Value and
     * <pre>
     * Codepage number:
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * DEFAULT_CHARSET           1 (x01)
     * SYMBOL_CHARSET            2 (x02)
     * OEM_CHARSET             255 (xFF)
     * ANSI_CHARSET              0 (x00)            1252
     * RUSSIAN_CHARSET         204 (xCC)            1251
     * EE_CHARSET              238 (xEE)            1250
     * GREEK_CHARSET           161 (xA1)            1253
     * TURKISH_CHARSET         162 (xA2)            1254
     * BALTIC_CHARSET          186 (xBA)            1257
     * HEBREW_CHARSET          177 (xB1)            1255
     * ARABIC _CHARSET         178 (xB2)            1256
     * SHIFTJIS_CHARSET        128 (x80)             932
     * HANGEUL_CHARSET         129 (x81)             949
     * GB2313_CHARSET          134 (x86)             936
     * CHINESEBIG5_CHARSET     136 (x88)             950
     * </pre>
     *
     */
    DString trRTFansicp() override
    {
      return "1250"; //EASTEUROPE_CHARSET
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "clas", "ele", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fişier", "ele", "ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "-urile", "-ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupu", "rile", "l");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pagin", "ile", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membr", "ii", "ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "ii", "ul");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Referinţe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Implementează "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Implementat în "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Cuprins";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Lista elementelor învechite (deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Evenimente";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Documentaţia aferentă evenimentelor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Tipuri în pachet";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Funcţii în pachet";
    }
    DString trPackageMembers() override
    {
      return "Membrii în pachet";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Funcţii statice în pachet";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Atribute în pachet";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Atribute statice în pachet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Toate";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Graful de apel al acestei funcţii:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Rezultatele căutarii";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "Din păcate nu am găsit nici un document care să corespundă cererii.";
      }
      else if (numDocuments==1)
      {
        return "Am găsit <b>1</b> document corespunzând cererii.";
      }
      else
      {
        return "Am găsit <b>$num</b> documente corespunzând cererii. "
               "Lista documentelor găsite, sortate după relevanţă.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Găsite:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

     /*! This is used in HTML as the title of page with source code for file filename
      */
     DString trSourceFile(const DString& filename) override
     {
       return " Fişierul sursă " + filename;
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

     /*! This is used as the name of the chapter containing the directory
      *  hierarchy.
      */
     DString trDirIndex() override
     { return "Ierarhia directoarelor"; }

     /*! This is used as the name of the chapter containing the documentation
      *  of the directories.
      */
     DString trDirDocumentation() override
     { return "Documentaţia directoarelor"; }

     /*! This is used as the title of the directory index and also in the
      *  Quick links of a HTML page, to link to the directory hierarchy.
      */
     DString trDirectories() override
     { return "Directoare"; }

     /*! This returns the title of a directory page. The name of the
      *  directory is passed via \a dirName.
      */
     DString trDirReference(const DString &dirName) override
     {
       DString  result="Director-referinţă "; result+=dirName;
       return result;
     }

     /*! This returns the word directory with or without starting capital
      *  (\a first_capital) and in singular or plural form (\a singular).
      */
     DString trDir(bool first_capital, bool singular) override
     {
      return createNoun(first_capital, singular, "directo", "are", "r");
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Aceasta este o funcţie membră suprascrisă. "
              "Diferă de funcţia de mai sus "
              "doar prin argumentele acceptate.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Graful de apeluri pentru această funcţie:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Documentaţia Enumeratorilor"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Documentaţia Funcţiei Membre/Subrutinei"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Lista Tipurilor de Date"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Câmpuri de date"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Tipurile de date, cu scurte descrieri:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Lista tuturor tipurilor de date ";
      if (!extractAll)
      {
        result+="documentate ";
      }
      result+=" cu legături către ";
      if (!extractAll)
      {
         result+="documentaţia structurii de date pentru fiecare membru";
      }
      else
      {
         result+="tipurile de date de care aparţin:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Indexul Tipurilor de Date"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Documentaţia Tipurilor de Date"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funcţii/Subrutine"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Documentaţia Funcţiilor/Subrutinelor"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Tipuri de Date"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Lista Modulelor"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Lista tuturor modulelor ";
      if (!extractAll) result+="documentate ";
      result+="cu scurte descrieri:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Referinţă la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="Modulul "; break;
        case ClassDef::Struct:     result+="Tipul "; break;
        case ClassDef::Union:      result+="Uniunea "; break;
        case ClassDef::Interface:  result+="Interfaţa "; break;
        case ClassDef::Protocol:   result+="Protocolul "; break;
        case ClassDef::Category:   result+="Categoria "; break;
        case ClassDef::Exception:  result+="Excepţia "; break;
        default: break;
      }
      if (isTemplate) result+="(Template) ";
	  result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result="Referinţă la Modulul ";
	  result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Membrii Modulului"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Lista tuturor membrilor ";
      if (!extractAll) result+="documentaţi ai ";
      result+="modulului cu legături către ";
      if (extractAll)
      {
        result+="documentaţia modulului pentru fiecare membru:";
      }
      else
      {
        result+="modulele de care aparţin:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Indexul Modulelor"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "ele", "ul");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="Documentaţia ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulului"; break;
        case ClassDef::Struct:     result+="tipului"; break;
        case ClassDef::Union:      result+="uniunii"; break;
        case ClassDef::Interface:  result+="interfeţei"; break;
        case ClassDef::Protocol:   result+="protocolului"; break;
        case ClassDef::Category:   result+="categoriei"; break;
        case ClassDef::Exception:  result+="excepţiei"; break;
        default: break;
      }
      result+=" a fost generată din următo";
      if (single) result+="rul fişier:"; else result+="arele fişiere:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "urile", "ul");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "ele", "ul");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Constrângerile de Tip";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name+" Relație";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Se încarcă...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Namespace Global";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Căutare...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Niciun rezultat";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    DString trFileIn(const DString &name) override
    {
      return "Fișierul din "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Include fișierul din "+name;
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
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Luni","Marți","Miercuri","Joi","Vineri","Sâmbătă","Duminică" };
      static const char *months[] = { "Ian","Feb","Mar","Apr","Mai","Iun","Iul","Aug","Sep","Oct","Noi","Dec" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "lun.", "mar.", "mie.", "joi", "vin.", "sâm.", "dum." };
      static const char *days_full[]    = { "luni", "mar?i", "miercuri", "joi", "vineri", "sâmbătă", "duminică" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "ian.", "feb.", "mar.", "apr.", "mai", "iun.", "iul.", "aug.", "sept.", "oct.", "nov.", "dec." };
      static const char *months_full[]  = { "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie", "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "a.m.", "p.m." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Referințe Bibliografice"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Grafic de dependență a directoarelor pentru ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "nivel de detaliu"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Parametri Template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "și încă " + number; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "Documentația pentru acest enum a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return name+" Referință Enum"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" moștenit(e) din "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Membri Moșteniți Adiționali"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "activa" : "dezactiva";
      return "apasă click pentru a "+opt+" sincronizarea panourilor";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Furnizat de categoria @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Extinde clasa @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Metodele Clasei";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Metodele Instanței";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Documentația Metodelor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Interfețe exportate"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Servicii Incluse"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Grupuri Constante"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Referință Grup Constant";
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Referință Serviciu";
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Referință Singleton";
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Documentația pentru acest serviciu "
                                "a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="Documentația pentru acest singleton "
                                "a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }

};

#endif
