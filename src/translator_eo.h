/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

 /* Tradukita kaj ĝisdatigata de Ander Martinez. */

#ifndef TRANSLATOR_EO_H
#define TRANSLATOR_EO_H

/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorEsperanto : public TranslatorAdapter_1_8_4
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
    { return "esperanto"; }

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
      return "\\usepackage[esperanto]{babel}\n";
    }

    DString trISOLang() override
    {
      return "eo";
    }

    // using fallback see translator.h
    DString getLanguageString() override
    {
      return "0x409 English (United States)";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Rilataj Funkcioj"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Atentu ke tiuj ĉi ne estas membraj funkcioj.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Detala Priskribo"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Detaloj"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Dokumentado de la Membraj Tipodifinoj"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Dokumentado de la Membraj Enumeracioj"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "Dokumentado de la Membraj Funkcioj"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Kampa Dokumentado";
      }
      else
      {
        return "Dokumentado de la Membraj Datumoj";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Pli..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Listo de ĉiuj membroj"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Membra Listo"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Tiu ĉi estas la kompleta membraro de"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", inkluzive ĉiujn hereditajn membrojn."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Generita aŭtomate de Doxygen";
      if (!s.empty()) result+=" por "+s;
      result+=" el la fontkodo.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "enum nomo"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "enum valoro"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "difinita en"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Moduloj"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Klasa Hierarkio"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumstruktoroj";
      }
      else
      {
        return "Klasaro";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Dosieraro"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumkampoj";
      }
      else
      {
        return "Klasaj membroj";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Mallokalaĵoj";
      }
      else
      {
        return "Dosieraj Membroj";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Rilataj Paĝoj"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Ekzemploj"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Serĉi"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Tiu ĉi heredada listo estas plimalpli, "
             "sed ne tute, ordigita alfabete:";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="dosieroj kun mallongaj priskriboj:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Jen datumstrukturoj kun mallongaj priskriboj:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Jen la klasoj kun mallongaj priskriboj:";
      }
      else
      {
        return "Jen la klasoj, strukturoj, kunigoj kaj interfacoj "
               "kun mallongaj priskriboj:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll)
      {
        result+="dokumentitaj ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="strukturaj kaj kunigaj kampoj";
      }
      else
      {
        result+="klasaj membroj";
      }
      result+=" kun ligiloj al ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="la struktura/kuniga dokumentado por ĉiu kampo:";
        }
        else
        {
          result+="la klasa dokumentado por ĉiu membro:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="la strukturoj/kunigoj al kiuj ili apartenas:";
        }
        else
        {
          result+="la klasoj al kiuj ili apartenas:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkcioj, variabloj, difinoj, enumeracioj kaj tipodifinoj";
      }
      else
      {
        result+="dosieraj membroj";
      }
      result+=" kun ligiloj al ";
      if (extractAll)
        result+="la dosieroj al kiuj ili apartenas:";
      else
        result+="la dokumentado:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Jen listo de ĉiuj la ekzemploj:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Jen listo de ĉiuj rilataj dokumentadaj paĝoj:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Jen listo de ĉiuj la moduloj:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Modula Indekso"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Hierarkia Indekso"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumstruktura Indekso";
      }
      else
      {
        return "Klasa Indekso";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Dosiera Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Modula Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumstruktura Dokumentado";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klasa Dokumentado";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Dosiera Dokumentado"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Referenca Manlibro"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Difinoj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Tipdifinoj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Enumeracioj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funkcioj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Variabloj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Enumeraciilo"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Difina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Tipdifina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Enumeracitipa Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Funkcia Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Variabla Dokumentado"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumstrukturoj";
      }
      else
      {
        return "Klasoj";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Generita la "+date;
      if (!projName.empty()) result+=" por "+projName;
      result+=" de";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Heredada diagramo por "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Averto"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Versio"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Dato"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Liveras"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Vido ankaŭ"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametroj"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Esceptoj"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Generita de"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Nomspacaro"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="nomspacoj kun mallongaj priskriboj:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Amikoj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Dokumentado pri amikoj kaj rilatitaj funkcioj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Referenco de la ";
      if (isTemplate) result+=" ŝablono de la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klaso "; break;
        case ClassDef::Struct:     result+="strukturo "; break;
        case ClassDef::Union:      result+="kunigo "; break;
        case ClassDef::Interface:  result+="interfaco "; break;
        case ClassDef::Protocol:   result+="protokolo "; break;
        case ClassDef::Category:   result+="kategorio "; break;
        case ClassDef::Exception:  result+="escepto "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName;
      result+=" Dosiera referenco";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Nomspaca referenco";
      return result;
    }

    DString trPublicMembers() override
    { return "Publikaj Membraj Funkcioj"; }
    DString trPublicSlots() override
    { return "Pubikaj Ingoj"; }
    DString trSignals() override
    { return "Signaloj"; }
    DString trStaticPublicMembers() override
    { return "Statikaj Publikaj Membraj Funkcioj"; }
    DString trProtectedMembers() override
    { return "Protektitaj Membraj Funkcioj"; }
    DString trProtectedSlots() override
    { return "Protektitaj Ingoj"; }
    DString trStaticProtectedMembers() override
    { return "Statikaj Protektitaj Membraj Funkcioj"; }
    DString trPrivateMembers() override
    { return "Privataj Membraj Funkcioj"; }
    DString trPrivateSlots() override
    { return "Privataj Ingoj"; }
    DString trStaticPrivateMembers() override
    { return "Statikaj Privataj Membraj Funkcioj"; }

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
            result+=", kaj ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Heredas de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Heredita de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Rerealigita el "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Rerealigita en "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Nomspacaj Membroj"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="nomspacaj membroj kun ligiloj al ";
      if (extractAll)
        result+="la nomspaca dokumentado de ĉiu membro:";
      else
        result+="la nomspacoj al kiuj ili apartenas:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Nomspaca Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Nomspaca Dokumentado"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Nomspacoj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      DString result="La dokumentado por tiu ĉi ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klaso"; break;
        case ClassDef::Struct:     result+="strukturo"; break;
        case ClassDef::Union:      result+="kunigo"; break;
        case ClassDef::Interface:  result+="interfaco"; break;
        case ClassDef::Protocol:   result+="protokolo"; break;
        case ClassDef::Category:   result+="kategorio"; break;
        case ClassDef::Exception:  result+="escepto"; break;
        default: break;
      }
      result+=" generitas el la ";
      if (single) result+="sekva dosiero:";
      else result+="sekvaj dosieroj:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Liveraĵoj"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Ĉefa Paĝo"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Difinita sur la lineo @0 de la dosiero @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Difinita en la dosiero @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Evitinda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Kunlaborada diagramo por "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Inkluzivaĵa dependeca diagramo por "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Konstruila kaj Detruila Dokumentado";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Iri al la fontkodo de tiu ĉi dosiero.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Iri al la dokumentado de tiu ĉi dosiero.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Antaŭkondiĉo";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Postkondiĉo";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Malvariaĵo";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Komenca valoro:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "kodo";
    }
    DString trGraphicalHierarchy() override
    {
      return "Grafika Klasa Hierarkio";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Iri al la grafika klasa hierarkio";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Iri al la teksta klasa hierarkio";
    }
    DString trPageIndex() override
    {
      return "Paĝa Indekso";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Noto";
    }
    DString trPublicTypes() override
    {
      return "Publikaj Tipoj";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumkampoj";
      }
      else
      {
        return "Publikaj Atributoj";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Statikaj Publikaj Atributoj";
    }
    DString trProtectedTypes() override
    {
      return "Protektitaj Tipoj";
    }
    DString trProtectedAttribs() override
    {
      return "Protektitaj Atributoj";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Statikaj Protektitaj Atributoj";
    }
    DString trPrivateTypes() override
    {
      return "Privataj Tipoj";
    }
    DString trPrivateAttribs() override
    {
      return "Privataj Atributoj";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Statikaj Privataj Atributoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "Farendaĵo";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Farendaĵaro";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Referencita de";
    }
    DString trRemarks() override
    {
      return "Rimarkoj";
    }
    DString trAttention() override
    {
      return "Atentu";
    }
    DString trInclByDepGraph() override
    {
      return "Tiu ĉi diagramo montras kiuj dosieroj rekte aŭ malrekte "
             "inkluzivas tiun ĉi dosieron:";
    }
    DString trSince() override
    {
      return "De";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Diagrama Klarigeto";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
    {
      return
        "Tiu ĉi paĝo klarigas kiel interpreti la diagramojn generitajn "
        "de doxygen.<p>\n"
        "Konsideru la sekvan ekzemplon:\n"
        "\\code\n"
        "/*! Nevidebla klaso pro trunkado */\n"
        "class Invisible { };\n\n"
        "/*! Trunkita klaso, hereda rilato kaŝiĝas */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klaso ne dokumentita per komentoj de doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Klaso de kiu herediĝas per publika heredado */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Ŝablona klaso */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klaso de kiu herediĝas per protektita heredado */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klaso de kiu herediĝas per privata heredado */\n"
        "class PrivateBase { };\n\n"
        "/*! Klaso uzata de la klaso Inherited */\n"
        "class Used { };\n\n"
        "/*! Supra klaso kiu heredas de kelkaj aliaj klasoj */\n"
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
        "Tio ĉi liveros la sekvan diagramon:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "La skatoloj de la supra diagramo havas la sekvajn signifojn:\n"
        "<ul>\n"
        "<li>%Plene griza skatolo reprezentas la strukturon aŭ klason "
        "kies diagramo generiĝis.\n"
        "<li>%Skatolo kun nigra bordero montras dokumentitan strukturon aŭ klason.\n"
        "<li>%Skatolo kun griza bordero montras nedokumentitan strukturon aŭ klason.\n"
        "<li>%Skatolo kun ruĝa bordero montras dokumentitan strukturon aŭ klason por "
        "kiu ne ĉiuj heredadoj/enhavoj montriĝas. %Diagramo estas trunkota "
        "se ĝi ne adaptiĝas en la donitajn limojn.\n"
        "</ul>\n"
        "La sagoj havas la sekvajn signifojn:\n"
        "<ul>\n"
        "<li>%Malhelblua sago uzatas por montri publika heredado "
        "inter du klasoj.\n"
        "<li>%Malhelverda sago uzatas por protektita heredado.\n"
        "<li>%Malhelruĝa sago uzatas por privata heredado.\n"
        "<li>%Purpura streka sago uzatas se klaso enhavatas aŭ uzatas "
        "de alia klaso. La sago estas etikedatas kun la variablo(j) "
        "tra kiu la montrita klaso aŭ strukturo estas alirebla.\n"
        "<li>%Flava streka sago montras rilato inter ŝablona apero kaj "
        "la ŝablona klaso el kiu ĝi realigitas. La sago etikeditas kun "
        "la parametroj de la ŝablona apero.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "klarigeto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Testo";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Testa Listo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Atributoj";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Atributa Dokumentado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datumstrukturoj";
      }
      else
      {
        return "Klasoj";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Pakaĵo "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Jen listo de pakaĵoj kun mallongaj priskriboj (se ekzistas):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Pakaĵoj";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Valoro:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Cimo";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Cima Listo";
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
      return "";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "1";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Indekso";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klaso", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "dosiero", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "nomspaco", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupo", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "paĝo", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membro", "j");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "mallokalaĵo", "j");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "aŭtoro", "j");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Referencoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Realigas "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Realigita en "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Enhava Tabelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Evitindaĵa Listo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Eventoj";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Eventa Dokumentado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Pakaĵaj Tipoj";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Pakaĵaj Funkcioj";
    }
    DString trPackageMembers() override
    {
      return "Pakaĵaj Membroj";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statikaj Pakaĵaj Funkcioj";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Pakaĵaj Atributoj";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statikaj Pakaĵaj Atributoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Ĉiuj";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Jen la vokdiagramo por tiu ĉi funkcio:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Serĉaj Rezultoj";
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
        return "Pardonu, nenio dokumento kongruas vian peton.";
      }
      else if (numDocuments==1)
      {
        return "Trafita <b>unu</b> dokumenton kongruantan vian peton.";
      }
      else
      {
        return "Trafitaj <b>$num</b> dokumentojn kongruantajn vian peton. "
               "Montriĝos plej bonaj kongruoj unue.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Kongruoj:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " Fonta Dosiero";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Dosieruja Hierarkio"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Dosieruja Dokumentado"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Dosierujoj"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" Dosieruja Referenco"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "dosierujo", "j");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Tiu ĉi estas superŝarĝita membra funkcio, "
              "donita por faciligo. Ĝi nur malsamas de la supra "
              "funkcio nur pro la argumento(j) kiujn ili akceptas.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Jen la vokdiagramo por tiu ĉi funkcio:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Enumeraciila Dokumentado"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Dokumentado de Membraj Funkcioj/Subrutinoj"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Datumtipa Listo"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Datumkampoj"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Jen la datumtipoj kun mallongaj priskriboj:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll)
      {
        result+="dokumentitaj ";
      }
      result+="datumtipaj membroj";
      result+=" kun ligiloj al ";
      if (!extractAll)
      {
         result+="la datumstruktura dokumentado de ĉiu membro";
      }
      else
      {
         result+="la datumtipoj al kiuj ili apartenas:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Datumtipa Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Datumtipa Dokumentado"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funkcioj/Subrutinoj"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Funkcia/Subrutina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Datumtipoj"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Modula Listo"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="moduloj kun mallongaj priskriboj:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Referenco de la ";
      if (isTemplate) result+=" ŝablono de la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulo "; break;
        case ClassDef::Struct:     result+="tipo "; break;
        case ClassDef::Union:      result+="kunigo "; break;
        case ClassDef::Interface:  result+="interfaco "; break;
        case ClassDef::Protocol:   result+="protokolo "; break;
        case ClassDef::Category:   result+="kategorio "; break;
        case ClassDef::Exception:  result+="escepto "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Modula Referenco";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Modulaj Membroj"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="modulaj membroj kun ligiloj al la ";
      if (extractAll)
      {
        result+="modula dokumentado de ĉiu membro:";
      }
      else
      {
        result+="moduloj al kiuj ili apartenas:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Indekso de Moduloj"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modulo", "j");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      DString result="La dokumentado por tiu ĉi ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulo"; break;
        case ClassDef::Struct:     result+="tipo"; break;
        case ClassDef::Union:      result+="kunigo"; break;
        case ClassDef::Interface:  result+="interfaco"; break;
        case ClassDef::Protocol:   result+="protokolo"; break;
        case ClassDef::Category:   result+="kategorio"; break;
        case ClassDef::Exception:  result+="escepto"; break;
        default: break;
      }
      result+=" kreiĝis el la ";
      if (single) result+="sekva dosiero:"; else result+="sekvaj dosieroj:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tipo", "j");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogramo", "j");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Tipaj Limigoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name+" Rilato";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Ŝarĝante...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Malloka Nomspaco";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Serĉante...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Nenia kongruo";
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
      return "Dosiero en "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Inkluzivas dosieron en "+name;
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
      static const char *days[]   = { "lundo","mardo","merkredo",
                                        "ĵaŭdo","vendredo","sabato",
                                        "dimanĉo" };

      static const char *months[] = { "Januaro", "Februaro", "Marto",
                                        "Aprilo", "Majo", "Junio",
                                        "Julio", "Aŭgusto", "Septembro",
                                        "Oktobro", "Novembro",
                                        "Decembro"
                                    };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d-a de %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
      static const char *days_short[]   = { "lu", "ma", "me", "ĵa", "ve", "sa", "di" };
      static const char *days_full[]    = { "lundo", "mardo", "merkredo", "ĵaŭdo", "vendredo", "sabato", "dimanĉo" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mar", "apr", "maj", "jun", "jul", "aŭg", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januaro", "februaro", "marto", "aprilo", "majo", "junio", "julio", "aŭgusto", "septembro", "oktobro", "novembro", "decembro" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "atm", "ptm" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Bibliografiaj Referencoj"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Kopirajto"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Dosieruja dependa diagramo por ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "detala nivelo"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Parametroj de ŝablonoj"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "kaj "+number+" pli..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "La dokumentaro por tiu ĉi enum estis generita el la sekva dosiero";
      if (!single) result += "s";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return name+" Enum Referenco"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" heredita el "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Kromaj Hereditaj Membroj"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "aktivigi" : "malaktivigi";
      return "klaku por "+opt+" panelan sinkronigon";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Provizita de kategorio @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Etendi klason @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Klasaj Metodoj";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Aperaj Metodoj";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Dokumentaro de la Metodo";
    }
};

#endif
