/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
  초기 작업자:  ryk
                TODO 주석을 보면 11/22/01, doxygen 버전 1.2.11까지 수고해 주신 것 같습니다.
  최근 갱신자: Astromaker(http://ngps.net/)
               gpgiki(http://www.gpgstudy.com/gpgiki/)
*/


#ifndef TRANSLATOR_KR_H
#define TRANSLATOR_KR_H

class TranslatorKorean : public TranslatorAdapter_1_3_8
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
		//see http://www.ktug.or.kr/jsboard/read.php?table=operate&no=4422&page=1
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
    { return "관련된 함수"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "관련주석"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "세부 사항"; }

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
          return "멤버 변수 문서화";
      }
      else
      {
          return "멤버 변수 문서화";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "세부 사항 보기"; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "전체 멤버 목록 보기"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "멤버(멤버함수, 멤버변수 등) 목록"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "모든 멤버(멤버함수, 멤버변수 등) 목록 " ; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", 상속받은 모든 멤버도 포함"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="";
      if (s) result+=(QCString)s+"에 ";
      result += "source 코드로 부터 Doxygen에 의해 자동으로 생성";
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
    { return "에서 정의"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "모듈"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "클래스 계통도"; } // "클래스 조직" or "클래스 분류체계"

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            //Alternate text: "혼합 목록", "합성(집합) 명단(리스트)"
            return "복합구조(클래스, 구조체, 공용체)";
        }
        else
        {
            return "복합구조(클래스, 구조체, 공용체)";
        }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "파일 목록"; } //"파일 목록", "파일 리스트"

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "헤더 파일"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            return "복합구조(클래스, 구조체, 공용체) 멤버";
        }
        else
        {
            // Alternate text: "합성(집합) 명단(멤버들)"
            return "복합구조(클래스, 구조체, 공용체) 멤버";
        }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            return "파일 멤버";
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
    { return "이 상속 목록은 알파벳 순으로 정렬되어있습니다. (완전하지는 않음)";}

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="다음은 간략한 설명을 가진 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 파일에 대한 목록입니다.";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
            return "다음은 간략한 설명을 가진 클래스, "
                "구조체, 공용체, 인터페이스의 목록입니다.";
        }
        else
        {
            return "다음은 간략한 설명을 가진 클래스, "
                "구조체, 공용체, 인터페이스의 목록입니다.";
        }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="다음은 문서화된 모든 클래스, 구조체, 공용체 멤버에 대한 목록입니다. ";
      if (!extractAll)
      {
          result+="문서화된 ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          result+="링크 된 클래스 멤버는 ";
      }
      else
      {
          result+="링크 된 클래스 멤버는 ";
      }
      if (extractAll)
          result+="그 멤버에 대한 클래스 문서화로 갑니다.";
      else
          result+="이하로 속한 클래스:";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="다음은 문서화된 모든 파일에 대한 목록입니다. ";
      if (!extractAll) result+="문서화된 ";
      result+="링크 된 파일 멤버는 ";
      if (extractAll)
        result+="그 멤버에 대한 파일 문서화로 갑니다.";
      else
        result+="그들이 속해있는 파일로 갑니다.";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "다음은 API를 구성하는 헤더 파일입니다."; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "다음은 모든 예제의 목록입니다."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "다음은 모든 관련된 문서화 페이지의 목록입니다."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "다음은 모든 모듈의 목록입니다."; }

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
    { return "복합구조(클래스, 구조체, 공용체) 색인"; }

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
    { return "매크로, #define"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "함수 원형"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "타입 정의"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "열거형"; }

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
    { return "열거형 값"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "#define 문서화"; }

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
    { return "열거형 타입 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "열거형 값 문서화"; }

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
    { return "복합구조"; }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result="";
      if (projName) result+=(QCString)projName+" 문서화. ";
      result += "생성일시 : " +(QCString)date+" by";
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
    { return "내부적 이유로 인해 재구현된: API가 영향을 받지않았다."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "경고"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "버그와 한계"; }

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
    { return "참조"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "매개변수"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "예외"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "에 의해 생성된"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "namespace 목록"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="다음은 간략한 설명을 가진 ";
      if (!extractAll) result+="문서화된 ";
      result+="namespace의 목록입니다.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "friend"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "friend, 그리고 관련된 함수 문서화"; }

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
        case ClassDef::Protocol:   result+=" protocol"; break; // translate me!
        case ClassDef::Category:   result+=" category"; break; // translate me!
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
      result+=" namespace 참조";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "public 메소드"; }
    virtual QCString trPublicSlots()
    { return "public slots"; }
    virtual QCString trSignals()
    { return "신호"; }
    virtual QCString trStaticPublicMembers()
    { return "static public 메소드"; }
    virtual QCString trProtectedMembers()
    { return "protected 메소드"; }
    virtual QCString trProtectedSlots()
    { return "protected slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "static protected 메소드"; }
    virtual QCString trPrivateMembers()
    { return "private 메소드"; }
    virtual QCString trPrivateSlots()
    { return "private slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "static private 메소드"; }

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
            result+=", 과 ";
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
      return trWriteList(numEntries)+"에 의해 상속되었습니다.";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return trWriteList(numEntries)+"으로부터 재구현되었습니다.";
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
    { return "namespace 멤버"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="다음은 모든 ";
      if (!extractAll) result+="문서화된 ";
      result+="namespace 멤버의 목록입니다. ";
      if (extractAll)
        result+="각 멤버에 대한 문서화의 ";
      else
        result+="속해있는 namespace의 ";
      result+="링크도 있습니다.";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "namespace 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "namespace 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "namespace"; }

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
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
        case ClassDef::Exception:  result+="예외"; break;
      }
      result+="을 위한 문서화는 다음의 파일";
      if (!single) result+="들";
      result+="로부터 생성되었습니다.";
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

    virtual QCString trSources()
    {
      return "출처";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "@1 파일의 @0 번째 라인에서 정의";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "@0 파일에서 정의";
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
      return (QCString)clName+"에 대한 협력 도표:";
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
      return "이 파일에 대한 소스 코드 보기";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "이 파일의 문서화 보기";
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
      return "초기값";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "코드";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "도표로된 클래스 분류체계"; // "도표의 클래스 조직"
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "도표로된 클래스 분류체계 보기"; // "도표의 클래스 조직으로 가시오"
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "문자로된 클래스 분류체계 보기"; // "문자의 클래스 조직으로 가시오"
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
        return "노트";
    }
    virtual QCString trPublicTypes()
    {
        return "public 타입";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          return "Data 필드";
      }
      else
      {
          return "public 속성";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
        return "static public 속성";
    }
    virtual QCString trProtectedTypes()
    {
        return "protected 타입";
    }
    virtual QCString trProtectedAttribs()
    {
        return "protected 속성";
    }
    virtual QCString trStaticProtectedAttribs()
    {
        return "static protected 속성";
    }
    virtual QCString trPrivateTypes()
    {
        return "private 타입";
    }
    virtual QCString trPrivateAttribs()
    {
        return "private 속성";
    }
    virtual QCString trStaticPrivateAttribs()
    {
        return "static private 속성";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
        return "Todo";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
        return "Todo 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
        return "참조하는 곳";
    }
    virtual QCString trRemarks()
    {
        return "설명";
    }
    virtual QCString trAttention()
    {
        return "주의";
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
        return "범례";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
        // TODO: Need to be translated. -ryk11/22/01.
        return
		"Doxygen에 의해 생성된 도표를 보기위한 설명입니다.<p>\n"
        //"This page explains how to interpret the graphs that are generated "
        //"by doxygen.<p>\n"
		"다음의 예제를 참고하세요.\n"
        //"Consider the following example:\n"
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
		"설정 파일의 MAX_DOT_GRAPH_HEIGHT 플래그가 240으로 설정되었다면 다음의 그래프가 나올 것이다."
        //"If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        //"is set to 240 this will result in the following graph:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
		"상자들은 다음을 뜻한다.\n"
        //"The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
		"<li>%검은 상자는 그래프를 산출한 구조체나 클래스를 말한다.\n"
        //"<li>%A filled black box represents the struct or class for which the "
        //"graph is generated.\n"
		"<li>%검은선으로된 상자는 문서화된 구조체나 클래스를 표시한다.\n"
        //"<li>%A box with a black border denotes a documented struct or class.\n"
		"<li>%회색선으로된 상자는 문서화되지 않은 구조체나 클래스를 표시한다.\n"
        //"<li>%A box with a grey border denotes an undocumented struct or class.\n"
		"<li>%빨간선으로된 상자는 모든 상속/containment 관계를 보이지 않은 문서화된 구조체나 클래스를 나타낸다. "
		"지정된 경계안에 들어가지 않으면 그래프는 짤려진다.\n"
        //"<li>%A box with a red border denotes a documented struct or class for"
        //"which not all inheritance/containment relations are shown. %A graph is "
        //"truncated if it does not fit within the specified boundaries.\n"
        "</ul>\n"
		"화살표는 다음을 뜻한다.\n"
        //"The arrows have the following meaning:\n"
        "<ul>\n"
		"<li>%어두운 파란 화살표는 두 클래스간의 public 상속관계를 나타낸다.\n"
        //"<li>%A dark blue arrow is used to visualize a public inheritance "
        //"relation between two classes.\n"
		"<li>%어두운 녹색 화살표는 protected 상속관계를 나타낸다.\n"
        //"<li>%A dark green arrow is used for protected inheritance.\n"
		"<li>%어두운 빨강 화살표는 private 상속관계를 나타낸다.\n"
        //"<li>%A dark red arrow is used for private inheritance.\n"
		"<li>%밝은 자주색 화살표는 클래스에 의해 포함되더나 사용된 클래스를 나타낸다. "
		"이 화살표의 라벨은 접근 가능한 변수명을 나타낸다.\n"
        //"<li>%A purple dashed arrow is used if a class is contained or used "
        //"by another class. The arrow is labeled with the variable(s) "
        //"through which the pointed class or struct is accessible.\n"
		"<li>%밝은 노랑색 화살표는 템플릿 인스턴스와 템플릿 클래스를 나타낸다. "
		"이 화살표의 라벨은 그 인스턴스의 템플릿 매개변수를 나타낸다.\n"
        //"<li>%A yellow dashed arrow denotes a relation between a template instance and "
        //"the template class it was instantiated from. The arrow is labeled with "
        //"the template parameters of the instance.\n"
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
        return "DCOP 메소드"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
        return "properties"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
        return "property 문서화"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
        return "인터페이스"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
          return "데이터 구조"; // TODO: Need to be translated. -ryk11/22/01.
      }
      else
      {
          return "클래스"; // TODO: Need to be translated. -ryk11/22/01.
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
        return (QCString)"패키지 "+name; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
        return "패키지 목록"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
		//Here are the packages with brief descriptions (if available):"; // TODO: Need to be translated. -ryk11/22/01.
        return "다음은 간략한 설명을 가진 패키지이다.";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
        return "패키지"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
        return "패키지 문서화"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
        return "값:"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
        return "버그"; // TODO: Need to be translated. -ryk11/22/01.
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
        return "버그 목록"; // TODO: Need to be translated. -ryk11/22/01.
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
        return "인덱스"; // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "클래스" : "클래스"));
      if (!singular)  result+="들";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "파일" : "파일"));
      if (!singular)  result+="들";
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
      QCString result((first_capital ? "그룹" : "그룹"));
      if (!singular)  result+="들";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "페이지" : "페이지"));
      if (!singular)  result+="들";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "멤버" : "멤버"));
      if (!singular)  result+="들";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "필드" : "필드"));
      if (!singular)  result+="들";
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
      QCString result((first_capital ? "작성자" : "작성자"));
      if (!singular)  result+="들";
      return result;  // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
        return "참조"; // TODO: Need to be translated. -ryk11/22/01.
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      //return "Implements "+trWriteList(numEntries)+".";
	  return trWriteList(numEntries)+" 구현.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      //return "Implemented in "+trWriteList(numEntries)+".";
	  return trWriteList(numEntries)+"에 구현되었다.";
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
      return "Deprecated 목록";
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
        return "패키지 타입들"; //"Package Types";
    }
    /*! Used as a heading for a list of Java class functions with package
    * scope.
    */
    virtual QCString trPackageMembers()
    {
        return "패키지 함수들"; //"Package Functions";
    }
    /*! Used as a heading for a list of static Java class functions with
    *  package scope.
    */
    virtual QCString trStaticPackageMembers()
    {
        return "정적 패키지 함수들"; //"Static Package Functions";
    }
    /*! Used as a heading for a list of Java class variables with package
    * scope.
    */
    virtual QCString trPackageAttribs()
    {
        return "패키지 속성들"; //"Package Attributes";
    }
    /*! Used as a heading for a list of static Java class variables with
    * package scope.
    */
    virtual QCString trStaticPackageAttribs()
    {
        return "정적 패키지 속성들"; //"Static Package Attributes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
    *  to link to the unfiltered list of all members.
    */
    virtual QCString trAll()
    {
        return "All";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
        return "다음은 이 함수에 대한 호출 그래프입니다:";
        //"Here is the call graph for this function:";
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
        return "검색: "; //"Search for";
    }
    /*! This string is used as the title for the page listing the search
    *  results.
    */
    virtual QCString trSearchResultsTitle()
    {
        return "검색 결과";//"Search Results";
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
            //return "Sorry, no documents matching your query.";
            return "죄송합니다. 질의와 일치하는 문서가 없습니다.";
        }
        else if (numDocuments==1)
        {
            //return "Found <b>1</b> document matching your query.";
            return "질의와 일치하는 <b>1</b>개의 문서를 찾았습니다.";
        }
        else
        {
            //return "Found <b>$num</b> documents matching your query. "
            //    "Showing best matches first.";
            return "질의와 일치하는 <b>$num</b>개의 문서들을 찾았습니다. "
                "처음에 나오는 것이 가장 일치하는 문서입니다.";
        }
    }
    /*! This string is put before the list of matched words, for each search
    *  result. What follows is the list of words that matched the query.
    */
    virtual QCString trSearchMatches()
    {
        return "Matches:";
    }
};

#endif
