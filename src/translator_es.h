/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

/*
 * translator_es.h modified by Lucas Cruz (7-julio-2000)
 * Some notes:
 * - It's posible that some sentences haven't got meaning because  
 * some words haven't got translate in spanish.
 */

#ifndef TRANSLATOR_ES_H
#define TRANSLATOR_ES_H

class TranslatorSpanish : public TranslatorAdapter_1_3_8
{
  public:
    virtual QCString idLanguage()
    { return "spanish"; }
    /*! returns the name of the package that is included by LaTeX */
    virtual QCString latexBabelPackage() 
    { return ""; }

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

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-1";
    }

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funciones relacionadas"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Observar que estas no son funciones miembro.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descripción detallada"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Documentación de los 'Tipos Definidos' miembros de la clase"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Documentación de las enumeraciones miembro de la clase"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Documentación de las funciones miembro"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { return "Documentación de los datos miembro"; }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Más..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista de todos los miembros."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Lista de los miembros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Esta es la lista completa de miembros para "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", incluyendo todos los miembros heredados."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Generado automáticamente por Doxygen";
      if (s) result+=(QCString)" para "+s;
      result+=" del código fuente."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nombre de la enumeración"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "valor enumerado"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definido en"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    virtual QCString trVerbatimText(const char *f)
    { return (QCString)"Este es el texto al pie de la letra del archivo "+f+" adjunto."; }
    
    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Módulos"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Jerarquía de la clase"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { return "Lista de componentes"; }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista de archivos"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "Archivos cabecera"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { return "Miembros de las clases"; }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { return "Archivos de los miembros"; }

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
    { return "Esta lista de herencias esta ordenada rigurosamente, "
             "pero no completa, por orden alfabético:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Lista de todos los archivos ";
      if (!extractAll) result+="documentados y ";
      result+="con descripciones breves:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { return "Lista de las clases, estructuras, "
             "uniones e interfaces con una breve descripción:"; 
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Lista de";
      if (!extractAll) result+=" toda la documentación de";
      result+=" los miembros de la clase con enlaces a ";
      if (!extractAll) 
        result+=" la documentación de cada clase:";
      else 
        result+=" la clase de la que forman parte:";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Lista de ";
      if (!extractAll) result+="toda la documentación de ";
      result+="los miembros con enlaces a ";
      if (extractAll) 
        result+="los documentos de cada archivo a los que pertenecen:";
      else 
        result+="los archivos de los que forman parte:";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "Lista de los archivos cabecera que crean el API:"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Lista de todos los ejemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Lista de toda la documentación relacionada:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Lista de todos los módulos:"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return "Descripción no disponible"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Documentación"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Indice de módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Indice jerárquico"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    { return "Indice de clases"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Indice de archivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Documentación de módulos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { return "Documentación de clases"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Documentación de archivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Documentación de ejemplos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Documentación de páginas"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Manual de referencia"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definiciones"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Funciones prototipo"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Tipos definidos"; }

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
    { return "Valores de la enumeración"; }
    
    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Documentación de las definiciones"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Documentación de las funciones prototipo"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Documentación de los tipos definidos"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Documentación de las enumeraciones"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentación de los valores de la enumeración"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Documentación de las funciones"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Documentación de las variables"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { return "Clases"; }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    virtual QCString trFiles()
    { return "Archivos"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generado el "+date;
      if (projName) result+=(QCString)" para "+projName;
      result+=(QCString)" por";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "escrito por";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagrama de herencias de "+clName;
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Para uso interno exclusivamente."; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "Reimplementado por razones internas; el API no se ve afectado."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Atención"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "Bugs y limitaciones"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versión"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Fecha"; }

    /*! this text is generated when the \\author command is used. */
    virtual QCString trAuthors()
    { return "Autor(es)"; }

    /*! this text is generated when the \return command is used. */
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

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Lista de namespace"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Lista de ";
      if (!extractAll) result+="toda la documentación de ";
      result+="los namespaces con una breve descripción:";
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
    { return "Documentación de las funciones relacionadas y clases amigas"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/)
    {
      
      QCString result="Referencia de ";
      switch(compType)
      {
        case ClassDef::Class:      result+="la Clase "; break;
        case ClassDef::Struct:     result+="la Estructura "; break;
        case ClassDef::Union:      result+="la Unión "; break;
        case ClassDef::Interface:  result+="la Interfaz "; break;
        case ClassDef::Protocol:   result+="Protocol "; break; // translate me!
        case ClassDef::Category:   result+="Category "; break; // translate me!
        case ClassDef::Exception:  result+="la Excepción "; break;
      }
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result="Referencia del Archivo ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="Referencia del Namespace ";
      result+=namespaceName;
      return result;
    }
    
    /* these are for the member sections of a class, struct or union */
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
            result+=", y ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Herencias "+trWriteList(numEntries)+".";
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
    { return "Miembros del Namespace "; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Lista de ";
      if (!extractAll) result+="toda la documentación de ";
      result+="los miembros del namespace con enlace a ";
      if (extractAll) 
        result+="los namespace de cada miembro:";
      else 
        result+="la documentación de los namespaces pertenecientes a:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Indice de namespace"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Documentación de namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namespaces"; }

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
      QCString result=(QCString)"La documentación para esta ";
      switch(compType)
      {
        case ClassDef::Class:      result+="clase"; break;
        case ClassDef::Struct:     result+="estructura"; break;
        case ClassDef::Union:      result+="unión"; break;
        case ClassDef::Interface:  result+="interfaz"; break;
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
        case ClassDef::Exception:  result+="excepción"; break;
      }
      result+=" fué generada a partir ";
      if (single) result+="del siguiente archivo:"; 
	else result+="de los siguientes archivos:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Lista alfabética"; }

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
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "Fuentes";
    }
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
	return "Desaprobado"; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////


    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagrama de colaboración para "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Dependencia gráfica adjunta para "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentación del constructor y destructor"; 
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
      return "código";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Representación gráfica de la clase";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Ir a la representación gráfica de la jerarquía de la clase";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Ir a la jerarquía textual de la clase";
    }
    virtual QCString trPageIndex()
    {
      return "Página indice";
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
      return "Atributos públicos";
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
      return "Atributos protegidos estáticos";
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
      return "Atributos privados estáticos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Tareas Pendientes";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Listado de Tareas Pendientes";
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
      return "Este gráfico muestra que archivos directa o "
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
      return "Leyenda del Gráfico";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs()
    {
      return 
        "Esta página explica como interpretar los gráficos que son generados "
	"por doxygen.<p>\n"
	"Considere el siguiente ejemplo:\n"
	"/*! Clase invisible por truncamiento */\n"  
        "class Invisible { };\n\n"
        "/*! Clase truncada, relación de herencia escondida */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Clase no documentada con comentarios de doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Clase que es heredera usando herencia publica */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Clase que es heredera usando herencia protegida  */\n"
        "class ProtectedBase { };\n\n"
        "/*! Clase que es heredera usando herencia privada  */\n"
        "class PrivateBase { };\n\n"
        "/*! Clase que es usada por la clase hija */\n"
        "class Used { };\n\n"
        "/*! Super-Clase que hereda de varias otras clases */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Si la etiqueta \\c MAX_DOT_GRAPH_HEIGHT en el archivo de configuración "
        "tiene valor 200 resultará en el siguiente gráfico:"
	"<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Las cajas en el gráfico arriba tienen el significado que sigue:\n"
        "<ul>\n"
        "<li>Una caja llena negra representa la estructura o clase para la cuál"
	"se generó el gráfico.\n"
        "<li>Una caja con borde negro señala una estructura o clase documentada.\n"
        "<li>Una caja con borde griz señala una estructura o clase no documentada.\n"
        "<li>una caja con borde rojo señala una estructura o clase documentada \n"
	" de la cuál no toda las relaciones de jerarquía/contenido son "
	"mostradas. El gráfico sera truncado si este no calza dentro de los "
	"límites especificados."
        "afgekapt als hij niet past binnen de gespecificeerde grenzen."
        "</ul>\n"
        "Las flechas tienen el siguiente significado:\n"
        "<ul>\n"
        "<li>Una flecha azul oscuro es usada para visualizar una relación "
        "herencia publica entre dos clases.\n"
        "<li>Una flecha verde oscura es usada para herencia protegida.\n"
        "<li>Una flecha rojo oscura es usada para herencia privada.\n"
        "<li>Una flecha segmentada púrpura se usa si la clase es contenida o "
	"usada por otra clase. La flecha está etiquetada por la variable "
	"con que se accede a la clase o estructura apuntada. \n"  
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend()
    {
      return "leyenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    QCString trTest()
    {
      return "Prueba";
    }
    /*! Used as the header of the test list */
    QCString trTestList()
    {
      return "Lista de Pruebas";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Métodos DCOP";
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
      return "Documentación de Propiedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Interfaces";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estructuras de Datos";
      }
      else
      {
        return "Clases";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paquetes "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Lista de Paquetes ";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Aquí van los paquetes con una breve descripción (si etá disponible):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paquetes";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Documentación del Paquete";
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
      return "Bug";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Lista de Bugs";
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
      if (first_capital) toupper(result.at(0));
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
      QCString result((first_capital ? "Grupo" : "grupo"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Página" : "pagina"));
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
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Campo" : "campo"));
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

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Hace referencia a";
    }

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

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Tabla de Contenidos";
    }

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista de Desaprobados";
    }

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
      return "Documentación de los Eventos";
    }

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Tipos del Paquete";
    }
    
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Funciones del Paquete";
    }

    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Funciones Estáticas del Paquete";
    }

    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Atributos del Paquete";
    }

    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Atributos Estáticos del Paquete";
    }
     
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Todo";
    }

    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Gráfico de llamadas para esta función:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////


    /*! When the search engine is enabled this text is put in the index 
     *  of each page before the search field. 
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
      return "Resultados de la Búsqueda";
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

};


#endif


