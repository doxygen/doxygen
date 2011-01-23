/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
   If you want insert a space whenever Chinese meets English charactors, set
   CN_SPC to " ", else null.
*/
#define CN_SPC

class TranslatorChinese : public Translator
{
  public:
		/*! Used for identification of the language. The identification
		 * should not be translated. It should be replaced by the name
		 * of the language in English using lower-case characters only
		 * (e.g. "czech", "japanese", "russian", etc.). It sould be equal to
		 * the identification used in the language.cpp.
		 */
    
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }


		/*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    { return "gb2312"; }

		/*! used in the compound documentation before a list of related functions. 
		 */
    virtual QCString trRelatedFunctions()
    { return "相关函数"; }

		/*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "（注意：这些不是成员函数。）"; }

		/*! header that is put before the detailed description of files, 
		 * classes and namespaces. 
		 */
    virtual QCString trDetailedDescription()
    { return "详细描述"; }

		/*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "成员类型定义文档"; }

		/*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "成员枚举类型文档"; }

		/*! header that is put before the list of member function. */
    virtual QCString trMemberFunctionDocumentation()
    { return "成员函数文档"; }

		/*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) 
			{
				return "字段文档"; 
			}
			else {
				return "成员数据文档"; 
			}
		}

		/*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "更多..."; }

		/*! put in the class documention */
    virtual QCString trListOfAllMembers()
    { return "所有成员的列表。"; }

		/*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "成员列表"; }

		/*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "成员的完整列表，这些成员属于"CN_SPC; }

		/*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return "，包括所有继承而来的成员"; }

		/*! this is put at the author sections at the bottom of man pages.
		 *  parameter s is name of the project name.
		 */
    virtual QCString trGeneratedAutomatically(const char *s)
    {
			QCString result;
      if (s) result=(QCString)"为"CN_SPC+s+"，";
      result+="由"CN_SPC"Doyxgen"CN_SPC"通过分析源代码自动生成。"; 
      return result;
    }

		/*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "枚举名称"; }

		/*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "枚举值"; }

		/*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "定义于"CN_SPC; }

		// quick reference sections


		/*! This is put above each page as a link to the list of all groups of 
		 *  compounds or files (see the \\group command).
		 */
    virtual QCString trModules()
    { return "模块"; }

		/*! This is put above each page as a link to the class hierarchy */
 		virtual QCString trClassHierarchy()
    { return "类继承关系"; }

		/*! This is put above each page as a link to the list of annotated class */
 		virtual QCString trCompoundList()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				return "数据结构"; 
			}
			else {
				return "组合类型列表"; 
			}
		}

		/*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "文件列表"; }

		/*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) { 
				return "数据字段"; 
			}
			else {
				return "组合类型成员"; 
			}
		}

		/*! This is put above each page as a link to all member of files. */
    virtual QCString trFileMembers()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				return "全局定义"; 
			} else {
				return "文件成员"; 
			}
		}

		/*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "相关页面"; }

		/*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "示例"; }

    virtual QCString trSearch()
    { return "搜索"; }

    virtual QCString trClassHierarchyDescription()
    { return "此列表基本按字典顺序排序："; }

    virtual QCString trFileListDescription(bool extractAll)
    {
   		QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="文件，附带简要说明：";
      return result;
    }

    virtual QCString trCompoundListDescription()
    { 

			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "这里列出所有数据结构，附带简要说明："; 
			}
			else
			{
				return "这里列出所有类、结构、联合以及接口定义，附带简要说明："; 
			}
    }

    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
   		QCString result="这里列出所有";
      if (!extractAll) {
				result+="文档化的";
			}
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				result+="结构和联合的成员，附带";
			}
			else {
				result+="类成员，附带";
			}

      if (extractAll) {
				if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
					result+="所在结构／联合的文档的链接：";
				}
				else {
					result+="所在类的文档的链接：";
				}
			}
      else {
				if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
					result+="所属结构／联合的链接：";
				}
				else {
					result+="所属类的链接：";
				}
			}
      return result;
    }

    virtual QCString trFileMembersDescription(bool extractAll)
    {
   		QCString result="这里列出所有";
      if (!extractAll) 
				result +="文档化的";

			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				result +="函数，变量，宏，枚举和类型定义，附带";
			} 
			else {
				result +="文件成员，附带";
			}

      if (extractAll) 
				result+="所在文件的文档的链接：";
      else 
				result+="所在文件的链接：";
      return result;
    }

    virtual QCString trExamplesDescription()
    { return "这里列出所有示例："; }

    virtual QCString trRelatedPagesDescription()
    { return "这里列出所有相关的页面："; }

    virtual QCString trModulesDescription()
    { return "这里列出所有模块"; }

    virtual QCString trDocumentation()
    { return "文档"; }

    virtual QCString trModuleIndex()
    { return "模块索引"; }

    virtual QCString trHierarchicalIndex()
    { return "继承关系索引"; }

    virtual QCString trCompoundIndex()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "数据结构索引"; 
			} 
			else {
				return "组合类型索引"; 
			}
		}

    virtual QCString trFileIndex() 
    { return "文件索引"; }

    virtual QCString trModuleDocumentation()
    { return "模块文档"; }

    virtual QCString trClassDocumentation()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "数据结构文档";
			}
			else {
				return "类文档"; 
			}
		}

    virtual QCString trFileDocumentation()
    { return "文件文档"; }

    virtual QCString trExampleDocumentation()
    { return "示例文档"; }

    virtual QCString trPageDocumentation()
    { return "页面文档"; }

    virtual QCString trReferenceManual()
    { return "参考手册"; }

    virtual QCString trDefines()
    { return "宏定义"; }

    virtual QCString trFuncProtos()
    { return "函数原型"; }

    virtual QCString trTypedefs()
    { return "类型定义"; }

    virtual QCString trEnumerations()
    { return "枚举"; }

    virtual QCString trFunctions()
    { return "函数"; }

    virtual QCString trVariables()
    { return "变量"; }

    virtual QCString trEnumerationValues()
    { return "枚举值"; }


 		virtual QCString trDefineDocumentation()
    { return "宏定义文档"; }

 		virtual QCString trFunctionPrototypeDocumentation()
    { return "函数原型文档"; }

 		virtual QCString trTypedefDocumentation()
    { return "类型定义文档"; }

 		virtual QCString trEnumerationTypeDocumentation()
    { return "枚举类型文档"; }

 		virtual QCString trFunctionDocumentation()
    { return "函数文档"; }

 		virtual QCString trVariableDocumentation()
    { return "变量文档"; }

 		virtual QCString trCompounds()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{ 
				return "数据结构";
			}
			else {
				return "组合类型"; 
			}
		}

 		virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
   		QCString result=(QCString)"Generated at "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" by";
      return result;
    }

 		virtual QCString trWrittenBy()
    {
      return "written by";
    }

 		virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"继承图，类"CN_SPC+clName;
    }

 		virtual QCString trForInternalUseOnly()
    { return "仅限内部使用。"; }

 		virtual QCString trWarning()
    { return "警告"; }

 		virtual QCString trVersion()
    { return "版本"; }

 		virtual QCString trDate()
    { return "日期"; }

 		virtual QCString trReturns()
    { return "返回"; }

 		virtual QCString trSeeAlso()
    { return "参见"; }

 		virtual QCString trParameters()
    { return "参数"; }

 		virtual QCString trExceptions()
    { return "异常"; }

 		virtual QCString trGeneratedBy()
    { return "制作者"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trNamespaceList()
    { return "名字空间列表"; }

 		virtual QCString trNamespaceListDescription(bool extractAll)
    {
   		QCString result="这里列出所有";
      if (!extractAll) result+="文档化的";
      result+="名字空间定义，附带简要说明：";
      return result;
    }

 		virtual QCString trFriends()
    { return "友元"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trRelatedFunctionDocumentation()
    { return "友元及相关函数文档"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trCompoundReference(const char *clName,
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
        case ClassDef::Protocol:   result+="协议"; break;
        case ClassDef::Category:   result+="分类"; break;
        case ClassDef::Exception:  result+="异常"; break;
      }
      result+="参考";
      return result;
    }

 		virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
   		QCString result=fileName;
      result+=CN_SPC"文件参考"; 
      return result;
    }

 		virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
   		QCString result=namespaceName;
      result+=CN_SPC"名字空间参考";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
 		virtual QCString trPublicMembers()
    { return "公有成员"; }

 		virtual QCString trPublicSlots()
    { return "公有槽"; }

 		virtual QCString trSignals()
    { return "信号"; }

 		virtual QCString trStaticPublicMembers()
    { return "静态公有成员"; }

 		virtual QCString trProtectedMembers()
    { return "保护成员"; }

 		virtual QCString trProtectedSlots()
    { return "保护槽"; }

 		virtual QCString trStaticProtectedMembers()
    { return "静态保护成员"; }
		
 		virtual QCString trPrivateMembers()
    { return "私有成员"; }
		
 		virtual QCString trPrivateSlots()
    { return "私有槽"; }
		
 		virtual QCString trStaticPrivateMembers()
    { return "静态私有成员"; }
		
    // end of member sections 
    
 		virtual QCString trWriteList(int numEntries)
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
    
 		virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "继承自"CN_SPC+trWriteList(numEntries)+"。";
    }

 		virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "被"CN_SPC+trWriteList(numEntries)+CN_SPC"继承.";
    }
		
 		virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "重载"CN_SPC+trWriteList(numEntries)+"。";
    }
		
 		virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "被"CN_SPC+trWriteList(numEntries)+CN_SPC"重载。";
    }

 		virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "名字空间成员"; }
		
 		virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
   		QCString result="这里列出了所有";
      if (!extractAll) result+="文档化的";
      result+="名字空间成员，附带";
      if (extractAll) 
        result+="所在类的文档的链接：";
      else 
        result+="所在类的链接：";
      return result;
    }

 		virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "名字空间索引"; }

 		virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "名字空间文档"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
 		virtual QCString trNamespaces()
    {
      // return "Namespaces";
      return "名字空间";
    }

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
   		QCString result=(QCString)"该";
      switch(compType)
      {
        case ClassDef::Class:      result+="类"; break;
        case ClassDef::Struct:     result+="结构"; break;
        case ClassDef::Union:      result+="联合"; break;
        case ClassDef::Interface:  result+="接口"; break;
        case ClassDef::Protocol:   result+="协议"; break;
        case ClassDef::Category:   result+="分类"; break;
        case ClassDef::Exception:  result+="异常"; break;
      }
      result+="的文档由以下文件生成：";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
 		virtual QCString trAlphabeticalList()
    { return "按字典顺序排序的列表"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
 		virtual QCString trReturnValues()
    { return "返回值"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
 		virtual QCString trMainPage()
    { return "首页"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
 		virtual QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

                virtual QCString trDefinedAtLineInSourceFile()
    {
      return "在文件"CN_SPC"@1"CN_SPC"第"CN_SPC"@0"CN_SPC"行定义。";
    }

 		virtual QCString trDefinedInSourceFile()
    {
      return "在文件"CN_SPC"@0"CN_SPC"中定义。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trDeprecated()
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
 		virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+CN_SPC"合作图：";
    }

    /*! this text is put before an include dependency graph */
 		virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+CN_SPC"包含/依赖关系图：";
    }

    /*! header that is put before the list of constructor/destructors. */
 		virtual QCString trConstructorDocumentation()
    {
      return "构造及析构函数文档"; 
    }

    /*! Used in the file documentation to point to the corresponding sources. */
 		virtual QCString trGotoSourceCode()
    {
      return "浏览该文件的源代码。";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
 		virtual QCString trGotoDocumentation()
    {
      return "浏览该文件的文档。";
    }

    /*! Text for the \\pre command */
 		virtual QCString trPrecondition()
    {
      return "前置条件";
    }

    /*! Text for the \\post command */
 		virtual QCString trPostcondition()
    {
      return "后置条件";
    }

    /*! Text for the \\invariant command */
 		virtual QCString trInvariant()
    {
      return "不变性";
    }

    /*! Text shown before a multi-line variable/enum initialization */
 		virtual QCString trInitialValue()
    {
      return "初始化序列：";
    }

    /*! Text used the source code in the file index */
 		virtual QCString trCode()
    {
      return "代码";
    }

 		virtual QCString trGraphicalHierarchy()
    {
      return "类继承关系图";
    }

 		virtual QCString trGotoGraphicalHierarchy()
    {
      return "浏览类继承关系图";
    }

 		virtual QCString trGotoTextualHierarchy()
    {
      return "浏览类继承关系表";
    }

 		virtual QCString trPageIndex()
    {
      return "页面索引";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trNote()
    {
      return "注解";
    }

 		virtual QCString trPublicTypes()
    {
      return "公有类型";
    }

 		virtual QCString trPublicAttribs()
    {
			 if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			 { 
				 return "数据成员";
			 }
			 else {
				 return "公有属性";
			 }
    }

 		virtual QCString trStaticPublicAttribs()
    {
      return "静态公有属性";
    }

 		virtual QCString trProtectedTypes()
    {
      return "保护类型";
    }

 		virtual QCString trProtectedAttribs()
    {
      return "保护属性";
    }

 		virtual QCString trStaticProtectedAttribs()
    {
      return "静态保护属性";
    }

 		virtual QCString trPrivateTypes()
    {
      return "私有类型";
    }

 		virtual QCString trPrivateAttribs()
    {
      return "私有属性";
    }

 		virtual QCString trStaticPrivateAttribs()
    {
      return "静态私有属性";
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
      return "TODO"CN_SPC"列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trReferencedBy()
    {
      return "参考自";
    }

 		virtual QCString trRemarks()
    {
      return "评论";
    }

 		virtual QCString trAttention()
    {
      return "注意";
    }

 		virtual QCString trInclByDepGraph()
    {
      return "此图展示直接或间接包含该文件的文件：";
    }

 		virtual QCString trSince()
    {
      return "自从";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
 		virtual QCString trLegendTitle()
    {
      return "图例";
    }

    /*! page explaining how the dot graph's should be interpreted */
 		virtual QCString trLegendDocs()
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
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
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
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "如果在配置文件中指定了"CN_SPC"MAX_DOT_GRAPH_HEIGHT"CN_SPC"的值为200，"
        "则Doxygen将生成如下的图形："
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
 		virtual QCString trLegend()
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

//////////////////////////////////////////////////////////////////////////
//// new since 1.2.1
////////////////////////////////////////////////////////////////////////////
		/*! Used as a section header for KDE-2 IDL methods */
		virtual QCString trDCOPMethods()
		{
			return "DCOP"CN_SPC"方法";
		}

//////////////////////////////////////////////////////////////////////////
//// new since 1.2.2
////////////////////////////////////////////////////////////////////////////

		/*! Used as a section header for IDL properties */
		virtual QCString trProperties()
		{
			return "属性";
		}

		/*! Used as a section header for IDL property documentation */
		virtual QCString trPropertyDocumentation()
		{
			return "属性文档";
		}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "数据结构";
			}
			else
			{
				return "类";
			}
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"包 "+name;
    }
		
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "包列表";
    }

    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
			 return "这里列出所有的包，附带简要说明(如果有的话)："; 
    }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "包";
    }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return 	"值:";
    }

////////////////////////////////////////////////////////////////////////////
//// new since 1.2.6
////////////////////////////////////////////////////////////////////////////
		virtual QCString trBug ()
		{
			return "缺陷";
		}

		virtual QCString trBugList ()
		{
			return "缺陷列表";
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
      return "936";
    }

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "134";
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
			/*
   		QCString result((first_capital ? "Class" : "class"));
      if (!singular)  result+="es";
      return result; 
			*/
			return "类";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "File" : "file"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "文件";

    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "名字空间";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "组";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Page" : "page"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "页";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Member" : "member"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "成员";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "全局定义";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool /*first_capital*/, bool /*singular*/)
    {                                                                         
			/*
   		QCString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "作者";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "参考";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      /* return "Implements "+trWriteList(numEntries)+"."; */
      return "实现了"CN_SPC+trWriteList(numEntries)+"。";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      /* return "Implemented in "+trWriteList(numEntries)+"."; */
      return "在"CN_SPC+trWriteList(numEntries)+CN_SPC"内被实现。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      /* return "Table of Contents"; */
      return "目录";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
/*    return "Deprecated List";  */
      return "过时列表";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      /*      return "Events"; */
      return "事件";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      /* return "Event Documentation"; */
      return "事件文档";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      /* return "Package Types"; */
      return "模块类型";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      /* return "Package Functions"; */
      return "模块函数";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      /* return "Static Package Functions"; */
      return "静态模块函数";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      /* return "Package Attributes"; */
      return "模块属性";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      /* return "Static Package Attributes"; */
      return "静态模块属性";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      /* return "All"; */
      return "全部";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      /* return "Here is the call graph for this function:"; */
      return "函数调用图:";
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
      /* return "Search for"; */
      return "搜索";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      /* return "Search Results"; */
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
    virtual QCString trSearchResults(int numDocuments)
    {
      if (numDocuments==0)
      {
        /* return "Sorry, no documents matching your query."; */
        return "对不起，找不到与你的查询相符的文档。";
      }
      else if (numDocuments==1)
      {
        /* return "Found <b>1</b> document matching your query."; */
        return "找到<b>1</b>篇与你的查询相符的文档。";
      }
      else 
      {
        /* return "Found <b>$num</b> documents matching your query. "
               "Showing best matches first."; */
        return "找到<b>$num</b>篇与你的查询相符的文档。"
               "先显示最吻合的文档。";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      /* return "Matches:"; */
      return "符合的结果:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      /* return filename + " Source File"; */
      return filename + CN_SPC"源文件";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    /*     { return "Directory Hierarchy"; } */
    {
      return "目录结构";
    }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    /*     { return "Directory Documentation"; } */
    {
      return "目录文档";
    }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    /*     { return "Directories"; } */
    {
      return "目录";
    }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
/*     { return "This directory hierarchy is sorted roughly, " */
/*              "but not completely, alphabetically:"; */
/*     } */
    {
      return "目录结构仅经过粗略的排序";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    /*     { QCString result=dirName; result+=" Directory Reference"; return result; } */
    {
      QCString result=dirName;
      result+=CN_SPC"目录参考"; 
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool)
    { 
      /*       QCString result((first_capital ? "Director" : "director")); */
      /*       if (singular) result+="y"; else result+="ies"; */
      /*       return result;  */
      return "目录";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       /* return "This is an overloaded member function, "
              "provided for convenience. It differs from the above "
              "function only in what argument(s) it accepts."; */
      return "这是为便于使用而提供的一个重载成员函数。"
             "与上面的函数相比，它接受不同类型的参数。";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      /* return "Here is the caller graph for this function:"; */
      return "这是这个函数的调用图。";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { 
      /* return "Enumerator Documentation"; */
      return "枚举变量文档";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    // { return "Member Function/Subroutine Documentation"; }
    { return "成员函数及过程文档"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    // { return "Data Types List"; }
    { return "数据类型列表"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    // { return "Data Fields"; }
    { return "数据项"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    // { return "Here are the data types with brief descriptions:"; }
    { return "带简要描述的数据类型列表:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
    //   QCString result="Here is a list of all ";
    //   if (!extractAll)
    //   {
    //     result+="documented ";
    //   }
    //   result+="data types members";
    //   result+=" with links to ";
    //   if (!extractAll) 
    //   {
    //      result+="the data structure documentation for each member";
    //   }
    //   else 
    //   {
    //      result+="the data types they belong to:";
    //   }
    //   return result;
      if(!extractAll) {
        return "这里是有文档的数据类型成员列表，含有到每个成员的数据结构文档的链接";
      } else {
        return "这里是数据类型成员列表，含有到成员所属的数据类型的链接:";
      }

    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    // { return "Data Type Index"; }
    { return "数据类型索引"; }


    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    // { return "Data Type Documentation"; }
    { return "数据类型文档"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    //{ return "Functions/Subroutines"; }
    { return "函数/过程"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    //{ return "Function/Subroutine Documentation"; }
    { return "函数/过程文档"; }


    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    // { return "Data Types"; }
    { return "数据类型"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    // { return "Modules List"; }
    { return "模块列表"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      // QCString result="Here is a list of all ";
      // if (!extractAll) result+="documented ";
      // result+="modules with brief descriptions:";
      // return result;
      if(!extractAll) {
        return "带简要描述的有文档的模块列表:";
      } else {
        return "带简要描述的模块列表:";
      }
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      // QCString result=(QCString)clName;
      // switch(compType)
      // {
      //   case ClassDef::Class:      result+=" Module"; break;
      //   case ClassDef::Struct:     result+=" Type"; break;
      //   case ClassDef::Union:      result+=" Union"; break;
      //   case ClassDef::Interface:  result+=" Interface"; break;
      //   case ClassDef::Protocol:   result+=" Protocol"; break;
      //   case ClassDef::Category:   result+=" Category"; break;
      //   case ClassDef::Exception:  result+=" Exception"; break;
      // }
      // if (isTemplate) result+=" Template";
      // result+=" Reference";
      // return result;
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=CN_SPC"模块"; break;
        case ClassDef::Struct:     result+=CN_SPC"类型"; break;
        case ClassDef::Union:      result+=CN_SPC"联合"; break;
        case ClassDef::Interface:  result+=CN_SPC"界面"; break;
        case ClassDef::Protocol:   result+=CN_SPC"接口"; break;
        case ClassDef::Category:   result+=CN_SPC"目录"; break;
        case ClassDef::Exception:  result+=CN_SPC"异常"; break;
      }
      if (isTemplate) result+="模板";
      result+="参考手册";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      // result+=" Module Reference";        
      result += CN_SPC"模块参考手册";
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    // { return "Module Members"; }
    { return "模块成员"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      // QCString result="Here is a list of all ";
      // if (!extractAll) result+="documented ";
      // result+="module members with links to ";
      // if (extractAll) 
      // {
      //   result+="the module documentation for each member:";
      // }
      // else 
      // {
      //   result+="the modules they belong to:";
      // }
      // return result;
      if(!extractAll) {
        return "这里是有文档的模块成员列表，含有到每个成员所在模块的文档的链接";
      } else {
        return "这里是模块成员列表，含有到成员所属的模块的链接:";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    // { return "Modules Index"; }
    { return "模块索引"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool, bool)
    {       
      // QCString result((first_capital ? "Module" : "module"));
      // if (!singular)  result+="s";
      // return result; 
      return "模块";
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      // QCString result=(QCString)"The documentation for this ";
      // switch(compType)
      // {
      //   case ClassDef::Class:      result+="module"; break;
      //   case ClassDef::Struct:     result+="type"; break;
      //   case ClassDef::Union:      result+="union"; break;
      //   case ClassDef::Interface:  result+="interface"; break;
      //   case ClassDef::Protocol:   result+="protocol"; break;
      //   case ClassDef::Category:   result+="category"; break;
      //   case ClassDef::Exception:  result+="exception"; break;
      // }
      // result+=" was generated from the following file";
      // if (single) result+=":"; else result+="s:";
      // return result;
      QCString result="该";
      switch(compType)
      {
        case ClassDef::Class:      result+=CN_SPC"模块"; break;
        case ClassDef::Struct:     result+=CN_SPC"类型"; break;
        case ClassDef::Union:      result+=CN_SPC"联合"; break;
        case ClassDef::Interface:  result+=CN_SPC"界面"; break;
        case ClassDef::Protocol:   result+=CN_SPC"接口"; break;
        case ClassDef::Category:   result+=CN_SPC"目录"; break;
        case ClassDef::Exception:  result+=CN_SPC"异常"; break;
      }
      result+="的文档由以下文件生成:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool, bool)
    { 
      return "类型";
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool, bool)
    { 
      return "子程序";
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "类型限制";
    }
	
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////
	virtual QCString trDirRelation(const char *name)
	{
		// return QCString(name)+" Relation";
		// unsure
		return QCString(name)+CN_SPC"关系";
	}
	
    virtual QCString trLoading()
	{
		return "载入中...";
	}
    
	virtual QCString trGlobalNamespace()
	{
		return "全局命名空间";
	}
	
    virtual QCString trSearching()
	{
		return "搜索中...";
	}
    
	virtual QCString trNoMatches()
	{
		return "未找到";
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDirDependency(const char *name)
	{
		// return (QCString)"Directory dependency graph for "+name;
		// unsure
		return QCString(name)+CN_SPC"的目录依赖关系图";
	}
    
	virtual QCString trFileIn(const char *name)
	{
		// return (QCString)"File in "+name;
		// unsure
		return (QCString)"文件在"+CN_SPC+name;
	}
	
    virtual QCString trIncludesFileIn(const char *name)
	{
		// return (QCString)"Includes file in "+name;
		// unsure
		return (QCString)"在"CN_SPC+name+CN_SPC"中引用";
	}

    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
	{
	  static const char *days[]   = { "一","二","三","四","五","六","日" };
      static const char *months[] = { "一","二","三","四","五","六","七","八","九","十","十一","十二" };
      
	  QCString sdate;
      
	  sdate.sprintf("%d年%s月%d日 星期%s",year, months[month-1], day, days[dayOfWeek-1]);
      
	  if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
	}                    
};

#endif

