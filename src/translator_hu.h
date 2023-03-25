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
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[magyar]{babel}\n";
    }

    virtual QCString trISOLang()
    {
      return "hu";
    }
    virtual QCString getLanguageString()
    {
      return "0x40E Hungarian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Kapcsolódó függvények"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Figyelem! Ezek a függvények nem tagjai az osztálynak!)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Részletes leírás"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails()
    { return "Részletek"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Típusdefiníció-tagok dokumentációja"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Enumeráció-tagok dokumentációja"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Tagfüggvények dokumentációja"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
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
    virtual QCString trMore()
    { return "Részletek..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "A tagok teljes listája"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Taglista"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "A(z) "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return " osztály tagjainak teljes listája, az örökölt tagokkal együtt."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s)
    { QCString result="Ezt a dokumentációt a Doxygen készítette ";
      if (!s.isEmpty()) result+=QCString(" a") + zed(s[0])+s+" projekthez";
      result+=" a forráskódból.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum-érték"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definiálja:"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Modulok"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Osztályhierarchia"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
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
    virtual QCString trFileList()
    { return "Fájllista"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
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
    virtual QCString trFileMembers()
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
    virtual QCString trRelatedPages()
    { return "Kapcsolódó lapok"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Példák"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Keresés"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Majdnem (de nem teljesen) betűrendbe szedett "
             "leszármazási lista:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="fájl listája rövid leírásokkal:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
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
    virtual QCString trCompoundMembersDescription(bool extractAll)
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
    virtual QCString trFileMembersDescription(bool extractAll)
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
    virtual QCString trExamplesDescription()
    { return "A példák listája:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "A kapcsolódó dokumentációk listája:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "A modulok listája:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentáció"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modulmutató"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarchikus mutató"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
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
    virtual QCString trFileIndex()
    { return "Fájlmutató"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modulok dokumentációja"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
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
    virtual QCString trFileDocumentation()
    { return "Fájlok dokumentációja"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referencia kézikönyv"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makródefiníciók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Típusdefiníciók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerációk"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Függvények"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Változók"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Enumeráció-értékek"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Makródefiníciók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Típusdefiníciók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Enumerációk dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Függvények dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Változók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
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
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName)
    {
      QCString result="";
      if (!projName.isEmpty()) result+="Projekt: "+projName;
      result+=" Készült: "+date+" Készítette: ";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName)
    {
      return QCString("A")+zed(clName[0])+clName+" osztály származási diagramja:";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Figyelmeztetés"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Verzió"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Dátum"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Visszatérési érték"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Lásd még"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Paraméterek"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Kivételek"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Készítette"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Névtérlista"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="névtér listája rövid leírásokkal:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Barátok"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Barát és kapcsolódó függvények dokumentációja"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
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
    virtual QCString trFileReference(const QCString &fileName)
    {
      QCString result=fileName;
      result+=" fájlreferencia";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName)
    {
      QCString result=namespaceName;
      result+=" névtér-referencia";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Publikus tagfüggvények"; }
    virtual QCString trPublicSlots()
    { return "Publikus rések"; }
    virtual QCString trSignals()
    { return "Szignálok"; }
    virtual QCString trStaticPublicMembers()
    { return "Statikus publikus tagfüggvények"; }
    virtual QCString trProtectedMembers()
    { return "Védett tagfüggvények"; }
    virtual QCString trProtectedSlots()
    { return "Védett rések"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statikus védett tagfüggvények"; }
    virtual QCString trPrivateMembers()
    { return "Privát tagfüggvények"; }
    virtual QCString trPrivateSlots()
    { return "Privát rések"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statikus privát tagfüggvények"; }

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
            result+=" és ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Ősök: "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Leszármazottak: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Újraimplementált ősök: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Újraimplementáló leszármazottak: "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Névtértagok"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Névtérmutató"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Névterek dokumentációja"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Névterek"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
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
    virtual QCString trReturnValues()
    { return "Visszatérési értékek"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Főoldal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "o."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definíció a(z) @1 fájl @0. sorában.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definíció a(z) @0 fájlban.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Ellenjavallt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName)
    {
      return QCString("A")+zed(clName[0])+clName+" osztály együttműködési diagramja:";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName)
    {
      return QCString("A")+zed(fName[0])+fName+" definíciós fájl függési gráfja:";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Konstruktorok és destruktorok dokumentációja";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Ugrás a fájl forráskódjához.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Ugrás a fájl dokumentációjához.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Előfeltétel";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Utófeltétel";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariáns";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Kezdő érték:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "forráskód";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Osztályhierarchia-ábra";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Ugrás az osztályhierarchia-ábrához";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Ugrás az szöveges osztályhierarchiához";
    }
    virtual QCString trPageIndex()
    {
      return "Oldalmutató";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Megjegyzés";
    }
    virtual QCString trPublicTypes()
    {
      return "Publikus típusok";
    }
    virtual QCString trPublicAttribs()
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
    virtual QCString trStaticPublicAttribs()
    {
      return "Statikus publikus attribútumok";
    }
    virtual QCString trProtectedTypes()
    {
      return "Védett típusok";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Védett attribútumok";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statikus védett attribútumok";
    }
    virtual QCString trPrivateTypes()
    {
      return "Privát típusok";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Privát attribútumok";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statikus privát attribútumok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Tennivaló";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Tennivalók listája";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Hivatkozások:";
    }
    virtual QCString trRemarks()
    {
      return "Megjegyzések";
    }
    virtual QCString trAttention()
    {
      return "Figyelem";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Ez az ábra azt mutatja, hogy mely fájlok ágyazzák be "
             "közvetve vagy közvetlenül ezt a fájlt:";
    }
    virtual QCString trSince()
    {
      return "Először bevezetve";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Jelmagyarázat";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
    virtual QCString trLegend()
    {
      return "Jelmagyarázat";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Teszt";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Tesztlista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Tulajdonságok";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Tulajdonságok dokumentációjka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
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
    virtual QCString trPackage(const QCString &name)
    {
      return name+" csomag";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "A csomagok rövid leírásai (ha léteznek):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Csomagok";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Érték:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Hiba";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1250";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Tárgymutató";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool /* singular */)
    {
      QCString result((first_capital ? "Osztály" : "osztály"));
      //if (!singular)  result+="ok";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Fájl" : "fájl"));
      if (!singular)  result+="ok";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result("");
      if (!singular) result+=first_capital ? "Névterek" : "névterek";
      else           result+=first_capital ? "Névtér"   : "névtér";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="ok";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Oldal" : "oldal"));
      if (!singular)  result+="ak";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tag" : "tag"));
      if (!singular)  result+="ok";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Globális elem" : "globális elem"));
      if (!singular)  result+="ek";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Szerző" : "szerző"));
      if (!singular)  result+="k";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Hivatkozások";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Megvalósítja a következőket: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Megvalósítják a következők: "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Tartalomjegyzék";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Ellenjavallt elemek listája";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Események";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Események dokumentációja";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Csomag típusok";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions()
    {
      return "Csomag függvények";
    }
    virtual QCString trPackageMembers()
    {
      return "Csomag tagok";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions()
    {
      return "Statikus csomag függvények";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Csomag attribútumok";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statikus csomag attribútumok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Összes";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "A függvény hívási gráfja:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Találatok:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Forrásfájl";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Könyvtárhierarchia"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Könyvtárak dokumentációja"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Könyvtárak"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName)
    { QCString result=dirName; result+=" könyvtárreferencia"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool /*singular*/)
    {
      QCString result((first_capital ? "Könyvtár" : "könyvtár"));
      //if (singular) result+="y"; else result+="ies";
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
       return "Ez egy túlterhelt tagfüggvény, "
              "a kényelem érdekében. A fenti függvénytől csak abban különbözik, "
              "hogy milyen argumentumokat fogad el.";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "A függvény hívó gráfja:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Enumerációs-érték dokumentáció"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Tagfüggvény/Alprogram dokumentáció"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Adattípusok listája"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Adatmezők"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Rövid leírással ellátott adattípusok:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Adattípus index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Adattípus dokumentáció"; }
    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Függvények/Alprogramok"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Függvény/Alprogram dokumentáció"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Adattípusok"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modulok listája"; }

   /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="rövid leírással ellátott modul:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
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
    virtual QCString trModuleReference(const QCString &namespaceName)
    {
      QCString result=namespaceName;
      result+=" modul hivatkozás";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modul adattagok"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Modulok indexe"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
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
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Típus" : "típus"));
      if (!singular)  result+="ok";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Alprogram" : "alprogram"));
      if (!singular)  result+="ok";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Típuskorlátozások";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name)
    {
      return QCString(name)+" kapcsolat";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Betöltés...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globális névtér";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Keresés...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
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
    virtual QCString trFileIn(const QCString &name)
    {
      return "Fájl a(z) "+name+" könyvtárban";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name)
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
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime)
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
    virtual QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full)
    {
      static const char *days_short[]   = { "H", "K", "Sze", "Cs", "P", "Szo", "V" };
      static const char *days_full[]    = { "hétfő", "kedd", "szerda", "csütörtök", "péntek", "szombat", "vasárnap" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    virtual QCString trMonth(int month, bool first_capital, bool full)
    {
      static const char *months_short[] = { "jan.", "febr.", "márc.", "ápr.", "máj.", "jún.", "júl.", "aug.", "szept.", "okt.", "nov.", "dec." };
      static const char *months_full[]  = { "január", "február", "március", "április", "május", "június", "július", "augusztus", "szeptember", "október", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    virtual QCString trDayPeriod(int period)
    {
      static const char *dayPeriod[] = { "de.", "du." };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Bibliográfia"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Szerzői jog"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name)
    { return QCString("Könyvtár függőségi gráf a(z) ")+name+"-könyvtárhoz:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "részletességi szint"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Sablon paraméterek"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "és "+number+" elemmel több..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "A dokumentáció ehhez az enum-hoz a következő fájl";
      if (!single) result+="ok";
      result+=" alapján készült:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name)
    { return QCString(name)+" felsoroló referencia"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what)
    { return QCString(members)+" a(z) "+what+" osztályból származnak"; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "További örökölt tagok"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "engedélyez" : "letilt";
      return "Kattintson a(z) "+opt+" panel synchronisation";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "@0 kategória szerint.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "@0 kiterjesztett osztály.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Osztály metódusok";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Példány metódusok";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Metódus dokumentáció";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces()
    { return "Exportált interfészek"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices()
    { return "Mellékelt szolgáltatások"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups()
    { return "Konstans csoportok"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName)
    {
      QCString result=namespaceName;
      result+=" konstans csoport referencia";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName)
    {
      QCString result=sName;
      result+=" szolgáltatás referencia";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName)
    {
      QCString result=sName;
      result+=" egyke példány referencia";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result="A szolgáltatás dokumentációja "
                                "a következő fájl";
      if (single) result+="ból"; else result+="okból";
      result+="lett létrehozva:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
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
