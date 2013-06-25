/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
 * translator_jp.h
 * Updates:
 * 1.2.5)
 * First Translation
 *      by Kenji Nagamatsu
 * 1.2.12)
 * Update and Shift-Jis(_WIN32)
 *      by Ryunosuke Sato (30-Dec-2001)
 * 1.5.8)
 * Translation for 1.5.8.
 *      by Hiroki Iseri (18-Feb-2009)
 */

#ifndef TRANSLATOR_JP_H
#define TRANSLATOR_JP_H

class TranslatorJapanese : public TranslatorAdapter_1_6_0
{
  public:
    virtual QCString idLanguage()
    { return "japanese"; }

    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "関連する関数"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "（これらはメソッドでないことに注意）"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "説明"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "型定義"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "列挙型"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    {
	  if( Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
	  {
		return "メソッド";
	  }
	  else
	  {
		return "関数";
	  }
	}

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if( Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
	    return "構造体";
	  }
      else
	  {
	    return "変数";
	  }
    }

    /*! this is the text of a link put after brief descriptions. */
	virtual QCString trMore()
    { return "[詳細]"; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "すべてのメンバ一覧"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "メンバ一覧"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "これは全メンバの一覧です。"; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return "継承メンバも含んでいます。"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result;
      if (s) result=(QCString)s+"の";
      result+="ソースから Doxygen により生成しました。";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "Enum"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "Enum 値"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "次で定義されています。"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "モジュール"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "クラス階層"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造";
	}
      else
	{
	  return "構成";
	}
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "ファイル一覧"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "ヘッダファイル"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データフィールド";
	}
      else
	{
	  return "構成メンバ";
	}
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "グローバル";
	}
      else
	{
	  return "ファイルメンバ";
	}
    }
    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "関連ページ"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "例"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "検索"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "この継承一覧はおおまかにはソートされていますが、"
             "完全にアルファベット順でソートされてはいません。";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool /*extractAll*/)
    {
      QCString result="これは";
      result+="ファイル一覧です。";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造の説明です。";
	}
      else
	{
	  return "クラス、構造体、共用体、インタフェースの説明です。";
	}
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="これは";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
	    result+="フィールドの一覧でそれぞれ";
	    if (extractAll) result+="が属している構造体/共用体";
	  }
      else
	  {
	    result+="クラスメンバの一覧で、それぞれ";
	    if (extractAll) result+="が属しているクラス";
	  }
      result+="の説明へリンクしています。";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool /*extractAll*/)
    {
      QCString result="これは";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  result+="関数、変数、マクロ、Enum、Typedef の";
	}
      else
	{
	  result+="ファイルメンバの";
	}
      result+="一覧です。それぞれが属しているファイルの説明へリンクしています。";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "APIを構成するヘッダファイルです。"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "すべての例の一覧です。"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "関連ページの一覧です。"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "すべてのモジュールの一覧です。"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given.
     */
    virtual QCString trNoDescriptionAvailable()
    { return "ドキュメントが記述されていません。"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "ドキュメント"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "モジュール索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "階層索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造索引";
	}
      else
	{
	  return "構成索引";
	}
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "ファイル索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "モジュール"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造";
	}
      else
	{
	  return "クラス";
	}
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "ファイル"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "例"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "ページ"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "リファレンスマニュアル"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "マクロ定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "型定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "列挙型"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "関数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "変数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
      { return "列挙型の値"; }
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "マクロ定義"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "型定義"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "列挙型"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "関数"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "変数"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造";
	}
      else
	{
	  return "構成";
	}
    }
    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result;
      if (projName) result+=(QCString)projName+"に対して";
      result+=(QCString)date+"に生成されました。";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"に対する継承グラフ";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "内部使用のみ。"; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "内部的な理由により再実装されましたが、APIには影響しません。";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "警告"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "バグと制限"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "バージョン"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "日付"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "戻り値"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "参照"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "引数"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "例外"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "作成："; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "ネームスペース一覧"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool /*extractAll*/)
    {
      return "ネームスペースの一覧です。";
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "フレンド"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "フレンドと関連する関数"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    {
      QCString result="";
      switch(compType)
      {
        case ClassDef::Class:      result+="クラス "; break;
        case ClassDef::Struct:     result+="構造体 "; break;
        case ClassDef::Union:      result+="共用体 "; break;
        case ClassDef::Interface:  result+="インタフェース "; break;
        case ClassDef::Protocol:   result+="プロトコル "; break;
        case ClassDef::Category:   result+="カテゴリ "; break;
        case ClassDef::Exception:  result+="例外 "; break;
        default: break;
      }
      if (isTemplate) result+="テンプレート ";
      result+=(QCString)clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=""+(QCString)fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result="ネームスペース "+(QCString)namespaceName;
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    virtual QCString trPublicMembers()
    { return "Public メソッド"; }
    virtual QCString trPublicSlots()
    { return "Public スロット"; }
    virtual QCString trSignals()
    { return "シグナル"; }
    virtual QCString trStaticPublicMembers()
    { return "Static Public メソッド"; }
    virtual QCString trProtectedMembers()
    { return "Protected メソッド"; }
    virtual QCString trProtectedSlots()
    { return "Protected スロット"; }
    virtual QCString trStaticProtectedMembers()
    { return "Static Protected メソッド"; }
    virtual QCString trPrivateMembers()
    { return "Private メソッド"; }
    virtual QCString trPrivateSlots()
    { return "Private スロット"; }
    virtual QCString trStaticPrivateMembers()
    { return "Static Private メソッド"; }

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
            result+=", と ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return trWriteList(numEntries)+"を継承しています。";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return trWriteList(numEntries)+"に継承されています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"を再定義しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"で再定義されています。";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "ネームスペースメンバ"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
	QCString result="これは";
      result+="ネームスペースの一覧です。それぞれ";
      if (extractAll)
	  result+="のネームスペース";
      else
	  result+="が属しているネームスペース";
      result+="へリンクしています。";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "ネームスペース索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "ネームスペース"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "ネームスペース"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
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
        case ClassDef::Protocol:   result+="プロトコル"; break;
        case ClassDef::Category:   result+="カテゴリ"; break;
        case ClassDef::Exception:  result+="例外"; break;
        default: break;
      }
      result+="の説明は次のファイルから生成されました:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "戻り値"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "メインページ"; }

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
      return "ソース";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return " @1 の @0 行で定義されています。";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return " @0 で定義されています。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "非推奨";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+"のコラボレーション図";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
	return (QCString)fName+"のインクルード依存関係図";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "コンストラクタとデストラクタ";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "ソースコードを見る。";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "説明を見る。";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "事前条件";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "事後条件";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "不変";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "初期値:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "コード";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "クラス階層図";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "クラス階層図を見る。";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "クラス階層図を見る。";
    }
    virtual QCString trPageIndex()
    {
      return "ページ索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "覚え書き";
    }
    virtual QCString trPublicTypes()
    {
      return "Public 型";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "変数";
	}
      else
	{
	  return "Public 変数";
	}
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Static Public 変数";
    }
    virtual QCString trProtectedTypes()
    {
      return "Protected 型";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Protected 変数";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Static Protected 変数";
    }
    virtual QCString trPrivateTypes()
    {
      return "Private 型";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Private 変数";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Static Private 変数";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "TODO";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "TODO一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "参照元";
    }
    virtual QCString trRemarks()
    {
      return "意見";
    }
    virtual QCString trAttention()
    {
      return "注意";
    }
    virtual QCString trInclByDepGraph()
    {
	return "このグラフは、どのファイルから直接、間接的に"
               "インクルードされているかを示しています。";
    }
    virtual QCString trSince()
    {
      return "から";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "グラフの凡例";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
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
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
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
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "設定ファイル中で、タグ \\c MAX_DOT_GRAPH_HEIGHT が 200 にセットされた"
        "場合、次のようなグラフとなります。"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "凡例";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "テスト";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "テスト一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "プロパティ";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "プロパティ";
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	{
	  return "データ構造";
	}
      else
	{
	  return "クラス";
	}
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"パッケージ "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "パッケージ一覧";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "これはパッケージ一覧です。";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "パッケージ";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "パッケージ";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "値:";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "バグ";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "バグ一覧";
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
      return "932";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "128";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "索引";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool /*first_capital*/, bool /*singular*/)
    {
      return "クラス";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool /*singular*/)
    {
      return "ファイル";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool /*first_capital*/, bool /*singular*/)
    {
      return "ネームスペース";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool /*first_capital*/, bool /*singular*/)
    {
      return "グループ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool /*first_capital*/, bool /*singular*/)
    {
      return "ページ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool /*first_capital*/, bool /*singular*/)
    {
      return "メンバ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trField(bool /*first_capital*/, bool /*singular*/)
    {
      return "フィールド";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool /*first_capital*/, bool /*singular*/)
    {
      return "グローバル";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool /*first_capital*/, bool /*singular*/)
    {
      return "作者";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "参照先";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"を実装しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"で実装されています。";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "目次";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "非推奨一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "イベント";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "イベント";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "パッケージ内の型定義";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "関数";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "スタティック関数";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "変数";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "スタティック変数";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "全て";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "関数の呼び出しグラフ:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "検索結果";
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
        return "入力された条件にマッチするドキュメントがありませんでした.";
      }
      else if (numDocuments==1)
      {
        return "入力された条件にマッチするドキュメントが <b>1</b> 件みつかりました.";
      }
      else
      {
        return "入力された条件にマッチするドキュメントが <b>$num</b> 件みつかりました. "
               "最も一致しているものから表示されます.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "マッチした単語:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " ソースファイル";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "ディレクトリ索引"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "ディレクトリ構成"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "ディレクトリ"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "このディレクトリ一覧はおおまかにはソートされていますが、"
             "完全にアルファベット順でソートされてはいません。";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" ディレクトリリファレンス"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool)
    {
      return "ディレクトリ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "これはオーバーロードされたメンバ関数です。"
              "利便性のために用意されています。"
              "元の関数との違いは引き数のみです。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      // return "Here is the caller graph for this function:";
      return "呼出しグラフ:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "列挙型"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "関数/サブルーチン"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "データ型"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "データフィールド"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "これはデータ型の一覧です"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
	  QCString result="これは";
	  result+="フィールドの一覧です。それぞれ";
	  if (extractAll)
	  {
	  	result+="が属しているデータ型";
	  }
	  result+="の説明へリンクしています。";
	  return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "データ型索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "データ型"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "関数/サブルーチン"; }


    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "関数/サブルーチン"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "データ型"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "モジュール一覧"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="これは";
      if (!extractAll) result+="生成された";
      result+="モジュール一覧です";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result="";
      switch(compType)
      {
        case ClassDef::Class:      result+="モジュール "; break;
        case ClassDef::Struct:     result+="TYPE "; break;
        case ClassDef::Union:      result+="共用体 "; break;
        case ClassDef::Interface:  result+="インターフェース "; break;
        case ClassDef::Protocol:   result+="プロトコル "; break;
        case ClassDef::Category:   result+="カテゴリ "; break;
        case ClassDef::Exception:  result+="例外 "; break;
        default: break;
      }
      if (isTemplate) result += "テンプレート ";
      result+=(QCString)clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+="モジュール";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "モジュールメンバ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result="これはモジュールメンバ一覧です。それぞれ ";
      if (extractAll)
      {
        result+="属しているモジュール";
      }
      result+="の説明へリンクしています。";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "モジュール索引"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool /*first_capital*/, bool /*singular*/)
    {
      return "モジュール";
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool /*single*/)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="";
      switch(compType)
      {
        case ClassDef::Class:      result+="モジュール"; break;
        case ClassDef::Struct:     result+="TYPE"; break;
        case ClassDef::Union:      result+="共用体"; break;
        case ClassDef::Interface:  result+="インターフェース"; break;
        case ClassDef::Protocol:   result+="プロトコル"; break;
        case ClassDef::Category:   result+="カテゴリ"; break;
        case ClassDef::Exception:  result+="例外"; break;
        default: break;
      }
      result+="の説明は次のファイルから生成されました:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool /*first_capital*/, bool /*singular*/)
    {
      QCString result = "TYPE";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool /*first_capital*/, bool /*singular*/)
    {
      QCString result = "サブプログラム";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "型制約";
    }

};

#endif
