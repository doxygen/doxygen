/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 * The translation into French was provided by
 *   Christophe Bordeux (bordeux@lig.di.epfl.ch)
 */

#ifndef TRANSLATOR_FR_H
#define TRANSLATOR_FR_H

#include "translator.h"

class TranslatorFrench : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "french"; }
    QCString trInherits()
    { return "Hérite de"; }
    QCString trAnd()
    { return "et"; }
    QCString trInheritedBy()
    { return "Dérivée par"; }
    QCString trRelatedFunctions()
    { return "Fonctions associées"; }
    QCString trRelatedSubscript()
    { return "(Noter que ces fonctions ne sont pas des méthodes de la classe)"; }
    QCString trDetailedDescription()
    { return "Description détaillée"; }
    QCString trMemberTypedefDocumentation()
    { return "Documentation des types imbriqués"; }
    QCString trMemberEnumerationDocumentation()
    { return "Documentation des énumérations imbriqués"; }
    QCString trMemberFunctionDocumentation()
    { return "Documentation des méthodes"; }
    QCString trMemberDataDocumentation()
    { return "Documentation des données imbriqués"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"La documentation pour cette"+s+
                     " a été générée à partir ";
      if (single) result+="du fichier suivant:";   
      else result+="des fichiers suivants:";
      return result;
    }
    QCString trMore()
    { return "Plus de détails..."; }
    QCString trReference()
    { return "Référence"; }
    QCString trListOfAllMembers()
    { return "Liste de tous les membres"; }
    QCString trMemberList()
    { return "Liste des membres"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Ceci est la liste complète des membres de"; }
    QCString trIncludingInheritedMembers()
    { return "y compris des membres des classes héritées."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Généré automatiquement par Doxygen"; 
      if (s) result+=(QCString)" pour "+s;
      result+=" à partir du code source."; 
      return result;
    }
    QCString trEnumName()
    { return "énumération"; }
    QCString trEnumValue()
    { return "élément d'une énumération"; }
    QCString trDefinedIn()
    { return "défini dans"; }
    QCString trIncludeFile()
    { return "Fichier inclu"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Ce texte provient du fichier inclu "+f+"."; }


    QCString trModules()
    { return "Modules"; }
    QCString trClassHierarchy()
    { return "Hiérarchie des classes"; }
    QCString trCompoundList()
    { return "Liste des composants"; }
    QCString trFileList()
    { return "Liste des fichiers"; }
    QCString trHeaderFiles()
    { return "Fichiers d'entête"; }
    QCString trCompoundMembers()
    { return "Composants"; }
    QCString trFileMembers()
    { return "Déclarations"; }
    QCString trRelatedPages()
    { return "Pages associées"; }
    QCString trExamples()
    { return "Exemples"; }
    QCString trSearch()
    { return "Recherche"; }
    QCString trClassHierarchyDescription()
    { return "Cette liste d'héritage est, autant que possible, "
	"classée par ordre alphabétique"; }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Liste de tous les fichiers ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Liste des classes, des strutures et des unions "
	"avec une brève description :"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Liste de tous les membres de classe ";
      if (!extractAll) result+="documentés ";
      result+="avec les liens vers ";
      if (extractAll) result+="la documentation de la classe correspondante :";
      else result+="les classes auxquelles ils appartiennent :";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Liste de toutes les déclarations";
      if (!extractAll) result+="documentées ";
      result+="avec liens sur ";
      if (extractAll) result+="la documentation du fichier correspondant "
			"pour chacune :";
      else result+="les fichiers dans lesquels elles sont définies :";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Liste de tous les fichiers d'entête constituant "
	"l'interface de programmation :"; }
    QCString trExamplesDescription()
    { return "Liste de tous les exemples :"; }
    QCString trRelatedPagesDescription()
    { return "Liste de toutes les pages de documentation associées :"; }
    QCString trModulesDescription()
    { return "Liste de tous les modules"; }
    QCString trNoDescriptionAvailable()
    { return "Aucune description n'est disponible"; }

    QCString trDocumentation()
    { return "Documentation"; }
    QCString trModuleIndex()
    { return "Index des modules"; }
    QCString trHierarchicalIndex()
    { return "Index hiérarchique"; }
    QCString trCompoundIndex()
    { return "Index des composants"; }
    QCString trFileIndex() 
    { return "Index des fichiers"; }
    QCString trModuleDocumentation()
    { return "Documentation du module"; }
    QCString trClassDocumentation()
    { return "Documentation de la classe"; }
    QCString trFileDocumentation()
    { return "Documentation du fichier"; }
    QCString trExampleDocumentation()
    { return "Documentation de l'exemple"; }
    QCString trPageDocumentation()
    { return "Documentation de la page"; }
    QCString trReferenceManual()
    { return "Manuel de référence"; }

    QCString trDefines()
    { return "Définitions des macros"; }
    QCString trFuncProtos()
    { return "Prototypes des fonctions"; }
    QCString trTypedefs()
    { return "Définitions des types"; }
    QCString trEnumerations()
    { return "Enumérations"; }
    QCString trFunctions()
    { return "Fonctions"; }
    QCString trVariables()
    { return "Variables"; }
    QCString trEnumerationValues()
    { return "Eléments énumérés"; }
    QCString trReimplementedFrom()
    { return "Redéfini à partir de"; }
    QCString trReimplementedIn()
    { return "Redéfini dans"; }
    QCString trAuthor()
    { return "Auteur"; }
    QCString trDefineDocumentation()
    { return "Documentation de la macro"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Documentation du prototype de la fonction"; }
    QCString trTypedefDocumentation()
    { return "Documentation du type"; }
    QCString trEnumerationTypeDocumentation()
    { return "Documentation du type de l'énumeration"; }
    QCString trEnumerationValueDocumentation()
    { return "Documentation de l'élément de l'énumeration"; }
    QCString trFunctionDocumentation()
    { return "Documentation de la fonction"; }
    QCString trVariableDocumentation()
    { return "Documentation de la variable"; }
    QCString trCompounds()
    { return "Composants"; }
    QCString trFiles()
    { return "Fichiers"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Généré le "+date;
      if (projName) result+=(QCString)" pour "+projName;
      result+=(QCString)" par ";
      return result;
    }
    QCString trWrittenBy()
    {
      return "écrit par";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Graphe d'héritage de la classe "+clName;
    }
    QCString trForInternalUseOnly()
    { return "A usage interne uniquement."; }
    QCString trReimplementedForInternalReasons()
    { return "Redéfini pour des raisons internes; "
             "l'interface n'est pas modifiée"; 
    }
    QCString trWarning()
    { return "Avertissement"; }
    QCString trBugsAndLimitations()
    { return "Bogues et limitations"; }
    QCString trVersion()
    { return "Version"; }
    QCString trDate()
    { return "Date"; }
    QCString trAuthors()
    { return "Auteur(s)"; }
    QCString trReturns()
    { return "Renvoie"; }
    QCString trSeeAlso()
    { return "Voir également"; }
    QCString trParameters()
    { return "Paramètres"; }
    QCString trExceptions()
    { return "Exceptions"; }
    QCString trGeneratedBy()
    { return "Généré par"; }

    // new since 0.49-990307
    
    virtual QCString trNamespaces()
    { return "Namespaces"; }
    virtual QCString trNamespaceList()
    { return "Liste des Namespaces"; }
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Liste de tous les namespaces ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }
    virtual QCString trFriends()
    { return "Friends"; }
    
    // new since 0.49-990405
    
    virtual QCString trRelatedFunctionDocumentation()
    { return "Documentation des fonctions amies et associées"; }
};

#endif
