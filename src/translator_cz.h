/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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


// The first translation from English to Czech was started by
// Vlastimil Havran. He wrote:
// 
//   "In the cases where are more translations possible I hope
//    that I have selected the most reasonable text. If not, please,
//    mail the comments and text proposals to
//
//    havran@fel.cvut.cz, 1999/04/11"
//    
// The prototype version of Czech strings with diacritics was
// implemented by Petr Prikryl (prikrylp@skil.cz), 2000/06/20. As
// Vlastimil was quite busy at the time, he agreed that Petr be the
// maintainer for a while (from 2000/06/20).
//
// Updates:
// --------
// 2000/06/20
//  - The prototype version of Czech strings with diacritics. The
//    translation was based on translator.h of Doxygen version
//    1.1.4 (from scratch).
//
// 2000/07/10
//  - Updated version based on 1.1.5 sources (including important updates
//    of the up-to-1.1.4 strings). Czech strings in this file were written
//    in windows-1250 encoding. On-line decoding into iso-8859-2 ensured
//    via conditional compilation if the sources are compiled under UNIX.
//
// 2000/07/19
//  - Encoding conversion tables moved to the methods that use them.
//  - Updates for "new since 1.1.5".
//
// 2000/08/02 (Petr Prikryl)
//  - Updated for 1.2.0
//
// 2000/08/24
//  - Changed trTodo() text from "Udelat" to "Planovane upravy"
//    which seems more appropriate in the document context.
//  - Typo corrected in trMemberTypedefDocumentation().
//
// 2000/08/30
//  - Macro DECODE replaced by the inline Decode() (proposed by
//    Boris Bralo <boris.bralo@zg.tel.hr> in translator_hr.h).
//
// 2000/08/31
//  - Methods ISOToWin() and WinToISO() renamed and moved to the
//    base class (in translator.h) to be shared with the Croatian
//    translator.
//
// 2000/09/06
//  - Reimplementation of the method trInheritsList() which takes 
//    into account quantity of base classes.
//
// 2000/09/11
//  - Update for "new since 1.2.1" version. The text of trDCOPMethods()
//    was translated rather blindly (not knowing what exactly
//    the DCOP means).
//    
// 2000/10/17
//  - Update for "new since 1.2.2" version. 
//
// 2001/01/09
//  - Update for "new since 1.2.4" version. As I do not use Java,
//    I tried my best to guess the Czech terminology for the Java.
//    Let me know if some wording should be changed/improved.
//         
// 2001/02/15
//  - trMore() now returns only "..." (ellipsis).  The Czech replacement
//    of "(more)" was too much visible.  The previous "(...)" did not 
//    look well.
//    
// 2001/02/26
//  - Update for "new since 1.2.5" version (trBug(), trBugList()).
//         
// 2001/03/12
//  - Minor correction of comments which copied the same
//    corrections in translator.h (doubled backslash) just after 
//    1.2.6 release.
//    
// 2001/04/10
//  - Update for OPTIMIZE_OUTPUT_FOR_C (1.2.6-20010408).
//  - Removed implementation of latexBabelPackage().
//  - Removed implementation of trVerbatimText().
//  
// 2001/04/20
//  - Update for "new since 1.2.6-20010422". Experimental version
//    introducing TranslatorAdapter class and the abstract base
//    class Translator.  The English translator is now on the same
//    level as other translators.
//
// 2001/05/02
//  - The Decode() inline identifier changed to decode (i.e. lower
//    case) to be consistent with HR and RU translator, which also
//    use the approach.
//  - Removed the obsolete method trFiles().
//         
// 2001/05/18
//  - The trRelatedPagesDescription() content updated to fit 
//    the context better.
//  - Implemented new method trAuthor(params).
//  - Removed obsolete methods trAuthor() and trAuthors().
//
// 2001/05/25
//  - Updated misleading information in trLegendDocs().
//  - The trReimplementedInList() updated to fit the context better.
//  - The trReimplementedFromList() updated to fit the context better.
//
// 2001/07/16
//  - trClassDocumentation() updated as in the English translator.
//    
// 2001/11/06
//  - trReferences() implemented.
//    
// 2002/01/23
//  - trImplementedFromList() and trImplementedInList() implemented
//    (new since 1.2.13)
//  
// 2002/03/05
//  - ... forgot to replace TranslatorAdapter... base class by Translator.
//
// 2002/07/08 (my birthday! ;)
//  - The new trRTFTableOfContents() implemented.
//
// 2002/07/29
//  - The new trDeprecatedList() implemented.
//
// 2002/10/15
//  - The new trEvents() and trEventDocumentation() implemented.
//  
// 2003/04/28
//  - The five new methods "since 1.3" implemented.
//
// Todo
// ----
//  - The trReimplementedFromList() should pass the kind of the 
//    reimplemented element.  It can be method, typedef or possibly 
//    something else.  It is difficult to find the general translation
//    for all kinds in the Czech language.
// 
// Notices:
// -------- 
// The conditional compilation ensures or the neutral functionality
// of the private inline decode(), or calling the WinToISO() method
// for on-line encoding conversion. If you want to maintain the
// source in the iso-8859-2, do convert the encoding of the source,
// change the conditional definition of the inline decode() using the
// method ISO88592ToWin1250() -- for conversion of strings for the
// Windows version. The version which does not call the function is
// probably slightly faster.

class TranslatorCzech : public Translator
{
  private:
    /*! The decode() inline assumes the source written in the 
        Windows encoding (maintainer only dependent). 
     */
    inline QCString decode(const QCString & sInput)
    { 
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      {
        return sInput;
      }
      else
      {
        return Win1250ToISO88592(sInput);
      }
    }
    
  public:
    // --- Language control methods -------------------

    virtual QCString idLanguage()
    { return "czech"; }

    virtual QCString latexLanguageSupportCommand()
    { return "\\usepackage{czech}\n"; }
    
    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      {
        return "windows-1250";
      }
      else
      {
        return "iso-8859-2";
      }
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return decode("Související funkce"); }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return decode("(Uvedené funkce nejsou èlenskými funkcemi.)"); }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return decode("Detailní popis"); }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return decode("Dokumentace k èlenským typùm"); }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return decode("Dokumentace k èlenským výètùm"); }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return decode("Dokumentace k metodám"); }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {       
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Dokumentace k položkám"); 
      }
      else
      {
        return decode("Dokumentace k datovým èlenùm");
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return decode("..."); }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return decode("Seznam všech èlenù."); }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return decode("Seznam èlenù tøídy"); }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return decode("Zde naleznete úplný seznam èlenù tøídy "); }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return decode(", vèetnì všech zdìdìných èlenù."); }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generováno automaticky programem Doxygen "
                      "ze zdrojových textù";
      if (s) result+=(QCString)" projektu "+s;
      result+="."; 
      return decode(result);
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return decode("jméno výètu"); }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return decode("hodnota výètu"); }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return decode("definován v"); }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return decode("Moduly"); }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return decode("Hierarchie tøíd"); }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {  
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Datové struktury");
      }
      else
      { 
        return decode("Seznam tøíd"); 
      }
    }
   
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return decode("Seznam souborù"); }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return decode("Seznam hlavièkových souborù"); }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Datové položky"); 
      }
      else
      {
        return decode("Seznam èlenù tøíd"); 
      }
    }
   
    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Globální symboly"); 
      }
      else
      {
        return decode("Symboly v souborech"); 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return decode("Ostatní stránky"); }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return decode("Pøíklady"); }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return decode("Hledat"); }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return decode("Zde naleznete seznam, vyjadøující vztah dìdiènosti tøíd. "
             "Je seøazen pøibližnì (ale ne úplnì) podle abecedy:");
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="souborù se struènými popisy:";
      return decode(result);
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {       
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Následující seznam obsahuje identifikace datových "
                      "struktur a jejich struèné popisy:"); 
      }
      else
      {
        return decode("Následující seznam obsahuje pøedevším identifikace "
                      "tøíd, ale nacházejí se zde i další netriviální prvky, "
                      "jako jsou struktury (struct), unie (union) a rozhraní "
                      "(interface). V seznamu jsou uvedeny jejich struèné "
                      "popisy:");
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result= "Zde naleznete seznam všech ";
      if (!extractAll) 
      {
        result += "dokumentovaných ";
      }
            
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result += "položek struktur (struct) a unií (union) ";
      }
      else
      {
        result += "èlenù tøíd ";
      }
      
      result += "s odkazy na ";  
      
      if (extractAll)
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result += "dokumentaci struktur/unií, ke kterým pøíslušejí:";
        }
        else
        {
          result += "dokumentaci tøíd, ke kterým pøíslušejí:";
        }
      }
      else
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="struktury/unie, ke kterým pøíslušejí:";
        }
        else
        {
          result+="tøídy, ke kterým pøíslušejí:";
        }
      }
        
      return decode(result);
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="funkcí, promìnných, maker, výètù a definic typù (typedef) "
                "s odkazy na ";
      }
      else
      {
        result+="symbolù, které jsou definovány na úrovni svých souborù. "
                "Pro každý symbol je uveden odkaz na ";
      }
        
      if (extractAll) 
        result+="soubory, ke kterým pøíslušejí:";
      else 
        result+="dokumentaci:";
        
      return decode(result);
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return decode("Zde naleznete hlavièkové soubory, které tvoøí "
             "aplikaèní programátorské rozhraní (API):"); }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return decode("Zde naleznete seznam všech pøíkladù:"); }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return decode("Následující seznam odkazuje na další stránky projektu:"); }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return decode("Zde naleznete seznam všech modulù:"); }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return decode("Popis není k dispozici"); }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return decode("Dokumentace"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return decode("Rejstøík modulù"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return decode("Rejstøík hierarchie tøíd"); }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return decode("Rejstøík datových struktur");
      }
      else
      {
        return decode("Rejstøík tøíd"); 
      }
    }
   
    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return decode("Rejstøík souborù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return decode("Dokumentace modulù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Dokumentace datových struktur"); 
      }
      else
      {
        return decode("Dokumentace tøíd"); 
      }      
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return decode("Dokumentace souborù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return decode("Dokumentace pøíkladù"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return decode("Dokumentace souvisejících stránek"); }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return decode("Referenèní pøíruèka"); }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return decode("Definice maker"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return decode("Prototypy"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return decode("Definice typù"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return decode("Výèty"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return decode("Funkce"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return decode("Promìnné"); }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return decode("Hodnoty výètu"); }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return decode("Dokumentace k definicím maker"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return decode("Dokumentace prototypù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return decode("Dokumentace definic typù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return decode("Dokumentace výètových typù"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return decode("Dokumentace výètových hodnot"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return decode("Dokumentace funkcí"); }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return decode("Dokumentace promìnných"); }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return decode("Datové struktry");
      }
      else
      {
        return decode("Tøídy"); 
      }
    }
   
    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generováno "+date;
      if (projName) result+=(QCString)" pro projekt "+projName;
      result+=(QCString)" programem";
      return decode(result);
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return decode(" -- autor ");
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return decode((QCString)"Diagram dìdiènosti pro tøídu "+clName);
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return decode("Pouze pro vnitøní použití."); }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return decode("Reimplementováno z interních dùvodù; "
                    "aplikaèní rozhraní zachováno."); }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return decode("Pozor"); }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return decode("Chyby a omezení"); }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return decode("Verze"); }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return decode("Datum"); }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return decode("Návratová hodnota"); }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return decode("Viz také"); }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return decode("Parametry"); }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return decode("Výjimky"); }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return decode("Generováno programem"); }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return decode("Seznam prostorù jmen"); }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Zde naleznete seznam všech ";
      if (!extractAll) result+="dokumentovaných ";
      result+="prostorù jmen se struèným popisem:";
      return decode(result);
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return decode("Friends"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return decode("Dokumentace k friends"); }
    
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
      return decode(result);
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result("Dokumentace souboru ");
      result+=fileName; 
      return decode(result);
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result("Dokumentace prostoru jmen ");
      result+=namespaceName;
      return decode(result);
    }
    
    /* 
     *  these are for the member sections of a class, struct or union 
     */
    virtual QCString trPublicMembers()
    { return decode("Veøejné metody"); }
    virtual QCString trPublicSlots()
    { return decode("Veøejné sloty"); }
    virtual QCString trSignals()
    { return decode("Signály"); }
    virtual QCString trStaticPublicMembers()
    { return decode("Statické veøejné metody"); }
    virtual QCString trProtectedMembers()
    { return decode("Chránìné metody"); }
    virtual QCString trProtectedSlots()
    { return decode("Chránìné sloty"); }
    virtual QCString trStaticProtectedMembers()
    { return decode("Statické chránìné metody"); }
    virtual QCString trPrivateMembers()
    { return decode("Privátní metody"); }
    virtual QCString trPrivateSlots()
    { return decode("Privátní sloty"); }
    virtual QCString trStaticPrivateMembers()
    { return decode("Statické privátní metody"); }
    
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
      return decode(result); 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      QCString result("Dìdí z ");
      result += (numEntries == 1) ? "bázové tøídy " : "bázových tøíd ";
      result += trWriteList(numEntries)+".";
      return decode(result);
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      QCString result("Zdìdìna ");
      result += (numEntries == 1) ? "tøídou " : "tøídami ";
      result += trWriteList(numEntries)+".";
      return decode(result);
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      QCString result("Reimplementuje stejnojmenný prvek z ");
      result += trWriteList(numEntries)+".";
      return decode(result);
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      QCString result("Reimplementováno v ");
      result += trWriteList(numEntries)+".";
      return decode(result);
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return decode("Symboly v prostorech jmen"); }

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
      return decode(result);
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return decode("Rejstøík prostorù jmen"); }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return decode("Dokumentace prostorù jmen"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return decode("Prostory jmen"); }

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
      return decode(result);
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return decode("Rejstøík tøíd"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return decode("Vracené hodnoty"); }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return decode("Hlavní stránka"); }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return decode("s."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return decode("Zdroje");
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return decode("Definice je uvedena na øádku @0 v souboru @1.");
    }
    virtual QCString trDefinedInSourceFile()
    {
      return decode("Definice v souboru @0.");
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return decode("Zastaralé");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return decode((QCString)"Diagram tøíd pro "+clName+":");
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return decode((QCString)"Graf závislostí na vkládaných souborech "
                    "pro "+fName+":");
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return decode("Dokumentace konstruktoru a destruktoru"); 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return decode("Zobrazit zdrojový text tohoto souboru.");
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return decode("Zobrazit dokumentaci tohoto souboru.");
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return decode("Precondition");
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return decode("Postcondition");
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return decode("Invariant");
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return decode("Initializer:");
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return decode("zdrojový text");
    }
    virtual QCString trGraphicalHierarchy()
    {
      return decode("Grafické zobrazení hierarchie tøíd");
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return decode("Zobrazit grafickou podobu hierarchie tøíd");
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return decode("Zobrazit textovou podobu hierarchie tøíd");
    }
    virtual QCString trPageIndex()
    {
      return decode("Rejstøík stránek");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return decode("Poznámka");
    }
    virtual QCString trPublicTypes()
    {
      return decode("Veøejné typy");
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return decode("Datové položky");
      }
      else
      {
        return decode("Veøejné atributy");
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return decode("Statické veøejné atributy");
    }
    virtual QCString trProtectedTypes()
    {
      return decode("Chránìné typy");
    }
    virtual QCString trProtectedAttribs()
    {
      return decode("Chránìné atributy");
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return decode("Statické chránìné atributy");
    }
    virtual QCString trPrivateTypes()
    {
      return decode("Privátní typy");
    }
    virtual QCString trPrivateAttribs()
    {
      return decode("Privátní atributy");
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return decode("Statické privátní atributy");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return decode("Plánované úpravy");
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return decode("Seznam plánovaných úprav");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return decode("Používá se v");
    }
    virtual QCString trRemarks()
    {
      return decode("Poznámky"); // ??? not checked in a context
    }
    virtual QCString trAttention()
    {
      return decode("Upozornìní"); // ??? not checked in a context
    }
    virtual QCString trInclByDepGraph()
    {
      return decode("Následující graf ukazuje, které soubory pøímo nebo "
             "nepøímo vkládají tento soubor:");
    }
    virtual QCString trSince()
    {
      return decode("Od"); // ??? not checked in a context
    }

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return decode("Vysvìtlivky ke grafu");
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
        "/*! Bázová tøída dìdìná veøejnì (public inheritance). */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Bázová tøída, použitá pro chránìné dìdìní "
        "(protected inheritance). */\n"
        "class ProtectedBase { };\n\n"
        "/*! Bázová tøída, využitá pro privátní dìdìní "
        "(private inheritance). */\n"
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
            "dìdiènosti (public) mezi dvìma tøídami.\n"
        "<li>Tmavì zelená šipka oznaèuje vztah chránìné dìdiènosti "
            "(protected).\n"
        "<li>Tmavì èervená šipka oznaèuje vztah privátní dìdiènosti "
            "(private).\n"
        "<li>Purpurová šipka kreslená èárkovanì se používá v pøípadì, "
            "kdy je tøída obsažena v jiné tøídì,\n"
            "nebo kdy je používána jinou tøídou. Je oznaèena identifikátorem "
            "jedné nebo více promìných, pøes které\n"
            "je tøída nebo struktura zpøístupnìna.\n"
        "</ul>\n");
        
        return decode(result);
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return decode("vysvìtlivky");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return decode("Test");
    }
   
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return decode("Seznam testù");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return decode("Metody DCOP");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return decode("Vlastnosti");
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return decode("Dokumentace k vlastnosti");
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return decode("Rozhraní");
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return decode("Tøídy");
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return decode((QCString)"Balík "+name);
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return decode("Seznam balíkù");
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return decode("Zde naleznete seznam balíkù se struèným popisem "
                    "(pokud byl uveden):");
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return decode("Balíky");
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return decode("Dokumentace balíku");
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return decode("Hodnota:");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return decode("Chyba");
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return decode("Seznam chyb");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1250";
    }
    
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "238";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return decode("Rejstøík");
    }
                                                                          
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tøíd" : "tøíd"));
      result+=(singular ? "a" : "y");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Soubor" : "soubor"));
      if (!singular)  result+="y";
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Prostor" : "prostor"));
      if (!singular)  result+="y";
      result+=" jmen";
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Skupin" : "skupin"));
      result+=(singular ? "a" : "y");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Stránk" : "stránk"));
      result+=(singular ? "a" : "y");
      return decode(result); 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Èlen" : "èlen"));
      if (!singular)  result+="y";
      return decode(result); 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Položk" : "položk"));
      result+=(singular ? "a" : "y");
      return decode(result); 
    }

    /*! ??? Jak to prelozit? Bylo by dobre, kdyby se ozval nekdo,
     * kdo to pouziva.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
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
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Auto" : "auto"));
      result += (singular) ? "r" : "øi";
      return decode(result); 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return decode("Odkazuje se na");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementuje "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return decode("Implementováno v "+trWriteList(numEntries)+".");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return decode("Obsah");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return decode("Seznam zastaralých prvkù");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return decode("Události");
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return decode("Dokumentace událostí");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return decode("Typy v balíku");
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return decode("Funkce v balíku");
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return decode("Statické funkce v balíku");
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return decode("Atributy balíku");
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return decode("Statické atributy balíku");
    }

};

#endif // TRANSLATOR_CZ_H
