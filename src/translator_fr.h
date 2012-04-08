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
 * The translation into French was provided by
 *   Christophe Bordeux (bordeux@lig.di.epfl.ch)
 *   and after version 1.2.0 by Xavier Outhier (xouthier@yahoo.fr)
 *   member of the non for profit association D2SET (http://d2set.free.fr)
 */

/******************************************************************************
 * History of content
 *
 *   Date       | Description
 *  ============+=============================================================
 *  2001-11-22  | Removed obsolet methods:
 *              |  QCString latexBabelPackage()
 *              |  QCString trAuthor()
 *              |  QCString trAuthors()
 *              |  QCString trFiles()
 *              |  QCString trIncludeFile()
 *              |  QCString trVerbatimText(const char *f)
 * -------------+------------------------------------------------------------
 *  2002-01-23  | Update for new since 1.2.13
 * -------------+------------------------------------------------------------
 *  2002-07-11  | Update for new since 1.2.16
 * -------------+------------------------------------------------------------
 *  2002-09-24  | Update for new since 1.2.17
 * -------------+------------------------------------------------------------
 *  2002-10-22  | Update for new since 1.2.18
 * -------------+------------------------------------------------------------
 *  2003-02-04  | Corrected typo. Thanks to Bertrand M. :)
 * -------------+------------------------------------------------------------
 *  2003-03-29  | Update for new since 1.3
 * -------------+------------------------------------------------------------
 *  2003-03-29  | Changed fonction into méthode. 
 * -------------+------------------------------------------------------------
 *  2003-06-06  | Fixed code page problem appeared between 1.42 and 1.43 in CVS
 * -------------+------------------------------------------------------------
 *  2003-06-10  | Update for new since 1.3.1
 * -------------+------------------------------------------------------------
 *  2003-09-12  | Update for new since 1.3.3
 * -------------+------------------------------------------------------------
 *  2004-04-30  | Updates by Jacques Bouchard <jacques.bouchard@noos.fr>:
 *              | - spaces between ':' removed (should be added by the renderer)
 *              | - missing spaces added
 *              | - missing tests for OPTIMIZE_OUTPUT_FOR_C added
 *              | - translations corrected
 *              | - translator_fr.h now conforms exactly to translator_en.h
 *              |   (try: gvim -d translator_en.h translator_fr.h)
 * -------------+------------------------------------------------------------
 *  2005-07-12  | Update for new since 1.4.1
 * -------------+------------------------------------------------------------
 *  2005-10-09  | Update for new since 1.4.6
 *              |   Added virtual QCString trCallerGraph()
 *              |   Removed virtual QCString trHeaderFilesDescription()
 *              |   Removed virtual QCString trField(bool first_capital, bool singular)
 *              |   Removed virtual QCString trPackageDocumentation()
 *              |   Removed virtual QCString trSources()
 *              |   Removed virtual QCString trReimplementedForInternalReasons()
 *              |   Removed virtual QCString trInterfaces()
 *              |   Removed virtual QCString trHeaderFiles()
 *              |   Removed virtual QCString trBugsAndLimitations()
 *              |   Removed virtual QCString trNoDescriptionAvailable()
 *              |   Corrected some misspelling thanx to Christophe C.
 * -------------+------------------------------------------------------------
 */ 

#ifndef TRANSLATOR_FR_H
#define TRANSLATOR_FR_H

// When defining a translator class for the new language, follow
// the description in the documentation.  One of the steps says
// that you should copy the translator_en.h (this) file to your
// translator_xx.h new file.  Your new language should use the
// Translator class as the base class.  This means that you need to
// implement exactly the same (pure virtual) methods as the
// TranslatorEnglish does.  Because of this, it is a good idea to
// start with the copy of TranslatorEnglish and replace the strings
// one by one.
//
// It is not necessary to include "translator.h" or
// "translator_adapter.h" here.  The files are included in the
// language.cpp correctly.  Not including any of the mentioned
// files frees the maintainer from thinking about whether the
// first, the second, or both files should be included or not, and
// why.  This holds namely for localized translators because their
// base class is changed occasionaly to adapter classes when the
// Translator class changes the interface, or back to the
// Translator class (by the local maintainer) when the localized
// translator is made up-to-date again.

class TranslatorFrench : public TranslatorAdapter_1_8_0
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
      { return "french"; }
   
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
     */
       virtual QCString latexLanguageSupportCommand()
      {
         return "\\usepackage[french]{babel}\n";
      }
   
    /*! return the language charset. This will be used 
        when transcoding the translatable strings in this file to UTF-8 */
       virtual QCString idLanguageCharset()
      {
         return "iso-8859-1";
      }
   
    // --- Language translation methods -------------------
   
    /*! used in the compound documentation before a list of related functions. */
       virtual QCString trRelatedFunctions()
      { return "Fonctions associées"; }
   
    /*! subscript for the related functions. */
       virtual QCString trRelatedSubscript()
      { return "(Notez que ce ne sont pas des fonctions membres)"; }
   
    /*! header that is put before the detailed description of files, classes and namespaces. */
       virtual QCString trDetailedDescription()
      { return "Description détaillée"; }
   
    /*! header that is put before the list of typedefs. */
       virtual QCString trMemberTypedefDocumentation()
      { return "Documentation des définitions de type membres"; }
    
    /*! header that is put before the list of enumerations. */
       virtual QCString trMemberEnumerationDocumentation()
      { return "Documentation des énumérations membres"; }
    
    /*! header that is put before the list of member functions. */
       virtual QCString trMemberFunctionDocumentation()
      { return "Documentation des fonctions membres"; }
    
    /*! header that is put before the list of member attributes. */
       virtual QCString trMemberDataDocumentation()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Documentation des champs"; 
         }
         else
         {
            return "Documentation des données membres";
         }
      }
   
    /*! this is the text of a link put after brief descriptions. */
       virtual QCString trMore()
      { return "Plus de détails..."; }
   
    /*! put in the class documentation */
       virtual QCString trListOfAllMembers()
      { return "Liste de tous les membres"; }
   
    /*! used as the title of the "list of all members" page of a class */
       virtual QCString trMemberList()
      { return "Liste des membres"; }
   
    /*! this is the first part of a sentence that is followed by a class name */
       virtual QCString trThisIsTheListOfAllMembers()
      { return "Liste complète des membres de "; }
   
    /*! this is the remainder of the sentence after the class name */
       virtual QCString trIncludingInheritedMembers()
      { return ", y compris les membres hérités :"; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
       virtual QCString trGeneratedAutomatically(const char *s)
      { QCString result="Généré automatiquement par Doxygen"; 
         if (s) result+=(QCString)" pour "+s;
         result+=" à partir du code source."; 
         return result;
      }
   
    /*! put after an enum name in the list of all members */
       virtual QCString trEnumName()
      { return "énumération"; }
    
    /*! put after an enum value in the list of all members */
       virtual QCString trEnumValue()
      { return "valeur énumérée"; }
    
    /*! put after an undocumented member in the list of all members */
       virtual QCString trDefinedIn()
      { return "défini dans"; }
   
    // quick reference sections
   
    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
       virtual QCString trModules()
      { return "Modules"; }
    
    /*! This is put above each page as a link to the class hierarchy */
       virtual QCString trClassHierarchy()
      { return "Hiérarchie des classes"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
       virtual QCString trCompoundList()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Structures de données";
         }
         else
         {
            return "Liste des classes"; 
         }
      }
    
    /*! This is put above each page as a link to the list of documented files */
       virtual QCString trFileList()
      { return "Liste des fichiers"; }
   
    /*! This is put above each page as a link to all members of compounds. */
       virtual QCString trCompoundMembers()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Champs de donnée"; 
         }
         else
         {
            return "Membres de classe"; 
         }
      }
   
    /*! This is put above each page as a link to all members of files. */
       virtual QCString trFileMembers()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Variables globale"; 
         }
         else
         {
            return "Membres de fichier"; 
         }
      }
   
    /*! This is put above each page as a link to all related pages. */
       virtual QCString trRelatedPages()
      { return "Pages associées"; }
   
    /*! This is put above each page as a link to all examples. */
       virtual QCString trExamples()
      { return "Exemples"; }
   
    /*! This is put above each page as a link to the search engine. */
       virtual QCString trSearch()
      { return "Recherche"; }
   
    /*! This is an introduction to the class hierarchy. */
       virtual QCString trClassHierarchyDescription()
      { return "Cette liste d'héritage est classée "
             "approximativement par ordre alphabétique :";
      }
   
    /*! This is an introduction to the list with all files. */
       virtual QCString trFileListDescription(bool extractAll)
      {
         QCString result="Liste de tous les fichiers ";
         if (!extractAll) result+="documentés ";
         result+="avec une brève description :";
         return result;
      }
   
    /*! This is an introduction to the annotated compound list. */
       virtual QCString trCompoundListDescription()
      {
      
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Liste des structures de données avec une brève description :"; 
         }
         else
         {
            return "Liste des classes, structures, "
               "unions et interfaces avec une brève description :"; 
         }
      }
   
    /*! This is an introduction to the page with all class members. */
       virtual QCString trCompoundMembersDescription(bool extractAll)
      {
         QCString result="Liste de tous les ";
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            result+="champs de structure et d'union ";
         }
         else
         {
            result+="membres de classe ";
         }
         if (!extractAll)
         {
            result+="documentés ";
         }
         result+="avec des liens vers ";
         if (!extractAll) 
         {
            if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            {
               result+="la documentation de structure/union de chaque champ :";
            }
            else
            {
               result+="la documentation de classe de chaque membre :";
            }
         }
         else 
         {
            if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            {
               result+="les structures/unions auxquelles ils appartiennent :";
            }
            else
            {
               result+="les classes auxquelles ils appartiennent :";
            }
         }
         return result;
      }
   
    /*! This is an introduction to the page with all file members. */
       virtual QCString trFileMembersDescription(bool extractAll)
      {
         QCString result="Liste ";
      
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            result+="de toutes les fonctions, variables, macros, enumérations, et définitions de type ";
         }
         else
         {
            result+="de tous les membres de fichier ";
         }
         if (!extractAll) result+="documentés ";
         result+="avec des liens vers ";
         if (extractAll) 
            result+="les fichiers auxquels ils appartiennent :";
         else 
            result+="la documentation :";
         return result;
      }
   
    /*! This is an introduction to the page with the list of all examples */
       virtual QCString trExamplesDescription()
      { return "Liste de tous les exemples :"; }
   
    /*! This is an introduction to the page with the list of related pages */
       virtual QCString trRelatedPagesDescription()
      { return "Liste de toutes les pages de documentation associées :"; }
   
    /*! This is an introduction to the page with the list of class/file groups */
       virtual QCString trModulesDescription()
      { return "Liste de tous les modules :"; }
   
    /*! This is used in HTML as the title of index.html. */
       virtual QCString trDocumentation()
      { return "Documentation"; }
   
    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
       virtual QCString trModuleIndex()
      { return "Index des modules"; }
   
    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
       virtual QCString trHierarchicalIndex()
      { return "Index hiérarchique"; }
   
    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
       virtual QCString trCompoundIndex()
      {
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         { 
            return "Index des structures de données";
         }
         else
         {
            return "Index des classes"; 
         }
      }
   
    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
       virtual QCString trFileIndex() 
      { return "Index des fichiers"; }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
       virtual QCString trModuleDocumentation()
      { return "Documentation des modules"; }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
       virtual QCString trClassDocumentation()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Documentation des structures de données"; 
         }
         else
         {
            return "Documentation des classes"; 
         }
      }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
       virtual QCString trFileDocumentation()
      { return "Documentation des fichiers"; }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
       virtual QCString trExampleDocumentation()
      { return "Documentation des exemples"; }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
       virtual QCString trPageDocumentation()
      { return "Documentation des pages associées"; }
   
    /*! This is used in LaTeX as the title of the document */
       virtual QCString trReferenceManual()
      { return "Manuel de référence"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
       virtual QCString trDefines()
      { return "Macros"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
       virtual QCString trFuncProtos()
      { return "Prototypes de fonction"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
       virtual QCString trTypedefs()
      { return "Définitions de type"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
       virtual QCString trEnumerations()
      { return "Énumérations"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
       virtual QCString trFunctions()
      { return "Fonctions"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
       virtual QCString trVariables()
      { return "Variables"; }
   
    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
       virtual QCString trEnumerationValues()
      { return "Valeurs énumérées"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
       virtual QCString trDefineDocumentation()
      { return "Documentation des macros"; }
   
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
       virtual QCString trFunctionPrototypeDocumentation()
      { return "Documentation des prototypes de fonction"; }
   
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
       virtual QCString trTypedefDocumentation()
      { return "Documentation des définitions de type"; }
   
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
       virtual QCString trEnumerationTypeDocumentation()
      { return "Documentation du type de l'énumération"; }
   
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
       virtual QCString trFunctionDocumentation()
      { return "Documentation des fonctions"; }
   
    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
       virtual QCString trVariableDocumentation()
      { return "Documentation des variables"; }
   
    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
       virtual QCString trCompounds()
      { 
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Structures de données"; 
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
         QCString result=(QCString)"Généré le "+date;
         if (projName) result+=(QCString)" pour "+projName;
         result+=(QCString)" par";
         return result;
      }
    /*! This is part of the sentence used in the standard footer of each page.
     */
       virtual QCString trWrittenBy()
      {
         return "écrit par";
      }
   
    /*! this text is put before a class diagram */
       virtual QCString trClassDiagram(const char *clName)
      {
         return (QCString)"Graphe d'héritage de "+clName+":";
      }
    
    /*! this text is generated when the \\internal command is used. */
       virtual QCString trForInternalUseOnly()
      { return "Pour un usage interne uniquement."; }
   
    /*! this text is generated when the \\warning command is used. */
       virtual QCString trWarning()
      { return "Avertissement"; }
   
    /*! this text is generated when the \\version command is used. */
       virtual QCString trVersion()
      { return "Version"; }
   
    /*! this text is generated when the \\date command is used. */
       virtual QCString trDate()
      { return "Date"; }
   
    /*! this text is generated when the \\return command is used. */
       virtual QCString trReturns()
      { return "Renvoie"; }
   
    /*! this text is generated when the \\sa command is used. */
       virtual QCString trSeeAlso()
      { return "Voir également"; }
   
    /*! this text is generated when the \\param command is used. */
       virtual QCString trParameters()
      { return "Paramètres"; }
   
    /*! this text is generated when the \\exception command is used. */
       virtual QCString trExceptions()
      { return "Exceptions"; }
    
    /*! this text is used in the title page of a LaTeX document. */
       virtual QCString trGeneratedBy()
      { return "Généré par"; }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-990307
   //////////////////////////////////////////////////////////////////////////
   
    /*! used as the title of page containing all the index of all namespaces. */
       virtual QCString trNamespaceList()
      { return "Liste des espaces de nommage"; }
   
    /*! used as an introduction to the namespace list */
       virtual QCString trNamespaceListDescription(bool extractAll)
      {
         QCString result="Liste de tous les espaces de nommage ";
         if (!extractAll) result+="documentés ";
         result+="avec une brève description:";
         return result;
      }
   
    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
       virtual QCString trFriends()
      { return "Amis"; }
    
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-990405
   //////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
       virtual QCString trRelatedFunctionDocumentation()
      { return "Documentation des fonctions amies et associées"; }
    
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-990425
   //////////////////////////////////////////////////////////////////////////
   
    /*! used as the title of the HTML page of a class/struct/union */
       virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
      {
         QCString result="Référence ";
         if (isTemplate) result+="du modèle ";
         result+="de ";
         switch(compType)
         {
            case ClassDef::Class:      result+="la classe "; break;
            case ClassDef::Struct:     result+="la structure "; break;
            case ClassDef::Union:      result+="l'union "; break;
            case ClassDef::Interface:  result+="l'interface "; break;
            case ClassDef::Protocol:   result+="le protocol "; break; 
            case ClassDef::Category:   result+="la catégorie "; break; 
            case ClassDef::Exception:  result+="l'exception "; break;
         }
         result+=(QCString)clName;
         return result;
      }
   
    /*! used as the title of the HTML page of a file */
       virtual QCString trFileReference(const char *fileName)
      {
         QCString result= "Référence du fichier ";
         result+=fileName;
         return result;
      }
   
    /*! used as the title of the HTML page of a namespace */
       virtual QCString trNamespaceReference(const char *namespaceName)
      {
         QCString result= "Référence de l'espace de nommage ";
         result+=namespaceName;
         return result;
      }
    
       virtual QCString trPublicMembers()
      { return "Fonctions membres publiques"; }
       virtual QCString trPublicSlots()
      { return "Connecteurs publics"; }
       virtual QCString trSignals()
      { return "Signaux"; }
       virtual QCString trStaticPublicMembers()
      { return "Fonctions membres publiques statiques"; }
       virtual QCString trProtectedMembers()
      { return "Fonctions membres protégées"; }
       virtual QCString trProtectedSlots()
      { return "Connecteurs protégés"; }
       virtual QCString trStaticProtectedMembers()
      { return "Fonctions membres protégées statiques"; }
       virtual QCString trPrivateMembers()
      { return "Fonctions membres privées"; }
       virtual QCString trPrivateSlots()
      { return "Connecteurs privés"; }
       virtual QCString trStaticPrivateMembers()
      { return "Fonctions membres privées statiques"; }
    
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
                  result+=", et ";
            }
         }
         return result; 
      }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
       virtual QCString trInheritsList(int numEntries)
      {
         return "Est dérivée de "+trWriteList(numEntries)+".";
      }
   
    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
       virtual QCString trInheritedByList(int numEntries)
      {
         return "Dérivée par "+trWriteList(numEntries)+".";
      }
   
    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
       virtual QCString trReimplementedFromList(int numEntries)
      {
         return "Réimplémentée à partir de "+trWriteList(numEntries)+".";
      }
   
    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
       virtual QCString trReimplementedInList(int numEntries)
      {
         return "Réimplémentée dans "+trWriteList(numEntries)+".";
      }
   
    /*! This is put above each page as a link to all members of namespaces. */
       virtual QCString trNamespaceMembers()
      { return "Membres de l'espace de nommage"; }
   
    /*! This is an introduction to the page with all namespace members */
       virtual QCString trNamespaceMemberDescription(bool extractAll)
      { 
         QCString result="Liste de tous les membres des espaces de nommage ";
         if (!extractAll) result+="documentés ";
         result+="avec des liens vers ";
         if (extractAll) 
            result+="la documentation de namespace de chaque membre :";
         else 
            result+="les espaces de nommage auxquels ils appartiennent :";
         return result;
      }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
       virtual QCString trNamespaceIndex()
      { return "Index des espaces de nommage"; }
   
    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
       virtual QCString trNamespaceDocumentation()
      { return "Documentation des espaces de nommage"; }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-990522
   //////////////////////////////////////////////////////////////////////////
   
    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
       virtual QCString trNamespaces()
      { return "Espaces de nommage"; }
   
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
         bool female = true;
         QCString result=(QCString)"La documentation de ";
         switch(compType)
         {
            case ClassDef::Class:      result+="cette classe"; break;
            case ClassDef::Struct:     result+="cette structure"; break;
            case ClassDef::Union:      result+="cette union"; break;
            case ClassDef::Interface:  result+="cette interface"; break;
            case ClassDef::Protocol:   result+="ce protocol"; female = false; break;
            case ClassDef::Category:   result+="cette catégorie"; break; 
            case ClassDef::Exception:  result+="cette exception"; break;
         }
         if (female) result+= " a été générée à partir ";
         else result+=" a été généré à partir ";
         if (single) result+="du fichier suivant :"; 
         else result+="des fichiers suivants :";
         return result;
      }
   
    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
       virtual QCString trAlphabeticalList()
      { return "Liste alphabétique"; }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-990901
   //////////////////////////////////////////////////////////////////////////
   
    /*! This is used as the heading text for the retval command. */
       virtual QCString trReturnValues()
      { return "Valeurs retournées"; }
   
    /*! This is in the (quick) index as a link to the main page (index.html)
     */
       virtual QCString trMainPage()
      { return "Page principale"; }
   
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
         return "Définition à la ligne @0 du fichier @1.";
      }
       virtual QCString trDefinedInSourceFile()
      {
         return "Définition dans le fichier @0.";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 0.49-991205
   //////////////////////////////////////////////////////////////////////////
   
       virtual QCString trDeprecated()
      {
         return "Obsolète";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.0.0
   //////////////////////////////////////////////////////////////////////////
   
    /*! this text is put before a collaboration diagram */
       virtual QCString trCollaborationDiagram(const char *clName)
      {
         return (QCString)"Graphe de collaboration de "+clName+":";
      }
    /*! this text is put before an include dependency graph */
       virtual QCString trInclDepGraph(const char *fName)
      {
         return (QCString)"Graphe des dépendances par inclusion de "+fName+":";
      }
    /*! header that is put before the list of constructor/destructors. */
       virtual QCString trConstructorDocumentation()
      {
         return "Documentation des constructeurs et destructeur"; 
      }
    /*! Used in the file documentation to point to the corresponding sources. */
       virtual QCString trGotoSourceCode()
      {
         return "Aller au code source de ce fichier.";
      }
    /*! Used in the file sources to point to the corresponding documentation. */
       virtual QCString trGotoDocumentation()
      {
         return "Aller à la documentation de ce fichier.";
      }
    /*! Text for the \\pre command */
       virtual QCString trPrecondition()
      {
         return "Précondition";
      }
    /*! Text for the \\post command */
       virtual QCString trPostcondition()
      {
         return "Postcondition";
      }
    /*! Text for the \\invariant command */
       virtual QCString trInvariant()
      {
         return "Invariant";
      }
    /*! Text shown before a multi-line variable/enum initialization */
       virtual QCString trInitialValue()
      {
         return "Valeur initiale :";
      }
    /*! Text used the source code in the file index */
       virtual QCString trCode()
      {
         return "code";
      }
       virtual QCString trGraphicalHierarchy()
      {
         return "Graphe hiérarchique des classes";
      }
       virtual QCString trGotoGraphicalHierarchy()
      {
         return "Aller au graphe hiérarchique des classes";
      }
       virtual QCString trGotoTextualHierarchy()
      {
         return "Aller à la hiérarchie des classes en mode texte";
      }
       virtual QCString trPageIndex()
      {
         return "Index des pages";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.1.0
   //////////////////////////////////////////////////////////////////////////
    
       virtual QCString trNote()
      {
         return "Note";
      }
       virtual QCString trPublicTypes()
      {
         return "Types publics";
      }
       virtual QCString trPublicAttribs()
      {
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Champs de données";
         }
         else
         {
            return "Attributs publics";
         }
      }
       virtual QCString trStaticPublicAttribs()
      {
         return "Attributs publics statiques";
      }
       virtual QCString trProtectedTypes()
      {
         return "Types protégés";
      }
       virtual QCString trProtectedAttribs()
      {
         return "Attributs protégés";
      }
       virtual QCString trStaticProtectedAttribs()
      {
         return "Attributs protégés statiques";
      }
       virtual QCString trPrivateTypes()
      {
         return "Types privés";
      }
       virtual QCString trPrivateAttribs()
      {
         return "Attributs privés";
      }
       virtual QCString trStaticPrivateAttribs()
      {
         return "Attributs privés statiques";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.1.3
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as a marker that is put before a \\todo item */
       virtual QCString trTodo()
      {
         return "A faire";
      }
    /*! Used as the header of the todo list */
       virtual QCString trTodoList()
      {
         return "Liste des choses à faire";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.1.4
   //////////////////////////////////////////////////////////////////////////
   
       virtual QCString trReferencedBy()
      {
         return "Référencé par";
      }
       virtual QCString trRemarks()
      {
         return "Remarques";
      }
       virtual QCString trAttention()
      {
         return "Attention";
      }
       virtual QCString trInclByDepGraph()
      {
         return "Ce graphe montre quels fichiers incluent directement "
             "ou indirectement ce fichier :";
      }
       virtual QCString trSince()
      {
         return "Depuis";
      }
    
   //////////////////////////////////////////////////////////////////////////
   // new since 1.1.5
   //////////////////////////////////////////////////////////////////////////
   
    /*! title of the graph legend page */
       virtual QCString trLegendTitle()
      {
         return "Légende du graphe";
      }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
       virtual QCString trLegendDocs()
      {
         return 
            "Cette page explique comment interpréter les graphes générés "
            "par doxygen.<p>\n"
            "Considérez l'exemple suivant :\n"
            "\\code\n"
            "/*! Classe invisible à cause d'une troncature */\n"
            "class Invisible { };\n\n"
            "/*! Classe tronquée, la relation d'héritage est masquée */\n"
            "class Truncated : public Invisible { };\n\n"
            "/*! Classe non documentée avec des commentaires Doxygen */\n"
            "class Undocumented { };\n\n"
            "/*! Classe dérivée par héritage public */\n"
            "class PublicBase : public Truncated { };\n\n"
            "/*! Un modèle de classe */\n"
            "template<class T> class Templ { };\n\n"
            "/*! Classe dérivée par héritage protégé */\n"
            "class ProtectedBase { };\n\n"
            "/*! Classe dérivée par héritage privé */\n"
            "class PrivateBase { };\n\n"
            "/*! Classe utilisée par la classe dérivée */\n"
            "class Used { };\n\n"
            "/*! Super-classe qui hérite de plusieurs autres classes */\n"
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
            "Si la valeur 240 est attribuée au tag \\c MAX_DOT_GRAPH_HEIGHT "
            "du fichier de configuration, cela génèrera le graphe suivant :"
            "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
            "<p>\n"
            "Les rectangles du graphe ci-dessus ont la signification suivante :\n"
            "<ul>\n"
            "<li>Un rectangle plein noir représente la structure ou la classe pour laquelle "
            "le graphe est généré.\n"
            "<li>Un rectangle avec un bord noir indique une classe ou une structure documentée.\n"
            "<li>Un rectangle avec un bord gris indique une classe ou une structure non documentée.\n"
            "<li>Un rectangle avec un bord rouge indique une structure ou une classe documentée\n"
            "pour laquelle des relations d'héritage ou de collaboration manquent. Un graphe est "
            "tronqué s'il n'entre pas dans les limites spécifiées."
            "</ul>\n"
            "Les flèches ont la signification suivante :\n"
            "<ul>\n"
            "<li>Une flèche bleu foncé est utilisée pour visualiser une relation d'héritage publique "
            "entre deux classes.\n"
            "<li>Une flèche vert foncé est utilisée pour une relation d'héritage protégée.\n"
            "<li>Une flèche rouge foncé est utilisée pour une relation d'héritage privée.\n"
            "<li>Une flèche violette en pointillés est utilisée si une classe est contenue ou "
            "utilisée par une autre classe. La flèche est étiquetée avec la ou les variable(s) "
            "qui permettent d'accéder à la classe ou structure pointée. \n"
            "<li>Une flèche jaune en pointillés indique une relation entre un modèle d'instance et "
            "le modèle de classe duquel il est instancié. La flèche est étiquetée avec "
            "les paramètres de modèle de l'instance.\n"
            "</ul>\n";
      }
    /*! text for the link to the legend page */
       virtual QCString trLegend()
      {
         return "légende";
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
         return "Liste des tests";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.1
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as a section header for KDE-2 IDL methods */
       virtual QCString trDCOPMethods()
      {
         return "Fonctions membres DCOP";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.2
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as a section header for IDL properties */
       virtual QCString trProperties()
      {
         return "Propriétés";
      }
    /*! Used as a section header for IDL property documentation */
       virtual QCString trPropertyDocumentation()
      {
         return "Documentation des propriétés";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.4
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used for Java classes in the summary section of Java packages */
       virtual QCString trClasses()
      {
         if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
         {
            return "Structures de données";
         }
         else
         {
            return "Classes";
         }
      }
    /*! Used as the title of a Java package */
       virtual QCString trPackage(const char *name)
      {
         return (QCString)"Paquetage "+name;
      }
    /*! Title of the package index page */
       virtual QCString trPackageList()
      {
         return "Liste des paquetages";
      }
    /*! The description of the package index page */
       virtual QCString trPackageListDescription()
      {
         return "Liste des paquetages avec une brève description (si disponible) :";
      }
    /*! The link name in the Quick links header for each page */
       virtual QCString trPackages()
      {
         return "Paquetages";
      }
    /*! Text shown before a multi-line define */
       virtual QCString trDefineValue()
      {
         return "Valeur :";
      }
    
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.5
   //////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
       virtual QCString trBug()
      {
         return "Bogue";
      }
    /*! Used as the header of the bug list */
       virtual QCString trBugList()
      {
         return "Liste des bogues";
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
         return "Index";
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
         QCString result((first_capital ? "Fichier" : "fichier"));
         if (!singular)  result+="s";
         return result; 
      }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
       virtual QCString trNamespace(bool first_capital, bool singular)
      { 
         QCString result((first_capital ? "Espace" : "espace"));
         if (!singular)  result+="s";
         result+=" de nommage";
         return result; 
      }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
       virtual QCString trGroup(bool first_capital, bool singular)
      { 
         QCString result((first_capital ? "Groupe" : "groupe"));
         if (!singular)  result+="s";
         return result; 
      }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
       virtual QCString trPage(bool first_capital, bool singular)
      { 
         QCString result((first_capital ? "Page" : "page"));
         if (!singular)  result+="s";
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
         QCString result((first_capital ? "Globa" : "globa"));
         if (!singular)  result+="ux(ales)"; else result+="l(e)";
         return result; 
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.7
   //////////////////////////////////////////////////////////////////////////
   
    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
       virtual QCString trAuthor(bool first_capital, bool singular)
      {                                                                         
         QCString result((first_capital ? "Auteur" : "auteur"));
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
         return "Références";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.13
   //////////////////////////////////////////////////////////////////////////
   
    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
       virtual QCString trImplementedFromList(int numEntries)
      {
         return "Implémente "+trWriteList(numEntries)+".";
      }
   
    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
       virtual QCString trImplementedInList(int numEntries)
      {
         return "Implémenté dans "+trWriteList(numEntries)+".";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.16
   //////////////////////////////////////////////////////////////////////////
   
    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
       virtual QCString trRTFTableOfContents()
      {
         return "Table des matières";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.17
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
       virtual QCString trDeprecatedList()
      {
         return "Liste des éléments obsolètes";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.2.18
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
       virtual QCString trEvents()
      {
         return "Événements";
      }
    /*! Header used for the documentation section of a class' events. */
       virtual QCString trEventDocumentation()
      {
         return "Documentation des événements";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.3
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used as a heading for a list of Java class types with package scope.
     */
       virtual QCString trPackageTypes()
      { 
         return "Types de paquetage";
      }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
       virtual QCString trPackageMembers()
      { 
         return "Fonctions de paquetage";
      }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
       virtual QCString trStaticPackageMembers()
      { 
         return "Fonctions statiques de paquetage";
      }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
       virtual QCString trPackageAttribs()
      { 
         return "Attributs de paquetage";
      }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
       virtual QCString trStaticPackageAttribs()
      { 
         return "Attributs statiques de paquetage";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.3.1
   //////////////////////////////////////////////////////////////////////////
   
    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
       virtual QCString trAll()
      {
         return "Tout";
      }
    /*! Put in front of the call graph for a function. */
       virtual QCString trCallGraph()
      {
         return "Voici le graphe d'appel pour cette fonction :";
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
         return "Rechercher";
      }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
       virtual QCString trSearchResultsTitle()
      {
         return "Résultats de la recherche";
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
            return "Désolé, aucun document ne correspond à votre requête.";
         }
         else if (numDocuments==1)
         {
            return "Trouvé <b>1</b> document correspondant à votre requête.";
         }
         else 
         {
            return "Trouvé  <b>$num</b> documents correspondant à votre requête. "
               "Classé par ordre de pertinence décroissant.";
         }
      }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
       virtual QCString trSearchMatches()
      {
         return "Correspondances :";
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.3.8
   //////////////////////////////////////////////////////////////////////////
   
     /*! This is used in HTML as the title of page with source code for file filename
      */
       virtual QCString trSourceFile(QCString& filename)
      {
         return " Fichier source de " + filename;
      }
   
   //////////////////////////////////////////////////////////////////////////
   // new since 1.3.9
   //////////////////////////////////////////////////////////////////////////
   
     /*! This is used as the name of the chapter containing the directory
      *  hierarchy.
      */
       virtual QCString trDirIndex()
      { return "Hiérarchie de répertoires"; }
   
     /*! This is used as the name of the chapter containing the documentation
      *  of the directories.
      */
       virtual QCString trDirDocumentation()
      { return "Documentation des répertoires"; }
   
     /*! This is used as the title of the directory index and also in the
      *  Quick links of a HTML page, to link to the directory hierarchy.
      */
       virtual QCString trDirectories()
      { return "Répertoires"; }
   
     /*! This returns a sentences that introduces the directory hierarchy.
      *  and the fact that it is sorted alphabetically per level
      */
       virtual QCString trDirDescription()
      { return "Cette hiérarchie de répertoire est triée approximativement, "
              "mais pas complètement, par ordre alphabétique :";
      }
   
     /*! This returns the title of a directory page. The name of the
      *  directory is passed via \a dirName.
      */
       virtual QCString trDirReference(const char *dirName)
      { QCString  result="Répertoire de référence de "; result+=dirName; return result; }
   
     /*! This returns the word directory with or without starting capital
      *  (\a first_capital) and in sigular or plural form (\a singular).
      */
       virtual QCString trDir(bool first_capital, bool singular)
      {
         QCString result((first_capital ? "Répertoire" : "répertoire"));
         if (singular) result+=""; else result+="s";
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
         return "Ceci est une fonction membre surchargée, "
              "proposée par commodité. Elle diffère de la fonction "
              "ci-dessus uniquement par le(s) argument(s) qu'elle accepte.";
      }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.4.6
    //////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Voici le graphe des appelants de cette fonction :";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentation des énumérations"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.5.4 (mainly for Fortran)
    //////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentation des fonctions/subroutines membres"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Liste des types de données"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Champs de données"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Liste des types de données avec une brève description :"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Liste de tous les membres de types de données ";
      if (!extractAll)
      {
        result+="documentés ";
      }
      result+="avec des liens vers ";
      if (!extractAll) 
      {
         result+="la documentation de la structure des données de chaque membre :";
      }
      else 
      {
         result+="les types des données auxquels ils appartiennent :";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Index du type de données"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentation du type de données"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Fonctions/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentation de la fonction/subroutine"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Les types de données"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Liste des modules"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Liste de tous les modules ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Réference ";
      if (isTemplate) result+="du modèle ";
      switch(compType)
      {
        case ClassDef::Class:      result+="du module "; break;
        case ClassDef::Struct:     result+="du type "; break;
        case ClassDef::Union:      result+="de l'union "; break;
        case ClassDef::Interface:  result+="de l'interface "; break;
        case ClassDef::Protocol:   result+="du protocole "; break;
        case ClassDef::Category:   result+="de la catégorie "; break;
        case ClassDef::Exception:  result+="de l'exception "; break;
      }
      result+=(QCString)clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result="Référence du module ";
      result+= namespaceName;        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Membres du module"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Liste de tous les membres ";
      if (!extractAll) result+="documentés ";
      result+="du module avec des liens vers ";
      if (extractAll) 
      {
        result+="la documentation du module de chaque membre :";
      }
      else 
      {
        result+="les modules auxquels ils appartiennent :";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Index des modules"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Module" : "module"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      QCString result=(QCString)"La documentation de ";
      switch(compType)
      {
        case ClassDef::Class:      result+="ce module"; break;
        case ClassDef::Struct:     result+="ce type"; break;
        case ClassDef::Union:      result+="cette union"; break;
        case ClassDef::Interface:  result+="cette interface"; break;
        case ClassDef::Protocol:   result+="ce protocole"; break;
        case ClassDef::Category:   result+="cette catégorie"; break;
        case ClassDef::Exception:  result+="cette exception"; break;
      }
      result+=" a été générée à partir ";
      if (single) result+="du fichier suivant :"; else result+="des fichiers suivants :";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Type" : "type"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Sous-programme" : "sous-programme"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Contraintes de type";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Relation " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Chargement...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Espace de nommage global";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Recherche...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Aucune correspondance";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Graphe des dépendances de répertoire pour "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Fichier dans "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Inclut le fichier dans "+name;
    }

    /** Compiles a date string. 
     *  @param year Year in 4 digits
     *  @param month Month of the year: 1=January
     *  @param day Day of the Month: 1..31
     *  @param dayOfWeek Day of the week: 1=Monday..7=Sunday
     *  @param hour Hour of the day: 0..23
     *  @param minutes Minutes in the hour: 0..59
     *  @param seconds Seconds within the minute: 0..59
     *  @param includeTime Include time in the result string?
     */
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    {
      static const char *days[]   = { "Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi","Dimanche" };
      static const char *months[] = { "Janvier","Février","Mars","Avril","Mai","Juin","Juillet","Août","Septembre","Octobre","Novembre","Décembre" };
      QCString sdate;
      sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Références bibliographiques"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Graphe des dépendances de répertoires pour ")+name+":"; }

};

#endif
