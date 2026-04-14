<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja">
<context>
    <name>Expert</name>
<message>
        <source>Possible values are:</source>
        <translation>可能な値は:</translation>
    </message>
<message>
        <source>and</source>
        <translation>と</translation>
    </message>
<message>
        <source>The default value is:</source>
        <translation>デフォルト値は</translation>
    </message>
<message>
        <source>Minimum value:</source>
        <translation>最小値:</translation>
    </message>
<message>
        <source>maximum value:</source>
        <translation>最大値:</translation>
    </message>
<message>
        <source>default value:</source>
        <translation>デフォルト値:</translation>
    </message>
<message>
        <source>The default value is: system dependent.</source>
        <translation>デフォルト値はシステム依存です。</translation>
    </message>
<message>
        <source>The default directory is:</source>
        <translation>デフォルトのディレクトリは:</translation>
    </message>
<message>
        <source>The default file is:</source>
        <translation>デフォルトのファイルは</translation>
    </message>
<message>
        <source>The default file (with absolute path) is:</source>
        <translation>デフォルトのファイル（絶対パス）は</translation>
    </message>
<message>
        <source>The file has to be specified with full path.</source>
        <translation>ファイルは完全パスで指定する必要があります。</translation>
    </message>
<message>
        <source>The default image is:</source>
        <translation>デフォルトの画像は</translation>
    </message>
<message>
        <source>The default image (with absolute path) is:</source>
        <translation>デフォルトの画像（絶対パス）は</translation>
    </message>
<message>
        <source>The image has to be specified with full path.</source>
        <translation>画像は完全パスで指定する必要があります。</translation>
    </message>
<message>
        <source>This tag requires that the tag</source>
        <translation>このタグはタグ</translation>
    </message>
<message>
        <source>is set to</source>
        <translation>に設定する必要があります</translation>
    </message>
</context>
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>このタグは、設定ファイル内のこれ以降のすべての文字に使用されるエンコーディングを指定します。デフォルトはUTF-8で、このタグが最初に現れる前のすべてのテキストにも使用されるエンコーディングです。Doxygenは\c libiconv（または\c libcに組み込まれたiconv）を使用してトランスコーディングを行います。可能なエンコーディングのリストについては、https://www.gnu.org/software/libiconv/ を参照してください。</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>\c PROJECT_NAMEタグは、ドキュメントが生成されるプロジェクトを識別する単一の単語（またはDoxywizardを使用していない場合は二重引用符で囲まれた一連の単語）です。この名前は、生成されたページのタイトルや他のいくつかの場所で使用されます。</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>\c PROJECT_NUMBERタグを使用して、プロジェクト番号またはリビジョン番号を入力できます。これは、生成されたドキュメントのアーカイブや、バージョン管理システムが使用されている場合に便利です。</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>\c PROJECT_BRIEFタグを使用すると、各ページの上部に表示されるプロジェクトのオプションの1行説明を提供でき、プロジェクトの目的について読者に素早いアイデアを与えることができます。説明は短くしてください。</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>\c PROJECT_LOGOタグを使用すると、ドキュメントに含まれるロゴまたはアイコンを指定できます。ロゴの最大高さは55ピクセルを超えてはならず、最大幅は200ピクセルを超えてはなりません。Doxygenはロゴを出力ディレクトリにコピーします。</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>\c PROJECT_ICONタグを使用すると、HTMLドキュメントが表示されるときにタブに含まれるアイコンを指定できます。Doxygenはロゴを出力ディレクトリにコピーします。</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>\c OUTPUT_DIRECTORYタグは、生成されたドキュメントが書き込まれるパス（相対または絶対）を指定するために使用されます。相対パスが入力された場合、Doxygenが開始された場所からの相対パスになります。空白のままにすると、現在のディレクトリが使用されます。</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>\c CREATE_SUBDIRSタグが\c YESに設定されている場合、Doxygenは各出力形式の出力ディレクトリの下に最大4096のサブディレクトリ（2レベル）を作成し、生成されたファイルをこれらのディレクトリに分散させます。このオプションを有効にすると、Doxygenに大量のソースファイルを供給する場合に便利です。すべての生成ファイルを同じディレクトリに置くと、ファイルシステムのパフォーマンスの問題が発生する可能性があります。サブディレクトリの数を制御するには\c CREATE_SUBDIRS_LEVELを調整してください。</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>\c CREATE_SUBDIRSタグが\c YESに設定されている場合に作成されるサブディレクトリの数を制御します。レベル0は16ディレクトリを表し、レベルの増加ごとにディレクトリの数が倍になり、レベル8では4096ディレクトリになります。これがデフォルトであり最大値でもあります。サブディレクトリは2レベルで編成され、最初のレベルは常に16ディレクトリの固定数を持っています。</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>\c ALLOW_UNICODE_NAMESタグが\c YESに設定されている場合、Doxygenは生成されたファイルの名前に非ASCII文字が表示されることを許可します。\c NOに設定されている場合、非ASCII文字はエスケープされます。例えば、Unicode U+3044には_xE3_x81_x84が使用されます。</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>\c OUTPUT_LANGUAGEタグは、Doxygenによって生成されるすべてのドキュメントが書かれる言語を指定するために使用されます。Doxygenはこの情報を使用して、適切な言語ですべての定数出力を生成します。</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>\c BRIEF_MEMBER_DESCタグが\c YESに設定されている場合、Doxygenはファイルおよびクラスのドキュメントにリストされているメンバーの後に簡潔なメンバー説明を含めます（\c Javadocと同様）。これを無効にするには\c NOに設定してください。</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>\c REPEAT_BRIEFタグが\c YESに設定されている場合、Doxygenはメンバーまたは関数の簡潔な説明を詳細な説明の前に追加します。&lt;br&gt;注意：\ref cfg_hide_undoc_members &quot;HIDE_UNDOC_MEMBERS&quot;と\ref cfg_brief_member_desc &quot;BRIEF_MEMBER_DESC&quot;の両方が\c NOに設定されている場合、簡潔な説明は完全に抑制されます。</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>このタグは、様々なリストのテキストを形成するために使用される準インテリジェントな簡潔説明の略語を実装します。このリストの各文字列が簡潔な説明の先頭テキストとして見つかった場合、テキストから削除され、リスト全体の処理後の結果が注釈付きテキストとして使用されます。それ以外の場合、簡潔な説明はそのまま使用されます。空白のままにすると、次の値が使用されます（`$name`はエンティティの名前に自動的に置き換えられます）：</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>\c ALWAYS_DETAILED_SECと\ref cfg_repeat_brief &quot;REPEAT_BRIEF&quot;タグの両方が\c YESに設定されている場合、簡潔な説明しかない場合でも、Doxygenは詳細セクションを生成します。</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>\c INLINE_INHERITED_MEMBタグが\c YESに設定されている場合、Doxygenはクラスのドキュメントに継承されたすべてのメンバーを、それらのメンバーが通常のクラスメンバーであるかのように表示します。基本クラスのコンストラクタ、デストラクタ、代入演算子は表示されません。</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>\c FULL_PATH_NAMESタグが\c YESに設定されている場合、Doxygenはファイルリストとヘッダーファイルのファイル名の前にフルパスを追加します。\c NOに設定されている場合、ファイル名を一意にする最短パスが使用されます。</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>\c STRIP_FROM_PATHタグを使用して、パスのユーザー定義部分を削除できます。削除は、指定された文字列のいずれかがパスの左側部分と一致する場合にのみ行われます。このタグを使用して、ファイルリストに相対パスを表示できます。空白のままにすると、Doxygenが実行されるディレクトリが削除するパスとして使用されます。&lt;br&gt;ここでは絶対パスを指定できますが、Doxygenが開始されるディレクトリからの相対パスも指定できることに注意してください。</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>\c STRIP_FROM_INC_PATHタグを使用して、クラスのドキュメントで言及されているパスのユーザー定義部分を削除できます。これは、クラスを使用するためにどのヘッダーファイルをインクルードするかを読者に伝えます。空白のままにすると、クラス定義を含むヘッダーファイルの名前のみが使用されます。それ以外の場合は、通常`-I`フラグを使用してコンパイラに渡されるインクルードパスのリストを指定する必要があります。</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>\c SHORT_NAMESタグが\c YESに設定されている場合、Doxygenははるかに短い（しかし読みにくい）ファイル名を生成します。これは、DOS、Mac、CD-ROMのようにファイルシステムが長い名前をサポートしていない場合に便利です。</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>\c JAVADOC_AUTOBRIEFタグが\c YESに設定されている場合、DoxygenはJavadocスタイルのコメントの最初の行（最初のピリオド、疑問符、または感嘆符まで）を簡潔な説明として解釈します。\c NOに設定されている場合、Javadocスタイルは通常のQtスタイルのコメントと同様に動作します（したがって、簡潔な説明には明示的な\ref cmdbrief &quot;\@brief&quot;コマンドが必要です）。</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>\c JAVADOC_BANNERタグが\c YESに設定されている場合、Doxygenは\verbatim /***************\endverbatimのような行をJavadocスタイルのコメント「バナー」の開始として解釈します。\c NOに設定されている場合、Javadocスタイルは通常のコメントと同様に動作し、Doxygenはそれを解釈しません。</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>\c QT_AUTOBRIEFタグが\c YESに設定されている場合、DoxygenはQtスタイルのコメントの最初の行（最初のピリオド、疑問符、または感嘆符まで）を簡潔な説明として解釈します。\c NOに設定されている場合、Qtスタイルは通常のQtスタイルのコメントと同様に動作します（したがって、簡潔な説明には明示的な\ref cmdbrief &quot;\brief&quot;コマンドが必要です）。</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>\c MULTILINE_CPP_IS_BRIEFタグを\c YESに設定すると、Doxygenは複数行のC++特別コメントブロック（つまり、\c //!または\c ///コメントのブロック）を簡潔な説明として扱います。これが以前のデフォルトの動作でした。新しいデフォルトは、複数行のC++コメントブロックを詳細な説明として扱うことです。以前の動作を好む場合は、このタグを\c YESに設定してください。&lt;br&gt;このタグを\c YESに設定すると、Rational Roseコメントが認識されなくなることにも注意してください。</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>デフォルトでは、Pythonのdocstringは整形済みテキストとして表示され、Doxygenの特別なコマンドは使用できません。\c PYTHON_DOCSTRINGを\c NOに設定すると、Doxygenの特別なコマンドを使用でき、docstringドキュメントブロックの内容がDoxygenドキュメントとして表示されます。</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>\c INHERIT_DOCSタグが\c YESに設定されている場合、文書化されていないメンバーは、それが再実装する文書化されたメンバーからドキュメントを継承します。</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>\c SEPARATE_MEMBER_PAGESタグが\c YESに設定されている場合、Doxygenは各メンバーの新しいページを生成します。\c NOに設定されている場合、メンバーのドキュメントはそれを含むファイル/クラス/名前空間の一部になります。</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>\c TAB_SIZEタグを使用して、タブのスペース数を設定できます。Doxygenはこの値を使用して、コードフラグメントのタブをスペースに置き換えます。</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>このタグを使用して、ドキュメント内のコマンドとして機能する多数のエイリアスを指定できます。エイリアスの形式は次のとおりです：\verbatim name=value\endverbatim 例えば、\verbatim &quot;sideeffect=@par Side Effects:^^&quot;\endverbatim を追加すると、ドキュメントに\c \sideeffect（または\c \@sideeffect）コマンドを配置でき、「Side Effects:」という見出しのユーザー定義段落が生成されます。エイリアスの値部分に\ref cmdn &quot;\n&quot;を入れて改行を挿入することはできません（結果の出力で）。エイリアスの値部分に`^^`を入れると、元のファイルに物理的な改行があるかのように改行を挿入できます。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>\c OPTIMIZE_OUTPUT_FOR_Cタグを\c YESに設定すると、プロジェクトがCソースのみで構成されている場合、DoxygenはCにより適した出力を生成します。例えば、使用される名前の一部が異なります。すべてのメンバーのリストは省略されます。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>\c OPTIMIZE_OUTPUT_JAVAタグを\c YESに設定すると、プロジェクトがJavaまたはPythonソースのみで構成されている場合、Doxygenはその言語により適した出力を生成します。例えば、名前空間はパッケージとして表示され、修飾されたスコープは異なって見えます。</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>\c OPTIMIZE_FOR_FORTRANタグを\c YESに設定すると、プロジェクトがFortranソースで構成されている場合、DoxygenはFortranに適した出力を生成します。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>\c OPTIMIZE_OUTPUT_VHDLタグを\c YESに設定すると、プロジェクトがVHDLソースで構成されている場合、DoxygenはVHDLに適した出力を生成します。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>\c OPTIMIZE_OUTPUT_SLICEタグを\c YESに設定すると、プロジェクトがSliceソースのみで構成されている場合、Doxygenはその言語により適した出力を生成します。例えば、名前空間はモジュールとして表示され、型はより多くのグループに分離されます。</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygenは、解析するファイルの拡張子に応じて使用するパーサーを選択します。このタグを使用すると、特定の拡張子に使用するパーサーを割り当てることができます。Doxygenには組み込みのマッピングがありますが、このタグを使用して上書きまたは拡張できます。形式は&lt;code&gt;ext=language&lt;/code&gt;で、\c extはファイル拡張子、languageはDoxygenがサポートするパーサーの1つです：IDL、Java、JavaScript、Csharp（C#）、C、C++、Lex、D、PHP、md（Markdown）、Objective-C、Python、Slice、VHDL、Fortran（固定形式Fortran：FortranFixed、自由形式Fortran：FortranFree、不明形式Fortran：Fortran。後者の場合、パーサーはコードが固定形式か自由形式かを推測しようとします。これがFortranタイプのファイルのデフォルトです）。例えば、Doxygenに&lt;code&gt;.inc&lt;/code&gt;ファイルをFortranファイルとして扱わせ（デフォルトはPHP）、&lt;code&gt;.f&lt;/code&gt;ファイルをCとして扱わせる（デフォルトはFortran）には、`inc=Fortran f=C`を使用します。&lt;br&gt;注意：拡張子のないファイルの場合、プレースホルダーとして`no_extension`を使用できます。&lt;br&gt;カスタム拡張子の場合、\ref cfg_file_patterns &quot;FILE_PATTERNS&quot;も設定する必要があります。そうしないと、ファイルはDoxygenによって読み取られません。`no_extension`を指定する場合は、\ref cfg_file_patterns &quot;FILE_PATTERNS&quot;に`*`を追加する必要があります。&lt;br&gt;\ref default_file_extension_mapping &quot;デフォルトのファイル拡張子マッピング&quot;のリストも参照してください。</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>\c MARKDOWN_SUPPORTタグが有効な場合、DoxygenはすべてのコメントをMarkdown形式に従って前処理し、より読みやすいドキュメントを可能にします。詳細については、https://daringfireball.net/projects/markdown/ を参照してください。Markdown処理の出力はDoxygenによってさらに処理されるため、MarkdownフォーマットとDoxygen、HTML、およびXMLコマンドを混在させることができます。下位互換性の問題がある場合にのみ無効にしてください。</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>\c MARKDOWN_STRICTタグが有効な場合、DoxygenはDoxygenのネイティブマークアップ形式がMarkdownと競合する場合でも、コメント内のテキストをMarkdownフォーマットとして扱います。これは、バッククォートが使用される場合にのみ関連します。Doxygenのネイティブマークアップスタイルでは、単一引用符がバッククォートで始まるテキストフラグメントを終了し、それを引用テキストとして扱うことができますが、Markdownではそのようなテキストフラグメントは逐語的として扱われ、2番目の一致するバッククォートが見つかった場合にのみ終了します。また、Doxygenのネイティブマークアップ形式では、バッククォートセクションに表示される二重引用符をエスケープする必要がありますが、Markdownではこれは必要ありません。</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>\c TOC_INCLUDE_HEADINGSタグが非ゼロ値に設定されている場合、id属性を持たない場合でも、そのレベルまでのすべての見出しが目次に自動的に含まれます。\note この機能は現在Markdown見出しにのみ適用されます。</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>\c MARKDOWN_ID_STYLEタグを使用して、Markdown見出しの識別子を生成するために使用されるアルゴリズムを指定できます。注意：各識別子は一意です。</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>有効な場合、Doxygenは文書化されたクラスまたは名前空間に対応する単語を対応するドキュメントにリンクしようとします。そのようなリンクは、単語の前に\c %記号を置くか、\c AUTOLINK_SUPPORTを\c NOに設定することで、個々のケースで防止できます。\c AUTOLINK_IGNORE_WORDSタグにリストされている単語は、自動リンクから除外されます。</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>このタグは、ドキュメント内の単語の先頭と一致する場合、AUTOLINK_SUPPORTを介して自動リンク生成が有効になっている場合に、自動リンク生成を抑制する単語のリストを指定します。このリストは、\#または\ref cmdlink &quot;\link&quot;または\ref cmdref &quot;\ref&quot;コマンドを使用して明示的に作成されたリンクには影響しません。</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>STLクラス（`std::string`、`std::vector`など）を使用しているが、STLソースの（タグファイル）を入力として含めたくない場合は、このタグを\c YESに設定して、Doxygenが引数にSTLクラスを含む関数宣言と定義を一致させることができるようにする必要があります（例：`func(std::string`);と`func(std::string) {}`）。これにより、STLクラスを含む継承図とコラボレーション図もより完全で正確になります。</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>MicrosoftのC++/CLI言語を使用している場合は、解析サポートを有効にするためにこのオプションを\c YESに設定する必要があります。</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>\c SIP_SUPPORTタグを\c YESに設定すると、プロジェクトが&lt;a href=&quot;https://www.riverbankcomputing.com/software&quot;&gt;sip&lt;/a&gt;ソースのみで構成されている場合、Doxygenはそれらを通常のC++のように解析しますが、明示的な保護キーワードが存在しない場合、すべてのクラスがプライベート継承ではなくパブリック継承を使用すると仮定します。</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>MicrosoftのIDLには、プロパティのゲッターおよびセッターメソッドを示す\c propgetおよび\c propput属性があります。このオプションを\c YESに設定すると、Doxygenはドキュメント内のgetおよびsetメソッドをプロパティに置き換えます。これは、メソッドが実際に単純型を取得または設定している場合にのみ機能します。そうでない場合、またはメソッドを表示したい場合は、このオプションを\c NOに設定する必要があります。</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>ドキュメントでメンバーグループ化が使用され、\c DISTRIBUTE_GROUP_DOCタグが\c YESに設定されている場合、Doxygenはグループの最初のメンバーのドキュメント（存在する場合）をグループの他のメンバーに再利用します。デフォルトでは、グループのすべてのメンバーを明示的に文書化する必要があります。</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>構造体またはクラスをグループに追加し、このオプションが有効な場合、ネストされたクラスまたは構造体も同じグループに追加されます。デフォルトでは、このオプションは無効であり、\ref cmdingroup &quot;\ingroup&quot;を介してネストされたコンパウンドを明示的に追加する必要があります。</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>\c SUBGROUPINGタグを\c YESに設定すると、同じタイプのクラスメンバーグループ（例えば、パブリック関数のグループ）をそのタイプのサブグループとして配置できます（例えば、パブリック関数セクションの下）。サブグループ化を防ぐには、\c NOに設定します。または、\ref cmdnosubgrouping &quot;\nosubgrouping&quot;コマンドを使用してクラスごとに行うこともできます。</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>\c INLINE_GROUPED_CLASSESタグが\c YESに設定されている場合、クラス、構造体、および共用体は、それらが含まれているグループ内に表示されます（例：\ref cmdingroup &quot;\ingroup&quot;を使用）。別のページ（HTMLおよびManページの場合）またはセクション（\f$\mbox{\LaTeX}\f$およびRTFの場合）ではなく。&lt;br&gt;この機能は\ref cfg_separate_member_pages &quot;SEPARATE_MEMBER_PAGES&quot;と組み合わせて動作しないことに注意してください。</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>\c INLINE_SIMPLE_STRUCTSタグが\c YESに設定されている場合、パブリックデータフィールドまたは単純なtypedefフィールドのみを持つ構造体、クラス、および共用体は、そのスコープが文書化されている場合、定義されているスコープのドキュメント内にインラインで表示されます（つまり、ファイル、名前空間、またはグループのドキュメント）。\c NOに設定されている場合、構造体、クラス、および共用体は別のページ（HTMLおよびManページの場合）またはセクション（\f$\mbox{\LaTeX}\f$およびRTFの場合）に表示されます。</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>\c TYPEDEF_HIDES_STRUCTタグが有効な場合、構造体、共用体、または列挙型のtypedefは、typedefの名前を持つ構造体、共用体、または列挙型として文書化されます。したがって、&lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt;は、ドキュメント内で\c TypeTという名前の構造体として表示されます。無効な場合、typedefはファイル、名前空間、またはクラスのメンバーとして表示されます。構造体は\c TypeSという名前になります。これは、コーディング規約ですべての複合型がtypedefされ、typedefのみが参照され、タグ名は決して参照されない場合に、Cコードで一般的に便利です。</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>シンボルルックアップキャッシュのサイズは、\c LOOKUP_CACHE_SIZEを使用して設定できます。このキャッシュは、名前とスコープに基づいてシンボルを解決するために使用されます。これはコストのかかるプロセスであり、同じシンボルがコード内で複数回出現することが多いため、Doxygenは事前に解決されたシンボルのキャッシュを保持します。キャッシュが小さすぎると、Doxygenは遅くなります。キャッシュが大きすぎると、メモリが無駄になります。キャッシュサイズは次の式で与えられます：\f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$。有効な範囲は0..9で、デフォルトは0で、キャッシュサイズは\f$2^{16} = 65536\f$シンボルに対応します。実行の終わりに、Doxygenはキャッシュの使用状況を報告し、速度の観点から最適なキャッシュサイズを提案します。</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>\c NUM_PROC_THREADSは、処理中にDoxygenが使用を許可されるスレッドの数を指定します。\c 0に設定すると、Doxygenはシステムで使用可能なコアの数に基づいて設定します。CPU負荷と処理速度のバランスをより制御するために、0より大きい値に明示的に設定できます。現時点では、入力処理のみが複数のスレッドを使用して行うことができます。これはまだ実験的な機能であるため、デフォルトは1に設定されており、並列処理は効果的に無効になっています。問題が発生した場合は報告してください。ドットグラフの並列生成は、\c DOT_NUM_THREADS設定によって制御されます。</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>\c TIMESTAMPタグが\c NO以外に設定されている場合、生成された各ページには、ページが生成された日付または日時が含まれます。これを\c NOに設定すると、複数の実行の出力を比較する場合に役立ちます。</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>\c EXTRACT_ALLタグが\c YESに設定されている場合、Doxygenはドキュメント内のすべてのエンティティが文書化されていると仮定します。ドキュメントが利用可能でなかったとしてもです。プライベートクラスメンバーと静的ファイルメンバーは、\ref cfg_extract_private &quot;EXTRACT_PRIVATE&quot;および\ref cfg_extract_static &quot;EXTRACT_STATIC&quot;タグが\c YESに設定されていない限り非表示になります。\note これにより、\ref cfg_warnings &quot;WARNINGS&quot;が\c YESに設定されている場合に通常生成される文書化されていないメンバーに関する警告も無効になります。</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>\c EXTRACT_PRIVATEタグが\c YESに設定されている場合、クラスのすべてのプライベートメンバーがドキュメントに含まれます。</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>\c EXTRACT_PRIV_VIRTUALタグが\c YESに設定されている場合、クラスの文書化されたプライベート仮想メソッドがドキュメントに含まれます。</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>\c EXTRACT_PACKAGEタグが\c YESに設定されている場合、パッケージまたは内部スコープを持つすべてのメンバーがドキュメントに含まれます。</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>\c EXTRACT_STATICタグが\c YESに設定されている場合、ファイルのすべての静的メンバーがドキュメントに含まれます。</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>\c EXTRACT_LOCAL_CLASSESタグが\c YESに設定されている場合、ソースファイルでローカルに定義されたクラス（および構造体）がドキュメントに含まれます。\c NOに設定されている場合、ヘッダーファイルで定義されたクラスのみが含まれます。Javaソースには効果がありません。</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>このフラグはObjective-Cコードにのみ役立ちます。\c YESに設定されている場合、インターフェースではなく実装セクションで定義されたローカルメソッドがドキュメントに含まれます。\c NOに設定されている場合、インターフェースのメソッドのみが含まれます。</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>このフラグが\c YESに設定されている場合、匿名名前空間のメンバーが抽出され、ドキュメント内で&#x27;anonymous_namespace{file}&#x27;という名前空間として表示されます。fileは匿名名前空間を含むファイルのベース名に置き換えられます。デフォルトでは、匿名名前空間は非表示です。</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>このフラグが\c YESに設定されている場合、宣言内の名前のないパラメータの名前は、対応する定義によって決定されます。デフォルトでは、名前のないパラメータは出力内で名前のないままです。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>\c HIDE_UNDOC_MEMBERSタグが\c YESに設定されている場合、Doxygenは文書化されたクラスまたはファイル内の文書化されていないすべてのメンバーを非表示にします。\c NOに設定されている場合、これらのメンバーは様々な概要に含まれますが、ドキュメントセクションは生成されません。このオプションは、\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が有効な場合は効果がありません。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>\c HIDE_UNDOC_CLASSESタグが\c YESに設定されている場合、Doxygenはクラス階層で通常表示される文書化されていないすべてのクラスを非表示にします。\c NOに設定されている場合、これらのクラスは様々な概要に含まれます。このオプションは、有効な場合、文書化されていないC++コンセプトも非表示にします。このオプションは、\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が有効な場合は効果がありません。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>\c HIDE_UNDOC_NAMESPACESタグが\c YESに設定されている場合、Doxygenは名前空間階層で通常表示される文書化されていないすべての名前空間を非表示にします。\c NOに設定されている場合、これらの名前空間は様々な概要に含まれます。このオプションは、\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が有効な場合は効果がありません。</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>\c HIDE_FRIEND_COMPOUNDSタグが\c YESに設定されている場合、Doxygenはすべてのfriend宣言を非表示にします。\c NOに設定されている場合、これらの宣言はドキュメントに含まれます。</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>\c HIDE_IN_BODY_DOCSタグが\c YESに設定されている場合、Doxygenは関数の本体内で見つかったすべてのドキュメントブロックを非表示にします。\c NOに設定されている場合、これらのブロックは関数の詳細なドキュメントブロックに追加されます。</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>\c INTERNAL_DOCSタグは、\ref cmdinternal &quot;\internal&quot;コマンドの後に入力されたドキュメントが含まれるかどうかを決定します。タグが\c NOに設定されている場合、ドキュメントは除外されます。内部ドキュメントを含めるには、\c YESに設定してください。</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>\c CASE_SENSE_NAMESオプションの正しい設定により、Doxygenは基礎となるファイルシステムの機能により良く一致できるようになります。ファイルシステムが大文字と小文字を区別する場合（つまり、大文字と小文字のみが異なる名前を持つファイルを同じディレクトリでサポートする場合）、入力にそのようなファイルが表示される場合、オプションを\c YESに設定してそれらのファイルを適切に処理する必要があります。大文字と小文字を区別しないファイルシステムの場合、オプションを\c NOに設定して、大文字と小文字のみが異なるシンボルのために書き込まれた出力ファイルを適切に処理する必要があります。例えば、\c CLASSと\c Classという名前の2つのクラス、および正確な一致する大文字小文字を指定せずにファイルへの参照もサポートするためです。Windows（Cygwinを含む）およびmacOSでは、ユーザーは通常このオプションを\c NOに設定する必要がありますが、Linuxまたは他のUnix系では、通常\c YESに設定する必要があります。</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>\c HIDE_SCOPE_NAMESタグが\c NOに設定されている場合、Doxygenはドキュメント内の完全なクラスおよび名前空間スコープを持つメンバーを表示します。\c YESに設定されている場合、スコープは非表示になります。</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>\c HIDE_COMPOUND_REFERENCEタグが\c NO（デフォルト）に設定されている場合、Doxygenはページのタイトルにクラスリファレンスなどの追加テキストを追加します。\c YESに設定されている場合、コンパウンドリファレンスは非表示になります。</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>\c SHOW_HEADERFILEタグが\c YESに設定されている場合、クラスのドキュメントには、クラスを使用するためにインクルードする必要があるファイルが表示されます。</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>\c SHOW_INCLUDE_FILESタグが\c YESに設定されている場合、Doxygenはそのファイルのドキュメントにファイルによってインクルードされるファイルのリストを配置します。</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>\c SHOW_GROUPED_MEMB_INCタグが\c YESに設定されている場合、Doxygenはグループ化された各メンバーのドキュメントにインクルードステートメントを追加し、メンバーを使用するためにどのファイルをインクルードするかを読者に伝えます。</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>\c FORCE_LOCAL_INCLUDESタグが\c YESに設定されている場合、Doxygenはドキュメント内のインクルードファイルを山括弧ではなく二重引用符でリストします。</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>\c INLINE_INFOタグが\c YESに設定されている場合、インラインメンバーのドキュメントに[inline]タグが挿入されます。</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>\c SORT_MEMBER_DOCSタグが\c YESに設定されている場合、Doxygenはファイルおよびクラスメンバーの（詳細な）ドキュメントをメンバー名でアルファベット順にソートします。\c NOに設定されている場合、メンバーは宣言順に表示されます。</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>\c SORT_BRIEF_DOCSタグが\c YESに設定されている場合、Doxygenはファイル、名前空間、およびクラスメンバーの簡潔な説明をメンバー名でアルファベット順にソートします。\c NOに設定されている場合、メンバーは宣言順に表示されます。これはクラスリスト内のクラスの順序にも影響することに注意してください。</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>\c SORT_MEMBERS_CTORS_1STタグが\c YESに設定されている場合、Doxygenはクラスメンバーの（簡潔および詳細な）ドキュメントをソートし、コンストラクタとデストラクタが最初にリストされるようにします。\c NOに設定されている場合、コンストラクタは\ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot;および\ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot;で定義されたそれぞれの順序で表示されます。\note \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot;が\c NOに設定されている場合、このオプションは簡潔なメンバードキュメントのソートには無視されます。\note \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot;が\c NOに設定されている場合、このオプションは詳細なメンバードキュメントのソートには無視されます。</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>\c SORT_GROUP_NAMESタグが\c YESに設定されている場合、Doxygenはグループ名の階層をアルファベット順にソートします。\c NOに設定されている場合、グループ名は定義された順序で表示されます。</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>\c SORT_BY_SCOPE_NAMEタグが\c YESに設定されている場合、クラスリストは名前空間を含む完全修飾名でソートされます。\c NOに設定されている場合、クラスリストは名前空間部分を含まずにクラス名のみでソートされます。\note このオプションは、\ref cfg_hide_scope_names &quot;HIDE_SCOPE_NAMES&quot;が\c YESに設定されている場合はあまり役に立ちません。\note このオプションはクラスリストにのみ適用され、アルファベット順リストには適用されません。</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>\c STRICT_PROTO_MATCHINGオプションが有効で、Doxygenが関数のすべてのパラメータの正しい型解決に失敗した場合、候補が1つしかない場合、または単純な文字列一致で選択する候補が明らかな場合でも、メンバー関数のプロトタイプと実装の間の一致を拒否します。\c STRICT_PROTO_MATCHINGを無効にすると、Doxygenはそのような場合でもプロトタイプと実装の間の一致を受け入れます。</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>\c GENERATE_TODOLISTタグを使用して、todoリストを有効（\c YES）または無効（\c NO）にできます。このリストは、ドキュメントに\ref cmdtodo &quot;\todo&quot;コマンドを配置することで作成されます。</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>\c GENERATE_TESTLISTタグを使用して、テストリストを有効（\c YES）または無効（\c NO）にできます。このリストは、ドキュメントに\ref cmdtest &quot;\test&quot;コマンドを配置することで作成されます。</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>\c GENERATE_BUGLISTタグを使用して、バグリストを有効（\c YES）または無効（\c NO）にできます。このリストは、ドキュメントに\ref cmdbug &quot;\bug&quot;コマンドを配置することで作成されます。</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>\c GENERATE_DEPRECATEDLISTタグを使用して、非推奨リストを有効（\c YES）または無効（\c NO）にできます。このリストは、ドキュメントに\ref cmddeprecated &quot;\deprecated&quot;コマンドを配置することで作成されます。</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>\c GENERATE_REQUIREMENTSタグを使用して、要件ページを有効（\c YES）または無効（\c NO）にできます。有効な場合、このページは、\ref cmdrequirement &quot;\requirement&quot;コマンドを含む少なくとも1つのコメントブロックが入力に現れると自動的に作成されます。</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>\c REQ_TRACEABILITY_INFOタグは、要件ページにトレーサビリティ情報が表示されるかどうかを制御します（\ref cmdrequirement &quot;\requirement&quot;コメントブロックを使用する場合にのみ関連）。\c NOに設定すると、トレーサビリティ情報は完全に無効になります。\c UNSATISFIED_ONLYに設定すると、満足関係が欠けている要件のリストが表示されます（\ref cmdsatisfies &quot;\satisfies&quot;コマンドを介して）。同様に、\c UNVERIFIED_ONLYに設定すると、検証関係が欠けている要件のリストが表示されます（\ref cmdverifies &quot;\verifies&quot;コマンドを介して）。タグを\c YES（デフォルト）に設定すると、該当する場合両方のリストが表示されます。</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>\c ENABLED_SECTIONSタグを使用して、\ref cmdif &quot;\if&quot; \&lt;section_label\&gt; ... \ref cmdendif &quot;\endif&quot;および\ref cmdcond &quot;\cond&quot; \&lt;section_label\&gt; ... \ref cmdendcond &quot;\endcond&quot;ブロックでマークされた条件付きドキュメントセクションを有効にできます。</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>\c MAX_INITIALIZER_LINESタグは、変数またはマクロ/定義の初期値がドキュメントに表示されるために持つことができる最大行数を決定します。初期化子がここで指定された行数を超える場合、非表示になります。初期化子を完全に非表示にするには、0の値を使用します。個々の変数とマクロ/定義の値の外観は、この設定に関係なく、ドキュメントで\ref cmdshowinitializer &quot;\showinitializer&quot;または\ref cmdhideinitializer &quot;\hideinitializer&quot;コマンドを使用して制御できます。</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>\c SHOW_USED_FILESタグを\c NOに設定すると、クラスと構造体のドキュメントの下部に生成されるファイルのリストが無効になります。\c YESに設定されている場合、リストはドキュメントの生成に使用されたファイルを示します。</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>\c SHOW_FILESタグを\c NOに設定すると、ファイルページの生成が無効になります。これにより、クイックインデックスとフォルダツリービュー（指定されている場合）からファイルエントリが削除されます。</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>\c SHOW_NAMESPACESタグを\c NOに設定すると、名前空間ページの生成が無効になります。これにより、クイックインデックスとフォルダツリービュー（指定されている場合）から名前空間エントリが削除されます。</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>\c FILE_VERSION_FILTERタグを使用して、Doxygenが各ファイルの現在のバージョンを取得するために呼び出すプログラムまたはスクリプトを指定できます（通常はバージョン管理システムから）。Doxygenは、&lt;code&gt;popen()&lt;/code&gt;を介してコマンド&lt;code&gt;command input-file&lt;/code&gt;を実行することでプログラムを呼び出します。ここで、\c commandは\c FILE_VERSION_FILTERタグの値、\c input-fileはDoxygenが提供する入力ファイルの名前です。プログラムが標準出力に書き込むものはすべてファイルバージョンとして使用されます。</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>\c LAYOUT_FILEタグを使用して、Doxygenが解析するレイアウトファイルを指定できます。レイアウトファイルは、出力形式に依存しない方法で生成された出力ファイルのグローバル構造を制御します。Doxygenのデフォルトを表すレイアウトファイルを作成するには、`-l`オプションを指定してDoxygenを実行します。オプションの後にファイル名をオプションで指定できます。省略した場合、\c DoxygenLayout.xmlがレイアウトファイルの名前として使用されます。詳細については、セクション\ref layoutも参照してください。&lt;br&gt;\c DoxygenLayout.xmlというファイルを含むディレクトリからDoxygenを実行すると、\c LAYOUT_FILEタグが空白のままでも、Doxygenはそれを自動的に解析することに注意してください。</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>\c CITE_BIB_FILESタグを使用して、参照定義を含む1つ以上の\c bibファイルを指定できます。これは&lt;code&gt;.bib&lt;/code&gt;ファイルのリストである必要があります。&lt;code&gt;.bib&lt;/code&gt;拡張子は省略された場合自動的に追加されます。これには\c bibtexツールがインストールされている必要があります。詳細については、https://en.wikipedia.org/wiki/BibTeXも参照してください。\f$\mbox{\LaTeX}\f$の場合、参考文献のスタイルは\ref cfg_latex_bib_style &quot;LATEX_BIB_STYLE&quot;を使用して制御できます。この機能を使用するには、検索パスで\c bibtexと\c perlが利用可能である必要があります。参照の作成方法については、\ref cmdcite &quot;\cite&quot;も参照してください。</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>\c EXTERNAL_TOOL_PATHタグを使用して、\c latexや\c gsなどの外部ツールが見つかるように検索パス（PATH環境変数）を拡張できます。\note EXTERNAL_TOOL_PATHで指定されたディレクトリは、PATH変数ですでに指定されているパスの前に追加され、指定された順序で追加されます。\note このオプションは、DoxywizardからDoxygenを実行する場合、macOSバージョン14（Sonoma）以降で特に便利です。この場合、PATHへのユーザー定義の変更は無視されるためです。macOSの典型的な例は、\verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim 標準パスと共に設定すると、外部ツールを起動するときにdoxygenが使用する完全な検索パスは、\verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim になります。</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>\c QUIETタグを使用して、Doxygenによって標準出力に生成されるメッセージをオン/オフできます。\c QUIETが\c YESに設定されている場合、メッセージがオフであることを意味します。</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>\c WARNINGSタグを使用して、Doxygenによって標準エラー（\c stderr）に生成される警告メッセージをオン/オフできます。\c WARNINGSが\c YESに設定されている場合、警告がオンであることを意味します。&lt;br&gt;\b ヒント：ドキュメントの作成中に警告をオンにしてください。</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>\c WARN_IF_UNDOCUMENTEDタグが\c YESに設定されている場合、Doxygenは文書化されていないメンバーの警告を生成します。\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が\c YESに設定されている場合、このフラグは自動的に無効になります。</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>\c WARN_IF_DOC_ERRORタグが\c YESに設定されている場合、Doxygenはドキュメント内の潜在的なエラー（文書化された関数で一部のパラメータを2回文書化する、存在しないパラメータを文書化する、マークアップコマンドを誤って使用するなど）に対して警告を生成します。</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>\c WARN_IF_INCOMPLETE_DOCが\c YESに設定されている場合、Doxygenは不完全な関数パラメータドキュメントについて警告します。\c NOに設定されている場合、Doxygenは一部のパラメータにドキュメントがないことを警告なしで受け入れます。</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>この\c WARN_NO_PARAMDOCオプションを有効にすると、文書化されているがパラメータまたは戻り値のドキュメントがない関数の警告を取得できます。\c NOに設定されている場合、Doxygenはパラメータドキュメントのエラーについてのみ警告し、ドキュメントの欠落については警告しません。\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が\c YESに設定されている場合、このフラグは自動的に無効になります。\ref cfg_warn_if_incomplete_doc &quot;WARN_IF_INCOMPLETE_DOC&quot;も参照してください。</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>\c WARN_IF_UNDOC_ENUM_VALオプションが\c YESに設定されている場合、Doxygenは文書化されていない列挙値について警告します。\c NOに設定されている場合、Doxygenは文書化されていない列挙値を受け入れます。\ref cfg_extract_all &quot;EXTRACT_ALL&quot;が\c YESに設定されている場合、このフラグは自動的に無効になります。</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>\c WARN_LAYOUT_FILEオプションが\c YESに設定されている場合、Doxygenはユーザー定義のレイアウトファイルの解析中に見つかった問題（欠落または誤った要素など）について警告します。詳細については、\ref cfg_layout_file &quot;LAYOUT_FILE&quot;も参照してください。\c NOに設定されている場合、レイアウトファイルの問題は抑制されます。</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>\c WARN_AS_ERRORタグが\c YESに設定されている場合、Doxygenは警告が発生するとすぐに停止します。\c WARN_AS_ERRORタグが\c FAIL_ON_WARNINGSに設定されている場合、Doxygenは\c WARN_AS_ERRORタグが\c NOに設定されているかのように実行を継続しますが、Doxygenプロセスの終了時に、Doxygenは非ゼロのステータスで戻ります。\c WARN_AS_ERRORタグが\c FAIL_ON_WARNINGS_PRINTに設定されている場合、Doxygenは\c FAIL_ON_WARNINGSのように動作しますが、\ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;が定義されていない場合、Doxygenは警告メッセージを他のメッセージの間に書き込まず、実行の終了時に書き込みます。\ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;が定義されている場合、警告メッセージは定義されたファイルにあることに加えて、実行の終了時にも表示されます。ただし、\ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;が`-`（つまり標準出力`stdout`）として定義されている場合を除き、その場合の動作は\c FAIL_ON_WARNINGSの設定と同じままです。</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>\c WARN_FORMATタグは、Doxygenが生成できる警告メッセージの形式を決定します。文字列には、警告が発生したファイルと行番号、および警告テキストに置き換えられる&lt;code&gt;\$file&lt;/code&gt;、&lt;code&gt;\$line&lt;/code&gt;、および&lt;code&gt;\$text&lt;/code&gt;タグが含まれている必要があります。オプションで、形式には&lt;code&gt;$version&lt;/code&gt;を含めることができます。これは、（\ref cfg_file_version_filter &quot;FILE_VERSION_FILTER&quot;を介して取得できた場合）ファイルのバージョンに置き換えられます。\sa \ref cfg_warn_line_format &quot;WARN_LINE_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>\ref cfg_warn_format &quot;WARN_FORMAT&quot;コマンドの`$text`部分で、より具体的な場所への参照が与えられる可能性があります。この場所にジャンプしやすくするために（Doxygenの外部で）、ユーザーはカスタムの「カット」/「ペースト」文字列を定義できます。例：\verbatim WARN_LINE_FORMAT = &quot;&#x27;vi $file +$line&#x27;&quot; \endverbatim \sa \ref cfg_warn_format &quot;WARN_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>\c WARN_LOGFILEタグを使用して、警告およびエラーメッセージが書き込まれるファイルを指定できます。空白のままにすると、出力は標準エラー（`stderr`）に書き込まれます。指定されたファイルが書き込み用に開けない場合、警告およびエラーメッセージは標準エラーに書き込まれます。`-`ファイルが指定されている場合、警告およびエラーメッセージは標準出力（`stdout`）に書き込まれます。</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>\c INPUTタグは、文書化されたソースファイルを含むファイルおよび/またはディレクトリを指定するために使用されます。\c myfile.cppのようなファイル名や\c /usr/src/myprojectのようなディレクトリを入力できます。ファイルまたはディレクトリをスペースで区切ってください。\ref cfg_file_patterns &quot;FILE_PATTERNS&quot;および\ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;も参照してください。\note このタグが空の場合、現在のディレクトリが検索されます。</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>このタグを使用して、Doxygenが解析するソースファイルの文字エンコーディングを指定できます。内部的には、DoxygenはUTF-8エンコーディングを使用します。Doxygenはトランスコーディングに`libiconv`（または`libc`に組み込まれた`iconv`）を使用します。可能なエンコーディングのリストについては、&lt;a href=&quot;https://www.gnu.org/software/libiconv/&quot;&gt;libiconvのドキュメント&lt;/a&gt;を参照してください。\sa \ref cfg_input_file_encoding &quot;INPUT_FILE_ENCODING&quot;</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>このタグを使用して、Doxygenが解析するソースファイルの文字エンコーディングを指定できます。\c INPUT_FILE_ENCODINGタグを使用して、ファイルパターンごとに文字エンコーディングを指定できます。Doxygenはファイル名を各パターンと比較し、一致がある場合、デフォルトの\ref cfg_input_encoding &quot;INPUT_ENCODING&quot;の代わりにエンコーディングを適用します。文字エンコーディングは、pattern=encodingの形式のリストです（`*.php=ISO-8859-1`のように）。\sa サポートされているエンコーディングの詳細については、\ref cfg_input_encoding &quot;INPUT_ENCODING&quot;を参照してください。</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>\ref cfg_input &quot;INPUT&quot;タグの値にディレクトリが含まれている場合、\c FILE_PATTERNSタグを使用して、ディレクトリ内のソースファイルをフィルタリングする1つ以上のワイルドカードパターン（`*.cpp`や`*.h`のように）を指定できます。&lt;br&gt;カスタム拡張子または直接サポートされていない拡張子の場合、拡張子の\ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;も設定する必要があります。そうしないと、ファイルはDoxygenによって読み取られません。&lt;br&gt;デフォルトでチェックされるファイルパターンのリストは、\ref default_file_extension_mapping &quot;デフォルトのファイル拡張子マッピング&quot;のリストと異なる場合があることに注意してください。&lt;br&gt;空白のままにすると、次のパターンがテストされます：</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>\c RECURSIVEタグを使用して、入力ファイルのサブディレクトリも検索するかどうかを指定できます。</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>\c EXCLUDEタグを使用して、\ref cfg_input &quot;INPUT&quot;ソースファイルから除外するファイルおよび/またはディレクトリを指定できます。これにより、\ref cfg_input &quot;INPUT&quot;タグでルートが指定されたディレクトリツリーからサブディレクトリを簡単に除外できます。&lt;br&gt;相対パスは、Doxygenが実行されるディレクトリからの相対パスであることに注意してください。</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>\c EXCLUDE_SYMLINKSタグを使用して、シンボリックリンク（Unixファイルシステムの機能）であるファイルまたはディレクトリを入力から除外するかどうかを選択できます。</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>\ref cfg_input &quot;INPUT&quot;タグの値にディレクトリが含まれている場合、\c EXCLUDE_PATTERNSタグを使用して、それらのディレクトリから特定のファイルを除外する1つ以上のワイルドカードパターンを指定できます。&lt;br&gt;ワイルドカードは絶対パスを持つファイルと一致するため、すべてのテストディレクトリを除外するには、例えばパターン`*``/test/``*`を使用します。</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>\c EXCLUDE_SYMBOLSタグを使用して、出力から除外する1つ以上のシンボル名（名前空間、クラス、関数など）を指定できます。シンボル名は、完全修飾名、単語、またはワイルドカード`*`が使用されている場合は部分文字列です。例：`ANamespace`、`AClass`、`ANamespace::AClass`、`ANamespace::*Test`</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>\c EXAMPLE_PATHタグを使用して、インクルードされるコード例のフラグメントを含む1つ以上のファイルまたはディレクトリを指定できます（\ref cmdinclude &quot;\include&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>\ref cfg_example_path &quot;EXAMPLE_PATH&quot;タグの値にディレクトリが含まれている場合、\c EXAMPLE_PATTERNSタグを使用して、ディレクトリ内のソースファイルをフィルタリングする1つ以上のワイルドカードパターン（`*.cpp`や`*.h`のように）を指定できます。空白のままにすると、すべてのファイルがインクルードされます。</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>\c EXAMPLE_RECURSIVEタグが\c YESに設定されている場合、\ref cfg_recursive &quot;RECURSIVE&quot;タグの値に関係なく、\ref cmdinclude &quot;\include&quot;または\ref cmddontinclude &quot;\dontinclude&quot;コマンドで使用する入力ファイルのサブディレクトリが検索されます。</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>\c IMAGE_PATHタグを使用して、ドキュメントにインクルードされる画像を含む1つ以上のファイルまたはディレクトリを指定できます（\ref cmdimage &quot;\image&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>\c INPUT_FILTERタグを使用して、Doxygenが各入力ファイルをフィルタリングするために呼び出すプログラムを指定できます。Doxygenは、&lt;code&gt;popen()&lt;/code&gt;を介してコマンド：&lt;br&gt;&lt;code&gt;\&lt;filter\&gt; \&lt;input-file\&gt;&lt;/code&gt;&lt;br&gt;を実行することでフィルタプログラムを呼び出します。ここで、&lt;code&gt;\&lt;filter\&gt;&lt;/code&gt;は\c INPUT_FILTERタグの値、&lt;code&gt;\&lt;input-file\&gt;&lt;/code&gt;は入力ファイルの名前です。Doxygenは、フィルタプログラムが標準出力に書き込む出力を使用します。\ref cfg_filter_patterns &quot;FILTER_PATTERNS&quot;が指定されている場合、このタグは無視されます。&lt;br&gt;フィルタは行を追加または削除してはならないことに注意してください。コードがスキャンされる前に適用されますが、出力コードが生成されるときには適用されません。行が追加または削除されると、アンカーが正しく配置されません。&lt;br&gt;Doxygenは、処理されたデータを標準出力に書き込んでさらに処理するために使用するため、デバッグステートメントや使用されたコマンドのような他のもの（Windowsバッチファイルの場合は常に`@echo OFF`を使用）は標準出力に書き込まれるべきではないことに注意してください。&lt;br&gt;カスタム拡張子または直接サポートされていない拡張子の場合、拡張子の\ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;も設定する必要があります。そうしないと、ファイルはDoxygenによって正しく処理されません。&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>\c FILTER_PATTERNSタグを使用して、ファイルパターンごとにフィルタを指定できます。Doxygenはファイル名を各パターンと比較し、一致がある場合にフィルタを適用します。フィルタは、pattern=filterの形式のリストです（`*.cpp=my_cpp_filter`のように）。フィルタの使用方法については、\ref cfg_input_filter &quot;INPUT_FILTER&quot;を参照してください。\c FILTER_PATTERNSタグが空の場合、またはパターンのいずれもファイル名と一致しない場合、\ref cfg_input_filter &quot;INPUT_FILTER&quot;が適用されます。&lt;br&gt;カスタム拡張子または直接サポートされていない拡張子の場合、拡張子の\ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;も設定する必要があります。そうしないと、ファイルはDoxygenによって正しく処理されません。&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>\c FILTER_SOURCE_FILESタグが\c YESに設定されている場合、入力フィルタ（\ref cfg_input_filter &quot;INPUT_FILTER&quot;を使用して設定されている場合）は、ブラウズするソースファイルを生成するために使用される入力ファイルもフィルタリングするために使用されます（つまり、\ref cfg_source_browser &quot;SOURCE_BROWSER&quot;が\c YESに設定されている場合）。</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>\c FILTER_SOURCE_PATTERNSタグを使用して、ファイルパターンごとにソースフィルタを指定できます。パターンは\ref cfg_filter_patterns &quot;FILTER_PATTERN&quot;の設定を上書きし（ある場合）、`*.ext=`を使用して特定のパターンのソースフィルタリングを無効にすることもできます（フィルタに名前を付けずに）。</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>\c USE_MDFILE_AS_MAINPAGEタグが入力の一部であるマークダウンファイルの名前を参照している場合、その内容はメインページ（`index.html`）に配置されます。これは、例えばGitHubにプロジェクトがあり、紹介ページもDoxygen出力に再利用したい場合に便利です。</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>\c IMPLICIT_DIR_DOCSタグが\c YESに設定されている場合、プロジェクトのルートのサブディレクトリで見つかった`README.md`ファイルは、`README.md`が\ref cmddir &quot;\dir&quot;、\ref cmdpage &quot;\page&quot;、または\ref cmdmainpage &quot;\mainpage&quot;コマンドで始まらない限り、そのサブディレクトリのドキュメントとして使用されます。\c NOに設定されている場合、ディレクトリドキュメントとして使用されるためには、`README.md`ファイルは明示的な\ref cmddir &quot;\dir&quot;コマンドで始まる必要があります。</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Fortran標準では、固定形式のFortranコードでは、位置72以降のすべての文字がコメントと見なされることが規定されています。一般的な拡張機能は、自動コメントが始まる前により長い行を許可することです。\c FORTRAN_COMMENT_AFTERの設定により、自動コメントが始まる前により長い行を処理できるようになります。</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>\c SOURCE_BROWSERタグが\c YESに設定されている場合、ソースファイルのリストが生成されます。文書化されたエンティティはこれらのソースと相互参照されます。&lt;br&gt;注意：生成された出力からすべてのソースコードを取り除くには、\ref cfg_verbatim_headers &quot;VERBATIM_HEADERS&quot;も\c NOに設定されていることを確認してください。</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>\c INLINE_SOURCESタグを\c YESに設定すると、関数、複数行マクロ、列挙型、またはリスト初期化変数の本体がドキュメントに直接インクルードされます。</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>\c STRIP_CODE_COMMENTSタグを\c YESに設定すると、Doxygenは生成されたソースコードフラグメントから特別なコメントブロックを非表示にするように指示します。通常のC、C++、Fortranコメントは常に表示されたままです。</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>\c REFERENCED_BY_RELATIONタグが\c YESに設定されている場合、文書化された各エンティティについて、それを参照する文書化されたすべての関数がリストされます。</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>\c REFERENCES_RELATIONタグが\c YESに設定されている場合、文書化された各関数について、その関数によって呼び出し/使用される文書化されたすべてのエンティティがリストされます。</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>\c REFERENCES_LINK_SOURCEタグが\c YESに設定され、\ref cfg_source_browser &quot;SOURCE_BROWSER&quot;タグが\c YESに設定されている場合、\ref cfg_references_relation &quot;REFERENCES_RELATION&quot;および\ref cfg_referenced_by_relation &quot;REFERENCED_BY_RELATION&quot;リストの関数からのハイパーリンクはソースコードにリンクします。それ以外の場合は、ドキュメントにリンクします。</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>\c SOURCE_TOOLTIPSが有効な場合（デフォルト）、ソースコード内のハイパーリンクにカーソルを合わせると、プロトタイプ、簡潔な説明、定義およびドキュメントへのリンクなどの追加情報を含むツールチップが表示されます。これによりHTMLファイルが大きくなり、大きなファイルの読み込みが少し遅くなるため、この機能を無効にすることを選択できます。</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>\c USE_HTAGSタグが\c YESに設定されている場合、ソースコードへの参照は、Doxygenの組み込みソースブラウザではなく、\c htags(1)ツールによって生成されたHTMLを指します。\c htagsツールは、GNUのグローバルソースタグ付けシステムの一部です（https://www.gnu.org/software/global/global.htmlを参照）。バージョン4.8.6以上が必要です。&lt;br&gt;使用するには、次の手順を実行します：-# \c globalの最新バージョンをインストールする -# 設定ファイルで\ref cfg_source_browser &quot;SOURCE_BROWSER&quot;と\c USE_HTAGSを有効にする -# \ref cfg_input &quot;INPUT&quot;がソースツリーのルートを指していることを確認する -# 通常通り\c doxygenを実行する&lt;br&gt;Doxygenは\c htagsを呼び出し（それがさらに\c gtagsを呼び出します）、これらのツールはコマンドライン（つまり検索パス内）から利用可能である必要があります。&lt;br&gt;結果：Doxygenによって生成されたソースブラウザの代わりに、ソースコードへのリンクは\c htagsの出力を指すようになります。</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>\c VERBATIM_HEADERSタグが\c YESに設定されている場合、Doxygenはインクルードが指定されている各クラスのヘッダーファイルの逐語的なコピーを生成します。これを無効にするには\c NOに設定します。\sa セクション\ref cmdclass &quot;\class&quot;。</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>\c CLANG_ASSISTED_PARSINGタグが\c YESに設定されている場合、Doxygenは&lt;a href=&quot;http://clang.llvm.org/&quot;&gt;clangパーサー&lt;/a&gt;を使用して、パフォーマンスの低下を犠牲にしてより正確な解析を行います。これは、Doxygenの組み込みパーサーが必要な型情報を欠いているテンプレート豊富なC++コードで特に役立ちます。@note このオプションの可用性は、DoxygenがCMakeの`-Duse_libclang=ON`オプションで生成されたかどうかに依存します。</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>\c CLANG_ASSISTED_PARSINGタグが\c YESに設定され、\c CLANG_ADD_INC_PATHSタグが\c YESに設定されている場合、Doxygenは各入力のディレクトリをインクルードパスに追加します。</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>clangアシスト解析が有効な場合、コンパイラを呼び出すときに通常使用するコマンドラインオプションをコンパイラに提供できます。インクルードパスは、\ref cfg_input &quot;INPUT&quot;および\ref cfg_include_path &quot;INCLUDE_PATH&quot;で指定されたファイルおよびディレクトリに対してDoxygenによってすでに設定されていることに注意してください。</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>clangアシスト解析が有効な場合、`compile_commands.json`というファイルを含むディレクトリへのパスをclangパーサーに提供できます。このファイルは、ソースファイルがビルドされたときに使用されたオプションを含む&lt;a href=&quot;http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html&quot;&gt;コンパイルデータベース&lt;/a&gt;です。これは、`clang-check`のようなclangツールに`-p`オプションを指定することと同等です。これらのオプションはパーサーに渡されます。\ref cfg_clang_options &quot;CLANG_OPTIONS&quot;で指定されたオプションも追加されます。@note このオプションの可用性は、DoxygenがCMakeの`-Duse_libclang=ON`オプションで生成されたかどうかに依存します。</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>\c ALPHABETICAL_INDEXタグが\c YESに設定されている場合、すべてのコンパウンドのアルファベット順インデックスが生成されます。プロジェクトに多数のクラス、構造体、共用体、またはインターフェースが含まれている場合はこれを有効にしてください。</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>\c IGNORE_PREFIXタグを使用して、インデックスヘッダーの生成中に無視されるプレフィックス（またはプレフィックスのリスト）を指定できます。\c IGNORE_PREFIXタグは、クラス、関数、およびメンバー名に対して機能します。エンティティは、プレフィックスを削除した後に残るエンティティ名の最初の文字の下にアルファベット順リストに配置されます。</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>\c GENERATE_HTMLタグが\c YESに設定されている場合、DoxygenはHTML出力を生成します。</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>\c HTML_OUTPUTタグは、HTMLドキュメントが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>\c HTML_FILE_EXTENSIONタグを使用して、生成される各HTMLページのファイル拡張子を指定できます（例：&lt;code&gt;.htm, .php, .asp&lt;/code&gt;）。</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>\c HTML_HEADERタグを使用して、生成される各HTMLページのユーザー定義HTMLヘッダーファイルを指定できます。タグが空白のままの場合、Doxygenは標準ヘッダーを生成します。&lt;br&gt;有効なHTMLを取得するには、ヘッダーファイルにDoxygenが必要とするすべてのスクリプトとスタイルシートを含める必要があります。これは使用される設定オプションに依存します（例：\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;の設定）。次を使用してデフォルトのヘッダーから始めることを強くお勧めします：\verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim 次にファイル\c new_header.htmlを変更します。Doxygenが通常使用するデフォルトヘッダーの生成方法については、セクション\ref doxygen_usageも参照してください。@note ヘッダーは変更される可能性があるため、通常はDoxygenの新しいバージョンにアップグレードする際にデフォルトヘッダーを再生成する必要があります。</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>\c HTML_FOOTERタグを使用して、生成される各HTMLページのユーザー定義HTMLフッターを指定できます。タグが空白のままの場合、Doxygenは標準フッターを生成します。デフォルトのフッターの生成方法とフッター内で使用できる特別なコマンドの詳細については、\ref cfg_html_header &quot;HTML_HEADER&quot;を参照してください。Doxygenが通常使用するデフォルトフッターの生成方法については、セクション\ref doxygen_usageも参照してください。</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>\c HTML_STYLESHEETタグを使用して、各HTMLページで使用されるユーザー定義のカスケーディングスタイルシートを指定できます。HTML出力の外観を微調整するために使用できます。空白のままにすると、Doxygenはデフォルトのスタイルシートを生成します。Doxygenが通常使用するスタイルシートの生成方法については、セクション\ref doxygen_usageも参照してください。\note より堅牢であるため、このタグの代わりに\ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot;を使用することをお勧めします。このタグ（&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;）は将来廃止される予定です。</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>\c HTML_EXTRA_STYLESHEETタグを使用して、Doxygenによって作成された標準スタイルシートの後にインクルードされる追加のユーザー定義カスケーディングスタイルシートを指定できます。このオプションを使用すると、特定のスタイルの側面をオーバーライドできます。これは、標準スタイルシートを置き換えないため、将来の更新に対してより堅牢な\ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot;を使用するよりも推奨されます。Doxygenはスタイルシートファイルを出力ディレクトリにコピーします。\note 追加のスタイルシートファイルの順序は重要です（例：リストの最後のスタイルシートは、リスト内の前のものの設定をオーバーライドします）。\note スクロールバーのスタイル設定は現在Webkit/Chromiumでオーバーライドできないため、1つ以上の追加スタイルシートが指定されている場合、スタイル設定はデフォルトのdoxygen.cssから除外されます。したがって、スクロールバーのカスタマイズが必要な場合は、明示的に追加する必要があります。</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>\c HTML_EXTRA_FILESタグを使用して、HTML出力ディレクトリにコピーされる1つ以上の追加画像または他のソースファイルを指定できます。これらのファイルはベースHTML出力ディレクトリにコピーされることに注意してください。\ref cfg_html_header &quot;HTML_HEADER&quot;および/または\ref cfg_html_footer &quot;HTML_FOOTER&quot;ファイルで&lt;code&gt;$relpath^&lt;/code&gt;マーカーを使用してこれらのファイルをロードします。\ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot;ファイルでは、ファイル名のみを使用します。また、ファイルはそのままコピーされることにも注意してください。利用可能なコマンドやマーカーはありません。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>\c HTML_COLORSTYLEタグを使用して、生成されたHTML出力をダークテーマまたはライトテーマでレンダリングするかどうかを指定できます。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>\c HTML_COLORSTYLE_HUEタグはHTML出力の色を制御します。Doxygenはこの色に従ってスタイルシートと背景画像の色を調整します。色相はカラーホイール上の角度として指定されます。詳細については、https://en.wikipedia.org/wiki/Hueを参照してください。例えば、値0は赤、60は黄、120は緑、180はシアン、240は青、300は紫、360は再び赤を表します。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>\c HTML_COLORSTYLE_SATタグはHTML出力の色の純度（または彩度）を制御します。値が0の場合、出力はグレースケールのみを使用します。値が255の場合、最も鮮やかな色が生成されます。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>\c HTML_COLORSTYLE_GAMMAタグはHTML出力の色の輝度成分に適用されるガンマ補正を制御します。100未満の値は出力を徐々に明るくし、100を超える値は出力を暗くします。値を100で割ったものが実際に適用されるガンマです。したがって、80はガンマ0.8を表し、値220はガンマ2.2を表し、100はガンマを変更しません。</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>\c HTML_DYNAMIC_MENUSタグが\c YESに設定されている場合、生成されたHTMLドキュメントには、JavaScriptを介して動的に作成される垂直ナビゲーションメニューを備えたメインインデックスが含まれます。無効にすると、ナビゲーションインデックスは、各HTMLページに静的に埋め込まれる複数レベルのタブで構成されます。QtヘルプブラウザのようにJavaScriptを持たないブラウザをサポートするには、このオプションを無効にします。</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>\c HTML_DYNAMIC_SECTIONSタグが\c YESに設定されている場合、生成されたHTMLドキュメントには、ページが読み込まれた後に非表示および表示できるセクションが含まれます。</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>\c HTML_CODE_FOLDINGタグが\c YESに設定されている場合、クラスと関数は生成されたHTMLソースコードで動的に折りたたみおよび展開できます。</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>\c HTML_COPY_CLIPBOARDタグが\c YESに設定されている場合、Doxygenはコードおよびテキストフラグメントの右上隅にアイコンを表示し、ユーザーがその内容をクリップボードにコピーできるようにします。これは、ブラウザでサポートされ、Webページが&lt;a href=&quot;https://www.w3.org/TR/secure-contexts/&quot;&gt;安全なコンテキスト&lt;/a&gt;を介して提供されている場合にのみ機能することに注意してください。つまり、https:またはfile:プロトコルを使用しています。</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygenは、ブラウザにいくつかの設定を永続的に保存します（例：Cookieを介して）。デフォルトでは、これらの設定はすべてのプロジェクトでDoxygenによって生成されたすべてのHTMLページに適用されます。\c HTML_PROJECT_COOKIEタグを使用して、設定をプロジェクト固有のキーの下に保存し、ユーザー設定が個別に保存されるようにすることができます。</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>\c HTML_INDEX_NUM_ENTRIESを使用すると、様々なツリー構造インデックスに最初に表示されるエントリの推奨数を制御できます。ユーザーは後で動的にエントリを展開および折りたたむことができます。Doxygenは、最大で指定された数のエントリが表示されるレベルまでツリーを展開します（完全に折りたたまれたツリーがすでにこの量を超えている場合を除く）。したがって、エントリ数を1に設定すると、デフォルトで完全に折りたたまれたツリーが生成されます。0は無限のエントリ数を表す特別な値で、デフォルトで完全に展開されたツリーになります。</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>\c GENERATE_DOCSETタグが\c YESに設定されている場合、OSX 10.5（Leopard）で導入された&lt;a href=&quot;https://developer.apple.com/xcode/&quot;&gt;AppleのXcode 3統合開発環境&lt;/a&gt;の入力として使用できる追加のインデックスファイルが生成されます。ドキュメントセットを作成するために、DoxygenはHTML出力ディレクトリにMakefileを生成します。\c makeを実行すると、そのディレクトリにdocsetが生成され、&lt;code&gt;make install&lt;/code&gt;を実行すると、docsetが&lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt;にインストールされ、Xcodeが起動時にそれを見つけられるようになります。詳細については、https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.htmlを参照してください。</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>このタグはdocsetフィードの名前を決定します。ドキュメントフィードは、単一のプロバイダー（企業や製品スイートなど）からの複数のドキュメントセットをグループ化できる傘を提供します。</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>このタグはdocsetフィードのURLを決定します。ドキュメントフィードは、単一のプロバイダー（企業や製品スイートなど）からの複数のドキュメントセットをグループ化できる傘を提供します。</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>このタグは、ドキュメントセットバンドルを一意に識別する文字列を指定します。これは逆ドメイン名スタイルの文字列である必要があります。例：&lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;。Doxygenは名前に&lt;code&gt;.docset&lt;/code&gt;を追加します。</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>\c DOCSET_PUBLISHER_IDタグは、ドキュメントパブリッシャーを一意に識別する文字列を指定します。これは逆ドメイン名スタイルの文字列である必要があります。例：&lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;。</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>\c DOCSET_PUBLISHER_NAMEタグはドキュメントパブリッシャーを識別します。</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>\c GENERATE_HTMLHELPタグが\c YESに設定されている場合、Doxygenは3つの追加HTMLインデックスファイルを生成します：\c index.hhp、\c index.hhc、および\c index.hhk。\c index.hhpは、Windows上のMicrosoftのHTMLヘルプワークショップで読み取ることができるプロジェクトファイルです。2021年初頭、Microsoftは元のページをオフラインにしました（HTMLヘルプワークショップはすでに何年もメンテナンスモードでした）。WebアーカイブからHTMLヘルプワークショップをダウンロードできます：&lt;a href=&quot;http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe&quot;&gt;インストール実行可能ファイル&lt;/a&gt;。&lt;br&gt;HTMLヘルプワークショップには、Doxygenによって生成されたすべてのHTML出力を単一のコンパイル済みHTMLファイル（`.chm`）に変換できるコンパイラが含まれています。コンパイル済みHTMLファイルは現在Windows 98ヘルプ形式として使用されており、将来すべてのWindowsプラットフォームで古いWindowsヘルプ形式（`.hlp`）に取って代わります。圧縮HTMLファイルにはインデックスと目次も含まれており、ドキュメント内の単語を検索できます。HTMLワークショップには、圧縮HTMLファイルのビューアも含まれています。</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>\c CHM_FILEタグを使用して、結果の`.chm`ファイルのファイル名を指定できます。結果をHTML出力ディレクトリに書き込まない場合は、ファイルの前にパスを追加できます。</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>\c HHC_LOCATIONタグを使用して、HTMLヘルプコンパイラ（\c hhc.exe）の場所（ファイル名を含む絶対パス）を指定できます。空でない場合、Doxygenは生成された\c index.hhpでHTMLヘルプコンパイラを実行しようとします。</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>\c GENERATE_CHIフラグは、別の`.chi`インデックスファイルが生成されるか（\c YES）、メインの`.chm`ファイルに含まれるか（\c NO）を制御します。</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>\c CHM_INDEX_ENCODINGは、HtmlHelpインデックス（\c hhk）、コンテンツ（\c hhc）、およびプロジェクトファイルのコンテンツをエンコードするために使用されます。</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>\c BINARY_TOCフラグは、`.chm`ファイルでバイナリ目次が生成されるか（\c YES）、通常の目次が生成されるか（\c NO）を制御します。さらに、`前へ`および`次へ`ボタンを有効にします。</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>\c TOC_EXPANDフラグを\c YESに設定すると、HTMLヘルプドキュメントの目次とツリービューにグループメンバーの追加項目が追加されます。</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>\c SITEMAP_URLタグは、ドキュメントのデプロイ中にユーザーが生成されたドキュメントをサーバーに配置する場所の完全なURLを指定するために使用されます。生成されたサイトマップは`sitemap.xml`と呼ばれ、\ref cfg_html_output &quot;HTML_OUTPUT&quot;で指定されたディレクトリに配置されます。\c SITEMAP_URLが指定されていない場合、サイトマップは生成されません。サイトマッププロトコルの詳細については、https://www.sitemaps.orgを参照してください。</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>\c GENERATE_QHPタグが\c YESに設定され、\ref cfg_qhp_namespace &quot;QHP_NAMESPACE&quot;と\ref cfg_qhp_virtual_folder &quot;QHP_VIRTUAL_FOLDER&quot;の両方が設定されている場合、生成されたHTMLドキュメントのQt圧縮ヘルプ（`.qch`）を生成するためにQtのqhelpgeneratorの入力として使用できる追加のインデックスファイルが生成されます。</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>\ref cfg_qhg_location &quot;QHG_LOCATION&quot;タグが指定されている場合、\c QCH_FILEタグを使用して結果の`.qch`ファイルのファイル名を指定できます。指定されたパスはHTML出力フォルダーからの相対パスです。</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>\c QHP_NAMESPACEタグは、Qtヘルププロジェクト出力を生成するときに使用する名前空間を指定します。詳細については、&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace&quot;&gt;Qtヘルププロジェクト/名前空間&lt;/a&gt;を参照してください。</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>\c QHP_VIRTUAL_FOLDERタグは、Qtヘルププロジェクト出力を生成するときに使用する名前空間を指定します。詳細については、&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders&quot;&gt;Qtヘルププロジェクト/仮想フォルダー&lt;/a&gt;を参照してください。</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>\c QHP_CUST_FILTER_NAMEタグが設定されている場合、追加するカスタムフィルターの名前を指定します。詳細については、&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qtヘルププロジェクト/カスタムフィルター&lt;/a&gt;を参照してください。</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>\c QHP_CUST_FILTER_ATTRSタグは、追加するカスタムフィルターの属性のリストを指定します。詳細については、&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qtヘルププロジェクト/カスタムフィルター&lt;/a&gt;を参照してください。</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>\c QHP_SECT_FILTER_ATTRSタグは、このプロジェクトのフィルターセクションが一致する属性のリストを指定します。&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes&quot;&gt;Qtヘルププロジェクト/フィルター属性&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>\c QHG_LOCATIONタグを使用して、Qtのqhelpgeneratorの場所（ファイル名を含む絶対パス）を指定できます。空でない場合、Doxygenは生成された`.qhp`ファイルでqhelpgeneratorを実行しようとします。</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>\c GENERATE_ECLIPSEHELPタグが\c YESに設定されている場合、HTMLファイルと共に追加のインデックスファイルが生成され、それらは`Eclipse`ヘルププラグインを形成します。このプラグインをインストールし、`Eclipse`のヘルプコンテンツメニューの下で使用できるようにするには、HTMLおよびXMLファイルを含むディレクトリの内容をeclipseのプラグインディレクトリにコピーする必要があります。プラグインディレクトリ内のディレクトリの名前は、\ref cfg_eclipse_doc_id &quot;ECLIPSE_DOC_ID&quot;値と同じである必要があります。コピー後、ヘルプが表示される前に`Eclipse`を再起動する必要があります。</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>`Eclipse`ヘルププラグインの一意の識別子。プラグインをインストールするとき、HTMLおよびXMLファイルを含むディレクトリ名もこの名前を持つ必要があります。各ドキュメントセットには独自の識別子が必要です。</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>生成されたHTMLページのレイアウトを完全に制御したい場合は、インデックスを無効にして独自のものに置き換える必要がある場合があります。\c DISABLE_INDEXタグを使用して、各HTMLページの上部にある圧縮インデックス（タブ）をオン/オフできます。\c NOの値はインデックスを有効にし、\c YESの値は無効にします。インデックスのタブにはナビゲーションツリーと同じ情報が含まれているため、\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;も\c YESに設定している場合は、このオプションを\c YESに設定できます。</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>\c GENERATE_TREEVIEWタグは、階層情報を表示するためのツリーのようなインデックス構造を生成するかどうかを指定するために使用されます。タグ値が\c YESに設定されている場合、ツリーのようなインデックス構造を含むサイドパネルが生成されます（HTMLヘルプ用に生成されるものと同様）。これが機能するには、JavaScript、DHTML、CSS、フレームをサポートするブラウザが必要です（つまり、最新のブラウザ）。WindowsユーザーはHTMLヘルプ機能を使用した方が良いでしょう。カスタムスタイルシート（\ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot;を参照）を使用すると、インデックスの外観をさらに微調整できます（\ref doxygen_finetuneを参照）。例として、Doxygenによって生成されたデフォルトのスタイルシートには、\ref cfg_project_name &quot;PROJECT_NAME&quot;の代わりにツリーのルートに画像を配置する方法を示す例があります。ツリーは基本的にタブインデックスよりも詳細な情報を持っているため、このオプションを有効にする際に\ref cfg_disable_index &quot;DISABLE_INDEX&quot;を\c YESに設定することを検討できます。</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;が\c YESに設定されている場合、\c PAGE_OUTLINE_PANELオプションは、画面の右側に追加のナビゲーションパネルを表示するかどうかを決定し、メインページのコンテンツのアウトラインを表示します（例：https://developer.android.com/reference）。\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;が\c NOに設定されている場合、このオプションは効果がありません。</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;が\c YESに設定されている場合、\c FULL_SIDEBARオプションは、サイドバーがツリービューエリアのみに制限されるか（値\c NO）、ウィンドウの全高に拡張されるか（値\c YES）を決定します。これを\c YESに設定すると、例えばhttps://docs.readthedocs.ioに似たレイアウトになり、コンテンツのためのスペースが増えますが、プロジェクトロゴ、タイトル、説明のためのスペースが減ります。\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;が\c NOに設定されている場合、このオプションは効果がありません。</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>\c ENUM_VALUES_PER_LINEタグを使用して、生成されたHTMLドキュメントでDoxygenが1行にグループ化する列挙値の数を設定できます。&lt;br&gt;値が0の場合、概要セクションから列挙値が完全に抑制されることに注意してください。</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>\c SHOW_ENUM_VALUESタグが設定されている場合、Doxygenは列挙ニーモニックの横に指定された列挙値を表示します。</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>ツリービューが有効になっている場合（\ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;を参照）、このタグを使用して、ツリーが表示されるフレームの初期幅（ピクセル単位）を設定できます。</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>\c EXT_LINKS_IN_WINDOWオプションが\c YESに設定されている場合、Doxygenはタグファイルを介してインポートされた外部シンボルへのリンクを別のウィンドウで開きます。</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>\c OBFUSCATE_EMAILSタグが\c YESに設定されている場合、Doxygenはメールアドレスを難読化します。</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>\c HTML_FORMULA_FORMATオプションが\c svgに設定されている場合、Doxygenはpdf2svgツール（https://github.com/dawbarton/pdf2svgを参照）またはinkscape（https://inkscape.orgを参照）を使用して、HTML出力のPNGの代わりに数式をSVG画像として生成します。これらの画像は、スケーリングされた解像度で一般的により良く見えます。</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>このタグを使用して、HTMLドキュメントに画像としてインクルードされる\f$\mbox{\LaTeX}\f$数式のフォントサイズを変更します。Doxygenの実行が成功した後にフォントサイズを変更する場合は、強制的に再生成するためにHTML出力ディレクトリからすべての`form_*.png`画像を手動で削除する必要があります。</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>\c FORMULA_MACROFILEには、数式の構成要素として使用される新しい\f$\mbox{\LaTeX}\f$コマンドを作成するための\f$\mbox{\LaTeX}\f$ `\newcommand`および`\renewcommand`コマンドを含めることができます。詳細については、セクション\ref formulasを参照してください。</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>\c USE_MATHJAXオプションを有効にすると、事前レンダリングされたビットマップを使用する代わりに、レンダリングにクライアント側JavaScriptを使用するMathJax（https://www.mathjax.orgを参照）を使用して\f$\mbox{\LaTeX}\f$数式をレンダリングします。\f$\mbox{\LaTeX}\f$がインストールされていない場合、またはHTML出力で数式をよりきれいに見せたい場合に使用します。有効にした場合、MathJaxを個別にインストールし、\ref cfg_mathjax_relpath &quot;MATHJAX_RELPATH&quot;オプションを使用してパスを設定する必要がある場合もあります。</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>\c MATHJAX_VERSIONを使用すると、使用するMathJaxのバージョンを指定できます。MathJaxの異なるバージョンは異なる設定に対して異なる要件を持っているため、MathJaxの異なるバージョン間で切り替える際に他のMathJax設定も変更する必要がある可能性があることに注意してください。</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>MathJaxが有効になっている場合、MathJax出力に使用されるデフォルトの出力形式を設定できます。出力形式の詳細については、&lt;a href=&quot;https://docs.mathjax.org/en/v2.7/output.html&quot;&gt;MathJaxバージョン2&lt;/a&gt;、&lt;a href=&quot;https://docs.mathjax.org/en/v3.2/output/index.html&quot;&gt;MathJaxバージョン3&lt;/a&gt;、および&lt;a href=&quot;https://docs.mathjax.org/en/v4.0/output/index.htm&quot;&gt;MathJaxバージョン4&lt;/a&gt;を参照してください。</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>MathJaxが有効になっている場合、\c MATHJAX_RELPATHオプションを使用してHTML出力ディレクトリからの相対的な場所を指定する必要があります。Mathjaxバージョン2の場合、宛先ディレクトリには`MathJax.js`スクリプトが含まれている必要があります。例えば、\c mathjaxディレクトリがHTML出力ディレクトリと同じレベルにある場合、\c MATHJAX_RELPATHは&lt;code&gt;../mathjax&lt;/code&gt;である必要があります。Mathjaxバージョン3および4の場合、宛先ディレクトリには`tex-&lt;format&gt;.js`スクリプト（`&lt;format&gt;`は`chtml`または`svg`のいずれか）が含まれている必要があります。デフォルト値はMathJaxコンテンツ配信ネットワークを指しているため、MathJaxをインストールしなくても結果をすぐに確認できます。ただし、デプロイ前にhttps://www.mathjax.orgからMathJaxのローカルコピーをインストールすることを強くお勧めします。デフォルト値は：- MathJaxバージョン2の場合：https://cdn.jsdelivr.net/npm/mathjax@2 - MathJaxバージョン3の場合：https://cdn.jsdelivr.net/npm/mathjax@3 - MathJaxバージョン4の場合：https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>\c MATHJAX_EXTENSIONSタグを使用して、MathJaxレンダリング中に有効にする1つ以上のMathJax拡張機能名を指定できます。例：MathJaxバージョン2（https://docs.mathjax.org/en/v2.7/tex.htmlを参照）：\verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim 例：MathJaxバージョン3（https://docs.mathjax.org/en/v3.2/input/tex/extensions/を参照）：\verbatim MATHJAX_EXTENSIONS = ams \endverbatim 例：MathJaxバージョン4（https://docs.mathjax.org/en/v4.0/input/tex/extensions/を参照）：\verbatim MATHJAX_EXTENSIONS = units \endverbatim Mathjaxバージョン4では、かなり多くの拡張機能がすでに自動的にロードされていることに注意してください。Mathjaxバージョン4でパッケージを無効にするには、マイナス記号を前に付けたパッケージ名を使用できます（`-`、例：`MATHJAX_EXTENSIONS += -textmacros`）</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>\c MATHJAX_CODEFILEタグを使用して、MathJaxコードの起動時に使用されるJavaScriptコードの断片を含むファイルを指定できます。詳細については、Mathjaxサイトを参照してください：- &lt;a href=&quot;https://docs.mathjax.org/en/v2.7/&quot;&gt;MathJaxバージョン2&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v3.2/&quot;&gt;MathJaxバージョン3&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v4.0/&quot;&gt;MathJaxバージョン4&lt;/a&gt;</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>\c SEARCHENGINEタグが有効な場合、DoxygenはHTML出力の検索ボックスを生成します。基礎となる検索エンジンはJavaScriptとDHTMLを使用し、最新のブラウザで動作するはずです。HTMLヘルプ（\ref cfg_generate_htmlhelp &quot;GENERATE_HTMLHELP&quot;）、Qtヘルプ（\ref cfg_generate_qhp &quot;GENERATE_QHP&quot;）、またはdocsets（\ref cfg_generate_docset &quot;GENERATE_DOCSET&quot;）を使用している場合、検索機能はすでに存在するため、これは通常無効にする必要があることに注意してください。大規模なプロジェクトでは、JavaScriptベースの検索エンジンは遅くなる可能性があるため、\ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;を有効にすると、より良いソリューションが提供される場合があります。キーボードを使用して検索できます。検索ボックスにジャンプするには、&lt;code&gt;\&lt;アクセスキー\&gt; + S&lt;/code&gt;を使用します（&lt;code&gt;\&lt;アクセスキー\&gt;&lt;/code&gt;が何であるかはOSとブラウザに依存しますが、通常は&lt;code&gt;\&lt;CTRL\&gt;&lt;/code&gt;、&lt;code&gt;\&lt;ALT\&gt;&lt;/code&gt;/&lt;code&gt;\&lt;option\&gt;&lt;/code&gt;、またはその両方です）。検索ボックス内で、&lt;code&gt;\&lt;下カーソルキー\&gt;&lt;/code&gt;を使用して検索結果ウィンドウにジャンプし、&lt;code&gt;\&lt;カーソルキー\&gt;&lt;/code&gt;を使用して結果をナビゲートできます。&lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt;を押して項目を選択するか、&lt;code&gt;\&lt;Esc\&gt;&lt;/code&gt;を押して検索をキャンセルします。カーソルが検索ボックス内にあるときに&lt;code&gt;\&lt;Shift\&gt;+\&lt;下カーソル\&gt;&lt;/code&gt;を押すと、フィルターオプションを選択できます。ここでも、&lt;code&gt;\&lt;カーソルキー\&gt;&lt;/code&gt;を使用してフィルターを選択し、&lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt;または&lt;code&gt;\&lt;Esc\&gt;&lt;/code&gt;を押してフィルターオプションをアクティブまたはキャンセルします。</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>\c SERVER_BASED_SEARCHタグが有効な場合、検索エンジンはJavaScriptを使用するWebクライアントの代わりにWebサーバーを使用して実装されます。\ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;設定に応じて、Webサーバーベースの検索には2つのタイプがあります。無効な場合、Doxygenは検索用のPHPスクリプトとスクリプトが使用するインデックスファイルを生成します。\ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;が有効な場合、インデックス作成と検索は外部ツールによって提供される必要があります。詳細については、セクション\ref extsearchを参照してください。</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>\c EXTERNAL_SEARCHタグが有効な場合、Doxygenは検索用のPHPスクリプトを生成しなくなります。代わりに、検索結果はXMLファイルに書き込まれ、外部インデクサーによって処理される必要があります。Doxygenは、\ref cfg_searchengine_url &quot;SEARCHENGINE_URL&quot;オプションで指定された外部検索エンジンを呼び出して検索結果を取得します。&lt;br&gt;Doxygenには、オープンソース検索エンジンライブラリ&lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;に基づいたサンプルインデクサー（\c doxyindexer）と検索エンジン（&lt;code&gt;doxysearch.cgi&lt;/code&gt;）が付属しています。&lt;br&gt;詳細については、セクション\ref extsearchを参照してください。</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>\c SEARCHENGINE_URLは、\ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;が有効な場合に検索結果を返すWebサーバーによってホストされる検索エンジンを指す必要があります。&lt;br&gt;Doxygenには、オープンソース検索エンジンライブラリ&lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;に基づいたサンプルインデクサー（\c doxyindexer）と検索エンジン（&lt;code&gt;doxysearch.cgi&lt;/code&gt;）が付属しています。詳細については、セクション\ref extsearchを参照してください。</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>\ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;と\ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;の両方が有効な場合、インデックス付けされていない検索データは、外部ツールによるインデックス作成のためにファイルに書き込まれます。\c SEARCHDATA_FILEタグでこのファイルの名前を指定できます。</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>\ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot;と\ref cfg_external_search &quot;EXTERNAL_SEARCH&quot;の両方が有効な場合、\c EXTERNAL_SEARCH_IDタグをプロジェクトの識別子として使用できます。これは、\ref cfg_extra_search_mappings &quot;EXTRA_SEARCH_MAPPINGS&quot;と組み合わせて、複数のプロジェクトを検索し、結果を正しいプロジェクトにリダイレクトするのに役立ちます。</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>\c EXTRA_SEARCH_MAPPINGSタグを使用して、この設定ファイルで定義されたプロジェクト以外のDoxygenプロジェクトを検索できるようにすることができますが、それらはすべて同じ外部検索インデックスに追加されます。各プロジェクトには、\ref cfg_external_search_id &quot;EXTERNAL_SEARCH_ID&quot;を介して設定された一意のIDが必要です。検索マッピングは、IDをドキュメントが見つかる相対的な場所にマッピングします。形式は：\verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>\c GENERATE_LATEXタグが\c YESに設定されている場合、Doxygenは\f$\mbox{\LaTeX}\f$出力を生成します。</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>\c LATEX_OUTPUTタグは、\f$\mbox{\LaTeX}\f$ドキュメントが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>\c LATEX_CMD_NAMEタグを使用して、呼び出す\f$\mbox{\LaTeX}\f$コマンド名を指定できます。&lt;br&gt;\ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot;を有効にしない場合、デフォルトは\c latexです。\ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot;を有効にする場合、デフォルトは\c pdflatexです。後者の場合、\c latexが選択されると、\c pdflatexによって上書きされます。特定の出力言語では、デフォルトが異なって設定されている可能性があり、これは出力言語の実装に依存します。</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>\c MAKEINDEX_CMD_NAMEタグを使用して、\f$\mbox{\LaTeX}\f$のインデックスを生成するコマンド名を指定できます。@note このタグは`Makefile` / `make.bat`で使用されます。\sa 生成された出力ファイル（`.tex`）の部分については、\ref cfg_latex_makeindex_cmd &quot;LATEX_MAKEINDEX_CMD&quot;。</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>\c LATEX_MAKEINDEX_CMDタグを使用して、\f$\mbox{\LaTeX}\f$のインデックスを生成するコマンド名を指定できます。最初の文字としてバックスラッシュ（`\`）がない場合、\f$\mbox{\LaTeX}\f$コードに自動的に追加されます。@note このタグは生成された出力ファイル（`.tex`）で使用されます。\sa `Makefile` / `make.bat`の部分については、\ref cfg_makeindex_cmd_name &quot;MAKEINDEX_CMD_NAME&quot;。</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>\c COMPACT_LATEXタグが\c YESに設定されている場合、Doxygenはよりコンパクトな\f$\mbox{\LaTeX}\f$ドキュメントを生成します。これは小さなプロジェクトに役立ち、一般的にいくつかの木を節約するのに役立ちます。</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>\c PAPER_TYPEタグを使用して、プリンターで使用される用紙タイプを設定できます。</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>\c EXTRA_PACKAGESタグを使用して、\f$\mbox{\LaTeX}\f$出力にインクルードされる1つ以上の\f$\mbox{\LaTeX}\f$パッケージ名を指定できます。パッケージは名前のみで指定するか、\f$\mbox{\LaTeX}\f$ `\usepackage`コマンドで使用する正しい構文で指定できます。例えば`times`フォントを取得するには、次のように指定できます：\verbatim EXTRA_PACKAGES=times または EXTRA_PACKAGES={times} \endverbatim `amsmath`パッケージで`intlimits`オプションを使用するには、次のように指定できます：\verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim 空白のままにすると、追加のパッケージはインクルードされません。</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>\c LATEX_HEADERタグを使用して、生成された\f$\mbox{\LaTeX}\f$ドキュメントのユーザー定義\f$\mbox{\LaTeX}\f$ヘッダーを指定できます。ヘッダーは最初の章までのすべてを含む必要があります。空白のままにすると、Doxygenは標準ヘッダーを生成します。次を使用してデフォルトのヘッダーから始めることを強くお勧めします：\verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim 次にファイル\c new_header.texを変更します。Doxygenが通常使用するデフォルトヘッダーの生成方法については、セクション\ref doxygen_usageも参照してください。&lt;br&gt;注意：自分が何をしているかを理解している場合にのみ、ユーザー定義ヘッダーを使用してください！@note ヘッダーは変更される可能性があるため、通常はDoxygenの新しいバージョンにアップグレードする際にデフォルトヘッダーを再生成する必要があります。次のコマンドはヘッダー（およびフッター）内で特別な意味を持ちます：</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>\c LATEX_FOOTERタグを使用して、生成された\f$\mbox{\LaTeX}\f$ドキュメントのユーザー定義\f$\mbox{\LaTeX}\f$フッターを指定できます。フッターは最後の章以降のすべてを含む必要があります。空白のままにすると、Doxygenは標準フッターを生成します。デフォルトのフッターの生成方法とフッター内で使用できる特別なコマンドの詳細については、\ref cfg_latex_header &quot;LATEX_HEADER&quot;を参照してください。Doxygenが通常使用するデフォルトフッターの生成方法については、セクション\ref doxygen_usageも参照してください。注意：自分が何をしているかを理解している場合にのみ、ユーザー定義フッターを使用してください！</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>\c LATEX_EXTRA_STYLESHEETタグを使用して、Doxygenによって作成された標準スタイルシートの後にインクルードされる追加のユーザー定義\f$\mbox{\LaTeX}\f$スタイルシートを指定できます。このオプションを使用すると、特定のスタイルの側面をオーバーライドできます。Doxygenはスタイルシートファイルを出力ディレクトリにコピーします。\note 追加のスタイルシートファイルの順序は重要です（例：リストの最後のスタイルシートは、リスト内の前のものの設定をオーバーライドします）。</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>\c LATEX_EXTRA_FILESタグを使用して、\ref cfg_latex_output &quot;LATEX_OUTPUT&quot;出力ディレクトリにコピーされる1つ以上の追加画像または他のソースファイルを指定できます。ファイルはそのままコピーされることに注意してください。利用可能なコマンドやマーカーはありません。</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>\c PDF_HYPERLINKSタグが\c YESに設定されている場合、生成された\f$\mbox{\LaTeX}\f$はPDFへの変換（\c ps2pdfまたは\c pdflatexを使用）のために準備されます。PDFファイルには、ページ参照の代わりにリンク（HTML出力と同様）が含まれます。これにより、PDFビューアを使用したオンラインブラウジングに適した出力になります。</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>\c USE_PDFLATEXタグが\c YESに設定されている場合、Doxygenは\ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot;で指定されたエンジンを使用して、\f$\mbox{\LaTeX}\f$ファイルから直接PDFファイルを生成します。より高品質のPDFドキュメントを取得するには、このオプションを\c YESに設定します。&lt;br&gt;エンジンの選択については、セクション\ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot;も参照してください。</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>\c LATEX_BATCHMODEタグは、エラーが発生した場合の\f$\mbox{\LaTeX}\f$の動作を通知します。</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>\c LATEX_HIDE_INDICESタグが\c YESに設定されている場合、Doxygenは出力にインデックスの章（ファイルインデックス、コンパウンドインデックスなど）を含めません。</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>\c LATEX_BIB_STYLEタグを使用して、参考文献に使用するスタイルを指定できます。例：\c plainnat、または\c ieeetr。詳細については、https://en.wikipedia.org/wiki/BibTeXおよび\ref cmdcite &quot;\cite&quot;を参照してください。</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>\c LATEX_EMOJI_DIRECTORYタグは、絵文字画像が読み込まれる（相対または絶対）パスを指定するために使用されます。相対パスが入力された場合、\ref cfg_latex_output &quot;LATEX_OUTPUT&quot;ディレクトリからの相対パスになります。空白のままにすると、\ref cfg_latex_output &quot;LATEX_OUTPUT&quot;ディレクトリが使用されます。</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>\c GENERATE_RTFタグが\c YESに設定されている場合、DoxygenはRTF出力を生成します。RTF出力はWord 97用に最適化されており、他のRTFリーダー/エディタではあまりきれいに見えない場合があります。</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>\c RTF_OUTPUTタグは、RTFドキュメントが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>\c COMPACT_RTFタグが\c YESに設定されている場合、DoxygenはよりコンパクトなRTFドキュメントを生成します。これは小さなプロジェクトに役立ち、一般的にいくつかの木を節約するのに役立ちます。</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>\c RTF_HYPERLINKSタグが\c YESに設定されている場合、生成されたRTFにはハイパーリンクフィールドが含まれます。RTFファイルには、ページ参照の代わりにリンク（HTML出力と同様）が含まれます。これにより、Wordまたはこれらのフィールドをサポートする他のWord互換リーダーを使用したオンラインブラウジングに適した出力になります。&lt;br&gt;注意：WordPad（write）およびその他はリンクをサポートしていません。</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>ファイルからスタイルシート定義をロードします。構文はDoxygenの設定ファイルと同様で、一連の割り当てです。オーバーライドのみを提供する必要があり、欠落している定義はデフォルト値に設定されます。&lt;br&gt;Doxygenが通常使用するデフォルトのスタイルシートの生成方法については、セクション\ref doxygen_usageも参照してください。</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>RTFドキュメントの生成で使用されるオプションの変数を定義します。構文はDoxygenの設定ファイルと同様です。テンプレート拡張ファイルは、&lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt;を使用して生成できます。</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>\c RTF_EXTRA_FILESタグを使用して、\ref cfg_rtf_output &quot;RTF_OUTPUT&quot;出力ディレクトリにコピーされる1つ以上の追加画像または他のソースファイルを指定できます。ファイルはそのままコピーされることに注意してください。利用可能なコマンドやマーカーはありません。</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>\c GENERATE_MANタグが\c YESに設定されている場合、Doxygenはクラスとファイルのマニュアルページを生成します。</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>\c MAN_OUTPUTタグは、マニュアルページが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。\c man3ディレクトリが\c MAN_OUTPUTで指定されたディレクトリ内に作成されます。</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>\c MAN_EXTENSIONタグは、生成されたマニュアルページに追加される拡張子を決定します。マニュアルセクションが数字で始まらない場合、数字3が先頭に追加されます。\c MAN_EXTENSIONタグの先頭のピリオド（.）はオプションです。</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>\c MAN_SUBDIRタグは、マニュアルページが配置される\c MAN_OUTPUT内に作成されるディレクトリの名前を決定します。デフォルトでは、\c MAN_EXTENSIONの先頭の`.`を削除したものがmanに続きます。</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>\c MAN_LINKSタグが\c YESに設定され、Doxygenがman出力を生成する場合、実際のマニュアルページ内の文書化された各エンティティに対して追加のmanファイルが生成されます。これらの追加ファイルは実際のマニュアルページをソースするだけですが、それらがないと\c manコマンドは正しいページを見つけることができません。</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>\c GENERATE_XMLタグが\c YESに設定されている場合、Doxygenはコード構造をキャプチャするXMLファイルを生成し、すべてのドキュメントを含みます。</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>\c XML_OUTPUTタグは、XMLページが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>\c XML_PROGRAMLISTINGタグが\c YESに設定されている場合、DoxygenはXML出力にプログラムリスト（構文強調表示と相互参照情報を含む）をコピーします。これを有効にするとXML出力のサイズが大幅に増加することに注意してください。</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>\c XML_NS_MEMB_FILE_SCOPEタグが\c YESに設定されている場合、Doxygenはファイルスコープ内の名前空間メンバーも含め、HTML出力に対応します。</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>\c GENERATE_DOCBOOKタグが\c YESに設定されている場合、DoxygenはPDFの生成に使用できるDocbookファイルを生成します。</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>\c DOCBOOK_OUTPUTタグは、Docbookページが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>\c GENERATE_AUTOGEN_DEFタグが\c YESに設定されている場合、Doxygenはコード構造をキャプチャするAutoGen定義ファイル（https://autogen.sourceforge.net/を参照）を生成し、すべてのドキュメントを含みます。この機能はまだ実験的であり、現時点では不完全であることに注意してください。</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>\c GENERATE_SQLITE3タグが\c YESに設定されている場合、Doxygenは\c Sqlite3データベースを生成し、Doxygenによって見つかったシンボルがテーブルに保存されます。</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>\c SQLITE3_OUTPUTタグは、\c Sqlite3データベースが配置される場所を指定するために使用されます。相対パスが入力された場合、\ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;の値がその前に配置されます。</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>\c SQLITE3_RECREATE_DBタグが\c YESに設定されている場合、既存のdoxygen_sqlite3.dbデータベースファイルはDoxygenが実行されるたびに再作成されます。\c NOに設定されている場合、Doxygenはデータベースファイルがすでに見つかった場合に警告し、変更しません。</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>\c GENERATE_PERLMODタグが\c YESに設定されている場合、Doxygenはコード構造をキャプチャするPerlモジュールファイルを生成し、すべてのドキュメントを含みます。&lt;br&gt;この機能はまだ実験的であり、現時点では不完全であることに注意してください。</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>\c PERLMOD_LATEXタグが\c YESに設定されている場合、Doxygenは必要な\c Makefileルール、\c Perlスクリプト、および\f$\mbox{\LaTeX}\f$コードを生成し、Perlモジュール出力からPDFおよびDVI出力を生成できるようにします。</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>\c PERLMOD_PRETTYタグが\c YESに設定されている場合、Perlモジュール出力は人間が読めるようにきれいにフォーマットされます。これは何が起こっているかを理解したい場合に便利です。一方、このタグが\c NOに設定されている場合、Perlモジュール出力のサイズははるかに小さくなり、Perlは同様に解析します。</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>生成された`doxyrules.make`ファイルのmake変数名は、\c PERLMOD_MAKEVAR_PREFIXに含まれる文字列が前に付きます。これは、同じ`Makefile`に含まれる異なる`doxyrules.make`ファイルが互いの変数を上書きしないようにするのに役立ちます。</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>\c ENABLE_PREPROCESSINGタグが\c YESに設定されている場合、Doxygenはソースおよびインクルードファイルで見つかったすべてのCプリプロセッサディレクティブを評価します。</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>\c MACRO_EXPANSIONタグが\c YESに設定されている場合、Doxygenはソースコード内のすべてのマクロ名を展開します。\c NOに設定されている場合、条件付きコンパイルのみが実行されます。マクロ展開は、\ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot;を\c YESに設定することで制御された方法で行うことができます。</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>\c EXPAND_ONLY_PREDEFおよび\ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot;タグの両方が\c YESに設定されている場合、マクロ展開は\ref cfg_predefined &quot;PREDEFINED&quot;および\ref cfg_expand_as_defined &quot;EXPAND_AS_DEFINED&quot;タグで指定されたマクロに制限されます。</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>\c SEARCH_INCLUDESタグが\c YESに設定されている場合、\c \#includeが見つかった場合、\ref cfg_include_path &quot;INCLUDE_PATH&quot;内のインクルードファイルが検索されます。</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>\c INCLUDE_PATHタグを使用して、入力ファイルではないがプリプロセッサによって処理する必要があるインクルードファイルを含む1つ以上のディレクトリを指定できます。\c INCLUDE_PATHは再帰的ではないため、\ref cfg_recursive &quot;RECURSIVE&quot;の設定はここでは効果がないことに注意してください。</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>\c INCLUDE_FILE_PATTERNSタグを使用して、ディレクトリ内のヘッダーファイルをフィルタリングする1つ以上のワイルドカードパターン（`*.h`や`*.hpp`のように）を指定できます。空白のままにすると、\ref cfg_file_patterns &quot;FILE_PATTERNS&quot;で指定されたパターンが使用されます。</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>\c PREDEFINEDタグを使用して、プリプロセッサが起動する前に定義される1つ以上のマクロ名を指定できます（例：\c gccの`-D`オプションと同様）。タグの引数は、&lt;code&gt;name&lt;/code&gt;または&lt;code&gt;name=definition&lt;/code&gt;の形式のマクロのリストです（スペースなし）。定義と`=`が省略された場合、`=1`が仮定されます。マクロ定義が\c \#undefを介して未定義にされたり、再帰的に展開されたりするのを防ぐには、\c =演算子の代わりに&lt;code&gt;:=&lt;/code&gt;演算子を使用します。</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>\ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot;および\ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot;タグが\c YESに設定されている場合、このタグを使用して展開するマクロ名のリストを指定できます。ソースで見つかったマクロ定義が使用されます。ソースコードで見つかった定義をオーバーライドする異なるマクロ定義を使用する場合は、\ref cfg_predefined &quot;PREDEFINED&quot;タグを使用します。</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>\c SKIP_FUNCTION_MACROSタグが\c YESに設定されている場合、Doxygenのプリプロセッサは、行単独で、すべて大文字の名前を持ち、セミコロンで終わらない関数のようなマクロへのすべての参照を削除します。そのような関数マクロは通常ボイラープレートコードに使用され、削除されないとパーサーを混乱させます。</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>\c TAGFILESタグを使用して、1つ以上のタグファイルを指定できます。各タグファイルについて、外部ドキュメントの場所を追加する必要があります。場所なしのタグファイルの形式は次のとおりです：\verbatim TAGFILES = file1 file2 ... \endverbatim タグファイルの場所の追加は次のように行います：\verbatim TAGFILES = file1=loc1 &quot;file2 = loc2&quot; ... \endverbatim ここで`loc1`と`loc2`は相対パス、絶対パス、またはURLにすることができます。タグファイルの使用の詳細については、セクション\ref externalを参照してください。\note 各タグファイルは一意の名前を持つ必要があります（または名前はパスを含みません）。タグファイルがDoxygenが実行されるディレクトリにない場合は、ここでタグファイルへのパスも指定する必要があります。</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>\c GENERATE_TAGFILEの後にファイル名が指定されている場合、Doxygenは読み取った入力ファイルに基づいてタグファイルを作成します。タグファイルの使用の詳細については、セクション\ref externalを参照してください。</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>\c ALLEXTERNALSタグが\c YESに設定されている場合、すべての外部クラスと名前空間がクラスおよび名前空間インデックスにリストされます。\c NOに設定されている場合、継承された外部クラスのみがリストされます。</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>\c EXTERNAL_GROUPSタグが\c YESに設定されている場合、すべての外部グループがトピックインデックスにリストされます。\c NOに設定されている場合、現在のプロジェクトのグループのみがリストされます。</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>\c EXTERNAL_PAGESタグが\c YESに設定されている場合、すべての外部ページが関連ページインデックスにリストされます。\c NOに設定されている場合、現在のプロジェクトのページのみがリストされます。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>\c YESに設定されている場合、継承およびコラボレーショングラフは、ターゲットが文書化されていないかクラスでない場合、継承および使用関係を非表示にします。</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>\c HAVE_DOTタグを\c YESに設定すると、Doxygenは\c pathから\c dotツールが利用可能であると仮定します。このツールはAT&amp;TおよびLucent Bell Labsの&lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt;グラフ可視化ツールキットの一部です。このオプションが\c NOに設定されている場合、このセクションの他のオプションは効果がありません。</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>\c DOT_NUM_THREADSは、Doxygenが並列で実行を許可される\c dot呼び出しの数を指定します。\c 0に設定すると、Doxygenはシステムで利用可能なプロセッサの数に基づいて設定します。CPU負荷と処理速度のバランスをより制御するために、0より大きい値に明示的に設定できます。</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTRは、ノード、エッジ、およびサブグラフラベルの共通属性です。Doxygenが生成するdotファイルで異なる外観のフォントを使用したい場合、fontname、fontcolor、およびfontsize属性を指定できます。詳細については、&lt;a href=https://graphviz.org/doc/info/attrs.html&gt;ノード、エッジ、およびグラフ属性の仕様&lt;/a&gt;を参照してください。dotがフォントを見つけることができることを確認する必要があります。これは、標準の場所に配置するか、\c DOTFONTPATH環境変数を設定するか、フォントを含むディレクトリに\ref cfg_dot_fontpath &quot;DOT_FONTPATH&quot;を設定することで行います。graphvizのデフォルトフォントサイズは14です。</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTRは\ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot;と連結されます。エレガントなスタイルのために、&#x27;arrowhead=open, arrowtail=open, arrowsize=0.5&#x27;を追加できます。&lt;a href=https://graphviz.org/doc/info/arrows.html&gt;矢印形状の完全なドキュメント。&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTRは\ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot;と連結されます。ノードの周りのボックスなしのビューには、&#x27;shape=plain&#x27;または&#x27;shape=plaintext&#x27;を設定します。&lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;形状の仕様。&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>\ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot;および他のdot属性でfontnameで指定されたフォントをdotが見つけることができるパスを設定できます。</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>\c CLASS_GRAPHタグが\c YESまたは\c GRAPHまたは\c BUILTINに設定されている場合、Doxygenは文書化された各クラスのグラフを生成し、直接および間接の継承関係を示します。\c CLASS_GRAPHタグが\c YESまたは\c GRAPHに設定され、\ref cfg_have_dot &quot;HAVE_DOT&quot;も有効な場合、`dot`がグラフの描画に使用されます。\c CLASS_GRAPHタグが\c YESに設定され、\ref cfg_have_dot &quot;HAVE_DOT&quot;が無効な場合、または\c CLASS_GRAPHタグが\c BUILTINに設定されている場合、組み込みジェネレーターが使用されます。\c CLASS_GRAPHタグが\c TEXTに設定されている場合、直接および間接の継承関係がテキスト/リンクとして表示されます。特定のクラスの継承グラフを明示的に有効にするか、異なる表現を選択するには、\ref cmdinheritancegraph &quot;\inheritancegraph&quot;コマンドを使用できます。継承グラフを無効にするには、\ref cmdhideinheritancegraph &quot;\hideinheritancegraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>\c COLLABORATION_GRAPHタグが\c YESに設定されている場合、Doxygenは文書化された各クラスのグラフを生成し、クラスと他の文書化されたクラスとの直接および間接の実装依存関係（継承、包含、およびクラス参照変数）を示します。\c COLLABORATION_GRAPHが\c NOに設定されている場合、コラボレーショングラフを明示的に有効にするには、\ref cmdcollaborationgraph &quot;\collaborationgraph&quot;コマンドを使用できます。コラボレーショングラフを無効にするには、\ref cmdhidecollaborationgraph &quot;\hidecollaborationgraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>\c UML_LOOKタグが\c YESに設定されている場合、DoxygenはOMGの統一モデリング言語と同様のスタイルで継承およびコラボレーションダイアグラムを生成します。</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>\ref cfg_uml_look &quot;UML_LOOK&quot;タグが有効な場合、フィールドとメソッドはクラスノード内に表示されます。多くのフィールドまたはメソッドと多くのノードがある場合、グラフは有用でないほど大きくなる可能性があります。\c UML_LIMIT_NUM_FIELDSしきい値は、サイズをより管理しやすくするために各タイプの要素数を制限します。制限なしの場合は0に設定します。しきい値は制限が適用される前に50%超過する可能性があることに注意してください。したがって、しきい値を10に設定すると、最大15フィールドが表示される可能性がありますが、数が15を超えると、表示されるフィールドの総数は10に制限されます。</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>\ref cfg_uml_look &quot;UML_LOOK&quot;タグが有効な場合、フィールドラベルは2つのクラスノード間のエッジに沿って表示されます。多くのフィールドと多くのノードがある場合、グラフは雑然としすぎる可能性があります。\c UML_MAX_EDGE_LABELSしきい値は、サイズをより管理しやすくするために要素数を制限します。制限なしの場合は0に設定します。</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>\c DOT_UML_DETAILSタグが\c NOに設定されている場合、DoxygenはUMLグラフでタイプと引数なしで属性とメソッドを表示します。\c DOT_UML_DETAILSタグが\c YESに設定されている場合、DoxygenはUMLグラフで属性とメソッドのタイプと引数を追加します。\c DOT_UML_DETAILSタグが\c NONEに設定されている場合、DoxygenはUMLグラフでクラスメンバー情報を持つフィールドを生成しません。クラス図はデフォルトのクラス図に似ていますが、関係にUML表記を使用します。</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>\c DOT_WRAP_THRESHOLDタグを使用して、1行に表示される最大文字数を設定できます。実際の行の長さがこのしきい値を大幅に超える場合、複数行に分割されます。見苦しい改行を避けるためにいくつかのヒューリスティックが適用されます。</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>\c TEMPLATE_RELATIONSタグが\c YESに設定されている場合、継承およびコラボレーショングラフはテンプレートとそのインスタンス間の関係を示します。</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>\c INCLUDE_GRAPH、\ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot;、および\ref cfg_search_includes &quot;SEARCH_INCLUDES&quot;タグがすべて\c YESに設定されている場合、Doxygenは文書化された各ファイルのグラフを生成し、ファイルと他の文書化されたファイルとの直接および間接のインクルード依存関係を示します。\c INCLUDE_GRAPHが\c NOに設定されている場合、インクルードグラフを明示的に有効にするには、\ref cmdincludegraph &quot;\includegraph&quot;コマンドを使用できます。インクルードグラフを無効にするには、\ref cmdhideincludegraph &quot;\hideincludegraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>\c INCLUDED_BY_GRAPH、\ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot;、および\ref cfg_search_includes &quot;SEARCH_INCLUDES&quot;タグがすべて\c YESに設定されている場合、Doxygenは文書化された各ファイルのグラフを生成し、ファイルと他の文書化されたファイルとの直接および間接のインクルード依存関係を示します。\c INCLUDED_BY_GRAPHが\c NOに設定されている場合、インクルード元グラフを明示的に有効にするには、\ref cmdincludedbygraph &quot;\includedbygraph&quot;コマンドを使用できます。インクルード元グラフを無効にするには、\ref cmdhideincludedbygraph &quot;\hideincludedbygraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>\c CALL_GRAPHタグが\c YESに設定されている場合、Doxygenは各グローバル関数またはクラスメソッドの呼び出し依存グラフを生成します。&lt;br&gt;このオプションを有効にすると実行時間が大幅に増加することに注意してください。したがって、ほとんどの場合、\ref cmdcallgraph &quot;\callgraph&quot;コマンドを使用して選択した関数の呼び出しグラフのみを有効にする方が良いでしょう。呼び出しグラフを無効にするには、\ref cmdhidecallgraph &quot;\hidecallgraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>\c CALLER_GRAPHタグが\c YESに設定されている場合、Doxygenは各グローバル関数またはクラスメソッドの呼び出し元依存グラフを生成します。&lt;br&gt;このオプションを有効にすると実行時間が大幅に増加することに注意してください。したがって、ほとんどの場合、\ref cmdcallergraph &quot;\callergraph&quot;コマンドを使用して選択した関数の呼び出し元グラフのみを有効にする方が良いでしょう。呼び出し元グラフを無効にするには、\ref cmdhidecallergraph &quot;\hidecallergraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>\c GRAPHICAL_HIERARCHYタグが\c YESに設定されている場合、Doxygenはテキストではなくすべてのクラスのグラフィカル階層を生成します。</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>\c DIRECTORY_GRAPHタグが\c YESに設定されている場合、Doxygenはディレクトリの他のディレクトリへの依存関係をグラフィカルに示します。依存関係は、ディレクトリ内のファイル間の\c \#include関係によって決定されます。\c DIRECTORY_GRAPHが\c NOに設定されている場合、ディレクトリグラフを明示的に有効にするには、\ref cmddirectorygraph &quot;\directorygraph&quot;コマンドを使用できます。ディレクトリグラフを無効にするには、\ref cmdhidedirectorygraph &quot;\hidedirectorygraph&quot;コマンドを使用できます。</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>\c DIR_GRAPH_MAX_DEPTHタグを使用して、\c dotによって生成されるディレクトリ依存グラフで生成されるサブディレクトリレベルの最大数を制限できます。</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>\c DOT_IMAGE_FORMATタグを使用して、\c dotによって生成される画像の画像形式を設定できます。画像形式の説明については、\c dotツールのドキュメントの出力形式のセクションを参照してください（&lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt;）。&lt;br&gt;`svg:cairo`および`svg:cairo:cairo`形式は\ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot;と組み合わせて使用できないことに注意してください（\ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot;は`NO`に設定されます）。</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>\ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot;が`svg`または`svg:svg`または`svg:svg:core`に設定されている場合、このオプションを\c YESに設定して、ズームとパンを可能にするインタラクティブSVG画像の生成を有効にできます。&lt;br&gt;Internet Explorer以外の最新のブラウザが必要です。テスト済みで動作するもの：Firefox、Chrome、Safari、Opera。&lt;br&gt;\ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot;が`svg:cairo`または`svg:cairo:cairo`に設定されている場合、このオプションは自動的に無効になることに注意してください。</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>\c DOT_PATHタグを使用して、\c dotツールが見つかるパスを指定できます。空白のままにすると、\c dotツールが\c pathで見つかると仮定されます。</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>\c DOTFILE_DIRSタグを使用して、ドキュメントにインクルードされるdotファイルを含む1つ以上のディレクトリを指定できます（\ref cmddotfile &quot;\dotfile&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>ドキュメントにdiaで作成されたダイアグラムをインクルードできます。Doxygenはdiaを実行してダイアグラムを生成し、ドキュメントに挿入します。DIA_PATHタグを使用すると、diaバイナリが存在するディレクトリを指定できます。空白のままにすると、diaはデフォルトの検索パスで見つかると仮定されます。</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>\c DIAFILE_DIRSタグを使用して、ドキュメントにインクルードされるdiaファイルを含む1つ以上のディレクトリを指定できます（\ref cmddiafile &quot;\diafile&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>PlantUMLを使用する場合、\c PLANTUML_JAR_PATHタグを使用して、javaが\c plantuml.jarファイルまたは使用する\c jarファイルのファイル名を見つけることができるパスを指定する必要があります。空白のままにすると、PlantUMLは使用されないか、前処理ステップ中に呼び出されると仮定されます。この場合、Doxygenは\ref cmdstartuml &quot;\startuml&quot;コマンドに遭遇すると警告を生成し、ダイアグラムの出力を生成しません。</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>PlantUMLを使用する場合、\c PLANTUML_CFG_FILEタグを使用してPlantUMLの設定ファイルを指定できます。</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>PlantUMLを使用する場合、指定されたパスはPlantUMLブロック内の\c !includeステートメントで指定されたファイルに対して検索されます。</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>\c PLANTUMLFILE_DIRSタグを使用して、ドキュメントにインクルードされるPlantUmlファイルを含む1つ以上のディレクトリを指定できます（\ref cmdplantumlfile &quot;\plantumlfile&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>CLIレンダリングでMermaidダイアグラムを使用する場合、\c MERMAID_PATHタグを使用して、\c mmdc（Mermaid CLI）実行可能ファイルが見つかるディレクトリを指定する必要があります。空白のままにすると、CLIベースのレンダリングは無効になります。HTML出力の場合、JavaScriptによるクライアント側レンダリングがデフォルトで使用され、\c mmdcは必要ありません。\f$\mbox{\LaTeX}\f$/PDF出力の場合、画像を事前生成するために\c mmdcが必要です。CLIレンダリングが必要だが\c mmdcが利用できない場合、Doxygenは警告を生成します。</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Mermaidダイアグラムを使用する場合、\c MERMAID_CONFIG_FILEタグを使用して、Mermaid CLIツール（\c mmdc）のJSON設定ファイルを指定できます。このファイルには、テーマ設定やその他のMermaid設定オプションを含めることができます。</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>\c MERMAID_RENDER_MODEタグは、Mermaidダイアグラムがレンダリングされる方法を選択します。</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>\c MERMAID_JS_URLタグは、クライアント側レンダリングを使用する場合（\c MERMAID_RENDER_MODEが\c CLIENT_SIDEまたは\c AUTO）に\c mermaid.jsをロードするURLを指定します。デフォルトはjsDelivr CDNの最新のMermaid v11を指しています。\par 例：- 最新のv11（デフォルト）：\c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs&#x27; - ピン留めされたバージョン：\c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs&#x27; - ローカルコピー：\c &#x27;./mermaid.esm.min.mjs&#x27;（ユーザーはファイルをHTML出力ディレクトリに配置する必要があります）\par デフォルトのCDN URLは、生成されたドキュメントを表示する際にインターネットアクセスが必要です。オフライン使用の場合は、\c mermaid.esm.min.mjsをダウンロードしてこれを相対パスに設定するか、代わりに\c MERMAID_RENDER_MODE=CLIを使用して画像を事前生成します。</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>\c MERMAIDFILE_DIRSタグを使用して、ドキュメントにインクルードされるMermaidファイルを含む1つ以上のディレクトリを指定できます（\ref cmdmermaidfile &quot;\mermaidfile&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>\c DOT_GRAPH_MAX_NODESタグを使用して、グラフに表示されるノードの最大数を設定できます。グラフ内のノード数がこの値を超える場合、Doxygenはグラフを切り捨て、ノードを赤いボックスとして表すことで視覚化します。グラフのルートノードの直接の子の数がすでに\c DOT_GRAPH_MAX_NODESを超えている場合、グラフはまったく表示されないことに注意してください。また、グラフのサイズは\ref cfg_max_dot_graph_depth &quot;MAX_DOT_GRAPH_DEPTH&quot;によってさらに制限される可能性があることにも注意してください。</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>\c MAX_DOT_GRAPH_DEPTHタグを使用して、\c dotによって生成されるグラフの最大深度を設定できます。深度値3は、最大3エッジを通るパスを介してルートから到達可能なノードのみが表示されることを意味します。ルートノードからより遠いノードは省略されます。このオプションを1または2に設定すると、大規模なコードベースの計算時間が大幅に削減される可能性があることに注意してください。また、グラフのサイズは\ref cfg_dot_graph_max_nodes &quot;DOT_GRAPH_MAX_NODES&quot;によってさらに制限される可能性があることにも注意してください。深度0を使用すると、深度の制限がないことを意味します。</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>\c DOT_MULTI_TARGETSタグを\c YESに設定すると、dotが1回の実行で複数の出力ファイルを生成できるようになります（つまり、コマンドラインで複数の-oおよび-Tオプション）。これにより\c dotがより速く動作しますが、\c dotの新しいバージョン（&gt;1.8.10）のみがこれをサポートしているため、この機能はデフォルトで無効になっています。</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>\c GENERATE_LEGENDタグが\c YESに設定されている場合、Doxygenはdotによって生成されたグラフの様々なボックスと矢印の意味を説明する凡例ページを生成します。\note このタグは、\ref cfg_uml_look &quot;UML_LOOK&quot;が設定されていない場合、つまり継承およびコラボレーションダイアグラムにDoxygenの内部グラフィカル表現が使用される場合に必要です。</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>\c DOT_CLEANUPタグが\c YESに設定されている場合、Doxygenは様々なグラフを生成するために使用される中間ファイルを削除します。&lt;br&gt;注意：この設定はdotファイルだけでなく、一時的なmscファイルにも使用されます。</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Doxygenコメント内で\ref cmdmsc &quot;\msc&quot;コマンドを使用してメッセージシーケンスダイアグラムを定義できます。\c MSCGEN_TOOLタグが空白のままにされている場合（デフォルト）、Doxygenはダイアグラムを生成するために組み込みバージョンのmscgenツールを使用します。または、\c MSCGEN_TOOLタグで外部ツールの名前を指定することもできます。例えば、値として`prog`を指定すると、Doxygenはツールを`prog -T &lt;outfile_format&gt; -o &lt;outputfile&gt; &lt;inputfile&gt;`として呼び出します。外部ツールは、出力ファイル形式「png」、「eps」、「svg」、および「ismap」をサポートする必要があります。</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>\c MSCFILE_DIRSタグを使用して、ドキュメントにインクルードされるmscファイルを含む1つ以上のディレクトリを指定できます（\ref cmdmscfile &quot;\mscfile&quot;コマンドを参照）。</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>\c GROUP_GRAPHSタグが\c YESに設定されている場合、Doxygenはグループのグラフを生成し、グループの直接の依存関係を示します。\c GROUP_GRAPHSが\c NOに設定されている場合、グループ依存グラフを明示的に有効にするには、\ref cmdgroupgraph &quot;\groupgraph&quot;コマンドを使用できます。ディレクトリグラフを無効にするには、\ref cmdhidegroupgraph &quot;\hidegroupgraph&quot;コマンドを使用できます。マニュアルの\ref grouping &quot;グループ化&quot;の章も参照してください。</translation>
    </message>
</context>
<context>
    <name>OptionValue</name>
<message>
        <source>(United States)</source>
        <translation>(アメリカ合衆国)</translation>
    </message>
<message>
        <source>(Persian)</source>
        <translation>(ペルシア語)</translation>
    </message>
<message>
        <source>(Japanese with English messages)</source>
        <translation>(英語メッセージ付き日本語)</translation>
    </message>
<message>
        <source>(Korean with English messages)</source>
        <translation>(英語メッセージ付き韓国語)</translation>
    </message>
<message>
        <source>(Farsi)</source>
        <translation>(ファルシ語)</translation>
    </message>
<message>
        <source>use a fixed 'autotoc_md' string followed by a sequence number starting at 0</source>
        <translation>0から始まるシーケンス番号が続く固定の'autotoc_md'文字列を使用</translation>
    </message>
<message>
        <source>use the lower case version of title with any whitespace replaced by '-' and punctuation characters removed</source>
        <translation>タイトルの小文字版を使用し、空白は'-'に置換し、句読点文字は削除</translation>
    </message>
<message>
        <source>(to be provided as Doxygen C comment)</source>
        <translation>(Doxygen Cコメントとして提供)</translation>
    </message>
<message>
        <source>always generates light mode output</source>
        <translation>常にライトモード出力を生成</translation>
    </message>
<message>
        <source>always generates dark mode output</source>
        <translation>常にダークモード出力を生成</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses light mode if no preference is set (the default)</source>
        <translation>ユーザー設定に従ってモードを自動設定、設定がない場合はライトモードを使用(デフォルト)</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses dark mode if no preference is set</source>
        <translation>ユーザー設定に従ってモードを自動設定、設定がない場合はダークモードを使用</translation>
    </message>
<message>
        <source>allows a user to switch between light and dark mode via a button</source>
        <translation>ボタンでライトモードとダークモードを切り替え可能</translation>
    </message>
<message>
        <source>(the default)</source>
        <translation>(デフォルト)</translation>
    </message>
<message>
        <source>(looks nicer but requires the pdf2svg or inkscape tool)</source>
        <translation>(より美しく表示されますが、pdf2svgまたはinkscapeツールが必要)</translation>
    </message>
<message>
        <source>(which is slower, but has the best compatibility. This is the name for Mathjax version 2, for MathJax version 3 this will be translated into \c chtml)</source>
        <translation>(より遅いですが、最高の互換性があります。これはMathJaxバージョン2の名称で、MathJaxバージョン3では\c chtmlに変換されます)</translation>
    </message>
<message>
        <source>(i.e. MathML. Only supported for MathJax 2. For MathJax version 3 \c chtml will be used instead.)</source>
        <translation>(MathML。MathJax 2のみサポート。MathJaxバージョン3では\c chtmlが代わりに使用されます。)</translation>
    </message>
<message>
        <source>(This is the name for Mathjax version 3, for MathJax version 2 this will be translated into \c HTML-CSS)</source>
        <translation>(これはMathJaxバージョン3の名称で、MathJaxバージョン2では\c HTML-CSSに変換されます)</translation>
    </message>
<message>
        <source>(210 x 297 mm)</source>
        <translation>(210 x 297 mm)</translation>
    </message>
<message>
        <source>(8.5 x 11 inches)</source>
        <translation>(8.5 x 11 インチ)</translation>
    </message>
<message>
        <source>(8.5 x 14 inches)</source>
        <translation>(8.5 x 14 インチ)</translation>
    </message>
<message>
        <source>(7.25 x 10.5 inches)</source>
        <translation>(7.25 x 10.5 インチ)</translation>
    </message>
<message>
        <source>same as ERROR_STOP</source>
        <translation>ERROR_STOPと同じ</translation>
    </message>
<message>
        <source>same as BATCH</source>
        <translation>BATCHと同じ</translation>
    </message>
<message>
        <source>In batch mode nothing is printed on the terminal, errors are scrolled as if \&lt;return\&gt; is hit at every error; missing files that TeX tries to input or request from keyboard input (\\read on a not open input stream) cause the job to abort</source>
        <translation>バッチモードでは端末に何も印刷されず、エラーは各エラーで\&lt;return\&gt;が押されたかのようにスクロールされます。TeXが入力またはキーボード入力から要求しようとする欠落ファイル(開かれていない入力ストリームでの\\read)は、ジョブを中止させます</translation>
    </message>
<message>
        <source>In nonstop mode the diagnostic message will appear on the terminal, but there is no possibility of user interaction just like in batch mode</source>
        <translation>非停止モードでは診断メッセージが端末に表示されますが、バッチモードと同様にユーザーとの対話はできません</translation>
    </message>
<message>
        <source>In scroll mode, TeX will stop only for missing files to input or if keyboard input is necessary</source>
        <translation>スクロールモードでは、TeXは入力するファイルが見つからない場合またはキーボード入力が必要な場合にのみ停止します</translation>
    </message>
<message>
        <source>In errorstop mode, TeX will stop at each error, asking for user intervention</source>
        <translation>エラー停止モードでは、TeXは各エラーで停止し、ユーザーの介入を求めます</translation>
    </message>
<message>
        <source>(use client-side rendering for HTML and \c mmdc for LaTeX/PDF and other formats. If \c MERMAID_PATH is not set, non-HTML diagrams will produce a warning)</source>
        <translation>(HTMLにはクライアント側レンダリングを使用し、LaTeX/PDFおよびその他の形式には\c mmdcを使用。\c MERMAID_PATHが設定されていない場合、非HTMLダイアグラムは警告を生成します)</translation>
    </message>
<message>
        <source>(use the \c mmdc tool to pre-generate images (requires \c Node.js and \c mermaid-js/mermaid-cli). Works for all output formats)</source>
        <translation>(\c mmdcツールを使用して画像を事前生成(\c Node.jsと\c mermaid-js/mermaid-cliが必要)。すべての出力形式で動作)</translation>
    </message>
<message>
        <source>(embed \c mermaid.js in HTML output for client-side rendering. Does not require \c mmdc but only works for HTML output)</source>
        <translation>(クライアント側レンダリングのために\c mermaid.jsをHTML出力に埋め込み。\c mmdcは不要ですが、HTML出力でのみ動作)</translation>
    </message>
</context>
</TS>
