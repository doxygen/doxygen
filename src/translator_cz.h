/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_CZ_H
#define TRANSLATOR_CZ_H

#include "translator.h"

// The translation from English to Czech by Vlastimil Havran.
// In the cases where are more translations possible I hope
// that I have selected the most reasonable text. If not, please,
// mail the comments and text proposals to
//
// havran@fel.cvut.cz, 1999/04/11
//
// Updates:
// --------
// 2000/06/20 
//  - The prototype version of Czech strings with diacritics implemented by
//    Petr Prikryl (prikrylp@skil.cz).  (Vlastimil Havran is still
//    the maintainer.)  The translation was based on translator.h
//    of Doxygen versison 1.1.4 (from scratch).
//
// 2000/07/10 (Petr Prikryl)
//  - Updated version based on 1.1.5 sources (including important updates
//    of the up-to-1.1.4 strings). Czech strings this file were written
//    in windows-1250 encoding. On-line decoding into iso-8859-2 ensured
//    via conditional compilation if the sources are compiled under UNIX.
//
// 2000/07/19 (Petr Prikryl)
//  - Encoding conversion tables moved to the methods that use them.
//  - Updates for "new since 1.1.5".
//
// 2000/08/02 (Petr Prikryl)
//  - Updated for 1.2.0
//
// 2000/08/24 (Petr Prikryl)
//  - Changed trTodo() text from "Udelat" to "Planovane upravy"
//    which seems more appropriate in the document context.
//  - Typo corrected in trMemberTypedefDocumentation().
//
// 2000/08/30 (Petr Prikryl)
//  - Macro DECODE replaced by the inline Decode() (proposed by
//    Boris Bralo <boris.bralo@zg.tel.hr> in translator_hr.h).
//
// 2000/08/31 (Petr Prikryl)
//  - Methods ISOToWin() and WinToISO() renamed and moved to the
//    base class (in translator.h) to be shared with the Croatian
//    translator.
//
// 2000/09/06 (Petr Prikryl)
//  - Reimplementation of the method trInheritsList() which takes 
//    into account quantity of base classes.
//
// 2000/09/11 (Petr Prikryl)
//  - Update for "new since 1.2.1" version. The text of trDCOPMethods()
//    was translated rather blindly (not knowing what exactly
//    the DCOP means).
//         
// Notices:
// -------- 
// The conditional compilation ensures or the neutral functionality
// of the private inline Decode(), or calling the WinToISO() method
// for on-line encoding conversion. If you want to maintain the
// source in the iso-8859-2, do convert the source encoding, change
// the conditional definition of the inline Decode() using the
// method ISO88592ToWin1250() -- for conversion of strings for the
// Windows version. The version which does not call the function is
// probably slightly faster.

class TranslatorCzech : public Translator
{
  private:
    /*! The Decode() inline assumes the source written in the 
        Windows encoding (maintainer dependent). 
     */
    inline QCString Decode(const QCString & sInput)
    { 
#ifdef _WIN32
      return sInput;
#else
      return Win1250ToISO88592(sInput);
#endif
    }
    
  public:
    // --- Language contol methods -------------------

    virtual QCString idLanguage()
    { return "czech"; }

    virtual QCString latexLanguageSupportCommand()
    { return "\\usepackage{czech}\n"; }
    
    /*! returns the name of the package that is included by LaTeX */
    virtual QCString latexBabelPackage() 
    { return ""; }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
#ifdef _WIN32
        return "windows-1250";
#else
        return "iso-8859-2";
#endif
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return Decode("Související funkce"); }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return Decode("(Uvedené funkce nejsou èlenskými funkcemi.)"); }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return Decode("Detailní popis"); }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return Decode("Dokumentace k èlenským typùm"); }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return Decode("Dokumentace k èlenským výètùm"); }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return Decode("Dokumentace k metodám"); }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { return Decode("Dokumentace k datovým èlenùm"); }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return Decode("(...)"); }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return Decode("Seznam všech èlenù."); }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return Decode("Seznam èlenù tøídy"); }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return Decode("Zde naleznete úplný seznam èlenù tøídy "); }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return Decode(", vèetnì všech zdìdìných èlenù."); }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generováno automaticky programem Doxygen "
                      "ze zdrojových textù";
      if (s) result+=(QCString)" projektu "+s;
      result+="."; 
      return Decode(result);
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return Decode("jméno výètu"); }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return Decode("hodnota výètu"); }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return Decode("definován v"); }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    virtual QCString trVerbatimText(const char *f)
    { return Decode((QCString)"Úplný text vkládaného souboru "+f+"."); }
    
    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \group command).
     */
    virtual QCString trModules()
    { return Decode("Moduly"); }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return Decode("Hierarchie tøíd"); }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { return Decode("Seznam tøíd"); }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return Decode("Seznam souborù"); }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return Decode("Seznam hlavièkových souborù"); }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { return Decode("Seznam èlenù tøíd"); }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { return Decode("Symboly v souborech"); }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return Decode("Související stránky"); }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return Decode("Pøíklady"); }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return Decode("Hledat"); }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return Decode("Zde naleznete seznam, vyjadøující vztah dìdiènosti tøíd. "
             "Je seøazen pøibližnì (ale ne úplnì) podle abecedy:");
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="souborù se struènými popisy:";
      return Decode(result);
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { return Decode("Následující seznam obsahuje pøedevším identifikace tøíd, "
             "ale nachází se zde i další netriviální prvky, jako jsou "
             "struktury (struct), unie (union) a rozhraní (interface). "
             "V seznamu jsou uvedeny jejich struèné popisy:"); 
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="èlenù tøíd s odkazy na ";
      if (extractAll) 
        result+="dokumentaci tøíd, ke kterým pøíslušejí:";
      else 
        result+="tøídy, ke kterým pøíslušejí:";
      return Decode(result);
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="symbolù, které jsou definovány na úrovni svých souborù. "
              "Pro každý symbol je uveden odkaz na ";
      if (extractAll) 
        result+="dokumentaci pøíslušného souboru";
      else 
        result+="soubor, ve kterém je symbol definován";
      result+=". Podle zpùsobu definice mùže být symbol globálním symbolem "
              "nebo symbolem, který je viditelný pouze z daného souboru "
              "(to se týká napøíklad statických promìnných v C++). "
              "Seznam neobsahuje symboly èlenù tøíd. "
              "Symbol mùže oznaèovat makro, typ, instanci tøídy, "
              "promìnnou, konstantu, funkci, výèet, hodnotu výètu a podobnì:";  
        
      return Decode(result);
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return Decode("Zde naleznete hlavièkové soubory, které tvoøí "
             "aplikaèní programátorské rozhraní (API):"); }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return Decode("Zde naleznete seznam všech pøíkladù:"); }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return Decode("Zde naleznete seznam všech souvisejících stránek "
                    "dokumentace:"); }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return Decode("Zde naleznete seznam všech modulù:"); }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return Decode("Popis není k dispozici"); }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return Decode("Dokumentace"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return Decode("Rejstøík modulù"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return Decode("Rejstøík hierarchie tøíd"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    { return Decode("Rejstøík tøíd"); }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return Decode("Rejstøík souborù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return Decode("Dokumentace modulù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { return Decode("Dokumentace tøíd"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return Decode("Dokumentace souborù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return Decode("Dokumentace pøíkladù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return Decode("Dokumentace souvisejících stránek"); }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return Decode("Referenèní pøíruèka"); }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return Decode("Definice maker"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return Decode("Prototypy"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return Decode("Definice typù"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return Decode("Výèty"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return Decode("Funkce"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return Decode("Promìnné"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return Decode("Hodnoty výètù"); }
    
    /*! This is used in man pages as the author section. */
    virtual QCString trAuthor()
    { return Decode("Autor"); }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return Decode("Dokumentace k definicím maker"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return Decode("Dokumentace prototypù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return Decode("Dokumentace definic typù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return Decode("Dokumentace výètových typù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return Decode("Dokumentace výètových hodnot"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return Decode("Dokumentace funkcí"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return Decode("Dokumentace promìnných"); }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { return Decode("Tøídy"); }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    virtual QCString trFiles()
    { return Decode("Soubory"); }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generováno "+date;
      if (projName) result+=(QCString)" pro projekt "+projName;
      result+=(QCString)" programem";
      return Decode(result);
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return Decode(" -- autor ");
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return Decode((QCString)"Diagram dìdiènosti pro tøídu "+clName);
    }
    
    /*! this text is generated when the \internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return Decode("Pouze pro vnitøní použití."); }

    /*! this text is generated when the \reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return Decode("Reimplementováno z interních dùvodù; "
                    "aplikaèní rozhraní zachováno."); }

    /*! this text is generated when the \warning command is used. */
    virtual QCString trWarning()
    { return Decode("Pozor"); }

    /*! this text is generated when the \bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return Decode("Chyby a omezení"); }

    /*! this text is generated when the \version command is used. */
    virtual QCString trVersion()
    { return Decode("Verze"); }

    /*! this text is generated when the \date command is used. */
    virtual QCString trDate()
    { return Decode("Datum"); }

    /*! this text is generated when the \author command is used. */
    virtual QCString trAuthors()
    { return Decode("Autor/autoøi"); }

    /*! this text is generated when the \return command is used. */
    virtual QCString trReturns()
    { return Decode("Návratová hodnota"); }

    /*! this text is generated when the \sa command is used. */
    virtual QCString trSeeAlso()
    { return Decode("Viz také"); }

    /*! this text is generated when the \param command is used. */
    virtual QCString trParameters()
    { return Decode("Parametry"); }

    /*! this text is generated when the \exception command is used. */
    virtual QCString trExceptions()
    { return Decode("Výjimky"); }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return Decode("Generováno programem"); }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return Decode("Seznam prostorù jmen"); }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="prostorù jmen se struèným popisem:";
      return Decode(result);
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return Decode("Friends"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return Decode("Dokumentace k friends"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result("Dokumentace ");
      if (isTemplate) result+="šablony ";
      switch(compType)
      {
        case ClassDef::Class:      result+="tøídy "; break;
        case ClassDef::Struct:     result+="struktury "; break;
        case ClassDef::Union:      result+="unie "; break;
        case ClassDef::Interface:  result+="rozhraní "; break;
        case ClassDef::Exception:  result+="výjimky "; break;
      }
      result+=clName;
      return Decode(result);
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result("Dokumentace souboru ");
      result+=fileName; 
      return Decode(result);
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result("Dokumentace prostoru jmen ");
      result+=namespaceName;
      return Decode(result);
    }
    
    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union 
     */
    virtual QCString trPublicMembers()
    { return Decode("Veøejné metody"); }
    virtual QCString trPublicSlots()
    { return Decode("Veøejné sloty"); }
    virtual QCString trSignals()
    { return Decode("Signály"); }
    virtual QCString trStaticPublicMembers()
    { return Decode("Statické veøejné metody"); }
    virtual QCString trProtectedMembers()
    { return Decode("Chránìné metody"); }
    virtual QCString trProtectedSlots()
    { return Decode("Chránìné sloty"); }
    virtual QCString trStaticProtectedMembers()
    { return Decode("Statické chránìné metody"); }
    virtual QCString trPrivateMembers()
    { return Decode("Privátní metody"); }
    virtual QCString trPrivateSlots()
    { return Decode("Privátní sloty"); }
    virtual QCString trStaticPrivateMembers()
    { return Decode("Statické privátní metody"); }
    /*! \endmgroup */ 
    
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
            result+=" a ";
        }
      }
      return Decode(result); 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      QCString result("Dìdí z ");
      result += (numEntries == 1) ? "bázové tøídy " : "bázových tøíd ";
      result += trWriteList(numEntries)+".";
      return Decode(result);
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      QCString result("Zdìdìna ");
      result += (numEntries == 1) ? "tøídou " : "tøídami ";
      result += trWriteList(numEntries)+".";
      return Decode(result);
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      QCString result("Reimplementuje ");
      result += (numEntries == 1) ? "metodu tøídy " : "metody tøíd ";
      result += trWriteList(numEntries)+".";
      return Decode(result);
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      QCString result("Reimplementováno ");
      result += (numEntries == 1) ? "tøídou " : "tøídami ";
      result += trWriteList(numEntries)+".";
      return Decode(result);
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return Decode("Symboly v prostorech jmen"); }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="symbolù, které jsou definovány ve svých prostorech jmen. "
              "U každého je uveden odkaz na ";
      if (extractAll) 
        result+="dokumentaci pøíslušného prostoru jmen:";
      else 
        result+="pøíslušný prostor jmen:";
      return Decode(result);
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return Decode("Rejstøík prostorù jmen"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return Decode("Dokumentace prostorù jmen"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return Decode("Prostory jmen"); }

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
      QCString result=(QCString)"Dokumentace pro ";
      switch(compType)
      {
        case ClassDef::Class:      result+="tuto tøídu"; break;
        case ClassDef::Struct:     result+="tuto strukturu (struct)"; break;
        case ClassDef::Union:      result+="tuto unii (union)"; break;
        case ClassDef::Interface:  result+="toto rozhraní"; break;
        case ClassDef::Exception:  result+="tuto výjimku"; break;
      }
      result+=" byla generována z ";
      if (single) result+="následujícího souboru:"; 
      else        result+="následujících souborù:";
      return Decode(result);
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return Decode("Rejstøík tøíd"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return Decode("Vracené hodnoty"); }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return Decode("Hlavní stránka"); }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return Decode("s."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return Decode("Zdroje");
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return Decode("Definice je uvedena na øádku @0 v souboru @1.");
    }
    virtual QCString trDefinedInSourceFile()
    {
      return Decode("Definice v souboru @0.");
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return Decode("Zastaralé");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return Decode((QCString)"Diagram tøíd pro "+clName+":");
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return Decode((QCString)"Graf závislostí na vkládaných souborech "
                    "pro "+fName+":");
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return Decode("Dokumentace konstruktoru a destruktoru"); 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return Decode("Zobrazit zdrojový text tohoto souboru.");
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return Decode("Zobrazit dokumentaci tohoto souboru.");
    }
    /*! Text for the \pre command */
    virtual QCString trPrecondition()
    {
      return Decode("Precondition");
    }
    /*! Text for the \post command */
    virtual QCString trPostcondition()
    {
      return Decode("Postcondition");
    }
    /*! Text for the \invariant command */
    virtual QCString trInvariant()
    {
      return Decode("Invariant");
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return Decode("Initializer:");
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return Decode("zdrojový text");
    }
    virtual QCString trGraphicalHierarchy()
    {
      return Decode("Grafické zobrazení hierarchie tøíd");
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return Decode("Zobrazit grafickou podobu hierarchie tøíd");
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return Decode("Zobrazit textovou podobu hierarchie tøíd");
    }
    virtual QCString trPageIndex()
    {
      return Decode("Rejstøík stránek");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return Decode("Poznámka");
    }
    virtual QCString trPublicTypes()
    {
      return Decode("Veøejné typy");
    }
    virtual QCString trPublicAttribs()
    {
      return Decode("Veøejné atributy");
    }
    virtual QCString trStaticPublicAttribs()
    {
      return Decode("Statické veøejné atributy");
    }
    virtual QCString trProtectedTypes()
    {
      return Decode("Chránìné typy");
    }
    virtual QCString trProtectedAttribs()
    {
      return Decode("Chránìné atributy");
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return Decode("Statické chránìné atributy");
    }
    virtual QCString trPrivateTypes()
    {
      return Decode("Privátní typy");
    }
    virtual QCString trPrivateAttribs()
    {
      return Decode("Privátní atributy");
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return Decode("Statické privátní atributy");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return Decode("Plánované úpravy");
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return Decode("Seznam plánovaných úprav");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return Decode("Používá se v");
    }
    virtual QCString trRemarks()
    {
      return Decode("Poznámky"); // ??? not checked in a context
    }
    virtual QCString trAttention()
    {
      return Decode("Upozornìní"); // ??? not checked in a context
    }
    virtual QCString trInclByDepGraph()
    {
      return Decode("Následující graf ukazuje, které soubory pøímo nebo "
             "nepøímo vkládají tento soubor:");
    }
    virtual QCString trSince()
    {
      return Decode("Od"); // ??? not checked in a context
    }

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return Decode("Vysvìtlivky ke grafu");
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      QCString result(  
        "Zde naleznete vysvìtlení, jak mají být interpretovány grafy, "
        "které byly generovány programem doxygen.<p>\n"
        "Uvažujte následující pøíklad:\n"
        "\\code\n"
        "/*! Neviditelná tøída, která se v grafu nezobrazuje, protože "
        "došlo k oøezání grafu. */\n"
        "class Invisible { };\n\n"
        "/*! Tøída, u které došlo k oøezání grafu. Vztah dìdiènosti "
        "je skryt. */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Tøída, která není dokumentována komentáøi programu doxygen. */\n"
        "class Undocumented { };\n\n"
        "/*! Odvozená tøída s veøejným (public) dìdìním bázové tøídy. */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Odvozená tøída s chránìným (protected) dìdìním bázové tøídy. */\n"
        "class ProtectedBase { };\n\n"
        "/*! Odvozená tøída s privátním dìdìním bázové tøídy. */\n"
        "class PrivateBase { };\n\n"
        "/*! Tøída, která je využívána tøídou Inherited. */\n"
        "class Used { };\n\n"
        "/*! Odvozená tøída, která rùzným zpùsobem dìdí z více bázových "
        "tøíd. */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Pokud je položka \\c MAX_DOT_GRAPH_HEIGHT konfiguraèního souboru "
        "nastavena na hodnotu 200, bude vygenerován následující graf:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Bloky (tj. uzly) v uvedeném grafu mají následující význam:\n"
        "<ul>\n"
        "<li>Èernì vyplnìný obdélník reprezentuje strukturu nebo tøídu, "
            "pro kterou byl graf generován.\n"
        "<li>Obdélník s èerným obrysem oznaèuje dokumentovanou "
            "strukturu nebo tøídu.\n"
        "<li>Obdélník s šedým obrysem oznaèuje nedokumentovanou "
            "strukturu nebo tøídu.\n"
        "<li>Obdélník s èerveným obrysem oznaèuje dokumentovanou "
            "strukturu nebo tøídu, pro kterou\n"
            "nejsou zobrazeny všechny vztahy dìdiènosti nebo obsažení. "
            "Graf je oøezán v pøípadì, kdy jej\n"
            "není možné umístit do vymezených hranic.\n"
        "</ul>\n"
        "Šipky (tj. hrany grafu) mají následující význam:\n"
        "<ul>\n"
        "<li>Tmavì modrá šipka se používá pro oznaèení vztahu veøejné "
            "dìdiènosti mezi dvìma tøídami.\n"
        "<li>Tmavì zelená šipka oznaèuje vztah chránìné dìdiènosti "
            "(protected).\n"
        "<li>Tmavì èervená šipka oznaèuje vztah privátní dìdiènosti.\n"
        "<li>Purpurová šipka kreslená èárkovanì se používá v pøípadì, "
            "kdy je tøída obsažena v jiné tøídì,\n"
            "nebo kdy je používána jinou tøídou. Je oznaèena identifikátorem "
            "jedné nebo více promìných, pøes které\n"
            "je tøída nebo struktura zpøístupnìna.\n"
        "</ul>\n");
        
        return Decode(result);
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return Decode("vysvìtlivky");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return Decode("Test");
    }
   
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return Decode("Seznam testù");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return Decode("Metody DCOP");
    }
};

#endif // TRANSLATOR_CZ_H
