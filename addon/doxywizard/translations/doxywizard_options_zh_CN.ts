<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>此标签指定配置文件中所有后续字符使用的编码。默认值为 UTF-8，这也是此标签首次出现之前所有文本使用的编码。Doxygen 使用 \c libiconv（或 \c libc 中内置的 iconv）进行转码。有关可能的编码列表，请参阅 https://www.gnu.org/software/libiconv/。</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>\c PROJECT_NAME 标签是一个单词（除非使用 Doxywizard，否则为用双引号括起来的一系列单词），用于标识生成文档的项目。此名称用于大多数生成页面的标题和其他一些地方。</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>\c PROJECT_NUMBER 标签可用于输入项目或修订号。这对于归档生成的文档或使用版本控制系统时很有用。</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>使用 \c PROJECT_BRIEF 标签可以为项目提供可选的一行描述，该描述显示在每个页面的顶部，应该给查看者一个关于项目目的的快速概念。保持描述简短。</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>使用 \c PROJECT_LOGO 标签可以指定包含在文档中的徽标或图标。徽标的最大高度不应超过 55 像素，最大宽度不应超过 200 像素。Doxygen 会将徽标复制到输出目录。</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>使用 \c PROJECT_ICON 标签可以指定显示 HTML 文档时包含在选项卡中的图标。Doxygen 会将徽标复制到输出目录。</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>\c OUTPUT_DIRECTORY 标签用于指定生成文档的（相对或绝对）路径。如果输入相对路径，它将相对于 Doxygen 启动的位置。如果留空，则使用当前目录。</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>如果 \c CREATE_SUBDIRS 标签设置为 \c YES，Doxygen 将在每个输出格式的输出目录下创建最多 4096 个子目录（2 个级别），并将生成的文件分发到这些目录中。当向 Doxygen 提供大量源文件时，启用此选项很有用，否则将所有生成的文件放在同一目录中可能会导致文件系统的性能问题。调整 \c CREATE_SUBDIRS_LEVEL 以控制子目录的数量。</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>控制当 \c CREATE_SUBDIRS 标签设置为 \c YES 时创建的子目录数量。级别 0 表示 16 个目录，每个级别增量使目录数量翻倍，在级别 8 时结果为 4096 个目录，这是默认值也是最大值。子目录分为 2 个级别组织，第一级始终有固定的 16 个目录。</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>如果 \c ALLOW_UNICODE_NAMES 标签设置为 \c YES，Doxygen 将允许非 ASCII 字符出现在生成文件的名称中。如果设置为 \c NO，非 ASCII 字符将被转义，例如 _xE3_x81_x84 将用于 Unicode U+3044。</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>\c OUTPUT_LANGUAGE 标签用于指定 Doxygen 生成的所有文档使用的语言。Doxygen 将使用此信息以正确的语言生成所有常量输出。</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>如果 \c BRIEF_MEMBER_DESC 标签设置为 \c YES，Doxygen 将在文件和类文档中包含简要成员描述（类似于 \c Javadoc）。设置为 \c NO 以禁用此功能。</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>如果 \c REPEAT_BRIEF 标签设置为 \c YES，Doxygen 将在详细描述之前添加成员或函数的简要描述。&lt;br&gt;注意：如果 \ref cfg_hide_undoc_members &quot;HIDE_UNDOC_MEMBERS&quot; 和 \ref cfg_brief_member_desc &quot;BRIEF_MEMBER_DESC&quot; 都设置为 \c NO，简要描述将被完全抑制。</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>此标签实现了一个准智能简要描述缩写器，用于形成各种列表中的文本。此列表中的每个字符串，如果作为简要描述的前导文本找到，将从文本中剥离，处理整个列表后的结果用作注释文本。否则，简要描述按原样使用。如果留空，则使用以下值（`$name` 自动替换为实体名称）：</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>如果 \c ALWAYS_DETAILED_SEC 和 \ref cfg_repeat_brief &quot;REPEAT_BRIEF&quot; 标签都设置为 \c YES，那么即使只有简要描述，Doxygen 也会生成详细部分。</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>如果 \c INLINE_INHERITED_MEMB 标签设置为 \c YES，Doxygen 将在类的文档中显示所有继承的成员，就像这些成员是普通类成员一样。基类的构造函数、析构函数和赋值运算符将不会显示。</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>如果 \c FULL_PATH_NAMES 标签设置为 \c YES，Doxygen 将在文件列表和头文件中添加完整路径前缀。如果设置为 \c NO，将使用使文件名唯一的最短路径。</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>\c STRIP_FROM_PATH 标签可用于剥离用户定义的部分路径。只有当指定的字符串之一与路径的左侧部分匹配时，才会进行剥离。该标签可用于在文件列表中显示相对路径。如果留空，则使用 Doxygen 运行的目录作为要剥离的路径。&lt;br&gt;请注意，您可以在此处指定绝对路径，也可以指定相对路径，这些路径将相对于 Doxygen 启动的目录。</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>\c STRIP_FROM_INC_PATH 标签可用于剥离类的文档中提到的路径的用户定义部分，该路径告诉读者应该包含哪个头文件才能使用类。如果留空，则只使用包含类定义的头文件的名称。否则，应该指定通常使用 `-I` 标志传递给编译器的包含路径列表。</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>如果 \c SHORT_NAMES 标签设置为 \c YES，Doxygen 将生成更短（但可读性较差）的文件名。如果您的文件系统不支持长名称（如 DOS、Mac 或 CD-ROM），这可能很有用。</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>如果 \c JAVADOC_AUTOBRIEF 标签设置为 \c YES，Doxygen 将把 Javadoc 风格注释的第一行（直到第一个句点、问号或感叹号）解释为简要描述。如果设置为 \c NO，Javadoc 风格将表现得像普通的 Qt 风格注释（因此需要显式的 \ref cmdbrief &quot;\@brief&quot; 命令来获得简要描述。）</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>如果 \c JAVADOC_BANNER 标签设置为 \c YES，Doxygen 将把像 \verbatim /***************\endverbatim 这样的行解释为 Javadoc 风格注释&quot;横幅&quot;的开始。如果设置为 \c NO，Javadoc 风格将表现得像普通注释，Doxygen 不会解释它。</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>如果 \c QT_AUTOBRIEF 标签设置为 \c YES，Doxygen 将把 Qt 风格注释的第一行（直到第一个句点、问号或感叹号）解释为简要描述。如果设置为 \c NO，Qt 风格将表现得像普通的 Qt 风格注释（因此需要显式的 \ref cmdbrief &quot;\brief&quot; 命令来获得简要描述。）</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>\c MULTILINE_CPP_IS_BRIEF 标签可以设置为 \c YES，使 Doxygen 将多行 C++ 特殊注释块（即 \c //! 或 \c /// 注释块）视为简要描述。这曾经是默认行为。新的默认设置是将多行 C++ 注释块视为详细描述。如果您更喜欢旧的行为，请将此标签设置为 \c YES。&lt;br&gt;请注意，将此标签设置为 \c YES 也意味着不再识别 Rational Rose 注释。</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>默认情况下，Python 文档字符串显示为预格式化文本，不能使用 Doxygen 的特殊命令。通过将 \c PYTHON_DOCSTRING 设置为 \c NO，可以使用 Doxygen 的特殊命令，并且文档字符串文档块的内容显示为 Doxygen 文档。</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>如果 \c INHERIT_DOCS 标签设置为 \c YES，则未记录的成员将继承其重新实现的任何已记录成员的文档。</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>如果 \c SEPARATE_MEMBER_PAGES 标签设置为 \c YES，Doxygen 将为每个成员生成一个新页面。如果设置为 \c NO，成员的文档将成为包含它的文件/类/命名空间的一部分。</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>\c TAB_SIZE 标签可用于设置制表符中的空格数。Doxygen 使用此值在代码片段中用空格替换制表符。</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>此标签可用于指定多个在文档中充当命令的别名。别名的形式为：\verbatim name=value\endverbatim 例如添加 \verbatim &quot;sideeffect=@par Side Effects:^^&quot;\endverbatim 将允许您在文档中放置命令 \c \sideeffect（或 \c \@sideeffect），这将产生一个带有标题&quot;Side Effects:&quot;的用户定义段落。请注意，您不能在别名的值部分放置 \ref cmdn &quot;\n&quot; 来插入换行符（在结果输出中）。您可以在别名的值部分放置 `^^` 来插入换行符，就像原始文件中有物理换行符一样。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>如果您的项目仅包含 C 源代码，请将 \c OPTIMIZE_OUTPUT_FOR_C 标签设置为 \c YES。Doxygen 随后将生成更适合 C 的输出。例如，将使用一些不同的名称。将省略所有成员的列表等。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>如果您的项目仅包含 Java 或 Python 源代码，请将 \c OPTIMIZE_OUTPUT_JAVA 标签设置为 \c YES。Doxygen 随后将生成更适合该语言的输出。例如，命名空间将显示为包，限定作用域将看起来不同等。</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>如果您的项目包含 Fortran 源代码，请将 \c OPTIMIZE_FOR_FORTRAN 标签设置为 \c YES。Doxygen 随后将生成适合 Fortran 的输出。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>如果您的项目包含 VHDL 源代码，请将 \c OPTIMIZE_OUTPUT_VHDL 标签设置为 \c YES。Doxygen 随后将生成适合 VHDL 的输出。</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>如果您的项目仅包含 Slice 源代码，请将 \c OPTIMIZE_OUTPUT_SLICE 标签设置为 \c YES。Doxygen 随后将生成更适合该语言的输出。例如，命名空间将显示为模块，类型将分成更多组等。</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen 根据解析文件的扩展名选择要使用的解析器。使用此标签，您可以为给定扩展名分配要使用的解析器。Doxygen 有内置映射，但您可以使用此标签覆盖或扩展它。格式为 &lt;code&gt;ext=language&lt;/code&gt;，其中 \c ext 是文件扩展名，language 是 Doxygen 支持的解析器之一：IDL、Java、JavaScript、Csharp (C#)、C、C++、Lex、D、PHP、md (Markdown)、Objective-C、Python、Slice、VHDL、Fortran（固定格式 Fortran：FortranFixed，自由格式 Fortran：FortranFree，未知格式 Fortran：Fortran。在后一种情况下，解析器尝试猜测代码是固定格式还是自由格式代码，这是 Fortran 类型文件的默认设置）。例如，要使 Doxygen 将 &lt;code&gt;.inc&lt;/code&gt; 文件视为 Fortran 文件（默认为 PHP），将 &lt;code&gt;.f&lt;/code&gt; 文件视为 C 文件（默认为 Fortran），请使用：`inc=Fortran f=C`。&lt;br&gt;注意：对于没有扩展名的文件，您可以使用 `no_extension` 作为占位符。&lt;br&gt;请注意，对于自定义扩展名，您还需要设置 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;，否则 Doxygen 不会读取这些文件。指定 `no_extension` 时，应将 `*` 添加到 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;。&lt;br&gt;注意，另请参阅 \ref default_file_extension_mapping &quot;默认文件扩展名映射&quot;列表。</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>如果启用了 \c MARKDOWN_SUPPORT 标签，Doxygen 将根据 Markdown 格式预处理所有注释，这允许更可读的文档。有关详细信息，请参阅 https://daringfireball.net/projects/markdown/。Markdown 处理的输出进一步由 Doxygen 处理，因此您可以将 Doxygen、HTML 和 XML 命令与 Markdown 格式混合使用。仅在向后兼容性问题时禁用。</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>如果启用了 \c MARKDOWN_STRICT 标签，Doxygen 将在 Doxygen 的本机标记格式与 Markdown 冲突的情况下，也将注释中的文本视为 Markdown 格式。这仅在使用反引号的情况下相关。Doxygen 的本机标记样式允许单引号关闭以反引号开始的文本片段，然后将其视为引用文本片段，而在 Markdown 中，此类文本片段被视为逐字文本，仅在找到第二个匹配的反引号时才关闭。此外，Doxygen 的本机标记格式要求在反引号部分中出现双引号时进行转义，而 Markdown 不需要这样做。</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>当 \c TOC_INCLUDE_HEADINGS 标签设置为非零值时，直到该级别的所有标题都会自动包含在目录中，即使它们没有 id 属性。\note 此功能目前仅适用于 Markdown 标题。</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>\c MARKDOWN_ID_STYLE 标签可用于指定用于为 Markdown 标题生成标识符的算法。注意：每个标识符都是唯一的。</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>启用后，Doxygen 尝试将对应于已记录类或命名空间的单词链接到其相应的文档。可以通过在单词前放置 \c % 符号来防止个别情况下的此类链接，或者通过将 \c AUTOLINK_SUPPORT 设置为 \c NO 来全局防止。\c AUTOLINK_IGNORE_WORDS 标签中列出的单词被排除在自动链接之外。</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>此标签指定一个单词列表，当与文档中单词的开头匹配时，如果通过 AUTOLINK_SUPPORT 启用了自动链接生成，将抑制自动链接生成。此列表不影响使用 \# 或 \ref cmdlink &quot;\link&quot; 或 \ref cmdref &quot;\ref&quot; 命令显式创建的链接。</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>如果您使用 STL 类（即 `std::string`、`std::vector` 等），但不想将 STL 源代码的（标签文件）作为输入包含，则应将此标签设置为 \c YES，以便让 Doxygen 匹配参数包含 STL 类的函数声明和定义（例如 `func(std::string`); 与 `func(std::string) {}`）。这也使涉及 STL 类的继承和协作图更加完整和准确。</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>如果您使用 Microsoft 的 C++/CLI 语言，应将此选项设置为 \c YES 以启用解析支持。</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>如果您的项目仅包含 &lt;a href=&quot;https://www.riverbankcomputing.com/software&quot;&gt;sip&lt;/a&gt; 源代码，请将 \c SIP_SUPPORT 标签设置为 \c YES。Doxygen 将像普通 C++ 一样解析它们，但会假设所有类在没有显式保护关键字时都使用公共继承而不是私有继承。</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>对于 Microsoft 的 IDL，有 \c propget 和 \c propput 属性来指示属性的 getter 和 setter 方法。将此选项设置为 \c YES 将使 Doxygen 在文档中用属性替换 get 和 set 方法。这仅在方法确实是获取或设置简单类型时才有效。如果不是这种情况，或者您想无论如何显示方法，应将此选项设置为 \c NO。</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>如果在文档中使用了成员分组，并且 \c DISTRIBUTE_GROUP_DOC 标签设置为 \c YES，那么 Doxygen 将为组中的其他成员重用组中第一个成员的文档（如果有）。默认情况下，组的所有成员必须显式记录。</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>如果将结构体或类添加到组中并启用了此选项，则任何嵌套的类或结构体也会添加到同一组中。默认情况下，此选项被禁用，必须通过 \ref cmdingroup &quot;\ingroup&quot; 显式添加嵌套复合类型。</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>将 \c SUBGROUPING 标签设置为 \c YES 以允许相同类型的类成员组（例如一组公共函数）作为该类型的子组放置（例如在公共函数部分下）。将其设置为 \c NO 以防止子组化。或者，可以使用 \ref cmdnosubgrouping &quot;\nosubgrouping&quot; 命令按类执行此操作。</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>当 \c INLINE_GROUPED_CLASSES 标签设置为 \c YES 时，类、结构体和联合体显示在包含它们的组中（例如使用 \ref cmdingroup &quot;\ingroup&quot;），而不是在单独的页面上（对于 HTML 和 Man 页面）或部分中（对于 \f$\mbox{\LaTeX}\f$ 和 RTF）。&lt;br&gt;请注意，此功能不能与 \ref cfg_separate_member_pages &quot;SEPARATE_MEMBER_PAGES&quot; 结合使用。</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>当 \c INLINE_SIMPLE_STRUCTS 标签设置为 \c YES 时，仅具有公共数据字段或简单 typedef 字段的结构体、类和联合体将显示在定义它们的作用域的文档中（即文件、命名空间或组文档），前提是此作用域已记录。如果设置为 \c NO，结构体、类和联合体将显示在单独的页面上（对于 HTML 和 Man 页面）或部分中（对于 \f$\mbox{\LaTeX}\f$ 和 RTF）。</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>启用 \c TYPEDEF_HIDES_STRUCT 标签后，结构体、联合体或枚举的 typedef 将记录为具有 typedef 名称的结构体、联合体或枚举。因此 &lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt; 将在文档中显示为名为 \c TypeT 的结构体。禁用时，typedef 将显示为文件、命名空间或类的成员。结构体将命名为 \c TypeS。这通常对于 C 代码很有用，如果编码约定要求所有复合类型都是 typedef 的，并且只引用 typedef，从不引用标签名称。</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>可以使用 \c LOOKUP_CACHE_SIZE 设置符号查找缓存的大小。此缓存用于根据符号的名称和作用域解析符号。由于这可能是一个昂贵的过程，并且通常相同的符号在代码中出现多次，Doxygen 会保留一个预解析符号的缓存。如果缓存太小，Doxygen 会变慢。如果缓存太大，会浪费内存。缓存大小由以下公式给出：\f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$。有效范围为 0..9，默认值为 0，对应于缓存大小 \f$2^{16} = 65536\f$ 个符号。在运行结束时，Doxygen 将报告缓存使用情况并从速度角度建议最佳缓存大小。</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>\c NUM_PROC_THREADS 指定 Doxygen 在处理期间允许使用的线程数。设置为 \c 0 时，Doxygen 将根据系统中可用的核心数进行设置。您可以将其显式设置为大于 0 的值，以更好地控制 CPU 负载和处理速度之间的平衡。目前只有输入处理可以使用多线程完成。由于这仍然是一个实验性功能，默认值设置为 1，这实际上禁用了并行处理。请报告您遇到的任何问题。并行生成 dot 图由 \c DOT_NUM_THREADS 设置控制。</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>如果 \c TIMESTAMP 标签设置为不同于 \c NO 的值，则每个生成的页面将包含页面生成时的日期或日期和时间。将此设置为 \c NO 有助于比较多次运行的输出。</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>如果 \c EXTRACT_ALL 标签设置为 \c YES，Doxygen 将假设文档中的所有实体都已记录，即使没有可用的文档。私有类成员和静态文件成员将被隐藏，除非分别设置了 \ref cfg_extract_private &quot;EXTRACT_PRIVATE&quot; 或 \ref cfg_extract_static &quot;EXTRACT_STATIC&quot; 标签为 \c YES。\note 这也将禁用通常在 \ref cfg_warnings &quot;WARNINGS&quot; 设置为 \c YES 时产生的有关未记录成员的警告。</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>如果 \c EXTRACT_PRIVATE 标签设置为 \c YES，类的所有私有成员将包含在文档中。</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>如果 \c EXTRACT_PRIV_VIRTUAL 标签设置为 \c YES，类的已记录私有虚拟方法将包含在文档中。</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>如果 \c EXTRACT_PACKAGE 标签设置为 \c YES，所有具有包或内部作用域的成员将包含在文档中。</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>如果 \c EXTRACT_STATIC 标签设置为 \c YES，文件的所有静态成员将包含在文档中。</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>如果 \c EXTRACT_LOCAL_CLASSES 标签设置为 \c YES，在源文件中本地定义的类（和结构体）将包含在文档中。如果设置为 \c NO，则只包含在头文件中定义的类。对 Java 源代码没有任何影响。</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>此标志仅对 Objective-C 代码有用。如果设置为 \c YES，在实现部分定义但不在接口中的本地方法将包含在文档中。如果设置为 \c NO，则只包含接口中的方法。</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>如果此标志设置为 \c YES，匿名命名空间的成员将被提取并在文档中显示为名为 &#x27;anonymous_namespace{file}&#x27; 的命名空间，其中 file 将替换为包含匿名命名空间的文件的基本名称。默认情况下，匿名命名空间是隐藏的。</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>如果此标志设置为 \c YES，声明中未命名参数的名称将由相应的定义确定。默认情况下，未命名参数在输出中保持未命名。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>如果 \c HIDE_UNDOC_MEMBERS 标签设置为 \c YES，Doxygen 将隐藏已记录类或文件中的所有未记录成员。如果设置为 \c NO，这些成员将包含在各种概述中，但不生成文档部分。如果启用了 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;，此选项无效。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>如果 \c HIDE_UNDOC_CLASSES 标签设置为 \c YES，Doxygen 将隐藏类层次结构中通常可见的所有未记录类。如果设置为 \c NO，这些类将包含在各种概述中。如果启用，此选项还将隐藏未记录的 C++ 概念。如果启用了 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;，此选项无效。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>如果 \c HIDE_UNDOC_NAMESPACES 标签设置为 \c YES，Doxygen 将隐藏命名空间层次结构中通常可见的所有未记录命名空间。如果设置为 \c NO，这些命名空间将包含在各种概述中。如果启用了 \ref cfg_extract_all &quot;EXTRACT_ALL&quot;，此选项无效。</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>如果 \c HIDE_FRIEND_COMPOUNDS 标签设置为 \c YES，Doxygen 将隐藏所有友元声明。如果设置为 \c NO，这些声明将包含在文档中。</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>如果 \c HIDE_IN_BODY_DOCS 标签设置为 \c YES，Doxygen 将隐藏在函数体内找到的任何文档块。如果设置为 \c NO，这些块将附加到函数的详细文档块。</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>\c INTERNAL_DOCS 标签确定是否包含在 \ref cmdinternal &quot;\internal&quot; 命令后键入的文档。如果标签设置为 \c NO，则文档将被排除。将其设置为 \c YES 以包含内部文档。</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>通过正确设置选项 \c CASE_SENSE_NAMES，Doxygen 将能够更好地匹配底层文件系统的功能。如果文件系统区分大小写（即它支持同一目录中名称仅大小写不同的文件），则必须将选项设置为 \c YES 以正确处理输入中出现的此类文件。对于不区分大小写的文件系统，应将选项设置为 \c NO 以正确处理为仅大小写不同的符号编写的输出文件，例如对于两个类，一个名为 \c CLASS，另一个名为 \c Class，并且还支持引用文件而无需指定完全匹配的大小写。在 Windows（包括 Cygwin）和 macOS 上，用户通常应将此选项设置为 \c NO，而在 Linux 或其他 Unix 系统上，通常应将其设置为 \c YES。</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>如果 \c HIDE_SCOPE_NAMES 标签设置为 \c NO，Doxygen 将在文档中显示具有完整类和命名空间作用域的成员。如果设置为 \c YES，作用域将被隐藏。</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>如果 \c HIDE_COMPOUND_REFERENCE 标签设置为 \c NO（默认），Doxygen 将在页面标题后附加额外的文本，例如类参考。如果设置为 \c YES，复合引用将被隐藏。</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>如果 \c SHOW_HEADERFILE 标签设置为 \c YES，则类的文档将显示需要包含哪个文件才能使用该类。</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>如果 \c SHOW_INCLUDE_FILES 标签设置为 \c YES，Doxygen 将在文件的文档中放置该文件包含的文件列表。</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>如果 \c SHOW_GROUPED_MEMB_INC 标签设置为 \c YES，Doxygen 将为每个分组成员添加一个包含语句到文档中，告诉读者需要包含哪个文件才能使用该成员。</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>如果 \c FORCE_LOCAL_INCLUDES 标签设置为 \c YES，Doxygen 将在文档中列出使用双引号而不是尖括号的包含文件。</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>如果 \c INLINE_INFO 标签设置为 \c YES，则会在内联成员的文档中插入 [inline] 标记。</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>如果 \c SORT_MEMBER_DOCS 标签设置为 \c YES，Doxygen 将按成员名称字母顺序对文件和类成员的（详细）文档进行排序。如果设置为 \c NO，成员将按声明顺序出现。</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>如果 \c SORT_BRIEF_DOCS 标签设置为 \c YES，Doxygen 将按成员名称字母顺序对文件、命名空间和类成员的简要描述进行排序。如果设置为 \c NO，成员将按声明顺序出现。请注意，这也会影响类列表中类的顺序。</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>如果 \c SORT_MEMBERS_CTORS_1ST 标签设置为 \c YES，Doxygen 将对类成员的（简要和详细）文档进行排序，使构造函数和析构函数首先列出。如果设置为 \c NO，构造函数将按 \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot; 和 \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot; 定义的各自顺序出现。\note 如果 \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot; 设置为 \c NO，则在排序简要成员文档时忽略此选项。\note 如果 \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot; 设置为 \c NO，则在排序详细成员文档时忽略此选项。</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>如果 \c SORT_GROUP_NAMES 标签设置为 \c YES，Doxygen 将按字母顺序对组名称的层次结构进行排序。如果设置为 \c NO，组名称将按其定义顺序出现。</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>如果 \c SORT_BY_SCOPE_NAME 标签设置为 \c YES，类列表将按完全限定名称（包括命名空间）排序。如果设置为 \c NO，类列表将仅按类名排序，不包括命名空间部分。\note 如果 \ref cfg_hide_scope_names &quot;HIDE_SCOPE_NAMES&quot; 设置为 \c YES，此选项不太有用。\note 此选项仅适用于类列表，不适用于字母列表。</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>如果启用了 \c STRICT_PROTO_MATCHING 选项，并且 Doxygen 无法正确解析函数的所有参数类型，它将拒绝成员函数的原型和实现之间的匹配，即使只有一个候选者或者通过简单的字符串匹配很明显应该选择哪个候选者。通过禁用 \c STRICT_PROTO_MATCHING，Doxygen 仍将在这种情况下接受原型和实现之间的匹配。</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>\c GENERATE_TODOLIST 标签可用于启用（\c YES）或禁用（\c NO）待办事项列表。此列表是通过在文档中放置 \ref cmdtodo &quot;\todo&quot; 命令创建的。</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>\c GENERATE_TESTLIST 标签可用于启用（\c YES）或禁用（\c NO）测试列表。此列表是通过在文档中放置 \ref cmdtest &quot;\test&quot; 命令创建的。</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>\c GENERATE_BUGLIST 标签可用于启用（\c YES）或禁用（\c NO）错误列表。此列表是通过在文档中放置 \ref cmdbug &quot;\bug&quot; 命令创建的。</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>\c GENERATE_DEPRECATEDLIST 标签可用于启用（\c YES）或禁用（\c NO）弃用列表。此列表是通过在文档中放置 \ref cmddeprecated &quot;\deprecated&quot; 命令创建的。</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>\c GENERATE_REQUIREMENTS 标签可用于启用（\c YES）或禁用（\c NO）需求页面。启用后，当输入中至少出现一个带有 \ref cmdrequirement &quot;\requirement&quot; 命令的注释块时，此页面会自动创建。</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>\c REQ_TRACEABILITY_INFO 标签控制是否在需求页面上显示可追溯性信息（仅在使用 \ref cmdrequirement &quot;\requirement&quot; 注释块时相关）。设置 \c NO 将完全禁用可追溯性信息。设置 \c UNSATISFIED_ONLY 将显示缺少满足关系（通过命令：\ref cmdsatisfies &quot;\satisfies&quot;）的需求列表。类似地，设置 \c UNVERIFIED_ONLY 将显示缺少验证关系（通过命令：\ref cmdverifies &quot;\verifies&quot;）的需求列表。将标签设置为 \c YES（默认）将在适用时显示两个列表。</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>\c ENABLED_SECTIONS 标签可用于启用条件文档部分，由 \ref cmdif &quot;\if&quot; \&lt;section_label\&gt; ... \ref cmdendif &quot;\endif&quot; 和 \ref cmdcond &quot;\cond&quot; \&lt;section_label\&gt; ... \ref cmdendcond &quot;\endcond&quot; 块标记。</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>\c MAX_INITIALIZER_LINES 标签确定变量或宏/定义的初始值可以显示的最大行数，以便它出现在文档中。如果初始化器包含的行数超过此处指定的数量，它将被隐藏。使用值 0 可完全隐藏初始化器。无论此设置如何，都可以在文档中使用 \ref cmdshowinitializer &quot;\showinitializer&quot; 或 \ref cmdhideinitializer &quot;\hideinitializer&quot; 命令控制单个变量和宏/定义的值的外观。</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>将 \c SHOW_USED_FILES 标签设置为 \c NO 以禁用类和结构体文档底部生成的文件列表。如果设置为 \c YES，列表将提及用于生成文档的文件。</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>将 \c SHOW_FILES 标签设置为 \c NO 以禁用文件页面的生成。这将从快速索引和文件夹树视图（如果指定）中删除文件条目。</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>将 \c SHOW_NAMESPACES 标签设置为 \c NO 以禁用命名空间页面的生成。这将从快速索引和文件夹树视图（如果指定）中删除命名空间条目。</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>\c FILE_VERSION_FILTER 标签可用于指定 Doxygen 应调用的程序或脚本，以获取每个文件的当前版本（通常来自版本控制系统）。Doxygen 将通过执行（通过 &lt;code&gt;popen()&lt;/code&gt;）命令 &lt;code&gt;command input-file&lt;/code&gt; 来调用程序，其中 \c command 是 \c FILE_VERSION_FILTER 标签的值，\c input-file 是 Doxygen 提供的输入文件的名称。程序写入标准输出的任何内容都用作文件版本。</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>\c LAYOUT_FILE 标签可用于指定 Doxygen 将解析的布局文件。布局文件以独立于输出格式的方式控制生成输出文件的全局结构。要创建表示 Doxygen 默认值的布局文件，请使用 `-l` 选项运行 Doxygen。您可以选择在选项后指定文件名，如果省略，则使用 \c DoxygenLayout.xml 作为布局文件的名称。另请参阅第 \ref layout 节以获取信息。&lt;br&gt;请注意，如果您从包含名为 \c DoxygenLayout.xml 的文件的目录运行 Doxygen，即使 \c LAYOUT_FILE 标签为空，Doxygen 也会自动解析它。</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>\c CITE_BIB_FILES 标签可用于指定一个或多个包含参考定义的 \c bib 文件。这必须是 &lt;code&gt;.bib&lt;/code&gt; 文件列表。如果省略，会自动附加 &lt;code&gt;.bib&lt;/code&gt; 扩展名。这需要安装 \c bibtex 工具。另请参阅 https://en.wikipedia.org/wiki/BibTeX 以获取更多信息。对于 \f$\mbox{\LaTeX}\f$，可以使用 \ref cfg_latex_bib_style &quot;LATEX_BIB_STYLE&quot; 控制参考文献的样式。要使用此功能，您需要在搜索路径中有 \c bibtex 和 \c perl 可用。另请参阅 \ref cmdcite &quot;\cite&quot; 以获取有关如何创建引用的信息。</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>\c EXTERNAL_TOOL_PATH 标签可用于扩展搜索路径（PATH 环境变量），以便可以找到 \c latex 和 \c gs 等外部工具。\note 使用 EXTERNAL_TOOL_PATH 指定的目录添加到 PATH 变量已指定的路径前面，并按指定的顺序添加。\note 此选项对于 macOS 版本 14 (Sonoma) 及更高版本特别有用，当从 Doxywizard 运行 Doxygen 时，因为在这种情况下，PATH 的任何用户定义更改都会被忽略。macOS 上的一个典型示例是设置 \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim 与标准路径一起，Doxygen 在启动外部工具时使用的完整搜索路径将变为 \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>\c QUIET 标签可用于打开/关闭 Doxygen 生成到标准输出的消息。如果 \c QUIET 设置为 \c YES，这意味着消息已关闭。</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>\c WARNINGS 标签可用于打开/关闭 Doxygen 生成到标准错误（\c stderr）的警告消息。如果 \c WARNINGS 设置为 \c YES，这意味着警告已打开。&lt;br&gt;\b 提示：在编写文档时打开警告。</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>如果 \c WARN_IF_UNDOCUMENTED 标签设置为 \c YES，Doxygen 将为未记录的成员生成警告。如果 \ref cfg_extract_all &quot;EXTRACT_ALL&quot; 设置为 \c YES，则此标志将自动禁用。</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>如果 \c WARN_IF_DOC_ERROR 标签设置为 \c YES，Doxygen 将为文档中的潜在错误生成警告，例如在已记录的函数中两次记录某些参数，或记录不存在的参数或错误使用标记命令。</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>如果 \c WARN_IF_INCOMPLETE_DOC 设置为 \c YES，Doxygen 将警告不完整的函数参数文档。如果设置为 \c NO，Doxygen 将接受某些参数没有文档而不发出警告。</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>可以启用此 \c WARN_NO_PARAMDOC 选项以获取已记录但没有参数或返回值文档的函数的警告。如果设置为 \c NO，Doxygen 只会警告错误的参数文档，而不会警告缺少文档。如果 \ref cfg_extract_all &quot;EXTRACT_ALL&quot; 设置为 \c YES，则此标志将自动禁用。另请参阅 \ref cfg_warn_if_incomplete_doc &quot;WARN_IF_INCOMPLETE_DOC&quot;</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>如果 \c WARN_IF_UNDOC_ENUM_VAL 选项设置为 \c YES，Doxygen 将警告未记录的枚举值。如果设置为 \c NO，Doxygen 将接受未记录的枚举值。如果 \ref cfg_extract_all &quot;EXTRACT_ALL&quot; 设置为 \c YES，则此标志将自动禁用。</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>如果 \c WARN_LAYOUT_FILE 选项设置为 \c YES，Doxygen 将警告在解析用户定义的布局文件时发现的问题，例如缺少或错误的元素。另请参阅 \ref cfg_layout_file &quot;LAYOUT_FILE&quot; 以获取详细信息。如果设置为 \c NO，布局文件的问题将被抑制。</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>如果 \c WARN_AS_ERROR 标签设置为 \c YES，则 Doxygen 将在遇到警告时立即停止。如果 \c WARN_AS_ERROR 标签设置为 \c FAIL_ON_WARNINGS，则 Doxygen 将继续运行，就像 \c WARN_AS_ERROR 标签设置为 \c NO 一样，但在 Doxygen 过程结束时，Doxygen 将返回非零状态。如果 \c WARN_AS_ERROR 标签设置为 \c FAIL_ON_WARNINGS_PRINT，则 Doxygen 的行为类似于 \c FAIL_ON_WARNINGS，但在未定义 \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot; 的情况下，Doxygen 不会在其他消息之间写入警告消息，而是在运行结束时写入它们，如果定义了 \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot;，警告消息除了在定义的文件中外，还会在运行结束时显示，除非 \ref cfg_warn_logfile &quot;WARN_LOGFILE&quot; 定义为 `-` 即标准输出（`stdout`），在这种情况下，行为将保持为设置 \c FAIL_ON_WARNINGS。</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>\c WARN_FORMAT 标签确定 Doxygen 可以生成的警告消息的格式。字符串应包含 &lt;code&gt;\$file&lt;/code&gt;、&lt;code&gt;\$line&lt;/code&gt; 和 &lt;code&gt;\$text&lt;/code&gt; 标签，它们将被警告起源的文件和行号以及警告文本替换。可选地，格式可以包含 &lt;code&gt;$version&lt;/code&gt;，它将被文件的版本替换（如果可以通过 \ref cfg_file_version_filter &quot;FILE_VERSION_FILTER&quot; 获取）\sa \ref cfg_warn_line_format &quot;WARN_LINE_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>在 \ref cfg_warn_format &quot;WARN_FORMAT&quot; 命令的 `$text` 部分中，可能会给出对更具体位置的引用。为了更容易跳转到此位置（在 Doxygen 之外），用户可以定义自定义的&quot;剪切&quot;/&quot;粘贴&quot;字符串。示例：\verbatim WARN_LINE_FORMAT = &quot;&#x27;vi $file +$line&#x27;&quot; \endverbatim \sa \ref cfg_warn_format &quot;WARN_FORMAT&quot;</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>\c WARN_LOGFILE 标签可用于指定应将警告和错误消息写入的文件。如果留空，输出将写入标准错误（`stderr`）。如果无法打开指定的文件进行写入，警告和错误消息将写入标准错误。当指定文件 `-` 时，警告和错误消息将写入标准输出（`stdout`）。</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>\c INPUT 标签用于指定包含已记录源文件的文件和/或目录。您可以输入文件名如 \c myfile.cpp 或目录如 \c /usr/src/myproject。用空格分隔文件或目录。另请参阅 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot; 和 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot; \note 如果此标签为空，则搜索当前目录。</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>此标签可用于指定 Doxygen 解析的源文件的字符编码。Doxygen 内部使用 UTF-8 编码。Doxygen 使用 `libiconv`（或 `libc` 内置的 `iconv`）进行转码。请参阅 &lt;a href=&quot;https://www.gnu.org/software/libiconv/&quot;&gt;libiconv 文档&lt;/a&gt;以获取可能的编码列表。\sa \ref cfg_input_file_encoding &quot;INPUT_FILE_ENCODING&quot;</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>此标签可用于指定 Doxygen 解析的源文件的字符编码。\c INPUT_FILE_ENCODING 标签可用于基于每个文件模式指定字符编码。Doxygen 将文件名与每个模式进行比较，如果有匹配，则应用编码而不是默认的 \ref cfg_input_encoding &quot;INPUT_ENCODING&quot;。字符编码是以下形式的列表：pattern=encoding（如 `*.php=ISO-8859-1`）。\sa \ref cfg_input_encoding &quot;INPUT_ENCODING&quot; 以获取有关支持编码的更多信息。</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>如果 \ref cfg_input &quot;INPUT&quot; 标签的值包含目录，则可以使用 \c FILE_PATTERNS 标签指定一个或多个通配符模式（如 `*.cpp` 和 `*.h`）来过滤目录中的源文件。&lt;br&gt;请注意，对于自定义扩展名或未直接支持的扩展名，您还需要为扩展名设置 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;，否则 Doxygen 不会读取这些文件。&lt;br&gt;请注意，默认选中的文件模式列表可能与 \ref default_file_extension_mapping &quot;默认文件扩展名映射&quot;列表不同。&lt;br&gt;如果留空，则测试以下模式：</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>\c RECURSIVE 标签可用于指定是否也应搜索输入文件的子目录。</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>\c EXCLUDE 标签可用于指定应从 \ref cfg_input &quot;INPUT&quot; 源文件中排除的文件和/或目录。这样，您可以轻松地从根目录使用 \ref cfg_input &quot;INPUT&quot; 标签指定的目录树中排除子目录。&lt;br&gt;请注意，相对路径相对于运行 Doxygen 的目录。</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>\c EXCLUDE_SYMLINKS 标签可用于选择是否从输入中排除作为符号链接（Unix 文件系统功能）的文件或目录。</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>如果 \ref cfg_input &quot;INPUT&quot; 标签的值包含目录，则可以使用 \c EXCLUDE_PATTERNS 标签指定一个或多个通配符模式，以从这些目录中排除某些文件。&lt;br&gt;请注意，通配符与具有绝对路径的文件匹配，因此要排除所有测试目录，例如使用模式 `*``/test/``*`</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>\c EXCLUDE_SYMBOLS 标签可用于指定一个或多个应从输出中排除的符号名称（命名空间、类、函数等）。符号名称可以是完全限定名称、单词，或者如果使用通配符 `*`，则可以是子字符串。示例：`ANamespace`、`AClass`、`ANamespace::AClass`、`ANamespace::*Test`</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>\c EXAMPLE_PATH 标签可用于指定一个或多个包含示例代码片段的文件或目录，这些代码片段被包含（请参阅 \ref cmdinclude &quot;\include&quot; 命令）。</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>如果 \ref cfg_example_path &quot;EXAMPLE_PATH&quot; 标签的值包含目录，则可以使用 \c EXAMPLE_PATTERNS 标签指定一个或多个通配符模式（如 `*.cpp` 和 `*.h`）来过滤目录中的源文件。如果留空，则包含所有文件。</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>如果 \c EXAMPLE_RECURSIVE 标签设置为 \c YES，则无论 \ref cfg_recursive &quot;RECURSIVE&quot; 标签的值如何，都将搜索子目录以查找与 \ref cmdinclude &quot;\include&quot; 或 \ref cmddontinclude &quot;\dontinclude&quot; 命令一起使用的输入文件。</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>\c IMAGE_PATH 标签可用于指定一个或多个包含要包含在文档中的图像的文件或目录（请参阅 \ref cmdimage &quot;\image&quot; 命令）。</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>\c INPUT_FILTER 标签可用于指定 Doxygen 应为每个输入文件调用的过滤器。Doxygen 将通过执行（通过 &lt;code&gt;popen()&lt;/code&gt;）命令来调用过滤器程序：&lt;br&gt;&lt;code&gt;\&lt;filter\&gt; \&lt;input-file\&gt;&lt;/code&gt;&lt;br&gt;其中 &lt;code&gt;\&lt;filter\&gt;&lt;/code&gt; 是 \c INPUT_FILTER 标签的值，&lt;code&gt;\&lt;input-file\&gt;&lt;/code&gt; 是输入文件的名称。Doxygen 随后将使用过滤器程序写入标准输出的输出进行进一步处理。如果指定了 \ref cfg_filter_patterns &quot;FILTER_PATTERNS&quot;，则忽略此标签。&lt;br&gt;请注意，过滤器不得添加或删除行；它在扫描代码之前应用，但在生成输出代码时不应用。如果添加或删除行，锚点将无法正确放置。&lt;br&gt;请注意，Doxygen 将使用处理并写入标准输出的数据进行进一步处理，因此不应将其他任何内容（如调试语句或使用的命令（因此在 Windows 批处理文件中始终使用 `@echo OFF`））写入标准输出。&lt;br&gt;请注意，对于自定义扩展名或未直接支持的扩展名，您还需要为扩展名设置 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;，否则 Doxygen 不会正确处理这些文件。&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>\c FILTER_PATTERNS 标签可用于基于每个文件模式指定过滤器。Doxygen 将文件名与每个模式进行比较，如果有匹配，则应用过滤器。过滤器是以下形式的列表：pattern=filter（如 `*.cpp=my_cpp_filter`）。请参阅 \ref cfg_input_filter &quot;INPUT_FILTER&quot; 以获取有关如何使用过滤器的更多信息。如果 \c FILTER_PATTERNS 标签为空或没有模式匹配文件名，则应用 \ref cfg_input_filter &quot;INPUT_FILTER&quot;。&lt;br&gt;请注意，对于自定义扩展名或未直接支持的扩展名，您还需要为扩展名设置 \ref cfg_extension_mapping &quot;EXTENSION_MAPPING&quot;，否则 Doxygen 不会正确处理这些文件。&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>如果 \c FILTER_SOURCE_FILES 标签设置为 \c YES，则输入过滤器（如果使用 \ref cfg_input_filter &quot;INPUT_FILTER&quot; 设置）也将用于过滤用于生成要浏览的源文件的输入文件（即当 \ref cfg_source_browser &quot;SOURCE_BROWSER&quot; 设置为 \c YES 时）。</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>\c FILTER_SOURCE_PATTERNS 标签可用于基于每个文件模式指定源过滤器。模式将覆盖 \ref cfg_filter_patterns &quot;FILTER_PATTERN&quot; 的设置（如果有），也可以使用 `*.ext=` 禁用特定模式的源过滤器（因此不命名过滤器）。</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>如果 \c USE_MDFILE_AS_MAINPAGE 标签引用作为输入一部分的 markdown 文件的名称，则其内容将放置在主页面上（`index.html`）。如果您的项目在例如 GitHub 上，并且想要将介绍页面也重用于 Doxygen 输出，这可能很有用。</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>如果 \c IMPLICIT_DIR_DOCS 标签设置为 \c YES，则在项目根目录的子目录中找到的任何 `README.md` 文件都将用作该子目录的文档，除非 `README.md` 以 \ref cmddir &quot;\dir&quot;、\ref cmdpage &quot;\page&quot; 或 \ref cmdmainpage &quot;\mainpage&quot; 命令开头。如果设置为 \c NO，则需要 `README.md` 文件以显式的 \ref cmddir &quot;\dir&quot; 命令开头才能用作目录文档。</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Fortran 标准规定，对于固定格式的 Fortran 代码，从位置 72 开始的所有字符都被视为注释。一个常见的扩展是允许在自动注释开始之前有更长的行。设置 \c FORTRAN_COMMENT_AFTER 也将使在自动注释开始之前可以处理更长的行。</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>如果 \c SOURCE_BROWSER 标签设置为 \c YES，则将生成源文件列表。已记录的实体将与这些源交叉引用。&lt;br&gt;注意：要摆脱生成输出中的所有源代码，请确保也将 \ref cfg_verbatim_headers &quot;VERBATIM_HEADERS&quot; 设置为 \c NO。</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>将 \c INLINE_SOURCES 标签设置为 \c YES 将直接在文档中包含函数体、多行宏、枚举或列表初始化变量。</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>将 \c STRIP_CODE_COMMENTS 标签设置为 \c YES 将指示 Doxygen 从生成的源代码片段中隐藏任何特殊注释块。普通的 C、C++ 和 Fortran 注释将始终可见。</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>如果 \c REFERENCED_BY_RELATION 标签设置为 \c YES，则对于每个已记录的实体，将列出引用它的所有已记录函数。</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>如果 \c REFERENCES_RELATION 标签设置为 \c YES，则对于每个已记录的函数，将列出该函数调用/使用的所有已记录实体。</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>如果 \c REFERENCES_LINK_SOURCE 标签设置为 \c YES 且 \ref cfg_source_browser &quot;SOURCE_BROWSER&quot; 标签设置为 \c YES，则 \ref cfg_references_relation &quot;REFERENCES_RELATION&quot; 和 \ref cfg_referenced_by_relation &quot;REFERENCED_BY_RELATION&quot; 列表中函数的超链接将链接到源代码。否则，它们将链接到文档。</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>如果启用了 \c SOURCE_TOOLTIPS（默认），则在源代码中悬停超链接将显示一个工具提示，其中包含其他信息，如原型、简要描述以及指向定义和文档的链接。由于这会使 HTML 文件更大并使大文件的加载速度稍慢，您可以选择禁用此功能。</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>如果 \c USE_HTAGS 标签设置为 \c YES，则对源代码的引用将指向由 \c htags(1) 工具生成的 HTML，而不是 Doxygen 内置的源浏览器。\c htags 工具是 GNU 全局源标记系统的一部分（请参阅 https://www.gnu.org/software/global/global.html）。您需要 4.8.6 或更高版本。&lt;br&gt;要使用它，请执行以下操作：-# 安装最新版本的 \c global -# 在配置文件中启用 \ref cfg_source_browser &quot;SOURCE_BROWSER&quot; 和 \c USE_HTAGS -# 确保 \ref cfg_input &quot;INPUT&quot; 指向源代码树的根目录 -# 正常运行 \c doxygen&lt;br&gt;Doxygen 将调用 \c htags（它将依次调用 \c gtags），因此这些工具必须可从命令行获得（即在搜索路径中）。&lt;br&gt;结果：链接到源代码的链接现在将指向 \c htags 的输出，而不是 Doxygen 生成的源浏览器。</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>如果 \c VERBATIM_HEADERS 标签设置为 \c YES，Doxygen 将为每个指定包含的类生成头文件的逐字副本。设置为 \c NO 以禁用此功能。\sa 第 \ref cmdclass &quot;\class&quot; 节。</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>如果 \c CLANG_ASSISTED_PARSING 标签设置为 \c YES，Doxygen 将使用 &lt;a href=&quot;http://clang.llvm.org/&quot;&gt;clang 解析器&lt;/a&gt;进行更准确的解析，但会降低性能。这对于模板丰富的 C++ 代码特别有用，因为 Doxygen 的内置解析器缺少必要的类型信息。@note 此选项的可用性取决于 Doxygen 是否使用 CMake 的 `-Duse_libclang=ON` 选项生成。</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>如果 \c CLANG_ASSISTED_PARSING 标签设置为 \c YES 且 \c CLANG_ADD_INC_PATHS 标签设置为 \c YES，Doxygen 将把每个输入的目录添加到包含路径。</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>如果启用了 clang 辅助解析，您可以向编译器提供在调用编译器时通常使用的命令行选项。请注意，Doxygen 已经为使用 \ref cfg_input &quot;INPUT&quot; 和 \ref cfg_include_path &quot;INCLUDE_PATH&quot; 指定的文件和目录设置了包含路径。</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>如果启用了 clang 辅助解析，您可以为 clang 解析器提供包含名为 `compile_commands.json` 的文件的目录的路径。此文件是 &lt;a href=&quot;http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html&quot;&gt;编译数据库&lt;/a&gt;，包含构建源文件时使用的选项。这等效于向 clang 工具（如 `clang-check`）指定 `-p` 选项。然后这些选项将传递给解析器。使用 \ref cfg_clang_options &quot;CLANG_OPTIONS&quot; 指定的任何选项也将被添加。@note 此选项的可用性取决于 Doxygen 是否使用 CMake 的 `-Duse_libclang=ON` 选项生成。</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>如果 \c ALPHABETICAL_INDEX 标签设置为 \c YES，将生成所有复合类型的字母索引。如果项目包含大量类、结构体、联合体或接口，请启用此选项。</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>\c IGNORE_PREFIX 标签可用于指定在生成索引标题时应忽略的前缀（或前缀列表）。\c IGNORE_PREFIX 标签适用于类、函数和成员名称。实体将放置在移除前缀后实体名称的首字母下的字母列表中。</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>如果 \c GENERATE_HTML 标签设置为 \c YES，Doxygen 将生成 HTML 输出</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>\c HTML_OUTPUT 标签用于指定 HTML 文档的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>\c HTML_FILE_EXTENSION 标签可用于指定每个生成的 HTML 页面的文件扩展名（例如：&lt;code&gt;.htm, .php, .asp&lt;/code&gt;）。</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>\c HTML_HEADER 标签可用于为每个生成的 HTML 页面指定用户定义的 HTML 头文件。如果标签留空，Doxygen 将生成标准头文件。&lt;br&gt;要获得有效的 HTML，头文件需要包含 Doxygen 需要的任何脚本和样式表，这取决于使用的配置选项（例如设置 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;）。强烈建议使用默认头文件开始：\verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim 然后修改文件 \c new_header.html。另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的默认头文件的信息。@note 头文件可能会更改，因此通常在升级到更新版本的 Doxygen 时必须重新生成默认头文件。</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>\c HTML_FOOTER 标签可用于为每个生成的 HTML 页面指定用户定义的 HTML 页脚。如果标签留空，Doxygen 将生成标准页脚。请参阅 \ref cfg_html_header &quot;HTML_HEADER&quot; 以获取有关如何生成默认页脚以及页脚内可以使用哪些特殊命令的更多信息。另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的默认页脚的信息。</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>\c HTML_STYLESHEET 标签可用于指定每个 HTML 页面使用的用户定义的级联样式表。它可用于微调 HTML 输出的外观。如果留空，Doxygen 将生成默认样式表。另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的样式表的信息。\note 建议使用 \ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot; 而不是此标签，因为它更健壮，并且此标签（&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;）将来会过时。</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>\c HTML_EXTRA_STYLESHEET 标签可用于指定在 Doxygen 创建的标准样式表之后包含的其他用户定义的级联样式表。使用此选项可以覆盖某些样式方面。这比使用 \ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot; 更可取，因为它不会替换标准样式表，因此对未来的更新更加健壮。Doxygen 会将样式表文件复制到输出目录。\note 额外样式表文件的顺序很重要（例如，列表中的最后一个样式表覆盖列表中前一个样式表的设置）。\note 由于目前无法在 Webkit/Chromium 中覆盖滚动条的样式，如果指定了一个或多个额外样式表，默认 doxygen.css 中将省略滚动条样式。因此，如果需要自定义滚动条，必须显式添加。</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>\c HTML_EXTRA_FILES 标签可用于指定一个或多个应复制到 HTML 输出目录的额外图像或其他源文件。请注意，这些文件将被复制到基本 HTML 输出目录。使用 \ref cfg_html_header &quot;HTML_HEADER&quot; 和/或 \ref cfg_html_footer &quot;HTML_FOOTER&quot; 文件中的 &lt;code&gt;$relpath^&lt;/code&gt; 标记来加载这些文件。在 \ref cfg_html_stylesheet &quot;HTML_STYLESHEET&quot; 文件中，仅使用文件名。另请注意，文件将按原样复制；没有可用的命令或标记。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>\c HTML_COLORSTYLE 标签可用于指定生成的 HTML 输出是否应以深色或浅色主题呈现。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>\c HTML_COLORSTYLE_HUE 标签控制 HTML 输出的颜色。Doxygen 将根据此颜色调整样式表和背景图像中的颜色。色调指定为色环上的角度，请参阅 https://en.wikipedia.org/wiki/Hue 以获取更多信息。例如，值 0 表示红色，60 是黄色，120 是绿色，180 是青色，240 是蓝色，300 是紫色，360 又是红色。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>\c HTML_COLORSTYLE_SAT 标签控制 HTML 输出中颜色的纯度（或饱和度）。值为 0 时，输出将仅使用灰度。值为 255 将产生最鲜艳的颜色。</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>\c HTML_COLORSTYLE_GAMMA 标签控制应用于 HTML 输出中颜色亮度分量的伽马校正。低于 100 的值逐渐使输出变亮，而高于 100 的值使输出变暗。该值除以 100 是实际应用的伽马值，因此 80 表示伽马为 0.8，值 220 表示伽马为 2.2，100 不改变伽马。</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>如果 \c HTML_DYNAMIC_MENUS 标签设置为 \c YES，则生成的 HTML 文档将包含一个主索引，其中包含通过 JavaScript 动态创建的垂直导航菜单。如果禁用，导航索引将由静态嵌入每个 HTML 页面的多级选项卡组成。禁用此选项以支持没有 JavaScript 的浏览器，如 Qt 帮助浏览器。</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>如果 \c HTML_DYNAMIC_SECTIONS 标签设置为 \c YES，则生成的 HTML 文档将包含在页面加载后可以隐藏和显示的部分。</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>如果 \c HTML_CODE_FOLDING 标签设置为 \c YES，则类和函数可以在生成的 HTML 源代码中动态折叠和展开。</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>如果 \c HTML_COPY_CLIPBOARD 标签设置为 \c YES，Doxygen 将在代码和文本片段的右上角显示一个图标，允许用户将其内容复制到剪贴板。请注意，这仅在浏览器支持且网页通过&lt;a href=&quot;https://www.w3.org/TR/secure-contexts/&quot;&gt;安全上下文&lt;/a&gt;提供服务时才有效，即使用 https: 或 file: 协议。</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen 在浏览器中持久存储一些设置（例如通过 cookie）。默认情况下，这些设置适用于 Doxygen 在所有项目中生成的所有 HTML 页面。\c HTML_PROJECT_COOKIE 标签可用于在项目特定的键下存储设置，以便用户首选项将单独存储。</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>使用 \c HTML_INDEX_NUM_ENTRIES 可以控制最初在各种树结构索引中显示的首选条目数；用户稍后可以动态展开和折叠条目。Doxygen 将树展开到最多可见指定数量条目的级别（除非完全折叠的树已经超过此数量）。因此，将条目数设置为 1 将默认生成完全折叠的树。0 是一个特殊值，表示无限数量的条目，将导致默认生成完全展开的树。</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>如果 \c GENERATE_DOCSET 标签设置为 \c YES，将生成额外的索引文件，可用作 OSX 10.5 (Leopard) 引入的 &lt;a href=&quot;https://developer.apple.com/xcode/&quot;&gt;Apple 的 Xcode 3 集成开发环境&lt;/a&gt;的输入。要创建文档集，Doxygen 将在 HTML 输出目录中生成 Makefile。运行 \c make 将在该目录中生成 docset，运行 &lt;code&gt;make install&lt;/code&gt; 将把 docset 安装在 &lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt; 中，以便 Xcode 在启动时找到它。请参阅 https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.html 以获取更多信息。</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>此标签确定 docset 订阅源的名称。文档订阅源提供了一个保护伞，可以将来自单个提供商（如公司或产品套件）的多个文档集分组。</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>此标签确定 docset 订阅源的 URL。文档订阅源提供了一个保护伞，可以将来自单个提供商（如公司或产品套件）的多个文档集分组。</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>此标签指定一个应唯一标识文档集包的字符串。这应该是反向域名样式的字符串，例如 &lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;。Doxygen 会将 &lt;code&gt;.docset&lt;/code&gt; 附加到名称后。</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>\c DOCSET_PUBLISHER_ID 标签指定一个应唯一标识文档发布者的字符串。这应该是反向域名样式的字符串，例如 &lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;。</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>\c DOCSET_PUBLISHER_NAME 标签标识文档发布者。</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>如果 \c GENERATE_HTMLHELP 标签设置为 \c YES，Doxygen 会生成三个额外的 HTML 索引文件：\c index.hhp、\c index.hhc 和 \c index.hhk。\c index.hhp 是一个项目文件，可以由 Windows 上的 Microsoft HTML Help Workshop 读取。2021 年初，Microsoft 将原始页面（包括下载链接）下线（HTML 帮助工作室已经处于维护模式多年）。您可以从网络档案下载 HTML 帮助工作室，网址为 &lt;a href=&quot;http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe&quot;&gt;安装可执行文件&lt;/a&gt;。&lt;br&gt;HTML Help Workshop 包含一个编译器，可以将 Doxygen 生成的所有 HTML 输出转换为单个编译的 HTML 文件（`.chm`）。编译的 HTML 文件现在用作 Windows 98 帮助格式，将来将在所有 Windows 平台上替换旧的 Windows 帮助格式（`.hlp`）。压缩的 HTML 文件还包含索引、目录，您可以在文档中搜索单词。HTML 工作室还包含一个用于压缩 HTML 文件的查看器。</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>\c CHM_FILE 标签可用于指定生成的 `.chm` 文件的文件名。如果结果不应写入 html 输出目录，可以在文件前添加路径。</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>\c HHC_LOCATION 标签可用于指定 HTML 帮助编译器（\c hhc.exe）的位置（包含文件名的绝对路径）。如果非空，Doxygen 将尝试在生成的 \c index.hhp 上运行 HTML 帮助编译器。</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>\c GENERATE_CHI 标志控制是否生成单独的 `.chi` 索引文件（\c YES）或将其包含在主 `.chm` 文件中（\c NO）。</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>\c CHM_INDEX_ENCODING 用于编码 HtmlHelp 索引（\c hhk）、内容（\c hhc）和项目文件内容。</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>\c BINARY_TOC 标志控制 `.chm` 文件中是生成二进制目录（\c YES）还是普通目录（\c NO）。此外，它启用 `Previous` 和 `Next` 按钮。</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>\c TOC_EXPAND 标志可以设置为 \c YES，以便为组成员向 HTML 帮助文档的目录和树视图添加额外项目。</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>\c SITEMAP_URL 标签用于指定用户在文档部署期间将生成的文档放置在服务器上的位置的完整 URL。生成的站点地图称为 `sitemap.xml`，放置在 \ref cfg_html_output &quot;HTML_OUTPUT&quot; 指定的目录中。如果未指定 \c SITEMAP_URL，则不会生成站点地图。有关站点地图协议的信息，请参阅 https://www.sitemaps.org</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>如果 \c GENERATE_QHP 标签设置为 \c YES 且 \ref cfg_qhp_namespace &quot;QHP_NAMESPACE&quot; 和 \ref cfg_qhp_virtual_folder &quot;QHP_VIRTUAL_FOLDER&quot; 都已设置，将生成额外的索引文件，可用作 Qt 的 qhelpgenerator 的输入，以生成生成 HTML 文档的 Qt 压缩帮助（`.qch`）。</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>如果指定了 \ref cfg_qhg_location &quot;QHG_LOCATION&quot; 标签，\c QCH_FILE 标签可用于指定生成的 `.qch` 文件的文件名。指定的路径相对于 HTML 输出文件夹。</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>\c QHP_NAMESPACE 标签指定生成 Qt 帮助项目输出时使用的命名空间。有关更多信息，请参阅 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace&quot;&gt;Qt 帮助项目 / 命名空间&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>\c QHP_VIRTUAL_FOLDER 标签指定生成 Qt 帮助项目输出时使用的命名空间。有关更多信息，请参阅 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders&quot;&gt;Qt 帮助项目 / 虚拟文件夹&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>如果设置了 \c QHP_CUST_FILTER_NAME 标签，它指定要添加的自定义过滤器的名称。有关更多信息，请参阅 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qt 帮助项目 / 自定义过滤器&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>\c QHP_CUST_FILTER_ATTRS 标签指定要添加的自定义过滤器的属性列表。有关更多信息，请参阅 &lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters&quot;&gt;Qt 帮助项目 / 自定义过滤器&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>\c QHP_SECT_FILTER_ATTRS 标签指定此项目的过滤器部分匹配的属性列表。&lt;a href=&quot;https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes&quot;&gt;Qt 帮助项目 / 过滤器属性&lt;/a&gt;。</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>\c QHG_LOCATION 标签可用于指定 Qt 的 qhelpgenerator 的位置（包含文件名的绝对路径）。如果非空，Doxygen 将尝试在生成的 `.qhp` 文件上运行 qhelpgenerator。</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>如果 \c GENERATE_ECLIPSEHELP 标签设置为 \c YES，将生成额外的索引文件，与 HTML 文件一起，它们形成一个 `Eclipse` 帮助插件。要安装此插件并使其在 `Eclipse` 的帮助内容菜单下可用，需要将包含 HTML 和 XML 文件的目录的内容复制到 eclipse 的插件目录中。插件目录中的目录名称应与 \ref cfg_eclipse_doc_id &quot;ECLIPSE_DOC_ID&quot; 值相同。复制后，`Eclipse` 需要重新启动才能显示帮助。</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>`Eclipse` 帮助插件的唯一标识符。安装插件时，包含 HTML 和 XML 文件的目录名称也应具有此名称。每个文档集应具有自己的标识符。</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>如果您想完全控制生成的 HTML 页面的布局，可能需要禁用索引并用您自己的索引替换它。\c DISABLE_INDEX 标签可用于打开/关闭每个 HTML 页面顶部的压缩索引（选项卡）。值为 \c NO 启用索引，值 \c YES 禁用它。由于索引中的选项卡包含与导航树相同的信息，如果您还将 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 设置为 \c YES，则可以将此选项设置为 \c YES。</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>\c GENERATE_TREEVIEW 标签用于指定是否应生成树状索引结构以显示层次信息。如果标签值设置为 \c YES，将生成一个包含树状索引结构的侧面板（就像为 HTML 帮助生成的结构一样）。为此，需要一个支持 JavaScript、DHTML、CSS 和框架的浏览器（即任何现代浏览器）。Windows 用户可能最好使用 HTML 帮助功能。通过自定义样式表（请参阅 \ref cfg_html_extra_stylesheet &quot;HTML_EXTRA_STYLESHEET&quot;），可以进一步微调索引的外观（请参阅 \ref doxygen_finetune）。例如，Doxygen 生成的默认样式表有一个示例，显示如何在树的根部放置图像而不是 \ref cfg_project_name &quot;PROJECT_NAME&quot;。由于树基本上比选项卡索引具有更多详细信息，您可以考虑在启用此选项时将 \ref cfg_disable_index &quot;DISABLE_INDEX&quot; 设置为 \c YES。</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>当 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 设置为 \c YES 时，\c PAGE_OUTLINE_PANEL 选项确定是否在屏幕右侧显示一个额外的导航面板，显示主页面的内容大纲，类似于例如 https://developer.android.com/reference。如果 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 设置为 \c NO，此选项无效。</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>当 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 设置为 \c YES 时，\c FULL_SIDEBAR 选项确定侧边栏是否仅限于树视图区域（值 \c NO）还是应该延伸到窗口的整个高度（值 \c YES）。将此设置为 \c YES 会产生类似于例如 https://docs.readthedocs.io 的布局，内容空间更大，但项目徽标、标题和描述的空间更小。如果 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; 设置为 \c NO，此选项无效。</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>\c ENUM_VALUES_PER_LINE 标签可用于设置 Doxygen 在生成的 HTML 文档中每行分组的枚举值数量。&lt;br&gt;请注意，值为 0 将完全抑制枚举值出现在概述部分中。</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>当设置 \c SHOW_ENUM_VALUES 标签时，Doxygen 将在枚举助记符旁边显示指定的枚举值。</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>如果启用了树视图（请参阅 \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot;），则此标签可用于设置显示树的框架的初始宽度（以像素为单位）。</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>如果 \c EXT_LINKS_IN_WINDOW 选项设置为 \c YES，Doxygen 将在单独的窗口中打开通过标签文件导入的外部符号的链接。</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>如果 \c OBFUSCATE_EMAILS 标签设置为 \c YES，Doxygen 将混淆电子邮件地址。</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>如果 \c HTML_FORMULA_FORMAT 选项设置为 \c svg，Doxygen 将使用 pdf2svg 工具（请参阅 https://github.com/dawbarton/pdf2svg）或 inkscape（请参阅 https://inkscape.org）为 HTML 输出生成 SVG 图像而不是 PNG。这些图像在缩放分辨率下通常看起来更好。</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>使用此标签更改包含在 HTML 文档中作为图像的 \f$\mbox{\LaTeX}\f$ 公式的字体大小。在成功运行 Doxygen 后更改字体大小时，您需要手动从 HTML 输出目录中删除任何 `form_*.png` 图像以强制重新生成它们。</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>\c FORMULA_MACROFILE 可以包含 \f$\mbox{\LaTeX}\f$ `\newcommand` 和 `\renewcommand` 命令，以创建新的 \f$\mbox{\LaTeX}\f$ 命令，用作公式中的构建块。有关详细信息，请参阅第 \ref formulas 节。</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>启用 \c USE_MATHJAX 选项以使用 MathJax（请参阅 https://www.mathjax.org）呈现 \f$\mbox{\LaTeX}\f$ 公式，它使用客户端 JavaScript 进行呈现，而不是使用预呈现的位图。如果您没有安装 \f$\mbox{\LaTeX}\f$ 或者您希望公式在 HTML 输出中看起来更漂亮，请使用此选项。启用后，您可能还需要单独安装 MathJax 并使用 \ref cfg_mathjax_relpath &quot;MATHJAX_RELPATH&quot; 选项配置其路径。</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>使用 \c MATHJAX_VERSION 可以指定要使用的 MathJax 版本。请注意，不同版本的 MathJax 对不同设置有不同的要求，因此在不同的 MathJax 版本之间切换时，可能还需要更改其他 MathJax 设置。</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>启用 MathJax 后，您可以设置 MathJax 输出使用的默认输出格式。有关输出格式的更多详细信息，请参阅 &lt;a href=&quot;https://docs.mathjax.org/en/v2.7/output.html&quot;&gt;MathJax 版本 2&lt;/a&gt;、&lt;a href=&quot;https://docs.mathjax.org/en/v3.2/output/index.html&quot;&gt;MathJax 版本 3&lt;/a&gt; 和 &lt;a href=&quot;https://docs.mathjax.org/en/v4.0/output/index.htm&quot;&gt;MathJax 版本 4&lt;/a&gt;。</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>启用 MathJax 后，您需要使用 \c MATHJAX_RELPATH 选项指定相对于 HTML 输出目录的位置。对于 Mathjax 版本 2，目标目录应包含 `MathJax.js` 脚本。例如，如果 \c mathjax 目录与 HTML 输出目录位于同一级别，则 \c MATHJAX_RELPATH 应为 &lt;code&gt;../mathjax&lt;/code&gt;。对于 Mathjax 版本 3 和 4，目标目录应包含 `tex-&lt;format&gt;.js` 脚本（其中 `&lt;format&gt;` 是 `chtml` 或 `svg`）。默认值指向 MathJax 内容分发网络，因此您可以快速查看结果而无需安装 MathJax。但是，强烈建议在部署前从 https://www.mathjax.org 安装 MathJax 的本地副本。默认值为：- MathJax 版本 2：https://cdn.jsdelivr.net/npm/mathjax@2 - MathJax 版本 3：https://cdn.jsdelivr.net/npm/mathjax@3 - MathJax 版本 4：https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>\c MATHJAX_EXTENSIONS 标签可用于指定在 MathJax 呈现期间应启用的一个或多个 MathJax 扩展名称。例如，对于 MathJax 版本 2（请参阅 https://docs.mathjax.org/en/v2.7/tex.html）：\verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim 例如，对于 MathJax 版本 3（请参阅 https://docs.mathjax.org/en/v3.2/input/tex/extensions/）：\verbatim MATHJAX_EXTENSIONS = ams \endverbatim 例如，对于 MathJax 版本 4（请参阅 https://docs.mathjax.org/en/v4.0/input/tex/extensions/）：\verbatim MATHJAX_EXTENSIONS = units \endverbatim 请注意，对于 Mathjax 版本 4，已经自动加载了相当多的扩展。要在 Mathjax 版本 4 中禁用包，可以使用前面带有减号的包名称（`-` 如 `MATHJAX_EXTENSIONS += -textmacros`）</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>\c MATHJAX_CODEFILE 标签可用于指定一个包含 JavaScript 代码片段的文件，这些代码将在 MathJax 代码启动时使用。有关更多详细信息，请参阅 Mathjax 网站：- &lt;a href=&quot;https://docs.mathjax.org/en/v2.7/&quot;&gt;MathJax 版本 2&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v3.2/&quot;&gt;MathJax 版本 3&lt;/a&gt; - &lt;a href=&quot;https://docs.mathjax.org/en/v4.0/&quot;&gt;MathJax 版本 4&lt;/a&gt;</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>当 \c SEARCHENGINE 标签启用时，Doxygen 将为 HTML 输出生成一个搜索框。底层搜索引擎使用 JavaScript 和 DHTML，应该可以在任何现代浏览器上运行。请注意，当使用 HTML 帮助（\ref cfg_generate_htmlhelp &quot;GENERATE_HTMLHELP&quot;）、Qt 帮助（\ref cfg_generate_qhp &quot;GENERATE_QHP&quot;）或文档集（\ref cfg_generate_docset &quot;GENERATE_DOCSET&quot;）时，已经有搜索功能，因此通常应禁用此功能。对于大型项目，基于 JavaScript 的搜索引擎可能很慢，然后启用 \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot; 可能提供更好的解决方案。可以使用键盘进行搜索；要跳转到搜索框，请使用 &lt;code&gt;\&lt;access key\&gt; + S&lt;/code&gt;（&lt;code&gt;\&lt;access key\&gt;&lt;/code&gt; 取决于操作系统和浏览器，但通常是 &lt;code&gt;\&lt;CTRL\&gt;&lt;/code&gt;、&lt;code&gt;\&lt;ALT\&gt;&lt;/code&gt;/&lt;code&gt;\&lt;option\&gt;&lt;/code&gt; 或两者）。在搜索框内，使用 &lt;code&gt;\&lt;cursor down key\&gt;&lt;/code&gt; 跳转到搜索结果窗口，可以使用 &lt;code&gt;\&lt;cursor keys\&gt;&lt;/code&gt; 导航结果。按 &lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt; 选择项目或 &lt;code&gt;\&lt;escape\&gt;&lt;/code&gt; 取消搜索。当光标在搜索框内时，可以通过按 &lt;code&gt;\&lt;Shift\&gt;+\&lt;cursor down\&gt;&lt;/code&gt; 选择过滤器选项。在这里也使用 &lt;code&gt;\&lt;cursor keys\&gt;&lt;/code&gt; 选择过滤器，使用 &lt;code&gt;\&lt;Enter\&gt;&lt;/code&gt; 或 &lt;code&gt;\&lt;escape\&gt;&lt;/code&gt; 激活或取消过滤器选项。</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>当 \c SERVER_BASED_SEARCH 标签启用时，搜索引擎将使用 Web 服务器而不是使用 JavaScript 的 Web 客户端来实现。根据 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 设置，有两种风格的基于 Web 服务器的搜索。禁用时，Doxygen 将生成用于搜索的 PHP 脚本和脚本使用的索引文件。当启用 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 时，需要由外部工具提供索引和搜索。有关详细信息，请参阅第 \ref extsearch 节。</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>当 \c EXTERNAL_SEARCH 标签启用时，Doxygen 将不再生成用于搜索的 PHP 脚本。相反，搜索结果将写入 XML 文件，需要由外部索引器处理。Doxygen 将调用 \ref cfg_searchengine_url &quot;SEARCHENGINE_URL&quot; 选项指向的外部搜索引擎以获取搜索结果。&lt;br&gt;Doxygen 附带一个示例索引器（\c doxyindexer）和搜索引擎（&lt;code&gt;doxysearch.cgi&lt;/code&gt;），它们基于开源搜索引擎库 &lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;。&lt;br&gt;有关详细信息，请参阅第 \ref extsearch 节。</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>\c SEARCHENGINE_URL 应指向由 Web 服务器托管的搜索引擎，当启用 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 时将返回搜索结果。&lt;br&gt;Doxygen 附带一个示例索引器（\c doxyindexer）和搜索引擎（&lt;code&gt;doxysearch.cgi&lt;/code&gt;），它们基于开源搜索引擎库 &lt;a href=&quot;https://xapian.org/&quot;&gt;Xapian&lt;/a&gt;。有关详细信息，请参阅第 \ref extsearch 节。</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>当 \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot; 和 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 都启用时，未索引的搜索数据将写入文件以供外部工具索引。使用 \c SEARCHDATA_FILE 标签可以指定此文件的名称。</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>当 \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot; 和 \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; 都启用时，\c EXTERNAL_SEARCH_ID 标签可用作项目的标识符。这与 \ref cfg_extra_search_mappings &quot;EXTRA_SEARCH_MAPPINGS&quot; 结合使用很有用，可以搜索多个项目并将结果重定向回正确的项目。</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>\c EXTRA_SEARCH_MAPPINGS 标签可用于启用搜索除此配置文件定义的项目以外的 Doxygen 项目，但这些项目都添加到同一外部搜索索引中。每个项目需要通过 \ref cfg_external_search_id &quot;EXTERNAL_SEARCH_ID&quot; 设置唯一的 id。然后搜索映射将 id 映射到可以找到文档的相对位置。格式为：\verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>如果 \c GENERATE_LATEX 标签设置为 \c YES，Doxygen 将生成 \f$\mbox{\LaTeX}\f$ 输出。</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>\c LATEX_OUTPUT 标签用于指定 \f$\mbox{\LaTeX}\f$ 文档的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>\c LATEX_CMD_NAME 标签可用于指定要调用的 \f$\mbox{\LaTeX}\f$ 命令名称。&lt;br&gt;请注意，当不启用 \ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot; 时，默认值为 \c latex；当启用 \ref cfg_use_pdflatex &quot;USE_PDFLATEX&quot; 时，默认值为 \c pdflatex；在后一种情况下，如果选择 \c latex，则会被 \c pdflatex 覆盖。对于特定的输出语言，默认值可能已设置为不同的值，这取决于输出语言的实现。</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>\c MAKEINDEX_CMD_NAME 标签可用于指定为 \f$\mbox{\LaTeX}\f$ 生成索引的命令名称。@note 此标签在 `Makefile` / `make.bat` 中使用。\sa \ref cfg_latex_makeindex_cmd &quot;LATEX_MAKEINDEX_CMD&quot; 用于生成输出文件（`.tex`）中的部分。</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>\c LATEX_MAKEINDEX_CMD 标签可用于指定为 \f$\mbox{\LaTeX}\f$ 生成索引的命令名称。如果第一个字符没有反斜杠（`\`），则会自动添加到 \f$\mbox{\LaTeX}\f$ 代码中。@note 此标签在生成输出文件（`.tex`）中使用。\sa \ref cfg_makeindex_cmd_name &quot;MAKEINDEX_CMD_NAME&quot; 用于 `Makefile` / `make.bat` 中的部分。</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>如果 \c COMPACT_LATEX 标签设置为 \c YES，Doxygen 会生成更紧凑的 \f$\mbox{\LaTeX}\f$ 文档。这对于小型项目可能很有用，通常可能有助于节省一些树木。</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>\c PAPER_TYPE 标签可用于设置打印机使用的纸张类型。</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>\c EXTRA_PACKAGES 标签可用于指定应包含在 \f$\mbox{\LaTeX}\f$ 输出中的一个或多个 \f$\mbox{\LaTeX}\f$ 包名称。可以仅通过名称或使用与 \f$\mbox{\LaTeX}\f$ `\usepackage` 命令一起使用的正确语法来指定包。例如，要获取 `times` 字体，您可以指定：\verbatim EXTRA_PACKAGES=times 或 EXTRA_PACKAGES={times} \endverbatim 要将选项 `intlimits` 与 `amsmath` 包一起使用，您可以指定：\verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim 如果留空，则不包含额外的包。</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>\c LATEX_HEADER 标签可用于为生成的 \f$\mbox{\LaTeX}\f$ 文档指定用户定义的 \f$\mbox{\LaTeX}\f$ 头文件。头文件应包含直到第一章的所有内容。如果留空，Doxygen 将生成标准头文件。强烈建议使用默认头文件开始：\verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim 然后修改文件 \c new_header.tex。另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的默认头文件的信息。&lt;br&gt;注意：只有在您知道自己在做什么时才使用用户定义的头文件！@note 头文件可能会更改，因此通常在升级到更新版本的 Doxygen 时必须重新生成默认头文件。</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>\c LATEX_FOOTER 标签可用于为生成的 \f$\mbox{\LaTeX}\f$ 文档指定用户定义的 \f$\mbox{\LaTeX}\f$ 页脚。页脚应包含最后一章之后的所有内容。如果留空，Doxygen 将生成标准页脚。请参阅 \ref cfg_latex_header &quot;LATEX_HEADER&quot; 以获取有关如何生成默认页脚以及页脚内可以使用哪些特殊命令的更多信息。另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的默认页脚的信息。注意：只有在您知道自己在做什么时才使用用户定义的页脚！</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>\c LATEX_EXTRA_STYLESHEET 标签可用于指定在 Doxygen 创建的标准样式表之后包含的其他用户定义的 \f$\mbox{\LaTeX}\f$ 样式表。使用此选项可以覆盖某些样式方面。Doxygen 会将样式表文件复制到输出目录。\note 额外样式表文件的顺序很重要（例如，列表中的最后一个样式表覆盖列表中前一个样式表的设置）。</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>\c LATEX_EXTRA_FILES 标签可用于指定一个或多个应复制到 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 输出目录的额外图像或其他源文件。请注意，文件将按原样复制；没有可用的命令或标记。</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>如果 \c PDF_HYPERLINKS 标签设置为 \c YES，生成的 \f$\mbox{\LaTeX}\f$ 将准备转换为 PDF（使用 \c ps2pdf 或 \c pdflatex）。PDF 文件将包含链接（就像 HTML 输出一样）而不是页面引用。这使得输出适合使用 PDF 查看器进行在线浏览。</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>如果 \c USE_PDFLATEX 标签设置为 \c YES，Doxygen 将使用 \ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot; 指定的引擎直接从 \f$\mbox{\LaTeX}\f$ 文件生成 PDF 文件。将此选项设置为 \c YES 以获得更高质量的 PDF 文档。&lt;br&gt;另请参阅第 \ref cfg_latex_cmd_name &quot;LATEX_CMD_NAME&quot; 节以选择引擎。</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>\c LATEX_BATCHMODE 标签指示 \f$\mbox{\LaTeX}\f$ 在出现错误时的行为。</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>如果 \c LATEX_HIDE_INDICES 标签设置为 \c YES，Doxygen 将不在输出中包含索引章节（如文件索引、复合索引等）。</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>\c LATEX_BIB_STYLE 标签可用于指定参考文献使用的样式，例如 \c plainnat 或 \c ieeetr。请参阅 https://en.wikipedia.org/wiki/BibTeX 和 \ref cmdcite &quot;\cite&quot; 以获取更多信息。</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>\c LATEX_EMOJI_DIRECTORY 标签用于指定读取 emoji 图像的（相对或绝对）路径。如果输入相对路径，它将相对于 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 目录。如果留空，则使用 \ref cfg_latex_output &quot;LATEX_OUTPUT&quot; 目录。</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>如果 \c GENERATE_RTF 标签设置为 \c YES，Doxygen 将生成 RTF 输出。RTF 输出针对 Word 97 进行了优化，在其他 RTF 阅读器/编辑器中可能看起来不太漂亮。</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>\c RTF_OUTPUT 标签用于指定 RTF 文档的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>如果 \c COMPACT_RTF 标签设置为 \c YES，Doxygen 会生成更紧凑的 RTF 文档。这对于小型项目可能很有用，通常可能有助于节省一些树木。</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>如果 \c RTF_HYPERLINKS 标签设置为 \c YES，生成的 RTF 将包含超链接字段。RTF 文件将包含链接（就像 HTML 输出一样）而不是页面引用。这使得输出适合使用 Word 或其他支持这些字段的 Word 兼容阅读器进行在线浏览。&lt;br&gt;注意：写字板（write）和其他程序不支持链接。</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>从文件加载样式表定义。语法类似于 Doxygen 的配置文件，即一系列赋值。您只需要提供替换，缺少的定义设置为其默认值。&lt;br&gt;另请参阅第 \ref doxygen_usage 节以获取有关如何生成 Doxygen 通常使用的默认样式表的信息。</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>设置生成 RTF 文档时使用的可选变量。语法类似于 Doxygen 的配置文件。可以使用 &lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt; 生成模板扩展文件。</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>\c RTF_EXTRA_FILES 标签可用于指定一个或多个应复制到 \ref cfg_rtf_output &quot;RTF_OUTPUT&quot; 输出目录的额外图像或其他源文件。请注意，文件将按原样复制；没有可用的命令或标记。</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>如果 \c GENERATE_MAN 标签设置为 \c YES，Doxygen 将为类和文件生成 man 页面。</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>\c MAN_OUTPUT 标签用于指定 man 页面的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。将在 \c MAN_OUTPUT 指定的目录内创建一个 \c man3 目录。</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>\c MAN_EXTENSION 标签确定添加到生成的 man 页面的扩展名。如果手册部分不以数字开头，则在前面加上数字 3。\c MAN_EXTENSION 标签开头的点（.）是可选的。</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>\c MAN_SUBDIR 标签确定在 \c MAN_OUTPUT 中创建的放置 man 页面的目录名称。默认为 man 后跟 \c MAN_EXTENSION，并移除初始 `.`。</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>如果 \c MAN_LINKS 标签设置为 \c YES 且 Doxygen 生成 man 输出，则它将为真实 man 页面中记录的每个实体生成一个额外的 man 文件。这些额外的文件仅引用真实的 man 页面，但没有它们，\c man 命令将无法找到正确的页面。</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>如果 \c GENERATE_XML 标签设置为 \c YES，Doxygen 将生成一个捕获代码结构（包括所有文档）的 XML 文件。</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>\c XML_OUTPUT 标签用于指定 XML 页面的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>如果 \c XML_PROGRAMLISTING 标签设置为 \c YES，Doxygen 将把程序列表（包括语法高亮和交叉引用信息）转储到 XML 输出。请注意，启用此功能将显著增加 XML 输出的大小。</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>如果 \c XML_NS_MEMB_FILE_SCOPE 标签设置为 \c YES，Doxygen 也会在文件作用域中包含命名空间成员，与 HTML 输出匹配。</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>如果 \c GENERATE_DOCBOOK 标签设置为 \c YES，Doxygen 将生成可用于生成 PDF 的 Docbook 文件。</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>\c DOCBOOK_OUTPUT 标签用于指定 Docbook 页面的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>如果 \c GENERATE_AUTOGEN_DEF 标签设置为 \c YES，Doxygen 将生成一个捕获代码结构（包括所有文档）的 AutoGen 定义（请参阅 https://autogen.sourceforge.net/）文件。请注意，此功能目前仍处于实验阶段且不完整。</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>如果 \c GENERATE_SQLITE3 标签设置为 \c YES，Doxygen 将生成一个 \c Sqlite3 数据库，其中包含 Doxygen 找到的符号存储在表中。</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>\c SQLITE3_OUTPUT 标签用于指定 \c Sqlite3 数据库的放置位置。如果输入相对路径，则 \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; 的值将放在其前面。</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>如果 \c SQLITE3_RECREATE_DB 标签设置为 \c YES，每次 Doxygen 运行时都会重新创建现有的 doxygen_sqlite3.db 数据库文件。如果设置为 \c NO，如果已经找到数据库文件，Doxygen 将发出警告并且不会修改它。</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>如果 \c GENERATE_PERLMOD 标签设置为 \c YES，Doxygen 将生成一个捕获代码结构（包括所有文档）的 Perl 模块文件。&lt;br&gt;请注意，此功能目前仍处于实验阶段且不完整。</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>如果 \c PERLMOD_LATEX 标签设置为 \c YES，Doxygen 将生成必要的 \c Makefile 规则、\c Perl 脚本和 \f$\mbox{\LaTeX}\f$ 代码，以便能够从 Perl 模块输出生成 PDF 和 DVI 输出。</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>如果 \c PERLMOD_PRETTY 标签设置为 \c YES，Perl 模块输出将被很好地格式化，以便人类阅读器解析。如果您想了解发生了什么，这很有用。另一方面，如果此标签设置为 \c NO，Perl 模块输出的大小将小得多，Perl 也会同样解析它。</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>生成的 `doxyrules.make` 文件中的 make 变量名称以 \c PERLMOD_MAKEVAR_PREFIX 中包含的字符串为前缀。这很有用，因此同一 `Makefile` 包含的不同 `doxyrules.make` 文件不会相互覆盖变量。</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>如果 \c ENABLE_PREPROCESSING 标签设置为 \c YES，Doxygen 将评估源文件和包含文件中找到的所有 C 预处理指令。</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>如果 \c MACRO_EXPANSION 标签设置为 \c YES，Doxygen 将展开源代码中的所有宏名称。如果设置为 \c NO，则只执行条件编译。可以通过将 \ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot; 设置为 \c YES 以受控方式完成宏展开。</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>如果 \c EXPAND_ONLY_PREDEF 和 \ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; 标签都设置为 \c YES，则宏展开仅限于使用 \ref cfg_predefined &quot;PREDEFINED&quot; 和 \ref cfg_expand_as_defined &quot;EXPAND_AS_DEFINED&quot; 标签指定的宏。</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>如果 \c SEARCH_INCLUDES 标签设置为 \c YES，如果找到 \c \#include，则搜索 \ref cfg_include_path &quot;INCLUDE_PATH&quot; 中的包含文件。</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>\c INCLUDE_PATH 标签可用于指定一个或多个包含不是输入文件但应由预处理器处理的包含文件的目录。请注意，\c INCLUDE_PATH 不是递归的，因此 \ref cfg_recursive &quot;RECURSIVE&quot; 的设置在此无效。</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>您可以使用 \c INCLUDE_FILE_PATTERNS 标签指定一个或多个通配符模式（如 `*.h` 和 `*.hpp`）来过滤目录中的头文件。如果留空，则使用 \ref cfg_file_patterns &quot;FILE_PATTERNS&quot; 指定的模式。</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>\c PREDEFINED 标签可用于指定在预处理器启动之前定义的一个或多个宏名称（类似于例如 \c gcc 的 `-D` 选项）。标签的参数是以下形式的宏列表：&lt;code&gt;name&lt;/code&gt; 或 &lt;code&gt;name=definition&lt;/code&gt;（无空格）。如果省略定义和 \c &quot;=&quot;，则假设为 &quot;=1&quot;。要防止宏定义通过 \c \#undef 取消定义或递归展开，请使用 &lt;code&gt;:=&lt;/code&gt; 运算符而不是 \c = 运算符。</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>如果 \ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; 和 \ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot; 标签设置为 \c YES，则此标签可用于指定应展开的宏名称列表。将使用在源代码中找到的宏定义。如果您想使用覆盖源代码中找到的定义的不同宏定义，请使用 \ref cfg_predefined &quot;PREDEFINED&quot; 标签。</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>如果 \c SKIP_FUNCTION_MACROS 标签设置为 \c YES，Doxygen 的预处理器将删除所有对单独一行、具有全大写名称且不以分号结尾的类函数宏的引用。此类函数宏通常用于样板代码，如果不删除，会使解析器混淆。</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>\c TAGFILES 标签可用于指定一个或多个标签文件。对于每个标签文件，应添加外部文档的位置。没有此位置的标签文件的格式如下：\verbatim TAGFILES = file1 file2 ... \endverbatim 为标签文件添加位置的方式如下：\verbatim TAGFILES = file1=loc1 &quot;file2 = loc2&quot; ... \endverbatim 其中 `loc1` 和 `loc2` 可以是相对或绝对路径或 URL。有关使用标签文件的更多信息，请参阅第 \ref external 节。\note 每个标签文件必须具有唯一的名称（其中名称不包括路径）。如果标签文件不在运行 Doxygen 的目录中，您还必须在此处指定标签文件的路径。</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>在 \c GENERATE_TAGFILE 后指定文件名时，Doxygen 将创建一个基于其读取的输入文件的标签文件。有关标签文件使用的更多信息，请参阅第 \ref external 节。</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>如果 \c ALLEXTERNALS 标签设置为 \c YES，所有外部类和命名空间都将列在类和命名空间索引中。如果设置为 \c NO，则只列出继承的外部类。</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>如果 \c EXTERNAL_GROUPS 标签设置为 \c YES，所有外部组都将列在主题索引中。如果设置为 \c NO，则只列出当前项目的组。</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>如果 \c EXTERNAL_PAGES 标签设置为 \c YES，所有外部页面都将列在相关页面索引中。如果设置为 \c NO，则只列出当前项目的页面。</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>如果设置为 \c YES，继承和协作图将隐藏目标未记录或不是类的继承和使用关系。</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>如果将 \c HAVE_DOT 标签设置为 \c YES，Doxygen 将假设 \c dot 工具可从 \c path 获得。此工具是 &lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt; 的一部分，这是 AT\&amp;T 和 Lucent Bell Labs 的图形可视化工具包。如果此选项设置为 \c NO，本节中的其他选项无效</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>\c DOT_NUM_THREADS 指定 Doxygen 允许并行运行的 \c dot 调用次数。设置为 \c 0 时，Doxygen 将根据系统中可用的处理器数量进行设置。您可以将其显式设置为大于 0 的值，以更好地控制 CPU 负载和处理速度之间的平衡。</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR 是子图的节点、边和标签的公共属性。当您希望 Doxygen 生成的 dot 文件中的字体外观不同时，可以指定 fontname、fontcolor 和 fontsize 属性。有关详细信息，请参阅 &lt;a href=https://graphviz.org/doc/info/attrs.html&gt;节点、边和图属性规范&lt;/a&gt; 您需要确保 dot 能够找到字体，这可以通过将其放在标准位置或设置 \c DOTFONTPATH 环境变量或将 \ref cfg_dot_fontpath &quot;DOT_FONTPATH&quot; 设置为包含字体的目录来完成。默认 graphviz 字体大小为 14。</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR 与 \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; 连接。对于优雅的样式，您可以添加 &#x27;arrowhead=open, arrowtail=open, arrowsize=0.5&#x27;。&lt;a href=https://graphviz.org/doc/info/arrows.html&gt;有关箭头形状的完整文档。&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR 与 \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; 连接。对于没有节点周围框的视图，设置 &#x27;shape=plain&#x27; 或 &#x27;shape=plaintext&#x27; &lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;形状规范&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>您可以设置 \c dot 可以找到在 \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; 和其他 dot 属性中使用 fontname 指定的字体的路径。</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>如果 \c CLASS_GRAPH 标签设置为 \c YES 或 \c GRAPH 或 \c BUILTIN，Doxygen 将为每个已记录的类生成一个图，显示直接和间接继承关系。如果 \c CLASS_GRAPH 标签设置为 \c YES 或 \c GRAPH 且启用了 \ref cfg_have_dot &quot;HAVE_DOT&quot;，则将使用 `dot` 绘制图。如果 \c CLASS_GRAPH 标签设置为 \c YES 且禁用了 \ref cfg_have_dot &quot;HAVE_DOT&quot;，或者如果 \c CLASS_GRAPH 标签设置为 \c BUILTIN，则将使用内置生成器。如果 \c CLASS_GRAPH 标签设置为 \c TEXT，直接和间接继承关系将显示为文本/链接。可以通过命令 \ref cmdinheritancegraph &quot;\inheritancegraph&quot; 为特定类显式启用继承图或选择不同的表示形式。可以通过命令 \ref cmdhideinheritancegraph &quot;\hideinheritancegraph&quot; 禁用继承图。</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>如果 \c COLLABORATION_GRAPH 标签设置为 \c YES，Doxygen 将为每个已记录的类生成一个图，显示该类与其他已记录类之间的直接和间接实现依赖关系（继承、包含和类引用变量）。当 \c COLLABORATION_GRAPH 设置为 \c NO 时，可以通过命令 \ref cmdcollaborationgraph &quot;\collaborationgraph&quot; 显式启用协作图。可以通过命令 \ref cmdhidecollaborationgraph &quot;\hidecollaborationgraph&quot; 禁用协作图。</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>如果 \c GROUP_GRAPHS 标签设置为 \c YES，Doxygen 将为组生成图，显示直接的组依赖关系。当 \c GROUP_GRAPHS 设置为 \c NO 时，可以通过命令 \ref cmdgroupgraph "\\groupgraph" 显式启用组依赖图。可以通过命令 \ref cmdhidegroupgraph "\\hidegroupgraph" 禁用目录图。</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>如果 \c UML_LOOK 标签设置为 \c YES，Doxygen 将以类似于 OMG 统一建模语言的风格生成继承和协作图。</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>如果启用了 \ref cfg_uml_look &quot;UML_LOOK&quot; 标签，字段和方法将显示在类节点内。如果有很多字段或方法和很多节点，图可能变得太大而无法使用。\c UML_LIMIT_NUM_FIELDS 阈值限制每种类型的项数，以使大小更易于管理。将此设置为 0 表示无限制。请注意，在强制执行限制之前，阈值可能会超出 50%。因此，当您将阈值设置为 10 时，最多可能出现 15 个字段，但如果数量超过 15，则显示的字段总数限制为 10。</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>如果启用了 \ref cfg_uml_look &quot;UML_LOOK&quot; 标签，字段标签将沿着两个类节点之间的边显示。如果有很多字段和很多节点，图可能变得太杂乱。\c UML_MAX_EDGE_LABELS 阈值限制项数，以使大小更易于管理。将此设置为 0 表示无限制。</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>如果 \c DOT_UML_DETAILS 标签设置为 \c NO，Doxygen 将在 UML 图中显示不带类型和参数的属性和方法。如果 \c DOT_UML_DETAILS 标签设置为 \c YES，Doxygen 将在 UML 图中为属性和方法添加类型和参数。如果 \c DOT_UML_DETAILS 标签设置为 \c NONE，Doxygen 将不在 UML 图中生成包含类成员信息的字段。类图看起来类似于默认类图，但使用 UML 表示法表示关系。</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>\c DOT_WRAP_THRESHOLD 标签可用于设置在单行上显示的最大字符数。如果实际行长度显著超过此阈值，它将被换行到多行。应用了一些启发式方法以避免难看的换行。</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>如果 \c TEMPLATE_RELATIONS 标签设置为 \c YES，继承和协作图将显示模板与其实例之间的关系。</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>如果 \c INCLUDE_GRAPH、\ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot; 和 \ref cfg_search_includes &quot;SEARCH_INCLUDES&quot; 标签设置为 \c YES，Doxygen 将为每个已记录的文件生成一个图，显示该文件与其他已记录文件之间的直接和间接包含依赖关系。当 \c INCLUDE_GRAPH 设置为 \c NO 时，可以通过命令 \ref cmdincludegraph &quot;\includegraph&quot; 显式启用包含图。可以通过命令 \ref cmdhideincludegraph &quot;\hideincludegraph&quot; 禁用包含图。</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>如果 \c INCLUDED_BY_GRAPH、\ref cfg_enable_preprocessing &quot;ENABLE_PREPROCESSING&quot; 和 \ref cfg_search_includes &quot;SEARCH_INCLUDES&quot; 标签设置为 \c YES，Doxygen 将为每个已记录的文件生成一个图，显示该文件与其他已记录文件之间的直接和间接包含依赖关系。当 \c INCLUDED_BY_GRAPH 设置为 \c NO 时，可以通过命令 \ref cmdincludedbygraph &quot;\includedbygraph&quot; 显式启用被包含图。可以通过命令 \ref cmdhideincludedbygraph &quot;\hideincludedbygraph&quot; 禁用被包含图。</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>如果 \c CALL_GRAPH 标签设置为 \c YES，Doxygen 将为每个全局函数或类方法生成调用依赖图。&lt;br&gt;请注意，启用此选项将显著增加运行时间。因此在大多数情况下，最好只使用 \ref cmdcallgraph &quot;\callgraph&quot; 命令为选定的函数启用调用图。可以通过 \ref cmdhidecallgraph &quot;\hidecallgraph&quot; 命令禁用调用图。</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>如果 \c CALLER_GRAPH 标签设置为 \c YES，Doxygen 将为每个全局函数或类方法生成调用者依赖图。&lt;br&gt;请注意，启用此选项将显著增加运行时间。因此在大多数情况下，最好只使用 \ref cmdcallergraph &quot;\callergraph&quot; 命令为选定的函数启用调用者图。可以通过 \ref cmdhidecallergraph &quot;\hidecallergraph&quot; 命令禁用调用者图。</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>如果 \c GRAPHICAL_HIERARCHY 标签设置为 \c YES，Doxygen 将以图形方式显示所有类的层次结构，而不是文本形式。</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>如果 \c DIRECTORY_GRAPH 标签设置为 \c YES，Doxygen 将以图形方式显示目录对其他目录的依赖关系。依赖关系由目录中文件之间的 \c \#include 关系确定。当 \c DIRECTORY_GRAPH 设置为 \c NO 时，可以通过命令 \ref cmddirectorygraph &quot;\directorygraph&quot; 显式启用目录图。可以通过命令 \ref cmdhidedirectorygraph &quot;\hidedirectorygraph&quot; 禁用目录图。</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>\c DIR_GRAPH_MAX_DEPTH 标签可用于限制由 \c dot 在目录依赖图中生成的子目录级别的最大数量。</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>\c DOT_IMAGE_FORMAT 标签可用于设置由 \c dot 生成的图像的图像格式。有关图像格式的说明，请参阅 \c dot 工具文档中的输出格式部分（&lt;a href=&quot;https://www.graphviz.org/&quot;&gt;Graphviz&lt;/a&gt;）。&lt;br&gt;请注意，格式 `svg:cairo` 和 `svg:cairo:cairo` 不能与 \ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot; 结合使用（\ref cfg_interactive_svg &quot;INTERACTIVE_SVG&quot; 将设置为 `NO`）。</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>如果 \ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot; 设置为 `svg` 或 `svg:svg` 或 `svg:svg:core`，则可以将此选项设置为 \c YES 以启用允许缩放和平移的交互式 SVG 图像的生成。&lt;br&gt;请注意，这需要除 Internet Explorer 之外的现代浏览器。已测试并正常工作的有 Firefox、Chrome、Safari 和 Opera。&lt;br&gt;注意，当 \ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot; 设置为 `svg:cairo` 或 `svg:cairo:cairo` 时，此选项将自动禁用。</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>\c DOT_PATH 标签可用于指定可以找到 \c dot 工具的路径。如果留空，则假设 \c dot 工具可以在 \c path 中找到。</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>\c DOTFILE_DIRS 标签可用于指定一个或多个包含包含在文档中的 dot 文件的目录（请参阅 \ref cmddotfile &quot;\dotfile&quot; 命令）。</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>您可以在 Doxygen 文档中包含使用 dia 制作的图表。Doxygen 随后将运行 dia 生成图表并将其插入文档中。DIA_PATH 标签允许您指定 dia 二进制文件所在的目录。如果留空，则假设 dia 可以在默认搜索路径中找到。</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>\c DIAFILE_DIRS 标签可用于指定一个或多个包含包含在文档中的 dia 文件的目录（请参阅 \ref cmddiafile &quot;\diafile&quot; 命令）。</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>使用 PlantUML 时，\c PLANTUML_JAR_PATH 标签应用于指定 java 可以找到 \c plantuml.jar 文件的路径或要使用的 \c jar 文件的文件名。如果留空，则假设不使用 PlantUML 或在预处理步骤中调用。在这种情况下，Doxygen 在遇到 \ref cmdstartuml &quot;\startuml&quot; 命令时会生成警告，并且不会为图表生成输出。</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>使用 PlantUML 时，\c PLANTUML_CFG_FILE 标签可用于为 PlantUML 指定配置文件。</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>使用 PlantUML 时，指定的路径用于搜索 PlantUML 块中 \c !include 语句指定的文件。</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>\c PLANTUMLFILE_DIRS 标签可用于指定一个或多个包含包含在文档中的 PlantUml 文件的目录（请参阅 \ref cmdplantumlfile &quot;\plantumlfile&quot; 命令）。</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>使用带有 CLI 呈现的 Mermaid 图表时，\c MERMAID_PATH 标签应用于指定可以找到 \c mmdc (Mermaid CLI) 可执行文件的目录。如果留空，则禁用基于 CLI 的呈现。对于 HTML 输出，默认通过 JavaScript 使用客户端呈现，不需要 \c mmdc。对于 LaTeX/PDF 输出，需要 \c mmdc 来预生成图像。当需要 CLI 呈现但 \c mmdc 不可用时，Doxygen 将生成警告。</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>使用 Mermaid 图表时，\c MERMAID_CONFIG_FILE 标签可用于为 Mermaid CLI 工具（\c mmdc）指定 JSON 配置文件。此文件可以包含主题设置和其他 Mermaid 配置选项。</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>\c MERMAID_RENDER_MODE 标签选择如何呈现 Mermaid 图表。</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>\c MERMAID_JS_URL 标签指定在使用客户端呈现时（\c MERMAID_RENDER_MODE 为 \c CLIENT_SIDE 或 \c AUTO）从中加载 \c mermaid.js 的 URL。默认指向 jsDelivr CDN 上最新的 Mermaid v11 版本。\par 示例：- 最新 v11（默认）：\c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs&#x27; - 固定版本：\c &#x27;https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs&#x27; - 本地副本：\c &#x27;./mermaid.esm.min.mjs&#x27;（用户必须将文件放在 HTML 输出目录中）\par 默认 CDN URL 在查看生成的文档时需要互联网访问。对于离线使用，请下载 \c mermaid.esm.min.mjs 并将其设置为相对路径，或使用 \c MERMAID_RENDER_MODE=CLI 代替预生成图像。</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>\c MERMAIDFILE_DIRS 标签可用于指定一个或多个包含包含在文档中的 Mermaid 文件的目录（请参阅 \ref cmdmermaidfile &quot;\mermaidfile&quot; 命令）。</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>\c DOT_GRAPH_MAX_NODES 标签可用于设置图中显示的最大节点数。如果图中的节点数变得大于此值，Doxygen 将截断图，这通过将节点表示为红色框来可视化。请注意，如果图中根节点的直接子节点数已经大于 \c DOT_GRAPH_MAX_NODES，则根本不会显示图。另请注意，图的大小可以通过 \ref cfg_max_dot_graph_depth &quot;MAX_DOT_GRAPH_DEPTH&quot; 进一步限制。</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>\c MAX_DOT_GRAPH_DEPTH 标签可用于设置由 \c dot 生成的图的最大深度。深度值 3 意味着只显示从根通过最多 3 条边的路径可达的节点。距离根节点更远的节点将被省略。请注意，将此选项设置为 1 或 2 可能会大大减少大型代码库所需的计算时间。另请注意，图的大小可以通过 \ref cfg_dot_graph_max_nodes &quot;DOT_GRAPH_MAX_NODES&quot; 进一步限制。使用深度 0 表示没有深度限制。</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>将 \c DOT_MULTI_TARGETS 标签设置为 \c YES 以允许 dot 在一次运行中生成多个输出文件（即命令行上有多个 -o 和 -T 选项）。这使 \c dot 运行得更快，但由于只有更新版本的 \c dot（&gt;1.8.10）支持此功能，因此默认情况下禁用此功能。</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>如果 \c GENERATE_LEGEND 标签设置为 \c YES，Doxygen 将生成一个图例页面，解释 dot 生成的图中各种框和箭头的含义。\note 此标签要求未设置 \ref cfg_uml_look &quot;UML_LOOK&quot;，即使用 Doxygen 内部继承和协作图的图形表示。</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>如果 \c DOT_CLEANUP 标签设置为 \c YES，Doxygen 将删除用于生成各种图的中间文件。&lt;br&gt;注意：此设置不仅用于 dot 文件，还用于 msc 临时文件。</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>您可以使用 \ref cmdmsc &quot;\msc&quot; 命令在 Doxygen 注释中定义消息序列图。如果 \c MSCGEN_TOOL 标签留空（默认），则 Doxygen 将使用内置版本的 mscgen 工具来生成图表。或者，\c MSCGEN_TOOL 标签也可以指定外部工具的名称。例如，指定 `prog` 作为值，Doxygen 将调用工具为 `prog -T &lt;outfile_format&gt; -o &lt;outputfile&gt; &lt;inputfile&gt;`。外部工具应支持输出文件格式 &quot;png&quot;、&quot;eps&quot;、&quot;svg&quot; 和 &quot;ismap&quot;。</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>\c MSCFILE_DIRS 标签可用于指定一个或多个包含包含在文档中的 msc 文件的目录（请参阅 \ref cmdmscfile &quot;\mscfile&quot; 命令）。</translation>
    </message>
</context>
</TS>
