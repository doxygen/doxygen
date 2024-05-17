/******************************************************************************
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
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorKorean : public TranslatorAdapter_1_8_15
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
    QCString latexLanguageSupportCommand() override
    {
      // I'm not sure what this should be.
      // When I figure it out, I'll update this.
      // see http://www.ktug.or.kr/jsboard/read.php?table=operate&no=4422&page=1
      return "\\usepackage{kotex}\n";
    }
    QCString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }
    QCString trISOLang() override
    {
      return "ko";
    }
    QCString getLanguageString() override
    {
      return "0x412 Korean";
    }
    bool needsPunctuation() override
    {
      return false;
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "관련된 함수들"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(다음은 멤버 함수들이 아닙니다. 주의하십시오.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "상세한 설명"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "상세"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "멤버 타입정의 문서화"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "멤버 열거형 문서화"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "멤버 함수 문서화"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
          return "필드 문서화";
      }
      else
      {
          return "멤버 데이터 문서화";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "더 자세히 ..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "모든 멤버 목록"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "멤버 목록"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "다음에 대한 모든 멤버의 목록입니다 :"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return " (모든 상속된 멤버들도 포함합니다.)"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="소스 코드로부터 ";
      if (!s.isEmpty()) result+=s+"를 위해 ";
      result+="Doxygen에 의해 자동으로 생성됨.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "열거형 이름"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "열거형 값"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "다음에서 정의됨 :"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "모듈"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "클래스 계통도"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 구조";
      }
      else
      {
        return "클래스 목록";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "파일 목록"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 필드";
      }
      else
      {
        return "클래스 멤버";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "전역";
      }
      else
      {
        return "파일 멤버";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "관련된 페이지"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "예제"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "검색"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "이 상속 목록은 완전하진 않지만 알파벳순으로 대략적으로 정렬되어있습니다.:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 파일에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "다음은 데이터 구조들입니다. (간략한 설명만을 보여줍니다) :";
      }
      else
      {
        return "다음은 클래스, 구조체, 공용체 그리고 인터페이스들입니다. "
               "(간략한 설명만을 보여줍니다) :";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll)
      {
        result+="문서화된 ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된  ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trExamplesDescription() override
    { return "다음은 모든 예제들의 목록입니다.:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "다음은 관련된 모든 문서화 페이지들의 목록입니다.:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "다음은 모든 모듈들의 목록입니다.:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "문서화"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "모듈 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "계통도 색인"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 구조 색인";
      }
      else
      {
        return "클래스 색인";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "파일 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "모듈 문서화"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 구조 문서화";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "클래스 문서화";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "파일 문서화"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "참조 매뉴얼"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "매크로"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "타입정의"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "열거형 타입"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "함수"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "변수"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "열거형 멤버"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "매크로 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "타입정의 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "열거형 타입 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "함수 문서화"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "변수 문서화"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 구조";
      }
      else
      {
        return "클래스";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="생성시간 : "+date;
      if (!projName.isEmpty()) result+=", 프로젝트명 : "+projName;
      result+=", 생성자 : ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return clName+"에 대한 상속 다이어그램 : ";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "경고"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "버전"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "날짜"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "반환값"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "참고"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "매개변수"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "예외"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "다음에 의해 생성됨 : "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "네임스페이스 목록"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 네임스페이스에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Friend, 그리고 관련된 함수 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" 클래스"; break;
        case ClassDef::Struct:     result+=" 구조체"; break;
        case ClassDef::Union:      result+=" 공용체"; break;
        case ClassDef::Interface:  result+=" 인터페이스"; break;
        case ClassDef::Protocol:   result+=" 프로토콜"; break;
        case ClassDef::Category:   result+=" 카테고리"; break;
        case ClassDef::Exception:  result+=" 예외"; break;
        default: break;
      }
      if (isTemplate) result+=" 템플릿";
      result+=" 참조";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" 파일 참조";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" 네임스페이스 참조";
      return result;
    }

    QCString trPublicMembers() override
    { return "Public 멤버 함수"; }
    QCString trPublicSlots() override
    { return "Public Slots"; }
    QCString trSignals() override
    { return "Signals"; }
    QCString trStaticPublicMembers() override
    { return "정적 Public 멤버 함수"; }
    QCString trProtectedMembers() override
    { return "Protected 멤버 함수"; }
    QCString trProtectedSlots() override
    { return "Protected Slots"; }
    QCString trStaticProtectedMembers() override
    { return "정적 Protected 멤버 함수"; }
    QCString trPrivateMembers() override
    { return "Private 멤버 함수"; }
    QCString trPrivateSlots() override
    { return "Private Slots"; }
    QCString trStaticPrivateMembers() override
    { return "정적 Private 멤버 함수"; }

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
          //if (i<numEntries-2) // not the fore last entry
            result+=", ";
          //else                // the fore last entry
          //  result+=", ";     // TODO: does the 'and' need to be translated here?
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return trWriteList(numEntries)+"를(을) 상속했습니다.";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return trWriteList(numEntries)+"에 의해 상속되었습니다.";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"(으)로부터 재구현되었습니다.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"에서 재구현되었습니다.";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "네임스페이스 멤버"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "네임스페이스 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "네임스페이스 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "네임스페이스"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="이 ";
      switch(compType)
      {
        case ClassDef::Class:      result+="클래스"; break;
        case ClassDef::Struct:     result+="구조체"; break;
        case ClassDef::Union:      result+="공용체"; break;
        case ClassDef::Interface:  result+="인터페이스"; break;
        case ClassDef::Protocol:   result+="프로토콜"; break;
        case ClassDef::Category:   result+="카테고리"; break;
        case ClassDef::Exception:  result+="예외"; break;
        default: break;
      }
      result+="에 대한 문서화 페이지는 다음의 파일";
      if (!single) result+="들";
      result+="로부터 생성되었습니다.:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "반환값"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "메인 페이지"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "페이지"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "@1 파일의 @0 번째 라인에서 정의되었습니다.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "@0 파일에서 정의되었습니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "잘못된 코드";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return clName+"에 대한 협력 다이어그램:";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return fName+"에 대한 include 의존 그래프";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "생성자 & 소멸자 문서화";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "이 파일의 소스 코드 페이지로 가기";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "이 파일의 문서화 페이지로 가기";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "전제조건";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "후미조건";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "변하지 않는";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "초기값:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "코드";
    }
    QCString trGraphicalHierarchy() override
    {
      return "그래픽컬한 클래스 계통도";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "그래픽컬한 클래스 계통도 페이지로 가기";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "텍스트 형식의 클래스 계통도 페이지로 가기";
    }
    QCString trPageIndex() override
    {
      return "페이지 색인";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "주의";
    }
    QCString trPublicTypes() override
    {
      return "Public 타입";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 필드";
      }
      else
      {
        return "Public 속성";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "정적 Public 속성";
    }
    QCString trProtectedTypes() override
    {
      return "Protected 타입";
    }
    QCString trProtectedAttribs() override
    {
      return "Protected 속성";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "정적 Protected 속성";
    }
    QCString trPrivateTypes() override
    {
      return "Private 타입";
    }
    QCString trPrivateAttribs() override
    {
      return "Private 속성";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "정적 Private 속성";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "할일";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "할일 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "다음에 의해서 참조됨 : ";
    }
    QCString trRemarks() override
    {
      return "Remarks";
    }
    QCString trAttention() override
    {
        return "주의";
    }
    QCString trInclByDepGraph() override
    {
      return "이 그래프는 이 파일을 직/간접적으로 include 하는 파일들을 보여줍니다.:";
    }
    QCString trSince() override
    {
      return "Since";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "그래프 범례";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
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
    QCString trLegend() override
    {
      return "범례";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "테스트";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "테스트 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "속성";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "속성 문서화";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "데이터 구조";
      }
      else
      {
        return "클래스";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return name+" 패키지";
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "다음은 패키지들입니다. (가능한한 간략한 설명만을 보여줍니다) :";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "패키지";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "값:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "버그";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "949";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "129";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "색인";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool, bool singular) override
    {
      return createNoun(false, singular, "클래스", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool, bool singular) override
    {
      return createNoun(false, singular, "파일", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool, bool singular) override
    {
      return createNoun(false, singular, "네임스페이스", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool, bool singular) override
    {
      return createNoun(false, singular, "그룹", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool, bool singular) override
    {
      return createNoun(false, singular, "페이지", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool, bool singular) override
    {
      return createNoun(false, singular, "멤버", "들");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool, bool singular) override
    {
      return createNoun(false, singular, "전역", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool, bool singular) override
    {
      return createNoun(false, singular, "작성자", "들");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "다음을 참조함 : ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return trWriteList(numEntries)+"를 구현.";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return trWriteList(numEntries)+"에서 구현되었습니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "목차";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "잘못된 코드 목록";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "이벤트";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "이벤트 문서화";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "패키지 타입";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "패키지 함수";
    }
    QCString trPackageMembers() override
    {
      return "패키지 멤버들";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "정적 패키지 함수";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "패키지 속성";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "정적 패키지 속성";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "모두";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "이 함수 내부에서 호출하는 함수들에 대한 그래프입니다.:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "결과:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " 소스 파일";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "디렉토리 계통도"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "디렉토리 문서화"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "디렉토리"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" 디렉토리 참조"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool, bool singular) override
    {
      return createNoun(false, singular, "디렉토리", "들");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "이 함수는 편의를 제공하기 위해 오버로드된 멤버 함수입니다. "
              "위의 함수와 틀린 점은 단지 받아들이는 아규먼트(argument)가 다르다는 것입니다.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "이 함수를 호출하는 함수들에 대한 그래프입니다.:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "열거형 문서화"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "멤버 함수/서브루틴 문서화"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "데이터 타입 목록"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "데이터 필드"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "대략적인 설명과 함께 데이터 타입들의 목록입니다.:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll)
      {
        result+="문서화된 ";
      }
      result+="모든 데이터 타입 멤버들의 목록입니다. ";

      result+="각 항목은 ";
      if (!extractAll)
      {
         result+="각 멤버에 대한 데이터 구조 문서화 페이지의 링크를 가지고 있습니다.";
      }
      else
      {
         result+="그들이 속한 데이터 타입의 링크를 가지고 있습니다. :";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "데이터 타입 색인"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "데이터 타입 문서화"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "함수/서브루틴"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "함수/서브루틴 문서화"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "데이터 타입들"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "모듈 목록"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="다음은 ";
      if (!extractAll) result+="문서화된 ";
      result+="모든 모듈에 대한 목록입니다. (간략한 설명만을 보여줍니다) :";
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
        case ClassDef::Class:      result+=" 모듈"; break;
        case ClassDef::Struct:     result+=" 타입"; break;
        case ClassDef::Union:      result+=" 공용체"; break;
        case ClassDef::Interface:  result+=" 인터페이스"; break;
        case ClassDef::Protocol:   result+=" 프로토콜"; break;
        case ClassDef::Category:   result+=" 카테고리"; break;
        case ClassDef::Exception:  result+=" 예외"; break;
        default: break;
      }
      if (isTemplate) result+=" 템플릿";
      result+=" 참조";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" 모듈 참조";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "모듈 멤버들"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "모듈 색인"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool, bool singular) override
    {
      return createNoun(false, singular, "모듈", "들");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="다음 파일";
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
        default: break;
      }
      result+="의 문서화 페이지:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool, bool singular) override
    {
      return createNoun(false, singular, "타입", "들");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool, bool singular) override
    {
      return createNoun(false, singular, "서브프로그램", "들");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "타입 한정자들";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" 관계";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "로딩중...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "전역 이름공간";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "검색중...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "일치하는것 없음";
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
      return QCString(name) + "의 파일";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "월","화","수","목","금","토","일" };
      static const char *months[] = { "1월","2월","3월","4월","5월","6월","7월","8월","9월","10월","11월","12월" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
      static const char *days_short[]   = { "월", "화", "수", "목", "금", "토", "일" };
      static const char *days_full[]    = { "월요일", "화요일", "수요일", "목요일", "금요일", "토요일", "일요일" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
      static const char *months_full[]  = { "1월", "2월", "3월", "4월", "5월", "6월", "7월", "8월", "9월", "10월", "11월", "12월" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "오전", "오후" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "참고 문헌"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString(name) + QCString("에 대한 디렉토리 의존성 그래프:"); }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "상세 단계"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "템플릿 파라메터"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return QCString("그리고 ")+number+"개 더..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool /*single*/) override
    { QCString result = "이 열거형에 대한 문서가 다음 파일(들)로부터 생성되었습니다.:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString(name)+" Enum Reference"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(what) + QCString("(으)로부터 상속된 ") + QCString(members); }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "추가로 상속된 멤버들"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "활성화" : "비활성화";
      return "패널 동기화를 "+opt+"하기 위해 클릭하십시오";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "카테고리 @0에 의해 제공됨.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "클래스 @0 확장.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "클래스 메소드들";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "인스턴스 메소드들";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "메소드 문서화";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "익스포트된 인터페이스들"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "포함된 서비스들"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "상수 그룹들"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" 상수 그룹 레퍼런스";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" 서비스 레퍼런스";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" 싱글톤 레퍼런스";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="이 서비스에 대한 문서화는 다음의 파일";
      if (!single) result+="들";
	  result+="로부터 생성되었습니다.:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="이 싱글톤에 대한 문서화는 다음의 파일";
      if (!single) result+="들";
	  result+="로부터 생성되었습니다.:";
      return result;
    }

};

#endif
