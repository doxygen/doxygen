/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_HU_H
#define TRANSLATOR_HU_H

#include "translator.h"

class TranslatorHungarian : public TranslatorAdapter_1_2_1
{
  public:

    // --- Language control methods -------------------
    QCString idLanguage()
    { return "hungarian"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    QCString latexLanguageSupportCommand()
    {
      return "";
    }
    /*! returns the name of the package that is included by LaTeX */
    QCString latexBabelPackage() 
    { return ""; }

    /*! return the language charset. This will be used for the HTML output */
    QCString idLanguageCharset()
    {
      return "iso-8859-2";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "Kapcsolódó függvények"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "(Figyelem! Ezek a függvények nem tagjai az osztálynak!)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription()
    { return "Részletes leírás"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation()
    { return "Osztálytag típusdefiníciók dokumentációja"; }
    
    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation()
    { return "Osztálytag enumerációk dokumentációja"; }
    
    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation()
    { return "Osztálytag függvények (metódusok) dokumentációja"; }
    
    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation()
    { return "Osztálytag adatok dokumentációja"; }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() 
    { return "Részletek..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "Osztálytagok listája."; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList()
    { return "Osztálytagok listája"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "A(z) "; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers()
    { return " osztály tagjainak teljes listája, az örökölt tagokkal együtt."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Ezt a dokumentációt a Doxygen készítette";
      if (s) result+=(QCString)" a(z) "+s+(QCString)" projekthez";
      result+=" a forráskódból."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName()
    { return "enumeráció neve"; }
    
    /*! put after an enum value in the list of all members */
    QCString trEnumValue()
    { return "enumeráció értéke"; }
    
    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn()
    { return "definiálva itt:"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    QCString trVerbatimText(const char *f)
    { return (QCString)"Ez a(z) "+f+" definíciós fájl pontos tartalma."; }
    
    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    QCString trModules()
    { return "Modulok"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy()
    { return "Osztályhierarchia"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
    { return "Összetevõk listája"; }
    
    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList()
    { return "Fájlok listája"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    QCString trHeaderFiles()
    { return "Definíciós fájlok"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
    { return "Osztályok összetevõi"; }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers()
    { return "Fájlok elemei"; }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages()
    { return "Kapcsolódó lapok"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples()
    { return "Példák"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch()
    { return "Keresés"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription()
    { return "Ez a leszármaztatási lista hozzávetõleg, "
             "de nem szigorúan véve, ábécé sorrendbe rendezett:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Ez az összes ";
      if (!extractAll) result+="doumentált ";
      result+="fájl listája rövid leírásokkal:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
    { return "Ez az osztályok, struktúrák, uniók és interfészek "
             "listája rövid leírásokkal:"; 
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Ez az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="osztály összetevõ listája ";
      if (!extractAll) 
        result+="hivatkozással a hozzájuk tartózó osztály dokumentációra:";
      else 
        result+="az õket tartalmazó osztály megjelölésével:";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Ez az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="fájl elem listája ";
      if (extractAll) 
        result+="hivatkozással a hozzájuk tartózó fájl dokumentációra:";
      else 
        result+="az õket tartalmazó fájl megjelölésével:";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    QCString trHeaderFilesDescription()
    { return "Ezek a Programozói Interfészt (API) alkotó definíciós fájlok:"; }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription()
    { return "Ez a példák listája:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "Ez a kapcsolódó dokumentációk listája:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription()
    { return "Ez a modulok listája:"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    QCString trNoDescriptionAvailable()
    { return "Nincs hozzá leírás"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation()
    { return "Dokumentáció"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    QCString trModuleIndex()
    { return "Modul index"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    QCString trHierarchicalIndex()
    { return "Hierarchikus index"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    QCString trCompoundIndex()
    { return "Összetevõk indexe"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() 
    { return "Fájl index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation()
    { return "Modul documentáció"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation()
    { return "Osztály documentatáció"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation()
    { return "Fájl documentatáció"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    QCString trExampleDocumentation()
    { return "Példák documentatációja"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    QCString trPageDocumentation()
    { return "Kapcsolódó lapok documentációja"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual()
    { return "Referencia kézikönyv"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    QCString trDefines()
    { return "Definíciók"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    QCString trFuncProtos()
    { return "Függvény prototípusok"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    QCString trTypedefs()
    { return "Típusdefiníciók"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    QCString trEnumerations()
    { return "Enumerációk"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    QCString trFunctions()
    { return "Függvények"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trVariables()
    { return "Változók"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trEnumerationValues()
    { return "Enumeráció értékek"; }
    
    /*! This is used in man pages as the author section. */
    QCString trAuthor()
    { return "Szerzõ"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation()
    { return "Definíciók dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    QCString trFunctionPrototypeDocumentation()
    { return "Függvény prototípusok documentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation()
    { return "Típusdefiníciók documentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "Enumerációk documentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation()
    { return "Enumeráció értékek documentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation()
    { return "Függvények dokumentációja"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation()
    { return "Változók documentációja"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    QCString trCompounds()
    { return "Összetevõk"; }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    QCString trFiles()
    { return "Fájlok"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"";
      if (projName) result+=(QCString)"Projekt: "+projName;
      result+=(QCString)" Készült: "+date+" Készítette: ";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    QCString trWrittenBy()
    {
      return " melyet írt ";
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const char *clName)
    {
      return clName+(QCString)" osztály származtatási diagramja";
    }
    
    /*! this text is generated when the \\internal command is used. */
    QCString trForInternalUseOnly()
    { return "CSAK BELSÕ HASZNÁLATRA!"; }

    /*! this text is generated when the \\reimp command is used. */
    QCString trReimplementedForInternalReasons()
    { return "Belsõ okok miatt újraimplementálva, az API-t nem érinti."; }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning()
    { return "Figyelmeztetés"; }

    /*! this text is generated when the \\bug command is used. */
    QCString trBugsAndLimitations()
    { return "Hibák és korlátozások"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion()
    { return "Verzió"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate()
    { return "Dátum"; }

    /*! this text is generated when the \\author command is used. */
    QCString trAuthors()
    { return "Szerzõ(k)"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns()
    { return "Visszaadott érték"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso()
    { return "Lásd még"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters()
    { return "Paraméterek"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions()
    { return "Kivételek"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy()
    { return "Készítette"; }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList()
    { return "Névtér lista"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Ez az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="névtér listája rövid leírásokkal:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends()
    { return "Barátok"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    QCString trRelatedFunctionDocumentation()
    { return "Barát és kapcsolódó függvények dokumentációja"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" osztály"; break;
        case ClassDef::Struct:     result+=" struktúra"; break;
        case ClassDef::Union:      result+=" unió"; break;
        case ClassDef::Interface:  result+=" interfész"; break;
        case ClassDef::Exception:  result+=" kivétel"; break;
      }
      if (isTemplate) result+=" sablon";
      result+=" referencia";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" fájl referencia"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" névtér reference";
      return result;
    }
    
    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers()
    { return "Publikus metódusok"; }
    QCString trPublicSlots()
    { return "Publikus adatok"; }
    QCString trSignals()
    { return "Szignálok"; }
    QCString trStaticPublicMembers()
    { return "Statikus publikus metódusok"; }
    QCString trProtectedMembers()
    { return "Védett metódusok"; }
    QCString trProtectedSlots()
    { return "Védett adatok"; }
    QCString trStaticProtectedMembers()
    { return "Statikus védett metódusok"; }
    QCString trPrivateMembers()
    { return "Privát metódusok"; }
    QCString trPrivateSlots()
    { return "Privát adatok"; }
    QCString trStaticPrivateMembers()
    { return "Statikus privát metódusok"; }
    
    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries)
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
    QCString trInheritsList(int numEntries)
    {
      return "Szülõk osztályok: "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries)
    {
      return "Származtatott osztályok: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries)
    {
      return "Újra implementálva ebbõl az osztályból: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries)
    {
      return "Újra implementálva ebben az osztályban: "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers()
    { return "Névtér tagjai"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Ez az összes ";
      if (!extractAll) result+="dokumentált ";
      result+="névtér tagjainak listája ";
      if (extractAll) 
        result+="a hozzájuk tartozó dokumentációra való hivatkozással:";
      else 
        result+="az öket tartalmazó névtérrel:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "Névtér index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "Névtér documentáció"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "Névterek"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Ez a dokumentáció ";
      switch(compType)
      {
        case ClassDef::Class:      result+="az osztályról"; break;
        case ClassDef::Struct:     result+="a struktúráról"; break;
        case ClassDef::Union:      result+="az unióról"; break;
        case ClassDef::Interface:  result+="az interfészrõl"; break;
        case ClassDef::Exception:  result+="a kivételrõl"; break;
      }
      result+=" a következõ fájl";
      if (!single) result+="ok";
      result+=" alapján készült:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "Ábécé rendes lista"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Visszaadott értékek"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "Kezdõ lap"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "o."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return "Források";
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return "Definíció a(z) @1 fájl @0. sorában.";
    }
    QCString trDefinedInSourceFile()
    {
      return "Definíció a(z) @0 fájlban.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Ellenjavallt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Együttmûködési diagram "+clName+" osztályhoz:";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Definíciós fájl függési ábra "+fName+" fájlhoz:";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "Konstruktor és destruktor dokumentáció"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "Ugrás a fájl forrásához.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "Ugrás a fájl dokumentációjához.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition()
    {
      return "Elõfeltétel";
    }
    /*! Text for the \\post command */
    QCString trPostcondition()
    {
      return "Utófeltétel";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant()
    {
      return "Invariáns";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Kezdõ érték:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "forráskód";
    }
    QCString trGraphicalHierarchy()
    {
      return "Osztály-hierarchia grafikon";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Ugrás az osztály-hierarchia grafikonhoz";
    }
    QCString trGotoTextualHierarchy()
    {
      return "Ugrás az osztály-hierarchia leíráshoz";
    }
    QCString trPageIndex()
    {
      return "Oldal index";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return "Megjegyzés";
    }
    QCString trPublicTypes()
    {
      return "Publikus típusok";
    }
    QCString trPublicAttribs()
    {
      return "Publikus attribútumok";
    }
    QCString trStaticPublicAttribs()
    {
      return "Statikus publikus attribútumok";
    }
    QCString trProtectedTypes()
    {
      return "Védett típusok";
    }
    QCString trProtectedAttribs()
    {
      return "Védett attribútumok";
    }
    QCString trStaticProtectedAttribs()
    {
      return "Statikus védett attribútumok";
    }
    QCString trPrivateTypes()
    {
      return "Privát típusok";
    }
    QCString trPrivateAttribs()
    {
      return "Privát attribútumok";
    }
    QCString trStaticPrivateAttribs()
    {
      return "Statikus privát attribútumok";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo()
    {
      return "Tennivaló";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList()
    {
      return "Tennivalók listája";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()
    {
      return "hivatkozás innen:";
    }
    QCString trRemarks()
    {
      return "Megjegyzések";
    }
    QCString trAttention()
    {
      return "Figyelem";
    }
    QCString trInclByDepGraph()
    {
      return "Ez az ábra azt mutatja, hogy mely fájlok ágyazzák be "
             "közvetve vagy közvetlenül ezt a fájlt:";
    }
    QCString trSince()
    {
      return "Elõször bevezetve";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle()
    {
      return "Jelmagyarázat";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs()
    {
      return 
        "Hogyan értelmezzük a DopxyGen által készített ábrákat?<p>\n"
        "Vegyük a következõ példát:\n"
        "\\code\n"
        "/*! Nem látható osztály, nem fér ki, vágásra kerül */\n"
        "class Invisible { };\n\n"
        "/*! Származtatás rejtett */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Nem dokumentált osztály */\n"
        "class Undocumented { };\n\n"
        "/*! Publikus származtatásal levezetett osztály */\n"
        "class PublicBase : public Truncated { };\n\n"
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
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Ha a konfigurációs fájl \\c MAX_DOT_GRAPH_HEIGHT elemének értékét "
        "200-ra állítjuk, az eredmény a következõ ábra lesz:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Az ábrán levõ dobozok jelentése:\n"
        "<ul>\n"
        "<li>Kitöltött fekete doboz jelzi azt az osztályt vagy struktúrát,"
        "amelyrõl az ábra szól.\n"
        "<li>Fekete keret jelszi a dokumentált osztályokat vagy struktúrákat.\n"
        "<li>Szürke keret jelzi a nem dokumentált osztályokat vagy struktúrákat.\n"
        "<li>Piros keret jelzi azokat az osztályokat vagy struktúrákat, amelyeknél vágás miatt nem látható "
        "az összes leszármaztatási kapcsolat. Egy ábra vágásra kerül, ha nem fér bele "
        "a megadott tartományba."
        "</ul>\n"
        "A nyilak jelentése:\n"
        "<ul>\n"
        "<li>Sötétkék nyíl jelzi a publikus származtatást.\n"
        "<li>Sötétzöld nyíl jelzi a védett származtatást.\n"
        "<li>Sötétvörös nyíl jelzi a privát származtatást.\n"
        "<li>Lila szaggatott nyíl jelzi, ha az osztály egy másikat használ vagy tartalmaz. "
        "A nyíl felirata jelzi a változók nevét, amelyeken keresztül a másik osztály kapcsolódik.\n"
        "</ul>\n";

    }
    /*! text for the link to the legend page */
    QCString trLegend()
    {
      return "Jelmagyarázat";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    QCString trTest()
    {
      return "Teszt";
    }
    /*! Used as the header of the test list */
    QCString trTestList()
    {
      return "Teszt lista";
    }

};

#endif
