/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

class TranslatorKorean : public TranslatorAdapter_1_2_13
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
        return "";
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
    { return "관련주석"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "상세한 내용"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "멤버 타입정의 문서화"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "구성원(member) 열거 문서화"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "멤버 함수 문서화"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          // TODO: This need to be translated. -ryk11/22/01.
          return "멤서 데이타 문서화"; 
      }
      else
      {
          return "멤서 데이타 문서화"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "More..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "모든 구성원들(members)의 명단"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "구성원(member) 명단"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "완전한 구성원들(members)의 명단 " ; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", 상속받은 모든 구성원(members)들도 포함"; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="";
      if (s) result+=(QCString)s+"에 ";
      result += "source 코드로 부터 Doxygen에 의해 자동적으로 생성";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "열거체 이름"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "열거체 값"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "에서 정의된"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "모듈들"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "클래스 계층(도)"; } // "클래스 조직" or "클래스 분류체계"
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            //Alternate text: "혼합 목록", "합성(집합) 명단(리스트)"
            return "혼합 목록";
        }
        else
        {
            //TODO: This needs to be translated. -ryk11/22/01.
            return "혼합 목록";
        }
    } 
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "파일 목록"; } //"파일 목록", "파일 리스트"

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "헤더 파일들"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            // TODO: This need to be translated. -ryk11/22/01.
            return "혼합 멤버들";
        }
        else
        {
            // Alternate text: "합성(집합) 명단(멤버들)"
            return "혼합 멤버들";
        }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            // TODO: This needs to be translated. -ryk11/22/01.
            return "파일 멤버들";
        }
        else
        {
            return "파일 멤버들";
        }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "관련된 페이지들"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "예제들"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "검색"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "이 상속 목록은 완전하지는 않지만 알파벳순으로 분류되었습니다.";}

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="이것은 간략한 설명을 가진 모든 ";
      if (!extractAll) result+="문서화된 ";
      result+="파일들에 대한 목록입니다.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            // TODO: This needs to be translated. -ryk11/22/01.
            return "이것은 간략한 설명을 가진 클래스들, "
                "구조체들, 공용체들, 그리고 인터페이스들입니다.";
        }
        else
        {
            return "이것은 간략한 설명을 가진 클래스들, "
                "구조체들, 공용체들, 그리고 인터페이스들입니다.";
        }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="이곳에 모든 리스트가 있습니다";
      if (!extractAll)
      {
          result+="문서화된 ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          // TODO: This need to be translated. -ryk11/22/01.
          result+="링크가 된 클래스 멤버들 ";
      }
      else
      {
          result+="링크가 된 클래스 멤버들 ";
      }
      if (extractAll) 
          result+="각각의 멤버를 위한 클래스 문서:";
      else 
          result+="이하로 속한 클래스들:";            
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="이곳에 모든 리스트가 있습니다";
      if (!extractAll) result+="문서화된 ";
      result+="링크가 된 파일 멤버들 ";
      if (extractAll) 
        result+="각 멤버들에 대한 파일 문서화";
      else 
        result+="그것들이 속해있는 파일들";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "이것은 API를 구성하는 헤더 파일들입니다."; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "이것은 모든 예제들의 목록입니다."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "이것은 모든 관련된 문서화 페이지들의 목록입니다."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "이것은 모든 모듈들의 목록입니다."; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return "유용한 설명이 없습니다."; }
    
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
    { return "분류체계 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    { return "합성 인덱스"; }

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
    { return "클래스 문서화"; }

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
    { return "참고서"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "정의들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "함수 원형들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "타입 정의들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerations"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "함수들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "변수들"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "열거체 값들"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "함수 원형 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "타입 정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "열거체 타입 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "열거체 값 문서화"; }

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
    { return "혼합들"; }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result="";
      if (projName) result+=(QCString)projName+"에 대해 ";
      result += (QCString)date+" 생성 by";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "written by";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)clName+"에 대한 상속 도표";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "내부 사용만을 위해"; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "내부적 이유를 위해 재구현된: API가 영향을 받지않았다."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "경고"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "버그들과 한계들"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "버전"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "날짜"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "반환"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "참조하시요"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "매개변수들"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "예외들"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "에 의해 생성된"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "이름공간 목록"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="이것은 모든 간략한 설명을 가진 ";
      if (!extractAll) result+="문서화된 ";
      result+="이름공간의 목록입니다:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
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
    virtual QCString trCompoundReference(const char *clName,
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
      result+=" 이름 공간 참조";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "공용 메소드"; }
    virtual QCString trPublicSlots()
    { return "공용 Slots"; }
    virtual QCString trSignals()
    { return "신호"; }
    virtual QCString trStaticPublicMembers()
    { return "정적 공용 메소드"; }
    virtual QCString trProtectedMembers()
    { return "프로텍티드 메소드"; }
    virtual QCString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "정적 프로텍티드 메소드"; }
    virtual QCString trPrivateMembers()
    { return "프라이베이트 메소드"; }
    virtual QCString trPrivateSlots()
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "정적 프라이베이트 메소드"; }
    
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
            result+=", and ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return trWriteList(numEntries)+" 들을 상속하다.";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return trWriteList(numEntries)+"에 의해 상속된.";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"으로부터 재구현된.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return trWriteList(numEntries)+"에서 재구현된.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "이름공간 멤버들"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "이름공간 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "이름공간 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "이름공간"; }

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
    virtual QCString trAlphabeticalList()
    { return "알파벳순서의 목록"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "반환값"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "주요 페이지"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "페이지"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "출처";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "파일 @1. 의 @0 번째 라인에서 정의";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "파일 @0. 에서 정의";
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
      return (QCString)clName+"에 대한 원조 도표:";
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
      return "이 파일에 대한 소스 코드로 가시오";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "이 파일의 문서화로 가시오";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "전제 조건";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "후미 조건"; //Alternate: "후치조건"
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "변하지 않는";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "초기화기";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "코드";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "도표의 클래스 분류체계"; // "도표의 클래스 조직"
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "도표의 클래스 분류체계로 가시오"; // "도표의 클래스 조직으로 가시오"
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "문자의 클래스 분류체계로 가시오"; // "문자의 클래스 조직으로 가시오"
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
        return "Note"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trPublicTypes()
    {
        return "Public Types"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          return "Data Fields"; // TODO: Need to be translated. -ryk11/22/01.
      }
      else
      {
          return "Public Attributes"; // TODO: Need to be translated. -ryk11/22/01.
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
        return "Static Public Attributes"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trProtectedTypes()
    {
        return "Protected Types"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trProtectedAttribs()
    {
        return "Protected Attributes"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trStaticProtectedAttribs()
    {
        return "Static Protected Attributes"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trPrivateTypes()
    {
        return "Private Types"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trPrivateAttribs()
    {
        return "Private Attributes"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trStaticPrivateAttribs()
    {
        return "Static Private Attributes"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
        return "Todo"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
        return "Todo List"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
        return "Referenced by"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trRemarks()
    {
        return "Remarks"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trAttention()
    {
        return "Attention"; // TODO: Need to be translated. -ryk11/22/01.
    }
    virtual QCString trInclByDepGraph()
    {
        // TODO: Need to be translated. -ryk11/22/01.
        return "This graph shows which files directly or "
             "indirectly include this file:";
    }
    virtual QCString trSince()
    {
        return "Since"; // TODO: Need to be translated. -ryk11/22/01.
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
        return "Graph Legend"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
        // TODO: Need to be translated. -ryk11/22/01.
        return
        "This page explains how to interpret the graphs that are generated "
        "by doxygen.<p>\n"
        "Consider the following example:\n"
        "\\code\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class that is inherited using protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class that is inherited using private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Class that is used by the Inherited class */\n"
        "class Used { };\n\n"
        "/*! Super class that inherits a number of other classes */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "is set to 240 this will result in the following graph:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a grey border denotes an undocumented struct or class.\n"
        "<li>%A box with a red border denotes a documented struct or class for"
        "which not all inheritance/containment relations are shown. %A graph is "
        "truncated if it does not fit within the specified boundaries.\n"
        "</ul>\n"
        "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>%A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.\n"
        "<li>%A dark green arrow is used for protected inheritance.\n"
        "<li>%A dark red arrow is used for private inheritance.\n"
        "<li>%A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible.\n"
        "<li>%A yellow dashed arrow denotes a relation between a template instance and "
        "the template class it was instantiated from. The arrow is labeled with "
        "the template parameters of the instance.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
        return "legend"; // TODO: Need to be translated. -ryk11/22/01.
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
        return "Test"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
        return "Test List"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
        return "DCOP Methods"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
        return "Properties"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
        return "Property Documentation"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
        return "Interfaces"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          return "Data Structures"; // TODO: Need to be translated. -ryk11/22/01.
      }
      else
      {
          return "Classes"; // TODO: Need to be translated. -ryk11/22/01.
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
        return (QCString)"Package "+name; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
        return "Package List"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
        return "Here are the packages with brief descriptions (if available):"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
        return "Packages"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
        return "Package Documentation"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
        return "Value:"; // TODO: Need to be translated. -ryk11/22/01.
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
        return "Bug"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
        return "Bug List"; // TODO: Need to be translated. -ryk11/22/01.
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
        return "1252";
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
        return "Index"; // TODO: Need to be translated. -ryk11/22/01.
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Class" : "class"));
      if (!singular)  result+="es";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "File" : "file"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Page" : "page"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Member" : "member"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Field" : "field"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
        return "References"; // TODO: Need to be translated. -ryk11/22/01.
    }

};

#endif
