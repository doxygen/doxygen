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

#ifndef TRANSLATOR_LV_H
#define TRANSLATOR_LV_H

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

/* Translation from English to Latvian by Lauris Bukšis-Haberkorns
 *  (lauris@nix.lv)
 *  -------------------------------------------
 *  Project start                  : 24.Sept.2012
 *  Last Doxygen version covered   : 1.8.2
 */

class TranslatorLatvian : public TranslatorAdapter_1_16_0
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
    { return "latvian"; }

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
     */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[latvian]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "lv";
    }
    QCString getLanguageString() override
    {
      return "0x426 Latvian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Saistītās funkcijas"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Šīs nav elementu funkcijas.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Detalizēts apraksts"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Sīkāka informācija"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Elementa Typedef dokumentācija"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Elementa uzskaitījumliterāļa dokumentācija"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Elementa funkcijas dokumentācija"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Lauka dokumentācija";
      }
      else
      {
        return "Elementa datu dokumentācija";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "vairāk..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Visu elementu saraksts"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Elementu saraksts"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Šis ir pilns elementu saraksts klasei"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", ieskaitot mantotos elementus."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automātiski ģenerēts izmantojot Doxygen";
      if (!s.isEmpty()) result+=" priekš "+s;
      result+=" no pirmkoda.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "uzskaitījuma nosaukums"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "uzskaitījuma vērtība"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definēts"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduļi"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Klašu hierarhija"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūras";
      }
      else
      {
        return "Klašu saraksts";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Failu saraksts"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu lauki";
      }
      else
      {
        return "Klases elementi";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globālie";
      }
      else
      {
        return "Faila elementi";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Saistītās lapas"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Piemēri"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Meklēt"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Šis mantojamības saraksts ir sakārtots aptuveni, "
             "bet ne pilnībā, alfabēta secībā:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";
      result+="failu saraksts ar īsu aprakstu:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Šeit ir visas datu struktūras ar īsu aprakstu:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Šeit ir visas klases ar īsu aprakstu:";
      }
      else
      {
        return "Šeit ir visas klases, struktūras, "
               "apvienojumi un interfeisi ar īsu aprakstu:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll)
      {
        result+="dokumentēto ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struktūru un apvienojumu lauku";
      }
      else
      {
        result+="klases elementu";
      }
      result+=" saraksts ar saitēm uz ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūru/apvienojumu dokumentāciju katram laukam:";
        }
        else
        {
          result+="klases dokumentāciju katram elementam:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūrām/apvienojumiem, kam tie pieder:";
        }
        else
        {
          result+="klasēm, kam tie pieder:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funkciju, mainīgo, definīciju, uzskaitījumliterāļu un typedefs";
      }
      else
      {
        result+="faila elementu";
      }
      result+=" saraksts ar saitēm uz ";
      if (extractAll)
        result+="failiem, kam tie pieder:";
      else
        result+="dokumentāciju:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Šeit ir visu piemēru saraksts:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Here is a list of all related documentation pages:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Šeit ir visu moduļu saraksts:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentācija"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Moduļu indekss"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hierarhijas indekss"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūru indess";
      }
      else
      {
        return "Klašu indekss";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Failu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Moduļa dokumentācija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūras dokomentācija";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klases dokumentācija";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Faila dokumentācija"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Rokasgrāmata"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Makro"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Uzskaitījumi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funkcijas"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Mainīgie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Uzskaitījumliterāļi"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Makro definīcijas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Typedef dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Uzskaitījumliterāļa tipa dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Funkcijas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Mainīgo dokumentācija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūras";
      }
      else
      {
        return "Klases";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Ģenerēts "+date;
      if (!projName.isEmpty()) result+=" projektam "+projName;
      result+=" ar";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Mantojamības diagramma klasei "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Brīdinājums"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versija"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Datums"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Atgriež"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Skatīties arī"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Izņēmumi"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Ģenerēts ar"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Nosaukumvietu saraksts"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Šeit ir visas ";
      if (!extractAll) result+="dokumentētās ";
      result+="nosaukumvietas ar īsu aprakstu:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Draugi"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Draugu un saistīto funkciju dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" klases"; break;
        case ClassDef::Struct:     result+=" struktūras"; break;
        case ClassDef::Union:      result+=" apvienojuma"; break;
        case ClassDef::Interface:  result+=" interfeisa"; break;
        case ClassDef::Protocol:   result+=" protokola"; break;
        case ClassDef::Category:   result+=" kategorijas"; break;
        case ClassDef::Exception:  result+=" izņēmuma"; break;
        default: break;
      }
      if (isTemplate) result+=" veidnes";
      result+=" apraksts";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" faila apraksts";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" nosaukumvietas apraksts";
      return result;
    }

    QCString trPublicMembers() override
    { return "Publiskās elementa funkcijas"; }
    QCString trPublicSlots() override
    { return "Publiskās spraugas"; }
    QCString trSignals() override
    { return "Signāli"; }
    QCString trStaticPublicMembers() override
    { return "Statiskās publiskās elementa funkcijas"; }
    QCString trProtectedMembers() override
    { return "Aizsargātās elementa funkcijas"; }
    QCString trProtectedSlots() override
    { return "Aizsargātās spraugas"; }
    QCString trStaticProtectedMembers() override
    { return "Statiskās aizsargātās elementa funkcijas"; }
    QCString trPrivateMembers() override
    { return "Privātās elementa funkcijas"; }
    QCString trPrivateSlots() override
    { return "Privātās spraugas"; }
    QCString trStaticPrivateMembers() override
    { return "Statiskās privātās elementa funkcijas"; }

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
            result+=", un ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Manto no "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Tiek mantots "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Pārimplementēts no "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Pārimplementēts "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Nosaukumvietas elementi"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Šeit ir visi ";
      if (!extractAll) result+="dokumentētie ";
      result+="nosaukumvietas elementi ar saitēm uz ";
      if (extractAll)
        result+="nosaukumvieta dokumentāciju katram elementam:";
      else
        result+="nosaukumvietu, kam tie pieder:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Nosaukumvietu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Nosaukumvietas dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Nosaukumvietas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      QCString result="Šī";
      switch(compType)
      {
        case ClassDef::Class:      result+="s klases"; break;
        case ClassDef::Struct:     result+="s struktūras"; break;
        case ClassDef::Union:      result+=" apvienojuma"; break;
        case ClassDef::Interface:  result+=" interfeisa"; break;
        case ClassDef::Protocol:   result+=" protokola"; break;
        case ClassDef::Category:   result+="s kategorijas"; break;
        case ClassDef::Exception:  result+=" izņēmuma"; break;
        default: break;
      }
      result+=" dokumentācijas tika ģenerēta no šāda fail";
      if (single) result+="a:"; else result+="iem:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Atgriežamās vērtības"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Sākumlapa"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "lpp."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definēts līnijā @0 failā @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definēts failā @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Novecojusi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Sadarbības diagramma klasei "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Include dependency graph for "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Konstruktora un destruktora dokumentācija";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Iet uz šī faila pirmkodu.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ietu uz šī faila dokumentāciju.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Pirmsnosacījums";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Pēcnosacījums";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariants";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Sākotnējā vērtība:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "pirmkods";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafiska klases hierarhija";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Iet uz grafisku klases hierarhiju";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Iet uz tekstuālu klases hierarhiju";
    }
    QCString trPageIndex() override
    {
      return "Lapas indekss";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Piezīme";
    }
    QCString trPublicTypes() override
    {
      return "Publiskie tipi";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu lauki";
      }
      else
      {
        return "Publiskie atribūti";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Statiskie publiskie atribūti";
    }
    QCString trProtectedTypes() override
    {
      return "Aizsargātie tipi";
    }
    QCString trProtectedAttribs() override
    {
      return "Aizsargātie atribūti";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statiskie aizsargātie atribūti";
    }
    QCString trPrivateTypes() override
    {
      return "Privātie tipi";
    }
    QCString trPrivateAttribs() override
    {
      return "Privātie atribūti";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statiskie privātie atribūti";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Jāizdara";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Darāmo darbu saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Atsauces no";
    }
    QCString trRemarks() override
    {
      return "Piebildes";
    }
    QCString trAttention() override
    {
      return "Uzmanību";
    }
    QCString trInclByDepGraph() override
    {
      return "Šis grafs rāda kuri faili tieši vai "
             "netieši iekļauj šo failu:";
    }
    QCString trSince() override
    {
      return "Kopš";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Grafika leģenda";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "This page explains how to interpret the graphs that are generated "
        "by doxygen.<p>\n"
        "Consider the following example:\n"
        "\\code\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class that is inherited using protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class that is inherited using private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Class that is used by the Inherited class */\n"
        "class Used { };\n\n"
        "/*! Super class that inherits a number of other classes */\n"
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
        "This will result in the following graph:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A filled gray box represents the struct or class for which the "
        "graph is generated.</li>\n"
        "<li>%A box with a black border denotes a documented struct or class.</li>\n"
        "<li>%A box with a gray border denotes an undocumented struct or class.</li>\n"
        "<li>%A box with a red border denotes a documented struct or class for"
        "which not all inheritance/containment relations are shown. %A graph is "
        "truncated if it does not fit within the specified boundaries.</li>\n"
        "</ul>\n"
        "<p>\n"
        "The arrows have the following meaning:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.</li>\n"
        "<li>%A dark green arrow is used for protected inheritance.</li>\n"
        "<li>%A dark red arrow is used for private inheritance.</li>\n"
        "<li>%A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible.</li>\n"
        "<li>%A yellow dashed arrow denotes a relation between a template instance and "
        "the template class it was instantiated from. The arrow is labeled with "
        "the template parameters of the instance.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "leģenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Testi";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Testu saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Rekvizīti";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Rekvizītu dokumentācija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūras";
      }
      else
      {
        return "Klases";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Pakotne "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Here are the packages with brief descriptions (if available):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pakotnes";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Vērtība:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Kļūda";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Kļūdu saraksts";
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
      return "1257";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "186";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Indekss";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klase", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fail", "i", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "nosaukumvieta", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupa", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "lapa", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "element", "i", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globāl", "ie", "ais");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "i", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Atsauces";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Īsteno "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Īstenots "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Satura rādītājs";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Novecojušo saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Notikumi";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Notikumu dokumentācija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Pakas tipi";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Pakas funkcijas";
    }
    QCString trPackageMembers() override
    {
      return "Pakas elementi";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statiskās pakas funkcijas";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Pakas atribūti";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statiskie pakas atribūti";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Visi";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Šeit ir visu funkciju izsaugumu grafs:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Meklēšanas rezultāti";
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
        return "Netika atrasts neviens dokuments.";
      }
      else if (numDocuments==1)
      {
        return "Atrasts <b>1</b> dokuments.";
      }
      else
      {
        return "Atrasti <b>$num</b> Dokumenti. "
               "Sākumā attēlo tos, kas atbilst visprecīzāk.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Atbilst meklētajam:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " pirmkoda fails";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Direktoriju struktūra"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Direktorijas dokumentācija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Direktorijas"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Direktorijas "; result+=dirName; result+=" atsauce"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "direktorija", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Šī ir elementa pārslogota funkcija, kas "
              "parādīta tikai informātīvo nolūkos. Tā atšķiras no iepriekšapraksītās "
              "funkcijas tikai ar parametriem, ko tā saņem.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Šeit ir šīs funkcijas izsaukuma grafs:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Uzskaitījumliterāļa dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Elementa funckijas/apakšrutīnas dokumentācija"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Datu tipu saraksts"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Datu lauki"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Šeit ir visu datu tipu saraksts ar īsu aprakstu:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll)
      {
        result+="dokumentēto ";
      }
      result+="datu tipu saraksts";
      result+=" ar saitēm uz ";
      if (!extractAll)
      {
         result+="datu struktūras dokumentāciju katram elementam:";
      }
      else
      {
         result+="datu tipiem, kam tie pieder:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Datu tipu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Datu tipa dokumentācija"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funkcijas/Apakšrutīnas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Funkcijas/Apakšrutīnas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Datu tipi"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Moduļu saraksts"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";
      result+="moduļu saraksts ar īsu aprakstu:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" moduļa"; break;
        case ClassDef::Struct:     result+=" tipa"; break;
        case ClassDef::Union:      result+=" apvienojuma"; break;
        case ClassDef::Interface:  result+=" interfeisa"; break;
        case ClassDef::Protocol:   result+=" protokola"; break;
        case ClassDef::Category:   result+=" kategorijas"; break;
        case ClassDef::Exception:  result+=" izņēmuma"; break;
        default: break;
      }
      if (isTemplate) result+=" sagataves";
      result+=" atsauce";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" moduļu atsauce";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Moduļa elementi"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";
      result+="moduļa elementu saraksts ar saitēm uz ";
      if (extractAll)
      {
        result+="moduļa dokumentāciju katram elementam:";
      }
      else
      {
        result+="moduļiem, kuriem tie pieder:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Moduļu indekss"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modu", "ļi", "lis");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Dokumentācija š";
      switch(compType)
      {
        case ClassDef::Class:      result+="im modulim"; break;
        case ClassDef::Struct:     result+="im tipam"; break;
        case ClassDef::Union:      result+="im apvienojumam"; break;
        case ClassDef::Interface:  result+="im interfeisam"; break;
        case ClassDef::Protocol:   result+="im protokolam"; break;
        case ClassDef::Category:   result+="ai kategorijai"; break;
        case ClassDef::Exception:  result+="im izņēmumam"; break;
        default: break;
      }
      result+=" tika ģenerēta no fail";
      if (single) result+="a:"; else result+="iem:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "i", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "apakšprogramma", "s");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Tipa ierobežojumi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" relācija";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Tiek veikta ielāde...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Global Namespace";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Meklē...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Nav atbilstību";
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
      return "Includes file in "+name;
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
      static const char *days[]   = { "Pirm","Otr","Tr","Cet","Piekt","Sest","Sv" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Mai","Jūn","Jūl","Aug","Sept","Okt","Nov","Dec" };
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
      static const char *days_short[]   = { "pirmd.", "otrd.", "trešd.", "ceturtd.", "piektd.", "sestd.", "svētd." };
      static const char *days_full[]    = { "pirmdiena", "otrdiena", "trešdiena", "ceturtdiena", "piektdiena", "sestdiena", "svētdiena" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "janv.", "febr.", "marts", "apr.", "maijs", "jūn.", "jūl.", "aug.", "sept.", "okt.", "nov.", "dec." };
      static const char *months_full[]  = { "janvāris", "februāris", "marts", "aprīlis", "maijs", "jūnijs", "jūlijs", "augusts", "septembris", "oktobris", "novembris", "decembris" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "priekšp.", "pēcp." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Bibliogrāfiskās atsauces"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Autortiesības"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Atkarību grafs direktorijai ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "detalizācijas līmenis"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Sagataves parametri"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "un vēl "+number+"..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumentācija šim uzskaitījumliterālim tika ģenerēta no sekojoš";
      if (single) result+="a"; else result += "iem";
      result+=" fail";
      if (single) result+="a"; else result += "iem";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" uzskaitījumliterāļa atsauce"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" manto no "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Papildus mantotie elementi"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "iespējotu" : "atspējotu";
      return "uzklikšķināt, lai "+opt+" paneļu sinhronizāciju";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Nodrošina kategorija @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Paplašina klasi @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Klases metodes";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Instances metodes";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Metožu dokumentācija";
    }

/*
 * Latvian translations for version 1.8.4
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Eksportētās saskarnes"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Iekļautie pakalpojumi"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Konstantu grupas"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Atsauce uz konstantu grupu";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Atsauce uz pakalpojumu";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Atsauce uz Singletona";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Šī pakalpojuma dokumentācija tika ģenerēta no sekojošā fail";
      if (single) result+="a:"; else result+="iem:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Šī Singletona dokumentācija tika ģenerēta no sekojošā fail";
      if (single) result+="a:"; else result+="iem:";
      return result;
    }

/*
 * Latvian translations for version 1.8.15
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Projektēšanas vienību hierarhija"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Projektēšanas vienību saraksts"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Projektēšanas vienības locekļi"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
      return "Šeit ir visu projektēšanas vienību locekļu saraksts ar saitēm uz "
             "tām vienībām, kurām tie pieder:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Projektēšanas vienību rādītājs"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Projektēšanas vienības"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funkcijas/Procedūras/Procesi"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
      case VhdlSpecifier::LIBRARY:
        if (single) return "Bibliotēka";
        else        return "Bibliotēkas";
      case VhdlSpecifier::PACKAGE:
        if (single) return "Pakotne";
        else        return "Pakotnes";
      case VhdlSpecifier::SIGNAL:
        if (single) return "Signāls";
        else        return "Signāli";
      case VhdlSpecifier::COMPONENT:
        if (single) return "Komponents";
        else        return "Komponenti";
      case VhdlSpecifier::CONSTANT:
        if (single) return "Konstante";
        else        return "Konstantes";
      case VhdlSpecifier::ENTITY:
        if (single) return "Vienība";
        else        return "Vienības";
      case VhdlSpecifier::TYPE:
        if (single) return "Tips";
        else        return "Tipi";
      case VhdlSpecifier::SUBTYPE:
        if (single) return "Apakštips";
        else        return "Apakštipi";
      case VhdlSpecifier::FUNCTION:
        if (single) return "Funkcija";
        else        return "Funkcijas";
      case VhdlSpecifier::RECORD:
        if (single) return "Ieraksts";
        else        return "Ieraksti";
      case VhdlSpecifier::PROCEDURE:
        if (single) return "Procedūra";
        else        return "Procedūras";
      case VhdlSpecifier::ARCHITECTURE:
        if (single) return "Arhitektūra";
        else        return "Arhitektūras";
      case VhdlSpecifier::ATTRIBUTE:
        if (single) return "Atribūts";
        else        return "Atribūti";
      case VhdlSpecifier::PROCESS:
        if (single) return "Process";
        else        return "Procesi";
      case VhdlSpecifier::PORT:
        if (single) return "Ports";
        else        return "Porti";
      case VhdlSpecifier::USE:
        if (single) return "Lietošanas nosacījums";
        else        return "Lietošanas nosacījumi";
      case VhdlSpecifier::GENERIC:
        if (single) return "Ģenerisks";
        else        return "Ģeneriski";
      case VhdlSpecifier::PACKAGE_BODY:
        return "Pakotnes ķermenis";
      case VhdlSpecifier::UNITS:
        return "Vienības";
      case VhdlSpecifier::SHAREDVARIABLE:
        if (single) return "Koplietojams mainīgais";
        else        return "Koplietojamie mainīgie";
      case VhdlSpecifier::VFILE:
        if (single) return "Fails";
        else        return "Faili";
      case VhdlSpecifier::GROUP:
        if (single) return "Grupa";
        else        return "Grupas";
      case VhdlSpecifier::INSTANTIATION:
        if (single) return "Instancēšana";
        else        return "Instancēšanas";
      case VhdlSpecifier::ALIAS:
        if (single) return "Aizstājvārds";
        else        return "Aizstājvārdi";
      case VhdlSpecifier::CONFIG:
        if (single) return "Konfigurācija";
        else        return "Konfigurācijas";
      case VhdlSpecifier::MISCELLANEOUS:
        return "Dažādi";
      case VhdlSpecifier::UCF_CONST:
        return "Ierobežojumi";
      default:
        return "Klase";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return name+" Atsauce"; }

    /* Slice */
    QCString trConstants() override
    {
      return "Konstantes";
    }
    QCString trConstantDocumentation() override
    {
      return "Konstantes dokumentācija";
    }
    QCString trSequences() override
    {
      return "Secības";
    }
    QCString trSequenceDocumentation() override
    {
      return "Secības dokumentācija";
    }
    QCString trDictionaries() override
    {
      return "Vārdnīcas";
    }
    QCString trDictionaryDocumentation() override
    {
      return "Vārdnīcas dokumentācija";
    }
    QCString trSliceInterfaces() override
    {
      return "Saskarnes";
    }
    QCString trInterfaceIndex() override
    {
      return "Saskarņu rādītājs";
    }
    QCString trInterfaceList() override
    {
      return "Saskarņu saraksts";
    }
    QCString trInterfaceListDescription() override
    {
      return "Šeit ir saskarnes ar īsiem aprakstiem:";
    }
    QCString trInterfaceHierarchy() override
    {
      return "Saskarņu hierarhija";
    }
    QCString trInterfaceHierarchyDescription() override
    {
      return "Šis pārmantošanas saraksts tiek kārtots aptuveni, bet ne pilnībā, alfabētiskā secībā:";
    }
    QCString trInterfaceDocumentation() override
    {
      return "Saskarnes dokumentācija";
    }
    QCString trStructs() override
    {
      return "Struktūras";
    }
    QCString trStructIndex() override
    {
      return "Struktūru rādītājs";
    }
    QCString trStructList() override
    {
      return "Struktūru saraksts";
    }
    QCString trStructListDescription() override
    {
      return "Šeit ir struktūras ar īsiem aprakstiem:";
    }
    QCString trStructDocumentation() override
    {
      return "Struktūras dokumentācija";
    }
    QCString trExceptionIndex() override
    {
      return "Izņēmumu rādītājs";
    }
    QCString trExceptionList() override
    {
      return "Izņēmumu saraksts";
    }
    QCString trExceptionListDescription() override
    {
      return "Šeit ir izņēmumi ar īsiem aprakstiem:";
    }
    QCString trExceptionHierarchy() override
    {
      return "Izņēmumu hierarhija";
    }
    QCString trExceptionHierarchyDescription() override
    {
      return "Šis pārmantošanas saraksts tiek kārtots aptuveni, bet ne pilnībā, alfabētiskā secībā:";
    }
    QCString trExceptionDocumentation() override
    {
      return "Izņēmumu dokumentācija";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result="Atsauce uz";
      if (isLocal) result+=" lokālo";
      switch(compType)
      {
      case ClassDef::Class:      result+=" klasi"; break;
      case ClassDef::Struct:     result+=" struktūru"; break;
      case ClassDef::Union:      result+=" savienojumu"; break;
      case ClassDef::Interface:  result+=" saskarni"; break;
      case ClassDef::Protocol:   result+=" protokolu"; break;
      case ClassDef::Category:   result+=" kategoriju"; break;
      case ClassDef::Exception:  result+=" izņēmumu"; break;
      default: break;
      }
      return result + clName;
    }
    QCString trOperations() override
    {
      return "Operācijas";
    }
    QCString trOperationDocumentation() override
    {
      return "Operācijas dokumentācija";
    }
    QCString trDataMembers() override
    {
      return "Klases lauki";
    }
    QCString trDataMemberDocumentation() override
    {
      return "Klases lauku dokumentācija";
    }

/*
 * Latvian translations for version 1.8.19
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Projektēšanas vienības dokumentācija"; }

/*
 * Latvian translations for version 1.9.2
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "koncept", "i", "s");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      return "Atsauce uz konceptu " + conceptName;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Konceptu saraksts"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Konceptu rādītājs"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Konceptu dokumentācija"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Šeit ir saraksts ar visiem ";
      if (!extractAll) result+="dokumentētiem ";
      result+="konceptiem ar īsiem aprakstiem:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Koncepta definīcija";
    }

/*
 * Latvian translations for version 1.9.4
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Pakotņu saraksts"; }

/*
 * Latvian translations for version 1.9.6
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
    { return "Blokshēma:"; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     */

/*
 * Latvian translations for version 1.9.7
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    QCString trRelatedSymbols() override
    { return "Saistītie simboli"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(Jāņem vērā, ka tie nav locekļu simboli.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Klases draugi un ar klasi saistīto simbolu dokumentācija"; }

    /*! the compound type as used for the xrefitems */
    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
      case ClassDef::Class:
        if (lang == SrcLangExt::Fortran) trType(true,true);
        else result=trClass(true,true);
        break;
      case ClassDef::Struct:     result="Struktūra"; break;
      case ClassDef::Union:      result="Savienojums"; break;
      case ClassDef::Interface:  result="Saskarne"; break;
      case ClassDef::Protocol:   result="Protokols"; break;
      case ClassDef::Category:   result="Categorija"; break;
      case ClassDef::Exception:  result="Izņēmums"; break;
      case ClassDef::Service:    result="Serviss"; break;
      case ClassDef::Singleton:  result="Singletons"; break;
      default: break;
      }
      return result;
    }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentētu ";

      switch (hl)
      {
      case FileMemberHighlight::All:
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="funkciju, mainīgo, definīciju, enumerāciju un tipdefu";
        }
        else
        {
          result+="faila locekļu";
        }
        break;
      case FileMemberHighlight::Functions:
        result+="funkciju";
        break;
      case FileMemberHighlight::Variables:
        result+="mainīgo";
        break;
      case FileMemberHighlight::Typedefs:
        result+="tipdefu";
        break;
      case FileMemberHighlight::Sequences:
        result+="sekvences";
        break;
      case FileMemberHighlight::Dictionaries:
        result+="vārdnīcu";
        break;
      case FileMemberHighlight::Enums:
        result+="enumerāciju";
        break;
      case FileMemberHighlight::EnumValues:
        result+="enumerācijas vērtību";
        break;
      case FileMemberHighlight::Defines:
        result+="makro komandu";
        break;
      case FileMemberHighlight::Total: // for completeness
        break;
      }
      result+=" saraksts ar saitēm uz ";
      if (extractAll)
        result+="failiem, kam tie pieder:";
      else
        result+="dokumentāciju:";
      return result;
    }
    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Šeit ir visu ";
      if (!extractAll)
      {
        result+="dokumentētu ";
      }

      switch (hl)
      {
      case ClassMemberHighlight::All:
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūru un savienojumu lauku";
        }
        else
        {
          result+="klases locekļu";
        }
        break;
      case ClassMemberHighlight::Functions:
        result+="funkciju";
        break;
      case ClassMemberHighlight::Variables:
        result+="mainīgo";
        break;
      case ClassMemberHighlight::Typedefs:
        result+="tipdefu";
        break;
      case ClassMemberHighlight::Enums:
        result+="enumerāciju";
        break;
      case ClassMemberHighlight::EnumValues:
        result+="enumerācijas vērtību";
        break;
      case ClassMemberHighlight::Properties:
        result+="īpašību";
        break;
      case ClassMemberHighlight::Events:
        result+="notikumu";
        break;
      case ClassMemberHighlight::Related:
        result+="saistīto simbolu";
        break;
      case ClassMemberHighlight::Total: // for completeness
        break;
      }
      result+=" saraksts ar saitēm uz ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="katra lauka struktūru/savienojumu dokumentāciju:";
        }
        else
        {
          result+="katra locekļa klases dokumentāciju:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struktūrām/savienojumiem, kam tie pieder:";
        }
        else
        {
          result+="klasēm, kam tās pieder:";
        }
      }
      return result;
    }
    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="documentētu ";
      QCString singularResult = "";
      QCString pluralResult = "";
      switch (hl)
      {
      case NamespaceMemberHighlight::All:
        singularResult="locekļa";
        pluralResult="locekļu";
        break;
      case NamespaceMemberHighlight::Functions:
        singularResult="funkcijas";
        pluralResult="funkciju";
        break;
      case NamespaceMemberHighlight::Variables:
        singularResult="mainīga";
        pluralResult="mainīgu";
        break;
      case NamespaceMemberHighlight::Typedefs:
        singularResult="tipdefa";
        pluralResult="tipdefu";
        break;
      case NamespaceMemberHighlight::Sequences:
        singularResult="sekvences";
        pluralResult="sekvenču";
        break;
      case NamespaceMemberHighlight::Dictionaries:
        singularResult="vārdnīcas";
        pluralResult="vārdnīcu";
        break;
      case NamespaceMemberHighlight::Enums:
        singularResult="enumerācijas";
        pluralResult="enumerāciju";
        break;
      case NamespaceMemberHighlight::EnumValues:
        singularResult="enumerācijas vērtības";
        pluralResult="enumerācijas vērtību";
        break;
      case NamespaceMemberHighlight::Total: // for completeness
        break;
      }
      result+=" ar saitēm uz ";
      if (extractAll)
        result+="nosaukumvietas dokumentāciju katram " + singularResult + ":";
      else
        result+="nosaukumvietu, kam tie pieder:";
      return result;
    }
    QCString trDefinition() override  { return "Definīcija";}
    QCString trDeclaration() override { return "Deklarācija";}

/*
 * Latvian translations for version 1.9.8
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    QCString trTopics() override
    { return "Tēmas"; }
    QCString trTopicDocumentation() override
    { return "Tēmu dokumentācija"; }
    QCString trTopicList() override
    { return "Tēmu saraksts"; }
    QCString trTopicIndex() override
    { return "Tēmu rādītājs"; }
    QCString trTopicListDescription() override
    { return "Šeit ir visu tēmu saraksts ar īsiem aprakstiem:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentētu ";
      result+="moduļa ";
      QCString singularResult = "";
      QCString pluralResult = "";
      switch (hl)
      {
      case ModuleMemberHighlight::All:
        singularResult="locekļa";
        pluralResult="locekļu";
        break;
      case ModuleMemberHighlight::Functions:
        singularResult="funkcijas";
        pluralResult="funkciju";
        break;
      case ModuleMemberHighlight::Variables:
        singularResult="mainīga";
        pluralResult="mainīgu";
        break;
      case ModuleMemberHighlight::Typedefs:
        singularResult="tipdefs";
        pluralResult="tipdefu";
        break;
      case ModuleMemberHighlight::Enums:
        singularResult="enumerācijas";
        pluralResult="enumerāciju";
        break;
      case ModuleMemberHighlight::EnumValues:
        singularResult="enumerācijas vērtības";
        pluralResult="enumerācijas vērtību";
        break;
      case ModuleMemberHighlight::Total: // for completeness
        break;
      }
      result+=(pluralResult.isEmpty() ? singularResult+"s" : pluralResult);
      result+=" ar saitēm uz ";
      if (extractAll)
        result+="moduļa dokumentāciju katram " + singularResult + ":";
      else
        result+="moduli, kam tie pieder:";
      return result;
    }
    QCString trExportedModules() override
    {
      return "Eksportētie moduļi";
    }

/*
 * Latvian translations for version 1.10.0
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "Kopēt uz starpliktuvi";
    }

/*
 * Latvian translations for version 1.11.0
 *
 * Artyom Fedosov, 2025
 * Github: artyom-fedosov
 */
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    QCString trImportant() override
    {
      return "Svarīgi";
    }
};

#endif
