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
 * The translation into Chinesetraditional was provided by
 *   Daniel YC Lin (dlin.tw <at> gmail.com) since v1.2.16-v1.5.5
 */

#ifndef TRANSLATOR_TW_H
#define TRANSLATOR_TW_H

// When defining a translator class for the new language, follow
// the description in the documentation.  One of the steps says
// that you should copy the translator_en.h (this) file to your
// translator_xx.h new file.  Your new language should use the
// Translator class as the base class.  This means that you need to
// implement exactly the same (pure virtual) override methods as the
// TranslatorEnglish does.  Because of this, it is a good idea to
// start with the copy of TranslatorEnglish and replace the strings
// one by one.
//
// It is not necessary to include "translator.h" or
// "translator_adapter.h" here.  The files are included in the
// language.cpp correctly.  Not including any of the mentioned
// files frees the maintainer from thinking about whether the
// first, the second, or both files should be included or not, and
// why.  This holds namely for localized translators because their
// base class is changed occasionally to adapter classes when the
// Translator class changes the interface, or back to the
// Translator class (by the local maintainer) when the localized
// translator is made up-to-date again.

class TranslatorChinesetraditional : public TranslatorAdapter_1_8_15
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
    { return "chinese-traditional"; }

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
      return "\\usepackage{CJKutf8}\n";
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
    QCString trISOLang() override
    {
      return "zh-Hant";
    }
    QCString getLanguageString() override
    {
      return "0x404 Chinese (Taiwan)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "相關函式"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(註：這些不是成員函式)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "詳細描述"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "詳細資訊"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "型態定義成員說明文件"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "列舉型態成員說明文件"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "函式成員說明文件"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "欄位說明文件";
      }
      else
      {
        return "資料成員說明文件";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "更多..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "全部成員列表"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "成員列表"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "完整成員列表，類別為"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", 包含所有繼承的成員"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="本文件由Doxygen";
      if (!s.isEmpty()) result+=" 自 "+s;
      result+=" 的原始碼中自動產生.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "列舉型態名稱"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "列舉值"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "被定義於"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "模組"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "類別階層"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料結構";
      }
      else
      {
        return "複合列表";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "檔案列表"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料欄位";
      }
      else
      {
        return "複合成員";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "全域資料";
      }
      else
      {
        return "檔案成員";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "相關頁面"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "範例"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "搜尋"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "這個繼承列表經過簡略的字母排序: ";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="這是附帶簡略說明";
      if (!extractAll) result+="且經過文件化";
      result+="的檔案列表:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "這是附帶簡略說明的資料結構:";
      }
      else
      {
        return "這是附帶簡略說明的類別，結構，"
               "聯合型態(unions)及介面(interfaces):";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="這是全部";
      if (!extractAll)
      {
        result+="文件化過";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="結構及聯合型態欄位";
      }
      else
      {
        result+="類別成員";
      }
      result+=", 並且帶有連結至";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="每個欄位的結構/聯合型態說明文件:";
        }
        else
        {
          result+="每個成員的類別說明文件:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="這些結構/聯合型態所屬:";
        }
        else
        {
          result+="這些類別所屬:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="這是全部";
      if (!extractAll) result+="文件化的";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="函式，變數，定義，列舉，及型態定義";
      }
      else
      {
        result+="檔案成員";
      }
      result+="，並且帶有連結至";
      if (extractAll)
        result+="這些檔案所屬:";
      else
        result+="說明文件:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "所有範例列表:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "所有相關文件頁面列表:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "所有模組列表:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "說明文件"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "模組索引"; }

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
        return "資料結構索引";
      }
      else
      {
        return "複合索引";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "檔案索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "模組說明文件"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料結構說明文件";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "類別說明文件";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "檔案說明文件"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "參考手冊"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "型態定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "列舉型態"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "函式"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "變數"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "列舉值"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "定義巨集說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "型態定義說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "列舉型態說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "列舉值說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "函式說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "變數說明文件"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料結構";
      }
      else
      {
        return "複合項目";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="產生日期:"+date;
      if (!projName.isEmpty()) result+=", 專案:"+projName;
      result+=", 產生器:";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "類別"+clName+"的繼承圖:";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "警告"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "版本"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "日期"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "傳回值"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "參閱"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "參數"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "例外"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "產生者:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "命名空間(name space)列表"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="這是所有附帶簡略說明的";
      if (!extractAll) result+="文件化的";
      result+="命名空間(namespaces):";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "類別朋友(Friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "類別朋友及相關函式說明文件"; }

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
        case ClassDef::Class:      result+=" 類別"; break;
        case ClassDef::Struct:     result+=" 結構"; break;
        case ClassDef::Union:      result+=" 聯合"; break;
        case ClassDef::Interface:  result+=" 介面"; break;
        case ClassDef::Protocol:   result+=" 協定"; break;
        case ClassDef::Category:   result+=" 分類"; break;
        case ClassDef::Exception:  result+=" 例外"; break;
        default: break;
      }
      if (isTemplate) result+=" 樣版";
      result+=" 參考文件";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" 檔案參考文件";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" 命名空間(Namespace)參考文件";
      return result;
    }

    QCString trPublicMembers() override
    { return "公開方法(Public Methods)"; }
    QCString trPublicSlots() override
    { return "公開插槽(Public Slots)"; }
    QCString trSignals() override
    { return "訊號(Signals)"; }
    QCString trStaticPublicMembers() override
    { return "靜態公開方法(Static Public Methods)"; }
    QCString trProtectedMembers() override
    { return "保護方法(Protected Methods)"; }
    QCString trProtectedSlots() override
    { return "保護插槽(Protected Slots)"; }
    QCString trStaticProtectedMembers() override
    { return "靜態保護方法(Static Protected Methods)"; }
    QCString trPrivateMembers() override
    { return "私有方法(Private Methods)"; }
    QCString trPrivateSlots() override
    { return "私有插槽(Private Slots)"; }
    QCString trStaticPrivateMembers() override
    { return "靜態私有方法(Static Private Methods)"; }

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
            result+=", 及 ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "繼承自 "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "被 "+trWriteList(numEntries)+"繼承.";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "依據"+trWriteList(numEntries)+"重新實作.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "在"+trWriteList(numEntries)+"重新實作.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "命名空間(Namespace)成員"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="此處列表為所有 ";
      if (!extractAll) result+="文件化的 ";
      result+="命名空間(namespace)成員，並且附帶連結至 ";
      if (extractAll)
        result+="每個成員的說明文件:";
      else
        result+="該命名空間所屬之處:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "命名空間(Namespace)索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "命名空間(Namespace)說明文件"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "命名空間(Namespaces)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,bool) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="此";
      switch(compType)
      {
        case ClassDef::Class:      result+="類別(class)"; break;
        case ClassDef::Struct:     result+="結構(structure)"; break;
        case ClassDef::Union:      result+="聯合(union)"; break;
        case ClassDef::Interface:  result+="介面(interface)"; break;
        case ClassDef::Protocol:   result+="協定(protocol)"; break;
        case ClassDef::Category:   result+="分類(category)"; break;
        case ClassDef::Exception:  result+="例外(exception)"; break;
        default: break;
      }
      result+=" 文件是由下列檔案中產生";
      result+=":";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "傳回值"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "主頁面"; }

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
      return "定義在 @1 檔案之第 @0 行.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "定義在 @0 檔.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "過時";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return ""+clName+"的合作圖:";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return ""+fName+"的包含相依圖:";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "建構子與解構子說明文件";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "查看本檔案的原始碼.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "查看本檔案說明文件.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "前置條件";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "後置條件";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "常數";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "初值:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "程式碼";
    }
    QCString trGraphicalHierarchy() override
    {
      return "圖形化之類別階層";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "查看圖形化之類別階層";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "查看文字化之類別階層";
    }
    QCString trPageIndex() override
    {
      return "頁面索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "註";
    }
    QCString trPublicTypes() override
    {
      return "公開型態";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料欄位";
      }
      else
      {
        return "公開屬性";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "靜態公開屬性";
    }
    QCString trProtectedTypes() override
    {
      return "保護型態";
    }
    QCString trProtectedAttribs() override
    {
      return "保護屬性";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "靜態保護屬性";
    }
    QCString trPrivateTypes() override
    {
      return "私有型態";
    }
    QCString trPrivateAttribs() override
    {
      return "私有屬性";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "靜態私有屬性";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "待辦事項";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "待辦事項列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "被參考於";
    }
    QCString trRemarks() override
    {
      return "備註";
    }
    QCString trAttention() override
    {
      return "注意";
    }
    QCString trInclByDepGraph() override
    {
      return "本圖顯示出哪些檔案直接或間接include本檔 "
             ":";
    }
    QCString trSince() override
    {
      return "自";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "圖示";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "本頁解釋如何解譯這些由doxygen所產生的圖示 "
        ".<p>\n"
        "請看下面範例:\n"
        "\\code\n"
        "/*! 因為截斷而造成的不可見類別 */\n"
        "class Invisible { };\n\n"
        "/*! 截斷的類別, 繼承關係被隱藏 */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* 未經過doxygen註解處理過的類別 */\n"
        "class Undocumented { };\n\n"
        "/*! 經過公開(Public)繼承的類別 */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! 一個樣版類別 */\n"
        "template<class T> class Templ { };\n\n"
        "/*! 使用保護(Protected)繼承的類別 */\n"
        "class ProtectedBase { };\n\n"
        "/*! 使用私有(Private)繼承的類別 */\n"
        "class PrivateBase { };\n\n"
        "/*! 由被繼承類別所使用的類別 */\n"
        "class Used { };\n\n"
        "/*! 由數個其他類別所繼承來的超類別(Super Class) */\n"
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
        "這個例子會產生下列的圖示:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "上圖中的各區塊意義如下:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A 填滿黑色的區塊代表產生這個圖示的類別或結構 "
        ".\n"
        "<li>%A 黑邊的區塊代表文件化過的結構或類別.</li>\n"
        "<li>%A 灰邊的區塊代表未經文件化的結構或是類別.</li>\n"
        "<li>%A 紅邊的區塊代表文件化的結構或是類別，"
        "這些結構或類別的繼承或包含關係不會全部顯示. %A 圖示 "
        "若無法塞入指定的邊界中將會被截斷.</li>\n"
        "</ul>\n"
        "<p>\n"
        "箭頭具有下面的意義:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A 深藍色箭頭用來代表兩個類別間的公開繼承 "
        "關係.\n"
        "<li>%A 深綠色箭頭代表保護繼承。</li>\n"
        "<li>%A 深紅色箭頭代表私有繼承。</li>\n"
        "<li>%A 紫色箭頭用來表示類別被另一個包含或是使用."
        "箭頭上標示著可存取該類別或是結構的對應變數。</li>\n"
        "<li>%A 黃色箭頭代表樣版實體與樣版類別之間的關係。"
        "箭頭上標記著樣版實體上的參數。</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "圖示";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "測試項目";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "測試項目列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "屬性(properties)";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "屬性(property)說明文件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "資料結構";
      }
      else
      {
        return "類別";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "此處為Package的概略說明(如果有的話):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Packages";
    }

    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "巨集內容:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "臭蟲";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "臭蟲列表";
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
      return "950";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "136";
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
      return QCString("類別");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("檔案");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("命名空間");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("群組");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("頁面");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("成員");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("全域");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("作者");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "參考";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "實作 "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "實作於 "+trWriteList(numEntries)+".";
    }

    //////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "目錄";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "過時項目(Deprecated) 列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Event 文件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Package 型別";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Package 函數列表";
    }
    QCString trPackageMembers() override
    {
      return "Package 成員列表";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "靜態 Package 函數列表";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Package 屬性";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "靜態 Package 屬性";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "全部";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "這是此函數的引用函數圖:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "搜尋結果";
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
        return "找不到符合的資料.";
      }
      else if (numDocuments==1)
      {
        return "找到 <b>1</b> 筆符合的資料.";
      }
      else
      {
        return "找到 <b>$num</b> 筆符合的資料. "
               "越符合的結果顯示在越前面.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "符合:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " 原始程式檔";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "目錄階層"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "目錄說明文件"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "目錄"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" 目錄參考文件"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool /*first_capital*/, bool /*singular*/) override
    {
      return QCString("目錄");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "這是一個為了便利性所提供 overload 成員函數，"
              "只有在接受的參數上，與前一個函數不同.";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "呼叫此函數的函數列表:";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "成員函數/子程序 文件"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "資料型態列表"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "資料欄位"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "資料型態簡短說明列表:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="此處列出所有";
      if (!extractAll)
      {
        result+="有文件的";
      }
      result+="資料型別成員函數";
      result+=" 附帶連結到 ";
      if (!extractAll)
      {
         result+="每個成員函數的資料結構文件";
      }
      else
      {
         result+="他們屬於的資料型別";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "資料型別索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "資料型別文件"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "函數/子程序"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "函數/子程序 文件"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "資料型別"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "模組列表"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="此處列出所有";
      if (!extractAll) result+="有文件的";
      result+="模組附帶簡短說明:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+="模組"; break;
        case ClassDef::Struct:     result+="型態"; break;
        case ClassDef::Union:      result+="聯合"; break;
        case ClassDef::Interface:  result+="介面"; break;
        case ClassDef::Protocol:   result+="協議"; break;
        case ClassDef::Category:   result+="分類"; break;
        case ClassDef::Exception:  result+="例外"; break;
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+="參考文件";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+="模組參考文件";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "模組成員"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="此處列出所有";
      if (!extractAll) result+="有文件的";
      result+="模組成員附帶連結到";
      if (extractAll)
      {
        result+="每個函數的模組文件:";
      }
      else
      {
        result+="他們所屬的模組:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "模組索引"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool /* first_capital */, bool /* singular */) override
    {
      QCString result("模組");
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool /* single */) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="這個";
      switch(compType)
      {
        case ClassDef::Class:      result+="模組"; break;
        case ClassDef::Struct:     result+="型態"; break;
        case ClassDef::Union:      result+="聯合"; break;
        case ClassDef::Interface:  result+="介面"; break;
        case ClassDef::Protocol:   result+="協議"; break;
        case ClassDef::Category:   result+="分類"; break;
        case ClassDef::Exception:  result+="例外"; break;
        default: break;
      }
      result+="文件由下列檔案產生";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool /* first_capital */, bool /* singular */) override
    {
      QCString result("型別");
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool /* first_capital */, bool /* singular */) override
    {
      QCString result("子程式");
      return result;
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "型別限制條件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" 關連";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "載入中...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "全域命名空間";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "搜尋中...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "無符合項目";
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
      return "檔案在"+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "含入檔案在"+name;
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
      static const char *days[]   = { "星期一","星期二","星期三","星期四","星期五","星期六","星期日" };
      static const char *months[] = { "1","2","3","4","5","6","7","8","9","10","11","12" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%d年%s月%d日 %s",year,months[month-1],day,days[dayOfWeek-1]);
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
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "周一", "周二", "周三", "周四", "周五", "周六", "周日" };
      static const char *days_full[]    = { "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月" };
      static const char *months_full[]  = { "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "上午", "下午" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "參考文獻資料"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "版權聲明"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString(name)+"的目錄關連圖"+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "詳細程度"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "樣版參數"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "及 "+number+" 個更多..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "此列舉型態的文件是由下列檔案所產生";
      if (!single) result += "";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" 列舉型態參考"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" 繼承自 "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "額外的繼承成員"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "啟用" : "停用";
      return "點擊 "+opt+" 面板進行同步";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "由 @0 分類所提供.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "延伸 @0 類別 .";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "類別方法";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "實體方法";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "方法文件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "導出介面"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "引入的服務"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "常數群組"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+="常數群組參考";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+="服務參考";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+="Singleton參考";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool) override
    {
      // single is true implies a single file
      QCString result="本服務的文件由以下的檔案"
                                "所產生";
      result+=":";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool) override
    {
      // single is true implies a single file
      QCString result="本singleton的文件由下面的檔案"
                                "所產生";
      result+=":";
      return result;
    }

};

#endif
