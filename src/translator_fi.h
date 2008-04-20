/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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
Suomenkielinen käännös:
Olli Korhonen ( -> 0.49-990425)     <olli.korhonen@ccc.fi>
Antti Laine   (0.49-990425 -> )     <antti.a.laine@tut.fi>
==================================================================================
1999/10/19
*  Alustava käännös valmis.

*  Huom: Kaikille termeille on keksitty käännösvastine, mikä ei ehkä ole 
         kaikissa tapauksissa hyvä ratkaisu, ja parempikin termi voi 
         olla vakiintuneessa käytössä.

2008/04/08
* Käännetty versioon 1.5.5 asti.

* Tehtävää: 
        - Termien kokoaminen listaksi ja suomennosten järkevyyden tarkastelu. (lista on jo melko kattava)
                Author          = Tekijä
                Class           = Luokka
                Compound        = Kooste
                Data            = Data
                Documentation   = Dokumentaatio
                Defines         = Määrittelyt
                Description     = Selite
                Detailed        = Yksityiskohtainen
                Diagram         = Kaavio
                Enum            = Enumeraatio / Luettelotyyppi
                Exceptions      = Poikkeukset
                File            = Tiedosto
                Friends         = Ystävät
                Functions       = Funktiot
                Hierarchical    = Hierarkinen
                Index           = Indeksi
                Inherits        = Perii
                Member          = Jäsen
                Module          = Moduli
                Namespace       = Nimiavaruus
                Parameters      = Parametrit
                Private         = Yksityinen
                Protected       = Suojattu
                Prototypes      = Prototyypit
                Public          = Julkinen
                Reference Manual= Käsikirja
                Reimplemented   = Uudelleen toteutettu
                Related         = Liittyvä
                Signals         = Signaalit
                Slots           = Vastineet
                Static          = Staattinen
                Struct          = Tietue
                Typedef         = Tyyppimäärittely
                Union           = Yhdiste
                Variables       = Muuttujat
        - Taivutusmuotojen virheettömyyden tarkastelu prepositioiden korvauskohdissa.
        - Sanasta sanaan käännöskohtien mielekkyyden tarkastelu valmiista dokumentista.
        - umlaut vastineiden käyttö scandien kohdalla.

positiiviset kommentit otetaan ilolla vastaan.
===================================================================================
*/

#ifndef TRANSLATOR_FI_H
#define TRANSLATOR_FI_H

class TranslatorFinnish : public Translator
{
  public:
    /*! This method is used to generate a warning message to signal
     *  the user that the translation of his/her language of choice
     *  needs updating.
     */
    /*virtual QCString updateNeededMessage()
    {
      return "Warning: The Finnish translator is really obsolete.\n"
             "It was not updated since version 1.0.0.  As a result,\n"
             "some sentences may appear in English.\n\n";
    }*/
    
    // --- Language control methods -------------------
    
    /*! Used for identification of the language. The identification 
     * should not be translated. It should be replaced by the name 
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to 
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "finnish"; }

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
      return "\\usepackage[finnish]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-1";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Liittyvät funktiot"; } // "Related Functions";

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Huomaa, että nämä eivät ole jäsenfunktioita.)"; } // "(Note that these are not member functions.)"

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Yksityiskohtainen selite"; } // "Detailed Description"

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "Jäsentyyppimäärittelyiden dokumentaatio"; } // "Member Typedef Documentation"

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Jäsenenumeraatioiden dokumentaatio"; } // "Member Enumeration Documentation"

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Jäsenfunktioiden dokumentaatio"; } // "Member Function Documentation"

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Kenttien dokumentaatio"; // "Field Documentation"; 
      }
      else
      {
        return "Jäsendatan dokumentaatio"; // "Member Data Documentation"
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Lisää..."; } // "More..."

     /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista kaikista jäsenistä."; } // "List of all members."
    virtual QCString trMemberList()

    /*! used as the title of the "list of all members" page of a class */
    { return "Jäsenlista"; } // "Member List"

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Tämä on lista kaikista jäsenistä"; } // "This is the complete list of members for "

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", sisältäen kaikki perityt jäsenet."; } // ", including all inherited members."

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result=(QCString)"Automaattisesti generoitu Doxygenilla "
                      "lähdekoodista projektille "+s; // "Generated automatically by Doxygen" ... "for" ... "from the sourcecode"
      //if (s) result+=(QCString)" voor "+s;
      // tässä on ongelma, kuinka taivuttaa parametria, esim. "Jcad"+"in"; "IFC2VRML konversio"+"n"
      // mutta ratkaistaan ongelma kätevästi kaksoispisteellä -> "Jcad:n" / "IFC2VRML konversio:n"
      // lopputulos on vähemmän kökkö ja täysin luettava, mutta ei kuitenkaan täydellinen.
      //
      // kierretään ongelma taivuttamalla sanaa projekti :)
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enumeraation nimi"; } // "enum name"

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enumeraation arvo"; } // "enum value"

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "määritelty"; } // "defined in"

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Moduulit"; } // "Modules"
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Luokkahierarkia"; } // "Class Hierarchy"
   
    /*! This is put above each page as a link to the list of annotated classes */ 
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueet"; // "Data Structures"
      }
      else
      {
        return "Luokkalista"; // "Class List"
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Tiedostolista"; } // "File List"
   
    /*! This is put above each page as a link to all members of compounds. */ 
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueen kentät"; // "Data Fields" 
      }
      else
      {
        return "Luokan jäsenet"; // "Class Members"
      }
    }
    
    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globaalit"; // "Globals" 
      }
      else
      {
        return "Tiedoston jäsenet"; // "File Members"
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Liittyvät sivut"; } // "Related Pages"
   
    /*! This is put above each page as a link to all examples. */ 
    virtual QCString trExamples()
    { return "Esimerkit"; } // "Examples"
   
    /*! This is put above each page as a link to the search engine. */ 
    virtual QCString trSearch()
    { return "Etsi"; } // "Search"
   
    /*! This is an introduction to the class hierarchy. */ 
    virtual QCString trClassHierarchyDescription()
    { return "Tämä periytymislista on päätasoltaan aakkostettu " // "This inheritance list is sorted roughly, "
             "mutta alijäsenet on aakkostettu itsenäisesti:"; // "but not completely, alphabetically:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Tässä on lista kaikista ";
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="tiedostoista lyhyen selitteen kera:"; // "files with brief descriptions:"
      return result;
    }
    
    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
    
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tällä ovat tietueet lyhyen selitteen kera:"; // "Here are the data structures with brief descriptions:"
      }
      else
      {
        return "Tässä ovat luokat, tietueet ja " // "Here are the classes, structs and "
             "yhdisteet lyhyen selitteen kera:"; // "unions with brief descriptions:"
      }
    }
    
    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll)
      {
        result+="dokumentoiduista "; // "documented "
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="tietuiden ja yhdisteiden kentistä"; // "struct and union fields"
      }
      else
      {
        result+="luokkien jäsenistä"; // "class members"
      }
      result+=" linkitettyinä "; // " with links to "
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="jokaisen kentän tietueen/yhdisteen dokumentaatioon:"; // "the struct/union documentation for each field:"
        }
        else
        {
          result+="jokaisen jäsenen luokkadokumentaatioon:"; // "the class documentation for each member:"
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+= "tietueisiin/yhdisteisiin, joihin ne kuuluvat:"; // "the structures/unions they belong to:"
        }
        else
        {
          result+="luokkiin, joihin ne kuuluvat"; //"the classes they belong to:"
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="funktioista, muuttujista, määrittelyistä, luetteloista ja tyyppimäärittelyistä"; // "functions, variables, defines, enums, and typedefs"
      }
      else
      {
        result+="tiedoston jäsenistä"; // "file members"
      }
      result+=" linkitettyinä "; // " with links to "
      if (extractAll) 
        result+="tiedostoon, johon ne kuuluvat:"; // "the files they belong to:"
      else 
        result+="dokumentaatioon:"; // "the documentation:"
      return result;
    }
    
    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Tässä on lista kaikista esimerkeistä:"; } //  "Here is a list of all examples:"
   
    /*! This is an introduction to the page with the list of related pages */ 
    virtual QCString trRelatedPagesDescription()
    { return "Tässä on lista kaikista liittyvistä dokumentaatiosivuista:"; } // "Here is a list of all related documentation pages:"
   
    /*! This is an introduction to the page with the list of class/file groups */ 
    virtual QCString trModulesDescription()
    { return "Tässä on lista kaikista moduleista:"; } // "Here is a list of all modules:"
    
    // index titles (the project name is prepended for these) 
    
    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentaatio"; } // "Documentation"
    
    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Moduuliluettelo"; } // "Module Index"
    
    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hierarkinen luettelo"; } // "Hierarchical Index"
    
    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Tietueluettelo"; // "Data Structure Index"
      }
      else
      {
        return "Luokkaluettelo"; // "Class Index"
      }
    }
    
    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Tiedostoluettelo"; } // "File Index"
    
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Moduulien dokumentaatio"; } // "Module Documentation"
    
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueiden dokumentaatio"; // "Data Structure Documentation"
      }
      else
      {
        return "Luokkien dokumentaatio"; // "Class Documentation"
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Tiedostojen dokumentaatio"; } // "File Documentation"
    
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Esimerkkien dokumentaatio"; } // "Example Documentation"
    
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Sivujen dokumentaatio"; } // "Page Documentation"
    
    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Käsikirja"; } // "Reference Manual"

    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Määrittelyt"; } // "Defines"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Funktioiden prototyypit"; } // "Function Prototypes"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Tyyppimäärittelyt"; } // "Typedefs"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Luettelotyypit"; } // "Enumerations"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funktiot"; } // "Functions"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Muuttujat"; } // "Variables"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Luettelotyyppien arvot"; } // "Enumerator"

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Määritysten dokumentointi"; } // "Define Documentation"
    
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Funktioprototyyppien dokumentaatio"; } // "Function Prototype Documentation"
    
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Tyyppimääritysten dokumentaatio"; } // "Typedef Documentation"
    
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Luettelotyyppien dokumentaatio"; } // "Enumeration Type Documentation"
    
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funktioiden dokumentaatio"; } // "Function Documentation"
    
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Muuttujien dokumentaatio"; } // "Variable Documentation"
    
    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueet"; // "Data Structures"
      }
      else
      {
        return "Luokat"; // "Classes"
      }
    }
    
    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
        // funktio on hiukan vaikea kääntää prepositioihin sidotun rakenteen vuoksi.
        // Muutetaan siis lauserakennetta suomalaisemmaksi
        // Generated on $date for $project by:
        // -> Generated for $project on $date by:
      QCString result=(QCString)"Generoinut ";
      if (projName) result+=(QCString)"projektille "+projName+" ";
      result+=(QCString)date+" ";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "kirjoittanut"; // "written by"
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return "Luokan "+(QCString)clName+" luokkakaavio"; // "Inheritance diagram for "
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Vain sisäiseen käyttöön."; } // "For internal use only."
    
    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Varoitus"; } // "Warning"
    
    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versio"; } // "Version"
    
    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Päiväys"; } // "Date"
    
    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Palauttaa"; } // "Returns"
    
    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Katso myös"; } // "See also"
    
    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametrit"; } // "Parameters"
    
    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Poikkeukset"; } // "Exceptions"
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generoinut"; } // "Generated by"
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Nimiavaruus Lista"; } // "Namespace List"
    
    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="nimiavaruuksista lyhyen selitteen kera:"; // "namespaces with brief descriptions:"
      return result;
    }
    
    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Ystävät"; } // "Friends"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Ystävät ja niihin liittyvien funktioiden dokumentaatio"; } // "Friends And Related Function Documentation"
    
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
        case ClassDef::Class:      result+=" Luokka"; break; // " Class"
        case ClassDef::Struct:     result+=" Tietue"; break; // " Struct"
        case ClassDef::Union:      result+=" Yhdiste"; break; // " Union"
        case ClassDef::Interface:  result+=" Rajapinta"; break; // " Interface"
        case ClassDef::Protocol:   result+=" Protokolla"; break; // " Protocol"
        case ClassDef::Category:   result+=" Kategoria"; break; // " Category"
        case ClassDef::Exception:  result+=" Poikkeus"; break; // " Exception"
      }
      if (isTemplate) result+="malli"; // " Template"
      result+="referenssi"; // " Reference"
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Tiedostoreferenssi"; // " File Reference"
      return result;
    }
    
    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Nimiavaruusreferenssi"; // " Namespace Reference"
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Julkiset jäsenfunktiot"; } // "Public Member Functions"
    virtual QCString trPublicSlots()
    { return "Julkiset vastineet"; } // "Public Slots"
    virtual QCString trSignals()
    { return "Signaalit"; } // "Signals"
    virtual QCString trStaticPublicMembers()
    { return "Staattiset julkiset jäsenfunktiot"; } // "Static Public Member Functions"
    virtual QCString trProtectedMembers()
    { return "Suojatut jäsenfunktiot"; } // "Protected Member Functions"
    virtual QCString trProtectedSlots()
    { return "Suojatut vastineet"; } // "Protected Slots"
    virtual QCString trStaticProtectedMembers()
    { return "Staattiset suojatut jäsenfunktiot"; } // "Static Protected Member Functions"
    virtual QCString trPrivateMembers()
    { return "Yksityiset jäsenfunktiot"; } // "Private Member Functions"
    virtual QCString trPrivateSlots()
    { return "Yksityiset vastineet"; } // "Private Slots"
    virtual QCString trStaticPrivateMembers()
    { return "Staattiset yksityiset jäsenfunktiot"; } // "Static Private Member Functions"

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
            result+=" ja "; // ", and "
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Perijät "+trWriteList(numEntries)+"."; // "Inherits "
    }
    
    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Periytyy "+trWriteList(numEntries)+"."; // "Inherited by "
    }
    
    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Uudelleen toteutettaa "+trWriteList(numEntries)+"."; // "Reimplemented from "
    }
    
    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Uudelleen toteutettu "+trWriteList(numEntries)+"."; // "Reimplemented in "
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Nimiavaruuden jäsenet"; } // "Namespace Members"
    
    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="nimiavaruuden jäsenistä linkitettynä "; // "namespace members with links to "
      if (extractAll) 
        result+="nimiavaruuden dokumentaatioon johon ne kuuluvat:"; // "the namespace documentation for each member:";
      else 
        result+="nimiavaruuksiin joihin ne kuuluvat:"; // "the namespaces they belong to:"
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */ 
    virtual QCString trNamespaceIndex()
    { return "Nimiavaruuksien luettelo"; } // "Namespace Index"
    
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Nimiavaruuden dokumentaatio"; } // "Namespace Documentation"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Nimiavaruudet"; } // "Namespaces"; }

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
      QCString result=(QCString)"Dokumentaatio tälle "; // "The documentation for this "
      switch(compType)
      {
        case ClassDef::Class:      result+="luokalle"; break; // "class"
        case ClassDef::Struct:     result+="tietueelle"; break; // "struct"
        case ClassDef::Union:      result+="yhdisteelle"; break; // "union"
        case ClassDef::Interface:  result+="rajapinnalle"; break; // "interface"
        case ClassDef::Protocol:   result+="protokollalle"; break; // "protocol"
        case ClassDef::Category:   result+="kategorialle"; break; // "category"
        case ClassDef::Exception:  result+="poikkeukselle"; break; // "exception"
      }
      if (single) result+=" generoitiin seuraavasta tiedostosta:"; // " was generated from the following file"
      else result+=" generoitiin seuraavista tiedostoista:"; // ":" or "s:"
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Aakkosellinen lista"; } // "Alphabetical List"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Paluuarvot"; } // "Return values"

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Pääsivu"; } // "Main Page"

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "s."; } // "p."

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Määrittely tiedoston @1 rivillä @0."; //  "Definition at line @0 of file @1."
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Määrittely tiedostossa @0."; // "Definition in file @0."
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Vanhentunut"; // "Deprecated"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Yhteistyökaavio luokalle "+clName+":"; // "Collaboration diagram for "+clName+":"
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Sisällytysriippuvuuskaavio tiedostolle "+fName+":"; // "Include dependency graph for "+fName+":"
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Rakentajien & purkajien dokumentaatio"; // "Constructor & Destructor Documentation";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Siirry tämän tiedoston lähdekoodiin."; // "Go to the source code of this file."
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Mene tämän tiedoston dokumentaatioon."; // "Go to the documentation of this file."
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Esiehto"; //"Precondition"
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Jälkiehto"; // "Postcondition"
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariantti"; // vai "Pysyväisväittämä"? "Invariant"
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Alkuarvo:"; // "Initial value:"
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "koodi"; // "code"
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Graafinen luokkahierarkia"; // "Graphical Class Hierarchy"
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Siirry graafiseen luokkahiearkiaan"; // "Go to the graphical class hierarchy"
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Siirry tekstimuotoiseen luokkahierarkiaan"; // "Go to the textual class hierarchy"
    }
    virtual QCString trPageIndex()
    {
      return "Sivuhakemisto"; // "Page Index"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
        // FIXME: Missähän merkityksessä tätä käytetään?
      return "Huomautus"; // "Note"
    }
    virtual QCString trPublicTypes()
    {
      return "Julkiset tyypit"; // "Public Types"
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueen kentät"; // "Data Fields"
      }
      else
      {
        return "Julkiset attribuutit"; // "Public Attributes"
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Staattiset julkiset attribuutit"; // "Static Public Attributes"
    }
    virtual QCString trProtectedTypes()
    {
      return "Suojellut tyypit"; // "Protected Types"
    }
    virtual QCString trProtectedAttribs()
    {
      return "Suojellut attribuutit"; // "Protected Attributes"
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Stattiset suojellut attribuutit"; // "Static Protected Attributes"
    }
    virtual QCString trPrivateTypes()
    {
      return "Yksityiset tyypit"; // "Private Types"
    }
    virtual QCString trPrivateAttribs()
    {
      return "Yksityiset attribuutit"; // "Private Attributes"
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Staattiset yksityiset attribuutit"; // "Static Private Attributes"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Vielä tehtävä"; // "Todo"
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Tehtävälista"; // "Todo List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Viitattu"; // "Referenced by"
    }
    virtual QCString trRemarks()
    {
      return "Huomioita"; // "Remarks"
    }
    virtual QCString trAttention()
    {
      return "Huomio"; // "Attention"
    }
    virtual QCString trInclByDepGraph()
    {
      return "Tämä kaavio näyttää, mitkä tiedostot suorasti"
             "tai epäsuorasti sisällyttävät tämän tiedoston";
             // "This graph shows which files directly or "
             // "indirectly include this file:"
    }
    virtual QCString trSince()
    {
        // FIXME: Missä merkityksessä tätä käytetään?
      return "Lähtien"; // "Since"
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Kaavion selite"; // "Graph Legend"
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "Tämä sivu selittää, kuinka doxygenin generoimia kaavioita tulkitaan.<p>\n"
        // "This page explains how to interpret the graphs that are generated "
        // "by doxygen.<p>\n"
        "Ajattele seuraavaa esimerkkiä:\n"
        // "Consider the following example:\n"
        "\\code\n"
        "/*! Näkymätön luokka katkaisun vuoksi */\n"
        // "/*! Invisible class because of truncation */\n"
        "class Nakymaton { };\n\n"
        // "class Invisible { };\n\n"
        "/*! Katkaistu luokka, periytymissuhde on piilotettu */\n"
        // "/*! Truncated class, inheritance relation is hidden */\n"
        "class Katkaistu : public Nakymaton { };\n\n"
        // "class Truncated : public Invisible { };\n\n"
        "/* Luokkaa ei ole dokumentoitu doxygen-kommenteilla */\n"
        // "/* Class not documented with doxygen comments */\n"
        "class Dokumentoimaton { };\n\n"
        // "class Undocumented { };\n\n"
        "/*! Julkista periyttämistä käyttäen periytetty luokka */\n"
        // "/*! Class that is inherited using public inheritance */\n"
        "class JulkinenKanta : public Katkaistu { };\n\n"
        // "class PublicBase : public Truncated { };\n\n"
        "/*! Malliluokka */\n"
        // "/*! A template class */\n"
        "template<class T> class Malli { };\n\n"
        // "template<class T> class Templ { };\n\n"
        "/*! Suojeltua periytymistä käyttäen periytetty luokka */\n"
        // "/*! Class that is inherited using protected inheritance */\n"
        "class SuojeltuKanta { };\n\n"
        // "class ProtectedBase { };\n\n"
        "/*! Yksityistä periytymistä käyttäen periytetty luokka */\n"
        // "/*! Class that is inherited using private inheritance */\n"
        "class YksityisKanta { };\n\n"
        // "class PrivateBase { };\n\n"
        "/*! Luokka jota periytetty luokka käyttää */\n"
        // "/*! Class that is used by the Inherited class */\n"
        "class Kaytetty { };\n\n"
        // "class Used { };\n\n"
        "/*! Kantaluokka joka periytyy useasta muusta luokasta */\n"
        // "/*! Super class that inherits a number of other classes */\n"
        "class Periytetty : public JulkinenKanta,\n"
        "                 : protected SuojeltuKanta,\n"
        "                 : private YksityisKanta,\n"
        "                 : public Dokumentoimaton,\n"
        "                 : public Malli<int>\n"
        "{\n"
        "  private:\n"
        "    Kaytetty *m_kaytettyLuokka;\n"
        "}\n";
        // "class Inherited : public PublicBase,\n"
        // "                  protected ProtectedBase,\n"
        // "                  private PrivateBase,\n"
        // "                  public Undocumented,\n"
        // "                  public Templ<int>\n"
        // "{\n"
        // "  private:\n"
        // "    Used *m_usedClass;\n"
        // "};\n"
        "\\endcode\n"
        "Jos \\c MAX_DOT_GRAPH_HEIGHT-kenttä asetustiedostossa "
        // "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "on asetettu arvoon 240, on tuloksena seuraavanlainen kaavio:"
        // "is set to 240 this will result in the following graph:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Ylläolevassa kaaviossa laatikoilla on seuraavat merkitykset\n:"
        // "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>Täytetty harmaa laatikko esittää tietuetta tai luokkaa, jolle "
        // "<li>%A filled gray box represents the struct or class for which the "
        "kaavio on generoitu.\n"
        // "graph is generated.\n"
        "<li>Mustareunainen laatikko merkitsee dokumentoitua tietuetta tai luokkaa.\n"
        // "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>Harmaareunainen laatikko merkitsee dokumentoimatonta tietuetta tai luokkaa.\n"
        // "<li>%A box with a grey border denotes an undocumented struct or class.\n"
        "<li>Punareunainen laatikko merkistee dokumentoitua luokkaa tai structia "
        // "<li>%A box with a red border denotes a documented struct or class for"
        "jolle ei näytetä kaikkia periytymis-/sisällyssuhteita. Kaavio "
        // "which not all inheritance/containment relations are shown. %A graph is "
        "katkaistaan, jos se ei mahdu määriteltyjen rajojen sisään.\n"
        // "truncated if it does not fit within the specified boundaries.\n"
        "</ul>\n"
        "Nuolilla on seuraavat merkitykset:\n"
        // "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>Tummansinistä nuolta käytetään osoittamaan julkista periytymis"
        // "<li>%A dark blue arrow is used to visualize a public inheritance "
        "suhdetta kahden luokan välillä.\n"
        // "relation between two classes.\n"
        "<li>Tummanvihreää nuolta käytetään suojellussa periytymisessä.\n"
        // "<li>%A dark green arrow is used for protected inheritance.\n"
        "<li>Tummanpunaista nuolta käytetään yksityisessä periytymisessä.\n"
        // "<li>%A dark red arrow is used for private inheritance.\n"
        "<li>Purppuranväristä katkoviivaa käytetään, jos luokka sisältyy tai "
        // "<li>%A purple dashed arrow is used if a class is contained or used "
        "on toisen luokan käyttämä. Nuoli nimetään sillä muuttujalla/muuttujilla "
        // "by another class. The arrow is labeled with the variable(s) "
        "jonka läpi osoitettua luokkaa tai tietuetta käytetään.\n"
        // "through which the pointed class or struct is accessible.\n"
        "<li>Keltainen katkoviivalla piirretty nuoli merkitsee suhdetta mallin esiintymän ja "
        // "<li>%A yellow dashed arrow denotes a relation between a template instance and "
        "malliluokan välillä. Nuoli nimetään "
        // "the template class it was instantiated from. The arrow is labeled with "
        "mallin esiintymän malliparametrilla.\n"
        // "the template parameters of the instance.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "selite"; // "legend"
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Testi"; // "Test"
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Testilista"; // "Test List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP-jäsenfunktiot"; // "DCOP Member Functions"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Ominaisuudet"; // "Properties"
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Ominaisuuksien dokumentaatio"; // "Property Documentation"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Tietueet"; // "Data Structures"
      }
      else
      {
        return "Luokat"; // "Classes"
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paketti "+name; // "Package "
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Pakettilista"; // "Package List"
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Tässä ovat paketit lyhyiden selitysten kanssa (jos saatavilla):"; // "Here are the packages with brief descriptions (if available):"
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paketit"; // "Packages"
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Arvo:"; // "Value:"
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Bugi"; // "Bug"
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Bugilista"; // "Bug List"
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
      return "1252";
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
      return "Hakemisto"; // "Index"
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Luokka" : "luokka")); // "Class" / "class"
      if (!singular)  result=(first_capital ? "Luokat" : "luokat"); // "+es" -> "Classes" / "classes"
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tiedosto" : "tiedosto")); // "File" / "file"
      if (!singular)  result+="t"; // "+s" -> "Files" / "files"
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Nimiavaruus" : "nimiavaruus")); // "Namespace" / "namespace"
      if (!singular)  result=(first_capital ? "Nimiavaruudet" : "nimiavaruudet"); // "+s"
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Ryhmä" : "ryhmä")); // "Group" / "group"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Sivu" : "sivu")); // "Page" / "page"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Jäsen" : "jäsen")); // "Member" / "member"
      if (!singular)  result+="et"; // "+s"
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Globaali" : "globaali")); // "Global" / "global"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Tekijä" : "tekijä")); // "Author" / "author"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Viittaukset"; // "References"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Toteuttaa "+trWriteList(numEntries)+"."; // "Implements "
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Toteutettu "+trWriteList(numEntries)+"."; // "Implemented in "
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Sisällysluettelo"; // "Table of Contents"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Vanhentuneiden lista"; // "Deprecated List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Tapahtumat"; // "Events"
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Tapahtumien dokumentaatio"; // "Event Documentation"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Paketin tyypit"; // "Package Types"
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Paketin funktiot"; // "Package Functions"
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Paketin staattiset funktiot"; // "Static Package Functions"
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Paketin attribuutit"; // "Package Attributes"
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Paketin staattiset attribuutit"; // "Static Package Attributes"
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Kaikki"; // "All"
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Tässä on kutsukaavio tälle funktiolle:"; // "Here is the call graph for this function:"
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
      return "Etsi"; // "Search for"
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Hakutulokset"; // "Search Results"
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
        return "Valitettavasti yksikään dokumentti ei vastannut hakuasi."; // "Sorry, no documents matching your query."
      }
      else if (numDocuments==1)
      {
        return "Löytyi <b>1</b> dokumentti, joka vastasi hakuasi."; // "Found <b>1</b> document matching your query.";
      }
      else 
      {
        return "Löytyi <b>$num dokumenttia, jotka vastasivat hakuasi. " // "Found <b>$num</b> documents matching your query. "
                "Parhaat tulokset näytetään ensin."; // "Showing best matches first."
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Osumat:"; // "Matches:"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " lähdekooditiedosto"; // " Source File"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Hakemistohierarkia"; } // "Directory Hierarchy"

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Hakemistojen dokumentaatio"; } // "Directory Documentation"

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Hakemistot"; } // "Directories"

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Tämä hakemistohierarkia on järjestetty aakkosellisesti tasoittain:";
             //This directory hierarchy is sorted roughly, "
             // "but not completely, alphabetically:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Hakemistoreferenssi"; return result; }
        // " Directory Reference"

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Hakemisto" : "hakemisto")); // "Director" / "director"
      if (singular) result+=""; else result+="t"; // "+y" / "+ies"
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
       return "Tämä on ylikuormitettu jäsenfunktio, ja se tarjotaan "
              "käytön helpottamiseksi. Se eroaa ylläolevasta "
              "funktiosta ainoastaan hyväksymiltä parametreiltaan.";
              // "This is an overloaded member function, "
              // "provided for convenience. It differs from the above "
              // "function only in what argument(s) it accepts."
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Tässä on kutsukaavio tälle funktiolle:"; // "Here is the caller graph for this function:"
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Luettelotyypin dokumentaatio"; } // "Enumerator Documentation"

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Jäsenfunktioiden/aliohjelmien dokumentaatio"; } // "Member Function/Subroutine Documentation"

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Tietotyyppien lista"; } // "Data Types List"

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Kentät"; } // "Data Fields";

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Tässä ovat tietotyypit lyhyillä selityksillä:"; } // "Here are the data types with brief descriptions:"

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll)
      {
        result+="dokumentoiduista "; // "documented "
      }
      result+="tietotyyppien jäsenistä"; // "data types members"
      result+=", sekä linkit "; // " with links to "
      if (!extractAll) 
      {
         result+="tietueen dokumentaatioon jokaiselle jäsenelle"; // "the data structure documentation for each member"
      }
      else 
      {
         result+="tietotyyppeihin, joihin ne kuuluvat:"; // "the data types they belong to:"
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Tietotyyppien hakemisto"; } // "Data Type Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Tietotyyppien dokumentaatio"; } // "Data Type Documentation"

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funktiot/aliohjelmat"; } // "Functions/Subroutines"

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Funktioiden/aliohjelmien dokumentaatio"; } // "Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tietotyypit"; } // "Data Types"
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Moduulilista"; } // "Modules List"

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="moduuleista lyhillä selityksillä:"; // "modules with brief descriptions:"
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
        case ClassDef::Class:      result+=" Moduuli"; break; // " Module"
        case ClassDef::Struct:     result+=" Tyyppi"; break; // " Type"
        case ClassDef::Union:      result+=" Yhdiste"; break; // " Union"
        case ClassDef::Interface:  result+=" Rajapinta"; break; // " Interface"
        case ClassDef::Protocol:   result+=" Protokolla"; break; // " Protocol"
        case ClassDef::Category:   result+=" Kategoria"; break; // " Category"
        case ClassDef::Exception:  result+=" Poikkeus"; break; // " Exception"
      }
      if (isTemplate) result+="malli"; // " Template"
      result+="referenssi"; // " Reference"
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Moduulin referenssi"; // " Module Reference"
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Moduulin jäsenet"; } // "Module Members"

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Tässä on lista moduulin kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista"; // "documented "
      result+="jäsenistä, sekä linkit "; // "module members with links to "
      if (extractAll) 
      {
        result+="moduulin dokumentaatioon jokaiselle jäsenelle:"; // "the module documentation for each member:"
      }
      else 
      {
        result+="moduuleihin, joihin ne kuuluvat:"; // "the modules they belong to:"
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Moduulien hakemisto"; } // "Modules Index"
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Moduuli" : "moduuli")); // "Module" / "module"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentaatio tälle "; // "The documentation for this "
      switch(compType)
      {
        case ClassDef::Class:      result+="moduulille"; break; // "module"
        case ClassDef::Struct:     result+="tyypille"; break; // "type"
        case ClassDef::Union:      result+="yhdisteelle"; break; // "union"
        case ClassDef::Interface:  result+="rajapinnalle"; break; // "interface"
        case ClassDef::Protocol:   result+="protokollalle"; break; // "protocol"
        case ClassDef::Category:   result+="kategorialle"; break; // "category"
        case ClassDef::Exception:  result+="poikkeukselle"; break; // "exception"
      }
      result+=" generoitiin "; // " was generated from the following file"
      if (single) result+="seuraavasta tiedostosta:"; else result+="seuraavista tiedostoista:"; // ":" / "s:"
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tyyppi" : "tyyppi")); // "Type" / "type"
      if (!singular)  result=(first_capital ? "Tyypit" : "tyypit"); // "+s"
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Aliohjelma" : "aliohjelma")); // "Subprogram" / "subprogram"
      if (!singular)  result+="t"; // "+s"
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Tyyppien rajoitteet"; // "Type Constraints"
    }

};

#endif
