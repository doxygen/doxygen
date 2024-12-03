/******************************************************************************
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
 * Original Hungarian translation by
 * György Földvári <foldvari@diatronltd.com>
 *
 * Extended, revised and updated by
 * Ákos Kiss <akiss@users.sourceforge.net>
 *
 * Further extended, revised and updated by
 * Ferenc Tamási <tf551@hszk.bme.hu>
 *
 * Further extended, since 1.4.6 to 1.8.4
 * László Kmety <silverkml@gmail.com>
 */

#ifndef TRANSLATOR_HU_H
#define TRANSLATOR_HU_H

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
class TranslatorHungarian : public TranslatorAdapter_1_8_15
{
  private:
    const char * zed(char c)
    {
        switch (c & ~('a' ^ 'A')) {
            case 'B': case 'C': case 'D': case 'F': case 'G':
            case 'H': case 'J': case 'K': case 'L': case 'M':
            case 'N': case 'P': case 'Q': case 'R': case 'S':
            case 'T': case 'V': case 'W': case 'X': case 'Z':
                return "  ";
            default:
                return "z ";
        }
    }
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
    { return "hungarian"; }

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
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[magyar]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "hu";
    }
    QCString getLanguageString() override
    {
      return "0x40E Hungarian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Kapcsolódó függvények"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Figyelem! Ezek a függvények nem tagjai az osztálynak!)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Részletes leírás"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Részletek"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Típusdefiníció-tagok dokumentációja"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Enumeráció-tagok dokumentációja"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Tagfüggvények dokumentációja"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatmezők dokumentációja";
      }
      else
      {
        return "Adattagok dokumentációja";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Részletek..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "A tagok teljes listája"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Taglista"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "A(z)"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return " osztály tagjainak teljes listája, az örökölt tagokkal együtt."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Ezt a dokumentációt a Doxygen készítette ";
      if (!s.isEmpty()) result+=QCString(" a") + zed(s[0])+s+" projekthez";
      result+=" a forráskódból.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "enum"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "enum-érték"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definiálja:"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Modulok"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Osztályhierarchia"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatszerkezetek";
      }
      else
      {
        return "Osztálylista";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Fájllista"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatmezők";
      }
      else
      {
        return "Osztálytagok";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globális elemek";
      }
      else
      {
        return "Fájlelemek";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Kapcsolódó lapok"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Példák"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Keresés"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Majdnem (de nem teljesen) betűrendbe szedett "
             "leszármazási lista:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="fájl listája rövid leírásokkal:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Az összes adatszerkezet listája rövid leírásokkal:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Az összes osztály listája rövid leírásokkal:";
      }
      else
      {
        return "Az összes osztály, struktúra, unió és interfész "
               "listája rövid leírásokkal:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll)
      {
        result+="dokumentált ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struktúra- és uniómező";
      }
      else
      {
        result+="osztálytag";
      }
      result+=" listája, valamint hivatkozás ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a megfelelő struktúra/unió dokumentációra minden mezőnél:";
        }
        else
        {
          result+="a megfelelő osztálydokumentációra minden tagnál:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a struktúrákra/uniókra, amikhez tartoznak:";
        }
        else
        {
          result+="az osztályokra, amikhez tartoznak:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="függvény, változó, makródefiníció, enumeráció és típusdefiníció";
      }
      else
      {
        result+="fájlelem";
      }
      result+=" listája, valamint hivatkozás ";
      if (extractAll)
        result+="a fájlokra, amikhez tartoznak:";
      else
        result+="a dokumentációra:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "A példák listája:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "A kapcsolódó dokumentációk listája:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "A modulok listája:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentáció"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Modulmutató"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hierarchikus mutató"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatszerkezet-mutató";
      }
      else
      {
        return "Osztálymutató";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Fájlmutató"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Modulok dokumentációja"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatszerkezetek dokumentációja";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Osztályok dokumentációja";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Fájlok dokumentációja"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Referencia kézikönyv"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Makródefiníciók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Típusdefiníciók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerációk"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Függvények"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Változók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Enumeráció-értékek"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Makródefiníciók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Típusdefiníciók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Enumerációk dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Függvények dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Változók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatszerkezetek";
      }
      else
      {
        return "Osztályok";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="";
      if (!projName.isEmpty()) result+="Projekt: "+projName;
      result+=" Készült: "+date+" Készítette: ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("A")+zed(clName[0])+clName+" osztály származási diagramja:";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Figyelmeztetés"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Verzió"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Dátum"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Visszatérési érték"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Lásd még"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Paraméterek"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Kivételek"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Készítette"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Névtérlista"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="névtér listája rövid leírásokkal:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Barátok"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Barát és kapcsolódó függvények dokumentációja"; }

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
        case ClassDef::Class:      result+=" osztály"; break;
        case ClassDef::Struct:     result+=" struktúra"; break;
        case ClassDef::Union:      result+=" unió"; break;
        case ClassDef::Interface:  result+=" interfész"; break;
        case ClassDef::Protocol:   result+=" protokoll"; break;
        case ClassDef::Category:   result+=" kategória"; break;
        case ClassDef::Exception:  result+=" kivétel"; break;
        default: break;
      }
      if (isTemplate) result+="sablon-";
      result+="referencia";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" fájlreferencia";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" névtér-referencia";
      return result;
    }

    QCString trPublicMembers() override
    { return "Publikus tagfüggvények"; }
    QCString trPublicSlots() override
    { return "Publikus rések"; }
    QCString trSignals() override
    { return "Szignálok"; }
    QCString trStaticPublicMembers() override
    { return "Statikus publikus tagfüggvények"; }
    QCString trProtectedMembers() override
    { return "Védett tagfüggvények"; }
    QCString trProtectedSlots() override
    { return "Védett rések"; }
    QCString trStaticProtectedMembers() override
    { return "Statikus védett tagfüggvények"; }
    QCString trPrivateMembers() override
    { return "Privát tagfüggvények"; }
    QCString trPrivateSlots() override
    { return "Privát rések"; }
    QCString trStaticPrivateMembers() override
    { return "Statikus privát tagfüggvények"; }

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
            result+=" és ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Ősök: "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Leszármazottak: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Újraimplementált ősök: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Újraimplementáló leszármazottak: "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Névtértagok"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="névtér tagjainak listája, valamint hivatkozás ";
      if (extractAll)
        result+=" a megfelelő névtér dokumentációra minden tagnál:";
      else
        result+=" a névterekre, amelynek tagjai:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Névtérmutató"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Névterek dokumentációja"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Névterek"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      QCString result="Ez a dokumentáció ";
      switch(compType)
      {
        case ClassDef::Class:      result+="az osztályról"; break;
        case ClassDef::Struct:     result+="a struktúráról"; break;
        case ClassDef::Union:      result+="az unióról"; break;
        case ClassDef::Interface:  result+="az interfészről"; break;
        case ClassDef::Protocol:   result+="a protokollról"; break;
        case ClassDef::Category:   result+="a kategóriáról"; break;
        case ClassDef::Exception:  result+="a kivételről"; break;
        default: break;
      }
      result+=" a következő fájl";
      if (!single) result+="ok";
      result+=" alapján készült:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Visszatérési értékek"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Főoldal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "o."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definíció a(z) @1 fájl @0. sorában.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definíció a(z) @0 fájlban.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Ellenjavallt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return QCString("A")+zed(clName[0])+clName+" osztály együttműködési diagramja:";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return QCString("A")+zed(fName[0])+fName+" definíciós fájl függési gráfja:";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Konstruktorok és destruktorok dokumentációja";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Ugrás a fájl forráskódjához.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ugrás a fájl dokumentációjához.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Előfeltétel";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Utófeltétel";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariáns";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Kezdő érték:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "forráskód";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Osztályhierarchia-ábra";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Ugrás az osztályhierarchia-ábrához";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Ugrás az szöveges osztályhierarchiához";
    }
    QCString trPageIndex() override
    {
      return "Oldalmutató";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Megjegyzés";
    }
    QCString trPublicTypes() override
    {
      return "Publikus típusok";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatmezők";
      }
      else
      {
        return "Publikus attribútumok";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Statikus publikus attribútumok";
    }
    QCString trProtectedTypes() override
    {
      return "Védett típusok";
    }
    QCString trProtectedAttribs() override
    {
      return "Védett attribútumok";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statikus védett attribútumok";
    }
    QCString trPrivateTypes() override
    {
      return "Privát típusok";
    }
    QCString trPrivateAttribs() override
    {
      return "Privát attribútumok";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statikus privát attribútumok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Tennivaló";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Tennivalók listája";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Hivatkozások:";
    }
    QCString trRemarks() override
    {
      return "Megjegyzések";
    }
    QCString trAttention() override
    {
      return "Figyelem";
    }
    QCString trInclByDepGraph() override
    {
      return "Ez az ábra azt mutatja, hogy mely fájlok ágyazzák be "
             "közvetve vagy közvetlenül ezt a fájlt:";
    }
    QCString trSince() override
    {
      return "Először bevezetve";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Jelmagyarázat";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Ez az oldal elmagyarázza hogyan kell értelmezni a "
        "doxygen által készített ábrákat.<p>\n"
        "Vegyük a következő példát:\n"
        "\\code\n"
        "/*! Vágás miatt nem látható osztály */\n"
        "class Invisible { };\n\n"
        "/*! Levágott osztály, származása rejtett */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Doxygen kommentekkel nem dokumentált osztály */\n"
        "class Undocumented { };\n\n"
        "/*! Publikus származtatásal levezetett osztály */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Egy sablonosztály */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Védett származtatásal levezetett osztály */\n"
        "class ProtectedBase { };\n\n"
        "/*! Privát származtatásal levezetett osztály */\n"
        "class PrivateBase { };\n\n"
        "/*! Osztály, melyet a származtatott osztály használ */\n"
        "class Used { };\n\n"
        "/*! Osztály, mely több másiknak leszármazottja */\n"
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
        "Az eredmény a következő ábra lesz:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "A fenti ábrán levő dobozok jelentése a következő:\n"
        "<ul>\n"
        "<li>Kitöltött fekete doboz jelzi azt az osztályt vagy struktúrát,"
        "amelyről az ábra szól.</li>\n"
        "<li>Fekete keret jelzi a dokumentált osztályokat és struktúrákat.</li>\n"
        "<li>Szürke keret jelzi a nem dokumentált osztályokat és struktúrákat.</li>\n"
        "<li>Piros keret jelzi azokat az osztályokat és struktúrákat, amelyeknél vágás miatt nem látható "
        "az összes leszármaztatási kapcsolat. Egy ábra vágásra kerül, ha nem fér bele "
        "a megadott tartományba.</li>\n"
        "</ul>\n"
        "<p>\n"
        "A nyilak jelentése a következő:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Sötétkék nyíl jelzi a publikus származtatás "
        "kapcsolatát két osztály között.</li>\n"
        "<li>Sötétzöld nyíl jelzi a védett származtatást.</li>\n"
        "<li>Sötétvörös nyíl jelzi a privát származtatást.</li>\n"
        "<li>Lila szaggatott nyíl jelzi, ha az osztály egy másikat használ vagy tartalmaz. "
        "A nyíl felirata jelzi a változó(k) nevét, amelyeken keresztül a másik osztály kapcsolódik.</li>\n"
        "<li>Sárga szaggatott nyíl jelzi a kapcsolatot a sablonpéldány és a példányosított "
        "osztálysablon között. A nyíl felirata jelzi a pélány sablonparamétereit.</li>\n"
        "</ul>\n";

    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "Jelmagyarázat";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Teszt";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Tesztlista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Tulajdonságok";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Tulajdonságok dokumentációjka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Adatszerkezetek";
      }
      else
      {
        return "Osztályok";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return name+" csomag";
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "A csomagok rövid leírásai (ha léteznek):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Csomagok";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Érték:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Hiba";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Hiba lista";
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
      return "1250";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Tárgymutató";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool /* singular */) override
    {
      return createNoun(first_capital, true, "osztály", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fájl", "ok");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "névt", "erek", "ér");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "ok");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "oldal", "ak");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tag", "ok");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globális elem", "ek");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "szerző", "k");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Hivatkozások";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Megvalósítja a következőket: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Megvalósítják a következők: "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Tartalomjegyzék";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Ellenjavallt elemek listája";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Események";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Események dokumentációja";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Csomag típusok";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Csomag függvények";
    }
    QCString trPackageMembers() override
    {
      return "Csomag tagok";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statikus csomag függvények";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Csomag attribútumok";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statikus csomag attribútumok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Összes";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "A függvény hívási gráfja:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "A keresés eredménye";
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
        return "Sajnos egy dokumentum sem felelt meg a keresési feltételeknek.";
      }
      else if (numDocuments==1)
      {
        return "<b>1</b> dokumentum felelt meg a keresési feltételeknek.";
      }
      else
      {
        return "<b>$num</b> dokumentum felelt meg a keresési feltételeknek."
               "Elsőnek a legjobb találatok vannak feltüntetve.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Találatok:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " Forrásfájl";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Könyvtárhierarchia"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Könyvtárak dokumentációja"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Könyvtárak"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" könyvtárreferencia"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool /*singular*/) override
    {
      return createNoun(first_capital, true, "könyvtár", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Ez egy túlterhelt tagfüggvény, "
              "a kényelem érdekében. A fenti függvénytől csak abban különbözik, "
              "hogy milyen argumentumokat fogad el.";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "A függvény hívó gráfja:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Enumerációs-érték dokumentáció"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Tagfüggvény/Alprogram dokumentáció"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Adattípusok listája"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Adatmezők"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Rövid leírással ellátott adattípusok:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll)
      {
        result+="dokumentált ";
      }
      result+="adattípusú tagváltozó";
      result+=" hivatkozásokkal ellátva ";
      if (!extractAll)
      {
         result+="az egyes adattagok adatszerkezetének dokumentációjára";
      }
      else
      {
         result+="azokhoz az adattípusokhoz, amelyekhez tartoznak:";
      }
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Adattípus index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Adattípus dokumentáció"; }
    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Függvények/Alprogramok"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Függvény/Alprogram dokumentáció"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Adattípusok"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Modulok listája"; }

   /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="rövid leírással ellátott modul:";
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
        case ClassDef::Class:      result+=" modul"; break;
        case ClassDef::Struct:     result+=" típus"; break;
        case ClassDef::Union:      result+=" unió"; break;
        case ClassDef::Interface:  result+=" interfész"; break;
        case ClassDef::Protocol:   result+=" protokoll"; break;
        case ClassDef::Category:   result+=" kategória"; break;
        case ClassDef::Exception:  result+=" kivétel"; break;
        default: break;
      }
      if (isTemplate) result+=" sablon";
      result+=" hivatkozás";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" modul hivatkozás";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Modul adattagok"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="modul adattagja hivatkozásokkal ellátva ";
      if (extractAll)
      {
        result+="az egyes adattagok moduljainak dokumentációjára:";
      }
      else
      {
        result+="azokhoz a modulokhoz, amelyekhez tartoznak:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Modulok indexe"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "s");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="Ez a dokumentáció ";
      switch(compType)
      {
        case ClassDef::Class:      result+="a modulról"; break;
        case ClassDef::Struct:     result+="a típusról"; break;
        case ClassDef::Union:      result+="az unióról"; break;
        case ClassDef::Interface:  result+="az interfészról"; break;
        case ClassDef::Protocol:   result+="a protokollról"; break;
        case ClassDef::Category:   result+="a kategóriáról"; break;
        case ClassDef::Exception:  result+="a kivételről"; break;
        default: break;
      }
      result+=" a következő fájl";
      if (!single) result+="ok";
      result+=" alapján készült:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "típus", "ok");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "alprogram", "ok");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Típuskorlátozások";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" kapcsolat";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Betöltés...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Globális névtér";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Keresés...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Nincs egyezés";
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
      return "Fájl a(z) "+name+" könyvtárban";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Tartalmazott fájl a(z) "+name+" könyvtárban";
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
      static const char *days[]   = { "Hétfő","Kedd","Szerda","Csütörtök","Péntek","Szombat","Vasárnap" };
      static const char *months[] = { "Január","Február","Március","Április","Május","Június","Július","Augusztus","Szeptember","Október","November","December" };
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
      static const char *days_short[]   = { "H", "K", "Sze", "Cs", "P", "Szo", "V" };
      static const char *days_full[]    = { "hétfő", "kedd", "szerda", "csütörtök", "péntek", "szombat", "vasárnap" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan.", "febr.", "márc.", "ápr.", "máj.", "jún.", "júl.", "aug.", "szept.", "okt.", "nov.", "dec." };
      static const char *months_full[]  = { "január", "február", "március", "április", "május", "június", "július", "augusztus", "szeptember", "október", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "de.", "du." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Bibliográfia"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Szerzői jog"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Könyvtár függőségi gráf a(z) ")+name+"-könyvtárhoz:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "részletességi szint"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Sablon paraméterek"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "és "+number+" elemmel több..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "A dokumentáció ehhez az enum-hoz a következő fájl";
      if (!single) result+="ok";
      result+=" alapján készült:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" felsoroló referencia"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" a(z) "+what+" osztályból származnak"; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "További örökölt tagok"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "engedélyez" : "letilt";
      return "Kattintson a(z) "+opt+" panel synchronisation";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "@0 kategória szerint.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "@0 kiterjesztett osztály.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Osztály metódusok";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Példány metódusok";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Metódus dokumentáció";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Exportált interfészek"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Mellékelt szolgáltatások"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Konstans csoportok"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" konstans csoport referencia";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" szolgáltatás referencia";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" egyke példány referencia";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="A szolgáltatás dokumentációja "
                                "a következő fájl";
      if (single) result+="ból"; else result+="okból";
      result+="lett létrehozva:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Az egyke példány dokomentációja "
                                "a következő fájl";
      if (single) result+="ból"; else result+="okból";
      result+="lett létrehozva:";
      return result;
    }
};

#endif
