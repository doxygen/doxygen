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

/*
 * translator_jp.h
 * Updates:
 * 1.2.5)
 * First Translation
 *      by Kenji Nagamatsu
 * 1.2.12)
 * Update and Shift-Jis(_WIN32) -> converted UTF-8 at version 1.8.5
 *      by Ryunosuke Sato (30-Dec-2001)
 * 1.5.8)
 * Translation for 1.5.8.
 *      by Hiroki Iseri (18-Feb-2009)
 * 1.8.5)
 * Translation Added for 1.8.4 and revised
 *      by Suzumizaki-Kimitaka (30-Aug-2013)
 * 1.18.0)
 * Added 76 missing translation methods from releases 1.8.15 to 1.16.0
 *      by GitHub Copilot (09-Jun-2026)
 */
/*
Messages for translators written in Japanese:
1.8.5 への追加にあたって過去の翻訳者三名への連絡を試みたところ、
井芹さん(Hiroki Iseri)さんからメールのお返事をいただけました。
その際教えていただいた過去の経緯によりますと当時連絡可能だった方々は
揃って従来訳から改変追加して構わない旨を表明されていたとのことです。
Doxygen の開発の方でもそれはそれでいーんじゃん？みたいな感じだったようで。

井芹さんも同様の見解で、私(鈴見咲=Suzumizaki-Kimitaka)も
今後この翻訳に関わり続けられるかは非常に怪しいところですので
将来の追加訳・既存訳改良は臆することなく進めていってよいのでは
ないかと思います。無論作業の衝突があるのは不経済ですので現在進行形で
活発に更新している方がいないかの簡単な確認(MLとかGitとか)をやるのも
いいでしょうし、それでも偶然衝突したら不運を諦めて相互に調整しましょう。

当面なさそうですが訳語の選択で喧嘩になることもあるかもしれません。
そのときは gettext を利用するようなパッチを作って doxygen の開発に
適用を求めるのが一番ではないかなと思います。

1.6.0以前の既存の訳についても多少弄りました。
特に structure を構造体ではなく構成としていたのはあんまりでしたので。
ほか、C++ での利用前提で改変したところもありますが、それが他の言語で
問題のようでしたらお手数掛けて申し訳ないですが相応に再修正しちゃって
構いません。

その際 doc/maintainers.txt を修正してから python doc/translator.py を
実行する点にご注意下さい。私のところに search 鈴見咲君高 と書いたのは
同姓同名がまず考えられないというのが大前提ですのでこちらもご注意。

"詳解"の語が厳しすぎると思う向きはありましょうが、その程度には書けと。
明記されてないけど使われてる動作や戻り値が想定内なのか想定外なのか
わからんのはメンテで困るじゃないですか。

(2013-08-30, 鈴見咲君高)
*/
#ifndef TRANSLATOR_JP_H
#define TRANSLATOR_JP_H

class TranslatorJapanese : public Translator
{
  public:
    DString idLanguage() override
    { return "japanese"; }

    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage{CJKutf8}\n";
    }
    DString trISOLang() override
    {
      return "ja";
    }
    DString getLanguageString() override
    {
      return "0x411 Japanese";
    }
    DString latexFontenc() override
    {
      return "";
    }
    DString latexDocumentPre() override
    {
      return "\\begin{CJK}{UTF8}{min}\n";
    }
    DString latexDocumentPost() override
    {
      return "\\end{CJK}\n";
    }
    bool needsPunctuation() override
    {
      return false;
    }

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "関連関数"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "（これらはメソッドではありません）"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "詳解"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "詳細"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "型定義メンバ詳解"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "列挙型メンバ詳解"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    {
      if( Config_getBool(OPTIMIZE_OUTPUT_JAVA))
      {
        return "メソッド詳解";
      }
      else
      {
        return "関数詳解";
      }
    }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if( Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "フィールド詳解";
      }
      else
      {
        return "メンバ詳解";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "[詳解]"; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "全メンバ一覧"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "メンバ一覧"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "継承メンバを含む"; }
    /* trIncludingInheritedMembers に続くように定義すること */

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return " の全メンバ一覧です。"; }
    /* trThisIsTheListOfAllMembers から続くように定義すること */

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result = "Doxygen により";
      if (!s.empty()) result=" "+s+"の";
      result+="ソースコードから抽出しました。";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "列挙名"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "列挙値"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "定義場所: "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "モジュール"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "クラス階層"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データ構造";
      }
      else
      {
        return "クラス一覧";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "ファイル一覧"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データフィールド";
      }
      else
      {
        return "クラスメンバ";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "大域各種";
      }
      else
      {
        return "ファイルメンバ";
      }
    }
    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "諸情報"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "各種例"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "検索"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    {
      return "クラス階層一覧です。大雑把に文字符号順で並べられています。";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      /* 概要がついているのは見ればわかるので省略 */
      /* extractAll こと EXTRACT_ALL はすべての詳解が存在することを
         実際の有無を度外視してユーザーが保証する設定なので
         詳解がなければこの関数が返す文字列は当然に矛盾を起こす。
      */
      if (extractAll)
      {
        return "ファイル一覧です。";
      }
      return "詳解が付けられているファイルの一覧です。";
    }


    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {
      /* 概要がついているのは見ればわかるので省略 */
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データ構造一覧です。";
      }
      else
      {
        return "クラス・構造体・共用体・インターフェースの一覧です。";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      const bool forC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
      DString result;
      if (forC)
      {
        result = "構造体・共用体の";
      }
      if (extractAll)
      {
        result += "全";
      }
      else
      {
        result += "詳解あり";
      }
      if (forC)
      {
        result += "フィールド";
      }
      else
      {
        result += "クラスメンバ";
      }
      if (!extractAll && !forC)
      {
        result += "の";
      }
      result += "一覧です。";
      if (!extractAll)
      {
        if (forC)
        {
          result+="各フィールド詳解";
        }
        else
        {
          result+="各クラスメンバ詳解";
        }
      }
      else
      {
        if (forC)
        {
          result+="各フィールドが属する構造体・共用体";
        }
        else
        {
          result+="各メンバが属するクラス";
        }
      }
      result += "へのリンクがあります。";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool /*extractAll*/) override
    {
      DString result;
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="関数・変数・マクロ・列挙・型定義";
      }
      else
      {
        result+="ファイル直下のメンバ";
      }
      result+="一覧です。各々詳解があればそこへリンクしています。";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "各種例の一覧です。"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "諸情報の一覧です。"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "全モジュールの一覧です。"; }

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "詳解"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "モジュール索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "階層索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データ構造索引";
      }
      else
      {
        return "クラス索引";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "ファイル索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "モジュール詳解"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データ構造詳解";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "クラス詳解";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "ファイル詳解"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "リファレンスマニュアル"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "マクロ定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "型定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "列挙型"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "関数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "変数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "列挙値"; }
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "マクロ定義詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "型定義詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "列挙型詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "関数詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "変数詳解"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "データ構造";
      }
      else
      {
        return "クラス";
      }
    }
    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result = date+"作成";
      if (!projName.empty()) result+=" - " + projName;
      result+=" / 構成: ";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return clName+" の継承関係図";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "警告"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "バージョン"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "日付"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "戻り値"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "参照"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "引数"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "例外"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "構築:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "名前空間一覧"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      if (extractAll)
      {
        return "全名前空間の一覧です。";
      }
      return "詳解が付いた名前空間の一覧です。";
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "フレンド"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "フレンドと関連関数の詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      DString result=clName+" ";
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
      if (isTemplate) result+="テンプレート";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName+" ファイル";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName+" 名前空間";
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    DString trPublicMembers() override
    { return "公開メンバ関数"; }
    DString trPublicSlots() override
    { return "公開スロット"; }
    DString trSignals() override
    { return "シグナル"; }
    DString trStaticPublicMembers() override
    { return "静的公開メンバ関数"; }
    DString trProtectedMembers() override
    { return "限定公開メンバ関数"; }
    DString trProtectedSlots() override
    { return "限定公開スロット"; }
    DString trStaticProtectedMembers() override
    { return "静的限定公開メンバ関数"; }
    DString trPrivateMembers() override
    { return "非公開メンバ関数"; }
    DString trPrivateSlots() override
    { return "非公開スロット"; }
    DString trStaticPrivateMembers() override
    { return "静的非公開メンバ関数"; }

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
          result+=", ";
        }
      }
      if ( result.length() > 60 )
      {
        DString countStr;
        countStr.sprintf(" (計%d項目)", numEntries);
        result += countStr;
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return trWriteList(numEntries)+"を継承しています。";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return trWriteList(numEntries)+"に継承されています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"を再実装しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"で再実装されています。";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "名前空間メンバ"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="これは";
      result+="名前空間の一覧です。それぞれ";
      if (extractAll)
      {
        result+="の名前空間";
      }
      else
      {
        result+="が属している名前空間";
      }
      result+="へリンクしています。";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "名前空間索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "名前空間詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "名前空間"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool /*single*/) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="この";
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
      result+="詳解は次のファイルから抽出されました:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "戻り値"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "総合概要"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return " @1 の @0 行目に定義があります。";
    }
    DString trDefinedInSourceFile() override
    {
      return " @0 に定義があります。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "非推奨";
    }

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return clName+" 連携図";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
    return fName+" の依存先関係図:";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "構築子と解体子";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "[ソースコード]";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "[詳解]";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "事前条件";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "事後条件";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "不変";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "初期値:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "コード";
    }
    DString trGraphicalHierarchy() override
    {
      return "クラス階層図";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "[クラス階層図]";
    }
    DString trGotoTextualHierarchy() override
    {
      return "[クラス階層表]";
    }
    DString trPageIndex() override
    {
      return "ページ索引";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "覚え書き";
    }
    DString trPublicTypes() override
    {
      return "公開型";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "フィールド";
      }
      else
      {
        return "公開変数類";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "静的公開変数類";
    }
    DString trProtectedTypes() override
    {
      return "限定公開型";
    }
    DString trProtectedAttribs() override
    {
      return "限定公開変数類";
    }
    DString trStaticProtectedAttribs() override
    {
      return "静的限定公開変数類";
    }
    DString trPrivateTypes() override
    {
      return "非公開型";
    }
    DString trPrivateAttribs() override
    {
      return "非公開変数類";
    }
    DString trStaticPrivateAttribs() override
    {
      return "静的非公開変数類";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    {
      return "todo";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "todo一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "参照元";
    }
    DString trRemarks() override
    {
      return "注釈";
    }
    DString trAttention() override
    {
      return "注意";
    }
    DString trInclByDepGraph() override
    {
      return "被依存関係図:";
    }
    DString trSince() override
    {
      return "から";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "グラフの凡例";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Doxygen が生成したグラフを読み方について。<p>\n"
        "次のコード例をご覧ください。\n"
        "\\code\n"
        "/*! 全体の大きさの関係で見えなくなるクラスです。 */\n"
        "class Invisible { };\n\n"
        "/*! 表示を切り捨てられたクラス(Invisibleクラスの分が見えません) */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Doxygen 用のコメントコードがないクラス */\n"
        "class Undocumented { };\n\n"
        "/*! 公開継承されているクラス */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! 限定公開で継承されているクラス */\n"
        "class ProtectedBase { };\n\n"
        "/*! 非公開継承されているクラス */\n"
        "class PrivateBase { };\n\n"
        "/*! Inherited クラス内で使われているクラス */\n"
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
        "\\c MAX_DOT_GRAPH_" /* わざわざちょん切っているのは doc/translator.py の検出回避のため */
        "HEIGHT タグに 200 を与えた設定ファイル"
        "を使うと、次のようなグラフとなります。"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "グラフ内の矩形は構造体やクラスを表しています。色の意味は次の通りです。\n"
        "<ul>\n"
        "<li>中を黒く塗られた四角は、図が注目している起点です。</li>\n"
        "<li>黒枠は詳解があることを示しています。</li>\n"
        "<li>灰色枠で示されたクラス等には詳解がありません。</li>\n"
        "<li>赤枠で示されたものは詳解を持つクラスですが、"
        "指定された大きさに収まらないことから一部の継承・包含関係が"
        "省略されていることを表します。</li>\n"
        "</ul>\n"
        "<p>矢印の意味は次の通りです。</p>\n"
        "<ul>\n"
        "<li>青い矢印は二つのクラス間の公開継承関係を示します。</li>\n"
        "<li>緑の矢印は限定公開の継承関係を示します。</li>\n"
        "<li>赤の矢印は非公開の継承関係を示します。</li>\n"
        "<li>紫の破線矢印は、そのクラスが他のクラスに含まれているか、"
      "利用されていることを示します。また、矢印のラベルは矢の先にあるクラス等を"
      "アクセスしている矢の根本のメンバを表しています。</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "凡例";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "テスト";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "テスト一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "プロパティ";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "プロパティ詳解";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
    {
      return "データ構造";
    }
      else
    {
      return "クラス";
    }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return name+" パッケージ";
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "パッケージ一覧です。";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "パッケージ";
    }

	    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "値:";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "バグ";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
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
    DString trRTFansicp() override
    {
      return "932";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "128";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "索引";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool /*first_capital*/, bool /*singular*/) override
    {
      return "クラス";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool /*first_capital*/, bool /*singular*/) override
    {
      return "ファイル";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool /*first_capital*/, bool /*singular*/) override
    {
      return "名前空間";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool /*first_capital*/, bool /*singular*/) override
    {
      return "グループ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool /*first_capital*/, bool /*singular*/) override
    {
      return "ページ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool /*first_capital*/, bool /*singular*/) override
    {
      return "メンバ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool /*first_capital*/, bool /*singular*/) override
    {
      return "大域各種";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool /*first_capital*/, bool /*singular*/) override
    {
      return "著者";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "参照先";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"を実装しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"で実装されています。";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "目次";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "非推奨一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "イベント";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "イベント詳解";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "パッケージ内の型定義";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "関数";
    }
    DString trPackageMembers() override
    {
      return "パッケージ内のメンバ";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "静的関数";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "変数";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "静的変数";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "全て";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "呼び出し関係図:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
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
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "入力条件を満たす文書がありません。";
      }
      else if (numDocuments==1)
      {
        return "入力条件を満たす文書が <b>1</b> 件ありました.";
      }
      else
      {
        return "入力条件を満たす文書が <b>$num</b> 件ありました. "
               "一致度の高いものから表示されます.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "照合語:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " ソースファイル";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "ディレクトリ索引"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "ディレクトリ詳解"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "ディレクトリ"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" ディレクトリリファレンス"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool, bool) override
    {
      return "ディレクトリ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "これはオーバーロードされたメンバ関数です。"
              "利便性のために用意されています。"
              "元の関数との違いは引き数のみです。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "被呼び出し関係図:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "列挙型詳解"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "メンバ関数/サブルーチン詳解"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "データ型一覧"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "データフィールド"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "これはデータ型の一覧です:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="これは";
      result+="フィールドの一覧です。それぞれ";
      if (extractAll)
      {
          result+="が属しているデータ型";
      }
      result+="の詳解へリンクしています。";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "データ型索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "データ型詳解"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "関数/サブルーチン"; }


    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "関数/サブルーチン詳解"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "データ型"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "モジュール一覧"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result;
      if (!extractAll)
      {
        result+="詳解が記されている";
      }
      else
      {
        result+="全";
      }
      result+="モジュールの一覧です";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result="";
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
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+="モジュール";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "モジュールメンバ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="これはモジュールメンバ一覧です。それぞれ ";
      if (extractAll)
      {
        result+="属しているモジュール";
      }
      result+="の詳解へリンクしています。";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "モジュール索引"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool /*first_capital*/, bool /*singular*/) override
    {
      return "モジュール";
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool /*single*/) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="次のファイルから";
      switch(compType)
      {
        case ClassDef::Class:      result+="モジュール"; break;
        case ClassDef::Struct:     result+="型"; break;
        case ClassDef::Union:      result+="共用体"; break;
        case ClassDef::Interface:  result+="インターフェース"; break;
        case ClassDef::Protocol:   result+="プロトコル"; break;
        case ClassDef::Category:   result+="カテゴリ"; break;
        case ClassDef::Exception:  result+="例外"; break;
        default: break;
      }
      result+="の詳解が抽出されました:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool /*first_capital*/, bool /*singular*/) override
    {
      DString result = "型";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool /*first_capital*/, bool /*singular*/) override
    {
      DString result = "サブプログラム";
      return result;
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "型制約";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name+" 関係";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "読み取り中…";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "大域名前空間";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "検索中…";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "一致する文字列を見つけられません";
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
      return name+"にあるファイル";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return name+"にあるファイルを include している";
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
      static const char *days[]   = { "月", "火", "水", "木", "金", "土", "日" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%.4d年%.2d月%.2d日(%s)",year,month,day,days[dayOfWeek-1]);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d時%.2d分%.2d秒",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "月", "火", "水", "木", "金", "土", "日" };
      static const char *days_full[]    = { "月曜日", "火曜日", "水曜日", "木曜日", "金曜日", "土曜日", "日曜日" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    DString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
      static const char *months_full[]  = { "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "午前", "午後" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "書誌参照"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "著作権所有"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return name+" のディレクトリ依存関係図"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "表示階層"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "テンプレート引数"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "ほか "+number+" 件…"; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool) override
    {
      return "次のファイルからこの列挙についての詳解を抽出しました:";
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return DString("列挙 ")+name+" 詳解"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return DString("基底クラス ")+what+" に属する継承"+members; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "その他の継承メンバ"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {

      DString opt = enable ? "有効" : "無効";
      return "クリックで同期表示が"+opt+"になります";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "@0 カテゴリーから提供されています。";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "@0 を拡張しています。";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "クラスメソッド";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "実体メソッド";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "メソッド詳解";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "実装されたインターフォース"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "継承されたサービス"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "定数グループ"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" 定数グループ詳解";
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" サービス詳解";
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Singleton 詳解";
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool /*single*/) override
    {
      // single is true implies a single file
      return "次のファイルからこのサービスについて"
             "の詳解を抽出しました:";
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool /*single*/) override
    {
      // single is true implies a single file
      return "次のファイルからこの Singleton について"
             "の詳解を抽出しました:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "デザインユニット階層"; }

    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "デザインユニット一覧"; }

    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "デザインユニットメンバ"; }

    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
      return "エンティティへのリンクを持つ全デザインユニットメンバの一覧です:";
    }

    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "デザインユニット索引"; }

    /** VHDL design units */
    DString trDesignUnits() override
    { return "デザインユニット"; }

    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "関数/プロシージャ/プロセス"; }

    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type, bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          return "ライブラリ";
        case VhdlSpecifier::PACKAGE:
          return "パッケージ";
        case VhdlSpecifier::SIGNAL:
          return "シグナル";
        case VhdlSpecifier::COMPONENT:
          return "コンポーネント";
        case VhdlSpecifier::CONSTANT:
          return "定数";
        case VhdlSpecifier::ENTITY:
          return "エンティティ";
        case VhdlSpecifier::TYPE:
          return "型";
        case VhdlSpecifier::SUBTYPE:
          return "サブタイプ";
        case VhdlSpecifier::FUNCTION:
          return "関数";
        case VhdlSpecifier::RECORD:
          return "レコード";
        case VhdlSpecifier::PROCEDURE:
          return "プロシージャ";
        case VhdlSpecifier::ARCHITECTURE:
          return "アーキテクチャ";
        case VhdlSpecifier::ATTRIBUTE:
          return "属性";
        case VhdlSpecifier::PROCESS:
          return "プロセス";
        case VhdlSpecifier::PORT:
          return "ポート";
        case VhdlSpecifier::USE:
          return "使用節";
        case VhdlSpecifier::GENERIC:
          return "ジェネリック";
        case VhdlSpecifier::PACKAGE_BODY:
          return "パッケージボディ";
        case VhdlSpecifier::UNITS:
          return "ユニット";
        case VhdlSpecifier::SHAREDVARIABLE:
          return "共有変数";
        case VhdlSpecifier::VFILE:
          return "ファイル";
        case VhdlSpecifier::GROUP:
          return "グループ";
        case VhdlSpecifier::INSTANTIATION:
          return "インスタンス化";
        case VhdlSpecifier::ALIAS:
          return "エイリアス";
        case VhdlSpecifier::CONFIG:
          return "コンフィギュレーション";
        case VhdlSpecifier::MISCELLANEOUS:
          return "その他";
        case VhdlSpecifier::UCF_CONST:
          return "制約";
        default:
          return "クラス";
      }
    }

    DString trCustomReference(const DString &name) override
    { return name+" 詳解"; }

    /* Slice */
    DString trConstants() override
    { return "定数"; }

    DString trConstantDocumentation() override
    { return "定数詳解"; }

    DString trSequences() override
    { return "シーケンス"; }

    DString trSequenceDocumentation() override
    { return "シーケンス詳解"; }

    DString trDictionaries() override
    { return "辞書"; }

    DString trDictionaryDocumentation() override
    { return "辞書詳解"; }

    DString trSliceInterfaces() override
    { return "インタフェース"; }

    DString trInterfaceIndex() override
    { return "インタフェース索引"; }

    DString trInterfaceList() override
    { return "インタフェース一覧"; }

    DString trInterfaceListDescription() override
    { return "概要付きインタフェースの一覧です:"; }

    DString trInterfaceHierarchy() override
    { return "インタフェース階層"; }

    DString trInterfaceHierarchyDescription() override
    { return "クラス階層一覧です。大雑把に文字符号順で並べられています。"; }

    DString trInterfaceDocumentation() override
    { return "インタフェース詳解"; }

    DString trStructs() override
    { return "構造体"; }

    DString trStructIndex() override
    { return "構造体索引"; }

    DString trStructList() override
    { return "構造体一覧"; }

    DString trStructListDescription() override
    { return "概要付き構造体の一覧です:"; }

    DString trStructDocumentation() override
    { return "構造体詳解"; }

    DString trExceptionIndex() override
    { return "例外索引"; }

    DString trExceptionList() override
    { return "例外一覧"; }

    DString trExceptionListDescription() override
    { return "概要付き例外の一覧です:"; }

    DString trExceptionHierarchy() override
    { return "例外階層"; }

    DString trExceptionHierarchyDescription() override
    { return "例外階層一覧です。大雑把に文字符号順で並べられています。"; }

    DString trExceptionDocumentation() override
    { return "例外詳解"; }

    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result=clName;
      if (isLocal) result+=" ローカル";
      switch(compType)
      {
        case ClassDef::Class:      result+=" クラス"; break;
        case ClassDef::Struct:     result+=" 構造体"; break;
        case ClassDef::Union:      result+=" 共用体"; break;
        case ClassDef::Interface:  result+=" インタフェース"; break;
        case ClassDef::Protocol:   result+=" プロトコル"; break;
        case ClassDef::Category:   result+=" カテゴリ"; break;
        case ClassDef::Exception:  result+=" 例外"; break;
        default: break;
      }
      result+=" 詳解";
      return result;
    }

    DString trOperations() override
    { return "操作"; }

    DString trOperationDocumentation() override
    { return "操作詳解"; }

    DString trDataMembers() override
    { return "データメンバ"; }

    DString trDataMemberDocumentation() override
    { return "データメンバ詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    DString trDesignUnitDocumentation() override
    { return "デザインユニット詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    DString trConcept(bool /*first_capital*/, bool /*singular*/) override
    { return "コンセプト"; }

    /*! used as the title of the HTML page of a C++20 concept page */
    DString trConceptReference(const DString &conceptName) override
    {
      DString result=conceptName;
      result+=" コンセプト詳解";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    DString trConceptList() override
    { return "コンセプト一覧"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    DString trConceptIndex() override
    { return "コンセプト索引"; }

    /*! used as the title of chapter containing all information about concepts. */
    DString trConceptDocumentation() override
    { return "コンセプト詳解"; }

    /*! used as an introduction to the concept list */
    DString trConceptListDescription(bool extractAll) override
    {
      if (extractAll)
      {
        return "全コンセプトの一覧です。";
      }
      return "詳解が付けられているコンセプトの一覧です。";
    }

    /*! used to introduce the definition of the C++20 concept */
    DString trConceptDefinition() override
    { return "コンセプトの定義"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    DString trPackageList() override
    { return "パッケージ一覧"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
    { return "フローチャート:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////

    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    DString trRelatedSymbols() override
    { return "関連シンボル"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    DString trRelatedSymbolsSubscript() override
    { return "（これらはメンバシンボルではありません）"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    DString trRelatedSymbolDocumentation() override
    { return "フレンドと関連シンボルの詳解"; }

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
        case ClassDef::Struct:     result="構造体"; break;
        case ClassDef::Union:      result="共用体"; break;
        case ClassDef::Interface:  result="インタフェース"; break;
        case ClassDef::Protocol:   result="プロトコル"; break;
        case ClassDef::Category:   result="カテゴリ"; break;
        case ClassDef::Exception:  result="例外"; break;
        case ClassDef::Service:    result="サービス"; break;
        case ClassDef::Singleton:  result="シングルトン"; break;
        default: break;
      }
      return result;
    }

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result;
      if (extractAll) result += "全";
      else result += "詳解あり";
      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result += "関数・変数・マクロ・列挙・型定義";
          else
            result += "ファイルメンバ";
          break;
        case FileMemberHighlight::Functions:
          result += "関数";
          break;
        case FileMemberHighlight::Variables:
          result += "変数";
          break;
        case FileMemberHighlight::Typedefs:
          result += "型定義";
          break;
        case FileMemberHighlight::Sequences:
          result += "シーケンス";
          break;
        case FileMemberHighlight::Dictionaries:
          result += "辞書";
          break;
        case FileMemberHighlight::Enums:
          result += "列挙型";
          break;
        case FileMemberHighlight::EnumValues:
          result += "列挙値";
          break;
        case FileMemberHighlight::Defines:
          result += "マクロ";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result += "の一覧です。";
      if (extractAll)
        result += "各々が属するファイルへのリンクがあります。";
      else
        result += "各詳解へのリンクがあります。";
      return result;
    }

    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result;
      if (extractAll) result += "全";
      else result += "詳解あり";
      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result += "構造体・共用体フィールド";
          else
            result += "クラスメンバ";
          break;
        case ClassMemberHighlight::Functions:
          result += "関数";
          break;
        case ClassMemberHighlight::Variables:
          result += "変数";
          break;
        case ClassMemberHighlight::Typedefs:
          result += "型定義";
          break;
        case ClassMemberHighlight::Enums:
          result += "列挙型";
          break;
        case ClassMemberHighlight::EnumValues:
          result += "列挙値";
          break;
        case ClassMemberHighlight::Properties:
          result += "プロパティ";
          break;
        case ClassMemberHighlight::Events:
          result += "イベント";
          break;
        case ClassMemberHighlight::Related:
          result += "関連シンボル";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result += "の一覧です。";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result += "各フィールドが属する構造体・共用体詳解へのリンクがあります。";
        else
          result += "各メンバが属するクラス詳解へのリンクがあります。";
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result += "各フィールドが属する構造体・共用体へのリンクがあります。";
        else
          result += "各メンバが属するクラスへのリンクがあります。";
      }
      return result;
    }

    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result;
      if (extractAll) result += "全";
      else result += "詳解あり";
      result += "名前空間";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          result += "メンバ";
          break;
        case NamespaceMemberHighlight::Functions:
          result += "関数";
          break;
        case NamespaceMemberHighlight::Variables:
          result += "変数";
          break;
        case NamespaceMemberHighlight::Typedefs:
          result += "型定義";
          break;
        case NamespaceMemberHighlight::Sequences:
          result += "シーケンス";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          result += "辞書";
          break;
        case NamespaceMemberHighlight::Enums:
          result += "列挙型";
          break;
        case NamespaceMemberHighlight::EnumValues:
          result += "列挙値";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result += "の一覧です。";
      if (extractAll)
        result += "各名前空間メンバの所属名前空間へのリンクがあります。";
      else
        result += "各メンバ詳解へのリンクがあります。";
      return result;
    }

    DString trDefinition() override
    { return "定義"; }

    DString trDeclaration() override
    { return "宣言"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    DString trTopics() override
    { return "トピック"; }

    DString trTopicDocumentation() override
    { return "トピック詳解"; }

    DString trTopicList() override
    { return "トピック一覧"; }

    DString trTopicIndex() override
    { return "トピック索引"; }

    DString trTopicListDescription() override
    { return "概要付きトピックの一覧です:"; }

    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result;
      if (extractAll) result += "全";
      else result += "詳解あり";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          result += "モジュールメンバ";
          break;
        case ModuleMemberHighlight::Functions:
          result += "関数";
          break;
        case ModuleMemberHighlight::Variables:
          result += "変数";
          break;
        case ModuleMemberHighlight::Typedefs:
          result += "型定義";
          break;
        case ModuleMemberHighlight::Enums:
          result += "列挙型";
          break;
        case ModuleMemberHighlight::EnumValues:
          result += "列挙値";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result += "の一覧です。";
      if (extractAll)
        result += "各モジュールメンバの所属モジュールへのリンクがあります。";
      else
        result += "各メンバ詳解へのリンクがあります。";
      return result;
    }

    DString trExportedModules() override
    { return "エクスポートされたモジュール"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    DString trCopyToClipboard() override
    { return "クリップボードにコピー"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////

    DString trImportant() override
    { return "重要"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    DString trRequirements() override
    { return "要件"; }

    // table header for the column with the requirements IDs
    DString trRequirementID() override
    { return "ID"; }

    // indicates a symbol implements (satisfies) a requirement
    DString trSatisfies(bool /*singular*/) override
    { return "実装する要件"; }

    // indicates a requirement is satisfied (implemented) by one or more symbols
    DString trSatisfiedBy(const DString &list) override
    { return list+" によって実装されています。"; }

    DString trUnsatisfiedRequirements() override
    { return "未実装の要件"; }

    DString trUnsatisfiedRequirementsText(bool /*singular*/, const DString &list) override
    { return "要件 "+list+" は '実装' 関係を持っていません。"; }

    // indicates a symbol verifies (tests) a requirement
    DString trVerifies(bool /*singular*/) override
    { return "検証する要件"; }

    // indicates a requirement is verified (tested) by one or more symbols
    DString trVerifiedBy(const DString &list) override
    { return list+" によって検証されています。"; }

    DString trUnverifiedRequirements() override
    { return "未検証の要件"; }

    DString trUnverifiedRequirementsText(bool /*singular*/, const DString &list) override
    { return "要件 "+list+" は '検証' 関係を持っていません。"; }

};

#endif
