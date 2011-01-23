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

#ifndef TRANSLATOR_CA_H
#define TRANSLATOR_CA_H

/*! 
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionaly to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorCatalan : public TranslatorAdapter_1_6_3
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
    { return "catalan"; }
    
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
      //return "\\usepackage[catalan]{babel}\n\\usepackage[latin1]{inputenc}";
      return "\\usepackage[catalan]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funcions Associades"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Remarcar que aquestes funcions no són funcions membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descripció Detallada"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Documentació de les Definicions de Tipus Membre"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Documentació de les Enumeracions Membre"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Documentació de les Funcions Membre"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentació dels Camps"; 
      }
      else
      {
        return "Documentació de les Dades Membre"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Més..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Llista de tots els membres."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Llista dels Membres"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Aquesta és la llista complerta dels membres de "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", incloent tots els membres heretats."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generat automàticament per Doxygen";
      if (s) result+=(QCString)" per a "+s;
      result+=" a partir del codi font."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nom de la enum"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "valors de la enum"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definit a"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Mòduls"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Jerarquia de Classes"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estructures de Dades";
      }
      else
      {
        return "Llista de Classes"; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Llista dels Fitxers"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Camps de Dades"; 
      }
      else
      {
        return "Membres de Classes"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globals"; 
      }
      else
      {
        return "Membres de Fitxers";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Pàgines Relacionades"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Exemples"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Cerca"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Aquesta llista d'herència està ordenada toscament, "
             "però no completa, de forma alfabètica:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Aquesta és la llista de tots els fitxers ";
      if (!extractAll) result+="documentats ";
      result+="acompanyats amb breus descripcions:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Aquestes són les estructures de dades acompanyades amb breus descripcions:"; 
      }
      else
      {
        return "Aquestes són les classes, estructures, "
               "unions i interfícies acompanyades amb breus descripcions:"; 
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Aquesta és la llista de tots els ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="camps d'estructures i unions";
      }
      else
      {
        result+="membres de classe";
      }
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="la documentació de l'estructura/unió per a cada camp:";
        }
        else
        {
          result+="la documentació de la classe per a cada membre:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="les estructures/unions a que pertanyen:";
        }
        else
        {
          result+="les classes a que pertanyen:";
        }
      }
      return result;
    }
    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Aquesta és la llista de ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="totes les funcions, variables, definicions, enumeracions, i definicions de tipus";
        if (!extractAll) result+=" documentades";
      }
      else
      {
        result+="tots els membres de fitxers";
        if (!extractAll) result+=" documentats";
      }
      result+=" amb enllaços ";
      if (extractAll) 
        result+="als fitxers als quals corresponen:";
      else 
        result+="a la documentació:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Aquesta és la llista de tots els exemples:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Aquesta és la llista de totes les pàgines de documentació associades:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Aquesta és la llista de mòduls:"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return ": Documentació"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Índex de Mòduls"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Índex Jeràrquic"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Índex d'Estructures de Dades";
      }
      else
      {
        return "Índex de Classes"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Índex de Fitxers"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Documentació dels Mòduls"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Documentació de les Estructures de Dades";
      }
      else
      {
        return "Documentació de les Classes"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Documentació dels Fitxers"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Documentació dels Exemples"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Documentació de les Pàgines"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Manual de Referència"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definicions"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototipus de Funcions"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definicions de Tipus"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumeracions"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funcions"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variables"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Valors de les Enumeracions"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Documentació de les Definicions"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Documentació de les Funcions Prototipus"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Documentació de les Definicions de Tipus"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Documentació dels Tipus de les Enumeracions"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Documentació de les Funcions"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Documentació de les Variables"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estructures de Dades"; 
      }
      else
      {
        return "Classes";
      }
    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generat a "+date;
      if (projName) result+=(QCString)" per a "+projName;
      result+=(QCString)" per";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "escrit per";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagrama d'Herència per a "+clName+":";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Tan sols per a ús intern."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Atenció"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versió"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Retorna"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Mireu també"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Paràmetres"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Excepcions"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generat per"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Llista dels Espais de Noms"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Aquests són tots els espais de noms ";
      if (!extractAll) result+="documentats ";
      result+="amb breus descripcions:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Classes Amigues"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Documentació de funcions amigues i relacionades"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referència de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" la Classe "; break;
        case ClassDef::Struct:     result+=" l'Estructura "; break;
        case ClassDef::Union:      result+=" la Unió "; break;
        case ClassDef::Interface:  result+=" la Interfície "; break;
        case ClassDef::Protocol:   result+="l Protocol "; break;
        case ClassDef::Category:   result+=" la Categoria "; break;
        case ClassDef::Exception:  result+=" l'Excepció "; break;
      }
      if (isTemplate) result+="Template ";
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result="Referència del Fitxer ";
      result+=fileName; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Referència de l'Espai de Noms ";
      result+=namespaceName;
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Mètodes públics"; }
    virtual QCString trPublicSlots()
    { return "Slots públics"; }
    virtual QCString trSignals()
    { return "Senyals"; }
    virtual QCString trStaticPublicMembers()
    { return "Mètodes Públics Estàtics"; }
    virtual QCString trProtectedMembers()
    { return "Mètodes Protegits"; }
    virtual QCString trProtectedSlots()
    { return "Slots Protegits"; }
    virtual QCString trStaticProtectedMembers()
    { return "Mètodes Protegits Estàtics"; }
    virtual QCString trPrivateMembers()
    { return "Mètodes Privats"; }
    virtual QCString trPrivateSlots()
    { return "Slots Privats"; }
    virtual QCString trStaticPrivateMembers()
    { return "Mètodes Privats Estàtics"; }
    
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
            result+=" i ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Hereta de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Heretat per "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementat de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementat a "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Membres de l'Espai de Noms"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Aquesta és la llista de tots els membres de l'espai de noms ";
      if (!extractAll) result+="documentats ";
      result+="amb enllaços a ";
      if (extractAll) 
        result+="la documentació de l'espai de noms de cada membre:";
      else 
        result+="l'espai de noms al qual corresponen:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Índex d'Espais de Noms"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Documentació de l'Espai de Noms"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Espais de Noms"; }

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
      QCString result=(QCString)"La documentació d'aquest";
      switch(compType)
      {
        case ClassDef::Class:      result+="a classe"; break;
        case ClassDef::Struct:     result+="a estructura"; break;
        case ClassDef::Union:      result+="a unió"; break;
        case ClassDef::Interface:  result+="a interfície"; break;
        case ClassDef::Protocol:   result+=" protocol"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a excepció"; break;
      }
      result+=" es va generar a partir del";
      if (!single) result+="s";
      result+=" següent";
      if (!single) result+="s";
      result+=" fitxer";
      if (!single) result+="s:"; else result+=":";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Llista Alfabètica"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Valors de retorn"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Pàgina principal"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definició a la línia @0 del fitxer @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definició al fitxer @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Antiquat";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagrama de col·laboració per a "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Inclou el graf de dependències per a "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentació del Constructor i el Destructor"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Veure el codi d'aquest fitxer.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Veure la documentació d'aquest fitxer.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondició";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondició";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Valor inicial:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "codi";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Jerarquia Gràfica de la Classe";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Veure la jerarquia gràfica de la classe";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Veure la jerarquia textual de la classe";
    }
    virtual QCString trPageIndex()
    {
      return "Índex de Pàgines";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Nota";
    }
    virtual QCString trPublicTypes()
    {
      return "Tipus Públics";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Camps de Dades";
      }
      else
      {
        return "Atributs Públics";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Atributs Públics Estàtics";
    }
    virtual QCString trProtectedTypes()
    {
      return "Tipus Protegits";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Atributs Protegits";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Atributs Protegits Estàtics";
    }
    virtual QCString trPrivateTypes()
    {
      return "Tipus Privats";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Atributs Privats";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Atributs Privats Estàtics";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Per fer";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Llista de coses per fer";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referenciat a";
    }
    virtual QCString trRemarks()
    {
      return "Remarca";
    }
    virtual QCString trAttention()
    {
      return "Atenció";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Aquest gràfic mostra quins fitxers inclouen, "
             "de forma directa o indirecta, aquest fitxer:";
    }
    virtual QCString trSince()
    {
      return "Des de";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Llegenda del Gràfic";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "Aquesta pàgina explica com s'interpreten els gràfics generats per doxygen.<p>\n"
        "Considera aquest exemple:\n"
        "\\code\n"
        "/*! Classe invisible per culpa del retall */\n"
        "class Invisible { };\n\n"
        "/*! Classe truncada, l'herència està amagada */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Classe no documentada amb comentaris doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Classe heredada amb herència pública */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Una classe Template */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Classe heredada utilitzant herència protegida */\n"
        "class ProtectedBase { };\n\n"
        "/*! Classe heredada utiltzant herència privada */\n"
        "class PrivateBase { };\n\n"
        "/*! Classe usada per la classe heretada */\n"
        "class Used { };\n\n"
        "/*! Super classe que hereda una quantitat de classes */\n"
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
        "Resultarà el gràfic següent:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Les caixes del gràfic superior tenen aquesta interpretació:\n"
        "<ul>\n"
        "<li>Una caixa negra plena represent l'estructura o classe per la qual el gràfic s'ha generat.\n"
        "<li>Una caixa de vora negra representa una estructura o classe documentada.\n"
        "<li>Una caixa de vora verda representa una estructura o classe indocumentada.\n"
        "<li>Una caixa de vora vermalla representa una estructura o classe documentada de la qual "
        "no es mostren totes les relacions d'herència/inclusió. Un gràfic és truncat si no s'ajusta als límits.\n"
        "</ul>\n"
        "Les sagetes tenen aquest significat:\n"
        "<ul>\n"
        "<li>Una sageta blau fosc remarca una relació d'herència de tipus pública entre dues classes.\n"
        "<li>Una sageta verd fosc remarca una relació d'herència de tipus protegida entre dues classes.\n"
        "<li>Una sageta roig fosc remarca una relació d'herència de tipus privada entre dues classes.\n"
        "<li>Una sageta puntejada de color porpra indica que una classe és continguda o usada per una altra classe."
        " La sageta s'etiqueta amb la variable o variables a través de les quals la classe o estructura apuntada és accessible.\n"
        "<li>Una sageta puntejada de color groc indica la relació entre una instància template i la classe template de què ha set instanciada."
        " La sageta s'etiqueta amb els paràmetres template de la instància.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "llegenda";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Prova";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Llista de proves";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Mètodes DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Propietats";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentació de les Propietats";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estructures de Dades";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paquet "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Llista de Paquets";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Aquesta és la llista de paquets, amb una breu descripció (si se'n disposa):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paquets";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Valor:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Error";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Llista d'Errors";
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
      return "Índex";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Classe" : "classe"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Fitxer" : "fitxer"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Grup" : "grup"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Pàgin" : "pàgin"));
      if (!singular)  result+="es"; else result+="a";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Membre" : "membre"));
      if (!singular)  result+="s";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
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
      QCString result((first_capital ? "Autor" : "autor"));
      if (!singular)  result+="s";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referències";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementat a "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Taula de Continguts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Llista d'Antiquats";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Esdeveniments";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Documentació dels Esdeveniments";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Tipus de paquets";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Funcions de Paquet";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Funcions Estàtiques de Paquet";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Atributs de Paquet";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Atributs Estàtics de Paquet";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Tot";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Gràfic de crides d'aquesta funció:";
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
      return "Buscar";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Resultats de la Búsqueda";
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
        return "No s'ha trobat cap document.";
      }
      else if (numDocuments==1)
      {
        return "Trobat <b>1</b> document.";
      }
      else 
      {
        return "Trobats <b>$num</b> documents. "
               "Mostrant els millors resultats primer.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Resultats:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Fitxer de Codi " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Jerarquia de Directoris"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Documentació dels Directoris"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Directoris"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Aquesta jerarquia de directoris està ordenada toscament, "
             "però no completa, de forma alfabètica:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Referència del Directori "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Directori" : "directori"));
      if (!singular) result+="s";
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
       return "Aquesta és una funció membre sobrecarregada, "
              "proveïda per conveniència. Es diferencia de la funció "
              "anterior només en els arguments que accepta.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Gràfic de crides a aquesta funció:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentació de les Enumeracions"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentació de les Funcions/Subrutines Membre"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Llista de Tipus de Dades"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Camps de Dades"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Aquests són els tipus de dades acompanyats amb breus descripcions:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Aquesta és la llista de tots els membres de tipus de dades";
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll) 
      {
        result+="la documentació del tipus de dades per a cada membre:";
      }
      else 
      {
        result+="els tipus de dades a que pertanyen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Índex de Tipus de Dades"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentació dels Tipus de Dades"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funcions/Subrutines"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentació de les Funcions/Subrutines"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tipus de Dades"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Llista de Mòduls"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Aquesta és la llista de tots els mòduls ";
      if (!extractAll) result+="documentats ";
      result+="amb breus descripcions:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referència de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" el Mòdul "; break;
        case ClassDef::Struct:     result+=" el Tipus "; break;
        case ClassDef::Union:      result+=" la Unió "; break;
        case ClassDef::Interface:  result+=" la Interfície "; break;
        case ClassDef::Protocol:   result+="l Protocol "; break;
        case ClassDef::Category:   result+=" la Categoria "; break;
        case ClassDef::Exception:  result+=" l'Excepció "; break;
      }
      if (isTemplate) result+="Template ";
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Referència del Mòdul ";
      result+=namespaceName;
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Membres del Mòdul"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result="Aquesta és la llista de tots els membres del mòdul";
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll) 
      {
        result+="la documentació del mòdul per a cada membre:";
      }
      else 
      {
        result+="els mòduls a que pertanyen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Índex de Mòduls"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Mòdul" : "mòdul"));
      if (!singular)  result+="s";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"La documentació d'aquest";
      switch(compType)
      {
        case ClassDef::Class:      result+=" mòdul"; break;
        case ClassDef::Struct:     result+=" tipus"; break;
        case ClassDef::Union:      result+="a unió"; break;
        case ClassDef::Interface:  result+="a interfície"; break;
        case ClassDef::Protocol:   result+=" protocol"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a excepció"; break;
      }
      result+=" es va generar a partir del";
      if (!single) result+="s";
      result+=" següent";
      if (!single) result+="s";
      result+=" fitxer";
      if (!single) result+="s:"; else result+=":";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool)
    { 
      QCString result((first_capital ? "Tipus" : "tipus"));
      //if (!singular)  result+="s";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Subprogram" : "subprogram"));
      if (!singular)  result+="es";
      else            result+="a";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Restriccions de Tipus";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Relació";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Carregant...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Espai de Noms Global";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Cercant...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Cap coincidència";
    }

};

#endif
