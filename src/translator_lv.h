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

/* Translation from English to Latvian by Lauris Bukšis-Haberkorns
 *  (lauris@nix.lv)
 *  -------------------------------------------
 *  Project start                  : 24.Sept.2012
 *  Last Doxygen version covered   : 1.8.2
 */

class TranslatorLatvian : public TranslatorAdapter_1_8_4
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[LV]{fontenc}\n"
             "\\usepackage[latvian]{babel}\n";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Saistītās funkcijas"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Šīs nav elementu funkcijas.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Detalizēts apraksts"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Elementa Typedef dokumentācija"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Elementa uzskaitījumliterāļa dokumentācija"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Elementa funkcijas dokumentācija"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
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
    virtual QCString trMore()
    { return "vairāk..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Visu elementu saraksts"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Elementu saraksts"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Šis ir pilns elementu saraksts klasei "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", ieskaitot mantotos elementus."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automātiski ģenerēts izmantojot Doxygen";
      if (s) result+=(QCString)" priekš "+s;
      result+=" no pirmkoda.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "uzskaitījuma nosaukums"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "uzskaitījuma vērtība"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definēts"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduļi"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Klašu hierarhija"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
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
    virtual QCString trFileList()
    { return "Failu saraksts"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
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
    virtual QCString trFileMembers()
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
    virtual QCString trRelatedPages()
    { return "Saistītās lapas"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Piemēri"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Meklēt"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Šis mantojamības saraksts ir sakārtots aptuveni, "
             "bet ne pilnībā, alfabēta secībā:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";
      result+="failu saraksts ar īsu aprakstu:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Šeit ir visas datu struktūras ar īsu aprakstu:";
      }
      else
      {
        return "Šeit ir visas klases, struktūras, "
               "apvienojumi un interfeisi ar īsu aprakstu:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
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
    virtual QCString trFileMembersDescription(bool extractAll)
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
    virtual QCString trExamplesDescription()
    { return "Šeit ir visu piemēru saraksts:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Here is a list of all related documentation pages:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Šeit ir visu moduļu saraksts:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentācija"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Moduļu indekss"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarhijas indekss"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
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
    virtual QCString trFileIndex()
    { return "Failu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Moduļa dokumentācija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Datu struktūras dokomentācija";
      }
      else
      {
        return "Klases dokumentācija";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Faila dokumentācija"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Piemēri"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Lapas dokumentācija"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Rokasgrāmata"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makro"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Uzskaitījumi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funkcijas"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Mainīgie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Uzskaitījumliterāļi"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Makro definīcijas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Typedef dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Uzskaitījumliterāļa tipa dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funkcijas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Mainīgo dokumentācija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Ģenerēts "+date;
      if (projName) result+=(QCString)" projektam "+projName;
      result+=(QCString)" ar";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Mantojamības diagramma klasei "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Tikai iekšējai lietošanai."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Brīdinājums"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versija"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Datums"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Atgriež"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Skatīties arī"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Izņēmumi"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Ģenerēts ar"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Nosaukumvietu saraksts"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Šeit ir visas ";
      if (!extractAll) result+="dokumentētās ";
      result+="nosaukumvietas ar īsu aprakstu:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Draugi"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Draugu un saistīto funkciju dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
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
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" faila apraksts";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" nosaukumvietas apraksts";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Publiskās elementa funkcijas"; }
    virtual QCString trPublicSlots()
    { return "Publiskās spraugas"; }
    virtual QCString trSignals()
    { return "Signāli"; }
    virtual QCString trStaticPublicMembers()
    { return "Statiskās publiskās elementa funkcijas"; }
    virtual QCString trProtectedMembers()
    { return "Aizsargātās elementa funkcijas"; }
    virtual QCString trProtectedSlots()
    { return "Aizsargātās spraugas"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statiskās aizsargātās elementa funkcijas"; }
    virtual QCString trPrivateMembers()
    { return "Privātās elementa funkcijas"; }
    virtual QCString trPrivateSlots()
    { return "Privātās spraugas"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statiskās privātās elementa funkcijas"; }

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
            result+=", un ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Manto no "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Tiek mantots "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Pārimplementēts no "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Pārimplementēts "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Nosaukumvietas elementi"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Nosaukumvietu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Nosaukumvietas dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Nosaukumvietas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // single is true implies a single file
      QCString result=(QCString)"Šī";
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
    virtual QCString trReturnValues()
    { return "Atgriežamās vērtības"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Sākumlapa"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "lpp."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definēts līnijā @0 failā @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definēts failā @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Novecojusi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Sadarbības diagramma klasei "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include dependency graph for "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Konstruktora un destruktora dokumentācija";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Iet uz šī faila pirmkodu.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Ietu uz šī faila dokumentāciju.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Pirmsnosacījums";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Pēcnosacījums";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariants";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Sākotnējā vērtība:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "pirmkods";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafiska klases hierarhija";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Iet uz grafisku klases hierarhiju";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Iet uz tekstuālu klases hierarhiju";
    }
    virtual QCString trPageIndex()
    {
      return "Lapas indekss";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Piezīme";
    }
    virtual QCString trPublicTypes()
    {
      return "Publiskie tipi";
    }
    virtual QCString trPublicAttribs()
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
    virtual QCString trStaticPublicAttribs()
    {
      return "Statiskie publiskie atribūti";
    }
    virtual QCString trProtectedTypes()
    {
      return "Aizsargātie tipi";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Aizsargātie atribūti";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statiskie aizsargātie atribūti";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privātie tipi";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privātie atribūti";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statiskie privātie atribūti";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Jāizdara";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Darāmo darbu saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Atsauces no";
    }
    virtual QCString trRemarks()
    {
      return "Piebildes";
    }
    virtual QCString trAttention()
    {
      return "Uzmanību";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Šis grafs rāda kuri faili tieši vai "
             "netieši iekļauj šo failu:";
    }
    virtual QCString trSince()
    {
      return "Kopš";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Grafika leģenda";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
    virtual QCString trLegend()
    {
      return "leģenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Testi";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Testu saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Rekvizīti";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Rekvizītu dokumentācija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
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
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Pakotne "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Pakotņu saraksts";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Here are the packages with brief descriptions (if available):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pakotnes";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Vērtība:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Kļūda";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1257";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "186";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Indekss";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Klase" : "klase"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Fail" : "fail"));
      if (singular) result+="s"; else result+="i";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Nosaukumvieta" : "nosaukumvieta"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Grupa" : "grupa"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Lapa" : "lapa"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Element" : "element"));
      if (singular) result+="s"; else result+="i";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Globāl" : "globāl"));
      if (singular) result+="ais"; else result+="ie";
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
      if (singular) result+="s"; else result+="i";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Atsauces";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Īsteno "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Īstenots "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Satura rādītājs";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Novecojušo saraksts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Notikumi";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Notikumu dokumentācija";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Pakas tipi";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Pakas funkcijas";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statiskās pakas funkcijas";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Pakas atribūti";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statiskie pakas atribūti";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Visi";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Šeit ir visu funkciju izsaugumu grafs:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Atbilst meklētajam:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " pirmkoda fails";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Direktoriju struktūra"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Direktorijas dokumentācija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Direktorijas"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Šī direktoriju hierarhija ir aptuveni, "
             "bet ne pilnībā, alfabēta secībā:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Direktorijas "; result+=dirName; result+=" atsauce"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Direktorija" : "direktorija"));
      if (!singular) result+="s";
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
       return "Šī ir elementa pārslogota funkcija, kas "
              "parādīta tikai informātīvo nolūkos. Tā atšķiras no iepriekšapraksītās "
              "funkcijas tikai ar parametriem, ko tā saņem.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Šeit ir šīs funkcijas izsaukuma grafs:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Uzskaitījumliterāļa dokumentācija"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Elementa funckijas/apakšrutīnas dokumentācija"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Datu tipu saraksts"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Datu lauki"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Šeit ir visu datu tipu saraksts ar īsu aprakstu:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Datu tipu indekss"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Datu tipa dokumentācija"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funkcijas/Apakšrutīnas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Funkcijas/Apakšrutīnas dokumentācija"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Datu tipi"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Moduļu saraksts"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Šeit ir visu ";
      if (!extractAll) result+="dokumentēto ";
      result+="moduļu saraksts ar īsu aprakstu:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
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
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" moduļu atsauce";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Moduļa elementi"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Moduļu indekss"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modu" : "modu"));
      if (singular) result+="lis"; else result+="ļi";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"Dokumentācija š";
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
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tip" : "tip"));
      if (singular) result+="s"; else result+="i";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Apakšprogramma" : "apakšprogramma"));
      if (!singular)  result+="s";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Tipa ierobežojumi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" relācija";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Tiek veikta ielāde...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Global Namespace";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Meklē...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
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
      return (QCString)"Includes file in "+name;
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
      static const char *days[]   = { "Pirm","Otr","Tr","Cet","Piekt","Sest","Sv" };
      static const char *months[] = { "Jan","Feb","Mar","Apr","Mai","Jūn","Jūl","Aug","Sept","Okt","Nov","Dec" };
      QCString sdate;
      sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
    { return "Bibliogrāfiskās atsauces"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Autortiesības"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Atkarību grafs direktorijai ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "detalizācijas līmenis"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Sagataves parametri"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "un vēl "+number+"..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Dokumentācija šim uzskaitījumliterālim tika ģenerēta no sekojoš";
      if (single) result+="a"; else result += "iem";
      result+=" fail";
      if (single) result+="a"; else result += "iem";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" uzskaitījumliterāļa atsauce"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" manto no "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Papildus mantotie elementi"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "iespējotu" : "atspējotu";
      return "uzklikšķināt, lai "+opt+" paneļu sinhronizāciju";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Nodrošina kategorija @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Paplašina klasi @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Klases metodes";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Instances metodes";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Metožu dokumentācija";
    }

    /*! Used as the title of the design overview picture created for the
     *  VHDL output.
     */
    virtual QCString trDesignOverview()
    {
      return "Dizaina pārskats";
    }

//////////////////////////////////////////////////////////////////////////

};

#endif
