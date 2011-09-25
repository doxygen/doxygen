/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

/* Original translation from English to Romanian by Alexandru Iosup [aiosup@yahoo.com].
 * Updated by Ionuţ Dumitraşcu [reddumy@yahoo.com]
 *
 * -------------------------------------------
 * Project start                : 20.09.2000
 * Last Doxygen version covered : 1.5.8
 * Last revision                : 15.01.2009
 * -------------------------------------------
 *
 * Revision history
 * ----------------
 * 15.01.2009 - Updated Romanian translation to Doxygen 1.5.8 and modified strings to UTF-8, as well as some other changes (Ionuţ Dumitraşcu)
 * 28.07.2008 - Updated version - covering Doxygen 1.5.6 - and some minor changes  (Ionuţ Dumitraşcu)
 *
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


class TranslatorRomanian : public TranslatorAdapter_1_6_0
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
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funcţii înrudite"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Atenţie: acestea nu sunt funcţii membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descriere Detaliată"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Documentaţia Definiţiilor de Tipuri (typedef) Membre"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Documentaţia Enumerărilor Membre"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Documentaţia Funcţiilor Membre"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentaţia Câmpurilor"; 
      }
      else
      {
        return "Documentaţia Datelor Membre"; 
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
    { return "Lista completă a membrilor din "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", inclusiv a tuturor membrilor moşteniţi."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generat automat de Doxygen";
      if (s) result+=(QCString)" pentru "+s;
      result+=" din codul sursă."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nume enumerare"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "valoare enumerare"; }
    
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
        return "Lista Claselor"; 
      }

    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista fişierelor"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Câmpurile de Date"; 
      }
      else
      {
        return "Membrii Componenţi"; //cu articol hotarat
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
        return "Membrii din Fişier"; //cu articol hotarat
      }

    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Pagini înrudite"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Exemple"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Caută"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Această listă de moşteniri este sortată în general, "
             "dar nu complet, în ordine alfabetică:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Lista tuturor ";
      result+="fişierelor";
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
        return "Lista claselor, structurilor, uniunilor şi interfeţelor"
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
	  if (!extractAll) result+="documentaţi ";
        result+="din toate clasele ";
      }
      result+=", cu legături către ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="documentaţia structurii/uniunii pentru fiecare câmp în parte:";
        }
        else
        {
          result+="documentaţia clasei pentru fiecare membru în parte:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="structurile/uniunile de care aparţin:";
        }
        else
        {
          result+="clasele de care aparţin:";
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
        result+="funcţiilor, variabilelor, definiţiilor, enumerărilor şi definiţiilor de tip";
	  if (!extractAll) result+=" documentate";
      }
      else
      {
        result+="membrilor ";
	  if (!extractAll) result+="documentaţi ";
	  result+="din toate fişierele";
      }
      result+=", cu legături către ";
      if (extractAll) 
        result+="fişierele de care aparţin:";
      else 
        result+="documentaţia aferentă:";

      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Lista tuturor exemplelor:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Lista tuturor documentaţiilor înrudite:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Lista tuturor modulelor:"; }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Documentaţie"; }

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
    { return "Indexul Fişierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Documentaţia Modulelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentaţia Structurilor de Date"; 
      }
      else
      {
        return "Documentaţia Claselor"; 
      }

    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Documentaţia Fişierelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Documentaţia Exemplelor"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Documentaţii înrudite"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Manual de utilizare"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definiţii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototipuri de funcţii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definiţii de tipuri"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerări"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funcţii"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variabile"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Valori de enumerări"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Documentaţia definiţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Documentaţia prototipurilor de funcţii"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Documentaţia definiţiilor de tipuri"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Documentaţia enumerărilor"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Documentaţia funcţiilor"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Documentaţia variabilelor"; }

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
      result+=(QCString)" de către";
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
      return (QCString)"Diagrama de relaţii pentru "+clName;
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Doar pentru uz intern."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Atenţie"; }

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
    { return "Vezi şi"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametri"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Excepţii"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generat de"; }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Lista de Namespace-uri"; }

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
    { return "Documentaţia funcţiilor prietene sau înrudite"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
	  QCString result="Referinţă la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasa "; break;
        case ClassDef::Struct:     result+="structura "; break;
        case ClassDef::Union:      result+="uniunea "; break;
        case ClassDef::Interface:  result+="interfaţa "; break;
        case ClassDef::Protocol:   result+="protocolul "; break;
        case ClassDef::Category:   result+="categoria "; break;
        case ClassDef::Exception:  result+="excepţia "; break;
      }
	  if (isTemplate) result+=" (Template) ";
	  result+=(QCString)clName;
      
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result="Referinţă la fişierul ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Referinţă la Namespace-ul ";
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
    { return "Conectori (slots) Protejaţi"; }
    virtual QCString trStaticProtectedMembers()
    { return "Metode Statice Protejate"; }
    virtual QCString trPrivateMembers()
    { return "Metode Private"; }
    virtual QCString trPrivateSlots()
    { return "Conectori (slots) Privaţi"; }
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
            result+=" şi ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Moşteneşte "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Moştenit de "+trWriteList(numEntries)+".";
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
      if (!extractAll) result+="documentaţi ";
	  result+="din toate namespace-urile, cu legături către ";
		  
      if (extractAll) 
        result+="documentaţia namespace-ului pentru fiecare membru în parte:";
      else 
        result+="namespace-urile de care aparţin:";
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
    { return "Documentaţia Namespace-ului"; }

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
      QCString result=(QCString)"Documentaţia pentru această ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clasă"; break;
        case ClassDef::Struct:     result+="structură"; break;
        case ClassDef::Union:      result+="uniune"; break;
        case ClassDef::Interface:  result+="interfaţă"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="categorie"; break;
        case ClassDef::Exception:  result+="excepţie"; break;
      }
      result+=" a fost generată din fişier";
      if (single) result+="ul:"; else result+="ele:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Listă Alfabetică"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Valori returnate"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Pagina principală"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "pg."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definiţia în linia @0 a fişierului @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definiţia în fişierul @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Învechită(Deprecated)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagrama de relaţii pentru "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graful dependenţelor prin incluziune pentru "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentaţia pentru Constructori şi Destructori"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Vezi sursele.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Vezi documentaţia.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondiţie";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondiţie";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Valoarea iniţială:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "cod";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Ierarhia Claselor în mod grafic";
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
      return "Notă";
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
      return "De făcut";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Lista lucrurilor de făcut";
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
      return "Observaţii";
    }
    virtual QCString trAttention()
    {
      return "Atenţie";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Acest graf arată care fişiere includ, "
		  "direct sau indirect, acest fişier:";
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
        "Această pagină arată modul în care trebuie să interpretaţi "
		"grafurile generate de doxygen.<p>\n"
        "Consideraţi următorul exemplu:\n"
        "\\code\n"
        "/*! Clasă invizibilă, tăiată din cauza depăşirii spaţiului */\n"
        "class Invisible { };\n\n"
        "/*! Altă clasă tăiată, relaţia de moştenire este ascunsă */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Clasă necomentată în stil doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Clasă care este moştenită în mod public */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Clasă template */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Clasă care este moştenită în mod protejat */\n"
        "class ProtectedBase { };\n\n"
        "/*! Clasă care este moştenită în mod privat */\n"
        "class PrivateBase { };\n\n"
        "/*! Clasă care este folosită de clasa Inherited */\n"
        "class Used { };\n\n"
        "/*! Superclasă care moşteneşte un număr de alte clase */\n"
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
        "Dacă tagul \\c MAX_DOT_GRAPH_HEIGHT din fişierul de configurare "
        "este setat la 200, acesta este graful rezultat:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Căsuţele din partea de sus au următoarea semnificaţie:\n"
        "<ul>\n"
        "<li>O căsuţă neagră reprezintă structura sau clasa pentru care "
        "graful este generat.\n"
        "<li>O căsuţă cu marginea neagră reprezintă o structură sau o clasă documentate.\n"
        "<li>O căsuţă cu marginea gri reprezintă o structură sau o clasă nedocumentate.\n"
        "<li>O căsuţă cu marginea roşie reprezintă o structură sau o clasă documentate, pentru\n"
        "care nu toate relaţiile de moştenire/incluziune sunt arătate. Un graf este "
        "tăiat dacă nu încape în marginile specificate."
        "</ul>\n"
        "Săgeţile au următoarea semnificaţie:\n"
        "<ul>\n"
        "<li>O săgeată de un albastru închis este folosită când avem o relaţie de "
        "moştenire publică între două clase.\n"
        "<li>O săgeată de un verde închis este folosită când avem o moştenire protejată.\n"
        "<li>O săgeată de un roşu închis este folosită când avem o moştenire privată.\n"
        "<li>O săgeată violetă punctată este folosită pentru o clasă conţinută sau folosită "
        "de o altă clasă. Săgeata este marcată cu variabila(e) "
        "prin care este accesibilă clasa sau structura spre care este îndreptată. \n"
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
      return "Listă de teste";
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
      return "Proprietăţi";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentaţia Proprietăţilor";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

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
      return "Lista pachetelor, însoţită de scurte explicaţii, acolo unde acestea există:";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pachete";
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
      return "1250"; //EASTEUROPE_CHARSET
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "238";
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
      QCString result((first_capital ? "Fişier" : "fişier"));
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
      return "Referinţe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementează "+trWriteList(numEntries)+".";
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
      return "Cuprins";
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
      return "Documentaţia aferentă evenimentelor";
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
      return "Funcţii în pachet";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Funcţii statice în pachet";
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
      return "Graful de apel al acestei funcţii:";
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
      return "Caută";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Rezultatele căutarii";
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
        return "Din păcate nu am găsit nici un document care să corespundă cererii.";
      }
      else if (numDocuments==1)
      {
        return "Am găsit <b>1</b> document corespunzând cererii.";
      }
      else 
      {
        return "Am găsit <b>$num</b> documente corespunzând cererii. "
               "Lista documentelor găsite, sortate după relevanţă.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Găsite:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

     /*! This is used in HTML as the title of page with source code for file filename
      */
     virtual QCString trSourceFile(QCString& filename)
     {
       return " Fişierul sursă " + filename;
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
     { return "Documentaţia directoarelor"; }

     /*! This is used as the title of the directory index and also in the
      *  Quick links of a HTML page, to link to the directory hierarchy.
      */
     virtual QCString trDirectories()
     { return "Directoare"; }

     /*! This returns a sentences that introduces the directory hierarchy.
      *  and the fact that it is sorted alphabetically per level
      */
     virtual QCString trDirDescription()
     { return "Această ierarhie de directoare este sortată în general, "
              "dar nu complet, în ordine alfabetică:";
     }

     /*! This returns the title of a directory page. The name of the
      *  directory is passed via \a dirName.
      */
     virtual QCString trDirReference(const char *dirName)
     { 
       QCString  result="Director-referinţă "; result+=dirName; 
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

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "Aceasta este o funcţie membră suprascrisă. "
              "Diferă de funcţia de mai sus "
              "doar prin argumentele acceptate.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Graful de apeluri pentru această funcţie:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentaţia Enumeratorilor"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentaţia Funcţiei Membre/Subrutinei"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Lista Tipurilor de Date"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Câmpuri de date"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Tipurile de date, cu scurte descrieri:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Lista tuturor tipurilor de date ";
      if (!extractAll)
      {
        result+="documentate ";
      }
      result+=" cu legături către ";
      if (!extractAll) 
      {
         result+="documentaţia structurii de date pentru fiecare membru";
      }
      else 
      {
         result+="tipurile de date de care aparţin:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Indexul Tipurilor de Date"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentaţia Tipurilor de Date"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funcţii/Subrutine"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentaţia Funcţiilor/Subrutinelor"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tipuri de Date"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Lista Modulelor"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Lista tuturor modulelor ";
      if (!extractAll) result+="documentate ";
      result+="cu scurte descrieri:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referinţă la ";
      switch(compType)
      {
        case ClassDef::Class:      result+="Modulul "; break;
        case ClassDef::Struct:     result+="Tipul "; break;
        case ClassDef::Union:      result+="Uniunea "; break;
        case ClassDef::Interface:  result+="Interfaţa "; break;
        case ClassDef::Protocol:   result+="Protocolul "; break;
        case ClassDef::Category:   result+="Categoria "; break;
        case ClassDef::Exception:  result+="Excepţia "; break;
      }
      if (isTemplate) result+="(Template) ";
	  result+=(QCString)clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Referinţă la Modulul ";
	  result += namespaceName;
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Membrii Modulului"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Lista tuturor membrilor ";
      if (!extractAll) result+="documentaţi ai ";
      result+="modulului cu legături către ";
      if (extractAll) 
      {
        result+="documentaţia modulului pentru fiecare membru:";
      }
      else 
      {
        result+="modulele de care aparţin:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Indexul Modulelor"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Modul" : "modul"));
      if (singular)  result+="ul";
	  else result += "ele";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Documentaţia ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modulului"; break;
        case ClassDef::Struct:     result+="tipului"; break;
        case ClassDef::Union:      result+="uniunii"; break;
        case ClassDef::Interface:  result+="interfeţei"; break;
        case ClassDef::Protocol:   result+="protocolului"; break;
        case ClassDef::Category:   result+="categoriei"; break;
        case ClassDef::Exception:  result+="excepţiei"; break;
      }
      result+=" a fost generată din următo";
      if (single) result+="rul fişier:"; else result+="arele fişiere:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tip" : "tip"));
      if (singular)  result+="ul";
	  else result += "urile";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Subprogram" : "subprogram"));
      if (singular)  result+="ul";
	  else result += "urile";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Constrângerile de Tip";
    }
	 
};

#endif
