/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
==================================================================================
Suomenkielinen k‰‰nnˆs:
Olli Korhonen      <olli.korhonen@ccc.fi>
==================================================================================
1999/10/19
*  Alustava k‰‰nnˆs valmis.

*  Huom: Kaikille termeille on keksitty k‰‰nnˆsvastine, mik‰ ei ehk‰ ole 
         kaikissa tapauksissa hyv‰ ratkaisu, ja parempikin termi voi 
         olla vakiintuneessa k‰ytˆss‰.

* Teht‰v‰‰: 
        - Termien kokoaminen listaksi ja suomennosten j‰rkevyyden tarkastelu. (lista on jo melko kattava)
                Author          = Tekij‰
                Class           = Luokka
                Compound        = Kooste ?
                Data            = Data
                Documentation   = Dokumentaatio
                Defines         = M‰‰rittelyt ?
                Description     = Selite
                Detailed        = Yksityiskohtainen
                diagram         = kaavio
                Enum            = Enumeraatio ?
                Exceptions      = Poikkeukset
                File            = Tiedosto
                Friends         = Yst‰v‰t ?
                Functions       = Funktiot
                Hierarchical    = Hierarkinen
                Index           = Indeksi
                Inherits        = Perii
                Member          = J‰sen
                Module          = Moduli
                Namespace       = Nimiavaruus ?
                Parameters      = Parametrit
                Private         = Yksityinen
                Protected       = Suojattu
                Prototypes      = Prototyypit
                Public          = Julkinen
                Reference Manual= K‰sikirja
                Reimplemented   = Uudelleen toteutettu
                Related         = Liittyv‰
                Signals         = Signaali
                Slots           = ? 
                Static          = Staattinen
                Struct          = ?
                Typedef         = Tyyppim‰‰rittely
                Union           = ?
                Variables       = Muuttujat
        - Taivutusmuotojen virheettˆmyyden tarkastelu prepositioiden korvauskohdissa.
        - Sanasta sanaan k‰‰nnˆskohtien mielekkyyden tarkastelu valmiista dokumentista.
        - umlaut vastineiden k‰yttˆ scandien kohdalla.

positiiviset kommentit otetaan ilolla vastaan.
===================================================================================
*/

#ifndef TRANSLATOR_FI_H
#define TRANSLATOR_FI_H

class TranslatorFinnish : public TranslatorEnglish
{
  public:
    /*! This method is used to generate a warning message to signal
     *  the user that the translation of his/her language of choice
     *  needs updating.
     */
    virtual QCString updateNeededMessage()
    {
      return "Warning: The Finnish translator is really obsolete.\n"
             "It was not updated since version 1.0.0.  As a result,\n"
             "some sentences may appear in English.\n\n";
    }

    QCString idLanguage()
    { return "finnish"; }
    QCString latexBabelPackage()
      // returns the name of the package that is included by LaTeX
    { return "finnish"; }

    QCString trRelatedFunctions()
      // used in the compound documentation before a list of related functions.
    { return "Liittyv‰t Funktiot"; } // "Related Functions";

    QCString trRelatedSubscript()
      // subscript for the related functions.
    { return "(Huomaa, ett‰ n‰m‰ eiv‰t ole j‰senfunktioita.)"; } // "(Note that these are not member functions.)"

    QCString trDetailedDescription()
      // header that is put before the detailed description of files, classes and namespaces.
    { return "Yksityiskohtainen Selite"; } // "Detailed Description"

    QCString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "J‰sen Tyyppim‰‰rittelyiden Dokumentaatio"; } // "Member Typedef Documentation"

    QCString trMemberEnumerationDocumentation()
      // header that is put before the list of enumerations.
    { return "J‰sen Enumeraatioiden Dokumentaatio"; } // "Member Enumeration Documentation"

    QCString trEnumerationValueDocumentation()
    { return "Enumeraatioarvojen Dokumentaatio"; } 

    QCString trMemberFunctionDocumentation()
      // header that is put before the list of member functions.
    { return "J‰sen Funktioiden Dokumentaatio"; } // "Member Function Documentation"
    QCString trMemberDataDocumentation()
      // header that is put before the list of member attributes.
    { return "J‰sen Datan Dokumentaatio"; } // "Member Data Documentation"

    QCString trGeneratedFrom(const char *s,bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"T‰m‰n "+s+ // "The documentation for this"+s+
                        " dokumentaatio on generoitu ";// " was generated from the following file";
      if (single) 
        result+="seuraavasta tiedostosta:";   
      else 
        result+="seuraavista tiedostoista:";
      return result;
    }

    QCString trMore()
      // this is the text of a link put after brief descriptions.
    { return "Lis‰‰..."; } // "More..."

    QCString trListOfAllMembers()
      // put in the class documentation
    { return "Lista kaikista j‰senist‰."; } // "List of all members."
    QCString trMemberList()

      // used as the title of the "list of all members" page of a class
    { return "J‰sen Lista"; } // "Member List"

    QCString trThisIsTheListOfAllMembers()
      // this is the first part of a sentence that is followed by a class name
    { return "T‰m‰ on lista kaikista j‰senist‰"; } // "This is the complete list of members for "
    QCString trIncludingInheritedMembers()
      // this is the remainder of the sentence after the class name
    { return ", sis‰lt‰‰ kaikki perityt j‰senet."; } // ", including all inherited members."

    QCString trGeneratedAutomatically(const char *s)
      // this is put at the author sections at the bottom of man pages.
      // parameter s is name of the project name.
    { QCString result="Automaattisesti generoitu Doxygen:lla "; // "Generated automatically by Doxygen"
      //if (s) result+=(QCString)" voor "+s;
      // t‰ss‰ on ongelma, kuinka taivuttaa parametria, esim. "Jcad"+"in"; "IFC2VRML konversio"+"n"
      // mutta ratkaistaan ongelma k‰tev‰sti kaksoispisteell‰ -> "Jcad:n" / "IFC2VRML konversio:n"
      // lopputulos on v‰hemm‰n kˆkkˆ ja t‰ysin luettava, mutta ei kuitenkaan t‰ydellinen.
      if (s) result+=(QCString)s+":n "; 
      result+="l‰hdekoodista."; // " from the source code."
      return result;
    }

    QCString trEnumName()
      // put after an enum name in the list of all members
    { return "enumeraation nimi"; } // "enum name"

    QCString trEnumValue()
      // put after an enum value in the list of all members
    { return "enumeraation arvo"; } // "enum value"

    QCString trDefinedIn()
      // put after an undocumented member in the list of all members
    { return "m‰‰ritelty"; } // "defined in"

    QCString trVerbatimText(const char *f)
      // put as in introduction in the verbatim header file of a class.
      // parameter f is the name of the include file.
    { return (QCString)"T‰m‰ on lyhennetty esitys "+f+" include tiedostosta."; } // "This is the verbatim text of the "+f+" include file."

    // quick reference sections
    QCString trModules()
      // This is put above each page as a link to the list of all groups of 
      // compounds or files (see the \group command).
    { return "Modulit"; } // "Modules"
    QCString trClassHierarchy()
      // This is put above each page as a link to the class hierarchy 
    { return "Luokka Hierarkia"; } // "Class Hierarchy"
    QCString trCompoundList()
      // This is put above each page as a link to the list of annotated classes
    { return "Kooste Lista"; } // "Compound List"
    QCString trFileList()
      // This is put above each page as a link to the list of documented files
    { return "Tiedosto Lista"; } // "File List"
    QCString trHeaderFiles()
      // This is put above each page as a link to the list of all verbatim headers
    { return "Header Tiedostot"; } // "Header Files"
    QCString trCompoundMembers()
      // This is put above each page as a link to all members of compounds.
    { return "Koosteen J‰senet"; } // "Compound Members"
    QCString trFileMembers()
      // This is put above each page as a link to all members of files.
    { return "Tiedosto J‰senet"; } // "File Members"
    QCString trRelatedPages()
      // This is put above each page as a link to all related pages.
    { return "Liittyv‰t Sivut"; } // "Related Pages"
    QCString trExamples()
      // This is put above each page as a link to all examples.
    { return "Esimerkit"; } // "Examples"
    QCString trSearch()
      // This is put above each page as a link to the search engine.
    { return "Etsi"; } // "Search"
    QCString trClassHierarchyDescription()
      // This is an introduction to the class hierarchy.
    { return "T‰m‰ periytymislista on p‰‰tasoltaan aakkostettu " // "This inheritance list is sorted roughly, "
             "mutta alij‰senet on aakkostettu itsen‰isesti:"; // "but not completely, alphabetically:";
    }
    QCString trFileListDescription(bool extractAll)
      // This is an introduction to the list with all files.
    {
      QCString result="T‰‰ll‰ on lista kaikista ";
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="tiedostoista lyhyen selitteen kera:"; // "files with brief descriptions:"
      return result;
    }
    QCString trCompoundListDescription()
      // This is an introduction to the annotated compound list
    { return "T‰‰ll‰ on luokat, struktuurit ja " // "Here are the classes, structs and "
             "unionit lyhyen selitteen kera:"; // "unions with brief descriptions:"
    }
    QCString trCompoundMembersDescription(bool extractAll)
      // This is an introduction to the page with all class members
    {
      QCString result="T‰‰ll‰ on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="luokan j‰senist‰ linkitettyn‰ "; // "class members with links to "
      if (!extractAll) 
        result+="jokaisen j‰senen luokkadokumentaatioon:"; // "the class documentation for each member:"
      else 
        result+="luokkaan johon ne kuuluvat:"; // "the classes they belong to:"
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
      // This is an introduction to the page with all file members
    {
      QCString result="T‰‰ll‰ on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented ";
      result+="tiedosto j‰senist‰ linkitettyn‰ "; // "file members with links to "
      if (extractAll) 
        result+="kunkin j‰senen tiedosto dokumentaatioon:"; // "the file documentation for each member:"
      else 
        result+="tiedostoon johon ne kuuluvat:"; // "the files they belong to:"
      return result;
    }
    QCString trHeaderFilesDescription()
      // This is an introduction to the page with the list of all header files
    { return "T‰‰ll‰ on kaikki header tiedostot jotka muodostavat API:n:"; } // "Here are the header files that make up the API:"
    QCString trExamplesDescription()
      // This is an introduction to the page with the list of all examples
    { return "T‰‰ll‰ on lista kaikista esimerkeist‰:"; } //  "Here is a list of all examples:"
    QCString trRelatedPagesDescription()
      // This is an introduction to the page with the list of related pages
    { return "T‰‰ll‰ on lista kaikista liittyvist‰ dokumentaatiosivuista:"; } // "Here is a list of all related documentation pages:"
    QCString trModulesDescription()
      // This is an introduction to the page with the list of class/file groups
    { return "T‰‰ll‰ on lista kaikista moduleista:"; } // "Here is a list of all modules:"
    QCString trNoDescriptionAvailable()
      // This sentences is used in the annotated class/file lists if no brief
      // description is given.
    { return "Selitett‰ ei ole saatavilla"; } // "No description available"

    // index titles (the project name is prepended for these) 
    QCString trDocumentation()
      // This is used in HTML as the title of index.html. 
    { return "Dokumentaatio"; } // "Documentation"
    QCString trModuleIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all groups.
    { return "Moduli Indeksi"; } // "Module Index"
    QCString trHierarchicalIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // class hierarchy.
    { return "Hierarkinen Indeksi"; } // "Hierarchical Index"
    QCString trCompoundIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // annotated compound index
    { return "Koosteiden Indeksi"; } // "Compound Index"
    QCString trFileIndex() 
      // This is used in LaTeX as the title of the chapter with the
      // list of all files.
    { return "Tiedosto Indeksi"; } // "File Index"
    QCString trModuleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all groups.
    { return "Moduli Dokumentaatio"; } // "Module Documentation"
    QCString trClassDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all classes, structs and unions.
    { return "Luokka Dokumentaatio"; } // "Class Documentation"
    QCString trFileDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all files.
    { return "Tiedosto Dokumentaatio"; } // "File Documentation"
    QCString trExampleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all examples.
    { return "Esimerkkien Dokumentaatio"; } // "Example Documentation"
    QCString trPageDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all related pages.
    { return "Sivujen Dokumentaatio"; } // "Page Documentation"
    QCString trReferenceManual()
      // This is used in LaTeX as the title of the document
    { return "K‰sikirja"; } // "Reference Manual"

    QCString trDefines()
      // This is used in the documentation of a file as a header before the 
      // list of defines
    { return "M‰‰rittelyt"; } // "Defines"
    QCString trFuncProtos()
      // This is used in the documentation of a file as a header before the 
      // list of function prototypes
    { return "Funktioiden Prototyypit"; } // "Function Prototypes"
    QCString trTypedefs()
      // This is used in the documentation of a file as a header before the 
      // list of typedefs
    { return "Tyyppim‰‰rittelyt"; } // "Typedefs"
    QCString trEnumerations()
      // This is used in the documentation of a file as a header before the 
      // list of enumerations
    { return "Enumeraatiot"; } // "Enumerations"
    QCString trFunctions()
      // This is used in the documentation of a file as a header before the 
      // list of (global) functions
    { return "Funktiot"; } // "Functions"
    QCString trVariables()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Muuttujat"; } // "Variables"
    QCString trEnumerationValues()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Enumeraation arvot"; } // "Enumeration values"

    QCString trAuthor()
      // This is used in man pages as the author section.
    { return "Tekij‰"; } // "Author"

    QCString trDefineDocumentation()
      // This is used in the documentation of a file before the list of
      // documentation blocks for defines
    { return "M‰‰ritysten dokumentointi"; } // "Define Documentation"
    QCString trFunctionPrototypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for function prototypes
    { return "Funktio Prototyyppien Dokumentaatio"; } // "Function Prototype Documentation"
    QCString trTypedefDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for typedefs
    { return "Tyyppim‰‰ritysten Dokumentaatio"; } // "Typedef Documentation"
    QCString trEnumerationTypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for enumeration types
    { return "Enumeraatio Tyyppien Dokumentaatio"; } // "Enumeration Type Documentation"
    QCString trFunctionDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for functions
    { return "Funktioiden Dokumentaatio"; } // "Function Documentation"
    QCString trVariableDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for variables
    { return "Muuttujien Dokumentaatio"; } // "Variable Documentation"
    QCString trCompounds()
      // This is used in the documentation of a file/namespace/group before 
      // the list of links to documented compounds
    { return "Koosteet"; } // "Compounds"
    QCString trFiles()
      // This is used in the documentation of a group before the list of 
      // links to documented files
    { return "Tiedostot"; } // "Files"

    QCString trGeneratedAt(const char *date,const char *projName)
    { 
        // funktio on hiukan vaikea k‰‰nt‰‰ prepositioihin sidotun rakenteen vuoksi.
      QCString result=(QCString)"Generoitu "+date; // "Generated at "
      if (projName) result+=(QCString)" projektille "+projName; // " for "
      result+=(QCString)" tekij‰: "; // " by"
      return result;
    }
    QCString trWrittenBy()
    {
      return "kirjoittanut"; // "written by"
    }
    QCString trClassDiagram(const char *clName)
      // this text is put before a class diagram
    {
      return (QCString)clName+":n Luokkakaavio"; // "Class diagram for "
    }

    QCString trForInternalUseOnly()
      // this text is generated when the \internal command is used.
    { return "Vain sis‰iseen k‰yttˆˆn."; } // "For internal use only."
    QCString trReimplementedForInternalReasons()
      // this text is generated when the \reimp command is used.
    { return "Uudelleen toteutettu sis‰isist‰ syist‰; API ei ole muuttunut."; } // "Reimplemented for internal reasons; the API is not affected."
    QCString trWarning()
      // this text is generated when the \warning command is used.
    { return "Varoitus"; } // "Warning"
      // this text is generated when the \bug command is used.
    QCString trBugsAndLimitations()
    { return "Virheet ja rajoitukset"; } // "Bugs and limitations"
    QCString trVersion()
      // this text is generated when the \version command is used.
    { return "Versio"; } // "Version"
    QCString trDate()
      // this text is generated when the \date command is used.
    { return "P‰iv‰ys"; } // "Date"
    QCString trAuthors()
      // this text is generated when the \author command is used.
    { return "Tekij‰(t)"; } // "Author(s)"
    QCString trReturns()
      // this text is generated when the \return command is used.
    { return "Palauttaa"; } // "Returns"
    QCString trSeeAlso()
      // this text is generated when the \sa command is used.
    { return "Katso myˆs"; } // "See also"
    QCString trParameters()
      // this text is generated when the \param command is used.
    { return "Parametrit"; } // "Parameters"
    QCString trExceptions()
      // this text is generated when the \exception command is used.
    { return "Poikkeukset"; } // "Exceptions"
    QCString trGeneratedBy()
      // this text is used in the title page of a LaTeX document.
    { return "Generoinut"; } // "Generated by"
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
      // used as the title of page containing all the index of all namespaces.
    { return "Nimiavaruus Lista"; } // "Namespace List"
    QCString trNamespaceListDescription(bool extractAll)
      // used as an introduction to the namespace list
    {
      QCString result="T‰‰ll‰ on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "Here is a list of all "
      result+="nimiavaruuksista lyhyen selitteen kera:"; // "namespaces with brief descriptions:"
      return result;
    }
    QCString trFriends()
      // used in the class documentation as a header before the list of all
      // friends of a class
    { return "Yst‰v‰t"; } // "Friends"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
      // used in the class documentation as a header before the list of all
      // related classes
    { return "Yst‰v‰t ja niihin Liittyvien Funktioiden Dokumentaatio"; } // "Friends And Related Function Documentation"
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Luokka"; break; // "Class"
        case ClassDef::Struct: result+=" Struct"; break; // "Struct"
        case ClassDef::Union:  result+=" Union"; break; // "Union"
        case ClassDef::Interface: result+=" Interface"; break; // "Interface"
        case ClassDef::Protocol: result+=" Protocol"; break; // translate me!
        case ClassDef::Category: result+=" Category"; break; // translate me!
        case ClassDef::Exception: result+=" Exception"; break; // "Interface"
      }
      result+=" Referenssi"; // " Reference"
      return result;
    }
    virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" Tiedosto Referenssi"; // " File Reference"
      return result;
    }
    virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Nimiavaruus Referenssi"; // " Namespace Reference"
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QCString trPublicMembers()
    { return "Julkiset J‰senet"; } // "Public Members"
    virtual QCString trPublicSlots()
    { return "Julkiset Slotit"; } // "Public Slots"
    virtual QCString trSignals()
    { return "Signaalit"; } // "Signals"
    virtual QCString trStaticPublicMembers()
    { return "Staattiset Julkiset J‰senet"; } // "Static Public Members"
    virtual QCString trProtectedMembers()
    { return "Suojatut J‰senet"; } // "Protected Members"
    virtual QCString trProtectedSlots()
    { return "Suojatut Slotit"; } // "Protected Slots"
    virtual QCString trStaticProtectedMembers()
    { return "Staattiset Suojatut J‰senet"; } // "Static Protected Members"
    virtual QCString trPrivateMembers()
    { return "Yksityiset J‰senet"; } // "Private Members"
    virtual QCString trPrivateSlots()
    { return "Yksityiset Slotit"; } // "Private Slots"
    virtual QCString trStaticPrivateMembers()
    { return "Staattiset Yksityiset J‰senet"; } // "Static Private Members"
    // end of member sections 
    
    virtual QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+=" ja "; // ", and "
        }
      }
      return result; 
    }
    
    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Perij‰t "+trWriteList(numEntries)+"."; // "Inherits "
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Periytyy "+trWriteList(numEntries)+"."; // "Inherited by "
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Uudelleen toteutettu "+trWriteList(numEntries)+"."; // "Reimplemented from "
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Uudelleen toteutettu "+trWriteList(numEntries)+"."; // "Reimplemented in "
    }

    virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Nimiavaruuden j‰senet"; } // "Namespace Members"
    virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="T‰‰ll‰ on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="nimiavaruuden j‰senist‰ linkitettyn‰ "; // "namespace members with links to "
      if (extractAll) 
        result+="nimiavaruuden dokumentaatioon johon ne kuuluvat:"; // "the namespace documentation for each member:";
      else 
        result+="nimiavaruuteen johon ne kuuluvat:"; // "the namespaces they belong to:"
      return result;
    }
    virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Nimiavaruuden Indeksi"; } // "Namespace Index"
    virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Nimiavaruuden Dokumentaatio"; } // "Namespace Documentation"
};

#endif
