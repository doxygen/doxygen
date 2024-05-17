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
 * Updated to 1.9.7 by David H. Martín (27-marzo-2023)
*/

namespace SpanishTranslatorUtils
{
    /*! Returns true if the gender of the given component is masculine in
    Spanish. */
    inline bool isClassMemberHighlightMasculine(ClassMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            // structs and unions
            return false;
          }
          else
          {
            // class members
            return true;
          }
        case ClassMemberHighlight::Functions:
        case ClassMemberHighlight::Variables:
        case ClassMemberHighlight::Enums:
        case ClassMemberHighlight::Properties:
          return false;
        case ClassMemberHighlight::EnumValues:
        case ClassMemberHighlight::Typedefs:
        case ClassMemberHighlight::Events:
        case ClassMemberHighlight::Related:
        default:
          return true;
      }
    }

    /*! Returns true if the gender of the given component is masculine in
    Spanish. */
    inline bool isFileMemberHighlightMasculine(FileMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            // functions, variables...
            return false;
          }
          else
          {
            // file members
            return true;
          }
        case FileMemberHighlight::Functions:
        case FileMemberHighlight::Variables:
        case FileMemberHighlight::Sequences:
        case FileMemberHighlight::Enums:
        case FileMemberHighlight::Defines:
          return false;
        case FileMemberHighlight::Dictionaries:
        case FileMemberHighlight::EnumValues:
        case FileMemberHighlight::Typedefs:
        default:
          return true;
      }
    }

    /*! Returns true if the gender of the given component is masculine in
    Spanish. */
    inline bool isNamespaceMemberHighlightMasculine(NamespaceMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case NamespaceMemberHighlight::Functions:
        case NamespaceMemberHighlight::Variables:
        case NamespaceMemberHighlight::Sequences:
        case NamespaceMemberHighlight::Enums:
          return false;
        case NamespaceMemberHighlight::All:
        case NamespaceMemberHighlight::Dictionaries:
        case NamespaceMemberHighlight::EnumValues:
        case NamespaceMemberHighlight::Typedefs:
        default:
          return true;
      }
    }
}

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
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[spanish, es-noshorthands, shorthands=off]{babel}";
    }

    QCString trISOLang() override
    {
      return "es";
    }

    QCString getLanguageString() override
    {
      return "0x40A Spanish";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funciones relacionadas"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Observar que estas no son funciones miembro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Descripción detallada"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detalles"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentación de los «Typedef» miembros de la clase"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentación de las enumeraciones miembro de la clase"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
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
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Más..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista de todos los miembros"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Lista de los miembros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Lista completa de los miembros de"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", incluyendo todos los heredados:"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Generado automáticamente por Doxygen";
      if (!s.isEmpty()) result+=" para "+s;
      result+=" del código fuente.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nombre de enumeración"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "valor enumerado"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definido en"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Módulos"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Jerarquía de clases"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Lista de archivos"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Páginas relacionadas"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Ejemplos"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Buscar"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Esta es la lista jerárquica de todas las entidades:";
      }
      else
      {
        return "Este listado de herencia está ordenado de forma general "
               "pero no está en orden alfabético estricto:";
      }
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Lista de todos los archivos ";
      if (!extractAll) result+="documentados y ";
      result+="con breves descripciones:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
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
    QCString trFileMembersDescription(bool extractAll) override
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
    QCString trExamplesDescription() override
    { return "Lista de todos los ejemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Lista de toda la documentación relacionada:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Lista de todos los módulos:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return "Documentación" + (!projName.isEmpty()? " de " + projName : ""); }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Índice de módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Índice jerárquico"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Índice de archivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Documentación de módulos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Documentación de archivos"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Manual de referencia"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "defines"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumeraciones"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funciones"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variables"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Valores de enumeraciones"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Documentación de «define»"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Documentación de «typedef»"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Documentación de enumeraciones"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Documentación de funciones"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Documentación de variables"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generado el "+date;
      if (!projName.isEmpty()) result+=" para "+projName;
      result+=" por";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagrama de herencia de "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Atención"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versión"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Fecha"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Devuelve"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Ver también"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parámetros"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Excepciones"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generado por"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Lista de espacios de nombres"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Lista de ";
      if (!extractAll) result+="todos ";
      result+="los espacios de nombres documentados, con breves descripciones:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Amigas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Documentación de clases amigas y funciones relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                         ClassDef::CompoundType compType,
                                         bool isTemplate) override
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Referencia del archivo ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Referencia del espacio de nombres ";
      result+=namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Métodos públicos"; }

    QCString trPublicSlots() override
    { return "Slots públicos"; }

    QCString trSignals() override
    { return "Señales"; }

    QCString trStaticPublicMembers() override
    { return "Métodos públicos estáticos"; }

    QCString trProtectedMembers() override
    { return "Métodos protegidos"; }

    QCString trProtectedSlots() override
    { return "Slots protegidos"; }

    QCString trStaticProtectedMembers() override
    { return "Métodos protegidos estáticos"; }

    QCString trPrivateMembers() override
    { return "Métodos privados"; }

    QCString trPrivateSlots() override
    { return "Slots privados"; }

    QCString trStaticPrivateMembers() override
    { return "Métodos privados estáticos"; }

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
            result+=" y ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Hereda de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Heredado por "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementado de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementado en "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Miembros del espacio de nombres "; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Índice de espacios de nombres"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Documentación de espacios de nombres"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Espacios de nombres"; }

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
    QCString trReturnValues() override
    { return "Valores devueltos"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Página principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "pág."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definición en la línea @0 del archivo @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definición en el archivo @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
    return "Obsoleto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagrama de colaboración de "+clName+":";
    }

    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Gráfico de dependencias incluidas en "+fName+":";
    }

    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Documentación de constructores y destructores";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Ir al código fuente de este archivo.";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ir a la documentación de este archivo.";
    }

    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondición";
    }

    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondición";
    }

    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariante";
    }

    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Valor inicial:";
    }

    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "código fuente";
    }

    QCString trGraphicalHierarchy() override
    {
      return "Gráfico de jerarquía de clases";
    }

    QCString trGotoGraphicalHierarchy() override
    {
      return "Ir al gráfico de jerarquía de clases";
    }

    QCString trGotoTextualHierarchy() override
    {
      return "Ir al texto de jerarquía de clases";
    }

    QCString trPageIndex() override
    {
      return "Índice de páginas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Nota";
    }

    QCString trPublicTypes() override
    {
      return "Tipos públicos";
    }

    QCString trPublicAttribs() override
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

    QCString trStaticPublicAttribs() override
    {
      return "Atributos públicos estáticos";
    }

    QCString trProtectedTypes() override
    {
      return "Tipos protegidos";
    }

    QCString trProtectedAttribs() override
    {
      return "Atributos protegidos";
    }

    QCString trStaticProtectedAttribs() override
    {
      return "Atributos estáticos protegidos";
    }

    QCString trPrivateTypes() override
    {
      return "Tipos privados";
    }

    QCString trPrivateAttribs() override
    {
      return "Atributos privados";
    }

    QCString trStaticPrivateAttribs() override
    {
      return "Atributos estáticos privados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Tareas pendientes";
    }

    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Lista de tareas pendientes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referenciado por";
    }

    QCString trRemarks() override
    {
      return "Comentarios";
    }

    QCString trAttention() override
    {
      return "Atención";
    }

    QCString trInclByDepGraph() override
    {
      return "Gráfico de los archivos que directa o "
              "indirectamente incluyen a este archivo:";
    }

    QCString trSince() override
    {
      return "Desde";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Leyenda del gráfico";
    }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "leyenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Prueba";
    }

    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Lista de pruebas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Propiedades";
    }

    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Documentación de propiedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Paquete "+name;
    }


    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Estos son los paquetes con breves descripciones (si están disponibles):";
    }

    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paquetes";
    }

    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Valor:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Defecto";
    }

    /*! Used as the header of the bug list */
    QCString trBugList() override
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
      return "Índice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "clase", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "archivo", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result = createNoun(first_capital, singular, "espacio", "s");
      result+=" de nombres";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "módulo", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "página", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "miembro", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "es");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "es");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Hace referencia a";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementado en "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Tabla de contenidos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista de obsoletos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Eventos";
    }

    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Documentación de eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipos de paquete";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funciones de paquete";
    }
    QCString trPackageMembers() override
    {
      return "Miembros de paquete";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Funciones estáticas de paquete";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributos de paquete";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Atributos estáticos de paquete";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Todos";
    }

    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Gráfico de llamadas de esta función:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Coincidencias:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Archivo de código fuente " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Jerarquía de directorios"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Documentación de directorios"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Directorios"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    {
      QCString result="Referencia del directorio ";
      result+=dirName;
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "directorio", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Esta es una función miembro sobrecargada que se "
               "suministra por conveniencia. Difiere de la anterior "
               "función solamente en los argumentos que acepta.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Gráfico de llamadas a esta función:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentación de valores enumerados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentación de funciones/subrutinas miembros"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lista de campos de datos"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Campos de datos"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Esta es la lista de los tipos de datos con breves descripciones:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Índice de tipos de datos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentación de tipos de datos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funciones/subrutinas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentación de funciones/subrutinas"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipos de datos"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Lista de módulos"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Lista de todos los módulos ";
      if (!extractAll) result+="documentados ";
      result+="con breves descripciones:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Referencia del módulo ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Miembros del módulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Índice de módulos"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "módulo", "s");
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tipo", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprograma", "s");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Restricciones de tipo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relación "+name;
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Cargando...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Espacio de nombres global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Buscando...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Archivo en "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
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
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "lun", "mar", "mié", "jue", "vie", "sáb", "dom" };
      static const char *days_full[]    = { "lunes", "martes", "miércoles", "jueves", "viernes", "sábado", "domingo" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return first_capital? text.mid(0,1).upper()+text.mid(1) : text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "ene", "feb", "mar", "abr", "may", "jun", "jul", "ago", "sep", "oct", "nov", "dic" };
      static const char *months_full[]  = { "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return first_capital? text.mid(0,1).upper()+text.mid(1) : text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "a. m.", "p. m." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Referencias bibliográficas"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return "Gráfico de dependencias de directorios de "+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "nivel de detalle"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parámetros de plantilla"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "y "+number+" más..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "La documentación de esta enumeración está generada de";
      if (single)
          result += "l siguiente archivo:";
      else
          result += " los siguientes archivos:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return "Referencia de la enumeración "+name; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return members+" heredados de "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Otros miembros heredados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "habilitar" : "deshabilitar";
      return "pulsar para "+opt+" sincronización";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Proporcionado por la categoría @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Extiende la clase @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Métodos de clase";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Métodos de instancia";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Documentación de métodos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Interfaces exportadas"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Servicios incluidos"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Grupos de constantes"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result="Referencia a grupos de constantes de ";
      result+=namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result="Referencia a servicios de ";
      result+=sName;
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result="Referencia a «singleton» de ";
      result+=sName;
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="La documentación de este servicio "
                      "está generada de";
      if (single) result+="l siguiente archivo:"; else result+=" los siguientes archivos:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
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
    QCString trDesignUnitHierarchy() override
    { return "Jerarquía de unidades de diseño"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Lista de unidades de diseño"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Miembros de unidades de diseño"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Esta es la lista de todos los miembros de unidades de diseño con "
            "enlaces a las entidades a las que pertenecen:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Índice de unidades de diseño"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Unidades de diseño"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funciones/procedimientos/procesos"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Biblioteca";
          else        return "Bibliotecas";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Paquete";
          else        return "Paquetes";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Señal";
          else        return "Señales";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Componente";
          else        return "Componentes";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Constante";
          else        return "Constantes";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entidad";
          else        return "Entidades";
        case VhdlSpecifier::TYPE:
          if (single) return "Tipo";
          else        return "Tipos";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Subtipo";
          else        return "Subtipos";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Función";
          else        return "Funciones";
        case VhdlSpecifier::RECORD:
          if (single) return "Registro";
          else        return "Registros";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedimiento";
          else        return "Procedimientos";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Arquitectura";
          else        return "Arquitecturas";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Atributo";
          else        return "Atributos";
        case VhdlSpecifier::PROCESS:
          if (single) return "Proceso";
          else        return "Procesos";
        case VhdlSpecifier::PORT:
          if (single) return "Puerto";
          else        return "Puertos";
        case VhdlSpecifier::USE:
          if (single) return "Cláusula de uso";
          else        return "Cláusulas de uso";
        case VhdlSpecifier::GENERIC:
          if (single) return "Genérico";
          else        return "Genéricos";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Cuerpo del paquete";
        case VhdlSpecifier::UNITS:
          return "Unidades";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Variable compartida";
          else        return "Variables compartidas";
        case VhdlSpecifier::VFILE:
          if (single) return "Archivo";
          else        return "Archivos";
        case VhdlSpecifier::GROUP:
          if (single) return "Grupo";
          else        return "Grupos";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Instanciación";
          else        return "Instanciaciones";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Aliases";
        case VhdlSpecifier::CONFIG:
          if (single) return "Configuración";
          else        return "Configuraciones";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Varios";
        case VhdlSpecifier::UCF_CONST:
          return "Restricciones";
        default:
          return "Clase";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return "Referencia de "+name; }

    QCString trConstants() override
    {
        return "Constantes";
    }
    QCString trConstantDocumentation() override
    {
        return "Documentación de constantes";
    }
    QCString trSequences() override
    {
        return "Secuencias";
    }
    QCString trSequenceDocumentation() override
    {
        return "Documentación de secuencias";
    }
    QCString trDictionaries() override
    {
        return "Diccionarios";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Documentación de diccionarios";
    }
    QCString trSliceInterfaces() override
    {
        return "Interfaces";
    }
    QCString trInterfaceIndex() override
    {
        return "Índice de interfaces";
    }
    QCString trInterfaceList() override
    {
        return "Lista de interfaces";
    }
    QCString trInterfaceListDescription() override
    {
        return "Esta es la lista de las interfaces con breves descripciones:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Jerarquía de interfaces";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Este listado de herencia está ordenado de forma general, pero no está en orden alfabético estricto:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Documentación de interfaces";
    }
    QCString trStructs() override
    {
        return "Estructuras de datos";
    }
    QCString trStructIndex() override
    {
        return "Índice de estructuras de datos";
    }
    QCString trStructList() override
    {
        return "Lista de estructuras de datos";
    }
    QCString trStructListDescription() override
    {
        return "Esta es la lista de las estructuras de datos con breves descripciones:";
    }
    QCString trStructDocumentation() override
    {
        return "Documentación de estructuras de datos";
    }
    QCString trExceptionIndex() override
    {
        return "Índice de excepciones";
    }
    QCString trExceptionList() override
    {
        return "Lista de excepciones";
    }
    QCString trExceptionListDescription() override
    {
        return "Esta es la lista de las excepciones con breves descripciones:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Jerarquía de excepciones";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Este listado de herencia está ordenado de forma general, pero no está en orden alfabético estricto:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Documentación de excepciones";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
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
    QCString trOperations() override
    {
        return "Operaciones";
    }
    QCString trOperationDocumentation() override
    {
        return "Documentación de operaciones";
    }
    QCString trDataMembers() override
    {
        return "Datos miembro";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Documentación de datos miembro";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Documentación de unidades de diseño"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "concepto", "s");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result="Referencia del concepto ";
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Lista de conceptos"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Índice de conceptos"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Documentación de conceptos"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Esta es la lista de todos los conceptos ";
      if (!extractAll) result+="documentados ";
      result+="con breves descripciones:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Definición de concepto";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Lista de paquetes"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
    { return "Diagrama de flujo:"; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     */

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    QCString trRelatedSymbols() override
    { return "Símbolos relacionados"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(Observar que estos no son símbolos miembro.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Documentación de símbolos amigos y relacionados"; }

    /*! the compound type as used for the xrefitems */
    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) trType(true,true);
          else result=trClass(true,true);
          break;
        case ClassDef::Struct:     result="Estructura"; break;
        case ClassDef::Union:      result="Unión"; break;
        case ClassDef::Interface:  result="Interface"; break;
        case ClassDef::Protocol:   result="Protocolo"; break;
        case ClassDef::Category:   result="Categoría"; break;
        case ClassDef::Exception:  result="Excepción"; break;
        case ClassDef::Service:    result="Servicio"; break;
        case ClassDef::Singleton:  result="Singleton"; break;
        default: break;
      }
      return result;
    }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = SpanishTranslatorUtils::isFileMemberHighlightMasculine(hl);
      QCString result="Lista de ";
      result+=(masculine ? "todos los " : "todas las ");
      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="funciones, variables, «defines», enumeraciones y «typedefs»";
          }
          else
          {
            result+="miembros de los archivos";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="funciones";
          break;
        case FileMemberHighlight::Variables:
          result+="variables";
          break;
        case FileMemberHighlight::Typedefs:
          result+="«typedefs»";
          break;
        case FileMemberHighlight::Sequences:
          result+="secuencias";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="diccionarios";
          break;
        case FileMemberHighlight::Enums:
          result+="enumeraciones";
          break;
        case FileMemberHighlight::EnumValues:
          result+="valores enumerados";
          break;
        case FileMemberHighlight::Defines:
          result+="macros";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      if (!extractAll) result+=(masculine ? "documentados " : "documentadas ");
      result+=" con enlaces ";
      if (extractAll)
        result+="a los archivos a los que corresponden:";
      else
        result+="a la documentación:";
      return result;
    }

    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = SpanishTranslatorUtils::isClassMemberHighlightMasculine(hl);
      QCString result="Lista de ";
      result+=(masculine ? "todos los " : "todas las ");
      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="estructuras y uniones";
          }
          else
          {
            result+="miembros de clases";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="funciones";
          break;
        case ClassMemberHighlight::Variables:
          result+="variables";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="«typedefs»";
          break;
        case ClassMemberHighlight::Enums:
          result+="enumeraciones";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="valores enumerados";
          break;
        case ClassMemberHighlight::Properties:
          result+="propiedades";
          break;
        case ClassMemberHighlight::Events:
          result+="eventos";
          break;
        case ClassMemberHighlight::Related:
          result+="símbolos relacionados";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      if (!extractAll) result+=(masculine ? "documentados " : "documentadas ");
      result+=" con enlaces ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a la documentación de la estructura/unión para cada campo:";
        }
        else
        {
          result+="a la documentación de la clase para cada miembro:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a las estructuras/uniones a las que pertenecen:";
        }
        else
        {
          result+="a las clases a las que pertenecen:";
        }
      }
      return result;
    }

    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = SpanishTranslatorUtils::isNamespaceMemberHighlightMasculine(hl);
      QCString result="Lista de ";
      result+=(masculine ? "todos los " : "todas las ");
      QCString singularResult = "";
      QCString pluralResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="miembro";
          pluralResult="miembros";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult="función";
          pluralResult="funciones";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult="variable";
          pluralResult="variables";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult="«typedef»";
          pluralResult="«typedefs»";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult="secuencia";
          pluralResult="secuencias";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult="diccionario";
          pluralResult="diccionarios";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult="enumeración";
          pluralResult="enumeraciones";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult="valor enumerado";
          pluralResult="valores enumerados";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+=(pluralResult.isEmpty() ? singularResult+"s" : pluralResult);

      result+="del espacio de nombres ";
      if (!extractAll) result+=(masculine ? "documentados " : "documentadas ");
      result+=" con enlaces ";
      if (extractAll)
        result+="a la documentación del espacio de nombres de cada " + singularResult + ":";
      else
        result+="a los espacios de nombres a los que pertenecen:";
      return result;
    }
};

#endif
