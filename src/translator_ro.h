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

/* The translation from English to Romanian by Alexandru Iosup [aiosup@yahoo.com].
 *
 * Disclaimer: I hope I translated these to Romanian keeping
 * the meaning of the sentences intact. I used C-words were possible,
 * (e.g. the Romanian "definire de tip" for "typedef" is too long and 
 * unexpressive).
 *
 * If you have suggestions, please mail the comments and text proposals to the address 
 * shown aprox.10 lines above
 * 
 * -------------------------------------------
 * Project start                : 20.Sep.2k
 * Last Doxygen version covered : 1.4.1
 * Last revision                : 01.Mar.2k5
 * -------------------------------------------
 *
 * Revision history
 * ----------------
 * 01.Mar.2k5  Third revision, covering Doxygen 1.4.1
 *
 * 07.Mar.2k2  Second revision, covering Doxygen 1.2.14
 *             - fixed some bugs
 *
 * 20.Sep.2k   First version, covering Doxygen 1.2.1
 *
 */
#ifndef TRANSLATOR_RO_H
#define TRANSLATOR_RO_H


class TranslatorRomanian : public TranslatorAdapter_1_4_1
{
  public:

    // --- Language control methods -------------------
    
    /*! Used for identification of the language. The identification 
     * should not be translated. It should be replaced by the name 
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to 
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "romanian"; }
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
      return "\\usepackage[romanian]{babel}\n";
    }

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
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentaþia Câmpurilor"; 
      }
      else
      {
        return "Documentaþia Datelor membre"; 
      }

    }

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
    { return "Lista completã a membrilor din "; }

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

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Module"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Ierarhia Claselor"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Structuri de Date";
      }
      else
      {
        return "Lista Membrilor Componenþi"; 
      }

    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista fiºierelor"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "Fiºiere Header"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Câmpurile de Date"; 
      }
      else
      {
        return "Membrii Componenþi"; //cu articol hotarat
      }

    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globale"; 
      }
      else
      {
        return "Membrii din Fiºier"; //cu articol hotarat
      }

    }

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
    { return "Aceastã listã de legãturi este sortatã în general, "
             "dar nu complet, în ordine alfabeticã:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Lista tuturor ";
      result+="fiºierelor";
	  if (!extractAll) result+=" documentate";
	  result+=", cu scurte descrieri:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Lista structurilor de date, cu scurte descrieri:"; 
      }
      else
      {
        return "Lista claselor, structurilor, uniunilor ºi interfeþelor"
		", cu scurte descrieri:"; 
      }

    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Lista tuturor ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="câmpurilor ";
        if (!extractAll) result+=" documentate ";
        result+="din structuri si uniuni ";
      }
      else
      {
        result+="membrilor ";
	  if (!extractAll) result+="documentaþi ";
        result+="din toate clasele ";
      }
      result+=", cu legãturi cãtre ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="documentaþia structurii/uniunii pentru fiecare câmp în parte:";
        }
        else
        {
          result+="documentaþia clasei pentru fiecare membru în parte:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="structurile/uniunile de care aparþin:";
        }
        else
        {
          result+="clasele de care aparþin:";
        }
      }
     
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Lista tuturor ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="funcþiilor, variabilelor, define-urilor, enum-urilor ºi typedef-urilor";
	  if (!extractAll) result+=" documentate";
      }
      else
      {
        result+="membrilor ";
	  if (!extractAll) result+="documentaþi ";
	  result+="din toate fiºierele";
      }
      result+=", cu legãturi cãtre ";
      if (extractAll) 
        result+="fiºierele de care aparþin:";
      else 
        result+="documentaþia aferentã:";

      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "Lista fiºierele Header care fac parte din API:"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Lista tuturor exemplelor:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Lista tuturor documentaþiilor înrudite:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Lista tuturor modulelor:"; }

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
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Indexul Structurilor de Date";
      }
      else
      {
        return "Indexul Claselor"; 
      }

    }

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
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentaþia Structurilor de Date"; 
      }
      else
      {
        return "Documentaþia Claselor"; 
      }

    }

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
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Structuri de Date"; 
      }
      else
      {
        return "Membri"; 
      }

    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generat "+date;
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
      QCString result="Lista tuturor ";
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
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
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
    
    /* these are for the member sections of a class, struct or union */
    virtual QCString trPublicMembers()
    { return "Metode Publice"; }
    virtual QCString trPublicSlots()
    { return "Conectori (slots) Publici"; }
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
    { return "Conectori (slots) Privaþi"; }
    virtual QCString trStaticPrivateMembers()
    { return "Metode Statice Private"; }
    
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
      QCString result="Lista tuturor membrilor ";
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
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
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
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondiþie";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondiþie";
    }
    /*! Text for the \\invariant command */
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
      return "Tipuri Publice";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Câmpuri de Date";
      }
      else
      {
        return "Atribute Publice";
      }
      
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
      return "Lista lucrurilor de fãcut";
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
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
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
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Dacã tagul \\c MAX_DOT_GRAPH_HEIGHT din fiºierul de configuraþie "
        "Este setat la 200 acesta este graful rezultat:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Metode DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Proprietãþi";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentaþia Proprietãþilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Interfeþe";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Structuri de Date";
      }
      else
      {
        return "Clase";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Pachet "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Lista Pachetelor";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Lista pachetelor, însoþitã de scurte explicaþii, acolo unde acestea existã:";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pachete";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Documentaþia Pachetelor";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Valoare:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Problema (Bug)";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Lista de Probleme (Bugs)";
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
      return "1250"; //EASTEUROPE_CHARSET [don't know if this is correct:Ro has a 0x418 index in that file]
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "0x418"; // well, I could not find a decent doc about this one - I think Ro code is 0x418
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Index";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Clas" : "clas"));
	result+= singular ? "a":"ele";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Fiºier" : "fiºier"));
	result+= singular ? "ul":"ele";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Namespace" : "namespace"));
	result+= singular ? "-ul":"-urile";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Grupu" : "grupu"));
	result+= singular ? "l":"rile";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Pagin" : "pagin"));
	result+= singular ? "a":"ile";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Membr" : "membr"));
	result+= singular ? "ul":"ii";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Câmp" : "câmp"));
	result+= singular ? "ul":"urile";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="e";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Autor" : "autor"));
	result+= singular ? "ul":"ii";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referinþe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementeazã "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementat în "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Conþinut";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista elementelor învechite (deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Evenimente";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Documentaþia aferentã evenimentelor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Tipuri în pachet";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Funcþii în pachet";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Funcþii statice în pachet";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Atribute în pachet";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Atribute statice în pachet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Toate";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Graful de apel al acestei funcþii:";
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
      return "Cautã";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Rezultatele cãutarii";
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
        return "Din pacate nu am gãsit nici un document care sã corespundã cererii.";
      }
      else if (numDocuments==1)
      {
        return "Am gãsit <b>1</b> document corespunzând cererii.";
      }
      else 
      {
        return "Am gãsit <b>$num</b> documente corespunzând cererii. "
               "Lista documentelor gãsite, sortate dupã relevanþã.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Gãsite:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

     /*! This is used in HTML as the title of page with source code for file filename
      */
     virtual QCString trSourceFile(QCString& filename)
     {
       return " Fiºierul sursã " + filename;
     }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

     /*! This is used as the name of the chapter containing the directory
      *  hierarchy.
      */
     virtual QCString trDirIndex()
     { return "Ierarhia directoarelor"; }

     /*! This is used as the name of the chapter containing the documentation
      *  of the directories.
      */
     virtual QCString trDirDocumentation()
     { return "Documentaþia directoarelor"; }

     /*! This is used as the title of the directory index and also in the
      *  Quick links of a HTML page, to link to the directory hierarchy.
      */
     virtual QCString trDirectories()
     { return "Directoare"; }

     /*! This returns a sentences that introduces the directory hierarchy.
      *  and the fact that it is sorted alphabetically per level
      */
     virtual QCString trDirDescription()
     { return "Aceastã ierarhie de directoare este sortatã, "
              "per nivel, in ordine alfabeticã:";
     }

     /*! This returns the title of a directory page. The name of the
      *  directory is passed via \a dirName.
      */
     virtual QCString trDirReference(const char *dirName)
     { 
       QCString  result="Director-referinþã "; result+=dirName; 
       return result; 
     }

     /*! This returns the word directory with or without starting capital
      *  (\a first_capital) and in sigular or plural form (\a singular).
      */
     virtual QCString trDir(bool first_capital, bool singular)
     {
       QCString result((first_capital ? "Directo" : "directo"));
       if (singular) result+="r"; else result="are";
       return result;
     }

};

#endif
