/******************************************************************************
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative workns derived from the
 * input used in their production; they are not affected by this license.
 *
 * Brazilian Portuguese translation version 20100531
 *    Maintainer: Fabio "FJTC" Jun Takada Chino <jun-chino at uol.com.br>
 *    Thanks to Jorge Ramos, Fernando Carijo and others for their contributions.
 *
 * History:
 *   20100531:
 *	- Updated to 1.6.3;
 *   20091218:
 *      - Updated to 1.6.1;
 *      - Copyright year updated;
 *      - Translation updates suggested by Fernando Carijó added;
 *   20080709:
 *	- References to MAX_DOT_GRAPH_HEIGHT removed from trLegendDocs().
 *   20080206:
 *      - Method trTypeContraints() renamed to trTypeConstraints().
 *   20071216:
 * 	- New methods since 1.5.4 updated. 
 *   Previous history removed from this version.
 */
#ifndef TRANSLATOR_BR_H
#define TRANSLATOR_BR_H

class TranslatorBrazilian : public Translator
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. May resemble
     * the string returned by latexBabelPackage(), but it is not used
     * for the same purpose. The identification should not be translated.
     * It should be replaced by the name of the language in English
     * (e.g. Czech, Japanese, Russian, etc.). It should be equal to
     * the identification in language.h.
     */
    virtual QCString idLanguage()
    {
      return "brazil";
    }

    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[brazil]{babel}";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Funções Relacionadas"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Note que estas não são funções membros.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Descrição Detalhada"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Definições de Tipos"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Enumerações"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Métodos"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Campos";
      }
      else
      {
        return "Atributos";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Mais..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Lista de todos os Membros."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Lista dos Membros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Esta é a lista de todos os membros de "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", incluindo os membros herdados."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Gerado automaticamente por Doxygen";
      if (s) result+=(QCString)" para "+s;
      result+=" a partir de seu código-fonte.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "[Enumeração]"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "[Valor enumerado]"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "definido(a) em"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Módulos"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Hierarquia de Classes"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estruturas de Dados";
      }
      else
      {
        return "Lista de Componentes";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Lista de Arquivos"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Campos de Dados";
      }
      else
      {
        return "Componentes Membros";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globais";
      }
      else
      {
        return "Arquivos Membros";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Páginas relacionadas"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Exemplos"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Busca"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Esta lista de hierarquias está parcialmente ordenada (ordem alfabética):"; }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Esta é a lista de todos os arquivos ";
      if (!extractAll) result+="documentados ";
      result+="e suas respectivas descrições:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Aqui estão as estruturas de dados, uniões e suas respectivas descrições:";
      }
      else
      {
        return "Aqui estão as classes, estruturas, uniões e interfaces e suas respectivas descrições:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Esta é a lista de ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="todas as estruturas e campos de uniões ";
        if (!extractAll)
        {
          result+="documentas";
        }
      }
      else
      {
        result+="todos os membros de classes ";
        if (!extractAll)
        {
          result+="documentos";
        }
      }
      result+=" com referências para ";
      if (!extractAll)
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="a documentação da estrutura/união de cada campo:";
        }
        else
        {
          result+="a documentação da classe de cada membro:";
        }
      }
      else
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="a estrutura/união a que pertencem:";
        }
        else
        {
          result+="a classe a que pertencem:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Esta é a lista de ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="todas as funções, variáveis, definições, enumerações e definições de tipos ";
        if (!extractAll) result+="documentadas ";
      }
      else
      {
        result+="todos os membros dos arquivos ";
        if (!extractAll) result+="documentados ";
      }
      result+="com referências para ";
      if (extractAll)
        result+="seus respectivos arquivos:";
      else
        result+="suas respectivas documentações:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Esta é a lista com todos os exemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Esta é a lista com toda a documentação relacionadas:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Esta é a lista com todos os módulos:"; }

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "\nDocumentação"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Índice dos Módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Índice Hierárquico"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Índice das Estruturas de Dados";
      }
      else
      {
        return "Índice dos Componentes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "Índice dos Arquivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Módulos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estruturas";
      }
      else
      {
        return "Classes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Arquivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Exemplos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Documentação Relacionada"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Guia de Referência"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Definições e Macros"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Protótipos de Funções"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definições de Tipos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerações"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Funções"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variáveis"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Valores enumerados"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Definições e macros"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Protótipos das funções"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Definições dos tipos"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Enumerações"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Funções"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Variáveis"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estruturas de Dados";
      }
      else
      {
        return "Componentes";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Gerado em "+date;
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
      return (QCString)"Diagrama de Hierarquia para "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Apenas para uso interno."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Aviso"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versão"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Retorna"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Veja também"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parâmetros"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Exceções"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Gerado por"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Lista de Namespaces"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Esta é a lista de todos os Namespaces ";
      if (!extractAll) result+="documentados ";
      result+="com suas respectivas descrições:";
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
    { return "Amigas e Funções Relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referência da";
      if (isTemplate) result+=" Template de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Classe "; break;
        case ClassDef::Struct:     result+=" Estrutura "; break;
        case ClassDef::Union:      result+=" União "; break;
        case ClassDef::Interface:  result+=" Interface "; break;
        case ClassDef::Protocol:   result+=" Protocolo "; break;
        case ClassDef::Category:   result+=" Categoria "; break;
        case ClassDef::Exception:  result+=" Exceção "; break;
      }
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result = "Referência do Arquivo ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result ="Refência do Namespace ";
      result += namespaceName;
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Métodos Públicos"; }
    virtual QCString trPublicSlots()
    { return "Slots Públicos"; }
    virtual QCString trSignals()
    { return "Sinais"; }
    virtual QCString trStaticPublicMembers()
    { return "Métodos Públicos Estáticos"; }
    virtual QCString trProtectedMembers()
    { return "Métodos Protegidos"; }
    virtual QCString trProtectedSlots()
    { return "Slots Protegidos"; }
    virtual QCString trStaticProtectedMembers()
    { return "Métodos Protegidos Estáticos"; }
    virtual QCString trPrivateMembers()
    { return "Métodos Privados"; }
    virtual QCString trPrivateSlots()
    { return "Slots Privados"; }
    virtual QCString trStaticPrivateMembers()
    { return "Métodos Privados Estáticos"; }

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
            result+=" e ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Herdeiro de " + trWriteList(numEntries) + ".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Superclasse de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplementação de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplementado por "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Membros do Namespace"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Esta é a lista com todos os membros do Namespace ";
      if (!extractAll) result+="documentados ";
      result+="com referências para ";
      if (extractAll)
        result+="a documentação de seus respectivos Namespaces:";
      else
        result+="os seus respectivos namespaces:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Namespaces"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Namespaces"; }

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
      QCString result=(QCString)"A documentação para esta ";
      switch(compType)
      {
        case ClassDef::Class:      result+="classe "; break;
        case ClassDef::Struct:     result+="estrutura "; break;
        case ClassDef::Union:      result+="união "; break;
        case ClassDef::Interface:  result+="interface "; break;
        case ClassDef::Protocol:   result+="protocolo "; break;
        case ClassDef::Category:   result+="categoria "; break;
        case ClassDef::Exception:  result+="exceção "; break;
      }
      result+=" foi gerada a partir ";
      if (single) result+="do seguinte arquivo:"; else result+="dos seguintes arquivos:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Lista Alfabética"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Valores Retornados"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Página Principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "pag."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definição na linha @0 do arquivo @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definição no arquivo @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      /*
      * This note is for brazilians only.
      * Esta é uma boa tradução para "deprecated"?
      */
      return "Descontinuado(a)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagrama de colaboração para "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Gráfico de dependência de inclusões para "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Construtores & Destrutores";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Vá para o código-fonte deste arquivo.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Vá para a documentação deste arquivo.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Pré-Condição";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Pós-Condição";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariante";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Valor Inicial:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "código";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Gráfico de Hierarquia de Classes";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Vá para o Gráfico de Hierarquia de Classes";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Vá para a Hierarquia de Classes (texto)";
    }
    virtual QCString trPageIndex()
    {
      return "Índice da Página";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Observação";
    }
    virtual QCString trPublicTypes()
    {
      return "Tipos Públicos";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Campos de Dados";
      }
      else
      {
        return "Atributos Públicos";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Atributos Estáticos Públicos";
    }
    virtual QCString trProtectedTypes()
    {
      return "Tipos Protegidos";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Atributos Protegidos";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Atributos Estáticos Protegidos";
    }
    virtual QCString trPrivateTypes()
    {
      return "Tipos Privados";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Atributos Privados";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Atributos Privados Estáticos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Futuras Atividades";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Lista de Futuras Atividades";
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
      return "Anotações";
    }
    virtual QCString trAttention()
    {
      return "Atenção";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Este grafo mostra quais arquivos estão direta ou indiretamente relacionados com este arquivo:";
    }
    virtual QCString trSince()
    {
      return "Desde";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda do Grafo";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
    {
      return
        "Esta página explica como interpretar os grafos gerados pelo doxygen.<p>\n"
        "Considere o seguinte exemplo:\n"
        "\\code\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class that is inherited using protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class that is inherited using private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Class that is used by the Inherited class */\n"
        "class Used { };\n\n"
        "/*! Super class that inherits a number of other classes */\n"
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
        "Isto irá gerar o seguinte gráfo:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "As caixas no grafo acima tem as seguintes interpretações:\n"
        "<ul>\n"
        "<li>Uma caixa inteiramente preta representa a estrutura ou a classe para qual "
        "o grafo foi gerado.\n"
        "<li>Uma caixa com bordas pretas denota uma estrutura ou classe documentada.\n"
        "<li>Uma caixa com bordas cinzas denota uma estrutura ou classe não documentada.\n"

        "<li>Uma caixa com bordas vermelhas denota uma estrutura ou classe documentada para\n"
        "a qual nem todas as heranças ou componentes são mostradas no grafo. Um grafo é "
        "truncado quando este é maior que o tamanho especificado."
        "</ul>\n"
        "As setas tem os seguintes significados:\n"
        "<ul>\n"
        "<li>Uma seta azul escura é utilizada para denotar uma relação de herança "
        "pública entre duas classes.\n"
        "<li>Uma seta verde escura é utilizada para denotar uma herança protegida.\n"
        "<li>Uma seta vermelho escura é utilizada para denotar uma herança privada.\n"
        "<li>Uma seta púrpura pontilhada é usada se uma classe está contida ou é utilizada"
        "por outra classe. A seta é marcada com a(s) variável(eis) "
        "através das quais a classe ou estrutura apontada está acessível. \n"
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
      return "Teste";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Lista de Testes";
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
      return "Propriedades";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Propriedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Estruturas de Dados";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Pacote "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Lista de Pacotes";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Esta é a lista com os pacotes e suas respectivas descrições (se disponíveis):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Pacotes";
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
      return "Problema";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Lista de Problemas";
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
      QCString result((first_capital ? "Arquivo": "arquivo"));
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
      QCString result((first_capital ? "Membro" : "membro"));
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
      if (!singular){
        result+="is";
      }else{
        result+="l";
      }
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
      return "Referências";
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
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementado por "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Conteúdo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista de Descontinuados(as)";
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
      return "Eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Tipos do Pacote";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Funções do Pacote";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Funções Estáticas do Pacote";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Atributos do Pacote";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Atributos Estáticos do Pacote";
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
      return "Este é o diagrama das funções utilizadas por esta função:";
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
      return "Procurar por";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Resultado da Busca";
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
        return "Nenhum documento foi encontrado.";
      }
      else if (numDocuments==1)
      {
        return "Apenas <b>1</b> documento foi encontrado.";
      }
      else
      {
        return "<b>$num</b> documentos encontrados. "
               "Os melhores resultados vem primeiro.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Resultados:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return  "Código-Fonte de " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Hierarquia de Diretórios"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Documentação do Directório"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Diretórios"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Esta Hierarquia de Diretórios está parcialmente ordenada (ordem alfabética)"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    {
        QCString result = "Referência do diretório ";
     	result += dirName;
     	return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Diretório" : "diretório"));
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
       return "Este é um método provido por conveniência. "
       		"Ele difere do método acima apenas na lista de "
       		"argumentos que devem ser utilizados.";
    }    

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Este é o diagrama das funções que utilizam esta função:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Documentação da enumeração"; }

  
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Documentação de Função Membro/Subrotina"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Lista de Tipos de Dados"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Campos de Dados"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Aqui estão os tipos de dados com descrições resumidas:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Esta é a lista de todos os membros ";
      if (!extractAll) result+="documentados ";
      result+="dos tipos de dados com links para ";
      if (extractAll) 
      {
        result+="a documentação dos tipos de dados para cada membro:";
      }
      else 
      {
        result+="os tipos de dados a que pertencem:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Índice de Tipos de Dados"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Documentação dos Tipos de Dados"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Funções/Subrotinas"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Documentação da Função/Subrotina"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Tipos de Dados"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Lista de Módulos"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Esta é a lista de todos os módulos ";
      if (!extractAll) result+="documentados ";
      result+="com descrições resumidas:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result = (QCString)"Referência ";

      if (isTemplate) result+="da Template ";
      
      switch(compType)
      {
        case ClassDef::Class:      result+="do Modulo "; break;
        case ClassDef::Struct:     result+="do Tipo "; break;
        case ClassDef::Union:      result+="da União "; break;
        case ClassDef::Interface:  result+="da Interface "; break;
        case ClassDef::Protocol:   result+="do Protocolo "; break;
        case ClassDef::Category:   result+="da Categoria "; break;
        case ClassDef::Exception:  result+="da Exceção "; break;
      }
      result += clName; 
     
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result = "Referência do Módulo ";
      result += namespaceName;        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Membros do Módulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Esta é a lista de todos os membros ";
      if (!extractAll) result+="documentados ";
      result+="dos módulos com links para ";
      if (extractAll) 
      {
        result+="a documentação dos módulos para cada membro:";
      }
      else 
      {
        result+="os módulos a que pertencem:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Índice dos Módulos"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Modulo" : "modulo"));
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
      QCString result=(QCString)"A documentação para ";
      switch(compType)
      {
        case ClassDef::Class:      result+="este modulo "; break;
        case ClassDef::Struct:     result+="este tipo "; break;
        case ClassDef::Union:      result+="esta união "; break;
        case ClassDef::Interface:  result+="esta interface "; break;
        case ClassDef::Protocol:   result+="esto protocolo "; break;
        case ClassDef::Category:   result+="esta categoria "; break;
        case ClassDef::Exception:  result+="esta exceção "; break;
      }
      
      result+=" foi gerada a partir do";
      if (single) 
    	result+=" seguinte arquivo:";
      else
    	result+="s seguintes arquivos:";
      
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

    /*! C# Type Contraint list */
    virtual QCString trTypeConstraints()
    {
      return "Restrições do Tipo";
    }    

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name
     */
    virtual QCString trDirRelation(const char *name)
    {
      return "Relação " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Carregando...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Namespace global";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Procurando...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Nenhuma entrada encontrada";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Grafo de dependência de diretórios para "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Arquivo em "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Inclui arquivo em "+name;
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
      static const char *days[]   = { "Segunda","Terça","Quarta","Quinta","Sexta","Sábado","Domingo" };
      static const char *months[] = { "Janeiro","Fevereiro","Março","Abril","Maio","Junho","Julho","Agosto","Setembro","Outubro","Novembro","Dezembro" };
      QCString sdate;
      sdate.sprintf("%s, %d de %s de %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
};
#endif
