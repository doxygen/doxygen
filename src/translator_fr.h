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
 */

#ifndef TRANSLATOR_FR_H
#define TRANSLATOR_FR_H

#include <qstring.h>

class TranslatorFrench : public Translator
{
  public:
    QString latexBabelPackage()
    { return "french"; }
    QString trInherits()
    { return "Hérite de"; }
    QString trAnd()
    { return "et"; }
    QString trInheritedBy()
    { return "Dérivée par"; }
    QString trRelatedFunctions()
    { return "Fonctions associées"; }
    QString trRelatedSubscript()
    { return "(Noter que ces fonctions ne sont pas des méthodes de la classe)"; }
    QString trDetailedDescription()
    { return "Description détaillée"; }
    QString trMemberTypedefDocumentation()
    { return "Documentation des types imbriqués"; }
    QString trMemberEnumerationDocumentation()
    { return "Documentation des énumérations imbriqués"; }
    QString trMemberFunctionDocumentation()
    { return "Documentation des méthodes"; }
    QString trMemberDataDocumentation()
    { return "Documentation des données imbriqués"; }
    QString trGeneratedFrom(const char *s,bool single)
    { 
      QString result=(QString)"La documentation pour cette"+s+
                     " a été générée à partir ";
      if (single) result+="du fichier suivant:";   
      else result+="des fichiers suivants:";
      return result;
    }
    QString trMore()
    { return "Plus de détails..."; }
    QString trReference()
    { return "Référence"; }
    QString trListOfAllMembers()
    { return "Liste de tous les membres"; }
    QString trMemberList()
    { return "Liste des membres"; }
    QString trThisIsTheListOfAllMembers()
    { return "Ceci est la liste complète des membres de"; }
    QString trIncludingInheritedMembers()
    { return "y compris des membres des classes héritées."; }
    QString trGeneratedAutomatically(const char *s)
    { QString result="Généré automatiquement par Doxygen"; 
      if (s) result+=(QString)" pour "+s;
      result+=" à partir du code source."; 
      return result;
    }
    QString trEnumName()
    { return "énumération"; }
    QString trEnumValue()
    { return "élément d'une énumération"; }
    QString trDefinedIn()
    { return "défini dans"; }
    QString trIncludeFile()
    { return "Fichier inclu"; }
    QString trVerbatimText(const char *f)
    { return (QString)"Ce texte provient du fichier inclu "+f+"."; }


    QString trModules()
    { return "Modules"; }
    QString trClassHierarchy()
    { return "Hiérarchie des classes"; }
    QString trCompoundList()
    { return "Liste des composants"; }
    QString trFileList()
    { return "Liste des fichiers"; }
    QString trHeaderFiles()
    { return "Fichiers d'entête"; }
    QString trCompoundMembers()
    { return "Composants"; }
    QString trFileMembers()
    { return "Déclarations"; }
    QString trRelatedPages()
    { return "Pages associées"; }
    QString trExamples()
    { return "Exemples"; }
    QString trSearch()
    { return "Recherche"; }
    QString trClassHierarchyDescription()
    { return "Cette liste d'héritage est, autant que possible, "
	"classée par ordre alphabétique"; }
    QString trFileListDescription(bool extractAll)
    {
      QString result="Liste de tous les fichiers ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }
    QString trCompoundListDescription()
    { return "Liste des classes, des strutures et des unions "
	"avec une brève description :"; 
    }
    QString trCompoundMembersDescription(bool extractAll)
    {
      QString result="Liste de tous les membres de classe ";
      if (!extractAll) result+="documentés ";
      result+="avec les liens vers ";
      if (extractAll) result+="la documentation de la classe correspondante :";
      else result+="les classes auxquelles ils appartiennent :";
      return result;
    }
    QString trFileMembersDescription(bool extractAll)
    {
      QString result="Liste de toutes les déclarations";
      if (!extractAll) result+="documentées ";
      result+="avec liens sur ";
      if (extractAll) result+="la documentation du fichier correspondant "
			"pour chacune :";
      else result+="les fichiers dans lesquels elles sont définies :";
      return result;
    }
    QString trHeaderFilesDescription()
    { return "Liste de tous les fichiers d'entête constituant "
	"l'interface de programmation :"; }
    QString trExamplesDescription()
    { return "Liste de tous les exemples :"; }
    QString trRelatedPagesDescription()
    { return "Liste de toutes les pages de documentation associées :"; }
    QString trModulesDescription()
    { return "Liste de tous les modules"; }
    QString trNoDescriptionAvailable()
    { return "Aucune description n'est disponible"; }

    QString trDocumentation()
    { return "Documentation"; }
    QString trModuleIndex()
    { return "Index des modules"; }
    QString trHierarchicalIndex()
    { return "Index hiérarchique"; }
    QString trCompoundIndex()
    { return "Index des composants"; }
    QString trFileIndex() 
    { return "Index des fichiers"; }
    QString trModuleDocumentation()
    { return "Documentation du module"; }
    QString trClassDocumentation()
    { return "Documentation de la classe"; }
    QString trFileDocumentation()
    { return "Documentation du fichier"; }
    QString trExampleDocumentation()
    { return "Documentation de l'exemple"; }
    QString trPageDocumentation()
    { return "Documentation de la page"; }
    QString trReferenceManual()
    { return "Manuel de référence"; }

    QString trDefines()
    { return "Définitions des macros"; }
    QString trFuncProtos()
    { return "Prototypes des fonctions"; }
    QString trTypedefs()
    { return "Définitions des types"; }
    QString trEnumerations()
    { return "Enumérations"; }
    QString trFunctions()
    { return "Fonctions"; }
    QString trVariables()
    { return "Variables"; }
    QString trEnumerationValues()
    { return "Eléments énumérés"; }
    QString trReimplementedFrom()
    { return "Redéfini à partir de"; }
    QString trReimplementedIn()
    { return "Redéfini dans"; }
    QString trAuthor()
    { return "Auteur"; }
    QString trDefineDocumentation()
    { return "Documentation de la macro"; }
    QString trFunctionPrototypeDocumentation()
    { return "Documentation du prototype de la fonction"; }
    QString trTypedefDocumentation()
    { return "Documentation du type"; }
    QString trEnumerationTypeDocumentation()
    { return "Documentation du type de l'énumeration"; }
    QString trEnumerationValueDocumentation()
    { return "Documentation de l'élément de l'énumeration"; }
    QString trFunctionDocumentation()
    { return "Documentation de la fonction"; }
    QString trVariableDocumentation()
    { return "Documentation de la variable"; }
    QString trCompounds()
    { return "Composants"; }
    QString trFiles()
    { return "Fichiers"; }
    QString trGeneratedAt(const char *date,const char *projName)
    { 
      QString result=(QString)"Généré le "+date;
      if (projName) result+=(QString)" pour "+projName;
      result+=(QString)" par ";
      return result;
    }
    QString trWrittenBy()
    {
      return "écrit par";
    }
    QString trClassDiagram(const char *clName)
    {
      return (QString)"Graphe d'héritage de la classe "+clName;
    }
    QString trForInternalUseOnly()
    { return "A usage interne uniquement."; }
    QString trReimplementedForInternalReasons()
    { return "Redéfini pour des raisons internes; "
             "l'interface n'est pas modifiée"; 
    }
    QString trWarning()
    { return "Avertissement"; }
    QString trBugsAndLimitations()
    { return "Bogues et limitations"; }
    QString trVersion()
    { return "Version"; }
    QString trDate()
    { return "Date"; }
    QString trAuthors()
    { return "Auteur(s)"; }
    QString trReturns()
    { return "Renvoie"; }
    QString trSeeAlso()
    { return "Voir également"; }
    QString trParameters()
    { return "Paramètres"; }
    QString trExceptions()
    { return "Exceptions"; }
    QString trGeneratedBy()
    { return "Généré par"; }

    // new since 0.49-990307
    
    virtual QString trNamespaces()
    { return "Namespaces"; }
    virtual QString trNamespaceList()
    { return "Liste des Namespaces"; }
    virtual QString trNamespaceListDescription(bool extractAll)
    {
      QString result="Liste de tous les namespaces ";
      if (!extractAll) result+="documentés ";
      result+="avec une brève description :";
      return result;
    }
    virtual QString trFriends()
    { return "Friends"; }
    
    // new since 0.49-990405
    
    virtual QString trRelatedFunctionDocumentation()
    { return "Documentation des fonctions amies et associées"; }
};

#endif
