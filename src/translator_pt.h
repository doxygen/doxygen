/******************************************************************************
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
 * Portuguese translation version
 *    Maintainer (from 04/28/2011):
 *       Fabio "FJTC" Jun Takada Chino <fjtc at brokenbits dot com dot br>
 *    Maintainer (until 04/28/2011):
 *       Rui Godinho Lopes <rui at ruilopes.com>
 *    Collaborators:
 *        Ulisses Guedes <uli1958 at hotmail dot com>
 *
 * Notes about this translation:
 *    Since I'm Brazilian, this translation may be odd or even incorrect for
 *    Portuguese (from Portugal) speakers. If you find any errors, feel free
 *    to contact me.
 *
 * VERSION HISTORY
 * ---------------
 * History:
 * 20260112:
 *  - Updated to 1.16.0;
 * 20240204:
 *  - Updated to 1.11.0:
 * 20231107:
 *  - Updated to 1.10.0:
 * 20230817:
 *  - Updated to 1.9.8;
 *  - Small fixes to the method TranslatorPortuguese::trNamespaceMembersDescriptionTotal().
 * 20230430:
 *  - Updated to 1.9.7;
 *  - Inclusion of translator_br.h's PortugueseTranslatorUtils namespace;
 *  - All entries of "Directório" has been replaced by "Diretório";
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
 * 20131129:
 *  - Updated to 1.8.5;
 *  - Translation in the method trFileMembers() fixed;
 * 20110628:
 *	- Updated to 1.7.5;
 *      - All obsolete methods have been removed;
 * 20110428
 *   - Updated to doxygen 1.6.3 using the Brazilian Portuguese as the base.
 *     Requires revision by a Portuguese (Portugal native speaker);
 * 007 09 june 2003
 *   ! Updated for doxygen v1.3.1
 * 006 30 july 2002
 *   ! Updated for doxygen v1.2.17
 * 005 10 july 2002
 *   ! Updated for doxygen v1.2.16
 * 004 03 march 2002
 *   ! Updated for doxygen v1.2.14
 * 003 23 november 2001
 *   - Removed some obsolete methods (latexBabelPackage, trAuthor, trAuthors and trFiles)
 * 002 19 november 2001
 *   ! Updated for doxygen v1.2.12
 * 001 20 july 2001
 *   ! Updated for doxygen v1.2.8.1
 * 000 ?
 *   + Initial translation for doxygen v1.1.5
 */

#ifndef TRANSLATOR_PT_H
#define TRANSLATOR_PT_H

#include "translator_br.h"

class TranslatorPortuguese : public Translator
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    DString idLanguage() override
    { return "portuguese"; }

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
    DString latexLanguageSupportCommand() override
    {
      return
        "\\usepackage[portuges]{babel}\n";
    }

    DString trISOLang() override
    {
      return "pt";
    }
    DString getLanguageString() override
    {
      return "0x816 Portuguese(Portugal)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "Funções associadas"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(Note que não são funções membro)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Descrição detalhada"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Detalhes"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Documentação das definições de tipo"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Documentação das enumerações"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
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
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentação dos campos e atributos";
      }
      else
      {
        return "Documentação dos dados membro";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Mais..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Mostrar lista completa dos membros"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Lista dos membros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Lista completa de todos os membros de"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return ", incluindo todos os membros herdados."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Gerado automaticamente por Doxygen";
      if (!s.empty()) result+=" para "+s;
      result+=" a partir do código fonte.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "enumeração"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "valor enumerado"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "definido em"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Módulos"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Hierarquia de classes"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estruturas de dados";
      }
      else
      {
        return "Lista de componentes";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Lista de ficheiros"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Campos de dados";
      }
      else
      {
        return "Componentes membro";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globais";
      }
      else
      {
        return "Membros dos Ficheiros";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "Páginas relacionadas"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Exemplos"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Localizar"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Esta lista de heranças está organizada, dentro do possível, por ordem alfabética:"; }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Lista de todos os ficheiros ";
      if (!extractAll) result+="documentados ";
      result+="com uma breve descrição:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Lista das estruturas de dados com uma breve descrição:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Lista de classes com uma breve descrição:";
      }
      else
      {
        return "Lista de classes, estruturas, uniões e interfaces com uma breve descrição:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Lista de todas as";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+=" estruturas e campos de uniões";
      }
      else
      {
        result+=" classes membro";
      }
      if (!extractAll)
      {
        result+=" documentadas";
      }
      result+=" com referência para ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="a respectiva documentação:";
        }
        else
        {
          result+="a documentação de cada membro:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="as estruturas/uniões a que pertencem:";
        }
        else
        {
          result+="as classes a que pertencem:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Lista de ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="todas as funções, variáveis, definições, enumerações e definições de tipo ";
        if (!extractAll) result+="documentadas ";
      }
      else
      {
        result+="todos os ficheiros membro ";
        if (!extractAll) result+="documentados ";
      }
      result+="com referência para ";
      if (extractAll)
        result+="o ficheiro a que pertecem:";
      else
        result+="a respectiva documentação:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Lista de todos os exemplos:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "Lista de documentação relacionada:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Lista de todos os módulos:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return "Documentação" + (!projName.empty()? " de " + projName : ""); }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Índice dos módulos"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Índice da hierarquia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Índice das estruturas de dados";
      }
      else
      {
        return "Índice dos componentes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Índice dos ficheiros"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Documentação do módulo"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentação da estruturas de dados";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Documentação da classe";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Documentação do ficheiro"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Manual de referência"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Macros"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Definições de tipos"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Enumerações"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Funções"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Variáveis"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Valores de enumerações"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Documentação das macros"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Documentação dos tipos"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Documentação dos valores da enumeração"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Documentação das funções"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Documentação das variáveis"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
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
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Gerado em "+date;
      if (!projName.empty()) result+=" para "+projName;
      result+=" por";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return "Diagrama de heranças da classe "+clName;
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Aviso"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Versão"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Retorna"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Veja também"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parâmetros"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "Excepções"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Gerado por"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Lista de namespaces"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Lista ";
      if (!extractAll) result+="de toda a documentação ";
      result+="dos namespaces com uma breve descrição:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Amigos"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Documentação das classes amigas e funções relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="Referência ";
      switch(compType)
      {
        case ClassDef::Class:      result+="à classe "; break;
        case ClassDef::Struct:     result+="à estrutura "; break;
        case ClassDef::Union:      result+="à união "; break;
        case ClassDef::Interface:  result+="ao interface "; break;
        case ClassDef::Protocol:   result+="ao protocolo "; break;
        case ClassDef::Category:   result+="à categoria "; break;
        case ClassDef::Exception:  result+="à excepção "; break;
        default: break;
      }
      if (isTemplate) result+="Template ";
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result= "Referência ao ficheiro ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result= "Referência ao namespace ";
      result += namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    DString trPublicMembers() override
    { return "Membros públicos"; }
    DString trPublicSlots() override
    { return "Slots públicos"; }
    DString trSignals() override
    { return "Sinais"; }
    DString trStaticPublicMembers() override
    { return "Membros públicos estáticos"; }
    DString trProtectedMembers() override
    { return "Membros protegidos"; }
    DString trProtectedSlots() override
    { return "Slots protegidos"; }
    DString trStaticProtectedMembers() override
    { return "Membros protegidos estáticos"; }
    DString trPrivateMembers() override
    { return "Membros privados"; }
    DString trPrivateSlots() override
    { return "Slots privados"; }
    DString trStaticPrivateMembers() override
    { return "Membros privados estáticos"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    DString trWriteList(int numEntries) override
    {
      DString result;
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
    DString trInheritsList(int numEntries) override
    {
      return "Derivada de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Herdado por "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementado de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Reimplementado em "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Membros do namespace"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Lista ";
      if (extractAll) result+="de todos os ";
      else result+="de toda a documentação dos ";
      result+="membros do namespace com referência para ";
      if (extractAll)
        result+="a documentação de cada membro:";
      else
        result+="o namespace correspondente:";
      return result;
    }

  /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Índice dos namespaces"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Documentação dos namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      DString result="A documentação para ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"esta Unidade de Design":"esta classe"; break;
        case ClassDef::Struct:     result+="esta estrutura"; break;
        case ClassDef::Union:      result+="esta união"; break;
        case ClassDef::Interface:  result+="este interface"; break;
        case ClassDef::Protocol:   result+="este protocolo"; break;
        case ClassDef::Category:   result+="esta categoria"; break;
        case ClassDef::Exception:  result+="esta excepção"; break;
        default: break;
      }
      result+=" foi gerada a partir ";
      if (single) result+=" do seguinte ficheiro:";
      else result+="dos seguintes ficheiros:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Valores retornados"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Página principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "p. "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "Definido na linha @0 do ficheiro @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "Definido no ficheiro @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Desaprovado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Diagrama de colaboração para "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Diagrama de dependências de inclusão para "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Documentação dos Construtores & Destrutor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Ir para o código fonte deste ficheiro.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Ir para a documentação deste ficheiro.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Precondição";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Poscondição";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariante";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Valor inicial:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "código";
    }
    DString trGraphicalHierarchy() override
    {
      return "Representação gráfica da hiearquia da classe";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Ir para a representação gráfica da hierarquia da classe";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Ir para a representação textual da hierarquia da classe";
    }
    DString trPageIndex() override
    {
      return "Índice da página";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Nota";
    }
    DString trPublicTypes() override
    {
      return "Tipos Públicos";
    }
    DString trPublicAttribs() override
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
    DString trStaticPublicAttribs() override
    {
      return "Atributos Públicos Estáticos";
    }
    DString trProtectedTypes() override
    {
      return "Tipos Protegidos";
    }
    DString trProtectedAttribs() override
    {
      return "Atributos Protegidos";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Atributos Protegidos Estáticos";
    }
    DString trPrivateTypes() override
    {
      return "Tipos Privados";
    }
    DString trPrivateAttribs() override
    {
      return "Atributos Privados";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Atributos Privados Estáticos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    {
      return "Tarefa";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Lista de tarefas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Referenciado por";
    }
    DString trRemarks() override
    {
      return "Observações";
    }
    DString trAttention() override
    {
      return "Atenção";
    }
    DString trInclByDepGraph() override
    {
      return "Este grafo mostra quais são os ficheiros que incluem directamente ou indirectamente este ficheiro:";
    }
    DString trSince() override
    {
      return "Desde";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Legenda do grafo";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Esta página explica como interpretar os grafos gerados pelo doxygen.<p>\n"
        "Considere o seguinte exemplo:\n"
        "\\code\n"
        "/*! Esta classe vai estar escondida devido à truncação */\n"
        "class Invisible { };\n\n"
        "/*! Esta classe tem a relação de herança escondida */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Classe não documentada por comentários doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Classe derivada usando derivação pública */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Classe derivada usando derivação protegida */\n"
        "class ProtectedBase { };\n\n"
        "/*! Classe derivada usando derivação privada */\n"
        "class PrivateBase { };\n\n"
        "/*! Classe usada pela classe Inherited */\n"
        "class Used { };\n\n"
        "/*! Super classe que deriva de várias classes */\n"
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
        "As caixas no grafo anterior têm as seguintes interpretações:\n"
        "<ul>\n"
        "<li>Uma caixa inteiramente preta representa a estrutura ou a classe para "
        "a qual o grafo foi gerado.\n"
        "<li>Uma caixa com borda preta representa uma estrutura ou classe documentada.\n"
        "<li>Uma caixa com borda cinzenta representa uma estrutura ou classe não documentada.\n"
        "<li>Uma caixa com borda vermelha representa uma estrutura ou classe documentada onde "
        "nem todas as relações de herança/encapsulamento são exibidas. Um grafo é truncado "
        "quando não cabe na sua área predefinida.\n"
        "</ul>\n"
        "As setas têm a seguinte interpretação:\n"
        "<ul>\n"
        "<li>Uma seta azul escura representa uma relação de herança pública entre duas classes.\n"
        "<li>Uma seta verde escura representa uma relação de herança protegida.\n"
        "<li>Uma seta vermelha escura representa uma relação de herança privada.\n"
        "<li>Uma seta rocha em tracejado representa uma relação de encapsulamento ou uso por "
        "parte de outra classe. A legenda da seta contém o nome da variável ou variáveis da "
        "relação. A seta aponta da classe que estabelece a relação para a classe ou estrutura que "
        "é acessível.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Teste";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Lista de teste";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Propriedades";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Documentação das propriedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
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
    DString trPackage(const DString &name) override
    {
      return "Pacote "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Lista de pacotes com uma breve descrição (se disponível):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Pacotes";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Valor:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
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
    DString trRTFansicp() override
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Índice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "classe", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "ficheiro", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grupo", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "página", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membro", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globa", "is", "l");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "es");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Referências";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Implementado em "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "Índice";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Lista de Deprecados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Eventos";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Documentação dos eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Tipos do Pacote";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Funções do Pacote";
    }
    DString trPackageMembers() override
    {
      return "Membros do Pacote";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Funções Estáticas do Pacote";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Atributos do Pacote";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Atributos Estáticos do Pacote";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Tudo";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Grafo de chamadas desta função:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3 - Based on the Brazilian Portuguese Translation
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
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
    DString trSearchResults(int numDocuments) override
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
    DString trSearchMatches() override
    {
      return "Resultados:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return  "Código-Fonte de " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Hierarquia de Diretórios"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Documentação do Diretório"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Diretórios"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    {
        DString result = "Referência do diretório ";
     	result += dirName;
     	return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "diretório", "s");
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Este é um método provido por conveniência. "
       		"Ele difere do método acima apenas na lista de "
       		"argumentos que devem ser utilizados.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Este é o diagrama das funções que utilizam esta função:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Documentação da enumeração"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Documentação de Função Membro/Subrotina"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Lista de Tipos de Dados"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Campos de Dados"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Aqui estão os tipos de dados com descrições resumidas:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Esta é a lista de todos os membros ";
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
    DString trCompoundIndexFortran() override
    { return "Índice de Tipos de Dados"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Documentação dos Tipos de Dados"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Funções/Subrotinas"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Documentação da Função/Subrotina"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Tipos de Dados"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Lista de Módulos"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Esta é a lista de todos os módulos ";
      if (!extractAll) result+="documentados ";
      result+="com descrições resumidas:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result = "Referência ";

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
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result = "Referência do Módulo ";
      result += namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Membros do Módulo"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Esta é a lista de todos os membros ";
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
    DString trModulesIndex() override
    { return "Índice dos Módulos"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modulo", "s");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="A documentação para ";
      switch(compType)
      {
        case ClassDef::Class:      result+="este modulo "; break;
        case ClassDef::Struct:     result+="este tipo "; break;
        case ClassDef::Union:      result+="esta união "; break;
        case ClassDef::Interface:  result+="esta interface "; break;
        case ClassDef::Protocol:   result+="esto protocolo "; break;
        case ClassDef::Category:   result+="esta categoria "; break;
        case ClassDef::Exception:  result+="esta exceção "; break;
        default: break;
      }

      result+=" foi gerada a partir do";
      if (single)
    	result+=" seguinte ficheiro:";
      else
    	result+="s seguintes ficheiros:";

      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tipo", "s");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprograma", "s");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Restrições do Tipo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name
     */
    DString trDirRelation(const DString &name) override
    {
      return "Relação " + name;
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Carregando...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Namespace global";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Procurando...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Nenhuma entrada encontrada";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    DString trFileIn(const DString &name) override
    {
      return "Ficheiro em "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Inclui ficheiro em "+name;
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
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Segunda","Terça","Quarta","Quinta","Sexta","Sábado","Domingo" };
      static const char *months[] = { "Janeiro","Fevereiro","Março","Abril","Maio","Junho","Julho","Agosto","Setembro","Outubro","Novembro","Dezembro" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s, %d de %s de %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "seg", "ter", "qua", "qui", "sex", "sáb", "dom" };
      static const char *days_full[]    = { "segunda-feira", "terça-feira", "quarta-feira", "quinta-feira", "sexta-feira", "sábado", "domingo" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "fev", "mar", "abr", "mai", "jun", "jul", "ago", "set", "out", "nov", "dez" };
      static const char *months_full[]  = { "janeiro", "fevereiro", "março", "abril", "maio", "junho", "julho", "agosto", "setembro", "outubro", "novembro", "dezembro" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Referências Bibliográficas"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    {
      // Note: I will leave it as is because "Direitos de autor" may not fit
      // in the text.
      return "Copyright";
    }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return DString("Grafo de dependências do diretório ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "nível de detalhes"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Parâmetros de template"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "e "+number+" mais..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "A documentação para esta enumeração foi gerada a partir";
      if (single) {
        result += "do seguinte ficheiro:";
      } else {
        result += "dos seguintes ficheiros:";
      }
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    {
      DString result = "Referência da enumeração ";
      return result + name;
    }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" herdados de "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Outros membros herdados"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "ativar" : "desativar";
      return "clique para "+opt+" a sincronização do painel";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Provido pela categoria @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "estende a classe @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Métodos de classe";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Métodos de instância";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Documentação do método";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Interfaces Exportadas"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Serviços Incluídos"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Grupos de Constantes"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result = "Referência do grupo de constantes ";
      result += namespaceName;
      return result;
    }

    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result = "Referência do serviço ";
      result += sName;
      return result;
    }

    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result = "Referência do Singleton ";
      result += sName;
      return result;
    }

    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="A documentação para este serviço "
                                "foi gerada a partir ";
      if (single) {
        result+="do seguinte ficheiro:";
      } else {
        result+="dos: seguintes ficheiros:";
      }
      return result;
    }

    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      DString result="A documentação para este singleton "
                                "foi gerada a partir ";
      if (single) {
        result+="do seguinte ficheiro:";
      } else {
        result+="dos: seguintes ficheiros:";
      }

      return result;
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.8.15
    //////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "Hierarquia da Unidade de Design"; }
    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "Lista de Unidades de Design"; }
    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "Membros da Unidade de Design"; }
    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
        return "Esta é uma lista de todos os membros de unidades de design "
        		"com ligações para as entidades às quais pertencem:";
    }
    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "Índice de Unidades de Design"; }
    /** VHDL design units */
    DString trDesignUnits() override
    { return "Unidades de Design"; }
    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "Funções/Procedimentos/Processos"; }
    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type,bool single) override
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
          if (single) return "Ficheiro";
          else        return "Ficheiros";
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
    DString trCustomReference(const DString &name) override
    { return "Referência de " + name; }

    /* Slice */
    DString trConstants() override
    {
        return "Constantes";
    }
    DString trConstantDocumentation() override
    {
        return "Constantes";
    }
    DString trSequences() override
    {
        return "Sequências";
    }
    DString trSequenceDocumentation() override
    {
        return "Sequências";
    }
    DString trDictionaries() override
    {
        return "Dicionários";
    }
    DString trDictionaryDocumentation() override
    {
        return "Dicionários";
    }
    DString trSliceInterfaces() override
    {
        return "Interfaces";
    }
    DString trInterfaceIndex() override
    {
        return "Índice de Interfaces";
    }
    DString trInterfaceList() override
    {
        return "Lista de Interfaces";
    }
    DString trInterfaceListDescription() override
    {
        return "Estas são as interfaces com suas respectivas descrições:";
    }
    DString trInterfaceHierarchy() override
    {
        return "Hierarquia de Interfaces";
    }
    DString trInterfaceHierarchyDescription() override
    {
        return "Esta é a lista parcialmente ordenadas de heranças:";
    }
    DString trInterfaceDocumentation() override
    {
        return "Interfaces";
    }
    DString trStructs() override
    {
        return "Estruturas";
    }
    DString trStructIndex() override
    {
        return "Índice de Estruturas";
    }
    DString trStructList() override
    {
        return "Lista de Estruturas";
    }
    DString trStructListDescription() override
    {
        return "Estas são as estruturas com suas respectivas descrições:";
    }
    DString trStructDocumentation() override
    {
        return "Estruturas";
    }
    DString trExceptionIndex() override
    {
        return "Índice the Exceções";
    }
    DString trExceptionList() override
    {
        return "Lista de Exceções";
    }
    DString trExceptionListDescription() override
    {
        return "Estas são as exceções com suas respectivas descrições:";
    }
    DString trExceptionHierarchy() override
    {
        return "Hierarquia de Exceções";
    }
    DString trExceptionHierarchyDescription() override
    {
        return "Esta é a lista parcialmente ordenadas de heranças:";
    }
    DString trExceptionDocumentation() override
    {
        return "Exceções";
    }
    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result="Referência ";
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
    DString trOperations() override
    {
        return "Operações";
    }
    DString trOperationDocumentation() override
    {
        return "Operações";
    }
    DString trDataMembers() override
    {
        return "Dados Membros";
    }
    DString trDataMemberDocumentation() override
    {
        return "Dados Membros";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.8.19
    //////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    DString trDesignUnitDocumentation() override
    {
        return "Documentação da Unidade de Projeto";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.2
    //////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    DString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "conceito", "s");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    DString trConceptReference(const DString &conceptName) override
    {
        DString result= "Referência do Conceito ";
        result+=conceptName;
        return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    DString trConceptList() override
    { return "Lista de Conceitos"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    DString trConceptIndex() override
    { return "Índice de Conceitos"; }

    /*! used as the title of chapter containing all information about concepts. */
    DString trConceptDocumentation() override
    { return "Documentação do Conceito"; }

    /*! used as an introduction to the concept list */
    DString trConceptListDescription(bool extractAll) override
    {
        DString result="Esta é a lista de todos os conceitos ";
        if (!extractAll) result+="documentados ";
        result+="com suas respectivas descrições:";
        return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    DString trConceptDefinition() override
    {
        return "Definição de conceito";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.4
    //////////////////////////////////////////////////////////////////////////
    DString trPackageList() override
    { return "Lista de pacotes"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.6
    //////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
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
    DString trRelatedSymbols() override
    { return "Símbolos relacionados"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    DString trRelatedSymbolsSubscript() override
    { return "(Note que estes não são símbolos membros.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    DString trRelatedSymbolDocumentation() override
    { return "Documentação dos símbolos amigos e relacionados"; }

    /*! the compound type as used for the xrefitems */
    DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      DString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) result=trType(true,true);
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

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isFileMemberHighlightMasculine(hl);
      DString result="Esta é a list de ";
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
            result+="membros do ficheiro";
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
        result+="os ficheiros aos quais pertencem:";
      else
        result+="a documentação:";
      return result;
    }

    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isClassMemberHighlightMasculine(hl);
      DString result="Esta é a list de ";
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

    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isNamespaceMemberHighlightMasculine(hl);
      DString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      DString singularResult = "";
      DString pluralResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="membros";

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
      result+=(pluralResult.empty() ? singularResult+"s" : pluralResult);
      result+=" do namespace ";
      if (!extractAll) result+="documentado ";
      result+=" com links para ";
      if (extractAll)
        result+="a documentação de cada " + singularResult + ":";
      else
        result+="o namespace a que pertencem:";
      return result;
    }
    DString trDefinition() override  { return "Definição";}
    DString trDeclaration() override { return "Declaração";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    DString trTopics() override
    { return "Tópicos"; }
    DString trTopicDocumentation() override
    { return "Tópico"; }
    DString trTopicList() override
    { return "Lista de Tópicos"; }
    DString trTopicIndex() override
    { return "Índice de Tópicos"; }
    DString trTopicListDescription() override
    { return "Esta é uma lista de todos os tópicos e suas descrições:"; }
    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = PortugueseTranslatorUtils::isModuleMemberHighlightMasculine(hl);
      DString result="Esta é a list de ";
      result+= (masculine?"todos os ":"todas as ");
      DString singularResult = "";
      DString pluralResult = "";
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
      result+=(pluralResult.empty() ? singularResult+"s" : pluralResult);
      result+="do módulo ";
      if (!extractAll) result+="documentado ";
      result+=" com links para ";
      if (extractAll)
        result+="a documentação de cada " + singularResult + ":";
      else
        result+="o módulo a que pertencem:";
      return result;
    }
    DString trExportedModules() override
    {
      return "Módulos Exportados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    DString trCopyToClipboard() override
    {
      return "Copiado para a área de transferência";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    DString trImportant() override
    {
      return "Importante";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    DString trRequirements() override
    {
      return "Requisitos";
    }
    // table header for the column with the requirements IDs
    DString trRequirementID() override
    {
      return "ID";
    }
    // indicates a symbol implements (satisfies) a requirement
    DString trSatisfies(bool singular) override
    {
      // createNoun() cannot be used because the article must match the noum
      DString result = "Satisfaz o";
      return result + (singular ? " requisito" : "s requisitos");
    }
    // indicates a requirement is satisfied (implemented) by one or more symbols
    DString trSatisfiedBy(const DString &list) override
    {
      return "Satisfeito por "+list+".";
    }
    DString trUnsatisfiedRequirements() override
    {
      return "Requisitos não satisfeitos";
    }
    DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "O requisito "+list+" não possui a relação 'satisfaz'." :
        "Os requisitos "+list+" não possuem a relação 'satisfaz'.";
    }
    // indicates a symbol verifies (tests) a requirement
    DString trVerifies(bool singular) override
    {
      // createNoun() cannot be used because the article must match the noum
      DString result = "Verifica o";
      return result + (singular ? " requisito" : "s requisitos");
    }
    // indicates a requirement is verified (tested) by one or more symbols
    DString trVerifiedBy(const DString &list) override
    {
      return "Verificado por "+list+".";
    }
    DString trUnverifiedRequirements() override
    {
      return "Requisitos não verificados";
    }
    DString trUnverifiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "O requisito "+list+" não possui a relação 'verificado por'." :
        "Os requisitos "+list+" não possuem a relação 'verificado por'.";
    }
};

#endif
