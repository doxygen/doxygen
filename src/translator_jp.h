/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_JP_H
#define TRANSLATOR_JP_H

#include "translator_adapter.h"

class TranslatorJapanese : public TranslatorAdapter_1_2_5
{
  public:
    QCString idLanguage()
    { return "japanese"; }
    /*! returns the name of the package that is included by LaTeX */
    QCString latexBabelPackage()
    { return "english"; }
    QCString idLanguageCharset()
    { return "euc-jp"; }

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "関連する関数"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "（これらはメンバ関数でないことに注意）"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription()
    { return "解説"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation()
    { return "メンバ型定義の解説"; }
    
    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation()
    { return "メンバ列挙型の解説"; }
    
    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation()
    { return "メンバ関数の解説"; }
    
    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation()
    { return "メンバデータの解説"; }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore()
    { return "より詳しく..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "すべてのメンバリスト"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList()
    { return "メンバリスト"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "これは全メンバリストです。"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers()
    { return "継承メンバもすべて含んでいます。"; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const char *s)
    { QCString result;
      if (s) result=(QCString)s+"の";
      result+="ソースコードから Doxygen が自動的に生成しました。";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName()
    { return "enum 型"; }
    
    /*! put after an enum value in the list of all members */
    QCString trEnumValue()
    { return "enum 値"; }
    
    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn()
    { return "次で定義されました。"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    QCString trVerbatimText(const char *f)
    { return (QCString)"これはインクルードファイル"+f+"の内容です。"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \group command).
     */
    QCString trModules()
    { return "モジュール"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy()
    { return "クラス階層"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
    { return "データ構造リスト"; }
    
    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList()
    { return "ファイルリスト"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    QCString trHeaderFiles()
    { return "ヘッダファイル"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
    { return "データ構造メンバ"; }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers()
    { return "ファイルメンバ"; }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages()
    { return "関連ページ"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples()
    { return "例"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch()
    { return "検索"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription()
    { return "この継承リストはおおまかにはソートされていますが、"
             "アルファベット順で完全にソートされてはいません。";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="このリストは、";
      if (!extractAll) result+="ドキュメント化され、";
      result+="簡易説明を持つファイルすべてのリストです。";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
    { return "これは簡易説明を持つ、クラス、構造体、共用体のリストです。";}

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="これは";
      if (!extractAll) result+="ドキュメント化された";
      result+="クラスメンバすべてのリストで、それぞれ";
      if (extractAll) result+="が属しているクラス";
      result+="の解説へのリンクが張られています。";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="これは";
      if (!extractAll) result+="ドキュメント化された";
      result+="ファイルメンバすべてのリストで、それぞれ";
      if (extractAll) result+="が属しているファイル";
      result+="の解説へのリンクが張られています。";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    QCString trHeaderFilesDescription()
    { return "APIを構成するヘッダファイルです。"; }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription()
    { return "すべての例のリストです。"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "関連するドキュメントページすべてのリストです。"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription()
    { return "すべてのモジュールのリストです。"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    QCString trNoDescriptionAvailable()
    { return "ドキュメントが記述されていません。"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation()
    { return "ドキュメント"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    QCString trModuleIndex()
    { return "モジュール索引"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    QCString trHierarchicalIndex()
    { return "階層索引"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    QCString trCompoundIndex()
    { return "データ構造索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() 
    { return "ファイル索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation()
    { return "モジュールの解説"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation()
    { return "クラスの解説"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation()
    { return "ファイルの解説"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    QCString trExampleDocumentation()
    { return "例題の解説"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    QCString trPageDocumentation()
    { return "ページの解説"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual()
    { return "リファレンスマニュアル"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    QCString trDefines()
    { return "マクロ定義"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    QCString trFuncProtos()
    { return "関数プロトタイプ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    QCString trTypedefs()
    { return "型定義"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    QCString trEnumerations()
    { return "列挙型"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    QCString trFunctions()
    { return "関数"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trVariables()
    { return "変数"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trEnumerationValues()
    { return "列挙型値"; }
    
    /*! This is used in man pages as the author section. */
    QCString trAuthor()
    { return "作者"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation()
    { return "マクロ定義の解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    QCString trFunctionPrototypeDocumentation()
    { return "関数プロトタイプの解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation()
    { return "型定義の解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "列挙型の解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation()
    { return "列挙型値の解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation()
    { return "関数の解説"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation()
    { return "変数の解説"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    QCString trCompounds()
    { return "データ構造"; }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    QCString trFiles()
    { return "ファイル"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result;
      if (projName) result+=(QCString)projName+"に対して";
      result+=(QCString)date+"に生成されました。";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    QCString trWrittenBy()
    {
      return "の開発者:";
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"に対するクラス階層図";
    }
    
    /*! this text is generated when the \\internal command is used. */
    QCString trForInternalUseOnly()
    { return "内部使用のみ。"; }

    /*! this text is generated when the \\reimp command is used. */
    QCString trReimplementedForInternalReasons()
    { return "内部的な理由により再実装されましたが、APIには影響しません。";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning()
    { return "警告"; }

    /*! this text is generated when the \\bug command is used. */
    QCString trBugsAndLimitations()
    { return "バグと制限"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion()
    { return "バージョン"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate()
    { return "日付"; }

    /*! this text is generated when the \\author command is used. */
    QCString trAuthors()
    { return "作者"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns()
    { return "戻り値"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso()
    { return "参照"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters()
    { return "引数"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions()
    { return "例外"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy()
    { return ""; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList()
    { return "名前空間リスト"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="このリストは、簡易説明を持つすべての";
      if (!extractAll) result+="ドキュメント化された";
      result+="名前空間のリストです。";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends()
    { return "フレンド"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "フレンドと関連する関数の解説"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool /*isTemplate*/)
    {
      QCString result="";
      switch(compType)
      {
        case ClassDef::Class:      result+="クラス "; break;
        case ClassDef::Struct:     result+="構造体 "; break;
        case ClassDef::Union:      result+="共用体 "; break;
        case ClassDef::Interface:  result+="インタフェース"; break;
        case ClassDef::Exception:  result+="Exception "; break; //TODO:fixme
      }
      result+=(QCString)clName+" の解説";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const char *fileName)
    {
      QCString result="ファイル "+(QCString)fileName+" の解説"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="名前空間 "+(QCString)namespaceName+" の解説";
      return result;
    }
    
    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union 
     */
    QCString trPublicMembers()
    { return "公開メンバ"; }
    QCString trPublicSlots()
    { return "公開スロット"; }
    QCString trSignals()
    { return "シグナル"; }
    QCString trStaticPublicMembers()
    { return "静的公開メンバ"; }
    QCString trProtectedMembers()
    { return "保護メンバ"; }
    QCString trProtectedSlots()
    { return "保護スロット"; }
    QCString trStaticProtectedMembers()
    { return "静的保護メンバ"; }
    QCString trPrivateMembers()
    { return "非公開メンバ"; }
    QCString trPrivateSlots()
    { return "非公開スロット"; }
    QCString trStaticPrivateMembers()
    { return "静的非公開メンバ"; }
    /*! \endmgroup */ 
    
    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries)
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
            result+=", と ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries)
    {
      return trWriteList(numEntries)+"を継承しています。";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries)
    {
      return trWriteList(numEntries)+"に継承されています。";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"を再定義しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"で再定義されています。";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers()
    { return "名前空間メンバ"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll)
    { 
	QCString result="これは";
      if (!extractAll) result+="ドキュメント化された";
      result+="名前空間すべてのリストで、それぞれ";
      if (extractAll) 
	  result+="、名前空間の解説";
      else 
	  result+="が属している名前空間";
      result+="へのリンクが張られています。";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "名前空間索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "名前空間の解説"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "名前空間"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"この";
      switch(compType)
      {
        case ClassDef::Class:      result+="クラス"; break;
        case ClassDef::Struct:     result+="構造体"; break;
        case ClassDef::Union:      result+="共用体"; break;
        case ClassDef::Interface:  result+="インタフェース"; break;
        case ClassDef::Exception:  result+="Exception"; break; //TODO:fixme
      }
      result+="の解説は次のファイルから生成されました:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "アルファベット順リスト"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "戻り値"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "メインページ"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return "ソース";
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return "ファイル @1 の @0 行で定義されています。";
    }
    QCString trDefinedInSourceFile()
    {
      return "ファイル @0 で定義されています。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+"のコラボレーション図";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
	return (QCString)fName+"のインクルード依存関係図";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "コンストラクタとデストラクタの解説"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "このファイルのソースコードを見る。";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "このファイルの解説を見る。";
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "事前条件";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "事後条件";
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "初期値:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "コード";
    }
    QCString trGraphicalHierarchy()
    {
      return "クラス階層図";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "クラス階層図を見る。";
    }
    QCString trGotoTextualHierarchy()
    {
      return "クラス階層図(テキスト)を見る。";
    }
    QCString trPageIndex()
    {
      return "ページ索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return "覚え書き";
    }
    QCString trPublicTypes()
    {
      return "公開型";
    }
    QCString trPublicAttribs()
    {
      return "公開属性";
    }
    QCString trStaticPublicAttribs()
    {
      return "静的公開属性";
    }
    QCString trProtectedTypes()
    {
      return "保護型";
    }
    QCString trProtectedAttribs()
    {
      return "保護属性";
    }
    QCString trStaticProtectedAttribs()
    {
      return "静的保護属性";
    }
    QCString trPrivateTypes()
    {
      return "非公開型";
    }
    QCString trPrivateAttribs()
    {
      return "非公開属性";
    }
    QCString trStaticPrivateAttribs()
    {
      return "静的非公開属性";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo()
    {
      return "TODO";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList()
    {
      return "TODOリスト";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()
    {
      return "次で参照されています。";
    }
    QCString trRemarks()
    {
      return "意見";
    }
    QCString trAttention()
    {
      return "注意";
    }
    QCString trInclByDepGraph()
    {
	return "このグラフは、このファイルがどのファイルから直接、間接的に"
             "インクルードされているかを示しています。";
    }
    QCString trSince()
    {
      return "Since";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle()
    {
      return "グラフの凡例";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs()
    {
      return 
        "このページでは、doxygen で生成されたグラフをどのようにみたらよいかを"
        "説明します。<p>\n"
        "次の例を考えてみます。\n"
        "\\code\n"
        "/*! 省略されて見えないクラス */\n"
        "class Invisible { };\n\n"
        "/*! 省略されたクラス(継承関係は隠されている) */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* doxygen コメントによるドキュメントがないクラス */\n"
        "class Undocumented { };\n\n"
        "/*! public で継承されたクラス */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! protected で継承されたクラス */\n"
        "class ProtectedBase { };\n\n"
        "/*! private で継承されたクラス */\n"
        "class PrivateBase { };\n\n"
        "/*! 継承されたクラスで使われているクラス */\n"
        "class Used { };\n\n"
        "/*! 複数のクラスを継承している上位クラス */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "設定ファイル中で、タグ \\c MAX_DOT_GRAPH_HEIGHT が 200 にセットされた"
        "場合、次のようなグラフとなります。"
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "上のグラフ内のボックスには次のような意味があります。\n"
        "<ul>\n"
        "<li>黒く塗りつぶされたボックスは、このグラフに対応する構造体やクラスを"
        "表します。\n"
        "<li>黒枠のボックスはドキュメントがある構造体やクラスを表します。\n"
        "<li>灰色の枠のボックスはドキュメントがない構造体やクラスを表します。\n"
        "<li>赤枠のボックスはドキュメントがある構造体やクラスを表しますが、"
	  "指定されたサイズに収まらないために継承・包含関係をすべて図示する"
	  "ことができなかったことを示します。"
        "</ul>\n"
        "矢印には次のような意味があります。\n"
        "<ul>\n"
        "<li>青い矢印は二つのクラス間の public 継承関係を示します。\n"
        "<li>緑の矢印は protected 継承関係を示します。\n"
        "<li>赤の矢印は private 継承関係を示します。\n"
        "<li>紫の破線矢印は、そのクラスが他のクラスに含まれていたり、"
	  "利用されていることを示します。また、矢印が指しているクラスや構造体を"
	  "どの変数でアクセスできるかを矢印のラベルとして示しています。\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend()
    {
      return "凡例";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    QCString trTest()
    {
      return "テスト";
    }
    /*! Used as the header of the test list */
    QCString trTestList()
    {
      return "テストリスト";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    QCString trDCOPMethods()
    {
      return "DCOPメソッド";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties()
    {
      return "プロパティ";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation()
    {
      return "プロパティの解説";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "インターフェース";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return "クラス";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"パッケージ "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "パッケージリスト";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "簡易説明を持つパッケージです。";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "パッケージ";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "パッケージの解説";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "値:";
    }
};

#endif
