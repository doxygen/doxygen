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
 
 /*
  * Original Hungarian translation by
  * György Földvári  <foldvari@diatronltd.com>
  *
  * Extended, revised and updated by
  * Ákos Kiss  <akiss@users.sourceforge.net>
  *
  * Further extended, revised and updated by
  * Tamási Ferenc <tf551@hszk.bme.hu>
  */

#ifndef TRANSLATOR_HU_H
#define TRANSLATOR_HU_H

class TranslatorHungarian : public TranslatorAdapter_1_4_6
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
    virtual QCString idLanguage()
    { return "hungarian"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[magyar]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-2";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Adatmezõk dokumentációja"; 
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
    
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Ezt a dokumentációt a Doxygen készítette ";
      if (s) result+=(QCString)" a" + zed(s[0])+s+(QCString)" projekthez";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Adatmezõk"; 
      }
      else
      {
        return "Osztálytagok"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    { return "Majdnem (de nem teljesen) betûrendbe szedett "
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
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Az összes adatszerkezet listája rövid leírásokkal:"; 
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="struktúra- és úniómezõ";
      }
      else
      {
        result+="osztálytag";
      }
      result+=" listája, valamint hivatkozás ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="a megfelelõ struktúra-/úniódokumentációra minden mezõnél:";
        }
        else
        {
          result+="a megfelelõ osztálydokumentációra minden tagnál:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="a struktúrákra/úniókra, amikhez tartoznak:";
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
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Adatszerkezetek dokumentációja";
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

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Példák dokumentációja"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Kapcsolódó dokumentációk"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referencia kézikönyv"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makródefiníciók"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Függvény-prototípusok"; }

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
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Függvény-prototípusok dokumentációja"; }

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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"";
      if (projName) result+=(QCString)"Projekt: "+projName;
      result+=(QCString)" Készült: "+date+" Készítette: ";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return " melyet írt ";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"A"+zed(clName[0])+clName+" osztály származási diagramja:";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "CSAK BELSÕ HASZNÁLATRA!"; }

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
    virtual QCString trCompoundReference(const char *clName,
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
        case ClassDef::Protocol:   result+=" protocol"; break; // translate me!
        case ClassDef::Category:   result+=" category"; break; // translate me!
        case ClassDef::Exception:  result+=" kivétel"; break;
      }
      if (isTemplate) result+="sablon-";
      result+="referencia";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" fájlreferencia"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
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
      return "Õsök: "+trWriteList(numEntries)+".";
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
      return "Újraimplementált õsök: "+trWriteList(numEntries)+".";
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
        result+=" a megfelelõ névtér dokumentációra minden tagnál:";
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
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Ez a dokumentáció ";
      switch(compType)
      {
        case ClassDef::Class:      result+="az osztályról"; break;
        case ClassDef::Struct:     result+="a struktúráról"; break;
        case ClassDef::Union:      result+="az unióról"; break;
        case ClassDef::Interface:  result+="az interfészrõl"; break;
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
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
    virtual QCString trAlphabeticalList()
    { return "Betûrendes lista"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Visszatérési értékek"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Fõoldal"; }

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
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"A"+zed(clName[0])+clName+" osztály együttmûködési diagramja:";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"A"+zed(fName[0])+fName+" definíciós fájl függési gráfja:";
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
      return "Elõfeltétel";
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
      return "Kezdõ érték:";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Adatmezõk";
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
      return "Elõször bevezetve";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Jelmagyarázat";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return 
        "Ez az oldal elmagyarázza hogyan kell értelmezni a "
        "doxygen által készített ábrákat.<p>\n"
        "Vegyük a következõ példát:\n"
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
        "Ha a konfigurációs fájl \\c MAX_DOT_GRAPH_HEIGHT elemének értékét "
        "240-re állítjuk, az eredmény a következõ ábra lesz:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Az ábrán levõ dobozok jelentése:\n"
        "<ul>\n"
        "<li>Kitöltött fekete doboz jelzi azt az osztályt vagy struktúrát,"
        "amelyrõl az ábra szól.\n"
        "<li>Fekete keret jelzi a dokumentált osztályokat és struktúrákat.\n"
        "<li>Szürke keret jelzi a nem dokumentált osztályokat és struktúrákat.\n"
        "<li>Piros keret jelzi azokat az osztályokat és struktúrákat, amelyeknél vágás miatt nem látható "
        "az összes leszármaztatási kapcsolat. Egy ábra vágásra kerül, ha nem fér bele "
        "a megadott tartományba."
        "</ul>\n"
        "A nyilak jelentése:\n"
        "<ul>\n"
        "<li>Sötétkék nyíl jelzi a publikus származtatást.\n"
        "<li>Sötétzöld nyíl jelzi a védett származtatást.\n"
        "<li>Sötétvörös nyíl jelzi a privát származtatást.\n"
        "<li>Lila szaggatott nyíl jelzi, ha az osztály egy másikat használ vagy tartalmaz. "
        "A nyíl felirata jelzi a változó(k) nevét, amelyeken keresztül a másik osztály kapcsolódik.\n"
        "<li>Sárga szaggatott nyíl jelzi a kapcsolatot a sablonpéldány és a példányosított "
        "osztálysablon között. A nyíl felirata jelzi a pélány sablonparamétereit.\n"
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
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP tagfüggvények";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Adatszerkezetek";
      }
      else
      {
        return "Osztályok";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return name+(QCString)" csomag";
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Csomaglista";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "A csomagok rövid leírásai (amennyiben léteznek):";
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
    virtual QCString trClass(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Osztály" : "osztály"));
      //if (!singular)  result+="es";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Fájl" : "fájl"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Névtér" : "névtér"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Csoport" : "csoport"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Oldal" : "oldal"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Tag" : "tag"));
      //if (!singular)  result+="s";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool /*singular*/)
    { 
      QCString result((first_capital ? "Globális elem" : "globális elem"));
      //if (!singular)  result+="s";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Szerzõ" : "szerzõ"));
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
      return "Megvalósítja a következõket: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Megvalósítják a következõk: "+trWriteList(numEntries)+".";
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
    virtual QCString trPackageMembers()
    { 
      return "Csomag függvények";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
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

    /*! When the search engine is enabled this text is put in the header 
     *  of each page before the field where one can enter the text to search 
     *  for. 
     */
    virtual QCString trSearchForIndex()
    {
      return "Keresési kulcs";
    }
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
               "Elsõnek a legjobb találatok vannak feltüntetve.";
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

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Majdnem (de nem teljesen) betûrendbe szedett "
             "könyvtárhierarchia:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
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
       return "Ez egy túlterhelt tagfüggvény."
              "A fenti függvénytõl csak argumentumaiban különbözik.";
    }
};

#endif
