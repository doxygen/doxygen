<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de">
<context>
    <name>OptionDocs</name>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>Dieses Tag implementiert einen quasi-intelligenten Kurzbeschreibungs-Abkurzer, der verwendet wird, um den Text in verschiedenen Auflistungen zu bilden. Jede Zeichenfolge in dieser Liste, wenn sie als fuhrender Text der Kurzbeschreibung gefunden wird, wird vom Text entfernt, und das Ergebnis nach der Verarbeitung der gesamten Liste wird als kommentierter Text verwendet. Andernfalls wird die Kurzbeschreibung wie angegeben verwendet. Wenn leer gelassen, werden die folgenden Werte verwendet (`$name` wird automatisch durch den Namen der Entitat ersetzt):</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>Dieses Tag kann verwendet werden, um eine Reihe von Aliasnamen anzugeben, die als Befehle in der Dokumentation fungieren. Ein Alias hat die Form: \verbatim name=wert\endverbatim Zum Beispiel wurde das Hinzufugen von \verbatim &quot;sideeffect=@par Nebeneffekte:^^&quot;\endverbatim es Ihnen ermoglichen, den Befehl \c \sideeffect (oder \c \@sideeffect) in der Dokumentation zu platzieren, was einen benutzerdefinierten Absatz mit der Uberschrift &quot;Nebeneffekte:&quot; erzeugt. Beachten Sie, dass Sie im Werteteil eines Alias kein \ref cmdn &quot;\n&quot; einfugen konnen, um einen Zeilenumbruch einzufugen (in der resultierenden Ausgabe). Sie konnen `^^` im Werteteil eines Alias einfugen, um einen Zeilenumbruch einzufugen, als ob ein physikalischer Zeilenumbruch in der Originaldatei vorhanden ware.</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>Wenn das \c ALLEXTERNALS-Tag auf \c YES gesetzt ist, werden alle externen Klassen und Namespaces im Index aufgefuhrt.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>Wenn das \c ALLOW_UNICODE_NAMES-Tag auf \c YES gesetzt ist, erlaubt Doxygen Nicht-ASCII-Zeichen in den Namen der generierten Dateien. Wenn auf \c NO gesetzt, werden Nicht-ASCII-Zeichen maskiert, zum Beispiel wird _xE3_x81_x84 fur Unicode U+3044 verwendet.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>Wenn das \c ALPHABETICAL_INDEX-Tag auf \c YES gesetzt ist, wird ein alphabetischer Index aller Verbindungen generiert.</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>Wenn sowohl die Tags \c ALWAYS_DETAILED_SEC als auch \ref cfg_repeat_brief &quot;REPEAT_BRIEF&quot; auf \c YES gesetzt sind, generiert Doxygen einen detaillierten Abschnitt, auch wenn es nur eine Kurzbeschreibung gibt.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>Dieses Tag gibt eine Liste von Wörtern an, die, wenn sie mit dem Anfang eines Wortes in der Dokumentation ubereinstimmen, die automatische Verknupfungsgenerierung unterdrucken, wenn diese durch AUTOLINK_SUPPORT aktiviert ist.</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>Wenn aktiviert, versucht Doxygen, Wörter, die einer dokumentierten Klasse oder einem Namespace entsprechen, mit der entsprechenden Dokumentation zu verknupfen. Dies kann in Einzelfallen verhindert werden, indem ein \c %-Symbol vor das Wort gesetzt wird, oder global, indem \c AUTOLINK_SUPPORT auf \c NO gesetzt wird.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>Das \c BINARY_TOC-Flag steuert, ob ein binaires Inhaltsverzeichnis generiert wird.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>Wenn das \c BRIEF_MEMBER_DESC-Tag auf \c YES gesetzt ist, wird Doxygen kurze Mitgliederbeschreibungen nach den Mitgliedern einfugen, die in der Datei- und Klassendokumentation aufgefuhrt sind (ahnlich wie \c Javadoc). Auf \c NO setzen, um dies zu deaktivieren.</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>Wenn Sie STL-Klassen verwenden (d.h. `std::string`, `std::vector` usw.), aber nicht den (Tag-Datei-)Quellcode der STL als Eingabe einbinden mochten, sollten Sie dieses Tag auf \c YES setzen, damit Doxygen Funktionsdeklarationen und -definitionen mit Argumenten, die STL-Klassen enthalten, abgleichen kann.</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>Wenn das \c CALLER_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Aufruferabhangigkeitsgraphen fur jede globale Funktion oder Klassenmethode.</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>Wenn das \c CALL_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Aufrufabhangigkeitsgraphen fur jede globale Funktion oder Klassenmethode.</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>Wenn das \c CASE_SENSE_NAMES-Tag auf \c YES gesetzt ist, wird bei Namen zwischen Grob- und Kleinschreibung unterschieden.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>Das \c CHM_FILE-Tag kann verwendet werden, um den Dateinamen der resultierenden `.chm`-Datei anzugeben.</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>Das \c CHM_INDEX_ENCODING wird verwendet, um HtmlHelp-Index, Inhalt und Projektdatei-Inhalt zu kodieren.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>Das \c CITE_BIB_FILES-Tag kann verwendet werden, um eine oder mehrere \c bib-Dateien anzugeben, die die Referenzdefinitionen enthalten.</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>Wenn das \c CLANG_ASSISTED_PARSING-Tag auf \c YES gesetzt ist und das \c CLANG_ADD_INC_PATHS-Tag auf \c YES gesetzt ist, fugt Doxygen das Verzeichnis jeder Eingabe zum Include-Pfad hinzu.</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>Wenn das \c CLANG_ASSISTED_PARSING-Tag auf \c YES gesetzt ist, verwendet Doxygen den clang-Parser fur genaueres Parsing auf Kosten reduzierter Leistung.</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>Wenn clang-gestutztes Parsing aktiviert ist, konnen Sie dem clang-Parser den Pfad zum Verzeichnis mit einer Datei namens `compile_commands.json` bereitstellen.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>Wenn clang-gestutztes Parsing aktiviert ist, konnen Sie dem Compiler Befehlszeilenoptionen bereitstellen.</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>Wenn das \c CLASS_GRAPH-Tag auf \c YES oder \c GRAPH oder \c BUILTIN gesetzt ist, generiert Doxygen einen Graphen fur jede dokumentierte Klasse.</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>Wenn das \c COLLABORATION_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Graphen fur jede dokumentierte Klasse.</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>Wenn das \c COMPACT_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen kompaktere \f$\mbox{\LaTeX}\f$-Dokumente.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>Wenn das \c COMPACT_RTF-Tag auf \c YES gesetzt ist, generiert Doxygen kompaktere RTF-Dokumente.</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>Wenn Sie die C++/CLI-Sprache von Microsoft verwenden, sollten Sie diese Option auf \c YES setzen, um Parsing-Unterstützung zu aktivieren.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>Wenn das \c CREATE_SUBDIRS-Tag auf \c YES gesetzt ist, erstellt Doxygen bis zu 4096 Unterverzeichnisse (in 2 Ebenen) unter dem Ausgabeverzeichnis jedes Ausgabeformats und verteilt die generierten Dateien uber diese Verzeichnisse. Das Aktivieren dieser Option kann nutzlich sein, wenn Doxygen eine grose Menge an Quelldateien zugefuhrt wird, wobei das Ablegen aller generierten Dateien im selben Verzeichnis sonst Leistungsprobleme fur das Dateisystem verursachen wurde. Passen Sie \c CREATE_SUBDIRS_LEVEL an, um die Anzahl der Unterverzeichnisse zu steuern.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>Steuert die Anzahl der Unterverzeichnisse, die erstellt werden, wenn das \c CREATE_SUBDIRS-Tag auf \c YES gesetzt ist. Ebene 0 reprasentiert 16 Verzeichnisse, und jede Ebenenerhohung verdoppelt die Anzahl der Verzeichnisse, was zu 4096 Verzeichnissen auf Ebene 8 fuhrt, was der Standardwert und auch der Maximalwert ist. Die Unterverzeichnisse sind in 2 Ebenen organisiert, wobei die erste Ebene immer eine feste Anzahl von 16 Verzeichnissen hat.</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>Das \c DIAFILE_DIRS-Tag kann verwendet werden, um Verzeichnisse anzugeben, die dia-Dateien enthalten.</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>Das DIA_PATH-Tag ermoglicht es Ihnen, das Verzeichnis anzugeben, in dem sich die dia-Binardatei befindet.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>Wenn das \c DIRECTORY_GRAPH-Tag auf \c YES gesetzt ist, zeigt Doxygen die Abhangigkeiten eines Verzeichnisses grafisch an.</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>Das \c DIR_GRAPH_MAX_DEPTH-Tag kann verwendet werden, um die maximale Anzahl von Ebenen zu begrenzen.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>Das \c DISABLE_INDEX-Tag kann verwendet werden, um den kondensierten Index (Tabs) oben auf jeder HTML-Seite ein- oder auszuschalten.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>Wenn in der Dokumentation eine Mitgliedergruppierung verwendet wird und das \c DISTRIBUTE_GROUP_DOC-Tag auf \c YES gesetzt ist, wird Doxygen die Dokumentation des ersten Mitglieds der Gruppe fur die anderen Mitglieder der Gruppe wiederverwenden (falls vorhanden).</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>Das \c DOCBOOK_OUTPUT-Tag wird verwendet, um anzugeben, wo die Docbook-Seiten platziert werden.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>Dieses Tag gibt eine Zeichenfolge an, die das Dokumentensatz-Bundle eindeutig identifizieren sollte.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>Dieses Tag bestimmt den Namen des Docset-Feeds.</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>Dieses Tag bestimmt die URL des Docset-Feeds.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>Das \c DOCSET_PUBLISHER_ID-Tag gibt eine Zeichenfolge an, die den Dokumentationsherausgeber eindeutig identifizieren sollte.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>Das \c DOCSET_PUBLISHER_NAME-Tag identifiziert den Dokumentationsherausgeber.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>Das \c DOTFILE_DIRS-Tag kann verwendet werden, um Verzeichnisse anzugeben, die dot-Dateien enthalten.</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>Wenn das \c DOT_CLEANUP-Tag auf \c YES gesetzt ist, loscht Doxygen die Zwischendateien, die zum Generieren der verschiedenen Diagramme verwendet wurden.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR sind gemeinsame Attribute fur Knoten, Kanten und Labels von Untergraphen.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR wird mit \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; verkettet.</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>Sie konnen den Pfad festlegen, in dem \c dot die Schrift finden kann.</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>Das \c DOT_GRAPH_MAX_NODES-Tag kann verwendet werden, um die maximale Anzahl von Knoten festzulegen, die im Graphen angezeigt werden.</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>Das \c DOT_IMAGE_FORMAT-Tag kann verwendet werden, um das Bildformat der von \c dot generierten Bilder festzulegen.</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>Setzen Sie das \c DOT_MULTI_TARGETS-Tag auf \c YES, um \c dot zu erlauben, mehrere Ausgabedateien in einem Lauf zu generieren.</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR wird mit \ref cfg_dot_common_attr &quot;DOT_COMMON_ATTR&quot; verkettet.</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>Das \c DOT_NUM_THREADS gibt die Anzahl der \c dot-Aufrufe an, die Doxygen parallel ausfuhren darf.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>Das \c DOT_PATH-Tag kann verwendet werden, um den Pfad anzugeben, in dem das \c dot-Tool gefunden werden kann.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>Wenn das \c DOT_UML_DETAILS-Tag auf \c NO gesetzt ist, zeigt Doxygen Attribute und Methoden ohne Typen und Argumente in den UML-Graphen.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>Das \c DOT_WRAP_THRESHOLD-Tag kann verwendet werden, um die maximale Anzahl von Zeichen festzulegen, die in einer einzelnen Zeile angezeigt werden.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>Eine eindeutige Kennung fur das Eclipse-Hilfe-Plugin.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>Das \c ENABLED_SECTIONS-Tag kann verwendet werden, um bedingte Dokumentationsabschnitte zu aktivieren.</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>Wenn das \c ENABLE_PREPROCESSING-Tag auf \c YES gesetzt ist, wertet Doxygen alle C-Praprozessor-Direktiven aus.</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>Das \c ENUM_VALUES_PER_LINE-Tag kann verwendet werden, um die Anzahl von Enum-Werten festzulegen, die Doxygen in einer Zeile gruppiert.</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>Das \c EXAMPLE_PATH-Tag kann verwendet werden, um eine oder mehrere Dateien oder Verzeichnisse anzugeben, die Beispielcode-Fragmente enthalten.</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>Wenn der Wert des \ref cfg_example_path &quot;EXAMPLE_PATH&quot;-Tags Verzeichnisse enthalt, konnen Sie das \c EXAMPLE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster anzugeben, um die Quelldateien in den Verzeichnissen zu filtern.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>Wenn das \c EXAMPLE_RECURSIVE-Tag auf \c YES gesetzt ist, werden Unterverzeichnisse nach Eingabedateien durchsucht.</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>Das \c EXCLUDE-Tag kann verwendet werden, um Dateien und/oder Verzeichnisse anzugeben, die von den \ref cfg_input &quot;INPUT&quot;-Quelldateien ausgeschlossen werden sollen.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>Wenn der Wert des \ref cfg_input &quot;INPUT&quot;-Tags Verzeichnisse enthalt, konnen Sie das \c EXCLUDE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster anzugeben, um bestimmte Dateien aus diesen Verzeichnissen auszuschlieben.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>Das \c EXCLUDE_SYMBOLS-Tag kann verwendet werden, um einen oder mehrere Symbolnamen anzugeben, die von der Ausgabe ausgeschlossen werden sollen.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>Das \c EXCLUDE_SYMLINKS-Tag kann verwendet werden, um auszuwahlen, ob Dateien oder Verzeichnisse, die symbolische Links sind, von der Eingabe ausgeschlossen werden.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>Wenn die Tags \ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; und \ref cfg_expand_only_predef &quot;EXPAND_ONLY_PREDEF&quot; auf \c YES gesetzt sind, kann dieses Tag verwendet werden, um eine Liste von Makronamen anzugeben, die expandiert werden sollen.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>Wenn die Tags \c EXPAND_ONLY_PREDEF und \ref cfg_macro_expansion &quot;MACRO_EXPANSION&quot; beide auf \c YES gesetzt sind, ist die Makroexpansion beschrankt.</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen wahlt den zu verwendenden Parser basierend auf der Erweiterung der zu parsenden Datei aus. Mit diesem Tag konnen Sie einen Parser zuweisen, der fur eine bestimmte Erweiterung verwendet werden soll. Doxygen hat eine eingebaute Zuordnung, aber Sie konnen diese mit diesem Tag uberschreiben oder erweitern. Das Format ist &amp;lt;code&amp;gt;ext=sprache&amp;lt;/code&amp;gt;, wobei \c ext eine Dateierweiterung ist und sprache einer der von Doxygen unterstutzten Parser: IDL, Java, JavaScript, Csharp (C#), C, C++, Lex, D, PHP, md (Markdown), Objective-C, Python, Slice, VHDL, Fortran (festes Format Fortran: FortranFixed, freies Format Fortran: FortranFree, unbekanntes Format Fortran: Fortran. Im letzteren Fall versucht der Parser zu erraten, ob der Code festes oder freies Format ist, was die Standardeinstellung fur Fortran-Typ-Dateien ist).</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>Wenn das \c EXTERNAL_GROUPS-Tag auf \c YES gesetzt ist, werden alle externen Gruppen im Themenindex aufgefuhrt.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>Wenn das \c EXTERNAL_PAGES-Tag auf \c YES gesetzt ist, werden alle externen Seiten im Related-Pages-Index aufgefuhrt.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>Wenn das \c EXTERNAL_SEARCH-Tag aktiviert ist, generiert Doxygen das PHP-Skript zum Suchen nicht mehr.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>Wenn \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot; und \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; beide aktiviert sind, kann das \c EXTERNAL_SEARCH_ID-Tag als Kennung fur das Projekt verwendet werden.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>Das \c EXTERNAL_TOOL_PATH-Tag kann verwendet werden, um den Suchpfad (PATH-Umgebungsvariable) zu erweitern, damit externe Tools wie \c latex und \c gs gefunden werden konnen.</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>Wenn das \c EXTRACT_ALL-Tag auf \c YES gesetzt ist, geht Doxygen davon aus, dass alle Entitaten dokumentiert sind, auch wenn keine Dokumentation verfugbar ist.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>Wenn das \c EXTRACT_ANON_NSPACES-Tag auf \c YES gesetzt ist, werden anonyme Namensraume in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>Wenn das \c EXTRACT_LOCAL_CLASSES-Tag auf \c YES gesetzt ist, werden lokal definierte Klassen in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>Wenn das \c EXTRACT_LOCAL_METHODS-Tag auf \c YES gesetzt ist, werden lokal definierte Methoden in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>Wenn das \c EXTRACT_PACKAGE-Tag auf \c YES gesetzt ist, werden alle Mitglieder mit package-Sichtbarkeit in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>Wenn das \c EXTRACT_PRIVATE-Tag auf \c YES gesetzt ist, werden alle privaten Mitglieder in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>Wenn das \c EXTRACT_PRIV_VIRTUAL-Tag auf \c YES gesetzt ist, werden private virtuelle Mitglieder in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>Wenn das \c EXTRACT_STATIC-Tag auf \c YES gesetzt ist, werden alle statischen Mitglieder in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>Das \c EXTRA_PACKAGES-Tag kann verwendet werden, um einen oder mehrere \f$\mbox{\LaTeX}\f$-Paketnamen anzugeben.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>Das \c EXTRA_SEARCH_MAPPINGS-Tag kann verwendet werden, um das Durchsuchen von Doxygen-Projekten zu ermoglichen.</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>Wenn die \c EXT_LINKS_IN_WINDOW-Option auf \c YES gesetzt ist, offnet Doxygen Links zu externen Symbolen in einem separaten Fenster.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>Wenn der Wert des \ref cfg_input &quot;INPUT&quot;-Tags Verzeichnisse enthalt, konnen Sie das \c FILE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster anzugeben, um die Quelldateien in den Verzeichnissen zu filtern.</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>Das \c FILE_VERSION_FILTER-Tag kann verwendet werden, um ein Programm oder Skript anzugeben, das Doxygen aufrufen soll, um die aktuelle Version fur jede Datei zu erhalten.</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>Das \c FILTER_PATTERNS-Tag kann verwendet werden, um Filter auf Dateimusterbasis anzugeben.</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>Wenn das \c FILTER_SOURCE_FILES-Tag auf \c YES gesetzt ist, wird der Eingabefilter auch verwendet, um die Eingabedateien zu filtern, die zum Erstellen der Quelldateien zum Durchsuchen verwendet werden.</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>Das \c FILTER_SOURCE_PATTERNS-Tag kann verwendet werden, um Quellfilter pro Dateimuster anzugeben.</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>Wenn das \c FORCE_LOCAL_INCLUDES-Tag auf \c YES gesetzt ist, wird Doxygen Include-Dateien mit doppelten Anfuhrungszeichen in der Dokumentation auflisten, anstatt mit spitzen Klammern.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>Verwenden Sie dieses Tag, um die Schriftgrobe von \f$\mbox{\LaTeX}\f$-Formeln zu andern.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>Die \c FORMULA_MACROFILE kann \f$\mbox{\LaTeX}\f$-Befehle `\newcommand` und `\renewcommand` enthalten.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Der Fortran-Standard gibt an, dass fur fest formatierten Fortran-Code alle Zeichen ab Position 72 als Kommentar betrachtet werden.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>Wenn das \c FULL_PATH_NAMES-Tag auf \c YES gesetzt ist, fugt Doxygen den vollstandigen Pfad vor Dateinamen in der Dateiliste und in den Header-Dateien hinzu. Wenn auf \c NO gesetzt, wird der kurzeste Pfad verwendet, der die Dateinamen eindeutig macht.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>Wenn \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; auf \c YES gesetzt ist, bestimmt die \c FULL_SIDEBAR-Option, ob die Seitenleiste auf den Treeview-Bereich beschrankt ist.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>Wenn das \c GENERATE_AUTOGEN_DEF-Tag auf \c YES gesetzt ist, generiert Doxygen eine AutoGen Definitions-Datei.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>Das \c GENERATE_BUGLIST-Tag kann verwendet werden, um die Bug-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>Das \c GENERATE_CHI-Flag steuert, ob eine separate `.chi`-Indexdatei generiert wird.</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>Das \c GENERATE_DEPRECATEDLIST-Tag kann verwendet werden, um die Veraltet-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>Wenn das \c GENERATE_DOCBOOK-Tag auf \c YES gesetzt ist, generiert Doxygen Docbook-Dateien.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>Wenn das \c GENERATE_DOCSET-Tag auf \c YES gesetzt ist, werden zusatzliche Indexdateien generiert, die als Eingabe fur Apples Xcode 3 verwendet werden konnen.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>Wenn das \c GENERATE_ECLIPSEHELP-Tag auf \c YES gesetzt ist, werden zusatzliche Indexdateien generiert, die ein Eclipse-Hilfe-Plugin bilden.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>Wenn das \c GENERATE_HTML-Tag auf \c YES gesetzt ist, generiert Doxygen HTML-Ausgabe.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>Wenn das \c GENERATE_HTMLHELP-Tag auf \c YES gesetzt ist, generiert Doxygen drei zusatzliche HTML-Indexdateien.</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>Wenn das \c GENERATE_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen \f$\mbox{\LaTeX}\f$-Ausgabe.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>Wenn das \c GENERATE_LEGEND-Tag auf \c YES gesetzt ist, generiert Doxygen eine Legenden-Seite.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>Wenn das \c GENERATE_MAN-Tag auf \c YES gesetzt ist, generiert Doxygen Man-Pages fur Klassen und Dateien.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>Wenn das \c GENERATE_PERLMOD-Tag auf \c YES gesetzt ist, generiert Doxygen eine Perl-Modul-Datei.</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>Wenn das \c GENERATE_QHP-Tag auf \c YES gesetzt ist, wird eine zusatzliche Indexdatei generiert, die als Eingabe fur Qt&#x27;s qhelpgenerator verwendet werden kann.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>Das \c GENERATE_REQUIREMENTS-Tag kann verwendet werden, um die Anforderungen-Seite zu aktivieren (\c YES) oder zu deaktivieren (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>Wenn das \c GENERATE_RTF-Tag auf \c YES gesetzt ist, generiert Doxygen RTF-Ausgabe.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>Wenn das \c GENERATE_SQLITE3-Tag auf \c YES gesetzt ist, generiert Doxygen eine \c Sqlite3-Datenbank.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>Wenn ein Dateiname nach \c GENERATE_TAGFILE angegeben wird, erstellt Doxygen eine Tag-Datei.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>Das \c GENERATE_TESTLIST-Tag kann verwendet werden, um die Testliste zu aktivieren (\c YES) oder zu deaktivieren (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>Das \c GENERATE_TODOLIST-Tag kann verwendet werden, um die Todo-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>Das \c GENERATE_TREEVIEW-Tag wird verwendet, um anzugeben, ob eine baumartige Indexstruktur generiert werden soll.</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>Wenn das \c GENERATE_XML-Tag auf \c YES gesetzt ist, generiert Doxygen eine XML-Datei.</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>Wenn das \c GRAPHICAL_HIERARCHY-Tag auf \c YES gesetzt ist, zeigt Doxygen eine grafische Hierarchie aller Klassen.</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>Wenn das \c GROUP_GRAPHS-Tag auf \c YES gesetzt ist, generiert Doxygen einen Graphen fur Gruppen.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>Wenn eine Struktur oder Klasse zu einer Gruppe hinzugefugt wird und diese Option aktiviert ist, werden alle verschachtelten Klassen oder Strukturen ebenfalls derselben Gruppe hinzugefugt.</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>Wenn Sie das \c HAVE_DOT-Tag auf \c YES setzen, geht Doxygen davon aus, dass das \c dot-Tool verfugbar ist.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>Das \c HHC_LOCATION-Tag kann verwendet werden, um den Speicherort des HTML-Help-Compilers (\c hhc.exe) anzugeben.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>Wenn das \c HIDE_COMPOUND_REFERENCE-Tag auf \c NO (Standard) gesetzt ist, wird Doxygen zusatzlichen Text an den Titel einer Seite anhangen, wie z.B. Klassenreferenz. Wenn auf \c YES gesetzt, wird die Verbundreferenz ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>Wenn das \c HIDE_FRIEND_COMPOUNDS-Tag auf \c YES gesetzt ist, werden Friend-Deklarationen in der Dokumentation ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>Wenn das \c HIDE_IN_BODY_DOCS-Tag auf \c YES gesetzt ist, wird die Dokumentation, die im Korper einer Funktion gefunden wird, ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>Wenn das \c HIDE_SCOPE_NAMES-Tag auf \c YES gesetzt ist, werden Bereichsnamen in der Dokumentation ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>Wenn das \c HIDE_UNDOC_CLASSES-Tag auf \c YES gesetzt ist, werden undokumentierte Klassen in der Dokumentation ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>Wenn das \c HIDE_UNDOC_MEMBERS-Tag auf \c YES gesetzt ist, werden undokumentierte Mitglieder in der Dokumentation ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>Wenn das \c HIDE_UNDOC_NAMESPACES-Tag auf \c YES gesetzt ist, werden undokumentierte Namensraume in der Dokumentation ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>Wenn auf \c YES gesetzt, blenden die Vererbungs- und Kollaborationsdiagramme Vererbungs- und Verwendungsbeziehungen aus.</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>Wenn das \c HTML_CODE_FOLDING-Tag auf \c YES gesetzt ist, konnen Klassen und Funktionen im generierten HTML-Quellcode dynamisch gefaltet und erweitert werden.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>Das \c HTML_COLORSTYLE-Tag kann verwendet werden, um anzugeben, ob die generierte HTML-Ausgabe mit einem dunklen oder hellen Thema gerendert werden soll.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>Das \c HTML_COLORSTYLE_GAMMA-Tag steuert die Gammakorrektur, die auf die Luminanzkomponente der Farben in der HTML-Ausgabe angewendet wird.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>Das \c HTML_COLORSTYLE_HUE-Tag steuert die Farbe der HTML-Ausgabe.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>Das \c HTML_COLORSTYLE_SAT-Tag steuert die Reinheit (oder Sattigung) der Farben in der HTML-Ausgabe.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>Wenn das \c HTML_COPY_CLIPBOARD-Tag auf \c YES gesetzt ist, zeigt Doxygen ein Symbol in der oberen rechten Ecke von Code- und Textfragmenten an.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>Wenn das \c HTML_DYNAMIC_MENUS-Tag auf \c YES gesetzt ist, enthalt die generierte HTML-Dokumentation einen Hauptindex mit vertikalen Navigationsmenus, die dynamisch uber JavaScript erstellt werden.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>Wenn das \c HTML_DYNAMIC_SECTIONS-Tag auf \c YES gesetzt ist, enthalt die generierte HTML-Dokumentation Abschnitte, die nach dem Laden der Seite ein- und ausgeblendet werden konnen.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>Das \c HTML_EXTRA_FILES-Tag kann verwendet werden, um eine oder mehrere zusatzliche Bilder oder andere Quelldateien anzugeben, die in das HTML-Ausgabeverzeichnis kopiert werden sollen.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>Das \c HTML_EXTRA_STYLESHEET-Tag kann verwendet werden, um zusatzliche benutzerdefinierte Cascading Stylesheets anzugeben.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>Das \c HTML_FILE_EXTENSION-Tag kann verwendet werden, um die Dateierweiterung fur jede generierte HTML-Seite anzugeben.</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>Das \c HTML_FOOTER-Tag kann verwendet werden, um eine benutzerdefinierte HTML-Fuszzeile fur jede generierte HTML-Seite anzugeben.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>Wenn die \c HTML_FORMULA_FORMAT-Option auf `svg` gesetzt ist, verwendet Doxygen das Tool pdf2svg oder Inkscape, um Formeln als SVG-Bilder zu generieren.</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>Das \c HTML_HEADER-Tag kann verwendet werden, um eine benutzerdefinierte HTML-Header-Datei fur jede generierte HTML-Seite anzugeben.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>Mit \c HTML_INDEX_NUM_ENTRIES kann die bevorzugte Anzahl von Eintragen gesteuert werden, die anfanglich in den verschiedenen baumstrukturierten Indizes angezeigt werden.</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>Das \c HTML_OUTPUT-Tag wird verwendet, um anzugeben, wo die HTML-Dokumente platziert werden.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen speichert einige Einstellungen dauerhaft im Browser (z.B. uber Cookies). Das \c HTML_PROJECT_COOKIE-Tag kann verwendet werden, um die Einstellungen unter einem projektspezifischen Schlussel zu speichern.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>Das \c HTML_STYLESHEET-Tag kann verwendet werden, um ein benutzerdefiniertes Cascading Stylesheet anzugeben, das von jeder HTML-Seite verwendet wird.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Fur Microsofts IDL gibt es die Attribute \c propget und \c propput, um Getter- und Setter-Methoden fur Eigenschaften anzuzeigen. Das Setzen dieser Option auf \c YES fuhrt dazu, dass Doxygen die get- und set-Methoden in der Dokumentation durch die Eigenschaft ersetzt.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>Das \c IGNORE_PREFIX-Tag kann verwendet werden, um ein Prax (oder eine Liste von Praxen) anzugeben, das beim Generieren der Index-Header ignoriert werden soll.</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>Das \c IMAGE_PATH-Tag kann verwendet werden, um eine oder mehrere Dateien oder Verzeichnisse anzugeben, die Bilder enthalten, die in die Dokumentation aufgenommen werden sollen.</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>Wenn das \c IMPLICIT_DIR_DOCS-Tag auf \c YES gesetzt ist, wird jede `README.md`-Datei, die in Unterverzeichnissen des Projektstamms gefunden wird, als Dokumentation fur dieses Unterverzeichnis verwendet.</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>Wenn die Tags \c INCLUDED_BY_GRAPH und andere auf \c YES gesetzt sind, generiert Doxygen einen Graphen fur jede dokumentierte Datei.</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>Sie konnen das \c INCLUDE_FILE_PATTERNS-Tag verwenden, um Wildcard-Muster anzugeben.</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>Wenn die Tags \c INCLUDE_GRAPH und andere auf \c YES gesetzt sind, generiert Doxygen einen Graphen fur jede dokumentierte Datei.</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>Das \c INCLUDE_PATH-Tag kann verwendet werden, um Verzeichnisse anzugeben, die Include-Dateien enthalten.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>Wenn das \c INHERIT_DOCS-Tag auf \c YES gesetzt ist, erbt ein undokumentiertes Mitglied die Dokumentation von jedem dokumentierten Mitglied, das es neu implementiert.</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>Wenn das \c INLINE_GROUPED_CLASSES-Tag auf \c YES gesetzt ist, werden Klassen, Strukturen und Unions in der Gruppe angezeigt, die sie enthalt (z.B. mit \ref cmdingroup &quot;\ingroup&quot;), anstatt auf separaten Seiten.</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>Wenn das \c INLINE_INFO-Tag auf \c YES gesetzt ist, wird ein Tag [inline] in die Dokumentation fur Inline-Mitglieder eingefugt.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>Wenn das \c INLINE_INHERITED_MEMB-Tag auf \c YES gesetzt ist, zeigt Doxygen alle geerbten Mitglieder in der Dokumentation der Klasse an, als waren diese Mitglieder normale Klassenmitglieder. Konstruktoren, Destruktoren und Zuweisungsoperatoren der Basisklasse werden nicht angezeigt.</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>Wenn das \c INLINE_SIMPLE_STRUCTS-Tag auf \c YES gesetzt ist, werden Strukturen, Klassen und Unions, die nur Datenfelder enthalten, inline in der Dokumentation der Datei angezeigt, in der sie definiert sind, anstatt auf separaten Seiten.</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>Das Setzen des \c INLINE_SOURCES-Tags auf \c YES fugt den Korper von Funktionen, mehrzeiligen Makros, Enums oder Listen-initialisierten Variablen direkt in die Dokumentation ein.</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>Das \c INPUT-Tag wird verwendet, um die Dateien und/oder Verzeichnisse anzugeben, die dokumentierte Quelldateien enthalten.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>Dieses Tag kann verwendet werden, um die Zeichenkodierung der Quelldateien anzugeben, die Doxygen parst.</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>Dieses Tag kann verwendet werden, um die Zeichenkodierung der Quelldateien anzugeben, die Doxygen parst, auf Dateimusterbasis.</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>Das \c INPUT_FILTER-Tag kann verwendet werden, um ein Programm anzugeben, das Doxygen fur jede Eingabedatei aufrufen soll, um zu filtern.</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>Wenn \ref cfg_dot_image_format &quot;DOT_IMAGE_FORMAT&quot; auf `svg` gesetzt ist, kann diese Option auf \c YES gesetzt werden, um interaktive SVG-Bilder zu generieren.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>Wenn das \c INTERNAL_DOCS-Tag auf \c YES gesetzt ist, wird die Dokumentation fur interne Entitaten generiert.</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>Wenn das \c JAVADOC_AUTOBRIEF-Tag auf \c YES gesetzt ist, interpretiert Doxygen die erste Zeile (bis zum ersten Punkt, Fragezeichen oder Ausrufezeichen) von Javadoc-Stil-Kommentaren als Kurzbeschreibung. Wenn auf \c NO gesetzt, verhalt sich der Javadoc-Stil wie normale Qt-Stil-Kommentare (daher ist ein expliziter \ref cmdbrief &quot;\@brief&quot;-Befehl erforderlich, um eine Kurzbeschreibung zu erhalten).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>Wenn das \c JAVADOC_BANNER-Tag auf \c YES gesetzt ist, interpretiert Doxygen Zeilen wie \verbatim /***************\endverbatim als Beginn eines Javadoc-Stil-Kommentar-Banners. Wenn auf \c NO gesetzt, verhalt sich der Javadoc-Stil wie normale Kommentare, und Doxygen interpretiert ihn nicht.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>Das \c LATEX_BATCHMODE-Tag signalisiert das Verhalten von \f$\mbox{\LaTeX}\f$ im Fehlerfall.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>Das \c LATEX_BIB_STYLE-Tag kann verwendet werden, um den Stil fur die Bibliografie anzugeben.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>Das \c LATEX_CMD_NAME-Tag kann verwendet werden, um den aufzurufenden \f$\mbox{\LaTeX}\f$-Befehlsnamen anzugeben.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>Das \c LATEX_EMOJI_DIRECTORY-Tag wird verwendet, um den Pfad anzugeben, aus dem die Emoji-Bilder gelesen werden.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>Das \c LATEX_EXTRA_FILES-Tag kann verwendet werden, um zusatzliche Dateien anzugeben, die in das \ref cfg_latex_output &quot;LATEX_OUTPUT&quot;-Ausgabeverzeichnis kopiert werden sollen.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>Das \c LATEX_EXTRA_STYLESHEET-Tag kann verwendet werden, um zusatzliche benutzerdefinierte \f$\mbox{\LaTeX}\f$-Stylesheets anzugeben.</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>Das \c LATEX_FOOTER-Tag kann verwendet werden, um einen benutzerdefinierten \f$\mbox{\LaTeX}\f$-Footer anzugeben.</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>Das \c LATEX_HEADER-Tag kann verwendet werden, um einen benutzerdefinierten \f$\mbox{\LaTeX}\f$-Header anzugeben.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>Wenn das \c LATEX_HIDE_INDICES-Tag auf \c YES gesetzt ist, wird Doxygen die Indexkapitel nicht in die Ausgabe aufnehmen.</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>Das \c LATEX_MAKEINDEX_CMD-Tag kann verwendet werden, um den Befehlsnamen zum Generieren des Index fur \f$\mbox{\LaTeX}\f$ anzugeben.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>Das \c LATEX_OUTPUT-Tag wird verwendet, um anzugeben, wo die \f$\mbox{\LaTeX}\f$-Dokumente platziert werden.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>Das \c LAYOUT_FILE-Tag kann verwendet werden, um eine Layout-Datei anzugeben, die von Doxygen geparst wird.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>Das \c LOOKUP_CACHE_SIZE-Tag kann verwendet werden, um die Grose des internen Caches festzulegen, der zum Auflosen von Symbolen verwendet wird.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>Wenn das \c MACRO_EXPANSION-Tag auf \c YES gesetzt ist, expandiert Doxygen alle Makronamen im Quellcode.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>Das \c MAKEINDEX_CMD_NAME-Tag kann verwendet werden, um den Befehlsnamen zum Generieren des Index fur \f$\mbox{\LaTeX}\f$ anzugeben.</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>Das \c MAN_EXTENSION-Tag bestimmt die Erweiterung, die den generierten Man-Pages hinzugefugt wird.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>Wenn das \c MAN_LINKS-Tag auf \c YES gesetzt ist, wird Doxygen eine zusatzliche Man-Datei fur jede Entitat generieren.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>Das \c MAN_OUTPUT-Tag wird verwendet, um anzugeben, wo die Man-Pages platziert werden.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>Das \c MAN_SUBDIR-Tag bestimmt den Namen des Verzeichnisses, das innerhalb von \c MAN_OUTPUT erstellt wird.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>Das \c MARKDOWN_ID_STYLE-Tag kann verwendet werden, um den Algorithmus anzugeben, der zum Generieren von Bezeichnern fur Markdown-Uberschriften verwendet wird. Hinweis: Jeder Bezeichner ist eindeutig.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>Wenn das \c MARKDOWN_STRICT-Tag aktiviert ist, behandelt Doxygen den Text in Kommentaren auch dann als Markdown-Format, wenn das native Markup-Format von Doxygen mit Markdown in Konflikt steht. Dies ist nur relevant, wenn Backticks verwendet werden.</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>Wenn das \c MARKDOWN_SUPPORT-Tag aktiviert ist, wird Doxygen alle Kommentare gemab dem Markdown-Format vorverarbeiten, was eine lesbarere Dokumentation ermoglicht. Weitere Informationen finden Sie unter https://daringfireball.net/projects/markdown/. Die Ausgabe der Markdown-Verarbeitung wird von Doxygen weiterverarbeitet, sodass Sie Doxygen-, HTML- und XML-Befehle mit Markdown-Formatierung mischen konnen. Deaktivieren Sie dies nur bei Problemen mit der Abwartskompatibilitat.</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>Das \c MATHJAX_CODEFILE-Tag kann verwendet werden, um eine Datei mit JavaScript-Code-Stucken anzugeben.</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>Das \c MATHJAX_EXTENSIONS-Tag kann verwendet werden, um einen oder mehrere MathJax-Erweiterungsnamen anzugeben.</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>Wenn MathJax aktiviert ist, konnen Sie das Standardausgabeformat festlegen.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>Wenn MathJax aktiviert ist, mussen Sie den Speicherort relativ zum HTML-Ausgabeverzeichnis angeben.</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>Mit \c MATHJAX_VERSION ist es moglich, die zu verwendende MathJax-Version anzugeben.</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>Das \c MAX_DOT_GRAPH_DEPTH-Tag kann verwendet werden, um die maximale Tiefe der von \c dot generierten Graphen festzulegen.</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>Das \c MAX_INITIALIZER_LINES-Tag bestimmt die maximale Anzahl von Zeilen, die der Anfangswert einer Variable oder eines Makros / Defines haben darf, um in der Dokumentation zu erscheinen.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>Das \c MERMAIDFILE_DIRS-Tag kann verwendet werden, um Verzeichnisse anzugeben, die Mermaid-Dateien enthalten.</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Bei Verwendung von Mermaid-Diagrammen kann das \c MERMAID_CONFIG_FILE-Tag verwendet werden, um eine JSON-Konfigurationsdatei anzugeben.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>Das \c MERMAID_JS_URL-Tag gibt die URL an, von der \c mermaid.js geladen werden soll.</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>Bei Verwendung von Mermaid-Diagrammen sollte das \c MERMAID_PATH-Tag verwendet werden, um das Verzeichnis mit dem \c mmdc-Programm anzugeben.</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>Das \c MERMAID_RENDER_MODE-Tag wahlt aus, wie Mermaid-Diagramme gerendert werden.</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>Das \c MSCFILE_DIRS-Tag kann verwendet werden, um Verzeichnisse anzugeben, die msc-Dateien enthalten.</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Sie konnen Nachrichtensequenzdiagramme in Doxygen-Kommentaren mit dem \ref cmdmsc &quot;\msc&quot;-Befehl definieren.</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>Das \c MULTILINE_CPP_IS_BRIEF-Tag kann auf \c YES gesetzt werden, damit Doxygen mehrzeilige C++-spezielle Kommentarblocke (d.h. \c //! oder \c /// Kommentarblocke) als Kurzbeschreibung behandelt. Dies war fruher das Standardverhalten. Die neue Standardeinstellung besteht darin, mehrzeilige C++-Kommentarblocke als detaillierte Beschreibung zu behandeln. Wenn Sie das alte Verhalten bevorzugen, setzen Sie dieses Tag auf \c YES. &amp;lt;br&amp;gt;Beachten Sie, dass das Setzen dieses Tags auf \c YES auch bedeutet, dass Rational Rose-Kommentare nicht mehr erkannt werden.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>Das \c NUM_PROC_THREADS-Tag kann verwendet werden, um die Anzahl der Threads festzulegen, die Doxygen fur die parallele Verarbeitung verwendet.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>Wenn das \c OBFUSCATE_EMAILS-Tag auf \c YES gesetzt ist, verschleiert Doxygen E-Mail-Adressen.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>Setzen Sie das \c OPTIMIZE_FOR_FORTRAN-Tag auf \c YES, wenn Ihr Projekt Fortran-Quellcode enthalt. Doxygen generiert dann eine Ausgabe, die fur Fortran geeignet ist.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_FOR_C-Tag auf \c YES, wenn Ihr Projekt nur C-Quellcode enthalt. Doxygen generiert dann eine Ausgabe, die besser auf C zugeschnitten ist. Zum Beispiel werden einige der verwendeten Namen unterschiedlich sein. Die Auflistung aller Mitglieder wird weggelassen usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_JAVA-Tag auf \c YES, wenn Ihr Projekt nur Java- oder Python-Quellcode enthalt. Doxygen generiert dann eine Ausgabe, die besser auf die Sprache zugeschnitten ist. Zum Beispiel werden Namespaces als Pakete angezeigt, qualifizierte Bereiche werden anders aussehen usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_SLICE-Tag auf \c YES, wenn Ihr Projekt nur Slice-Quellcode enthalt. Doxygen generiert dann eine Ausgabe, die besser auf die Sprache zugeschnitten ist. Zum Beispiel werden Namespaces als Module angezeigt, Typen werden in mehr Gruppen aufgeteilt usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_VHDL-Tag auf \c YES, wenn Ihr Projekt VHDL-Quellcode enthalt. Doxygen generiert dann eine Ausgabe, die fur VHDL geeignet ist.</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>Das \c OUTPUT_DIRECTORY-Tag wird verwendet, um den (relativen oder absoluten) Pfad anzugeben, in den die generierte Dokumentation geschrieben wird. Wenn ein relativer Pfad eingegeben wird, ist er relativ zu dem Ort, an dem Doxygen gestartet wurde. Wenn leer gelassen, wird das aktuelle Verzeichnis verwendet.</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>Das \c OUTPUT_LANGUAGE-Tag wird verwendet, um die Sprache anzugeben, in der die gesamte von Doxygen generierte Dokumentation geschrieben wird. Doxygen verwendet diese Informationen, um die gesamte konstante Ausgabe in der richtigen Sprache zu generieren.</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>Wenn \ref cfg_generate_treeview &quot;GENERATE_TREEVIEW&quot; auf \c YES gesetzt ist, bestimmt die \c PAGE_OUTLINE_PANEL-Option, ob ein zusatzliches Navigationspanel auf der rechten Seite des Bildschirms angezeigt wird.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>Das \c PAPER_TYPE-Tag kann verwendet werden, um das Papierformat festzulegen.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>Wenn das \c PDF_HYPERLINKS-Tag auf \c YES gesetzt ist, ist das generierte \f$\mbox{\LaTeX}\f$ fur die Konvertierung zu PDF vorbereitet.</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>Wenn das \c PERLMOD_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen die notwendigen Dateien fur PDF- und DVI-Ausgabe.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>Die Namen der Make-Variablen werden mit der Zeichenfolge vorangestellt, die in \c PERLMOD_MAKEVAR_PREFIX enthalten ist.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>Wenn das \c PERLMOD_PRETTY-Tag auf \c YES gesetzt ist, wird die Perl-Modul-Ausgabe schon formatiert.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>Das \c PLANTUMLFILE_DIRS-Tag kann verwendet werden, um Verzeichnisse anzugeben, die PlantUml-Dateien enthalten.</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>Bei Verwendung von PlantUML kann das \c PLANTUML_CFG_FILE-Tag verwendet werden, um eine Konfigurationsdatei fur PlantUML anzugeben.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>Bei Verwendung von PlantUML werden die angegebenen Pfade nach Dateien durchsucht, die durch die \c !include-Anweisung angegeben werden.</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>Bei Verwendung von PlantUML sollte das \c PLANTUML_JAR_PATH-Tag verwendet werden, um den Pfad zur \c plantuml.jar-Datei anzugeben.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>Das \c PREDEFINED-Tag kann verwendet werden, um Makronamen anzugeben, die definiert sind, bevor der Praprozessor gestartet wird.</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>Mit dem \c PROJECT_BRIEF-Tag kann man eine optionale einzeilige Beschreibung fur ein Projekt angeben, die oben auf jeder Seite erscheint und den Betrachtern einen schnellen Uberblick uber den Zweck des Projekts geben sollte. Halten Sie die Beschreibung kurz.</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>Mit dem \c PROJECT_ICON-Tag kann man ein Symbol angeben, das in den Registerkarten angezeigt wird, wenn das HTML-Dokument angezeigt wird. Doxygen kopiert das Logo in das Ausgabeverzeichnis.</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>Mit dem \c PROJECT_LOGO-Tag kann man ein Logo oder Symbol angeben, das in die Dokumentation aufgenommen wird. Die maximale Hohe des Logos sollte 55 Pixel nicht uberschreiten und die maximale Breite sollte 200 Pixel nicht uberschreiten. Doxygen kopiert das Logo in das Ausgabeverzeichnis.</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>Das \c PROJECT_NAME-Tag ist ein einzelnes Wort (oder eine Folge von Wortern, die von Anfuhrungszeichen umgeben sind, es sei denn, Sie verwenden Doxywizard), das das Projekt identifizieren soll, fur das die Dokumentation erstellt wird. Dieser Name wird im Titel der meisten generierten Seiten und an einigen anderen Stellen verwendet.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>Das \c PROJECT_NUMBER-Tag kann verwendet werden, um eine Projekt- oder Revisionsnummer einzugeben. Dies kann zum Archivieren der generierten Dokumentation oder bei Verwendung eines Versionskontrollsystems nutzlich sein.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>Standardmabig werden Python-Docstrings als vorformatierter Text angezeigt und konnen keine Doxygen-Spezialbefehle verwenden. Durch Setzen von \c PYTHON_DOCSTRING auf \c NO konnen Doxygen-Spezialbefehle verwendet werden, und der Inhalt des Docstring-Dokumentationsblocks wird als Doxygen-Dokumentation angezeigt.</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>Wenn das \ref cfg_qhg_location &quot;QHG_LOCATION&quot;-Tag angegeben ist, kann das \c QCH_FILE-Tag verwendet werden, um den Dateinamen der resultierenden `.qch`-Datei anzugeben.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>Das \c QHG_LOCATION-Tag kann verwendet werden, um den Speicherort von Qt&#x27;s qhelpgenerator anzugeben.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>Das \c QHP_CUST_FILTER_ATTRS-Tag gibt die Liste der Attribute des benutzerdefinierten Filters an.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>Wenn das \c QHP_CUST_FILTER_NAME-Tag gesetzt ist, gibt es den Namen eines benutzerdefinierten Filters an.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>Das \c QHP_NAMESPACE-Tag gibt den Namespace an, der beim Generieren der Qt Help Project-Ausgabe verwendet werden soll.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>Das \c QHP_SECT_FILTER_ATTRS-Tag gibt die Liste der Attribute an, mit denen der Filterabschnitt dieses Projekts ubereinstimmt.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>Das \c QHP_VIRTUAL_FOLDER-Tag gibt den virtuellen Ordner an, der beim Generieren der Qt Help Project-Ausgabe verwendet werden soll.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>Wenn das \c QT_AUTOBRIEF-Tag auf \c YES gesetzt ist, interpretiert Doxygen die erste Zeile (bis zum ersten Punkt, Fragezeichen oder Ausrufezeichen) von Qt-Stil-Kommentaren als Kurzbeschreibung. Wenn auf \c NO gesetzt, verhalt sich der Qt-Stil wie normale Qt-Stil-Kommentare (daher ist ein expliziter \ref cmdbrief &quot;\brief&quot;-Befehl erforderlich, um eine Kurzbeschreibung zu erhalten).</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>Das \c QUIET-Tag kann verwendet werden, um die Nachrichten ein- oder auszuschalten, die von Doxygen auf die Standardausgabe generiert werden.</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>Das \c RECURSIVE-Tag kann verwendet werden, um anzugeben, ob Unterverzeichnisse ebenfalls nach Eingabedateien durchsucht werden sollen.</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>Wenn das \c REFERENCED_BY_RELATION-Tag auf \c YES gesetzt ist, wird fur jede dokumentierte Entitat alle dokumentierten Funktionen aufgelistet, die sie referenzieren.</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>Wenn das \c REFERENCES_LINK_SOURCE-Tag auf \c YES gesetzt ist, verweisen die Hyperlinks von Funktionen auf den Quellcode.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>Wenn das \c REFERENCES_RELATION-Tag auf \c YES gesetzt ist, wird fur jede dokumentierte Funktion alle dokumentierten Entitaten aufgelistet, die von dieser Funktion aufgerufen/verwendet werden.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>Wenn das \c REPEAT_BRIEF-Tag auf \c YES gesetzt ist, wird Doxygen die Kurzbeschreibung eines Mitglieds oder einer Funktion vor der detaillierten Beschreibung voranstellen. &amp;lt;br&amp;gt;Hinweis: Wenn sowohl \ref cfg_hide_undoc_members &quot;HIDE_UNDOC_MEMBERS&quot; als auch \ref cfg_brief_member_desc &quot;BRIEF_MEMBER_DESC&quot; auf \c NO gesetzt sind, werden die Kurzbeschreibungen vollstandig unterdruckt.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>Das \c REQ_TRACEABILITY_INFO-Tag steuert, ob Ruckverfolgbarkeitsinformationen auf der Anforderungen-Seite angezeigt werden.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>Wenn das \c RESOLVE_UNNAMED_PARAMS-Tag auf \c YES gesetzt ist, werden unbenannte Parameter in Funktionsdeklarationen aufgelost.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>Setzen Sie optionale Variablen, die bei der Generierung eines RTF-Dokuments verwendet werden.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>Das \c RTF_EXTRA_FILES-Tag kann verwendet werden, um zusatzliche Dateien anzugeben, die in das \ref cfg_rtf_output &quot;RTF_OUTPUT&quot;-Ausgabeverzeichnis kopiert werden sollen.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>Wenn das \c RTF_HYPERLINKS-Tag auf \c YES gesetzt ist, enthalt das generierte RTF Hyperlink-Felder.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>Das \c RTF_OUTPUT-Tag wird verwendet, um anzugeben, wo die RTF-Dokumente platziert werden.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>Laden Sie Stylesheet-Definitionen aus einer Datei.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>Wenn \ref cfg_server_based_search &quot;SERVER_BASED_SEARCH&quot; und \ref cfg_external_search &quot;EXTERNAL_SEARCH&quot; beide aktiviert sind, werden die unindizierten Suchdaten in eine Datei geschrieben.</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>Wenn das \c SEARCHENGINE-Tag aktiviert ist, generiert Doxygen ein Suchfeld fur die HTML-Ausgabe.</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>Die \c SEARCHENGINE_URL sollte auf eine Suchmaschine zeigen, die von einem Webserver gehostet wird.</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>Wenn das \c SEARCH_INCLUDES-Tag auf \c YES gesetzt ist, werden die Include-Dateien durchsucht.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>Wenn das \c SEPARATE_MEMBER_PAGES-Tag auf \c YES gesetzt ist, generiert Doxygen eine neue Seite fur jedes Mitglied. Wenn auf \c NO gesetzt, wird die Dokumentation des Mitglieds Teil der Datei/Klasse/Namespace, die es enthalt.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>Wenn das \c SERVER_BASED_SEARCH-Tag aktiviert ist, wird die Suchmaschine mit einem Webserver implementiert.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>Wenn das \c SHORT_NAMES-Tag auf \c YES gesetzt ist, generiert Doxygen viel kurzere (aber weniger lesbare) Dateinamen. Dies kann nutzlich sein, wenn Ihr Dateisystem keine langen Namen unterstutzt (wie DOS, Mac oder CD-ROM).</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>Wenn das \c SHOW_ENUM_VALUES-Tag gesetzt ist, zeigt Doxygen die angegebenen Enumerationswerte neben den Enumerationsmnemonics an.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>Setzen Sie das \c SHOW_FILES-Tag auf \c NO, um die Generierung der Datei-Seite zu deaktivieren.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>Wenn das \c SHOW_GROUPED_MEMB_INC-Tag auf \c YES gesetzt ist, wird Doxygen fur jedes gruppierte Mitglied eine Include-Anweisung zur Dokumentation hinzufugen.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>Wenn das \c SHOW_HEADERFILE-Tag auf \c YES gesetzt ist, zeigt die Dokumentation fur eine Klasse an, welche Datei eingebunden werden muss, um die Klasse zu verwenden.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>Wenn das \c SHOW_INCLUDE_FILES-Tag auf \c YES gesetzt ist, wird Doxygen eine Liste der Dateien einfugen, die von einer Datei in der Dokumentation dieser Datei eingebunden werden.</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>Setzen Sie das \c SHOW_NAMESPACES-Tag auf \c NO, um die Generierung der Namespace-Seite zu deaktivieren.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>Setzen Sie das \c SHOW_USED_FILES-Tag auf \c NO, um die Liste der Dateien zu deaktivieren, die unten in der Dokumentation von Klassen und Strukturen generiert wird.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>Wenn Ihr Projekt nur &amp;lt;a href=&quot;https://www.riverbankcomputing.com/software&quot;&amp;gt;sip&amp;lt;/a&amp;gt;-Quellcode enthalt, setzen Sie das \c SIP_SUPPORT-Tag auf \c YES. Doxygen wird sie wie normales C++ parsen, aber annehmen, dass alle Klassen offentliche Vererbung verwenden, wenn kein explizites Schutzschlusswort vorhanden ist, anstatt private Vererbung.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>Das \c SITEMAP_URL-Tag wird verwendet, um die vollstandige URL des Ortes anzugeben, an dem die generierte Dokumentation platziert wird.</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>Wenn das \c SKIP_FUNCTION_MACROS-Tag auf \c YES gesetzt ist, entfernt Doxygens Praprozessor alle Referenzen auf funktionsahnliche Makros.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>Wenn das \c SORT_BRIEF_DOCS-Tag auf \c YES gesetzt ist, wird Doxygen die Kurzbeschreibungen von Datei-, Namespace- und Klassenmitgliedern alphabetisch nach Mitgliedsnamen sortieren.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>Wenn das \c SORT_BY_SCOPE_NAME-Tag auf \c YES gesetzt ist, wird die Klassenliste nach vollqualifizierten Namen sortiert, einschlieblich Namensraume.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>Wenn das \c SORT_GROUP_NAMES-Tag auf \c YES gesetzt ist, wird Doxygen die Hierarchie der Gruppennamen in alphabetische Reihenfolge sortieren.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>Wenn das \c SORT_MEMBERS_CTORS_1ST-Tag auf \c YES gesetzt ist, wird Doxygen die (Kurz- und Detaillierte) Dokumentation von Klassenmitgliedern so sortieren, dass Konstruktoren und Destruktoren zuerst aufgefuhrt werden.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>Wenn das \c SORT_MEMBER_DOCS-Tag auf \c YES gesetzt ist, wird Doxygen die (detaillierte) Dokumentation von Datei- und Klassenmitgliedern alphabetisch nach Mitgliedsnamen sortieren.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>Wenn das \c SOURCE_BROWSER-Tag auf \c YES gesetzt ist, wird eine Liste der Quelldateien generiert.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>Wenn \c SOURCE_TOOLTIPS aktiviert ist (Standard), zeigt das Schweben uber einem Hyperlink im Quellcode einen Tooltip mit zusatzlichen Informationen an.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>Das \c SQLITE3_OUTPUT-Tag wird verwendet, um anzugeben, wo die \c Sqlite3-Datenbank platziert wird.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>Wenn das \c SQLITE3_RECREATE_DB-Tag auf \c YES gesetzt ist, wird die vorhandene Datenbankdatei bei jedem Doxygen-Lauf neu erstellt.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>Wenn die \c STRICT_PROTO_MATCHING-Option aktiviert ist und Doxygen keine korrekte Typauflosung aller Parameter einer Funktion durchfuhren kann, wird eine Ubereinstimmung zwischen dem Prototyp und der Implementierung einer Mitgliedsfunktion abgelehnt.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>Das Setzen des \c STRIP_CODE_COMMENTS-Tags auf \c YES weist Doxygen an, alle speziellen Kommentarblocke aus generierten Quellcode-Fragmenten auszublenden.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>Das \c STRIP_FROM_INC_PATH-Tag kann verwendet werden, um einen benutzerdefinierten Teil des Pfads zu entfernen, der in der Dokumentation einer Klasse erwahnt wird, der dem Leser sagt, welche Header-Datei er einbinden muss, um die Klasse zu verwenden. Wenn leer gelassen, wird nur der Name der Header-Datei verwendet, die die Klassendefinition enthalt. Andernfalls sollten Sie eine Liste von Include-Pfaden angeben, die normalerweise mit dem `-I`-Flag an den Compiler ubergeben werden.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>Das \c STRIP_FROM_PATH-Tag kann verwendet werden, um einen benutzerdefinierten Teil des Pfads zu entfernen. Das Entfernen erfolgt nur, wenn eine der angegebenen Zeichenfolgen mit dem linken Teil des Pfads ubereinstimmt. Dieses Tag kann verwendet werden, um relative Pfade in der Dateiliste anzuzeigen. Wenn leer gelassen, wird das Verzeichnis, in dem Doxygen lauft, als zu entfernender Pfad verwendet. &amp;lt;br&amp;gt;Beachten Sie, dass Sie hier absolute Pfade angeben konnen, aber auch relative Pfade, die relativ zu dem Verzeichnis sind, in dem Doxygen gestartet wurde.</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>Setzen Sie das \c SUBGROUPING-Tag auf \c YES, um Gruppen von Klassenmitgliedern desselben Typs (z.B. eine Gruppe offentlicher Funktionen) als Untergruppe dieses Typs zu platzieren (z.B. unter dem Abschnitt Offentliche Funktionen).</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>Das \c TAB_SIZE-Tag kann verwendet werden, um die Anzahl der Leerzeichen in einem Tabulator festzulegen. Doxygen verwendet diesen Wert, um Tabulatoren in Codefragmenten durch Leerzeichen zu ersetzen.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>Das \c TAGFILES-Tag kann verwendet werden, um eine oder mehrere Tag-Dateien anzugeben.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>Wenn das \c TEMPLATE_RELATIONS-Tag auf \c YES gesetzt ist, zeigen die Diagramme die Beziehungen zwischen Templates und ihren Instanzen.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>Wenn das \c TIMESTAMP-Tag auf \c YES gesetzt ist, wird das Datum und die Uhrzeit der Generierung in die Fuszzeile jeder Seite eingefugt.</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>Das \c TOC_EXPAND-Flag kann auf \c YES gesetzt werden, um zusatzliche Elemente fur Gruppenmitglieder zum Inhaltsverzeichnis hinzuzufugen.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>Wenn das \c TOC_INCLUDE_HEADINGS-Tag auf einen Wert ungleich null gesetzt ist, werden alle Uberschriften bis zu dieser Ebene automatisch in das Inhaltsverzeichnis aufgenommen, auch wenn sie kein id-Attribut haben. \note Diese Funktion funktioniert derzeit nur fur Markdown-Uberschriften.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>Wenn der Treeview aktiviert ist, kann dieses Tag verwendet werden, um die anfangliche Breite des Frames festzulegen.</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>Wenn das \c TYPEDEF_HIDES_STRUCT-Tag auf \c YES gesetzt ist, wird eine Struktur/Union/Klasse, deren Name vollstandig durch eine Typedef geschwärzt ist, als Typedef mit dem Namen der Typedef behandelt.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>Wenn das \ref cfg_uml_look &quot;UML_LOOK&quot;-Tag aktiviert ist, begrenzt der \c UML_LIMIT_NUM_FIELDS-Schwellenwert die Anzahl der Elemente.</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>Wenn das \c UML_LOOK-Tag auf \c YES gesetzt ist, generiert Doxygen Diagramme in UML-Stil.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>Wenn das \ref cfg_uml_look &quot;UML_LOOK&quot;-Tag aktiviert ist, begrenzt der \c UML_MAX_EDGE_LABELS-Schwellenwert die Anzahl der Elemente.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>Wenn das \c USE_HTAGS-Tag auf \c YES gesetzt ist, verweisen die Referenzen auf den Quellcode auf das vom \c htags(1)-Tool generierte HTML.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>Aktivieren Sie die \c USE_MATHJAX-Option, um \f$\mbox{\LaTeX}\f$-Formeln mit MathJax zu rendern.</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>Wenn das \c USE_MDFILE_AS_MAINPAGE-Tag auf den Namen einer Markdown-Datei verweist, die Teil der Eingabe ist, wird ihr Inhalt auf der Hauptseite (`index.html`) platziert.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>Wenn das \c USE_PDFLATEX-Tag auf \c YES gesetzt ist, verwendet Doxygen die angegebene Engine, um die PDF-Datei direkt zu generieren.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>Wenn das \c VERBATIM_HEADERS-Tag auf \c YES gesetzt ist, generiert Doxygen eine wortliche Kopie der Header-Datei fur jede Klasse, fur die ein Include angegeben ist.</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>Das \c WARNINGS-Tag kann verwendet werden, um die Warnmeldungen ein- oder auszuschalten, die von Doxygen auf die Standardfehlerausgabe (\c stderr) generiert werden.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>Wenn das \c WARN_AS_ERROR-Tag auf \c YES gesetzt ist, stoppt Doxygen sofort, wenn eine Warnung auftritt.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>Das \c WARN_FORMAT-Tag bestimmt das Format der Warnmeldungen, die Doxygen erzeugen kann.</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>Wenn das \c WARN_IF_DOC_ERROR-Tag auf \c YES gesetzt ist, generiert Doxygen Warnungen fur potenzielle Fehler in der Dokumentation.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>Wenn \c WARN_IF_INCOMPLETE_DOC auf \c YES gesetzt ist, warnt Doxygen vor unvollstandiger Funktionsparameter-Dokumentation.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>Wenn das \c WARN_IF_UNDOCUMENTED-Tag auf \c YES gesetzt ist, generiert Doxygen Warnungen fur undokumentierte Mitglieder.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>Wenn die \c WARN_IF_UNDOC_ENUM_VAL-Option auf \c YES gesetzt ist, warnt Doxygen vor undokumentierten Enumerationswerten.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>Wenn die \c WARN_LAYOUT_FILE-Option auf \c YES gesetzt ist, warnt Doxygen vor Problemen, die beim Parsen der benutzerdefinierten Layout-Datei gefunden wurden.</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>Im `$text`-Teil des \ref cfg_warn_format &quot;WARN_FORMAT&quot;-Befehls ist es moglich, dass ein Verweis auf einen spezifischeren Ort angegeben wird.</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>Das \c WARN_LOGFILE-Tag kann verwendet werden, um eine Datei anzugeben, in die Warn- und Fehlermeldungen geschrieben werden sollen.</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>Diese \c WARN_NO_PARAMDOC-Option kann aktiviert werden, um Warnungen fur Funktionen zu erhalten, die dokumentiert sind, aber keine Dokumentation fur ihre Parameter oder Ruckgabewerte haben.</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>Wenn das \c XML_NS_MEMB_FILE_SCOPE-Tag auf \c YES gesetzt ist, wird Doxygen Namespace-Mitglieder auch im Dateibereich aufnehmen.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>Das \c XML_OUTPUT-Tag wird verwendet, um anzugeben, wo die XML-Seiten platziert werden.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>Wenn das \c XML_PROGRAMLISTING-Tag auf \c YES gesetzt ist, gibt Doxygen die Programmauflistungen in die XML-Ausgabe.</translation>
    </message>
</context>
</TS>
