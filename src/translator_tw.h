/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
// implement exactly the same (pure virtual) methods as the
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
// base class is changed occasionaly to adapter classes when the
// Translator class changes the interface, or back to the
// Translator class (by the local maintainer) when the localized
// translator is made up-to-date again.

class TranslatorChinesetraditional : public TranslatorAdapter_1_8_0
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "big5";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "相關函式"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(註：這些不是成員函式)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "詳細描述"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "型態定義成員說明文件"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "列舉型態成員說明文件"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "函式成員說明文件"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "欄位說明文件";
      }
      else
      {
        return "資料成員說明文件";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "更多..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "全部成員列表"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "成員列表"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "完整成員列表，類別為"; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", 包含所有繼承的成員"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="本文件由Doxygen";
      if (s) result+=(QCString)" 自 "+s;
      result+=" 的原始碼中自動產生.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "列舉型態名稱"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "列舉值"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "被定義於"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "模組"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "類別階層"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "資料結構";
      }
      else
      {
        return "複合列表";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "檔案列表"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "資料欄位";
      }
      else
      {
        return "複合成員";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "全域資料";
      }
      else
      {
        return "檔案成員";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "相關頁面"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "範例"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "搜尋"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "這個繼承列表經過簡略的字母排序: ";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="這是附帶簡略說明";
      if (!extractAll) result+="且經過文件化";
      result+="的檔案列表:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="這是全部";
      if (!extractAll)
      {
        result+="文件化過";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="這是全部";
      if (!extractAll) result+="文件化的";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "所有範例列表:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "所有相關文件頁面列表:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "所有模組列表:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "說明文件"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "模組索引"; }

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
    virtual QCString trFileIndex()
    { return "檔案索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "模組說明文件"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "資料結構說明文件";
      }
      else
      {
        return "類別說明文件";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "檔案說明文件"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "範例說明文件"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "頁面說明文件"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "參考手冊"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "函式原型"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "型態定義"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "列舉型態"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "函式"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "變數"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "列舉值"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "定義巨集說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "函式原型說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "型態定義說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "列舉型態說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "列舉值說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "函式說明文件"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "變數說明文件"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"產生日期:"+date;
      if (projName) result+=(QCString)", 專案:"+projName;
      result+=(QCString)", 產生器:";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "撰寫人:";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"類別"+clName+"的繼承圖:";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "僅供內部使用."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "警告"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "版本"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "日期"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "傳回值"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "參閱"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "參數"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "例外"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "產生者:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "命名空間(name space)列表"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="這是所有附帶簡略說明的";
      if (!extractAll) result+="文件化的";
      result+="命名空間(namespaces):";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "類別朋友(Friends)"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "類別朋友及相關函式說明文件"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:      result+=" 類別"; break;
        case ClassDef::Struct:     result+=" 結構"; break;
        case ClassDef::Union:      result+=" 聯合"; break;
        case ClassDef::Interface:  result+=" 介面"; break;
        case ClassDef::Protocol:   result+=" 協定"; break; 
        case ClassDef::Category:   result+=" 分類"; break;
        case ClassDef::Exception:  result+=" 例外"; break;
      }
      if (isTemplate) result+=" 樣版";
      result+=" 參考文件";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" 檔案參考文件";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" 命名空間(Namespace)參考文件";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "公開方法(Public Methods)"; }
    virtual QCString trPublicSlots()
    { return "公開插槽(Public Slots)"; }
    virtual QCString trSignals()
    { return "訊號(Signals)"; }
    virtual QCString trStaticPublicMembers()
    { return "靜態公開方法(Static Public Methods)"; }
    virtual QCString trProtectedMembers()
    { return "保護方法(Protected Methods)"; }
    virtual QCString trProtectedSlots()
    { return "保護插槽(Protected Slots)"; }
    virtual QCString trStaticProtectedMembers()
    { return "靜態保護方法(Static Protected Methods)"; }
    virtual QCString trPrivateMembers()
    { return "私有方法(Private Methods)"; }
    virtual QCString trPrivateSlots()
    { return "私有插槽(Private Slots)"; }
    virtual QCString trStaticPrivateMembers()
    { return "靜態私有方法(Static Private Methods)"; }

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
            result+=", 及 ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "繼承自 "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "被 "+trWriteList(numEntries)+"繼承.";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "依據"+trWriteList(numEntries)+"重新實作.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "在"+trWriteList(numEntries)+"重新實作.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "命名空間(Namespace)成員"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "命名空間(Namespace)索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "命名空間(Namespace)說明文件"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "命名空間(Namespaces)"; }

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
      QCString result=(QCString)"此";
      switch(compType)
      {
        case ClassDef::Class:      result+="類別(class)"; break;
        case ClassDef::Struct:     result+="結構(structure)"; break;
        case ClassDef::Union:      result+="聯合(union)"; break;
        case ClassDef::Interface:  result+="介面(interface)"; break;
        case ClassDef::Protocol:   result+="協定(protocol)"; break;
        case ClassDef::Category:   result+="分類(category)"; break;
        case ClassDef::Exception:  result+="例外(exception)"; break;
      }
      result+=" 文件是由下列檔案中產生";
      if (single) result+=":"; else result+=":";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "字母順序列表"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "傳回值"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "主頁面"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "定義在 @1 檔案之第 @0 行.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "定義在 @0 檔.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "過時";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)""+clName+"的合作圖:";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)""+fName+"的包含相依圖:";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "建構子與解構子說明文件";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "查看本檔案的原始碼.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "查看本檔案說明文件.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "前置條件";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "後置條件";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "常數";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "初值:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "程式碼";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "圖形化之類別階層";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "查看圖形化之類別階層";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "查看文字化之類別階層";
    }
    virtual QCString trPageIndex()
    {
      return "頁面索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "註";
    }
    virtual QCString trPublicTypes()
    {
      return "公開型態";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "資料欄位";
      }
      else
      {
        return "公開屬性";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "靜態公開屬性";
    }
    virtual QCString trProtectedTypes()
    {
      return "保護型態";
    }
    virtual QCString trProtectedAttribs()
    {
      return "保護屬性";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "靜態保護屬性";
    }
    virtual QCString trPrivateTypes()
    {
      return "私有型態";
    }
    virtual QCString trPrivateAttribs()
    {
      return "私有屬性";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "靜態私有屬性";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "待辦事項";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "待辦事項列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "被參考於";
    }
    virtual QCString trRemarks()
    {
      return "備註";
    }
    virtual QCString trAttention()
    {
      return "注意";
    }
    virtual QCString trInclByDepGraph()
    {
      return "本圖顯示出哪些檔案直接或間接include本檔 "
             ":";
    }
    virtual QCString trSince()
    {
      return "自";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "圖示";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return
        "本頁解釋如何解譯這些由doxygen所產生的圖示 "
        ".<p>\n"
        "請看下面範例:\n"
        "\\code\n"
        "/*! 因為截斷的不可見類別 */\n"
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
        "若在組態檔中的 \\c MAX_DOT_GRAPH_HEIGHT tag "
        "設為 240，將會產生下列的圖示:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "上圖中的各區塊意義如下:\n"
        "<ul>\n"
        "<li>%A 填滿黑色的區塊代表產生這個圖示的類別或結構 "
        ".\n"
        "<li>%A 黑邊的區塊代表文件化過的結構或類別.\n"
        "<li>%A 灰邊的區塊代表未經文件化的結構或是類別.\n"
        "<li>%A 紅邊的區塊代表文件化的結構或是類別，"
        "這些結構或類別的繼承或包含關係不會全部顯示. %A 圖示 "
        "若無法塞入指定的邊界中將會被截斷.\n"
        "</ul>\n"
        "箭頭具有下面的意義:\n"
        "<ul>\n"
        "<li>%A 深藍色箭頭用來代表兩個類別間的公開繼承 "
        "關係.\n"
        "<li>%A 深綠色箭頭代表保護繼承.\n"
        "<li>%A 深紅色箭頭代表私有繼承.\n"
        "<li>%A 紫色箭頭用來表示類別被另一個包含或是使用."
        "箭頭上標示著可存取該類別或是結構的對應變數"
        ".\n"
        "<li>%A 黃色箭頭代表樣版實體與樣版類別之間的關係。"
        "箭頭上標記著樣版實體上的參數"
        ".\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "圖示";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "測試項目";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "測試項目列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP方法";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "屬性(properties)";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "屬性(property)說明文件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "資料結構";
      }
      else
      {
        return "類別";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Package "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Package列表";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "此處為Package的概略說明(如果有的話):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Packages";
    }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "巨集內容:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "臭蟲";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "950";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "136";
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
      return QCString("類別");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("檔案");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("命名空間");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("群組");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("頁面");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("成員");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("全域");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("作者");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "參考";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "實作 "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "實作於 "+trWriteList(numEntries)+".";
    }

    //////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "目錄";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "過時項目(Deprecated) 列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Event 文件";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Package 型別";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Package 函數列表";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "靜態 Package 函數列表";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Package 屬性";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "靜態 Package 屬性";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "全部";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "這是此函數的引用函數圖:";
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
      return "搜尋索引";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "符合:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " 原始程式檔";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "目錄階層"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "目錄說明文件"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "目錄"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "這個目錄階層經過簡略的字母排序: ";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" 目錄參考文件"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool /*first_capital*/, bool /*singular*/)
    {
      return QCString("目錄");
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "這是一個為了便利性所提供 overload 成員函數，"
              "只有在接受的參數上，與前一個函數不同.";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "呼叫此函數的函數列表:";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "成員函數/子程序 文件"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "資料型態列表"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "資料欄位"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "資料型態簡短說明列表:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "資料型別索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "資料型別文件"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "函數/子程序"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "函數/子程序 文件"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "資料型別"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "模組列表"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="此處列出所有";
      if (!extractAll) result+="有文件的";
      result+="模組附帶簡短說明:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+="模組"; break;
        case ClassDef::Struct:     result+="型態"; break;
        case ClassDef::Union:      result+="聯合"; break;
        case ClassDef::Interface:  result+="介面"; break;
        case ClassDef::Protocol:   result+="協議"; break;
        case ClassDef::Category:   result+="分類"; break;
        case ClassDef::Exception:  result+="例外"; break;
      }
      if (isTemplate) result+=" Template";
      result+="參考文件";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+="模組參考文件";
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "模組成員"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "模組索引"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool /* first_capital */, bool /* singular */)
    {       
      QCString result("模組");
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool /* single */)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"這個";
      switch(compType)
      {
        case ClassDef::Class:      result+="模組"; break;
        case ClassDef::Struct:     result+="型態"; break;
        case ClassDef::Union:      result+="聯合"; break;
        case ClassDef::Interface:  result+="介面"; break;
        case ClassDef::Protocol:   result+="協議"; break;
        case ClassDef::Category:   result+="分類"; break;
        case ClassDef::Exception:  result+="例外"; break;
      }
      result+="文件由下列檔案產生";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool /* first_capital */, bool /* singular */)
    { 
      QCString result("型別");
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool /* first_capital */, bool /* singular */)
    { 
      QCString result("子程式");
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "型別限制條件";
    }
	
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" 關連";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "載入中...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "全域命名空間";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "搜尋中...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "無符合項目";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)name+"的目錄關連圖";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"檔案在"+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"含入檔案在"+name;
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
      static const char *days[]   = { "星期一","星期二","星期三","星期四","星期五","星期六","星期日" };
      static const char *months[] = { "1","2","3","4","5","6","7","8","9","10","11","12" };
      QCString sdate;
      sdate.sprintf("%d年%s月%d日 %s",year,months[month-1],day,days[dayOfWeek-1]);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "參考文獻資料"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "版權聲明"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString(name)+"的目錄關連圖"+":"; }

};

#endif
