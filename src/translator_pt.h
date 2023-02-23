/******************************************************************************
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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
 * Shared code with the Brazilian Portuguese translator:
 *    In order to reduce the amount of efforts used to maintain both Portuguese
 *    translators, in 02/2023, I decided to merge the Portuguese translator with
 *    the Brazilian Portuguse translator as most methods have the exact same
 *    implementation. Altough Brazilian Portuguse and Portugal's Portuguese have
 *    some differences, the written form, specially after the Portuguese 
 *    Language Orthographic Agreement (1990), became the same with the exception
 *    of some words that are not shared (e.g.: "arquivo"="ficheiro").
 * 
 *    It is important to notice that when the translation written by Rui Godinho
 *    Lopes was better than mine and had the same meaning in both languates, I
 *    moved his translation to the Brazilian translator and removed it from the
 *    Portuguese translator.
 *
 *    From now on, the translations must be performed on the Brazilian 
 *    translator first and the differences overriden by the Portuguese 
 *    translator whenever necessary.
 *
 * VERSION HISTORY
 * ---------------
 * History:
 *  20230219:
 *  - Updated to 1.9.7;
 *  - TranslatorPortuguese is now a subclass of TranslatorBrazilian. Only 
 *    methods that are really distinct will be overriden. 
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

class TranslatorPortuguese : public TranslatorBrazilian
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return
        "\\usepackage[portuges]{babel}\n";
    }

    virtual QCString trISOLang()
    {
      return "pt";
    }
    virtual QCString getLanguageString()
    {
      return "0x816 Portuguese(Portugal)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "Funções associadas"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "(Note que não são funções membro)"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation()
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

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "Mostrar lista completa dos membros"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "Lista completa de todos os membros de "; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers()
    { return ", incluindo todos os membros herdados."; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn()
    { return "definido em"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
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
    QCString trFileList()
    { return "Lista de ficheiros"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
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
    QCString trFileMembers()
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

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch()
    { return "Localizar"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription()
    { return "Esta lista de heranças está organizada, dentro do possível, por ordem alfabética:"; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Lista de todos os ficheiros ";
      if (!extractAll) result+="documentados ";
      result+="com uma breve descrição:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
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
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Lista de todas as";
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
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Lista de ";
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

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "Lista de documentação relacionada:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex()
    { return "Índice da hierarquia"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex()
    { return "Índice dos ficheiros"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation()
    { return "Documentação do módulo"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation()
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
    QCString trFileDocumentation()
    { return "Documentação do ficheiro"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual()
    { return "Manual de referência"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines()
    { return "Macros"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues()
    { return "Valores de enumerações"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation()
    { return "Documentação das macros"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation()
    { return "Documentação dos tipos"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "Documentação dos valores da enumeração"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation()
    { return "Documentação das funções"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation()
    { return "Documentação das variáveis"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions()
    { return "Excepções"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Lista ";
      if (!extractAll) result+="de toda a documentação ";
      result+="dos namespaces com uma breve descrição:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Documentação das classes amigas e funções relacionadas"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="Referência ";
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
    virtual QCString trFileReference(const QCString &fileName)
    {
      QCString result= "Referência ao ficheiro ";
      result += fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName)
    {
      QCString result= "Referência ao namespace ";
      result += namespaceName;
      return result;
    }


    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Derivada de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Herdado por "+trWriteList(numEntries)+".";
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
      return "Reimplementado em "+trWriteList(numEntries)+".";
    }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Lista ";
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
    virtual QCString trNamespaceIndex()
    { return "Índice dos namespaces"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Documentação dos namespaces"; }

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
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="A documentação para ";
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
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definido na linha @0 do ficheiro @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definido no ficheiro @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Desaprovado";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName)
    {
      return "Diagrama de dependências de inclusão para "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Documentação dos Construtores & Destrutor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Ir para o código fonte deste ficheiro.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Ir para a documentação deste ficheiro.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Precondição";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Poscondição";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Representação gráfica da hiearquia da classe";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Ir para a representação gráfica da hierarquia da classe";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Ir para a representação textual da hierarquia da classe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Nota";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Tarefa";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Lista de tarefas";
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
      return "Observações";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Este grafo mostra quais são os ficheiros que incluem directamente ou indirectamente este ficheiro:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda do grafo";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
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

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Documentação das propriedades";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Lista de pacotes com uma breve descrição (se disponível):";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Índice";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Ficheiro" : "ficheiro"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementado em "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Índice";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Lista de Deprecados";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Documentação dos eventos";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Tudo";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Grafo de chamadas desta função:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3 - Based on the Brazilian Portuguese Translation
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="A documentação para ";
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
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Subprograma" : "subprograma"));
      if (!singular)  result+="s";
      return result;
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
      return "Ficheiro em "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name)
    {
      return "Inclui ficheiro em "+name;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parâmetros de template"; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "A documentação para esta enumeração foi gerada a partir";
      if (single) {
        result += "do seguinte ficheiro:";
      } else {
        result += "dos seguintes ficheiros:";
      }
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Métodos de classe";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Métodos de instância";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result="A documentação para este serviço "
                                "foi gerada a partir ";
      if (single) {
        result+="do seguinte ficheiro:";
      } else {
        result+="dos: seguintes ficheiros:";
      }
      return result;
    }

    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result="A documentação para este singleton "
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

    /** VHDL type */
    virtual QCString trVhdlType(uint64_t type,bool single)
    {
      switch(type)
      {
        case VhdlDocGen::LIBRARY:
          if (single) return "Biblioteca";
          else        return "Bibliotecas";
        case VhdlDocGen::PACKAGE:
          if (single) return "Pacote";
          else        return "Pacotes";
        case VhdlDocGen::SIGNAL:
          if (single) return "Sinal";
          else        return "Sinais";
        case VhdlDocGen::COMPONENT:
          if (single) return "Componente";
          else        return "Componentes";
        case VhdlDocGen::CONSTANT:
          if (single) return "Constante";
          else        return "Constantes";
        case VhdlDocGen::ENTITY:
          if (single) return "Entidade";
          else        return "Entidades";
        case VhdlDocGen::TYPE:
          if (single) return "Tipo";
          else        return "Tipos";
        case VhdlDocGen::SUBTYPE:
          if (single) return "Subtipo";
          else        return "Subtipos";
        case VhdlDocGen::FUNCTION:
          if (single) return "Função";
          else        return "Funções";
        case VhdlDocGen::RECORD:
          if (single) return "Registro";
          else        return "Registros";
        case VhdlDocGen::PROCEDURE:
          if (single) return "Procedimento";
          else        return "Procedimentos";
        case VhdlDocGen::ARCHITECTURE:
          if (single) return "Arquitetura";
          else        return "Arquiteturas";
        case VhdlDocGen::ATTRIBUTE:
          if (single) return "Atributo";
          else        return "Atributos";
        case VhdlDocGen::PROCESS:
          if (single) return "Processo";
          else        return "Processos";
        case VhdlDocGen::PORT:
          if (single) return "Porta";
          else        return "Portas";
        case VhdlDocGen::USE:
          if (single) return "cláusula de uso";
          else        return "cláusulas de uso";
        case VhdlDocGen::GENERIC:
          if (single) return "Generico";
          else        return "Genericos";
        case VhdlDocGen::PACKAGE_BODY:
          return "Corpo do Pacote";
        case VhdlDocGen::UNITS:
          return "Unidades";
        case VhdlDocGen::SHAREDVARIABLE:
          if (single) return "Variável Compartilhada";
          else        return "Variáveis Compartilhadas";
        case VhdlDocGen::VFILE:
          if (single) return "Ficheiro";
          else        return "Ficheiros";
        case VhdlDocGen::GROUP:
          if (single) return "Grupo";
          else        return "Grupos";
        case VhdlDocGen::INSTANTIATION:
          if (single) return "Instância";
          else        return "Instâncias";
        case VhdlDocGen::ALIAS:
          if (single) return "Apelido";
          else        return "Apelidos";
        case VhdlDocGen::CONFIG:
          if (single) return "Configuração";
          else        return "Configurações";
        case VhdlDocGen::MISCELLANEOUS:
          return "Outros"; // Is this correct for VHDL?
        case VhdlDocGen::UCF_CONST:
          return "Restrições";
        default:
          return "Classe";
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.9.7
    //////////////////////////////////////////////////////////////////////////

    virtual QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl)
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      bool masculine = isFileMemberHighlightMasculine(hl);
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
        result+="os ficheiros aos quais pertencem:";
      else
        result+="a documentação:";
      return result;
    }

};

#endif
