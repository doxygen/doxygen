/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef TRANSLATOR_JP_H
#define TRANSLATOR_JP_H

#include "translator.h"

class TranslatorJapanese : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "a4j"; }
    QCString trInherits()
    { return "継承"; }
    QCString trAnd()
    { return "と"; }
    QCString trInheritedBy()
    { return "次に継承されています。"; }
    QCString trRelatedFunctions()
    { return "関連する関数"; }
    QCString trRelatedSubscript()
    { return "（これらはメンバ関数でないことに注意）"; }
    QCString trDetailedDescription()
    { return "解説"; }
    QCString trMemberTypedefDocumentation()
    { return "メンバ型定義の解説"; }
    QCString trMemberEnumerationDocumentation()
    { return "メンバ列挙型の解説"; }
    QCString trEnumerationValueDocumentation()
    { return "列挙型値の解説"; }
    QCString trMemberFunctionDocumentation()
    { return "メンバ関数の解説"; }
    QCString trMemberDataDocumentation()
    { return "メンバデータの解説"; }
    QCString trGeneratedFrom(const char *s,bool)
    { 
      QCString result=(QCString)"この"+s+
	           "に対するドキュメントは以下のファイルから生成されました。";
      return result;
    }
    QCString trMore()
    { return "より詳しく..."; }
    QCString trReference()
    { return "リファレンス"; }
    QCString trListOfAllMembers()
    { return "すべてのメンバリスト"; }
    QCString trMemberList()
    { return "メンバリスト"; }
    QCString trThisIsTheListOfAllMembers()
    { return "これは次の全メンバリストです。"; }
    QCString trIncludingInheritedMembers()
    { return "継承メンバすべてを含んで。"; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result;
      if (s) result=(QCString)s+"に対して";
      result+="ソースコードから Doxygen が自動的に生成しました。";
      return result;
    }
    QCString trEnumName()
    { return "enum 型"; }
    QCString trEnumValue()
    { return "enum 値"; }
    QCString trDefinedIn()
    { return "次で定義されました。"; }
    QCString trIncludeFile()
    { return "インクルードファイル"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"これはインクルードファイル"+f+
	"の Verbatim テキストです。"; }
    QCString trModules()
    { return "モジュール"; }
    QCString trClassHierarchy()
    { return "クラス階層"; }
    QCString trCompoundList()
    { return "データ構造リスト"; }
    QCString trFileList()
    { return "ファイルリスト"; }
    QCString trHeaderFiles()
    { return "ヘッダファイル"; }
    QCString trCompoundMembers()
    { return "データ構造メンバ"; }
    QCString trFileMembers()
    { return "ファイルメンバ"; }
    QCString trRelatedPages()
    { return "関連ページ"; }
    QCString trExamples()
    { return "例"; }
    QCString trSearch()
    { return "検索"; }
    QCString trClassHierarchyDescription()
    { return "この継承リストはおおまかにはソートされていますが、"
             "アルファベット順で完全にソートされてはいません。";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="このリストは、";
      if (!extractAll) result+="ドキュメント化され、";
      result+="簡易説明を持つファイルすべてのリストです。";
      return result;
    }
    QCString trCompoundListDescription()
    { return "これは簡易説明を持つ、クラス、構造体、共用体のリストです。";}
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="これは";
      if (!extractAll) result+="ドキュメント化された";
      result+="クラスメンバすべてのリストで、それぞれ";
      if (extractAll) result+="が属しているクラス";
      result+="の解説へのリンクが張られています。";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="これは";
      if (!extractAll) result+="ドキュメント化された";
      result+="ファイルメンバすべてのリストで、それぞれ";
      if (extractAll) result+="が属しているファイル";
      result+="の解説へのリンクが張られています。";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "APIを構成するヘッダファイルです。"; }
    QCString trExamplesDescription()
    { return "すべての例のリストです。"; }
    QCString trRelatedPagesDescription()
    { return "関連するドキュメントページすべてのリストです。"; }
    QCString trModulesDescription()
    { return "すべてのモジュールのリストです。"; }
    QCString trNoDescriptionAvailable()
    { return "ドキュメントが記述されていません。"; }

    QCString trDocumentation()
    { return "ドキュメント"; }
    QCString trModuleIndex()
    { return "モジュール索引"; }
    QCString trHierarchicalIndex()
    { return "階層索引"; }
    QCString trCompoundIndex()
    { return "データ構造索引"; }
    QCString trFileIndex() 
    { return "ファイル索引"; }
    QCString trModuleDocumentation()
    { return "モジュールの解説"; }
    QCString trClassDocumentation()
    { return "クラスの解説"; }
    QCString trFileDocumentation()
    { return "ファイルの解説"; }
    QCString trExampleDocumentation()
    { return "例題の解説"; }
    QCString trPageDocumentation()
    { return "ページの解説"; }
    QCString trReferenceManual()
    { return "リファレンスマニュアル"; }

    QCString trDefines()
    { return "マクロ定義"; }
    QCString trFuncProtos()
    { return "関数プロトタイプ"; }
    QCString trTypedefs()
    { return "型定義"; }
    QCString trEnumerations()
    { return "列挙型"; }
    QCString trFunctions()
    { return "関数"; }
    QCString trVariables()
    { return "変数"; }
    QCString trEnumerationValues()
    { return "列挙型値"; }
    QCString trReimplementedFrom()
    { return "次を再定義"; }
    QCString trReimplementedIn()
    { return "次で再定義"; }
    QCString trAuthor()
    { return "作者"; }
    QCString trDefineDocumentation()
    { return "マクロ定義の解説"; }
    QCString trFunctionPrototypeDocumentation()
    { return "関数プロトタイプの解説"; }
    QCString trTypedefDocumentation()
    { return "型定義の解説"; }
    QCString trEnumerationTypeDocumentation()
    { return "列挙型の解説"; }
    QCString trFunctionDocumentation()
    { return "関数の解説"; }
    QCString trVariableDocumentation()
    { return "変数の解説"; }
    QCString trCompounds()
    { return "データ構造"; }
    QCString trFiles()
    { return "ファイル"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result;
      if (projName) result+=(QCString)projName+"に対して";
      result+=(QCString)date+"に生成されました。";
      return result;
    }
    QCString trWrittenBy()
    {
      return "を開発したのは";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"に対するクラス階層図";
    }
    QCString trForInternalUseOnly()
    { return "内部使用のみ。"; }
    QCString trReimplementedForInternalReasons()
    { return "内部的な理由により再実装されましたが、APIには影響しません。";
    }
    QCString trWarning()
    { return "注意"; }
    QCString trBugsAndLimitations()
    { return "バグと制限"; }
    QCString trVersion()
    { return "バージョン"; }
    QCString trDate()
    { return "日付"; }
    QCString trAuthors()
    { return "作者"; }
    QCString trReturns()
    { return "戻り値"; }
    QCString trSeeAlso()
    { return "参照"; }
    QCString trParameters()
    { return "引数"; }
    QCString trExceptions()
    { return "例外"; }
    QCString trGeneratedBy()
    { return ""; }
    
    // new since 0.49-990307 
    
    QCString trNamespaces()
    { return "名前空間"; }
    QCString trNamespaceList()
    { return "名前空間リスト"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="このリストは、簡易説明を持つすべての";
      if (!extractAll) result+="ドキュメント化された";
      result+="名前空間のリストです。";
      return result;
    }
    QCString trFriends()
    { return "フレンド"; }
    
    // new since 0.49-990405
    
    virtual QCString trRelatedFunctionDocumentation()
    { return "フレンドと関連する関数の解説"; }
};

#endif
