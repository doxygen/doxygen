<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko">
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>이 태그는 구성 파일에서 이 태그 이후에 오는 모든 문자에 사용되는 인코딩을 지정합니다. 기본값은 UTF-8이며, 이 태그가 처음 나타나기 전의 모든 텍스트에도 사용되는 인코딩입니다. Doxygen은 트랜스코딩을 위해 \c libiconv(또는 \c libc에 내장된 iconv)를 사용합니다. 가능한 인코딩 목록은 https://www.gnu.org/software/libiconv/을 참조하세요.</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>\c PROJECT_NAME 태그는 문서가 생성되는 프로젝트를 식별하는 단일 단어(또는 Doxywizard를 사용하지 않는 경우 큰따옴표로 묶인 일련의 단어)입니다. 이 이름은 생성된 대부분의 페이지의 제목과 몇몇 다른 곳에서 사용됩니다.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>\c PROJECT_NUMBER 태그를 사용하여 프로젝트 번호 또는 개정 번호를 입력할 수 있습니다. 이는 생성된 문서를 보관하거나 버전 관리 시스템이 사용되는 경우에 유용합니다.</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>\c PROJECT_BRIEF 태그를 사용하여 각 페이지 상단에 표시되는 프로젝트의 선택적 한 줄 설명을 제공할 수 있으며, 독자에게 프로젝트의 목적에 대한 빠른 아이디어를 제공합니다. 설명은 짧아야 합니다.</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>\c PROJECT_LOGO 태그를 사용하여 문서에 포함될 로고 또는 아이콘을 지정할 수 있습니다. 로고의 최대 높이는 55픽셀을 초과해서는 안 되며, 최대 너비는 200픽셀을 초과해서는 안 됩니다. Doxygen은 로고를 출력 디렉토리에 복사합니다.</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>\c PROJECT_ICON 태그를 사용하여 HTML 문서가 볼 때 탭에 포함될 아이콘을 지정할 수 있습니다. Doxygen은 로고를 출력 디렉토리에 복사합니다.</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>\c OUTPUT_DIRECTORY 태그는 생성된 문서가 기록될 경로(상대 또는 절대)를 지정하는 데 사용됩니다. 상대 경로가 입력되면 Doxygen이 시작된 위치에서의 상대 경로가 됩니다. 비워 두면 현재 디렉토리가 사용됩니다.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>\c CREATE_SUBDIRS 태그가 \c YES로 설정되면 Doxygen은 각 출력 형식의 출력 디렉토리 아래에 최대 4096개의 하위 디렉토리(2레벨)를 생성하고 생성된 파일을 이러한 디렉토리에 분산시킵니다. 이 옵션을 활성화하면 Doxygen에 많은 소스 파일을 공급할 때 편리합니다. 모든 생성 파일을 동일한 디렉토리에 두면 파일 시스템 성능 문제가 발생할 수 있습니다. 하위 디렉토리 수를 제어하려면 \c CREATE_SUBDIRS_LEVEL을 조정하세요.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>\c CREATE_SUBDIRS 태그가 \c YES로 설정된 경우 생성되는 하위 디렉토리 수를 제어합니다. 레벨 0은 16개 디렉토리를 나타내며, 레벨이 증가할 때마다 디렉토리 수가 두 배가 되어 레벨 8에서 4096개 디렉토리가 됩니다. 이것이 기본값이자 최대값입니다. 하위 디렉토리는 2레벨로 구성되며, 첫 번째 레벨은 항상 16개 디렉토리의 고정된 수를 가집니다.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>\c ALLOW_UNICODE_NAMES 태그가 \c YES로 설정되면 Doxygen은 생성된 파일 이름에 비ASCII 문자가 나타나는 것을 허용합니다. \c NO로 설정되면 비ASCII 문자가 이스케이프됩니다. 예를 들어, 유니코드 U+3044의 경우 _xE3_x81_x84가 사용됩니다.</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>\c OUTPUT_LANGUAGE 태그는 Doxygen에 의해 생성되는 모든 문서가 작성되는 언어를 지정하는 데 사용됩니다. Doxygen은 이 정보를 사용하여 적절한 언어로 모든 상수 출력을 생성합니다.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>\c BRIEF_MEMBER_DESC 태그가 \c YES로 설정되면 Doxygen은 파일 및 클래스 문서에 나열된 멤버 뒤에 간결한 멤버 설명을 포함합니다(\c Javadoc과 유사). 이를 비활성화하려면 \c NO로 설정하세요.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>\c REPEAT_BRIEF 태그가 \c YES로 설정되면 Doxygen은 멤버 또는 함수의 간결한 설명을 상세 설명 앞에 추가합니다.&lt;br&gt;참고: \ref cfg_hide_undoc_members &quot;HIDE_UNDOC_MEMBERS&quot;와 \ref cfg_brief_member_desc &quot;BRIEF_MEMBER_DESC&quot;가 모두 \c NO로 설정되면 간결한 설명이 완전히 억제됩니다.</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>이 태그는 다양한 목록의 텍스트를 형성하기 위해 사용되는 준지능적 간결 설명 약어를 구현합니다. 이 목록의 각 문자열이 간결한 설명의 선행 텍스트로 발견되면 텍스트에서 제거되고 전체 목록 처리 후의 결과가 주석이 달린 텍스트로 사용됩니다. 그렇지 않으면 간결한 설명이 그대로 사용됩니다. 비워 두면 다음 값이 사용됩니다(`$name`은 엔티티의 이름으로 자동 교체됨):</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>\c ALWAYS_DETAILED_SEC와 \ref cfg_repeat_brief &quot;REPEAT_BRIEF&quot; 태그가 모두 \c YES로 설정되면 Doxygen은 간결한 설명만 있는 경우에도 상세 섹션을 생성합니다.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>\c INLINE_INHERITED_MEMB 태그가 \c YES로 설정되면 Doxygen은 클래스의 문서에 상속된 모든 멤버를 일반 클래스 멤버인 것처럼 표시합니다. 기본 클래스의 생성자, 소멸자 및 할당 연산자는 표시되지 않습니다.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>\c FULL_PATH_NAMES 태그가 \c YES로 설정되면 Doxygen은 파일 목록과 헤더 파일의 파일 이름 앞에 전체 경로를 추가합니다. \c NO로 설정되면 파일 이름을 고유하게 만드는 가장 짧은 경로가 사용됩니다.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>\c STRIP_FROM_PATH 태그를 사용하여 경로의 사용자 정의 부분을 제거할 수 있습니다. 제거는 지정된 문자열 중 하나가 경로의 왼쪽 부분과 일치하는 경우에만 수행됩니다. 이 태그를 사용하여 파일 목록에 상대 경로를 표시할 수 있습니다. 비워 두면 Doxygen이 실행되는 디렉토리가 제거할 경로로 사용됩니다.&lt;br&gt;여기에 절대 경로를 지정할 수 있지만, Doxygen이 시작되는 디렉토리에서의 상대 경로도 지정할 수 있습니다.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>\c STRIP_FROM_INC_PATH 태그를 사용하여 클래스 문서에서 언급된 경로의 사용자 정의 부분을 제거할 수 있습니다. 이는 클래스를 사용하기 위해 어떤 헤더 파일을 포함해야 하는지 독자에게 알립니다. 비워 두면 클래스 정의를 포함하는 헤더 파일의 이름만 사용됩니다. 그렇지 않으면 일반적으로 `-I` 플래그를 사용하여 컴파일러에 전달되는 포함 경로 목록을 지정해야 합니다.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>\c SHORT_NAMES 태그가 \c YES로 설정되면 Doxygen은 훨씬 짧은(그러나 읽기 어려운) 파일 이름을 생성합니다. 이는 DOS, Mac, CD-ROM과 같이 파일 시스템이 긴 이름을 지원하지 않는 경우에 편리합니다.</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>\c JAVADOC_AUTOBRIEF 태그가 \c YES로 설정되면 Doxygen은 Javadoc 스타일 주석의 첫 번째 줄(첫 번째 마침표, 물음표 또는 느낌표까지)을 간결한 설명으로 해석합니다. \c NO로 설정되면 Javadoc 스타일은 일반 Qt 스타일 주석과 유사하게 작동합니다(따라서 간결한 설명에는 명시적인 \ref cmdbrief &quot;\@brief&quot; 명령이 필요함).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>\c JAVADOC_BANNER 태그가 \c YES로 설정되면 Doxygen은 \verbatim /***************\endverbatim와 같은 줄을 Javadoc 스타일 주석 &quot;배너&quot;의 시작으로 해석합니다. \c NO로 설정되면 Javadoc 스타일은 일반 주석처럼 작동하며 Doxygen은 이를 해석하지 않습니다.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>\c QT_AUTOBRIEF 태그가 \c YES로 설정되면 Doxygen은 Qt 스타일 주석의 첫 번째 줄(첫 번째 마침표, 물음표 또는 느낌표까지)을 간결한 설명으로 해석합니다. \c NO로 설정되면 Qt 스타일은 일반 Qt 스타일 주석과 유사하게 작동합니다(따라서 간결한 설명에는 명시적인 \ref cmdbrief &quot;\brief&quot; 명령이 필요함).</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>\c MULTILINE_CPP_IS_BRIEF 태그를 \c YES로 설정하면 Doxygen은 여러 줄 C++ 특수 주석 블록(즉, \c //! 또는 \c /// 주석 블록)을 간결한 설명으로 처리합니다. 이것이 이전 기본 동작이었습니다. 새로운 기본값은 여러 줄 C++ 주석 블록을 상세 설명으로 처리하는 것입니다. 이전 동작을 선호하는 경우 이 태그를 \c YES로 설정하세요.&lt;br&gt;이 태그를 \c YES로 설정하면 Rational Rose 주석이 인식되지 않는다는 점에도 유의하세요.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>기본적으로 Python docstring은 미리 형식화된 텍스트로 표시되며 Doxygen의 특수 명령을 사용할 수 없습니다. \c PYTHON_DOCSTRING을 \c NO로 설정하면 Doxygen의 특수 명령을 사용할 수 있으며 docstring 문서 블록의 내용이 Doxygen 문서로 표시됩니다.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>\c INHERIT_DOCS 태그가 \c YES로 설정되면 문서화되지 않은 멤버는 재구현하는 문서화된 멤버로부터 문서를 상속받습니다.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>\c SEPARATE_MEMBER_PAGES 태그가 \c YES로 설정되면 Doxygen은 각 멤버에 대한 새 페이지를 생성합니다. \c NO로 설정되면 멤버의 문서는 이를 포함하는 파일/클래스/네임스페이스의 일부가 됩니다.</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>\c TAB_SIZE 태그를 사용하여 탭의 공백 수를 설정할 수 있습니다. Doxygen은 이 값을 사용하여 코드 조각의 탭을 공백으로 대체합니다.</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>이 태그를 사용하여 문서에서 명령으로 작동하는 여러 별칭을 지정할 수 있습니다. 별칭의 형식은 다음과 같습니다: \verbatim name=value\endverbatim 예를 들어, \verbatim &quot;sideeffect=@par Side Effects:^^&quot;\endverbatim 를 추가하면 문서에 \c \sideeffect(또는 \c \@sideeffect) 명령을 배치할 수 있으며, &quot;Side Effects:&quot;라는 제목의 사용자 정의 단락이 생성됩니다. 별칭의 값 부분에 \ref cmdn &quot;\n&quot;을 넣어 줄 바꿈을 삽입할 수 없습니다(결과 출력에서). 별칭의 값 부분에 `^^`를 넣으면 원본 파일에 물리적 줄 바꿈이 있는 것처럼 줄 바꿈을 삽입할 수 있습니다.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>\c OPTIMIZE_OUTPUT_FOR_C 태그를 \c YES로 설정하면 프로젝트가 C 소스로만 구성된 경우 Doxygen은 C에 더 적합한 출력을 생성합니다. 예를 들어, 사용되는 이름의 일부가 다릅니다. 모든 멤버의 목록은 생략됩니다.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>\c OPTIMIZE_OUTPUT_JAVA 태그를 \c YES로 설정하면 프로젝트가 Java 또는 Python 소스로만 구성된 경우 Doxygen은 해당 언어에 더 적합한 출력을 생성합니다. 예를 들어, 네임스페이스가 패키지로 표시되고 정규화된 범위가 다르게 보입니다.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>\c OPTIMIZE_FOR_FORTRAN 태그를 \c YES로 설정하면 프로젝트가 Fortran 소스로 구성된 경우 Doxygen은 Fortran에 적합한 출력을 생성합니다.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>\c OPTIMIZE_OUTPUT_VHDL 태그를 \c YES로 설정하면 프로젝트가 VHDL 소스로 구성된 경우 Doxygen은 VHDL에 적합한 출력을 생성합니다.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>\c OPTIMIZE_OUTPUT_SLICE 태그를 \c YES로 설정하면 프로젝트가 Slice 소스로만 구성된 경우 Doxygen은 해당 언어에 더 적합한 출력을 생성합니다. 예를 들어, 네임스페이스가 모듈로 표시되고 유형이 더 많은 그룹으로 분리됩니다.</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen은 구문 분석하는 파일의 확장자에 따라 사용할 파서를 선택합니다. 이 태그를 사용하여 특정 확장자에 사용할 파서를 할당할 수 있습니다. Doxygen에는 기본 제공 매핑이 있지만 이 태그를 사용하여 재정의하거나 확장할 수 있습니다. 형식은 &lt;code&gt;ext=language&lt;/code&gt;이며, \c ext는 파일 확장자, language는 Doxygen이 지원하는 파서 중 하나입니다: IDL, Java, JavaScript, Csharp(C#), C, C++, Lex, D, PHP, md(Markdown), Objective-C, Python, Slice, VHDL, Fortran(고정 형식 Fortran: FortranFixed, 자유 형식 Fortran: FortranFree, 알 수 없는 형식 Fortran: Fortran. 후자의 경우 파서는 코드가 고정 형식인지 자유 형식인지 추측하려고 시도합니다. 이것이 Fortran 유형 파일의 기본값입니다). 예를 들어 Doxygen이 &lt;code&gt;.inc&lt;/code&gt; 파일을 Fortran 파일로 처리하게 하고(기본값은 PHP), &lt;code&gt;.f&lt;/code&gt; 파일을 C로 처리하게 하려면(기본값은 Fortran), `inc=Fortran f=C`를 사용하세요.&lt;br&gt;참고: 확장자가 없는 파일의 경우 플레이스홀더로 `no_extension`을 사용할 수 있습니다.&lt;br&gt;사용자 정의 확장자의 경우 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;도 설정해야 합니다. 그렇지 않으면 파일이 Doxygen에서 읽히지 않습니다. `no_extension`을 지정하는 경우 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;에 `*`를 추가해야 합니다.&lt;br&gt;\ref default_file_extension_mapping &quot;기본 파일 확장자 매핑&quot; 목록도 참조하세요.</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>\c MARKDOWN_SUPPORT 태그가 활성화되면 Doxygen은 모든 주석을 Markdown 형식에 따라 전처리하여 더 읽기 쉬운 문서를 가능하게 합니다. 자세한 내용은 https://daringfireball.net/projects/markdown/을 참조하세요. Markdown 처리의 출력은 Doxygen에 의해 추가로 처리되므로 Markdown 형식과 Doxygen, HTML 및 XML 명령을 혼합할 수 있습니다. 하위 호환성 문제가 있는 경우에만 비활성화하세요.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>\c MARKDOWN_STRICT 태그가 활성화되면 Doxygen은 Doxygen의 네이티브 마크업 형식이 Markdown과 충돌하더라도 주석 내의 텍스트를 Markdown 형식으로 처리합니다. 이는 백틱이 사용되는 경우에만 관련이 있습니다. Doxygen의 네이티브 마크업 스타일에서는 백틱으로 시작하는 텍스트 조각을 종료하는 데 단일 따옴표를 사용할 수 있으며, 이를 인용된 텍스트로 처리하지만, Markdown에서는 이러한 텍스트 조각이 축어적으로 처리되며 두 번째 일치하는 백틱이 발견된 경우에만 종료됩니다. 또한 Doxygen의 네이티브 마크업 형식에서는 백틱 섹션에 나타나는 큰따옴표를 이스케이프해야 하지만 Markdown에서는 필요하지 않습니다.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>\c TOC_INCLUDE_HEADINGS 태그가 0이 아닌 값으로 설정되면 id 속성이 없는 경우에도 해당 레벨까지의 모든 제목이 목차에 자동으로 포함됩니다.\note 이 기능은 현재 Markdown 제목에만 적용됩니다.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>\c MARKDOWN_ID_STYLE 태그를 사용하여 Markdown 제목의 식별자를 생성하는 데 사용되는 알고리즘을 지정할 수 있습니다. 참고: 각 식별자는 고유합니다.</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>활성화되면 Doxygen은 문서화된 클래스 또는 네임스페이스에 해당하는 단어를 해당 문서에 연결하려고 시도합니다. 이러한 링크는 단어 앞에 \c % 기호를 배치하거나 \c AUTOLINK_SUPPORT를 \c NO로 설정하여 개별 경우에 방지할 수 있습니다. \c AUTOLINK_IGNORE_WORDS 태그에 나열된 단어는 자동 링크에서 제외됩니다.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>이 태그는 문서의 단어 시작과 일치하는 경우 AUTOLINK_SUPPORT를 통한 자동 링크 생성이 활성화된 경우 자동 링크 생성을 억제하는 단어 목록을 지정합니다. 이 목록은 \# 또는 \ref cmdlink &quot;\link&quot; 또는 \ref cmdref &quot;\ref&quot; 명령을 사용하여 명시적으로 생성된 링크에는 영향을 주지 않습니다.</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>STL 클래스(`std::string`, `std::vector` 등)를 사용하지만 STL 소스의 (태그파일)을 입력으로 포함하지 않으려면 이 태그를 \c YES로 설정하여 Doxygen이 인수에 STL 클래스를 포함하는 함수 선언과 정의를 일치시킬 수 있도록 해야 합니다(예: `func(std::string`);와 `func(std::string) {}`). 이렇게 하면 STL 클래스를 포함하는 상속 다이어그램과 협업 다이어그램도 더 완전하고 정확해집니다.</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>Microsoft의 C++/CLI 언어를 사용하는 경우 구문 분석 지원을 활성화하기 위해 이 옵션을 \c YES로 설정해야 합니다.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>\c SIP_SUPPORT 태그를 \c YES로 설정하면 프로젝트가 &lt;a href=&quot;https://www.riverbankcomputing.com/software&quot;&gt;sip&lt;/a&gt; 소스로만 구성된 경우 Doxygen은 이를 일반 C++처럼 구문 분석하지만, 명시적인 보호 키워드가 없는 경우 모든 클래스가 private 상속이 아닌 public 상속을 사용한다고 가정합니다.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Microsoft의 IDL에는 속성의 getter 및 setter 메서드를 나타내는 \c propget 및 \c propput 속성이 있습니다. 이 옵션을 \c YES로 설정하면 Doxygen은 문서의 get 및 set 메서드를 속성으로 대체합니다. 이는 메서드가 실제로 단순 유형을 가져오거나 설정하는 경우에만 작동합니다. 그렇지 않거나 메서드를 표시하려면 이 옵션을 \c NO로 설정해야 합니다.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>문서에서 멤버 그룹화가 사용되고 \c DISTRIBUTE_GROUP_DOC 태그가 \c YES로 설정되면 Doxygen은 그룹의 첫 번째 멤버의 문서(있는 경우)를 그룹의 다른 멤버에 재사용합니다. 기본적으로 그룹의 모든 멤버를 명시적으로 문서화해야 합니다.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>구조체 또는 클래스를 그룹에 추가하고 이 옵션이 활성화된 경우 중첩된 클래스 또는 구조체도 동일한 그룹에 추가됩니다. 기본적으로 이 옵션은 비활성화되어 있으며 \ref cmdingroup &quot;\ingroup&quot;를 통해 중첩된 컴파운드를 명시적으로 추가해야 합니다.</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>\c SUBGROUPING 태그를 \c YES로 설정하면 동일한 유형의 클래스 멤버 그룹(예: public 함수 그룹)을 해당 유형의 하위 그룹으로 배치할 수 있습니다(예: public 함수 섹션 아래). 하위 그룹화를 방지하려면 \c NO로 설정합니다. 또는 \ref cmdnosubgrouping &quot;\nosubgrouping&quot; 명령을 사용하여 클래스별로 수행할 수도 있습니다.</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>\c INLINE_GROUPED_CLASSES 태그가 \c YES로 설정되면 클래스, 구조체 및 공용체가 포함된 그룹 내에 표시됩니다(예: \ref cmdingroup &quot;\ingroup&quot; 사용). 별도의 페이지(HTML 및 Man 페이지의 경우) 또는 섹션(\f$\mbox{\LaTeX}\f$ 및 RTF의 경우) 대신.&lt;br&gt;이 기능은 \ref cfg_separate_member_pages &quot;SEPARATE_MEMBER_PAGES&quot;와 함께 작동하지 않습니다.</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>\c INLINE_SIMPLE_STRUCTS 태그가 \c YES로 설정되면 공용 데이터 필드 또는 단순 typedef 필드만 있는 구조체, 클래스 및 공용체는 정의된 범위가 문서화된 경우 해당 범위의 문서 내에 인라인으로 표시됩니다(즉, 파일, 네임스페이스 또는 그룹의 문서). \c NO로 설정되면 구조체, 클래스 및 공용체는 별도의 페이지(HTML 및 Man 페이지의 경우) 또는 섹션(\f$\mbox{\LaTeX}\f$ 및 RTF의 경우)에 표시됩니다.</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>\c TYPEDEF_HIDES_STRUCT 태그가 활성화되면 구조체, 공용체 또는 열거형의 typedef는 typedef의 이름을 가진 구조체, 공용체 또는 열거형으로 문서화됩니다. 따라서 &lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt;는 문서에서 \c TypeT라는 이름의 구조체로 나타납니다. 비활성화된 경우 typedef는 파일, 네임스페이스 또는 클래스의 멤버로 표시됩니다. 구조체는 \c TypeS라는 이름이 됩니다. 이는 코딩 규칙이 모든 복합 유형을 typedef하고 typedef만 참조되며 태그 이름은 참조되지 않는 C 코드에서 일반적으로 편리합니다.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>기호 조회 캐시의 크기는 \c LOOKUP_CACHE_SIZE를 사용하여 설정할 수 있습니다. 이 캐시는 이름과 범위를 기반으로 기호를 해결하는 데 사용됩니다. 이는 비용이 많이 드는 프로세스이며 동일한 기호가 코드에서 여러 번 나타나는 경우가 많으므로 Doxygen은 미리 해결된 기호의 캐시를 유지합니다. 캐시가 너무 작으면 Doxygen이 느려집니다. 캐시가 너무 크면 메모리가 낭비됩니다. 캐시 크기는 다음 공식으로 제공됩니다: \f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$. 유효한 범위는 0..9이며, 기본값은 0이며, 캐시 크기는 \f$2^{16} = 65536\f$ 기호에 해당합니다. 실행 종료 시 Doxygen은 캐시 사용량을 보고하고 속도 관점에서 최적의 캐시 크기를 제안합니다.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>\c NUM_PROC_THREADS는 처리 중에 Doxygen이 사용을 허용하는 스레드 수를 지정합니다. \c 0으로 설정하면 Doxygen은 시스템에서 사용 가능한 코어 수에 따라 설정합니다. CPU 부하와 처리 속도의 균형을 더 제어하기 위해 0보다 큰 값으로 명시적으로 설정할 수 있습니다. 현재 입력 처리만 여러 스레드를 사용하여 수행할 수 있습니다. 이것은 아직 실험적인 기능이므로 기본값은 1로 설정되어 있으며 병렬 처리는 효과적으로 비활성화되어 있습니다. 문제가 발생하면 보고해 주세요. dot 그래프의 병렬 생성은 \c DOT_NUM_THREADS 설정에 의해 제어됩니다.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>\c TIMESTAMP 태그가 \c NO 이외의 값으로 설정되면 생성된 각 페이지에는 페이지가 생성된 날짜 또는 날짜/시간이 포함됩니다. 이를 \c NO로 설정하면 여러 실행의 출력을 비교할 때 유용합니다.</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>\c EXTRACT_ALL 태그가 \c YES로 설정되면 Doxygen은 문서의 모든 엔티티가 문서화된 것으로 가정합니다. 문서를 사용할 수 없는 경우에도 마찬가지입니다. private 클래스 멤버와 static 파일 멤버는 \ref cfg_extract_private &quot;EXTRACT_PRIVATE&quot; 및 \ref cfg_extract_static &quot;EXTRACT_STATIC&quot; 태그가 \c YES로 설정되지 않은 한 숨겨집니다.\note 이렇게 하면 \ref cfg_warnings &quot;WARNINGS&quot;가 \c YES로 설정된 경우 일반적으로 생성되는 문서화되지 않은 멤버에 대한 경고도 비활성화됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>\c EXTRACT_PRIVATE 태그가 \c YES로 설정되면 클래스의 모든 private 멤버가 문서에 포함됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>\c EXTRACT_PRIV_VIRTUAL 태그가 \c YES로 설정되면 클래스의 문서화된 private 가상 메서드가 문서에 포함됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>\c EXTRACT_PACKAGE 태그가 \c YES로 설정되면 패키지 또는 internal 범위를 가진 모든 멤버가 문서에 포함됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>\c EXTRACT_STATIC 태그가 \c YES로 설정되면 파일의 모든 static 멤버가 문서에 포함됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>\c EXTRACT_LOCAL_CLASSES 태그가 \c YES로 설정되면 소스 파일에서 로컬로 정의된 클래스(및 구조체)가 문서에 포함됩니다. \c NO로 설정되면 헤더 파일에서 정의된 클래스만 포함됩니다. Java 소스에는 효과가 없습니다.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>이 플래그는 Objective-C 코드에만 유용합니다. \c YES로 설정되면 인터페이스가 아닌 구현 섹션에서 정의된 로컬 메서드가 문서에 포함됩니다. \c NO로 설정되면 인터페이스의 메서드만 포함됩니다.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>이 플래그가 \c YES로 설정되면 익명 네임스페이스의 멤버가 추출되어 문서에서 &#x27;anonymous_namespace{file}&#x27;이라는 네임스페이스로 표시됩니다. file은 익명 네임스페이스를 포함하는 파일의 기본 이름으로 대체됩니다. 기본적으로 익명 네임스페이스는 숨겨집니다.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>이 플래그가 \c YES로 설정되면 선언에서 이름 없는 매개변수의 이름은 해당 정의에 의해 결정됩니다. 기본적으로 이름 없는 매개변수는 출력에서 이름 없는 상태로 유지됩니다.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>\c HIDE_UNDOC_MEMBERS 태그가 \c YES로 설정되면 Doxygen은 문서화된 클래스 또는 파일의 문서화되지 않은 모든 멤버를 숨깁니다. \c NO로 설정되면 이러한 멤버는 다양한 개요에 포함되지만 문서 섹션은 생성되지 않습니다. 이 옵션은 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 활성화된 경우 효과가 없습니다.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>\c HIDE_UNDOC_CLASSES 태그가 \c YES로 설정되면 Doxygen은 클래스 계층 구조에서 일반적으로 표시되는 문서화되지 않은 모든 클래스를 숨깁니다. \c NO로 설정되면 이러한 클래스는 다양한 개요에 포함됩니다. 이 옵션은 활성화된 경우 문서화되지 않은 C++ 컨셉도 숨깁니다. 이 옵션은 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 활성화된 경우 효과가 없습니다.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>\c HIDE_UNDOC_NAMESPACES 태그가 \c YES로 설정되면 Doxygen은 네임스페이스 계층 구조에서 일반적으로 표시되는 문서화되지 않은 모든 네임스페이스를 숨깁니다. \c NO로 설정되면 이러한 네임스페이스는 다양한 개요에 포함됩니다. 이 옵션은 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 활성화된 경우 효과가 없습니다.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>\c HIDE_FRIEND_COMPOUNDS 태그가 \c YES로 설정되면 Doxygen은 모든 friend 선언을 숨깁니다. \c NO로 설정되면 이러한 선언은 문서에 포함됩니다.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>\c HIDE_IN_BODY_DOCS 태그가 \c YES로 설정되면 Doxygen은 함수 본문 내에서 발견된 모든 문서 블록을 숨깁니다. \c NO로 설정되면 이러한 블록은 함수의 상세 문서 블록에 추가됩니다.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>\c INTERNAL_DOCS 태그는 \ref cmdinternal &quot;\internal&quot; 명령 뒤에 입력된 문서가 포함되는지 여부를 결정합니다. 태그가 \c NO로 설정되면 문서는 제외됩니다. 내부 문서를 포함하려면 \c YES로 설정하세요.</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>\c CASE_SENSE_NAMES 옵션의 올바른 설정을 통해 Doxygen은 기본 파일 시스템의 기능과 더 잘 일치할 수 있습니다. 파일 시스템이 대소문자를 구분하는 경우(즉, 대소문자만 다른 이름을 가진 파일을 동일한 디렉토리에서 지원하는 경우), 입력에 그러한 파일이 나타나는 경우 옵션을 \c YES로 설정하여 해당 파일을 적절하게 처리해야 합니다. 대소문자를 구분하지 않는 파일 시스템의 경우, 옵션을 \c NO로 설정하여 대소문자만 다른 기호에 대해 작성된 출력 파일을 적절하게 처리해야 합니다. 예를 들어, \c CLASS와 \c Class라는 이름의 두 클래스, 그리고 파일에 대한 참조도 정확히 일치하는 대소문자를 지정하지 않고 지원하기 위해서입니다. Windows(Cygwin 포함) 및 macOS에서는 사용자가 일반적으로 이 옵션을 \c NO로 설정해야 하지만, Linux 또는 기타 Unix 계열에서는 일반적으로 \c YES로 설정해야 합니다.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>\c HIDE_SCOPE_NAMES 태그가 \c NO로 설정되면 Doxygen은 문서에서 완전한 클래스 및 네임스페이스 범위를 가진 멤버를 표시합니다. \c YES로 설정되면 범위가 숨겨집니다.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>\c HIDE_COMPOUND_REFERENCE 태그가 \c NO(기본값)로 설정되면 Doxygen은 페이지 제목에 클래스 참조와 같은 추가 텍스트를 추가합니다. \c YES로 설정되면 컴파운드 참조가 숨겨집니다.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>\c SHOW_HEADERFILE 태그가 \c YES로 설정되면 클래스의 문서에는 클래스를 사용하기 위해 포함해야 하는 파일이 표시됩니다.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>\c SHOW_INCLUDE_FILES 태그가 \c YES로 설정되면 Doxygen은 해당 파일의 문서에 파일에 의해 포함되는 파일 목록을 배치합니다.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>\c SHOW_GROUPED_MEMB_INC 태그가 \c YES로 설정되면 Doxygen은 그룹화된 각 멤버의 문서에 include 문을 추가하여 멤버를 사용하기 위해 어떤 파일을 포함해야 하는지 독자에게 알립니다.</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>\c FORCE_LOCAL_INCLUDES 태그가 \c YES로 설정되면 Doxygen은 문서의 include 파일을 꺾쇠 괄호 대신 큰따옴표로 나열합니다.</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>\c INLINE_INFO 태그가 \c YES로 설정되면 인라인 멤버의 문서에 [inline] 태그가 삽입됩니다.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>\c SORT_MEMBER_DOCS 태그가 \c YES로 설정되면 Doxygen은 파일 및 클래스 멤버의 (상세) 문서를 멤버 이름으로 알파벳순으로 정렬합니다. \c NO로 설정되면 멤버는 선언 순서대로 표시됩니다.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>\c SORT_BRIEF_DOCS 태그가 \c YES로 설정되면 Doxygen은 파일, 네임스페이스 및 클래스 멤버의 간결한 설명을 멤버 이름으로 알파벳순으로 정렬합니다. \c NO로 설정되면 멤버는 선언 순서대로 표시됩니다. 이는 클래스 목록 내의 클래스 순서에도 영향을 미칩니다.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>\c SORT_MEMBERS_CTORS_1ST 태그가 \c YES로 설정되면 Doxygen은 클래스 멤버의 (간결 및 상세) 문서를 정렬하여 생성자와 소멸자가 먼저 나열되도록 합니다. \c NO로 설정되면 생성자는 \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot; 및 \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot;에서 정의된 각각의 순서로 표시됩니다.\note \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot;가 \c NO로 설정되면 이 옵션은 간결한 멤버 문서 정렬에 무시됩니다.\note \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot;가 \c NO로 설정되면 이 옵션은 상세한 멤버 문서 정렬에 무시됩니다.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>\c SORT_GROUP_NAMES 태그가 \c YES로 설정되면 Doxygen은 그룹 이름의 계층 구조를 알파벳순으로 정렬합니다. \c NO로 설정되면 그룹 이름은 정의된 순서대로 표시됩니다.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>\c SORT_BY_SCOPE_NAME 태그가 \c YES로 설정되면 클래스 목록은 네임스페이스를 포함한 완전히 정규화된 이름으로 정렬됩니다. \c NO로 설정되면 클래스 목록은 네임스페이스 부분을 포함하지 않고 클래스 이름만으로 정렬됩니다.\note 이 옵션은 \ref cfg_hide_scope_names &quot;HIDE_SCOPE_NAMES&quot;가 \c YES로 설정된 경우 그다지 유용하지 않습니다.\note 이 옵션은 클래스 목록에만 적용되며 알파벳순 목록에는 적용되지 않습니다.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>\c STRICT_PROTO_MATCHING 옵션이 활성화되고 Doxygen이 함수의 모든 매개변수에 대한 올바른 유형 해결에 실패한 경우, 후보가 하나만 있거나 단순 문자열 일치로 선택할 후보가 명백한 경우에도 멤버 함수의 프로토타입과 구현 간의 일치를 거부합니다. \c STRICT_PROTO_MATCHING을 비활성화하면 Doxygen은 그러한 경우에도 프로토타입과 구현 간의 일치를 수락합니다.</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>\c GENERATE_TODOLIST 태그를 사용하여 todo 목록을 활성화(\c YES) 또는 비활성화(\c NO)할 수 있습니다. 이 목록은 문서에 \ref cmdtodo &quot;\todo&quot; 명령을 배치하여 생성됩니다.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>\c GENERATE_TESTLIST 태그를 사용하여 테스트 목록을 활성화(\c YES) 또는 비활성화(\c NO)할 수 있습니다. 이 목록은 문서에 \ref cmdtest &quot;\test&quot; 명령을 배치하여 생성됩니다.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>\c GENERATE_BUGLIST 태그를 사용하여 버그 목록을 활성화(\c YES) 또는 비활성화(\c NO)할 수 있습니다. 이 목록은 문서에 \ref cmdbug &quot;\bug&quot; 명령을 배치하여 생성됩니다.</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>\c GENERATE_DEPRECATEDLIST 태그를 사용하여 사용되지 않는 목록을 활성화(\c YES) 또는 비활성화(\c NO)할 수 있습니다. 이 목록은 문서에 \ref cmddeprecated &quot;\deprecated&quot; 명령을 배치하여 생성됩니다.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>\c GENERATE_REQUIREMENTS 태그를 사용하여 요구 사항 페이지를 활성화(\c YES) 또는 비활성화(\c NO)할 수 있습니다. 활성화된 경우 이 페이지는 입력에 \ref cmdrequirement &quot;\requirement&quot; 명령을 포함하는 하나 이상의 주석 블록이 나타나면 자동으로 생성됩니다.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>\c REQ_TRACEABILITY_INFO 태그는 요구 사항 페이지에 추적 가능성 정보가 표시되는지 여부를 제어합니다(\ref cmdrequirement &quot;\requirement&quot; 주석 블록을 사용하는 경우에만 관련). \c NO로 설정하면 추적 가능성 정보가 완전히 비활성화됩니다. \c UNSATISFIED_ONLY로 설정하면 충족 관계가 누락된 요구 사항 목록이 표시됩니다(\ref cmdsatisfies &quot;\satisfies&quot; 명령을 통해). 마찬가지로, \c UNVERIFIED_ONLY로 설정하면 검증 관계가 누락된 요구 사항 목록이 표시됩니다(\ref cmdverifies &quot;\verifies&quot; 명령을 통해). 태그를 \c YES(기본값)로 설정하면 해당되는 경우 두 목록이 모두 표시됩니다.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>\c ENABLED_SECTIONS 태그를 사용하여 \ref cmdif &quot;\if&quot; \&lt;section_label\&gt; ... \ref cmdendif &quot;\endif&quot; 및 \ref cmdcond &quot;\cond&quot; \&lt;section_label\&gt; ... \ref cmdendcond &quot;\endcond&quot; 블록으로 표시된 조건부 문서 섹션을 활성화할 수 있습니다.</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>\c MAX_INITIALIZER_LINES 태그는 변수 또는 매크로/정의의 초기 값이 문서에 표시되기 위해 가질 수 있는 최대 줄 수를 결정합니다. 초기화자가 여기에 지정된 줄 수를 초과하면 숨겨집니다. 초기화자를 완전히 숨기려면 0 값을 사용합니다. 개별 변수와 매크로/정의의 값 외관은 이 설정과 관계없이 문서에서 \ref cmdshowinitializer &quot;\showinitializer&quot; 또는 \ref cmdhideinitializer &quot;\hideinitializer&quot; 명령을 사용하여 제어할 수 있습니다.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>\c SHOW_USED_FILES 태그를 \c NO로 설정하면 클래스와 구조체의 문서 하단에 생성되는 파일 목록이 비활성화됩니다. \c YES로 설정되면 목록은 문서 생성에 사용된 파일을 나타냅니다.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>\c SHOW_FILES 태그를 \c NO로 설정하면 파일 페이지 생성이 비활성화됩니다. 이렇게 하면 빠른 인덱스와 폴더 트리 보기(지정된 경우)에서 파일 항목이 제거됩니다.</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>\c SHOW_NAMESPACES 태그를 \c NO로 설정하면 네임스페이스 페이지 생성이 비활성화됩니다. 이렇게 하면 빠른 인덱스와 폴더 트리 보기(지정된 경우)에서 네임스페이스 항목이 제거됩니다.</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>\c FILE_VERSION_FILTER 태그를 사용하여 Doxygen이 각 파일의 현재 버전을 가져오기 위해 호출하는 프로그램 또는 스크립트를 지정할 수 있습니다(일반적으로 버전 관리 시스템에서). Doxygen은 &lt;code&gt;popen()&lt;/code&gt;을 통해 명령&lt;code&gt;command input-file&lt;/code&gt;을 실행하여 프로그램을 호출합니다. 여기서 \c command는 \c FILE_VERSION_FILTER 태그의 값, \c input-file은 Doxygen이 제공하는 입력 파일의 이름입니다. 프로그램이 표준 출력에 쓰는 모든 것은 파일 버전으로 사용됩니다.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>\c LAYOUT_FILE 태그를 사용하여 Doxygen이 구문 분석할 레이아웃 파일을 지정할 수 있습니다. 레이아웃 파일은 생성된 출력 파일의 전역 구조를 출력 형식 독립적인 방식으로 제어합니다. Doxygen의 기본값을 나타내는 레이아웃 파일을 생성하려면 `-l` 옵션을 지정하여 Doxygen을 실행합니다. 선택적으로 파일 이름을 지정할 수 있습니다. 생략하면 \c DoxygenLayout.xml이 레이아웃 파일의 이름으로 사용됩니다. 자세한 내용은 섹션 \ref layout도 참조하세요.&lt;br&gt;\c DoxygenLayout.xml이라는 파일을 포함하는 디렉토리에서 Doxygen을 실행하면 \c LAYOUT_FILE 태그가 비워 두어도 Doxygen이 자동으로 이를 구문 분석합니다.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>\c CITE_BIB_FILES 태그를 사용하여 참조 정의를 포함하는 하나 이상의 \c bib 파일을 지정할 수 있습니다. 이것은 &lt;code&gt;.bib&lt;/code&gt; 파일 목록이어야 합니다. &lt;code&gt;.bib&lt;/code&gt; 확장자는 생략된 경우 자동으로 추가됩니다. 이를 위해서는 \c bibtex 도구가 설치되어 있어야 합니다. 자세한 내용은 https://en.wikipedia.org/wiki/BibTeX도 참조하세요. \f$\mbox{\LaTeX}\f$의 경우 참고 문헌의 스타일은 \ref cfg_latex_bib_style &quot;LATEX_BIB_STYLE&quot;를 사용하여 제어할 수 있습니다. 이 기능을 사용하려면 검색 경로에서 \c bibtex와 \c perl을 사용할 수 있어야 합니다. 참조 생성 방법은 \ref cmdcite &quot;\cite&quot;도 참조하세요.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>\c EXTERNAL_TOOL_PATH 태그를 사용하여 \c latex 및 \c gs와 같은 외부 도구를 찾을 수 있도록 검색 경로(PATH 환경 변수)를 확장할 수 있습니다.\note EXTERNAL_TOOL_PATH에서 지정된 디렉토리는 PATH 변수에서 이미 지정된 경로 앞에 추가되며, 지정된 순서대로 추가됩니다.\note 이 옵션은 Doxywizard에서 Doxygen을 실행하는 경우 macOS 버전 14(Sonoma) 이상에서 특히 편리합니다. 이 경우 PATH에 대한 사용자 정의 변경 사항이 무시되기 때문입니다. macOS의 일반적인 예는 \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim 표준 경로와 함께 설정하면 외부 도구를 시작할 때 doxygen이 사용하는 전체 검색 경로는 \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim이 됩니다.</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>\c QUIET 태그를 사용하여 Doxygen에 의해 표준 출력에 생성되는 메시지를 켜거나 끌 수 있습니다. \c QUIET가 \c YES로 설정되면 메시지가 꺼져 있음을 의미합니다.</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>\c WARNINGS 태그를 사용하여 Doxygen에 의해 표준 오류(\c stderr)에 생성되는 경고 메시지를 켜거나 끌 수 있습니다. \c WARNINGS가 \c YES로 설정되면 경고가 켜져 있음을 의미합니다.&lt;br&gt;\b 팁: 문서 작성 중에 경고를 켜두세요.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>\c WARN_IF_UNDOCUMENTED 태그가 \c YES로 설정되면 Doxygen은 문서화되지 않은 멤버에 대한 경고를 생성합니다. \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 \c YES로 설정되면 이 플래그는 자동으로 비활성화됩니다.</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>\c WARN_IF_DOC_ERROR 태그가 \c YES로 설정되면 Doxygen은 문서의 잠재적 오류(문서화된 함수에서 일부 매개변수를 두 번 문서화, 존재하지 않는 매개변수 문서화, 마크업 명령 잘못 사용 등)에 대해 경고를 생성합니다.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>\c WARN_IF_INCOMPLETE_DOC이 \c YES로 설정되면 Doxygen은 불완전한 함수 매개변수 문서에 대해 경고합니다. \c NO로 설정되면 Doxygen은 일부 매개변수에 문서가 없는 것을 경고 없이 수락합니다.</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>이 \c WARN_NO_PARAMDOC 옵션을 활성화하면 문서화되었지만 매개변수 또는 반환 값 문서가 없는 함수에 대한 경고를 얻을 수 있습니다. \c NO로 설정되면 Doxygen은 매개변수 문서의 오류에 대해서만 경고하고 문서 누락에 대해서는 경고하지 않습니다. \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 \c YES로 설정되면 이 플래그는 자동으로 비활성화됩니다. \ref cfg_warn_if_incomplete_doc &quot;WARN_IF_INCOMPLETE_DOC&quot;도 참조하세요.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>\c WARN_IF_UNDOC_ENUM_VAL 옵션이 \c YES로 설정되면 Doxygen은 문서화되지 않은 열거형 값에 대해 경고합니다. \c NO로 설정되면 Doxygen은 문서화되지 않은 열거형 값을 수락합니다. \ref cfg_extract_all &quot;EXTRACT_ALL&quot;이 \c YES로 설정되면 이 플래그는 자동으로 비활성화됩니다.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>\c WARN_LAYOUT_FILE 옵션이 \c YES로 설정되면 Doxygen은 사용자 정의 레이아웃 파일 구문 분석 중에 발견된 문제(누락 또는 잘못된 요소 등)에 대해 경고합니다. 자세한 내용은 \ref cfg_layout_file &quot;LAYOUT_FILE&quot;도 참조하세요. \c NO로 설정되면 레이아웃 파일 문제가 억제됩니다.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>\c WARN_AS_ERROR 태그가 \c YES로 설정되면 Doxygen은 경고가 발생하자마자 중지됩니다. \c WARN_AS_ERROR 태그가 \c FAIL_ON_WARNINGS로 설정되면 Doxygen은 \c WARN_AS_ERROR 태그가 \c NO로 설정된 것처럼 실행을 계속하지만, Doxygen 프로세스 종료 시 Doxygen은 0이 아닌 상태로 반환됩니다. \c WARN_AS_ERROR 태그가 \c FAIL_ON_WARNINGS_PRINT로 설정되면 Doxygen은 \c FAIL_ON_WARNINGS처럼 작동하지만 \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;이 정의되지 않은 경우 Doxygen은 경고 메시지를 다른 메시지 사이에 쓰지 않고 실행 종료 시 씁니다. \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;이 정의된 경우 경고 메시지는 정의된 파일에 있는 것 외에도 실행 종료 시에도 표시됩니다. 단, \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;이 `-`(즉, 표준 출력 `stdout`)으로 정의된 경우는 제외되며, 이 경우 동작은 \c FAIL_ON_WARNINGS 설정과 동일하게 유지됩니다.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>\c WARN_FORMAT 태그는 Doxygen이 생성할 수 있는 경고 메시지의 형식을 결정합니다. 문자열에는 경고가 발생한 파일과 줄 번호, 그리고 경고 텍스트로 대체되는 &lt;code&gt;\$file&lt;/code&gt;, &lt;code&gt;\$line&lt;/code&gt;, 및 &lt;code&gt;\$text&lt;/code&gt; 태그가 포함되어야 합니다. 선택적으로 형식에는 &lt;code&gt;$version&lt;/code&gt;을 포함할 수 있습니다. 이는 (\ref cfg_file_version_filter &quot;FILE_VERSION_FILTER&quot;를 통해 얻을 수 있는 경우) 파일의 버전으로 대체됩니다.\sa \ref cfg_warn_line_format &quot;WARN_LINE_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>\ref cfg_warn_format &quot;WARN_FORMAT&quot; 명령의 `$text` 부분에서 더 구체적인 위치에 대한 참조가 제공될 수 있습니다. 이 위치로 쉽게 이동할 수 있도록(Doxygen 외부에서) 사용자는 사용자 정의 &quot;잘라내기&quot;/&quot;붙여넣기&quot; 문자열을 정의할 수 있습니다. 예: \verbatim WARN_LINE_FORMAT = &quot;&#x27;vi $file +$line&#x27;&quot; \endverbatim \sa \ref cfg_warn_format &quot;WARN_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>\c WARN_LOGFILE 태그를 사용하여 경고 및 오류 메시지가 기록될 파일을 지정할 수 있습니다. 비워 두면 출력은 표준 오류(`stderr`)에 기록됩니다. 지정된 파일이 쓰기 위해 열 수 없는 경우 경고 및 오류 메시지는 표준 오류에 기록됩니다. `-` 파일이 지정된 경우 경고 및 오류 메시지는 표준 출력(`stdout`)에 기록됩니다.</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>\c INPUT 태그는 문서화된 소스 파일을 포함하는 파일 및/또는 디렉토리를 지정하는 데 사용됩니다. \c myfile.cpp와 같은 파일 이름 또는 \c /usr/src/myproject와 같은 디렉토리를 입력할 수 있습니다. 파일 또는 디렉토리를 공백으로 구분하세요. \ref cfg_file_patterns &quot;FILE_PATTERNS&quot; 및 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;도 참조하세요.\note 이 태그가 비어 있으면 현재 디렉토리가 검색됩니다.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>이 태그를 사용하여 Doxygen이 구문 분석하는 소스 파일의 문자 인코딩을 지정할 수 있습니다. 내부적으로 Doxygen은 UTF-8 인코딩을 사용합니다. Doxygen은 트랜스코딩을 위해 `libiconv`(또는 `libc`에 내장된 `iconv`)를 사용합니다. 가능한 인코딩 목록은 &lt;a href=&quot;https://www.gnu.org/software/libiconv/&quot;&gt;libiconv 문서&lt;/a&gt;를 참조하세요.\sa \ref cfg_input_file_encoding &quot;INPUT_FILE_ENCODING&quot;</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>이 태그를 사용하여 Doxygen이 구문 분석하는 소스 파일의 문자 인코딩을 지정할 수 있습니다. \c INPUT_FILE_ENCODING 태그를 사용하여 파일 패턴별로 문자 인코딩을 지정할 수 있습니다. Doxygen은 파일 이름을 각 패턴과 비교하고 일치하는 경우 기본 \ref cfg_input_encoding &quot;INPUT_ENCODING&quot; 대신 인코딩을 적용합니다. 문자 인코딩은 pattern=encoding 형식의 목록입니다(`*.php=ISO-8859-1`처럼).\sa 지원되는 인코딩에 대한 자세한 내용은 \ref cfg_input_encoding &quot;INPUT_ENCODING&quot;을 참조하세요.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>\ref cfg_input &quot;INPUT&quot; 태그의 값에 디렉토리가 포함된 경우 \c FILE_PATTERNS 태그를 사용하여 디렉토리 내의 소스 파일을 필터링하는 하나 이상의 와일드카드 패턴(`*.cpp` 및 `*.h`처럼)을 지정할 수 있습니다.&lt;br&gt;사용자 정의 확장자 또는 직접 지원되지 않는 확장자의 경우 확장자에 대한 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;도 설정해야 합니다. 그렇지 않으면 파일이 Doxygen에서 읽히지 않습니다.&lt;br&gt;기본적으로 확인되는 파일 패턴 목록은 \ref default_file_extension_mapping &quot;기본 파일 확장자 매핑&quot; 목록과 다를 수 있습니다.&lt;br&gt;비워 두면 다음 패턴이 테스트됩니다:</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>\c RECURSIVE 태그를 사용하여 입력 파일의 하위 디렉토리도 검색할지 여부를 지정할 수 있습니다.</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>\c EXCLUDE 태그를 사용하여 \ref cfg_input &quot;INPUT&quot; 소스 파일에서 제외할 파일 및/또는 디렉토리를 지정할 수 있습니다. 이를 통해 \ref cfg_input &quot;INPUT&quot; 태그에서 루트가 지정된 디렉토리 트리에서 하위 디렉토리를 쉽게 제외할 수 있습니다.&lt;br&gt;상대 경로는 Doxygen이 실행되는 디렉토리에서의 상대 경로입니다.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>\c EXCLUDE_SYMLINKS 태그를 사용하여 심볼릭 링크(Unix 파일 시스템의 기능)인 파일 또는 디렉토리를 입력에서 제외할지 여부를 선택할 수 있습니다.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>\ref cfg_input &quot;INPUT&quot; 태그의 값에 디렉토리가 포함된 경우 \c EXCLUDE_PATTERNS 태그를 사용하여 해당 디렉토리에서 특정 파일을 제외하는 하나 이상의 와일드카드 패턴을 지정할 수 있습니다.&lt;br&gt;와일드카드는 절대 경로를 가진 파일과 일치하므로 모든 테스트 디렉토리를 제외하려면 예를 들어 패턴 `*``/test/``*`를 사용하세요.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>\c EXCLUDE_SYMBOLS 태그를 사용하여 출력에서 제외할 하나 이상의 기호 이름(네임스페이스, 클래스, 함수 등)을 지정할 수 있습니다. 기호 이름은 완전히 정규화된 이름, 단어, 또는 와일드카드 `*`가 사용되는 경우 부분 문자열일 수 있습니다. 예: `ANamespace`, `AClass`, `ANamespace::AClass`, `ANamespace::*Test`</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>\c EXAMPLE_PATH 태그를 사용하여 포함될 코드 예제 조각을 포함하는 하나 이상의 파일 또는 디렉토리를 지정할 수 있습니다(\ref cmdinclude &quot;\include&quot; 명령 참조).</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>\ref cfg_example_path &quot;EXAMPLE_PATH&quot; 태그의 값에 디렉토리가 포함된 경우 \c EXAMPLE_PATTERNS 태그를 사용하여 디렉토리 내의 소스 파일을 필터링하는 하나 이상의 와일드카드 패턴(`*.cpp` 및 `*.h`처럼)을 지정할 수 있습니다. 비워 두면 모든 파일이 포함됩니다.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>\c EXAMPLE_RECURSIVE 태그가 \c YES로 설정되면 \ref cfg_recursive &quot;RECURSIVE&quot; 태그의 값과 관계없이 \ref cmdinclude &quot;\include&quot; 또는 \ref cmddontinclude &quot;\dontinclude&quot; 명령에 사용할 입력 파일의 하위 디렉토리가 검색됩니다.</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>\c IMAGE_PATH 태그를 사용하여 문서에 포함될 이미지를 포함하는 하나 이상의 파일 또는 디렉토리를 지정할 수 있습니다(\ref cmdimage &quot;\image&quot; 명령 참조).</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>\c INPUT_FILTER 태그를 사용하여 Doxygen이 각 입력 파일을 필터링하기 위해 호출하는 프로그램을 지정할 수 있습니다. Doxygen은 &lt;code&gt;popen()&lt;/code&gt;을 통해 명령:&lt;br&gt;&lt;code&gt;\&lt;filter\&gt; \&lt;input-file\&gt;&lt;/code&gt;&lt;br&gt;을 실행하여 필터 프로그램을 호출합니다. 여기서 &lt;code&gt;\&lt;filter\&gt;&lt;/code&gt;는 \c INPUT_FILTER 태그의 값, &lt;code&gt;\&lt;input-file\&gt;&lt;/code&gt;은 입력 파일의 이름입니다. Doxygen은 필터 프로그램이 표준 출력에 쓰는 출력을 사용합니다. \ref cfg_filter_patterns &quot;FILTER_PATTERNS&quot;가 지정된 경우 이 태그는 무시됩니다.&lt;br&gt;필터는 줄을 추가하거나 제거해서는 안 됩니다. 코드가 스캔되기 전에 적용되지만 출력 코드가 생성될 때는 적용되지 않습니다. 줄이 추가되거나 제거되면 앵커가 올바르게 배치되지 않습니다.&lt;br&gt;Doxygen은 처리된 데이터를 표준 출력에 쓰여 추가 처리를 위해 사용하므로 디버그 문이나 사용된 명령과 같은 다른 것(Windows 배치 파일의 경우 항상 `@echo OFF` 사용)은 표준 출력에 쓰여서는 안 됩니다.&lt;br&gt;사용자 정의 확장자 또는 직접 지원되지 않는 확장자의 경우 확장자에 대한 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;도 설정해야 합니다. 그렇지 않으면 파일이 Doxygen에서 올바르게 처리되지 않습니다.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>\c FILTER_PATTERNS 태그를 사용하여 파일 패턴별로 필터를 지정할 수 있습니다. Doxygen은 파일 이름을 각 패턴과 비교하고 일치하는 경우 필터를 적용합니다. 필터는 pattern=filter 형식의 목록입니다(`*.cpp=my_cpp_filter`처럼). 필터 사용 방법은 \ref cfg_input_filter &quot;INPUT_FILTER&quot;를 참조하세요. \c FILTER_PATTERNS 태그가 비어 있거나 패턴 중 어느 것도 파일 이름과 일치하지 않으면 \ref cfg_input_filter &quot;INPUT_FILTER&quot;가 적용됩니다.&lt;br&gt;사용자 정의 확장자 또는 직접 지원되지 않는 확장자의 경우 확장자에 대한 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;도 설정해야 합니다. 그렇지 않으면 파일이 Doxygen에서 올바르게 처리되지 않습니다.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>\c FILTER_SOURCE_FILES 태그가 \c YES로 설정되면 입력 필터(\ref cfg_input_filter &quot;INPUT_FILTER&quot;를 사용하여 설정된 경우)는 브라우징할 소스 파일을 생성하는 데 사용되는 입력 파일도 필터링하는 데 사용됩니다(즉, \ref cfg_source_browser &quot;SOURCE_BROWSER&quot;가 \c YES로 설정된 경우).</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>\c FILTER_SOURCE_PATTERNS 태그를 사용하여 파일 패턴별로 소스 필터를 지정할 수 있습니다. 패턴은 \ref cfg_filter_patterns &quot;FILTER_PATTERN&quot; 설정을 재정의하며(있는 경우), `*.ext=`를 사용하여 특정 패턴에 대한 소스 필터링을 비활성화할 수도 있습니다(필터에 이름을 지정하지 않고).</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>\c USE_MDFILE_AS_MAINPAGE 태그가 입력의 일부인 마크다운 파일의 이름을 참조하는 경우 해당 내용은 메인 페이지(`index.html`)에 배치됩니다. 이는 예를 들어 GitHub에 프로젝트가 있고 소개 페이지도 Doxygen 출력에 재사용하려는 경우에 편리합니다.</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>\c IMPLICIT_DIR_DOCS 태그가 \c YES로 설정되면 프로젝트 루트의 하위 디렉토리에서 발견된 `README.md` 파일은 `README.md`가 \ref cmddir &quot;\dir&quot;, \ref cmdpage &quot;\page&quot;, 또는 \ref cmdmainpage &quot;\mainpage&quot; 명령으로 시작하지 않는 한 해당 하위 디렉토리의 문서로 사용됩니다. \c NO로 설정되면 `README.md` 파일이 디렉토리 문서로 사용되기 위해서는 명시적인 \ref cmddir &quot;\dir&quot; 명령으로 시작해야 합니다.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Fortran 표준은 고정 형식 Fortran 코드에서 위치 72 이후의 모든 문자가 주석으로 간주되도록 규정합니다. 일반적인 확장은 자동 주석이 시작되기 전에 더 긴 줄을 허용하는 것입니다. \c FORTRAN_COMMENT_AFTER 설정을 통해 자동 주석이 시작되기 전에 더 긴 줄을 처리할 수 있습니다.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>\c SOURCE_BROWSER 태그가 \c YES로 설정되면 소스 파일 목록이 생성됩니다. 문서화된 엔티티는 이러한 소스와 상호 참조됩니다.&lt;br&gt;참고: 생성된 출력에서 모든 소스 코드를 제거하려면 \ref cfg_verbatim_headers &quot;VERBATIM_HEADERS&quot;도 \c NO로 설정되어 있는지 확인하세요.</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>\c INLINE_SOURCES 태그를 \c YES로 설정하면 함수, 여러 줄 매크로, 열거형 또는 목록 초기화 변수의 본문이 문서에 직접 포함됩니다.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>\c STRIP_CODE_COMMENTS 태그를 \c YES로 설정하면 Doxygen은 생성된 소스 코드 조각에서 특수 주석 블록을 숨기도록 지시합니다. 일반 C, C++, Fortran 주석은 항상 표시된 상태로 유지됩니다.</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>\c REFERENCED_BY_RELATION 태그가 \c YES로 설정되면 문서화된 각 엔티티에 대해 이를 참조하는 모든 문서화된 함수가 나열됩니다.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>\c REFERENCES_RELATION 태그가 \c YES로 설정되면 문서화된 각 함수에 대해 해당 함수에 의해 호출/사용되는 모든 문서화된 엔티티가 나열됩니다.</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>\c REFERENCES_LINK_SOURCE 태그가 \c YES로 설정되고 \ref cfg_source_browser &quot;SOURCE_BROWSER&quot; 태그가 \c YES로 설정되면 \ref cfg_references_relation &quot;REFERENCES_RELATION&quot; 및 \ref cfg_referenced_by_relation &quot;REFERENCED_BY_RELATION&quot; 목록의 함수에서 하이퍼링크는 소스 코드에 연결됩니다. 그렇지 않으면 문서에 연결됩니다.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>\c SOURCE_TOOLTIPS가 활성화된 경우(기본값) 소스 코드의 하이퍼링크 위에 마우스를 올리면 프로토타입, 간결한 설명, 정의 및 문서에 대한 링크와 같은 추가 정보가 포함된 툴팁이 표시됩니다. 이로 인해 HTML 파일이 커지고 큰 파일의 로딩이 약간 느려질 수 있으므로 이 기능을 비활성화할 수 있습니다.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>\c USE_HTAGS 태그가 \c YES로 설정되면 소스 코드에 대한 참조는 Doxygen의 기본 제공 소스 브라우저 대신 \c htags(1) 도구에 의해 생성된 HTML을 가리킵니다. \c htags 도구는 GNU의 글로벌 소스 태깅 시스템의 일부입니다(https://www.gnu.org/software/global/global.html 참조). 버전 4.8.6 이상이 필요합니다.&lt;br&gt;사용하려면 다음 단계를 수행하세요:-# \c global의 최신 버전을 설치하세요 -# 설정 파일에서 \ref cfg_source_browser &quot;SOURCE_BROWSER&quot;와 \c USE_HTAGS를 활성화하세요 -# \ref cfg_input &quot;INPUT&quot;이 소스 트리의 루트를 가리키는지 확인하세요 -# 평소처럼 \c doxygen을 실행하세요&lt;br&gt;Doxygen은 \c htags를 호출하며(이는 다시 \c gtags를 호출함), 이러한 도구는 명령줄에서(즉, 검색 경로에서) 사용할 수 있어야 합니다.&lt;br&gt;결과: Doxygen에 의해 생성된 소스 브라우저 대신 소스 코드에 대한 링크는 \c htags의 출력을 가리킵니다.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>\c VERBATIM_HEADERS 태그가 \c YES로 설정되면 Doxygen은 include가 지정된 각 클래스의 헤더 파일의 축어적 복사본을 생성합니다. 이를 비활성화하려면 \c NO로 설정하세요.\sa 섹션 \ref cmdclass &quot;\class&quot;.</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>\c CLANG_ASSISTED_PARSING 태그가 \c YES로 설정되면 Doxygen은 &lt;a href=&quot;http://clang.llvm.org/&quot;&gt;clang 파서&lt;/a&gt;를 사용하여 성능 저하를 대가로 더 정확한 구문 분석을 수행합니다. 이는 Doxygen의 기본 제공 파서가 필요한 유형 정보가 부족한 템플릿이 풍부한 C++ 코드에서 특히 유용합니다. @note 이 옵션의 가용성은 Doxygen이 CMake의 `-Duse_libclang=ON` 옵션으로 생성되었는지 여부에 따라 달라집니다.</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>\c CLANG_ASSISTED_PARSING 태그가 \c YES로 설정되고 \c CLANG_ADD_INC_PATHS 태그가 \c YES로 설정되면 Doxygen은 각 입력의 디렉토리를 include 경로에 추가합니다.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>clang 지원 구문 분석이 활성화된 경우 컴파일러를 호출할 때 일반적으로 사용하는 명령줄 옵션을 컴파일러에 제공할 수 있습니다. include 경로는 \ref cfg_input &quot;INPUT&quot; 및 \ref cfg_include_path &quot;INCLUDE_PATH&quot;에서 지정된 파일 및 디렉토리에 대해 Doxygen에 의해 이미 설정되어 있습니다.</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>clang 지원 구문 분석이 활성화된 경우 `compile_commands.json`이라는 파일을 포함하는 디렉토리의 경로를 clang 파서에 제공할 수 있습니다. 이 파일은 소스 파일이 빌드될 때 사용된 옵션을 포함하는 &lt;a href=&quot;http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html&quot;&gt;컴파일 데이터베이스&lt;/a&gt;입니다. 이는 `clang-check`와 같은 clang 도구에 `-p` 옵션을 지정하는 것과 동일합니다. 이러한 옵션은 파서에 전달됩니다. \ref cfg_clang_options &quot;CLANG_OPTIONS&quot;에서 지정된 옵션도 추가됩니다. @note 이 옵션의 가용성은 Doxygen이 CMake의 `-Duse_libclang=ON` 옵션으로 생성되었는지 여부에 따라 달라집니다.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>\c ALPHABETICAL_INDEX 태그가 \c YES로 설정되면 모든 컴파운드의 알파벳순 인덱스가 생성됩니다. 프로젝트에 많은 수의 클래스, 구조체, 공용체 또는 인터페이스가 포함된 경우 이를 활성화하세요.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>\c IGNORE_PREFIX 태그를 사용하여 인덱스 헤더 생성 중에 무시되는 접두사(또는 접두사 목록)를 지정할 수 있습니다. \c IGNORE_PREFIX 태그는 클래스, 함수 및 멤버 이름에 대해 작동합니다. 엔티티는 접두사를 제거한 후 남은 엔티티 이름의 첫 번째 문자 아래에 알파벳순 목록에 배치됩니다.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>\c GENERATE_HTML 태그가 \c YES로 설정되면 Doxygen은 HTML 출력을 생성합니다.</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>\c HTML_OUTPUT 태그는 HTML 문서가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>\c HTML_FILE_EXTENSION 태그를 사용하여 생성되는 각 HTML 페이지의 파일 확장자를 지정할 수 있습니다(예: &lt;code&gt;.htm, .php, .asp&lt;/code&gt;).</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>\c HTML_HEADER 태그를 사용하여 생성되는 각 HTML 페이지의 사용자 정의 HTML 헤더 파일을 지정할 수 있습니다. 태그가 비워 두면 Doxygen은 표준 헤더를 생성합니다. &lt;br&gt;유효한 HTML을 얻으려면 헤더 파일에 Doxygen이 필요로 하는 모든 스크립트와 스타일시트를 포함해야 합니다. 이는 사용되는 설정 옵션에 따라 다릅니다(예: \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 설정). 다음을 사용하여 기본 헤더에서 시작하는 것이 좋습니다: \verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim 그런 다음 파일 \c new_header.html을 수정하세요. Doxygen이 일반적으로 사용하는 기본 헤더 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요. @note 헤더는 변경될 수 있으므로 일반적으로 Doxygen의 새 버전으로 업그레이드할 때 기본 헤더를 다시 생성해야 합니다.</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>\c HTML_FOOTER 태그를 사용하여 생성되는 각 HTML 페이지의 사용자 정의 HTML 푸터를 지정할 수 있습니다. 태그가 비워 두면 Doxygen은 표준 푸터를 생성합니다. 기본 푸터 생성 방법과 푸터 내에서 사용할 수 있는 특수 명령에 대한 자세한 내용은 \ref cfg_html_header &quot;HTML_HEADER&quot;를 참조하세요. Doxygen이 일반적으로 사용하는 기본 푸터 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>\c HTML_STYLESHEET 태그를 사용하여 각 HTML 페이지에서 사용되는 사용자 정의 계단식 스타일시트를 지정할 수 있습니다. HTML 출력의 외관을 미세 조정하는 데 사용할 수 있습니다. 비워 두면 Doxygen은 기본 스타일시트를 생성합니다. Doxygen이 일반적으로 사용하는 스타일시트 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요. \note 더 견고하므로 이 태그 대신 \ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot;를 사용하는 것이 좋습니다. 이 태그(&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;)는 향후 폐기될 예정입니다.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>\c HTML_EXTRA_STYLESHEET 태그를 사용하여 Doxygen에 의해 생성된 표준 스타일시트 이후에 포함되는 추가 사용자 정의 계단식 스타일시트를 지정할 수 있습니다. 이 옵션을 사용하면 특정 스타일 측면을 재정의할 수 있습니다. 이는 표준 스타일시트를 대체하지 않으므로 향후 업데이트에 대해 더 견고한 \ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot;를 사용하는 것보다 권장됩니다. Doxygen은 스타일시트 파일을 출력 디렉토리에 복사합니다. \note 추가 스타일시트 파일의 순서는 중요합니다(예: 목록의 마지막 스타일시트는 목록에서 이전 것의 설정을 재정의함). \note 스크롤바 스타일링은 현재 Webkit/Chromium에서 재정의할 수 없으므로 하나 이상의 추가 스타일시트가 지정된 경우 스타일링은 기본 doxygen.css에서 제외됩니다. 따라서 스크롤바 사용자 정의가 필요한 경우 명시적으로 추가해야 합니다.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>\c HTML_EXTRA_FILES 태그를 사용하여 HTML 출력 디렉토리에 복사될 하나 이상의 추가 이미지 또는 다른 소스 파일을 지정할 수 있습니다. 이러한 파일은 기본 HTML 출력 디렉토리에 복사됩니다. \ref cfg_html_header &quot;HTML_HEADER&quot; 및/또는 \ref cfg_html_footer &quot;HTML_FOOTER&quot; 파일에서 &lt;code&gt;$relpath^&lt;/code&gt; 마커를 사용하여 이러한 파일을 로드합니다. \ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot; 파일에서는 파일 이름만 사용합니다. 또한 파일은 있는 그대로 복사된다는 점에도 유의하세요. 사용 가능한 명령이나 마커가 없습니다.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>\c HTML_COLORSTYLE 태그를 사용하여 생성된 HTML 출력이 어두운 테마 또는 밝은 테마로 렌더링되는지 여부를 지정할 수 있습니다.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>\c HTML_COLORSTYLE_HUE 태그는 HTML 출력의 색상을 제어합니다. Doxygen은 이 색상에 따라 스타일시트와 배경 이미지의 색상을 조정합니다. 색조는 색상 환에서 각도로 지정됩니다. 자세한 내용은 https://en.wikipedia.org/wiki/Hue를 참조하세요. 예를 들어, 값 0은 빨강, 60은 노랑, 120은 녹색, 180은 청록, 240은 파랑, 300은 보라, 360은 다시 빨강을 나타냅니다.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>\c HTML_COLORSTYLE_SAT 태그는 HTML 출력의 색상 순도(또는 채도)를 제어합니다. 값이 0이면 출력은 회색조만 사용합니다. 값이 255이면 가장 선명한 색상이 생성됩니다.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>\c HTML_COLORSTYLE_GAMMA 태그는 HTML 출력의 색상 휘도 성분에 적용되는 감마 보정을 제어합니다. 100 미만의 값은 출력을 점차 밝게 하고, 100을 초과하는 값은 출력을 어둡게 합니다. 값을 100으로 나눈 것이 실제로 적용되는 감마입니다. 따라서 80은 감마 0.8을 나타내고, 값 220은 감마 2.2를 나타내며, 100은 감마를 변경하지 않습니다.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>\c HTML_DYNAMIC_MENUS 태그가 \c YES로 설정되면 생성된 HTML 문서에는 JavaScript를 통해 동적으로 생성되는 수직 탐색 메뉴가 있는 메인 인덱스가 포함됩니다. 비활성화하면 탐색 인덱스는 각 HTML 페이지에 정적으로 포함되는 여러 레벨의 탭으로 구성됩니다. Qt 도움말 브라우저와 같이 JavaScript가 없는 브라우저를 지원하려면 이 옵션을 비활성화하세요.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>\c HTML_DYNAMIC_SECTIONS 태그가 \c YES로 설정되면 생성된 HTML 문서에는 페이지가 로드된 후 숨기거나 표시할 수 있는 섹션이 포함됩니다.</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>\c HTML_CODE_FOLDING 태그가 \c YES로 설정되면 클래스와 함수는 생성된 HTML 소스 코드에서 동적으로 접기 및 펼치기가 가능합니다.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>\c HTML_COPY_CLIPBOARD 태그가 \c YES로 설정되면 Doxygen은 코드 및 텍스트 조각의 오른쪽 상단 모서리에 아이콘을 표시하여 사용자가 내용을 클립보드에 복사할 수 있도록 합니다. 이는 브라우저에서 지원되고 웹 페이지가 &lt;a href=&quot;https://www.w3.org/TR/secure-contexts/&quot;&gt;보안 컨텍스트&lt;/a&gt;를 통해 제공되는 경우에만 작동합니다. 즉, https: 또는 file: 프로토콜을 사용합니다.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen은 브라우저에 몇 가지 설정을 영구적으로 저장합니다(예: 쿠키를 통해). 기본적으로 이러한 설정은 모든 프로젝트에서 Doxygen에 의해 생성된 모든 HTML 페이지에 적용됩니다. \c HTML_PROJECT_COOKIE 태그를 사용하여 설정을 프로젝트별 키 아래에 저장하여 사용자 기본 설정이 별도로 저장되도록 할 수 있습니다.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>\c HTML_INDEX_NUM_ENTRIES를 사용하여 다양한 트리 구조 인덱스에 처음에 표시되는 항목의 선호 수를 제어할 수 있습니다. 사용자는 나중에 항목을 동적으로 확장 및 축소할 수 있습니다. Doxygen은 지정된 수의 항목이 최대로 표시되는 수준까지 트리를 확장합니다(완전히 축소된 트리가 이미 이 양을 초과하는 경우 제외). 따라서 항목 수를 1로 설정하면 기본적으로 완전히 축소된 트리가 생성됩니다. 0은 무한한 항목 수를 나타내는 특수 값이며, 기본적으로 완전히 확장된 트리가 됩니다.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>\c GENERATE_DOCSET 태그가 \c YES로 설정되면 OSX 10.5(Leopard)에서 도입된 &lt;a href=&quot;https://developer.apple.com/xcode/&quot;&gt;Apple의 Xcode 3 통합 개발 환경&lt;/a&gt;에 대한 입력으로 사용할 수 있는 추가 인덱스 파일이 생성됩니다. 문서 세트를 생성하기 위해 Doxygen은 HTML 출력 디렉토리에 Makefile을 생성합니다. \c make를 실행하면 해당 디렉토리에 docset이 생성되고, &lt;code&gt;make install&lt;/code&gt;을 실행하면 docset이 &lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt;에 설치되어 Xcode가 시작 시 이를 찾을 수 있습니다. 자세한 내용은 https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.html을 참조하세요.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>이 태그는 docset 피드의 이름을 결정합니다. 문서 피드는 단일 공급자(회사 또는 제품 제품군 등)의 여러 문서 세트를 그룹화할 수 있는 포괄적인 역할을 합니다.</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>이 태그는 docset 피드의 URL을 결정합니다. 문서 피드는 단일 공급자(회사 또는 제품 제품군 등)의 여러 문서 세트를 그룹화할 수 있는 포괄적인 역할을 합니다.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>이 태그는 문서 세트 번들을 고유하게 식별하는 문자열을 지정합니다. 역 도메인 이름 스타일의 문자열이어야 합니다. 예: &lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;. Doxygen은 이름에 &lt;code&gt;.docset&lt;/code&gt;을 추가합니다.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>\c DOCSET_PUBLISHER_ID 태그는 문서 게시자를 고유하게 식별하는 문자열을 지정합니다. 역 도메인 이름 스타일의 문자열이어야 합니다. 예: &lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>\c DOCSET_PUBLISHER_NAME 태그는 문서 게시자를 식별합니다.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>\c GENERATE_HTMLHELP 태그가 \c YES로 설정되면 Doxygen은 세 개의 추가 HTML 인덱스 파일을 생성합니다: \c index.hhp, \c index.hhc, 및 \c index.hhk. \c index.hhp는 Windows의 Microsoft HTML 도움말 워크샵에서 읽을 수 있는 프로젝트 파일입니다. 2021년 초 Microsoft는 원래 페이지를 오프라인으로 만들었습니다(HTML 도움말 워크샵은 이미 몇 년 동안 유지 관리 모드였음). 웹 아카이브에서 HTML 도움말 워크샵을 다운로드할 수 있습니다: &lt;a href=&quot;http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe&quot;&gt;설치 실행 파일&lt;/a&gt;. &lt;br&gt;HTML 도움말 워크샵에는 Doxygen에 의해 생성된 모든 HTML 출력을 단일 컴파일된 HTML 파일(`.chm`)로 변환할 수 있는 컴파일러가 포함되어 있습니다. 컴파일된 HTML 파일은 이제 Windows 98 도움말 형식으로 사용되며, 향후 모든 Windows 플랫폼에서 이전 Windows 도움말 형식(`.hlp`)을 대체할 것입니다. 압축된 HTML 파일에는 인덱스와 목차도 포함되어 있으며, 문서에서 단어를 검색할 수 있습니다. HTML 워크샵에는 압축된 HTML 파일의 뷰어도 포함되어 있습니다.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>\c CHM_FILE 태그를 사용하여 결과 `.chm` 파일의 파일 이름을 지정할 수 있습니다. 결과를 HTML 출력 디렉토리에 쓰지 않으려면 파일 앞에 경로를 추가할 수 있습니다.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>\c HHC_LOCATION 태그를 사용하여 HTML 도움말 컴파일러(\c hhc.exe)의 위치(파일 이름을 포함한 절대 경로)를 지정할 수 있습니다. 비어 있지 않으면 Doxygen은 생성된 \c index.hhp에서 HTML 도움말 컴파일러를 실행하려고 시도합니다.</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>\c GENERATE_CHI 플래그는 별도의 `.chi` 인덱스 파일이 생성되는지(\c YES) 또는 메인 `.chm` 파일에 포함되는지(\c NO)를 제어합니다.</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>\c CHM_INDEX_ENCODING은 HtmlHelp 인덱스(\c hhk), 콘텐츠(\c hhc) 및 프로젝트 파일의 콘텐츠를 인코딩하는 데 사용됩니다.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>\c BINARY_TOC 플래그는 `.chm` 파일에서 바이너리 목차가 생성되는지(\c YES) 또는 일반 목차가 생성되는지(\c NO)를 제어합니다. 또한 `이전` 및 `다음` 버튼을 활성화합니다.</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>\c TOC_EXPAND 플래그를 \c YES로 설정하면 HTML 도움말 문서의 목차와 트리 보기에 그룹 멤버에 대한 추가 항목이 추가됩니다.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>\c SITEMAP_URL 태그는 문서 배포 중에 사용자가 생성된 문서를 서버에 배치할 위치의 전체 URL을 지정하는 데 사용됩니다. 생성된 사이트맵은 `sitemap.xml`이라고 하며 \ref cfg_html_output &quot;HTML_OUTPUT&quot;에서 지정된 디렉토리에 배치됩니다. \c SITEMAP_URL이 지정되지 않으면 사이트맵이 생성되지 않습니다. 사이트맵 프로토콜에 대한 자세한 내용은 https://www.sitemaps.org를 참조하세요.</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>\c GENERATE_QHP 태그가 \c YES로 설정되고 \ref cfg_qhp_namespace &quot;QHP_NAMESPACE&quot;와 \ref cfg_qhp_virtual_folder &quot;QHP_VIRTUAL_FOLDER&quot;가 모두 설정되면 생성된 HTML 문서의 Qt 압축 도움말(`.qch`)을 생성하기 위해 Qt의 qhelpgenerator에 대한 입력으로 사용할 수 있는 추가 인덱스 파일이 생성됩니다.</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>\ref cfg_qhg_location &quot;QHG_LOCATION&quot; 태그가 지정된 경우 \c QCH_FILE 태그를 사용하여 결과 `.qch` 파일의 파일 이름을 지정할 수 있습니다. 지정된 경로는 HTML 출력 폴더에서의 상대 경로입니다.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>\c QHP_NAMESPACE 태그는 Qt 도움말 프로젝트 출력을 생성할 때 사용할 네임스페이스를 지정합니다. 자세한 내용은 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace&quot;&gt;Qt 도움말 프로젝트/네임스페이스&lt;/a&gt;를 참조하세요.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>\c QHP_VIRTUAL_FOLDER 태그는 Qt 도움말 프로젝트 출력을 생성할 때 사용할 네임스페이스를 지정합니다. 자세한 내용은 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders&quot;&gt;Qt 도움말 프로젝트/가상 폴더&lt;/a&gt;를 참조하세요.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>\c QHP_CUST_FILTER_NAME 태그가 설정되면 추가할 사용자 정의 필터의 이름을 지정합니다. 자세한 내용은 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qt 도움말 프로젝트/사용자 정의 필터&lt;/a&gt;를 참조하세요.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>\c QHP_CUST_FILTER_ATTRS 태그는 추가할 사용자 정의 필터의 속성 목록을 지정합니다. 자세한 내용은 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qt 도움말 프로젝트/사용자 정의 필터&lt;/a&gt;를 참조하세요.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>\c QHP_SECT_FILTER_ATTRS 태그는 이 프로젝트의 필터 섹션이 일치하는 속성 목록을 지정합니다. &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes&quot;&gt;Qt 도움말 프로젝트/필터 속성&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>\c QHG_LOCATION 태그를 사용하여 Qt의 qhelpgenerator의 위치(파일 이름을 포함한 절대 경로)를 지정할 수 있습니다. 비어 있지 않으면 Doxygen은 생성된 `.qhp` 파일에서 qhelpgenerator를 실행하려고 시도합니다.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>\c GENERATE_ECLIPSEHELP 태그가 \c YES로 설정되면 HTML 파일과 함께 추가 인덱스 파일이 생성되며, 이들은 `Eclipse` 도움말 플러그인을 형성합니다. 이 플러그인을 설치하고 `Eclipse`의 도움말 콘텐츠 메뉴 아래에서 사용할 수 있도록 하려면 HTML 및 XML 파일을 포함하는 디렉토리의 내용을 eclipse의 플러그인 디렉토리에 복사해야 합니다. 플러그인 디렉토리 내의 디렉토리 이름은 \ref cfg_eclipse_doc_id &quot;ECLIPSE_DOC_ID&quot; 값과 같아야 합니다. 복사 후 `Eclipse`를 다시 시작해야 도움말이 나타납니다.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>`Eclipse` 도움말 플러그인의 고유 식별자. 플러그인을 설치할 때 HTML 및 XML 파일을 포함하는 디렉토리 이름도 이 이름을 가져야 합니다. 각 문서 세트에는 자체 식별자가 있어야 합니다.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>생성된 HTML 페이지의 레이아웃을 완전히 제어하려는 경우 인덱스를 비활성화하고 자신만의 것으로 교체해야 할 수 있습니다. \c DISABLE_INDEX 태그를 사용하여 각 HTML 페이지 상단의 압축 인덱스(탭)를 켜거나 끌 수 있습니다. \c NO 값은 인덱스를 활성화하고 \c YES 값은 비활성화합니다. 인덱스의 탭에는 탐색 트리와 동일한 정보가 포함되어 있으므로 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;도 \c YES로 설정한 경우 이 옵션을 \c YES로 설정할 수 있습니다.</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>\c GENERATE_TREEVIEW 태그는 계층 정보를 표시하기 위한 트리와 같은 인덱스 구조를 생성할지 여부를 지정하는 데 사용됩니다. 태그 값이 \c YES로 설정되면 트리와 같은 인덱스 구조를 포함하는 사이드 패널이 생성됩니다(HTML 도움말용으로 생성되는 것과 유사). 이것이 작동하려면 JavaScript, DHTML, CSS 및 프레임을 지원하는 브라우저가 필요합니다(즉, 모든 최신 브라우저). Windows 사용자는 HTML 도움말 기능을 사용하는 것이 더 나을 수 있습니다. 사용자 정의 스타일시트(\ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot; 참조)를 통해 인덱스의 외관을 더 미세 조정할 수 있습니다(\ref doxygen_finetune 참조). 예를 들어, Doxygen에 의해 생성된 기본 스타일시트에는 \ref cfg_project_name &quot;PROJECT_NAME&quot; 대신 트리의 루트에 이미지를 배치하는 방법을 보여주는 예가 있습니다. 트리는 기본적으로 탭 인덱스보다 더 자세한 정보를 가지고 있으므로 이 옵션을 활성화할 때 \ref cfg_disable_index &quot;DISABLE_INDEX&quot;를 \c YES로 설정하는 것을 고려할 수 있습니다.</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;가 \c YES로 설정된 경우 \c PAGE_OUTLINE_PANEL 옵션은 화면 오른쪽에 추가 탐색 패널을 표시할지 여부를 결정하며, 메인 페이지의 콘텐츠 개요를 표시합니다(예: https://developer.android.com/reference). \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;가 \c NO로 설정된 경우 이 옵션은 효과가 없습니다.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;가 \c YES로 설정된 경우 \c FULL_SIDEBAR 옵션은 사이드바가 트리 보기 영역에만 제한되는지(값 \c NO) 또는 창의 전체 높이로 확장되는지(값 \c YES)를 결정합니다. 이것을 \c YES로 설정하면 예를 들어 https://docs.readthedocs.io와 유사한 레이아웃이 되며 콘텐츠를 위한 공간이 늘어나지만 프로젝트 로고, 제목 및 설명을 위한 공간이 줄어듭니다. \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;가 \c NO로 설정된 경우 이 옵션은 효과가 없습니다.</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>\c ENUM_VALUES_PER_LINE 태그를 사용하여 생성된 HTML 문서에서 Doxygen이 한 줄에 그룹화할 열거형 값의 수를 설정할 수 있습니다. &lt;br&gt;값이 0이면 개요 섹션에서 열거형 값이 완전히 억제됩니다.</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>\c SHOW_ENUM_VALUES 태그가 설정되면 Doxygen은 열거형 니모닉 옆에 지정된 열거형 값을 표시합니다.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>트리 보기가 활성화된 경우(\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 참조) 이 태그를 사용하여 트리가 표시되는 프레임의 초기 너비(픽셀)를 설정할 수 있습니다.</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>\c EXT_LINKS_IN_WINDOW 옵션이 \c YES로 설정되면 Doxygen은 태그 파일을 통해 가져온 외부 기호에 대한 링크를 별도의 창에서 엽니다.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>\c OBFUSCATE_EMAILS 태그가 \c YES로 설정되면 Doxygen은 이메일 주소를 난독화합니다.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>\c HTML_FORMULA_FORMAT 옵션이 \c svg로 설정되면 Doxygen은 pdf2svg 도구(https://github.com/dawbarton/pdf2svg 참조) 또는 inkscape(https://inkscape.org 참조)를 사용하여 HTML 출력에서 PNG 대신 수식을 SVG 이미지로 생성합니다. 이러한 이미지는 일반적으로 확장된 해상도에서 더 잘 보입니다.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>이 태그를 사용하여 HTML 문서에 이미지로 포함되는 \f$\mbox{\LaTeX}\f$ 수식의 글꼴 크기를 변경할 수 있습니다. Doxygen 실행이 성공한 후 글꼴 크기를 변경하는 경우 강제로 다시 생성하기 위해 HTML 출력 디렉토리에서 모든 `form_*.png` 이미지를 수동으로 제거해야 합니다.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>\c FORMULA_MACROFILE에는 수식의 구성 요소로 사용되는 새로운 \f$\mbox{\LaTeX}\f$ 명령을 만들기 위한 \f$\mbox{\LaTeX}\f$ `\newcommand` 및 `\renewcommand` 명령이 포함될 수 있습니다. 자세한 내용은 섹션 \ref formulas를 참조하세요.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>\c USE_MATHJAX 옵션을 활성화하면 사전 렌더링된 비트맵을 사용하는 대신 클라이언트 측 JavaScript를 사용하여 렌더링하는 MathJax(https://www.mathjax.org 참조)를 사용하여 \f$\mbox{\LaTeX}\f$ 수식을 렌더링합니다. \f$\mbox{\LaTeX}\f$가 설치되어 있지 않거나 HTML 출력에서 수식을 더 보기 좋게 하려는 경우 사용합니다. 활성화한 경우 MathJax를 별도로 설치하고 \ref cfg_mathjax_relpath &quot;MATHJAX_RELPATH&quot; 옵션을 사용하여 경로를 설정해야 할 수도 있습니다.</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>\c MATHJAX_VERSION을 사용하여 사용할 MathJax 버전을 지정할 수 있습니다. MathJax의 다른 버전은 다른 설정에 대해 다른 요구 사항을 가지므로 MathJax의 다른 버전 간에 전환할 때 다른 MathJax 설정도 변경해야 할 수 있습니다.</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>MathJax가 활성화된 경우 MathJax 출력에 사용되는 기본 출력 형식을 설정할 수 있습니다. 출력 형식에 대한 자세한 내용은 &lt;a href=&quot;https://docs.mathjax.org/en/v2.7/output.html&quot;&gt;MathJax 버전 2&lt;/a&gt;, &lt;a href=&quot;https://docs.mathjax.org/en/v3.2/output/index.html&quot;&gt;MathJax 버전 3&lt;/a&gt;, 및 &lt;a href=&quot;https://docs.mathjax.org/en/v4.0/output/index.htm&quot;&gt;MathJax 버전 4&lt;/a&gt;를 참조하세요.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>MathJax가 활성화된 경우 \c MATHJAX_RELPATH 옵션을 사용하여 HTML 출력 디렉토리에서의 상대적 위치를 지정해야 합니다. Mathjax 버전 2의 경우 대상 디렉토리에는 `MathJax.js` 스크립트가 포함되어야 합니다. 예를 들어, \c mathjax 디렉토리가 HTML 출력 디렉토리와 동일한 레벨에 있는 경우 \c MATHJAX_RELPATH는 &lt;code&gt;../mathjax&lt;/code&gt;여야 합니다. Mathjax 버전 3 및 4의 경우 대상 디렉토리에는 `tex-&lt;format&gt;.js` 스크립트(`&lt;format&gt;`은 `chtml` 또는 `svg`)가 포함되어야 합니다. 기본값은 MathJax 콘텐츠 전송 네트워크를 가리키므로 MathJax를 설치하지 않고도 결과를 빠르게 볼 수 있습니다. 그러나 배포 전에 https://www.mathjax.org에서 MathJax의 로컬 복사본을 설치하는 것이 좋습니다. 기본값은: - MathJax 버전 2의 경우: https://cdn.jsdelivr.net/npm/mathjax@2 - MathJax 버전 3의 경우: https://cdn.jsdelivr.net/npm/mathjax@3 - MathJax 버전 4의 경우: https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>\c MATHJAX_EXTENSIONS 태그를 사용하여 MathJax 렌더링 중에 활성화할 하나 이상의 MathJax 확장 이름을 지정할 수 있습니다. 예: MathJax 버전 2(https://docs.mathjax.org/en/v2.7/tex.html 참조): \verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim 예: MathJax 버전 3(https://docs.mathjax.org/en/v3.2/input/tex/extensions/ 참조): \verbatim MATHJAX_EXTENSIONS = ams \endverbatim 예: MathJax 버전 4(https://docs.mathjax.org/en/v4.0/input/tex/extensions/ 참조): \verbatim MATHJAX_EXTENSIONS = units \endverbatim Mathjax 버전 4에서는 꽤 많은 확장이 이미 자동으로 로드된다는 점에 유의하세요. Mathjax 버전 4에서 패키지를 비활성화하려면 마이너스 기호가 앞에 온 패키지 이름을 사용할 수 있습니다(`-`, 예: `MATHJAX_EXTENSIONS += -textmacros`)</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>\c MATHJAX_CODEFILE 태그를 사용하여 MathJax 코드의 시작 시 사용되는 JavaScript 코드 조각을 포함하는 파일을 지정할 수 있습니다. 자세한 내용은 Mathjax 사이트를 참조하세요: - &lt;a href=&quot;https://docs.mathjax.org/en/v2.7/&quot;&gt;MathJax 버전 2&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v3.2/&quot;&gt;MathJax 버전 3&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v4.0/&quot;&gt;MathJax 버전 4&lt;/a&gt;</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>\c SEARCHENGINE 태그가 활성화된 경우 Doxygen은 HTML 출력에 대한 검색 상자를 생성합니다. 기본 검색 엔진은 JavaScript와 DHTML을 사용하며 모든 최신 브라우저에서 작동해야 합니다. HTML 도움말(\ref cfg_generate_htmlhelp &quot;GENERATE_HTMLHELP&quot;), Qt 도움말(\ref cfg_generate_qhp &quot;GENERATE_QHP&quot;) 또는 docsets(\ref cfg_generate_docset &quot;GENERATE_DOCSET&quot;)를 사용하는 경우 검색 기능이 이미 존재하므로 이것은 일반적으로 비활성화해야 합니다. 대규모 프로젝트의 경우 JavaScript 기반 검색 엔진은 느릴 수 있으므로 \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;를 활성화하면 더 나은 솔루션이 제공될 수 있습니다. 키보드를 사용하여 검색할 수 있습니다. 검색 상자로 이동하려면 &lt;code&gt;\&lt;액세스 키\&gt; + S&lt;/code&gt;를 사용합니다(&lt;code&gt;\&lt;액세스 키\&gt;&lt;/code&gt;가 무엇인지는 OS와 브라우저에 따라 다르지만 일반적으로 &lt;code&gt;\&lt;CTRL\&gt;&lt;/code&gt;, &lt;code&gt;\&lt;ALT\&gt;&lt;/code&gt;/&lt;code&gt;\&lt;option\&gt;&lt;/code&gt; 또는 둘 다입니다). 검색 상자 내에서 &lt;code&gt;\&lt;아래 커서 키\&gt;&lt;/code&gt;를 사용하여 검색 결과 창으로 이동하고 &lt;code&gt;\&lt;커서 키\&gt;&lt;/code&gt;를 사용하여 결과를 탐색할 수 있습니다. &lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt;를 눌러 항목을 선택하거나 &lt;code&gt;\&lt;Esc\&gt;&lt;/code&gt;를 눌러 검색을 취소합니다. 커서가 검색 상자에 있을 때 &lt;code&gt;\&lt;Shift\&gt;+\&lt;아래 커서\&gt;&lt;/code&gt;를 누르면 필터 옵션을 선택할 수 있습니다. 여기서도 &lt;code&gt;\&lt;커서 키\&gt;&lt;/code&gt;를 사용하여 필터를 선택하고 &lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt; 또는 &lt;code&gt;\&lt;Esc\&gt;&lt;/code&gt;를 눌러 필터 옵션을 활성화 또는 취소합니다.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>\c SERVER_BASED_SEARCH 태그가 활성화된 경우 검색 엔진은 JavaScript를 사용하는 웹 클라이언트 대신 웹 서버를 사용하여 구현됩니다. \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 설정에 따라 웹 서버 기반 검색에는 두 가지 유형이 있습니다. 비활성화된 경우 Doxygen은 검색용 PHP 스크립트와 스크립트가 사용하는 인덱스 파일을 생성합니다. \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;가 활성화된 경우 인덱싱 및 검색은 외부 도구에 의해 제공되어야 합니다. 자세한 내용은 섹션 \ref extsearch를 참조하세요.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>\c EXTERNAL_SEARCH 태그가 활성화된 경우 Doxygen은 더 이상 검색용 PHP 스크립트를 생성하지 않습니다. 대신 검색 결과는 XML 파일에 기록되며 외부 인덱서에 의해 처리되어야 합니다. Doxygen은 \ref cfg_searchengine_url &quot;SEARCHENGINE_URL&quot; 옵션에서 지정된 외부 검색 엔진을 호출하여 검색 결과를 가져옵니다. &lt;br&gt;Doxygen에는 오픈 소스 검색 엔진 라이브러리 &lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;을 기반으로 하는 샘플 인덱서(\c doxyindexer)와 검색 엔진(&lt;code&gt;doxysearch.cgi&lt;/code&gt;)이 포함되어 있습니다. &lt;br&gt;자세한 내용은 섹션 \ref extsearch를 참조하세요.</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>\c SEARCHENGINE_URL은 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;가 활성화된 경우 검색 결과를 반환하는 웹 서버에 의해 호스팅되는 검색 엔진을 가리켜야 합니다. &lt;br&gt;Doxygen에는 오픈 소스 검색 엔진 라이브러리 &lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;을 기반으로 하는 샘플 인덱서(\c doxyindexer)와 검색 엔진(&lt;code&gt;doxysearch.cgi&lt;/code&gt;)이 포함되어 있습니다. 자세한 내용은 섹션 \ref extsearch를 참조하세요.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>\ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;와 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;가 모두 활성화된 경우 인덱싱되지 않은 검색 데이터는 외부 도구에 의한 인덱싱을 위해 파일에 기록됩니다. \c SEARCHDATA_FILE 태그로 이 파일의 이름을 지정할 수 있습니다.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>\ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;와 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;가 모두 활성화된 경우 \c EXTERNAL_SEARCH_ID 태그를 프로젝트의 식별자로 사용할 수 있습니다. 이는 \ref cfg_extra_search_mappings &quot;EXTRA_SEARCH_MAPPINGS&quot;와 결합하여 여러 프로젝트를 검색하고 결과를 올바른 프로젝트로 리디렉션하는 데 유용합니다.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>\c EXTRA_SEARCH_MAPPINGS 태그를 사용하여 이 구성 파일로 정의된 프로젝트 이외의 Doxygen 프로젝트를 검색할 수 있지만 모두 동일한 외부 검색 인덱스에 추가됩니다. 각 프로젝트에는 \ref cfg_external_search_id &quot;EXTERNAL_SEARCH_ID&quot;를 통해 설정된 고유 ID가 있어야 합니다. 검색 매핑은 ID를 문서를 찾을 수 있는 상대적 위치에 매핑합니다. 형식은: \verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>\c GENERATE_LATEX 태그가 \c YES로 설정되면 Doxygen은 \f$\mbox{\LaTeX}\f$ 출력을 생성합니다.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>\c LATEX_OUTPUT 태그는 \f$\mbox{\LaTeX}\f$ 문서가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>\c LATEX_CMD_NAME 태그를 사용하여 호출할 \f$\mbox{\LaTeX}\f$ 명령 이름을 지정할 수 있습니다. &lt;br&gt;\ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot;를 활성화하지 않으면 기본값은 \c latex입니다. \ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot;를 활성화하면 기본값은 \c pdflatex입니다. 후자의 경우 \c latex가 선택되면 \c pdflatex로 덮어씌워집니다. 특정 출력 언어의 경우 기본값이 다르게 설정되었을 수 있으며, 이는 출력 언어의 구현에 따라 다릅니다.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>\c MAKEINDEX_CMD_NAME 태그를 사용하여 \f$\mbox{\LaTeX}\f$의 인덱스를 생성하는 명령 이름을 지정할 수 있습니다. @note 이 태그는 `Makefile` / `make.bat`에서 사용됩니다. \sa 생성된 출력 파일(`.tex`)의 부분에 대해서는 \ref cfg_latex_makeindex_cmd &quot;LATEX_MAKEINDEX_CMD&quot;.</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>\c LATEX_MAKEINDEX_CMD 태그를 사용하여 \f$\mbox{\LaTeX}\f$의 인덱스를 생성하는 명령 이름을 지정할 수 있습니다. 첫 번째 문자로 백슬래시(`\`)가 없으면 \f$\mbox{\LaTeX}\f$ 코드에 자동으로 추가됩니다. @note 이 태그는 생성된 출력 파일(`.tex`)에서 사용됩니다. \sa `Makefile` / `make.bat`의 부분에 대해서는 \ref cfg_makeindex_cmd_name &quot;MAKEINDEX_CMD_NAME&quot;.</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>\c COMPACT_LATEX 태그가 \c YES로 설정되면 Doxygen은 더 컴팩트한 \f$\mbox{\LaTeX}\f$ 문서를 생성합니다. 이는 작은 프로젝트에 유용하며 일반적으로 몇 그루의 나무를 절약하는 데 도움이 됩니다.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>\c PAPER_TYPE 태그를 사용하여 프린터에서 사용되는 용지 유형을 설정할 수 있습니다.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>\c EXTRA_PACKAGES 태그를 사용하여 \f$\mbox{\LaTeX}\f$ 출력에 포함될 하나 이상의 \f$\mbox{\LaTeX}\f$ 패키지 이름을 지정할 수 있습니다. 패키지는 이름만으로 지정하거나 \f$\mbox{\LaTeX}\f$ `\usepackage` 명령에서 사용할 올바른 구문으로 지정할 수 있습니다. 예를 들어 `times` 글꼴을 얻으려면 다음을 지정할 수 있습니다: \verbatim EXTRA_PACKAGES=times 또는 EXTRA_PACKAGES={times} \endverbatim `amsmath` 패키지와 함께 `intlimits` 옵션을 사용하려면 다음을 지정할 수 있습니다: \verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim 비워 두면 추가 패키지가 포함되지 않습니다.</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>\c LATEX_HEADER 태그를 사용하여 생성된 \f$\mbox{\LaTeX}\f$ 문서의 사용자 정의 \f$\mbox{\LaTeX}\f$ 헤더를 지정할 수 있습니다. 헤더는 첫 번째 장까지의 모든 것을 포함해야 합니다. 비워 두면 Doxygen은 표준 헤더를 생성합니다. 다음을 사용하여 기본 헤더에서 시작하는 것이 좋습니다: \verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim 그런 다음 파일 \c new_header.tex를 수정하세요. Doxygen이 일반적으로 사용하는 기본 헤더 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요. &lt;br&gt;참고: 자신이 무엇을 하고 있는지 이해하는 경우에만 사용자 정의 헤더를 사용하세요! @note 헤더는 변경될 수 있으므로 일반적으로 Doxygen의 새 버전으로 업그레이드할 때 기본 헤더를 다시 생성해야 합니다. 다음 명령은 헤더(및 푸터) 내에서 특별한 의미를 갖습니다:</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>\c LATEX_FOOTER 태그를 사용하여 생성된 \f$\mbox{\LaTeX}\f$ 문서의 사용자 정의 \f$\mbox{\LaTeX}\f$ 푸터를 지정할 수 있습니다. 푸터는 마지막 장 이후의 모든 것을 포함해야 합니다. 비워 두면 Doxygen은 표준 푸터를 생성합니다. 기본 푸터 생성 방법과 푸터 내에서 사용할 수 있는 특수 명령에 대한 자세한 내용은 \ref cfg_latex_header &quot;LATEX_HEADER&quot;를 참조하세요. Doxygen이 일반적으로 사용하는 기본 푸터 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요. 참고: 자신이 무엇을 하고 있는지 이해하는 경우에만 사용자 정의 푸터를 사용하세요!</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>\c LATEX_EXTRA_STYLESHEET 태그를 사용하여 Doxygen에 의해 생성된 표준 스타일시트 이후에 포함되는 추가 사용자 정의 \f$\mbox{\LaTeX}\f$ 스타일시트를 지정할 수 있습니다. 이 옵션을 사용하면 특정 스타일 측면을 재정의할 수 있습니다. Doxygen은 스타일시트 파일을 출력 디렉토리에 복사합니다. \note 추가 스타일시트 파일의 순서는 중요합니다(예: 목록의 마지막 스타일시트는 목록에서 이전 것의 설정을 재정의함).</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>\c LATEX_EXTRA_FILES 태그를 사용하여 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 출력 디렉토리에 복사될 하나 이상의 추가 이미지 또는 다른 소스 파일을 지정할 수 있습니다. 파일은 있는 그대로 복사된다는 점에 유의하세요. 사용 가능한 명령이나 마커가 없습니다.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>\c PDF_HYPERLINKS 태그가 \c YES로 설정되면 생성된 \f$\mbox{\LaTeX}\f$는 PDF로 변환하기 위해 준비됩니다(\c ps2pdf 또는 \c pdflatex 사용). PDF 파일에는 페이지 참조 대신 링크(HTML 출력과 유사)가 포함됩니다. 이를 통해 PDF 뷰어를 사용한 온라인 브라우징에 적합한 출력이 됩니다.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>\c USE_PDFLATEX 태그가 \c YES로 설정되면 Doxygen은 \ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot;에서 지정된 엔진을 사용하여 \f$\mbox{\LaTeX}\f$ 파일에서 직접 PDF 파일을 생성합니다. 더 고품질의 PDF 문서를 얻으려면 이 옵션을 \c YES로 설정하세요. &lt;br&gt;엔진 선택에 대해서는 섹션 \ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot;도 참조하세요.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>\c LATEX_BATCHMODE 태그는 오류가 발생한 경우 \f$\mbox{\LaTeX}\f$의 동작을 알립니다.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>\c LATEX_HIDE_INDICES 태그가 \c YES로 설정되면 Doxygen은 출력에 인덱스 장(파일 인덱스, 컴파운드 인덱스 등)을 포함하지 않습니다.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>\c LATEX_BIB_STYLE 태그를 사용하여 참고 문헌에 사용할 스타일을 지정할 수 있습니다. 예: \c plainnat 또는 \c ieeetr. 자세한 내용은 https://en.wikipedia.org/wiki/BibTeX 및 \ref cmdcite &quot;\cite&quot;를 참조하세요.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>\c LATEX_EMOJI_DIRECTORY 태그는 이모지 이미지가 로드되는 (상대 또는 절대) 경로를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 디렉토리에서의 상대 경로가 됩니다. 비워 두면 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 디렉토리가 사용됩니다.</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>\c GENERATE_RTF 태그가 \c YES로 설정되면 Doxygen은 RTF 출력을 생성합니다. RTF 출력은 Word 97용으로 최적화되어 있으며 다른 RTF 리더/편집기에서는 그다지 예쁘지 않을 수 있습니다.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>\c RTF_OUTPUT 태그는 RTF 문서가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>\c COMPACT_RTF 태그가 \c YES로 설정되면 Doxygen은 더 컴팩트한 RTF 문서를 생성합니다. 이는 작은 프로젝트에 유용하며 일반적으로 몇 그루의 나무를 절약하는 데 도움이 됩니다.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>\c RTF_HYPERLINKS 태그가 \c YES로 설정되면 생성된 RTF에는 하이퍼링크 필드가 포함됩니다. RTF 파일에는 페이지 참조 대신 링크(HTML 출력과 유사)가 포함됩니다. 이를 통해 Word 또는 이러한 필드를 지원하는 다른 Word 호환 리더를 사용한 온라인 브라우징에 적합한 출력이 됩니다. &lt;br&gt;참고: WordPad(write) 및 기타는 링크를 지원하지 않습니다.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>파일에서 스타일시트 정의를 로드합니다. 구문은 Doxygen의 구성 파일과 유사하며 일련의 할당입니다. 재정의만 제공하면 되며 누락된 정의는 기본값으로 설정됩니다. &lt;br&gt;Doxygen이 일반적으로 사용하는 기본 스타일시트 생성 방법에 대해서는 섹션 \ref doxygen_usage도 참조하세요.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>RTF 문서의 생성에서 사용되는 선택적 변수를 정의합니다. 구문은 Doxygen의 구성 파일과 유사합니다. 템플릿 확장 파일은 &lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt;를 사용하여 생성할 수 있습니다.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>\c RTF_EXTRA_FILES 태그를 사용하여 \ref cfg_rtf_output &quot;RTF_OUTPUT&quot; 출력 디렉토리에 복사될 하나 이상의 추가 이미지 또는 다른 소스 파일을 지정할 수 있습니다. 파일은 있는 그대로 복사된다는 점에 유의하세요. 사용 가능한 명령이나 마커가 없습니다.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>\c GENERATE_MAN 태그가 \c YES로 설정되면 Doxygen은 클래스와 파일의 매뉴얼 페이지를 생성합니다.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>\c MAN_OUTPUT 태그는 매뉴얼 페이지가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다. \c man3 디렉토리가 \c MAN_OUTPUT에서 지정된 디렉토리 내에 생성됩니다.</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>\c MAN_EXTENSION 태그는 생성된 매뉴얼 페이지에 추가되는 확장자를 결정합니다. 매뉴얼 섹션이 숫자로 시작하지 않으면 숫자 3이 앞에 추가됩니다. \c MAN_EXTENSION 태그의 선행 마침표(.)는 선택 사항입니다.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>\c MAN_SUBDIR 태그는 매뉴얼 페이지가 배치되는 \c MAN_OUTPUT 내에 생성되는 디렉토리의 이름을 결정합니다. 기본적으로 \c MAN_EXTENSION의 선행 `.`이 제거된 것 뒤에 man이 옵니다.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>\c MAN_LINKS 태그가 \c YES로 설정되고 Doxygen이 man 출력을 생성하는 경우 실제 매뉴얼 페이지 내의 문서화된 각 엔티티에 대한 추가 man 파일이 생성됩니다. 이러한 추가 파일은 실제 매뉴얼 페이지만 소스하지만 없으면 \c man 명령이 올바른 페이지를 찾을 수 없습니다.</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>\c GENERATE_XML 태그가 \c YES로 설정되면 Doxygen은 코드 구조를 캡처하는 XML 파일을 생성하며 모든 문서를 포함합니다.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>\c XML_OUTPUT 태그는 XML 페이지가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>\c XML_PROGRAMLISTING 태그가 \c YES로 설정되면 Doxygen은 XML 출력에 프로그램 목록(구문 강조 및 상호 참조 정보 포함)을 복사합니다. 이를 활성화하면 XML 출력의 크기가 크게 증가합니다.</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>\c XML_NS_MEMB_FILE_SCOPE 태그가 \c YES로 설정되면 Doxygen은 파일 범위 내의 네임스페이스 멤버도 포함하며 HTML 출력에 해당합니다.</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>\c GENERATE_DOCBOOK 태그가 \c YES로 설정되면 Doxygen은 PDF 생성에 사용할 수 있는 Docbook 파일을 생성합니다.</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>\c DOCBOOK_OUTPUT 태그는 Docbook 페이지가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>\c GENERATE_AUTOGEN_DEF 태그가 \c YES로 설정되면 Doxygen은 코드 구조를 캡처하는 AutoGen 정의 파일(https://autogen.sourceforge.net/ 참조)을 생성하며 모든 문서를 포함합니다. 이 기능은 아직 실험적이며 현재 불완전합니다.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>\c GENERATE_SQLITE3 태그가 \c YES로 설정되면 Doxygen은 \c Sqlite3 데이터베이스를 생성하며 Doxygen에 의해 발견된 기호가 테이블에 저장됩니다.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>\c SQLITE3_OUTPUT 태그는 \c Sqlite3 데이터베이스가 배치될 위치를 지정하는 데 사용됩니다. 상대 경로가 입력되면 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;의 값이 그 앞에 배치됩니다.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>\c SQLITE3_RECREATE_DB 태그가 \c YES로 설정되면 기존 doxygen_sqlite3.db 데이터베이스 파일은 Doxygen이 실행될 때마다 다시 생성됩니다. \c NO로 설정되면 Doxygen은 데이터베이스 파일이 이미 발견된 경우 경고하고 수정하지 않습니다.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>\c GENERATE_PERLMOD 태그가 \c YES로 설정되면 Doxygen은 코드 구조를 캡처하는 Perl 모듈 파일을 생성하며 모든 문서를 포함합니다. &lt;br&gt;이 기능은 아직 실험적이며 현재 불완전합니다.</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>\c PERLMOD_LATEX 태그가 \c YES로 설정되면 Doxygen은 필요한 \c Makefile 규칙, \c Perl 스크립트 및 \f$\mbox{\LaTeX}\f$ 코드를 생성하여 Perl 모듈 출력에서 PDF 및 DVI 출력을 생성할 수 있도록 합니다.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>\c PERLMOD_PRETTY 태그가 \c YES로 설정되면 Perl 모듈 출력은 사람이 읽을 수 있도록 예쁘게 형식화됩니다. 이는 무슨 일이 일어나고 있는지 이해하려는 경우에 편리합니다. 반면에 이 태그가 \c NO로 설정되면 Perl 모듈 출력의 크기가 훨씬 작아지며 Perl은 동일하게 구문 분석합니다.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>생성된 `doxyrules.make` 파일의 make 변수 이름은 \c PERLMOD_MAKEVAR_PREFIX에 포함된 문자열이 앞에 옵니다. 이는 동일한 `Makefile`에 포함된 다른 `doxyrules.make` 파일이 서로의 변수를 덮어쓰지 않도록 하는 데 유용합니다.</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>\c ENABLE_PREPROCESSING 태그가 \c YES로 설정되면 Doxygen은 소스 및 포함 파일에서 발견된 모든 C 전처리기 지시문을 평가합니다.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>\c MACRO_EXPANSION 태그가 \c YES로 설정되면 Doxygen은 소스 코드의 모든 매크로 이름을 확장합니다. \c NO로 설정되면 조건부 컴파일만 수행됩니다. 매크로 확장은 \ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot;를 \c YES로 설정하여 제어된 방식으로 수행할 수 있습니다.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>\c EXPAND_ONLY_PREDEF 및 \ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; 태그가 모두 \c YES로 설정되면 매크로 확장은 \ref cfg_predefined &quot;PREDEFINED&quot; 및 \ref cfg_expand_as_defined &quot;EXPAND_AS_DEFINED&quot; 태그로 지정된 매크로로 제한됩니다.</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>\c SEARCH_INCLUDES 태그가 \c YES로 설정되면 \c \#include가 발견된 경우 \ref cfg_include_path &quot;INCLUDE_PATH&quot;의 포함 파일이 검색됩니다.</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>\c INCLUDE_PATH 태그를 사용하여 입력 파일이 아니지만 전처리기에 의해 처리되어야 하는 포함 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다. \c INCLUDE_PATH는 재귀적이지 않으므로 \ref cfg_recursive &quot;RECURSIVE&quot;의 설정은 여기서 효과가 없습니다.</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>\c INCLUDE_FILE_PATTERNS 태그를 사용하여 디렉토리 내의 헤더 파일을 필터링하는 하나 이상의 와일드카드 패턴(`*.h` 및 `*.hpp`처럼)을 지정할 수 있습니다. 비워 두면 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;에서 지정된 패턴이 사용됩니다.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>\c PREDEFINED 태그를 사용하여 전처리기가 시작되기 전에 정의되는 하나 이상의 매크로 이름을 지정할 수 있습니다(예: \c gcc의 `-D` 옵션과 유사). 태그의 인수는 &lt;code&gt;name&lt;/code&gt; 또는 &lt;code&gt;name=definition&lt;/code&gt; 형식의 매크로 목록입니다(공백 없음). 정의와 `=`가 생략된 경우 `=1`이 가정됩니다. 매크로 정의가 \c \#undef를 통해 정의 해제되거나 재귀적으로 확장되는 것을 방지하려면 \c = 연산자 대신 &lt;code&gt;:=&lt;/code&gt; 연산자를 사용하세요.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>\ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; 및 \ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot; 태그가 \c YES로 설정된 경우 이 태그를 사용하여 확장할 매크로 이름 목록을 지정할 수 있습니다. 소스에서 발견된 매크로 정의가 사용됩니다. 소스 코드에서 발견된 정의를 재정의하는 다른 매크로 정의를 사용하려면 \ref cfg_predefined &quot;PREDEFINED&quot; 태그를 사용하세요.</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>\c SKIP_FUNCTION_MACROS 태그가 \c YES로 설정되면 Doxygen의 전처리기는 줄에 단독으로 있고, 모두 대문자 이름을 가지며, 세미콜론으로 끝나지 않는 함수형 매크로에 대한 모든 참조를 제거합니다. 그러한 함수 매크로는 일반적으로 상용구 코드에 사용되며 제거되지 않으면 파서를 혼란스럽게 합니다.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>\c TAGFILES 태그를 사용하여 하나 이상의 태그 파일을 지정할 수 있습니다. 각 태그 파일에 대해 외부 문서의 위치를 추가해야 합니다. 위치 없는 태그 파일의 형식은 다음과 같습니다: \verbatim TAGFILES = file1 file2 ... \endverbatim 태그 파일의 위치 추가는 다음과 같이 수행됩니다: \verbatim TAGFILES = file1=loc1 &quot;file2 = loc2&quot; ... \endverbatim 여기서 `loc1`과 `loc2`는 상대 경로, 절대 경로 또는 URL이 될 수 있습니다. 태그 파일 사용에 대한 자세한 내용은 섹션 \ref external을 참조하세요. \note 각 태그 파일은 고유한 이름을 가져야 합니다(또는 이름은 경로를 포함하지 않음). 태그 파일이 Doxygen이 실행되는 디렉토리에 없는 경우 여기서 태그 파일의 경로도 지정해야 합니다.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>\c GENERATE_TAGFILE 뒤에 파일 이름이 지정되면 Doxygen은 읽은 입력 파일을 기반으로 태그 파일을 생성합니다. 태그 파일 사용에 대한 자세한 내용은 섹션 \ref external을 참조하세요.</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>\c ALLEXTERNALS 태그가 \c YES로 설정되면 모든 외부 클래스와 네임스페이스가 클래스 및 네임스페이스 인덱스에 나열됩니다. \c NO로 설정되면 상속된 외부 클래스만 나열됩니다.</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>\c EXTERNAL_GROUPS 태그가 \c YES로 설정되면 모든 외부 그룹이 주제 인덱스에 나열됩니다. \c NO로 설정되면 현재 프로젝트의 그룹만 나열됩니다.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>\c EXTERNAL_PAGES 태그가 \c YES로 설정되면 모든 외부 페이지가 관련 페이지 인덱스에 나열됩니다. \c NO로 설정되면 현재 프로젝트의 페이지만 나열됩니다.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>\c YES로 설정되면 상속 및 협업 그래프는 대상이 문서화되지 않았거나 클래스가 아닌 경우 상속 및 사용 관계를 숨깁니다.</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>\c HAVE_DOT 태그를 \c YES로 설정하면 Doxygen은 \c path에서 \c dot 도구를 사용할 수 있다고 가정합니다. 이 도구는 AT&amp;T 및 Lucent Bell Labs의 &lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt; 그래프 시각화 도구 키트의 일부입니다. 이 옵션이 \c NO로 설정되면 이 섹션의 다른 옵션은 효과가 없습니다.</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>\c DOT_NUM_THREADS는 Doxygen이 병렬로 실행을 허용하는 \c dot 호출 수를 지정합니다. \c 0으로 설정하면 Doxygen은 시스템에서 사용 가능한 프로세서 수에 따라 설정합니다. CPU 부하와 처리 속도의 균형을 더 제어하기 위해 0보다 큰 값으로 명시적으로 설정할 수 있습니다.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR는 노드, 에지 및 서브그래프 레이블의 공통 속성입니다. Doxygen이 생성하는 dot 파일에서 다른 외관의 글꼴을 사용하려면 fontname, fontcolor 및 fontsize 속성을 지정할 수 있습니다. 자세한 내용은 &lt;a href=https://graphviz.org/doc/info/attrs.html&gt;노드, 에지 및 그래프 속성 사양&lt;/a&gt;을 참조하세요. dot이 글꼴을 찾을 수 있는지 확인해야 합니다. 이는 표준 위치에 배치하거나 \c DOTFONTPATH 환경 변수를 설정하거나 글꼴을 포함하는 디렉토리에 \ref cfg_dot_fontpath &quot;DOT_FONTPATH&quot;를 설정하여 수행할 수 있습니다. graphviz의 기본 글꼴 크기는 14입니다.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR는 \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot;와 연결됩니다. 우아한 스타일을 위해 &#x27;arrowhead=open, arrowtail=open, arrowsize=0.5&#x27;를 추가할 수 있습니다. &lt;a href=https://graphviz.org/doc/info/arrows.html&gt;화살표 모양의 전체 문서.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR는 \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot;와 연결됩니다. 노드 주위의 상자 없는 보기를 위해 &#x27;shape=plain&#x27; 또는 &#x27;shape=plaintext&#x27;를 설정합니다. &lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;모양 사양.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>\ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; 및 다른 dot 속성에서 fontname으로 지정된 글꼴을 dot이 찾을 수 있는 경로를 설정할 수 있습니다.</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>\c CLASS_GRAPH 태그가 \c YES 또는 \c GRAPH 또는 \c BUILTIN으로 설정되면 Doxygen은 문서화된 각 클래스의 그래프를 생성하여 직접 및 간접 상속 관계를 보여줍니다. \c CLASS_GRAPH 태그가 \c YES 또는 \c GRAPH로 설정되고 \ref cfg_have_dot &quot;HAVE_DOT&quot;도 활성화된 경우 `dot`이 그래프를 그리는 데 사용됩니다. \c CLASS_GRAPH 태그가 \c YES로 설정되고 \ref cfg_have_dot &quot;HAVE_DOT&quot;이 비활성화된 경우 또는 \c CLASS_GRAPH 태그가 \c BUILTIN으로 설정된 경우 내장 생성기가 사용됩니다. \c CLASS_GRAPH 태그가 \c TEXT로 설정된 경우 직접 및 간접 상속 관계가 텍스트/링크로 표시됩니다. 특정 클래스의 상속 그래프를 명시적으로 활성화하거나 다른 표현을 선택하려면 \ref cmdinheritancegraph &quot;\inheritancegraph&quot; 명령을 사용할 수 있습니다. 상속 그래프를 비활성화하려면 \ref cmdhideinheritancegraph &quot;\hideinheritancegraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>\c COLLABORATION_GRAPH 태그가 \c YES로 설정되면 Doxygen은 문서화된 각 클래스의 그래프를 생성하여 클래스와 다른 문서화된 클래스 간의 직접 및 간접 구현 종속성(상속, 포함 및 클래스 참조 변수)을 보여줍니다. \c COLLABORATION_GRAPH가 \c NO로 설정된 경우 협업 그래프를 명시적으로 활성화하려면 \ref cmdcollaborationgraph &quot;\collaborationgraph&quot; 명령을 사용할 수 있습니다. 협업 그래프를 비활성화하려면 \ref cmdhidecollaborationgraph &quot;\hidecollaborationgraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>\c UML_LOOK 태그가 \c YES로 설정되면 Doxygen은 OMG의 통합 모델링 언어와 유사한 스타일로 상속 및 협업 다이어그램을 생성합니다.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>\ref cfg_uml_look &quot;UML_LOOK&quot; 태그가 활성화된 경우 필드와 메서드는 클래스 노드 내에 표시됩니다. 많은 필드 또는 메서드와 많은 노드가 있는 경우 그래프가 유용하지 않을 정도로 커질 수 있습니다. \c UML_LIMIT_NUM_FIELDS 임계값은 크기를 더 관리하기 쉽게 만들기 위해 각 유형의 요소 수를 제한합니다. 제한 없이 0으로 설정합니다. 임계값은 제한이 적용되기 전에 50% 초과할 수 있습니다. 따라서 임계값을 10으로 설정하면 최대 15개 필드가 나타날 수 있지만 수가 15를 초과하면 표시되는 총 필드 수는 10으로 제한됩니다.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>\ref cfg_uml_look &quot;UML_LOOK&quot; 태그가 활성화된 경우 필드 레이블은 두 클래스 노드 사이의 에지를 따라 표시됩니다. 많은 필드와 많은 노드가 있는 경우 그래프가 너무 복잡해질 수 있습니다. \c UML_MAX_EDGE_LABELS 임계값은 크기를 더 관리하기 쉽게 만들기 위해 요소 수를 제한합니다. 제한 없이 0으로 설정합니다.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>\c DOT_UML_DETAILS 태그가 \c NO로 설정되면 Doxygen은 UML 그래프에서 유형 및 인수 없이 속성과 메서드를 표시합니다. \c DOT_UML_DETAILS 태그가 \c YES로 설정되면 Doxygen은 UML 그래프에서 속성과 메서드의 유형 및 인수를 추가합니다. \c DOT_UML_DETAILS 태그가 \c NONE으로 설정되면 Doxygen은 UML 그래프에서 클래스 멤버 정보가 있는 필드를 생성하지 않습니다. 클래스 다이어그램은 기본 클래스 다이어그램과 유사하지만 관계에 UML 표기법을 사용합니다.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>\c DOT_WRAP_THRESHOLD 태그를 사용하여 한 줄에 표시되는 최대 문자 수를 설정할 수 있습니다. 실제 줄 길이가 이 임계값을 크게 초과하면 여러 줄로 나뉩니다. 보기 흉한 줄 바꿈을 피하기 위해 몇 가지 휴리스틱이 적용됩니다.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>\c TEMPLATE_RELATIONS 태그가 \c YES로 설정되면 상속 및 협업 그래프는 템플릿과 해당 인스턴스 간의 관계를 보여줍니다.</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>\c INCLUDE_GRAPH, \ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot; 및 \ref cfg_search_includes &quot;SEARCH_INCLUDES&quot; 태그가 모두 \c YES로 설정되면 Doxygen은 문서화된 각 파일의 그래프를 생성하여 파일과 다른 문서화된 파일 간의 직접 및 간접 포함 종속성을 보여줍니다. \c INCLUDE_GRAPH가 \c NO로 설정된 경우 포함 그래프를 명시적으로 활성화하려면 \ref cmdincludegraph &quot;\includegraph&quot; 명령을 사용할 수 있습니다. 포함 그래프를 비활성화하려면 \ref cmdhideincludegraph &quot;\hideincludegraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>\c INCLUDED_BY_GRAPH, \ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot; 및 \ref cfg_search_includes &quot;SEARCH_INCLUDES&quot; 태그가 모두 \c YES로 설정되면 Doxygen은 문서화된 각 파일의 그래프를 생성하여 파일과 다른 문서화된 파일 간의 직접 및 간접 포함 종속성을 보여줍니다. \c INCLUDED_BY_GRAPH가 \c NO로 설정된 경우 포함된 그래프를 명시적으로 활성화하려면 \ref cmdincludedbygraph &quot;\includedbygraph&quot; 명령을 사용할 수 있습니다. 포함된 그래프를 비활성화하려면 \ref cmdhideincludedbygraph &quot;\hideincludedbygraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>\c CALL_GRAPH 태그가 \c YES로 설정되면 Doxygen은 각 전역 함수 또는 클래스 메서드에 대한 호출 종속성 그래프를 생성합니다. &lt;br&gt;이 옵션을 활성화하면 실행 시간이 크게 증가합니다. 따라서 대부분의 경우 \ref cmdcallgraph &quot;\callgraph&quot; 명령을 사용하여 선택한 함수에 대해서만 호출 그래프를 활성화하는 것이 좋습니다. 호출 그래프를 비활성화하려면 \ref cmdhidecallgraph &quot;\hidecallgraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>\c CALLER_GRAPH 태그가 \c YES로 설정되면 Doxygen은 각 전역 함수 또는 클래스 메서드에 대한 호출자 종속성 그래프를 생성합니다. &lt;br&gt;이 옵션을 활성화하면 실행 시간이 크게 증가합니다. 따라서 대부분의 경우 \ref cmdcallergraph &quot;\callergraph&quot; 명령을 사용하여 선택한 함수에 대해서만 호출자 그래프를 활성화하는 것이 좋습니다. 호출자 그래프를 비활성화하려면 \ref cmdhidecallergraph &quot;\hidecallergraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>\c GRAPHICAL_HIERARCHY 태그가 \c YES로 설정되면 Doxygen은 텍스트 대신 모든 클래스의 그래픽 계층 구조를 생성합니다.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>\c DIRECTORY_GRAPH 태그가 \c YES로 설정되면 Doxygen은 디렉토리의 다른 디렉토리에 대한 종속성을 그래픽으로 보여줍니다. 종속성 관계는 디렉토리 내의 파일 간의 \c \#include 관계에 의해 결정됩니다. \c DIRECTORY_GRAPH가 \c NO로 설정된 경우 디렉토리 그래프를 명시적으로 활성화하려면 \ref cmddirectorygraph &quot;\directorygraph&quot; 명령을 사용할 수 있습니다. 디렉토리 그래프를 비활성화하려면 \ref cmdhidedirectorygraph &quot;\hidedirectorygraph&quot; 명령을 사용할 수 있습니다.</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>\c DIR_GRAPH_MAX_DEPTH 태그를 사용하여 \c dot에 의해 생성되는 디렉토리 종속성 그래프에서 생성되는 하위 디렉토리 레벨의 최대 수를 제한할 수 있습니다.</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>\c DOT_IMAGE_FORMAT 태그를 사용하여 \c dot에 의해 생성되는 이미지의 이미지 형식을 설정할 수 있습니다. 이미지 형식에 대한 설명은 \c dot 도구 문서의 출력 형식 섹션을 참조하세요(&lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt;). &lt;br&gt;`svg:cairo` 및 `svg:cairo:cairo` 형식은 \ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot;와 함께 사용할 수 없습니다(\ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot;는 `NO`로 설정됨).</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>\ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot;이 `svg` 또는 `svg:svg` 또는 `svg:svg:core`로 설정된 경우 이 옵션을 \c YES로 설정하여 확대 및 이동이 가능한 대화형 SVG 이미지 생성을 활성화할 수 있습니다. &lt;br&gt;Internet Explorer 이외의 최신 브라우저가 필요합니다. 테스트되어 작동하는 것: Firefox, Chrome, Safari 및 Opera. &lt;br&gt;\ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot;이 `svg:cairo` 또는 `svg:cairo:cairo`로 설정된 경우 이 옵션은 자동으로 비활성화됩니다.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>\c DOT_PATH 태그를 사용하여 \c dot 도구를 찾을 수 있는 경로를 지정할 수 있습니다. 비워 두면 \c dot 도구가 \c path에서 찾을 수 있다고 가정합니다.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>\c DOTFILE_DIRS 태그를 사용하여 문서에 포함될 dot 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다(\ref cmddotfile &quot;\dotfile&quot; 명령 참조).</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>문서에 dia로 만든 다이어그램을 포함할 수 있습니다. Doxygen은 dia를 실행하여 다이어그램을 생성하고 문서에 삽입합니다. DIA_PATH 태그를 사용하면 dia 바이너리가 있는 디렉토리를 지정할 수 있습니다. 비워 두면 dia는 기본 검색 경로에서 찾을 수 있다고 가정합니다.</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>\c DIAFILE_DIRS 태그를 사용하여 문서에 포함될 dia 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다(\ref cmddiafile &quot;\diafile&quot; 명령 참조).</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>PlantUML을 사용하는 경우 \c PLANTUML_JAR_PATH 태그를 사용하여 java가 \c plantuml.jar 파일 또는 사용할 \c jar 파일의 파일 이름을 찾을 수 있는 경로를 지정해야 합니다. 비워 두면 PlantUML이 사용되지 않거나 전처리 단계에서 호출된다고 가정합니다. 이 경우 Doxygen은 \ref cmdstartuml &quot;\startuml&quot; 명령을 만나면 경고를 생성하고 다이어그램에 대한 출력을 생성하지 않습니다.</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>PlantUML을 사용하는 경우 \c PLANTUML_CFG_FILE 태그를 사용하여 PlantUML의 구성 파일을 지정할 수 있습니다.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>PlantUML을 사용하는 경우 지정된 경로는 PlantUML 블록 내의 \c !include 문에 지정된 파일에 대해 검색됩니다.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>\c PLANTUMLFILE_DIRS 태그를 사용하여 문서에 포함될 PlantUml 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다(\ref cmdplantumlfile &quot;\plantumlfile&quot; 명령 참조).</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>CLI 렌더링으로 Mermaid 다이어그램을 사용하는 경우 \c MERMAID_PATH 태그를 사용하여 \c mmdc(Mermaid CLI) 실행 파일을 찾을 수 있는 디렉토리를 지정해야 합니다. 비워 두면 CLI 기반 렌더링이 비활성화됩니다. HTML 출력의 경우 JavaScript를 통한 클라이언트 측 렌더링이 기본적으로 사용되며 \c mmdc가 필요하지 않습니다. \f$\mbox{\LaTeX}\f$/PDF 출력의 경우 이미지를 미리 생성하기 위해 \c mmdc가 필요합니다. CLI 렌더링이 필요하지만 \c mmdc를 사용할 수 없는 경우 Doxygen은 경고를 생성합니다.</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Mermaid 다이어그램을 사용하는 경우 \c MERMAID_CONFIG_FILE 태그를 사용하여 Mermaid CLI 도구(\c mmdc)에 대한 JSON 구성 파일을 지정할 수 있습니다. 이 파일에는 테마 설정 및 기타 Mermaid 구성 옵션이 포함될 수 있습니다.</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>\c MERMAID_RENDER_MODE 태그는 Mermaid 다이어그램이 렌더링되는 방식을 선택합니다.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>\c MERMAID_JS_URL 태그는 클라이언트 측 렌더링을 사용하는 경우(\c MERMAID_RENDER_MODE가 \c CLIENT_SIDE 또는 \c AUTO) \c mermaid.js를 로드할 URL을 지정합니다. 기본값은 jsDelivr CDN의 최신 Mermaid v11을 가리킵니다. \par 예: - 최신 v11(기본값): \c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs&#x27; - 고정된 버전: \c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs&#x27; - 로컬 복사본: \c &#x27;./mermaid.esm.min.mjs&#x27;(사용자는 파일을 HTML 출력 디렉토리에 배치해야 함) \par 기본 CDN URL은 생성된 문서를 볼 때 인터넷 액세스가 필요합니다. 오프라인 사용의 경우 \c mermaid.esm.min.mjs를 다운로드하고 이것을 상대 경로로 설정하거나 대신 \c MERMAID_RENDER_MODE=CLI를 사용하여 이미지를 미리 생성하세요.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>\c MERMAIDFILE_DIRS 태그를 사용하여 문서에 포함될 Mermaid 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다(\ref cmdmermaidfile &quot;\mermaidfile&quot; 명령 참조).</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>\c DOT_GRAPH_MAX_NODES 태그를 사용하여 그래프에 표시될 노드의 최대 수를 설정할 수 있습니다. 그래프의 노드 수가 이 값을 초과하면 Doxygen은 그래프를 잘라내고 노드를 빨간 상자로 표시하여 시각화합니다. 그래프의 루트 노드의 직접 자식 수가 이미 \c DOT_GRAPH_MAX_NODES를 초과하면 그래프가 전혀 표시되지 않습니다. 또한 그래프의 크기는 \ref cfg_max_dot_graph_depth &quot;MAX_DOT_GRAPH_DEPTH&quot;에 의해 더 제한될 수 있습니다.</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>\c MAX_DOT_GRAPH_DEPTH 태그를 사용하여 \c dot에 의해 생성되는 그래프의 최대 깊이를 설정할 수 있습니다. 깊이 값 3은 최대 3개의 에지를 통하는 경로를 통해 루트에서 도달할 수 있는 노드만 표시됨을 의미합니다. 루트 노드에서 더 멀리 있는 노드는 생략됩니다. 이 옵션을 1 또는 2로 설정하면 대규모 코드베이스의 계산 시간이 크게 줄어들 수 있습니다. 또한 그래프의 크기는 \ref cfg_dot_graph_max_nodes &quot;DOT_GRAPH_MAX_NODES&quot;에 의해 더 제한될 수 있습니다. 깊이 0을 사용하면 깊이 제한이 없음을 의미합니다.</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>\c DOT_MULTI_TARGETS 태그를 \c YES로 설정하면 dot이 한 번의 실행에서 여러 출력 파일을 생성할 수 있습니다(즉, 명령줄에서 여러 -o 및 -T 옵션). 이렇게 하면 \c dot이 더 빠르게 작동하지만 \c dot의 새 버전(&gt;1.8.10)만 이를 지원하므로 이 기능은 기본적으로 비활성화되어 있습니다.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>\c GENERATE_LEGEND 태그가 \c YES로 설정되면 Doxygen은 dot에 의해 생성된 그래프의 다양한 상자와 화살표의 의미를 설명하는 범례 페이지를 생성합니다. \note 이 태그는 \ref cfg_uml_look &quot;UML_LOOK&quot;이 설정되지 않은 경우, 즉 상속 및 협업 다이어그램에 Doxygen의 내부 그래픽 표현이 사용되는 경우에 필요합니다.</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>\c DOT_CLEANUP 태그가 \c YES로 설정되면 Doxygen은 다양한 그래프를 생성하는 데 사용되는 중간 파일을 삭제합니다. &lt;br&gt;참고: 이 설정은 dot 파일뿐만 아니라 임시 msc 파일에도 사용됩니다.</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Doxygen 주석 내에서 \ref cmdmsc &quot;\msc&quot; 명령을 사용하여 메시지 시퀀스 다이어그램을 정의할 수 있습니다. \c MSCGEN_TOOL 태그가 비워진 경우(기본값) Doxygen은 다이어그램을 생성하기 위해 내장 버전의 mscgen 도구를 사용합니다. 또는 \c MSCGEN_TOOL 태그로 외부 도구의 이름을 지정할 수도 있습니다. 예를 들어 값으로 `prog`를 지정하면 Doxygen은 도구를 `prog -T &lt;outfile_format&gt; -o &lt;outputfile&gt; &lt;inputfile&gt;`로 호출합니다. 외부 도구는 출력 파일 형식 &quot;png&quot;, &quot;eps&quot;, &quot;svg&quot; 및 &quot;ismap&quot;을 지원해야 합니다.</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>\c MSCFILE_DIRS 태그를 사용하여 문서에 포함될 msc 파일을 포함하는 하나 이상의 디렉토리를 지정할 수 있습니다(\ref cmdmscfile &quot;\mscfile&quot; 명령 참조).</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>\c GROUP_GRAPHS 태그가 \c YES로 설정되면 Doxygen은 그룹의 그래프를 생성하여 그룹의 직접 종속성을 보여줍니다. \c GROUP_GRAPHS가 \c NO로 설정된 경우 그룹 종속 그래프를 명시적으로 활성화하려면 \ref cmdgroupgraph &quot;\groupgraph&quot; 명령을 사용할 수 있습니다. 디렉토리 그래프를 비활성화하려면 \ref cmdhidegroupgraph &quot;\hidegroupgraph&quot; 명령을 사용할 수 있습니다. 매뉴얼의 \ref grouping &quot;그룹화&quot; 장도 참조하세요.</translation>
    </message>
</context>
</TS>
