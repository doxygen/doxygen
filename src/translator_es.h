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
 */

#ifndef TRANSLATOR_ES_H
#define TRANSLATOR_ES_H

/*!
 * translator_es.h modified by Lucas Cruz (7-julio-2000)
 * Updated from 1.3.8 to 1.4.6 by Guillermo Ballester Valor (May-05-2006)
 * Updated to 1.5.1 by Bartomeu Creus Navarro (22-enero-2007)
 * Updated to 1.5.5 by Bartomeu Creus Navarro (5-febrero-2008)
 * Updated to 1.5.8 by Bartomeu Creus Navarro (10-abril-2009)
 * Updated to 1.6.3 by Bartomeu Creus Navarro (3-marzo-2010)
 * Updated to 1.6.4 by Bartomeu Creus Navarro (26-mayo-2010) [(16-jun-2010) grabado en UTF-8]
 * Updated to 1.8.0 by Bartomeu Creus Navarro (11-abril-2012)
 * Updated to 1.8.2 by Bartomeu Creus Navarro (01-julio-2012)
 * Updated to 1.8.4 by Bartomeu Creus Navarro (17-julio-2013)
 * Updated to 1.9.6 by David H. Martín (28-diciembre-2022)
*/
class TranslatorSpanish : public TranslatorAdapter_1_9_6
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
    { return "spanish"; }

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
      return "\\usepackage[spanish]{babel}";
    }

    virtual QCString trISOLang()
    {
      return "es";
    }

    virtual QCString getLanguageString()
    {
      return "0x40A Spanish";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funciones relacionadas"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Observar que estas no son funciones miembro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descripción detallada"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails()
    { return "Detalles"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Documentación de los «Typedef» miembros de la clase"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Documentación de las enumeraciones miembro de la clase"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Documentación de funciones/procedimientos/procesos miembro";
      }
      else
      {
        return "Documentación de funciones miembro";
      }
    }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentación de campos";
      }
      else
      {
        return "Documentación de datos miembro";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Más..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista de todos los miembros"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Lista de los miembros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Lista completa de los miembros de "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", incluyendo todos los heredados:"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s)
    { QCString result="Generado automáticamente por Doxygen";
      if (!s.isEmpty()) result+=" para "+s;
      result+=" del código fuente.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nombre de enumeración"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "valor enumerado"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definido en"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Módulos"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Jerarquía de clases"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructuras de datos";
      }
      else
      {
        return "Lista de clases";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista de archivos"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
    if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos de datos";
      }
      else
      {
        return "Miembros de clases";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globales";
      }
      else
      {
        return "Miembros de los archivos";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Páginas relacionadas"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Ejemplos"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Buscar"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Esta es la lista jerárquica de todas las entidades:";
      }
      else
      {
        return "Este listado de herencia esta ordenado de forma general "
               "pero no está en orden alfabético estricto:";
      }
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Lista de todos los archivos ";
      if (!extractAll) result+="documentados y ";
      result+="con breves descripciones:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Lista de estructuras con breves descripciones:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Lista de clases con breves descripciones:";
      }
      else
      {
       return "Lista de clases, estructuras, "
             "uniones e interfaces con breves descripciones:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Lista de todos los ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="campos de estructuras y uniones";
      }
      else
      {
        result+="campos de clases";
      }
      if (!extractAll)
      {
        result+=" documentados";
      }
      result+=" con enlaces a ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="la documentación de la estructura/unión para cada campo:";
        }
        else
        {
          result+="la documentación de la clase para cada miembro:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="las estructuras/uniones a las que pertenecen:";
        }
        else
        {
          result+="las clases a las que pertenecen:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Lista de ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="todas las funciones, variables, «defines», enumeraciones y «typedefs»";
      }
      else
      {
        result+="todos los miembros de los archivos";
      }
      if (!extractAll) result+=" documentados";
      result+=" con enlaces ";
      if (extractAll)
        result+="a los archivos a los que corresponden:";
      else
        result+="a la documentación:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Lista de todos los ejemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Lista de toda la documentación relacionada:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Lista de todos los módulos:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    {
      // TODO: Replace with something like trDocumentationOf(projName).
	  // This will allow the latin construction "Documentación de projName"
	  return "documentación"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Índice de módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Índice jerárquico"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Índice de estructuras de datos";
      }
      else
      {
        return "Índice de clases";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "Índice de archivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Documentación de módulos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentación de estructuras de datos";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Documentación de clases";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Documentación de archivos"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Manual de referencia"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "defines"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumeraciones"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funciones"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variables"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Valores de enumeraciones"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Documentación de «define»"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Documentación de «typedef»"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Documentación de enumeraciones"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Documentación de funciones"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Documentación de variables"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructuras de datos";
      }
      else
      {
        return "Clases";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName)
    {
      QCString result="Generado el "+date;
      if (!projName.isEmpty()) result+=" para "+projName;
      result+=" por";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName)
    {
      return "Diagrama de herencia de "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Atención"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versión"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Fecha"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Devuelve"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Ver también"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parámetros"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Excepciones"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Generado por"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Lista de espacios de nombres"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Lista de ";
      if (!extractAll) result+="todos ";
      result+="los espacios de nombres documentados, con breves descripciones:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Amigas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Documentación de clases amigas y funciones relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                         ClassDef::CompoundType compType,
                                         bool isTemplate)
    {
      QCString result="Referencia de";
      if (isTemplate) result+=" la plantilla de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" la clase "; break;
        case ClassDef::Struct:     result+=" la estructura "; break;
        case ClassDef::Union:      result+=" la unión "; break;
        case ClassDef::Interface:  result+=" la interface "; break;
        case ClassDef::Protocol:   result+="l protocolo "; break;
        case ClassDef::Category:   result+=" la categoría "; break;
        case ClassDef::Exception:  result+=" la excepción "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName)
    {
      QCString result="Referencia del archivo ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName)
    {
      QCString result="Referencia del espacio de nombres ";
      result+=namespaceName;
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Métodos públicos"; }

    virtual QCString trPublicSlots()
    { return "Slots públicos"; }

    virtual QCString trSignals()
    { return "Señales"; }

    virtual QCString trStaticPublicMembers()
    { return "Métodos públicos estáticos"; }

    virtual QCString trProtectedMembers()
    { return "Métodos protegidos"; }

    virtual QCString trProtectedSlots()
    { return "Slots protegidos"; }

    virtual QCString trStaticProtectedMembers()
    { return "Métodos protegidos estáticos"; }

    virtual QCString trPrivateMembers()
    { return "Métodos privados"; }

    virtual QCString trPrivateSlots()
    { return "Slots privados"; }

    virtual QCString trStaticPrivateMembers()
    { return "Métodos privados estáticos"; }

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
            result+=" y ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Hereda de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Heredado por "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementado de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementado en "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Miembros del espacio de nombres "; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Lista de todos los miembros de espacios de nombres ";
      if (!extractAll) result+="documentados ";
      result+="con enlaces a ";
      if (extractAll)
        result+="la documentación del espacio de nombres de cada miembro:";
      else
        result+="al espacio de nombres al que pertenecen:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Índice de espacios de nombres"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Documentación de espacios de nombres"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Espacios de nombres"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="La documentación de est";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt? "a unidad de diseño":"a clase"; break;
        case ClassDef::Struct:     result+="a estructura"; break;
        case ClassDef::Union:      result+="a unión"; break;
        case ClassDef::Interface:  result+="a interface"; break;
        case ClassDef::Protocol:   result+="e protocolo"; break;
        case ClassDef::Category:   result+="a categoría"; break;
        case ClassDef::Exception:  result+="a excepción"; break;
        default: break;
      }
      result+=" está generada de";
      if (single) result+="l siguiente archivo:";
      else result+=" los siguientes archivos:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Valores devueltos"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Página principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "pág."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definición en la línea @0 del archivo @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definición en el archivo @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
    return "Obsoleto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName)
    {
      return "Diagrama de colaboración de "+clName+":";
    }

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName)
    {
      return "Gráfico de dependencias incluidas en "+fName+":";
    }

    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentación de constructores y destructores";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Ir al código fuente de este archivo.";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Ir a la documentación de este archivo.";
    }

    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondición";
    }

    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Postcondición";
    }

    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariante";
    }

    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Valor inicial:";
    }

    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "código fuente";
    }

    virtual QCString trGraphicalHierarchy()
    {
      return "Gráfico de jerarquía de clases";
    }

    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Ir al gráfico de jerarquía de clases";
    }

    virtual QCString trGotoTextualHierarchy()
    {
      return "Ir al texto de jerarquía de clases";
    }

    virtual QCString trPageIndex()
    {
      return "Índice de páginas";
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
      return "Tipos públicos";
    }

    virtual QCString trPublicAttribs()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos de datos";
      }
      else
      {
        return "Atributos públicos";
      }
    }

    virtual QCString trStaticPublicAttribs()
    {
      return "Atributos públicos estáticos";
    }

    virtual QCString trProtectedTypes()
    {
      return "Tipos protegidos";
    }

    virtual QCString trProtectedAttribs()
    {
      return "Atributos protegidos";
    }

    virtual QCString trStaticProtectedAttribs()
    {
      return "Atributos estáticos protegidos";
    }

    virtual QCString trPrivateTypes()
    {
      return "Tipos privados";
    }

    virtual QCString trPrivateAttribs()
    {
      return "Atributos privados";
    }

    virtual QCString trStaticPrivateAttribs()
    {
      return "Atributos estáticos privados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Tareas pendientes";
    }

    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Lista de tareas pendientes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referenciado por";
    }

    virtual QCString trRemarks()
    {
      return "Comentarios";
    }

    virtual QCString trAttention()
    {
      return "Atención";
    }

    virtual QCString trInclByDepGraph()
    {
      return "Gráfico de los archivos que directa o "
              "indirectamente incluyen a este archivo:";
    }

    virtual QCString trSince()
    {
      return "Desde";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle()
    {
      return "Leyenda del gráfico";
    }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return
        "Esta página explica cómo interpretar los gráficos generados "
        "por doxygen.<p>\n"
        "Considérese el siguiente ejemplo:\n"
        "\\code\n"
        "/*! Clase invisible por truncamiento */\n"
        "class Invisible { };\n\n"
        "/*! Clase truncada, relación de herencia escondida */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Clase no documentada con comentarios de doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Clase que es heredera usando herencia publica */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Plantilla de clases */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Clase que es heredera usando herencia protegida  */\n"
        "class ProtectedBase { };\n\n"
        "/*! Clase que es heredera usando herencia privada  */\n"
        "class PrivateBase { };\n\n"
        "/*! Clase que es usada por la clase heredada */\n"
        "class Used { };\n\n"
        "/*! Superclase que hereda de varias otras clases */\n"
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
        "Dará como resultado el siguiente gráfico:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Las cajas del gráfico superior tienen el siguiente significado:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Una caja gris rellena representa la estructura o clase para la cual "
        "se generó el gráfico.\n"
        "<li>Una caja con borde negro indica una estructura o clase documentada.\n"
        "<li>Una caja con borde gris indica una estructura o clase no documentada.\n"
        "<li>Una caja con borde rojo indica una estructura o clase documentada"
        " de la que no todas las relaciones de herencia/asociación se "
        "muestran. Un gráfico queda cortado si no encaja dentro de los "
        "límites especificados."
        "</ul>\n"
        "<p>\n"
        "Las flechas tienen el siguiente significado:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Una flecha azul oscuro se usa para mostrar una relación de herencia publica entre dos clases.\n"
        "<li>Una flecha verde oscuro se usa para una herencia protegida.\n"
        "<li>Una flecha rojo oscuro se usa para herencia privada.\n"
        "<li>Una flecha púrpura discontinua se usa si la clase está contenida o "
        "se usa por otra clase. La flecha está etiquetada por la variable "
        "con que se accede a la clase o estructura apuntada. \n"
        "<li>Una flecha amarilla discontinua indica la relación entre una instancia de plantilla y la clase de plantilla de la que se ha instanciado."
        " La flecha se etiqueta con los parámetros de plantilla de la instancia.\n"
        "</ul>\n";
    }

    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "leyenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Prueba";
    }

    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Lista de pruebas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Propiedades";
    }

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentación de propiedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructuras de datos";
      }
      else
      {
        return "Clases";
      }
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name)
    {
      return "Paquete "+name;
    }


    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Estos son los paquetes con breves descripciones (si están disponibles):";
    }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paquetes";
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
      return "Defecto";
    }

    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Lista de defectos";
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
      return "Índice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Clase" : "clase"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Archivo" : "archivo"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Espacio" : "espacio"));
      if (!singular)  result+="s";
      result+=" de nombres";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Módulo" : "módulo"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Página" : "página"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Miembro" : "miembro"));
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
      if (!singular)  result+="es";
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
      if (!singular)  result+="es";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Hace referencia a";
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
      return "Implementado en "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Tabla de contenidos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista de obsoletos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Eventos";
    }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Documentación de eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Tipos de paquete";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions()
    {
      return "Funciones de paquete";
    }
    virtual QCString trPackageMembers()
    {
      return "Miembros de paquete";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions()
    {
      return "Funciones estáticas de paquete";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Atributos de paquete";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Atributos estáticos de paquete";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Todos";
    }

    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Gráfico de llamadas de esta función:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Resultados de la búsqueda";
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
        return "Disculpe, no se encontraron documentos que coincidan con su búsqueda.";
      }
      else if (numDocuments==1)
      {
        return "Se encontró <b>1</b> documento que coincide con su búsqueda.";
      }
      else
      {
        return "Se encontraron <b>$num</b> documentos que coinciden con su búsqueda. "
                "Se muestran los mejores resultados primero.";
      }
    }

    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Coincidencias:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Archivo de código fuente " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Jerarquía de directorios"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Documentación de directorios"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Directorios"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName)
    {
      QCString result="Referencia del directorio ";
      result+=dirName;
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Directorio" : "directorio"));
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
       return "Esta es una función miembro sobrecargada que se "
               "suministra por conveniencia. Difiere de la anterior "
               "función solamente en los argumentos que acepta.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Gráfico de llamadas a esta función:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentación de valores enumerados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentación de funciones/subrutinas miembros"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Lista de campos de datos"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Campos de datos"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Esta es la lista de los tipos de datos con breves descripciones:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Esta es la lista de todos ";
      result+="los tipos de datos miembro ";
      if (!extractAll)
      {
        result+="documentados ";
      }
      result+="con enlaces a ";
      if (!extractAll)
      {
         result+="la documentación de la estructura de datos para cada miembro";
      }
      else
      {
         result+="los tipos de datos a los que pertenecen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Índice de tipos de datos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentación de tipos de datos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funciones/subrutinas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentación de funciones/subrutinas"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tipos de datos"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Lista de módulos"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Lista de todos los módulos ";
      if (!extractAll) result+="documentados ";
      result+="con breves descripciones:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referencia de";
      if (isTemplate) result+=" la plantilla de";
      switch(compType)
      {
        case ClassDef::Class:      result+="l módulo"; break;
        case ClassDef::Struct:     result+="l tipo"; break;
        case ClassDef::Union:      result+=" la unión"; break;
        case ClassDef::Interface:  result+=" la interface"; break;
        case ClassDef::Protocol:   result+="l protocolo"; break;
        case ClassDef::Category:   result+=" la categoría"; break;
        case ClassDef::Exception:  result+=" la excepción"; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName)
    {
      QCString result="Referencia del módulo ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Miembros del módulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result="Lista de todos los miembros del módulo ";
      if (!extractAll) result+="documentados ";
      result+="con enlaces ";
      if (extractAll)
      {
        result+="a la documentación del módulo para cada uno:";
      }
      else
      {
        result+="al módulo al que pertenecen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Índice de módulos"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Módulo" : "módulo"));
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
      QCString result="La documentación de est";
      switch(compType)
      {
    	case ClassDef::Class:      result+="e módulo"; break;
    	case ClassDef::Struct:     result+="e tipo"; break;
    	case ClassDef::Union:      result+="a unión"; break;
    	case ClassDef::Interface:  result+="a interface"; break;
    	case ClassDef::Protocol:   result+="e protocolo"; break;
    	case ClassDef::Category:   result+="a categoría"; break;
    	case ClassDef::Exception:  result+="a excepción"; break;
        default: break;
      }
      result+=" está generada de";
      if (single) result+="l siguiente archivo:";
      else result+=" los siguientes archivos:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Tipo" : "tipo"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Subprograma" : "subprograma"));
      if (!singular)  result+="s";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Restricciones de tipo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name)
    {
      return "Relación "+name;
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Cargando...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Espacio de nombres global";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Buscando...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nada coincide";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const QCString &name)
    {
      return "Archivo en "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name)
    {
      return "Incluye archivo en "+name;
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
                                DateTimeType includeTime)
    {
      static const char *days[]   = { "Lunes","Martes","Miércoles","Jueves",
                                      "Viernes","Sábado","Domingo" };
      static const char *months[] = { "enero","febrero","marzo","abril",
                                      "mayo","junio","julio","agosto",
                                      "septiembre","octubre","noviembre","diciembre" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d de %s de %d",days[dayOfWeek-1],day,months[month-1],year);
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
    virtual QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full)
    {
      static const char *days_short[]   = { "lun", "mar", "mié", "jue", "vie", "sáb", "dom" };
      static const char *days_full[]    = { "lunes", "martes", "miércoles", "jueves", "viernes", "sábado", "domingo" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return first_capital? text.mid(0,1).upper()+text.mid(1) : text;
    }
    virtual QCString trMonth(int month, bool first_capital, bool full)
    {
      static const char *months_short[] = { "ene", "feb", "mar", "abr", "may", "jun", "jul", "ago", "sep", "oct", "nov", "dic" };
      static const char *months_full[]  = { "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return first_capital? text.mid(0,1).upper()+text.mid(1) : text;
    }
    virtual QCString trDayPeriod(int period)
    {
      static const char *dayPeriod[] = { "a. m.", "p. m." };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Referencias bibliográficas"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name)
    { return "Gráfico de dependencias de directorios de "+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "nivel de detalle"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parámetros de plantilla"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "y "+number+" más..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "La documentación de esta enumeración está generada de";
      if (single)
          result += "l siguiente archivo:";
      else
          result += " los siguientes archivos:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name)
    { return "Referencia de la enumeración "+name; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what)
    { return members+" heredados de "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Otros miembros heredados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "habilitar" : "deshabilitar";
      return "pulsar para "+opt+" sincronización";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Proporcionado por la categoría @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Extiende la clase @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Métodos de clase";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Métodos de instancia";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Documentación de métodos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces()
    { return "Interfaces exportadas"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices()
    { return "Servicios incluidos"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups()
    { return "Grupos de constantes"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName)
    {
      QCString result="Referencia a grupos de constantes de ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName)
    {
      QCString result="Referencia a servicios de ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName)
    {
      QCString result="Referencia a «singleton» de ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result="La documentación de este servicio "
                      "está generada de";
      if (single) result+="l siguiente archivo:"; else result+=" los siguientes archivos:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result="La documentación de este «singleton» "
                      "está generada de";
      if (single) result+="l siguiente archivo:"; else result+=" los siguientes archivos:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy()
    { return "Jerarquía de unidades de diseño"; }
    /** VHDL design unit list */
    virtual QCString trDesignUnitList()
    { return "Lista de unidades de diseño"; }
    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers()
    { return "Miembros de unidades de diseño"; }
    /** VHDL design unit list description */
    virtual QCString trDesignUnitListDescription()
    {
        return "Esta es la lista de todos los miembros de unidades de diseño con "
            "enlaces a las entidades a las que pertenecen:";
    }
    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex()
    { return "Índice de unidades de diseño"; }
    /** VHDL design units */
    virtual QCString trDesignUnits()
    { return "Unidades de diseño"; }
    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc()
    { return "Funciones/procedimientos/procesos"; }
    /** VHDL type */
    virtual QCString trVhdlType(uint64_t type,bool single)
    {
      switch(type)
      {
        case VhdlDocGen::LIBRARY:
          if (single) return "Biblioteca";
          else        return "Bibliotecas";
        case VhdlDocGen::PACKAGE:
          if (single) return "Paquete";
          else        return "Paquetes";
        case VhdlDocGen::SIGNAL:
          if (single) return "Señal";
          else        return "Señales";
        case VhdlDocGen::COMPONENT:
          if (single) return "Componente";
          else        return "Componentes";
        case VhdlDocGen::CONSTANT:
          if (single) return "Constante";
          else        return "Constantes";
        case VhdlDocGen::ENTITY:
          if (single) return "Entidad";
          else        return "Entidades";
        case VhdlDocGen::TYPE:
          if (single) return "Tipo";
          else        return "Tipos";
        case VhdlDocGen::SUBTYPE:
          if (single) return "Subtipo";
          else        return "Subtipos";
        case VhdlDocGen::FUNCTION:
          if (single) return "Función";
          else        return "Funciones";
        case VhdlDocGen::RECORD:
          if (single) return "Registro";
          else        return "Registros";
        case VhdlDocGen::PROCEDURE:
          if (single) return "Procedimiento";
          else        return "Procedimientos";
        case VhdlDocGen::ARCHITECTURE:
          if (single) return "Arquitectura";
          else        return "Arquitecturas";
        case VhdlDocGen::ATTRIBUTE:
          if (single) return "Atributo";
          else        return "Atributos";
        case VhdlDocGen::PROCESS:
          if (single) return "Proceso";
          else        return "Procesos";
        case VhdlDocGen::PORT:
          if (single) return "Puerto";
          else        return "Puertos";
        case VhdlDocGen::USE:
          if (single) return "Cláusula de uso";
          else        return "Cláusulas de uso";
        case VhdlDocGen::GENERIC:
          if (single) return "Genérico";
          else        return "Genéricos";
        case VhdlDocGen::PACKAGE_BODY:
          return "Cuerpo del paquete";
        case VhdlDocGen::UNITS:
          return "Unidades";
        case VhdlDocGen::SHAREDVARIABLE:
          if (single) return "Variable compartida";
          else        return "Variables compartidas";
        case VhdlDocGen::VFILE:
          if (single) return "Archivo";
          else        return "Archivos";
        case VhdlDocGen::GROUP:
          if (single) return "Grupo";
          else        return "Grupos";
        case VhdlDocGen::INSTANTIATION:
          if (single) return "Instanciación";
          else        return "Instanciaciones";
        case VhdlDocGen::ALIAS:
          if (single) return "Alias";
          else        return "Aliases";
        case VhdlDocGen::CONFIG:
          if (single) return "Configuración";
          else        return "Configuraciones";
        case VhdlDocGen::MISCELLANEOUS:
          return "Varios";
        case VhdlDocGen::UCF_CONST:
          return "Restricciones";
        default:
          return "Clase";
      }
    }
    virtual QCString trCustomReference(const QCString &name)
    { return "Referencia de "+name; }

    virtual QCString trConstants()
    {
        return "Constantes";
    }
    virtual QCString trConstantDocumentation()
    {
        return "Documentación de constantes";
    }
    virtual QCString trSequences()
    {
        return "Secuencias";
    }
    virtual QCString trSequenceDocumentation()
    {
        return "Documentación de secuencias";
    }
    virtual QCString trDictionaries()
    {
        return "Diccionarios";
    }
    virtual QCString trDictionaryDocumentation()
    {
        return "Documentación de diccionarios";
    }
    virtual QCString trSliceInterfaces()
    {
        return "Interfaces";
    }
    virtual QCString trInterfaceIndex()
    {
        return "Índice de interfaces";
    }
    virtual QCString trInterfaceList()
    {
        return "Lista de interfaces";
    }
    virtual QCString trInterfaceListDescription()
    {
        return "Esta es la lista de las interfaces con breves descripciones:";
    }
    virtual QCString trInterfaceHierarchy()
    {
        return "Jerarquía de interfaces";
    }
    virtual QCString trInterfaceHierarchyDescription()
    {
        return "Este listado de herencia está ordenado de forma general, pero no está en orden alfabético estricto:";
    }
    virtual QCString trInterfaceDocumentation()
    {
        return "Documentación de interfaces";
    }
    virtual QCString trStructs()
    {
        return "Estructuras de datos";
    }
    virtual QCString trStructIndex()
    {
        return "Índice de estructuras de datos";
    }
    virtual QCString trStructList()
    {
        return "Lista de estructuras de datos";
    }
    virtual QCString trStructListDescription()
    {
        return "Esta es la lista de las estructuras de datos con breves descripciones:";
    }
    virtual QCString trStructDocumentation()
    {
        return "Documentación de estructuras de datos";
    }
    virtual QCString trExceptionIndex()
    {
        return "Índice de excepciones";
    }
    virtual QCString trExceptionList()
    {
        return "Lista de excepciones";
    }
    virtual QCString trExceptionListDescription()
    {
        return "Esta es la lista de las excepciones con breves descripciones:";
    }
    virtual QCString trExceptionHierarchy()
    {
        return "Jerarquía de excepciones";
    }
    virtual QCString trExceptionHierarchyDescription()
    {
        return "Este listado de herencia está ordenado de forma general, pero no está en orden alfabético estricto:";
    }
    virtual QCString trExceptionDocumentation()
    {
        return "Documentación de excepciones";
    }
    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal)
    {
      QCString result="Referencia de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" la clase "; break;
        case ClassDef::Struct:     result+=" la estructura "; break;
        case ClassDef::Union:      result+=" la unión "; break;
        case ClassDef::Interface:  result+=" la interface "; break;
        case ClassDef::Protocol:   result+="l protocolo "; break;
        case ClassDef::Category:   result+=" la categoría "; break;
        case ClassDef::Exception:  result+=" la excepción "; break;
        default: break;
      }
      if (isLocal) result+=" local ";
      result+=clName;
      return result;
    }
    virtual QCString trOperations()
    {
        return "Operaciones";
    }
    virtual QCString trOperationDocumentation()
    {
        return "Documentación de operaciones";
    }
    virtual QCString trDataMembers()
    {
        return "Datos miembro";
    }
    virtual QCString trDataMemberDocumentation()
    {
        return "Documentación de datos miembro";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation()
    { return "Documentación de unidades de diseño"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    virtual QCString trConcept(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Concepto" : "concepto"));
      if (!singular) result+="s";
      return result;
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    virtual QCString trConceptReference(const QCString &conceptName)
    {
      QCString result="Referencia del concepto ";
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    virtual QCString trConceptList()
    { return "Lista de conceptos"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    virtual QCString trConceptIndex()
    { return "Índice de conceptos"; }

    /*! used as the title of chapter containing all information about concepts. */
    virtual QCString trConceptDocumentation()
    { return "Documentación de conceptos"; }

    /*! used as an introduction to the concept list */
    virtual QCString trConceptListDescription(bool extractAll)
    {
      QCString result="Esta es la lista de todos los conceptos ";
      if (!extractAll) result+="documentados ";
      result+="con breves descripciones:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    virtual QCString trConceptDefinition()
    {
      return "Definición de concepto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trPackageList()
    { return "Lista de paquetes"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    virtual QCString trFlowchart()
    { return "Diagrama de flujo: "; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     */
};

#endif
