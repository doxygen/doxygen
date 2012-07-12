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
 */

#ifndef TRANSLATOR_KR_H
#define TRANSLATOR_KR_H


/* Korean translators
 * doxygen-svn
 *   * fly1004@gmail.com
 * doxygen-1.5.3
 *   * Astromaker(http://ngps.net/)
 *   * gpgiki(http://www.gpgstudy.com/gpgiki/)
 * doxygen-1.2.11
 *   * ryk */

/*! 
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionaly to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorKorean : public TranslatorAdapter_1_7_5
{
  protected:
    friend class TranslatorAdapterBase;
    virtual ~TranslatorKorean() {}

  public:

    // --- Language control methods -------------------
    
    /*! Used for identification of the language. The identification 
     * should not be translated. It should be replaced by the name 
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to 
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "korean"; }
    
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
      // I'm not sure what this should be.
      // When I figure it out, I'll update this.
      // see http://www.ktug.or.kr/jsboard/read.php?table=operate&no=4422&page=1
      return "\\usepackage{hfont}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "euc-kr";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "관련된 함수들"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(다음은 멤버 함수들이 아닙니다. 주의하십시오.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "상세한 설명"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "멤버 타입정의 문서화"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "멤버 열거형 문서화"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "멤버 함수 문서화"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          return "필드 문서화";
      }
      else
      {
          return "멤버 데이타 문서화";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "더 자세히 ..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "모든 멤버 목록"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "멤버 목록"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "다음에 대한 모든 멤버의 목록입니다 : "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return " (모든 상속된 멤버들도 포함합니다.)"; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="소스 코드로부터 ";
      if (s) result+=s+(QCString)"를 위해 ";
      result+="Doxygen에 의해 자동으로 생성됨."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "열거형 이름"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "열거형 값"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "다음에서 정의됨 :"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "모듈"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "클래스 계통도"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 구조";
      }
      else
      {
        return "클래스 목록"; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "파일 목록"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 필드"; 
      }
      else
      {
        return "클래스 멤버"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "전역"; 
      }
      else
      {
        return "파일 멤버"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "관련된 페이지"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "예제"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "검색"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "이 상속 목록은 완전하진 않지만 알파벳순으로 대략적으로 정렬되어있습니다.:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 파일에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "다음은 데이타 구조들입니다. (간략한 설명만을 보여줍니다) :"; 
      }
      else
      {
        return "다음은 클래스, 구조체, 공용체 그리고 인터페이스들입니다. "
               "(간략한 설명만을 보여줍니다) :"; 
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll)
      {
        result+="문서화된 ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="모든 구조체와 공용체의 필드들";
      }
      else
      {
        result+="모든 클래스 멤버들";
      }
      result+="의 목록입니다. ";

      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="각 필드들은 해당 필드에 대한 구조체와 공용체의 "
		  "문서화 페이지의 링크를 가지고 있습니다. :";
        }
        else
        {
          result+="각 멤버들은 해당 멤버에 대한 클래스의 문서화 페이지의 "
		  "링크를 가지고 있습니다. :";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="각 필드들은 해당 필드가 속해 있는 구조체와 공용체에 "
		  "대한 링크를 가지고 있습니다. :";
        }
        else
        {
          result+="각 멤버들은 해당 멤버가 속해 있는 클래스에 대한 "
		  "링크를 가지고 있습니다. :";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된  ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="모든 함수, 변수, 매크로, 열거형, 타입정의들";
      }
      else
      {
        result+="파일 멤버들";
      }
      result+="의 목록입니다. ";

      result+="각 항목은 ";
      if (extractAll) 
        result+="그들이 속한 파일 페이지의 링크를 가지고 있습니다. :";
      else 
        result+="그들에 대한 문서화 페이지의 링크를 가지고 있습니다. :";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "다음은 모든 예제들의 목록입니다.:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "다음은 관련된 모든 문서화 페이지들의 목록입니다.:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "다음은 모든 모듈들의 목록입니다.:"; }

    // index titles (the project name is prepended for these) 

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "문서화"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "모듈 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "계통도 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "데이타 구조 색인";
      }
      else
      {
        return "클래스 색인"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "파일 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "모듈 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 구조 문서화"; 
      }
      else
      {
        return "클래스 문서화"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "파일 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "예제 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "페이지 문서화"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "참조 매뉴얼"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "매크로"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "함수 원형"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "타입정의"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "열거형 타입"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "함수"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "변수"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "열거형 멤버"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "매크로 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "함수 원형 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "타입정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "열거형 타입 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "함수 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "변수 문서화"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 구조"; 
      }
      else
      {
        return "클래스"; 
      }
    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"생성시간 : "+date;
      if (projName) result+=(QCString)", 프로젝트명 : "+projName;
      result+=(QCString)", 생성자 : ";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "작성자 : ";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"에 대한 상속 다이어그램 : ";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "내부적적으로만 사용하기 위해."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "경고"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "버전"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "날짜"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "반환값"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "참고"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "매개변수"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "예외"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "다음에 의해 생성됨 : "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "네임스페이스 목록"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 네임스페이스에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Friends"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Friend, 그리고 관련된 함수 문서화"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" 클래스"; break;
        case ClassDef::Struct:     result+=" 구조체"; break;
        case ClassDef::Union:      result+=" 공용체"; break;
        case ClassDef::Interface:  result+=" 인터페이스"; break;
        case ClassDef::Protocol:   result+=" 프로토콜"; break;
        case ClassDef::Category:   result+=" 카테고리"; break;
        case ClassDef::Exception:  result+=" 예외"; break;
      }
      if (isTemplate) result+=" 템플릿";
      result+=" 참조";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" 파일 참조";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" 네임스페이스 참조";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Public 멤버 함수"; }
    virtual QCString trPublicSlots()
    { return "Public Slots"; }
    virtual QCString trSignals()
    { return "Signals"; }
    virtual QCString trStaticPublicMembers()
    { return "정적 Public 멤버 함수"; }
    virtual QCString trProtectedMembers()
    { return "Protected 멤버 함수"; }
    virtual QCString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "정적 Protected 멤버 함수"; }
    virtual QCString trPrivateMembers()
    { return "Private 멤버 함수"; }
    virtual QCString trPrivateSlots()
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "정적 Private 멤버 함수"; }
    
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
            result+=", ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return trWriteList(numEntries)+"를(을) 상속했습니다.";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return trWriteList(numEntries)+"에 의해 상속되었습니다.";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"(으)로부터 재구현되었습니다.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"에서 재구현되었습니다.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "네임스페이스 멤버"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 네임스페이스 멤버들의 목록입니다. ";
      if (extractAll) 
        result+="각 멤버들은 해당 멤버의 네임스페이스 문서화 페이지의 링크를 가지고 있습니다. :";
      else 
        result+="각 멤버들은 해당 멤버가 속한 네임스페이스 페이지의 링크를 가지고 있습니다. :";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "네임스페이스 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "네임스페이스 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "네임스페이스"; }

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
      QCString result=(QCString)"이 ";
      switch(compType)
      {
        case ClassDef::Class:      result+="클래스"; break;
        case ClassDef::Struct:     result+="구조체"; break;
        case ClassDef::Union:      result+="공용체"; break;
        case ClassDef::Interface:  result+="인터페이스"; break;
        case ClassDef::Protocol:   result+="프로토콜"; break;
        case ClassDef::Category:   result+="카테고리"; break;
        case ClassDef::Exception:  result+="예외"; break;
      }
      result+="에 대한 문서화 페이지는 다음의 파일";
      if (!single) result+="들";
      result+="로부터 생성되었습니다.:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "알파벳순 목록"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "반환값"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "메인 페이지"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "페이지"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "@1 파일의 @0 번째 라인에서 정의되었습니다.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "@0 파일에서 정의되었습니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "잘못된 코드";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+"에 대한 협력 다이어그램:";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+"에 대한 include 의존 그래프";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "생성자 & 소멸자 문서화";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "이 파일의 소스 코드 페이지로 가기";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "이 파일의 문서화 페이지로 가기";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "전제조건";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "후미조건";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "변하지 않는";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "초기값:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "코드";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "그래픽컬한 클래스 계통도";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "그래픽컬한 클래스 계통도 페이지로 가기";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "텍스트 형식의 클래스 계통도 페이지로 가기";
    }
    virtual QCString trPageIndex()
    {
      return "페이지 색인";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "주의";
    }
    virtual QCString trPublicTypes()
    {
      return "Public 타입";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 필드";
      }
      else
      {
        return "Public 속성";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "정적 Public 속성";
    }
    virtual QCString trProtectedTypes()
    {
      return "Protected 타입";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Protected 속성";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "정적 Protected 속성";
    }
    virtual QCString trPrivateTypes()
    {
      return "Private 타입";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Private 속성";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "정적 Private 속성";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "할일";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "할일 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "다음에 의해서 참조됨 : ";
    }
    virtual QCString trRemarks()
    {
      return "Remarks";
    }
    virtual QCString trAttention()
    {
        return "주의";
    }
    virtual QCString trInclByDepGraph()
    {
      return "이 그래프는 이 파일을 직/간접적으로 include 하는 파일들을 보여줍니다.:";
    }
    virtual QCString trSince()
    {
      return "Since";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "그래프 범례";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "이 페이지는 doxygen에 의해 생성된 그래프들을 이해하는 방법을 설명합니다.<p>\n"
        "다음의 예제를 참고하십시오.:\n"
        "\\code\n"
        "/*! 생략되었기 때문에 보이지 않는 클래스 */\n"
        "class Invisible { };\n\n"
        "/*! Truncated 클래스, 상속관계가 숨겨짐 */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* doxygen 주석에 의해서 문서화되지 않는 클래스 */\n"
        "class Undocumented { };\n\n"
        "/*! public 상속을 통해서 상속된 클래스 */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! 템플릿 클래스 */\n"
        "template<class T> class Templ { };\n\n"
        "/*! protected 상속을 통해서 상속된 클래스 */\n"
        "class ProtectedBase { };\n\n"
        "/*! private 상속을 통해서 상속된 클래스 */\n"
        "class PrivateBase { };\n\n"
        "/*! 상속되어진 클래스에 의해 (멤버로) 사용되어지는 클래스 */\n"
        "class Used { };\n\n"
        "/*! 다른 클래스들을 상속하는 슈퍼 클래스 */\n"
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
        "다음과 같은 그래프가 출력될 것입니다. :"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "위 그래프의 박스들은 다음과 같은 의미를 가집니다. :\n"
        "<ul>\n"
        "<li>%A 회색으로 채워진 박스는 이 그래프를 생성해 낸 구조체나 클래스를 의미합니다.\n"
        "<li>%A 검은색 테두리의 박스는 문서화된 구조체나 클래스를 의미합니다.\n"
        "<li>%A 회색 테두리의 박스는 문서화되지 않은 구조체나 클래스를 의미합니다.\n"
        "<li>%A 빨간색 테두리의 박스는 모든 상속이나 포함관계가 보여지지 않는 "
	"구조체나 클래스를 의미합니다."
        "%A 만약 그래프가 지정된 경계내에 맞지 않으면, 그래프가 잘려집니다.\n"
        "</ul>\n"
        "화살표들은 다음과 같은 의미를 가집니다. :\n"
        "<ul>\n"
        "<li>%A 어두운 파랑색 화살표는 두 클래스들 간에 public 상속이 있음을 의미합니다.\n"
        "<li>%A 어두운 연두색 화살표는 protected 상속이 있음을 의미합니다.\n"
        "<li>%A 어두운 빨간색 화살표는 private 상속이 있음을 의미합니다.\n"
        "<li>%A 보라색 점선 화살표는 다른 클래스에 의해 포함되거나 사용되어짐을 의미합니다. "
	"화살표의 라벨은 화살표가 가리키는 클래스나 구조체로 접근하는 변수명(들)으로 붙습니다.\n"
        "<li>%A 노란색 점선 화살표는 템플릿 인스턴스와 템프릿 클래스에 대한 관계를 의미합니다. "
	"화살표의 라벨은 인스턴스의 템플릿 파라메터로 붙습니다.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "범례";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "테스트";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "테스트 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP 멤버 함수";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "속성";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "속성 문서화";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "데이타 구조";
      }
      else
      {
        return "클래스";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return name+(QCString)" 패키지";
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "패키지 목록";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "다음은 패키지들입니다. (가능한한 간략한 설명만을 보여줍니다) :";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "패키지";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "값:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "버그";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "버그 목록";
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
      return "949";
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "129";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "색인";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "클래스" : "클래스"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "파일" : "파일"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "네임스페이스" : "네임스페이스"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "그룹" : "그룹"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "페이지" : "페이지"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "멤버" : "멤버"));
      if (!singular)  result+="들";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "전역" : "전역"));
      if (!singular)  result+="";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "작성자" : "작성자"));
      if (!singular)  result+="들";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "다음을 참조함 : ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"를 구현.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"에서 구현되었습니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "목차";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "잘못된 코드 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "이벤트";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "이벤트 문서화";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "패키지 타입";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "패키지 함수";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "정적 패키지 함수";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "패키지 속성";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "정적 패키지 속성";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "모두";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "이 함수 내부에서 호출하는 함수들에 대한 그래프입니다.:";
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
      return "검색";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "검색 결과";
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
        return "죄송합니다. 질의에 일치하는 문서가 없습니다.";
      }
      else if (numDocuments==1)
      {
        return "질의에 일치하는 <b>1</b> 개의 문서를 찾았습니다.";
      }
      else 
      {
        return "질의에 일치하는 <b>$num</b> 개의 문서를 찾았습니다. "
               "가장 많이 일치하는 문서를 가장 먼저 보여줍니다.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "결과:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " 소스 파일";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "디렉토리 계통도"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "디렉토리 문서화"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "디렉토리"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "이 디렉토리 목록은 완전하진 않지만, (대략적으로) 알파벳순으로 정렬되어있습니다.:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" 디렉토리 참조"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "디렉토리" : "디렉토리"));
      if (singular) result+=""; else result+="들";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "이 함수는 편의를 제공하기 위해 오버로드된 멤버 함수입니다. "
              "위의 함수와 틀린 점은 단지 받아들이는 아규먼트(argument)가 다르다는 것입니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "이 함수를 호출하는 함수들에 대한 그래프입니다.:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "열거형 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "멤버 함수/서브루틴 문서화"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "데이타 타입 목록"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "데이타 필드"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "대략적인 설명과 함께 데이타 타입들의 목록입니다.:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll)
      {
        result+="문서화된 ";
      }
      result+="모든 데이타 타입 멤버들의 목록입니다. ";

      result+="각 항목은 ";
      if (!extractAll) 
      {
         result+="각 멤버에 대한 데이타 구조 문서화 페이지의 링크를 가지고 있습니다.";
      }
      else 
      {
         result+="그들이 속한 데이타 타입의 링크를 가지고 있습니다. :";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "데이타 타입 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "데이타 타입 문서화"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "함수/서브루틴"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "함수/서브루틴 문서화"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "데이타 타입들"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "모듈 목록"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 모듈에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
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
        case ClassDef::Class:      result+=" 모듈"; break;
        case ClassDef::Struct:     result+=" 타입"; break;
        case ClassDef::Union:      result+=" 공용체"; break;
        case ClassDef::Interface:  result+=" 인터페이스"; break;
        case ClassDef::Protocol:   result+=" 프로토콜"; break;
        case ClassDef::Category:   result+=" 카테고리"; break;
        case ClassDef::Exception:  result+=" 예외"; break;
      }
      if (isTemplate) result+=" 템플릿";
      result+=" 참조";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" 모듈 참조";        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "모듈 멤버들"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 모듈 멤버의 목록입니다. ";
      if (extractAll) 
      {
        result+="각 항목은 각 멤버의 모듈 문서화 페이지의 링크를 가지고 있습니다. :";
      }
      else 
      {
        result+="각 항목은 그들이 속한 모듈의 링크를 가지고 있습니다. :";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "모듈 색인"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "모듈" : "모듈"));
      if (!singular)  result+="들";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"다음 파일";
      if (single) result+=""; else result+="들";
      result+="로부터 생성된 ";
      result+="이 ";
      switch(compType)
      {
        case ClassDef::Class:      result+="모듈"; break;
        case ClassDef::Struct:     result+="타입"; break;
        case ClassDef::Union:      result+="공용체"; break;
        case ClassDef::Interface:  result+="인터페이스"; break;
        case ClassDef::Protocol:   result+="프로토콜"; break;
        case ClassDef::Category:   result+="카테고리"; break;
        case ClassDef::Exception:  result+="예외"; break;
      }
      result+="의 문서화 페이지:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "타입" : "타입"));
      if (!singular)  result+="들";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "서브프로그램" : "서브프로그램"));
      if (!singular)  result+="들";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "타입 한정자들";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" 관계";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "로딩중...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "전역 이름공간";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "검색중...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "일치하는것 없음";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return QCString(name) + "에 대한 디렉토리 의존 그래프";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return QCString(name) + "의 파일";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return QCString(name) + "의 파일 포함";
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
      static const char *days[]   = { "월","화","수","목","금","토","일" };
      static const char *months[] = { "1월","2월","3월","4월","5월","6월","7월","8월","9월","10월","11월","12월" };
      QCString sdate;
      sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
    { return "참고 문헌"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString(name) + QCString("에 대한 디렉토리 의존성 그래프:"); }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "상세 단계"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "템플릿 파라메터"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return QCString("그리고 ")+number+"개 더..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool /*single*/)
    { QCString result = "이 열거형에 대한 문서가 다음 파일(들)로부터 생성되었습니다.:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Enum Reference"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(what) + QCString("(으)로부터 상속된 ") + QCString(members); }

    /*! Header of the sections with inherited members specific for the 
     *  base class(es) 
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "추가로 상속된 멤버들"; }

//////////////////////////////////////////////////////////////////////////

};

#endif
