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
 * The translation into French was provided by
 *   Christophe Bordeux (bordeux@lig.di.epfl.ch)
 *   and after version 1.2.0 by Xavier Outhier (xouthier@yahoo.fr)
 *   member of the non for profit association D2SET (http://d2set.free.fr).
 *   Benoît BROSSE (Benoit dot BROSSE at ingenico dot com) was added
 *   after the 1.8.4 release.
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
 *              |   Added QCString trCallerGraph() override
 *              |   Removed QCString trHeaderFilesDescription() override
 *              |   Removed QCString trField(bool first_capital, bool singular) override
 *              |   Removed QCString trPackageDocumentation() override
 *              |   Removed QCString trSources() override
 *              |   Removed QCString trReimplementedForInternalReasons() override
 *              |   Removed QCString trInterfaces() override
 *              |   Removed QCString trHeaderFiles() override
 *              |   Removed QCString trBugsAndLimitations() override
 *              |   Removed QCString trNoDescriptionAvailable() override
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
// implement exactly the same (pure virtual) override methods as the
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
// base class is changed occasionally to adapter classes when the
// Translator class changes the interface, or back to the
// Translator class (by the local maintainer) when the localized
// translator is made up-to-date again.

class TranslatorFrench : public TranslatorAdapter_1_9_5
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
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[french]{babel}\n"
             "\\NoAutoSpaceBeforeFDP\n";
    }

    QCString trISOLang() override
    {
      return "fr";
    }
    QCString getLanguageString() override
    {
      return "0x40C French";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Fonctions associées"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Notez que ce ne sont pas des fonctions membres)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Description détaillée"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Détails"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentation des définitions de type membres"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentation des énumérations membres"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Documentation des fonctions membres"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentation des champs";
      }
      else
      {
        return "Documentation des données membres";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Plus de détails..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Liste de tous les membres"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Liste des membres"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Liste complète des membres de"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", y compris les membres hérités :"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Généré automatiquement par Doxygen";
      if (!s.isEmpty()) result+=" pour "+s;
      result+=" à partir du code source.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "énumération"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "valeur énumérée"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "défini dans"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Modules"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hiérarchie des classes"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Structures de données";
      }
      else
      {
        return "Liste des classes";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Liste des fichiers"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Champs de donnée";
      }
      else
      {
        return "Membres de classe";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Variables globale";
      }
      else
      {
        return "Membres de fichier";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Pages associées"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Exemples"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Recherche"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Liste hiérarchique de toutes les entités :";
      }
      else
      {
        return "Cette liste d'héritage est classée "
               "approximativement par ordre alphabétique :";
      }
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Liste de tous les fichiers ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Liste des structures de données avec une brève description :";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Liste des classes avec une brève description :";
      }
      else
      {
        return "Liste des classes, structures, "
               "unions et interfaces avec une brève description :";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Liste de tous les ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Liste ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trExamplesDescription() override
    { return "Liste de tous les exemples :"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Liste de toutes les pages de documentation associées :"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Liste de tous les modules :"; }

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Documentation"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Index des modules"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Index hiérarchique"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Index des fichiers"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Documentation des modules"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentation des structures de données";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Documentation des classes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Documentation des fichiers"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Manuel de référence"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Macros"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Définitions de type"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Énumérations"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Fonctions"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variables"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Valeurs énumérées"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Documentation des macros"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Documentation des définitions de type"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Documentation du type de l'énumération"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Documentation des fonctions"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Documentation des variables"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Généré le "+date;
      if (!projName.isEmpty()) result+=" pour "+projName;
      result+=" par";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Graphe d'héritage de "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Avertissement"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Version"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Date"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Renvoie"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Voir également"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Paramètres"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Exceptions"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Généré par"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Liste des espaces de nommage"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Liste de tous les espaces de nommage ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Amis"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Documentation des fonctions amies et associées"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
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
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result= "Référence du fichier ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result= "Référence de l'espace de nommage ";
      result+=namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Fonctions membres publiques"; }
    QCString trPublicSlots() override
    { return "Connecteurs publics"; }
    QCString trSignals() override
    { return "Signaux"; }
    QCString trStaticPublicMembers() override
    { return "Fonctions membres publiques statiques"; }
    QCString trProtectedMembers() override
    { return "Fonctions membres protégées"; }
    QCString trProtectedSlots() override
    { return "Connecteurs protégés"; }
    QCString trStaticProtectedMembers() override
    { return "Fonctions membres protégées statiques"; }
    QCString trPrivateMembers() override
    { return "Fonctions membres privées"; }
    QCString trPrivateSlots() override
    { return "Connecteurs privés"; }
    QCString trStaticPrivateMembers() override
    { return "Fonctions membres privées statiques"; }

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
            result+=", et ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Est dérivée de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Dérivée par "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Réimplémentée à partir de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Réimplémentée dans "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Membres de l'espace de nommage"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Index des espaces de nommage"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Documentation des espaces de nommage"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Espaces de nommage"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      bool feminine = true;
      QCString result="La documentation de ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt? "cette unités de conception":"cette classe"; break;
        case ClassDef::Struct:     result+="cette structure"; break;
        case ClassDef::Union:      result+="cette union"; break;
        case ClassDef::Interface:  result+="cette interface"; break;
        case ClassDef::Protocol:   result+="ce protocol"; feminine = false; break;
        case ClassDef::Category:   result+="cette catégorie"; break;
        case ClassDef::Exception:  result+="cette exception"; break;
        default: break;
      }
      if (feminine) result+= " a été générée à partir ";
      else result+=" a été généré à partir ";
      if (single) result+="du fichier suivant :";
      else result+="des fichiers suivants :";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Valeurs retournées"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Page principale"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Définition à la ligne @0 du fichier @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Définition dans le fichier @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Obsolète";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Graphe de collaboration de "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graphe des dépendances par inclusion de "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Documentation des constructeurs et destructeur";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Aller au code source de ce fichier.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Aller à la documentation de ce fichier.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Précondition";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondition";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Valeur initiale :";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "code";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Graphe hiérarchique des classes";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Aller au graphe hiérarchique des classes";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Aller à la hiérarchie des classes en mode texte";
    }
    QCString trPageIndex() override
    {
      return "Index des pages";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Note";
    }
    QCString trPublicTypes() override
    {
      return "Types publics";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Champs de données";
      }
      else
      {
        return "Attributs publics";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Attributs publics statiques";
    }
    QCString trProtectedTypes() override
    {
      return "Types protégés";
    }
    QCString trProtectedAttribs() override
    {
      return "Attributs protégés";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Attributs protégés statiques";
    }
    QCString trPrivateTypes() override
    {
      return "Types privés";
    }
    QCString trPrivateAttribs() override
    {
      return "Attributs privés";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Attributs privés statiques";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "A faire";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Liste des choses à faire";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Référencé par";
    }
    QCString trRemarks() override
    {
      return "Remarques";
    }
    QCString trAttention() override
    {
      return "Attention";
    }
    QCString trInclByDepGraph() override
    {
      return "Ce graphe montre quels fichiers incluent directement "
             "ou indirectement ce fichier :";
    }
    QCString trSince() override
    {
      return "Depuis";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Légende du graphe";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
        "Cela aboutira au graphe suivant :"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Les rectangles du graphe ci-dessus ont la signification suivante :\n"
        "</p>\n"
        "<ul>\n"
        "<li>Un rectangle plein noir représente la structure ou la classe pour laquelle "
        "le graphe est généré.\n"
        "<li>Un rectangle avec un bord noir indique une classe ou une structure documentée.\n"
        "<li>Un rectangle avec un bord gris indique une classe ou une structure non documentée.\n"
        "<li>Un rectangle avec un bord rouge indique une structure ou une classe documentée\n"
        "pour laquelle des relations d'héritage ou de collaboration manquent. Un graphe est "
        "tronqué s'il n'entre pas dans les limites spécifiées."
        "</ul>\n"
        "<p>\n"
        "Les flèches ont la signification suivante :\n"
        "</p>\n"
        "<ul>\n"
        "<li>Une bleu foncé est utilisée pour visualiser une relation d'héritage publique "
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
    QCString trLegend() override
    {
      return "légende";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Liste des tests";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Propriétés";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Documentation des propriétés";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Structures de données";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paquetage "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Liste des paquetages avec une brève description (si disponible) :";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paquetages";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Valeur :";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bogue";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "classe", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fichier", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result = createNoun(first_capital, singular, "espace", "s");
      result+=" de nommage";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "groupe", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "page", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membre", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globa", "ux(ales)", "l(e)");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "auteur", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Références";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implémente "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implémenté dans "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Table des matières";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Liste des éléments obsolètes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Événements";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Documentation des événements";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Types de paquetage";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Fonctions de paquetage";
    }
    QCString trPackageMembers() override
    {
      return "Membres de paquetage";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Fonctions statiques de paquetage";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Attributs de paquetage";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Attributs statiques de paquetage";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Tout";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Voici le graphe d'appel pour cette fonction :";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Correspondances :";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return " Fichier source de " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hiérarchie de répertoires"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Documentation des répertoires"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Répertoires"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString  result="Répertoire de référence de "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "répertoire", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Ceci est une fonction membre surchargée, "
              "proposée par commodité. Elle diffère de la fonction "
              "ci-dessus uniquement par le(s) argument(s) qu'elle accepte.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Voici le graphe des appelants de cette fonction :";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentation des énumérations"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentation des fonctions/subroutines membres"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Liste des types de données"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Champs de données"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Liste des types de données avec une brève description :"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Index du type de données"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentation du type de données"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Fonctions/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentation de la fonction/subroutine"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Les types de données"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Liste des modules"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Liste de tous les modules ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
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
        default: break;
      }
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Référence du module ";
      result+= namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Membres du module"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Index des modules"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "module", "s");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      QCString result="La documentation de ";
      switch(compType)
      {
        case ClassDef::Class:      result+="ce module"; break;
        case ClassDef::Struct:     result+="ce type"; break;
        case ClassDef::Union:      result+="cette union"; break;
        case ClassDef::Interface:  result+="cette interface"; break;
        case ClassDef::Protocol:   result+="ce protocole"; break;
        case ClassDef::Category:   result+="cette catégorie"; break;
        case ClassDef::Exception:  result+="cette exception"; break;
        default: break;
      }
      result+=" a été générée à partir ";
      if (single) result+="du fichier suivant :"; else result+="des fichiers suivants :";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sous-programme", "s");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Contraintes de type";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relation " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Chargement...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Espace de nommage global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Recherche...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Aucune correspondance";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    QCString trFileIn(const QCString &name) override
    {
      return "Fichier dans "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Inclut le fichier dans "+name;
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi","Dimanche" };
      static const char *months[] = { "Janvier","Février","Mars","Avril","Mai","Juin","Juillet","Août","Septembre","Octobre","Novembre","Décembre" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "lun.", "mar.", "mer.", "jeu.", "ven.", "sam.", "dim." };
      static const char *days_full[]    = { "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "janv.", "févr.", "mars", "avr.", "mai", "juin", "juil.", "août", "sept.", "oct.", "nov.", "déc." };
      static const char *months_full[]  = { "janvier", "février", "mars", "avril", "mai", "juin", "juillet", "août", "septembre", "octobre", "novembre", "décembre" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Références bibliographiques"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Graphe des dépendances de répertoires pour ")+name+":"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "Niveau de détails"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Paramètres du template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "et "+number+" de plus..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "La documentation pour cette énumération a été générée à partir ";
      if (!single) result += "du fichier suivant"; else result += "des fichiers suivants";
      result+=" :";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Référence de l'énumération"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" hérités de "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Membres hérités additionnels"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "activer" : "désactiver";
      return "cliquez pour "+opt+" la synchronisation du panel";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Déclarée dans la catégorie @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Dérive la classe @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Méthodes de classe";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Méthodes d'instance";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Documentation des méthodes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Interfaces exportées"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Services inclus"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Groupes constants"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result="Référence du groupe constant ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result="Référence du service ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result="Référence du singleton ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="La documentation pour ce service "
                                "a été générée par ";
      if (single) result+="le fichier suivant :"; else result+="les fichiers suivants :";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="La documentation pour ce singleton "
                                "a été générée par ";
      if (single) result+="le fichier suivant :"; else result+="les fichiers suivants :";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Hiérarchie des unités de conception"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Liste des unités de conception"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Membres des unités de conception"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Liste de tous les membres des unités de conception avec liens vers "
            "les entités auxquelles ils appartiennent :";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Index des unités de conception"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Unités de conception"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Fonctions/Procédures/Processes"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Librairie";
          else        return "Librairies";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Paquetage";
          else        return "Paquetages";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Signal";
          else        return "Signaux";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Composant";
          else        return "Composants";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Constante";
          else        return "Constantes";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entité";
          else        return "Entités";
        case VhdlSpecifier::TYPE:
          if (single) return "Type";
          else        return "Types";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Sous-type";
          else        return "Sous-types";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Fonction";
          else        return "Fonctions";
        case VhdlSpecifier::RECORD:
          if (single) return "Enregistrement";
          else        return "Enregistrements";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procédure";
          else        return "Procédures";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Architecture";
          else        return "Architectures";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Attribut";
          else        return "Attributs";
        case VhdlSpecifier::PROCESS:
          if (single) return "Process";
          else        return "Processes";
        case VhdlSpecifier::PORT:
          if (single) return "Port";
          else        return "Ports";
        case VhdlSpecifier::USE:
          if (single) return "Clause d'utilisation";
          else        return "Clauses d'utilisation";
        case VhdlSpecifier::GENERIC:
          if (single) return "Generique";
          else        return "Generiques";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Corps du paquetage";
        case VhdlSpecifier::UNITS:
          return "Unités";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Variable partagée";
          else        return "Variables partagées";
        case VhdlSpecifier::VFILE:
          if (single) return "Fichier";
          else        return "Fichiers";
        case VhdlSpecifier::GROUP:
          if (single) return "Groupe";
          else        return "Groupes";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Instanciation";
          else        return "Instanciations";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Alias";
        case VhdlSpecifier::CONFIG:
          if (single) return "Configuration";
          else        return "Configurations";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Divers";
        case VhdlSpecifier::UCF_CONST:
          return "Contraintes";
        default:
          return "Classe";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return QCString("Référence ") + QCString(name); }

    QCString trConstants() override
    {
        return "Constantes";
    }
    QCString trConstantDocumentation() override
    {
        return "Documentation des constantes";
    }
    QCString trSequences() override
    {
        return "Séquences";
    }
    QCString trSequenceDocumentation() override
    {
        return "Documentation des séquences";
    }
    QCString trDictionaries() override
    {
        return "Dictionnaires";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Documentation des dictionnaires";
    }
    QCString trSliceInterfaces() override
    {
        return "Interfaces";
    }
    QCString trInterfaceIndex() override
    {
        return "Index des interfaces";
    }
    QCString trInterfaceList() override
    {
        return "Liste des interfaces";
    }
    QCString trInterfaceListDescription() override
    {
        return "Liste des interfaces avec une brève description :";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Hiérarchies des interfaces";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Cette liste d'héritage est classée approximativement par ordre alphabétique :";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Documentation des interfaces";
    }
    QCString trStructs() override
    {
        return "Structures";
    }
    QCString trStructIndex() override
    {
        return "Index des structures";
    }
    QCString trStructList() override
    {
        return "Liste des structures";
    }
    QCString trStructListDescription() override
    {
        return "Liste des structures avec une brève description :";
    }
    QCString trStructDocumentation() override
    {
        return "Documentation des structures";
    }
    QCString trExceptionIndex() override
    {
        return "Index des exceptions";
    }
    QCString trExceptionList() override
    {
        return "Liste des exceptions";
    }
    QCString trExceptionListDescription() override
    {
        return "Liste des exceptions avec une brève description :";
    }
    QCString trExceptionHierarchy() override
    {
        return "Hiérarchies des exceptions";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Cette liste d'héritage est classée approximativement par ordre alphabétique :";
    }
    QCString trExceptionDocumentation() override
    {
        return "Documentation des exceptions";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result = "Référence ";
      bool feminine = true;
      switch(compType)
      {
        case ClassDef::Class:      result+="de la classe "; break;
        case ClassDef::Struct:     result+="de la structure "; break;
        case ClassDef::Union:      result+="de l'union "; break;
        case ClassDef::Interface:  result+="de l'interface "; break;
        case ClassDef::Protocol:   result+="du protocole "; feminine=false; break;
        case ClassDef::Category:   result+="de la catégorie "; break;
        case ClassDef::Exception:  result+="de l'exception "; break;
        default: break;
      }

      if(isLocal)
      {
        result += (feminine) ? "locale " : "local ";
      }

      result += clName;

      return result;
    }
    QCString trOperations() override
    {
        return "Opérations";
    }
    QCString trOperationDocumentation() override
    {
        return "Documentation des opérations";
    }
    QCString trDataMembers() override
    {
        return "Champs de données";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Documentation des champs de données";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Documentation de l'unité de conception"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "concept", "s");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result="Reference du concept ";
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Liste des concepts"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Index des concepts"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Documentation des concepts"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Liste de tous les concepts ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Définition du concept";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Liste des paquetages"; }

};

#endif
