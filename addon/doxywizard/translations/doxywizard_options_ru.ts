<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>Этот тег указывает кодировку, используемую для всех символов в файле конфигурации, которые следуют за ним. Значение по умолчанию — UTF-8, которое также является кодировкой, используемой для всего текста перед первым появлением этого тега. Doxygen использует \c libiconv (или встроенный в \c libc iconv) для перекодирования. См. https://www.gnu.org/software/libiconv/ для списка возможных кодировок.</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>Тег \c PROJECT_NAME — это одно слово (или последовательность слов, заключенная в двойные кавычки, если вы не используете Doxywizard), которое должно идентифицировать проект, для которого создается документация. Это имя используется в заголовке большинства сгенерированных страниц и в некоторых других местах.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>Тег \c PROJECT_NUMBER можно использовать для ввода номера проекта или ревизии. Это может быть полезно для архивирования сгенерированной документации или при использовании системы контроля версий.</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>Используя тег \c PROJECT_BRIEF, можно предоставить необязательное однострочное описание проекта, которое появляется в верхней части каждой страницы и должно дать читателям быстрое представление о назначении проекта. Описание должно быть кратким.</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>С помощью тега \c PROJECT_LOGO можно указать логотип или значок, который будет включен в документацию. Максимальная высота логотипа не должна превышать 55 пикселей, а максимальная ширина — 200 пикселей. Doxygen скопирует логотип в выходной каталог.</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>С помощью тега \c PROJECT_ICON можно указать значок, который будет включен в вкладки при отображении HTML-документа. Doxygen скопирует логотип в выходной каталог.</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>Тег \c OUTPUT_DIRECTORY используется для указания (относительного или абсолютного) пути, по которому будет записана сгенерированная документация. Если введен относительный путь, он будет относительным к месту запуска Doxygen. Если оставить пустым, будет использоваться текущий каталог.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>Если тег \c CREATE_SUBDIRS установлен в \c YES, Doxygen создаст до 4096 подкаталогов (на 2 уровнях) в выходном каталоге каждого выходного формата и распределит сгенерированные файлы по этим каталогам. Включение этой опции может быть полезно, когда Doxygen получает большое количество исходных файлов, и размещение всех сгенерированных файлов в одном каталоге может вызвать проблемы с производительностью файловой системы. Настройте \c CREATE_SUBDIRS_LEVEL для управления количеством подкаталогов.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>Управляет количеством подкаталогов, которые будут созданы, когда тег \c CREATE_SUBDIRS установлен в \c YES. Уровень 0 представляет 16 каталогов, и каждое увеличение уровня удваивает количество каталогов, что дает 4096 каталогов на уровне 8, который является значением по умолчанию и максимальным значением. Подкаталоги организованы в 2 уровня, первый уровень всегда имеет фиксированное количество 16 каталогов.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>Если тег \c ALLOW_UNICODE_NAMES установлен в \c YES, Doxygen разрешит появление не-ASCII символов в именах сгенерированных файлов. Если установлен в \c NO, не-ASCII символы будут экранированы, например, _xE3_x81_x84 будет использоваться для Unicode U+3044.</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>Тег \c OUTPUT_LANGUAGE используется для указания языка, на котором будет написана вся документация, сгенерированная Doxygen. Doxygen будет использовать эту информацию для генерации всего постоянного вывода на соответствующем языке.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>Если тег \c BRIEF_MEMBER_DESC установлен в \c YES, Doxygen включит краткие описания членов после членов, перечисленных в документации файла и класса (аналогично \c Javadoc). Установите в \c NO, чтобы отключить это.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>Если тег \c REPEAT_BRIEF установлен в \c YES, Doxygen добавит краткое описание члена или функции перед подробным описанием. &amp;lt;br&amp;gt;Примечание: Если оба тега \ref cfg_hide_undoc_members &quot;HIDE_UNDOC_MEMBERS&quot; и \ref cfg_brief_member_desc &quot;BRIEF_MEMBER_DESC&quot; установлены в \c NO, краткие описания будут полностью подавлены.</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>Этот тег реализует почти интеллектуальный сокращатель кратких описаний, который используется для формирования текста в различных списках. Каждая строка в этом списке, если она найдена как начальный текст краткого описания, будет удалена из текста, и результат после обработки всего списка будет использован как аннотированный текст. В противном случае краткое описание используется как есть. Если оставить пустым, будут использоваться следующие значения (`$name` автоматически заменяется именем сущности):</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>Если оба тега \c ALWAYS_DETAILED_SEC и \ref cfg_repeat_brief &quot;REPEAT_BRIEF&quot; установлены в \c YES, Doxygen сгенерирует подробный раздел, даже если есть только краткое описание.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>Если тег \c INLINE_INHERITED_MEMB установлен в \c YES, Doxygen покажет все унаследованные члены в документации класса, как если бы эти члены были обычными членами класса. Конструкторы, деструкторы и операторы присваивания базового класса не будут показаны.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>Если тег \c FULL_PATH_NAMES установлен в \c YES, Doxygen добавит полный префикс пути к файлам в списке файлов и в заголовочных файлах. Если установлен в \c NO, будет использоваться самый короткий путь, который делает имена файлов уникальными.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>Тег \c STRIP_FROM_PATH можно использовать для удаления пользовательской части пути. Удаление выполняется только если одна из указанных строк совпадает с левой частью пути. Этот тег можно использовать для отображения относительных путей в списке файлов. Если оставить пустым, будет использоваться каталог, в котором запущен Doxygen, как путь для удаления. &amp;lt;br&amp;gt;Обратите внимание, что вы можете указать здесь абсолютные пути, но также и относительные пути, которые будут относительными к каталогу, в котором был запущен Doxygen.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>Тег \c STRIP_FROM_INC_PATH можно использовать для удаления пользовательской части пути, упомянутой в документации класса, которая сообщает читателю, какой заголовочный файл нужно включить, чтобы использовать класс. Если оставить пустым, будет использоваться только имя заголовочного файла, содержащего определение класса. В противном случае вы должны указать список путей включения, которые обычно передаются компилятору с флагом `-I`.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>Если тег \c SHORT_NAMES установлен в \c YES, Doxygen будет генерировать более короткие (но менее читаемые) имена файлов. Это может быть полезно, если ваша файловая система не поддерживает длинные имена (как DOS, Mac или CD-ROM).</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>Если тег \c JAVADOC_AUTOBRIEF установлен в \c YES, Doxygen будет интерпретировать первую строку комментариев в стиле Javadoc (до первой точки, вопросительного знака или восклицательного знака) как краткое описание. Если установлен в \c NO, стиль Javadoc будет вести себя как обычные комментарии в стиле Qt (поэтому требуется явная команда \ref cmdbrief &quot;\@brief&quot; для получения краткого описания).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>Если тег \c JAVADOC_BANNER установлен в \c YES, Doxygen будет интерпретировать строки типа \verbatim /***************\endverbatim как начало &quot;баннера&quot; комментария в стиле Javadoc. Если установлен в \c NO, стиль Javadoc будет вести себя как обычные комментарии, и Doxygen не будет интерпретировать его.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>Если тег \c QT_AUTOBRIEF установлен в \c YES, Doxygen будет интерпретировать первую строку комментариев в стиле Qt (до первой точки, вопросительного знака или восклицательного знака) как краткое описание. Если установлен в \c NO, стиль Qt будет вести себя как обычные комментарии в стиле Qt (поэтому требуется явная команда \ref cmdbrief &quot;\brief&quot; для получения краткого описания).</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>Тег \c MULTILINE_CPP_IS_BRIEF можно установить в \c YES, чтобы Doxygen обрабатывал многострочные блоки специальных комментариев C++ (т.е. блоки комментариев \c //! или \c ///) как краткие описания. Это раньше было поведением по умолчанию. Новое значение по умолчанию — обрабатывать многострочные блоки комментариев C++ как подробные описания. Если вы предпочитаете старое поведение, установите этот тег в \c YES. &amp;lt;br&amp;gt;Обратите внимание, что установка этого тега в \c YES также означает, что комментарии Rational Rose больше не будут распознаваться.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>По умолчанию строки документации Python отображаются как предварительно отформатированный текст и не могут использовать специальные команды Doxygen. Установив \c PYTHON_DOCSTRING в \c NO, можно использовать специальные команды Doxygen, и содержимое блока документации строки документации будет отображаться как документация Doxygen.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>Если тег \c INHERIT_DOCS установлен в \c YES, недокументированный член унаследует документацию от любого документированного члена, который он переопределяет.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>Если тег \c SEPARATE_MEMBER_PAGES установлен в \c YES, Doxygen будет генерировать новую страницу для каждого члена. Если установлен в \c NO, документация члена станет частью файла/класса/пространства имен, который его содержит.</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>Тег \c TAB_SIZE можно использовать для установки количества пробелов в табуляции. Doxygen использует это значение для замены табуляций пробелами во фрагментах кода.</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>Этот тег можно использовать для указания нескольких псевдонимов, которые будут действовать как команды в документации. Псевдоним имеет форму: \verbatim имя=значение\endverbatim Например, добавление \verbatim &quot;sideeffect=@par Побочные эффекты:^^&quot;\endverbatim позволит вам разместить команду \c \sideeffect (или \c \@sideeffect) в документации, что приведет к созданию пользовательского абзаца с заголовком &quot;Побочные эффекты:&quot;. Обратите внимание, что вы не можете поместить \ref cmdn &quot;\n&quot; в часть значения псевдонима для вставки разрыва строки (в результирующем выводе). Вы можете поместить `^^` в часть значения псевдонима для вставки разрыва строки, как если бы в исходном файле был физический разрыв строки.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>Если ваш проект содержит только исходный код C, установите тег \c OPTIMIZE_OUTPUT_FOR_C в \c YES. Doxygen затем сгенерирует вывод, более адаптированный для C. Например, некоторые из используемых имен будут отличаться. Список всех членов будет опущен и т.д.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>Если ваш проект содержит только исходный код Java или Python, установите тег \c OPTIMIZE_OUTPUT_JAVA в \c YES. Doxygen затем сгенерирует вывод, более адаптированный для языка. Например, пространства имен будут отображаться как пакеты, квалифицированные области будут выглядеть иначе и т.д.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>Если ваш проект содержит исходный код Fortran, установите тег \c OPTIMIZE_FOR_FORTRAN в \c YES. Doxygen затем сгенерирует вывод, подходящий для Fortran.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>Если ваш проект содержит исходный код VHDL, установите тег \c OPTIMIZE_OUTPUT_VHDL в \c YES. Doxygen затем сгенерирует вывод, подходящий для VHDL.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>Если ваш проект содержит только исходный код Slice, установите тег \c OPTIMIZE_OUTPUT_SLICE в \c YES. Doxygen затем сгенерирует вывод, более адаптированный для языка. Например, пространства имен будут отображаться как модули, типы будут разделены на больше групп и т.д.</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen выбирает парсер для использования на основе расширения анализируемого файла. С этим тегом вы можете назначить парсер для использования с данным расширением. Doxygen имеет встроенное сопоставление, но вы можете переопределить или расширить его с помощью этого тега. Формат: &amp;lt;code&amp;gt;ext=язык&amp;lt;/code&amp;gt;, где \c ext — расширение файла, а язык — один из парсеров, поддерживаемых Doxygen: IDL, Java, JavaScript, Csharp (C#), C, C++, Lex, D, PHP, md (Markdown), Objective-C, Python, Slice, VHDL, Fortran (Fortran с фиксированным форматом: FortranFixed, Fortran со свободным форматом: FortranFree, Fortran с неизвестным форматом: Fortran. В последнем случае парсер попытается угадать, является ли код фиксированным или свободным форматом, что является значением по умолчанию для файлов типа Fortran). Например, чтобы Doxygen обрабатывал файлы &amp;lt;code&amp;gt;.inc&amp;lt;/code&amp;gt; как файлы Fortran (по умолчанию PHP), а файлы &amp;lt;code&amp;gt;.f&amp;lt;/code&amp;gt; как файлы C (по умолчанию Fortran), используйте: `inc=Fortran f=C`. &amp;lt;br&amp;gt;Примечание: Для файлов без расширения вы можете использовать `no_extension` как заполнитель. &amp;lt;br&amp;gt;Обратите внимание, что для пользовательских расширений вам также нужно установить \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;, иначе Doxygen не будет читать эти файлы. Когда указан `no_extension`, вы должны добавить `*` в \ref cfg_file_patterns &quot;FILE_PATTERNS&quot;. &amp;lt;br&amp;gt;Примечание, см. также список \ref default_file_extension_mapping &quot;сопоставления расширений файлов по умолчанию&quot;.</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>Если тег \c MARKDOWN_SUPPORT включен, Doxygen будет предварительно обрабатывать все комментарии в соответствии с форматом Markdown, что позволяет создавать более читаемую документацию. См. https://daringfireball.net/projects/markdown/ для получения дополнительной информации. Вывод обработки Markdown дополнительно обрабатывается Doxygen, поэтому вы можете смешивать команды Doxygen, HTML и XML с форматированием Markdown. Отключайте это только в случае проблем с обратной совместимостью.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>Если тег \c MARKDOWN_STRICT включен, Doxygen будет обрабатывать текст в комментариях как формат Markdown, даже если собственный формат разметки Doxygen конфликтует с Markdown. Это актуально только при использовании обратных кавычек. Собственный стиль разметки Doxygen позволяет одинарной кавычке закрывать фрагмент текста, начинающийся с обратной кавычки, который затем рассматривается как цитируемый фрагмент текста, в то время как в Markdown такие фрагменты текста рассматриваются как буквальный текст и закрываются только при нахождении второй соответствующей обратной кавычки. Кроме того, собственный формат разметки Doxygen требует, чтобы двойные кавычки в разделе с обратными кавычками экранировались при их появлении, в то время как Markdown этого не требует.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>Когда тег \c TOC_INCLUDE_HEADINGS установлен в ненулевое значение, все заголовки до этого уровня будут автоматически включены в оглавление, даже если у них нет атрибута id. \note Эта функция в настоящее время работает только для заголовков Markdown.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>Тег \c MARKDOWN_ID_STYLE можно использовать для указания алгоритма, используемого для генерации идентификаторов для заголовков Markdown. Примечание: Каждый идентификатор уникален.</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>Когда включено, Doxygen будет пытаться связать слова, соответствующие документированному классу или пространству имен, с соответствующей документацией. Это можно предотвратить в отдельных случаях, поместив символ \c % перед словом, или глобально, установив \c AUTOLINK_SUPPORT в \c NO. Слова, перечисленные в теге \c AUTOLINK_IGNORE_WORDS, исключаются из автоматического связывания.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>Этот тег указывает список слов, которые при совпадении с началом слова в документации будут подавлять генерацию автоматических ссылок, если автоматическое связывание включено через AUTOLINK_SUPPORT. Этот список не влияет на ссылки, созданные явно с помощью команд \# или \ref cmdlink &quot;\link&quot; или \ref cmdref &quot;\ref&quot;.</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>Если вы используете классы STL (т.е. `std::string`, `std::vector` и т.д.), но не хотите включать исходный код (файл тегов) STL в качестве входных данных, вы должны установить этот тег в \c YES, чтобы Doxygen мог сопоставлять объявления и определения функций, чьи аргументы содержат классы STL (например, `func(std::string`); с `func(std::string) {}`). Это также делает диаграммы наследования и сотрудничества, включающие классы STL, более полными и точными.</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>Если вы используете язык C++/CLI от Microsoft, вы должны установить эту опцию в \c YES для включения поддержки синтаксического анализа.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>Если ваш проект содержит только &amp;lt;a href=&quot;https://www.riverbankcomputing.com/software&quot;&amp;gt;sip&amp;lt;/a&amp;gt; исходный код, установите тег \c SIP_SUPPORT в \c YES. Doxygen будет анализировать их как обычный C++, но будет предполагать, что все классы используют публичное наследование, когда нет явного ключевого слова защиты, вместо приватного наследования.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Для Microsoft IDL существуют атрибуты \c propget и \c propput для указания методов getter и setter для свойств. Установка этой опции в \c YES заставит Doxygen заменять методы get и set в документации на свойство. Это работает только если методы действительно получают или устанавливают простой тип. Если это не так, или вы хотите отображать методы в любом случае, вы должны установить эту опцию в \c NO.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>Если в документации используется группировка членов и тег \c DISTRIBUTE_GROUP_DOC установлен в \c YES, то Doxygen повторно использует документацию первого члена группы для других членов группы (если она есть). По умолчанию все члены группы должны быть явно документированы.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>Если структура или класс добавлены в группу и эта опция включена, любые вложенные классы или структуры также будут добавлены в ту же группу. По умолчанию эта опция отключена, и вложенные составные типы должны быть добавлены явно через \ref cmdingroup &quot;\ingroup&quot;.</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>Установите тег \c SUBGROUPING в \c YES, чтобы разрешить группам членов класса одного типа (например, группу публичных функций) быть помещенными в подгруппу этого типа (например, в разделе Публичные функции). Установите в \c NO, чтобы предотвратить подгруппировку. Альтернативно, это можно сделать для каждого класса с помощью команды \ref cmdnosubgrouping &quot;\nosubgrouping&quot;.</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>Когда тег \c INLINE_GROUPED_CLASSES установлен в \c YES, классы, структуры и объединения отображаются в содержащей их группе (например, с использованием \ref cmdingroup &quot;\ingroup&quot;), а не на отдельных страницах (для HTML и страниц Man) или в разделах (для \f$\mbox{\LaTeX}\f$ и RTF). &amp;lt;br&amp;gt;Обратите внимание, что эта функция не может быть объединена с \ref cfg_separate_member_pages &quot;SEPARATE_MEMBER_PAGES&quot;.</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>Когда тег \c INLINE_SIMPLE_STRUCTS установлен в \c YES, структуры, классы и объединения, которые имеют только публичные поля данных или простые поля typedef, будут отображаться в документации области, в которой они определены (т.е. документация файла, пространства имен или группы), при условии, что эта область документирована. Если установлено в \c NO, структуры, классы и объединения будут отображаться на отдельных страницах (для HTML и страниц Man) или в разделах (для \f$\mbox{\LaTeX}\f$ и RTF).</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>Когда тег \c TYPEDEF_HIDES_STRUCT включен, typedef для структуры, объединения или перечисления будет документирован как структура, объединение или перечисление с именем typedef. Таким образом, &amp;lt;code&amp;gt;typedef struct TypeS {} TypeT&amp;lt;/code&amp;gt; будет отображаться в документации как структура с именем \c TypeT. Когда отключено, typedef будет отображаться как член файла, пространства имен или класса. Структура будет называться \c TypeS. Это обычно полезно для кода C, если соглашение о кодировании требует, чтобы все составные типы были typedef, и на typedef ссылаются только по имени, никогда по имени тега.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>Размер кэша поиска символов можно установить с помощью \c LOOKUP_CACHE_SIZE. Этот кэш используется для разрешения символов на основе их имени и области видимости. Поскольку это может быть дорогостоящим процессом, и обычно одни и те же символы появляются много раз в коде, Doxygen поддерживает кэш предварительно проанализированных символов. Если кэш слишком мал, Doxygen станет медленнее. Если кэш слишком велик, память будет потрачена впустую. Размер кэша определяется следующей формулой: \f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$. Допустимый диапазон — 0..9, значение по умолчанию — 0, что соответствует размеру кэша в \f$2^{16} = 65536\f$ символов. В конце выполнения Doxygen сообщит об использовании кэша и предложит оптимальный размер кэша с точки зрения скорости.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>\c NUM_PROC_THREADS указывает количество потоков, которые Doxygen может использовать во время обработки. Когда установлено в \c 0, Doxygen установит его на основе количества ядер, доступных в системе. Вы можете установить его явно в значение больше 0, чтобы лучше контролировать баланс между нагрузкой на ЦП и скоростью обработки. В настоящее время только обработка входных данных может выполняться несколькими потоками. Поскольку это все еще экспериментальная функция, значение по умолчанию установлено в 1, что фактически отключает параллельную обработку. Пожалуйста, сообщайте о любых проблемах, с которыми вы столкнулись. Параллельная генерация диаграмм dot управляется настройкой \c DOT_NUM_THREADS.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>Если тег \c TIMESTAMP установлен в значение, отличное от \c NO, каждая сгенерированная страница будет содержать дату или дату и время, когда страница была сгенерирована. Установка в \c NO помогает сравнивать вывод нескольких запусков.</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>Если тег \c EXTRACT_ALL установлен в \c YES, Doxygen будет предполагать, что все сущности в документации документированы, даже если документация недоступна. Закрытые члены класса и статические члены файла будут скрыты, если только теги \ref cfg_extract_private &quot;EXTRACT_PRIVATE&quot; или \ref cfg_extract_static &quot;EXTRACT_STATIC&quot; не установлены в \c YES соответственно. \note Это также отключит предупреждения о недокументированных членах, которые обычно генерируются, когда \ref cfg_warnings &quot;WARNINGS&quot; установлен в \c YES.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>Если тег \c EXTRACT_PRIVATE установлен в \c YES, все закрытые члены класса будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>Если тег \c EXTRACT_PRIV_VIRTUAL установлен в \c YES, документированные закрытые виртуальные методы класса будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>Если тег \c EXTRACT_PACKAGE установлен в \c YES, все члены с областью видимости пакета или внутренней областью будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>Если тег \c EXTRACT_STATIC установлен в \c YES, все статические члены файла будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>Если тег \c EXTRACT_LOCAL_CLASSES установлен в \c YES, классы (и структуры), определенные локально в исходных файлах, будут включены в документацию. Если установлено в \c NO, будут включены только классы, определенные в заголовочных файлах. Не влияет на исходный код Java.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>Этот флаг полезен только для кода Objective-C. Если установлен в \c YES, локальные методы, определенные в разделе реализации, но не в интерфейсе, будут включены в документацию. Если установлено в \c NO, будут включены только методы в интерфейсе.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>Если этот флаг установлен в \c YES, члены анонимных пространств имен будут извлечены и показаны в документации как пространство имен с именем &amp;#x27;anonymous_namespace{file}&amp;#x27;, где file заменяется базовым именем файла, содержащего анонимное пространство имен. По умолчанию анонимные пространства имен скрыты.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>Если этот флаг установлен в \c YES, имя неименованных параметров в объявлении будет определено соответствующим определением. По умолчанию неименованные параметры остаются безымянными в выводе.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>Если тег \c HIDE_UNDOC_MEMBERS установлен в \c YES, Doxygen скроет все недокументированные члены внутри документированных классов или файлов. Если установлено в \c NO, эти члены будут включены в различные обзоры, но раздел документации не будет сгенерирован. Эта опция не имеет эффекта, если \ref cfg_extract_all &quot;EXTRACT_ALL&quot; включен.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>Если тег \c HIDE_UNDOC_CLASSES установлен в \c YES, Doxygen скроет все недокументированные классы, которые обычно были бы видны в иерархии классов. Если установлено в \c NO, эти классы будут включены в различные обзоры. Если включено, эта опция также скроет недокументированные концепции C++. Эта опция не имеет эффекта, если \ref cfg_extract_all &quot;EXTRACT_ALL&quot; включен.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>Если тег \c HIDE_UNDOC_NAMESPACES установлен в \c YES, Doxygen скроет все недокументированные пространства имен, которые обычно были бы видны в иерархии пространств имен. Если установлено в \c NO, эти пространства имен будут включены в различные обзоры. Эта опция не имеет эффекта, если \ref cfg_extract_all &quot;EXTRACT_ALL&quot; включен.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>Если тег \c HIDE_FRIEND_COMPOUNDS установлен в \c YES, Doxygen скроет все объявления friend. Если установлено в \c NO, эти объявления будут включены в документацию.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>Если тег \c HIDE_IN_BODY_DOCS установлен в \c YES, Doxygen скроет любые блоки документации, найденные внутри тела функции. Если установлено в \c NO, эти блоки будут добавлены к подробному блоку документации функции.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>Тег \c INTERNAL_DOCS определяет, будет ли включена документация, написанная после команды \ref cmdinternal &quot;\internal&quot;. Если тег установлен в \c NO, документация будет исключена. Установите в \c YES, чтобы включить внутреннюю документацию.</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>Правильно установив опцию \c CASE_SENSE_NAMES, Doxygen сможет лучше адаптироваться к возможностям базовой файловой системы. Если файловая система чувствительна к регистру (т.е. поддерживает файлы в одном каталоге, имена которых отличаются только регистром), вы должны установить опцию в \c YES, чтобы правильно обрабатывать такие файлы, которые появляются во входных данных. Для файловых систем, не чувствительных к регистру, вы должны установить опцию в \c NO, чтобы правильно обрабатывать выходные файлы, записанные для символов, которые отличаются только регистром, например, для двух классов, один с именем \c CLASS, а другой с именем \c Class, а также для поддержки ссылки на файлы без указания точного регистра. В Windows (включая Cygwin) и macOS пользователи обычно должны установить эту опцию в \c NO, в то время как в Linux или других системах Unix она обычно должна быть установлена в \c YES.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>Если тег \c HIDE_SCOPE_NAMES установлен в \c NO, Doxygen будет показывать члены с их полным классом и областью пространства имен в документации. Если установлено в \c YES, область будет скрыта.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>Если тег \c HIDE_COMPOUND_REFERENCE установлен в \c NO (по умолчанию), Doxygen добавит дополнительный текст после заголовка страницы, например, Справочник класса. Если установлено в \c YES, ссылка на составной тип будет скрыта.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>Если тег \c SHOW_HEADERFILE установлен в \c YES, документация класса покажет, какой заголовочный файл нужно включить, чтобы использовать класс.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>Если тег \c SHOW_INCLUDE_FILES установлен в \c YES, Doxygen поместит список файлов, включенных файлом, в документацию этого файла.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>Если тег \c SHOW_GROUPED_MEMB_INC установлен в \c YES, Doxygen добавит для каждой сгруппированной функции-члена файлы включения, которые нужны этой функции, помещая эти файлы включения в порядке, в котором они появлялись бы, если бы сгруппированная функция-член была встроенной. По умолчанию установлено в \c NO для сохранения обратной совместимости с предыдущим поведением.</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>Если тег \c FORCE_LOCAL_INCLUDES установлен в \c YES, файлы включения в документации функций, которые являются членами группы, будут записаны с двойными кавычками вместо угловых скобок.</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>Если тег \c INLINE_INFO установлен в \c YES, раздел с встроенной информацией будет включен в документацию, показывающий, какие функции являются встроенными.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>Если тег \c SORT_MEMBER_DOCS установлен в \c YES, Doxygen отсортирует (подробную) документацию членов файла и класса в алфавитном порядке по имени члена. Если установлено в \c NO, члены появятся в порядке объявления.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>Если тег \c SORT_BRIEF_DOCS установлен в \c YES, Doxygen отсортирует краткие описания членов файла, пространства имен и класса в алфавитном порядке по имени члена. Если установлено в \c NO, члены появятся в порядке объявления. Обратите внимание, что это также повлияет на порядок классов в списке классов.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>Если тег \c SORT_MEMBERS_CTORS_1ST установлен в \c YES, Doxygen отсортирует (краткую и подробную) документацию членов класса так, чтобы конструкторы и деструкторы были перечислены первыми. Если установлено в \c NO, конструкторы появятся в соответствующих порядках, определенных \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot; и \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot;. \note Если \ref cfg_sort_brief_docs &quot;SORT_BRIEF_DOCS&quot; установлен в \c NO, эта опция игнорируется при сортировке краткой документации членов. \note Если \ref cfg_sort_member_docs &quot;SORT_MEMBER_DOCS&quot; установлен в \c NO, эта опция игнорируется при сортировке подробной документации членов.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>Если тег \c SORT_GROUP_NAMES установлен в \c YES, Doxygen отсортирует иерархию имен групп в алфавитном порядке. Если установлено в \c NO, имена групп появятся в порядке их определения.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>Если тег \c SORT_BY_SCOPE_NAME установлен в \c YES, список классов будет отсортирован по полным именам (включая пространство имен). Если установлено в \c NO, список классов будет отсортирован только по имени класса, исключая часть пространства имен. \note Если \ref cfg_hide_scope_names &quot;HIDE_SCOPE_NAMES&quot; установлен в \c YES, эта опция менее полезна. \note Эта опция применяется только к списку классов, а не к алфавитному списку.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>Если опция \c STRICT_PROTO_MATCHING включена и Doxygen не может правильно проанализировать все типы аргументов функции, он отклонит соответствие между прототипом и реализацией функции-члена, даже если есть только один кандидат или если очевидно из простого строкового сопоставления, какой кандидат должен быть выбран. Отключение \c STRICT_PROTO_MATCHING все равно позволит Doxygen принимать соответствие между прототипом и реализацией в таком случае.</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>Тег \c GENERATE_TODOLIST можно использовать для включения (\c YES) или отключения (\c NO) списка задач. Этот список создается путем размещения команд \ref cmdtodo &quot;\todo&quot; в документации.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>Тег \c GENERATE_TESTLIST можно использовать для включения (\c YES) или отключения (\c NO) списка тестов. Этот список создается путем размещения команд \ref cmdtest &quot;\test&quot; в документации.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>Тег \c GENERATE_BUGLIST можно использовать для включения (\c YES) или отключения (\c NO) списка ошибок. Этот список создается путем размещения команд \ref cmdbug &quot;\bug&quot; в документации.</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>Тег \c GENERATE_DEPRECATEDLIST можно использовать для включения (\c YES) или отключения (\c NO) списка устаревших. Этот список создается путем размещения команд \ref cmddeprecated &quot;\deprecated&quot; в документации.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>Тег \c GENERATE_REQUIREMENTS можно использовать для включения (\c YES) или отключения (\c NO) страницы требований. Когда включено, эта страница создается автоматически, когда во входных данных появляется хотя бы один блок комментариев с командой \ref cmdrequirement &quot;\requirement&quot;.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>Тег \c REQ_TRACEABILITY_INFO управляет отображением информации о прослеживаемости на странице требований (актуально только при использовании блоков комментариев \ref cmdrequirement &quot;\requirement&quot;). Установка в \c NO полностью отключит информацию о прослеживаемости. Установка в \c UNSATISFIED_ONLY покажет список требований, которым не хватает отношения удовлетворения (через команду: \ref cmdsatisfies &quot;\satisfies&quot;). Аналогично, установка в \c UNVERIFIED_ONLY покажет список требований, которым не хватает отношения верификации (через команду: \ref cmdverifies &quot;\verifies&quot;). Установка тега в \c YES (по умолчанию) покажет оба списка, когда применимо.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>Тег \c ENABLED_SECTIONS можно использовать для включения условных разделов документации, помеченных блоками \ref cmdif &quot;\if&quot; \&amp;lt;section_label\&amp;gt; ... \ref cmdendif &quot;\endif&quot; и \ref cmdcond &quot;\cond&quot; \&amp;lt;section_label\&amp;gt; ... \ref cmdendcond &quot;\endcond&quot;.</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>Тег \c MAX_INITIALIZER_LINES определяет максимальное количество строк, которое начальное значение переменной или макроса/определения может отображать, чтобы появиться в документации. Если инициализатор содержит больше строк, чем указано здесь, он будет скрыт. Используйте значение 0, чтобы полностью скрыть инициализатор. Внешний вид значения отдельных переменных и макросов/определений можно контролировать в документации с помощью команд \ref cmdshowinitializer &quot;\showinitializer&quot; или \ref cmdhideinitializer &quot;\hideinitializer&quot;, независимо от этой настройки.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>Установите тег \c SHOW_USED_FILES в \c NO, чтобы отключить список файлов, генерируемый в нижней части документации классов и структур. Если установлено в \c YES, в списке будут упомянуты файлы, которые использовались для генерации документации.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>Установите тег \c SHOW_FILES в \c NO, чтобы отключить генерацию страниц файлов. Это удалит записи файлов из быстрого индекса и из древовидного представления папок (если указано).</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>Установите тег \c SHOW_NAMESPACES в \c NO, чтобы отключить генерацию страниц пространств имен. Это удалит записи пространств имен из быстрого индекса и из древовидного представления папок (если указано).</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>Тег \c FILE_VERSION_FILTER можно использовать для указания программы или скрипта, который Doxygen должен вызвать для получения текущей версии каждого файла (обычно из системы контроля версий). Doxygen вызовет программу, выполнив (через &amp;lt;code&amp;gt;popen()&amp;lt;/code&amp;gt;) команду &amp;lt;code&amp;gt;command input-file&amp;lt;/code&amp;gt;, где \c command — значение тега \c FILE_VERSION_FILTER, а \c input-file — имя входного файла, предоставленного Doxygen. Все, что программа выведет в стандартный вывод, будет использовано как версия файла.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>Тег \c LAYOUT_FILE можно использовать для указания файла макета, который Doxygen проанализирует. Файл макета управляет глобальной структурой сгенерированных выходных файлов независимым от формата вывода способом. Чтобы создать файл макета, представляющий настройки Doxygen по умолчанию, запустите Doxygen с опцией `-l`. Вы можете опционально указать имя файла после опции, если опустите, \c DoxygenLayout.xml будет использовано как имя файла макета. См. также раздел \ref layout для информации. &amp;lt;br&amp;gt;Обратите внимание, что если вы запускаете Doxygen из каталога, содержащего файл с именем \c DoxygenLayout.xml, Doxygen автоматически проанализирует его, даже если тег \c LAYOUT_FILE пуст.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>Тег \c CITE_BIB_FILES можно использовать для указания одного или нескольких файлов \c bib, содержащих определения ссылок. Это должен быть список файлов &amp;lt;code&amp;gt;.bib&amp;lt;/code&amp;gt;. Расширение &amp;lt;code&amp;gt;.bib&amp;lt;/code&amp;gt; будет добавлено автоматически, если опущено. Это требует, чтобы инструмент \c bibtex был установлен. См. также https://en.wikipedia.org/wiki/BibTeX для дополнительной информации. Для \f$\mbox{\LaTeX}\f$ стиль ссылок может контролироваться с помощью \ref cfg_latex_bib_style &quot;LATEX_BIB_STYLE&quot;. Чтобы использовать эту функцию, вам нужно иметь \c bibtex и \c perl доступными в пути поиска. См. также \ref cmdcite &quot;\cite&quot; для информации о создании цитат.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>Тег \c EXTERNAL_TOOL_PATH можно использовать для расширения пути поиска (переменная окружения PATH), чтобы внешние инструменты, такие как \c latex и \c gs, могли быть найдены. \note Каталоги, указанные с EXTERNAL_TOOL_PATH, добавляются перед путями, уже указанными переменной PATH, и добавляются в указанном порядке. \note Эта опция особенно полезна для macOS версии 14 (Sonoma) и выше, когда Doxygen запускается из Doxywizard, потому что в этом случае любые пользовательские изменения PATH игнорируются. Типичный пример на macOS — установить \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim Вместе со стандартными путями полный путь поиска, который Doxygen использует при запуске внешних инструментов, станет \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>Тег \c QUIET можно использовать для включения/выключения сообщений, генерируемых Doxygen на стандартный вывод. Если \c QUIET установлен в \c YES, это означает, что сообщения выключены.</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>Тег \c WARNINGS можно использовать для включения/выключения предупреждающих сообщений, генерируемых Doxygen на стандартный поток ошибок (\c stderr). Если \c WARNINGS установлен в \c YES, это означает, что предупреждения включены. &amp;lt;br&amp;gt;\b Совет: Включите предупреждения при написании документации.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>Если тег \c WARN_IF_UNDOCUMENTED установлен в \c YES, Doxygen будет генерировать предупреждения для недокументированных членов. Если \ref cfg_extract_all &quot;EXTRACT_ALL&quot; установлен в \c YES, этот флаг будет автоматически отключен.</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>Если тег \c WARN_IF_DOC_ERROR установлен в \c YES, Doxygen будет генерировать предупреждения о потенциальных ошибках в документации, таких как документирование некоторых параметров в документированной функции дважды, или документирование несуществующих параметров, или неправильное использование команд разметки.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>Если тег \c WARN_IF_INCOMPLETE_DOC установлен в \c YES, Doxygen будет генерировать предупреждения о потенциально неполной документации, такой как отсутствие документации всех параметров функции или отсутствие документации возвращаемого значения, когда функция имеет тип возврата, отличный от void.</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>Если тег \c WARN_NO_PARAMDOC установлен в \c YES, Doxygen будет предупреждать, если блок документации для функции или метода не содержит раздел \ref cmdparam &quot;\param&quot; для каждого параметра, присутствующего в объявлении, или раздел \ref cmdreturn &quot;\return&quot; для функций с типом возврата, отличным от void.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>Если тег \c WARN_IF_UNDOC_ENUM_VAL установлен в \c YES, Doxygen будет генерировать предупреждения для недокументированных значений перечислений. Если \ref cfg_extract_all &quot;EXTRACT_ALL&quot; установлен в \c YES, этот флаг будет автоматически отключен.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>Если тег \c WARN_LAYOUT_FILE установлен в \c YES, Doxygen будет предупреждать, если файл макета указан и не может быть найден. Если установлено в \c NO, Doxygen молча использует файл макета по умолчанию.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>Если тег \c WARN_AS_ERROR установлен в \c YES, Doxygen будет рассматривать все предупреждения как ошибки и прерывать обработку при возникновении предупреждений. Если установлено в \c NO, предупреждения будут рассматриваться только как предупреждения.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>Тег \c WARN_FORMAT определяет формат предупреждающих сообщений, генерируемых Doxygen. Строка должна содержать теги \c $file, \c $line и \c $text, которые будут заменены именем файла, номером строки и текстом предупреждения соответственно.</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>Тег \c WARN_LINE_FORMAT определяет формат номера строки в предупреждающих сообщениях, генерируемых Doxygen. Строка должна содержать тег \c $line, который будет заменен номером строки.</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>Тег \c WARN_LOGFILE можно использовать для указания файла, в который Doxygen может записывать предупреждающие и ошибочные сообщения, которые он генерирует. Если оставить пустым, вывод будет записан в стандартный поток ошибок (\c stderr).</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>Тег \c INPUT используется для указания файлов и/или каталогов, содержащих источники документации. Вы можете вводить имена файлов с шаблоном (например, &amp;lt;code&amp;gt;*.cpp&amp;lt;/code&amp;gt;) и каталоги для добавления во входные данные. Вы также можете вводить имена файлов, относительные к каталогу, из которого был запущен Doxygen. Если оставить пустым, будет использоваться текущий каталог.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>Этот тег можно использовать для указания кодировки, используемой во входных данных. Doxygen использует эту информацию для анализа входных файлов. См. https://en.wikipedia.org/wiki/Character_encoding для дополнительной информации. Кодировка по умолчанию — UTF-8.</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>Этот тег можно использовать для указания кодировки для каждого файла в списке входных данных. Если тег опущен или пуст, будет использоваться кодировка, указанная тегом \c INPUT_ENCODING.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>Если значение тега \c INPUT содержит каталоги, вы можете использовать тег \c FILE_PATTERNS для указания одного или нескольких шаблонов (например, &amp;lt;code&amp;gt;*.c&amp;lt;/code&amp;gt; и &amp;lt;code&amp;gt;*.h&amp;lt;/code&amp;gt;) для фильтрации файлов в этих каталогах. Если оставить пустым, будут использоваться следующие шаблоны: `*.c, *.cc, *.cxx, *.cpp, *.c++, *.java, *.ii, *.ixx, *.ipp, *.i++, *.inl, *.idl, *.ddl, *.odl, *.h, *.hh, *.hxx, *.hpp, *.h++, *.l, *.cs, *.d, *.php, *.php4, *.php5, *.phtml, *.inc, *.m, *.markdown, *.md, *.mm, *.dox (для предоставления примеров), *.py, *.f90, *.f, *.for, *.tcl, *.vhd, *.vhdl, *.ucf, *.qsf и *.ice.</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>Если тег \c RECURSIVE установлен в \c YES, подкаталоги будут искаться для входных файлов для обработки. Если установлено в \c NO, будет искаться только текущий каталог и указанные файлы.</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>Тег \c EXCLUDE можно использовать для указания файлов и/или каталогов, которые должны быть исключены из источника \c INPUT. Это позволяет использовать простые фильтры на входе.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>Если тег \c EXCLUDE_SYMLINKS установлен в \c YES, символические ссылки (на файлы или каталоги) будут исключены из входных данных.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>Если тег \c EXCLUDE содержит каталоги, вы можете использовать тег \c EXCLUDE_PATTERNS для указания одного или нескольких шаблонов для исключения файлов в этих каталогах из обработки.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>Тег \c EXCLUDE_SYMBOLS можно использовать для указания одного или нескольких имен символов (пространств имен, классов, функций, макросов и т.д.), которые должны быть исключены из документации. Имя символа может быть полным именем, словом или шаблоном, использующим \c * и \c ? как подстановочные знаки.</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>Тег \c EXAMPLE_PATH можно использовать для указания одного или нескольких файлов или каталогов, содержащих примеры фрагментов кода, которые Doxygen включит в документацию.</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>Если значение тега \c EXAMPLE_PATH содержит каталоги, вы можете использовать тег \c EXAMPLE_PATTERNS для указания одного или нескольких шаблонов для фильтрации файлов в этих каталогах.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>Если тег \c EXAMPLE_RECURSIVE установлен в \c YES, подкаталоги будут искаться для примеров файлов для обработки. Если установлено в \c NO, будет искаться только текущий каталог и указанные файлы.</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>Тег \c IMAGE_PATH можно использовать для указания одного или нескольких файлов или каталогов, содержащих изображения, которые должны быть включены в документацию. Путь относителен к каталогу, из которого был запущен Doxygen.</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>Тег \c INPUT_FILTER можно использовать для указания программы, которая должна быть вызвана для каждого входного файла перед тем, как Doxygen обработает его. Программа вызывается с именем файла в качестве аргумента и должна вывести содержимое файла в стандартный вывод. Если программа возвращает ненулевой код выхода, файл будет исключен из обработки.</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>Тег \c FILTER_PATTERNS можно использовать для указания фильтров для конкретных шаблонов файлов. Каждый элемент имеет форму `шаблон=фильтр`. Шаблон — это выражение с подстановочными знаками, например `*.cpp`, а фильтр — это имя программы, которая должна быть вызвана.</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>Если тег \c FILTER_SOURCE_FILES установлен в \c YES, фильтр ввода также будет применен к исходным файлам перед тем, как они будут показаны в документации.</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>Тег \c FILTER_SOURCE_PATTERNS можно использовать для указания фильтров исходного кода для конкретных шаблонов файлов. Он работает аналогично \c FILTER_PATTERNS, но применяется к исходным файлам.</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>Тег \c USE_MDFILE_AS_MAINPAGE можно использовать для указания имени файла Markdown, который будет использоваться как главная страница. Если оставить пустым, Doxygen попытается найти файл с именем `mainpage.md` во входном каталоге.</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>Если тег \c IMPLICIT_DIR_DOCS установлен в \c YES, Doxygen будет генерировать документацию для каталогов, даже если у них нет явной документации. Если установлено в \c NO, будут документированы только каталоги с явной документацией.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Тег \c FORTRAN_COMMENT_AFTER можно использовать для указания строки, которая ожидается после комментария Fortran. По умолчанию установлено в `!`, что означает, что комментарии должны начинаться с `!`.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>Если тег \c SOURCE_BROWSER установлен в \c YES, будет сгенерирован список исходных файлов. Для вывода HTML исходные файлы также будут показаны с подсветкой синтаксиса.</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>Если тег \c INLINE_SOURCES установлен в \c YES, тело функций, классов и пространств имен будет вставлено непосредственно в документацию.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>Если тег \c STRIP_CODE_COMMENTS установлен в \c YES, специфические для Doxygen комментарии будут удалены из исходного кода перед его отображением.</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>Если тег \c REFERENCED_BY_RELATION установлен в \c YES, для каждой документированной функции будет создан список всех функций, которые вызывают эту функцию.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>Если тег \c REFERENCES_RELATION установлен в \c YES, для каждой документированной функции будет создан список всех документированных сущностей, которые вызываются/используются этой функцией.</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>Если тег \c REFERENCES_LINK_SOURCE установлен в \c YES, ссылки из списков ссылок будут указывать на исходный код. Если установлено в \c NO, они будут указывать на документацию.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>Если тег \c SOURCE_TOOLTIPS установлен в \c YES, всплывающие подсказки будут отображаться для объявлений и определений функций в исходном коде.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>Если тег \c USE_HTAGS установлен в \c YES, инструмент HTAGS будет использоваться для генерации ссылок на исходный код. Это требует, чтобы инструмент HTAGS был установлен и доступен в пути.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>Если тег \c VERBATIM_HEADERS установлен в \c YES, заголовочные файлы будут отображаться в браузере исходного кода с подсветкой синтаксиса.</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>Если тег \c CLANG_ASSISTED_PARSING установлен в \c YES, Clang будет использоваться для анализа исходного кода. Это может дать более точные результаты, но требует, чтобы Clang был установлен.</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>Тег \c CLANG_ADD_INC_PATHS можно использовать для указания дополнительных путей включения для Clang.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>Тег \c CLANG_OPTIONS можно использовать для указания дополнительных опций для Clang.</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>Тег \c CLANG_DATABASE_PATH можно использовать для указания пути к базе данных компиляции Clang.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>Если тег \c ALPHABETICAL_INDEX установлен в \c YES, будет сгенерирован алфавитный указатель всех классов, функций, переменных и т.д.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>Тег \c IGNORE_PREFIX можно использовать для указания одного или нескольких префиксов, которые должны игнорироваться в алфавитном указателе.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>Если тег \c GENERATE_HTML установлен в \c YES, будет сгенерирован вывод HTML.</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>Тег \c HTML_OUTPUT используется для указания выходного каталога для HTML файлов. Если введен относительный путь, он будет добавлен к пути, указанному \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot;.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>Тег \c HTML_FILE_EXTENSION можно использовать для указания расширения файла для каждой сгенерированной HTML страницы. По умолчанию установлено в `.html`.</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>Тег \c HTML_HEADER можно использовать для указания пользовательского HTML файла заголовка, который будет вставлен в каждую сгенерированную HTML страницу.</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>Тег \c HTML_FOOTER можно использовать для указания пользовательского HTML файла нижнего колонтитула, который будет вставлен в каждую сгенерированную HTML страницу.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>Тег \c HTML_STYLESHEET можно использовать для указания пользовательского CSS файла, который будет вставлен в вывод HTML.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>Тег \c HTML_EXTRA_STYLESHEET можно использовать для указания дополнительных CSS файлов, которые будут вставлены в вывод HTML.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>Тег \c HTML_EXTRA_FILES можно использовать для указания дополнительных файлов, которые должны быть скопированы в выходной каталог HTML.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>Тег \c HTML_COLORSTYLE можно использовать для указания, должен ли сгенерированный HTML вывод быть представлен в темной или светлой теме.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>Тег \c HTML_COLORSTYLE_HUE управляет цветом вывода HTML. Doxygen настроит цвета в таблице стилей и фоновом изображении на основе этого цвета.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>Тег \c HTML_COLORSTYLE_SAT управляет чистотой (или насыщенностью) цветов в выводе HTML. Значение 0 заставит вывод использовать только оттенки серого. Значение 255 даст самые яркие цвета.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>Тег \c HTML_COLORSTYLE_GAMMA управляет гамма-коррекцией, применяемой к компоненте яркости цветов в выводе HTML.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>Если тег \c HTML_DYNAMIC_MENUS установлен в \c YES, сгенерированная HTML документация будет содержать главный индекс с вертикальным навигационным меню, создаваемым динамически с помощью JavaScript.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>Если тег \c HTML_DYNAMIC_SECTIONS установлен в \c YES, сгенерированная HTML документация будет содержать разделы, которые можно скрыть и показать после загрузки страницы.</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>Если тег \c HTML_CODE_FOLDING установлен в \c YES, классы и функции могут быть динамически свернуты и развернуты в сгенерированном HTML исходном коде.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>Если тег \c HTML_COPY_CLIPBOARD установлен в \c YES, Doxygen покажет значок в правом верхнем углу фрагментов кода и текста, позволяющий пользователю копировать их содержимое в буфер обмена.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen сохраняет некоторые настройки в браузере постоянно (например, через cookies). По умолчанию эти настройки применяются ко всем HTML страницам, которые Doxygen генерирует во всех проектах. Тег \c HTML_PROJECT_COOKIE можно использовать для сохранения настроек под проектно-специфичным ключом.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>С помощью \c HTML_INDEX_NUM_ENTRIES вы можете управлять предпочтительным количеством записей, отображаемых изначально в различных древовидных индексах.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>Если тег \c GENERATE_DOCSET установлен в \c YES, будут сгенерированы дополнительные индексные файлы, которые могут использоваться как входные данные для &amp;lt;a href=&quot;https://developer.apple.com/xcode/&quot;&amp;gt;интегрированной среды разработки Apple Xcode 3&amp;lt;/a&amp;gt;.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>Этот тег определяет имя набора документации. Набор документации предоставляет зонтичный термин, под которым могут быть сгруппированы несколько наборов документации от одного поставщика (например, компании или набора продуктов).</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>Этот тег определяет URL набора документации.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>Этот тег указывает строку, которая должна однозначно идентифицировать пакет набора документации. Это должна быть строка в обратном доменном стиле, например &amp;lt;code&amp;gt;com.mycompany.MyDocSet&amp;lt;/code&amp;gt;.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>Тег \c DOCSET_PUBLISHER_ID указывает строку, которая должна однозначно идентифицировать издателя документации.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>Тег \c DOCSET_PUBLISHER_NAME идентифицирует издателя документации.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>Если тег \c GENERATE_HTMLHELP установлен в \c YES, Doxygen генерирует три дополнительных HTML индексных файла: \c index.hhp, \c index.hhc и \c index.hhk.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>Тег \c CHM_FILE можно использовать для указания имени файла сгенерированного `.chm` файла.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>Тег \c HHC_LOCATION можно использовать для указания местоположения (абсолютный путь, включая имя файла) компилятора HTML помощи (\c hhc.exe).</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>Флаг \c GENERATE_CHI управляет тем, будет ли сгенерирован отдельный `.chi` индексный файл (\c YES) или включен в основной `.chm` файл (\c NO).</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>\c CHM_INDEX_ENCODING используется для кодирования содержимого индексного файла HtmlHelp (\c hhk), файла содержимого (\c hhc) и файла проекта.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>Флаг \c BINARY_TOC управляет тем, будет ли сгенерировано двоичное оглавление в файле `.chm` (\c YES) или обычное оглавление (\c NO).</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>Флаг \c TOC_EXPAND можно установить в \c YES, чтобы добавить дополнительные элементы для членов групп в оглавление и древовидное представление документации HTML помощи.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>Тег \c SITEMAP_URL используется для указания полного URL места, где пользователь разместит сгенерированную документацию на сервере во время развертывания.</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>Если тег \c GENERATE_QHP установлен в \c YES, будет сгенерирован сжатый файл помощи Qt (`.qch`).</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>Тег \c QCH_FILE можно использовать для указания имени файла сгенерированного сжатого файла помощи Qt.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>Тег \c QHP_NAMESPACE указывает пространство имен, которое будет использоваться для сгенерированного файла проекта помощи Qt.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>Тег \c QHP_VIRTUAL_FOLDER указывает виртуальную папку, которая будет использоваться для сгенерированного файла проекта помощи Qt.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>Тег \c QHP_CUST_FILTER_NAME указывает имя пользовательского фильтра, который может использоваться для фильтрации документации.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>Тег \c QHP_CUST_FILTER_ATTRS указывает атрибуты пользовательского фильтра.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>Тег \c QHP_SECT_FILTER_ATTRS указывает атрибуты, которые будут использоваться для фильтрации разделов.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>Тег \c QHG_LOCATION можно использовать для указания местоположения генератора помощи Qt.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>Если тег \c GENERATE_ECLIPSEHELP установлен в \c YES, будет сгенерирован файл помощи Eclipse.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>Тег \c ECLIPSE_DOC_ID указывает идентификатор документации, который будет использоваться для сгенерированной помощи Eclipse.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>Если тег \c DISABLE_INDEX установлен в \c YES, индекс будет отключен в выводе HTML.</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>Если тег \c GENERATE_TREEVIEW установлен в \c YES, будет сгенерировано древовидное представление в выводе HTML.</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>Если тег \c PAGE_OUTLINE_PANEL установлен в \c YES, будет сгенерирована панель структуры в выводе HTML.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>Если тег \c FULL_SIDEBAR установлен в \c YES, будет сгенерирована полная боковая панель в выводе HTML.</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>Тег \c ENUM_VALUES_PER_LINE определяет количество значений перечислений, отображаемых в каждой строке в выводе HTML.</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>Тег \c SHOW_ENUM_VALUES управляет отображением значений перечислений в выводе HTML.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>Тег \c TREEVIEW_WIDTH определяет ширину древовидного представления в выводе HTML.</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>Если тег \c EXT_LINKS_IN_WINDOW установлен в \c YES, внешние ссылки будут открываться в новом окне.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>Если тег \c OBFUSCATE_EMAILS установлен в \c YES, адреса электронной почты будут запутаны в выводе HTML.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>Тег \c HTML_FORMULA_FORMAT определяет формат, в котором формулы отображаются в выводе HTML.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>Тег \c FORMULA_FONTSIZE определяет размер шрифта, используемый для формул.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>Тег \c FORMULA_MACROFILE можно использовать для указания файла с определениями макросов для формул.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>Если тег \c USE_MATHJAX установлен в \c YES, MathJax будет использоваться для рендеринга формул в выводе HTML.</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>Тег \c MATHJAX_VERSION определяет версию MathJax, которая используется.</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>Тег \c MATHJAX_FORMAT определяет выходной формат, который использует MathJax.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>Тег \c MATHJAX_RELPATH определяет относительный путь к файлам MathJax.</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>Тег \c MATHJAX_EXTENSIONS можно использовать для указания дополнительных расширений MathJax.</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>Тег \c MATHJAX_CODEFILE можно использовать для указания дополнительных файлов конфигурации MathJax.</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>Если тег \c SEARCHENGINE установлен в \c YES, поисковая система будет интегрирована в вывод HTML.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>Если тег \c SERVER_BASED_SEARCH установлен в \c YES, будет использоваться серверный поиск.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>Если тег \c EXTERNAL_SEARCH установлен в \c YES, будет использоваться внешняя поисковая система.</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>Тег \c SEARCHENGINE_URL указывает URL внешней поисковой системы.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>Тег \c SEARCHDATA_FILE указывает имя файла данных поиска.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>Тег \c EXTERNAL_SEARCH_ID указывает идентификатор внешней поисковой системы.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>Тег \c EXTRA_SEARCH_MAPPINGS можно использовать для указания дополнительных сопоставлений поиска.</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>Если тег \c GENERATE_LATEX установлен в \c YES, будет сгенерирован вывод LaTeX.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>Тег \c LATEX_OUTPUT используется для указания выходного каталога для файлов LaTeX.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>Тег \c LATEX_CMD_NAME указывает имя команды LaTeX.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>Тег \c MAKEINDEX_CMD_NAME указывает имя команды MakeIndex.</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>Тег \c LATEX_MAKEINDEX_CMD указывает команду, используемую для создания индекса.</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>Если тег \c COMPACT_LATEX установлен в \c YES, будет сгенерирован компактный вывод LaTeX.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>Тег \c PAPER_TYPE определяет размер бумаги для вывода LaTeX.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>Тег \c EXTRA_PACKAGES можно использовать для указания дополнительных пакетов LaTeX.</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>Тег \c LATEX_HEADER можно использовать для указания пользовательского файла заголовка LaTeX.</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>Тег \c LATEX_FOOTER можно использовать для указания пользовательского файла нижнего колонтитула LaTeX.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>Тег \c LATEX_EXTRA_STYLESHEET можно использовать для указания дополнительных файлов таблиц стилей LaTeX.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>Тег \c LATEX_EXTRA_FILES можно использовать для указания дополнительных файлов, которые должны быть скопированы в выходной каталог LaTeX.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>Если тег \c PDF_HYPERLINKS установлен в \c YES, будут сгенерированы PDF гиперссылки в выводе LaTeX.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>Если тег \c USE_PDFLATEX установлен в \c YES, pdfLaTeX будет использоваться для генерации PDF файлов.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>Если тег \c LATEX_BATCHMODE установлен в \c YES, LaTeX будет выполняться в пакетном режиме.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>Если тег \c LATEX_HIDE_INDICES установлен в \c YES, индексы будут скрыты в выводе LaTeX.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>Тег \c LATEX_BIB_STYLE определяет стиль, используемый для библиографических записей.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>Тег \c LATEX_EMOJI_DIRECTORY указывает каталог, где хранятся файлы эмодзи для вывода LaTeX.</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>Если тег \c GENERATE_RTF установлен в \c YES, будет сгенерирован вывод RTF.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>Тег \c RTF_OUTPUT используется для указания выходного каталога для файлов RTF.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>Если тег \c COMPACT_RTF установлен в \c YES, будет сгенерирован компактный вывод RTF.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>Если тег \c RTF_HYPERLINKS установлен в \c YES, будут сгенерированы гиперссылки в выводе RTF.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>Тег \c RTF_STYLESHEET_FILE можно использовать для указания пользовательского файла таблицы стилей RTF.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>Тег \c RTF_EXTENSIONS_FILE можно использовать для указания пользовательского файла расширений RTF.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>Тег \c RTF_EXTRA_FILES можно использовать для указания дополнительных файлов, которые должны быть скопированы в выходной каталог RTF.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>Если тег \c GENERATE_MAN установлен в \c YES, будет сгенерирован вывод страниц Man.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>Тег \c MAN_OUTPUT используется для указания выходного каталога для страниц Man.</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>Тег \c MAN_EXTENSION определяет расширение файла для страниц Man.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>Тег \c MAN_SUBDIR определяет подкаталог для страниц Man.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>Если тег \c MAN_LINKS установлен в \c YES, будут сгенерированы ссылки на страницах Man.</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>Если тег \c GENERATE_XML установлен в \c YES, будет сгенерирован вывод XML.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>Тег \c XML_OUTPUT используется для указания выходного каталога для файлов XML.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>Если тег \c XML_PROGRAMLISTING установлен в \c YES, исходный код будет включен в вывод XML.</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>Если тег \c XML_NS_MEMB_FILE_SCOPE установлен в \c YES, члены пространства имен в области файла будут включены в вывод XML.</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>Если тег \c GENERATE_DOCBOOK установлен в \c YES, будет сгенерирован вывод DocBook.</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>Тег \c DOCBOOK_OUTPUT используется для указания выходного каталога для файлов DocBook.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>Если тег \c GENERATE_AUTOGEN_DEF установлен в \c YES, будет сгенерирован файл определений AutoGen.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>Если тег \c GENERATE_SQLITE3 установлен в \c YES, Doxygen сгенерирует базу данных \c Sqlite3 с символами, найденными Doxygen, сохраненными в таблицах.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>Тег \c SQLITE3_OUTPUT используется для указания, где будет размещена база данных \c Sqlite3. Если введен относительный путь, значение \ref cfg_output_directory &quot;OUTPUT_DIRECTORY&quot; будет добавлено в начало.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>Если тег \c SQLITE3_RECREATE_DB установлен в \c YES, существующий файл базы данных doxygen_sqlite3.db будет создаваться заново при каждом запуске Doxygen. Если установлено в \c NO, Doxygen предупредит, если файл базы данных уже найден, и не изменит его.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>Если тег \c GENERATE_PERLMOD установлен в \c YES, Doxygen сгенерирует файл модуля Perl, который захватывает структуру кода (включая всю документацию).</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>Если тег \c PERLMOD_LATEX установлен в \c YES, формулы LaTeX будут сгенерированы в выводе модуля Perl.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>Если тег \c PERLMOD_PRETTY установлен в \c YES, вывод модуля Perl будет красиво отформатирован.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>Тег \c PERLMOD_MAKEVAR_PREFIX можно использовать для указания префикса для переменных Make в выводе модуля Perl.</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>Если тег \c ENABLE_PREPROCESSING установлен в \c YES, препроцессор C будет применен к исходному коду.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>Если тег \c MACRO_EXPANSION установлен в \c YES, макросы будут развернуты в исходном коде.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>Если тег \c EXPAND_ONLY_PREDEF установлен в \c YES, будут развернуты только предопределенные макросы.</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>Если тег \c SEARCH_INCLUDES установлен в \c YES, включаемые файлы будут искаться для определений макросов.</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>Тег \c INCLUDE_PATH можно использовать для указания дополнительных путей включения для препроцессора.</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>Тег \c INCLUDE_FILE_PATTERNS можно использовать для указания шаблонов для включаемых файлов.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>Тег \c PREDEFINED можно использовать для указания предопределенных макросов.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>Тег \c EXPAND_AS_DEFINED можно использовать для указания макросов, которые будут развернуты как определенные.</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>Если тег \c SKIP_FUNCTION_MACROS установлен в \c YES, макросы функций будут пропущены.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>Тег \c TAGFILES можно использовать для указания внешних файлов тегов.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>Тег \c GENERATE_TAGFILE можно использовать для указания имени файла тегов, который будет сгенерирован.</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>Если тег \c ALLEXTERNALS установлен в \c YES, все внешние классы и пространства имен будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>Если тег \c EXTERNAL_GROUPS установлен в \c YES, внешние группы будут включены в документацию.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>Если тег \c EXTERNAL_PAGES установлен в \c YES, внешние страницы будут включены в документацию.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>Если тег \c HIDE_UNDOC_RELATIONS установлен в \c YES, недокументированные отношения будут скрыты в диаграммах классов.</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>Если тег \c HAVE_DOT установлен в \c YES, Graphviz/dot будет использоваться для генерации диаграмм.</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>Тег \c DOT_NUM_THREADS определяет количество потоков, используемых dot.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>Тег \c DOT_COMMON_ATTR можно использовать для указания общих атрибутов для диаграмм dot.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>Тег \c DOT_EDGE_ATTR можно использовать для указания атрибутов ребер для диаграмм dot.</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>Тег \c DOT_NODE_ATTR можно использовать для указания атрибутов узлов для диаграмм dot.</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>Тег \c DOT_FONTPATH можно использовать для указания пути к шрифтам для dot.</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>Если тег \c CLASS_GRAPH установлен в \c YES, будет сгенерирована диаграмма классов.</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>Если тег \c COLLABORATION_GRAPH установлен в \c YES, будет сгенерирована диаграмма сотрудничества.</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>Если тег \c GROUP_GRAPHS установлен в \c YES, Doxygen сгенерирует диаграмму для групп, показывающую прямые зависимости групп.</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>Если тег \c UML_LOOK установлен в \c YES, диаграммы наследования и сотрудничества будут сгенерированы в стиле, похожем на OMG Unified Modeling Language.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>Тег \c UML_LIMIT_NUM_FIELDS ограничивает количество полей, отображаемых в диаграммах UML.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>Тег \c UML_MAX_EDGE_LABELS ограничивает количество меток ребер, отображаемых в диаграммах UML.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>Тег \c DOT_UML_DETAILS управляет тем, какие детали отображаются в диаграммах UML.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>Тег \c DOT_WRAP_THRESHOLD определяет порог для переноса строк в диаграммах dot.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>Если тег \c TEMPLATE_RELATIONS установлен в \c YES, отношения шаблонов будут отображаться в диаграммах.</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>Если тег \c INCLUDE_GRAPH установлен в \c YES, будет сгенерирована диаграмма включений.</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>Если тег \c INCLUDED_BY_GRAPH установлен в \c YES, будет сгенерирована диаграмма &quot;включен в&quot;.</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>Если тег \c CALL_GRAPH установлен в \c YES, диаграмма вызовов будет сгенерирована для каждой глобальной функции или метода класса.</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>Если тег \c CALLER_GRAPH установлен в \c YES, диаграмма вызывающих будет сгенерирована для каждой глобальной функции или метода класса.</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>Если тег \c GRAPHICAL_HIERARCHY установлен в \c YES, иерархия классов будет отображаться графически.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>Если тег \c DIRECTORY_GRAPH установлен в \c YES, будет сгенерирована диаграмма каталогов.</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>Тег \c DIR_GRAPH_MAX_DEPTH можно использовать для ограничения максимального количества уровней подкаталогов, генерируемых в диаграмме зависимостей каталогов.</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>Тег \c DOT_IMAGE_FORMAT можно использовать для установки формата изображения для изображений, сгенерированных \c dot.</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>Если \ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot; установлен в `svg` или `svg:svg` или `svg:svg:core`, эту опцию можно установить в \c YES, чтобы включить интерактивные SVG изображения, которые позволяют масштабирование и панорамирование.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>Тег \c DOT_PATH можно использовать для указания пути к инструменту \c dot.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>Тег \c DOTFILE_DIRS можно использовать для указания каталогов, содержащих файлы dot.</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>Вы можете включить диаграммы, созданные с помощью dia, в документацию Doxygen. Doxygen запустит dia для генерации диаграммы и вставит ее в документацию. Тег DIA_PATH позволяет указать каталог, где находится бинарный файл dia.</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>Тег \c DIAFILE_DIRS можно использовать для указания каталогов, содержащих файлы dia.</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>Тег \c PLANTUML_JAR_PATH указывает путь к файлу JAR PlantUML.</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>Тег \c PLANTUML_CFG_FILE можно использовать для указания файла конфигурации для PlantUML.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>Тег \c PLANTUML_INCLUDE_PATH можно использовать для указания путей включения для PlantUML.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>Тег \c PLANTUMLFILE_DIRS можно использовать для указания каталогов, содержащих файлы PlantUML.</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>При использовании диаграмм Mermaid с CLI рендерингом тег \c MERMAID_PATH должен использоваться для указания каталога, где может быть найден исполняемый файл \c mmdc (Mermaid CLI).</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>При использовании диаграмм Mermaid тег \c MERMAID_CONFIG_FILE можно использовать для указания файла конфигурации JSON для инструмента CLI Mermaid (\c mmdc).</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>Тег \c MERMAID_RENDER_MODE выбирает, как рендерить диаграммы Mermaid.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>Тег \c MERMAID_JS_URL указывает URL, с которого загружается \c mermaid.js при использовании клиентского рендеринга.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>Тег \c MERMAIDFILE_DIRS можно использовать для указания каталогов, содержащих файлы Mermaid.</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>Тег \c DOT_GRAPH_MAX_NODES можно использовать для установки максимального количества узлов, отображаемых в графе.</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>Тег \c MAX_DOT_GRAPH_DEPTH можно использовать для установки максимальной глубины графов, сгенерированных \c dot.</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>Установите тег \c DOT_MULTI_TARGETS в \c YES, чтобы позволить dot генерировать несколько выходных файлов за один запуск.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>Если тег \c GENERATE_LEGEND установлен в \c YES, Doxygen сгенерирует страницу легенды, объясняющую значение различных блоков и стрелок в диаграммах, сгенерированных dot.</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>Если тег \c DOT_CLEANUP установлен в \c YES, Doxygen удалит промежуточные файлы, использованные для генерации различных графов. &amp;lt;br&amp;gt;Примечание: Эта настройка используется не только для файлов dot, но и для временных файлов msc.</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Вы можете определить диаграммы последовательности сообщений в комментариях Doxygen с помощью команды \ref cmdmsc &quot;\msc&quot;. Если тег \c MSCGEN_TOOL оставлен пустым (по умолчанию), Doxygen будет использовать встроенную версию инструмента mscgen для генерации диаграмм. Альтернативно, тег \c MSCGEN_TOOL также может указывать имя внешнего инструмента.</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>Тег \c MSCFILE_DIRS можно использовать для указания каталогов, содержащих файлы msc.</translation>
    </message>
</context>
</TS>
