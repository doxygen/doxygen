/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include "translator_adapter.h"

class TranslatorKorean : public TranslatorAdapter_1_1_0
{
  public:
    QCString idLanguage()
    { return "korean"; }
    /*! returns the name of the package that is included by LaTeX */
    QCString latexBabelPackage()
    { return ""; } // What is the correct value here?

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "euc-kr";
    }

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions()
    { return "관련된 함수들"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript()
    { return "관련주석"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription()
    { return "상세한 내용"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation()
    { return "멤버 타입정의 문서화"; }
    
    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation()
    { return "구성원(member) 열거 문서화"; }
    
    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation()
    { return "멤버 함수 문서화"; }
    
    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation()
    { return "멤서 데이타 문서화"; }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore()
    { return "More..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers()
    { return "모든 구성원들(members)의 명단"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList()
    { return "구성원(member) 명단"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers()
    { return "완전한 구성원들(members)의 명단 " ; } // "This is the complete list of members for "

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers()
    { return ", 상속받은 모든 구성원(members)들도 포함"; } // ", including all inherited members."
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="";
    if (s) result+=(QCString)s+"에 ";
    result+="source 코드로 부터 Doxygen에 의해 자동적으로 생성";
    return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName()
    { return "열거체 이름"; }
    
    /*! put after an enum value in the list of all members */
    QCString trEnumValue()
    { return "열거체 값"; }
    
    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn()
    { return "에서 정의된"; }

    /*! put as in introduction in the verbatim header file of a class.
     *  parameter f is the name of the include file.
     */
    QCString trVerbatimText(const char *f)
    { return (QCString)"이것은 "+f+" 포함 파일의 축약적인 문서이다"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \group command).
     */
    QCString trModules()
    { return "모듈들"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy()
    { return "클래스 계층(도)"; } // "클래스 조직" or "클래스 분류체계"
    
    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList()
    { return "혼합 목록"; } //"혼합 목록", "합성(집합) 명단(리스트)"
    
    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList()
    { return "파일 목록"; } //"파일 목록", "파일 리스트"

    /*! This is put above each page as a link to the list of all verbatim headers */
    QCString trHeaderFiles()
    { return "헤더 파일들"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers()
    { return "혼합 멤버들"; } // "합성(집합) 명단(멤버들)"

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers()
    { return "파일 멤버들"; }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages()
    { return "관련된 페이지들"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples()
    { return "예제들"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch()
    { return "검색"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription()
    { return "이 상속 목록은 완전하지는 않지만 알파벳순으로 분류되었습니다.";}

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="이것은 간략한 설명을 가진 모든 "; // "Here is a list of all "
      if (!extractAll) result+="문서화된 "; // "documented "
      result+="파일들에 대한 목록입니다."; // "files with brief descriptions:"
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription()
    { return "이것은 간략한 설명을 가진 클래스들, "
        "구조체들, 공용체들, 그리고 인터페이스들입니다.";
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="이곳에 모든 리스트가 있습니다";  //"Here is a list of all "
      if (!extractAll) result+="문서화된 ";             //"documented "
      result+="링크가 된 클래스 멤버들 ";               //"class members with links to "
      if (extractAll) 
        result+="각각의 멤버를 위한 클래스 문서:";      //"the class documentation for each member:"
      else 
        result+="이하로 속한 클래스들:";                //"the classes they belong to:"
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="이곳에 모든 리스트가 있습니다";
      if (!extractAll) result+="문서화된 ";
      result+="링크가 된 파일 멤버들 ";
      if (extractAll) 
        result+="각 멤버들에 대한 파일 문서화"; // "the file documentation for each member:"
      else 
        result+="그것들이 속해있는 파일들"; // "the files they belong to:"
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    QCString trHeaderFilesDescription()
    { return "이것은 API를 구성하는 헤더 파일들입니다."; } // "Here are the header files that make up the API:"

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription()
    { return "이것은 모든 예제들의 목록입니다."; } // "Here is a list of all examples:"

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription()
    { return "이것은 모든 관련된 문서화 페이지들의 목록입니다."; }
    // "Here is a list of all related documentation pages:"

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription()
    { return "이것은 모든 모듈들의 목록입니다."; } // "Here is a list of all modules:"

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    QCString trNoDescriptionAvailable()
    { return "유용한 설명이 없습니다."; } // "No description available"
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation()
    { return "문서화"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    QCString trModuleIndex()
    { return "모듈 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    QCString trHierarchicalIndex()
    { return "분류체계 색인"; } // "조직 색인", "계층적 인덱스"

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    QCString trCompoundIndex()
    { return "합성 인덱스"; } // "혼합 색인"

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() 
    { return "파일 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation()
    { return "모듈 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation()
    { return "클래스 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation()
    { return "파일 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    QCString trExampleDocumentation()
    { return "예제 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    QCString trPageDocumentation()
    { return "페이지 문서화"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual()
    { return "참고서"; } // "참고서","참고 매뉴얼", "참조 메뉴얼"
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    QCString trDefines()
    { return "정의들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    QCString trFuncProtos()
    { return "함수 원형들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    QCString trTypedefs()
    { return "타입 정의들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    QCString trEnumerations()
    { return "Enumerations"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    QCString trFunctions()
    { return "함수들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trVariables()
    { return "변수들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    QCString trEnumerationValues()
    { return "열거체 값들"; }
    
    /*! This is used in man pages as the author section. */
    QCString trAuthor()
    { return "저자"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation()
    { return "정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    QCString trFunctionPrototypeDocumentation()
    { return "함수 원형 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation()
    { return "타입 정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation()
    { return "열거체 타입 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation()
    { return "열거체 값 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation()
    { return "함수 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation()
    { return "변수 문서화"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    QCString trCompounds()
    { return "혼합들"; }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    QCString trFiles()
    { return "파일들"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result="";
      if (projName) result+=(QCString)projName+"에 대해 ";
      result += (QCString)date+" 생성 by";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    QCString trWrittenBy()
    {
      return "written by"; // "에 의해 쓰여진?"
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"에 대한 상속 도표"; // "Inheritance diagram for "+clName
    }
    
    /*! this text is generated when the \\internal command is used. */
    QCString trForInternalUseOnly()
    { return "내부 사용만을 위해"; }

    /*! this text is generated when the \\reimp command is used. */
    QCString trReimplementedForInternalReasons()
    { return "내부적 이유를 위해 재구현된: API가 영향을 받지않았다."; }
    // "Reimplemented for internal reasons; the API is not affected."

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning()
    { return "경고"; }

    /*! this text is generated when the \\bug command is used. */
    QCString trBugsAndLimitations()
    { return "버그들과 한계들"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion()
    { return "버전"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate()
    { return "날짜"; }

    /*! this text is generated when the \\author command is used. */
    QCString trAuthors()
    { return "저자(들)"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns()
    { return "반환"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso()
    { return "참조하시요"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters()
    { return "매개변수들"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions()
    { return "예외들"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy()
    { return "에 의해 생성된"; }

    // new since 0.49-990307
    
    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList()
    { return "이름공간 목록"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="이것은 모든 간략한 설명을 가진 ";
      if (!extractAll) result+="문서화된 ";
      result+="이름공간의 목록입니다:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends()
    { return "프렌드"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "프렌드, 그리고 관련된 함수 문서화"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      if (isTemplate) result+=" 템플릿";
      switch(compType)
      {
        case ClassDef::Class:      result+=" 클래스"; break;
        case ClassDef::Struct:     result+=" 구조체"; break;
        case ClassDef::Union:      result+=" 공용체"; break;
        case ClassDef::Interface:  result+=" 인터페이스"; break;
        case ClassDef::Exception:  result+=" 예외"; break;
      }
      result+=" 참조";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" 파일 참조"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" 이름 공간 참조";
      return result;
    }
    
    /*! \mgroup Class sections
     *  these are for the member sections of a class, struct or union 
     */
    QCString trPublicMembers()
    { return "공용 메소드"; }
    QCString trPublicSlots()
    { return "공용 Slots"; }
    QCString trSignals()
    { return "신호"; }
    QCString trStaticPublicMembers()
    { return "정적 공용 메소드"; }
    QCString trProtectedMembers()
    { return "프로텍티드 메소드"; }
    QCString trProtectedSlots()
    { return "Protected Slots"; }
    QCString trStaticProtectedMembers()
    { return "정적 프로텍티드 메소드"; }
    QCString trPrivateMembers()
    { return "프라이베이트 메소드"; }
    QCString trPrivateSlots()
    { return "Private Slots"; }
    QCString trStaticPrivateMembers()
    { return "정적 프라이베이트 메소드"; }
    /*! \endmgroup */ 
    
    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries)
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
            result+=", and ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries)
    {
      return trWriteList(numEntries)+" 들을 상속하다.";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries)
    {
      return trWriteList(numEntries)+"에 의해 상속된.";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"으로부터 재구현된.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"에서 재구현된.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers()
    { return "이름공간 멤버들"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="이것은 모든 ";
      if (!extractAll) result+="문서화된 ";
      result+="이름공간 멤버들의 목록입니다.";
      if (extractAll) 
        result+="각 멤버들에 대한 문서화에 ";
      else 
        result+="속해있는 이름공간에 ";
      result+="링크도있읍니다.";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    QCString trNamespaceIndex()
    { return "이름공간 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation()
    { return "이름공간 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    { return "이름공간"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
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
        case ClassDef::Exception:  result+="예외"; break;
      }
      result+="을 위한 문서화는 다음의 파일";
      if (!single) result+="들";
      result+="로부터 생성되었습니다:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "알파벳순서의 목록"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "반환값"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "주요 페이지"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "페이지"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return "출처";
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return "파일 @1. 의 @0 번째 라인에서 정의"; // "Definition at line @0 of file @1."
    }
    QCString trDefinedInSourceFile()
    {
      return "파일 @0. 에서 정의"; // "Definition in file @0."
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Deprecated"; // "비난받은(??)","반대하는"
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+"에 대한 원조 도표:";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+"에 대한 include 의존 그래프";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "생성자 & 소멸자 문서화"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "이 파일에 대한 소스 코드로 가시오";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "이 파일의 문서화로 가시오";
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "전제 조건";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "후미 조건"; // "후치조건"
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "변하지 않는";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "초기화기";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "코드";
    }
    QCString trGraphicalHierarchy()
    {
      return "도표의 클래스 분류체계"; // "도표의 클래스 조직"
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "도표의 클래스 분류체계로 가시오"; // "도표의 클래스 조직으로 가시오"
    }
    QCString trGotoTextualHierarchy()
    {
      return "문자의 클래스 분류체계로 가시오"; // "문자의 클래스 조직으로 가시오"
    }
    QCString trPageIndex()
    {
      return "페이지 색인";
    }
};

#endif
