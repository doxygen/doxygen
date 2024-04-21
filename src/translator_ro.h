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
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[romanian]{babel}\n";
    }
    QCString trISOLang() override
    {
      return "ro";
    }
    QCString getLanguageString() override
    {
      return "0x418 Romanian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funcţii înrudite"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Atenţie: acestea nu sunt funcţii membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Descriere Detaliată"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detalii"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentaţia Definiţiilor de Tipuri (typedef) Membre"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentaţia Enumerărilor Membre"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Documentaţia Funcţiilor Membre"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Mai mult..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista tuturor membrilor"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Lista Membrilor"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Lista completă a membrilor din"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", inclusiv a tuturor membrilor moşteniţi."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Generat automat de Doxygen";
      if (!s.isEmpty()) result+=" pentru "+s;
      result+=" din codul sursă.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nume enumerare"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "valoare enumerare"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definit în"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Module"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Ierarhia Claselor"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Lista fişierelor"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Pagini înrudite"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Exemple"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Caută"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Această listă de moşteniri este sortată în general, "
             "dar nu complet, în ordine alfabetică:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Lista tuturor ";
      result+="fişierelor";
	  if (!extractAll) result+=" documentate";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Lista tuturor ";

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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Lista tuturor ";
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
    QCString trExamplesDescription() override
    { return "Lista tuturor exemplelor:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Lista tuturor documentaţiilor înrudite:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Lista tuturor modulelor:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Documentaţie"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Indexul Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Index Ierarhic"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Indexul Fişierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Documentaţia Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Documentaţia Fişierelor"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Manual de utilizare"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definiţii"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definiţii de tipuri"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerări"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funcţii"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variabile"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Valori de enumerări"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Documentaţia definiţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Documentaţia definiţiilor de tipuri"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Documentaţia enumerărilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Documentaţia funcţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Documentaţia variabilelor"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generat "+date;
      if (!projName.isEmpty()) result+=" pentru "+projName;
      result+=" de către";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagrama de relaţii pentru "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Atenţie"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versiunea"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Întoarce"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Vezi şi"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Excepţii"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generat de"; }

    // new since 0.49-990307

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Lista de Namespace-uri"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Lista tuturor ";
      result+="namespace-urilor ";
	  if (!extractAll) result+="documentate ";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Prieteni"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Documentaţia funcţiilor prietene sau înrudite"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
	  QCString result="Referinţă la ";
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Referinţă la fişierul ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Referinţă la Namespace-ul ";
      result+=namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers() override
    { return "Metode Publice"; }
    QCString trPublicSlots() override
    { return "Conectori (slots) Publici"; }
    QCString trSignals() override
    { return "Semnale"; }
    QCString trStaticPublicMembers() override
    { return "Metode Statice Publice"; }
    QCString trProtectedMembers() override
    { return "Metode Protejate"; }
    QCString trProtectedSlots() override
    { return "Conectori (slots) Protejaţi"; }
    QCString trStaticProtectedMembers() override
    { return "Metode Statice Protejate"; }
    QCString trPrivateMembers() override
    { return "Metode Private"; }
    QCString trPrivateSlots() override
    { return "Conectori (slots) Privaţi"; }
    QCString trStaticPrivateMembers() override
    { return "Metode Statice Private"; }

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
            result+=" şi ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Moşteneşte "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Moştenit de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementat din "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementat în "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Membrii Namespace-ului"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Lista tuturor membrilor ";
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
    QCString trNamespaceIndex() override
    { return "Indexul Namespace-ului"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Documentaţia Namespace-ului"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Namespace-uri"; }

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
      QCString result="Documentaţia pentru această ";
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
    QCString trReturnValues() override
    { return "Valori returnate"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Pagina principală"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "pg."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definiţia în linia @0 a fişierului @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definiţia în fişierul @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Învechită(Deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagrama de relaţii pentru "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graful dependenţelor prin incluziune pentru "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Documentaţia pentru Constructori şi Destructori";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Vezi sursele.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Vezi documentaţia.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondiţie";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondiţie";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Valoarea iniţială:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "cod";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Ierarhia Claselor în mod grafic";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Vezi ierarhia claselor în mod grafic";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Vezi ierarhia claselor în mod text";
    }
    QCString trPageIndex() override
    {
      return "Indexul Paginilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Notă";
    }
    QCString trPublicTypes() override
    {
      return "Tipuri Publice";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Atribute Statice Publice";
    }
    QCString trProtectedTypes() override
    {
      return "Tipuri Protejate";
    }
    QCString trProtectedAttribs() override
    {
      return "Atribute Protejate";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Atribute Statice Protejate";
    }
    QCString trPrivateTypes() override
    {
      return "Tipuri Private";
    }
    QCString trPrivateAttribs() override
    {
      return "Atribute Private";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Atribute Statice Private";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "De făcut";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Lista lucrurilor de făcut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Semnalat de";
    }
    QCString trRemarks() override
    {
      return "Observaţii";
    }
    QCString trAttention() override
    {
      return "Atenţie";
    }
    QCString trInclByDepGraph() override
    {
      return "Acest graf arată care fişiere includ, "
		  "direct sau indirect, acest fişier:";
    }
    QCString trSince() override
    {
      return "Din";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Legenda grafului";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
      return "Listă de teste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Proprietăţi";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Documentaţia Proprietăţilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Pachet "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Lista pachetelor, însoţită de scurte explicaţii, acolo unde acestea există:";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pachete";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Valoare:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Problema (Bug)";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1250"; //EASTEUROPE_CHARSET
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "238";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "clas", "ele", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fişier", "ele", "ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "-urile", "-ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupu", "rile", "l");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pagin", "ile", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membr", "ii", "ul");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "e");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "ii", "ul");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referinţe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementează "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementat în "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Cuprins";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista elementelor învechite (deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Evenimente";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Documentaţia aferentă evenimentelor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipuri în pachet";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funcţii în pachet";
    }
    QCString trPackageMembers() override
    {
      return "Membrii în pachet";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Funcţii statice în pachet";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atribute în pachet";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Atribute statice în pachet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Toate";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Graful de apel al acestei funcţii:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Găsite:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

     /*! This is used in HTML as the title of page with source code for file filename
      */
     QCString trSourceFile(const QCString& filename) override
     {
       return " Fişierul sursă " + filename;
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

     /*! This is used as the name of the chapter containing the directory
      *  hierarchy.
      */
     QCString trDirIndex() override
     { return "Ierarhia directoarelor"; }

     /*! This is used as the name of the chapter containing the documentation
      *  of the directories.
      */
     QCString trDirDocumentation() override
     { return "Documentaţia directoarelor"; }

     /*! This is used as the title of the directory index and also in the
      *  Quick links of a HTML page, to link to the directory hierarchy.
      */
     QCString trDirectories() override
     { return "Directoare"; }

     /*! This returns the title of a directory page. The name of the
      *  directory is passed via \a dirName.
      */
     QCString trDirReference(const QCString &dirName) override
     {
       QCString  result="Director-referinţă "; result+=dirName;
       return result;
     }

     /*! This returns the word directory with or without starting capital
      *  (\a first_capital) and in sigular or plural form (\a singular).
      */
     QCString trDir(bool first_capital, bool singular) override
     {
      return createNoun(first_capital, singular, "directo", "are", "r");
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Aceasta este o funcţie membră suprascrisă. "
              "Diferă de funcţia de mai sus "
              "doar prin argumentele acceptate.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Graful de apeluri pentru această funcţie:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentaţia Enumeratorilor"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentaţia Funcţiei Membre/Subrutinei"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lista Tipurilor de Date"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Câmpuri de date"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Tipurile de date, cu scurte descrieri:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Lista tuturor tipurilor de date ";
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
    QCString trCompoundIndexFortran() override
    { return "Indexul Tipurilor de Date"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentaţia Tipurilor de Date"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funcţii/Subrutine"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentaţia Funcţiilor/Subrutinelor"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipuri de Date"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Lista Modulelor"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Lista tuturor modulelor ";
      if (!extractAll) result+="documentate ";
      result+="cu scurte descrieri:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Referinţă la ";
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Referinţă la Modulul ";
	  result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Membrii Modulului"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Lista tuturor membrilor ";
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
    QCString trModulesIndex() override
    { return "Indexul Modulelor"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "ele", "ul");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Documentaţia ";
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "urile", "ul");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "ele", "ul");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Constrângerile de Tip";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Relație";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Se încarcă...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Namespace Global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Căutare...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Fișierul din "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Luni","Marți","Miercuri","Joi","Vineri","Sâmbătă","Duminică" };
      static const char *months[] = { "Ian","Feb","Mar","Apr","Mai","Iun","Iul","Aug","Sep","Oct","Noi","Dec" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
      static const char *days_short[]   = { "lun.", "mar.", "mie.", "joi", "vin.", "sâm.", "dum." };
      static const char *days_full[]    = { "luni", "mar?i", "miercuri", "joi", "vineri", "sâmbătă", "duminică" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "ian.", "feb.", "mar.", "apr.", "mai", "iun.", "iul.", "aug.", "sept.", "oct.", "nov.", "dec." };
      static const char *months_full[]  = { "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie", "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "a.m.", "p.m." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Referințe Bibliografice"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Grafic de dependență a directoarelor pentru ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "nivel de detaliu"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parametri Template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "și încă " + number; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Documentația pentru acest enum a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Referință Enum"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" moștenit(e) din "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Membri Moșteniți Adiționali"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "activa" : "dezactiva";
      return "apasă click pentru a "+opt+" sincronizarea panourilor";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Furnizat de categoria @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Extinde clasa @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Metodele Clasei";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Metodele Instanței";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Documentația Metodelor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Interfețe exportate"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Servicii Incluse"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Grupuri Constante"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Referință Grup Constant";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Referință Serviciu";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Referință Singleton";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Documentația pentru acest serviciu "
                                "a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Documentația pentru acest singleton "
                                "a fost generată din ";
      if (single)
        result += "următorul fișier:";
      else
        result += "următoarele fișiere:";
      return result;
    }

};

#endif
