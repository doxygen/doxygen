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

 /*
 *  Bronne vir hierdie vertaling (Sources for this translation):
 *  Die Stigting vir Afrikaans se rekenaartermelys:
 *      - http://www.afrikaans.com/rekenaarterme.html
 *  Werkgroep vir Afrikaanse IT-terme:
 *      - http://www.vertaal.org/index.html
 */

#ifndef TRANSLATOR_ZA_H
#define TRANSLATOR_ZA_H

class TranslatorAfrikaans : public TranslatorAdapter_1_6_0
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage() override
    { return "afrikaans"; }

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
     * The Afrikaans LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    virtual QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[afrikaans]{babel}\n";
    }
    virtual QCString trISOLang() override
    {
      return "af";
    }
    virtual QCString getLanguageString() override
    {
      return "0x436 Afrikaans";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Verwante Funksies"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Let daarop dat hierdie nie lede funksies is nie.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Detail Beskrywing"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Besonderhede"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Lede Typedef Dokumentasie"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Lede Enumerasie Dokumentasie"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Lede Funksie Dokumentasie"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veld Dokumentasie";
      }
      else
      {
        return "Lede Data Dokumentasie";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Meer detail..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Lys van alle lede"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Lede Lys"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Hierdie is 'n volledige lys van lede vir "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return ", insluitend alle afgeleide lede."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automaties gegenereer deur Doxygen";
      if (!s.isEmpty()) result+=" vir "+s;
      result+=" van die bron kode af.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "enum naam"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "enum waarde"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "gedefinieër in"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Modules"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Klas Hierargie"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Strukture";
      }
      else
      {
        return "Klas Lys";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Leër Lys"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Velde";
      }
      else
      {
        return "Klas Lede";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globals";
      }
      else
      {
        return "Leër Lede";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Verwante Bladsye"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Voorbeelde"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Soek"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    {
      return "Hierdie afgeleide lys word rofweg gesorteer: ";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Hier is 'n lys van alle  ";
      if (!extractAll) result+="gedokumenteerde ";
      result+="leërs met kort beskrywings:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return " Data strukture met kort beskrywings:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Klasse met kort beskrywings:";
      }
      else
      {
        return "Klasse, structs, "
               "unions en intervlakke met kort beskrywings:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="'n Lys van alle ";
      if (!extractAll)
      {
        result+="gedokumenteerde ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struct en union velde";
      }
      else
      {
        result+="klas lede";
      }
      result+=" met skakels na ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="die struct/union dokumentasie vir elke veld:";
        }
        else
        {
          result+="die klas dokumentasie vir elke lid:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="die structures/unions waaraan hulle behoort:";
        }
        else
        {
          result+="die klasse waaraan hulle behoort:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="'n Lys van alle ";
      if (!extractAll) result+="gedokumenteerde ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funksies, veranderlikes, defines, enums, en typedefs";
      }
      else
      {
        result+="leër lede";
      }
      result+=" met skakels na ";
      if (extractAll)
        result+="die leërs waaraan hulle behoort:";
      else
        result+="die dokumentasie:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() override
    { return "'n Lys van alle voorbeelde:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "'n Lys van alle verwante dokumentasie:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "'n Lys van alle modules:"; }

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Dokumentasie"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Module Indeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Hierargiese Indeks"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Strukture Indeks";
      }
      else
      {
        return "Klas Indeks";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "Leër Indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Module Dokumentasie"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Strukture Dokumentasie";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klas Dokumentasie";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Leër Dokumentasie"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Verwysings Handleiding"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Definiesies"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Enumerations"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Funksies"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Veranderlikes"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Enumeration waardes"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Typedef Dokumentasie"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Enumeration Type Dokumentasie"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Funksie Dokumentasie"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Veranderlike Dokumentasie"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Strukture";
      }
      else
      {
        return "Klasse";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Gegenereer op "+date;
      if (!projName.isEmpty()) result+=" vir "+projName;
      result+=" deur";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return "Afleidings diagram vir "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Waarskuwing"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Weergawe"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Datum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Returns"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Sien ook"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Parameters"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Exceptions"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Gegenereer deur"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Namespace Lys"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="'n Lys van alle ";
      if (!extractAll) result+="gedokumenteerde ";
      result+="namespaces met kort beskrywings:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Friends En Verwante Funksie Dokumentasie"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" klas"; break;
        case ClassDef::Struct:     result+=" Struct"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Intervlak"; break;
        case ClassDef::Protocol:   result+=" Protocol"; break;
        case ClassDef::Category:   result+=" Kategorie"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+=" Verwysing";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" Leër Verwysing";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Namespace Verwysing";
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Publieke Lede Funksies"; }
    virtual QCString trPublicSlots() override
    { return "Publieke Slots"; }
    virtual QCString trSignals() override
    { return "Signals"; }
    virtual QCString trStaticPublicMembers() override
    { return "Statiese Publieke Lede Funksies"; }
    virtual QCString trProtectedMembers() override
    { return "Beskermde Lede Funksies"; }
    virtual QCString trProtectedSlots() override
    { return "Beskermde Slots"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Statiese Beskermde Lede Funksies"; }
    virtual QCString trPrivateMembers() override
    { return "Private Lede Funksies"; }
    virtual QCString trPrivateSlots() override
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Statiese Private Lede Funksies"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries) override
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
            result+=", en ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Afgelei van"+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Afgelei van"+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return "Hergeimplimenteer van "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Hergeimplimenter in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Namespace Lede"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="'n Lys van alle ";
      if (!extractAll) result+="gedokumenteerde ";
      result+="namespace lede met skakels na ";
      if (extractAll)
        result+="die namespace dokumentasie vir elke lid:";
      else
        result+="die namespaces waaraan hulle behoort:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Namespace Indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Namespace Dokumentasie"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Die dokumentasie vir hierdie ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klas"; break;
        case ClassDef::Struct:     result+="struct"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="intervlak"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategorie"; break;
        case ClassDef::Exception:  result+="eksepsie"; break;
        default: break;
      }
      result+=" is gegenereer vanaf die volgende leër";
      if (single) result+=":"; else result+="s:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Return waardes"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Hoof Bladsy"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Gedefinieër by lyn @0 van leër @1.";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Definisie in leër @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Verouderd";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Samewerkings diagram vir "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Insluitings afhanklikheid diagram vir "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Konstruktor & Destruktor Dokumentasie";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Skakel na die bron kode van hierdie leër.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Skakel na die dokumentasie van hierdie leër.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Prekondisie";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Postkondisie";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Oorspronklike waarde:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "kode";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Grafiese Klasse Hierargie";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Skakel na die grafiese klasse hierargie";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Skakel na die teks klasse hierargie";
    }
    virtual QCString trPageIndex() override
    {
      return "Bladsy Indeks";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Nota";
    }
    virtual QCString trPublicTypes() override
    {
      return "Publieke Tipes";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Velde";
      }
      else
      {
        return "Publieke Public Attributes";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Statiese Publieke Attribute";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Beskermde Tipes";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Beskermde Attribute";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Statiese Beskermde Attribute";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Private Tipes";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Private Attribute";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Statiese Private Attribute";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() override
    {
      return "Aksies";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Aksie Lys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Verwysing van";
    }
    virtual QCString trRemarks() override
    {
      return "Opmerkings";
    }
    virtual QCString trAttention() override
    {
      return "Aandag";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Hierdie diagram verduidelik watter leërs direk of"
             "indirek hierdie leër insluit:";
    }
    virtual QCString trSince() override
    {
      return "Sederd";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Diagram beskrywing";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
    {
      return
        "Hierdie bladsy beskryf die diagram konvensies wat gebruik word "
        "deur doxygen.<p>\n"
        "in hierdie voorbeeld:\n"
        "\\code\n"
        "/*! Onsigbare klas weens afkorting */\n"
        "class Invisible { };\n\n"
        "/*! Afgekorte klas, afgeleide verwantskap word versteek */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Ongedokumenteerde Klas, geen doxygen kommentaar nie */\n"
        "class Undocumented{ };\n\n"
        "/*! 'n Klas wat afgelei is met 'n publieke verwantskap */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! 'n template klas */\n"
        "template<class T> class Templ { };\n\n"
        "/*! 'n Klas wat afgelei is met 'n beskermde verwantskap */\n"
        "class ProtectedBase { };\n\n"
        "/*! 'n Klas wat afgelei is met 'n private verwantskap */\n"
        "class PrivateBase { };\n\n"
        "/*! 'n Klas wat gebrui word deur die Afgeleide klas */\n"
        "class GebruikMy { };\n\n"
        "/*! 'n Super klas wat afgelei word van 'n aantal basis klasse */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Ongedokumenteer,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "As die \\c MAX_DOT_GRAPH_HEIGHT merker in die konfigurasie leër "
        "aan 240 gelyk gestel is, word die volgende diagram geproduseer:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Die reghoeke in die diagram het die volgende betekenis:\n"
        "<ul>\n"
        "<li>%'n Soliede swart reghoek verteenwoordig die klas waarvoor "
        "die diagram gegenereer is.\n"
        "<li>%'n Reghoek met 'n swart omlyning verteenwoordig 'n gedokumenteerde klas.\n"
        "<li>%'n Reghoek met 'n grys omlyning verteenwoordig 'n ongedokumenteerde klas.\n"
        "<li>%'n Reghoek met 'n rooi omlyning verteenwoordig 'n gedokumenteerde klas waarvoor"
        "alle verwante klasse (afgeleide of gebruik) nie getoon word nie. %'n Diagram word "
        "op hierie manier afgekort as dit nie in die gespesifiseerde raam pas nie.\n"
        "</ul>\n"
        "Die pyltjies het die volgende betekenis:\n"
        "<ul>\n"
        "<li>%'n Donker blou pyltjie verteenwoordig 'n publieke afgeleide "
        "verwantskap tussen twee klasse.\n"
        "<li>%'n Donker groen pyltjie word gebruik vir 'n beskermde verwantskap.\n"
        "<li>%'n Donker rooi pyltjie verteenwoordig private verwantskappe.\n"
        "<li>%'n Pers pyltjie word gebruik as 'n klas gebruik of bevat word "
        "deur 'n ander klas. Die pyltjie word gemerk met die veranderlike(s) waar deur "
        "die verwysde klass verkrygbaar is.\n"
        "<li>%'n Geel stippel pyl verteenwoordig die verwantslap tussen 'n template instansie en "
        "die template waarvan die klas vervaardig is. Die pyltjie word gemerk met die "
        "template parameters van die instansie.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend() override
    {
      return "beskrywing";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Toets";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Toets Lys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Eienskappe";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Eienskap Dokumentasie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Strukture";
      }
      else
      {
        return "Klasse";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Pakket "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Die pakkette met kort beskrywings (indien beskikbaar):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Pakkette";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Waarde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Bug Lys";
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
    virtual QCString trRTFansicp() override
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Klas" : "klas"));
      if (!singular)  result+="se";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Leër" : "leër"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Groep" : "groep"));
      if (!singular)  result+="e";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Bladsy" : "bladsy"));
      if (!singular)  result+="e";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Lid" : "lid"));
      if (!singular)  result = (first_capital ? "Lede" : "lede");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Outeur" : "outeur"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Verwysings";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Implimenteer "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Geimplimenteer in "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Inhoudsopgawe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Verouderde Lys";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Event Dokumentasie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Pakket Tipes";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Pakket Funksies";
    }
    virtual QCString trPackageMembers() override
    {
      return "Pakket Lede";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Statiese Pakket Funksies";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Pakket Eienskappe";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Statiese Pakket Eienskappe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Alle Lede";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "'n gebruiks diagram vir hierdie funksie:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
    {
      return "Soektog Resultate";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    virtual QCString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "Geen dokumente na gelang van jou navraag nie.";
      }
      else if (numDocuments==1)
      {
        return "Die soektog het <b>1</b> dokument gevind na gelang van jou navraag.";
      }
      else
      {
        return "Die soektog het <b>$num</b> dokumente gevind na gelang van jou navraag. "
               "Beste resultate eerste.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches() override
    {
      return "Teikens:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return filename + " Bron kode Leër";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Directory Hiërargie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Directory Documentasie"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Directories"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Directory Verwysing"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Director" : "director"));
      if (singular) result+="y"; else result+="ies";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText() override
    {
       return "Hierdie is 'n oorlaaide lede funksie, "
              "vertoon vir volledigheid. Dit verskil slegs van die bogegewe "
              "funksie in die argument(e) wat dit aanvaar.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Hier is die roep skema vir die funksie:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Enumerator Dokumentasie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Lede Funksie/Subroetine Dokumentasie"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Data Tipes Lys"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Data Velde"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Hier is die data tipes met kort beskrywings:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Hier is 'n lys van alle ";
      if (!extractAll)
      {
        result+="gedokumenteerde ";
      }
      result+="data tipe lede";
      result+=" met skakels na ";
      if (!extractAll)
      {
         result+="die data strukture dokumentasie vir elke lid";
      }
      else
      {
         result+="die data tipes waaraan hulle behoort:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran() override
    { return "Data Tipe Indeks"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Data Tipe Dokumentasie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Funksies/Subroetines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Funksies/Subroetine Dokumentasie"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Data Tipes"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Modules Lys"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Hier is 'n lys van alle ";
      if (!extractAll) result+="gedokumenteerde ";
      result+="modules met kort beskrywings:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Module"; break;
        case ClassDef::Struct:     result+=" Tipe"; break;
        case ClassDef::Union:      result+=" Unie"; break;
        case ClassDef::Interface:  result+=" Interflak"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategorie"; break;
        case ClassDef::Exception:  result+=" Eksepsie"; break;
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+=" Bron";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Module Bron";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Module Lede"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Hier is 'n lys van alle ";
      if (!extractAll) result+="gedokumenteerde ";
      result+="module lede met skakels na ";
      if (extractAll)
      {
        result+="die module dokumentasie vir elke lid:";
      }
      else
      {
        result+="die modules waaraan hulle behoort:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex() override
    { return "Modules Indeks"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Module" : "module"));
      if (!singular)  result+="s";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="The documentation for this ";
      switch(compType)
      {
        case ClassDef::Class:      result+="module"; break;
        case ClassDef::Struct:     result+="tipe"; break;
        case ClassDef::Union:      result+="unie"; break;
        case ClassDef::Interface:  result+="Interflak"; break;
        case ClassDef::Protocol:   result+="protokcol"; break;
        case ClassDef::Category:   result+="kategorie"; break;
        case ClassDef::Exception:  result+="eksepsie"; break;
        default: break;
      }
      result+=" is gegenereer vanaf die foldende leer";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Tipe" : "tipe"));
      if (!singular)  result+="s";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Subprogram" : "subprogram"));
      if (!singular)  result+="me";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Tipe Limiete";
    }

    virtual QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "Ma.", "Di.", "Wo.", "Do.", "Vr.", "Sa.", "So." };
      static const char *days_full[]    = { "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrydag", "Saterdag", "Sondag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    virtual QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "Jan.", "Feb.", "Mrt.", "Apr.", "Mei", "Jun.", "Jul.", "Aug.", "Sep.", "Okt.", "Nov.", "Des." };
      static const char *months_full[]  = { "Januarie", "Februarie", "Maart", "April", "Mei", "Junie", "Julie", "Augustus", "September", "Oktober", "November", "Desember" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "vm.", "nm." };
      return dayPeriod[period];
    }
};

#endif
