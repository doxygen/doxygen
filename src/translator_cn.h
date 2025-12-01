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

#ifndef TRANSLATOR_CN_H
#define TRANSLATOR_CN_H

/*!
   If you want insert a space whenever Chinese meets English characters, set
   CN_SPC to " ", else null.
*/
#define CN_SPC " "

class TranslatorChinese : public TranslatorAdapter_1_16_0
{
  public:
    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in the language.cpp.
     */

    QCString idLanguage() override
    { return "chinese"; }

    /*! Used to get the LaTeX command(s) for the language support.
     * This method should return string with commands that switch
     * LaTeX to the desired language.  For example
     * <pre>"\\usepackage[german]{babel}\n"
     * </pre>
     * or
     * <pre>"\\usepackage{polski}\n"
     * "\\usepackage[latin2]{inputenc}\n"
     * "\\usepackage[T1]{fontenc}\n"
     * </pre>
     *
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage{CJKutf8}\n";
    }
    QCString trISOLang() override
    {
      return "zh";
    }
    QCString getLanguageString() override
    {
      return "0x804 Chinese (PRC)";
    }
    QCString latexFontenc() override
    {
      return "";
    }
    QCString latexDocumentPre() override
    {
      return "\\begin{CJK}{UTF8}{gbsn}\n";
    }
    QCString latexDocumentPost() override
    {
      return "\\end{CJK}\n";
    }
    bool needsPunctuation() override
    {
      return false;
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions.
     */
    QCString trRelatedFunctions() override
    { return "相关函数"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(请注意: 这些不是成员函数.)"; }

    /*! header that is put before the detailed description of files,
     * classes and namespaces.
     */
    QCString trDetailedDescription() override
    { return "详细描述"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "详细信息"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "成员类型定义说明"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "成员枚举类型说明"; }

    /*! header that is put before the list of member function. */
    QCString trMemberFunctionDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "成员函数/过程/进程说明";
      }
      else
      {
        return "成员函数说明";
      }
    }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "结构体成员变量说明";
      }
      else
      {
        return "类成员变量说明";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "更多..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "所有成员列表"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "成员列表"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "成员的完整列表，这些成员属于"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ",包括所有继承而来的类成员"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result;
      result = "由" CN_SPC "Doyxgen" CN_SPC "通过分析" CN_SPC;
      if (!s.isEmpty()) result += (s+CN_SPC "的" CN_SPC);
      result+= "源代码自动生成.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "枚举名称"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "枚举值"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "定义于" CN_SPC; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "模块"; }

    /*! This is put above each page as a link to the class hierarchy */
     QCString trClassHierarchy() override
    { return "类继承关系"; }

    /*! This is put above each page as a link to the list of annotated class */
     QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "结构体";
      }
      else {
        return "类列表";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "文件列表"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "成员变量";
      }
      else {
        return "类成员";
      }
    }

    /*! This is put above each page as a link to all member of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        return "全局定义";
      } else {
        return "文件成员";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "相关页面"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "示例"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "搜索"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "这里给出了所有实体的层次化列表:";
      }
      else
      {
        return "此继承关系列表按字典顺序粗略的排序:" CN_SPC;
      }
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="文件，并附带简要说明:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "这里列出了所有结构体，并附带简要说明:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "这里列出了所有类并附带简要说明:";
      }
      else
      {
        return "这里列出了所有类、结构、联合以及接口定义等，并附带简要说明:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll) {
        result+="文档化的";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result+="结构体和联合体的成员变量";
      }
      else {
        result+="类成员";
      }
      result+="，并附带";
      if (extractAll) {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
          result+="结构体或联合的详细说明:";
        }
        else {
          result+="类的详细说明:";
        }
      }
      else {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
          result+="结构或联合所属的文件:";
        }
        else {
          result+="类所属的文件:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll)
        result +="文档化的";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C)) {
        result +="函数、变量、宏、枚举和类型定义等";
      }
      else {
        result +="文件成员";
      }
      result+="，并附带";

      if (extractAll)
        result+="其所属的文件:";
      else
        result+="其详细说明:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "这里列出了所有示例:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "这里列出了所有相关页面:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "这里列出了所有模块:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "文档"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "模块索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "继承关系索引"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "结构体索引";
      }
      else {
        return "类索引";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "文件索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "模块说明"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "结构体说明";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else {
        return "类说明";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trFileDocumentation() override
    { return "文件说明"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "参考手册"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "宏定义"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "类型定义"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "枚举"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "函数"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "变量"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "枚举值"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "宏定义说明"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "类型定义说明"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "枚举类型说明"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "函数说明"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "变量说明"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "结构体";
      }
      else {
        return "类";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="生成于" CN_SPC+date;
      if (!projName.isEmpty()) result+=CN_SPC ", 为" CN_SPC+projName;
      result+="使用" CN_SPC;
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "类" CN_SPC+clName+CN_SPC "继承关系图:";
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
    { return "返回"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "参见"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "参数"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "异常"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "制作者"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "命名空间列表"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="命名空间定义，附带简要说明:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "友元"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "友元及相关函数文档"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      QCString result=clName;
      if (isTemplate) result+=CN_SPC "模板";
      switch(compType)
      {
        case ClassDef::Class:  result+="类"; break;
        case ClassDef::Struct: result+="结构体"; break;
        case ClassDef::Union:  result+="联合体"; break;
        case ClassDef::Interface:  result+="接口"; break;
        case ClassDef::Protocol:   result+="协议"; break;
        case ClassDef::Category:   result+="分类"; break;
        case ClassDef::Exception:  result+="异常"; break;
        default: break;
      }
      result+=CN_SPC "参考";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=CN_SPC "文件参考";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    { QCString result=namespaceName;
      result+=CN_SPC "命名空间参考";
      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers() override
    { return "Public 成员函数"; }

    QCString trPublicSlots() override
    { return "Public 槽"; }

    QCString trSignals() override
    { return "信号"; }

    QCString trStaticPublicMembers() override
    { return "静态 Public 成员函数"; }

    QCString trProtectedMembers() override
    { return "Protected 成员函数"; }

    QCString trProtectedSlots() override
    { return "Protected 槽"; }

    QCString trStaticProtectedMembers() override
    { return "静态 Protected 成员函数"; }

    QCString trPrivateMembers() override
    { return "Private 成员函数"; }

    QCString trPrivateSlots() override
    { return "Private 槽"; }

    QCString trStaticPrivateMembers() override
    { return "静态 Private 成员函数"; }

    // end of member sections

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
            result+="," CN_SPC;
          else                // the fore last entry
            result+=CN_SPC ", 以及" CN_SPC;
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    { return "继承自" CN_SPC+trWriteList(numEntries)+CN_SPC ".";  }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "被" CN_SPC+trWriteList(numEntries)+CN_SPC "继承.";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "重载" CN_SPC+trWriteList(numEntries)+CN_SPC ".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "被" CN_SPC+trWriteList(numEntries)+CN_SPC "重载.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "命名空间成员"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="命名空间成员，并附带";
      if (extractAll)
        result+="其对应命名空间的说明文档:";
      else
        result+="其所属的命名空间:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "命名空间索引"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "命名空间文档"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    {
      return "命名空间";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool) override
    { // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="该";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"设计单元":"类"; break;
        case ClassDef::Struct:     result+="结构体"; break;
        case ClassDef::Union:      result+="联合体"; break;
        case ClassDef::Interface:  result+="接口"; break;
        case ClassDef::Protocol:   result+="协议"; break;
        case ClassDef::Category:   result+="分类"; break;
        case ClassDef::Exception:  result+="异常"; break;
        default: break;
      }
      result+="的文档由以下文件生成:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "返回值"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "首页"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "在文件" CN_SPC "@1" CN_SPC "第" CN_SPC "@0" CN_SPC "行定义.";
    }

    QCString trDefinedInSourceFile() override
    {
      return "在文件" CN_SPC "@0" CN_SPC "中定义.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "弃用";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return clName+CN_SPC "的协作图:";
    }

    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return fName+CN_SPC "的引用(Include)关系图:";
    }

    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "构造及析构函数说明";
    }

    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "浏览该文件的源代码.";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "浏览该文件的文档.";
    }

    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "前置条件";
    }

    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "后置条件";
    }

    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "不变性";
    }

    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "初始值:";
    }

    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "代码";
    }

    QCString trGraphicalHierarchy() override
    {
      return "类继承关系图";
    }

    QCString trGotoGraphicalHierarchy() override
    {
      return "浏览类继承关系图";
    }

    QCString trGotoTextualHierarchy() override
    {
      return "浏览类继承关系表";
    }

    QCString trPageIndex() override
    {
      return "页面索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "注解";
    }

    QCString trPublicTypes() override
    {
      return "Public 类型";
    }

    QCString trPublicAttribs() override
    {
       if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
       {
         return "成员变量";
       }
       else
       {
         return "Public 属性";
       }
    }

    QCString trStaticPublicAttribs() override
    {
      return "静态 Public 属性";
    }

    QCString trProtectedTypes() override
    {
      return "Protected 类型";
    }

    QCString trProtectedAttribs() override
    {
      return "Protected 属性";
    }

    QCString trStaticProtectedAttribs() override
    {
      return "静态 Protected 属性";
    }

    QCString trPrivateTypes() override
    {
      return "Private 类型";
    }

    QCString trPrivateAttribs() override
    {
      return "Private 属性";
    }

    QCString trStaticPrivateAttribs() override
    {
      return "静态 Private 属性";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "待办事项";
    }

    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "待办事项列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "被这些函数引用";
    }

    QCString trRemarks() override
    {
      return "备注";
    }

    QCString trAttention() override
    {
      return "注意";
    }

    QCString trInclByDepGraph() override
    {
      return "此图展示该文件被哪些文件直接或间接地引用了:";
    }

    QCString trSince() override
    {
      return "自从";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "图例";
    }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "本页将向您解释如何理解由" CN_SPC "doxygen" CN_SPC "生成的图.<p>\n"
        "考虑如下例子:\n"
        "\\code\n"
        "/*! 由于截断而使" CN_SPC "Invisible" CN_SPC "不可见 */\n"
        "class Invisible { };\n\n"
        "/*! Truncated" CN_SPC "的继承关系将被隐藏 */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* 没有被" CN_SPC "doxygen" CN_SPC "文档化的类 */\n"
        "class Undocumented { };\n\n"
        "/*! public" CN_SPC "继承关系的基类 */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! 一个模板类 */\n"
        "template<class T> class Templ { };\n\n"
        "/*! protected" CN_SPC "继承关系的基类 */\n"
        "class ProtectedBase { };\n\n"
        "/*! private" CN_SPC "继承关系的基类 */\n"
        "class PrivateBase { };\n\n"
        "/*! 被" CN_SPC "Inherited" CN_SPC "使用的类 */\n"
        "class Used { };\n\n"
        "/*! 继承自其它若干类的超级类 */\n"
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
        "结果将会生成以下图:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "上图中的矩形含义为:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%灰色填充的矩形 表示生成上图的结构体或类.</li>\n"
        "<li>%黑色边框的矩形 表示已经被文档化的结构体或类.</li>\n"
        "<li>%灰色边框的矩形 表示未被文档化的结构体或类.</li>\n"
        "<li>%红色边框的矩形 表示该结构体或类的关系没有被完全显示."
        "%如果生成的图超出了指定的尺寸范围，有一些关系就会被截断而无法显示.</li>\n"
        "</ul>\n"
        "<p>\n"
        "箭头的含义为:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%蓝色的箭头 表示" CN_SPC "public" CN_SPC "继承关系.</li>\n"
        "<li>%深绿色的箭头 表示" CN_SPC "protected" CN_SPC "继承关系.</li>\n"
        "<li>%深红色的箭头 表示" CN_SPC "private" CN_SPC "继承关系.</li>\n"
        "<li>%紫色虚线箭头 表示两个类之间的聚合关系. 可以通过箭头旁标明的变量访问箭头指向的类或结构体实例.</li>\n"
        "<li>%黄色虚线箭头 表示模板类实例和模板类之间的关系. 箭头旁边标明了模板类实例化时所用的模板参数.</li>\n"
        "</ul>\n";
    }

    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "图例";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "测试";
    }

    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "测试列表";
    }

//////////////////////////////////////////////////////////////////////////
//// new since 1.2.2
////////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "属性";
    }

    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "属性说明";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "结构体";
      }
      else
      {
        return "类";
      }
    }

    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "包" CN_SPC+name;
    }


    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
       return "这里列出所有的包，附带简要说明(如果有的话):";
    }

    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "包";
    }

    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "值:";
    }

////////////////////////////////////////////////////////////////////////////
//// new since 1.2.6
////////////////////////////////////////////////////////////////////////////
    /*! Used as a marker that is put before a \\bug item */
    QCString trBug () override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList () override
    {
      return "Bug" CN_SPC "列表";
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
      return "936";
    }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "134";
    }


    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "索引";
    }

    // Following translations (from trClass to trAuthor) are used for languages
    // having plural / singular form for nouns, but Chinese does not distinguish
    // plural form from singular form, so the returned value is constant.

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool /*first_capital*/, bool /*singular*/) override
    {
      return "类";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool /*first_capital*/, bool /*singular*/) override
    {
      return "文件";

    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool /*first_capital*/, bool /*singular*/) override
    {
      return "命名空间";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool /*first_capital*/, bool /*singular*/) override
    {
      return "组";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool /*first_capital*/, bool /*singular*/) override
    {
      return "页";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool /*first_capital*/, bool /*singular*/) override
    {
      return "成员";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool /*first_capital*/, bool /*singular*/) override
    {
      return "全局";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool /*first_capital*/, bool /*singular*/) override
    {
      return "作者";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "引用了";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "实现了" CN_SPC+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "在" CN_SPC+trWriteList(numEntries)+CN_SPC "内被实现.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "目录";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "弃用列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "事件";
    }

    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "事件说明";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "包类型";
    }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "包函数";
    }
    QCString trPackageMembers() override
    {
      return "包成员";
    }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "静态包函数";
    }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "包属性";
    }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "静态包属性";
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
      return "函数调用图:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "搜索结果";
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
        return "抱歉,未找到与您查询相符的文档.";
      }
      else if (numDocuments==1)
      {
        return "找到<b>1</b>篇与您查询相符的文档.";
      }
      else
      {
        return "找到<b>$num</b>篇与您查询相符的文档."
               "优先显示最符合的文档.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "符合的结果:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + CN_SPC "源文件";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    {
      return "目录结构";
    }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    {
      return "目录说明";
    }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "目录"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    {
      QCString result=dirName;
      result+=CN_SPC "目录参考";
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool /*first_capital*/, bool /*singular*/) override
    {
      return "目录";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
      return "这是为便于使用而提供的一个重载成员函数."
             "与上面的函数相比，它接受不同类型的参数.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "这是这个函数的调用关系图:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    {
      return "枚举变量说明";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "成员函数/子程序说明"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "数据类型列表"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "数据项"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "带简要描述的数据类型列表:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll)
      {
        result+="文档化的";
      }
      result+="数据类型成员,并附带";
      if (!extractAll)
      {
        result+="其数据结构说明文档:";
      }
      else
      {
        result+="其所属的数据类型:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "数据类型索引"; }


    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "数据类型文档"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "函数/子程序"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "函数/子程序说明"; }


    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "数据类型"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "模块列表"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="模块，并附带简要说明:";
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
        case ClassDef::Class:      result+=CN_SPC "模块"; break;
        case ClassDef::Struct:     result+=CN_SPC "类型"; break;
        case ClassDef::Union:      result+=CN_SPC "联合体"; break;
        case ClassDef::Interface:  result+=CN_SPC "接口"; break;
        case ClassDef::Protocol:   result+=CN_SPC "协议"; break;
        case ClassDef::Category:   result+=CN_SPC "目录"; break;
        case ClassDef::Exception:  result+=CN_SPC "异常"; break;
        default: break;
      }
      if (isTemplate) result+="模板" CN_SPC;
      result+="参考手册";
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result += CN_SPC "模块参考手册";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "模块成员"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      if(!extractAll) {
        return "这里是所有文档化的模块成员列表，含有到每个成员所在模块的文档的链接:";
      } else {
        return "这里是模块成员列表，含有到成员所属的模块的链接:";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "模块索引"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool /*first_capital*/, bool /*singular*/) override
    {
      return "模块";
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool) override
    {
      QCString result="该";
      switch(compType)
      {
        case ClassDef::Class:      result+=CN_SPC "模块"; break;
        case ClassDef::Struct:     result+=CN_SPC "结构体"; break;
        case ClassDef::Union:      result+=CN_SPC "联合体"; break;
        case ClassDef::Interface:  result+=CN_SPC "接口"; break;
        case ClassDef::Protocol:   result+=CN_SPC "协议"; break;
        case ClassDef::Category:   result+=CN_SPC "目录"; break;
        case ClassDef::Exception:  result+=CN_SPC "异常"; break;
        default: break;
      }
      result+=CN_SPC "的文档由以下文件生成:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool /*first_capital*/, bool /*singular*/) override
    {
      return "类型";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool /*first_capital*/, bool /*singular*/) override
    {
      return "子程序";
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "类型限制";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      // return QCString(name)+" Relation";
      // unsure
      return QCString(name)+CN_SPC "关系";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "载入中...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "全局命名空间";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "搜索中...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "未找到";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    QCString trFileIn(const QCString &name) override
    {
      return QCString("文件在")+CN_SPC+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "在" CN_SPC+name+CN_SPC "中引用";
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
      static const char *days[]   = { "一","二","三","四","五","六","日" };
      static const char *months[] = { "一","二","三","四","五","六","七","八","九","十","十一","十二" };

      QCString sdate;

      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%d年" CN_SPC "%s月" CN_SPC "%d日" CN_SPC "星期%s",year, months[month-1], day, days[dayOfWeek-1]);
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
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月" };
      static const char *months_full[]  = { "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月" };
      return full? months_full[month-1] : months_short[month-1];
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
    { return "参考文献"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "版权所有"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    {
      return QCString(name)+CN_SPC "的目录依赖关系图";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "详情级别"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "模板参数"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "和" CN_SPC+number+CN_SPC "更多..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool /*single*/) override
    { return "枚举说明文档从下列文件生成:"; }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+CN_SPC "枚举类型参考"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+CN_SPC "继承自" CN_SPC+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "额外继承的成员函数"; }

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "开启" : "关闭";
      return "点击" CN_SPC+opt+CN_SPC "面板同步";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "由 category @0 提供.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "扩展类 @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "类方法";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "实例方法";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "成员函数文档";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "导出的接口"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "包含的服务"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "常量组"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+= CN_SPC "常量组参考";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+= CN_SPC "服务参考";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+= CN_SPC "单例参考";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool /* single */) override
    {
      // single is true implies a single file
      return "该服务的文档由下列文件生成:";
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool /* single */) override
    {
      // single is true implies a single file
      return "该单例的文档由下列文件生成:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "设计单元层次结构"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "设计单元列表"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "设计单元成员"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "这是所有设计单元成员的列表，以及指向他们所属实体的链接:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "设计单元索引"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "设计单元"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "函数/调用过程/进程语句"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool /*single*/) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          return "库";
        case VhdlSpecifier::PACKAGE:
          return "包";
        case VhdlSpecifier::SIGNAL:
          return "信号";
        case VhdlSpecifier::COMPONENT:
          return "元件";
        case VhdlSpecifier::CONSTANT:
          return "常量";
        case VhdlSpecifier::ENTITY:
          return "实体";
        case VhdlSpecifier::TYPE:
          return "类型";
        case VhdlSpecifier::SUBTYPE:
          return "子类型";
        case VhdlSpecifier::FUNCTION:
          return "函数";
        case VhdlSpecifier::RECORD:
          return "记录";
        case VhdlSpecifier::PROCEDURE:
          return "过程";
        case VhdlSpecifier::ARCHITECTURE:
          return "结构体";
        case VhdlSpecifier::ATTRIBUTE:
          return "属性";
        case VhdlSpecifier::PROCESS:
          return "进程语句";
        case VhdlSpecifier::PORT:
          return "端口";
        case VhdlSpecifier::USE:
          return "使用语句";
        case VhdlSpecifier::GENERIC:
          return "类属";
        case VhdlSpecifier::PACKAGE_BODY:
          return "包体";
        case VhdlSpecifier::UNITS:
          return "单元";
        case VhdlSpecifier::SHAREDVARIABLE:
          return "共享变量";
        case VhdlSpecifier::VFILE:
          return "文件";
        case VhdlSpecifier::GROUP:
          return "组";
        case VhdlSpecifier::INSTANTIATION:
          return "实例化";
        case VhdlSpecifier::ALIAS:
          return "别名";
        case VhdlSpecifier::CONFIG:
          return " 配置";
        case VhdlSpecifier::MISCELLANEOUS:
          return "混合运算";
        case VhdlSpecifier::UCF_CONST:
          return "约束";
        default:
          return "类";
      }
    }
    QCString trCustomReference(const QCString &name) override
    { return QCString(name)+" 引用"; }

    /* Slice */
    QCString trConstants() override
    {
        return "常量";
    }
    QCString trConstantDocumentation() override
    {
        return "常量文档";
    }
    QCString trSequences() override
    {
        return "序列";
    }
    QCString trSequenceDocumentation() override
    {
        return "序列文档";
    }
    QCString trDictionaries() override
    {
        return "字典";
    }
    QCString trDictionaryDocumentation() override
    {
        return "字典文档";
    }
    QCString trSliceInterfaces() override
    {
        return "接口";
    }
    QCString trInterfaceIndex() override
    {
        return "接口索引";
    }
    QCString trInterfaceList() override
    {
        return "接口列表";
    }
    QCString trInterfaceListDescription() override
    {
        return "以下是带有简要说明的接口:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "接口层次结构";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "此继承列表大致按字母顺序排序:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "接口文档";
    }
    QCString trStructs() override
    {
        return "结构";
    }
    QCString trStructIndex() override
    {
        return "结构索引";
    }
    QCString trStructList() override
    {
        return "结构列表";
    }
    QCString trStructListDescription() override
    {
        return "以下是带有简要说明的结构:";
    }
    QCString trStructDocumentation() override
    {
        return "结构文档";
    }
    QCString trExceptionIndex() override
    {
        return "异常索引";
    }
    QCString trExceptionList() override
    {
        return "异常列表";
    }
    QCString trExceptionListDescription() override
    {
        return "以下是带有简要说明的异常:";
    }
    QCString trExceptionHierarchy() override
    {
        return "异常层次结构";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "此继承列表大致按字母顺序排序:";
    }
    QCString trExceptionDocumentation() override
    {
        return "异常文档";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result=clName;
      if (isLocal) result+=CN_SPC "局部";
      switch(compType)
      {
        case ClassDef::Class:      result+=CN_SPC "类"; break;
        case ClassDef::Struct:     result+=CN_SPC "结构"; break;
        case ClassDef::Union:      result+=CN_SPC "联合"; break;
        case ClassDef::Interface:  result+=CN_SPC "接口"; break;
        case ClassDef::Protocol:   result+=CN_SPC "协议"; break;
        case ClassDef::Category:   result+=CN_SPC "类别"; break;
        case ClassDef::Exception:  result+=CN_SPC "异常"; break;
        default: break;
      }
      result+="引用";
      return result;
    }
    QCString trOperations() override
    {
        return "操作";
    }
    QCString trOperationDocumentation() override
    {
        return "操作文档";
    }
    QCString trDataMembers() override
    {
        return "数据成员";
    }
    QCString trDataMemberDocumentation() override
    {
        return "数据成员文档";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "设计单元文档"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool /* first_capital */, bool /* singular */) override
    {
      return "概念";
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result=conceptName;
      result+=CN_SPC "概念参考";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "概念列表"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "概念索引"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "概念文档"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="这里是所有";
      if (!extractAll) result+="文档化的";
      result+="概念, 并附有简要说明:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "概念定义";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "包列表"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
    { return "流程图:"; }

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
    { return "相关符号"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(注意: 这些不是成员符号.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "友元及相关符号说明"; }

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
        case ClassDef::Struct:     result="结构体"; break;
        case ClassDef::Union:      result="联合体"; break;
        case ClassDef::Interface:  result="接口"; break;
        case ClassDef::Protocol:   result="协议"; break;
        case ClassDef::Category:   result="分类"; break;
        case ClassDef::Exception:  result="异常"; break;
        case ClassDef::Service:    result="服务"; break;
        case ClassDef::Singleton:  result="单例"; break;
        default: break;
      }
      return result;
    }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="这里是所有";
      if (!extractAll) result+="文档化的";

      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="函数、变量、定义、枚举和类型定义";
          }
          else
          {
            result+="文件成员";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="函数";
          break;
        case FileMemberHighlight::Variables:
          result+="变量";
          break;
        case FileMemberHighlight::Typedefs:
          result+="类型定义";
          break;
        case FileMemberHighlight::Sequences:
          result+="序列";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="字典";
          break;
        case FileMemberHighlight::Enums:
          result+="枚举";
          break;
        case FileMemberHighlight::EnumValues:
          result+="枚举值";
          break;
        case FileMemberHighlight::Defines:
          result+="宏";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result+=", 及指向";
      if (extractAll)
        result+="其所属文件";
      else
        result+="其文档";
      result+="的链接:" CN_SPC;
      return result;
    }
    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="这里是所有";
      if (!extractAll)
      {
        result+="文档化的";
      }

      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="结构体及联合体成员";
          }
          else
          {
            result+="类成员";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="函数";
          break;
        case ClassMemberHighlight::Variables:
          result+="变量";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="类型定义";
          break;
        case ClassMemberHighlight::Enums:
          result+="枚举";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="枚举值";
          break;
        case ClassMemberHighlight::Properties:
          result+="属性";
          break;
        case ClassMemberHighlight::Events:
          result+="事件";
          break;
        case ClassMemberHighlight::Related:
          result+="相关符号";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result+=", 及指向";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="结构体 (或联合体) 中所有成员文档";
        }
        else
        {
          result+="类中所有成员文档";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="其所属结构体 (或联合体)";
        }
        else
        {
          result+="其所属类";
        }
      }
      result+="的链接:" CN_SPC;
      return result;
    }
    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="这里是所有";
      if (!extractAll) result+="文档化的";
      result+="命名空间";
      QCString singularResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="成员";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult="函数";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult="变量";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult="类型定义";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult="序列";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult="字典";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult="枚举";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult="枚举值";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+=singularResult;
      result+=", 及指向";
      if (extractAll)
        result+="每个" + singularResult + "所属命名空间文档";
      else
        result+="其所属命名空间";
      result+="的链接:" CN_SPC;
      return result;
    }
    QCString trDefinition() override  { return "定义";}
    QCString trDeclaration() override { return "声明";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    QCString trTopics() override
    { return "专题"; }
    QCString trTopicDocumentation() override
    { return "专题文档"; }
    QCString trTopicList() override
    { return "专题列表"; }
    QCString trTopicIndex() override
    { return "专题索引"; }
    QCString trTopicListDescription() override
    { return "这里是所有专题及其简介:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="这里是所有";
      if (!extractAll) result+="文档化的";
      result+="模块";
      QCString singularResult = "";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          singularResult="成员";
          break;
        case ModuleMemberHighlight::Functions:
          singularResult="函数";
          break;
        case ModuleMemberHighlight::Variables:
          singularResult="变量";
          break;
        case ModuleMemberHighlight::Typedefs:
          singularResult="类型定义";
          break;
        case ModuleMemberHighlight::Enums:
          singularResult="枚举";
          break;
        case ModuleMemberHighlight::EnumValues:
          singularResult="枚举值";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result+=singularResult;
      result+=", 及指向";
      if (extractAll)
        result+="每个" + singularResult + "的模块文档";
      else
        result+="其所属模块";
      result+="的链接:" CN_SPC;
      return result;
    }
    QCString trExportedModules() override
    {
      return "导出的模块";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "复制到剪贴板";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    QCString trImportant() override
    {
      return "重要事项";
    }
};

#endif

