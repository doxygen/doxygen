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


/**
 * translator_es.h modifief by Francisco Oltra 
 * Some notes: 
 * - Computer related use of the spanish language differs from
 * country to country, so some words might not sound so good
 * to some people (but it does to me, *grin* ). A good example
 * is the spanish for File: i use "Archivo", others may prefer
 * "Fichero".
 * - Some words were not translated for understandability, since
 * we are talking about a  computer program here!
 * - Some of the functions are called in some order coherent with
 * the english grammar, this results in some ugly constructs.
 */

#ifndef TRANSLATOR_ES_H
#define TRANSLATOR_ES_H

#include "translator.h"

class TranslatorSpanish : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "spanish"; }
    QCString trRelatedFunctions()
    { return "Funciones Relacionadas"; }
    QCString trRelatedSubscript()
    { return "(Note que estos no son funciones miembro.)"; }
    QCString trDetailedDescription()
    { return "Descripción Detallada"; }
    QCString trMemberTypedefDocumentation()
    { return "Documentación de Miembros Typedef"; }
    QCString trMemberEnumerationDocumentation()
    { return "Documentación de Miembros de Enumeración"; }
    QCString trEnumerationValueDocumentation()
    { return "Documentación de los Valores de Enumeración"; }
    QCString trMemberFunctionDocumentation()
    { return "Documentación de Funciones Miembro"; }
    QCString trMemberDataDocumentation()
    { return "Documentación de Datos Miembro"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"La documentación para esta"+s+
                     " fue generada del siguiente archivo";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QCString trMore()
    { return "Más..."; }
    QCString trListOfAllMembers()
    { return "Lista de todos los miembros."; }
    QCString trMemberList()
    { return "Lista de Miembros"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Esta es la lista completa de miembros para "; }
    QCString trIncludingInheritedMembers()
    { return ", incluyendo todos los miembros heredados."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generado automáticamente por Doxygen"; 
      if (s) result+=(QCString)" para "+s;
      result+=" desde el código fuente."; 
      return result;
    }
    QCString trEnumName()
    { return "nombre de enum"; }
    QCString trEnumValue()
    { return "valor de enum"; }
    QCString trDefinedIn()
    { return "definido en"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Esto es texto verbatim del archivo incluído "+f+"."; }
    QCString trModules()
    { return "Modulos"; }
    QCString trClassHierarchy()
    { return "Jerarquía de la clase"; }
    QCString trCompoundList()
    { return "Lista de Tipos Compuestos"; }
    QCString trFileList()
    { return "Lista de Archivos"; }
    QCString trHeaderFiles()
    { return "Archivos de Cabecera"; }
    QCString trCompoundMembers()
    { return "Miembros de Compuestos"; }
    QCString trFileMembers()
    { return "Miembros de los Archivos"; }
    QCString trRelatedPages()
    { return "Páginas Relacionadas"; }
    QCString trExamples()
    { return "Ejemplos"; }
    QCString trSearch()
    { return "Buscar"; }
    QCString trClassHierarchyDescription()
    { return "Esta lista de herencia está ordenada alfabéticamente, "
             "pero no completamente:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Esta es una lista de todos ";
      result+="los archivos ";
      if (!extractAll) result+="documentados ";
      result+="con breves descripciones:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Aquí estan las clases, estructuras y "
             "uniones con descripciones breves:"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Esta es una lista de todos ";
      result+="miembros de clases ";
      if (!extractAll) result+="documentados ";
      result+="con enlaces a ";
      if (extractAll) result+="la documentación de clase para cada miembro:";
      else result+="las clases a que pertenecen:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Esta es una lista de todos los miembros de los archivos ";
      if (!extractAll) result+="documentados ";
      result+="con enlaces a ";
      if (extractAll) result+="la documentación de archivo para cada miembro:";
      else result+="los archivos a que pertenecen:";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Aquí estan los archivos de cabecera que forman el API:"; }
    QCString trExamplesDescription()
    { return "Esta es la lista de todos los ejemplos:"; }
    QCString trRelatedPagesDescription()
    { return "Esta es una lista de todos las páginas con documentación relacionada:"; }
    QCString trModulesDescription()
    { return "Esta es una lista con todos los modulos:"; }
    QCString trNoDescriptionAvailable()
    { return "No existe descripción disponible"; }

    QCString trDocumentation()
    { return "Documentación"; }
    QCString trModuleIndex()
    { return "Índice de Modulos"; }
    QCString trHierarchicalIndex()
    { return "Índice Jerárquico"; }
    QCString trCompoundIndex()
    { return "Índice de Tipos Compuestos"; }
    QCString trFileIndex() 
    { return "Índice de Archivos"; }
    QCString trModuleDocumentation()
    { return "Documentación de los Modulos"; }
    QCString trClassDocumentation()
    { return "Documentación de las clases"; }
    QCString trFileDocumentation()
    { return "Documentación de los Archivos"; }
    QCString trExampleDocumentation()
    { return "Documentación de los Ejemplos"; }
    QCString trPageDocumentation()
    { return "Documentación de las Páginas Relacionadas"; }
    QCString trReferenceManual()
    { return "Manual de Referencia"; }

    QCString trDefines()
    { return "Definiciones"; }
    QCString trFuncProtos()
    { return "Prototipo de Funciones"; }
    QCString trTypedefs()
    { return "Typedefs"; }
    QCString trEnumerations()
    { return "Enumeraciones"; }
    QCString trFunctions()
    { return "Funciones"; }
    QCString trVariables()
    { return "Variables"; }
    QCString trEnumerationValues()
    { return "Valores de Enumeraciones"; }
    QCString trAuthor()
    { return "Autor"; }
    QCString trDefineDocumentation()
    { return "Documentación de las Definiciones"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Documentación de los Prototipos de Funciones"; }
    QCString trTypedefDocumentation()
    { return "Documentación de Typedefs"; }
    QCString trEnumerationTypeDocumentation()
    { return "Documentación de Enumeración de tipos"; }
    QCString trFunctionDocumentation()
    { return "Documentación de Funciones"; }
    QCString trVariableDocumentation()
    { return "Documentación de Variables"; }
    QCString trCompounds()
    { return "Tipos Compuestos"; }
    QCString trFiles()
    { return "Archivos"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generado el "+date;
      if (projName) result+=(QCString)" para "+projName;
      result+=(QCString)" por";
      return result;
    }
    QCString trWrittenBy()
    {
      return "escrito por";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagrama de clases para "+clName;
    }

    QCString trForInternalUseOnly()
    { return "Para uso interno solamente."; }
    QCString trReimplementedForInternalReasons()
    { return "Reimplementado por razones internas; el API no se ve afectado."; 
    }
    QCString trWarning()
    { return "Atención"; }
    QCString trBugsAndLimitations()
    { return "Bugs y Limitaciones"; }
    QCString trVersion()
    { return "Versión"; }
    QCString trDate()
    { return "Fecha"; }
    QCString trAuthors()
    { return "Autor(es)"; }
    QCString trReturns()
    { return "Devuelve"; }
    QCString trSeeAlso()
    { return "Ver también"; }
    QCString trParameters()
    { return "Parámetros"; }
    QCString trExceptions()
    { return "Excepciones"; }
    QCString trGeneratedBy()
    { return "Generado por"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return "Lista de Namespaces"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Esta es una lista de los namespaces ";
      if (!extractAll) result+="documentados ";
      result+="con descripciones breves:";
      return result;
    }
    QCString trFriends()
    { return "Clases Amigas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "Documentación de Clases Amigas y Funciones Relacionadas"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Clase"; break;
        case ClassDef::Struct: result+=" Estructura"; break;
        case ClassDef::Union:  result+=" Unión"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
      }
      result+=" Referencia";
      return result;
    }
    virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" Referencia de Archivos"; 
      return result;
    }
    virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Referencia de Namespace";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QCString trPublicMembers()
    { return "Miembros Públicos"; }
    virtual QCString trPublicSlots()
    { return "Slots Públicos"; }
    virtual QCString trSignals()
    { return "Señales"; }
    virtual QCString trStaticPublicMembers()
    { return "Miembros Estáticos Públicos"; }
    virtual QCString trProtectedMembers()
    { return "Miembros Protegidos"; }
    virtual QCString trProtectedSlots()
    { return "Slots Protegidos"; }
    virtual QCString trStaticProtectedMembers()
    { return "Miembros Protegidos Estáticos"; }
    virtual QCString trPrivateMembers()
    { return "Miembros Privados"; }
    virtual QCString trPrivateSlots()
    { return "Slots Privados"; }
    virtual QCString trStaticPrivateMembers()
    { return "Miembros Estáticos Privados"; }
    // end of member sections 
    
    virtual QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+=" y ";
        }
      }
      return result; 
    }
    
    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Hereda a "+trWriteList(numEntries)+".";
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Heredado por "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Reimplementado por "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplementado en "+trWriteList(numEntries)+".";
    }

    virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Miembros del Namespace"; }
    virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="Esta es una lista de todos los miembros de namespace ";
      if (!extractAll) result+="documentados ";
      result+="con enlaces a ";
      if (extractAll) 
        result+="a la documentación de namespaces para cada miembro:";
      else 
        result+="los namespaces a los que pertenecen:";
      return result;
    }
    virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Índice de Namespaces"; }
    virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Documentación de Namespaces"; }
};

#endif
