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

 /* Tradukita kaj ĝisdatigata de Ander Martinez. */

#ifndef TRANSLATOR_EO_H
#define TRANSLATOR_EO_H

/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionaly to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorEsperanto : public Translator
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[esperanto]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Rilataj Funkcioj"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Atentu ke tiuj ĉi ne estas membraj funkcioj.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detala Priskribo"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentado de la Membraj Tipodifinoj"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentado de la Membraj Enumeracioj"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentado de la Membraj Funkcioj"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Kampa Dokumentado";
      }
      else
      {
        return "Dokumentado de la Membraj Datumoj";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Pli..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Listo de ĉiuj membroj"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Membra Listo"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Tiu ĉi estas la kompleta membraro de "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", inkluzive ĉiujn hereditajn membrojn."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generita aŭtomate de Doxygen";
      if (s) result+=(QCString)" por "+s;
      result+=" el la fontkodo.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum nomo"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum valoro"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "difinita en"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduloj"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klasa Hierarkio"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datumstruktoroj";
      }
      else
      {
        return "Klasaro";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Dosieraro"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datumkampoj";
      }
      else
      {
        return "Klasaj membroj";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Mallokalaĵoj";
      }
      else
      {
        return "Dosieraj Membroj";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Rilataj Paĝoj"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Ekzemploj"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Serĉi"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Tiu ĉi heredada listo estas plimalpli, "
             "sed ne tute, ordigita alfabete:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="dosieroj kun mallongaj priskriboj:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Jen datumstrukturoj kun mallongaj priskriboj:";
      }
      else
      {
        return "Jen la klasoj, strukturoj, kunigoj kaj interfacoj "
               "kun mallongaj priskriboj:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
      if (!extractAll)
      {
        result+="dokumentitaj ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Jen listo de ĉiuj la ekzemploj:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Jen listo de ĉiuj rilataj dokumentadaj paĝoj:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Jen listo de ĉiuj la moduloj:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modula Indekso"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarkia Indekso"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex()
    { return "Dosiera Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modula Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datumstruktura Dokumentado";
      }
      else
      {
        return "Klasa Dokumentado";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Dosiera Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Ekzempla Dokumentado"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Paĝa Dokumentado"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referenca Manlibro"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Difinoj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Funkciaj Prototipoj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Tipdifinoj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumeracioj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funkcioj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variabloj"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Enumeraciilo"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Difina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentado de Funkciaj Prototipoj"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Tipdifina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Enumeracitipa Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funkcia Dokumentado"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Variabla Dokumentado"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Generita la "+date;
      if (projName) result+=(QCString)" por "+projName;
      result+=(QCString)" de";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "skribita de";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Heredada diagramo por "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Nur por ena uzado."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Averto"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versio"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Dato"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Liveras"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Vido ankaŭ"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametroj"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Esceptoj"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generita de"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Nomspacaro"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="nomspacoj kun mallongaj priskriboj:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Amikoj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Dokumentado pri amikoj kaj rilatitaj funkcioj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referenco de la ";
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
      }
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Dosiera referenco";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Nomspaca referenco";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Publikaj Membraj Funkcioj"; }
    virtual QCString trPublicSlots()
    { return "Pubikaj Ingoj"; }
    virtual QCString trSignals()
    { return "Signaloj"; }
    virtual QCString trStaticPublicMembers()
    { return "Statikaj Publikaj Membraj Funkcioj"; }
    virtual QCString trProtectedMembers()
    { return "Protektitaj Membraj Funkcioj"; }
    virtual QCString trProtectedSlots()
    { return "Protektitaj Ingoj"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statikaj Protektitaj Membraj Funkcioj"; }
    virtual QCString trPrivateMembers()
    { return "Privataj Membraj Funkcioj"; }
    virtual QCString trPrivateSlots()
    { return "Privataj Ingoj"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statikaj Privataj Membraj Funkcioj"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
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
            result+=", kaj ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Heredas de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Heredita de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Rerealigita el "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Rerealigita en "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Nomspacaj Membroj"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
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
    virtual QCString trNamespaceIndex()
    { return "Nomspaca Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Nomspaca Dokumentado"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Nomspacoj"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // single is true implies a single file
      QCString result=(QCString)"La dokumentado por tiu ĉi ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klaso"; break;
        case ClassDef::Struct:     result+="strukturo"; break;
        case ClassDef::Union:      result+="kunigo"; break;
        case ClassDef::Interface:  result+="interfaco"; break;
        case ClassDef::Protocol:   result+="protokolo"; break;
        case ClassDef::Category:   result+="kategorio"; break;
        case ClassDef::Exception:  result+="escepto"; break;
      }
      result+=" generitas el la ";
      if (single) result+="sekva dosiero:";
      else result+="sekvaj dosieroj:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Alfabeta Listo"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Liveraĵoj"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Ĉefa Paĝo"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Difinita sur la lineo @0 de la dosiero @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Difinita en la dosiero @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Evitinda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Kunlaborada diagramo por "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Inkluzivaĵa dependeca diagramo por "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Konstruila kaj Detruila Dokumentado";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Iri al la fontkodo de tiu ĉi dosiero.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Iri al la dokumentado de tiu ĉi dosiero.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Antaŭkondiĉo";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postkondiĉo";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Malvariaĵo";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Komenca valoro:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "kodo";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafika Klasa Hierarkio";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Iri al la grafika klasa hierarkio";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Iri al la teksta klasa hierarkio";
    }
    virtual QCString trPageIndex()
    {
      return "Paĝa Indekso";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Noto";
    }
    virtual QCString trPublicTypes()
    {
      return "Publikaj Tipoj";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datumkampoj";
      }
      else
      {
        return "Publikaj Atributoj";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statikaj Publikaj Atributoj";
    }
    virtual QCString trProtectedTypes()
    {
      return "Protektitaj Tipoj";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Protektitaj Atributoj";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statikaj Protektitaj Atributoj";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privataj Tipoj";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privataj Atributoj";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statikaj Privataj Atributoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Farendaĵo";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Farendaĵaro";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referencita de";
    }
    virtual QCString trRemarks()
    {
      return "Rimarkoj";
    }
    virtual QCString trAttention()
    {
      return "Atentu";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Tiu ĉi diagramo montras kiuj dosieroj rekte aŭ malrekte "
             "inkluzivas tiun ĉi dosieron:";
    }
    virtual QCString trSince()
    {
      return "De";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Diagrama Klarigeto";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "klarigeto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Testo";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Testa Listo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Membraj Funkcioj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Atributoj";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Atributa Dokumentado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Datumstrukturoj";
      }
      else
      {
        return "Klasoj";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Pakaĵo "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Pakaĵa Listo";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Jen listo de pakaĵoj kun mallongaj priskriboj (se ekzistas):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pakaĵoj";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Valoro:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Cimo";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "1";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Indekso";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Klaso" : "klaso"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Dosiero" : "dosiero"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Nomspaco" : "nomspaco"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Grupo" : "grupo"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Paĝo" : "paĝo"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Membro" : "membro"));
      if (!singular)  result+="j";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Mallokalaĵo" : "mallokalaĵo"));
      if (!singular)  result+="j";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Aŭtoro" : "aŭtoro"));
      if (!singular)  result+="j";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referencoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Realigas "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Realigita en "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Enhava Tabelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Evitindaĵa Listo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Eventoj";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Eventa Dokumentado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Pakaĵaj Tipoj";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Pakaĵaj Funkcioj";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statikaj Pakaĵaj Funkcioj";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Pakaĵaj Atributoj";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statikaj Pakaĵaj Atributoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Ĉiuj";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Jen la vokdiagramo por tiu ĉi funkcio:";
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
      return "Serĉi";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Kongruoj:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Fonta Dosiero";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Dosieruja Hierarkio"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dosieruja Dokumentado"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Dosierujoj"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Tiu ĉi dosieruja hierarkio estas plimalpli, "
             "sed ne tute, ordigita alfabete:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Dosieruja Referenco"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Dosierujo" : "dosierujo"));
      if (!singular) result+="j";
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
       return "Tiu ĉi estas superŝarĝita membra funkcio, "
              "donita por faciligo. Ĝi nur malsamas de la supra "
              "funkcio nur pro la argumento(j) kiujn ili akceptas.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Jen la vokdiagramo por tiu ĉi funkcio:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Enumeraciila Dokumentado"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Dokumentado de Membraj Funkcioj/Subrutinoj"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Datumtipa Listo"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Datumkampoj"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Jen la datumtipoj kun mallongaj priskriboj:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
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
    virtual QCString trCompoundIndexFortran()
    { return "Datumtipa Indekso"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Datumtipa Dokumentado"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funkcioj/Subrutinoj"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Funkcia/Subrutina Dokumentado"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datumtipoj"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modula Listo"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
      if (!extractAll) result+="dokumentitaj ";
      result+="moduloj kun mallongaj priskriboj:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referenco de la ";
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
      }
      result+=(QCString)clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Modula Referenco";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modulaj Membroj"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result="Jen listo de ĉiuj ";
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
    virtual QCString trModulesIndex()
    { return "Indekso de Moduloj"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modulo" : "modulo"));
      if (!singular)  result+="j";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"La dokumentado por tiu ĉi ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulo"; break;
        case ClassDef::Struct:     result+="tipo"; break;
        case ClassDef::Union:      result+="kunigo"; break;
        case ClassDef::Interface:  result+="interfaco"; break;
        case ClassDef::Protocol:   result+="protokolo"; break;
        case ClassDef::Category:   result+="kategorio"; break;
        case ClassDef::Exception:  result+="escepto"; break;
      }
      result+=" kreiĝis el la ";
      if (single) result+="sekva dosiero:"; else result+="sekvaj dosieroj:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tipo" : "tipo"));
      if (!singular)  result+="j";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Subprogramo" : "subprogramo"));
      if (!singular)  result+="j";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Tipaj Limigoj";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Rilato";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Ŝarĝante...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Malloka Nomspaco";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Serĉante...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nenia kongruo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Diagramo de dependecoj dosierujaj por "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Dosiero en "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Inkluzivas dosieron en "+name;
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
      static const char *days[]   = { "lundo","mardo","merkredo",
                                        "ĵaŭdo","vendredo","sabato",
                                        "dimanĉo" };

      static const char *months[] = { "Januaro", "Februaro", "Marto",
                                        "Aprilo", "Majo", "Junio",
                                        "Julio", "Aŭgusto", "Septembro",
                                        "Oktobro", "Novembro",
                                        "Decembro"
                                    };
      QCString sdate;
      sdate.sprintf("%s, %d-a de %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    { return "Bibliografiaj Referencoj"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Kopirajto"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Dosieruja dependa diagramo por ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "detala nivelo"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parametroj de ŝablonoj"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "kaj "+number+" pli..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "La dokumentaro por tiu ĉi enum estis generita el la sekva dosiero";
      if (!single) result += "s";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Enum Referenco"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" heredita el "+what; }

    /*! Header of the sections with inherited members specific for the 
     *  base class(es) 
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Kromaj Hereditaj Membroj"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is 
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "aktivigi" : "malaktivigi";
      return "klaku por "+opt+" panelan sinkronigon";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Provizita de kategorio @1.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Etendi klason @1.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Klasaj Metodoj";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Aperaj Metodoj";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Dokumentaro de la Metodo";
    }

    /*! Used as the title of the design overview picture created for the
     *  VHDL output.
     */
    virtual QCString trDesignOverview()
    {
      return "Fasona Superrigardo";
    }

//////////////////////////////////////////////////////////////////////////

};

#endif
