/******************************************************************************
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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
 * Brazilian Portuguese translation
 *    Maintainer:
 *        Fabio "FJTC" Jun Takada Chino <fjtc at brokenbits dot com dot br>
 *    Collaborators:
 *        Emerson Ferreira <nuskorpios at gmail dot com>
 *        Ulisses Guedes <uli1958 at hotmail dot com>
 *    Thanks to Jorge Ramos, Fernando Carijo and others for their contributions.
 *
 * History:
 * 20240204:
 *  - Updated to 1.11.0:
 * 20231107:
 *  - Updated to 1.10.0:
 * 20230817:
 *  - Updated to 1.9.8;
 *  - Small fixes to the method TranslatorBrazilian::trNamespaceMembersDescriptionTotal().
 * 20230430:
 *  - Updated to 1.9.7;
 *  - Adding the namespace PortugueseTranslatorUtils to hold common
 *    functions shared by both Portuguese translators;
 *  - Some translations used by TranslatorPortuguese have been merged
 *    into this translator to make both classes more similar.
 * 20220911:
 *  - Updated to 1.9.6;
 * 20220525:
 * 	- Updated to 1.9.4;
 * 20211003:
 *  - Updated to 1.9.3;
 * 20200112:
 * 	- Updated to 1.9.1;
 * 20190203:
 *	- Slice methods added;
 * 20180612:
 *	- Updated to 1.8.15;
 * 20170123:
 *  - Full translation revision;
 *  - trLegendDocs() is now synchronized with the English version;
 *  - Merging most of the suggestions sent by Emerson Ferreira;
 * 20131129:
 *  - Updated to 1.8.5;
 *  - Translation in the method trFileMembers() fixed;
 *  - Translation in the method trEnumerationValues() fixed;
 * 20110628:
 *  - Updated to 1.7.5;
 * 20100531:
 *  - Updated to 1.6.3;
 * 20091218:
 *  - Updated to 1.6.1;
 *  - Copyright year updated;
 *  - Translation updates suggested by Fernando Carijó added;
 * 20080709:
 *	- References to MAX_DOT_GRAPH_HEIGHT removed from trLegendDocs().
 * 20080206:
 *  - Method trTypeContraints() renamed to trTypeConstraints().
 * 20071216:
 * 	- New methods since 1.5.4 updated.
 * Previous history removed from this version.
 */
#ifndef TRANSLATOR_BR_H
#define TRANSLATOR_BR_H

namespace PortugueseTranslatorUtils
{
    /*! Returns true if the gender of the given component is masculine in
    Portuguese. */
    inline bool isClassMemberHighlightMasculine(ClassMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            // as estruturas e uniões
            return false;
          }
          else
          {
            // os membros da classe
            return true;
          }
          break;
        case ClassMemberHighlight::Functions:
        case ClassMemberHighlight::Variables:
        case ClassMemberHighlight::Enums:
        case ClassMemberHighlight::Properties:
        case ClassMemberHighlight::Typedefs:
          return false;
        case ClassMemberHighlight::EnumValues:
        case ClassMemberHighlight::Events:
        case ClassMemberHighlight::Related:
        default:
          return true;
      }
    }

    /*! Returns true if the gender of the given component is masculine in
    Portuguese. */
    inline bool isFileMemberHighlightMasculine(FileMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case FileMemberHighlight::All:
        case FileMemberHighlight::Dictionaries:
        case FileMemberHighlight::EnumValues:
          return true;
        case FileMemberHighlight::Functions:
        case FileMemberHighlight::Variables:
        case FileMemberHighlight::Typedefs:
        case FileMemberHighlight::Sequences:
        case FileMemberHighlight::Enums:
        case FileMemberHighlight::Defines:
          return false;
        default:
          return true;
      }
    }

    /*! Returns true if the gender of the given component is masculine in
    Brazilian Portuguese and European Portuguese. */
    inline bool isNamespaceMemberHighlightMasculine(NamespaceMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
        case NamespaceMemberHighlight::Dictionaries:
        case NamespaceMemberHighlight::EnumValues:
          return true;
        case NamespaceMemberHighlight::Functions:
        case NamespaceMemberHighlight::Variables:
        case NamespaceMemberHighlight::Typedefs:
        case NamespaceMemberHighlight::Sequences:
        case NamespaceMemberHighlight::Enums:
          return false;
        default:
          return true;
      }
    }

    /*! Returns true if the gender of the given component is masculine in
    Brazilian Portuguese and European Portuguese. */
    inline bool isModuleMemberHighlightMasculine(ModuleMemberHighlight::Enum hl)
    {
      switch (hl)
      {
        case ModuleMemberHighlight::All:
	case ModuleMemberHighlight::EnumValues:
          return true;
	case ModuleMemberHighlight::Functions:
	case ModuleMemberHighlight::Variables:
	case ModuleMemberHighlight::Typedefs:
	case ModuleMemberHighlight::Enums:
          return false;
	default:
          return true;
      }
    }
}

class TranslatorBrazilian : public TranslatorAdapter_1_16_0
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
    {
      return "brazilian";
    }

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
      return "\\usepackage[brazil]{babel}";
    }

    QCString trISOLang() override
    {
      return "pt-BR";
    }
    QCString getLanguageString() override
    {
      return "0x416 Portuguese(Brazil)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funções relacionadas"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Note que estas não são funções membros.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Descrição detalhada"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detalhes"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentação das definições de tipos"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentação das enumerações"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    {
        if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
        {
          return "Documentação das funções, procedimentos e processos";
        }
        else
        {
            return "Documentação das funções";
        }
    }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos";
      }
      else
      {
        return "Atributos";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Mais..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Lista de todos os Membros"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Lista dos Membros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Esta é a lista de todos os membros de"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", incluindo os membros herdados."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Gerado automaticamente por Doxygen";
      if (!s.isEmpty()) result+=" para "+s;
      result+=" a partir do código fonte.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "Enumeração"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "Valor enumerado"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definido(a) em"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Módulos"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hierarquia de Classes"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estruturas de Dados";
      }
      else
      {
        return "Lista de Classes";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Lista de Arquivos"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos de Dados";
      }
      else
      {
        return "Membros de classe";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globais";
      }
      else
      {
        return "Membros dos Arquivos";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Páginas relacionadas"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Exemplos"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Busca"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Esta lista de hierarquias está parcialmente ordenada (ordem alfabética):"; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Esta é a lista de todos os arquivos ";
      if (!extractAll) result+="documentados ";
      result+="e suas respectivas descrições:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Aqui estão as estruturas de dados, uniões e suas respectivas descrições:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Aqui estão as classes e suas respectivas descrições:";
      }
      else
      {
        return "Aqui estão as classes, estruturas, uniões e interfaces e suas respectivas descrições:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Esta é a lista de ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="todas as estruturas e campos de uniões ";
        if (!extractAll)
        {
          result+="documentados";
        }
      }
      else
      {
        result+="todos os membros de classes ";
        if (!extractAll)
        {
          result+="documentados";
        }
      }
      result+=" com referências para ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Esta é a lista de ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trExamplesDescription() override
    { return "Lista de todos os exemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Lista de toda a documentação relacionadas:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Lista de todos os módulos:"; }

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return "Documentação" + (!projName.isEmpty()? " de " + projName : ""); }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Índice dos módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Índice Hierárquico"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Índice dos Arquivos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Módulos"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estruturas";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Classes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Arquivos"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Guia de Referência"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definições e Macros"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definições de Tipos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerações"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funções"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variáveis"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Enumeradores"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Definições e macros"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Definições dos tipos"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Enumerações"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Funções"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Variáveis"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Gerado em "+date;
      if (!projName.isEmpty()) result+=" para "+projName;
      result+=" por";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagrama de hierarquia da classe "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Aviso"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versão"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Retorna"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Veja também"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parâmetros"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Exceções"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Gerado por"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Lista de Namespaces"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Esta é a lista de todos os Namespaces ";
      if (!extractAll) result+="documentados ";
      result+="com suas respectivas descrições:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Amigos"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Amigos e Funções Relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Referência";
      if (isTemplate) result+=" do Template";
      if (compType == ClassDef::Protocol)
      {
        result+=" do ";
      }
      else
      {
        result+=" da ";
      }
      switch(compType)
      {
        case ClassDef::Class:      result+="Classe "; break;
        case ClassDef::Struct:     result+="Estrutura "; break;
        case ClassDef::Union:      result+="União "; break;
        case ClassDef::Interface:  result+="Interface "; break;
        case ClassDef::Protocol:   result+="Protocolo "; break;
        case ClassDef::Category:   result+="Categoria "; break;
        case ClassDef::Exception:  result+="Exceção "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result = "Referência do Arquivo ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result ="Refência do Namespace ";
      result += namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_JAVA))
      {
        return "Métodos Públicos";
      }
      else
      {
        return "Membros Públicos";
      }
    }

    QCString trPublicSlots() override
    { return "Slots Públicos"; }

    QCString trSignals() override
    { return "Sinais"; }

    QCString trStaticPublicMembers() override
    { return "Membros públicos estáticos"; }

    QCString trProtectedMembers() override
    { return "Membros protegidos"; }

    QCString trProtectedSlots() override
    { return "Slots protegidos"; }

    QCString trStaticProtectedMembers() override
    { return "Membros protegidos estáticos"; }

    QCString trPrivateMembers() override
    { return "Membros privados"; }

    QCString trPrivateSlots() override
    { return "Slots privados"; }

    QCString trStaticPrivateMembers() override
    { return "Membros privados estáticos"; }

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
            result+=" e ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Subclasse de " + trWriteList(numEntries) + ".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Superclasse de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementado por "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Membros do namespace"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Essa é a lista com todos os membros ";
      if (!extractAll) result+="documentados ";
      result+="do Namespace com referências para ";
      if (extractAll)
        result+="a documentação do Namespace para cada membro:";
      else
        result+="os seus respectivos Namespaces:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Namespaces"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="A documentação para ";
      if (compType == ClassDef::Protocol)
      {
         result+= "esse ";
      }
      else
      {
         result+= "essa ";
      }
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"Unidade de Design ":"classe "; break;
        case ClassDef::Struct:     result+="estrutura "; break;
        case ClassDef::Union:      result+="união "; break;
        case ClassDef::Interface:  result+="interface "; break;
        case ClassDef::Protocol:   result+="protocolo "; break;
        case ClassDef::Category:   result+="categoria "; break;
        case ClassDef::Exception:  result+="exceção "; break;
        default: break;
      }
      result+=" foi gerada a partir ";
      if (single) result+="do seguinte arquivo:"; else result+="dos seguintes arquivos:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Valores Retornados"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Página Principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    {
      // Decidi mudar de página para "p." para ficar mais compacto.
      return "p.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definição na linha @0 do arquivo @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definição no arquivo @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Obsoleto(a)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagrama de colaboração para "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Gráfico de dependência de inclusões para "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Construtores e Destrutores";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Ir para o código-fonte desse arquivo.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ir para a documentação desse arquivo.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Pré-condição";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Pós-condição";
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
      return "código";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Gráfico de Hierarquia de Classes";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Ir para o gráfico de hierarquia de classes";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Ir para a hierarquia de classes (texto)";
    }
    QCString trPageIndex() override
    {
      return "Índice da Página";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Observação";
    }
    QCString trPublicTypes() override
    {
      return "Tipos Públicos";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos de Dados";
      }
      else
      {
        return "Atributos Públicos";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Atributos Públicos Estáticos";
    }
    QCString trProtectedTypes() override
    {
      return "Tipos Protegidos";
    }
    QCString trProtectedAttribs() override
    {
      return "Atributos Protegidos";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Atributos Protegidos Estáticos";
    }
    QCString trPrivateTypes() override
    {
      return "Tipos Privados";
    }
    QCString trPrivateAttribs() override
    {
      return "Atributos Privados";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Atributos Privados Estáticos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Atividades futuras";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Lista de atividades futuras";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referenciado(a) por";
    }
    QCString trRemarks() override
    {
      return "Anotações";
    }
    QCString trAttention() override
    {
      return "Atenção";
    }
    QCString trInclByDepGraph() override
    {
      return "Este grafo mostra quais arquivos estão direta ou indiretamente relacionados com esse arquivo:";
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
      return "Legenda do Grafo";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Esta página explica como interpretar os grafos gerados pelo doxygen.<p>\n"
        "Considere o seguinte exemplo:\n"
        "\\code\n"
        "/*! Classe oculta por causa da truncagem */\n"
        "class Invisible { };\n\n"
        "/*! Classe Truncated, relação de herança está oculta */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Classe não documentada com os comentários do Doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Classe possui que possui uma relação de herança pública */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Um template de classe */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Classe possui que possui uma relação de herança protegida  */\n"
        "class ProtectedBase { };\n\n"
        "/*! Classe possui que possui uma relação de herança privada  */\n"
        "class PrivateBase { };\n\n"
        "/*! Classe utilizada pela classe Inherited */\n"
        "class Used { };\n\n"
        "/*! Super classe que herda outras classes */\n"
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
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "As caixas no grafo acima possuem os seguintes significados:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Uma caixa cinza denota a estrutura ou a classe que gerou"
	"este grafo.</li>\n"
        "<li>Uma caixa com a borda preta denota uma estrutura ou classe documentada.</li>\n"
        "<li>Uma caixa com a borda cinza denota uma estrutura ou classe não documentada.</li>\n"
        "<li>Uma caixa com a borda vermelha denota uma estrutura ou classe na"
        "qual nem todas as relações são mostradas. Um grafo é truncado quando "
        "esse não cabe no espaço especificado.</li>\n"
        "</ul>\n"
        "<p>\n"
        "As setas possuem os seguintes significados:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Uma seta azul escura denota a relação de herança pública entre "
        "duas classes.</li>\n"
        "<li>Uma seta verde escura denota a relação de herança protegida entre "
        "duas classes.</li>\n"
        "<li>Uma seta vermelha escura denota a relação de herança privada entre "
        "duas classes.</li>\n"
        "<li>Uma seta pontilhada púrpura denota a relação de uso e/ou agragação "
        "por outra classe/estrutura. A etiqueta da seta denota o nome do(s) campo(s) "
        "que representa(m) este apontamento.</li>\n"
        "<li>Uma seta pontilhada amarela denota a relação de instanciação de uma "
        "<em>template</em>. A etiqueta da seta denota os parâmetros de instanciação "
        "desta <em>template</em>.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Teste";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Lista de Testes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Propriedades";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Propriedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estruturas de dados";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Pacote "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Esta é a lista com os pacotes e suas respectivas descrições (quando disponíveis):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Pacotes";
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
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
      return "Sumário";
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
      return createNoun(first_capital, singular, "arquivo", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupo", "s");
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
      return createNoun(first_capital, singular, "membro", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globa", "ais", "l");
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
      return "Referencias";
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
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementado por "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Sumário";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Lista de itens obsoletos";
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
      return "Eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipos do Pacote";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funções do Pacote";
    }
    QCString trPackageMembers() override
    {
      return "Membros do Pacote";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Funções Estáticas do Pacote";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributos do Pacote";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Atributos Estáticos do Pacote";
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
      return "Este é o diagrama das funções utilizadas por essa função:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
               "Mostrando os melhores resultados primeiro.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Resultados:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return  "Código-Fonte de " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hierarquia de Diretórios"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Documentação do Diretório"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Diretórios"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    {
        QCString result = "Referência do diretório ";
     	result += dirName;
     	return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "diretório", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Esse é um método provido por conveniência. "
       		"Ele difere do método acima apenas na lista de "
       		"argumentos que devem ser utilizados.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Esse é o diagrama das funções que utilizam essa função:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentação da enumeração"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentação de Função Membro/Subrotina"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lista de Tipos de Dados"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Campos de Dados"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Aqui estão os tipos de dados com descrições resumidas:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Índice de Tipos de Dados"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentação dos Tipos de Dados"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funções/Subrotinas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentação da Função/Subrotina"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipos de Dados"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Lista de Módulos"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Esta é a lista de todos os módulos ";
      if (!extractAll) result+="documentados ";
      result+="com descrições resumidas:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result = "Referência ";

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
        default: break;
      }
      result += clName;

      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result = "Referência do Módulo ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Membros do Módulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Índice dos Módulos"; }

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
      QCString result="A documentação para ";
      switch(compType)
      {
        case ClassDef::Class:      result+="esse modulo "; break;
        case ClassDef::Struct:     result+="esse tipo "; break;
        case ClassDef::Union:      result+="essa união "; break;
        case ClassDef::Interface:  result+="essa interface "; break;
        case ClassDef::Protocol:   result+="esso protocolo "; break;
        case ClassDef::Category:   result+="essa categoria "; break;
        case ClassDef::Exception:  result+="essa exceção "; break;
        default: break;
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
      return createNoun(first_capital, singular, "subrotina", "s");
    }

    /*! C# Type Contraint list */
    QCString trTypeConstraints() override
    {
      return "Restrições do Tipo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relação " + QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Carregando...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Namespace global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Procurando...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Nenhuma entrada encontrado";
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
      return "Arquivo em "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Inclui arquivo em "+name;
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
      static const char *days[]   = { "Segunda","Terça","Quarta","Quinta","Sexta","Sábado","Domingo" };
      static const char *months[] = { "Janeiro","Fevereiro","Março","Abril","Maio","Junho","Julho","Agosto","Setembro","Outubro","Novembro","Dezembro" };
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
      static const char *days_short[]   = { "seg", "ter", "qua", "qui", "sex", "sáb", "dom" };
      static const char *days_full[]    = { "segunda-feira", "terça-feira", "quarta-feira", "quinta-feira", "sexta-feira", "sábado", "domingo" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "fev", "mar", "abr", "mai", "jun", "jul", "ago", "set", "out", "nov", "dez" };
      static const char *months_full[]  = { "janeiro", "fevereiro", "março", "abril", "maio", "junho", "julho", "agosto", "setembro", "outubro", "novembro", "dezembro" };
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
    { return "Referências Bibliográficas"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    {
      // Note: I will left it as is because "Direitos autorais" may not fit
      // in the text.
      return "Copyright";
    }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Grafo de dependências do diretório ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "nível de detalhes"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parâmetros do template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "e "+number+" mais..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "A documentação para essa enumeração foi gerada a partir ";
      if (single) {
        result += "do seguinte arquivo:";
      } else {
        result += "dos seguintes arquivos:";
      }
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    {
      QCString result = "Referência da enumeração ";
      return result + name;
    }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" herdados de "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Outros membros herdados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "ativar" : "desativar";
      return "clique para "+opt+" a sincronização do painel";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Provido pela categoria @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "estende a classe @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Métodos da classe";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Métodos da instância";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Documentação do método";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Interfaces Exportadas"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Serviços Inclusos"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Grupos de Constantes"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result = "Referência do grupo de constantes ";
      result += namespaceName;
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result = "Referência do serviço ";
      result += sName;
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      // TODO: Emerson Ferreira proposes to add emphasis around the word
      // Singleton but I'm not sure if this will work for non HTML generators.
      QCString result = "Referência do Singleton ";
      result += sName;
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="A documentação para esse serviço "
                                "foi gerada a partir ";
      if (single) {
        result+="do seguinte arquivo:";
      } else {
        result+="dos seguintes arquivos:";
      }
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="A documentação para esse Singleton "
                                "foi gerada a partir ";
      if (single) {
        result+="do seguinte arquivo:";
      } else {
        result+="dos: seguintes arquivos:";
      }

      return result;
    }
    //////////////////////////////////////////////////////////////////////////
    // new since 1.8.15
    //////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Hierarquia da Unidade de Design"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Lista de Unidades de Design"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Membros da Unidade de Design"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Esta é uma lista de todos os membros de unidades de design "
        		"com ligações para as entidades às quais pertencem:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Índice de Unidades de Design"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Unidades de Design"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Funções/Procedimentos/Processos"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Biblioteca";
          else        return "Bibliotecas";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Pacote";
          else        return "Pacotes";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Sinal";
          else        return "Sinais";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Componente";
          else        return "Componentes";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Constante";
          else        return "Constantes";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entidade";
          else        return "Entidades";
        case VhdlSpecifier::TYPE:
          if (single) return "Tipo";
          else        return "Tipos";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Subtipo";
          else        return "Subtipos";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Função";
          else        return "Funções";
        case VhdlSpecifier::RECORD:
          if (single) return "Registro";
          else        return "Registros";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedimento";
          else        return "Procedimentos";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Arquitetura";
          else        return "Arquiteturas";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Atributo";
          else        return "Atributos";
        case VhdlSpecifier::PROCESS:
          if (single) return "Processo";
          else        return "Processos";
        case VhdlSpecifier::PORT:
          if (single) return "Porta";
          else        return "Portas";
        case VhdlSpecifier::USE:
          if (single) return "cláusula de uso";
          else        return "cláusulas de uso";
        case VhdlSpecifier::GENERIC:
          if (single) return "Generico";
          else        return "Genericos";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Corpo do Pacote";
        case VhdlSpecifier::UNITS:
          return "Unidades";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Variável Compartilhada";
          else        return "Variáveis Compartilhadas";
        case VhdlSpecifier::VFILE:
          if (single) return "Arquivo";
          else        return "Arquivos";
        case VhdlSpecifier::GROUP:
          if (single) return "Grupo";
          else        return "Grupos";
        case VhdlSpecifier::INSTANTIATION:
            if (single) return "Instância";
            else        return "Instâncias";
        case VhdlSpecifier::ALIAS:
          if (single) return "Apelido";
          else        return "Apelidos";
        case VhdlSpecifier::CONFIG:
          if (single) return "Configuração";
          else        return "Configurações";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Outros"; // Is this correct for VHDL?
        case VhdlSpecifier::UCF_CONST:
          return "Restrições";
        default:
          return "Classe";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return "Referência de " + QCString(name); }

    /* Slice */
    QCString trConstants() override
    {
        return "Constantes";
    }
    QCString trConstantDocumentation() override
    {
        return "Constantes";
    }
    QCString trSequences() override
    {
        return "Sequências";
    }
    QCString trSequenceDocumentation() override
    {
        return "Sequências";
    }
    QCString trDictionaries() override
    {
        return "Dicionários";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Dicionários";
    }
    QCString trSliceInterfaces() override
    {
        return "Interfaces";
    }
    QCString trInterfaceIndex() override
    {
        return "Índice de Interfaces";
    }
    QCString trInterfaceList() override
    {
        return "Lista de Interfaces";
    }
    QCString trInterfaceListDescription() override
    {
        return "Estas são as interfaces com suas respectivas descrições:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Hierarquia de Interfaces";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Esta é a lista parcialmente ordenadas de heranças:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Interfaces";
    }
    QCString trStructs() override
    {
        return "Estruturas";
    }
    QCString trStructIndex() override
    {
        return "Índice de Estruturas";
    }
    QCString trStructList() override
    {
        return "Lista de Estruturas";
    }
    QCString trStructListDescription() override
    {
        return "Estas são as estruturas com suas respectivas descrições:";
    }
    QCString trStructDocumentation() override
    {
        return "Estruturas";
    }
    QCString trExceptionIndex() override
    {
        return "Índice the Exceções";
    }
    QCString trExceptionList() override
    {
        return "Lista de Exceções";
    }
    QCString trExceptionListDescription() override
    {
        return "Estas são as exceções com suas respectivas descrições:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Hierarquia de Exceções";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Esta é a lista parcialmente ordenadas de heranças:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Exceções";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result="Referência ";
      switch(compType)
      {
        case ClassDef::Class:      result+="da Classe "; break;
        case ClassDef::Struct:     result+="da Estrutura "; break;
        case ClassDef::Union:      result+="da União "; break;
        case ClassDef::Interface:  result+="da Interface "; break;
        case ClassDef::Protocol:   result+="do Protocolo "; break;
        case ClassDef::Category:   result+="da Categoria "; break;
        case ClassDef::Exception:  result+="da Exceção "; break;
        default: break;
      }
      if (isLocal) result+="Local ";
      result+= clName;
      return result;
    }
    QCString trOperations() override
    {
        return "Operações";
    }
    QCString trOperationDocumentation() override
    {
        return "Operações";
    }
    QCString trDataMembers() override
    {
        return "Dados Membros";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Dados Membros";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.8.19
    //////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    {
        return "Documentação da Unidade de Projeto";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.2
    //////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "conceito", "s");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
        QCString result= "Referência do Conceito ";
        result+=conceptName;
        return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Lista de Conceitos"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Índice de Conceitos"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Documentação do Conceito"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
        QCString result="Esta é a lista de todos os conceitos ";
        if (!extractAll) result+="documentados ";
        result+="com suas respectivas descrições:";
        return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
        return "Definição de conceito";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.4
    //////////////////////////////////////////////////////////////////////////
    QCString trPackageList() override
    { return "Lista de pacotes"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.6
    //////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
    {
        return "Fluxograma:";
    }

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
    { return "(Note que estes não são símbolos membros.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Documentação dos símbolos amigos e relacionados"; }

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
        case ClassDef::Struct:     result="Estrutura"; break;
        case ClassDef::Union:      result="União"; break;
        case ClassDef::Interface:  result="Interface"; break;
        case ClassDef::Protocol:   result="Protocolo"; break;
        case ClassDef::Category:   result="Categoria"; break;
        case ClassDef::Exception:  result="Exceção"; break;
        case ClassDef::Service:    result="Serviço"; break;
        case ClassDef::Singleton:  result="Singleton"; break;
        default: break;
      }
      return result;
    }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isFileMemberHighlightMasculine(hl);
      QCString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="funções, variáveis, definições, enumerações e definições de tipos";
          }
          else
          {
            result+="membros do arquivo";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="funções";
          break;
        case FileMemberHighlight::Variables:
          result+="variáveis";
          break;
        case FileMemberHighlight::Typedefs:
          result+="definições de tipos";
          break;
        case FileMemberHighlight::Sequences:
          result+="sequencias";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="dicionários";
          break;
        case FileMemberHighlight::Enums:
          result+="enumerações";
          break;
        case FileMemberHighlight::EnumValues:
          result+="valores da enumeração";
          break;
        case FileMemberHighlight::Defines:
          result+="definições/macros";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      if (!extractAll)
      {
        result+= masculine? " documentados": " documentadas";
      }
      result+=" com links para ";
      if (extractAll)
        result+="os arquivos aos quais pertencem:";
      else
        result+="a documentação:";
      return result;
    }

    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isClassMemberHighlightMasculine(hl);
      QCString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="estruturas e uniões";
          }
          else
          {
            result+="membros de classe";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="funções";
          break;
        case ClassMemberHighlight::Variables:
          result+="variáveis";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="definições de tipo";
          break;
        case ClassMemberHighlight::Enums:
          result+="enumerações";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="valores enumerados";
          break;
        case ClassMemberHighlight::Properties:
          result+="propriedades";
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
      if (!extractAll)
      {
        result+= masculine?" documentados":" documentadas";
      }
      result+=" com links para ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a documentação da estrutura/união para cada campo:";
        }
        else
        {
          result+="a documentação da classe para cada membro:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a estrutura/união a que petencem:";
        }
        else
        {
          result+="a classe a que pertencem:";
        }
      }
      return result;
    }

    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isNamespaceMemberHighlightMasculine(hl);
      QCString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      QCString singularResult = "";
      QCString pluralResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="membro";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult="função";
          pluralResult="funções";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult="variável";
          pluralResult="variáveis";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult="definição de tipo";
          pluralResult="definições de tipo";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult="sequencia";
          pluralResult="sequencias";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult="dicionário";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult="enumeração";
          pluralResult="enumerações";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult="valor enumerado";
          pluralResult="valores enumerados";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+=(pluralResult.isEmpty() ? singularResult+"s" : pluralResult);
      result+=" do namespace ";
      if (!extractAll) result+=" documentado";
      result+=" com links para ";
      if (extractAll)
        result+="a documentação de cada " + singularResult + ":";
      else
        result+="o namespace a que pertencem:";
      return result;
    }
    QCString trDefinition() override  { return "Definição";}
    QCString trDeclaration() override { return "Declaração";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    QCString trTopics() override
    { return "Tópicos"; }
    QCString trTopicDocumentation() override
    { return "Tópico"; }
    QCString trTopicList() override
    { return "Lista de Tópicos"; }
    QCString trTopicIndex() override
    { return "Índice de Tópicos"; }
    QCString trTopicListDescription() override
    { return "Esta é uma lista de todos os tópicos e suas descrições:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isModuleMemberHighlightMasculine(hl);
      QCString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      QCString singularResult = "";
      QCString pluralResult = "";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          singularResult="membro";
          break;
        case ModuleMemberHighlight::Functions:
          singularResult="função";
          pluralResult="funções";
          break;
        case ModuleMemberHighlight::Variables:
          singularResult="variável";
          pluralResult="variáveis";
          break;
        case ModuleMemberHighlight::Typedefs:
          singularResult="definição de tipo";
          pluralResult="definições de tipo";
          break;
        case ModuleMemberHighlight::Enums:
          singularResult="enumeração";
          pluralResult="enumerações";
          break;
        case ModuleMemberHighlight::EnumValues:
          singularResult="valor enumerado";
          pluralResult="valores enumerados";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result+=(pluralResult.isEmpty() ? singularResult+"s" : pluralResult);
      result+="do módulo ";
      if (!extractAll) result+="documentado ";
      result+=" com links para ";
      if (extractAll)
        result+="a documentação de cada " + singularResult + ":";
      else
        result+="o módulo a que pertencem:";
      return result;
    }
    QCString trExportedModules() override
    {
      return "Módulos Exportados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "Copiado para a área de transferência";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    QCString trImportant() override
    {
      return "Importante";
    }
};

#endif
