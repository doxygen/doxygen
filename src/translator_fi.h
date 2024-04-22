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

2008-04-08
* Käännetty versioon 1.5.5 asti.
2008-08-26
* MAX_DOT_GRAPH_HEIGHT poistettu, versio 1.5.6
* Tekstit muutettu UTF-8:ksi
2009-01-17
* versio 1.5.8
* Kantaluokka vaihdettu TranslatorEnglishistä Translatoriksi
* Enumeraatio -> luettelotyyppi
* Paranneltu kieltä ja poistettu kirjoitusvirheitä

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
                Enum            = Luettelotyyppi
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

class TranslatorFinnish : public TranslatorAdapter_1_6_0
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
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[finnish]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "fi";
    }
    QCString getLanguageString() override
    {
      return "0x40B Finnish";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Liittyvät funktiot"; } // "Related Functions";

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Huomaa, että nämä eivät ole jäsenfunktioita.)"; } // "(Note that these are not member functions.)"

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Yksityiskohtainen selite"; } // "Detailed Description"

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Yksityiskohdat"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
      // header that is put before the list of typedefs.
    { return "Jäsentyyppimäärittelyiden dokumentaatio"; } // "Member Typedef Documentation"

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Jäsenluettelotyyppien dokumentaatio"; } // "Member Enumeration Documentation"

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Jäsenfunktioiden dokumentaatio"; } // "Member Function Documentation"

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Kenttien dokumentaatio"; // "Field Documentation";
      }
      else
      {
        return "Jäsendatan dokumentaatio"; // "Member Data Documentation"
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Lisää..."; } // "More..."

     /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista kaikista jäsenistä"; } // "List of all members."
    QCString trMemberList() override

    /*! used as the title of the "list of all members" page of a class */
    { return "Jäsenlista"; } // "Member List"

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Tämä on lista kaikista jäsenistä luokassa"; } // "This is the complete list of members for "

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", sisältäen kaikki perityt jäsenet."; } // ", including all inherited members."

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automaattisesti luotu Doxygenilla "
                      "lähdekoodista projektille "+s; // "Generated automatically by Doxygen" ... "for" ... "from the sourcecode"
      //if (s) result+=" voor "+s;
      // tässä on ongelma, kuinka taivuttaa parametria, esim. "Jcad"+"in"; "IFC2VRML konversio"+"n"
      // mutta ratkaistaan ongelma kätevästi kaksoispisteellä -> "Jcad:n" / "IFC2VRML konversio:n"
      // lopputulos on vähemmän kökkö ja täysin luettava, mutta ei kuitenkaan täydellinen.
      //
      // kierretään ongelma taivuttamalla sanaa projekti :)
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "luettelotyypin nimi"; } // "enum name"

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "luettelotyypin arvo"; } // "enum value"

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "määritelty"; } // "defined in"

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Moduulit"; } // "Modules"

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Luokkahierarkia"; } // "Class Hierarchy"

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tietueet"; // "Data Structures"
      }
      else
      {
        return "Luokkalista"; // "Class List"
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Tiedostolista"; } // "File List"

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tietueen kentät"; // "Data Fields"
      }
      else
      {
        return "Luokan jäsenet"; // "Class Members"
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globaalit"; // "Globals"
      }
      else
      {
        return "Tiedoston jäsenet"; // "File Members"
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Liittyvät sivut"; } // "Related Pages"

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Esimerkit"; } // "Examples"

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Etsi"; } // "Search"

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Tämä periytymislista on päätasoltaan aakkostettu " // "This inheritance list is sorted roughly, "
             "mutta alijäsenet on aakkostettu itsenäisesti:"; // "but not completely, alphabetically:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Tässä on lista kaikista ";
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="tiedostoista lyhyen selitteen kanssa:"; // "files with brief descriptions:"
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tässä ovat tietueet lyhyen selitteen kanssa:"; // "Here are the data structures with brief descriptions:"
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Tässä ovat luokat " // "Here are the classes
             "lyhyen selitteen kanssa:"; // "with brief descriptions:"
      }
      else
      {
        return "Tässä ovat luokat, tietueet ja " // "Here are the classes, structs and "
             "yhdisteet lyhyen selitteen kanssa:"; // "unions with brief descriptions:"
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll)
      {
        result+="dokumentoiduista "; // "documented "
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="funktioista, muuttujista, määrittelyistä, luettelotyypeistä ja tyyppimäärittelyistä"; // "functions, variables, defines, enums, and typedefs"
      }
      else
      {
        result+="tiedoston jäsenistä"; // "file members"
      }
      result+=" linkitettyinä "; // " with links to "
      if (extractAll)
        result+="tiedostoihin, joihin ne kuuluvat:"; // "the files they belong to:"
      else
        result+="dokumentaatioon:"; // "the documentation:"
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Tässä on lista kaikista esimerkeistä:"; } //  "Here is a list of all examples:"

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Tässä on lista kaikista liittyvistä dokumentaatiosivuista:"; } // "Here is a list of all related documentation pages:"

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Tässä on lista kaikista moduleista:"; } // "Here is a list of all modules:"

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentaatio"; } // "Documentation"

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Moduuliluettelo"; } // "Module Index"

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Hierarkinen luettelo"; } // "Hierarchical Index"

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Tiedostoluettelo"; } // "File Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Moduulien dokumentaatio"; } // "Module Documentation"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tietueiden dokumentaatio"; // "Data Structure Documentation"
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Luokkien dokumentaatio"; // "Class Documentation"
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Tiedostojen dokumentaatio"; } // "File Documentation"

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Käsikirja"; } // "Reference Manual"

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Määrittelyt"; } // "Defines"

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Tyyppimäärittelyt"; } // "Typedefs"

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Luettelotyypit"; } // "Enumerations"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funktiot"; } // "Functions"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Muuttujat"; } // "Variables"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Luettelotyyppien arvot"; } // "Enumerator"

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Määritysten dokumentointi"; } // "Define Documentation"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Tyyppimääritysten dokumentaatio"; } // "Typedef Documentation"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Luettelotyyppien dokumentaatio"; } // "Enumeration Type Documentation"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Funktioiden dokumentaatio"; } // "Function Documentation"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Muuttujien dokumentaatio"; } // "Variable Documentation"

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
        // funktio on hiukan vaikea kääntää prepositioihin sidotun rakenteen vuoksi.
        // Muutetaan siis lauserakennetta suomalaisemmaksi
        // Generated on $date for $project by:
        // -> Generated for $project on $date by:
      QCString result="Generoinut ";
      if (!projName.isEmpty()) result+="projektille "+projName+" ";
      result+=date+" ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Luokan "+clName+" luokkakaavio"; // "Inheritance diagram for "
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Varoitus"; } // "Warning"

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versio"; } // "Version"

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Päiväys"; } // "Date"

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Palauttaa"; } // "Returns"

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Katso myös"; } // "See also"

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parametrit"; } // "Parameters"

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Poikkeukset"; } // "Exceptions"

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generoinut"; } // "Generated by"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Nimiavaruus Lista"; } // "Namespace List"

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="nimiavaruuksista lyhyen selitteen kanssa:"; // "namespaces with brief descriptions:"
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Ystävät"; } // "Friends"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Ystävät ja niihin liittyvien funktioiden dokumentaatio"; } // "Friends And Related Function Documentation"

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
        case ClassDef::Class:      result+=" luokka"; break; // " Class"
        case ClassDef::Struct:     result+=" tietue"; break; // " Struct"
        case ClassDef::Union:      result+=" yhdiste"; break; // " Union"
        case ClassDef::Interface:  result+=" rajapinta"; break; // " Interface"
        case ClassDef::Protocol:   result+=" protokolla"; break; // " Protocol"
        case ClassDef::Category:   result+=" kategoria"; break; // " Category"
        case ClassDef::Exception:  result+=" poikkeus"; break; // " Exception"
        default: break;
      }
      if (isTemplate) result+="malli"; // " Template"
      result+="referenssi"; // " Reference"
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" tiedostoreferenssi"; // " File Reference"
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" nimiavaruusreferenssi"; // " Namespace Reference"
      return result;
    }

    QCString trPublicMembers() override
    { return "Julkiset jäsenfunktiot"; } // "Public Member Functions"
    QCString trPublicSlots() override
    { return "Julkiset vastineet"; } // "Public Slots"
    QCString trSignals() override
    { return "Signaalit"; } // "Signals"
    QCString trStaticPublicMembers() override
    { return "Staattiset julkiset jäsenfunktiot"; } // "Static Public Member Functions"
    QCString trProtectedMembers() override
    { return "Suojatut jäsenfunktiot"; } // "Protected Member Functions"
    QCString trProtectedSlots() override
    { return "Suojatut vastineet"; } // "Protected Slots"
    QCString trStaticProtectedMembers() override
    { return "Staattiset suojatut jäsenfunktiot"; } // "Static Protected Member Functions"
    QCString trPrivateMembers() override
    { return "Yksityiset jäsenfunktiot"; } // "Private Member Functions"
    QCString trPrivateSlots() override
    { return "Yksityiset vastineet"; } // "Private Slots"
    QCString trStaticPrivateMembers() override
    { return "Staattiset yksityiset jäsenfunktiot"; } // "Static Private Member Functions"

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
            result+=" ja "; // ", and "
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return QCString("Periytyy ")+(numEntries > 1 ? "luokista " : "luokasta ")+trWriteList(numEntries)+"."; // "Inherits "
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return QCString("Periytetään ")+(numEntries > 1 ? "luokissa " : "luokassa ")+trWriteList(numEntries)+"."; // "Inherited by "
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return QCString("Uudelleentoteuttaa ")+(numEntries > 1 ? "luokat " : "luokan  ")+trWriteList(numEntries)+"."; // "Reimplemented from "
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return QCString("Uudelleentoteutetaan ")+(numEntries > 1 ? "luokissa " : "luokassa ")+trWriteList(numEntries)+"."; // "Reimplemented in "
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Nimiavaruuden jäsenet"; } // "Namespace Members"

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Nimiavaruuksien luettelo"; } // "Namespace Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Nimiavaruuden dokumentaatio"; } // "Namespace Documentation"

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Nimiavaruudet"; } // "Namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentaatio tälle "; // "The documentation for this "
      switch(compType)
      {
        case ClassDef::Class:      result+="luokalle"; break; // "class"
        case ClassDef::Struct:     result+="tietueelle"; break; // "struct"
        case ClassDef::Union:      result+="yhdisteelle"; break; // "union"
        case ClassDef::Interface:  result+="rajapinnalle"; break; // "interface"
        case ClassDef::Protocol:   result+="protokollalle"; break; // "protocol"
        case ClassDef::Category:   result+="kategorialle"; break; // "category"
        case ClassDef::Exception:  result+="poikkeukselle"; break; // "exception"
        default: break;
      }
      if (single) result+=" luotiin seuraavasta tiedostosta:"; // " was generated from the following file"
      else result+=" luotiin seuraavista tiedostoista:"; // ":" or "s:"
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Paluuarvot"; } // "Return values"

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Pääsivu"; } // "Main Page"

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "s."; } // "p."

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Määrittely tiedoston @1 rivillä @0."; //  "Definition at line @0 of file @1."
    }
    QCString trDefinedInSourceFile() override
    {
      return "Määrittely tiedostossa @0."; // "Definition in file @0."
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Vanhentunut"; // "Deprecated"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Yhteistyökaavio luokalle "+clName+":"; // "Collaboration diagram for "+clName+":"
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Sisällytysriippuvuuskaavio tiedostolle "+fName+":"; // "Include dependency graph for "+fName+":"
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Rakentajien & purkajien dokumentaatio"; // "Constructor & Destructor Documentation";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Siirry tämän tiedoston lähdekoodiin."; // "Go to the source code of this file."
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Siirry tämän tiedoston dokumentaatioon."; // "Go to the documentation of this file."
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Esiehto"; //"Precondition"
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Jälkiehto"; // "Postcondition"
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariantti"; // vai "Pysyväisväittämä"? "Invariant"
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Alkuarvo:"; // "Initial value:"
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "koodi"; // "code"
    }
    QCString trGraphicalHierarchy() override
    {
      return "Graafinen luokkahierarkia"; // "Graphical Class Hierarchy"
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Siirry graafiseen luokkahierarkiaan"; // "Go to the graphical class hierarchy"
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Siirry tekstimuotoiseen luokkahierarkiaan"; // "Go to the textual class hierarchy"
    }
    QCString trPageIndex() override
    {
      return "Sivuhakemisto"; // "Page Index"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
        // FIXME: Missähän merkityksessä tätä käytetään?
      return "Huomautus"; // "Note"
    }
    QCString trPublicTypes() override
    {
      return "Julkiset tyypit"; // "Public Types"
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tietueen kentät"; // "Data Fields"
      }
      else
      {
        return "Julkiset attribuutit"; // "Public Attributes"
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Staattiset julkiset attribuutit"; // "Static Public Attributes"
    }
    QCString trProtectedTypes() override
    {
      return "Suojellut tyypit"; // "Protected Types"
    }
    QCString trProtectedAttribs() override
    {
      return "Suojellut attribuutit"; // "Protected Attributes"
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Staattiset suojellut attribuutit"; // "Static Protected Attributes"
    }
    QCString trPrivateTypes() override
    {
      return "Yksityiset tyypit"; // "Private Types"
    }
    QCString trPrivateAttribs() override
    {
      return "Yksityiset attribuutit"; // "Private Attributes"
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Staattiset yksityiset attribuutit"; // "Static Private Attributes"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Vielä tehtävänä"; // "Todo"
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Tehtävälista"; // "Todo List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Viitattu"; // "Referenced by"
    }
    QCString trRemarks() override
    {
      return "Huomioita"; // "Remarks"
    }
    QCString trAttention() override
    {
      return "Huomio"; // "Attention"
    }
    QCString trInclByDepGraph() override
    {
      return "Tämä kaavio näyttää, mitkä tiedostot suorasti "
             "tai epäsuorasti sisällyttävät tämän tiedoston";
             // "This graph shows which files directly or "
             // "indirectly include this file:"
    }
    QCString trSince() override
    {
        // FIXME: Missä merkityksessä tätä käytetään?
      return "Lähtien"; // "Since"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Kaavion selite"; // "Graph Legend"
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Tämä sivu selittää, kuinka doxygenin luomia kaavioita tulkitaan.<p>\n"
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
        "}\n"
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
        "Tuloksena on seuraavanlainen kaavio:"
        //"This will result in the following graph:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Ylläolevassa kaaviossa laatikoilla on seuraavat merkitykset\n:"
        // "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>Täytetty harmaa laatikko esittää tietuetta tai luokkaa, jolle "
        // "<li>%A filled gray box represents the struct or class for which the "
        "kaavio on luotu.\n"
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
    QCString trLegend() override
    {
      return "selite"; // "legend"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Testi"; // "Test"
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Testilista"; // "Test List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Ominaisuudet"; // "Properties"
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Ominaisuuksien dokumentaatio"; // "Property Documentation"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Tietueet"; // "Data Structures"
      }
      else
      {
        return "Luokat"; // "Classes"
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paketti "+name; // "Package "
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Tässä ovat paketit lyhyiden selitysten kanssa (jos saatavilla):"; // "Here are the packages with brief descriptions (if available):"
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paketit"; // "Packages"
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Arvo:"; // "Value:"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bugi"; // "Bug"
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1252";
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
      return "Hakemisto"; // "Index"
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "luok", "at", "ka");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tiedosto", "t");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "nimiavaruu", "det", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "ryhmä", "t");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sivu", "t");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "jäsen", "et");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globaali", "t");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tekijä", "t");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Viittaukset"; // "References"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Toteuttaa luokan "+trWriteList(numEntries)+"."; // "Implements "
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Toteutettu luokassa "+trWriteList(numEntries)+"."; // "Implemented in "
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Sisällysluettelo"; // "Table of Contents"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Vanhentuneiden lista"; // "Deprecated List"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Tapahtumat"; // "Events"
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Tapahtumien dokumentaatio"; // "Event Documentation"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Paketin tyypit"; // "Package Types"
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Paketin funktiot"; // "Package Functions"
    }
    QCString trPackageMembers() override
    {
      return "Paketin jäsenet"; // "Package Members"
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Paketin staattiset funktiot"; // "Static Package Functions"
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Paketin attribuutit"; // "Package Attributes"
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Paketin staattiset attribuutit"; // "Static Package Attributes"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Kaikki"; // "All"
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Tässä on kutsukaavio tälle funktiolle:"; // "Here is the call graph for this function:"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
        return "Löytyi <b>$num</b> dokumenttia, jotka vastasivat hakuasi. " // "Found <b>$num</b> documents matching your query. "
                "Parhaat tulokset näytetään ensin."; // "Showing best matches first."
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Osumat:"; // "Matches:"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " lähdekooditiedosto"; // " Source File"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hakemistohierarkia"; } // "Directory Hierarchy"

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Hakemistojen dokumentaatio"; } // "Directory Documentation"

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Hakemistot"; } // "Directories"

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" hakemistoreferenssi"; return result; }
        // " Directory Reference"

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "hakemisto", "t");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Tämä on ylikuormitettu jäsenfunktio, ja se tarjotaan "
              "käytön helpottamiseksi. Se eroaa ylläolevasta "
              "funktiosta ainoastaan hyväksymiltään parametreilta.";
              // "This is an overloaded member function, "
              // "provided for convenience. It differs from the above "
              // "function only in what argument(s) it accepts."
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Tässä on kutsukaavio tälle funktiolle:"; // "Here is the caller graph for this function:"
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Luettelotyypin dokumentaatio"; } // "Enumerator Documentation"

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Jäsenfunktioiden/aliohjelmien dokumentaatio"; } // "Member Function/Subroutine Documentation"

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Tietotyyppien lista"; } // "Data Types List"

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Kentät"; } // "Data Fields";

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Tässä ovat tietotyypit lyhyiden selitysten kanssa:"; } // "Here are the data types with brief descriptions:"

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Tietotyyppien hakemisto"; } // "Data Type Index"

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Tietotyyppien dokumentaatio"; } // "Data Type Documentation"

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funktiot/aliohjelmat"; } // "Functions/Subroutines"

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Funktioiden/aliohjelmien dokumentaatio"; } // "Function/Subroutine Documentation"

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tietotyypit"; } // "Data Types"

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Moduulilista"; } // "Modules List"

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Tässä on lista kaikista "; // "Here is a list of all "
      if (!extractAll) result+="dokumentoiduista "; // "documented "
      result+="moduuleista lyhyiden selitysten kanssa:"; // "modules with brief descriptions:"
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
        case ClassDef::Class:      result+=" moduuli"; break; // " Module"
        case ClassDef::Struct:     result+=" tyyppi"; break; // " Type"
        case ClassDef::Union:      result+=" yhdiste"; break; // " Union"
        case ClassDef::Interface:  result+=" rajapinta"; break; // " Interface"
        case ClassDef::Protocol:   result+=" protokolla"; break; // " Protocol"
        case ClassDef::Category:   result+=" kategoria"; break; // " Category"
        case ClassDef::Exception:  result+=" poikkeus"; break; // " Exception"
        default: break;
      }
      if (isTemplate) result+="malli"; // " Template"
      result+="referenssi"; // " Reference"
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" moduulin referenssi"; // " Module Reference"
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Moduulin jäsenet"; } // "Module Members"

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Moduulien hakemisto"; } // "Modules Index"

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "moduuli", "t");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentaatio tälle "; // "The documentation for this "
      switch(compType)
      {
        case ClassDef::Class:      result+="moduulille"; break; // "module"
        case ClassDef::Struct:     result+="tyypille"; break; // "type"
        case ClassDef::Union:      result+="yhdisteelle"; break; // "union"
        case ClassDef::Interface:  result+="rajapinnalle"; break; // "interface"
        case ClassDef::Protocol:   result+="protokollalle"; break; // "protocol"
        case ClassDef::Category:   result+="kategorialle"; break; // "category"
        case ClassDef::Exception:  result+="poikkeukselle"; break; // "exception"
        default: break;
      }
      result+=" luotiin "; // " was generated from the following file"
      if (single) result+="seuraavasta tiedostosta:"; else result+="seuraavista tiedostoista:"; // ":" / "s:"
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tyyp", "it", "pi");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "aliohjelma", "t");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Tyyppien rajoitteet"; // "Type Constraints"
    }

    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "ma", "ti", "ke", "to", "pe", "la", "su" };
      static const char *days_full[]    = { "maanantai", "tiistai", "keskiviikko", "torstai", "perjantai", "lauantai", "sunnuntai" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "tammi", "helmi", "maalis", "huhti", "touko", "kesä", "heinä", "elo", "syys", "loka", "marras", "joulu" };
      static const char *months_full[]  = { "tammikuu", "helmikuu", "maaliskuu", "huhtikuu", "toukokuu", "kesäkuu", "heinäkuu", "elokuu", "syyskuu", "lokakuu", "marraskuu", "joulukuu" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "ap.", "ip." };
      return dayPeriod[period?1:0];
    }
};

#endif
