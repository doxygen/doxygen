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

#ifndef TRANSLATOR_CN_H
#define TRANSLATOR_CN_H

#include "translator.h"

/*!
   If you want insert a space whenever Chinese meets English charactors, set
   CN_SPC to " ", else null.
*/
#define CN_SPC

class TranslatorChinese : public Translator
{
  public:
    QCString idLanguage()
    { return "chinese"; }
    QCString idLanguageCharset()
    { return "gb2312"; }
    QCString latexBabelPackage()
    { return "chinese"; }
    QCString trRelatedFunctions()
    { return "相关函数"; }
    QCString trRelatedSubscript()
    { return "（注意：这些不是成员函数）"; }
    QCString trDetailedDescription()
    { return "详悉描述"; }
    QCString trMemberTypedefDocumentation()
    { return "成员类型定义文档"; }
    QCString trMemberEnumerationDocumentation()
    { return "成员枚举类型文档"; }
    QCString trEnumerationValueDocumentation()
    { return "成员枚举值文档"; }
    QCString trMemberFunctionDocumentation()
    { return "成员函数文档"; }
    QCString trMemberDataDocumentation()
    { return "成员数据文档"; }
    QCString trMore()
    { return "更多..."; }
    QCString trListOfAllMembers()
    { return "所有成员的列表。"; }
    QCString trMemberList()
    { return "成员列表"; }
    QCString trThisIsTheListOfAllMembers()
    { return "成员的完整列表，这些成员属于"CN_SPC; }
    QCString trIncludingInheritedMembers()
    { return "，包括所有继承而来的成员"; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result;
      if (s) result=(QCString)"为"CN_SPC+s+"，";
      result+="由"CN_SPC"Doyxgen"CN_SPC"通过分析源代码自动生成。"; 
      return result;
    }
    QCString trEnumName()
    { return "枚举名称"; }
    QCString trEnumValue()
    { return "枚举值"; }
    QCString trDefinedIn()
    { return "定义于"CN_SPC; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"这是头文件"CN_SPC+f+CN_SPC"的源代码。"; }
    QCString trModules()
    { return "模块"; }
    QCString trClassHierarchy()
    { return "类继承关系"; }
    QCString trCompoundList()
    { return "组合类型列表"; }
    QCString trFileList()
    { return "文件列表"; }
    QCString trHeaderFiles()
    { return "头文件"; }
    QCString trCompoundMembers()
    { return "组合类型成员"; }
    QCString trFileMembers()
    { return "文件成员"; }
    QCString trRelatedPages()
    { return "相关页面"; }
    QCString trExamples()
    { return "示例"; }
    QCString trSearch()
    { return "搜索"; }
    QCString trClassHierarchyDescription()
    { return "此列表基本按字典顺序排序："; }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="文件，附带简要说明：";
      return result;
    }
    QCString trCompoundListDescription()
    { return "这里列出所有类、结构、联合以及接口定义，附带简要说明："; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="类成员，附带";
      if (extractAll) result+="所在类的文档的链接：";
      else result+="所在类的链接：";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="文件成员，附带";
      if (extractAll) result+="所在文件的文档的链接：";
      else result+="所在文件的链接：";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "这里列出组成API的头文件："; }
    QCString trExamplesDescription()
    { return "这里列出所有示例："; }
    QCString trRelatedPagesDescription()
    { return "这里列出所有相关的页面："; }
    QCString trModulesDescription()
    { return "这里列出所有模块"; }
    QCString trNoDescriptionAvailable()
    { return "无可用文档"; }

    QCString trDocumentation()
    { return "文档"; }
    QCString trModuleIndex()
    { return "模块索引"; }
    QCString trHierarchicalIndex()
    { return "继承关系索引"; }
    QCString trCompoundIndex()
    { return "组合类型索引"; }
    QCString trFileIndex() 
    { return "文件索引"; }
    QCString trModuleDocumentation()
    { return "模块文档"; }
    QCString trClassDocumentation()
    { return "类文档"; }
    QCString trFileDocumentation()
    { return "文件文档"; }
    QCString trExampleDocumentation()
    { return "示例文档"; }
    QCString trPageDocumentation()
    { return "页面文档"; }
    QCString trReferenceManual()
    { return "参考手册"; }

    QCString trDefines()
    { return "宏定义"; }
    QCString trFuncProtos()
    { return "函数原型"; }
    QCString trTypedefs()
    { return "类型定义"; }
    QCString trEnumerations()
    { return "枚举"; }
    QCString trFunctions()
    { return "函数"; }
    QCString trVariables()
    { return "变量"; }
    QCString trEnumerationValues()
    { return "枚举值"; }
    QCString trAuthor()
    { return "作者"; }
    QCString trDefineDocumentation()
    { return "宏定义文档"; }
    QCString trFunctionPrototypeDocumentation()
    { return "函数原型文档"; }
    QCString trTypedefDocumentation()
    { return "类型定义文档"; }
    QCString trEnumerationTypeDocumentation()
    { return "枚举类型文档"; }
    QCString trFunctionDocumentation()
    { return "函数文档"; }
    QCString trVariableDocumentation()
    { return "变量文档"; }
    QCString trCompounds()
    { return "组合类型"; }
    QCString trFiles()
    { return "文件"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generated at "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" by";
      return result;
    }
    QCString trWrittenBy()
    {
      return "written by";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"继承图，类"CN_SPC+clName;
    }
    QCString trForInternalUseOnly()
    { return "仅限内部使用。"; }
    QCString trReimplementedForInternalReasons()
    { return "由于内部原因被重载；但不影响API"; 
    }
    QCString trWarning()
    { return "警告"; }
    QCString trBugsAndLimitations()
    { return "BUG"CN_SPC"与局限"; }
    QCString trVersion()
    { return "版本"; }
    QCString trDate()
    { return "日期"; }
    QCString trAuthors()
    { return "作者"; }
    QCString trReturns()
    { return "返回"; }
    QCString trSeeAlso()
    { return "参见"; }
    QCString trParameters()
    { return "参数"; }
    QCString trExceptions()
    { return "异常"; }
    QCString trGeneratedBy()
    { return "制作者"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return "命名空间列表"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="命名空间定义，附带简要说明：";
      return result;
    }
    QCString trFriends()
    { return "友元"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "友元及相关函数文档"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName;
      if (isTemplate) result+=CN_SPC"模板";
      switch(compType)
      {
        case ClassDef::Class:  result+="类"; break;
        case ClassDef::Struct: result+="结构"; break;
        case ClassDef::Union:  result+="联合"; break;
        case ClassDef::Interface:  result+="接口"; break;
        case ClassDef::Exception:  result+="异常"; break;
      }
      result+="参考";
      return result;
    }
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=CN_SPC"文件参考"; 
      return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=CN_SPC"命名空间参考";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return "公有成员"; }
    QCString trPublicSlots()
    { return "公有槽"; }
    QCString trSignals()
    { return "信号"; }
    QCString trStaticPublicMembers()
    { return "静态公有成员"; }
    QCString trProtectedMembers()
    { return "保护成员"; }
    QCString trProtectedSlots()
    { return "保护槽"; }
    QCString trStaticProtectedMembers()
    { return "静态保护成员"; }
    QCString trPrivateMembers()
    { return "私有成员"; }
    QCString trPrivateSlots()
    { return "私有槽"; }
    QCString trStaticPrivateMembers()
    { return "静态私有成员"; }
    // end of member sections 
    
    QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+="、";
          else                // the fore last entry
            result+=CN_SPC"及"CN_SPC;
        }
      }
      return result; 
    }
    
    QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "继承自"CN_SPC+trWriteList(numEntries)+"。";
    }
    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "被"CN_SPC+trWriteList(numEntries)+CN_SPC"继承.";
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "重载"CN_SPC+trWriteList(numEntries)+"。";
    }
    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "被"CN_SPC+trWriteList(numEntries)+CN_SPC"重载。";
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "命名空间成员"; }
    QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="命名空间成员，附带";
      if (extractAll) 
        result+="所在类的文档的链接：";
      else 
        result+="所在类的链接：";
      return result;
    }
    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "命名空间索引"; }
    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "命名空间文档"; }

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
      QCString result=(QCString)"该";
      switch(compType)
      {
        case ClassDef::Class:      result+="类"; break;
        case ClassDef::Struct:     result+="结构"; break;
        case ClassDef::Union:      result+="联合"; break;
        case ClassDef::Interface:  result+="接口"; break;
        case ClassDef::Exception:  result+="异常"; break;
      }
      result+="的文档由以下文件生成：";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "按字典顺序排序的列表"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "返回值"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "首页"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return "源代码";
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return "在文件"CN_SPC"@1"CN_SPC"第"CN_SPC"@0"CN_SPC"行定义。";
    }
    QCString trDefinedInSourceFile()
    {
      return "在文件"CN_SPC"@0"CN_SPC"中定义。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+CN_SPC"合作图：";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+CN_SPC"包含/依赖关系图：";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "构造及析构函数文档"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "浏览该文件的源代码。";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "浏览该文件的文档。";
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "前提条件";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "Postcondition";
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "初始化序列：";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "代码";
    }
    QCString trGraphicalHierarchy()
    {
      return "类继承关系图";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "浏览类继承关系图";
    }
    QCString trGotoTextualHierarchy()
    {
      return "浏览类继承关系表";
    }
    QCString trPageIndex()
    {
      return "页面索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return "注解";
    }
    QCString trPublicTypes()
    {
      return "公有类型";
    }
    QCString trPublicAttribs()
    {
      return "公有属性";
    }
    QCString trStaticPublicAttribs()
    {
      return "静态公有属性";
    }
    QCString trProtectedTypes()
    {
      return "保护类型";
    }
    QCString trProtectedAttribs()
    {
      return "保护属性";
    }
    QCString trStaticProtectedAttribs()
    {
      return "静态保护属性";
    }
    QCString trPrivateTypes()
    {
      return "私有类型";
    }
    QCString trPrivateAttribs()
    {
      return "私有属性";
    }
    QCString trStaticPrivateAttribs()
    {
      return "静态私有属性";
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
      return "TODO"CN_SPC"列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()
    {
      return "参考自";
    }
    QCString trRemarks()
    {
      return "评论";
    }
    QCString trAttention()
    {
      return "注意";
    }
    QCString trInclByDepGraph()
    {
      return "此图展示直接或间接包含该文件的文件：";
    }
    QCString trSince()
    {
      return "自从";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle()
    {
      return "图例";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs()
    {
      return 
        "本页向您展示如何理解由"CN_SPC"Doxygen"CN_SPC"生成的图形。<p>\n"
        "请考虑如下示例：\n"
        "\\code\n"
        "/*! 由于截取而使该类不可见 */\n"
        "class Invisible { };\n\n"
        "/*! 被截取的类，继承关系被隐藏起来了 */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* 没有被"CN_SPC"doxygen"CN_SPC"的注释文档化的类 */\n"
        "class Undocumented { };\n\n"
        "/*! 被公有继承的类 */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! 被保护继承的类 */\n"
        "class ProtectedBase { };\n\n"
        "/*! 被私有继承的类 */\n"
        "class PrivateBase { };\n\n"
        "/*! 被使用的类 */\n"
        "class Used { };\n\n"
        "/*! 继承了若干其它类的类 */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "如果在配置文件中指定了"CN_SPC"MAX_DOT_GRAPH_HEIGHT"CN_SPC"的值为200，"
        "则Doxygen将生成如下的图形："
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
        "<p>\n"
        "以上图形中的矩形有如下的含义：\n"
        "<ul>\n"
        "<li>被黑色填充的矩形代表当前的类或结构。\n"
        "<li>黑色边框的矩形代表<i>文档化</i>的类或结构。\n"
        "<li>灰色边框的矩形代表没有<i>文档化</i>的类或结构。\n"
        "<li>红色边框的矩形代表继承/包含关系没有被完整显示出的类或结构。如果一幅图像的尺"
        "寸大于指定尺寸，它将被截取。"
        "</ul>\n"
        "各个箭头有如下的含义：\n"
        "<ul>\n"
        "<li>深蓝色的箭头用于显示两个类之间的公有继承关系。\n"
        "<li>深绿色的箭头用于显示保护继承关系。\n"
        "<li>深红色的箭头用于显示私有继承关系。\n"
        "<li>紫色点状线条的箭头用于显示两个类之间包含或者使用的关系。通过箭头旁边的变量可以"
        "访问到箭头所指的类或结构。\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend()
    {
      return "图例";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "测试";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "测试列表";
    }

};

#endif
