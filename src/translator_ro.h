/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

/* The translation from English to Romanian by Alexandru Iosup [aiosup@yahoo.com].
 *
 * Disclaimer: I hope I translated these to Romanian keeping
 * the meaning of the sentences intact. Keep in mind that I used C-words were possible,
 * (e.g. the romanian for typedef is much unexpressive than the original), since we are
 * producing C-documentation of our projects.
 *
 * If you have suggestions, please mail the comments and text proposals to the address 
 * shown aprox.10 lines above
 *
 * Project start:
 * --------------
 * 20.Sep.2k 
 */
#ifndef TRANSLATOR_RO_H
#define TRANSLATOR_RO_H

#include "translator_adapter.h"

class TranslatorRomanian: public TranslatorAdapter_1_2_1
{
  public:

    // --- Language contol methods -------------------
    
    /*! Used for identification of the language. May resemble 
     * the string returned by latexBabelPackage(), but it is not used
     * for the same purpose. The identification should not be translated.
     * It should be replaced by the name of the language in English
     * (e.g. Czech, Japanese, Russian, etc.). It should be equal to 
     * the identification in language.h.
     */
    virtual QCString idLanguage()
    { return "romanian"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage{romanian}\n";
    }
    /*! returns the name of the package that is included by LaTeX */
    virtual QCString latexBabelPackage() 
    { return "romanian"; }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-2";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funcþii înrudite"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Atenþie: NU sunt funcþii membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descriere Detaliatã"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Documentaþia Declaraþiilor Typedef membre"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Documentaþia membrilor Enum"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Documentaþia Funcþiilor membre"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { return "Documentaþia Datelor membre"; }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Mai mult..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista tuturor membrilor."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Lista Membrilor"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Aceasta este lista completã a membrilor din "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", inclusiv a tuturor membrilor moºteniþi."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generat automat de Doxygen";
      if (s) result+=(QCString)" pentru "+s;
      result+=" din codul sursã."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nume enum"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "valoare enum"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definit în"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    virtual QCString trVerbatimText(const char *f)
    { return (QCString)"Acesta este textul original al fiºierului inclus "+f; }
    
    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \group command).
     */
    virtual QCString trModules()
    { return "Module"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Ierarhia Claselor"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { return "Lista Componenþilor"; }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista fiºierelor"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "Fiºiere Header"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { return "Membrii Componenþi"; }//cu articol hotarat

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { return "Membrii din Fiºier"; }//cu articol hotarat

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Pagini înrudite"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Exemples"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Cautã"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Aceastã listã de legãturi este sortatã în mare, "
             "dar nu complet, în ordine alfabeticã:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Aici este lista tuturor ";
      result+="fiºierelor";
	  if (!extractAll) result+=" documentate";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { return "Aici sunt clasele, structurile, uniunile ºi interfeþele"
		", cu scurte descrieri"; 
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Aici este lista tuturor ";
      result+="membrilor ";
	  if (!extractAll) result+="documentaþi ";
	  result+="din toate clasele, cu legãturi cãtre ";
		  
      if (extractAll) 
        result+="documentaþia clasei pentru fiecare membru în parte:";
      else 
        result+="clasele de care aparþin:";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Aici este lista tuturor ";
	  result+="membrilor ";
	  if (!extractAll) result+="documentaþi ";
	  result+="din toate fiºierele, cu legãturi cãtre ";
      if (extractAll) 
        result+="documentaþia fiºierului pentru fiecare membru în parte:";
      else 
        result+="fiºierele de care aparþin:";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "Aici sunt fiºierele Header care fac parte din API:"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Aici este lista tuturor exemplelor:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Aici este lista tuturor documentaþiilor înrudite:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Aici este lista tuturor modulelor:"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return "Nici o descriere disponibilã"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Documentaþie"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Indexul Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Index Ierarhic"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    { return "Indexul Componenþilor"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Indexul Fiºierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Documentaþia Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { return "Documentaþia Claselor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Documentaþia Fiºierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Documentaþia Exemplelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Documentaþii înrudite"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Manual de utilizare"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definiþii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototipuri de funcþii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Declaraþii Typedef"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumeraþii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funcþii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variabile"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Valori enum"; }
    
    /*! This is used in man pages as the author section. */
    virtual QCString trAuthor()
    { return "Autor"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Documentaþia definiþiilor"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Documentaþia prototipurilor de funcþii"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Documentaþia definiþiilor Typedef"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Documentaþia tipurilor enum"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentaþia valorilor enum"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Documentaþia funcþiilor"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Documentaþia variabilelor"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { return "Membri"; }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    virtual QCString trFiles()
    { return "Fiºiere"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generat la "+date;
      if (projName) result+=(QCString)" pentru "+projName;
      result+=(QCString)" de cãtre";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "scris de";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagrama de relaþii pentru "+clName;
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Doar pentru uz intern."; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "Reimplementat din motive interne; API-ul nu este afectat."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Atenþie"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "Buguri ºi limitãri"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versiunea"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \\author command is used. */
    virtual QCString trAuthors()
    { return "Autorul/Autorii"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Întoarce"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Vezi ºi"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Excepþii"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generat de"; }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Lista Namespace"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Aici este lista tuturor ";
      result+="namespace-urilor ";
	  if (!extractAll) result+="documentate ";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Prieteni"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Documentaþia funcþiilor prietene sau înrudite"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
	  QCString result="Referinþã la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasa"; break;
        case ClassDef::Struct:     result+="structura"; break;
        case ClassDef::Union:      result+="uniunea"; break;
        case ClassDef::Interface:  result+="interfaþa"; break;
        case ClassDef::Exception:  result+="excepþia"; break;
      }
	  if (isTemplate) result+=" (Template) ";
	  result+=(QCString)clName;
      
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result="Referinþã la fiºierul";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Referinþã la Namespace-ul ";
      result+=namespaceName;
      return result;
    }
    
    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union 
     */
    virtual QCString trPublicMembers()
    { return "Metode Publice"; }
    virtual QCString trPublicSlots()
    { return "Sloturi Publice"; }
    virtual QCString trSignals()
    { return "Semnale"; }
    virtual QCString trStaticPublicMembers()
    { return "Metode Statice Publice"; }
    virtual QCString trProtectedMembers()
    { return "Metode Protejate"; }
    virtual QCString trProtectedSlots()
    { return "Sloturi Protejate"; }
    virtual QCString trStaticProtectedMembers()
    { return "Metode Statice Protejate"; }
    virtual QCString trPrivateMembers()
    { return "Metode Private"; }
    virtual QCString trPrivateSlots()
    { return "Sloturi Private"; }
    virtual QCString trStaticPrivateMembers()
    { return "Metode Statice Private"; }
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
            result+=" ºi ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Moºteneºte "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Moºtenit de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementat din "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementat în "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Membrii Namespace-ului"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Aici este lista tuturor membrilor ";
      if (!extractAll) result+="documentaþi ";
	  result+="din toate namespace-urile, cu legãturi cãtre ";
		  
      if (extractAll) 
        result+="documentaþia namespace-ului pentru fiecare membru în parte:";
      else 
        result+="namespace-urile de care aparþin:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Indexul Namespace-ului"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Documentaþia Namespace-ului"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namespace-uri"; }

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
      QCString result=(QCString)"Documentaþia pentru aceastã ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasã"; break;
        case ClassDef::Struct:     result+="structurã"; break;
        case ClassDef::Union:      result+="uniune"; break;
        case ClassDef::Interface:  result+="interfaþã"; break;
        case ClassDef::Exception:  result+="excepþie"; break;
      }
      result+=" a fost generatã din fiºier";
      if (single) result+="ul:"; else result+="ele:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Listã Alfabeticã"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Valori returnate"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Pagina principalã"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "pg."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "Surse";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definiþia în linia @0 a fiºierului @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definiþia în fiºierul @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Învechitã(Deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagrama de relaþii pentru "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graful dependenþelor prin incluziune pentru "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentaþia pentru Constructori ºi Destructori"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Vezi sursele.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Vezi documentaþia.";
    }
    /*! Text for the \pre command */
    virtual QCString trPrecondition()
    {
      return "Precondiþie";
    }
    /*! Text for the \post command */
    virtual QCString trPostcondition()
    {
      return "Postcondiþie";
    }
    /*! Text for the \invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Iniþializare:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "cod";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Ierarhia claselor în mod grafic";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Vezi ierarhia claselor în mod grafic";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Vezi ierarhia claselor în mod text";
    }
    virtual QCString trPageIndex()
    {
      return "Indexul Paginilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Notã";
    }
    virtual QCString trPublicTypes()
    {
      return "Tipuri publice";
    }
    virtual QCString trPublicAttribs()
    {
      return "Atribute Publice";
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Atribute Statice Publice";
    }
    virtual QCString trProtectedTypes()
    {
      return "Tipuri Protejate";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Atribute Protejate";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Atribute Statice Protejate";
    }
    virtual QCString trPrivateTypes()
    {
      return "Tipuri Private";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Atribute Private";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Atribute Statice Private";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "De fãcut";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Lista lucrurilor rãmase de fãcut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Semnalat de";
    }
    virtual QCString trRemarks()
    {
      return "Observaþii";
    }
    virtual QCString trAttention()
    {
      return "Atenþie";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Acest graf aratã care fiºiere includ, "
		  "direct sau indirect, acest fiºier:";
    }
    virtual QCString trSince()
    {
      return "Din";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda grafului";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return 
        "Aceastã paginã aratã modul în care trebuie sã interpretaþi "
		"grafurile generate de doxygen.<p>\n"
        "Consideraþi urmãtorul exemplu:\n"
        "\\code\n"
        "/*! Clasã invizibilã, tãiatã din cauza depãºirii spaþiului */\n"
        "class Invisible { };\n\n"
        "/*! Altã clasã tãiatã, relaþia de moºtenire este ascunsã */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Clasã necomentatã în stil doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Clasã care este moºtenitã în mod public */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Clasã care este moºtenitã în mod protejat */\n"
        "class ProtectedBase { };\n\n"
        "/*! Clasã care este moºtenitã în mod privat */\n"
        "class PrivateBase { };\n\n"
        "/*! Clasã care este folositã de clasa Inherited */\n"
        "class Used { };\n\n"
        "/*! Superclasã care moºteneºte un numãr de alte clase */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Dacã tagul \\c MAX_DOT_GRAPH_HEIGHT din fiºierul de configuraþie "
        "Este setat la 200 acesta este graful rezultat:"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "Cãsuþele din partea de sus au urmãtoarea semnificaþie:\n"
        "<ul>\n"
        "<li>O cãsuþã neagrã reprezintã structura sau clasa pentru care "
        "graful este generat.\n"
        "<li>O cãsuþã cu marginea neagrã reprezintã o structurã sau o clasã documentate.\n"
        "<li>O cãsuþã cu marginea gri reprezintã o structurã sau o clasã nedocumentate.\n"
        "<li>O cãsuþã cu marginea roºie reprezintã o structurã sau o clasã documentate, pentru\n"
        "care nu toate relaþiile de moºtenire/incluziune sunt arãtate. Un graf este "
        "tãiat dacã nu încape în marginile specificate."
        "</ul>\n"
        "Sãgeþile au urmãtoarea semnificaþie:\n"
        "<ul>\n"
        "<li>O sãgeatã de un albastru închis este folositã când avem o relaþie de "
        "moºtenire publicã între douã clase.\n"
        "<li>O sãgeatã de un verde închis este folositã când avem o moºtenire protejatã.\n"
        "<li>O sãgeatã de un roºu închis este folositã când avem o moºtenire privatã.\n"
        "<li>O sãgeatã violetã punctatã este folositã pentru o clasã conþinutã sau folositã "
        "de o altã clasã. Sãgeata este marcatã cu variabila(e) "
        "prin care este accesibilã clasa sau structura spre care este îndreptatã. \n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "legenda";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Listã de teste";
    }

};

#endif
