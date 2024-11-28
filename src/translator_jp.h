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

class TranslatorJapanese : public TranslatorAdapter_1_8_15
{
  public:
    QCString idLanguage() override
    { return "japanese"; }

    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage{CJKutf8}\n";
    }
    QCString trISOLang() override
    {
      return "ja";
    }
    QCString getLanguageString() override
    {
      return "0x411 Japanese";
    }
    QCString latexFontenc() override
    {
      return "";
    }
    QCString latexDocumentPre() override
    {
      return "\\begin{CJK}{UTF8}{min}\n";
    }
    QCString latexDocumentPost() override
    {
      return "\\end{CJK}\n";
    }
    bool needsPunctuation() override
    {
      return false;
    }

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "関連関数"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "（これらはメソッドではありません）"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "詳解"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "詳細"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "型定義メンバ詳解"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "列挙型メンバ詳解"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
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
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "[詳解]"; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "全メンバ一覧"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "メンバ一覧"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "継承メンバを含む"; }
    /* trIncludingInheritedMembers に続くように定義すること */

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return " の全メンバ一覧です。"; }
    /* trThisIsTheListOfAllMembers から続くように定義すること */

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result = "Doxygen により";
      if (!s.isEmpty()) result=" "+s+"の";
      result+="ソースコードから抽出しました。";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "列挙名"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "列挙値"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "定義場所: "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "モジュール"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "クラス階層"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "ファイル一覧"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "諸情報"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "各種例"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "検索"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
      return "クラス階層一覧です。大雑把に文字符号順で並べられています。";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
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
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      const bool forC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
      QCString result;
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
    QCString trFileMembersDescription(bool /*extractAll*/) override
    {
      QCString result;
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
    QCString trExamplesDescription() override
    { return "各種例の一覧です。"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "諸情報の一覧です。"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "全モジュールの一覧です。"; }

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "詳解"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "モジュール索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "階層索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "ファイル索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "モジュール詳解"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "ファイル詳解"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "リファレンスマニュアル"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "マクロ定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "型定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "列挙型"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "関数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "変数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "列挙値"; }
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "マクロ定義詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "型定義詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "列挙型詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "関数詳解"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "変数詳解"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result = date+"作成";
      if (!projName.isEmpty()) result+=" - " + projName;
      result+=" / 構成: ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return clName+" の継承関係図";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "警告"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "バージョン"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "日付"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "戻り値"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "参照"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "引数"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "例外"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "構築:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "名前空間一覧"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
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
    QCString trFriends() override
    { return "フレンド"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "フレンドと関連関数の詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      QCString result=clName+" ";
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName+" ファイル";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName+" 名前空間";
      return result;
    }

    /* these are for the member sections of a class, struct or union */
    QCString trPublicMembers() override
    { return "公開メンバ関数"; }
    QCString trPublicSlots() override
    { return "公開スロット"; }
    QCString trSignals() override
    { return "シグナル"; }
    QCString trStaticPublicMembers() override
    { return "静的公開メンバ関数"; }
    QCString trProtectedMembers() override
    { return "限定公開メンバ関数"; }
    QCString trProtectedSlots() override
    { return "限定公開スロット"; }
    QCString trStaticProtectedMembers() override
    { return "静的限定公開メンバ関数"; }
    QCString trPrivateMembers() override
    { return "非公開メンバ関数"; }
    QCString trPrivateSlots() override
    { return "非公開スロット"; }
    QCString trStaticPrivateMembers() override
    { return "静的非公開メンバ関数"; }

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
          result+=", ";
        }
      }
      if ( result.length() > 60 )
      {
        QCString countStr;
        countStr.sprintf(" (計%d項目)", numEntries);
        result += countStr;
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return trWriteList(numEntries)+"を継承しています。";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return trWriteList(numEntries)+"に継承されています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"を再実装しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"で再実装されています。";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "名前空間メンバ"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="これは";
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
    QCString trNamespaceIndex() override
    { return "名前空間索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "名前空間詳解"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "名前空間"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool /*single*/) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="この";
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
    QCString trReturnValues() override
    { return "戻り値"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "総合概要"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return " @1 の @0 行目に定義があります。";
    }
    QCString trDefinedInSourceFile() override
    {
      return " @0 に定義があります。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "非推奨";
    }

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return clName+" 連携図";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
    return fName+" の依存先関係図:";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "構築子と解体子";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "[ソースコード]";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "[詳解]";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "事前条件";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "事後条件";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "不変";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "初期値:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "コード";
    }
    QCString trGraphicalHierarchy() override
    {
      return "クラス階層図";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "[クラス階層図]";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "[クラス階層表]";
    }
    QCString trPageIndex() override
    {
      return "ページ索引";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "覚え書き";
    }
    QCString trPublicTypes() override
    {
      return "公開型";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "静的公開変数類";
    }
    QCString trProtectedTypes() override
    {
      return "限定公開型";
    }
    QCString trProtectedAttribs() override
    {
      return "限定公開変数類";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "静的限定公開変数類";
    }
    QCString trPrivateTypes() override
    {
      return "非公開型";
    }
    QCString trPrivateAttribs() override
    {
      return "非公開変数類";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "静的非公開変数類";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "todo";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "todo一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "参照元";
    }
    QCString trRemarks() override
    {
      return "注釈";
    }
    QCString trAttention() override
    {
      return "注意";
    }
    QCString trInclByDepGraph() override
    {
      return "被依存関係図:";
    }
    QCString trSince() override
    {
      return "から";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "グラフの凡例";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "凡例";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "テスト";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "テスト一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "プロパティ";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "プロパティ詳解";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return name+" パッケージ";
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "パッケージ一覧です。";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "パッケージ";
    }

	    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "値:";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "バグ";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "932";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "128";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "索引";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool /*first_capital*/, bool /*singular*/) override
    {
      return "クラス";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool /*first_capital*/, bool /*singular*/) override
    {
      return "ファイル";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool /*first_capital*/, bool /*singular*/) override
    {
      return "名前空間";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool /*first_capital*/, bool /*singular*/) override
    {
      return "グループ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool /*first_capital*/, bool /*singular*/) override
    {
      return "ページ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool /*first_capital*/, bool /*singular*/) override
    {
      return "メンバ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool /*first_capital*/, bool /*singular*/) override
    {
      return "大域各種";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool /*first_capital*/, bool /*singular*/) override
    {
      return "著者";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "参照先";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"を実装しています。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"で実装されています。";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "目次";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "非推奨一覧";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "イベント";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "イベント詳解";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "パッケージ内の型定義";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "関数";
    }
    QCString trPackageMembers() override
    {
      return "パッケージ内のメンバ";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "静的関数";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "変数";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "静的変数";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "全て";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "呼び出し関係図:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "照合語:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " ソースファイル";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "ディレクトリ索引"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "ディレクトリ詳解"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "ディレクトリ"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" ディレクトリリファレンス"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool, bool) override
    {
      return "ディレクトリ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "これはオーバーロードされたメンバ関数です。"
              "利便性のために用意されています。"
              "元の関数との違いは引き数のみです。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "被呼び出し関係図:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "列挙型詳解"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "メンバ関数/サブルーチン詳解"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "データ型一覧"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "データフィールド"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "これはデータ型の一覧です:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="これは";
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
    QCString trCompoundIndexFortran() override
    { return "データ型索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "データ型詳解"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "関数/サブルーチン"; }


    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "関数/サブルーチン詳解"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "データ型"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "モジュール一覧"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result;
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
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
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
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+="モジュール";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "モジュールメンバ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="これはモジュールメンバ一覧です。それぞれ ";
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
    QCString trModulesIndex() override
    { return "モジュール索引"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool /*first_capital*/, bool /*singular*/) override
    {
      return "モジュール";
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool /*single*/) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="次のファイルから";
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
    QCString trType(bool /*first_capital*/, bool /*singular*/) override
    {
      QCString result = "型";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool /*first_capital*/, bool /*singular*/) override
    {
      QCString result = "サブプログラム";
      return result;
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "型制約";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" 関係";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "読み取り中…";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "大域名前空間";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "検索中…";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return name+"にあるファイル";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "月", "火", "水", "木", "金", "土", "日" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%.4d年%.2d月%.2d日(%s)",year,month,day,days[dayOfWeek-1]);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d時%.2d分%.2d秒",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "月", "火", "水", "木", "金", "土", "日" };
      static const char *days_full[]    = { "月曜日", "火曜日", "水曜日", "木曜日", "金曜日", "土曜日", "日曜日" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
      static const char *months_full[]  = { "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "午前", "午後" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "書誌参照"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "著作権所有"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString(name)+" のディレクトリ依存関係図"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "表示階層"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "テンプレート引数"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "ほか "+number+" 件…"; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool) override
    {
      return "次のファイルからこの列挙についての詳解を抽出しました:";
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString("列挙 ")+name+" 詳解"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString("基底クラス ")+what+" に属する継承"+members; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "その他の継承メンバ"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {

      QCString opt = enable ? "有効" : "無効";
      return "クリックで同期表示が"+opt+"になります";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "@0 カテゴリーから提供されています。";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "@0 を拡張しています。";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "クラスメソッド";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "実体メソッド";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "メソッド詳解";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "実装されたインターフォース"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "継承されたサービス"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "定数グループ"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" 定数グループ詳解";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" サービス詳解";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Singleton 詳解";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool /*single*/) override
    {
      // single is true implies a single file
      return "次のファイルからこのサービスについて"
             "の詳解を抽出しました:";
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool /*single*/) override
    {
      // single is true implies a single file
      return "次のファイルからこの Singleton について"
             "の詳解を抽出しました:";
    }

};

#endif
